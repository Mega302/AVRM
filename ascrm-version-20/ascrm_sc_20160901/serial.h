/*
 * Copyright © Mega Designs Pvt. Ltd.
 *
 * 
 * File : serial.h
 *
 * Usage: Header to make physical data exchange from/to the serial port
 *
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include "atvmlog.h"
#include "SmartCardApi.h"

#define SUCCESS             1

#define FAIL                0

#define POLYNOMIAL 0x08408 

#define MAX_WAIT_TIME_COMPORT   3 //DEFAULT  1 SECOND




//////////////////////////////////////////////////////////////////////////////////////////////////


int  OpenPort(const int port_nmbr, int baudrate,int* const h_commport);

int ClosePort(const int);

int SendSingleByteToSerialPort(const int, const unsigned char);

int ReceiveSingleByteFromSerialPort(const int, unsigned char* const,int* const);

int ClearTransmitBuffer(const int);

int ClearReceiveBuffer (const int);

unsigned int GetCRC16(unsigned char* bufData, unsigned int sizeData);



unsigned char getBcc(int length , unsigned char* const rcvPkt );

int statusRead( int pcomhandler, 
                unsigned char* const rcvPkt, 
                unsigned int* const rcvPktLen,
                int totalByteToRecv 
              );

int statusRead_V2(   int pcomhandler, 
                     unsigned char* rcvPkt, 
                     unsigned int*  rcvPktLen
                 );



#endif


