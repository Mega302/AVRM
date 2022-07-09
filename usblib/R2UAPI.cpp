// R2UAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CardReader.h"

#define CHECKLENGTH(lResult, expr) \
	{ if (lResult == 0) \
		{	if ( !(expr)) \
			return SCARD_F_INTERNAL_ERROR; } \
	else  \
		return lResult; } 


static 
LONG 
TRANSMIT(
	__in	READERHANDLE hReader,
	__in	PBYTE	pbSendBuff,
	__in	DWORD	cbSendLength,
	__out	PBYTE	pbRecvBuff,
	__inout	PDWORD	pcbRecvLength,
	__out_opt PUSHORT	pwStatusWord
	)
{
	CCardReader *pReader = CCardReader::FromHandle(hReader);
	if ( pReader == NULL )
		return SCARD_E_INVALID_HANDLE;
	return pReader->Transmit(pbSendBuff, cbSendLength, pbRecvBuff, pcbRecvLength, pwStatusWord );
}

static
LONG 
CONTROL(
	__in	READERHANDLE hReader,
	__in	PBYTE	pbSendBuff,
	__in	DWORD	cbSendLength,
	__out	PBYTE	pbRecvBuff,
	__inout	PDWORD	pcbRecvLength,
	__out_opt PUSHORT	pwStatusWord
	)
{
	CCardReader *pReader = CCardReader::FromHandle(hReader);
	if ( pReader == NULL )
		return SCARD_E_INVALID_HANDLE;
	return pReader->Control(pbSendBuff, cbSendLength, pbRecvBuff, pcbRecvLength, pwStatusWord );
}


LONG 
WINAPI
R2_Connect(
	IN	PCWSTR	pszReaderName, 
	IN	DWORD	dwShareMode, 
	OUT	PREADERHANDLE phReader
	)
{
	if ( phReader == NULL )
		return SCARD_E_INVALID_PARAMETER;

	CCardReader *pReader = new CCardReader();
	if ( pReader == NULL )
		return SCARD_E_NO_MEMORY;

	LONG lResult = pReader->Connect( pszReaderName, dwShareMode );
	if ( lResult != 0)
	{
		delete pReader;
	}
	else
	{
		*phReader = (READERHANDLE) pReader;
	}

	return lResult;
}


LONG 
WINAPI
R2_Disconnect(
	IN	READERHANDLE hReader
	)
{
	CCardReader *pReader = CCardReader::FromHandle( hReader );
	if ( pReader == NULL )
		return SCARD_E_INVALID_HANDLE;

	return pReader->Disconnect();
}


SCARDHANDLE
WINAPI
R2_GetCardHandle(
	IN	READERHANDLE hReader
	)
{
	CCardReader *pReader = CCardReader::FromHandle( hReader );
	if ( pReader == NULL )
		return NULL;

	return pReader->m_hCard;
}

LONG
WINAPI
R2_GetContext(
    IN READERHANDLE hReader,
    OUT SCARDCONTEXT *phContext
    )
{
	CCardReader *pReader = CCardReader::FromHandle( hReader );
	if ( pReader == NULL )
		return SCARD_E_INVALID_HANDLE;
    if (phContext == NULL)
        return SCARD_E_INVALID_HANDLE;

    *phContext = pReader->m_hContext;
    return 0;
}


LONG 
WINAPI
R2_ListReaders(
	IN	PWSTR	mszReaders,
	IN OUT	PDWORD	pcchReaders
	)
{
	return CCardReader::ListReaders(mszReaders, pcchReaders);
}


LONG 
WINAPI 
R2_IccTransmit(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	)
{
	return TRANSMIT(hReader, pbSendBuff, cbSendLength, pbRecvBuff, pcbRecvLength, NULL);
}


LONG
WINAPI
R2_GetVersionString(
	IN	READERHANDLE hReader,
	IN	PSTR	pszBuff,
	IN OUT	PDWORD	pcbLength,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pszBuff == NULL
		|| pcbLength == NULL
		|| pwStatusWord == NULL)
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x18, 0 };

	return CONTROL(hReader, SendBuff, sizeof(SendBuff), (PBYTE)pszBuff, pcbLength, pwStatusWord);
}


