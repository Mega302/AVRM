#include "atvmlog.h"
#include <pthread.h>

static pthread_mutex_t g_LogFilemutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_LogConsolemutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_getDateTimeprocmutex = PTHREAD_MUTEX_INITIALIZER;
static char g_general_log[MAX_STR_SIZE_OF_LOG]={'\0'};
static char g_machine_fault_log[MAX_STR_SIZE_OF_LOG]={'\0'};
static char g_crisapi_timeout_log[MAX_STR_SIZE_OF_LOG]={'\0'};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char  g_Deviceid[MAX_STR_SIZE_OF_LOG];
int   g_LogModeLevel;
void *g_lib_handle;
int  (*g_Ascrm_writeFileLog)(char *str,char *LogFilePathstr);
int  (*g_AVRM_writeFileLog)(char *str,char *LogFilePathstr,int fnLogLevel);
int  (*g_Ascrm_WriteLogInConsole)(char *str);
char  g_logdllpath[MAX_STR_SIZE_OF_LOG];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int  SetSoFilePath(unsigned char* SoFilePath ) {

    memset(g_logdllpath,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_logdllpath ,SoFilePath );
    //printf("\n[SetSoFilePath()] log dll file path=%s \n",g_logdllpath);
    return 1;

}//int  SetSoFilePath(unsigned char* SoFilePath ) end

int  SetGeneralFileLogPath(unsigned char* LogFilePath ) {

    #if defined(PRINT_FILE)
    memset(g_general_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_general_log ,LogFilePath );
    //printf("\n[SetGeneralFileLogPath()] ascrm log file path=%s \n",g_general_log);
    #endif

    return 1;

}//int  SetGeneralFileLogPath(unsigned char* LogFilePath )  END

int  SetMachineFaultFileLogPath(unsigned char* LogFilePath ) {

    #if defined(PRINT_FILE)
    memset(g_machine_fault_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_machine_fault_log ,LogFilePath );
    printf("\n atvmlog machine fault log file path=%s \n",g_machine_fault_log);
    #endif

    return 1;

}//int  SetMachineFaultFileLogPath(unsigned char* LogFilePath ) END

int  SetAPITimeoutFileLogPath(unsigned char* LogFilePath ) {

    #if  defined(PRINT_FILE)
    memset(g_crisapi_timeout_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_crisapi_timeout_log ,LogFilePath );
    printf("\n atvm api timeout test file path=%s \n",g_crisapi_timeout_log);
    #endif

    return 1;

}//int  SetAPITimeoutFileLogPath(unsigned char* LogFilePath )  END

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define OFF           40
//#define TRACE         41
//#define DEBUG         42
//#define INFO          43
//#define WARN          44
//#define ERROR         45
//#define FATAL         46
//#define ALL           47
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int AVRM_writeFileLog(char *str,int fnLogLevel) { 
	return AVRM_writeMDSLog(str,g_general_log,fnLogLevel,"API");	     
}//++AVRM_writeFileLog() end

int AVRM_Currency_writeFileLog(char *str,int fnLogLevel) { 
	return AVRM_writeMDSLog(str,g_general_log,fnLogLevel,"API");	     
}//++AVRM_writeFileLog() end

int AVRM_writeFileLogV2(char *str,int fnLogLevel,unsigned char* DeviceName){ 
	return AVRM_writeMDSLog(str,g_general_log,fnLogLevel,DeviceName);	     
}//++AVRM_writeFileLogV2() end

