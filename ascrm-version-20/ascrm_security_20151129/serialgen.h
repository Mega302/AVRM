/*
 * Copyright © Mega Designs Pvt. Ltd.
 *
 * This source code is not part of any free software and so
 * not avilable for free usage and distribution. Only the
 * specified authorties of CRIS, India can use it for integration
 * of Currency Operated Ticket Vending Machine and related projects.
 *
 *
 * File : serialB2B.h
 *
 * Usage: Header to make physical data exchange from/to the serial port
 *
 */

#ifndef _SERIALGEN_H_
#define _SERIALGEN_H_

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define SERIALB2B_SUCCESS             1
#define SERIALB2B_FAIL                0

int GenricOpenPort(const int, int* const);
int GenricClosePort(const int);
int GenricSendSingleByteToSerialPort(const int, const unsigned char);
int GenricReceiveSingleByteFromSerialPort(const int, unsigned char* const,int* const);
int GenricClearTransmitBuffer(const int);
int GenricClearReceiveBuffer (const int);

#endif


