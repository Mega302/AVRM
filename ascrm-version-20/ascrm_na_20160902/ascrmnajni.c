#include "ascrmnajni.h"
#include "currencyreturn.h"

//++#ifdef JNI_EXPORT_ENABLE 
static unsigned int g_cashboxopenexpirytime;
static unsigned int g_dooropenexpirytime;
static bool g_activate_device_flag=false;
static pthread_mutex_t g_activatedevicemutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_ComPortAccessmutex  = PTHREAD_MUTEX_INITIALIZER;

//++/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool g_ThreadStop =false;
static pthread_mutex_t  g_connectdeviceprocmutex              = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_deactivateDeviceprocmutex           = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_GetAcceptedAmountprocmutex 		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_deviceStatusprocmutex 			  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_acceptNotesprocmutex 				  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_StackAcceptedNotesprocmutex 		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_ReturnAcceptedNotesprocmutex 		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_defaultCancelprocmutex 			  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_defaultCommitprocmutex 			  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_disableAlarmprocmutex 			  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_getDoorstatusprocmutex 		      = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_getUpsStatusprocmutex 	          = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_getBatteryStatusprocmutex 	      = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_printerStatusprocmutex 			  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_CashBoxmutex 						  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t   g_CashBoxEnableThreadCond 			  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t  g_EnableTheseDenominationsprocmutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_IsNoteRemovedprocmutex      		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_GetValidNoteprocmutex       		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_AcceptCurrentNoteprocmutex  		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_ReturnCurrentNoteprocmutex  		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_HoldCurrentNoteprocmutex    		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_getTimeprocmutex                    = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_getDenominationprocmutex            = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_ActivateComPortprocmutex            = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_DeActivateComPortprocmutex          = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_SetupCurrencyLogFileprocmutex       = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_EnableSingleCurrencyFlagprocmutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_DisableSingleCurrencyFlagprocmutex  = PTHREAD_MUTEX_INITIALIZER;
static bool isCurrencyLogFileSet=false;
static bool g_EnableSingleCurrencyFlag=false;

static pthread_mutex_t  g_ClearJammedCurrenciesAcceptprocmutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_ClearJammedCurrenciesRejectprocmutex  = PTHREAD_MUTEX_INITIALIZER;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupCurrencyLogFile(  JNIEnv  *env, jclass  jobj,jstring  jniLogfile,jint jniLogLevel, jstring jniLogdllPath) {

               pthread_mutex_lock( &g_SetupCurrencyLogFileprocmutex);
              
               if( false == isCurrencyLogFileSet  ) {
				   
				   g_Ascrm_writeFileLog=NULL;
				   g_lib_handle=NULL;

				   //++Step 1:Log dll Path
				   const char *LogdllPathstr= (*env)->GetStringUTFChars(env,jniLogdllPath,0);
				   //printf("\n[Java_Cris_Currency_SetupCurrencyLogFile()] Log dll Path : %s", LogdllPathstr);
				   SetSoFilePath((unsigned char*)LogdllPathstr);
				   (*env)->ReleaseStringUTFChars(env, jniLogdllPath, LogdllPathstr);

				   //++Step 2:Log File Name
				   const char *LogFileName= (*env)->GetStringUTFChars(env,jniLogfile,0);
				   //printf("\n[Java_Cris_Currency_SetupCurrencyLogFile()] Log File Name : %s", LogFileName);
				   SetGeneralFileLogPath( (unsigned char*) LogFileName );
				   (*env)->ReleaseStringUTFChars(env, jniLogfile, LogFileName);

				   //++Step 3: Set LogLevel Mode
				   g_LogModeLevel = (int)jniLogLevel;
				   //++printf("\n[Java_Cris_Currency_SetupCurrencyLogFile()] Log File Mode : %d", g_LogModeLevel);

				   pthread_mutex_unlock( &g_SetupCurrencyLogFileprocmutex);
				   
				   return true;
				   
               } else {
				   pthread_mutex_unlock( &g_SetupCurrencyLogFileprocmutex);
				   return true;
			   }//else end
               


}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupCurrencyLogFile END

JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupNoteAcceptorID(JNIEnv  *env, jclass  jobj,jstring jniDeviceId){
		       
		       //Step : Get Device id
		       unsigned char fnDeviceid[200];
		       memset( fnDeviceid,'\0',200);
               const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);
               //printf("\n[Java_Cris_Currency_SetupNoteAcceptorID()] Device Id : %s", deviceid);
               strcpy(fnDeviceid,deviceid);
               SetBNADeviceId( fnDeviceid );

}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupNoteAcceptorID end

JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupCoinAcceptorID(JNIEnv  *env, jclass  jobj,jstring jniDeviceId){
		       
		       //Step : Get Device id
		       unsigned char fnDeviceid[200];
		       memset( fnDeviceid,'\0',200);
               const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);
               //printf("\n[Java_Cris_Currency_SetupNoteAcceptorID()] Device Id : %s", deviceid);
               strcpy(fnDeviceid,deviceid);
               SetBCADeviceId( fnDeviceid );

}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupCoinAcceptorID end

JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupCoinEscrowID(JNIEnv  *env, jclass  jobj,jstring jniDeviceId){
		       
		       //Step : Get Device id
		       unsigned char fnDeviceid[200];
		       memset( fnDeviceid,'\0',200);
               const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);
               //printf("\n[Java_Cris_Currency_SetupCoinEscrowID()] Device Id : %s", deviceid);
               strcpy(fnDeviceid,deviceid);
               SetBCEDeviceId( fnDeviceid );

}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupCoinEscrowID end

JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupKioskSecurityID(JNIEnv  *env, jclass  jobj,jstring jniDeviceId){
		       
		       //Step : Get Device id
		       unsigned char fnDeviceid[200];
		       memset( fnDeviceid,'\0',200);
               const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);
               //printf("\n[Java_Cris_Currency_SetupKioskSecurityID()] Device Id : %s", deviceid);
               strcpy(fnDeviceid,deviceid);
               SetKSDDeviceId( fnDeviceid );

}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupKioskSecurityID end
 
JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupUPSID(JNIEnv  *env, jclass  jobj,jstring jniDeviceId){
		       
		       //Step : Get Device id
		       unsigned char fnDeviceid[200];
		       memset( fnDeviceid,'\0',200);
               const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);
               //printf("\n[Java_Cris_Currency_SetupUPSID()] Device Id : %s", deviceid);
               strcpy(fnDeviceid,deviceid);
            
}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupUPSID end

JNIEXPORT jboolean JNICALL Java_Cris_Currency_EnableSingleCurrencyFlag(JNIEnv  *env, jclass  jobj){
           pthread_mutex_lock( &g_EnableSingleCurrencyFlagprocmutex);
           g_EnableSingleCurrencyFlag=true;
           pthread_mutex_unlock( &g_EnableSingleCurrencyFlagprocmutex);
           return true;
}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_EnableSingleCurrencyFlag end

JNIEXPORT jboolean JNICALL Java_Cris_Currency_DisableSingleCurrencyFlag(JNIEnv  *env, jclass  jobj){
           pthread_mutex_lock( &g_DisableSingleCurrencyFlagprocmutex);
           g_EnableSingleCurrencyFlag=false;
           pthread_mutex_unlock( &g_DisableSingleCurrencyFlagprocmutex);
           return true;
}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_EnableSingleCurrencyFlag end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupNativeLogFile ( JNIEnv   *env, jclass   jobj, jstring  jniLogfile, jstring  jniLogdllPath) {

     
               ///////////////////////////////////////////////////////////////////////////////////////////////////////
               /*
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

	           */
               ///////////////////////////////////////////////////////////////////////////////////////////////////

               return true;
               

}//JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupNoteAcceptorLogFile END

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_Cris_Currency_WriteNativeLog( JNIEnv *env, jobject jobj, jstring jniLogMessage) {

       
           const char *LogMessage= (*env)->GetStringUTFChars(env,jniLogMessage,0);

	       //printf("\n [Java_Cris_Currency_WriteNativeLog()] Log File Name : %s", LogMessage);

	       //++writeFileLog((char*)LogMessage);

	       (*env)->ReleaseStringUTFChars(env, jniLogMessage, LogMessage);





}//JNIEXPORT void JNICALL Java_Cris_Currency_WriteNativeLog end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jlongArray JNICALL Java_Cris_Currency_getTime(JNIEnv *env, jobject jobj){
            
            pthread_mutex_lock( &g_getTimeprocmutex);
            
            struct timespec currentTime;
            long long int TimeArray[2];
            TimeArray[0] = 0;
            TimeArray[1] = 0;
            jlongArray rtArray  = (*env)->NewLongArray(env,2);
            int rtcode = clock_gettime(CLOCK_REALTIME, &currentTime);
            TimeArray[0] = currentTime.tv_sec;
            TimeArray[1] = (long long int)currentTime.tv_nsec;
            //printf("\n[JNICALL Java_Cris_Currency_getTime()] rtcode: %d\n",rtcode);
            //printf("\n[JNICALL Java_Cris_Currency_getTime()] CLOCK_REALTIME SECOND: %d\n", currentTime.tv_sec); 
            //printf("\n[JNICALL Java_Cris_Currency_getTime()] CLOCK_REALTIME NANOSECOND: %ld\n", currentTime.tv_nsec); 
            (*env)->SetLongArrayRegion(env,rtArray,0,2,TimeArray);
            
            pthread_mutex_unlock( &g_getTimeprocmutex);
            
            return rtArray;
            

}//JNIEXPORT jintArray JNICALL Java_Cris_Currency_getTime(JNIEnv *env, jobject jobj) end

static pthread_mutex_t  g_getCoinFirmwareVersionprocmutex  = PTHREAD_MUTEX_INITIALIZER;

JNIEXPORT jstring JNICALL Java_Cris_CashApi_getCoinFirmwareVersion(JNIEnv *env, jobject jobj){
				
				  pthread_mutex_lock( &g_getCoinFirmwareVersionprocmutex );
                  pthread_mutex_lock( &g_activatedevicemutex );
                  pthread_mutex_lock( &g_ComPortAccessmutex );
                  
	              pthread_mutex_unlock( &g_activatedevicemutex );
                  pthread_mutex_unlock( &g_ComPortAccessmutex );
                  pthread_mutex_unlock( &g_getCoinFirmwareVersionprocmutex ); 

                  return NULL;
}//JNIEXPORT jstring JNICALL Java_Cris_CashApi_getCoinFirmwareVersion end

static pthread_mutex_t  g_getCashFirmwareVersionprocmutex  = PTHREAD_MUTEX_INITIALIZER;

JNIEXPORT jstring JNICALL Java_Cris_CashApi_getCashFirmwareVersion(JNIEnv *env, jobject jobj){
 
                  pthread_mutex_lock( &g_getCashFirmwareVersionprocmutex );
                  pthread_mutex_lock( &g_activatedevicemutex );
                  pthread_mutex_lock( &g_ComPortAccessmutex );
                  
                  jstring rtstr;
                  int index=0;
                  unsigned char str[20];
                  memset(str,'\0',20);
                  unsigned char log[100];
                  memset(log,'\0',100);
                  int Romversion=0;
                  unsigned char Response[31];
                  int rtcode=-1;
                  memset(Response,'\0',31);
                  rtcode = IssueGetRomVersion(Response);
                  WriteFormattedDataExchange(Response,"Rx",31 );
                  if( (1 == rtcode) && ( 0x70 == Response[2] ) && ( 0x02 == Response[3] ) ){

                        
                        if( 0x10 == Response[29] ){
                            index = 25;
                        }else if( 0x03 == Response[29] ){
                            index = 24;         
                        }

                        memset(log,'\0',100);
                        sprintf(log,"[Java_Cris_CashApi_getCashFirmwareVersion()] RomVersion HighDigit:%c",Response[index] ); 
                        //writeFileLog(log);
                        AVRM_Currency_writeFileLog(log,INFO);
                        
                        memset(log,'\0',100);
                        sprintf(log,"[Java_Cris_CashApi_getCashFirmwareVersion()] RomVersion LowDigit:%c",Response[index+1] ); 
                        //writeFileLog(log);
                        AVRM_Currency_writeFileLog(log,INFO);

                        Romversion = ( GetAsciiCode (Response[index])*10) + GetAsciiCode (Response[index+1] );

                       
                        memset(log,'\0',100);
                        sprintf(log,"[Java_Cris_CashApi_getCashFirmwareVersion()] RomVersion :%d",Romversion ); 
                        //writeFileLog(log);
                        AVRM_Currency_writeFileLog(log,INFO);
                        
                        //convert romversion to string 
                        sprintf(str, "JCM,41.%d", Romversion);
                        
                        //print our string
                        //printf("\n[Java_Cris_CashApi_getCashFirmwareVersion()] RomVersion: %s\n", str);
                       


                  }else{
                        //writeFileLog("[Java_Cris_CashApi_getCashFirmwareVersion()] No Valid Data Return ");
                        AVRM_Currency_writeFileLog("[Java_Cris_CashApi_getCashFirmwareVersion()] No Valid Data Return ",INFO);
                        //convert romversion to string 
                        sprintf(str, "JCM,41.%d", Romversion);

                  }
                  
                  pthread_mutex_unlock( &g_activatedevicemutex );
                  pthread_mutex_unlock( &g_ComPortAccessmutex );
                  pthread_mutex_unlock( &g_getCashFirmwareVersionprocmutex ); 

                  return (*env)->NewStringUTF(env, str);
                  
                  

}//Java_Cris_CashApi_getCashFirmwareVersion() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++[Note And Coin Acceptor API List]
//++ Currency Device Flag: 0 

