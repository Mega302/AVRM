#ifndef _BNA_H_
#define _BNA_H_


///////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "atvmdevice.h"
#include "atvmlog.h"
#include "common.h"
#include "BnrCtlL32.h"
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>#include <stdlib.h>
#include <string.h>



///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef BNA_NOTE_ACCEPTOR

//#define BNA_MAIN

//#define BNA_DEBUG

//#define BNA_DEBUG_EVENT_CLEAR



#define MEI_DEVICE_STRING                                  "BNR"
#define BNR_OPEN_OPERATION_TIME_OUT_IN_SECONDS             10
#define BNR_RESET_OPERATION_TIME_OUT_IN_SECONDS            60
#define BNR_CASHTAKEN_TIME_OUT_IN_MS                       (30000)
#define BNR_CASHIN_OPERATION_TIME_OUT_IN_MS                (30000)
#define BNR_DEFAULT_OPERATION_TIME_OUT_IN_MS               (5000)


//BNR Operation Id
#define OPERATION_RESET                                   6117
#define OPERATION_CASH_IN_START                           6121
#define OPERATION_CASH_IN                                 6122
#define OPERATION_CASH_IN_ROLLBACK                        6124
#define OPERATION_CASH_IN_END                             6123



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//1 =Enable,0=Disable [API Level]
typedef struct NOTE
{

   bool INR_5;
   bool INR_10;
   bool INR_20;
   bool INR_50;
   bool INR_100;
   bool INR_500;
   bool INR_1000;

}NOTE;

//1 =Enable,0=Disable [Note Acceptor Level]
typedef struct MeiDenom
{
   
   bool INR5;
   bool INR10;
   bool INR20;
   bool INR50;
   bool INR100;
   bool INR500;
   bool INR1000;
  
} MeiDenom;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                //Start:ATVM API//

int OpenCashAcptr();

int Deactivate_BNA_NoteAcptr();

int GetNoteAcptrStatus();

int UnloadCashAcptr();

int DispenseCashAcptr();

int EnableDenom(NOTE UserdefEnablebyte);

int IssuedisableAllBillCmd();

inline void InitNATransModel();

inline int DisableSpecificNotes(int fare);

inline unsigned int InhibitNoteAcptr();

inline void  CloseNoteAcceptorPollThread();

inline int SetAlreadyCoinInhibit();

void* BNA_Credit_Poll(void *ptr);

int GetNoteDetailInEscrow();

inline int SetCurrentfare(int Remainbalance);

inline unsigned int  BNA_GetAcceptedDenomInfo();

inline unsigned int BNA_GetEscrowStateInfo();

inline void SetCoinFullDisableFlag(int maxNoOfCash , int maxNoOfCoin);

int GetNoteDetailsInRecycleCst(int *qty);

unsigned int  NoteAcptrStartCreditPollThread();

int enableSpecificBillsAndMakeIdle(  int fare,                  //API fare (in)
                                     int maxNoteQtyAllowed      //API Max no of cash allow number (in)
                                  );
int CompleteCashInProcess();

unsigned int WaitforNoteAcptrLowlevelflagClosed(double);
                                      
                                                //End:ATVM API//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                          
int BNA_Activate();

int BNA_GetStatus();

int BNA_EnableDenom(MeiDenom);

int BNA_DeActivate();

int BNA_ListCashUnit();

int BNA_EnableDenom( MeiDenom MeiDenomvar );

int BNA_CopyGlobalDenomTable();

int EnableDenom(NOTE UserdefEnablebyte);

                                         
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void bna_delay_mSec(const int milisec);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//NonAPI List

void ClearGlobalEventState();

static void __stdcall operationCompleteOccured(
 LONG32 identificationId, LONG32 operationId, LONG32 result, LONG32 extendedResult, void *data);

static void __stdcall statusOccured(
  LONG32 status, LONG32 result, LONG32 extendedResult, void *data);

static void __stdcall intermediateOccured(
  LONG32 identificationId, LONG32 operationId, LONG32 reason, void *data);

static unsigned int WaitforMeiOperationComplete( double waittime );


#endif

#endif
