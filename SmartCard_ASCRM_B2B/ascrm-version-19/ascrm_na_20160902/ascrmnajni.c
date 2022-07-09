#include "ascrmnajni.h"


#ifdef JNI_EXPORT_ENABLE 

static unsigned int g_cashboxopenexpirytime;

static unsigned int g_dooropenexpirytime;

static bool g_activate_device_flag=false;

static pthread_mutex_t g_activatedevicemutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_ComPortAccessmutex = PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool g_ThreadStop =false;

static pthread_mutex_t g_connectdeviceprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_deactivateDeviceprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_GetAcceptedAmountprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_deviceStatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_acceptNotesprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_StackAcceptedNotesprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_ReturnAcceptedNotesprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_defaultCancelprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_defaultCommitprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_disableAlarmprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getDoorstatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getUpsStatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getBatteryStatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_printerStatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_CashBoxmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t  g_CashBoxEnableThreadCond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t  g_EnableTheseDenominationsprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_IsNoteRemovedprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_GetValidNoteprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_AcceptCurrentNoteprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_ReturnCurrentNoteprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_HoldCurrentNoteprocmutex = PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*

bool SetupSmartCardLogFile()
{

         

          bool rtcode = false;

          //Set SmartCard General LOG File path
         
          unsigned char LogFilePath[8192];
      
          memset(LogFilePath,'\0',8192);
           
          int day=0,mon=0,yr=0,hr=0,min=0,sec=0;

          char *destStr=NULL,*file=NULL;

	  getDateTime(&day,&mon,&yr,&hr,&min,&sec);

	  destStr = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

          file = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

	  sprintf(destStr,"/Mega_Smartcard_Dt_%d%d%d_Tm_%d_%d.log",yr,mon,day,hr,min,sec);

          ////////////////////////////////////////////////////////////////////////////////////////////////////////

	  if( -1 == GetLogFilePath(file) )
	  {

	          printf("\n/[Mega Designs Pvt. Ltd.] Error Create smartcard general log file.");
                  
                  rtcode = false;

	  } 
          else
          {

		 //Copy trans logfile name
                 strcat(file,destStr);
		 
                 strcat(LogFilePath,file);
                 
                 SetGeneralFileLogPath( LogFilePath );

                 printf("\n[Mega Designs Pvt. Ltd.] successfully created smartcard general log file.");

                 rtcode = true;
                 
         }

         ////////////////////////////////////////////////////////////////////////////////////////////////////////
          
          char logfilemode[LOG_ARRAY_SIZE];

	  memset(logfilemode,'\0',LOG_ARRAY_SIZE);

          if( READ_INI_SUCCESS == GetLogFileMode(logfilemode) )
          {
                 if( 0 == strcmp( logfilemode, "true" ) )
                 {
                        g_LogModeType=true; //now logmode enable
                        printf("\n[Mega Designs Pvt. Ltd.] Log mode enable.");
                 }
                 else if( 0 == strcmp( logfilemode, "false" ) )
                 {
                        g_LogModeType=false; //now logmode disable
                         printf("\n[Mega Designs Pvt. Ltd.] Log mode disable.");
                 }
                 else
                 {
                        g_LogModeType=false; //now logmode disable
                        printf("\n[Mega Designs Pvt. Ltd.] Log mode disable.");
                 }
          }
          else
          {
              g_LogModeType=false;
              printf("\n[Mega Designs Pvt. Ltd.] Log mode disable.");

          }

         ////////////////////////////////////////////////////////////////////////////////////////////////////////
        
         if(NULL!= destStr)
         {
             free(destStr);
         }
        
         if(NULL!=file)
         {
	     free(file);
         }


         return rtcode;
        



}//SetupSmartCardLogFile() end