//++Activate Device
JNIEXPORT jint JNICALL Java_Cris_Currency_JniConnectDevice(  JNIEnv *env, jobject jobj, jint PortId,jint DeviceType,jint EscrowClearanceMode, jint Timeout) {


							
							//writeFileLog("[Java_Cris_Currency_ConnectDevice()] Entry.");
							
							AVRM_Currency_writeFileLog("[Java_Cris_Currency_ConnectDevice()] Entry.",TRACE);

							pthread_mutex_lock( &g_connectdeviceprocmutex );

							pthread_mutex_lock( &g_activatedevicemutex );

							pthread_mutex_lock( &g_ComPortAccessmutex );

							jint JniRtCode=-1;
							
							unsigned char log[100];
							memset(log,'\0',100);
						 
							#ifdef CRISAPI_TIMEOUT_TEST
							struct timespec begints, endts,diffts;
							unsigned char log[100];
							clock_gettime(CLOCK_MONOTONIC, &begints);
							#endif
							
							
							memset(log,'\0',100);
		                    sprintf(log,"[Java_Cris_Currency_ConnectDevice()] DeviceType: %d.",DeviceType);
                            //writeFileLog(log);
                            AVRM_Currency_writeFileLog(log,DEBUG);
							
							memset(log,'\0',100);
		                    sprintf(log,"[Java_Cris_Currency_ConnectDevice()] PortID: %d.",PortId);
                            //writeFileLog(log);
                            AVRM_Currency_writeFileLog(log,DEBUG);


							if( false == g_activate_device_flag ){

									
									JniRtCode=(jint)activateDevice2(PortId,DeviceType,EscrowClearanceMode,Timeout);

									if( ( 0 == JniRtCode ) || ( 1 == JniRtCode ) ||  (3 == JniRtCode ) ||  (4 == JniRtCode ) ||  (5 == JniRtCode ) ) {
										
										g_activate_device_flag=true;

									}else {
										g_activate_device_flag=false;
                                    } 
                                        

							}else if( true == g_activate_device_flag ) {
								  
								  JniRtCode=(jint)activateDevice2(PortId,DeviceType,EscrowClearanceMode,Timeout);
								  
								
							}else{
								//writeFileLog("[Java_Cris_Currency_ConnectDevice()] Already device activated so no need to activate again.");
								AVRM_Currency_writeFileLog("[Java_Cris_Currency_ConnectDevice()] Already device activated so no need to activate again.",INFO);
							}//else end


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

							//writeFileLog("[Java_Cris_Currency_ConnectDevice()] Exit.");
							
							AVRM_Currency_writeFileLog("[Java_Cris_Currency_ConnectDevice()] Exit.",TRACE);

							return JniRtCode;




}//Java_Cris_CashApi_activateDevice() end

//++Disconnect Device
JNIEXPORT jint JNICALL Java_Cris_Currency_JniDisConnectDevice(JNIEnv *env, jobject jobj,jint DeviceType,jint timeout) {

             
             //writeFileLog("[Java_Cris_Currency_DisConnectDevice()] Entry.");
             
             AVRM_Currency_writeFileLog("[Java_Cris_Currency_DisConnectDevice()] Entry.",TRACE);

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

			 if( true == g_activate_device_flag ){

				 rtcode =(jint)deactivateDevice(DeviceType);
                 g_activate_device_flag=false;
				 
			 } else if( false == g_activate_device_flag ){

				 rtcode =(jint)deactivateDevice(DeviceType);
                
			 }

             ////////////////////////////////////////////////////////////////////////////////////////

             memset(log,'\0',100);
             sprintf(log,"[Java_Cris_Currency_DisConnectDevice()]  deactivateDevice return code : %d.", rtcode); 
             //writeFileLog(log);
	         AVRM_Currency_writeFileLog(log,INFO);
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

             AVRM_Currency_writeFileLog("[Java_Cris_Currency_DisConnectDevice()] Exit.",TRACE);
             
             AVRM_Currency_writeFileLog("[Java_Cris_Currency_DisConnectDevice()] Exit.",TRACE);

	         return rtcode;


}//Java_Cris_Currency_DisConnectDevice() end

//++Device Status
#define 	CURENNCY_DEVICE_ARRAY_SIZE 		12
JNIEXPORT jbyteArray JNICALL Java_Cris_Currency_JniDeviceStatus(JNIEnv *env, jobject jobj, jint DeviceType,jint Timeout) {
	
            
            //writeFileLog("[Java_Cris_Currency_DeviceStatus()] Entry.");
            AVRM_Currency_writeFileLog("[Java_Cris_Currency_DeviceStatus()] Entry.",TRACE);
            
            pthread_mutex_lock( &g_deviceStatusprocmutex );
            pthread_mutex_lock( &g_activatedevicemutex );
            pthread_mutex_lock( &g_ComPortAccessmutex );

            jbyteArray rtcode= NULL;

            #ifdef CRISAPI_TIMEOUT_TEST
            struct timespec begints, endts,diffts;
            unsigned char log[100];
            clock_gettime(CLOCK_MONOTONIC, &begints);
            #endif

			if( true == g_activate_device_flag){
				
				char tempArray[CURENNCY_DEVICE_ARRAY_SIZE];
				memset( tempArray,0x00,12);	   
			    deviceStatusV3(tempArray,DeviceType,Timeout);
			    
			    //++Create Blank byte Array
			    rtcode=(*env)->NewByteArray(env, CURENNCY_DEVICE_ARRAY_SIZE);
			    
			    //++GET THE "EMPTY" ELEMENTS OF MY JAVA BYTE ARRAY
                jbyte *bytes = (*env)->GetByteArrayElements( env,rtcode, 0);
                
                //++ADD ALL THE ELEMENTS
                for(int counter=0;counter<CURENNCY_DEVICE_ARRAY_SIZE;counter++){
			          bytes[counter] = (jbyte)tempArray[counter];
			    }//for end
			    
                //++ADD THE BYTES COLLECTED TO THE JAVA BYTE ARRAY
				(*env)->SetByteArrayRegion(env,rtcode, 0, CURENNCY_DEVICE_ARRAY_SIZE, bytes );
				
				//++NEED TO RELEASE THE BYTE ARRAY
				(*env)->ReleaseByteArrayElements( env,rtcode, bytes, 0 );
				
				//writeFileLog("[Java_Cris_Currency_DeviceStatus()] Successfully return device status.");
				AVRM_Currency_writeFileLog("[Java_Cris_Currency_DeviceStatus()] Successfully return device status.",INFO);
			    
			}else if( false  == g_activate_device_flag) {
				AVRM_Currency_writeFileLog("[Java_Cris_Currency_DeviceStatus()] DEVICE IS NOT ACTIVATED",TRACE);
				rtcode = NULL;
            }//++else end
			
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

            AVRM_Currency_writeFileLog("[Java_Cris_Currency_DeviceStatus()] Exit.",TRACE);
            return rtcode;

}//Java_Cris_Currency_DeviceStatus() end

/*
JNIEXPORT jint JNICALL Java_Cris_Currency_JniDeviceStatus(JNIEnv *env, jobject jobj, jint DeviceType,jint Timeout) {



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

			if( true == g_activate_device_flag){
					   
				rtcode=(jint)deviceStatusV2(DeviceType,Timeout);
				
			}else if( false  == g_activate_device_flag) {
				
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
*/ 

JNIEXPORT jint JNICALL Java_Cris_Currency_JnidefaultCommit(JNIEnv *env, jobject jobj,jint timeout) {

            //writeFileLog("[Java_Cris_Currency_defaultCommit()] Entry.");

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
		     //writeFileLog("[Java_Cris_Currency_defaultCommit()] Communication failure.");
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

            //writeFileLog("[Java_Cris_Currency_defaultCommit()] Exit.");

	    return rtcode;


}//Java_Cris_Currency_defaultCommit() end

JNIEXPORT jint JNICALL Java_Cris_Currency_JnidefaultCancel(JNIEnv *env, jobject jobj,jint timeout) {

            //writeFileLog("[Java_Cris_Currency_defaultCancel()] Entry.");

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
		     //writeFileLog("[Java_Cris_Currency_defaultCancel()] Communication failure.");
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

            //writeFileLog("[Java_Cris_Currency_defaultCancel()] Exit.");

	    return rtcode;



}//Java_Cris_Currency_defaultCancel() end

JNIEXPORT jint JNICALL Java_Cris_Currency_JniEnableTheseDenominations(JNIEnv *env, jobject jobj, jint CurrencyType,jint DenomMask, jint Timeout ) {
	
         
         //writeFileLog("[Java_Cris_Currency_EnableTheseDenominatio()] Entry.");
         AVRM_Currency_writeFileLog("[Java_Cris_Currency_EnableTheseDenominatio()] Entry.",TRACE);

         pthread_mutex_lock( &g_EnableTheseDenominationsprocmutex );

         int rtcode =-1;

         rtcode = EnableTheseDenomination( CurrencyType,DenomMask,Timeout) ;

         pthread_mutex_unlock( &g_EnableTheseDenominationsprocmutex );

         //writeFileLog("[Java_Cris_Currency_EnableTheseDenominatio()] Exit.");
         AVRM_Currency_writeFileLog("[Java_Cris_Currency_EnableTheseDenominatio()] Exit.",TRACE);

         return rtcode;
         


}//Java_Cris_Currency_EnableTheseDenominations() end

JNIEXPORT jint JNICALL Java_Cris_Currency_ClearJammedCurrenciesAccept(JNIEnv *env, jobject jobj, jint fnTimeout, jint fnDeviceType){
	
	        pthread_mutex_lock( &g_ClearJammedCurrenciesRejectprocmutex );
	        pthread_mutex_lock( &g_activatedevicemutex );
	        pthread_mutex_lock( &g_ComPortAccessmutex );
            
            int rtcode = 0; //Default Success
            
            rtcode = defaultCommit2(  fnTimeout, fnDeviceType );
	
	        pthread_mutex_unlock( &g_ClearJammedCurrenciesRejectprocmutex );
	        pthread_mutex_unlock( &g_activatedevicemutex );
	        pthread_mutex_unlock( &g_ComPortAccessmutex );
	        
	        return rtcode;
	

}//JNIEXPORT jint JNICALL Java_Cris_Currency_ClearJammedCurrenciesAccept end

JNIEXPORT jint JNICALL Java_Cris_Currency_ClearJammedCurrenciesReject(JNIEnv *env, jobject jobj, jint fnTimeout, jint fnDeviceType){
	      
	        pthread_mutex_lock( &g_ClearJammedCurrenciesAcceptprocmutex );
	        pthread_mutex_lock( &g_activatedevicemutex );
	        pthread_mutex_lock( &g_ComPortAccessmutex );
            
            int rtcode = 0; //Default Success
            
            rtcode = defaultCancel2(  fnTimeout, fnDeviceType );
	
	        pthread_mutex_unlock( &g_ClearJammedCurrenciesAcceptprocmutex );
	        pthread_mutex_unlock( &g_activatedevicemutex );
	        pthread_mutex_unlock( &g_ComPortAccessmutex );
	        
	        return rtcode;
	


}//Java_Cris_Currency_ClearJammedCurrenciesReject end

//++Start accept fare Multi Notes&Coins
JNIEXPORT jboolean JNICALL Java_Cris_Currency_JniAcceptCurrencies(JNIEnv *env, jobject jobj,jint CurrencyType , jint jFare, jint Timeout ) {
	

                     jint jmaxNoOfCash =20;

                     jint jmaxNoOfCoin =20;

                     //writeFileLog("[Java_Cris_Currency_JniAcceptCurrencies()] Entry.");
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniAcceptCurrencies()] Entry.",TRACE);

                     pthread_mutex_lock( &g_acceptNotesprocmutex );

		             pthread_mutex_lock( &g_activatedevicemutex );

                     pthread_mutex_lock( &g_ComPortAccessmutex );

                     jboolean rtcode=false;
               
                     #ifdef CRISAPI_TIMEOUT_TEST
                     struct timespec begints, endts,diffts;
                     unsigned char log[100];
                     clock_gettime(CLOCK_MONOTONIC, &begints);
                     #endif

					 if( true == g_activate_device_flag){
                        
                        if( 0 == CurrencyType) { //++Both 
							if( true == g_EnableSingleCurrencyFlag ){
								
								jmaxNoOfCash = 1;
								jmaxNoOfCoin = 1;
						    }//if end
							rtcode = (jboolean)acceptFare(jFare,jmaxNoOfCash,jmaxNoOfCoin,Timeout);
							g_EnableSingleCurrencyFlag = false; //Set to default flag value
							
						} else if( 1 == CurrencyType) {//++Only Notes
							
							if( true == g_EnableSingleCurrencyFlag ){
								jmaxNoOfCash = 1;
						    }//if end
							rtcode = (jboolean)acceptFare(jFare,jmaxNoOfCash,0,Timeout);
							g_EnableSingleCurrencyFlag = false; //Set to default flag value
							
						}else if( 2 == CurrencyType) {//++Only Coins
							
							if( true == g_EnableSingleCurrencyFlag ){
								jmaxNoOfCoin = 1;
						    }//if end
							rtcode = (jboolean)acceptFare(jFare,0,jmaxNoOfCoin,Timeout);
							g_EnableSingleCurrencyFlag = false; //Set to default flag value
							
						}//else if end
                        
					 }else if( false  == g_activate_device_flag ){
						rtcode =  false;
					 }

                     #ifdef CRISAPI_TIMEOUT_TEST
                     clock_gettime(CLOCK_MONOTONIC, &endts);
                     diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                     memset(log,'\0',100);
                     sprintf(log,"[Java_Cris_Currency_JniAcceptCurrencies()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
                     writeAPITimeoutLog(log);
                     #endif

                     pthread_mutex_unlock( &g_ComPortAccessmutex );

		             pthread_mutex_unlock( &g_activatedevicemutex );

                     pthread_mutex_unlock( &g_acceptNotesprocmutex );
		 
                     //writeFileLog("[Java_Cris_Currency_JniAcceptCurrencies()] Exit.");
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniAcceptCurrencies()] Exit.",TRACE);

		             return rtcode;



}//Java_Cris_Currency_JniAcceptCurrencies() end

