#ifndef __IMAGEACQUISITION_H__
#define __IMAGEACQUISITION_H__
          
#include <stdio.h>
#include <unistd.h>

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
void AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline, MyPipelineEventSink *lMyPipelineEventSink);

#endif // __IMAGEACQUISITION_H__