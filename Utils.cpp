#include "Utils.h"


void logPrintf(const char *level, const char *msg) {
	printf("%s[%s] %s %s\n", KBLU, level, msg, KNRM);
}

void logError(const char *level, const char *msg) {
	printf("%s[%s] %s %s\n", KRED, level, msg, KNRM);
}

int loadCounter() {
	int imageCounter = 0;

    FILE *fid = fopen(COUNTER_FILE,"rt");
    fscanf(fid, "%d", &imageCounter);
    fclose(fid);

    return imageCounter;
}

void writeCounter(int imageCounter) {
    FILE *fid = fopen(COUNTER_FILE,"wt");
    fprintf(fid, "%d", imageCounter);
    fclose(fid);
} 



void fillBuffer(uint8_t **buffer, int imageSizeW, int imageSizeH) {

    buffer[0] = new uint8_t[imageSizeW*imageSizeH];

    for (int i=0; i<imageSizeW*imageSizeH; i++) {
        buffer[0][i] = 0;
    }
}



void addLogLine(struct sockaddr_in  cliAddr, const char *level, char *cmd, char *result) {
    char *ipStr;
    char *timeStr;
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    ipStr = inet_ntoa(cliAddr.sin_addr);

    timeStr = asctime(timeinfo);

    FILE *fid = fopen("log.txt","a");


    ipStr[strcspn(ipStr, "\r\n")] = 0;
    timeStr[strcspn(timeStr, "\r\n")] = 0;
/*
    int pos;
    int len = strlen(cmd);
    while ((pos = strcspn(cmd, "\r\n"))<len)
        if (pos<len) cmd[pos] = ';';
*/
    fprintf(fid,"Level: %s\nIP: %s\nDate time: %s\ncommand: %s\nresult: %s\n",level,ipStr, timeStr, cmd, result);

    fclose(fid);
}




void compressMainBufferRawImage(uint8_t *buffer, int bufferSize, uint8_t *outputBuffer, int *outputBufferSize) {


    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    // setup "a" as the input and "b" as the compressed output
    defstream.avail_in = bufferSize; // size of input, string + terminator
    defstream.next_in = (Bytef *)buffer; // input char array
    defstream.avail_out = bufferSize; // size of output
    defstream.next_out = (Bytef *)outputBuffer; // output char array
    
    // the actual compression work.
    deflateInit2 (&defstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                             WINDOWS_BITS | GZIP_ENCODING, 8,
                             Z_DEFAULT_STRATEGY);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);

    *outputBufferSize = (unsigned char*)defstream.next_out - outputBuffer;
}


time_t saveBufferRawImage(uint8_t *buffer, int W, int H, time_t lastTime, time_t safeInterval) {
    time_t thisTime;
    time_t thisInterval;
    char fileName[256];
    int outputBufferSize;

    time(&thisTime);

    thisInterval = thisTime - lastTime;

    if (thisInterval > safeInterval) {

        int imageCounter = loadCounter();

        uint8_t *outputBuffer = (uint8_t *)malloc(W*H);

        compressMainBufferRawImage(buffer, W*H, outputBuffer, &outputBufferSize);

        sprintf(fileName, "output/img%d.raw.gz", imageCounter);

        FILE *fid = fopen(fileName,"wb");

        fwrite(outputBuffer, 1, outputBufferSize, fid);

        fclose(fid);

        free(outputBuffer);


        imageCounter++;
        writeCounter(imageCounter);

        return thisTime;

    } else {
        
        return lastTime;
    }

}


long getMicrotime() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}




void buffer2float(uint8_t *buffer, float *floatImage, int width, int height) {
    int i;

    for (i=0; i<height*width; i++) {
        floatImage[i] = buffer[2*i+1]*255 + buffer[2*i];
    }
}



void zeros(float *floatImage, int width, int height) {
    int i;

    for (i=0; i<height*width; i++) {
        floatImage[i] = 0.0;
    }
}






// MyPipelineEventSink class

void MyPipelineEventSink::OnBufferReady(PvPipeline *aPipeline) {
    this->isReady = 1;
    //printf("Buffer ready!\n");
}

void MyPipelineEventSink::OnStop(PvPipeline *aPipeline)  {
    this->isReady = 0;
}

void MyPipelineEventSink::OnStart(PvPipeline *aPipeline)  {
    this->isReady = 0;
}