//++Commit Multi Notes&Coins
JNIEXPORT jint JNICALL Java_Cris_Currency_JniStackAcceptedCurrencies( JNIEnv *env, jobject jobj,jint Timeout ) {
		     

                     //writeFileLog("[Java_Cris_Currency_JniStackAcceptedCurrencies()] Entry.");
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniStackAcceptedCurrencies()] Entry.",TRACE);
                     
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

					 if( true == g_activate_device_flag){
						 rtcode= commitTranInterface(Timeout);
					 }
					 else if( false  == g_activate_device_flag ){
						  //writeFileLog("[Java_Cris_Currency_JniStackAcceptedCurrencies()] Communication failure.");
						  AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniStackAcceptedCurrencies()] Communication failure.",ERROR);
						  rtcode = -1; //Communication failure
					 }//else

                     /////////////////////////////////////////////////////////////////////////////////////
		    
                     #ifdef CRISAPI_TIMEOUT_TEST
                     clock_gettime(CLOCK_MONOTONIC, &endts);
                     diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                     memset(log,'\0',100);
                     sprintf(log,"[Java_Cris_Currency_JniStackAcceptedCurrencies()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
                     writeAPITimeoutLog(log);
                     #endif


                     /////////////////////////////////////////////////////////////////////////////////

                     memset(log,'\0',100);

                     sprintf(log,"[Java_Cris_Currency_JniStackAcceptedCurrencies()] Return Code : %d .",rtcode);

                     //writeFileLog(log);
                     
                     AVRM_Currency_writeFileLog(log,INFO);

                     //////////////////////////////////////////////////////////////////////////////////

                     pthread_mutex_unlock( &g_ComPortAccessmutex );

		             pthread_mutex_unlock( &g_activatedevicemutex );

                     pthread_mutex_unlock( &g_StackAcceptedNotesprocmutex );

                     //writeFileLog("[Java_Cris_Currency_JniStackAcceptedCurrencies()] Exit.");
                     
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniStackAcceptedCurrencies()] Exit.",TRACE);

		             return rtcode;

  
}//Java_Cris_Currency_JniStackAcceptedCurrencies() end 

//++Return Multi Notes&Coins
JNIEXPORT jint JNICALL Java_Cris_Currency_JniReturnAcceptedCurrencies(JNIEnv *env, jobject jobj, jint Timeout ) {
 
             //writeFileLog("[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Entry.");
             AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Entry.",TRACE);

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

			 if( true == g_activate_device_flag){
				  rtcode = cancelTranInterface(Timeout); 
			 }else if( false  == g_activate_device_flag) {
                  //writeFileLog("[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Communication failure.");
                  AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Communication failure.",ERROR);
		          rtcode = -1; //Communication failure
             }//else


            memset(log,'\0',100);

            sprintf(log,"[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Return Code : %d .",rtcode);

            //writeFileLog(log);
            
            AVRM_Currency_writeFileLog(log,INFO);
	     
            #ifdef CRISAPI_TIMEOUT_TEST
            clock_gettime(CLOCK_MONOTONIC, &endts);
            diffts.tv_sec = endts.tv_sec - begints.tv_sec;
            memset(log,'\0',100);
            sprintf(log,"[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
            writeAPITimeoutLog(log);
            #endif

            pthread_mutex_unlock( &g_ComPortAccessmutex );

	        pthread_mutex_unlock( &g_activatedevicemutex );

            pthread_mutex_unlock( &g_ReturnAcceptedNotesprocmutex );

            //writeFileLog("[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Exit.");
            AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniReturnAcceptedCurrencies()] Exit.",TRACE);

	        return rtcode;
 
}//Java_Cris_Currency_JniReturnAcceptedCurrencies() end 

//++Get Denomoniation
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetAcceptedAmount( JNIEnv *env, jobject jobj, jobjectArray array ) {

                  
                  //++writeFileLog("[Java_Cris_Currency_GetAcceptedAmount()] Entry.");

                  pthread_mutex_lock( &g_GetAcceptedAmountprocmutex );

                  pthread_mutex_lock( &g_activatedevicemutex );

                  #ifdef CRISAPI_TIMEOUT_TEST
                  //++struct timespec begints, endts,diffts;
                  //++clock_gettime(CLOCK_MONOTONIC, &begints);
                  #endif
                  int counter=0;
                  unsigned char log[100];
                  memset(log,'\0',100);
                  
                  if( true == g_activate_device_flag ) {

                       pthread_mutex_unlock( &g_activatedevicemutex );
                       jint rtcode=-1;
					   jint JniRtcode=0;

					   //rtcode=(*env)->EnsureLocalCapacity(env,20);
					   //if (0!=rtcode) {
								  
								    //writeFileLog("[Java_Cris_Currency_GetAcceptedAmount()] Given number of local references cannot be created.");
									//pthread_mutex_unlock( &g_GetAcceptedAmountprocmutex );
									//writeFileLog("[Java_Cris_CashApi_getDenomination()] Exit.");
                                    //#ifdef CRISAPI_TIMEOUT_TEST
							        //clock_gettime(CLOCK_MONOTONIC, &endts);
						            //diffts.tv_sec = endts.tv_sec - begints.tv_sec;
									//memset(log,'\0',100);
							        //sprintf(log,"[Java_Cris_Currency_GetAcceptedAmount()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
							        //writeAPITimeoutLog(log);
						            //#endif
									//return 0; 

					   //}
                       
                       //++Get Java Array Row Size
                       int AcceptedAmount=0;
					   int ArrayColSize=MAX_DENOM_NUMBER; //#define MAX_DENOM_NUMBER  40
					   int ArrayRowSize=2;
					   int Carray[ArrayRowSize][ArrayColSize]; 
					   int i=0,j=0;
					   //fill with zeros  
					   for (i = 0; i <ArrayRowSize; i++) {
                         for (j = 0; j <ArrayColSize; j++) {
							 Carray[i][j]=0;
                         }//++for end  
                       }//++for end
                       
                       //++Call C function
                       JniRtcode=(jint)getDenomination(Carray);
		       
					   //++Copy Local JNI Array to Original JAVA Array  
					   /*    
					   for (i = 0; i <ArrayRowSize; i++) {

						 jbyte tmp[2]={0}; 
						 int j=0;
						 jbyteArray iarr = (*env)->NewByteArray(env, 2);
						 
						 //++out of memory error thrown 
						 if(NULL== iarr) {
											
							      //++writeFileLog("[Java_Cris_CashApi_getDenomination()] Out of memory."); 
								  pthread_mutex_unlock( &g_getDenominationprocmutex );
								  //writeFileLog("[Java_Cris_CashApi_getDenomination()] Exit.");
								  #ifdef CRISAPI_TIMEOUT_TEST
								  //clock_gettime(CLOCK_MONOTONIC, &endts);
								  //diffts.tv_sec = endts.tv_sec - begints.tv_sec;
								  //memset(log,'\0',100);
								  //sprintf(log,"[Java_Cris_CashApi_getDenomination()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
								  //writeAPITimeoutLog(log);
								  #endif
							      return 0; 
							      
                         }//++if end
                         
						 for (j = 0; j < 2; j++) {
							 tmp[j] =(jbyte) Carray[i][j];
						 }//++for end
						 
						 (*env)->SetByteArrayRegion(env, iarr, 0, 2, tmp);
                         (*env)->SetObjectArrayElement(env, array, i, iarr);
                         (*env)->DeleteLocalRef(env, iarr);
					 
					   }//++for end
					   */
					   
					   for (i = 0; i <ArrayRowSize; i++) {

						 jbyte tmp[MAX_DENOM_NUMBER]={0}; 
						 int j=0;
						 jbyteArray iarr = (*env)->NewByteArray(env, ArrayColSize);
						 
						 //++out of memory error thrown 
						 if(NULL== iarr) {
							 //++writeFileLog("[Java_Cris_CashApi_getDenomination()] Out of memory."); 
						     return 0; 
						 }//++if end
                         
						 for (j = 0; j < ArrayColSize; j++) {
							 tmp[j] =(jbyte) Carray[i][j];
						     AcceptedAmount =  AcceptedAmount+(int)tmp[j];
						 }//++for end
						 
						 (*env)->SetByteArrayRegion(env, iarr, 0, ArrayColSize, tmp);
                         (*env)->SetObjectArrayElement(env, array, i, iarr);
                         (*env)->DeleteLocalRef(env, iarr);
					 
					   }//++for end
					   
					   if( AcceptedAmount > 0 ){
						  memset(log,'\0',100);
						  sprintf(log,"[Java_Cris_Currency_GetAcceptedAmount()] Accepted Ammount: Rs. %d .",AcceptedAmount); 
						  //AVRM_Currency_writeFileLog(log,INFO);
					   }//if end
		 
                       pthread_mutex_unlock( &g_GetAcceptedAmountprocmutex );

                       //++writeFileLog("[Java_Cris_Currency_GetAcceptedAmount()] Exit.");

                       return JniRtcode;

              }//if end
             
              pthread_mutex_unlock( &g_activatedevicemutex );
             
              pthread_mutex_lock( &g_activatedevicemutex );

              if( false  == g_activate_device_flag) {

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

            
}//Java_Cris_CashApi_getDenomination() end

//++Is Note Removed
JNIEXPORT jint JNICALL Java_Cris_Currency_JniIsNoteRemoved(JNIEnv *env, jobject jobj, jint timeout) {

         //writeFileLog("[Java_Cris_Currency_IsNoteRemoved()] Entry.");
         AVRM_Currency_writeFileLog("[Java_Cris_Currency_IsNoteRemoved()] Entry.",TRACE);

         pthread_mutex_lock( &g_IsNoteRemovedprocmutex );

         int rtcode =-1;
         
         rtcode = IsNoteRemoved(timeout);

         pthread_mutex_unlock( &g_IsNoteRemovedprocmutex );

         //writeFileLog("[Java_Cris_Currency_IsNoteRemoved()] Exit.");
         AVRM_Currency_writeFileLog("[Java_Cris_Currency_IsNoteRemoved()] Exit.",TRACE);
         
         return rtcode;
         
}//Java_Cris_Currency_IsNoteRemoved() end

//++Single Note Acceptance API
//++CurrencyType: 1: Currency Note Accepter 2: Currency Coin Accepter
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetValidCurrency(JNIEnv *env, jobject jobj, jint CurrencyType,jint Denom, jint Timeout) {
                        
           if( 1 == CurrencyType) //++For Note Device
           { 
			     
					 #ifdef JCM_NOTE_ACCEPTOR
					 
                     //writeFileLog("[Java_Cris_Currency_GetValidCurrency()] Entry.");
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_GetValidCurrency()] Entry.",TRACE);

                     pthread_mutex_lock( &g_GetValidNoteprocmutex );

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
						
						if( 1 == CurrencyType ){//++1: Currency Note Accepter
							rtcode = (jboolean)acceptFare(Denom,1,0,Timeout);
						} else if( 2 == CurrencyType ){ //++2: Currency Coin Accepter
							rtcode = (jboolean)acceptFare(Denom,0,1,Timeout);
						}//else if
						
					 }
					 else if( false  == g_activate_device_flag )
					 {
						rtcode =  false;
					 }//else

                     #ifdef CRISAPI_TIMEOUT_TEST
                     clock_gettime(CLOCK_MONOTONIC, &endts);
                     diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                     memset(log,'\0',100);
                     sprintf(log,"[Java_Cris_Currency_GetValidCurrency()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
                     writeAPITimeoutLog(log);
                     #endif

                     pthread_mutex_unlock( &g_ComPortAccessmutex );

		             pthread_mutex_unlock( &g_activatedevicemutex );

                     pthread_mutex_unlock( &g_GetValidNoteprocmutex );
		 
                     //writeFileLog("[Java_Cris_Currency_GetValidCurrency()] Exit.");
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_GetValidCurrency()] Exit.",TRACE);

		             return rtcode;
		             
		             #endif

                     #ifdef B2B_NOTE_ACCEPTOR 
                     
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

					 #ifdef B2B_NOTE_ACCEPTOR

					 if( true == g_activate_device_flag)
					 {
						 rtcode= GetValidNote(Denom,Timeout); 

					 }
					 else if( false  == g_activate_device_flag)
					 {
						
						 rtcode = 28;//Communication failure

					 }

					 #endif

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
					 
					 #endif
					 
	       }
	       else
	       {
				AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetValidCurrency()] Coin Currency Type not included",INFO);
				return 31; //Other Error
		   }//++else end				 

}//Java_Cris_Currency_GetValidCurrency end

