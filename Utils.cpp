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

    logPrintf("UTILS","filling initial buffer");

    buffer[0] = new uint8_t[imageSizeW*imageSizeH];

    for (int i=0; i<imageSizeW*imageSizeH; i++) {
        buffer[0][i] = 0;
    }
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


void saveBufferRawImage(uint8_t *buffer, int W, int H) {
    char fileName[256];
    int outputBufferSize;

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

    logPrintf("UTILS","Image saved");

}












// MyPipelineEventSink class

void MyPipelineEventSink::OnBufferReady(PvPipeline *aPipeline) {
    this->isReady = 1;
    printf("Buffer ready!\n");
}

void MyPipelineEventSink::OnStop(PvPipeline *aPipeline)  {
    this->isReady = 0;
}

void MyPipelineEventSink::OnStart(PvPipeline *aPipeline)  {
    this->isReady = 0;
}