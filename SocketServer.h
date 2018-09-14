 
#ifndef __SOCKETSERVER_H__
#define __SOCKETSERVER_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <strings.h>

#include <time.h>

#include "Utils.h"



int readLine(int sockFd, char *buffer, int maxSize);
int initServer(int portNo);
int waitForConnection(int sockFd, struct sockaddr_in &cliAddr);
int sendImageBuffer(int clientSocket, uint8_t *imageBuffer, int imageSizeW, int imageSizeH);
int sendFluxImage(int clientSocket, float *fluxImage, int fluxImageW, int fluxImageH);






#endif // __SOCKETSERVER_H__
