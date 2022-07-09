#ifndef _ASCRMSECURITYJNI_H_
#define _ASCRMSECURITYJNI_H_

//#define  CRISAPI_TIMEOUT_TEST

#define  JNI_EXPORT_ENABLE                      

#ifdef   JNI_EXPORT_ENABLE 

#define  WAIT_TIME_DOOR_OPEN                  60  //(Second)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <time.h>
#include "jni.h" 
#include "Cris_Security.h" 
#include "atvmdevice.h"
#include "atvmapi.h"
#include "cctalkdevs.h"
#include "DSCU.h"
#include "atvmalarm.h"
#include "readini.h"
#include "upsstatus.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* OpenCashBox(void *ptr);

static void JNI_delay_mSec(int milisec) ;

static int ActivateCCTALKComPort_V2(int CCTALKPortNumber);

static int ActivateCCTALKComPort();

static int DeActivateCCTALKComPort();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

#endif
