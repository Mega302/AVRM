#ifndef _CCTALKDEVS_H_
#define _CCTALKDEVS_H_


///////////////////////////////////////////////////////////////////////////////////////////////////////////

//System Headers
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//API HEADERS
#include "atvmdevice.h"
#include "common.h"
#include "serialc.h"
#include "atvmalarm.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Coin Acceptor Command Debug
//#define COINACCPTEOR_DEBUG
//#define COINESCROW_DEBUG
//#define ERROR_LOG_ON 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define   NRI_FIRMWARE_12_03 

#define   NRI_FIRMWARE_12_06


#if !defined(NRI_FIRMWARE_12_03) && !defined(NRI_FIRMWARE_12_06)
#error any nri firmware must be defined 
#endif

#if defined(NRI_FIRMWARE_12_03) && defined(NRI_FIRMWARE_12_06)
#error Both nri firmware cannot be defined same time
#endif



/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Malay 3 feb 2013 macro add
//MilliSeconds  20ms default
#define   COIN_ACPTR_POLL_DELAY                  20      // repeate counter=10 each counter delay=20ms total 200ms  

#define   CCTALK_REPLY_WAIT_TIME                 3       //In Second malay add 29 march 2013

#define   MAX_COIN_TYPE                          4    

#define   COINACPTR_MAX_LEN_OF_PKT               40      

#define   COINACPTR_RES_ESCROW                   0x05     

#define   MAX_CMD_LEN                            30

#define   COIN_ESCROW_REPLY_WAIT_TIME            4


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct coinType
{
                   int rs_50;

                   int rs_1;

                   int rs_2;

                   int rs_5;

                   int rs_10;

}coinType;


//Created By Malay Maji Data 13 Dec 2012
typedef struct CoinTable
{
    
    float Credit;
    int   CreditCode;                     
    int   FirstCreditByteEnableMask;
    int   SecondCreditByteEnableMask;

}CoinTable;


//API LEVEL FUNCTIONS

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//malay add 31 jan 2014

int           IsCCTALKPortOpen();

int           ActivateCCTALKPort(int portNmbr);

int           DeActivateCCTALKPort();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int           OpenCoinAcptr(int);
int           CloseCoinAcptr(void);
int           GetCoinAcptrStatus(void);
int           CoinAcceptorSimplePoll(void);
void          GetCoinDetailInEscrow(int[],int*);
int           AcceptFromCoinEscrow(void);
int           RejectFromCoinEscrow(void);
unsigned int  MONEYCONTROL_CoinAcptrCreateCreditPoll(void);
int           enableSpecificCoinsAndMakeIdle(int,int);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Local functions
static void*        ThreadProc1_MONEYCONTROL_CoinAcptrPoll(void*);
static int          MONEYCONTROL_CoinAcptr_statusRead(unsigned char* const,unsigned int* const);
static void         CoinAcptrCopyReceivedPacketGlobally(const unsigned char* const,const unsigned int);
static void         CoinAcptrdisplayReceivedPacket(const unsigned char* const,const int );
static void         MONEYCONTROL_CoinAcptr_coinAmountReceived(const unsigned char* const,int* const);
static void         delay_mSec(const int milisec);
static int          totalEventOccured(int*,int,int);
static void         MONEYCONTROL_CoinAcptr_Cmd_inhibit(void);
static void         MONEYCONTROL_CoinAcptr_Cmd_inhibit50(void);
static void         MONEYCONTROL_CoinAcptr_Cmd_CreditPoll();
static void         MONEYCONTROL_CoinAcptr_Cmd_Poll();
static void         MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow(void);
static void         MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow(void);
static void         MONEYCONTROL_CoinAcptr_Cmd_PollFromEscrow(void);
static coinType     MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypesRead();
static void         MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(coinType COIN);
static void         MONEYCONTROL_CoinAcptr_Cmd_inhibitStat(void);
static char         GetCheckSum(const unsigned char* const, const unsigned int);
static void         MONEYCONTROL_CoinAcptr_Cmd_InhibitAndRply(coinType COIN);
int                 DisableAllCoins(void);
static inline void  CoinIntelligentInhibit(void);
inline void         SetAllCoinInhibitStatus(void);
inline int          DisableSpecificCoins(int);
inline void         SetSpecificCoinInhibitStatus(int);
inline unsigned int GetCoinAcptrLowLevelFlagStatus(void);
static inline void  CoinIntelligentDelay(unsigned int);
inline void         CloseCoinCreditPollingThread(void);
inline void         ClearCCTALKBuffer(void);
inline int          EnableSpecificCoins(int);
inline void         NaCoinEnableSet(int);
unsigned int        WaitforCoinAcptrLowlevelflagClosed(double waittime);
static void         cctalk_delay_mSec(const int milisec) ;
static int          cctalk_Wait(unsigned int Second);
bool                GetCADisableAcceptanceSignal();

int getSecuredState(  int* const doorOpenStatus, 
                      int* const alarmOnStatus,
                      int* const cashboxOpenStatus,
                      int* const PrinterCoverstatus
                   );
#endif 



