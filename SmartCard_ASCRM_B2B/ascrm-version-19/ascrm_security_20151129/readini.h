
#ifndef _READINI_H_
#define _READINI_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

//Macros defitions
//#define READINI_DEBUG
//#define READINI_MAIN
#define READ_INI_SUCCESS    1
#define READ_INI_FAIL      -1

//Global variable defitions
static volatile unsigned int g_IniFileSize;
static volatile int g_SectionHeaderlength;

//Function Defitions
//Lower Level
static int          IniOpenFile(char *,char*);
static int          IniGetKeyValue(char*,char*,char*,char*);
static unsigned int IniFileSize(char*);
               
//Middle level
static int   GetStringKeyValue(char*,char*,char*,char*);
static int   GetIntKeyValue(char*,char*,char*);
static int   GetIniAbsoluteFilePath(char*,char*);

//Upper level : API Interface
int GetDSCUThreshold(void);    //On Success return notes number of threshold  otherwise return -1 (fail)
//int GetB2BPortNo(void);      //On Success return portno otherwise return -1 (fail)
int GetNoteAcceptorPortNo();   //On Success return portno otherwise return -1 (fail)
int GetB2BPortNo(void);        //On Success return portno otherwise return -1 (fail)
int GetCCTALKPortNo(void);   //On Success return portno otherwise return -1 (fail)
int GetLogFilePath(char*);     //On Success return logfile string otherwise return -1(fail)
int GetDSCUPortNo(void);       //On Success return dscu portno otherwise return -1(fail)
int GetTransTimeout();         //On Success return trans timeout otherwise return -1(fail)
int GetElectronicsLockTime(void); //On Success return electronics time otherwise return -1(fail)
#endif
