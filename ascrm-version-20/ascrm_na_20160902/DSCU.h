#ifndef _DSCU_H_
#define _DSCU_H_

#include "atvmdevice.h"

#ifdef B2B_TRAP_GATE


//Linux System Headers
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

//user hedaers files
#include "atvmdevice.h"
#include "readini.h"
#include "serialgen.h"

/*
 Normal DSCU Unit Response reply pakt lngth
(without data, Data may be 0-250 bytes) =sync+adr+length+crclsb+crcmsb=5byte
*/

//Macros

//Debug Macro for display packet communication between pc and dscu unit
//#define DEBUG

//Main Macro for turn on main()
//#define DSCUMAIN


#define POLL_DELAY

#define DSCU_SUCCESS                      1
#define DSCU_FAIL                         0

#define DSCU_FUN_PARAMETER_NOT_OK         0

#define DSCU_WAIT_TIME_MS                 5000 // DSCU inactivity time = 5 sec (5000ms) for DSCU reply 

#define DSCU_POLL_INTERVAL_DELAY          100  //In Millisecond

//CRC-CCITT POLYNOMIAL
#define DSCU_POLYNOMIAL 0x08408

//Drop and Seal Unit State Defined enum variables

//DSCU Reply Bytes Indexlist for various operation
typedef enum ReplyBytesIndexId
{
 
 ReplyPaktLengthIndex=2,      //This index in reply bytes array define where whole reply bytes length  value defined
 ReplyPaktStateIndex =3,      //This index in reply bytes array define where dscu unit current state defined
 ReplyPaktErrorStateIndex=5   //This index in reply bytes array define where dscu unit current state defined

}ReplyBytesIndexId;

/* Defines Drop and Seal Unit STATE */
typedef enum DSCU_STATE      
{
    Powerup=0,       
    Initializing=1,         
    Idling=2,         
    Failure=3,
    Busy=4,      
    Dropping=5,
    DropOk=6,
    DropFailed=7,
    UndefinedState=8
}DSCU_STATE;  

/* Defines Drop and Seal Unit ERROR STATE */
typedef enum DSCU_ERROR_STATE       
{
    DropCassettesPlateMotorFailure=0,
    DropCassettesPlateSensorsFailure=1,
    DropCassettesGateMotorFailure=2,
    DropCassettesGatesSensorFailure=3,
    MainPowerFailure=4,
    PowerSwitchFailure=5,
    UndefinedErrorState=6
}DSCU_ERROR_STATE; 

//DropGate Type or Dropgate Operation type Enum variable
typedef enum DropGate     
{
    DropGateOpen=0,       
    DropGateClose=1
}DropGate;  


//Drop And Seal Unit Command List Structure

//Drop And Seal Unit General Command Table
typedef struct DSCUCommands
{
   unsigned char Command[6];
}DSCUCommands;   

//Drop And Seal Unit Special Command Table
typedef struct DSCUSpclCommands
{
   unsigned char Command[8];
}DSCUSpclCommands; 

//Drop and Seal Unit Normal State Tables
typedef struct DSCUState
{
   unsigned char    StateHexValue;
   DSCU_STATE       StateValue;
}DSCUState;   

//Drop and Seal Unit Error State Tables
typedef struct DSCUErrorState
{
   unsigned char      ErrorStateHexValue;
   DSCU_ERROR_STATE   ErrorStateValue;
}DSCUErrorState;

typedef struct DSCUPollReplyState
{
   DSCU_STATE         CurrentStatus;
   DSCU_ERROR_STATE   CurrentErrorStatus;
   bool BagClampOpen;
   bool DropCastGateOpen;
   bool DropCastGateClose;
   bool ServiceMode;
   bool DropCassettesPlateMotorFailure;
   bool DropCassettesPlateSensorsFailure;
   bool DropCassettesGateMotorFailure;
   bool DropCassettesGatesSensorFailure;
   bool MainPowerFailure;
   bool PowerSwitchFailure;
}DSCUPollReplyState;

                  
//Drop and Seal Unit Lower level Functions
static unsigned int       OpenDSCU (unsigned int);
static unsigned int       CloseDSCU(unsigned int);
static unsigned int       DSCUSendCommand(unsigned char*,unsigned int,unsigned char*,unsigned int,unsigned int);  
static unsigned int       DSCUReplyCRCCheck(unsigned char*,unsigned int);
static DSCUPollReplyState DSCUPollReplyPktAnalysis(unsigned char*,unsigned int);
static unsigned int       DSCUAckReplyPktAnalysis(unsigned  char*);
static void               Delay(unsigned int);
static unsigned int       DSCUGetCRC16(unsigned char*,unsigned int,unsigned char*,unsigned char*);

//Drop and Seal Unit Middle level Functions
static unsigned int       DSCUSendPoll(unsigned char *,unsigned int,int);
static unsigned int       DSCUSendAck();
static unsigned int       DSCUSendDropOpen(int);
static unsigned int       DSCUSendReset(int);
static unsigned int       DSCUSendDropClose(int);
static unsigned int       DSCUSendIdentification(unsigned char*,unsigned int,int);
static unsigned int       DSCUSendSetoption(unsigned char*,int);

//Drop and Seal Unit Upper level Functions (Main API Interface) (My Api List)
static int GetCurrentDSCUState(void); 
static int IsDSCUInIdle(void);
static int GetDSCUDropGateStatus(void);
static int ResetDSCU(void);
static int GetDSCUStatus(void);
static unsigned int DSCUGetReply   (unsigned int,unsigned char*,unsigned int,unsigned char*,unsigned int,unsigned int);
static int DSCURecvData            (unsigned int,unsigned char*,unsigned int,unsigned int);

static int DSCUPolling(void);
static int SetDSCUDropGateOperation         (DropGate);
static int ContinuousPollandGetDropStatus   (DropGate);
static int ContinuousPollStatus             (unsigned char*,unsigned,unsigned char);
static int GetDoorStatusDuringPoll          (unsigned char*, unsigned int,DropGate);
static int CheckDSCUBeforeIssueDropCommand  (DropGate);


//For ATVM API
int IsDropandSealUnitPortOpen(void);
int OpenDropandSealUnit  (unsigned int);
int CloseDropandSealUnit (void);
int AtvmGetDSCUStatus    (void);
int DropNotes            (void);
int GetDSCULastErroCode  (void);

#endif

#endif
