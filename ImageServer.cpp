#include "ImageServer.h"
#include "Utils.h"
#include "ImageAcquisition.h"
#include "SerialComm.h"
#include "SocketServer.h"

#define RX_BUFFER_SIZE 1024
#define CMD_BUFFER_SIZE 1024


uint8_t *mainImageBuffer;
int mainImageSizeW;
int mainImageSizeH;
char mainCmdBuffer[CMD_BUFFER_SIZE];
char mainResultBuffer[RX_BUFFER_SIZE];
int imageCounter;

// socket stuff
#define SOCKET_SERVER_PORT 6660
#define READ_BUFFER_SIZE 1024

int socketServer, socketClient;
struct sockaddr_in clientAddr;
char readSocketBuffer[READ_BUFFER_SIZE];
int keepServing = 1;
int keepConnected = 1;


int main()
{
    PvDevice *lDevice = NULL;
    PvStream *lStream = NULL;
    PvGenCommand *lStop = NULL;
    PvGenCommand *lStart = NULL;
    PvDeviceAdapter *lDeviceAdapter = NULL;
    PvDeviceSerialPort *lPort = NULL;

    int errorState;


    strcpy(mainCmdBuffer, "temperatures\n");
    

    logPrintf("MAIN","Image Server");

    fillBuffer(&mainImageBuffer, MAX_IMG_W, MAX_IMG_H);

    PvString lConnectionID = CAM_IP;
    lDevice = ConnectToDevice( lConnectionID );
    if ( lDevice != NULL ) {
        logPrintf("MAIN","Conected to device");


        lStream = OpenStream( lConnectionID );
        if ( lStream != NULL ) {
            logPrintf("MAIN","Stream open");


            PvPipeline *lPipeline = NULL;
            ConfigureStream( lDevice, lStream );


            lPipeline = CreatePipeline( lDevice, lStream );
            if( lPipeline != NULL ) {
                logPrintf("MAIN","Pipeline OK");

                MyPipelineEventSink *lMyPipelineEventSink = new MyPipelineEventSink();
                PvResult lResult = lPipeline->RegisterEventSink((PvPipelineEventSink *)lMyPipelineEventSink);

                if ( lResult.IsOK() ) {

                    lDeviceAdapter = CreateCameraDeviceAdapter(lDevice);

                    if (lDeviceAdapter) {

                        lPort = ConfigureCameraSerial(lDevice, lDeviceAdapter);

                        if (lPort) {



                            socketServer = initServer(SOCKET_SERVER_PORT);


                            while (keepServing) {

                                if (socketServer>0) {
                                    socketClient = waitForConnection(socketServer, clientAddr);

                                    if (socketClient>0) {
                                        logPrintf("MAIN","connection OK!");


                                        while (keepConnected) {

                                            logPrintf("MAIN","Wait for another command");


                                            int n = readLine(socketClient,readSocketBuffer,READ_BUFFER_SIZE);
                                            if (n > 0) {


                                                printf("%s\n",readSocketBuffer);


                                                if (strcmp(readSocketBuffer,"quit")==0) { 

                                                    // quit this connection

                                                    keepConnected = 0;






                                                } else if (strcmp(readSocketBuffer,"stop")==0) {

                                                    // quit everything 

                                                    keepConnected = 0;
                                                    keepServing = 0;








                                                } else if (strcmp(readSocketBuffer,"get img")==0) {

                                                    AcquireImages( lDevice, lStream, lPipeline, lMyPipelineEventSink, mainImageBuffer, mainImageSizeW, mainImageSizeH );
                                                    errorState = sendImageBuffer(socketClient, mainImageBuffer, mainImageSizeW, mainImageSizeH);

                                                    if (errorState) {
                                                        logError("MAIN","Sending image failure");
                                                    }









                                                } else if (strcmp(readSocketBuffer,"get n img")==0) {
                                                    n = readLine(socketClient,readSocketBuffer,READ_BUFFER_SIZE);
                                                    if (n < 0) {

                                                        logError("MAIN","ERROR reading from socket");

                                                    } else {
                                                    
                                                        int nImgs = 0;
                                                        sscanf(readSocketBuffer, "%d", &nImgs);

                                                        for (int i = 0; i < nImgs; ++i)
                                                        {
                                                            AcquireImages( lDevice, lStream, lPipeline, lMyPipelineEventSink, mainImageBuffer, mainImageSizeW, mainImageSizeH );
                                                            errorState = sendImageBuffer(socketClient, mainImageBuffer, mainImageSizeW, mainImageSizeH);

                                                            if (errorState) {
                                                                logError("MAIN","Sending image failure");
                                                            }
                                                        }


                                                    }









                                                    } else if (strcmp(readSocketBuffer,"get serial")==0) {

                                                        // send cmdBuffer to camera serial interface
                                                        SendCameraCommand(lPort, mainCmdBuffer);

                                                        // receive camera serial interface response
                                                        ReceiveCameraResult(lPort, mainResultBuffer);
                                                        
                                                        errorState = SendResultBuffer(socketClient, mainResultBuffer);
                                                        if (errorState) 
                                                            logError("MAIN","ERROR writing to socket");

                                                        addLogLine(clientAddr, mainResultBuffer);










                                                } else if (strcmp(readSocketBuffer,"set serial")==0) {

                                                    n = readLine(socketClient,readSocketBuffer,READ_BUFFER_SIZE);
                                                    if (n < 0) {

                                                        logError("MAIN","ERROR reading from socket");

                                                    } else {

                                                        // set mainCmdBuffer to what came form serial
                                                        sprintf(mainCmdBuffer, "%s\r\n", readSocketBuffer);

                                                        addLogLine(clientAddr, readSocketBuffer);

                                                    }



                                                }






                                            } else {

                                                logError("MAIN","ERROR reading from socket");

                                            }

                                        }

                                        logPrintf("MAIN","Connection will close");

                                        keepConnected = 1;

                                        close(socketClient);

                                    } else {
                                        logError("MAIN","Socket connection error");
                                    }

                                } else {
                                    logError("MAIN","Socket server error");
                                }

                            }

                            logPrintf("MAIN","Server stoped");















                            for (int i=0; i<0; i++) {

                                clock_t begin = clock();

                                // aquire image into main buffer
                                // TODO: return false on error
                                AcquireImages( lDevice, lStream, lPipeline, lMyPipelineEventSink, mainImageBuffer, mainImageSizeW, mainImageSizeH );

                                // saves output/imgxx.raw.gz
                                saveBufferRawImage(mainImageBuffer, mainImageSizeW, mainImageSizeH);

                                // sends mainCmdBuffer to camera serial interface
                                SendCameraCommand(lPort, mainCmdBuffer);
                                
                                usleep(1000);

                                // receive camera serial interface response
                                ReceiveCameraResult(lPort, mainResultBuffer);
                                printf("%s\n",mainResultBuffer);


                                clock_t end = clock();      
                                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                                printf("(%.4f)\n",elapsed_secs);

                            }














                            lPort->Close();
                            logPrintf("MAIN","Serial port closed");

                            // Delete device adapter (before freeing PvDevice!)
                            delete lDeviceAdapter;

                            delete lPipeline;

                            delete lMyPipelineEventSink;

                            // Close the stream
                            lStream->Close();
                            PvStream::Free( lStream );


                            // Disconnect the device
                            logPrintf("MAIN","Disconnecting device");
                            lDevice->Disconnect();
                            PvDevice::Free( lDevice );


                        } else {
                            logError("MAIN","Problem opening camera serial port");
                        }

                    } else {
                        logError("MAIN","Problem openeng camera device adapter");
                    }


                } else {
                    logError("MAIN","Problem registerig sink");
                }


            } else {
                logError("MAIN","Problem creatig pipeline");
            }
            
        } else {
            logError("MAIN","Problem opening stream");
        }

    } else {
        logError("MAIN","Problem opening device");
    }


    if (mainImageBuffer)
        free(mainImageBuffer);


    return 0;
}

