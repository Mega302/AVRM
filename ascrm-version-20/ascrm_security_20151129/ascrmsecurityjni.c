#include "ascrmsecurityjni.h"


#ifdef JNI_EXPORT_ENABLE 

static unsigned int g_cashboxopenexpirytime;

static unsigned int g_dooropenexpirytime;

static bool g_activate_device_flag=false;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static pthread_mutex_t g_ComPortAccessmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_disableAlarmprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getDoorstatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getUpsStatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getBatteryStatusprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_ActivateComPortprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_DeActivateComPortprocmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_CashBoxmutex = PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

bool SetupSecurityLogFile()
{

          

          bool rtcode = true;

          /////////////////////////////////////////////////////////////////////////////////////////////////////////////

          char logfilemode[MAX_STR_SIZE_OF_TRNSLOG];

	  memset(logfilemode,'\0',MAX_STR_SIZE_OF_TRNSLOG);

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


         /////////////////////////////////////////////////////////////////////////////////////////////////////////////

          if( true == g_LogModeType )
          {

			  //Set SmartCard General LOG File path
			 
			  unsigned char LogFilePath[8192];
		      
			  memset(LogFilePath,'\0',8192);
			   
			  int day=0,mon=0,yr=0,hr=0,min=0,sec=0;

			  char *destStr=NULL,*file=NULL;

			  getDateTime(&day,&mon,&yr,&hr,&min,&sec);

			  destStr = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

			  file = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

			  sprintf(destStr,"/Mega_Security_Dt_%d%d%d_Tm_%d_%d.log",yr,mon,day,hr,min,sec);

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
		
			 if(NULL!= destStr)
			 {
			   free(destStr);
			 }
		
			 if(NULL!=file)
			 {
			   free(file);
			 }

                         

         }

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         return rtcode;
        




}//SetupSecurityLogFile() end

*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JNIEXPORT jboolean JNICALL Java_Cris_Security_SetupSecurityLogFile( JNIEnv *env, 
                                                                    jclass  SecurityClass,
                                                                    jstring jniLogfile, 
                                                                    jint    jniLogLevel, 
                                                                    jstring jniDeviceId,
                                                                    jstring jniLogdllPath
                                                                  )
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_Cris_Security_WriteNativeLog(  JNIEnv *env, 
                                                           jobject jobj, 
                                                           jstring jniLogMessage
                                                         )
{

               
       
               const char *LogMessage= (*env)->GetStringUTFChars(env,jniLogMessage,0);

	       //printf("\n [Java_Cris_Security_WriteNativeLog()] Log File Name : %s", LogMessage);

	       writeFileLog((char*)LogMessage);

	       (*env)->ReleaseStringUTFChars(env, jniLogMessage, LogMessage);


               


}//JNIEXPORT void JNICALL Java_Cris_Security_WriteNativeLog end



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_Security_ActivateSecurityPort( JNIEnv *env, 
                                                                jobject jobj, 
                                                                jint ComPortId
                                                              )
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_Security_DeActivateSecurityPort(JNIEnv *env, jobject jobj)
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Get UPS Status
JNIEXPORT jint JNICALL Java_Cris_Security_GetUPSStatus(JNIEnv *env, jobject jobj)
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



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Get UPS Battery Status
JNIEXPORT jint JNICALL Java_Cris_Security_GetBatteryStatus(JNIEnv *env, jobject jobj)
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static int ActivateCCTALKComPort_V2(int CCTALKPortNumber)
{
              

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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static int DeActivateCCTALKComPort()
{

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// disableAlarm [ failed operation=0 , success operation=1 ]
JNIEXPORT jint JNICALL Java_Cris_Security_DisableAlarm( JNIEnv *env, 
                                                        jobject jobj,
                                                        jint dooropenflag, 
                                                        jint dooropenexpirytime, 
                                                        jint cashboxopenflag, 
                                                        jint cashboxopenexpirytime
                                                       )
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



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Get Door Status
JNIEXPORT jint JNICALL Java_Cris_Security_GetDoorStatus(JNIEnv *env, jobject jobj,jint DoorType)
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