static int AVRM_writeMDSLog(char *str, char *filename,int fnLogLevel,unsigned char* DeviceName) {
               
        int rtcode=0;  
        int logdllpathlegth=0;
        int counter=0;
        
        //printf("\n[AVRM_writeMDSLog()] Entry ");
        //++No Log Mode Defined
        //printf("\n[AVRM_writeMDSLog()] Log File Mode: %d",fnLogLevel);
        
        if( 0 == g_LogModeLevel )  {
			WriteLogInConsole(str); 
            printf("\n[AVRM_writeMDSLog()] No Log Mode defined");
            return;

        }//++if end
        
        pthread_mutex_lock(&g_LogFilemutex);

        if(strlen (g_logdllpath) <=0 ) {

             printf("\n[AVRM_writeMDSLog()] Error Empty Log dll path");
             pthread_mutex_unlock(&g_LogFilemutex);
             return;

        }//++if(strlen (g_logdllpath) <=0 ) end

        if(strlen (g_general_log) <=0 ) {
             
             printf("\n[AVRM_writeMDSLog()] Error Empty Log file name");
             pthread_mutex_unlock(&g_LogFilemutex);
             return; 
                

        }//if(strlen (g_general_log) <=0 ) end

        while (g_logdllpath[counter] != '\0'){
			
              logdllpathlegth++;
              counter++;

        }//while end

        //printf("\n[writeMDSLog()] dllLogPath String Length: %d",logdllpathlegth);

        if(logdllpathlegth<=0){
             printf("\n[AVRM_writeMDSLog()] dllLogPath is empty");
             pthread_mutex_unlock(&g_LogFilemutex);
             return 0;
 
        }//if end

        //printf("\n[writeMDSLog()] dllLogPath: %s",g_logdllpath);

        if( NULL == g_lib_handle ){
			
			   g_lib_handle = dlopen(g_logdllpath, RTLD_LAZY);

			   if (NULL==g_lib_handle) {
			       printf("\n[AVRM_writeMDSLog()] Get so memory pointer failed");
                   pthread_mutex_unlock(&g_LogFilemutex);
			       return false; //failed
			   }else{
		            //success
                    //printf("\n[AVRM_writeMDSLog() if block] Get so memory pointer successfully done");
	           }//ELSE END


        }//if( NULL == g_lib_handle ) end

        if(NULL==g_Ascrm_writeFileLog) {

              g_AVRM_writeFileLog = NULL;
              g_AVRM_writeFileLog = dlsym(g_lib_handle, "AVRM_writeFileLog");
              if (NULL == g_AVRM_writeFileLog)  {
					printf("\n[AVRM_writeMDSLog()] Get so memory function pointer failed");
					pthread_mutex_unlock(&g_LogFilemutex);
					return 0;
			  }else{
					//printf("\n[AVRM_writeMDSLog()] Get so memory function pointer get success");
              }//else end
        }else{
            //printf("\n[AVRM_writeMDSLog()] Get so memory function pointer already done");
        }//else end
        
        //++Write Log
        char alllog[9600]; 
        memset(alllog,'\0',9600);
        //++sprintf(alllog,"[%s]: %s",g_Deviceid,str);
        //++printf("\n[AVRM_writeMDSLog()] DeviceID: %s",DeviceName);
        //++printf("\n[AVRM_writeMDSLog()] Message: %s",str);
        sprintf(alllog,"[%s]: %s",DeviceName,str);
        
        //++Now Write Log through so file
		g_AVRM_writeFileLog(alllog,g_general_log,fnLogLevel);

        //++Free File Pointer
        if(NULL != g_Ascrm_writeFileLog ){
             g_Ascrm_writeFileLog = NULL;
        }//if end

        pthread_mutex_unlock(&g_LogFilemutex);

        //++printf("\n[AVRM_writeMDSLog()] Exit");
        return rtcode;
        
}//AVRM_writeMDSLog() end

void getDateTime(int *day,int *mon,int *yr,int *hr,int *min,int *sec) {

     pthread_mutex_lock(&g_getDateTimeprocmutex);

	 time_t tm; 

	 struct tm dttm;

	 tm = time(NULL);

	 dttm = *localtime(&tm);

	 *day = dttm.tm_mday;

	 *mon = dttm.tm_mon + 1;

	 *yr  =  dttm.tm_year+1900;


	 *hr  = dttm.tm_hour;

	 *min = dttm.tm_min; 

	 *sec = dttm.tm_sec;

	 //printf("\n dd/mm/yyyy %d / %d / %d  hh:mm:ss= %d : %d : %d",*day,*mon,*yr,*hr,*min,*sec);

         pthread_mutex_unlock(&g_getDateTimeprocmutex);


}//getDateTime() end