//++CurrencyType: 1: Currency Note Accepter 2: Currency Coin Accepter
JNIEXPORT jint JNICALL Java_Cris_Currency_JniAcceptCurrentCurrency(JNIEnv *env, jobject jobj, jint CurrencyType,jint Denom, jint Timeout){

			if( 1 == CurrencyType) { //++For Note Device
				
                     #ifdef JCM_NOTE_ACCEPTOR 
                     
                     //writeFileLog("[Java_Cris_Currency_AcceptCurrentCurrency()] Entry.");
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_AcceptCurrentCurrency()] Entry.",TRACE);
                     
                     pthread_mutex_lock( &g_AcceptCurrentNoteprocmutex );

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
					 }else if( false  == g_activate_device_flag ){
							  //writeFileLog("[Java_Cris_Currency_AcceptCurrentCurrency()] Communication failure.");
							  AVRM_Currency_writeFileLog("[Java_Cris_Currency_AcceptCurrentCurrency()] Communication failure.",ERROR);
							  rtcode = -1; //Communication failure
					 }//else if

                     #ifdef CRISAPI_TIMEOUT_TEST
                     clock_gettime(CLOCK_MONOTONIC, &endts);
                     diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                     memset(log,'\0',100);
                     sprintf(log,"[Java_Cris_Currency_StackAcceptedNotes()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
                     writeAPITimeoutLog(log);
                     #endif

                     memset(log,'\0',100);

                     sprintf(log,"[Java_Cris_Currency_AcceptCurrentCurrency()] Return Code : %d .",rtcode);

                     //writeFileLog(log);
                     
                     AVRM_Currency_writeFileLog(log,INFO);

                     //////////////////////////////////////////////////////////////////////////////////

                     pthread_mutex_unlock( &g_ComPortAccessmutex );

		             pthread_mutex_unlock( &g_activatedevicemutex );

                     pthread_mutex_unlock( &g_AcceptCurrentNoteprocmutex );

                     //writeFileLog("[Java_Cris_Currency_AcceptCurrentCurrency()] Exit.");
                     
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_AcceptCurrentCurrency()] Exit.",INFO);

		             return rtcode;
		             
		             #endif
		             
					 #ifdef B2B_NOTE_ACCEPTOR 
					 
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

					 #ifdef B2B_NOTE_ACCEPTOR

					 if( true == g_activate_device_flag)
					 {
						 rtcode= AcceptCurrentNote(Denom,Timeout); 

					 }
					 else if( false  == g_activate_device_flag)
					 {
						 rtcode = 28;//Communication failure

					 }
				 
					 #endif

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
					 
					 #endif
			
			}else{
				AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniAcceptCurrentCurrency()] Coin Currency Type not included",INFO);
				return 31; //Other Error
		    }//++else end	

}//Java_Cris_Currency_AcceptCurrentNote end

//++CurrencyType: 1: Currency Note Accepter 2: Currency Coin Accepter
JNIEXPORT jint JNICALL Java_Cris_Currency_JniReturnCurrentCurrency(JNIEnv *env, jobject jobj, jint CurrencyType,jint Timeout) {
				
			if( 1 == CurrencyType) { //++For Note Device
						 
					#ifdef JCM_NOTE_ACCEPTOR 
					  
                     //writeFileLog("[Java_Cris_Currency_ReturnCurrentCurrency()] Entry.");
                     AVRM_Currency_writeFileLog("[Java_Cris_Currency_ReturnCurrentCurrency()] Entry.",TRACE);

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

					 if( true == g_activate_device_flag){
							rtcode = cancelTranInterface(Timeout); 
					 }else if( false  == g_activate_device_flag){
							//writeFileLog("[Java_Cris_Currency_ReturnCurrentCurrency()] Communication failure.");
							AVRM_Currency_writeFileLog("[Java_Cris_Currency_ReturnCurrentCurrency()] Communication failure.",ERROR);
					        rtcode = -1; //Communication failure
					 }

                     memset(log,'\0',100);

					 sprintf(log,"[Java_Cris_Currency_ReturnCurrentCurrency()] Return Code : %d .",rtcode);

					 //writeFileLog(log);
					 
					 AVRM_Currency_writeFileLog(log,INFO);
				 
					#ifdef CRISAPI_TIMEOUT_TEST
					clock_gettime(CLOCK_MONOTONIC, &endts);
					diffts.tv_sec = endts.tv_sec - begints.tv_sec;
					memset(log,'\0',100);
					sprintf(log,"[Java_Cris_Currency_ReturnCurrentCurrency()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
					writeAPITimeoutLog(log);
					#endif

					pthread_mutex_unlock( &g_ComPortAccessmutex );

				    pthread_mutex_unlock( &g_activatedevicemutex );

					pthread_mutex_unlock( &g_ReturnAcceptedNotesprocmutex );

					//writeFileLog("[Java_Cris_Currency_ReturnCurrentCurrency()] Exit.");
					
					AVRM_Currency_writeFileLog("[Java_Cris_Currency_ReturnCurrentCurrency()] Exit.",TRACE);

				    return rtcode;
				    
				    #endif

					#ifdef B2B_NOTE_ACCEPTOR 
					 
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

					 #ifdef B2B_NOTE_ACCEPTOR

					 if( true == g_activate_device_flag)
					 {
								  rtcode= ReturnCurrentNote(Timeout); 

						 }
					 else if( false  == g_activate_device_flag)
					 {
						
						 rtcode = 28;//Communication failure

					 }
		 
					 #endif

				 
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
				#endif	 
			
			}else{
				AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniReturnCurrentCurrency()] Coin Currency Type not included",INFO);
				return 31; //Other Error
		    }//++else end
		    			
}//++Java_Cris_Currency_ReturnCurrentNote end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++[TokenReader Device API]
static pthread_mutex_t g_ActivateCCTalkPortprocmutex        = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_DeActivateCCTalkPorprocmutex       = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_GetReplyFromCCTalkPortprocmutex    = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_IsCCTALKPortOpenprocmutex          = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_WriteLogprocmutex                  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_WriteDataExchangeLogprocmutex      = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_GetSecurityVersionprocmutex        = PTHREAD_MUTEX_INITIALIZER;

JNIEXPORT jint JNICALL Java_Cris_Currency_JniActivateCCTalkPort(JNIEnv *env, jclass jobj, jint fnDeviceId,jint fnPortId) {
        
             pthread_mutex_lock( &g_ComPortAccessmutex  );
             pthread_mutex_lock( &g_activatedevicemutex );
             pthread_mutex_lock( &g_ActivateCCTalkPortprocmutex );
             
             //++Code
             int  rtcode =-1;
             //#if defined(CCTALK_DEVICE)
             
             //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
             //++rtcode =  ActivateCCTALKPort(fnPortId);
             
             rtcode = ActivateCCTALKPortV2(fnDeviceId,fnPortId);
             
             //#endif

             pthread_mutex_unlock( &g_ComPortAccessmutex  );
             pthread_mutex_unlock( &g_activatedevicemutex );
             pthread_mutex_unlock( &g_ActivateCCTalkPortprocmutex );

             return rtcode;

}//Java_Cris_Currency_ActivateCCTalkPort end

JNIEXPORT jint JNICALL Java_Cris_Currency_JniDeActivateCCTalkPort(JNIEnv *env, jclass jobj,jint fnDeviceId) {
             
             
             pthread_mutex_lock( &g_ComPortAccessmutex  );
             pthread_mutex_lock( &g_activatedevicemutex );
             pthread_mutex_lock( &g_DeActivateCCTalkPorprocmutex );

             //Code
             int rtcode =-1;

             //#if defined(CCTALK_DEVICE)
             
             //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
             //++rtcode = DeActivateCCTALKPort();
             
             //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security  -1: Any Devices
             rtcode = DeActivateCCTALKPortV2(-1);
             
             //#endif

             pthread_mutex_unlock( &g_ComPortAccessmutex  );
             pthread_mutex_unlock( &g_activatedevicemutex );
             pthread_mutex_unlock( &g_DeActivateCCTalkPorprocmutex );
             
             return rtcode;
             

}//Java_Cris_Currency_DeActivateCCTalkPort end

JNIEXPORT jint JNICALL JNICALL Java_Cris_Currency_GetReplyFromCCTalkPort___3BI_3BII( JNIEnv *env, jclass jobj,jbyteArray CmdBytes,jint fnbytestoberead,jbyteArray Reply,jint WaitBeforeReadReply,jint CcTalkReplyWaitTime){
              

                        pthread_mutex_lock( &g_ComPortAccessmutex  );

						pthread_mutex_lock( &g_activatedevicemutex );

                        pthread_mutex_lock( &g_GetReplyFromCCTalkPortprocmutex );

                        unsigned char log[100];
                        unsigned char *cmd=NULL;
                        int rtcode=-1,counter=0;
                        unsigned char recvpck[1024];
                        unsigned int recvlength=-1;
                        memset(recvpck,'\0',1024);
                        memset(log,'\0',100);
                        
                        //Get Command Array     
                        
                        jsize cmdarrLength =  (*env)->GetArrayLength(env,CmdBytes);

                        cmd= (char*) malloc(cmdarrLength*sizeof(char) );

                        //Get ByteArray Element from java byte array
                        jbyte *cmdbytearray = (*env)->GetByteArrayElements(env, CmdBytes, 0);

                        //Copy Local c Array to java byte Array
                        //printf("\n [Java_Cris_Currency_GetReplyFromCCTalkPort] CommandArray Length = %d",(int) cmdarrLength);
      
						for (counter = 0; counter < cmdarrLength ; counter++) 
						{
                               cmd[counter] = cmdbytearray[counter] ;
                               //printf("\n [Java_Cris_Currency_GetReplyFromCCTalkPort] cmd[%d] = 0x%xh",counter,cmd[counter]);
                        }
                        
                        //++#if defined(CCTALK_DEVICE)
                        rtcode = GetReplyFromCCTalkPortV2(cmd,recvpck,(int) cmdarrLength,&recvlength,fnbytestoberead,WaitBeforeReadReply,CcTalkReplyWaitTime);
                        //++#endif

                        //Get JAVA bytearray pointer
                        jsize arrLength = 0;

                        arrLength =  (*env)->GetArrayLength(env,Reply);

                        //memset(log,'\0',100);

						//sprintf(log,"[Java_Cris_Currency_GetReplyFromCCTalkPort()] Java Byte Array Length=%d .",arrLength );
 
                        //writeFileLog(log);

                        //Get ByteArray Element from java byte array
                        jbyte *bytearray = (*env)->GetByteArrayElements(env, Reply, 0);

                        //Copy Local c Array to java byte Array      
						for (counter = 0; counter < arrLength ; counter++) {
                               bytearray[counter]=recvpck[counter];
                               //++printf("\n [Java_Cris_Currency_GetReplyFromCCTalkPort] Reply[%d] = 0x%xh",counter,bytearray[counter]);
                        }

                        //Now release byte array elements
                        (*env)->ReleaseByteArrayElements(env, Reply,bytearray, 0 );

                        if( NULL !=cmd ){
                            free(cmd);
                     
                        }
                        pthread_mutex_unlock( &g_ComPortAccessmutex  );

						pthread_mutex_unlock( &g_activatedevicemutex );

                        pthread_mutex_unlock( &g_GetReplyFromCCTalkPortprocmutex );

                        return rtcode;


}//Java_Cris_Currency_GetReplyFromCCTalkPort___3BI_3BI end

JNIEXPORT jint JNICALL JNICALL Java_Cris_Currency_GetReplyFromCCTalkPort___3BI_3BI( JNIEnv *env, jclass jobj,jbyteArray CmdBytes,jint fnbytestoberead,jbyteArray Reply,jint CcTalkReplyWaitTime){
              

                        pthread_mutex_lock( &g_ComPortAccessmutex  );

	                    pthread_mutex_lock( &g_activatedevicemutex );

                        pthread_mutex_lock( &g_GetReplyFromCCTalkPortprocmutex );

                        unsigned char log[100];
                        unsigned char *cmd=NULL;
                        int rtcode=-1,counter=0;
                        unsigned char recvpck[1024];
                        unsigned int recvlength=-1;
                        memset(recvpck,'\0',1024);
                        memset(log,'\0',100);
                        
                        //Get Command Array     
                        
                        jsize cmdarrLength =  (*env)->GetArrayLength(env,CmdBytes);

                        cmd= (char*) malloc(cmdarrLength*sizeof(char) );

                        //Get ByteArray Element from java byte array
                        jbyte *cmdbytearray = (*env)->GetByteArrayElements(env, CmdBytes, 0);

                        //Copy Local c Array to java byte Array
                        //printf("\n [Java_Cris_Currency_GetReplyFromCCTalkPort] CommandArray Length = %d",(int) cmdarrLength);
      
						for (counter = 0; counter < cmdarrLength ; counter++) {
                               cmd[counter] = cmdbytearray[counter] ;
                               //printf("\n [Java_Cris_Currency_GetReplyFromCCTalkPort] cmd[%d] = 0x%xh",counter,cmd[counter]);
                        }
                        
                        #if defined(CCTALK_DEVICE)
                        rtcode = GetReplyFromCCTalkPort(cmd,recvpck,(int) cmdarrLength,&recvlength,fnbytestoberead,CcTalkReplyWaitTime);
                        #endif

                        //Get JAVA bytearray pointer
                        jsize arrLength = 0;

                        arrLength =  (*env)->GetArrayLength(env,Reply);

                        //memset(log,'\0',100);

		                //sprintf(log,"[Java_Cris_Currency_GetReplyFromCCTalkPort()] Java Byte Array Length=%d .",arrLength );
 
                        //writeFileLog(log);

                        //Get ByteArray Element from java byte array
                        jbyte *bytearray = (*env)->GetByteArrayElements(env, Reply, 0);

                        //Copy Local c Array to java byte Array      
						for (counter = 0; counter < arrLength ; counter++) 
						{
                               bytearray[counter]=recvpck[counter];
                               //printf("\n [Java_Cris_Currency_GetReplyFromCCTalkPort] Reply[%d] = 0x%xh",counter,bytearray[counter]);
                        }

                        //Now release byte array elements
                        (*env)->ReleaseByteArrayElements(env, Reply,bytearray, 0 );

                        if( NULL !=cmd ){
                            free(cmd);
                        }
                        pthread_mutex_unlock( &g_ComPortAccessmutex  );

	                    pthread_mutex_unlock( &g_activatedevicemutex );

                        pthread_mutex_unlock( &g_GetReplyFromCCTalkPortprocmutex );

                        return rtcode;


}//Java_Cris_Currency_GetReplyFromCCTalkPort___3BI_3B end

