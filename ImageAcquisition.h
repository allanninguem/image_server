#ifndef __IMAGEACQUISITION_H__
#define __IMAGEACQUISITION_H__
          
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <PvSampleUtils.h>
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
#include "SerialComm.h"
#include "Science.h"

void saveBuffer(PvBuffer *lBuffer);
PvDevice *ConnectToDevice( const PvString &aConnectionID );
PvStream *OpenStream( const PvString &aConnectionID );
void ConfigureStream( PvDevice *aDevice, PvStream *aStream );
PvPipeline *CreatePipeline( PvDevice *aDevice, PvStream *aStream );
void waitForBufferReady(MyPipelineEventSink *lMyPipelineEventSink);
int AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline, MyPipelineEventSink *lMyPipelineEventSink, uint8_t *imageBuffer, int &imageSizeW, int &imageSizeH);

int AcquireFluxImage(PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline, MyPipelineEventSink *lMyPipelineEventSink, int fluxImageW, int fluxImageH, int N, int maxADU, float *fluxImage, float *timeStamps, uint8_t *pointsConsidered);
int setNbreadworeset(PvDeviceSerialPort *aPort, int N);

#endif // __IMAGEACQUISITION_H__