static int WriteLogInConsole(char *str) {
                 
            /*
            pthread_mutex_lock(&g_LogConsolemutex);

			int day=0,mon=0,yr=0,hr=0,min=0,sec=0;

			int counter=0;

			char * outString = (char *)malloc(MAX_STR_SIZE_OF_LOG*2);

			if(outString == NULL)
			{
			 pthread_mutex_unlock(&g_LogConsolemutex); 
			 return 0;

			}

			getDateTime(&day,&mon,&yr,&hr,&min,&sec);
		  
			sprintf(outString,"\n[%02d/%02d/%02d] [%02d:%02d:%02d] ",day,mon,yr,hr,min,sec);

			strcat(outString,str);

			for(counter=0;outString[counter]!='\0';counter++)
			{
					 printf("%c",outString[counter]);
		    }

			free(outString);

			pthread_mutex_unlock(&g_LogConsolemutex);
            */
			return 1;

}//WriteLogInConsole() end

/*
static int writeMDSLog(char *str, char *filename) {
               
        int rtcode=0;  
        int logdllpathlegth=0;
        int counter=0;
        
        //++printf("\n[writeMDSLog()] Entry ");
        
        //No Log Mode Defined
        //++printf("\n[writeMDSLog()] Log File Mode: %d",g_LogModeLevel);
        
        if( 0 == g_LogModeLevel )  {
			WriteLogInConsole(str); 
            printf("\n[writeMDSLog()] No Log Mode defined");
            return;

        }//++if end
        
        pthread_mutex_lock(&g_LogFilemutex);

        if(strlen (g_logdllpath) <=0 ) {

             printf("\n[writeMDSLog()] Error Empty Log dll path");
             pthread_mutex_unlock(&g_LogFilemutex);
             return;

        }//++if(strlen (g_logdllpath) <=0 ) end

        if(strlen (g_general_log) <=0 ) {
             
             printf("\n[writeMDSLog()] Error Empty Log file name");
             pthread_mutex_unlock(&g_LogFilemutex);
             return; 
                

        }//if(strlen (g_general_log) <=0 ) end

        while (g_logdllpath[counter] != '\0'){
			
              logdllpathlegth++;
              counter++;

        }//while end

        //printf("\n[writeMDSLog()] dllLogPath String Length: %d",logdllpathlegth);

        if(logdllpathlegth<=0){
             printf("\n[SmartCard_writeMDSLog()] dllLogPath is empty");
             pthread_mutex_unlock(&g_LogFilemutex);
             return 0;
 
        }//if end

        //printf("\n[writeMDSLog()] dllLogPath: %s",g_logdllpath);

        if( NULL == g_lib_handle ){
			
			   g_lib_handle = dlopen(g_logdllpath, RTLD_LAZY);

			   if (NULL==g_lib_handle) {
			       printf("\n[writeMDSLog()] Get so memory pointer failed");
                   pthread_mutex_unlock(&g_LogFilemutex);
			       return false; //failed
			   }else{
		            //success
                    //printf("\n[writeMDSLog() if block] Get so memory pointer successfully done");
	           }


        }//if( NULL == g_lib_handle ) end

        if(NULL==g_Ascrm_writeFileLog) {

              
              g_Ascrm_writeFileLog = dlsym(g_lib_handle, "Ascrm_writeFileLog");

              if (NULL == g_Ascrm_writeFileLog)  {
					printf("\n[writeMDSLog()] Get so memory function pointer failed");
					pthread_mutex_unlock(&g_LogFilemutex);
					return 0;
			  }else{
					//printf("\n[writeMDSLog()] Get so memory function pointer get success");
              }

              

        }else{
            //printf("\n[writeMDSLog()] Get so memory function pointer already done");
        }//else end

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
        char *string = strdup(str);
        char *writemssg=NULL;
        char *token=NULL;

        if( NULL== string ){
             //printf("\n[writeMDSLog()] Duplicate String create failed");
             free(string);
             pthread_mutex_unlock(&g_LogFilemutex);
             return;

        }//if( NULL== string ) end
        
        token=strtok(string," ");
        
        if( ( 0 == strcmp(token,"DATA_EXCHANGE_ALL") ) && ( DATA_EXCHANGE_ALL == g_LogModeLevel )  )  {
             
             //++printf("\n[writeMDSLog()] Log Type: Data Exchange All ");
             //writemssg = replace_str(str, "DATA_EXCHANGE", " ");
             //++Now Write Log through so file
             g_Ascrm_writeFileLog(str,g_general_log);

        }else if( ( 0 == strcmp(token,"DATA_EXCHANGE_ERROR_ONLY") ) && (DATA_EXCHANGE_ERROR_ONLY == g_LogModeLevel)  ) {
             
             //++printf("\n[writeMDSLog()] Log Type: Data Exchange Error ");
             //writemssg = replace_str(str, "DATA_EXCHANGE", " ");
             //++Now Write Log through so file
             g_Ascrm_writeFileLog(str,g_general_log);

        }else if( ALL_LOG == g_LogModeLevel )  {

             //++printf("\n[writeMDSLog()] Log Type: General log");
             char alllog[9600]; 
             memset(alllog,'\0',9600);
             sprintf(alllog,"[%s]: %s",g_Deviceid,str);
             //++Now Write Log through so file
		     g_Ascrm_writeFileLog(alllog,g_general_log);
             
        }else {
              printf("\n[writeMDSLog()] No Log mode matched");
        }//else end

        //++Free File Pointer
        if(NULL != string ){
             free(string);
        }//if end

        pthread_mutex_unlock(&g_LogFilemutex);

        //++printf("\n[writeMDSLog()] Exit");
        return rtcode;
        
}//writeMDSLog() end
*/

