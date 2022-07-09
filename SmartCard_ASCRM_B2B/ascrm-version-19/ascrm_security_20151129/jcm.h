#ifndef  _JCM_H_
#define  _JCM_H_

#include "atvmdevice.h"

#include "common.h"

#ifdef JCM_NOTE_ACCEPTOR


///////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>   // Standard input/output definitions 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <fcntl.h>   // File control definitions 
#include <errno.h>   // Error number definitions 
#include <termios.h> // POSIX terminal control definitions 
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sched.h>

#include "serial.h"
#include "atvmlog.h"
#include "cctalkdevs.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                           //Macro Table//

#define JCM_STX                    0x02
#define JCM_ETX                    0x03
#define JCM_EOT                    0x04
#define JCM_ENQ                    0x05
#define JCM_DLE                    0x10
#define JCM_DLE0_FIRST_BYTE        0x10
#define JCM_DLE0_SECOND_BYTE       0x30
#define JCM_DLE1_FIRST_BYTE        0x10
#define JCM_DLE1_SECOND_BYTE       0x31
#define JCM_NAK                    0x15


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define JCM_RESET                              0x30
#define JCM_SENSE                              0x40
#define JCM_INSERTION_AUTHORISED               0x50
#define JCM_INSERTION_INHIBITED                0x51
#define JCM_RECEIPT                            0x52
#define JCM_RETURN                             0x53
#define JCM_INTAKE                             0x54
#define JCM_REJECT                             0x56
#define JCM_ESCROW                             0x55

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JCM_CMD_CMN_LENGTH                     6

#define JCM_CMD_INSERTION_AUTH_LENGTH          8

#define JCM_CMD_DITTO_INSERTION_AUTH_LENGTH    9

#define JCM_RECV_DELAY                      80  //60 tested//in milliseconds (Between Transmit and Recv reply) tested 100

#define JCM_HANDSHAKE_DELAY                 80  //80 //70  tested in milliseconds (between each step of transmit and receieve) tested 100/50/40/30/20

#define JCM_RESPONSE_BUFFER_SIZE            50

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JCM_RESPONSE_TIME                   2         //SECONDS [default:5 Second]

#define JCM_RETURN_TIME                     3600      //SECONDS [Default:1hr]

#define JCM_UNLOAD_TIME                     300       //SECONDS [Default:5Min]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JCM_STANDBY_STATE_WAIT_TIME           300     //SECONDS [default: 5 min= 300 seconds]

#define JCM_REJECTION_OVER_WAIT_TIME          3600     //SECONDS [default: 60 min= 3600 seconds]

#define JCM_STANDBY_WAIT_TIME                 300      //SECONDS [default: 5 min=  300 seconds]

#define JCM_SENSOR_OVER_WAIT_TIME             300      //SECONDS [default: 5 min= 300 seconds]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define JCM_ENABLE_DELAY_DURING_CREDIT_POLLING       160    //MILLISECONDS

#define JCM_WAIT_DELAY_DURING_CREDIT_POLLING         100    //MILLISECONDS

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JCM_MAX_TRY_FOR_DLE0                          4

#define JCM_MAX_TRY_FOR_DLE1                          4

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JCM_RESET_REPLY_SIZE                                10

#define JCM_SENSE_REPLY_SIZE                                28

#define JCM_INSERTION_AUTHORIZING_REPLY_SIZE                25

#define JCM_INSERTION_INHIBITING_REPLY_SIZE                 25

#define JCM_INSERTION_INHIBITING_RSP_REPLY_SIZE             25

#define JCM_RECEIPT_REPLY_SIZE                              27

#define JCM_RETURN_REPLY_SIZE                               25

#define JCM_INTAKE_REPLY_SIZE                               27

#define JCM_RSP_REPLY_SIZE                                  26    //must

#define JCM_ESCROW_REPLY_SIZE                               25

#define JCM_REJECT_REPLY_SIZE                               25

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JCM_ENABLE                                          1

#define JCM_CASH_IN_WAIT_STANDBY_MODE                       60   //Seconds [2 Min = 120 seconds]


#define JCM_ENQ_WAIT_FOR_RESET                              30   //IN Seconds

//#define JCM_RECV_TIME_DEBUG 
//#define JCM_TRANS_TIME_DEBUG
//#define JCM_DEBUG         
//#define JCM_CREDIT_POLL_DEBUG    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JCM_UNLOAD_OPERATION        1   

#define JCM_DISPENSE_OPERATION      2


#define JCM_INTERNAL_COMMAND_REPLY_READ      0

#define JCM_EXTERNAL_COMMAND_REPLY_READ      1

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define  JCM_ACCEPTING      	            0x04   

#define  JCM_STACKED                        0x12

#define  JCM_DISABLED_WITHOUT_ESCROW        0x02

