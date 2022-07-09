#ifndef _UPSSTATUS_H_
#define _UPSSTATUS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atvmlog.h"

//web link:http://www.cyberciti.biz/faq/debian-ubuntu-centos-rhel-install-apcups/

//#define UPSSTATUS_MAIN

void RemoveSpaceFromString(char *Dest,char *Source);

int CheckAPCServiceStatus();

int StartAPCServiceStatus();

int GetAPCServiceStatus();

static void ups_delay_mSec(int milisec) ;


#endif

