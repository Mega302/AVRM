

#ifndef _B2B_H_
#define _B2B_H_
 
#include "atvmdevice.h"

#ifdef  B2B_NOTE_ACCEPTOR


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "atvmdevice.h"
#include "serial.h"
#include "common.h"




//#define B2B_MAIN //Malay add 11 2013 //turn on b2b main

#define B2B_ACPTR                  
#ifdef  B2B_ACPTR
        #define    DEVICE_ADDRESS                 0x01
#else
        #define    DEVICE_ADDRESS                 0x03
#endif



//Write fault log about b2b unit
//#define ERROR_LOG_ON 

//#define DISPLAY_PKT                // To display received pkt, uncomment this.  

//#define DISPLAY_PKT_DETAIL  


//#define B2B_LOGLEVEL_ONE       //api level       
//#define B2B_LOGLEVEL_TWO       //middle lower level
//#define B2B_LOGLEVEL_THREE     //lower level
//#define B2B_LOGLEVEL_FOUR
//#define B2B_LOGLEVEL_FIVE


//========================= End: PROGRAM FLOW CONTROL MACROS ======================


#define  MAX_NOTE_TYPE                   7
#define  POLYNOMIAL                      0x8408
#define  B2B_MAX_CMD_LEN                 30      //25
#define  MAX_LEN_OF_PKT                  200     //200

//malay
#define  B2B_COMMAND_LEN_OF_PKT          30     

//Malay escrow macro

 
#define  WAIT_BEFORE_TRANS_END               2     //In Second default 5 Seconds (5 Min) 


#define  WAIT_TIME_ISSUE_NEXT_COMMAND        300      //In millisecond (200 ms or greter than 200ms)

#define  WAIT_TIME_FOR_GET_DISABLE       5     //In Second default 5 Seconds (5 Min) 

#define  WAIT_TIME_FOR_MAKE_DISABLE      10    //In Second

#define  WAIT_TIME_FOR_RESET             60     //In Second  1min=60Seconds
#define  WAIT_TIME_FOR_B2BREPLY          5      //In Second Fixed according to manual

#define  WAIT_TIME_FOR_UNLOADING         5       //In Second  
#define  WAIT_TIME_FOR_UNLOADED          600     //In Second  10 Min=600Second

#define  WAIT_TIME_FOR_DISPENSING        5       //In Second     
#define  WAIT_TIME_FOR_DISPENSED         600     //In Second  10 Min=600Second

#define  WAIT_FORRECV_NOTE               1      //3,5,4=tested In Second  

           
#define  NOTE_CAME_LAST_MOMENT                 2 
#define  MAX_NO_RPLY_LIM_COUNT                 1 //DEFAULT 4   (2 Malay tested)  

//.................. COMMAND VALUES .......................

#define  CASHCODE_SYNC                   0x02
#define  CASHCODE_CMD_ACK                0x00   
#define  CASHCODE_CMD_RESET              0x30
#define  CASHCODE_CMD_GET_STATUS         0x31
#define  CASHCODE_CMD_SET_SECURITY       0x32
#define  CASHCODE_CMD_POLL               0x33   
#define  CASHCODE_CMD_ENABLE_BILL_TYPE   0x34
#define  CASHCODE_CMD_STACK		 0x35
#define  CASHCODE_CMD_RETURN             0x36 
#define  CASHCODE_CMD_IDENTIFICATION     0x37
#define  CASHCODE_CMD_HOLD               0x38   
#define  CASHCODE_CMD_GET_BILL_TABLE     0x41
#define  CASHCODE_CMD_UNLOAD             0x3D
#define  CASHCODE_CMD_DISPENSE           0x3C


#define  CASHCODE_CMD_SET_RECYCLE_CASSETTE           0x40
#define  CASHCODE_CMD_RECYCL_CST_STATUS              0x3B

//.................. RESPONSE VALUES...................

#define  CASHCODE_RES_ACK                0x00 
#define  CASHCODE_RES_POWERUP            0x10 //Against Poll Cmd, RPLY: 02, 03, 06, 10, 43, 92 
#define  CASHCODE_RES_INITIALIZE     	 0x13 //Against Poll Cmd, RPLY: 02, 03, 06, 13, D8, A0
#define  CASHCODE_RES_IDLING         	 0x14
#define  CASHCODE_RES_ACCEPTING      	 0x15      
#define  CASHCODE_RES_STACKING        	 0x17
#define  CASHCODE_RES_RETURNING       	 0x18 //ret frm escrow or for jam
#define  CASHCODE_RES_DISABLED        	 0x19 //Against Poll Cmd, RPLY: 02, 03, 06, 19, 82, 0F
#define  CASHCODE_RES_HOLDING        	 0x1A     
#define  CASHCODE_RES_BUSY               0x1B
#define  CASHCODE_RES_REJECTING       	 0x1C
#define  CASHCODE_RES_CSTFULL            0x41
#define  CASHCODE_RES_CSTREMOVED         0x42
#define  CASHCODE_RES_ACPTR_JAM          0x43 
#define  CASHCODE_RES_STKR_JAM           0x44  
#define  CASHCODE_RES_CHEATED            0x45
#define  CASHCODE_RES_ESCROW             0x80
#define  CASHCODE_RES_INVALID_CMD     	 0x30
#define  CASHCODE_RES_STACKED            0x81
#define  CASHCODE_RES_RETURNED           0x82
#define  CASHCODE_RES_INVALID_BILL_NMBR	 0x28
#define  CASHCODE_RES_GENERIC_FAIL	 0x47

