#ifndef __SCIENCE_H__
#define __SCIENCE_H__
          
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


void computeFlux(float *sumY, float *sumXY, float *sumX, float *sumX2, uint8_t *pointsConsidered, int width, int height, float *fluxImage);
void sumProducts(float *sumXY, float *Y, float X, int maxADU, int width, int height);
void sumProductsX(float *sumX, float X1, float X2, float *Y, int maxADU, int width, int height);
float computeMeanOfFrame(float *Y, int width, int height);

#endif // __SCIENCE_H__