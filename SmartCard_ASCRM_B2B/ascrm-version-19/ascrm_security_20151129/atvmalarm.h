#ifndef _ATVMALARM_H_
#define _ATVMALARM_H_

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//system header 
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cctalkdevs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define  COMMANDLENGTH                            9

#define  COMMANDREPLYLENGTH                       COMMANDLENGTH+5


//Coin acceptor Unit General Command Table
typedef struct AtvmAlarmCommands
{

   unsigned char Command[9];
  
}AtvmAlarmCommands;   


////////////////////////////////////////////////////////////////////////////////////////////////////////////


int setDisableAlarmState(const int,const int,const int,const int);//All times in minutes and maximum value is 255 minute

int getDoorOpenStatus(void); //returns 0 when door is in close state, returns 1 when door is in open state, returns -1 when error reading the status


int getAlarmOnStatus(void); //returns 0 when alarm is off, returns 1 when alarm is on, returns -1 when error reading the status


int getCashboxOpenStatus(void); //returns 0 when cashbox is in close state, returns 1 when cashbox is in open state, returns -1 when error reading the status

int getButtomDoorStatus(int*);

int getSecuredState(int* const , int* const , int* const,int* const); //Returns all status values in a single call

void* OpenVaultBox(void *ptr);

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
