#ifndef _COMMON_H_

#define _COMMON_H_

#include <pthread.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define  SUCCESS             1

#define  FAIL               -1

#define  ON                  1

#define  OFF                 0

#define  ENABLE	 	     1

#define  DISABLE	     0

#define MAX_STR_SIZE_OF_TRNSLOG   500  

#define MAX_STR_SIZE_OF_MCHN_LOG  250

/////////////////////////////////////////////////////////////////////////////////////////////////////


//Various Main Test Funtion

//#define ATVM_DEBUG  

//#define ASCRMAPIMAIN                              

//#define ATVM_ESCORW_MAIN

//#define ATVMCA_MAIN

//#define ATVMALARM_MAIN

//#define READINI_MAIN


////////////////////////////////////////////////////////////////////////////////////////////////////


extern pthread_mutex_t g_NAEnableThreadmutex ;

extern pthread_cond_t  g_NAEnableThreadCond ;

extern pthread_mutex_t g_CAEnableThreadmutex ;

extern pthread_cond_t  g_CAEnableThreadCond ;

////////////////////////////////////////////////////////////////////////////////////////////////////
                    
#endif 


