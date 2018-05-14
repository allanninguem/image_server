#ifndef __SERIALCOMM_H__
#define __SERIALCOMM_H__

#include <PvDevice.h>
#include <PvDeviceSerialPort.h>
#include <PvDeviceAdapter.h>

#include <string.h>


#define RX_BUFFER_SIZE 1024
#define CMD_BUFFER_SIZE 1024

#define SPEED ( "Baud115200" )
#define STOPBITS ( "One" )
#define PARITY ( "None" )

PvDeviceAdapter *CreateCameraDeviceAdapter(PvDevice *lDevice);
PvDeviceSerialPort *ConfigureCameraSerial(PvDevice *lDevice, PvDeviceAdapter *lDeviceAdapter);
int SendCameraCommand(PvDeviceSerialPort *lPort, char *lInBuffer);
int ReceiveCameraResult(PvDeviceSerialPort *lPort, char *lOutBuffer);
void TerminateCameraCommunication(PvDeviceAdapter *lDeviceAdapter, PvDeviceSerialPort *lPort);
int SendResultBuffer(int clientSocket, char *lInBuffer);

//void ListenSerial(PvDeviceSerialPort *lPort, char *mainCmdBuffer, char *mainResultBuffer, int &isAquiring, int &isTransmiting);

#endif // __SERIALCOMM_H__
