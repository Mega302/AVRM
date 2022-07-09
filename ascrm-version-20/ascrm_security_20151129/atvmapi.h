#ifndef _ATVMAPI_H_
#define _ATVMAPI_H_

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//System headers
#include <time.h>
#include <stdbool.h>
#include <assert.h>

//User defined headers
#include "atvmdevice.h"
#include "common.h"
#include "jcm.h"
#include "b2b.h"
#include "cctalkdevs.h"
#include "readini.h"
#include "DSCU.h"
#include "atvmalarm.h"
#include "atvmlog.h"
#include "ascrmsecurityjni.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define ASCRMAPIMAIN

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//Malay Add 3 FEB 2013

#if defined(B2B_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE             5 //(Second)
#endif

#if defined(JCM_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE             900 //(Second) default : 15minute =300 second 
#endif


#if defined(BNA_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE             6 //(Second)
#endif



#if defined(NOTE_ACCEPTOR) && defined(B2B_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_ACCEPTANCE_COMPLETE                 900 //(Second) default : 15minute =900 second 
#endif

#if defined(NOTE_ACCEPTOR) && defined(JCM_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_ACCEPTANCE_COMPLETE                 900 //(Second) default : 15minute =900 second 
#endif


#if defined(COIN_ACCEPTOR) 
#define  DELAY_FOR_COIN_DENOM_DISABLE_COMPLETE              60 //(Second) default : 1minute =60 second 
#define  DELAY_FOR_COIN_ACCEPTANCE_COMPLETE                 2  //(Second)
#endif

//Malay Add 14 MARCH 2013
#define  NOTE_ACPTR_THREAD_END_DELAY                   80  //(MilliSecond)

#define  NOTE_ACPTR_STATUS_CHECK_DELAY                 200  //(MilliSecond)

#define DELAY_FOR_TRANS_COMPLETE                       8




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define  WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD               120  //(Second)  [Default: 2 Min ]

#define  WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD               120  //(Second)  [Default: 2 Min ]

#define  WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD                      2100 //(Second)  [Default: 35 Min ]

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Malay Add 19 Jan 2013
#define  THREAD_START               1
#define  THREAD_CLOSE               2
#define  THREAD_KILL                3
#define  THREAD_NO_KILL             4
#define  NOTE                       1
#define  COIN                       0
#define  MAX_NMBR_OF_NOTE           20 
#define  MAX_NMBR_OF_COIN           20


#define GET_DENOM_ACCEPTING                         0

#define GET_DENOM_CORRECT_FARE                      1

#define GET_DENOM_MORE_FARE                         4

#define GET_DENOM_TRNS_TIME_OUT                     5

#define GET_DENOM_MORE_QTY_INSRTD_AND_DSPNSED       6


#define  LEVEL1    1
                             
#define  LEVEL2    2
                             
#define  LEVEL3    3
                            
#define  LEVEL4    4


#define COMMIT_TRANS       1

#define CANCEL_TRANS       2


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//CRIS API

int activateDevice( int NoteAcceprtorComPort,int EscrowClearanceMode );

int  deviceStatus(int TransTime);

int  deactivateDevice(void);

bool acceptFare(int Fare,int maxNoOfCash,int maxNoOfCoin,int TransTime);

int  getDenomination(int arry[][2]);

bool commitTran(void);

bool cancelTran(void);

bool defaultCancel(void);

bool defaultCommit(void);

bool cancelTran_Ver2();

bool commitTran_Ver2();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct NotesCounter
{

       int N5;     
       int N10;
       int N20;
       int N50;
       int N100;
       int N500;
       int N1000;

}NotesCounter;


typedef struct CoinsCounter
{

       int C1;     
       int C5;
       int C10;
  
}CoinsCounter;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void IncrementNotesCounter(int);

void IncrementCoinsCounter(int);

void ClearCoinsCounter(void);

void ClearNotesCounter(void);

void LogThisTransData(void);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int           GetCurrentAcceptedAmount();

int           GetFare();

void*         ThreadProc_acceptFare(void *ptr);

void          StopAcceptingState(void);

void*         ThreadProc_NoteAcptrStart(void *ptr);

void*         ThreadProc_CoinAcptrStart(void *ptr);

static  int   makeAccepters_at_ActiveteMode(int,int,int);

static  int   after_Deactivt_SetStatus_And_DoAction(void);

int           InhibitGreaterDenominationCurrency(int fare,int ttlAcptdAmnt);

int           KillAcceptFareThread(void);

static void   delay_mSec(int mdly);


//Malay Add 24 Jan 2013 for testing purpose atvm api
static void TestAtvm(void);

static void TestAtvmAlarm(void);

//Add By Malay 25 Jan 2013
static int  AtvmDeActivate(void);

//Malay Add 30 Jan 2013
static inline int  DeactivateCoinAcptr(void);

static inline int  DeactivateNoteAcptr(void);

static inline int  DeactivateTrapGate(void);

static void  GetLastMomentCoinAmount(void);

static void  ReturnCoinAndNotes(void);

static void  InhibitAllAcceptor(void);

static int   KillAllRunningThread(double waittime);

static unsigned int StartAllAcceptThreads(unsigned int);

inline int Wait(unsigned int );

inline void CloseCoinAcptrUpperThread(void);

inline void LockCoinAcptrUpperThread(void);

inline void FreeCoinAcptrUpperThread(void);

static inline void DisableAllAcceptor(void);

static inline void DisableAllAcceptorV2(void);

static inline void GetRecycleStatusBeforeStartAcceptfare(void);

static inline void GetRecycleStatusAfterFinishAcceptfare(void);

static inline unsigned int CalculateCurrentBillQuantity(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void InitAcceptFaresModel(void);

static inline bool ActivateAllAcceptors(void);

static inline bool StartManagerThread(void);

static inline bool StartNoteAcceptorAllThreads(void);

static inline bool StartCoinAcceptorAllThreads(void);

static inline void StartAllAcceptProcessThreads(void);

static inline void StopAllAcceptProcessThreads(int Level);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int  atvm_NACreateTransCompleteThread( unsigned int Transtype );
unsigned int  atvm_CACreateTransCompleteThread( unsigned int Transtype );
static void*  NACompleteTransactionThread(void *ptr);
static void*  CACompleteTransactionThread(void *ptr);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StopNAUpperCreditPollThread();

void StopNALowerCreditPollThread();

void StopCAUpperCreditPollThread();

void StopCALowerCreditPollThread();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void TestAscrm();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