LONG
WINAPI
R2_SetControlParameter(
	IN	READERHANDLE hReader,
	IN	PR2CP	pDCP,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pDCP == NULL
		|| pwStatusWord == NULL)
		return SCARD_E_INVALID_PARAMETER;

	BYTE Data0, Data1, Data2;

	Data0 = pDCP->ControlModeOfRedLed;

	Data1 = 0;
	if (pDCP->LightLedWhenPICCActivation)
		Data1 |= 0x1;
	if (pDCP->LightLedWhenICCActivation)
		Data1 |= 0x2;
	if (pDCP->LightLedWhenSAMActivation)
		Data1 |= 0x4;
	if (pDCP->BlinkLedWhenPICCTransmission)
		Data1 |= 0x8;
	if (pDCP->BlinkLedWhenICCTransmission)
		Data1 |= 0x10;
	if (pDCP->BlinkLedWhenSAMTransmission)
		Data1 |= 0x20;

	Data1 |= pDCP->ControlModeOfGreenLed;

	Data2 = pDCP->ControlModeOfBuzzer;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x21, 0x3, Data0, Data1, Data2 };
	BYTE RecvBuff[3];
	DWORD RecvLength = sizeof(RecvBuff);

	return CONTROL(hReader, SendBuff, sizeof(SendBuff), RecvBuff, &RecvLength, pwStatusWord);
}


LONG
WINAPI
R2_GetControlParameter(
	IN	READERHANDLE hReader,
	OUT	PR2CP	pDCP,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pDCP == NULL
		|| pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x21, 0};
	BYTE RecvBuff[3];
	DWORD RecvLength = sizeof(RecvBuff);

	LONG lResult = CONTROL(hReader, SendBuff, sizeof(SendBuff), RecvBuff, &RecvLength, pwStatusWord);
	CHECKLENGTH(lResult, RecvLength == sizeof(RecvBuff));

	if (lResult == 0 &&
		*pwStatusWord == 0x9000)
	{
		pDCP->ControlModeOfRedLed = RecvBuff[0] & CONTROL_SHARE;

		pDCP->LightLedWhenPICCActivation = RecvBuff[1] & 0x1;
		pDCP->LightLedWhenICCActivation = RecvBuff[1] & 0x2;
		pDCP->LightLedWhenSAMActivation = RecvBuff[1] & 0x4;
		pDCP->BlinkLedWhenPICCTransmission = RecvBuff[1] & 0x8;
		pDCP->BlinkLedWhenICCTransmission = RecvBuff[1] & 0x10;
		pDCP->BlinkLedWhenSAMTransmission = RecvBuff[1] & 0x20;
		pDCP->ControlModeOfGreenLed = RecvBuff[1]  & CONTROL_SHARE;

		pDCP->ControlModeOfBuzzer = RecvBuff[2]  & CONTROL_SHARE;
	}

	return lResult;
}


LONG
WINAPI
R2_SetPCDParameter(
	IN	READERHANDLE hReader,
	IN	PPCDCP	pPCDCP,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pPCDCP == NULL
		|| pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE Data0, Data1, Data2;

	Data0 = (pPCDCP->SupportTypeA) ? 1 : 0;
	Data0 |= (pPCDCP->SupportTypeB) ? 2 : 0;

	switch (pPCDCP->Rate)
	{
	case PCDS_106: Data1 = 0; break;
	case PCDS_212: Data1 = 1; break;
	case PCDS_424: Data1 = 2; break;
	case PCDS_848: Data1 = 3; break;
	}

	if (pPCDCP->EnableRF)
		Data0 |= (1 << 7);

	Data2 = pPCDCP->SelectInterval;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x23, 0x3, Data0, Data1, Data2 };

	BYTE RecvBuff[3];
	DWORD RecvLength = sizeof(RecvBuff);

	return CONTROL(hReader, SendBuff, sizeof(SendBuff), RecvBuff, &RecvLength, pwStatusWord);
}