#define  CASHCODE_RES_UNLOADING       	 0x1E
#define  CASHCODE_RES_UNLOADED           0x26
#define  CASHCODE_RES_DISPENSING      	 0x1D
#define  CASHCODE_RES_DISPESED      	 0x25
#define  CASHCODE_RES_RCYCL_CST_STAT     0x98

//Malay


#define  CASHCODE_ESCROW_CASSETTE_TYPE                  0b00011000  //0X18(24d)
#define  CASHCODE_NOT_ASSIGNED_CASSETTE_TYPE            0b00011111  //0X1F


#define  WAIT_TIME_SET_RECYCLE                            10  //(Seconds)


//#define  CASHCODE_NOTEACPTR_STATUS_NILL  0xFD  //MY CODE
//#define  CASHCODE_NOTEACPTR_STCKED_ERR   0xFE  //MY ERR. CODE
//#define  API_FORCE_STOP_B2B              0xFF  //API_FORCE_STOP_B2B is NOT acptr's return value, it's defined by me.
//.........................................................  

//.................. CAUSE OF REJECT ......................  
#define  CASHCODE_RES_RJCTNG_INSRT_ERR             0x60 
#define  CASHCODE_RES_RJCTNG_MAGNTC_ERR            0x61  
#define  CASHCODE_RES_RJCTNG_BILL_IN_HEAD          0x62
#define  CASHCODE_RES_RJCTNG_MULTIPLY_ERR          0x63
#define  CASHCODE_RES_RJCTNG_CONVEY_ERR            0x64 
#define  CASHCODE_RES_RJCTNG_IDNTFY_ERR            0x65  
#define  CASHCODE_RES_RJCTNG_VRFY_ERR              0x66
#define  CASHCODE_RES_RJCTNG_OPTIC_ERR             0x67
#define  CASHCODE_RES_RJCTNG_INHIBIT_DENOM         0x68
#define  CASHCODE_RES_RJCTNG_CAPACITY_ERR          0x69
#define  CASHCODE_RES_RJCTNG_OPERATION_ERR         0x6A
#define  CASHCODE_RES_RJCTNG_LNGTH_ERR             0x6C
#define  CASHCODE_RES_RJCTNG_UV_PROPTY_MISMATCH    0x6D
#define  CASHCODE_RES_RJCTNG_TREATED_AS_BARCODE    0x92
#define  CASHCODE_RES_RJCTNG_WRONG_BARCODE_CHAR    0x93
#define  CASHCODE_RES_RJCTNG_WRONG_BARCODE_START   0x94
#define  CASHCODE_RES_RJCTNG_WRONG_BARCODE_STOP    0x95
//.........................................................  

#define RQST_FRM_UP_LEVEL_NILL           0x00
#define RQST_FRM_UP_LEVEL_INIT           0x01
#define RQST_FRM_UP_LEVEL_DO_ESCROW      0x02   // ready to Keep in escrow
#define RQST_FRM_UP_LEVEL_ESCROW_ACPT    0x03   // accept from escrow
#define RQST_FRM_UP_LEVEL_ESCROW_RET     0x04   // return from escrow 
//.........................................................

/////////////////////////////////////////////////////////////////////////////////////

#define YES                 1
#define NO                  0
#define COMPLETED           2   
/////////////////////////////////////////////////////////////////////////////////////

typedef struct noteType
{
                   int rs_5;

                   int rs_10;

                   int rs_20;

                   int rs_50;

                   int rs_100;

                   int rs_500;

                   int rs_1000;

                   int escrow_on_off;
}noteType;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Generic Note Acceptor API

int GetNoteDetailsInRecycleCst(int *qty);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int   WaitforNoteAcptrLowlevelflagClosed();

unsigned int   NoteAcptrStartCreditPollThread(void);

void*          ThreadProc1_CASHCODE_NoteAcptrPoll(void*);

void           CASHCODE_NoteAcptr_sendPollCmd();

int            CASHCODE_NoteAcptr_stopPollForNoteAcptnce();

void           CASHCODE_NoteAcptr_startPollForNoteAcptnce();

int            CASHCODE_NoteAcptr_recvPollReply(int*);

void           CASHCODE_NoteAcptr_reste();

void           CASHCODE_NoteAcptr_sendACK();  

int            CASHCODE_NoteAcptr_enableNoteTypesRead(noteType*);