JNIEXPORT jint JNICALL Java_Cris_Currency_IsCCTALKPortBusy(JNIEnv *env, jclass jobj){
		return IsCCTALKPortBusy();
}

JNIEXPORT void JNICALL Java_Cris_Currency_SetCCTalkPollInterValTime(JNIEnv *env, jclass jobj,jint fnCCTalkPollInterValTime){
		return SetCCTalkPollInterValTime(fnCCTalkPollInterValTime);
}

JNIEXPORT jint JNICALL Java_Cris_Currency_IsCCTALKPortOpen(JNIEnv *env, jclass jobj) {
        
             
             pthread_mutex_lock( &g_ComPortAccessmutex  );

	         pthread_mutex_lock( &g_activatedevicemutex );

             pthread_mutex_lock( &g_IsCCTALKPortOpenprocmutex );


             //Code
             int  rtcode =-1;

             #if defined(CCTALK_DEVICE)
             rtcode = IsCCTALKPortOpen();
             #endif

             pthread_mutex_unlock( &g_ComPortAccessmutex  );

	         pthread_mutex_unlock( &g_activatedevicemutex );

             pthread_mutex_unlock( &g_IsCCTALKPortOpenprocmutex );

             return rtcode;

}//Java_Cris_Currency_IsCCTALKPortOpen end

JNIEXPORT jint JNICALL Java_Cris_Currency_WriteLog(JNIEnv *env, jclass jobj, jstring fnDeviceId, jstring fnMessage,jint fnLogLevel) {
       
           pthread_mutex_lock( &g_WriteLogprocmutex );
           
	       const unsigned char *Messagestr= (*env)->GetStringUTFChars(env,fnMessage,0);
	       const unsigned char *DeviceIdstr= (*env)->GetStringUTFChars(env,fnDeviceId,0);
	       //printf("\n[Java_Cris_Currency_WriteLog()] DeviceID: %s",DeviceIdstr);
           //printf("\n[Java_Cris_Currency_WriteLog()] Message: %s",Messagestr);
           AVRM_writeFileLogV2( (unsigned char*)Messagestr,(int)fnLogLevel,(unsigned char*)DeviceIdstr);
	       (*env)->ReleaseStringUTFChars(env, fnMessage, Messagestr);
	       (*env)->ReleaseStringUTFChars(env, fnDeviceId, DeviceIdstr);
           pthread_mutex_unlock( &g_WriteLogprocmutex );
	       return 0; //++Default Success Return

}//Java_Cris_Currency_WriteLog() end

JNIEXPORT jint JNICALL Java_Cris_Currency_WriteDataExchangeLog(JNIEnv *env, jclass jobj, jstring fnDeviceId, jstring fnMessageType,jstring fnMessage) {
                        
                       pthread_mutex_lock( &g_WriteDataExchangeLogprocmutex ); 

                       const char *deviceidstr= (*env)->GetStringUTFChars(env,fnDeviceId,0);
                       const char *messagetype= (*env)->GetStringUTFChars(env,fnMessageType,0);
                       const char *message= (*env)->GetStringUTFChars(env,fnMessage,0);

                       jsize NumberofBytestobeWrite = 0;

                       NumberofBytestobeWrite =  (*env)->GetArrayLength(env,fnMessage);
                       
                       LogDataExchnage(  DATA_EXCHANGE_ALL,
										 (char*)deviceidstr, 
										 (char*)messagetype,
									     (char*)message,
									     NumberofBytestobeWrite
				       );
                       

                       (*env)->ReleaseStringUTFChars(env, fnDeviceId, deviceidstr);
                       (*env)->ReleaseStringUTFChars(env, fnMessageType, messagetype);
                       (*env)->ReleaseStringUTFChars(env, fnMessage, message);

                       pthread_mutex_unlock( &g_WriteDataExchangeLogprocmutex ); 
                       return 0;

}//Java_Cris_Currency_WriteDataExchangeLog end here

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++[UPS Device API List]

//++Get UPS Status
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetUPSStatus(JNIEnv *env, jclass jobj) {

      
           //writeFileLog("[Java_Cris_Security_getUpsStatus()] Entry.");
           
           AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetUPSStatus()] Entry.",TRACE);

           pthread_mutex_lock( &g_getUpsStatusprocmutex );
          
           int rtcode =-1;
           
           #ifdef CRISAPI_TIMEOUT_TEST
           struct timespec begints, endts,diffts;
           unsigned char log[100];
           clock_gettime(CLOCK_MONOTONIC, &begints);
           #endif

           rtcode = GetAPCServiceStatus();

           #ifdef CRISAPI_TIMEOUT_TEST
           clock_gettime(CLOCK_MONOTONIC, &endts);
           diffts.tv_sec = endts.tv_sec - begints.tv_sec;
           memset(log,'\0',100);
           sprintf(log,"[Java_Cris_Security_getUpsStatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
           writeAPITimeoutLog(log);
           #endif

           pthread_mutex_unlock( &g_getUpsStatusprocmutex );
           
           AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetUPSStatus()] Exit.",TRACE);

           //writeFileLog("[Java_Cris_Security_getUpsStatus()] Exit.");

           return rtcode;



}//Java_Cris_Security_getUpsStatus() end here

