#include "SerialServer.h"
#include "Utils.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

PvDeviceAdapter *CreateDeviceAdapter(PvDevice *lDevice)
{
    // Create PvDevice adapter
    PvDeviceAdapter *lDeviceAdapter = new PvDeviceAdapter( lDevice );

    return lDeviceAdapter;
}



PvDeviceSerialPort *ConfigureSerial(PvDevice *lDevice, PvDeviceAdapter *lDeviceAdapter) 
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
        logError("SERIAL","Unable to open serial port on device");
        return NULL;
    }
    logPrintf("SERIAL","Serial port opened");

    uint32_t lSize = RX_BUFFER_SIZE;
    
    // Make sure the PvDeviceSerialPort receive queue is big enough to buffer all bytes 
    // Note that for every iteration in the test below, lSize is doubled so lSize << TEST_COUNT is the size (2x to give extra)
    lPort->SetRxBufferSize( RX_BUFFER_SIZE );

    return lPort;
}



void SendCommand(PvDeviceSerialPort *lPort, char *strCmd)
{
    int lSize;

    PvResult lResult = PvResult::Code::INVALID_PARAMETER;

    uint8_t *lInBuffer = new uint8_t[ CMD_BUFFER_SIZE ];
    

    // Fill input buffer with cmd
    lSize = strlen(strCmd);
    memcpy(lInBuffer, strCmd, lSize );


    // Send the buffer content on the serial port
    uint32_t lBytesWritten = 0;
    lResult = lPort->Write( lInBuffer, lSize, lBytesWritten );
    if ( !lResult.IsOK() ) {
        // Unable to send data over serial port!
        logError("SERIAL","Error sending data over the serial port");
    }

    //logPrint("SERIAL", "Sent: %s (size: %d)\n", strCmd , lBytesWritten );

    if ( lInBuffer != NULL )
    {
        delete []lInBuffer;
        lInBuffer = NULL;
    }

}

void ReceiveResult(PvDeviceSerialPort *lPort, char *buffer)
{

    PvResult lResult = PvResult::Code::INVALID_PARAMETER;

    uint8_t *lOutBuffer = new uint8_t[ RX_BUFFER_SIZE ];
    lOutBuffer[0] = 0;

    uint32_t lTotalBytesRead = 0;
    uint32_t lBytesRead = 0;
    do 
    {
        lResult = lPort->Read( lOutBuffer + lTotalBytesRead, RX_BUFFER_SIZE, lBytesRead, 500 );
//        if ( lResult.GetCode() == PvResult::Code::TIMEOUT )   {
//            error("SERIAL","Timeout");
//        }
        // Increments read head
        lTotalBytesRead += lBytesRead;
    } while ( (lBytesRead>0) && (lTotalBytesRead<RX_BUFFER_SIZE) );

    if (lTotalBytesRead>0) {
        logPrintf("SERIAL","Received from serial");
        lOutBuffer[lTotalBytesRead-1] = 0;
    } else {
        logError("SERIAL","Did not receive anything....");
    }

    //logPrint("SERIAL","Received: %s (size: %d)\n", lOutBuffer , lTotalBytesRead );

    memcpy(buffer, lOutBuffer, lTotalBytesRead);

    if ( lOutBuffer != NULL )
    {
        delete []lOutBuffer;
        lOutBuffer = NULL;
    }

}















