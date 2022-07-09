#ifndef  _ATVMPARPORT_H_
#define  _ATVMPARPORT_H_

#include  <stdio.h> 
#include  <fcntl.h>     
#include  <sys/types.h>   
#include  <sys/stat.h>   
#include  <asm/ioctl.h> 
#include  <linux/parport.h>
#include  <linux/ppdev.h>
#include  <string.h>

#include "atvmlog.h"

//#define PARPORT_MAIN

#define DEVICE "/dev/parport0" 


static int write_data(int fd, unsigned char data) ;
static int read_data(int fd);
static int strobe_blink(int fd);
static void PrintTestPage(int fd);
static ssize_t write_printer (int fd, const char *ptr, size_t count);

int OpenParPort();
int CloseParPort(int devhandle);
int status_pins(int devhandle);
#endif
