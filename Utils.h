#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvDeviceU3V.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvStreamU3V.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvBufferWriter.h>

#include "zlib.h"
#include "time.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define BUFFER_COUNT 16
#define CAM_IP "10.194.64.83"
#define COUNTER_FILE "counter.txt"
#define MAX_IMG_W 1280
#define MAX_IMG_H 512

// zlib stuff
#define WINDOWS_BITS 15
#define GZIP_ENCODING 16


void logPrintf(const char *level, const char *msg);
void logError(const char *level, const char *msg);
int loadCounter();
void writeCounter(int imageCounter);
void fillBuffer(uint8_t **buffer, int imageSizeW, int imageSizeH);
void compressMainBufferRawImage(uint8_t *buffer, int bufferSize, uint8_t *outputBuffer, int *outputBufferSize);
time_t saveBufferRawImage(uint8_t *buffer, int W, int H, time_t currentTime, time_t safeTime);
void addLogLine(struct sockaddr_in  cliAddr, const char *level, char *cmd, char *result);
long getMicrotime();


// MyPipelineEventSink class


class MyPipelineEventSink : PvPipelineEventSink {

public:
    int isReady;

    void OnBufferReady(PvPipeline *aPipeline);

    void OnStop(PvPipeline *aPipeline);

    void OnStart(PvPipeline *aPipeline);
}; 


#endif // __UTILS_H__