LONG
WINAPI
R2_GetPCDParameter(
	IN	READERHANDLE hReader,
	OUT	PPCDCP	pPCDCP,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pPCDCP == NULL 
		|| pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x23, 0x0};
	BYTE RecvBuff[3];
	DWORD RecvLength = sizeof(RecvBuff);

	LONG lResult = CONTROL(hReader, SendBuff, sizeof(SendBuff), RecvBuff, &RecvLength, pwStatusWord);
	CHECKLENGTH(lResult, RecvLength == sizeof(RecvBuff));

	if (lResult == 0 &&  *pwStatusWord == 0x9000)
	{
		pPCDCP->SupportTypeA = RecvBuff[0] & 0x1;
		pPCDCP->SupportTypeB = RecvBuff[0] & 0x2;

		pPCDCP->EnableRF = RecvBuff[0] >> 7;

		switch (RecvBuff[1])
		{
		case 0: pPCDCP->Rate = PCDS_106; break;
		case 1: pPCDCP->Rate = PCDS_212; break;
		case 2: pPCDCP->Rate = PCDS_424; break;
		case 3: pPCDCP->Rate = PCDS_848; break;
		}

		pPCDCP->SelectInterval = RecvBuff[2];
	}

	return lResult;
}


LONG
WINAPI
R2_SetPPSRate(
	IN	READERHANDLE hReader,
	IN	DWORD	dwRate,
	OUT	PUSHORT	pwStatusWord
	)
{
	BYTE bRateIdx = 0;
	switch (dwRate)
	{
	case PCDS_106: bRateIdx = 0; break;
	case PCDS_212: bRateIdx = 1; break;
	case PCDS_424: bRateIdx = 2; break;
	case PCDS_848: bRateIdx = 3; break;
	}

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x24, 0x1, bRateIdx };

	return CONTROL(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG 
WINAPI 
R2_GetPPSRate(
	IN	READERHANDLE hReader,
	OUT	PDWORD	pdwRate,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pdwRate == NULL
		|| pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x24, 0x0};

	BYTE RecvBuff[2];
	DWORD RecvLength = sizeof(RecvBuff);
	LONG lResult = CONTROL(hReader, SendBuff, sizeof(SendBuff), RecvBuff, &RecvLength, pwStatusWord);
	CHECKLENGTH(lResult, RecvLength == sizeof(RecvBuff));
	if (lResult == 0 && *pwStatusWord == 0x9000)
	{
		switch (RecvBuff[0])
		{
		case 0: *pdwRate = PCDS_106; break;
		case 1: *pdwRate = PCDS_212; break;
		case 2: *pdwRate = PCDS_424; break;
		case 3: *pdwRate = PCDS_848; break;
		default: return SCARD_F_INTERNAL_ERROR;
		}
	}
	return lResult;
}


LONG
WINAPI
R2_EnableBuzzer (
	IN	READERHANDLE hReader,
	IN	BOOL	bEnable,
	OUT	PUSHORT	pwStatusWord
	)
{
	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x28, 0x1, bEnable};
	return CONTROL(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG 
WINAPI 
R2_GetBuzzerStatus(
	IN	READERHANDLE hReader,
	OUT	PBOOL	pbEnable,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbEnable == NULL ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x28, 0x0};
	BYTE RecvBuff[1];
	DWORD RecvLength = sizeof(RecvBuff);

	LONG lResult = CONTROL(hReader, SendBuff, sizeof(SendBuff), RecvBuff, &RecvLength, pwStatusWord);
	CHECKLENGTH(lResult, RecvLength == sizeof(RecvBuff));

	if (lResult == 0 && 
		*pwStatusWord == 0x9000)
	{
		if ( ! (RecvBuff[0] == 1 || RecvBuff[0] == 0))
			return SCARD_F_INTERNAL_ERROR;

		*pbEnable = (BOOL) RecvBuff[0];
	}

	return lResult;
}