int            CASHCODE_NoteAcptr_Cmd_InhibitAndRply(noteType);

void           CASHCODE_NoteAcptr_enableBillTypes(noteType);

int            CASHCODE_NoteAcptr_disableAllBillTypes();

void           CASHCODE_NoteAcptr_disableBillTypes();

int            CASHCODE_NoteAcptr_ChkLastMomentNoteAndDisableAllBillTypes(int*,int*);

void           NoteAcptrPollFlagControl(int);

void           CASHCODE_NoteAcptr_hold();

int            CASHCODE_NoteAcptr_stack();

int            CASHCODE_NoteAcptr_return();

void           CASHCODE_NoteAcptr_status();

void           CASHCODE_NoteAcptr_identification();

void           CASHCODE_NoteAcptr_setSecurity(const unsigned int ,const unsigned int ,const unsigned int);

void           CASHCODE_NoteAcptr_getBillTable();

static         unsigned int CASHCODE_NoteAcptr_SetRecycleCassette(const unsigned int,const unsigned int);

static void             CASHCODE_NoteAcptr_ExtendedRecycleCstStatus();

static void             CASHCODE_NoteAcptr_sendNAK();


int                     CASHCODE_NoteAcptr_unloadBills(char frmRcyclCstNmbr, char qty);

int                     CASHCODE_NoteAcptr_dispenseBills(char billType, char qty);

int                     ensure_NoteAcptrPktTrnsmsnSqncCmplted();

int                     CASHCODE_NoteAcptr_isThisNoteAllowed(int);	 

void                    CASHCODE_NoteAcptr_getRecycleCstStatus();


static   int            CASHCODE_NoteAcptr_statusRead(unsigned char* const,int* const);

static   void           displayReceivedPacket(const unsigned char* const,const int);

static   void           CopyReceivedPacketGlobally(const unsigned char* const,const int);

static   unsigned int   GetCRC16(const unsigned char*, const unsigned int,char* const,char* const);

static   int            CASHCODE_NoteAcptr_analysisRcvdPkt(const unsigned char*,int* const);

static   int            CASHCODE_NoteAcptr_getCauseOfReject(const unsigned char* const,int* const);

static   void           CASHCODE_NoteAcptr_noteAmountReceived(const unsigned char* const,int* const) ;

static   void           CASHCODE_NoteAcptr_noteQtyReceived(const unsigned char* const,int* const);

static   void           decimalToBinary(int, int* const);
void displaySentPacket(); 

void display(char*);

static void delay_mSec(const int);

int  enableAllBillsAndMakeIdle();

int  enableSpecificBillsAndMakeIdle(int,int);

int  getNoteAcptrPollFlagStatus();

int  IsNoteInsertedAtLastMomentOfTransaction(int*);

int  IssuedisableAllBillCmd();

void intelligent_delay_mSec(int);

int  CloseNoteAcptrImmediate(int*,int*);

int  ensureStackedSeassionCompleted();

//-------------Start: Kolkata Metro ---------------

// DATE: 03-Mar-2012
int   OpenCashAcptr(int);
int   CloseNoteAcptr(int*);
int   GetNoteAcptrStatus();
#ifdef B2B_ACPTR
int   AcceptFromEscrow(char cstNmbr, char qty);
int   RejectFromEscrow(char billType,char qty);
#else
int  AcceptFromEscrow();
int  RejectFromEscrow();
#endif

int GetNoteDetailInEscrow(int*,int*);

int GetAcceptedNoteDetail(int*,int*);

int GetReturnDetail(int*,int*);

//------------ End: Kolkata Metro -----------------

//Add By Malay 25 Jan 2013

inline unsigned int   InhibitNoteAcptr();
inline void           SetSpecificNoteInhibitStatus(int);
inline int            DisableSpecificNotes(int);
static inline void    IntelligentNoteAcceptorInhibit(void);
inline void           CloseNoteAcceptorPollThread(void);
unsigned int          DeactivateB2BUnit(void);
static int            ClearB2BSerialPortbuffer(void);
static int            WaitforState(const unsigned int,const unsigned int);
static int            IssueB2BResetCommand();
static unsigned int   GetCurrentB2BState( unsigned char* const,unsigned int,unsigned int* const);
static int            MakeDisableState();
inline int            SetCurrentfare(int);
static unsigned int   SetRecycleCassetteCommandStatus(const unsigned int ,const unsigned int);
static unsigned int   SetRecycleCassette();
int                   B2BFinishRejectingState();
int                   B2BGetExtendedRecycleStatus( int* const,const unsigned);
inline int            SetAlreadyCoinInhibit();
static inline void    ClearGlobalPollVars();
static int            B2BGetExtendedRecycleStatusRead(unsigned char* const,int* const);
inline void           SetCoinFullDisableFlag(int,int);
int                   InitCashAcptr();
unsigned int          WaitforNoteAcptrLowlevelflagClosed(double);

#endif

#endif