static bool OpenLogdll(char* dlllogpath){       
 
                  
                   if( NULL == g_lib_handle )
                   {
					   g_lib_handle = dlopen(dlllogpath, RTLD_LAZY);

					   if (NULL==g_lib_handle) 
					   {
						  
						   return false; //failed
					   }
		           else
		           {
		               return true; //success
	 
		           }
                   }
                   else
                   {
                      return true; //already open

                   }



}//static bool LoadLogdll(char* dlllogpath) end

static bool GetFileLogDllFunctionAddress(){

                   if(NULL == g_Ascrm_writeFileLog)
                   {
                   
			   g_Ascrm_writeFileLog = dlsym(g_lib_handle, "Ascrm_writeFileLog");

			   if (NULL == g_Ascrm_writeFileLog)  
			   {
			      return false;
			   }
		           else
		           {
		              return true;

		           }

                   }
                   else
                   {
                      return true;

                   }

}//bool GetFileLogDllFunctionAddress() end

static bool GetConsoleLogDllFunctionAddress(){

                   if(NULL == g_Ascrm_WriteLogInConsole){
					   
					   g_Ascrm_WriteLogInConsole = dlsym(g_lib_handle, "Ascrm_WriteLogInConsole");
                       if (NULL == g_Ascrm_WriteLogInConsole)  {
						  return false;
					   }else{
						  return true;
                       } //else end

                   }else{
                      return true;
                   }//else end

}//bool GetConsoleLogDllFunctionAddress() end

static bool CloseLogdll(){
		   
               if(NULL == g_lib_handle)
               {
 
			   if( 0 == dlclose(g_lib_handle))
		           {
		               g_Ascrm_writeFileLog=NULL;
		               g_lib_handle=NULL;
			       return true;

		           }
		           else
		           {
		               return false;

		           }

               }
               else
               {
                   return true;

               }

}//bool CloseLogdll() end