LONG 
WINAPI 
R2_SetLedStatus(
	IN	READERHANDLE hReader,
	IN	BYTE	bStatusFlags,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x29, 0x1, bStatusFlags };
	return CONTROL(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG
WINAPI 
R2_GetLedStatus(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbStatusFlags,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbStatusFlags == NULL ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;


	BYTE SendBuff[] = { 0xE0, 0x0, 0x0, 0x29, 0 };
	BYTE RecvBuff[1];
	DWORD RecvLength = sizeof(RecvBuff);

	LONG lResult = CONTROL(hReader, SendBuff, sizeof(SendBuff), RecvBuff, &RecvLength, pwStatusWord);
	CHECKLENGTH(lResult, RecvLength == sizeof(RecvBuff));

	if (lResult == 0 && 
		*pwStatusWord == 0x9000)
	{
		*pbStatusFlags = RecvBuff[0];
	}

	return lResult;
}


LONG
WINAPI
R2_GetCardUID(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbUIDBuff,
	IN OUT	PDWORD	pcbUIDLength,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbUIDBuff == NULL ||
		pcbUIDLength == NULL ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xCA, 0, 0, 0 };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), pbUIDBuff, pcbUIDLength, pwStatusWord);
}


LONG
WINAPI
R2_GetATS(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbATSBuff,
	IN OUT	PDWORD	pcbATSLength,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbATSBuff == NULL ||
		pcbATSLength == NULL ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xCA, 1, 0, 0 };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), pbATSBuff, pcbATSLength, pwStatusWord);
}


LONG
WINAPI
R2_GetExtData(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbExtBuff,
	IN OUT	PDWORD	pcbExtLength,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbExtBuff == NULL ||
		pcbExtLength == NULL ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xCA, 2, 0, 0 };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), pbExtBuff, pcbExtLength, pwStatusWord);
}


LONG
WINAPI 
R2_NfcTransmit(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	BYTE	bSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbSendBuff == NULL ||
		pbRecvBuff == NULL ||
		pcbRecvLength == NULL ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE *pbNewSendBuff;
	DWORD dwNewSendLength = bSendLength + 5;
	
	pbNewSendBuff = (BYTE*) malloc(dwNewSendLength);
	if ( pbNewSendBuff == NULL )
		return SCARD_E_NO_MEMORY;

	pbNewSendBuff[0] = 0xE0;
	pbNewSendBuff[1] = 0x00;
	pbNewSendBuff[2] = 0x00;
	pbNewSendBuff[3] = 0x52;
	pbNewSendBuff[4] = bSendLength;

	memcpy(&pbNewSendBuff[ 5], pbSendBuff, bSendLength);

	LONG lResult = CONTROL(hReader, pbNewSendBuff, dwNewSendLength, pbRecvBuff, pcbRecvLength, pwStatusWord);

	free(pbNewSendBuff);

	return lResult;
}


// Mifare 1K/4K Card Operations
//

LONG
WINAPI 
R2_MfLoadKey(
	IN	READERHANDLE hReader,
	IN	BYTE	bKeyNumber, 
	IN	BYTE	(&bKeyBytes)[6],
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0x82, 0x0, bKeyNumber, 0x6, 
	bKeyBytes[0], bKeyBytes[1], bKeyBytes[2], bKeyBytes[3], bKeyBytes[4], bKeyBytes[5] };

	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG
WINAPI 
R2_MfAuth(
	IN	READERHANDLE hReader,
	IN	BOOL	bKeyA,
	IN	BYTE	bKeyNumber,
	IN	BYTE	bBlockNumber,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( bKeyNumber > 0x20 ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0x86, 0x0, 0x0, 0x5, 
		0x1, 0x0, bBlockNumber,  bKeyA ? 0x60 : 0x61, bKeyNumber};

	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG 
WINAPI 
R2_MfReadBlocks(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	BYTE	bBytesToRead,
	IN	PBYTE	pbBuffer,
	IN	PDWORD	pcbLength,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbBuffer == NULL ||
		pcbLength == NULL ||
		pwStatusWord == NULL)
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xB0, 0x0, bBlockNumber, bBytesToRead };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), pbBuffer, pcbLength, pwStatusWord);
}


