#ifndef _SMARTCARDAPI_H_
#define _SMARTCARDAPI_H_
#include <stdbool.h>
#include <pthread.h>
#include "delaytime.h"
#include "serial.h"
#include "atvmlog.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MTK_DEVICE_ADDRESS 0x00
#define MUTEK_REPLY_LENGTH_MAX 1024
#define UNKNOWN_CARD       10
#define TYPA_A_CPU_CARD    11
#define TYPA_B_CPU_CARD    12
#define MIFARE_S50_CARD    13
#define MIFARE_S70_CARD    14
#define MIFARE_UL_CARD     15
#define SMARTCARDMAIN
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void SmartCardSetupLog ( char *LogdllPathstr,char *LogFileName,char *deviceid,int fnLogfileMode );
int ConnectDevice_c           ( int  PortId, int  ChannelClearanceMode, int  Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int DisConnectDevice_c        ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int DeviceStatus_C            ( int ComponentId,int Timeout,char *ReplyDeviceStatus,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int AcceptCard_c              ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int DisableCardAcceptance_c   ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int EnableCardAcceptance_c    ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int IsCardInChannel_c         ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int IsCardRemoved_c           ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int CollectCard_c             ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int DispenseCard_c            ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int RejectCard_c              ( int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int ReturnCard_c              ( int DispenseMode,int Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int ActivateContactLessCard   ( char *APIReply, int  *ReplyAPDULength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int ActivateSAMCard           ( int   SAMSlotId,int   Timeout,char *SAMAPIReply, char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int DeActivateContactLessCard ( int  *Reply,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode); 
int DeActivateContactCard     ( int  *Reply,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int ContactLessCardXChangeAPDU( char *CommandAPDU,int   CommandAPDULength,char *ReplyAPDU,int  *ReplyAPDULength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int ContactCardXChangeAPDU    ( char *CommandAPDU,int   CommandAPDULength,char *ReplyAPDU,int  *ReplyAPDULength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int WarmResetSAM              ( int   SAMSlotId, char *ReplyByte,int  *ReplyByteLength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int ColdResetSAM              ( int   SAMSlotId,char *ReplyByte,int  *ReplyByteLength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int isSAMEnable               ( char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
int GetSmartCardLastError     ( char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool MUTEK_CommandTransmit(char *Command,int CommandLength);
static bool MUTEK_CommandReceieve(char *RecvPackt,int totalByteToRecv);
static bool MUTEK_CommandReceieve_V2(char *RecvPackt,unsigned int *rcvPktLen);
static bool SendAckToMutek();
static bool IsAckReceieve();
static int CheckCardType();
static int IsAnyCardPresentInReaderPosition();
static int ActivateContactCard(char *APIReply);
static int GetMTKeaderStatus(char *MTKStatus);
static int GetSAMeaderStatus();
static int GetRFIDReaderStatus();
static int AscrmMutekInitWithCardRetain();
static void SetLastError( unsigned int e1, unsigned int e2 );
static void ResetSmartCardLastError();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
