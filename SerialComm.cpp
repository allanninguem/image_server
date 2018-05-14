#include "SerialComm.h"
#include "Utils.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

PvDeviceAdapter *CreateCameraDeviceAdapter(PvDevice *lDevice)
{
    // Create PvDevice adapter
    PvDeviceAdapter *lDeviceAdapter = new PvDeviceAdapter( lDevice );

    return lDeviceAdapter;
}



PvDeviceSerialPort *ConfigureCameraSerial(PvDevice *lDevice, PvDeviceAdapter *lDeviceAdapter) 
{
    PvResult lResult = PvResult::Code::INVALID_PARAMETER;

    // Get device parameters need to control streaming
    PvGenParameterArray *lParams = lDevice->GetParameters();

    // Configure serial port - this is done directly on the device GenICam interface, not 
    // on the serial port object! 

    lParams->SetEnumValue( "BulkSelector", "Bulk0" );
    lParams->SetEnumValue( "BulkMode", "UART" );
    lParams->SetEnumValue( "BulkBaudRate", SPEED );
    lParams->SetEnumValue( "BulkNumOfStopBits", STOPBITS );
    lParams->SetEnumValue( "BulkParity", PARITY );

    lParams->SetBooleanValue( "BulkLoopback", false );

    // Open serial port
    PvDeviceSerialPort *lPort = new PvDeviceSerialPort();

    lResult = lPort->Open( lDeviceAdapter, PvDeviceSerialBulk0 );

    if ( !lResult.IsOK() )
    {
        return 0;
    }

    uint32_t lSize = RX_BUFFER_SIZE;
    
    // Make sure the PvDeviceSerialPort receive queue is big enough to buffer all bytes 
    // Note that for every iteration in the test below, lSize is doubled so lSize << TEST_COUNT is the size (2x to give extra)
    lPort->SetRxBufferSize( RX_BUFFER_SIZE );

    return lPort;
}



int SendCameraCommand(PvDeviceSerialPort *lPort, char *lInBuffer)
{
    int lSize;
    int errorState = 0;

    PvResult lResult = PvResult::Code::INVALID_PARAMETER;

    // Send the buffer content on the serial port
    uint32_t lBytesWritten = 0;
    lResult = lPort->Write( (uint8_t *)lInBuffer, strlen(lInBuffer), lBytesWritten );
    if ( !lResult.IsOK() ) {
        // Unable to send data over serial port!
        errorState = 1;
    }


    return errorState;
}




int ReceiveCameraResult(PvDeviceSerialPort *lPort, char *lOutBuffer)
{
    int errorState = 0;
    PvResult lResult = PvResult::Code::INVALID_PARAMETER;

    uint32_t lTotalBytesRead = 0;
    uint32_t lBytesRead = 0;
    do 
    {
        lResult = lPort->Read( (uint8_t *)lOutBuffer + lTotalBytesRead, RX_BUFFER_SIZE, lBytesRead, 500 );

        //if (!lResult.IsOK()) {
        //    errorState = 1;
        //}

        lTotalBytesRead += lBytesRead;
    } while ( (lBytesRead>0) && (lTotalBytesRead<RX_BUFFER_SIZE) );

    if (lTotalBytesRead>=2) {

        lOutBuffer[lTotalBytesRead-3] = 13;
        lOutBuffer[lTotalBytesRead-2] = 10;
        lOutBuffer[lTotalBytesRead-1] = 0;

    } else {

        lOutBuffer[0] = 13;
        lOutBuffer[1] = 10;
        lOutBuffer[2] = 0;
    }


    // always returns something (even if it is an \n\r)
    return 0;
}