LONG
WINAPI
R2_MfWriteBlocks(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pbBuffer == NULL ||
		pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE *pbSendBuff;
	DWORD dwSendLength = 0;
	
	pbSendBuff = (BYTE*) malloc(bBytesToWrite + 5);
	pbSendBuff[dwSendLength++] = 0xFF;
	pbSendBuff[dwSendLength++] = 0xD6;
	pbSendBuff[dwSendLength++] = 0x0;
	pbSendBuff[dwSendLength++] = bBlockNumber;
	pbSendBuff[dwSendLength++] = bBytesToWrite;

	memcpy(&pbSendBuff[dwSendLength], pbBuffer, bBytesToWrite);
	dwSendLength += bBytesToWrite;

	LONG lResult = TRANSMIT(hReader, pbSendBuff, dwSendLength, NULL, NULL, pwStatusWord);

	free(pbSendBuff);

	return lResult;
}


LONG 
WINAPI
R2_MfInitialize(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xD7, 0x0, bBlockNumber, 0x5, 0x0,
	iValue >> 24, iValue >> 16, iValue >> 8, iValue & 0xFF };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG 
WINAPI 
R2_MfIncrement(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xD7, 0x0, bBlockNumber, 0x5, 0x1,
	iValue >> 24, iValue >> 16, iValue >> 8, iValue & 0xFF };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG 
WINAPI 
R2_MfDecrement(
	IN	READERHANDLE hReader,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xD7, 0x0, bBlockNumber, 0x5, 0x2,
				iValue >> 24, iValue >> 16, iValue >> 8, iValue & 0xFF };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG 
WINAPI 
R2_MfRestore(
	IN	READERHANDLE hReader,
	IN	BYTE	bSourceBlockNumber,
	IN	BYTE	bDestBlockNumber,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xFF, 0xD7, 0x0, bSourceBlockNumber, 0x2, 0x3, bDestBlockNumber };
	return TRANSMIT(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}


LONG 
WINAPI
R2_EnterIAP(
	IN	READERHANDLE hReader,
	OUT	PUSHORT	pwStatusWord
	)
{
	if ( pwStatusWord == NULL )
		return SCARD_E_INVALID_PARAMETER;

	BYTE SendBuff[] = { 0xE0, 0x0, 0x80, 0xff, 0x03, 0x4b, 0x30, 0x00 };
	return CONTROL(hReader, SendBuff, sizeof(SendBuff), NULL, NULL, pwStatusWord);
}



LONG
WINAPI
R2_EnableBallonTips(
	IN	BOOL	bEnable
	)
{
	HKEY hKey = NULL;

	LONG lResult = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\", 0, KEY_ALL_ACCESS, &hKey);
	if ( lResult != ERROR_SUCCESS )
		return lResult;

	DWORD Value = bEnable ? 1: 0;
	lResult = RegSetValueExA(hKey, "EnableBalloonTips", 0, REG_DWORD, (BYTE*) &Value, sizeof(DWORD));
	if ( lResult != ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		return lResult;
	}

	return 0;
}


BOOL
WINAPI
R2_IsBallonTipsEnabled()
{
	//HKEY hKey = NULL;

	//LONG lResult = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\", 0, KEY_ALL_ACCESS, &hKey);
	//if ( lResult != ERROR_SUCCESS )
	//	return FALSE;

	//DWORD Value;
	//DWORD Size = sizeof(Value);
	//lResult = RegGetValueA(hKey, "EnableBalloonTips", NULL, RRF_RT_DWORD, NULL, &Value, &Size);
	//if ( lResult != ERROR_SUCCESS )
	//{
	//	RegCloseKey( hKey );
	//	return FALSE;
	//}

	//return Value >= 1;
	return 0;
}


#define USBROOT_XP	"System\\CurrentControlSet\\Enum\\USB\\"
#define USBROOT_WIN7	"System\\CurrentControlSet\\Enum\\USB\\"
#define TARG_ENTRY_XP	"Device Parameters"
#define TARG_ENTRY_WIN7 "Device Parameters\\WUDFUsbccidDriver"
#define SEARCH_NAME	"Vid_4143&Pid_5407"