*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupNoteAcceptorLogFile(  JNIEnv  *env, 
                                                                         jobject  jobj, 
		                                                         jstring  jniLogfile, 
		                                                         jint     jniLogLevel, 
		                                                         jstring  jniDeviceId,
		                                                         jstring  jniLogdllPath
                                                                      )
{

     
               ///////////////////////////////////////////////////////////////////////////////////////////////////////

	       g_Ascrm_writeFileLog=NULL;

	       g_lib_handle=NULL;

               ///////////////////////////////////////////////////////////////////////////////////////////////////////
	       
	       //Step 1:Log dll Path

	       const char *LogdllPathstr= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

	       //printf("\n[Java_Cris_Currency_SetupNoteAcceptorLogFile()] Log dll Path : %s", LogdllPathstr);

               SetSoFilePath((unsigned char*)LogdllPathstr);

	       (*env)->ReleaseStringUTFChars(env, jniLogdllPath, LogdllPathstr);


	       //////////////////////////////////////////////////////////////////////////////////////////////////////

	       //Step 2:Log File Name

	       const char *LogFileName= (*env)->GetStringUTFChars(env,jniLogfile,0);

	       //printf("\n[Java_Cris_Currency_SetupNoteAcceptorLogFile()] Log File Name : %s", LogFileName);

	       SetGeneralFileLogPath( (unsigned char*) LogFileName );

	       (*env)->ReleaseStringUTFChars(env, jniLogfile, LogFileName);

	       ///////////////////////////////////////////////////////////////////////////////////////////////////
               
               //Step 3: Set LogLevel Mode
               g_LogModeLevel = (int)jniLogLevel;
               
               //printf("\n[Java_Cris_Currency_SetupNoteAcceptorLogFile()] JNI Log File Mode : %d", (int)jniLogLevel);

               //printf("\n[Java_Cris_Currency_SetupNoteAcceptorLogFile()] C Log File Mode : %d", g_LogModeLevel);

               ///////////////////////////////////////////////////////////////////////////////////////////////////

               //Step 4: Get Device id

               const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);

	       //printf("\n[Java_Cris_SmartCard_SetupSmartCardLogFile()] Device Id : %s", deviceid);

               strcpy(g_Deviceid,deviceid);

               (*env)->ReleaseStringUTFChars(env,jniDeviceId, deviceid);


               /////////////////////////////////////////////////////////////////////////////////////////////////////////


	       return true;




 
}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupNoteAcceptorLogFile END


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_Cris_Currency_WriteNativeLog( JNIEnv *env, 
                                                          jobject jobj, 
                                                          jstring jniLogMessage
                                                        )
{

       
               const char *LogMessage= (*env)->GetStringUTFChars(env,jniLogMessage,0);

	       //printf("\n [Java_Cris_Currency_WriteNativeLog()] Log File Name : %s", LogMessage);

	       writeFileLog((char*)LogMessage);

	       (*env)->ReleaseStringUTFChars(env, jniLogMessage, LogMessage);





}//JNIEXPORT void JNICALL Java_Cris_Currency_WriteNativeLog end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Activate Device
JNIEXPORT jint JNICALL Java_Cris_Currency_ConnectDevice(  JNIEnv *env, 
                                                          jobject jobj, 
                                                          jint PortId, 
                                                          jint EscrowClearanceMode, 
                                                          jint Timeout
                                                        )
{


                    writeFileLog("[Java_Cris_Currency_ConnectDevice()] Entry.");

                    pthread_mutex_lock( &g_connectdeviceprocmutex );

		    pthread_mutex_lock( &g_activatedevicemutex );

                    pthread_mutex_lock( &g_ComPortAccessmutex );

                    jint JniRtCode=-1;
                 
                    #ifdef CRISAPI_TIMEOUT_TEST
                    struct timespec begints, endts,diffts;
                    unsigned char log[100];
                    clock_gettime(CLOCK_MONOTONIC, &begints);
                    #endif

		    if( false == g_activate_device_flag )
		    {

			    JniRtCode=(jint)activateDevice(PortId,EscrowClearanceMode,Timeout);

			    if( ( 2 == JniRtCode ) || ( 1 == JniRtCode ) || ( (-6) == JniRtCode ))
			    {

				 g_activate_device_flag=true;

			    }
			    else 
			    {
				 g_activate_device_flag=false;

			    }     

                    }
                    else
                    {
                         writeFileLog("[Java_Cris_Currency_ConnectDevice()] Already device activated so no need to activate again.");

                    }

                    #ifdef CRISAPI_TIMEOUT_TEST
                    clock_gettime(CLOCK_MONOTONIC, &endts);
                    diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                    memset(log,'\0',100);
		    sprintf(log,"[Java_Cris_Currency_ConnectDevice()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
                    writeAPITimeoutLog(log);
                    #endif

                    pthread_mutex_unlock( &g_ComPortAccessmutex );

		    pthread_mutex_unlock( &g_activatedevicemutex );

                    pthread_mutex_unlock( &g_connectdeviceprocmutex );

                    writeFileLog("[Java_Cris_Currency_ConnectDevice()] Exit.");

		    return JniRtCode;




}//Java_Cris_CashApi_activateDevice() end



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Disconnect Device
JNIEXPORT jint JNICALL Java_Cris_Currency_DisConnectDevice(JNIEnv *env, jobject jobj,jint timeout)
{


             writeFileLog("[Java_Cris_Currency_DisConnectDevice()] Entry.");

             pthread_mutex_lock( &g_deactivateDeviceprocmutex );

             pthread_mutex_lock( &g_activatedevicemutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             jint rtcode=-1;
             
             unsigned char log[100];

             ////////////////////////////////////////////////////////////////////////////////////////////

             #ifdef CRISAPI_TIMEOUT_TEST

             struct timespec begints, endts,diffts;

             clock_gettime(CLOCK_MONOTONIC, &begints);

             #endif

	     if( true == g_activate_device_flag )
	     {

		     rtcode =(jint)deactivateDevice();

		     g_activate_device_flag=false;
		     

	     }

             ////////////////////////////////////////////////////////////////////////////////////////

             memset(log,'\0',100);

             sprintf(log,"[Java_Cris_Currency_DisConnectDevice()]  deactivateDevice return code : %d.", rtcode); 
             writeFileLog(log);
	     
             ////////////////////////////////////////////////////////////////////////////////////////
	     
             #ifdef CRISAPI_TIMEOUT_TEST
             clock_gettime(CLOCK_MONOTONIC, &endts);

             diffts.tv_sec = endts.tv_sec - begints.tv_sec;

             memset(log,'\0',100);

             sprintf(log,"[Java_Cris_Currency_DisConnectDevice()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 

             writeAPITimeoutLog(log);

             #endif

             //////////////////////////////////////////////////////////////////////////////////////

             pthread_mutex_unlock( &g_ComPortAccessmutex );

	     pthread_mutex_unlock( &g_activatedevicemutex );

             pthread_mutex_unlock( &g_deactivateDeviceprocmutex );

             writeFileLog("[Java_Cris_Currency_DisConnectDevice()] Exit.");

	     return rtcode;



}//Java_Cris_Currency_DisConnectDevice() end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Device Status
JNIEXPORT jint JNICALL Java_Cris_Currency_DeviceStatus(JNIEnv *env, jobject jobj, jint Timeout)
{



            writeFileLog("[Java_Cris_Currency_DeviceStatus()] Entry.");

            pthread_mutex_lock( &g_deviceStatusprocmutex );
   
	    pthread_mutex_lock( &g_activatedevicemutex );

            pthread_mutex_lock( &g_ComPortAccessmutex );

            jint rtcode=-1;

            #ifdef CRISAPI_TIMEOUT_TEST
            struct timespec begints, endts,diffts;
            unsigned char log[100];
            clock_gettime(CLOCK_MONOTONIC, &begints);
            #endif

	    if( true == g_activate_device_flag)
	    {
                   
		    rtcode=(jint)deviceStatus(Timeout);
		    
	    }
	    else if( false  == g_activate_device_flag)
	    {
		    
		     rtcode =-1;

	    }
	    
            #ifdef CRISAPI_TIMEOUT_TEST
            clock_gettime(CLOCK_MONOTONIC, &endts);
            diffts.tv_sec = endts.tv_sec - begints.tv_sec; 
            memset(log,'\0',100);
            sprintf(log,"[Java_Cris_Currency_DeviceStatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
            writeAPITimeoutLog(log);
            #endif

            pthread_mutex_unlock( &g_ComPortAccessmutex );

	    pthread_mutex_unlock( &g_activatedevicemutex );

            pthread_mutex_unlock( &g_deviceStatusprocmutex );

            writeFileLog("[Java_Cris_Currency_DeviceStatus()] Exit.");

	    return rtcode;




}//Java_Cris_Currency_DeviceStatus() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Accept Notes
JNIEXPORT jint JNICALL Java_Cris_Currency_defaultCommit(JNIEnv *env, jobject jobj,jint timeout)
{

            writeFileLog("[Java_Cris_Currency_defaultCommit()] Entry.");

            pthread_mutex_lock( &g_defaultCommitprocmutex );
   
	    pthread_mutex_lock( &g_activatedevicemutex );

            pthread_mutex_lock( &g_ComPortAccessmutex );

            jint rtcode=-1;

            #ifdef CRISAPI_TIMEOUT_TEST

            struct timespec begints, endts,diffts;

            unsigned char log[100];

            clock_gettime(CLOCK_MONOTONIC, &begints);

            #endif

	    if( true == g_activate_device_flag)
	    {

		    rtcode=(jint)defaultCommit(timeout);
		    
	    }
	    else if( false  == g_activate_device_flag)
	    {
		     writeFileLog("[Java_Cris_Currency_defaultCommit()] Communication failure.");
		     rtcode =-1; //Exception

	    }
	    
            #ifdef CRISAPI_TIMEOUT_TEST

            clock_gettime(CLOCK_MONOTONIC, &endts);

            diffts.tv_sec = endts.tv_sec - begints.tv_sec; 

            memset(log,'\0',100);

            sprintf(log,"[Java_Cris_Currency_defaultCommit()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 

            writeAPITimeoutLog(log);

            #endif

            pthread_mutex_unlock( &g_ComPortAccessmutex );

	    pthread_mutex_unlock( &g_activatedevicemutex );

            pthread_mutex_unlock( &g_defaultCommitprocmutex);

            writeFileLog("[Java_Cris_Currency_defaultCommit()] Exit.");

	    return rtcode;


}//Java_Cris_Currency_defaultCommit() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Reject notes
JNIEXPORT jint JNICALL Java_Cris_Currency_defaultCancel(JNIEnv *env, jobject jobj,jint timeout)
{

            writeFileLog("[Java_Cris_Currency_defaultCancel()] Entry.");

            pthread_mutex_lock( &g_defaultCancelprocmutex );
   
	    pthread_mutex_lock( &g_activatedevicemutex );

            pthread_mutex_lock( &g_ComPortAccessmutex );

            jint rtcode=-1;

            #ifdef CRISAPI_TIMEOUT_TEST

            struct timespec begints, endts,diffts;

            unsigned char log[100];

            clock_gettime(CLOCK_MONOTONIC, &begints);

            #endif

	    if( true == g_activate_device_flag)
	    {

		     rtcode=(jint)defaultCancel(timeout);
		    
	    }
	    else if( false  == g_activate_device_flag)
	    {
		     writeFileLog("[Java_Cris_Currency_defaultCancel()] Communication failure.");
		     rtcode =-1; //Exception

	    }
	    
            #ifdef CRISAPI_TIMEOUT_TEST

            clock_gettime(CLOCK_MONOTONIC, &endts);

            diffts.tv_sec = endts.tv_sec - begints.tv_sec; 

            memset(log,'\0',100);

            sprintf(log,"[Java_Cris_Currency_defaultCancel()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec );
 
            writeAPITimeoutLog(log);

            #endif

            pthread_mutex_unlock( &g_ComPortAccessmutex );

	    pthread_mutex_unlock( &g_activatedevicemutex );

            pthread_mutex_unlock( &g_defaultCancelprocmutex);

            writeFileLog("[Java_Cris_Currency_defaultCancel()] Exit.");

	    return rtcode;



}//Java_Cris_Currency_defaultCancel() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_Currency_EnableTheseDenominations(JNIEnv *env, jobject jobj, jint DenomMask, jint Timeout)
{
         
         writeFileLog("[Java_Cris_Currency_EnableTheseDenominatio()] Entry.");

         pthread_mutex_lock( &g_EnableTheseDenominationsprocmutex );

         int rtcode =-1;

         rtcode = EnableTheseDenomination( DenomMask,Timeout) ;

         pthread_mutex_unlock( &g_EnableTheseDenominationsprocmutex );

         writeFileLog("[Java_Cris_Currency_EnableTheseDenominatio()] Exit.");

         return rtcode;
         


}//Java_Cris_Currency_EnableTheseDenominations() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Is Note Removed
JNIEXPORT jint JNICALL Java_Cris_Currency_IsNoteRemoved(JNIEnv *env, jobject jobj, jint timeout)
{

         writeFileLog("[Java_Cris_Currency_IsNoteRemoved()] Entry.");

         pthread_mutex_lock( &g_IsNoteRemovedprocmutex );

         int rtcode =-1;

         rtcode = IsNoteRemoved(timeout);

         pthread_mutex_unlock( &g_IsNoteRemovedprocmutex );

         writeFileLog("[Java_Cris_Currency_IsNoteRemoved()] Exit.");

         return rtcode;
         



}//Java_Cris_Currency_IsNoteRemoved() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Start accept fare
JNIEXPORT jboolean JNICALL Java_Cris_Currency_AcceptNotes(JNIEnv *env, jobject jobj,jint jFare, jint Timeout)

{

                     jint jmaxNoOfCash =MAX_NMBR_OF_NOTE;

                     jint jmaxNoOfCoin =0;

                     writeFileLog("[Java_Cris_Currency_AcceptNotes()] Entry.");

                     pthread_mutex_lock( &g_acceptNotesprocmutex );

		     pthread_mutex_lock( &g_activatedevicemutex );

                     pthread_mutex_lock( &g_ComPortAccessmutex );

                     jboolean rtcode=false;
               
                     #ifdef CRISAPI_TIMEOUT_TEST
                     struct timespec begints, endts,diffts;
                     unsigned char log[100];
                     clock_gettime(CLOCK_MONOTONIC, &begints);
                     #endif

                     
		     if( true == g_activate_device_flag)
		     {

			     rtcode = (jboolean)acceptFare(jFare,
jmaxNoOfCash,jmaxNoOfCoin,Timeout);
	             }
		     else if( false  == g_activate_device_flag )
		     {
			    
			     rtcode =  false;
                     }

                     #ifdef CRISAPI_TIMEOUT_TEST
                     clock_gettime(CLOCK_MONOTONIC, &endts);
                     diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                     memset(log,'\0',100);
                     sprintf(log,"[Java_Cris_Currency_AcceptNotes()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
                     writeAPITimeoutLog(log);
                     #endif

                     pthread_mutex_unlock( &g_ComPortAccessmutex );

		     pthread_mutex_unlock( &g_activatedevicemutex );

                     pthread_mutex_unlock( &g_acceptNotesprocmutex );
		 
                     writeFileLog("[Java_Cris_Currency_AcceptNotes()] Exit.");

		     return rtcode;



}//Java_Cris_CashApi_acceptFare() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Commit Notes
JNIEXPORT jint JNICALL Java_Cris_Currency_StackAcceptedNotes( JNIEnv *env, jobject jobj,jint Timeout)
{
		     

                     writeFileLog("[Java_Cris_Currency_StackAcceptedNotes()] Entry.");
                     
                     pthread_mutex_lock( &g_StackAcceptedNotesprocmutex );

		     pthread_mutex_lock( &g_activatedevicemutex );

                     pthread_mutex_lock( &g_ComPortAccessmutex );

                     char log[100];

                     jint rtcode= 0;

                     #ifdef CRISAPI_TIMEOUT_TEST
                     struct timespec begints, endts,diffts;
                     unsigned char log[100];
                     clock_gettime(CLOCK_MONOTONIC, &begints);
                     #endif

                     /////////////////////////////////////////////////////////////////////////////////////

		     if( true == g_activate_device_flag)
		     {
                           rtcode= commitTranInterface(Timeout);
                     }
		     else if( false  == g_activate_device_flag )
		     {
                           writeFileLog("[Java_Cris_Currency_StackAcceptedNotes()] Communication failure.");
                           rtcode = -1; //Communication failure

		     }

                     /////////////////////////////////////////////////////////////////////////////////////
		    
                     #ifdef CRISAPI_TIMEOUT_TEST
                     clock_gettime(CLOCK_MONOTONIC, &endts);
                     diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                     memset(log,'\0',100);
                     sprintf(log,"[Java_Cris_Currency_StackAcceptedNotes()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
                     writeAPITimeoutLog(log);
                     #endif


                     /////////////////////////////////////////////////////////////////////////////////

                     memset(log,'\0',100);

                     sprintf(log,"[Java_Cris_Currency_StackAcceptedNotes()] Return Code : %d .",rtcode);

                     writeFileLog(log);

                     //////////////////////////////////////////////////////////////////////////////////

                     pthread_mutex_unlock( &g_ComPortAccessmutex );

		     pthread_mutex_unlock( &g_activatedevicemutex );

                     pthread_mutex_unlock( &g_StackAcceptedNotesprocmutex );

                     writeFileLog("[Java_Cris_Currency_StackAcceptedNotes()] Exit.");

		     return rtcode;

  
}//Java_Cris_CashApi_commitTran() end 



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Return notes
JNIEXPORT jint JNICALL Java_Cris_Currency_ReturnAcceptedNotes(JNIEnv *env, jobject jobj, jint Timeout)
{
 

  
             writeFileLog("[Java_Cris_Currency_ReturnAcceptedNotes()] Entry.");

             char log[100];

             pthread_mutex_lock( &g_ReturnAcceptedNotesprocmutex );

	     pthread_mutex_lock( &g_activatedevicemutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             jint rtcode=0;
	   
             #ifdef CRISAPI_TIMEOUT_TEST
             struct timespec begints, endts,diffts;
             unsigned char log[100];
             clock_gettime(CLOCK_MONOTONIC, &begints);
             #endif

	     if( true == g_activate_device_flag)
	     {
                  rtcode = cancelTranInterface(Timeout); 
             }
	     else if( false  == g_activate_device_flag)
	     {
                  writeFileLog("[Java_Cris_Currency_ReturnAcceptedNotes()] Communication failure.");
		  rtcode = -1; //Communication failure
             }


             memset(log,'\0',100);

             sprintf(log,"[Java_Cris_Currency_ReturnAcceptedNotes()] Return Code : %d .",rtcode);

             writeFileLog(log);
	     
            #ifdef CRISAPI_TIMEOUT_TEST
            clock_gettime(CLOCK_MONOTONIC, &endts);
            diffts.tv_sec = endts.tv_sec - begints.tv_sec;
            memset(log,'\0',100);
            sprintf(log,"[Java_Cris_Currency_ReturnAcceptedNotes()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
            writeAPITimeoutLog(log);
            #endif

            pthread_mutex_unlock( &g_ComPortAccessmutex );

	    pthread_mutex_unlock( &g_activatedevicemutex );

            pthread_mutex_unlock( &g_ReturnAcceptedNotesprocmutex );

            writeFileLog("[Java_Cris_Currency_ReturnAcceptedNotes()] Exit.");

	    return rtcode;


 
}//Java_Cris_CashApi_cancelTran() end 



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Get Denomoniation
JNIEXPORT jint JNICALL Java_Cris_Currency_GetAcceptedAmount( JNIEnv *env, jobject jobj, jbyteArray array)
{

                  
                  //writeFileLog("[Java_Cris_Currency_GetAcceptedAmount()] Entry.");

                  pthread_mutex_lock( &g_GetAcceptedAmountprocmutex );

                  pthread_mutex_lock( &g_activatedevicemutex );

                  #ifdef CRISAPI_TIMEOUT_TEST
                  //struct timespec begints, endts,diffts;
                  //clock_gettime(CLOCK_MONOTONIC, &begints);
                  #endif
                  int Carray[MAX_NMBR_OF_NOTE ][2],counter=0;
                  unsigned char log[100];
                  memset(log,'\0',100);
                  for (counter = 0; counter < MAX_NMBR_OF_NOTE  ; counter++) 
		  {
                        Carray[counter][0]=0;
                        Carray[counter][1]=0;
                  }

                  if( true == g_activate_device_flag )
                  {

                       pthread_mutex_unlock( &g_activatedevicemutex );
                       jint rtcode=-1;
		       jint JniRtcode=0;

                       /*
		       rtcode=(*env)->EnsureLocalCapacity(env,20);
		       if (0!=rtcode) 
		       {
                          
			     writeFileLog("[Java_Cris_Currency_GetAcceptedAmount()] Given number of local references cannot be created.");
			     pthread_mutex_unlock( &g_GetAcceptedAmountprocmutex );

                             writeFileLog("[Java_Cris_CashApi_getDenomination()] Exit.");

                             #ifdef CRISAPI_TIMEOUT_TEST
		             //clock_gettime(CLOCK_MONOTONIC, &endts);
			     //diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                             //memset(log,'\0',100);
		             //sprintf(log,"[Java_Cris_Currency_GetAcceptedAmount()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
		             //writeAPITimeoutLog(log);
			     #endif
                             return 0; 

		       }
                       */

		       //////////////////////////////////////////////////////////////////////////////////////////////////

		       //Call C function
                       JniRtcode=(jint)getDenomination(Carray);

                       //////////////////////////////////////////////////////////////////////////////////////////////////
                       
                       /*
                       //Create jni array in c layer
                       jbyte jnibytearray [40]; 

                       for (counter = 0; counter < 40 ; counter++) 
		       {
                          jnibytearray[counter]=0;
                       }
		       
		       //Copy Local c Array to jni local byte Array      
		       for (counter = 0; counter < 40 ; counter++) 
		       {

				jnibytearray[counter]=Carray[counter][1];

                       }
		 
                       //Now set JAVA original bytearray
                       (*env)->SetByteArrayRegion(env, array, 0, arrLength, jnibytearray);
                       */

                       //////////////////////////////////////////////////////////////////////////////////////////////
                       
                       //Get JAVA bytearray pointer
                       jsize arrLength = 0;

                       arrLength =  (*env)->GetArrayLength(env,array);

                       memset(log,'\0',100);

		       sprintf(log,"[Java_Cris_Currency_GetAcceptedAmount()] Java Byte Array Length=%d .",arrLength );
 
                       //writeFileLog(log);

                       //Get ByteArray Element from java byte array
                       jbyte *bytearray = (*env)->GetByteArrayElements(env, array, 0);

                       //Copy Local c Array to java byte Array  

		       for (counter = 0; counter < arrLength ; counter++) 
		       {
                               bytearray[counter]=-1;
                       }    

		       for (counter = 0; counter < arrLength ; counter++) 
		       {
			       if( Carray[counter][1] > 0 ){
				  memset(log,'\0',100);
				  sprintf(log,"[Java_Cris_Currency_GetAcceptedAmount()] Denom Code=%d .", Carray[counter][1] );
				  //writeFileLog(log);
			       }//if end
                               bytearray[counter]=Carray[counter][1];
                       }//if end

                       //Now release byte array elements
                       (*env)->ReleaseByteArrayElements(env, array,bytearray, 0 );

                       ///////////////////////////////////////////////////////////////////////////////////////////////////

                       pthread_mutex_unlock( &g_GetAcceptedAmountprocmutex );

                       //writeFileLog("[Java_Cris_Currency_GetAcceptedAmount()] Exit.");

                       #ifdef CRISAPI_TIMEOUT_TEST

		       //clock_gettime(CLOCK_MONOTONIC, &endts);

		       //diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                       //memset(log,'\0',100);

		       //sprintf(log,"[Java_Cris_Currency_GetAcceptedAmount()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
		       //writeAPITimeoutLog(log);

		       #endif

                       return JniRtcode;

             }
             
             pthread_mutex_unlock( &g_activatedevicemutex );
             
             ///////////////////////////////////////////////////////////////////////////////////////////////

             pthread_mutex_lock( &g_activatedevicemutex );

             if( false  == g_activate_device_flag)
	     {

		     pthread_mutex_unlock( &g_activatedevicemutex );
                     pthread_mutex_unlock( &g_GetAcceptedAmountprocmutex );
                     //writeFileLog("[Java_Cris_Currency_GetAcceptedAmount()] Exit.");

                     #ifdef CRISAPI_TIMEOUT_TEST

		     //clock_gettime(CLOCK_MONOTONIC, &endts);

		     //diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                     //memset(log,'\0',100);

		     //sprintf(log,"[Java_Cris_Currency_GetAcceptedAmount()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
		     //writeAPITimeoutLog(log);

		     #endif

		     return  (-1);


	     }//if( false  == g_activate_device_flag)


             pthread_mutex_unlock( &g_GetAcceptedAmountprocmutex );

             pthread_mutex_unlock( &g_activatedevicemutex );

             ///////////////////////////////////////////////////////////////////////////////////////////////


}//Java_Cris_CashApi_getDenomination() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



JNIEXPORT jint JNICALL Java_Cris_Currency_GetValidNote(JNIEnv *env, jobject jobj, jint Denom, jint Timeout)
{

             writeFileLog("[Java_Cris_Currency_GetValidNote()] Entry.");

             pthread_mutex_lock( &g_GetValidNoteprocmutex );

	     pthread_mutex_lock( &g_activatedevicemutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             jint rtcode=0;
	   
             #ifdef CRISAPI_TIMEOUT_TEST
             struct timespec begints, endts,diffts;
             unsigned char log[100];
             clock_gettime(CLOCK_MONOTONIC, &begints);
             #endif

	     if( true == g_activate_device_flag)
	     {
                      rtcode= GetValidNote(Denom,Timeout); 

             }
	     else if( false  == g_activate_device_flag)
	     {
		    
		     rtcode = -1;//Communication failure

	     }
	     
             #ifdef CRISAPI_TIMEOUT_TEST
             clock_gettime(CLOCK_MONOTONIC, &endts);
             diffts.tv_sec = endts.tv_sec - begints.tv_sec;
             memset(log,'\0',100);
             sprintf(log,"[Java_Cris_Currency_GetValidNote()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
             writeAPITimeoutLog(log);
             #endif

             pthread_mutex_unlock( &g_ComPortAccessmutex  );

	     pthread_mutex_unlock( &g_activatedevicemutex );

             pthread_mutex_unlock( &g_GetValidNoteprocmutex );

             writeFileLog("[Java_Cris_Currency_GetValidNote()] Exit.");

	     return rtcode;



}//Java_Cris_Currency_GetValidNote end


JNIEXPORT jint JNICALL Java_Cris_Currency_AcceptCurrentNote(JNIEnv *env, jobject jobj, jint Denom, jint Timeout)
{

             writeFileLog("[Java_Cris_Currency_AcceptCurrentNote()] Entry.");

             pthread_mutex_lock( &g_AcceptCurrentNoteprocmutex );

	     pthread_mutex_lock( &g_activatedevicemutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             jint rtcode=0;
	   
             #ifdef CRISAPI_TIMEOUT_TEST
             struct timespec begints, endts,diffts;
             unsigned char log[100];
             clock_gettime(CLOCK_MONOTONIC, &begints);
             #endif

	     if( true == g_activate_device_flag)
	     {
                      rtcode= AcceptCurrentNote(Denom,Timeout); 

             }
	     else if( false  == g_activate_device_flag)
	     {
		    
		     rtcode = -1;//Communication failure

	     }
	     
             #ifdef CRISAPI_TIMEOUT_TEST
             clock_gettime(CLOCK_MONOTONIC, &endts);
             diffts.tv_sec = endts.tv_sec - begints.tv_sec;
             memset(log,'\0',100);
             sprintf(log,"[Java_Cris_Currency_AcceptCurrentNote()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
             writeAPITimeoutLog(log);
             #endif

             pthread_mutex_unlock( &g_ComPortAccessmutex  );

	     pthread_mutex_unlock( &g_activatedevicemutex );

             pthread_mutex_unlock( &g_AcceptCurrentNoteprocmutex );

             writeFileLog("[Java_Cris_Currency_AcceptCurrentNote()] Exit.");

	     return rtcode;


}//Java_Cris_Currency_AcceptCurrentNote end



JNIEXPORT jint JNICALL Java_Cris_Currency_ReturnCurrentNote(JNIEnv *env, jobject jobj, jint Timeout)
{


             writeFileLog("[Java_Cris_Currency_ReturnCurrentNote()] Entry.");

             pthread_mutex_lock( &g_AcceptCurrentNoteprocmutex );

	     pthread_mutex_lock( &g_activatedevicemutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             jint rtcode=0;
	   
             #ifdef CRISAPI_TIMEOUT_TEST
             struct timespec begints, endts,diffts;
             unsigned char log[100];
             clock_gettime(CLOCK_MONOTONIC, &begints);
             #endif

	     if( true == g_activate_device_flag)
	     {
                      rtcode= ReturnCurrentNote(Timeout); 

             }
	     else if( false  == g_activate_device_flag)
	     {
		    
		     rtcode = -1;//Communication failure

	     }
	     
             #ifdef CRISAPI_TIMEOUT_TEST
             clock_gettime(CLOCK_MONOTONIC, &endts);
             diffts.tv_sec = endts.tv_sec - begints.tv_sec;
             memset(log,'\0',100);
             sprintf(log,"[Java_Cris_Currency_ReturnCurrentNote()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
             writeAPITimeoutLog(log);
             #endif

             pthread_mutex_unlock( &g_ComPortAccessmutex  );

	     pthread_mutex_unlock( &g_activatedevicemutex );

             pthread_mutex_unlock( &g_AcceptCurrentNoteprocmutex );

             writeFileLog("[Java_Cris_Currency_ReturnCurrentNote()] Exit.");

	     return rtcode;



}//Java_Cris_Currency_ReturnCurrentNote end




JNIEXPORT jint JNICALL Java_Cris_Currency_HoldCurrentNote(JNIEnv *env, jobject jobj, jint Timeout)
{


             writeFileLog("[Java_Cris_Currency_HoldCurrentNote()] Entry.");

             pthread_mutex_lock( &g_HoldCurrentNoteprocmutex );

	     pthread_mutex_lock( &g_activatedevicemutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             jint rtcode=0;
	   
             #ifdef CRISAPI_TIMEOUT_TEST
             struct timespec begints, endts,diffts;
             unsigned char log[100];
             clock_gettime(CLOCK_MONOTONIC, &begints);
             #endif

	     if( true == g_activate_device_flag)
	     {
                    rtcode= B2BHoldCurrentNote(Timeout); 
             }
	     else if( false  == g_activate_device_flag)
	     {
		    rtcode = -1; //Communication failure
             }
	     
             #ifdef CRISAPI_TIMEOUT_TEST
             clock_gettime(CLOCK_MONOTONIC, &endts);
             diffts.tv_sec = endts.tv_sec - begints.tv_sec;
             memset(log,'\0',100);
             sprintf(log,"[Java_Cris_Currency_HoldCurrentNote()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
             writeAPITimeoutLog(log);
             #endif

             pthread_mutex_unlock( &g_ComPortAccessmutex  );

	     pthread_mutex_unlock( &g_activatedevicemutex );

             pthread_mutex_unlock( &g_HoldCurrentNoteprocmutex );

             writeFileLog("[Java_Cris_Currency_HoldCurrentNote()] Exit.");

	     return rtcode;



}//Java_Cris_Currency_ReturnCurrentNote end





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Millisecond delay
static void JNI_delay_mSec(int milisec) 
{


        unsigned char log[100];

        memset(log,'\0',100);

        struct timespec req = {0},rim={0};

	req.tv_sec = 0;

	req.tv_nsec = milisec * 1000000L;

      

        int rtcode=-1;

        rtcode=clock_nanosleep( CLOCK_MONOTONIC,
                                0,
                                &req,
                                NULL
                              );
        
       

        if(rtcode<0)
        {

		memset(log,'\0',100);
				   
		sprintf(log,"[JNI_delay_mSec()] clock_nanosleep failed with return code %d .",rtcode); 

		writeFileLog(log);


        }


}//JNI_delay_mSec() end


#endif
