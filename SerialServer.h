#ifndef __SERIALSERVER_H__
#define __SERIALSERVER_H__

#include <PvDevice.h>
#include <PvDeviceSerialPort.h>
#include <PvDeviceAdapter.h>

#include <string.h>


#define RX_BUFFER_SIZE 1024
#define CMD_BUFFER_SIZE 1024

#define SPEED ( "Baud115200" )
#define STOPBITS ( "One" )
#define PARITY ( "None" )

PvDeviceAdapter *CreateDeviceAdapter(PvDevice *lDevice);
PvDeviceSerialPort *ConfigureSerial(PvDevice *lDevice, PvDeviceAdapter *lDeviceAdapter);
void SendCommand(PvDeviceSerialPort *lPort, char *strCmd);
void ReceiveResult(PvDeviceSerialPort *lPort, char *buffer);
void TerminateCommunication(PvDeviceAdapter *lDeviceAdapter, PvDeviceSerialPort *lPort);

void ListenSerial(PvDeviceSerialPort *lPort, char *mainCmdBuffer, char *mainResultBuffer, int &isAquiring, int &isTransmiting);

#endif // __SERIALSERVER_H__