//++Get UPS Battery Status
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetBatteryStatus(JNIEnv *env, jclass jobj) {

           
           //writeFileLog("[Java_Cris_Security_getBatteryStatus()] Entry.");
           
           AVRM_Currency_writeFileLog("[Java_Cris_Security_getBatteryStatus()] Entry.",TRACE);

           pthread_mutex_lock( &g_getBatteryStatusprocmutex );

           int rtcode =-1;

           #ifdef CRISAPI_TIMEOUT_TEST
           struct timespec begints, endts,diffts;
           unsigned char log[100];
           clock_gettime(CLOCK_MONOTONIC, &begints);
           #endif
           
           rtcode = GetAPCBatteryCharge();
           
           #ifdef CRISAPI_TIMEOUT_TEST
           clock_gettime(CLOCK_MONOTONIC, &endts);
           diffts.tv_sec = endts.tv_sec - begints.tv_sec;
           memset(log,'\0',100);
           sprintf(log,"[Java_Cris_Security_getBatteryStatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
           writeAPITimeoutLog(log);
           #endif
           
           pthread_mutex_unlock( &g_getBatteryStatusprocmutex );
           
           unsigned char log[100];
           memset(log,'\0',100);
           sprintf(log,"[Java_Cris_Security_getBatteryStatus()] Battery Charge=%d .",rtcode ); 
           AVRM_Currency_writeFileLog(log,TRACE);
           
           AVRM_Currency_writeFileLog("[Java_Cris_Security_getBatteryStatus()] Exit.",TRACE);

           //writeFileLog("[Java_Cris_Security_getBatteryStatus()] Exit.");

           return rtcode;


}//Java_Cris_Security_getBatteryStatus() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++[Security Device API List]
static int ActivateCCTALKComPort() {
              

          //++AVRM_Currency_writeFileLog("[ActivateCCTALKComPort()] Going to check CCTALK port open status.");

	      //port open must be done here
	      int  cctalkrtcode=-1;

	      bool cctalkportopenflag=false;
	      
	      cctalkrtcode=IsCCTALKPortOpen();

	      //CCTALK Port not opened
	      if(0==cctalkrtcode) 
	      {

		        cctalkrtcode=-1;

		        int CCTALKPortNumber=-1;
	     
		        CCTALKPortNumber = GetCCTALKPortNo();

		        if( CCTALKPortNumber < 0 )
			{
				
			      //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort()] CCTALK Port Number wrong input."); 
                              return false; //failed operation
                        }

		        //Open CCTALK Port 
                        cctalkrtcode=ActivateCCTALKPort(CCTALKPortNumber);

		        if( 0 == cctalkrtcode )
			{

		                 cctalkportopenflag=false;

		                 //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort()] CCTALK Open Port failed.");
				  

			}
			else if( 1 == cctalkrtcode )
			{  

				 cctalkportopenflag=true;

				 //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort()] CCTALK Open Port opened successfully.");

			}
		        

	     }
	     else if(1 == cctalkrtcode) //CCTALK Port opened already
	     {
		   
		    cctalkportopenflag=true;
             }


             return cctalkportopenflag;


}//ActivateCCTALKComPort() end

static int ActivateCCTALKComPort_V2(int CCTALKPortNumber) {
              

              //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort_V2()] Going to check CCTALK port open status.");

	      //port open must be done here
	      int  cctalkrtcode=-1;

	      int cctalkportopenflag= 0;
	      
	      cctalkrtcode=IsCCTALKPortOpen();

	      //CCTALK Port not opened
	      if(0==cctalkrtcode) 
	      {

                   //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Port is not opened."); 

		           cctalkrtcode=-1;

		           if( CCTALKPortNumber < 0 ) {
                              //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Port Number wrong input."); 
                              return false; //failed operation
                   }

		           //Open CCTALK Port 
                   cctalkrtcode=ActivateCCTALKPort(CCTALKPortNumber);

					if( 0 == cctalkrtcode )
					{

		                 cctalkportopenflag=false;
                                 //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Open Port failed.");
		        }
			else if( 1 == cctalkrtcode )
			{  

				  cctalkportopenflag=true;
				  //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Open Port opened successfully.");
	                }
		        

	     }
	     else if(1 == cctalkrtcode) //CCTALK Port opened already
	     {
		    //AVRM_Currency_writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Port is already opened.");

		    cctalkportopenflag=true;
             }


             return cctalkportopenflag;


}//int ActivateCCTALKComPort_V2(int CCTALKPortNumber) end

static int DeActivateCCTALKComPort() {

              int  cctalkrtcode=-1;

              cctalkrtcode=IsCCTALKPortOpen();

			  //CCTALK Port not opened
			  if(0==cctalkrtcode) //Port not opened
			  {
                  //AVRM_Currency_writeFileLog("[DeActivateCCTALKComPort()] CCTALK port already closed.");
                  return 1;//1 =Success
              }
              else if(1 == cctalkrtcode) //CCTALK Port opened already
	          {
		          //AVRM_Currency_writeFileLog("[DeActivateCCTALKComPort()] Now going to closed CCTALK port.");
                  return (DeActivateCCTALKPort());
                  
              }//else if end


}//static int DeActivateCCTALKComPort() end

JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetSecurityVersion(JNIEnv *env, jclass jobj, jbyteArray versionNO){

			
					        //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetSecurityVersion()] Entry.");

			                pthread_mutex_lock( &g_GetSecurityVersionprocmutex );

		                    pthread_mutex_lock( &g_ComPortAccessmutex );

                           /////////////////////////////////////////////////////////////////////////////////////////////
                          
                           
						   #ifdef CRISAPI_TIMEOUT_TEST
						   struct timespec begints, endts,diffts;
						   clock_gettime(CLOCK_MONOTONIC, &begints);
						   #endif

                           /////////////////////////////////////////////////////////////////////////////////////////////
                           
                           #if defined(CCTALK_DEVICE) && defined(ATVM_ALARM_VAULT)

                          
                           //here get cctalk port open status
				 
                           bool cctalkportopenflag=false;

                           int comportopenflag= 0;
									  
						   comportopenflag=IsCCTALKPortOpen();

						   //CCTALK Port not opened
						   if(0==comportopenflag) { 
							 
							 pthread_mutex_unlock( &g_GetSecurityVersionprocmutex );
                             pthread_mutex_unlock( &g_ComPortAccessmutex );
                             //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetSecurityVersion()] Error Security com port not opened.");
                             return  (COMMUNICATION_FAILURE); //failed operation
                             
                           }else{
							 cctalkportopenflag = true;
								  
						   }//else end
						       
                           if( true == cctalkportopenflag ) {
							       
							       int rtcode = 0;
							       unsigned char SecurityVersionNumber[5];
							       memset(SecurityVersionNumber,'\0',5);
							       rtcode = getSecurityVersionNumber(SecurityVersionNumber);
							       pthread_mutex_unlock( &g_ComPortAccessmutex );
								   pthread_mutex_unlock( &g_GetSecurityVersionprocmutex );
								   return  rtcode; //success operation
							       
                           }else if( false == cctalkportopenflag ){
								  
								   pthread_mutex_unlock( &g_ComPortAccessmutex );
								   pthread_mutex_unlock( &g_GetSecurityVersionprocmutex );
								   //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetSecurityVersion()]  Unable to get any door status due to cctalk port still not opened.");
								   //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetSecurityVersion()] Exit.");
								   #ifdef CRISAPI_TIMEOUT_TEST
								   clock_gettime(CLOCK_MONOTONIC, &endts);
								   diffts.tv_sec = endts.tv_sec - begints.tv_sec;
								   memset(log,'\0',100);
								   sprintf(log,"[Java_Cris_Currency_JniGetSecurityVersion()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
								   writeAPITimeoutLog(log);
								   #endif
								   return  (COMMUNICATION_FAILURE); //failed operation
					      }
                    
                      #endif
 
                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      #if !defined(CCTALK_DEVICE) ||  !defined(ATVM_ALARM_VAULT)

					  //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetSecurityVersion()]  CCTALK or ATVM_ALARM_VAULT not defined in atvmdevice headers files.");

					  pthread_mutex_unlock( &g_ComPortAccessmutex );

					  pthread_mutex_unlock( &g_GetSecurityVersionprocmutex );

					  //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetSecurityVersion()] Exit.");

					  #ifdef CRISAPI_TIMEOUT_TEST
					  
					  clock_gettime(CLOCK_MONOTONIC, &endts);
					  diffts.tv_sec = endts.tv_sec - begints.tv_sec;
					  memset(log,'\0',100);
					  sprintf(log,"[Java_Cris_Currency_JniGetSecurityVersion()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
					  writeAPITimeoutLog(log);
					  
					  #endif
					  
					  return  (COMMUNICATION_FAILURE); //failed operation
                      #endif

                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




}//Java_Cris_Currency_JniGetSecurityVersion end

//++DisableAlarm [ failed operation=0 , success operation=1 ]
JNIEXPORT jint JNICALL Java_Cris_Currency_JniDisableAlarm( JNIEnv *env, jclass jobj,jint dooropenflag, jint dooropenexpirytime, jint cashboxopenflag, jint cashboxopenexpirytime , jbyte alarmLogicBits) {

             
             //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Entry.");
             
             AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Entry.",TRACE);

             pthread_mutex_lock( &g_disableAlarmprocmutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             unsigned char log[100];

             memset(log,'\0',100);

             #ifdef CRISAPI_TIMEOUT_TEST

             struct timespec begints, endts,diffts;

             clock_gettime(CLOCK_MONOTONIC, &begints);

             #endif

             ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
			 //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Entry.");
	     
			 #if defined(CCTALK_DEVICE) && defined(ATVM_ALARM_VAULT)

			  //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Going to open ascrm vault.");

              bool cctalkportopenflag= false;
	      
              int cctalkrtcode=0;

	          cctalkrtcode=IsCCTALKPortOpen();

			  //CCTALK Port not opened
			  if(0==cctalkrtcode) {
				   
                   pthread_mutex_unlock( &g_disableAlarmprocmutex );

                   pthread_mutex_unlock( &g_ComPortAccessmutex );

                   //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Error Security com port not opened.");
                   
                   AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Error Security com port not opened.",ERROR);

                   return  (COMMUNICATION_FAILURE); //failed operation

              }else{
				  
                  cctalkportopenflag = true;

              }//else end
              
			 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			 jint JniRtCode=0;
			 
			 //if cctalk port opened then execute vault open command  command else reject vault open operation
			 if( true == cctalkportopenflag ) {

                             //++ 1 Main Door
							 if(1 == dooropenflag)  {
								 
									memset(log,'\0',100);

									sprintf(log,"[Java_Cris_Security_DisableAlarm()]  Main Door Allow Time : %d .",dooropenexpirytime );
											  
									//writeFileLog(log);
									
									AVRM_Currency_writeFileLog(log,INFO);

							 }//else end
							 
                             //++ 2 Cash Vault
							 if(2 == cashboxopenflag){
								 
                                    memset(log,'\0',100);

								    sprintf(log,"[Java_Cris_Security_DisableAlarm()] Cash vault Allow Time: %d .",cashboxopenexpirytime ); 

								    //writeFileLog(log);
								    
								    AVRM_Currency_writeFileLog(log,INFO);

                             }//if end

                             /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                             if( (2 != cashboxopenflag) && (1 != dooropenflag) ){

                                   pthread_mutex_unlock( &g_ComPortAccessmutex );

                                   pthread_mutex_unlock( &g_disableAlarmprocmutex );

                                   //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] No Door type defined.");

                                   //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Exit.");
                                   
                                   AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] No Door type defined.",INFO);
                                    
                                   AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Exit.",INFO);

			                       return OTHER_ERROR;//0: fail operation

                             }//if end

                             /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	   
							 
										  //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Before issue setDisableAlarmState() api for open vault.");
										  AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Before issue setDisableAlarmState() api for open vault.",INFO);
										  g_cashboxopenexpirytime=0;
										  g_dooropenexpirytime=0;
                                          g_cashboxopenexpirytime = cashboxopenexpirytime;                 
                                          g_dooropenexpirytime    = dooropenexpirytime;
                                          //++Open vault or door
                                          JniRtCode = setDisableAlarmState( dooropenflag,dooropenexpirytime, cashboxopenflag, cashboxopenexpirytime,(char)alarmLogicBits) ;
                                          //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] After issue setDisableAlarmState() api for open vault.");
                                          AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] After issue setDisableAlarmState() api for open vault.",INFO);
                                          pthread_mutex_unlock( &g_ComPortAccessmutex );
                                          pthread_mutex_unlock( &g_disableAlarmprocmutex );
                                          //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Exit.");
                                          AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Exit.",TRACE);
                                          return JniRtCode;
		
	     }else if( false == cctalkportopenflag ) {

                   pthread_mutex_unlock( &g_ComPortAccessmutex );

                   pthread_mutex_unlock( &g_disableAlarmprocmutex );

		           //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()]  Unable to open vault due to cctalk port still not opened.");

                   //writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Exit.");
                   
                   AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()]  Unable to open vault due to cctalk port still not opened.",INFO);
                   
                   AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Exit..",TRACE);

                   #ifdef CRISAPI_TIMEOUT_TEST
				   clock_gettime(CLOCK_MONOTONIC, &endts);
				   diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                   memset(log,'\0',100);
				   sprintf(log,"[Java_Cris_Currency_JniDisableAlarm()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
				   writeAPITimeoutLog(log);
				   #endif
		           return  COMMUNICATION_FAILURE; //failed operation

	     } //else end

	     #endif

	     #if !defined(CCTALK_DEVICE) ||  !defined(ATVM_ALARM_VAULT)

	         //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()]  CCTALK or ATVM_ALARM_VAULT not defined in atvmdevice headers files.");

             pthread_mutex_unlock( &g_ComPortAccessmutex );

             pthread_mutex_unlock( &g_disableAlarmprocmutex );

             //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniDisableAlarm()] Exit.");

             #ifdef CRISAPI_TIMEOUT_TEST
             clock_gettime(CLOCK_MONOTONIC, &endts);
	         diffts.tv_sec = endts.tv_sec - begints.tv_sec;
             memset(log,'\0',100);
             sprintf(log,"[Java_Cris_Currency_JniDisableAlarm()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
	         writeAPITimeoutLog(log);
             #endif
	         return  COMMUNICATION_FAILURE; //failed operation

	      #endif

}//Java_Cris_Currency_JniDisableAlarm() end

//++Get Door Status
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetDoorStatus(JNIEnv *env, jclass jobj,jint DoorType) {

		 
							AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Entry.",TRACE);
			                pthread_mutex_lock( &g_getDoorstatusprocmutex );
		                    pthread_mutex_lock( &g_ComPortAccessmutex );

                           /////////////////////////////////////////////////////////////////////////////////////////////
                          
						   #ifdef CRISAPI_TIMEOUT_TEST
						   struct timespec begints, endts,diffts;
						   clock_gettime(CLOCK_MONOTONIC, &begints);
						   #endif

                           /////////////////////////////////////////////////////////////////////////////////////////////
                           
                           #if defined(CCTALK_DEVICE) && defined(ATVM_ALARM_VAULT)

                           int rtcode=-1;			   
			               int cashboxOpenStatus=2;			  
                           int alarmOnStatus=2;                        
                           int TopDoorOpenStatus=2;
                           int BottomDoorOpenStatus=2;

                           //++here get cctalk port open status
                           bool cctalkportopenflag=false;

                           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			                        int comportopenflag= 0;
									  
									comportopenflag=IsCCTALKPortOpen();

									//++CCTALK Port not opened
									if(0==comportopenflag) 
									{ 
									     pthread_mutex_unlock( &g_getDoorstatusprocmutex );
									     pthread_mutex_unlock( &g_ComPortAccessmutex );
									     AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Error Security com port not opened.",INFO);
									     return  (COMMUNICATION_FAILURE); //failed operation
								    }else{
										 cctalkportopenflag = true;
								    }//++else end

				           //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                           
                           if( true == cctalkportopenflag ) {
							   
                                           int ButtomDoorStatus=0;
                                           //++JNI_delay_mSec(200);
                                            
										   //++Call C Function
										   if( 0 == DoorType ){ //++Main Door [Top Door]								
												rtcode=getSecuredState(&TopDoorOpenStatus,NULL,NULL,NULL,NULL);
                                           }else if( 1 == DoorType ){ //++CashBox Door
                                                rtcode=getSecuredState(NULL,NULL,&cashboxOpenStatus,NULL,NULL);
									       }else if( 2 == DoorType ){ //++Bottom Door
                                                //rtcode=getSecuredState(NULL,NULL,NULL,&BottomDoorOpenStatus,NULL);
									       }else if( 3 == DoorType ){ //++Alarm
                                                //rtcode=getSecuredState(NULL,&alarmOnStatus,NULL,NULL,NULL);
									       }//else if end
									   
                                           // 0 = Main Door and 1 : Cash vault and 2= Bottom Door
                                           switch( DoorType )
                                           {
                                                   
												   case 0: //++Top Main Door
															   
													   pthread_mutex_unlock( &g_ComPortAccessmutex );
													   pthread_mutex_unlock( &g_getDoorstatusprocmutex );

													   //Java Metro Top Door: 0: Open 1: Closed
													   //C 0:Closed 1: Open
													   if( (0 == TopDoorOpenStatus) /*&& (0 == BottomDoorOpenStatus)*/) {
														  AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()]  Door Close.",INFO);
                                                          return 1; //close =1
													   }else if( (1 == TopDoorOpenStatus) /*|| (1 == BottomDoorOpenStatus)*/ ) {
														 AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()]  Door open.",INFO);
                                                         return 0; //open =0													   
													   }else {
														 AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Door status is unknown.",INFO);
                                                         return (OTHER_ERROR); //unknown status 
													   }//else end

													   break;
													   
											  case 1: //++Cash Vault
                                                           
                                                       pthread_mutex_unlock( &g_ComPortAccessmutex );
													   pthread_mutex_unlock( &g_getDoorstatusprocmutex );
													   	
                                                       //Metro Cash vault : 0: Open 1: Closed
													   if( 0 == cashboxOpenStatus){
														  AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] CashBox status is closed.",INFO);
														  return 1; //close =1

													   }else if( 1 == cashboxOpenStatus ){
														 AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] CashBox status is open.",INFO);
														 return 0; //open =0
													   
													   }else {
														 AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] CashBox status is unknown.",INFO);
														 return (OTHER_ERROR); //unknown status 
													   }

		                                               break;

													   
											   case 2: //++Bottom Door Status
											           
											           pthread_mutex_unlock( &g_ComPortAccessmutex );
													   pthread_mutex_unlock( &g_getDoorstatusprocmutex );
                                                       
                                                       if( 0 == BottomDoorOpenStatus ){
															AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Bottom Door status is closed.",INFO);
															return 1; //close =1
													   }
													   else if( 1 == BottomDoorOpenStatus ){
															AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Bottom Door status is open.",INFO);
															return 0; //open =0
													   }else{
													        AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Bottom Door status is unknown.",INFO);
															return OTHER_ERROR; //unknown status 
												       }
												       break;
											   
											   case 3: //++Alarm Status
											           
											           pthread_mutex_unlock( &g_ComPortAccessmutex );
													   pthread_mutex_unlock( &g_getDoorstatusprocmutex );

											           if( 0 == alarmOnStatus ){
															AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Alarm is off.",INFO);
															return 0; //Off =0
													   }
													   else if( 1 == alarmOnStatus ){
															AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Alarm is on.",INFO);
															return 1; //On =1
													   }else{
													        AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Alarm status is unknown.",INFO);
															return OTHER_ERROR; //unknown status 
												       }
												       break;
												       
                                               default:
													AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] No Door Type Defined..",INFO);
													pthread_mutex_unlock( &g_ComPortAccessmutex );
													pthread_mutex_unlock( &g_getDoorstatusprocmutex );
													return (OTHER_ERROR); //unknown status 
													break;

                                 };//switch end
                                 
                           }else if( false == cctalkportopenflag ){
								  
								   pthread_mutex_unlock( &g_ComPortAccessmutex );
								   pthread_mutex_unlock( &g_getDoorstatusprocmutex );
								   AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Unable to get any door status due to cctalk port still not opened..",INFO);
								   #ifdef CRISAPI_TIMEOUT_TEST
								   clock_gettime(CLOCK_MONOTONIC, &endts);
								   diffts.tv_sec = endts.tv_sec - begints.tv_sec;
								   memset(log,'\0',100);
								   sprintf(log,"[Java_Cris_Currency_JniGetDoorStatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
								   writeAPITimeoutLog(log);
								   #endif
								   return  (COMMUNICATION_FAILURE); //failed operation
					      }
                    
                      #endif
 
                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      #if !defined(CCTALK_DEVICE) ||  !defined(ATVM_ALARM_VAULT)

					  //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()]  CCTALK or ATVM_ALARM_VAULT not defined in atvmdevice headers files.");

					  pthread_mutex_unlock( &g_ComPortAccessmutex );

					  pthread_mutex_unlock( &g_getDoorstatusprocmutex );

					  //AVRM_Currency_writeFileLog("[Java_Cris_Currency_JniGetDoorStatus()] Exit.");

					  #ifdef CRISAPI_TIMEOUT_TEST
					  clock_gettime(CLOCK_MONOTONIC, &endts);
					  diffts.tv_sec = endts.tv_sec - begints.tv_sec;
					  memset(log,'\0',100);
					  sprintf(log,"[Java_Cris_Currency_JniGetDoorStatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
					  writeAPITimeoutLog(log);
					  #endif
					  
					  return  (-1); //failed operation
                      #endif

                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

     
   
}//Java_Cris_Currency_JniGetDoorStatus() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Millisecond delay
static void JNI_delay_mSec(int milisec)  {


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

			//AVRM_Currency_writeFileLog(log);


        }


}//JNI_delay_mSec() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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



JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupSecurityLogFile( JNIEnv *env, jclass CurrencyClass,jstring jniLogfile, jint jniLogLevel, jstring jniDeviceId,jstring jniLogdllPath)
{

               printf("\n[Java_Cris_Security_SetupSecurityLogFile()] Entry.");


               /////////////////////////////////////////////////////////////////////////////////////////////////////////

         
               g_Ascrm_writeFileLog=NULL;

	       g_lib_handle=NULL;

	       /////////////////////////////////////////////////////////////////////////////////////////////////////////
	       
	       //Step 1:Log dll Path

	       unsigned char *LogdllPathstr= (unsigned char *)(*env)->GetStringUTFChars(env,jniLogdllPath,0);

	       //printf("\n[Java_Cris_Security_SetupSecurityLogFile()] Log dll Path : %s", LogdllPathstr);

               SetSoFilePath(LogdllPathstr);

	       (*env)->ReleaseStringUTFChars(env, jniLogdllPath, LogdllPathstr);


	       //////////////////////////////////////////////////////////////////////////////////////////////////////////

	       //Step 2:Log File Name

	       unsigned char *LogFileName= (unsigned char *)(*env)->GetStringUTFChars(env,jniLogfile,0);

	       //printf("\n[Java_Cris_Security_SetupSecurityLogFile()] Log File Name : %s", LogFileName);

	       SetGeneralFileLogPath( LogFileName );

	       (*env)->ReleaseStringUTFChars(env, jniLogfile, LogFileName);

	       //////////////////////////////////////////////////////////////////////////////////////////////////////////
                
               //Step 3: Set LogLevel Mode
               g_LogModeLevel = (int)jniLogLevel;
               
               //printf("\n[Java_Cris_Security_SetupSecurityLogFile()] Log File Mode: %d", g_LogModeLevel);

               //////////////////////////////////////////////////////////////////////////////////////////////////////////

               //Step 4: Get Device id

               unsigned char *deviceid= (unsigned char *)(*env)->GetStringUTFChars(env,jniDeviceId,0);

	       //printf("\n[Java_Cris_SmartCard_SetupSmartCardLogFile()] Device ID : %s", deviceid);

               strcpy(g_Deviceid,deviceid);

               (*env)->ReleaseStringUTFChars(env,jniDeviceId, deviceid);


               //////////////////////////////////////////////////////////////////////////////////////////////////////////

               printf("\n[Java_Cris_Security_SetupSecurityLogFile()] Exit.");

	       return true;


}//JNIEXPORT jboolean JNICALL Java_Cris_Security_SetupSecurityLogFile end

JNIEXPORT jint JNICALL Java_Cris_Currency_JniActivateSecurityPort( JNIEnv *env, jclass jobj, jint ComPortId)
{


          writeFileLog("[Java_Cris_Security_ActivateSecurityPort()] Entry.");

          pthread_mutex_lock( &g_ActivateComPortprocmutex  );

          pthread_mutex_lock( &g_ComPortAccessmutex );

          int rtcode =-1;

          rtcode =  ActivateCCTALKComPort_V2(ComPortId);

          pthread_mutex_unlock( &g_ComPortAccessmutex );

          pthread_mutex_unlock( &g_ActivateComPortprocmutex  );

          writeFileLog("[Java_Cris_Security_ActivateSecurityPort()] Exit.");

          return rtcode ;



}//Java_Cris_Security_ActivateSecurityPort() end

JNIEXPORT jint JNICALL Java_Cris_Currency_JniDeActivateSecurityPort(JNIEnv *env, jclass jobj)
{


	       writeFileLog("[Java_Cris_Security_DeActivateSecurityPort()] Entry.");

	       pthread_mutex_lock( &g_DeActivateComPortprocmutex  );

               pthread_mutex_lock( &g_ComPortAccessmutex );

	       int rtcode =-1;

	       rtcode = DeActivateCCTALKComPort();

               pthread_mutex_unlock( &g_ComPortAccessmutex );

	       pthread_mutex_unlock(&g_DeActivateComPortprocmutex  );

	       writeFileLog("[Java_Cris_Security_DeActivateSecurityPort()] Exit.");

	       return rtcode ;



}//Java_Cris_Security_DeActivateSecurityPort() end

