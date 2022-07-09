#ifndef _ASCRMNAJNI_H_
#define _ASCRMNAJNI_H_

//#define  CRISAPI_TIMEOUT_TEST

#define  JNI_EXPORT_ENABLE                      

#ifdef   JNI_EXPORT_ENABLE 

#define  WAIT_TIME_DOOR_OPEN                  60  //(Second)

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <time.h>
#include "jni.h" 
#include "Cris_Currency.h" 
#include "atvmdevice.h"
#include "atvmapi.h"
#include "cctalkdevs.h"
#include "DSCU.h"
#include "atvmalarm.h"
#include "readini.h"
#include "upsstatus.h"
#include "b2b.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* OpenCashBox(void *ptr);

static void JNI_delay_mSec(int milisec) ;

#endif

#endif