void LogDataExchnage( int DataExchangelevel,unsigned char* Deviceid,unsigned char* TransmitType,unsigned char* command,int datalength ) {
	
       int counter=0;
       static char DATA_EXCHANGE_ERROR_Tx[LOG_ARRAY_SIZE];
       static char DATA_EXCHANGE_ERROR_Type[10];
       static int  DATA_EXCHANGE_ERROR_TxLength=0;
       unsigned char MessageLog[LOG_ARRAY_SIZE];
       unsigned char CommandHexLog[LOG_ARRAY_SIZE];
       unsigned char LogTypeMesssage[LOG_ARRAY_SIZE];
       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);
       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
       memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);
       strcpy(LogTypeMesssage,"DEBUG");
       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage,Deviceid,TransmitType,datalength);
       for(counter=0;counter<datalength;counter++){
           sprintf(CommandHexLog,"%x ",command[counter]);
           strcat(MessageLog,CommandHexLog);
       }//for loop
       
       //++AVRM_writeFileLogV2(MessageLog,"DEBUG",Deviceid);
       //printf("\n[LogDataExchnage()] String:%s\n", MessageLog);

}//void LogDataExchnage( int DataExchangelevel,unsigned char* Deviceid,unsigned char* TransmitType,unsigned char* command,int datalength ) 

void LogDataExchnageV2( int DataExchangelevel,unsigned char* Deviceid,unsigned char* TransmitType,unsigned char* command,int datalength) {
       
       int counter=0;
       static char DATA_EXCHANGE_ERROR_Tx[LOG_ARRAY_SIZE];
       static char DATA_EXCHANGE_ERROR_Type[10];
       static int  DATA_EXCHANGE_ERROR_TxLength=0;
       unsigned char MessageLog[LOG_ARRAY_SIZE];
       unsigned char CommandHexLog[LOG_ARRAY_SIZE];
       unsigned char LogTypeMesssage[LOG_ARRAY_SIZE];
       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);
       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
       memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);
       strcpy(LogTypeMesssage,"DEBUG");
       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
       sprintf(MessageLog,"[%s] [%s] [%s] [%d]0x:",LogTypeMesssage,Deviceid,TransmitType,datalength);
       for(counter=0;counter<datalength;counter++){
           sprintf(CommandHexLog,"%x ",command[counter]);
           strcat(MessageLog,CommandHexLog);
       }//for loop
       
       //++printf("\n[LogDataExchnageV2()] %s",MessageLog);
       AVRM_writeFileLogV2(MessageLog,DataExchangelevel,Deviceid);
                      
}//void LogDataExchnageV2( int DataExchangelevel,unsigned char* Deviceid,unsigned char* TransmitType,unsigned char* command,int datalength) end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

void LogDataExchnage( unsigned char* Deviceid,
                      unsigned char* TransmitType,
                      unsigned char* command,
                      int            datalength
                    )
{

       //printf("\n[LogDataExchnage()] Entry \n");

       int counter=0;

       unsigned char MessageLog[LOG_ARRAY_SIZE];

       unsigned char CommandHexLog[LOG_ARRAY_SIZE];

       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

       memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);


       ///////////////////////////////////////////////////////////////////////////////////////

       //printf("\n[LogDataExchnage()] Log Mode Level : %d\n", g_LogModeLevel);

       if(   (DATA_EXCHANGE_ALL        == g_LogModeLevel)  || 
             (DATA_EXCHANGE_ERROR_ONLY == g_LogModeLevel)  ||
             (ALL_LOG                  == g_LogModeLevel) 
         )
       {

		       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
        
                       sprintf(MessageLog,"DATA_EXCHANGE_ALL %s %s[%d]:",Deviceid,TransmitType,datalength);

                       for(counter=0;counter<datalength;counter++)
		       {
                            sprintf(CommandHexLog,"0x%x ",command[counter]);
                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop
                       

                       //printf("\n[LogDataExchnage()] String: %s\n", MessageLog);
                       
                       //printf("\n[LogDataExchnage() if] Exit \n");

                       writeFileLog(MessageLog);

                       return;

       }//if block end
       else
       {
             //printf("\n[LogDataExchnage() elseblock] Exit \n");
             return ;

       }

       ///////////////////////////////////////////////////////////////////////////////////

}//void LogDataExchnage(unsigned char* command,int  datalength) end

