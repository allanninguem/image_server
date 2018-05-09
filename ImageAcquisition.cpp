#include "ImageAcquisition.h"



void saveBuffer(PvBuffer *lBuffer) {

    int imageCounter = loadCounter();

    PvResult lResult = PvResult::Code::INVALID_PARAMETER;

    PvBufferWriter *lBufferWriter = new PvBufferWriter();
    char fileName[256];

    snprintf( fileName, 255, "output/img%04d.tiff", imageCounter );

    lResult = lBufferWriter->Store(lBuffer, fileName, PvBufferFormatTIFF, NULL);
    if ( !lResult.IsOK() )
    {
        logError("IMG_ACQ","Problem saving image");
        return;
    } else {

        imageCounter++;
        writeCounter(imageCounter);

        logPrintf("IMG_ACQ","Image saved");
    }


    delete lBufferWriter;
}




PvDevice *ConnectToDevice( const PvString &aConnectionID )
{
    PvDevice *lDevice;
    PvResult lResult;

    // Connect to the GigE Vision or USB3 Vision device
    logPrintf("IMG_ACQ","Connecting to device");
    lDevice = PvDevice::CreateAndConnect( aConnectionID, &lResult );
    if ( lDevice == NULL )
    {
        logError("IMG_ACQ","Unable to connect to device");
    }

    return lDevice;
}

PvStream *OpenStream( const PvString &aConnectionID )
{
    PvStream *lStream;
    PvResult lResult;

    // Open stream to the GigE Vision or USB3 Vision device
    logPrintf("IMG_ACQ","Opening stream from device");
    lStream = PvStream::CreateAndOpen( aConnectionID, &lResult );
    if ( lStream == NULL )
    {
        logError("IMG_ACQ","Unable to stream from device");
    }

    return lStream;
}

void ConfigureStream( PvDevice *aDevice, PvStream *aStream )
{
    // If this is a GigE Vision device, configure GigE Vision specific streaming parameters
    PvDeviceGEV* lDeviceGEV = dynamic_cast<PvDeviceGEV *>( aDevice );
    if ( lDeviceGEV != NULL )
    {
        PvStreamGEV *lStreamGEV = static_cast<PvStreamGEV *>( aStream );

        // Negotiate packet size
        lDeviceGEV->NegotiatePacketSize();

        // Configure device streaming destination
        lDeviceGEV->SetStreamDestination( lStreamGEV->GetLocalIPAddress(), lStreamGEV->GetLocalPort() );
    }
}

PvPipeline *CreatePipeline( PvDevice *aDevice, PvStream *aStream )
{
    // Create the PvPipeline object
    PvPipeline* lPipeline = new PvPipeline( aStream );

    if ( lPipeline != NULL )
    {        
        // Reading payload size from device
        uint32_t lSize = aDevice->GetPayloadSize();
    
        // Set the Buffer count and the Buffer size
        lPipeline->SetBufferCount( BUFFER_COUNT );
        lPipeline->SetBufferSize( lSize );
    }
    
    return lPipeline;
}






void waitForBufferReady(MyPipelineEventSink *lMyPipelineEventSink) {
    while (!lMyPipelineEventSink->isReady) {
        usleep(1);
    }
}



void AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline, MyPipelineEventSink *lMyPipelineEventSink)
{



    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = aDevice->GetParameters();

    // Map the GenICam AcquisitionStart and AcquisitionStop commands
    PvGenCommand *aStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    PvGenCommand *aStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );


    // Note: the pipeline must be initialized before we start acquisition
    logPrintf("IMG_ACQ","Starting pipeline");
    aPipeline->Start();

    // Enable streaming and send the AcquisitionStart command
    logPrintf("IMG_ACQ","Enabling streaming and sending AcquisitionStart command");
    aDevice->StreamEnable();
    aStart->Execute();


    logPrintf("IMG_ACQ","Waitng for buffer to fill...");
    waitForBufferReady(lMyPipelineEventSink);



    PvBuffer *lBuffer = NULL;
    PvResult lOperationResult;

    // Retrieve next buffer


    PvResult lResult = aPipeline->RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );

    if ( lResult.IsOK() )
    {
        logPrintf("IMG_ACQ","Buffer Retrieved (result ok)");
        if ( lOperationResult.IsOK() )
        {
        	logPrintf("IMG_ACQ","Buffer Retrieved (operation ok)");

            PvPayloadType lType;

            //
            // We now have a valid buffer. This is where you would typically process the buffer.
            // -----------------------------------------------------------------------------------------
            // ...


            // If the buffer contains an image, display width and height.
            uint32_t lWidth = 0, lHeight = 0;
            lType = lBuffer->GetPayloadType();

            if ( lType == PvPayloadTypeImage )
            {

                logPrintf("IMG_ACQ","image received");

                // Get image specific buffer interface.
                PvImage *lImage = lBuffer->GetImage();

                // Read width, height.
                lWidth = lImage->GetWidth();
                lHeight = lImage->GetHeight();

                saveBuffer(lBuffer);

            } else {
                logError("IMG_ACQ","Buffer does not contain image");
            }
        }
        else
        {
            logError("IMG_ACQ","Buffer Retrieved ERROR! (Operation)");
            // Non OK operational result
        }

        // Release the buffer back to the pipeline
        aPipeline->ReleaseBuffer( lBuffer );
    }
    else
    {
        logError("IMG_ACQ","Buffer Retrieved ERROR! (result)");
    }


    // Tell the device to stop sending images.
    logPrintf("IMG_ACQ","Sending AcquisitionStop command to the device");
    aStop->Execute();

    // Disable streaming on the device
    logPrintf("IMG_ACQ","Disable streaming on the controller");
    aDevice->StreamDisable();

    // Stop the pipeline
    logPrintf("IMG_ACQ","Stoping pipeline");
    aPipeline->Stop();
    
}
 
