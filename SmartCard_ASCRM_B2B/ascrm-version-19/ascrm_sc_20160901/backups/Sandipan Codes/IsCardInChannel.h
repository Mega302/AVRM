
#ifndef _IsCardInChannel_h_
#define _IsCardInChannel_h_

#include "delaytime.h"
#include "serial.h"
#include "ConnectDevice.h"

/*

  
  STX (F2H)  = Representing the start of text in a command or a response
  ADDR = Representing the address of MTK-571
  LENH(1 byte) = Length of high byte of text
  LENL(1 byte) = Length of low byte of text 
  CMT = Command head (āCā , 43H )
  CM = Specify as command
  PM = Command parameter
  DATA = Transmission data ( N byte, N=0~512)
  ETX (03H) = End of text
  BCC = XOR

*/





int IsCardInChannel_c(int Timeout);


#endif


