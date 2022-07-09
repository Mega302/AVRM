#include "smartcardjni.h"
#include "atvmlog.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static pthread_mutex_t g_ComPortAccessmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_ConnectDeviceprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_AcceptCardprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_DisableCardAcceptanceprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_DisConnectDeviceprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_EnableCardAcceptanceprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_IsCardInChannelprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_IsCardRemovedprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_RejectCardprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_ReturnCardprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_DispenseCardprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_CollectCardprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_DeviceStatusprocmutex = PTHREAD_MUTEX_INITIALIZER;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef JNI_EXPORT_ENABLE 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void SetSmartCardLog(const char *ObjectdllPath)
{

                
                ///////////////////////////////////////////////////////////////////////////////////////////

                //SmartCardSetupLog(char *LogdllPathstr,char *LogFileName,char *deviceid)
                void *lib_handle;
 
                void (*fnptr)( char *LogdllPathstr, 
                               char *LogFileName,
                               char *deviceid,
                               int  fnLogfileMode)=NULL;
		      
		lib_handle = dlopen(ObjectdllPath,RTLD_NOW|RTLD_GLOBAL);
		
		if (NULL==lib_handle)
		{
		      //printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory pointer failed");
		      
		}
		else
		{
	                //success
		        //printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory pointer successfully done");

                        fnptr = dlsym(lib_handle, "SmartCardSetupLog");

			if (NULL == fnptr )  
			{
			       //printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory function pointer failed");
			       
			}
			else
			{ 
			      //printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory function pointer get success");

                              //Now set log parameter in so file
			      fnptr(  (char*)g_logdllpath,
                                      (char*)g_general_log,
                                      (char*)g_Deviceid,
                                      g_LogModeLevel
                                   );
			 
			      if(NULL!= lib_handle)
			      {
			             //dlclose(lib_handle);
                                     fnptr=NULL;

			      }
		
			}

			
		
		}
                
                /////////////////////////////////////////////////////////////////////////////////////////////


}//void SetSmartCardLog() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jboolean JNICALL Java_Cris_SmartCard_JniSetupSmartCardLogFile( JNIEnv   *env, 
                                                                         jclass   SmartCard,
                                                                         jstring  jniObjectdllPath,
                                                                         jstring  jniLogfile, 
                                                                         jint     jniLogLevel, 
                                                                         jstring  jniDeviceId,
                                                                         jstring  jniLogdllPath
                                                                       )
{


               //printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Entry");

               //////////////////////////////////////////////////////////////////////////////////////
               
               g_Ascrm_writeFileLog=NULL;
               g_lib_handle=NULL;
               const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);
               //printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Object dll path:%s",ObjectdllPath);

	           //Step 1:Log dll Path
               const char *LogdllPathstr= (*env)->GetStringUTFChars(env,jniLogdllPath,0);
               //printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Log dll Path : %s", LogdllPathstr);
               if(NULL == LogdllPathstr ){
                   printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Log dll Path is NULL");
               }else{
                   SetSoFilePath((unsigned char*) LogdllPathstr);
               }//else end

	           //Step 2:Log File Name
	           const char *LogFileName= (*env)->GetStringUTFChars(env,jniLogfile,0);
               //++printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Log File Name : %s", LogFileName);
               if(NULL == LogFileName ){
                    printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Log file Path is NULL");
               }else{
                    SetGeneralFileLogPath( (unsigned char*) LogFileName );
               }
               //Step 3: Set LogLevel Mode
               g_LogModeLevel = (int)jniLogLevel;
               //++printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] LogMode Level: %d",g_LogModeLevel);
               
               //Step 4: Get Device id
               const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);
               //++printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] SmartCard Device ID : %s", deviceid);
               if(NULL == deviceid ){
                  printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] device id is NULL");
               }else{
                   strcpy(g_Deviceid,deviceid);
               }

               
               ///////////////////////////////////////////////////////////////////////////////////////////

               /*

                //SmartCardSetupLog(char *LogdllPathstr,char *LogFileName,char *deviceid)
                void *lib_handle;
 
                void (*fnptr)( char *LogdllPathstr, 
                               char *LogFileName,
                               char *deviceid,
                               int fnLogfileMode)=NULL;
					  
				lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
				
				if (NULL==lib_handle)
				{
					  printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory pointer failed");
					  
				}
				else
				{
							   //success
						       printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory pointer successfully done");

								fnptr = dlsym(lib_handle, "SmartCardSetupLog");

								if (NULL == fnptr )  
								{
									   printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory function pointer failed");
									   
								}
								else
								{ 
							  printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Get so memory function pointer get success");

										  //Now set log parameter in so file
							  fnptr(  (char*)LogdllPathstr,
												  (char*)LogFileName,
												  (char*)deviceid,
												  g_LogModeLevel
											   );
						 
							  if(NULL!= lib_handle)
							  {
								   //dlclose(lib_handle);
								   fnptr=NULL;

							  }
					
						}

						
					
					}//if( NULL == lib_handle ) end

               */
		
                /////////////////////////////////////////////////////////////////////////////////////////////

                (*env)->ReleaseStringUTFChars(env, jniLogdllPath, LogdllPathstr);
                (*env)->ReleaseStringUTFChars(env, jniLogfile,    LogFileName);
                (*env)->ReleaseStringUTFChars(env, jniDeviceId,   deviceid);
                //++printf("\n[Java_Cris_SmartCard_JniSetupSmartCardLogFile()] Exit"); 
                return true;

}//JNIEXPORT jboolean JNICALL Java_Cris_SmartCard_SetupSmartCardLogFile end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

JNIEXPORT void JNICALL Java_Cris_SmartCard_WriteNativeLog( JNIEnv *env, 
                                                           jobject jobj, 
                                                           jstring jniLogMessage
                                                         )
{

               
       
               const char *LogMessage= (*env)->GetStringUTFChars(env,jniLogMessage,0);

	       //printf("\n [Java_Cris_SmartCard_WriteNativeLog()] Log File Name : %s", LogMessage);

	       //writeFileLog((char*)LogMessage);

	       (*env)->ReleaseStringUTFChars(env, jniLogMessage, LogMessage);


               


}//JNIEXPORT void JNICALL Java_Cris_Currency_WriteNativeLog end

*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniGetLastError( JNIEnv   *env, 
		                                            jobject  jobj,
		                                            jstring  jniObjectdllPath,
		                                            jstring  jniLogfilePath, 
		                                            jint     jniLogLevel, 
		                                            jstring  jniDeviceId,
		                                            jstring  jniLogdllPath)
{


        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)(char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

       

        //printf("\n[Java_Cris_SmartCard_JniGetLastError()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      //printf("\n[Java_Cris_SmartCard_JniGetLastError()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 7; //failed
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_JniGetLastError()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "GetSmartCardLastError");

		if (NULL == fnptr )  
		{
		       //printf("\n[Java_Cris_SmartCard_JniGetLastError()] Get so memory function pointer failed");
		       return 7;
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_JniGetLastError()] Get so memory function pointer get success");

		        //Now Write Log through so file
			rtcode  = fnptr( (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_JniGetLastError()] function call return code: %d",rtcode);

                        

			if(NULL!= lib_handle)
			{
                             //dlclose(lib_handle);
			     fnptr=NULL;
                        }
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
       

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_JniGetLastError()] GetLastError() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //printf("\n[Java_Cris_SmartCard_JniGetLastError()] Exit");

        return( rtcode  );
   

}//Java_Cris_SmartCard_JniGetLastError end

