#ifndef _ATVMLOG_H_
#define _ATVMLOG_H_

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <dlfcn.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_STR_SIZE_OF_LOG  4096

#define LOG_ARRAY_SIZE 1000

//#define PRINT_CONSOLE
#define PRINT_FILE
//++#define PRINT_NONE

//Both log mode defined error
#if defined(PRINT_CONSOLE) && defined(PRINT_FILE) 
#error Only one Log mode must be activate 
#endif

//No log mode defined error
#if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE) && !defined(PRINT_NONE) 
#error No Log mode defined
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////


#define NO_LOG                              0
#define OPERATION_COMPLETE_LOG              1
#define DATA_EXCHANGE_ERROR_ONLY            2
#define DATA_EXCHANGE_ERROR_SUCCESS_ONLY    3 //No Monitoring Data
#define DATA_EXCHANGE_ALL                   4 
#define ALL_LOG                             5 

////////////////////////////////////////////////////////////////////////////////////////////////

extern char  g_general_log[MAX_STR_SIZE_OF_LOG];

extern char  g_Deviceid[MAX_STR_SIZE_OF_LOG];

extern int   g_LogModeLevel;

extern void *g_lib_handle;

extern char  g_logdllpath[MAX_STR_SIZE_OF_LOG];

extern int  (*g_Ascrm_writeFileLog)(char *str,char *LogFilePathstr);

extern int  (*g_Ascrm_WriteLogInConsole)(char *str);

int  SetSoFilePath(unsigned char* SoFilePath );

bool OpenLogdll(char* dlllogpath);

bool GetFileLogDllFunctionAddress();

bool GetConsoleLogDllFunctionAddress();

bool CloseLogdll();

//////////////////////////////////////////////////////////////////////////////////////////////////

int  SetGeneralFileLogPath(unsigned char*);

int  SetMachineFaultFileLogPath(unsigned char*);

int  SetAPITimeoutFileLogPath(unsigned char*);

int  writeFaultLog(char *str);

int  writeFileLog(char *str);

int  DefaultwriteFileLog(char *file,char *str);

static int WriteLogInConsole(char *str);

static int  writeMDSLog(char *str, char *filename);

int writeAPITimeoutLog(char *str);

void getDateTime(int *day,int *mon,int *yr,int *hr,int *min,int *sec);

bool OpenLogdll(char* dlllogpath);

bool GetFileLogDllFunctionAddress();

bool GetConsoleLogDllFunctionAddress();

bool CloseLogdll();

void LogDataExchnage( unsigned char* Deviceid,
                      unsigned char* TransmitType,
                      unsigned char* command,
                      int            datalength
                    );

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////

