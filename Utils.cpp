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