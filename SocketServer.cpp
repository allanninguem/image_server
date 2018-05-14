#include "SocketServer.h"




int readLine(int sockFd, char *buffer, int maxSize) {
    char c = 0;
    int count = 0;

    bzero(buffer,256);
    while ( (c!=10)&&(count<maxSize) ) {
        read(sockFd, &c, 1);
        buffer[count] = c;
        count++;
    }

    if (count>=2)
    	buffer[count-2] = 0;


    return count;
}

int initServer(int portNo) {
    int sockFd, n;
    struct sockaddr_in servAddr;
    int keepServing = 1;

    sockFd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockFd > 0) {
        bzero((char *) &servAddr, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_port = htons(portNo);
        servAddr.sin_addr.s_addr = INADDR_ANY;

        int optVal = 1;
        setsockopt(sockFd,SOL_SOCKET,SO_REUSEADDR,&optVal,sizeof(int));

        if (bind(sockFd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
            sockFd = -1;
        } else {
            listen(sockFd,1);
        }
    }


    return sockFd;
}


int waitForConnection(int sockFd, struct sockaddr_in &cliAddr) {
    //struct sockaddr_in cliAddr;
    socklen_t clilen;

    int newSockFd = -1;

    clilen = sizeof(cliAddr);
    newSockFd = accept(sockFd, (struct sockaddr *) &cliAddr, &clilen);

    return newSockFd;
}







int sendImageBuffer(int clientSocket, uint8_t *imageBuffer, int imageSizeW, int imageSizeH) {
	int errorState = 0;

	char utfSize[4];
	utfSize[1] = (imageSizeW&0xff00)>>8;
	utfSize[0] = imageSizeW&0xff;
	utfSize[3] = (imageSizeH&0xff00)>>8;
	utfSize[2] = imageSizeH&0xff;
	int n = write(clientSocket,utfSize,4);
	if (n < 0) {
	    errorState = 1;
	}
	n = write(clientSocket,imageBuffer,imageSizeW*imageSizeH);
	if (n < 0) {
	    errorState = 1;
	}

	return errorState;

}




int SendResultBuffer(int clientSocket, char *lInBuffer) {
    int errorState = 0;

    int lInBufferSize = strlen(lInBuffer);

    if (lInBufferSize == 0) {
        lInBufferSize = 2;
        lInBuffer[0] = 13;
        lInBuffer[1] = 10;
    }

    // send string size (for utf string reading...)
    char utfSize[2];
    utfSize[0] = (lInBufferSize&0xff00)>>8;
    utfSize[1] = lInBufferSize&0xff;
    int n = write(clientSocket,utfSize,2);
    if (n < 0)
        errorState = 1;

    n = write(clientSocket,lInBuffer,strlen(lInBuffer));
    if (n < 0)
        errorState = 1;



    return errorState;
}









 
