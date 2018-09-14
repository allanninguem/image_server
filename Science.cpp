#include "Science.h"





float computeMeanOfFrame(float *Y, int width, int height) {
    int i;
    float m = 0.0;

    for (i=0; i<height*width; i++) {
        m += Y[i];
    }

    return m/(float)(height*width);

}



void computeFlux(float *sumY, float *sumXY, float *sumX, float *sumX2, uint8_t *pointsConsidered, int width, int height, float *fluxImage) {
    int i;
    int N;

    for (i=0; i<height*width; i++) {
        N = pointsConsidered[i];
        if (N>1) {
            fluxImage[i] = (sumXY[i]*(float)N - sumX[i]*sumY[i])/(sumX2[i]*(float)N - sumX[i]*sumX[i]);
        } else {
            fluxImage[i] = 0.0/0.0;
        }
    }

}




void sumProducts(float *sumXY, float *Y, float X, int maxADU, int width, int height) {
    int i;

    for (i=0; i<height*width; i++) {
        if (Y[i]<maxADU)
            sumXY[i] += Y[i]*X;
    }
}



void sumProductsX(float *sumX, float X1, float X2, float *Y, int maxADU, int width, int height) {
    int i;

    for (i=0; i<height*width; i++) {
        if (Y[i]<maxADU)
            sumX[i] += X1*X2;
    }
}



