#include "smartcardjni.h"
#include "readini.h"
#include "atvmlog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

bool SetupSmartCardLogFile(){


         

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

#ifdef JNI_EXPORT_ENABLE 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jboolean JNICALL Java_Cris_SmartCard_SetupSmartCardLogFile( JNIEnv  *env, jclass  SmartCard,jstring jniLogfile, jint  jniLogLevel, jstring jniDeviceId,jstring jniLogdllPath) {

           g_Ascrm_writeFileLog=NULL;
           g_lib_handle=NULL;

	       //++Step 1:Log dll Path
           const char *LogdllPathstr= (*env)->GetStringUTFChars(env,jniLogdllPath,0);
           //printf("\n[Java_Cris_SmartCard_SetupSmartCardLogFile()] Log dll Path : %s", LogdllPathstr);
           SetSoFilePath((unsigned char*) LogdllPathstr);
           (*env)->ReleaseStringUTFChars(env, jniLogdllPath, LogdllPathstr);

	       //++Step 2:Log File Name
           const char *LogFileName= (*env)->GetStringUTFChars(env,jniLogfile,0);
	       //printf("\n[Java_Cris_SmartCard_SetupSmartCardLogFile()] Log File Name : %s", LogFileName);
           SetGeneralFileLogPath( (unsigned char*) LogFileName );
           (*env)->ReleaseStringUTFChars(env, jniLogfile, LogdllPathstr);
           
           //++Step 3: Set LogLevel Mode
           g_LogModeLevel = (int)jniLogfile;
           //printf("\n[Java_Cris_SmartCard_SetupSmartCardLogFile()] Log Mode : %s", g_LogModeLevel);

           //++Step 4: Get Device id
           const char *deviceid= (*env)->GetStringUTFChars(env,jniDeviceId,0);
           //printf("\n[Java_Cris_SmartCard_SetupSmartCardLogFile()] DeviceID : %s", deviceid);
           strcpy(g_Deviceid,deviceid);
           (*env)->ReleaseStringUTFChars(env,jniDeviceId, deviceid);
           return true;

}//JNIEXPORT jboolean JNICALL Java_Cris_SmartCard_SetupSmartCardLogFile end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_Cris_SmartCard_WriteNativeLog( JNIEnv *env, jobject jobj, jstring jniLogMessage){

               
       
               const char *LogMessage= (*env)->GetStringUTFChars(env,jniLogMessage,0);

	       //printf("\n [Java_Cris_SmartCard_WriteNativeLog()] Log File Name : %s", LogMessage);

	       //++writeFileLog((char*)LogMessage);

	       (*env)->ReleaseStringUTFChars(env, jniLogMessage, LogMessage);


               


}//JNIEXPORT void JNICALL Java_Cris_Currency_WriteNativeLog end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_GetCardBlockStatus(JNIEnv *env,jobject jobj){

          return(GetCardBlockStatus());
}//JNIEXPORT jint JNICALL Java_Cris_SmartCard_GetCardBlockStatus end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithCardRetain(JNIEnv *env,jobject jobj){

           return(MutekInitWithCardRetain());
           
}//JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithCardRetain end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithCardMoveInFront(JNIEnv *env,jobject jobj){
 
           return(MutekInitWithCardMoveInFront());
}//JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithCardMoveInFront end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithErrorCardBin(JNIEnv *env,jobject jobj){

            return(MutekInitWithErrorCardBin());
            
}//JNIEXPORT jint JNICALL Java_Cris_SmartCard_MutekInitWithErrorCardBin end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_ConnectDevice( JNIEnv *env, jobject jobj, jint PortId, jint ChannelClearanceMode, jint Timeout) {

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_ConnectDevice()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  = ConnectDevice_c(PortId,ChannelClearanceMode, Timeout);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_ConnectDevice()] ConnectDevice_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_ConnectDevice()] Exit",TRACE);

        return( rtcode  );



}//Java_Cris_SmartCard_ConnectDevice() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_DisConnectDevice( JNIEnv *env, jobject jobj, jint Timeout) {

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_DisConnectDevice()] Entry",TRACE);
       
        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  = DisConnectDevice_c(Timeout);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_DisConnectDevice()] DisConnectDevice_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_DisConnectDevice()] Exit",TRACE);

        return( rtcode );




}//Java_Cris_SmartCard_DisConnectDevice() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Java_Cris_SmartCard_GetDeviceStatus End
JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_GetDeviceStatus( JNIEnv *env, jobject jobj, jint ComponentId, jint Timeout) {


             char ReplyDeviceStatus[DEVICE_STATUS_LNEGTH ];

             memset( ReplyDeviceStatus,0,DEVICE_STATUS_LNEGTH  );

             int counter = 0;

             jbyteArray ReturnActivateArray = NULL;
             
             jbyte *ByteArrayElement= NULL;
            
             char log[LOG_ARRAY_SIZE];
             
             memset(log,'\0',LOG_ARRAY_SIZE);

             DeviceStatus_C(ComponentId,Timeout,ReplyDeviceStatus);

             //create new byte array
             ReturnActivateArray = (*env)->NewByteArray( env, DEVICE_STATUS_LNEGTH );

             //get newly created byte array element
             ByteArrayElement= (*env)->GetByteArrayElements( env,ReturnActivateArray,0 );

             for(counter=0;counter<DEVICE_STATUS_LNEGTH;counter++){

                     ByteArrayElement[counter] = ReplyDeviceStatus[counter];
                     
                     //memset(log,'\0',LOG_ARRAY_SIZE);

                     //sprintf(log,"[Java_Cris_SmartCard_GetDeviceStatus()] ByteArrayElement[%d] = 0x%xh. ",counter,ByteArrayElement[counter]);

                     //AVRM_SCRD_writeFileLog(log);

             }
           
             //now release byte array elements
             (*env)->ReleaseByteArrayElements(env,ReturnActivateArray,ByteArrayElement,0);

             AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_GetDeviceStatus()] Exit .",TRACE);

             return ReturnActivateArray;




}//Java_Cris_SmartCard_GetDeviceStatus() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_AcceptCard( JNIEnv *env,  jobject jobj, jint Timeout) {

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_AcceptCard()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  = AcceptCard_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_AcceptCard()] AcceptCard_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_AcceptCard()] Exit",TRACE);

        return rtcode;

}//Java_Cris_SmartCard_AcceptCard() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_DisableCardAcceptance( JNIEnv *env, jobject jobj, jint Timeout ) {
     
        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_DisableCardAcceptance()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  = DisableCardAcceptance_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_DisableCardAcceptance()] DisableCardAcceptance_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_DisableCardAcceptance()] Exit",TRACE);

        return( rtcode );


}//Java_Cris_SmartCard_DisableCardAcceptance() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_EnableCardAcceptance( JNIEnv *env, jobject jobj, jint Timeout) {

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_EnableCardAcceptance()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  =  EnableCardAcceptance_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_EnableCardAcceptance()]  EnableCardAcceptance_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_EnableCardAcceptance()] Exit",TRACE);

        return( rtcode );


}//Java_Cris_SmartCard_EnableCardAcceptance() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_IsCardInChannel(JNIEnv *env, jobject jobj, jint Timeout) {

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_IsCardInChanne()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  =  IsCardInChannel_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_IsCardInChannel()]  IsCardInChannel_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_IsCardInChanne()] Exit",TRACE);

        return( rtcode );



}//Java_Cris_SmartCard_IsCardInChannel() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_IsCardRemoved(JNIEnv *env, jobject jobj, jint Timeout) {

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_IsCardRemoved()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  =  IsCardRemoved_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_IsCardRemoved()]  IsCardRemoved_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_IsCardRemoved()] Exit",TRACE);

        return( rtcode );

}//Java_Cris_SmartCard_IsCardRemoved() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_CollectCard(JNIEnv *env, jobject jobj, jint Timeout) {

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_CollectCard()] Entry",TRACE);
        
        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  = CollectCard_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_CollectCard()] CollectCard_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_CollectCard()] Exit",TRACE);

        return( rtcode );

}//Java_Cris_SmartCard_CollectCard() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_DispenseCard(JNIEnv *env, jobject jobj, jint Timeout) {
 
        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_DispenseCard()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  =  DispenseCard_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_DispenseCard()]  DispenseCard_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_DispenseCard()] Exit",TRACE);

        return( rtcode );


}//Java_Cris_SmartCard_DispenseCard() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_RejectCard(JNIEnv *env, jobject jobj, jint Timeout) {

      
        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_RejectCard()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  =  RejectCard_c(Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_RejectCard()]  RejectCard_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,DEBUG);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_RejectCard()] Exit",TRACE);

        return( rtcode );



}//Java_Cris_SmartCard_RejectCard() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_ReturnCard (JNIEnv *env, jobject jobj, jint DispenseMode,jint Timeout) {
  
        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_ReturnCard()] Entry",TRACE);

        int rtcode =-1;

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        rtcode  =   ReturnCard_c(DispenseMode,Timeout) ;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[Java_Cris_SmartCard_ReturnCard()] ReturnCard_c() return Code: %d ",rtcode );
           
        AVRM_SCRD_writeFileLog(log,INFO);

        AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_ReturnCard()] Exit",TRACE);

        return( rtcode );



}//Java_Cris_SmartCard_ReturnCard() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_SAMSlotPowerOnOff( JNIEnv *env, jobject jobj,jint SAMSlotId,jint PowerOnOffState) {

       AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_SAMSlotPowerOnOff()] Entry ",TRACE);
 
       int rtcode = 1;

       AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_SAMSlotPowerOnOff ()] Exit ",TRACE);

       return rtcode;

}//Java_Cris_SmartCard_SAMSlotPowerOnOff() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_ActivateCard( JNIEnv *env, jobject jobj, jint CardTechType,jint SAMSlotId,jint Timeout ) {

            AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_ActivateCard()] Entry ",TRACE);

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
            
            //++create new byte array
            ReturnActivateArray = (*env)->NewByteArray(env,CommandAPDUlength);

            //++get newly created byte array element
            ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnActivateArray,0);

            /////////////////////////////////////////////////////////////////////////////////////////////////

            //++Now call our c api
            if( 0 == CardTechType ) //0 =ContactLess card
            {

                  ActivateContactLessCard(APIReply,&CommandAPDUlength);

                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] APIReply[%d] = 0x%xh. ",counter,APIReply[counter]);
           
                     AVRM_SCRD_writeFileLog(log,DEBUG);

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
           
                     AVRM_SCRD_writeFileLog(log,DEBUG);

                  }


            }//if( 0 == CardTechType )
            else if( 1 == CardTechType ) //1= Contact card
            {
            
                  
                  ActivateSAMCard(SAMSlotId,Timeout,SAMAPIReply);
                  
                  for(counter=0;counter<CommandAPDUlength;counter++)
                  {

                     memset(log,'\0',LOG_ARRAY_SIZE);

                     sprintf(log,"[Java_Cris_SmartCard_ActivateCard()] SAMAPIReply[%d] = 0x%xh. ",counter,SAMAPIReply[counter]);
           
                     AVRM_SCRD_writeFileLog(log,DEBUG);

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

                     AVRM_SCRD_writeFileLog(log,DEBUG);

                  }
           

            }//else if( 1 == CardTechType )
            
            //////////////////////////////////////////////////////////////////////////////////////////////////

            //now release byte array elements
            (*env)->ReleaseByteArrayElements(env,ReturnActivateArray,ByteArrayElement,0);

            AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_ActivateCard()] Exit ",TRACE);

            return ReturnActivateArray;



}//Java_Cris_SmartCard_ActivateCard() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_DeactivateCard(  JNIEnv *env, jobject jobj, jint CardTechType,jint SAMSlotId,jint Timeout) {

		int Reply =-1;

		if( 0 == CardTechType ) //0 =ContactLess card
		{

		    DeActivateContactLessCard(&Reply) ;

		}
		else if( 1 == CardTechType ) //1 =Contact card
		{

		    DeActivateContactCard(&Reply);

		}

		return (Reply);



}//Java_Cris_SmartCard_DeactivateCard() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_XChangeAPDU( JNIEnv *env, jobject jobj,jint CardTechType, jbyteArray CommandAPDU, jint SAMSlotId,jint Timeout) {

            
            AVRM_SCRD_writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] Entry",TRACE);

            char log[LOG_ARRAY_SIZE];

            memset(log,'\0',LOG_ARRAY_SIZE);

            char ReplyAPDU[300];

            memset(ReplyAPDU,'-1',300);

            int ReplyAPDULength=-1,CommandAPDUlength=-1;

            jbyteArray ReturnAPDUArray = NULL;

            jbyte *ByteArrayElement=NULL,*CommandAPDUElement=NULL;

            int counter=0;

            char C_CommandAPDU[100];

            memset(C_CommandAPDU,'\0',100);

            int rtcode =-1;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            //Construct APDU Command Bytes
 
            //get jni array length
            CommandAPDUlength = (*env)->GetArrayLength(env,CommandAPDU);

            //get java byte array element
            CommandAPDUElement = (*env)->GetByteArrayElements(env,CommandAPDU,0);
            
            for(counter=0;counter<CommandAPDUlength;counter++) {

                 C_CommandAPDU[counter] = CommandAPDUElement[counter];

                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] C_CommandAPDU[%d] = 0x%xh.  and CommandAPDUElement[%d]=0x%xh. ",counter,C_CommandAPDU[counter],counter,CommandAPDUElement[counter]);
           
				 AVRM_SCRD_writeFileLog(log,DEBUG);

            }
            
            //now release java byte array elements
            (*env)->ReleaseByteArrayElements(env,CommandAPDU,CommandAPDUElement,0);

            memset(log,'\0',LOG_ARRAY_SIZE);

            sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Command APDU Length = %d .",CommandAPDUlength);
           
            AVRM_SCRD_writeFileLog(log,DEBUG);
 
           //////////////////////////////////////////////////////////////////////////////////////////////////////////
           
           if( 0 == CardTechType ) //0 =ContactLess card
           {

				rtcode = ContactLessCardXChangeAPDU(C_CommandAPDU,CommandAPDUlength,ReplyAPDU,&ReplyAPDULength);

           }
           else if( 1 == CardTechType ) //1 =Contact card
           {

				rtcode = ContactCardXChangeAPDU(C_CommandAPDU,CommandAPDUlength,ReplyAPDU,&ReplyAPDULength);

           }
           
            memset(log,'\0',LOG_ARRAY_SIZE);

            sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()]  XChangeAPDU return code= %d .",rtcode);
           
            AVRM_writeFileLog(log,DEBUG);

            if( SUCCESS ==  rtcode ) {

                 AVRM_writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] XchangeAPDU API Call success ",INFO);

		         memset(log,'\0',LOG_ARRAY_SIZE);

		         sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] Return APDU array length = %d .",ReplyAPDULength);
		   
		         AVRM_writeFileLog(log,DEBUG);

                 //create new byte array
                 ReplyAPDULength = 3;
		         ReturnAPDUArray = (*env)->NewByteArray(env,ReplyAPDULength);
		         
		         //get newly created byte array element
		         ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnAPDUArray,0);

		         //fill data into return byte array
		         for(counter =0 ;counter<ReplyAPDULength;counter++) {
					 
		              ByteArrayElement[counter] = ReplyAPDU[counter];

		              memset(log,'\0',LOG_ARRAY_SIZE);

		              sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] ByteArrayElement[%d] = 0x%xh.  and ReplyAPDU[%d]=0x%xh. ",counter,ByteArrayElement[counter],counter,ReplyAPDU[counter]);
		   
		              AVRM_writeFileLog(log,DEBUG);

		         }//for end

		         //now release byte array elements
		         (*env)->ReleaseByteArrayElements(env,ReturnAPDUArray,ByteArrayElement,0);

		         AVRM_writeFileLog("[Java_Cris_SmartCard_XChangeAPDU() Exit] Successfully return apdu array(NULL)",INFO);

		         //return ReturnAPDUArray;
		         return NULL;

                
            }
            else
            {
		         AVRM_writeFileLog("[Java_Cris_SmartCard_XChangeAPDU()] XchangeAPDU API Call failed. ",ERROR);
		         AVRM_writeFileLog("[Java_Cris_SmartCard_XChangeAPDU() Exit] Null byte array return",ERROR);
		         
		         /*
		         //printf("[Java_Cris_SmartCard_XChangeAPDU()] XchangeAPDU API Call failed. ");
		         
		         //create new byte array
		         ReturnAPDUArray = (*env)->NewByteArray(env,ReplyAPDULength);
		         
		         //get newly created byte array element
		         ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnAPDUArray,0);

		         //fill data into return byte array
		         for(counter =0 ;counter<ReplyAPDULength;counter++) {
					 
					  if( 0 == counter ){
						  ByteArrayElement[counter] = rtcode;
					  }else{
						  ByteArrayElement[counter] = '-1';
					  }
		              memset(log,'\0',LOG_ARRAY_SIZE);

		              sprintf(log,"[Java_Cris_SmartCard_XChangeAPDU()] ByteArrayElement[%d] = 0x%xh.  and ReplyAPDU[%d]=0x%xh. ",counter,ByteArrayElement[counter],counter,ReplyAPDU[counter]);
		   
		              AVRM_writeFileLog(log,DEBUG);

		         }//for end

		         //now release byte array elements
		         (*env)->ReleaseByteArrayElements(env,ReturnAPDUArray,ByteArrayElement,0);

		         return ReturnAPDUArray;
		         */
				 return NULL;	

            }//else end

            


}//JNICALL Java_Cris_SmartCard_XChangeAPDUNative() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_ReadUltralightBlock(JNIEnv *env, jobject jobj, jint Addr,jint Timeout){
     
      return NULL; 
 
}//Java_Cris_SmartCard_ReadUltralightBlock() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jint JNICALL Java_Cris_SmartCard_WriteUltralightPage(JNIEnv *env, jobject jobj, jint Addr, jbyteArray Data, jint Timeout){


        return 2; //communication failure

}//Java_Cris_SmartCard_WriteUltralightPage() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT jbyteArray JNICALL Java_Cris_SmartCard_ResetSAM ( JNIEnv *env,jobject jobj, jint SAMSlotId,jint ResetType, jint Timeout) {


                char ReplyAPDU[29];

                memset(ReplyAPDU,'0',29);

                int ReplyAPDULength=0,counter=0;

                jbyteArray ReturnAPDUArray = NULL;

                jbyte *ByteArrayElement=NULL;

                char log[LOG_ARRAY_SIZE];

                memset(log,'\0',LOG_ARRAY_SIZE);

                /////////////////////////////////////////////////////////////////////////////////////////////

                //create new byte array
		ReturnAPDUArray = (*env)->NewByteArray(env,RESET_RETURN_LENGTH);//Total 29 Byte
		         
		//get newly created byte array element
		ByteArrayElement= (*env)->GetByteArrayElements(env,ReturnAPDUArray,0);

                for(counter =0 ;counter<RESET_RETURN_LENGTH;counter++)
		{
		              
                         ByteArrayElement[counter] = 0 ;

		}

                //////////////////////////////////////////////////////////////////////////////////////////

                if( 0 == ResetType ) //warm reset
                {
                     WarmResetSAM(SAMSlotId,ReplyAPDU,&ReplyAPDULength);

                }
                else if( 1 == ResetType ) //cold reset
                {
                     ColdResetSAM( SAMSlotId, ReplyAPDU, &ReplyAPDULength);

                }
                
                ////////////////////////////////////////////////////////////////////////////////////////////
                //Now copy all data to jni array
                 
		for(counter =0 ;counter<ReplyAPDULength;counter++)
		{
		              
                         ByteArrayElement[counter] = ReplyAPDU[counter];

		         memset(log,'\0',LOG_ARRAY_SIZE);

		         sprintf(log,"[Java_Cris_SmartCard_ResetSAM()] ByteArrayElement[%d] = 0x%xh.  and ReplyAPDU[%d]=0x%xh. ",counter,ByteArrayElement[counter],counter,ReplyAPDU[counter]);
		   
		         AVRM_SCRD_writeFileLog(log,DEBUG);

		}

                //now release byte array elements
		(*env)->ReleaseByteArrayElements(env,ReturnAPDUArray,ByteArrayElement,0);

               //////////////////////////////////////////////////////////////////////////////////////////////

               return ReturnAPDUArray;



}//Java_Cris_SmartCard_ResetSAM() end

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Millisecond delay
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

		//memset(log,'\0',100);
				   
		//sprintf(log,"[JNI_delay_mSec()] clock_nanosleep failed with return code %d .",rtcode); 

		//AVRM_SCRD_writeFileLog(log);


        }


}//JNI_delay_mSec() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


