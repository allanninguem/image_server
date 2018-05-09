#include "ImageServer.h"
#include "Utils.h"
#include "ImageAcquisition.h"




int main()
{
    PvDevice *lDevice = NULL;
    PvStream *lStream = NULL;
    PvGenCommand *lStop = NULL;
    PvGenCommand *lStart = NULL;


    logPrintf("MAIN","Image Server");

    PvString lConnectionID = CAM_IP;
    lDevice = ConnectToDevice( lConnectionID );
    if ( lDevice != NULL )
    {
        logPrintf("MAIN","Conected to device");

        lStream = OpenStream( lConnectionID );
        if ( lStream != NULL )
        {
            logPrintf("MAIN","Stream open");

            PvPipeline *lPipeline = NULL;

            ConfigureStream( lDevice, lStream );
            lPipeline = CreatePipeline( lDevice, lStream );
            if( lPipeline != NULL )
            {

                logPrintf("MAIN","Pipeline OK");

                MyPipelineEventSink *lMyPipelineEventSink = new MyPipelineEventSink();

                PvResult lResult = lPipeline->RegisterEventSink((PvPipelineEventSink *)lMyPipelineEventSink);

                if ( lResult.IsOK() ) {


                    for (int i=0; i<5; i++) {

                        clock_t begin = clock();

                        AcquireImages( lDevice, lStream, lPipeline, lMyPipelineEventSink );

                        clock_t end = clock();      
                        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                        printf("(%.4f)\n",elapsed_secs);

                    }


                    delete lPipeline;

                    delete lMyPipelineEventSink;

                } else {
                    logError("MAIN","Problem registerig sink");
                }


            }
            
            // Close the stream
            logPrintf("MAIN","Problem registerig sink");
            lStream->Close();
            PvStream::Free( lStream );
        }

        // Disconnect the device
        logPrintf("MAIN","Disconnecting device");
        lDevice->Disconnect();
        PvDevice::Free( lDevice );
    }


    return 0;
}