//++Get UPS Status
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetUPSStatus(JNIEnv *env, jclass jobj)
{

      
           writeFileLog("[Java_Cris_Security_getUpsStatus()] Entry.");

           pthread_mutex_lock( &g_getUpsStatusprocmutex );
          
           int rtcode =-1;
           
           #ifdef CRISAPI_TIMEOUT_TEST
           struct timespec begints, endts,diffts;
           unsigned char log[100];
           clock_gettime(CLOCK_MONOTONIC, &begints);
           #endif

           rtcode = GetAPCServiceStatus();

           #ifdef CRISAPI_TIMEOUT_TEST
           clock_gettime(CLOCK_MONOTONIC, &endts);
           diffts.tv_sec = endts.tv_sec - begints.tv_sec;
           memset(log,'\0',100);
           sprintf(log,"[Java_Cris_Security_getUpsStatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
           writeAPITimeoutLog(log);
           #endif

           pthread_mutex_unlock( &g_getUpsStatusprocmutex );

           writeFileLog("[Java_Cris_Security_getUpsStatus()] Exit.");

           return rtcode;



}//Java_Cris_Security_getUpsStatus() end here

//++Get UPS Battery Status
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetBatteryStatus(JNIEnv *env, jclass jobj)
{

           
           writeFileLog("[Java_Cris_Security_getBatteryStatus()] Entry.");

           pthread_mutex_lock( &g_getBatteryStatusprocmutex );

           int rtcode =-1;

           #ifdef CRISAPI_TIMEOUT_TEST
           struct timespec begints, endts,diffts;
           unsigned char log[100];
           clock_gettime(CLOCK_MONOTONIC, &begints);
           #endif
           rtcode = GetAPCBatteryCharge();
           #ifdef CRISAPI_TIMEOUT_TEST
           clock_gettime(CLOCK_MONOTONIC, &endts);
           diffts.tv_sec = endts.tv_sec - begints.tv_sec;
           memset(log,'\0',100);
           sprintf(log,"[Java_Cris_Security_getBatteryStatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
           writeAPITimeoutLog(log);
           #endif
           pthread_mutex_unlock( &g_getBatteryStatusprocmutex );

           writeFileLog("[Java_Cris_Security_getBatteryStatus()] Exit.");

           return rtcode;


}//Java_Cris_Security_getBatteryStatus() end

int ActivateCCTALKComPort_V2(int CCTALKPortNumber){
              

          writeFileLog("[ActivateCCTALKComPort_V2()] Going to check CCTALK port open status.");

	      //port open must be done here
	      int  cctalkrtcode=-1;

	      int cctalkportopenflag= 0;
	      
	      cctalkrtcode=IsCCTALKPortOpen();

	      //CCTALK Port not opened
	      if(0==cctalkrtcode) 
	      {

                        writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Port is not opened."); 

		        cctalkrtcode=-1;

		        if( CCTALKPortNumber < 0 )
			{
                              writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Port Number wrong input."); 
                              return false; //failed operation
                        }

		        //Open CCTALK Port 
                        cctalkrtcode=ActivateCCTALKPort(CCTALKPortNumber);

		        if( 0 == cctalkrtcode )
			{

		                 cctalkportopenflag=false;
                                 writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Open Port failed.");
		        }
			else if( 1 == cctalkrtcode )
			{  

				  cctalkportopenflag=true;
				  writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Open Port opened successfully.");
	                }
		        

	     }
	     else if(1 == cctalkrtcode) //CCTALK Port opened already
	     {
		    writeFileLog("[ActivateCCTALKComPort_V2()] CCTALK Port is already opened.");

		    cctalkportopenflag=true;
             }


             return cctalkportopenflag;


}//int ActivateCCTALKComPort_V2(int CCTALKPortNumber) end

int DeActivateCCTALKComPort(){
	

              int  cctalkrtcode=-1;

              cctalkrtcode=IsCCTALKPortOpen();

	      //CCTALK Port not opened
	      if(0==cctalkrtcode) //Port not opened
	      {
                  writeFileLog("[DeActivateCCTALKComPort()] CCTALK port already closed.");
                  return 1;//1 =Success
              }
              else if(1 == cctalkrtcode) //CCTALK Port opened already
	      {
		  writeFileLog("[DeActivateCCTALKComPort()] Now going to closed CCTALK port.");
                  return (DeActivateCCTALKPort());
              }

}//static int DeActivateCCTALKComPort() end

//++disableAlarm [ failed operation=0 , success operation=1 ]
JNIEXPORT jint JNICALL Java_Cris_Currency_JniDisableAlarm( JNIEnv *env, jclass jobj,jint dooropenflag, jint dooropenexpirytime, jint cashboxopenflag, jint cashboxopenexpirytime)
{


             

             writeFileLog("[Java_Cris_Security_DisableAlarm()] Entry.");

             pthread_mutex_lock( &g_disableAlarmprocmutex );

             pthread_mutex_lock( &g_ComPortAccessmutex );

             unsigned char log[100];

             memset(log,'\0',100);

             #ifdef CRISAPI_TIMEOUT_TEST

             struct timespec begints, endts,diffts;

             clock_gettime(CLOCK_MONOTONIC, &begints);

             #endif

             //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
	     writeFileLog("[Java_Cris_Security_DisableAlarm()] Entry.");

	     
	     #if defined(CCTALK_DEVICE) && defined(ATVM_ALARM_VAULT)

	     writeFileLog("[Java_Cris_Security_DisableAlarm()] Going to open ascrm vault.");

             //////////////////////////////////////////////////////////////////////////////////////////////////////////////

              bool cctalkportopenflag= false;
	      
              int cctalkrtcode=0;

	      cctalkrtcode=IsCCTALKPortOpen();

	      //CCTALK Port not opened
	      if(0==cctalkrtcode) 
	      { 
                   pthread_mutex_unlock( &g_disableAlarmprocmutex );

                   pthread_mutex_unlock( &g_ComPortAccessmutex );

                   writeFileLog("[Java_Cris_Security_DisableAlarm()] Error Security com port not opened.");

                   return  (-1); //failed operation

              }
              else
              {
                  cctalkportopenflag = true;

              }
              
	     //////////////////////////////////////////////////////////////////////////////////////////////

	     jint JniRtCode=0;
	     
	     //if cctalk port opened then execute vault open command  command else reject vault open operation
	     if( true == cctalkportopenflag ) 
	     {

                             if(0 == dooropenflag) //0 Main Door
			     {
                                  memset(log,'\0',100);

				  sprintf(log,"[Java_Cris_Security_DisableAlarm()]  Main Door Allow Time : %d .",dooropenexpirytime );
                                  writeFileLog(log);

                             }

                             if(1 == cashboxopenflag)//1 Cash Vault
			     {
                                  memset(log,'\0',100);

				  sprintf(log,"[Java_Cris_Security_DisableAlarm()] Cash vault Allow Time: %d .",cashboxopenexpirytime ); 

				  writeFileLog(log);

                             }

                             ////////////////////////////////////////////////////////////////////////////////////

                             if( (1 != cashboxopenflag) && (0 != dooropenflag) )
                             {

                                   pthread_mutex_unlock( &g_ComPortAccessmutex );

                                   pthread_mutex_unlock( &g_disableAlarmprocmutex );

                                   writeFileLog("[Java_Cris_Security_DisableAlarm()] No Door type defined.");

                                   writeFileLog("[Java_Cris_Security_DisableAlarm()] Exit.");

			           return 0;//0: fail operation

                             }

                             ////////////////////////////////////////////////////////////////////////////////////
	   
			     if( (1 == cashboxopenflag) && (cashboxopenexpirytime>0) )
			     {

		                          writeFileLog("[Java_Cris_Security_DisableAlarm()] Before issue setDisableAlarmState() api for open vault.");
					  g_cashboxopenexpirytime=0;

                                          g_dooropenexpirytime=0;

					  g_cashboxopenexpirytime = cashboxopenexpirytime;                 

                                          g_dooropenexpirytime    = dooropenexpirytime;

                                          //Open vault 
                                          JniRtCode = setDisableAlarmState( 1 ,cashboxopenexpirytime, 0, 0) ;

                                          writeFileLog("[Java_Cris_Security_DisableAlarm()] After issue setDisableAlarmState() api for open vault.");
                                          pthread_mutex_unlock( &g_ComPortAccessmutex );

                                          pthread_mutex_unlock( &g_disableAlarmprocmutex );

                                          writeFileLog("[Java_Cris_Security_DisableAlarm()] Exit.");

			                  return JniRtCode;
			 
			     }               
                             else
			     {

                                   writeFileLog("[Java_Cris_Security_DisableAlarm()] atvm vault will be opned shortly by direct command issue process.");
                                   return JniRtCode;
    

			     }


	     }
	     else if( false == cctalkportopenflag )
	     {

                   pthread_mutex_unlock( &g_ComPortAccessmutex );

                   pthread_mutex_unlock( &g_disableAlarmprocmutex );

		   writeFileLog("[Java_Cris_Security_DisableAlarm()]  Unable to open vault due to cctalk port still not opened.");

                   writeFileLog("[Java_Cris_Security_DisableAlarm()] Exit.");

                   #ifdef CRISAPI_TIMEOUT_TEST
		   clock_gettime(CLOCK_MONOTONIC, &endts);
		   diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                   memset(log,'\0',100);
		   sprintf(log,"[Java_Cris_Security_DisableAlarm()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
		   writeAPITimeoutLog(log);
		   #endif
		   return  0; //failed operation

	     }

	     #endif


	     #if !defined(CCTALK_DEVICE) ||  !defined(ATVM_ALARM_VAULT)

	     writeFileLog("[Java_Cris_Security_DisableAlarm()]  CCTALK or ATVM_ALARM_VAULT not defined in atvmdevice headers files.");

             pthread_mutex_unlock( &g_ComPortAccessmutex );

             pthread_mutex_unlock( &g_disableAlarmprocmutex );

             writeFileLog("[Java_Cris_Security_DisableAlarm()] Exit.");

             #ifdef CRISAPI_TIMEOUT_TEST
             clock_gettime(CLOCK_MONOTONIC, &endts);
	     diffts.tv_sec = endts.tv_sec - begints.tv_sec;
             memset(log,'\0',100);
             sprintf(log,"[Java_Cris_Security_DisableAlarm()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
	     writeAPITimeoutLog(log);
             #endif
	     return  0; //failed operation

	     #endif




}//Java_Cris_Security_DisableAlarm() end

//++Get Door Status
JNIEXPORT jint JNICALL Java_Cris_Currency_JniGetDoorStatus(JNIEnv *env, jclass jobj,jint DoorType)
{

		 
		           writeFileLog("[Java_Cris_Security_getDoorstatus()] Entry.");

			   pthread_mutex_lock( &g_getDoorstatusprocmutex );

		           pthread_mutex_lock( &g_ComPortAccessmutex );

                           /////////////////////////////////////////////////////////////////////////////////////////////
                          
                           
			   #ifdef CRISAPI_TIMEOUT_TEST
			   struct timespec begints, endts,diffts;
			   clock_gettime(CLOCK_MONOTONIC, &begints);
			   #endif

                           /////////////////////////////////////////////////////////////////////////////////////////////
                           
                           #if defined(CCTALK_DEVICE) && defined(ATVM_ALARM_VAULT)

                           int rtcode=-1;
			   
			   int cashboxOpenStatus=2;
			  
                           int alarmOnStatus=2;
                        
                           int TopDoorOpenStatus=2;

                           //here get cctalk port open status
				 
                           bool cctalkportopenflag=false;

                           //////////////////////////////////////////////////////////////////////////////////////////////

			    int comportopenflag= 0;
			      
			    comportopenflag=IsCCTALKPortOpen();

			    //CCTALK Port not opened
			    if(0==comportopenflag) 
			    { 
				  pthread_mutex_unlock( &g_getDoorstatusprocmutex );

		                  pthread_mutex_unlock( &g_ComPortAccessmutex );

				  writeFileLog("[Java_Cris_Security_getDoorstatus()] Error Security com port not opened.");

				  return  (-1); //failed operation

			   }
                           else
                           {
                                 cctalkportopenflag = true;
                          
			   }    

	                   //////////////////////////////////////////////////////////////////////////////////////////////

                           if( true == cctalkportopenflag ) 
                           {
                                           
				           //Call C Function
					   rtcode=getSecuredState(&TopDoorOpenStatus,&alarmOnStatus,&cashboxOpenStatus,NULL);

                                           // 0 = Main Door and 1 : Cash vault
                                           switch( DoorType )
                                           {
                                                   
                                                      case 1: //Cash Vault
                                                           
                                                           //Metro Cash vault : 0: Open 1: Closed
							   if( 0 == cashboxOpenStatus)
							   {
								  writeFileLog("[Java_Cris_Security_getDoorstatus()] Cash Vault close.");
								  writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");
								  pthread_mutex_unlock( &g_ComPortAccessmutex );
								  pthread_mutex_unlock( &g_getDoorstatusprocmutex );	
								  return 1; //close =1

							   }
							   else if( 1 == cashboxOpenStatus )
							   {

								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Cash Vault open.");
								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");
								 pthread_mutex_unlock( &g_ComPortAccessmutex );
								 pthread_mutex_unlock( &g_getDoorstatusprocmutex );
								 return 0; //open =0
							   }
							   else 
							   {

								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Cash Vault status is unknown.");
								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");

								 pthread_mutex_unlock( &g_ComPortAccessmutex );

								 pthread_mutex_unlock( &g_getDoorstatusprocmutex );

								 return (-1); //unknown status 
							   }

		                                           break;


                                               case 0: //Top Main Door
                                                           
                                                           //Metro Top Door: 0: Open 1: Closed
							   if( 0 == TopDoorOpenStatus)
							   {
								  writeFileLog("[Java_Cris_Security_getDoorstatus()] Top Door close.");
								  writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");

								  pthread_mutex_unlock( &g_ComPortAccessmutex );

								  pthread_mutex_unlock( &g_getDoorstatusprocmutex );	

								  return 1; //close =1

							   }
							   else if( 1 == TopDoorOpenStatus )
							   {

								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Top Door open.");

								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");

								 pthread_mutex_unlock( &g_ComPortAccessmutex );

								 pthread_mutex_unlock( &g_getDoorstatusprocmutex );

								 return 0; //open =0
							   }
							   else 
							   {

								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Top Door status is unknown.");
								 writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");

								 pthread_mutex_unlock( &g_ComPortAccessmutex );

								 pthread_mutex_unlock( &g_getDoorstatusprocmutex );

								 return (-1); //unknown status 
							   }

		                                           break;
                                      default:
		                                        writeFileLog("[Java_Cris_Security_getDoorstatus()] No Door Type Defined.");
		                                        pthread_mutex_unlock( &g_ComPortAccessmutex );
							pthread_mutex_unlock( &g_getDoorstatusprocmutex );
							return (-1); //unknown status 
		                                        break;


                                 };//switch end
                      }//if( true == cctalkportopenflag ) 
                      else if( false == cctalkportopenflag )
		      {
		           pthread_mutex_unlock( &g_ComPortAccessmutex );
		           pthread_mutex_unlock( &g_getDoorstatusprocmutex );
			   writeFileLog("[Java_Cris_Security_getDoorstatus()]  Unable to get any door status due to cctalk port still not opened.");
		           writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");
		           #ifdef CRISAPI_TIMEOUT_TEST
			   clock_gettime(CLOCK_MONOTONIC, &endts);
			   diffts.tv_sec = endts.tv_sec - begints.tv_sec;
		           memset(log,'\0',100);
			   sprintf(log,"[Java_Cris_Security_getDoorstatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
			   writeAPITimeoutLog(log);
			   #endif
			   return  (-1); //failed operation
		      }
                    
                      #endif
 
                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      #if !defined(CCTALK_DEVICE) ||  !defined(ATVM_ALARM_VAULT)

		      writeFileLog("[Java_Cris_Security_getDoorstatus()]  CCTALK or ATVM_ALARM_VAULT not defined in atvmdevice headers files.");

		      pthread_mutex_unlock( &g_ComPortAccessmutex );

		      pthread_mutex_unlock( &g_getDoorstatusprocmutex );

		      writeFileLog("[Java_Cris_Security_getDoorstatus()] Exit.");

		      #ifdef CRISAPI_TIMEOUT_TEST
		      clock_gettime(CLOCK_MONOTONIC, &endts);
		      diffts.tv_sec = endts.tv_sec - begints.tv_sec;
		      memset(log,'\0',100);
		      sprintf(log,"[Java_Cris_Security_getDoorstatus()] Cycle complete in Seconds Elapsed=%d .",diffts.tv_sec ); 
		      writeAPITimeoutLog(log);
		      #endif
		      return  (-1); //failed operation

		      #endif

                      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

     
   
}//Java_Cris_Security_getDoorstatus() end

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
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

				 #ifdef B2B_NOTE_ACCEPTOR

			 if( true == g_activate_device_flag)
			 {
						  rtcode= GetValidNote(Denom,Timeout); 

				 }
			 else if( false  == g_activate_device_flag)
			 {
				
				 rtcode = -1;//Communication failure

			 }

			 #endif


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

				  #ifdef B2B_NOTE_ACCEPTOR

			 if( true == g_activate_device_flag)
			 {
						  rtcode= AcceptCurrentNote(Denom,Timeout); 

				 }
			 else if( false  == g_activate_device_flag)
			 {
				
				 rtcode = -1;//Communication failure

			 }
			 
				 #endif

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

				 #ifdef B2B_NOTE_ACCEPTOR

			 if( true == g_activate_device_flag)
			 {
						  rtcode= ReturnCurrentNote(Timeout); 

				 }
			 else if( false  == g_activate_device_flag)
			 {
				
				 rtcode = -1;//Communication failure

			 }
	 
				 #endif

			 
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

				 #ifdef B2B_NOTE_ACCEPTOR

			 if( true == g_activate_device_flag)
			 {
						rtcode= B2BHoldCurrentNote(Timeout); 
				 }
			 else if( false  == g_activate_device_flag)
			 {
				rtcode = -1; //Communication failure
				 }

				 #endif
			 
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

    static int ActivateCCTALKComPort()
	{
				  

			  writeFileLog("[ActivateCCTALKComPort()] Going to check CCTALK port open status.");

			  //port open must be done here
			  int  cctalkrtcode=-1;

			  bool cctalkportopenflag=false;
			  
			  cctalkrtcode=IsCCTALKPortOpen();

			  //CCTALK Port not opened
			  if(0==cctalkrtcode) 
			  {

					cctalkrtcode=-1;

					int CCTALKPortNumber=-1;
			 
					CCTALKPortNumber = GetCCTALKPortNo();

					if( CCTALKPortNumber < 0 )
				{
					
					  writeFileLog("[ActivateCCTALKComPort()] CCTALK Port Number wrong input."); 
								  return false; //failed operation
							}

					//Open CCTALK Port 
							cctalkrtcode=ActivateCCTALKPort(CCTALKPortNumber);

					if( 0 == cctalkrtcode )
				{

							 cctalkportopenflag=false;

							 writeFileLog("[ActivateCCTALKComPort()] CCTALK Open Port failed.");
					  

				}
				else if( 1 == cctalkrtcode )
				{  

					 cctalkportopenflag=true;

					 writeFileLog("[ActivateCCTALKComPort()] CCTALK Open Port opened successfully.");

				}
					

			 }
			 else if(1 == cctalkrtcode) //CCTALK Port opened already
			 {
			   
				cctalkportopenflag=true;
				 }


				 return cctalkportopenflag;


	}//ActivateCCTALKComPort() end
	JNIEXPORT jbyteArray JNICALL Java_com_vn_getArray(JNIEnv *env, jobject obj) {
	    jbyte byteUrl[] = {1,2,3,3,4};
	    int sizeByteUrl = 5;

	    jbyteArray data = (*env)->NewByteArray(env, sizeByteUrl);
	    if (data == NULL) {
		return NULL; //  out of memory error thrown
	    }

	    // creat bytes from byteUrl
	    jbyte *bytes = (*env)->GetByteArrayElements(env, data, 0);
	    int i;
	    for (i = 0; i < sizeByteUrl; i++) {
		bytes[i] = byteUrl[i];
	    }

	    // move from the temp structure to the java structure
	    (*env)->SetByteArrayRegion(env, data, 0, sizeByteUrl, bytes);

	    return data;
	}

	JNIEXPORT void JNICALL Java_Cris_Security_WriteNativeLog(  JNIEnv *env, jobject jobj, jstring jniLogMessage){

				   
		   
				   const char *LogMessage= (*env)->GetStringUTFChars(env,jniLogMessage,0);

			   //printf("\n [Java_Cris_Security_WriteNativeLog()] Log File Name : %s", LogMessage);

			   writeFileLog((char*)LogMessage);

			   (*env)->ReleaseStringUTFChars(env, jniLogMessage, LogMessage);


				   


	}//JNIEXPORT void JNICALL Java_Cris_Security_WriteNativeLog end
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++#endif