*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void LogDataExchnage( int DataExchangelevel,unsigned char* Deviceid,unsigned char* TransmitType,unsigned char* command,int datalength) {


       //printf("\n[LogDataExchnage()] Entry");

       int counter=0;

       ////////////////////////////////////////////////////////////////////////////////////////
    
       static char DATA_EXCHANGE_ERROR_Tx[LOG_ARRAY_SIZE];

       static char DATA_EXCHANGE_ERROR_Type[10];

       static int  DATA_EXCHANGE_ERROR_TxLength=0;

       ////////////////////////////////////////////////////////////////////////////////////////

       unsigned char MessageLog[LOG_ARRAY_SIZE];

       unsigned char CommandHexLog[LOG_ARRAY_SIZE];

       unsigned char LogTypeMesssage[LOG_ARRAY_SIZE];

       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);

       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

       memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);

       /////////////////////////////////////////////////////////////////////////////////////////

       
       //printf("\n[LogDataExchnage()] Transmit Type: %s ",TransmitType);

       //for(counter=0;counter<datalength;counter++)
       //{
            //printf("\nTx[%d] : 0x%x ",counter,command[counter]);

       //}//for loop
       /*
       if( 0== strcmp("Tx",TransmitType) )
       {

		   memset(DATA_EXCHANGE_ERROR_Tx,'\0',LOG_ARRAY_SIZE);

                   memset(DATA_EXCHANGE_ERROR_Type,'\0',10);

                   DATA_EXCHANGE_ERROR_TxLength=0;
	 
		   for(counter=0;counter<datalength;counter++)
		   {
	                //printf("\nTx[%d] : 0x%x ",counter,command[counter]);
                        DATA_EXCHANGE_ERROR_Tx[counter]=command[counter];

		   }//for loop

                   strcpy(DATA_EXCHANGE_ERROR_Type,TransmitType);

                   DATA_EXCHANGE_ERROR_TxLength=0;

                   DATA_EXCHANGE_ERROR_TxLength=datalength;

                   
                   //for(counter=0;counter<DATA_EXCHANGE_ERROR_TxLength;counter++)
		   //{
                         //printf("\nDATA_EXCHANGE_ERROR_Tx[%d] : 0x%x ",counter,DATA_EXCHANGE_ERROR_Tx[counter]);

                   //}//for loop
                   
        }
        else
        {

                  //printf("\n[LogDataExchnage()] DATA_EXCHANGE_ALL String not matched ");
        }
         */

       ///////////////////////////////////////////////////////////////////////////////////////////

       /*
       if( DATA_EXCHANGE_ALL == DataExchangelevel )  
       {
          
           strcpy(LogTypeMesssage,"DATA_EXCHANGE_ALL");

       }     
       else if( DATA_EXCHANGE_ERROR_ONLY == DataExchangelevel )
       {
           //strcpy(LogTypeMesssage,"DATA_EXCHANGE_ERROR_ONLY");

       }// else if( DATA_EXCHANGE_ERROR_ONLY == DataExchangelevel ) end
       */
      
       ////////////////////////////////////////////////////////////////////////////////////////////
       /*
       //if(   (DATA_EXCHANGE_ALL == g_LogModeLevel )   ||  //3 
       //      (ALL_LOG           == g_LogModeLevel )       //4
        // )
       //{


                       strcpy(LogTypeMesssage,"DEBUG");
                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
                       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage,Deviceid,TransmitType,datalength);
                       for(counter=0;counter<datalength;counter++){
                            sprintf(CommandHexLog,"%x ",command[counter]);
                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop
                       
                       AVRM_writeFileLogV2(MessageLog,"DEBUG",Deviceid);
                       //printf("\n[LogDataExchnage()] String:%s\n", MessageLog);



       //}//if block end
       
       /*
       else if( DATA_EXCHANGE_ERROR_ONLY == g_LogModeLevel )   //2
       {

               if( ( 0== strcmp("Rx",TransmitType) ) && ( datalength <= 0  ) )
               {
 
                       
                        
		       //for(counter=0;counter<DATA_EXCHANGE_ERROR_TxLength;counter++)
		       //{
		            //printf("\nDATA_EXCHANGE_ERROR_Tx[%d] : 0x%x ",counter,DATA_EXCHANGE_ERROR_Tx[counter]);

		       //}//for loop
                   
                       
                       ///////////////////////////////////////////////////////////////////////////
                       
                       //Log Tx Message

                       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);

                       strcpy(LogTypeMesssage,"DATA_EXCHANGE_ERROR_ONLY");

                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

                       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage, Deviceid,DATA_EXCHANGE_ERROR_Type,DATA_EXCHANGE_ERROR_TxLength);

                       for(counter=0;counter<DATA_EXCHANGE_ERROR_TxLength;counter++)
		       {
                            sprintf(CommandHexLog,"%x ",DATA_EXCHANGE_ERROR_Tx[counter]);

                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop

                       writeFileLog(MessageLog);

                       memset(DATA_EXCHANGE_ERROR_Tx,'\0',LOG_ARRAY_SIZE);

                       memset(DATA_EXCHANGE_ERROR_Type,'\0',10);
                           
                       DATA_EXCHANGE_ERROR_TxLength =0;
                      
                       /////////////////////////////////////////////////////////////////////////////    
                       //Log Rx Message

                       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);

                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

                       strcpy(LogTypeMesssage,"DATA_EXCHANGE_ERROR_ONLY");

                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

                       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage,
                       Deviceid,TransmitType,datalength);

                       for(counter=0;counter<datalength;counter++)
		       {
                            sprintf(CommandHexLog,"%x ",command[counter]);

                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop

                       writeFileLog(MessageLog);
                       
                       ////////////////////////////////////////////////////////////

                       

               }
               else
               {


               }

              

       }
       else
       {
             printf("\n[LogDataExchnage()] No log Mode defined");
             return ;

       }
       

       ///////////////////////////////////////////////////////////////////////////////////


}//void LogDataExchnage(unsigned char* command,int  datalength) 
*/

