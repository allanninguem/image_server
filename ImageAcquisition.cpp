#include "ImageAcquisition.h"



void saveBuffer(PvBuffer *lBuffer) {

    int imageCounter = loadCounter();

    PvResult lResult = PvResult::Code::INVALID_PARAMETER;

    PvBufferWriter *lBufferWriter = new PvBufferWriter();
    char fileName[256];

    snprintf( fileName, 255, "output/img%04d.tiff", imageCounter );

    lResult = lBufferWriter->Store(lBuffer, fileName, PvBufferFormatTIFF, NULL);
    if ( lResult.IsOK() ) {

        imageCounter++;
        writeCounter(imageCounter);

    }

    delete lBufferWriter;
}




PvDevice *ConnectToDevice( const PvString &aConnectionID )
{
    PvDevice *lDevice;
    PvResult lResult;

    // Connect to the GigE Vision or USB3 Vision device

    lDevice = PvDevice::CreateAndConnect( aConnectionID, &lResult );

    return lDevice;
}

PvStream *OpenStream( const PvString &aConnectionID )
{
    PvStream *lStream;
    PvResult lResult;

    // Open stream to the GigE Vision or USB3 Vision device

    lStream = PvStream::CreateAndOpen( aConnectionID, &lResult );

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






void waitForBufferReady(MyPipelineEventSink *lMyPipelineEventSink)
{
    while (!lMyPipelineEventSink->isReady) {
        usleep(1);
    }
}



int AcquireImages( PvDevice *aDevice, PvStream *aStream, PvPipeline *aPipeline, MyPipelineEventSink *lMyPipelineEventSink, uint8_t *imageBuffer, int &imageSizeW, int &imageSizeH)
{
    int errorState = 0;


    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = aDevice->GetParameters();

    /*
    for (int i=0; i<lDeviceParams->GetCount(); i++) {
        PvGenParameter *par = lDeviceParams->Get(i);
        printf("%s\n",par->GetName().GetAscii());
    }
    */

    PvResult res = lDeviceParams->SetEnumValue("AcquisitionMode", "SingleFrame");
    res = lDeviceParams->SetIntegerValue("AcquisitionFrameCount", 1);

    //printf("%s\n",res.GetDescription().GetAscii());

    PvGenParameter *par = lDeviceParams->Get("AcquisitionFrameToSkip");

    //printf("%s\n",par->ToString().GetAscii());

    


    // Map the GenICam AcquisitionStart and AcquisitionStop commands
    PvGenCommand *aStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    PvGenCommand *aStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );


    // Note: the pipeline must be initialized before we start acquisition
    aPipeline->Start();

    // Enable streaming and send the AcquisitionStart command
    aDevice->StreamEnable();

    // start aqcuisition
    aStart->Execute();

    waitForBufferReady(lMyPipelineEventSink);


    PvBuffer *lBuffer = NULL;
    PvResult lOperationResult;

    // Retrieve next buffer


    PvResult lResult = aPipeline->RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );

    if ( lResult.IsOK() )
    {
        if ( lOperationResult.IsOK() )
        {
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

                // Get image specific buffer interface.
                PvImage *lImage = lBuffer->GetImage();

                // Read width, height.
                lWidth = lImage->GetWidth();
                lHeight = lImage->GetHeight();

                imageSizeW = lWidth;
                imageSizeH = lHeight;
                memcpy(imageBuffer, lBuffer->GetDataPointer(), imageSizeW*imageSizeH);


                //saveBuffer(lBuffer);

            } else {
                errorState = 1;
                //logError("IMG_ACQ","Buffer does not contain image");
            }
        }
        else
        {
            errorState = 1;
            //logError("IMG_ACQ","Buffer Retrieved ERROR! (Operation)");
        }

        // Release the buffer back to the pipeline
        aPipeline->ReleaseBuffer( lBuffer );
    }
    else
    {
        errorState = 1;
        //logError("IMG_ACQ","Buffer Retrieved ERROR! (result)");
    }


    // Tell the device to stop sending images.
    aStop->Execute();

    // Disable streaming on the device
    aDevice->StreamDisable();

    // Stop the pipeline
    aPipeline->Stop();
    
}
 
