#ifndef _ATVMAPI_H_
#define _ATVMAPI_H_

//System headers
#include <time.h>
#include <stdbool.h>
#include <assert.h>

//For getch()
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

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
#include "ascrmnajni.h"

#define ASCRMAPIMAIN
#define BNA  1
#define BCA  2
#define BCE  3
#define ALL  0

//Malay Add 3 FEB 2013
#if defined(B2B_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE             5 //(Second)
#endif

#if defined(JCM_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE             10 //(Second) 
#endif

#if defined(BNA_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE             6 //(Second)
#endif


#if defined(NOTE_ACCEPTOR) && defined(B2B_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_ACCEPTANCE_COMPLETE                 15 //(Second) 
#endif

#if defined(NOTE_ACCEPTOR) && defined(JCM_NOTE_ACCEPTOR)
#define  DELAY_FOR_NOTE_ACCEPTANCE_COMPLETE                 15 //(Second) 
#endif


#if defined(COIN_ACCEPTOR) 
#define  DELAY_FOR_COIN_DENOM_DISABLE_COMPLETE              10 //(Second) 
#define  DELAY_FOR_COIN_ACCEPTANCE_COMPLETE                 2  //(Second)
#endif

//Malay Add 14 MARCH 2013
#define  NOTE_ACPTR_THREAD_END_DELAY                   80    //(MilliSecond)
#define  NOTE_ACPTR_STATUS_CHECK_DELAY                 200   //(MilliSecond)
#define  DELAY_FOR_TRANS_COMPLETE                       8

#define  WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD               10  //(Second)  [Default: 10 Second ]
#define  WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD               10  //(Second)  [Default: 10 Second ]
#define  WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD                      20  //(Second)  [Default:  20  Second]



//Malay Add 19 Jan 2013
#define  THREAD_START               1
#define  THREAD_CLOSE               2
#define  THREAD_KILL                3
#define  THREAD_NO_KILL             4
#define  NOTE                       1
#define  COIN                       0

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
#define  MAX_LOG_BUFFER        4096

#if !defined(NOTE_ACCEPTOR)

typedef struct noteType{
	
                   int rs_5;
                   int rs_10;
                   int rs_20;

                   int rs_50;

                   int rs_100;
                   
                   int rs_200;

                   int rs_500;

                   int rs_1000;
                   
                   int rs_2000;

                   int escrow_on_off;
}noteType;

int g_StackAcceptedTime;

int g_ReturnAcceptedTime;

int g_NoteAcceptingFlag;

#endif

#define  MAX_NMBR_OF_NOTE           20           
#define  MAX_NMBR_OF_COIN           20

//CRIS API
int  activateDevice( int NoteAcceprtorComPort,int EscrowClearanceMode,int timeout);
int  activateDevice2( int fnComPort,int fnDeviceType,int fnEscrowClearanceMode,int fntimeout );
int  deviceStatus(int TransTime);
int  deactivateDevice(int fnDeviceType);
bool acceptFare(int Fare,int maxNoOfCash,int maxNoOfCoin,int TransTime);


#define MAX_DENOM_NUMBER  40

int  getDenomination(int arry[][MAX_DENOM_NUMBER]);

bool commitTran(void);
bool cancelTran(void);
int defaultCancel(int);
int defaultCommit(int);
//Multi Note Commit and Cancel
int cancelTran_Ver2();
int commitTran_Ver2();

int  GetValidNote(int Denom,int Timeout);
int  AcceptCurrentNote(int Denom,int Timeout);
int  ReturnCurrentNote(int Timeout);
int  cancelTran_Ver3(int timeout);
int  commitTran_Ver3(int timeout);

typedef struct NotesCounter{

       int N5;     
       int N10;
       int N20;
       int N50;
       int N100;
       int N500;
       int N1000;

}NotesCounter;

typedef struct CoinsCounter{

       int C1;     
       int C5;
       int C10;
  
}CoinsCounter;

void IncrementNotesCounter(int);
void IncrementCoinsCounter(int);
void ClearCoinsCounter(void);
void ClearNotesCounter(void);
void LogThisTransData(void);

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

static inline void InitAcceptFaresModel(void);
static inline bool ActivateAllAcceptors(void);
static inline bool StartManagerThread(void);
static inline bool StartNoteAcceptorAllThreads(void);
static inline bool StartCoinAcceptorAllThreads(void);
static inline void StartAllAcceptProcessThreads(void);
static inline void StopAllAcceptProcessThreads(int Level);

unsigned int  atvm_NACreateTransCompleteThread( unsigned int Transtype );
unsigned int  atvm_CACreateTransCompleteThread( unsigned int Transtype );
static void* NACompleteTransactionThread(void *ptr);
static void* CACompleteTransactionThread(void *ptr);

void StopNAUpperCreditPollThread();
void StopNALowerCreditPollThread();
void StopCAUpperCreditPollThread();
void StopCALowerCreditPollThread();

static void TestAscrm(int portno);
static void TestSingleNoteAscrm(int portno);
static void TestAscrmCassetteStatus(int portno);

void SetDenomInfo(int noteValue );
int  GetValidNote(int Denom,int Timeout);
int  AcceptCurrentNote(int Denom,int Timeout);
int  ReturnCurrentNote(int Timeout);
int cancelTran_Ver3();
int commitTran_Ver3();
int cancelTranInterface(int timeout);
int commitTranInterface(int timeout);
void ResetTransData();

#endif