///////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_GetCardBlockStatus( JNIEnv *env, 
                                                               jobject jobj,
                                                               jstring  jniObjectdllPath,
                                                               jstring  jniLogfilePath, 
                                                               jint     jniLogLevel, 
                                                               jstring  jniDeviceId,
                                                               jstring  jniLogdllPath
                                                             )
{


        //writeFileLog("[Java_Cris_SmartCard_GetCardBlockStatus()] Entry");

        //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)(char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 7; //failed
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "GetCardBlockStatus");

		if (NULL == fnptr )  
		{
		       //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] Get so memory function pointer failed");
		       return 7;
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] Get so memory function pointer get success");

		        //Now Write Log through so file
			rtcode  = fnptr( (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] function call return code: %d",rtcode);

                        

			if(NULL!= lib_handle)
			{
                             //dlclose(lib_handle);
			     fnptr=NULL;
                        }
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
       

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_GetCardBlockStatus()] ConnectDevice_c() return Code: %d ",rtcode );
           
        ////writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_GetCardBlockStatus()] Exit");

        //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] Exit");

        return( rtcode  );



}//Java_Cris_SmartCard_GetCardBlockStatus() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_CardBlockStatusRequest( JNIEnv   *env, 
                                                                   jobject  jobj,
                                                                   jstring  jniObjectdllPath,
                                                                   jstring  jniLogfilePath, 
                                                                   jint     jniLogLevel, 
                                                                   jstring  jniDeviceId,
                                                                   jstring  jniLogdllPath
                                                                )
{


        //writeFileLog("[Java_Cris_SmartCard_CardBlockStatusRequest()] Entry");

        //printf("\n[Java_Cris_SmartCard_CardBlockStatusRequest()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)(char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

       

        //printf("\n[Java_Cris_SmartCard_CardBlockStatusRequest()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      //printf("\n[Java_Cris_SmartCard_CardBlockStatusRequest()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 7; //failed
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_CardBlockStatusRequest()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "CardBlockStatusRequest");

		if (NULL == fnptr )  
		{
		       //printf("\n[Java_Cris_SmartCard_CardBlockStatusRequest()] Get so memory function pointer failed");
		       return 7;
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_CardBlockStatusRequest()] Get so memory function pointer get success");

		        //Now Write Log through so file
			rtcode  = fnptr( (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_CardBlockStatusRequest()] function call return code: %d",rtcode);

                        

			if(NULL!= lib_handle)
			{
                             //dlclose(lib_handle);
			     fnptr=NULL;
                        }
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
       

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_CardBlockStatusRequest()] ConnectDevice_c() return Code: %d ",rtcode );
           
        ////writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_CardBlockStatusRequest()] Exit");

        //printf("\n[Java_Cris_SmartCard_GetCardBlockStatus()] Exit");

        return( rtcode  );



}//Java_Cris_SmartCard_GetCardBlockStatus() end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithCardRetain( JNIEnv *env, 
                                                               jobject jobj,
                                                               jstring  jniObjectdllPath,
                                                               jstring  jniLogfilePath, 
                                                               jint     jniLogLevel, 
                                                               jstring  jniDeviceId,
                                                               jstring  jniLogdllPath
                                                             )
{


        //writeFileLog("[Java_Cris_SmartCard_MutekInitWithCardRetain()] Entry");

        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)(char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

       

        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 7; //failed
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "MutekInitWithCardRetain");

		if (NULL == fnptr )  
		{
		       //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] Get so memory function pointer failed");
		       return 7;
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] Get so memory function pointer get success");

		        //Now Write Log through so file
			rtcode  = fnptr( (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] function call return code: %d",rtcode);

                        

			if(NULL!= lib_handle)
			{
                             //dlclose(lib_handle);
			     fnptr=NULL;
                        }
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
       

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_MutekInitWithCardRetain()] ConnectDevice_c() return Code: %d ",rtcode );
           
        ////writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_MutekInitWithCardRetain()] Exit");

        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] Exit");

        return( rtcode  );



}//Java_Cris_SmartCard_MutekInitWithCardRetain() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithCardMoveInFront( JNIEnv *env, 
                                                               jobject jobj,
                                                               jstring  jniObjectdllPath,
                                                               jstring  jniLogfilePath, 
                                                               jint     jniLogLevel, 
                                                               jstring  jniDeviceId,
                                                               jstring  jniLogdllPath
                                                             )
{


        //writeFileLog("[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] Entry");

        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)(char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

       

        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      //printf("\n[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 7; //failed
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "MutekInitWithCardMoveInFront");

		if (NULL == fnptr )  
		{
		       //printf("\n[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] Get so memory function pointer failed");
		       return 7;
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardRetain()] Get so memory function pointer get success");

		        //Now Write Log through so file
			rtcode  = fnptr( (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] function call return code: %d",rtcode);

                        

			if(NULL!= lib_handle)
			{
                             //dlclose(lib_handle);
			     fnptr=NULL;
                        }
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
       

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] ConnectDevice_c() return Code: %d ",rtcode );
           
        ////writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] Exit");

        //printf("\n[Java_Cris_SmartCard_MutekInitWithCardMoveInFront()] Exit");

        return( rtcode  );



}//Java_Cris_SmartCard_MutekInitWithCardMoveInFront() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithErrorCardBin( JNIEnv *env, 
                                                               jobject jobj,
                                                               jstring  jniObjectdllPath,
                                                               jstring  jniLogfilePath, 
                                                               jint     jniLogLevel, 
                                                               jstring  jniDeviceId,
                                                               jstring  jniLogdllPath
                                                             )
{


        //writeFileLog("[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Entry");

        //printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)(char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

       

        //printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      //printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 7; //failed
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "MutekInitWithErrorCardBin");

		if (NULL == fnptr )  
		{
		       //printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Get so memory function pointer failed");
		       return 7;
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Get so memory function pointer get success");

		        //Now Write Log through so file
			rtcode  = fnptr( (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] function call return code: %d",rtcode);

                        

			if(NULL!= lib_handle)
			{
                             //dlclose(lib_handle);
			     fnptr=NULL;
                        }
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
       

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] ConnectDevice_c() return Code: %d ",rtcode );
           
        ////writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Exit");

        //printf("\n[Java_Cris_SmartCard_MutekInitWithErrorCardBin()] Exit");

        return( rtcode  );



}//Java_Cris_SmartCard_MutekInitWithErrorCardBin() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 
JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniConnectDevice( JNIEnv *env, 
                                                             jobject jobj,
                                                             jstring  jniObjectdllPath,
                                                             jstring  jniLogfilePath, 
                                                             jint     jniLogLevel, 
                                                             jstring  jniDeviceId,
                                                             jstring  jniLogdllPath, 
                                                             jint PortId, 
                                                             jint ChannelClearanceMode, 
                                                             jint Timeout
                                                           )
{


        //writeFileLog("[Java_Cris_SmartCard_ConnectDevice()] Entry");

        //++printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)(int  PortId, 
                     int  ChannelClearanceMode, 
                     int  Timeout,
                     char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

       

        //printf("\n[Java_Cris_SmartCard_JniConnectDevice()] dll path:%s",ObjectdllPath);

        lib_handle = NULL;
        
        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL );
        
        if (NULL==lib_handle)
        {
	           printf("\n[Java_Cris_SmartCard_JniConnectDevice()] ObjectdllPath Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	          return 31; //Other Error
        }
        else
        {
		         //success
                //++printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "ConnectDevice_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer failed");
		       return 31; //Other Error
		}
		else
		{ 
		        //++printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer get success");

		        //Now Write Log through so file
			rtcode  = fnptr( PortId,
				         ChannelClearanceMode,
				         Timeout,
				         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//++printf("\n[Java_Cris_SmartCard_JniConnectDevice()] function call return code: %d",rtcode);

                        

			if(NULL!= lib_handle)
			{
                             //dlclose(lib_handle);
			     fnptr=NULL;
                        }
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
      
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_ConnectDevice()] ConnectDevice_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_ConnectDevice()] Exit");

        //++printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Exit");

        return( rtcode  );



}//Java_Cris_SmartCard_ConnectDevice() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniDisConnectDevice( JNIEnv *env, 
                                                                jobject jobj, 
                                                                jstring  jniObjectdllPath,
                                                                jstring  jniLogfilePath, 
                                                                jint     jniLogLevel, 
                                                                jstring  jniDeviceId,
                                                                jstring  jniLogdllPath, 
                                                                jint Timeout
                                                              )
{

        //writeFileLog("[Java_Cris_SmartCard_DisConnectDevice()] Entry");

        //printf("[Java_Cris_SmartCard_DisConnectDevice()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);
         
        //++printf("\n[Java_Cris_SmartCard_JniDeactivateCard()] Device DLLPATH:%s",ObjectLogdllPath);
        
        //printf("\n[Java_Cris_SmartCard_JniDeactivateCard()] Device ID:%s",ObjectDeviceId);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int   Timeout,
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int   fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_DisConnectDevice()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      //printf("\n[Java_Cris_SmartCard_DisConnectDevice()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "DisConnectDevice_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_DisConnectDevice()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout,
				         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_DisConnectDevice()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);

				//printf("\n[Java_Cris_SmartCard_DisConnectDevice()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
        
        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_DisConnectDevice()] DisConnectDevice_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_DisConnectDevice()] Exit");

        //printf("[Java_Cris_SmartCard_DisConnectDevice()] Exit");

        return( rtcode );




}//Java_Cris_SmartCard_DisConnectDevice() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Java_Cris_SmartCard_GetDeviceStatus End
JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_JniGetDeviceStatus( JNIEnv  *env, 
                                                                     jobject  jobj,
                                                                     jstring  jniObjectdllPath,
                                                                     jstring  jniLogfilePath, 
                                                                     jint     jniLogLevel, 
                                                                     jstring  jniDeviceId,
                                                                     jstring  jniLogdllPath,  
                                                                     jint     ComponentId, 
                                                                     jint     Timeout
                                                                )
{


             int rtcode =-1;

             char ReplyDeviceStatus[DEVICE_STATUS_LNEGTH ];

             memset( ReplyDeviceStatus,0,DEVICE_STATUS_LNEGTH  );

             int counter = 0;

             jbyteArray ReturnActivateArray = NULL;
             
             jbyte *ByteArrayElement= NULL;
            
             char log[LOG_ARRAY_SIZE];
             
             memset(log,'\0',LOG_ARRAY_SIZE);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
		const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

		const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

		const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
		void *lib_handle;
	 
		int (*fnptr)( int ComponentId , 
                              int Timeout,
		              char *ReplyDeviceStatus,
		              char *LogdllPathstr,
		              char *LogFileName,
		              char *deviceid,
		              int  fnLogfileMode)=NULL;

		const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

		//printf("\n[Java_Cris_SmartCard_JniGetDeviceStatus()] dll path:%s",ObjectdllPath);

		lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
		if (NULL==lib_handle)
		{
		      printf("\n[Java_Cris_SmartCard_JniGetDeviceStatus()] Get so memory pointer failed");

		      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

		      return NULL; //failed
		}
		else
		{
			//success
		        //printf("\n[Java_Cris_SmartCard_JniGetDeviceStatus()] Get so memory pointer successfully done");
		        
		        fnptr = dlsym(lib_handle, "DeviceStatus_C");

			if (NULL == fnptr )  
			{
			       printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer failed");
			       return NULL;
			}
			else
			{ 
				//printf("\n[Java_Cris_SmartCard_JniGetDeviceStatus()] Get so memory function pointer get success");
				//Now Write Log through so file
				rtcode  = fnptr( ComponentId , 
                                                 Timeout,
		                                 ReplyDeviceStatus,
                                                 (char*)ObjectLogdllPath,
				                 (char*)ObjectLogFilePath,
				                 (char*)ObjectDeviceId,
				                 jniLogLevel
					       );
			 
				//printf("\n[Java_Cris_SmartCard_JniGetDeviceStatus()] function call return code: %d",rtcode);


				if(NULL!= lib_handle)
				{
                                        //dlclose(lib_handle);

					//printf("\n[Java_Cris_SmartCard_JniGetDeviceStatus()] dllclose return code:%d",dlclose(lib_handle)); 
					fnptr=NULL;

				}
		
			}//else block end
		
		}//if( NULL == lib_handle ) end

		(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

                (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

                (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

                (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
		
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

             //create new byte array
             ReturnActivateArray = (*env)->NewByteArray( env, DEVICE_STATUS_LNEGTH );

             //get newly created byte array element
             ByteArrayElement= (*env)->GetByteArrayElements( env,ReturnActivateArray,0 );

             for(counter=0;counter<DEVICE_STATUS_LNEGTH;counter++)
             {

                     ByteArrayElement[counter] = ReplyDeviceStatus[counter];
                     
                     //memset(log,'\0',LOG_ARRAY_SIZE);

                     //sprintf(log,"[Java_Cris_SmartCard_JniGetDeviceStatus()] ByteArrayElement[%d] = 0x%xh. ",counter,ByteArrayElement[counter]);
                     ////writeFileLog(log);

             }
           
             //now release byte array elements
             (*env)->ReleaseByteArrayElements(env,ReturnActivateArray,ByteArrayElement,0);

             //writeFileLog("[Java_Cris_SmartCard_JniGetDeviceStatus()] Exit .");

             return ReturnActivateArray;




}//Java_Cris_SmartCard_JniGetDeviceStatus() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniAcceptCard( JNIEnv *env,  
                                                       jobject jobj,
                                                       jstring  jniObjectdllPath,
                                                       jstring  jniLogfilePath, 
                                                       jint     jniLogLevel, 
                                                       jstring  jniDeviceId,
                                                       jstring  jniLogdllPath,  
                                                       jint Timeout
                                                     )
{

        //writeFileLog("[Java_Cris_SmartCard_AcceptCard()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_JniConnectDevice()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; ////Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "AcceptCard_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer failed");
		       return 31;//Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_JniConnectDevice()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);
				//printf("\n[Java_Cris_SmartCard_JniConnectDevice()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
		
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_AcceptCard()] AcceptCard_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_AcceptCard()] Exit");

        return rtcode;



}//Java_Cris_SmartCard_AcceptCard() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniDisableCardAcceptance( JNIEnv   *env, 
                                                                     jobject  jobj,
                                                                     jstring  jniObjectdllPath,
                                                                     jstring  jniLogfilePath, 
                                                                     jint     jniLogLevel, 
                                                                     jstring  jniDeviceId,
                                                                     jstring  jniLogdllPath,   
                                                                     jint     Timeout
                                                                )
{
     
        //writeFileLog("[Java_Cris_SmartCard_DisableCardAcceptance()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_DisableCardAcceptance()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_DisableCardAcceptance()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_DisableCardAcceptance()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "DisableCardAcceptance_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_DisableCardAcceptance()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_DisableCardAcceptance()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_DisableCardAcceptance()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);
				//printf("\n[Java_Cris_SmartCard_DisableCardAcceptance()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        
        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        


        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_DisableCardAcceptance()] DisableCardAcceptance_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_DisableCardAcceptance()] Exit");

        return( rtcode );


}//Java_Cris_SmartCard_DisableCardAcceptance() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniEnableCardAcceptance( JNIEnv *env, 
                                                                 jobject jobj,
                                                                 jstring  jniObjectdllPath,
                                                                 jstring  jniLogfilePath, 
                                                                 jint     jniLogLevel, 
                                                                 jstring  jniDeviceId,
                                                                 jstring  jniLogdllPath,    
                                                                 jint Timeout
                                                               )
{

        //writeFileLog("[Java_Cris_SmartCard_EnableCardAcceptance()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

        //++printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] Device DLLPATH:%s",ObjectLogdllPath);
        
        //printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] Device ID:%s",ObjectDeviceId);
            
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

	      return 31; //Other error

        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "EnableCardAcceptance_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] Get so memory function pointer failed");

		       return 31; //Other error

		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);
				//printf("\n[Java_Cris_SmartCard_EnableCardAcceptance()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        
        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        


        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_EnableCardAcceptance()]  EnableCardAcceptance_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_EnableCardAcceptance()] Exit");

        return( rtcode );


}//Java_Cris_SmartCard_EnableCardAcceptance() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniIsCardInChannel(JNIEnv *env, jobject jobj, 
                                                             jstring  jniObjectdllPath,
                                                             jstring  jniLogfilePath, 
                                                             jint     jniLogLevel, 
                                                             jstring  jniDeviceId,
                                                             jstring  jniLogdllPath, jint Timeout)
{

        //writeFileLog("[Java_Cris_SmartCard_IsCardInChannel()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_IsCardInChannel()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_IsCardInChannel()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_IsCardInChannel()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "IsCardInChannel_c");

		if (NULL == fnptr )  
		{
		       //printf("\n[Java_Cris_SmartCard_IsCardInChannel()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_IsCardInChannel()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_IsCardInChannel()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{

                                //dlclose(lib_handle);

				//printf("\n[Java_Cris_SmartCard_IsCardInChannel()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end
        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_IsCardInChannel()]  IsCardInChannel_c() return Code: %d ",rtcode );
           
        ////writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_IsCardInChanne()] Exit");

        return( rtcode );



}//Java_Cris_SmartCard_IsCardInChannel() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniIsCardRemoved( JNIEnv *env, jobject jobj, 
                                                             jstring  jniObjectdllPath,
                                                             jstring  jniLogfilePath, 
                                                             jint     jniLogLevel, 
                                                             jstring  jniDeviceId,
                                                             jstring  jniLogdllPath,jint Timeout)
{


        //writeFileLog("[Java_Cris_SmartCard_IsCardRemoved()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_IsCardRemoved()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_IsCardRemoved()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_IsCardRemoved()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "IsCardRemoved_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_IsCardRemoved()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_IsCardRemoved()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_IsCardRemoved()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);

				//printf("\n[Java_Cris_SmartCard_IsCardRemoved()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end

        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_IsCardRemoved()]  IsCardRemoved_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_IsCardRemoved()] Exit");

        return( rtcode );



}//Java_Cris_SmartCard_IsCardRemoved() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniCollectCard( JNIEnv  *env, 
                                                           jobject  jobj, 
                                                           jstring  jniObjectdllPath,
                                                           jstring  jniLogfilePath, 
                                                           jint     jniLogLevel, 
                                                           jstring  jniDeviceId,
                                                           jstring  jniLogdllPath,
                                                           jint     Timeout
                                                         )
{

        //writeFileLog("[Java_Cris_SmartCard_CollectCard()] Entry");
        
        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_CollectCard()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_CollectCard()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_CollectCard()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "CollectCard_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_CollectCard()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_CollectCard()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_CollectCard()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);

				//printf("\n[Java_Cris_SmartCard_CollectCard()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end

        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_CollectCard()] CollectCard_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_CollectCard()] Exit");

        return( rtcode );

}//Java_Cris_SmartCard_CollectCard() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniDispenseCard( JNIEnv *env, 
                                                            jobject jobj,
                                                            jstring  jniObjectdllPath,
                                                             jstring  jniLogfilePath, 
                                                             jint     jniLogLevel, 
                                                             jstring  jniDeviceId,
                                                             jstring  jniLogdllPath,
                                                            jint Timeout)
{
 
        //writeFileLog("[Java_Cris_SmartCard_DispenseCard()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_DispenseCard()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_DispenseCard()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_DispenseCard()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "DispenseCard_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_DispenseCard()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_DispenseCard()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_DispenseCard()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);

				//printf("\n[Java_Cris_SmartCard_DispenseCard()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end

        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        
        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        

        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_DispenseCard()]  DispenseCard_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_DispenseCard()] Exit");

        return( rtcode );


}//Java_Cris_SmartCard_DispenseCard() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniRejectCard( JNIEnv *env, 
                                                          jobject jobj, 
                                                          jstring  jniObjectdllPath,
                                                             jstring  jniLogfilePath, 
                                                             jint     jniLogLevel, 
                                                             jstring  jniDeviceId,
                                                             jstring  jniLogdllPath,jint Timeout)
{

      
        //writeFileLog("[Java_Cris_SmartCard_RejectCard()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_RejectCard()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_RejectCard()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_RejectCard()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "RejectCard_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_RejectCard()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_RejectCard()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			//printf("\n[Java_Cris_SmartCard_RejectCard()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);

				//printf("\n[Java_Cris_SmartCard_RejectCard()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end

        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
        
        
        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        


        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_RejectCard()]  RejectCard_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_RejectCard()] Exit");

        return( rtcode );



}//Java_Cris_SmartCard_RejectCard() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniReturnCard (JNIEnv *env, jobject jobj, jstring  jniObjectdllPath,
                                                             jstring  jniLogfilePath, 
                                                             jint     jniLogLevel, 
                                                             jstring  jniDeviceId,
                                                             jstring  jniLogdllPath,jint DispenseMode,jint Timeout)
{
  
        //writeFileLog("[Java_Cris_SmartCard_ReturnCard()] Entry");

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        void *lib_handle;
 
        int (*fnptr)( int  DispenseMode,
                      int  Timeout, 
                      char *LogdllPathstr,
                      char *LogFileName,
                      char *deviceid,
                      int  fnLogfileMode)=NULL;

        const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

        //printf("\n[Java_Cris_SmartCard_ReturnCard()] dll path:%s",ObjectdllPath);

        lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
        
        if (NULL==lib_handle)
        {
	      printf("\n[Java_Cris_SmartCard_ReturnCard()] Get so memory pointer failed");
              (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
	      return 31; //Other error
        }
        else
        {
		//success
                //printf("\n[Java_Cris_SmartCard_ReturnCard()] Get so memory pointer successfully done");
                
                fnptr = dlsym(lib_handle, "ReturnCard_c");

		if (NULL == fnptr )  
		{
		       printf("\n[Java_Cris_SmartCard_ReturnCard()] Get so memory function pointer failed");
		       return 31; //Other error
		}
		else
		{ 
		        //printf("\n[Java_Cris_SmartCard_ReturnCard()] Get so memory function pointer get success");
		        //Now Write Log through so file
			rtcode  = fnptr( DispenseMode,
                                         Timeout, 
                                         (char*)ObjectLogdllPath,
				         (char*)ObjectLogFilePath,
				         (char*)ObjectDeviceId,
				         jniLogLevel
				       );
		 
			printf("\n[Java_Cris_SmartCard_ReturnCard()] function call return code: %d",rtcode);


			if(NULL!= lib_handle)
			{
                                //dlclose(lib_handle);

				//printf("\n[Java_Cris_SmartCard_ReturnCard()] dllclose return code:%d",dlclose(lib_handle)); 
				fnptr=NULL;

			}
		
		}//else block end

        
        }//if( NULL == lib_handle ) end

        (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

        
        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

        (*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

        (*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        


        //memset(log,'\0',LOG_ARRAY_SIZE);

        //sprintf(log,"[Java_Cris_SmartCard_ReturnCard()] ReturnCard_c() return Code: %d ",rtcode );
           
        //writeFileLog(log);

        //writeFileLog("[Java_Cris_SmartCard_ReturnCard()] Exit");

        return( rtcode );



}//Java_Cris_SmartCard_ReturnCard() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniSAMSlotPowerOnOff( JNIEnv *env, 
                                                                 jobject jobj,
                                                                 jstring  jniObjectdllPath,
                                                             jstring  jniLogfilePath, 
                                                             jint     jniLogLevel, 
                                                             jstring  jniDeviceId,
                                                             jstring  jniLogdllPath, 
                                                                 jint SAMSlotId,
                                                                 jint PowerOnOffState
                                                               )
{

       //writeFileLog("[Java_Cris_SmartCard_SAMSlotPowerOnOff()] Entry ");
 
       int rtcode = 1;

       //writeFileLog("[Java_Cris_SmartCard_SAMSlotPowerOnOff ()] Exit ");

       return rtcode;



}//Java_Cris_SmartCard_SAMSlotPowerOnOff() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_JniActivateCard( JNIEnv *env, 
                                                               jobject jobj,
                                                               jstring jniObjectdllPath, 
                                                               jint CardTechType,
                                                               jint SAMSlotId,
                                                               jint Timeout
                                                             )
{

            //writeFileLog("[Java_Cris_SmartCard_ActivateCard()] Entry ");


            jbyteArray ReturnActivateArray = NULL;
            jbyte *ByteArrayElement=NULL;
            int rtcode =-1,counter=0;
            char APIReply[100];
            memset(APIReply,'\0',100);
            char log[LOG_ARRAY_SIZE];
            memset(log,'\0',LOG_ARRAY_SIZE);
            int CommandAPDUlength=10;
            char SAMAPIReply[10];
            memset(SAMAPIReply,'\0',10);
            /////////////////////////////////////////////////////////////////////////////////////////////////
            
            //create new byte array
            ReturnActivateArray = (*env)->NewByteArray(env,CommandAPDUlength);

            //get newly created byte array element
            ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnActivateArray,0);

            /////////////////////////////////////////////////////////////////////////////////////////////////

            //Now call our c api
            if( 0 == CardTechType ) //0 =ContactLess card
            {

                  ActivateContactLessCard(APIReply,&CommandAPDUlength);

                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] APIReply[%d] = 0x%xh. ",counter,APIReply[counter]);
           
                     //writeFileLog(log);

                  }

                  //copy all data
		  for(counter=0;counter<CommandAPDUlength;counter++)
		  {

		       ByteArrayElement[counter] = APIReply[counter];
		    
		  }

                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] ByteArrayElement[%d] = 0x%xh. ",counter,ByteArrayElement[counter]);
           
                     //writeFileLog(log);

                  }


            }//if( 0 == CardTechType )
            else if( 1 == CardTechType ) //1= Contact card
            {
            
                  
                  ActivateSAMCard(SAMSlotId,Timeout,SAMAPIReply);
                  
                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] SAMAPIReply[%d] = 0x%xh. ",counter,SAMAPIReply[counter]);
           
                     //writeFileLog(log);

                  }

                  //copy all data
		  for(counter=0;counter<10;counter++)
		  {
                     ByteArrayElement[counter] = SAMAPIReply[counter];
		  }

                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] ByteArrayElement[%d] = 0x%xh. ",counter,ByteArrayElement[counter]);

                     //writeFileLog(log);

                  }
           

            }//else if( 1 == CardTechType )
            
 
            //////////////////////////////////////////////////////////////////////////////////////////////////

            //now release byte array elements
            (*env)->ReleaseByteArrayElements(env,ReturnActivateArray,ByteArrayElement,0);

            //writeFileLog("[Java_Cris_SmartCard_ActivateCard()] Exit ");

            return ReturnActivateArray;



}//Java_Cris_SmartCard_ActivateCard() end

