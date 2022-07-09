#ifndef _ASCRMLOG_H_
#define _ASCRMLOG_H_

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_STR_SIZE_OF_LOG  4096

#define LOG_ARRAY_SIZE 1000

//#define PRINT_CONSOLE
//#define PRINT_FILE
#define PRINT_NONE

//Both log mode defined error
#if defined(PRINT_CONSOLE) && defined(PRINT_FILE) 
#error Only one Log mode must be activate 
#endif

//No log mode defined error
#if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE) && !defined(PRINT_NONE) 
#error No Log mode defined
#endif

extern bool g_LogModeType;

static int  Ascrm_writeFaultLog(char *str);

static int  Ascrm_DefaultwriteFileLog(char *file,char *str);

static int  Ascrm_writeMDSLog(char *str, char *filename);

static int Ascrm_writeAPITimeoutLog(char *str);

static void Ascrm_getDateTime(int *day,int *mon,int *yr,int *hr,int *min,int *sec);

int  Ascrm_writeFileLog(char *str,char *LogFilePathstr);

int  Ascrm_WriteLogInConsole(char *str);

#endif


