/*++

Module Name:

    R2UAPI.H

Abstract:

    Master include file for applications that call 
    functions provided by R2UAPI.DLL

Revision History:

    2014-02-13 : created

--*/

#ifndef _R2UAPI_H
#define _R2UAPI_H


#include <Windows.h>
#include <WinSCard.h>


// Handle of the C/R
//
typedef LONG READERHANDLE;
typedef READERHANDLE *PREADERHANDLE;


#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

// Operations
//

LONG 
WINAPI
R2_Connect(
	IN	PCWSTR	pszReaderName, 
	IN	DWORD	dwShareMode, 
	OUT	PREADERHANDLE phReader
	);
LONG 
WINAPI
R2_Disconnect(
	IN	READERHANDLE hReader
	);

SCARDHANDLE
WINAPI
R2_GetCardHandle(
	IN	READERHANDLE hReader
	);

LONG
WINAPI
R2_GetContext(
    IN READERHANDLE hReader,
    OUT SCARDCONTEXT *phContext
    );

LONG 
WINAPI
R2_ListReaders(
	IN	PWSTR	mszReaders,
	IN OUT	PDWORD	pcchReaders
	);
LONG
WINAPI
R2_GetVersionString(
	IN	READERHANDLE hReader,
	IN	PSTR	pszBuff,
	IN OUT	PDWORD	pcbLength,
	OUT	PUSHORT	pwStatusWord
	);

// Control Mode
//
#define CONTROL_AUTO		0x40
#define CONTROL_MANUAL		0x80
#define CONTROL_SHARE		0xC0


// Device Control Parameter
//
typedef struct _R2CP
{
	BOOL	LightLedWhenPICCActivation;
	BOOL	LightLedWhenICCActivation;
	BOOL	LightLedWhenSAMActivation;
	BOOL	BlinkLedWhenPICCTransmission;
	BOOL	BlinkLedWhenICCTransmission;
	BOOL	BlinkLedWhenSAMTransmission;
	BYTE	ControlModeOfRedLed;
	BYTE	ControlModeOfGreenLed;
	BYTE	ControlModeOfBuzzer;
} R2CP, *PR2CP;

LONG
WINAPI
R2_SetControlParameter(
	IN	READERHANDLE hReader,
	IN	PR2CP	pDCP,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI
R2_GetControlParameter(
	IN	READERHANDLE hReader,
	OUT	PR2CP	pDCP,
	OUT	PUSHORT	pwStatusWord
	);

// PCD Rate
//
#define PCDS_106		106
#define PCDS_212		212
#define PCDS_424		424
#define PCDS_848		848

typedef struct _PCDCP
{
	BOOL	SupportTypeA;
	BOOL	SupportTypeB;
	BOOL	EnableRF;
	DWORD	Rate;
	BYTE	SelectInterval;
} PCDCP, *PPCDCP;

LONG
WINAPI
R2_SetPCDParameter(
	IN	READERHANDLE hReader,
	IN	PPCDCP	pPCDCP,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI
R2_GetPCDParameter(
	IN	READERHANDLE hReader,
	OUT	PPCDCP	pPCDCP,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI
R2_SetPPSRate(
	IN	READERHANDLE hReader,
	IN	DWORD	dwRate,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI 
R2_GetPPSRate(
	IN	READERHANDLE hReader,
	OUT	PDWORD	pdwRate,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI
R2_EnableBuzzer(
	IN	READERHANDLE hReader,
	IN	BOOL	bEnable,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI 
R2_GetBuzzerStatus(
	IN	READERHANDLE hReader,
	OUT	PBOOL	pbEnable,
	OUT	PUSHORT	pwStatusWord
	);


// LED Status Flags
//
#define LED_STATUS_RED_ON	0x01
#define LED_STATUS_GREEN_ON	0x02
#define LED_STATUS_RED_OFF	0
#define LED_STATUS_GREEN_OFF	0

LONG 
WINAPI 
R2_SetLedStatus(
	IN	READERHANDLE hReader,
	IN	BYTE	bStatusFlags,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI 
R2_GetLedStatus(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbStatusFlags,
	OUT	PUSHORT	pwStatusWord
	);

LONG
WINAPI
R2_GetCardUID(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbUIDBuff,
	IN OUT	PDWORD	pcbUIDLength,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI
R2_GetATS(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbATSBuff,
	IN OUT	PDWORD	pcbATSLength,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI
R2_GetExtData(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbExtBuff,
	IN OUT	PDWORD	pcbExtLength,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI 
R2_IccTransmit(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	);
LONG
WINAPI 
R2_NfcTransmit(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	BYTE	bSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength,
	OUT	PUSHORT	pwStatusWord
	);


// Mifare 1K/4K Card Operations
//

LONG
WINAPI 
R2_MfLoadKey(
	IN	READERHANDLE hReader,
	IN	BYTE	bKeyNumber, 
	IN	BYTE	(&bKeyBytes)[6],
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI 
R2_MfAuth(
	IN	READERHANDLE hReader,
	IN	BOOL	bKeyA,
	IN	BYTE	bKeyNumber,
	IN	BYTE	bBlockNumber,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI 
R2_MfReadBlocks(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	BYTE	bBytesToRead,
	IN	PBYTE	pbBuffer,
	IN	PDWORD	pcbLength,
	OUT	PUSHORT	pwStatusWord
	);
LONG
WINAPI
R2_MfWriteBlocks(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI
R2_MfInitialize(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI 
R2_MfIncrement(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI 
R2_MfDecrement(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI 
R2_MfRestore(
	IN	READERHANDLE hReader,
	IN	BYTE	bSourceBlockNumber,
	IN	BYTE	bDestBlockNumber,
	OUT	PUSHORT	pwStatusWord
	);
LONG 
WINAPI
R2_EnterIAP(
	IN	READERHANDLE hReader,
	OUT	PUSHORT	pwStatusWord
	);


LONG
WINAPI
R2_EnableBallonTips(
	IN	BOOL	bEnable
	);


BOOL
WINAPI
R2_EnableEscapeCommands(
	IN	BOOL	bEnable
	);

#endif