*/

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_JniActivateCard( JNIEnv  *env, 
                                                                  jobject  jobj,
                                                                  jstring  jniObjectdllPath,
                                                                  jstring  jniLogfilePath, 
                                                                  jint     jniLogLevel, 
                                                                  jstring  jniDeviceId,
                                                                  jstring  jniLogdllPath, 
                                                                  jint     CardTechType,
                                                                  jint     SAMSlotId,
                                                                  jint     Timeout
                                                             )
{

            //++writeFileLog("[Java_Cris_SmartCard_ActivateCard()] Entry ");
            
            
            
            
            jbyteArray ReturnActivateArray = NULL;
            jbyte *ByteArrayElement=NULL;
            int rtcode =-1,counter=0;
            char APIReply[100];
            memset(APIReply,'\0',100);
            char log[LOG_ARRAY_SIZE];
            memset(log,'\0',LOG_ARRAY_SIZE);
            int CommandAPDUlength=10;
            char SAMAPIReply[10];
            memset(SAMAPIReply,'\0',10);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
            const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

            const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

            const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

            //++printf("\n[Java_Cris_SmartCard_ActivateCard()] Device DLLPATH:%s",ObjectLogdllPath);
            //printf("\n[Java_Cris_SmartCard_ActivateCard()] Device ID:%s",ObjectDeviceId);
            
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            //create new byte array
            ReturnActivateArray = (*env)->NewByteArray(env,CommandAPDUlength);

            //get newly created byte array element
            ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnActivateArray,0);

            /////////////////////////////////////////////////////////////////////////////////////////////////

            //Now call our c api
            if( 0 == CardTechType ) //0 =ContactLess card
            {

                        //ActivateContactLessCard(APIReply,&CommandAPDUlength);
                  
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( char *APIReply,
                          int *ReplyAPDULength, 
						  char *LogdllPathstr,
						  char *LogFileName,
						  char *deviceid,
						  int  fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_ActivateCard()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
			
			//++lib_handle = dlopen(ObjectdllPath, RTLD_NODELETE|RTLD_NOLOAD );
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      return NULL; //failed
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "ActivateContactLessCard");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory function pointer failed");
				       return NULL;
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( APIReply,
                                                         &CommandAPDUlength,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_ActivateCard()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_ActivateCard()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);

                        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
		
	          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] APIReply[%d] = 0x%xh. ",counter,(unsigned char)APIReply[counter]);
           
                     //writeFileLog(log);

                  }

                  //copy all data
		  for(counter=0;counter<CommandAPDUlength;counter++)
		  {

		       ByteArrayElement[counter] = APIReply[counter];
		    
		  }

                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] ByteArrayElement[%d] = 0x%xh. ",counter,(unsigned char)ByteArrayElement[counter]);
                     //writeFileLog(log);
    
                  }


            }//if( 0 == CardTechType )
            else if( 1 == CardTechType ) //1= Contact card
            {
            
                  
                  //ActivateSAMCard(SAMSlotId,Timeout,SAMAPIReply);
                  
                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( int  SAMSlotId,
                                      int  Timeout,
                                      char *SAMAPIReply,
                                      char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int  fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_ActivateCard()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      
                              return NULL; //failed
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "ActivateSAMCard");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory function pointer failed");
				       return NULL;
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_ActivateCard()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( SAMSlotId,
                                                         Timeout,
                                                         SAMAPIReply,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_ActivateCard()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_ActivateCard()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
                        
			(*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath); 
                      
		
		  
                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
                  
                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] SAMAPIReply[%d] = 0x%xh. ",counter,(unsigned char)SAMAPIReply[counter]);
           
                     //writeFileLog(log);

                  }

                  //copy all data
		  for(counter=0;counter<10;counter++)
		  {
                     ByteArrayElement[counter] = SAMAPIReply[counter];
		  }

                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] ByteArrayElement[%d] = 0x%xh. ",counter,(unsigned char)ByteArrayElement[counter]);
                     //writeFileLog(log);

                  }
           

            }//else if( 1 == CardTechType )
            
 
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////

            //now release byte array elements
            (*env)->ReleaseByteArrayElements(env,ReturnActivateArray,ByteArrayElement,0);

            ////writeFileLog("[Java_Cris_SmartCard_ActivateCard()] Exit ");

            return ReturnActivateArray;



}//Java_Cris_SmartCard_ActivateCard() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniDeactivateCard(  JNIEnv *env, 
                                                               jobject jobj, 
                                                               jstring  jniObjectdllPath,
                                                               jstring  jniLogfilePath, 
                                                               jint     jniLogLevel, 
                                                               jstring  jniDeviceId,
                                                               jstring  jniLogdllPath, 
                                                               jint     CardTechType,
                                                               jint     SAMSlotId,
                                                               jint     Timeout
                                                            )
{

		int Reply =-1;

                int rtcode =-1;

                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
                const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

                const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

                const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);
                
               

               /////////////////////////////////////////////////////////////////////////////////////////

		if( 0 == CardTechType ) //0 =ContactLess card
		{

		    //DeActivateContactLessCard(&Reply) ;
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( int *Reply, 
                                      char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int  fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_DeactivateCard()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      
                              return 31; ////Other error
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "DeActivateContactLessCard");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory function pointer failed");
				       return 31; //Other error
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( &Reply,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_DeactivateCard()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_DeactivateCard()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
		   
                        
			(*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);  
   
		   
                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

		}
		else if( 1 == CardTechType ) //1 =Contact card
		{

		    //DeActivateContactCard(&Reply);
                      
                       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( int *Reply, 
                                      char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int  fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_DeactivateCard()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      
                              return 7; //failed
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "DeActivateContactCard");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory function pointer failed");
				       return 7;
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_DeactivateCard()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( &Reply,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_DeactivateCard()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_DeactivateCard()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
		
		        
			(*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

		}

		return (Reply);



}//Java_Cris_SmartCard_DeactivateCard() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_JniXChangeAPDU( JNIEnv *env, 
                                                                 jobject jobj,
                                                                 jstring    jniObjectdllPath,
                                                                 jstring    jniLogfilePath, 
                                                                 jint       jniLogLevel, 
                                                                 jstring    jniDeviceId,
                                                                 jstring    jniLogdllPath,  
                                                                 jint       CardTechType, 
                                                                 jbyteArray CommandAPDU, 
                                                                 jint       SAMSlotId,
                                                                 jint       Timeout
                                                            )

{

            //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] Entry");

            char log[LOG_ARRAY_SIZE];

            memset(log,'\0',LOG_ARRAY_SIZE);

            char ReplyAPDU[300];

            memset(ReplyAPDU,'\0',300);

            int ReplyAPDULength=-1,CommandAPDUlength=-1;

            jbyteArray ReturnAPDUArray = NULL;

            jbyte *ByteArrayElement=NULL,*CommandAPDUElement=NULL;

            int counter=0;

            char C_CommandAPDU[100];

            memset(C_CommandAPDU,'\0',100);

            int rtcode =-1;

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
                const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

                const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

                const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);


            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            //Construct APDU Command Bytes
 
            //get jni array length
            CommandAPDUlength = (*env)->GetArrayLength(env,CommandAPDU);

            //get java byte array element
            CommandAPDUElement = (*env)->GetByteArrayElements(env,CommandAPDU,0);
            

            for(counter=0;counter<CommandAPDUlength;counter++)
            {

                 C_CommandAPDU[counter] = CommandAPDUElement[counter];

                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] C_CommandAPDU[%d] = 0x%xh.  and CommandAPDUElement[%d]=0x%xh. ",counter,C_CommandAPDU[counter],counter,CommandAPDUElement[counter]);
           
                 //writeFileLog(log);

            }
            

            //now release java byte array elements
            (*env)->ReleaseByteArrayElements(env,CommandAPDU,CommandAPDUElement,0);

            
            //memset(log,'\0',LOG_ARRAY_SIZE);

            //sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Command APDU Length = %d .",CommandAPDUlength);
           
            //writeFileLog(log);
 
           //////////////////////////////////////////////////////////////////////////////////////////////////////////
           
           if( 0 == CardTechType ) //++0 =ContactLess card
           {

		    /*
                      rtcode = ContactLessCardXChangeAPDU( C_CommandAPDU, 
                                                           CommandAPDUlength, 
                                                           ReplyAPDU,
                                                           &ReplyAPDULength);
                   */

                     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( char *CommandAPDU,
                                      int   CommandAPDULength,
                                      char *ReplyAPDU,
                                      int  *ReplyAPDULength, 
                                      char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int  fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      return NULL; //failed
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "ContactLessCardXChangeAPDU");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory function pointer failed");
				       return NULL;
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( C_CommandAPDU, 
                                                         CommandAPDUlength, 
                                                         ReplyAPDU,
                                                         &ReplyAPDULength,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //++dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
		      
                        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
		  
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

           }
           else if( 1 == CardTechType ) //++1 =Contact card
           {

		        //rtcode = ContactCardXChangeAPDU(C_CommandAPDU,CommandAPDUlength,ReplyAPDU,&ReplyAPDULength);
                     
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( char *CommandAPDU,
                                      int   CommandAPDULength,
                                      char *ReplyAPDU,
                                      int  *ReplyAPDULength, 
                                      char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int  fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      return NULL; //failed
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "ContactCardXChangeAPDU");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory function pointer failed");
				       return NULL;
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( C_CommandAPDU,
							 CommandAPDUlength,
							 ReplyAPDU,
							 &ReplyAPDULength,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_XChangeAPDU()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);
                                                //printf("\n[Java_Cris_SmartCard_XChangeAPDU()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
		 
                        
			(*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
		  
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

           }//else if( 1 == CardTechType ) block


           ///////////////////////////////////////////////////////////////////////////////////////////////////

            //memset(log,'\0',LOG_ARRAY_SIZE);
            //sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] API Return Code: %d. ",rtcode);
		    //writeFileLog(log);
		    
		    //++printf("[Java_Cris_SmartCard_XChangeAPDU()] API Return Code: %d. ",rtcode);
		                
           if( 0 ==  rtcode )
           {

                         //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] XchangeAPDU API Call success ");

		         //memset(log,'\0',LOG_ARRAY_SIZE);

		         //sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Return APDU array length = %d .",ReplyAPDULength);
		   
		         //writeFileLog(log);

                         //create new byte array
		         ReturnAPDUArray = (*env)->NewByteArray(env,ReplyAPDULength);

                         if( NULL == ReturnAPDUArray ){
                                 
                                //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] Array Create failed!");
                                
                                //memset(log,'\0',LOG_ARRAY_SIZE);

								//sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Return Code: %d. ",rtcode);
				   
								//writeFileLog(log);

                                return NULL;

                         }
		         
						 //get newly created byte array element
						 ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnAPDUArray,0);

                         if( NULL == ByteArrayElement ){
                                 
                                //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] Get Array Element failed!");
                                
                                //memset(log,'\0',LOG_ARRAY_SIZE);

								//sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Return Code: %d. ",rtcode);
				   
								//writeFileLog(log);

                                return NULL;

                         }

		         //fill data into return byte array
		         for(counter =0 ;counter<ReplyAPDULength;counter++) {
		              ByteArrayElement[counter] = ReplyAPDU[counter];

		              //memset(log,'\0',LOG_ARRAY_SIZE);

		              //sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] ByteArrayElement[%d] = 0x%xh.  and ReplyAPDU[%d]=0x%xh. ",counter,(unsigned char)ByteArrayElement[counter],counter,(unsigned char)ReplyAPDU[counter]);
		   
		              //writeFileLog(log);

		         }

		         //now release byte array elements
		         (*env)->ReleaseByteArrayElements(env,ReturnAPDUArray,ByteArrayElement,0);

		         //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU() Exit] Successfully return apdu array");

		         
		         return ReturnAPDUArray;
		         

                
            }
            else
            {
		         //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] XchangeAPDU API Call failed. ");

		         //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU() Exit] Null byte array return");
		         
		         printf( "[Java_Cris_SmartCard_XChangeAPDU() Exit] return code other than sucess found");
		         
		         //create new byte array
		         ReturnAPDUArray = (*env)->NewByteArray(env,3);

                         if( NULL == ReturnAPDUArray ){
                                 
                                //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] Array Create failed!");
                                
                                memset(log,'\0',LOG_ARRAY_SIZE);

		                sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Return Code: %d. ",rtcode);
		   
		                //writeFileLog(log);

                                return NULL;

                         }
		         
		         //get newly created byte array element
		         ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnAPDUArray,0);

                         if( NULL == ByteArrayElement ){
                                 
                                //writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] Get Array Element failed!");
                                
                                memset(log,'\0',LOG_ARRAY_SIZE);

		                sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Return Code: %d. ",rtcode);
		   
		                //writeFileLog(log);

                                return NULL;

                         }
						 /*	
                         switch(rtcode){
                                
                              //Negative PMT header 0x4e
                              case 2: ByteArrayElement[0] = 0;
                                      break;

                              //operation time out 
                              case 3: ByteArrayElement[0] = 3;
                                      break;

                              //Communicaion Error
                              case 4: ByteArrayElement[0] = 2;
                                      break;

                              //Error by default
                              default: ByteArrayElement[0] = 0; 
                                       break;

                         }*/
                         
                 ByteArrayElement[0] = rtcode;
		         ByteArrayElement[1] = '-1';
		         ByteArrayElement[2] = '-1';

		         //memset(log,'\0',LOG_ARRAY_SIZE);

		         //sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] ByteArrayElement[0] = 0x%xh.  and ByteArrayElement[1]=0x%xh. ",ByteArrayElement[0],ByteArrayElement[1]);
		   
		         //writeFileLog(log);

		         //now release byte array elements
		         (*env)->ReleaseByteArrayElements(env,ReturnAPDUArray,ByteArrayElement,0);

                         return ReturnAPDUArray;


            }

}//JNICALL Java_Cris_SmartCard_XChangeAPDUNative() end here


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_ReadUltralightBlock(JNIEnv *env, jobject jobj, jint Addr,jint Timeout)
{
     
      return NULL; 
 
}//Java_Cris_SmartCard_ReadUltralightBlock() end

