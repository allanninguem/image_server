#ifndef __IMAGEACQUISITION_H__
#define __IMAGEACQUISITION_H__
          
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvDeviceU3V.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvStreamU3V.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvBufferWriter.h>

#include "Utils.h"

void saveBuffer(PvBuffer *lBuffer);
PvDevice *ConnectToDevice( const PvString &aConnectionID );
PvStream *OpenStream( const PvString &aConnectionID );
void ConfigureStream( PvDevice *aDevice, PvStream *aStream );
PvPipeline *CreatePipeline( PvDevice *aDevice, PvStream *aStream );
void waitForBufferReady(MyPipelineEventSink *lMyPipelineEventSink);
int AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline, MyPipelineEventSink *lMyPipelineEventSink, uint8_t *imageBuffer, int &imageSizeW, int &imageSizeH);

int AcquireFluxImage(PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline, MyPipelineEventSink *lMyPipelineEventSink, int fluxImageW, int fluxImageH, int N, float *fluxImage);
void computeFlux(float *sumY, float *sumXY, float sumX, float sumX2, int N, int width, int height, float *fluxImage);
void buffer2float(uint8_t *buffer, float *floatImage, int width, int height);
void zeros(float *floatImage, int width, int height);
void sumValues(float *sumY, float *Y, int width, int height);
void sumSquares(float *sumY2, float *Y, int width, int height);
void sumProducts(float *sumXY, float *Y, float X, int width, int height);


#endif // __IMAGEACQUISITION_H__