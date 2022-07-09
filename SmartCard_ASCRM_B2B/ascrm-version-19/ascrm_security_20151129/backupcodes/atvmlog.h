
#ifndef _ATVMLOG_H_
#define _ATVMLOG_H_



#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>


#define MAX_STR_SIZE_OF_LOG  4096


//#define PRINT_CONSOLE
#define PRINT_FILE
//#define PRINT_NONE

//Both log mode defined error
#if defined(PRINT_CONSOLE) && defined(PRINT_FILE) 
#error Only one Log mode must be activate 
#endif

//No log mode defined error
#if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE) && !defined(PRINT_NONE) 
#error No Log mode defined
#endif

extern bool g_LogModeType;

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


#endif
