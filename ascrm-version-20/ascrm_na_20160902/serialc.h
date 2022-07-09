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

#ifndef _SERIALC_H_
#define _SERIALC_H_

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

#define CCTALK_SUCCESS             1
#define CCTALK_FAIL                0
#define OPEN                       1
#define CLOSE                      0

#define BOARDUART                      
//#define USBUART                    

int OpenPort_C(const int, int* const);
int ClosePort_C(const int);
int SendSingleByteToSerialPort_C(const int, const unsigned char);
int ReceiveSingleByteFromSerialPort_C(const int, unsigned char* const,int* const);
int FlushSerialPortOutBuff_C(const int);
int FlushSerialPortOutBuff_C(const int);

#endif