/*
void LogDataExchnageV2( int DataExchangelevel,unsigned char* Deviceid,unsigned char* TransmitType,unsigned char* command,int datalength) {


       //printf("\n[LogDataExchnage()] Entry");

       int counter=0;

       ////////////////////////////////////////////////////////////////////////////////////////
    
       static char DATA_EXCHANGE_ERROR_Tx[LOG_ARRAY_SIZE];

       static char DATA_EXCHANGE_ERROR_Type[10];

       static int  DATA_EXCHANGE_ERROR_TxLength=0;

       ////////////////////////////////////////////////////////////////////////////////////////

       unsigned char MessageLog[LOG_ARRAY_SIZE];

       unsigned char CommandHexLog[LOG_ARRAY_SIZE];

       unsigned char LogTypeMesssage[LOG_ARRAY_SIZE];

       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);

       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

       memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);

       /////////////////////////////////////////////////////////////////////////////////////////

       
       //printf("\n[LogDataExchnageV2()] Transmit Type: %s ",TransmitType);

       //for(counter=0;counter<datalength;counter++)
       //{
            //printf("\nTx[%d] : 0x%x ",counter,command[counter]);

       //}//for loop
       /*
       if( 0== strcmp("Tx",TransmitType) ) {

		   memset(DATA_EXCHANGE_ERROR_Tx,'\0',LOG_ARRAY_SIZE);

                   memset(DATA_EXCHANGE_ERROR_Type,'\0',10);

                   DATA_EXCHANGE_ERROR_TxLength=0;
	 
				   for(counter=0;counter<datalength;counter++)
				   {
	                //printf("\nTx[%d] : 0x%x ",counter,command[counter]);
                        DATA_EXCHANGE_ERROR_Tx[counter]=command[counter];

		   }//for loop

                   strcpy(DATA_EXCHANGE_ERROR_Type,TransmitType);

                   DATA_EXCHANGE_ERROR_TxLength=0;

                   DATA_EXCHANGE_ERROR_TxLength=datalength;

                   
                   //for(counter=0;counter<DATA_EXCHANGE_ERROR_TxLength;counter++)
		   //{
                         //printf("\nDATA_EXCHANGE_ERROR_Tx[%d] : 0x%x ",counter,DATA_EXCHANGE_ERROR_Tx[counter]);

                   //}//for loop
                   
        }
        else
        {

                  //printf("\n[LogDataExchnage()] DATA_EXCHANGE_ALL String not matched ");
        }
          */

       ////////////////////////////////////////////////////////////////////////////////////////////