LONG	FindSubEntry(PSTR pszParentName, PSTR pszTargetName, PHKEY phKey )
{
	LONG lResult;
	HKEY hParentKey;
	CHAR szFullName[ 256 ];
	CHAR szName[ 256 ];
	DWORD cchName;
	DWORD dwIndex;

	hParentKey = NULL;
	lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, pszParentName, 0, KEY_READ, &hParentKey);
	if (lResult != ERROR_SUCCESS)
		return lResult;

	dwIndex = 0;
	while (TRUE)
	{
		cchName = sizeof(szName) - 1;
		lResult = RegEnumKeyExA(hParentKey, dwIndex++, szName, &cchName, NULL, NULL, NULL,NULL);
		if (lResult != ERROR_SUCCESS)
		{
			if (lResult == ERROR_NO_MORE_ITEMS)
			{
				lResult = 0;
				break;
			}
		}
		

		if (memcmp(pszTargetName, szName,strlen(szName)) == 0)
		{
			sprintf( szFullName, "%s%s\\", pszParentName, pszTargetName );
			lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szFullName, 0, KEY_WRITE, phKey);
			break;
		}
		else
		{   sprintf( szFullName, "%s%s\\", pszParentName, szName );
			lResult = FindSubEntry(szFullName, pszTargetName, phKey);
			if (lResult == ERROR_SUCCESS)
				break;
		}
	}

	RegCloseKey(hParentKey);

	return lResult;
}


BOOL	EnableEscapeCommands(BOOL fEnable, BOOL bOnWin7)
{
	PSTR	pszRoot;
	PSTR	pszTargEntry;
	HKEY	hRootKey;
	HKEY	hSubKey;
	LONG	lResult;
	CHAR	szFullName[ 256 ];
	CHAR	szName[ 128 ];
	DWORD	cchName;
	DWORD	dwIndex;
	DWORD	dwValue;

	hRootKey = hSubKey = NULL;
	if ( bOnWin7 )
	{
		pszRoot = USBROOT_WIN7;
		pszTargEntry = TARG_ENTRY_WIN7;
	}
	else 
	{
		pszRoot = USBROOT_XP;
		pszTargEntry = TARG_ENTRY_XP;
	}

	lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, pszRoot, 0, KEY_READ, &hRootKey);
	if (lResult != ERROR_SUCCESS)
		goto L_EXIT;

	dwIndex = 0;
	while ( TRUE )
	{
		cchName = sizeof( szName ) - 1;
		lResult = RegEnumKeyExA( hRootKey, dwIndex++, szName, &cchName, NULL, NULL, NULL,NULL );
		if (lResult != ERROR_SUCCESS )
		{
			if (lResult == ERROR_NO_MORE_ITEMS)
			{
				lResult = 0;
				break;
			}
			else
				goto L_EXIT;
		}

		if (  strnicmp( szName, SEARCH_NAME, strlen(SEARCH_NAME) ) == 0 )
		{
			
			sprintf( szFullName, "%s%s\\", pszRoot, szName );
			hSubKey = NULL;
			lResult = FindSubEntry( szFullName, pszTargEntry, &hSubKey);
			if ( lResult != ERROR_SUCCESS )
				goto L_EXIT;

			if ( hSubKey == NULL )
				continue;

			dwValue = fEnable ? 1 : 0;
			lResult = RegSetValueExA(hSubKey,  "EscapeCommandEnable", 0, REG_DWORD, (BYTE*) &dwValue, sizeof(DWORD));
			if ( lResult != ERROR_SUCCESS )
			{
				RegCloseKey(hSubKey);
				goto L_EXIT;
			}
			
		}
	}

L_EXIT:
	if ( hRootKey != NULL )
		RegCloseKey( hRootKey );

	return lResult == 0;
}



BOOL
WINAPI
R2_EnableEscapeCommands(
	IN	BOOL	bEnable
	)
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if ( osvi.dwMajorVersion < 6 )
		return EnableEscapeCommands( bEnable, FALSE );
	else
		return EnableEscapeCommands( bEnable, TRUE );
}