#define  JCM_ENABLED_WITHOUT_ESCROW         0x03

#define  JCM_ALARM                          0x80

#define  JCM_SENSE_TROUBLE                  0x81

#define  JCM_REJECTION                      0x08

#define  JCM_RETURN_WAITING                 0x09 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                                  //Command Table//

#define JCM_RESET_CMD                   {JCM_DLE,JCM_STX,JCM_RESET,JCM_DLE,JCM_ETX,0x00}   //1  Reset

#define JCM_SENSE_CMD                   {JCM_DLE,JCM_STX,JCM_SENSE,JCM_DLE,JCM_ETX,0x00}   //1  Sense


#define JCM_INSERTION_AUTHORISED_CMD    {JCM_DLE,JCM_STX,JCM_INSERTION_AUTHORISED,0x00,0x00,JCM_DLE,JCM_ETX,0x00}  //3  Insertion Authorized

#define JCM_INSERTION_AUTHORISED_DITTO_CMD {JCM_DLE,JCM_STX,JCM_INSERTION_AUTHORISED,0x00,0x10,0x00,JCM_DLE,JCM_ETX,0x00}  //3  Insertion Authorized


#define JCM_INSERTION_INHIBIT_CMD       {JCM_DLE,JCM_STX,JCM_INSERTION_INHIBITED,JCM_DLE,JCM_ETX,0x00}             //4  Insertion Inhibited

#define JCM_RECEIPT_CMD                 {JCM_DLE,JCM_STX,JCM_RECEIPT,JCM_DLE,JCM_ETX,0x00}                    //5  Receipt

#define JCM_RETURN_CMD		        {JCM_DLE,JCM_STX,JCM_RETURN,JCM_DLE,JCM_ETX,0x00}    //6  Return


#define JCM_INTAKE_CMD                  {JCM_DLE,JCM_STX,JCM_INTAKE,JCM_DLE,JCM_ETX,0x00}    //7  Intake


#define JCM_ESCROW_CMD                  {JCM_DLE,JCM_STX,JCM_ESCROW,JCM_DLE,JCM_ETX,0x00}    //7   Escrow


#define JCM_REJECT_CMD                  {JCM_DLE,JCM_STX,JCM_REJECT,JCM_DLE,JCM_ETX,0x00}    //7   Reject


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                             //Global Structure Table//

/*

MASKING VALUE FOR ENABLE DENOM IN JCM NOTE ACCEPTOR
bit position:   8   7  6  5  4  3  2  1     : Note value
                ---------------------------------------------------------------
                0   0  0  0  0  0  0  1 b   = 5     //0b00000001
	        0   0  0  0  0  0  1  0 b   = 10    //0b00000010
	        0   0  0  0  0  1  0  0 b   = 20    //0b00000100;
	        0   0  0  0  1  0  0  0 b   = 50    //0b00001000;
	        0   0  0  1  0  0  0  0 b   = 100   //0b00010000
	        0   0  1  0  0  0  0  0 b   = 500   //0b00100000
	        0   1  0  0  0  0  0  0 b   = 1000  //0b01000000

*/


typedef struct NOTE
{

   unsigned int INR_5:1;
   unsigned int INR_10:1;
   unsigned int INR_20:1;
   unsigned int INR_50:1;
   unsigned int INR_100:1;
   unsigned int INR_500:1;
   unsigned int INR_1000:1;

}NOTE;



typedef struct JCM_SENSOR_STATE
{
   
   //group 1
   int sensor_validation_ptc;
   int sensor_validation_ptr;
   int sensor_acceptor_lid_detection;
   int sensor_insertion_detection;

   //group 2
   int sensor_validation_sensor_ptl;
   int sensor_outlet_shutter;
   int sensor_freefall;
   
   //group 3 
   int sensor_position_s13;
   int sensor_position_s14;
   int sensor_position_s15;
   int sensor_stacking_control;
   int sensor_insertion_detection_right;

 
}JCM_SENSOR_STATE;



typedef struct JCM_ESCROW_NOTE
{

   unsigned int AcceptedNumber_INR_5;
   unsigned int AcceptedNumber_INR_10;
   unsigned int AcceptedNumber_INR_20;
   unsigned int AcceptedNumber_INR_50;
   unsigned int AcceptedNumber_INR_100;
   unsigned int AcceptedNumber_INR_500;
   unsigned int AcceptedNumber_INR_1000;

}JCM_ESCROW_NOTE;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                                                //API Fucntions Chart//

int JCM_Activate(const int);

int JCM_InitAfterPortOpen(void);

int JCM_Reset();

int JCM_AcceptEscrowMoney();

int JCM_DefaultCommit();

int JCM_RejectEscrowMoney();

int JCM_DefaultCancel();

int JCM_Activate(const int);

