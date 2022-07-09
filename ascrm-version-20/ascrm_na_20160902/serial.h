/*
 * Copyright © Mega Designs Pvt. Ltd.
 *
 * This source code is not part of any free software and so
 * not avilable for free usage and distribution. Only the
 * specified authorties of CRIS, India can use it for integration
 * of Currency Operated Ticket Vending Machine and related projects.
 *
 *
 * File : serial.h
 *
 * Usage: Header to make physical data exchange from/to the serial port
 *
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_
#include "atvmdevice.h"
#include "atvmlog.h"

//#if defined(B2B_NOTE_ACCEPTOR) ||  defined(JCM_NOTE_ACCEPTOR)

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define B2B_SUCCESS             1
#define B2B_FAIL                0

#define BOARDUART                      
//#define USBUART    

int BNA_OpenPort(const int, int* const);
int BNA_ClosePort(const int);
int BNA_SendSingleByteToSerialPort(const int, const unsigned char);
int BNA_ReceiveSingleByteFromSerialPort(const int, unsigned char* const,int* const);
int BNA_ClearTransmitBuffer(const int);
int BNA_ClearReceiveBuffer (const int);


//#endif

#endif


