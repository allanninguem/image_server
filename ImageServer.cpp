#include "ImageServer.h"
#include "Utils.h"
#include "ImageAcquisition.h"
#include "SerialComm.h"

#define RX_BUFFER_SIZE 1024
#define CMD_BUFFER_SIZE 1024

uint8_t *mainImageBuffer;
int mainImageW;
int mainImageH;
char mainCmdBuffer[CMD_BUFFER_SIZE];
char mainResultBuffer[RX_BUFFER_SIZE];
int imageCounter;

int main()
{
    PvDevice *lDevice = NULL;
    PvStream *lStream = NULL;
    PvGenCommand *lStop = NULL;
    PvGenCommand *lStart = NULL;
    PvDeviceAdapter *lDeviceAdapter = NULL;
    PvDeviceSerialPort *lPort = NULL;



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

                    lDeviceAdapter = CreateDeviceAdapter(lDevice);

                    if (lDeviceAdapter) {

                        lPort = ConfigureSerial(lDevice, lDeviceAdapter);

                        if (lPort) {














                            for (int i=0; i<5; i++) {

                                clock_t begin = clock();

                                // aquire image into main buffer
                                // TODO: return false on error
                                printf("%x\n", mainImageBuffer);
                                AcquireImages( lDevice, lStream, lPipeline, lMyPipelineEventSink, mainImageBuffer, mainImageW, mainImageH );

                                // saves output/imgxx.raw.gz
                                saveBufferRawImage(mainImageBuffer, mainImageW, mainImageH);

                                // sends mainCmdBuffer to camera serial interface
                                SendCommand(lPort, mainCmdBuffer);
                                
                                usleep(1000);

                                // receive camera serial interface response
                                ReceiveResult(lPort, mainResultBuffer);
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