int JCM_Deactivate(void);

int JCM_ReInit(int);

int JCM_LogSensorState(unsigned char,unsigned char,unsigned char);

int JCM_GetStatus(void);

int JCM_Unload(void);

int JCM_Return(unsigned char*);

int JCM_Dispense(void);

int JCM_Receipt(void);

int JCM_Intake(void);

int JCM_GetAcceptedMoney(int*,int*);

int JCM_EnableDenom(NOTE);

int DisableSpecificNotes(int);

int DisableAcceptance();

int Disable_Acceptance_During_RSPPoll();

int JCM_GetAcceptedMoney(int*,int*);

void DelayInSeconds(int);

int  JCM_CashIn(int fare,int *State,int *Credit);

unsigned int  JCM_CashInEnd(int *);

unsigned int  JCM_CashInStart();

unsigned char GetBCC(unsigned char* Packet,int StartIndex,unsigned int Packetlength);

static int ReadEnqFromJcm();

static int ReadEnqFromJcmWithinTime(int);

static int ReadReceptibleState(int);

unsigned int Make_JCM_StandBy_Mode(unsigned int );

int JCM_Make_StandBy(int);

void GetEscrowMoneyPatch(JCM_ESCROW_NOTE *currentnote,unsigned char *Response,int EscrowIndex);

inline void DisplayEscrowTable(JCM_ESCROW_NOTE *currentnote);

int GetDeonm(JCM_ESCROW_NOTE *Currentnote,JCM_ESCROW_NOTE *Oldnote);

unsigned int  JCM_NoteAcptrCreatePollThread();



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Common Note Acceptor Generic API

int  OpenCashAcptr(int noteAcptrPortNmbr);

int  Deactivate_JCM_NoteAcptr();

int  UnloadCashAcptr();

int  DispenseCashAcptr();

int GetNoteDetailsInRecycleCst( int *CastQuanity );
 
int  GetNoteAcptrStatus(void);

unsigned int  NoteAcptrStartCreditPollThread();

unsigned int  DeactivateJCMNoteAcptr();

unsigned int  NoteAcptrUnloadEscrow();

unsigned int  NoteAcptrDispenseEscrow();

inline void  CloseNoteAcceptorPollThread();

unsigned int WaitforNoteAcptrLowlevelflagClosed();

inline void InitNATransModel();

inline int SetAlreadyCoinInhibit();

inline int SetAlreadyCoinInhibit();

inline void  JCM_CloseNoteAcceptorPollThread();

static unsigned int JCM_GetDenom_From_EscrowState(unsigned char* Response, JCM_ESCROW_NOTE *currentnote);

int EnableDenomDuringCreditPolling(int fare,int flag);

unsigned int  JCM_Atvm_InitCashInStartThread();

int GetEscrowMoneyPatchV2(unsigned char *Response,int EscrowIndex);

inline void DisplayEscrowTableV2(unsigned char *currentnote,int EscrowIndex);

int enableSpecificBillsAndMakeIdle( int fare, int maxNoteQtyAllowed);

static  void* JCM_RSP_Poll(void *ptr);

unsigned int WaitforNoteAcptrLowlevelflagClosed(double);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                       // Command Functions //

//Make JCM note acceptor initialized

int JCM_Cmd_Reset( int );

int JCM_Cmd_Return(unsigned char *Response,const int ResponseLength,const unsigned int Delay);

int JCM_Cmd_Sense(unsigned char*,const int,const unsigned int);

int JCM_Cmd_InsertionAuthorized(unsigned char,unsigned char*,const int,const unsigned int);

int JCM_Cmd_InsertionInhibited(unsigned char*,const int,const unsigned int);

int JCM_Cmd_Receipt(unsigned char*,const int,const unsigned int);

int JCM_Cmd_Intake(unsigned char*,const int,const unsigned int);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
                                //Low Level communication Functions//

static int  JCM_OpenComPort(const int);

static int  JCM_CloseComPort(const int);

static int  JCM_TransmitCmdSequence(const int ,unsigned char* const,const unsigned int); 
  
static int  JCM_ReceieveCmdReplySequence(const int,unsigned char* ,const unsigned int); 

static int JCM_ReceieveReplyWithoutReadENQ( const int HANDLE,unsigned char*  Response,const unsigned int NumberofBytestobeRead);

static int JCM_Complete_ReceieveCmdReplySequence(int ); 

static void JCM_delay_miliseconds(const long long int);

static int  JCM_ReadResponse(int,unsigned char*,const int,int* const);

int JCM_GetReply(unsigned int,unsigned int,unsigned char*,unsigned char *,int);

bool GetNADisableAcceptanceSignal();

inline int JCM_Wait(unsigned int Second);

static int EscrowIssueEOTPatch();

static int IssueEscrowCommand();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

#endif