*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

JNIEXPORT jint JNICALL Java_Cris_SmartCard_WriteUltralightPage(JNIEnv *env, 
                                                               jobject jobj, 
                                                               jint Addr, 
                                                               jbyteArray Data, 
                                                               jint Timeout
                                                              )
{


        return 2; //communication failure

}//Java_Cris_SmartCard_WriteUltralightPage() end

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jint JNICALL Java_Cris_SmartCard_JniisSAMEnable (    JNIEnv   *env,
                                                               jobject  jobj,
                                                               jstring  jniObjectdllPath,
                                                               jstring  jniLogfilePath, 
                                                               jint     jniLogLevel, 
                                                               jstring  jniDeviceId,
                                                               jstring  jniLogdllPath ){
                                                               
                                                               
                 	int rtcode =0;
                 	
                 	void *lib_handle;
		 
			int (*fnptr)( char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int  fnLogfileMode)=NULL;
				      
		        
		        const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

		        const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

		        const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_JniisSAMEnable()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_JniisSAMEnable()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      
                              return 31; ////Other error
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_JniisSAMEnable()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "isSAMEnable");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory function pointer failed");
				       return 31; ////Other error
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_JniisSAMEnable()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_JniisSAMEnable()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_JniisSAMEnable()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
		         
                        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
			
			return rtcode;
		  
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
                                                               
}//Java_Cris_SmartCard_JniisSAMEnable() ends


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_JniResetSAM ( JNIEnv   *env,
                                                               jobject  jobj,
                                                               jstring  jniObjectdllPath,
                                                               jstring  jniLogfilePath, 
                                                               jint     jniLogLevel, 
                                                               jstring  jniDeviceId,
                                                               jstring  jniLogdllPath,   
                                                               jint     SAMSlotId,
                                                               jint     ResetType, 
                                                               jint     Timeout
                                                          )
{


                int rtcode =-1;

                char ReplyAPDU[29];

                memset(ReplyAPDU,'0',29);

                int ReplyAPDULength=0,counter=0;

                jbyteArray ReturnAPDUArray = NULL;

                jbyte *ByteArrayElement=NULL;

                char log[LOG_ARRAY_SIZE];

                memset(log,'\0',LOG_ARRAY_SIZE);

                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
                const char *ObjectLogFilePath= (*env)->GetStringUTFChars(env,jniLogfilePath,0);

                const char *ObjectDeviceId= (*env)->GetStringUTFChars(env,jniDeviceId,0);

                const char *ObjectLogdllPath= (*env)->GetStringUTFChars(env,jniLogdllPath,0);

                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

                //create new byte array
		ReturnAPDUArray = (*env)->NewByteArray(env,RESET_RETURN_LENGTH);//Total 29 Byte
		         
		//get newly created byte array element
		ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnAPDUArray,0);

                for(counter =0 ;counter<RESET_RETURN_LENGTH;counter++)
		{
		              
                         ByteArrayElement[counter] = 0 ;

		}

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                if( 0 == ResetType ) //warm reset
                {
                      
                        //WarmResetSAM(SAMSlotId,ReplyAPDU,&ReplyAPDULength);
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( int   SAMSlotId, 
                                      char *ReplyByte,
                                      int  *ReplyByteLength,
                                      char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int  fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_ResetSAM()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      return NULL; //failed
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "WarmResetSAM");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory function pointer failed");
				       return NULL;
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( SAMSlotId,
                                                         ReplyAPDU,
                                                         &ReplyAPDULength,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_ResetSAM()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_ResetSAM()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
		         
                        
			(*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
		  
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

                }
                else if( 1 == ResetType ) //cold reset
                {
                     //ColdResetSAM( SAMSlotId, ReplyAPDU, &ReplyAPDULength);
                     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			void *lib_handle;
		 
			int (*fnptr)( int   SAMSlotId,
                                      char *ReplyByte,
                                      int  *ReplyByteLength,
                                      char *LogdllPathstr,
				      char *LogFileName,
				      char *deviceid,
				      int   fnLogfileMode)=NULL;

			const char *ObjectdllPath= (*env)->GetStringUTFChars(env,jniObjectdllPath,0);

			//printf("\n[Java_Cris_SmartCard_ResetSAM()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen(ObjectdllPath, RTLD_NOW|RTLD_GLOBAL);
		
			if (NULL==lib_handle)
			{
			      printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory pointer failed");
			      (*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
			      
                              return NULL; //failed
			}
			else
			{
				//success
				//printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory pointer successfully done");
				
				fnptr = dlsym(lib_handle, "ColdResetSAM");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory function pointer failed");
				       return NULL;
				}
				else
				{ 
					//printf("\n[Java_Cris_SmartCard_ResetSAM()] Get so memory function pointer get success");
					//Now Write Log through so file
					rtcode  = fnptr( SAMSlotId, 
                                                         ReplyAPDU, 
                                                         &ReplyAPDULength,
				                         (char*)ObjectLogdllPath,
				                         (char*)ObjectLogFilePath,
				                         (char*)ObjectDeviceId,
				                         jniLogLevel
						       );
				 
					//printf("\n[Java_Cris_SmartCard_ResetSAM()] function call return code: %d",rtcode);


					if(NULL!= lib_handle)
					{
                                                //dlclose(lib_handle);

						//printf("\n[Java_Cris_SmartCard_ResetSAM()] dllclose return code:%d",dlclose(lib_handle)); 
						fnptr=NULL;

					}
		
				}//else block end

		
			}//if( NULL == lib_handle ) end

			(*env)->ReleaseStringUTFChars(env, jniObjectdllPath, ObjectdllPath);
		       
                        (*env)->ReleaseStringUTFChars(env, jniLogfilePath, ObjectLogFilePath);

			(*env)->ReleaseStringUTFChars(env, jniDeviceId, ObjectDeviceId);

			(*env)->ReleaseStringUTFChars(env, jniLogdllPath, ObjectLogdllPath);
		  
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

                }//else if( 1 == ResetType )  end
                
                ////////////////////////////////////////////////////////////////////////////////////////////
                //Now copy all data to jni array
                 
		for(counter =0 ;counter<ReplyAPDULength;counter++)
		{
		              
                         ByteArrayElement[counter] = ReplyAPDU[counter];

		         memset(log,'\0',LOG_ARRAY_SIZE);

		         sprintf(log,"[Java_Cris_SmartCard_ResetSAM()] ByteArrayElement[%d] = 0x%xh.  and ReplyAPDU[%d]=0x%xh. ",counter,ByteArrayElement[counter],counter,ReplyAPDU[counter]);
		   
		         //writeFileLog(log);

		}

                //now release byte array elements
		(*env)->ReleaseByteArrayElements(env,ReturnAPDUArray,ByteArrayElement,0);

               //////////////////////////////////////////////////////////////////////////////////////////////

               return ReturnAPDUArray;



}//Java_Cris_SmartCard_ResetSAM() end

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		//memset(log,'\0',100);
				   
		//sprintf(log,"[JNI_delay_mSec()] clock_nanosleep failed with return code %d .",rtcode); 

		////writeFileLog(log);


        }


}//JNI_delay_mSec() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