/*
       //if(   (DATA_EXCHANGE_ALL == g_LogModeLevel )   ||  //3 
             //(ALL_LOG           == g_LogModeLevel )       //4
        // )
      // {


                       strcpy(LogTypeMesssage,"DEBUG");
                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
                       sprintf(MessageLog,"[%s] [%s] [%s] [%d]0x:",LogTypeMesssage,Deviceid,TransmitType,datalength);
                       for(counter=0;counter<datalength;counter++){
                            sprintf(CommandHexLog,"%x ",command[counter]);
                            strcat(MessageLog,CommandHexLog);
                       }//for loop
                       //printf("\n[LogDataExchnageV2()] %s",MessageLog);
                       //++writeFileLog(MessageLog);
                       AVRM_writeFileLogV2(MessageLog,"DEBUG",Deviceid);
                       //printf("\n[ LogDataExchnageV2()] String:%s\n", MessageLog);
                       
       //}//if block end
       /*else if( DATA_EXCHANGE_ERROR_ONLY == g_LogModeLevel )   //2
       {

               if( ( 0== strcmp("Rx",TransmitType) ) && ( datalength <= 0  ) )
               {
 
                       
                        
		       //for(counter=0;counter<DATA_EXCHANGE_ERROR_TxLength;counter++)
		       //{
		            //printf("\nDATA_EXCHANGE_ERROR_Tx[%d] : 0x%x ",counter,DATA_EXCHANGE_ERROR_Tx[counter]);

		       //}//for loop
                   
                       
                       ///////////////////////////////////////////////////////////////////////////
                       
                       //Log Tx Message

                       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);

                       strcpy(LogTypeMesssage,"DATA_EXCHANGE_ERROR_ONLY");

                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

                       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage, Deviceid,DATA_EXCHANGE_ERROR_Type,DATA_EXCHANGE_ERROR_TxLength);

                       for(counter=0;counter<DATA_EXCHANGE_ERROR_TxLength;counter++)
		       {
                            sprintf(CommandHexLog,"%x ",DATA_EXCHANGE_ERROR_Tx[counter]);

                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop

                       writeFileLog(MessageLog);

                       memset(DATA_EXCHANGE_ERROR_Tx,'\0',LOG_ARRAY_SIZE);

                       memset(DATA_EXCHANGE_ERROR_Type,'\0',10);
                           
                       DATA_EXCHANGE_ERROR_TxLength =0;
                      
                       /////////////////////////////////////////////////////////////////////////////    
                       //Log Rx Message

                       memset(LogTypeMesssage,'\0',LOG_ARRAY_SIZE);

                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

                       strcpy(LogTypeMesssage,"DATA_EXCHANGE_ERROR_ONLY");

                       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

                       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage,
                       Deviceid,TransmitType,datalength);

                       for(counter=0;counter<datalength;counter++)
		       {
                            sprintf(CommandHexLog,"%x ",command[counter]);

                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop

                       writeFileLog(MessageLog);
                       
                       ////////////////////////////////////////////////////////////

                       

               }
               else
               {


               }

              

       }
       else
       {
             printf("\n[ LogDataExchnageV2()] No log Mode defined");
             return ;

       }
       
        return ;

       ///////////////////////////////////////////////////////////////////////////////////


}//void LogDataExchnage(unsigned char* command,int  datalength) 
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

