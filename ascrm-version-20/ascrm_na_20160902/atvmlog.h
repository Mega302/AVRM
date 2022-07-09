#ifndef _ATVMLOG_H_
#define _ATVMLOG_H_

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <dlfcn.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_STR_SIZE_OF_LOG  4096

#define LOG_ARRAY_SIZE 1000

//++#define PRINT_CONSOLE
//#define PRINT_NONE
#define PRINT_FILE

//++Both log mode defined error
#if defined(PRINT_CONSOLE) && defined(PRINT_FILE) 
#error Only one Log mode must be activate 
#endif

//++No log mode defined error
#if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE) && !defined(PRINT_NONE) 
#error No Log mode defined
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NO_LOG                                     0   
#define OPERATION_COMPLTE_LOG                      1
#define DATA_EXCHANGE_ERROR_ONLY                   2   //++Errors only
#define DATA_EXCHANGE_STATUS_WITHOUT_DATA          3   //++Error and Success but no monitoring data 
#define DATA_EXCHANGE_ALL                          4   //++All exchange
#define ALL_LOG                                    5  

//++New LogLevels
#define OFF           40
#define TRACE         41
#define DEBUG         42
#define INFO          43
#define WARN          44
#define ERROR         45
#define FATAL         46
#define ALL           47

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern char  g_Deviceid[MAX_STR_SIZE_OF_LOG];
extern int   g_LogModeLevel;
extern void *g_lib_handle;
extern char  g_logdllpath[MAX_STR_SIZE_OF_LOG];
extern int  (*g_AVRM_writeFileLog)(char *str,char *LogFilePathstr,int fnLogLevel);
extern int  (*g_Ascrm_writeFileLog)(char *str,char *LogFilePathstr);
extern int  (*g_Ascrm_WriteLogInConsole)(char *str);
int SetSoFilePath(unsigned char* SoFilePath );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int  SetGeneralFileLogPath(unsigned char*);
int  SetMachineFaultFileLogPath(unsigned char*);
int  SetAPITimeoutFileLogPath(unsigned char*);
//++int  writeFileLog(char *str);
void getDateTime(int *day,int *mon,int *yr,int *hr,int *min,int *sec);

static int  WriteLogInConsole(char *str);
static int  writeMDSLog(char *str, char *filename);

//++New AVRM API Log Functions
int AVRM_Currency_writeFileLog(char *str,int fnLogLevel);
int AVRM_writeFileLogV2(char *str,int fnLogLevel,unsigned char* DeviceName);

void LogDataExchnage( int DataExchangelevel,unsigned char* Deviceid,unsigned char* TransmitType,unsigned char* command,int datalength);

static int AVRM_writeMDSLog(char *str, char *filename,int fnLogLevel,unsigned char* DeviceName);
static bool OpenLogdll(char* dlllogpath);
static bool GetFileLogDllFunctionAddress();
static bool GetConsoleLogDllFunctionAddress();
static bool CloseLogdll();

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

