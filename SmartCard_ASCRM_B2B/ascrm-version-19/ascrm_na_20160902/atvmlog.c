#include "atvmlog.h"

#include <pthread.h>

static pthread_mutex_t g_LogFilemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_LogConsolemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getDateTimeprocmutex = PTHREAD_MUTEX_INITIALIZER;

static char g_general_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_machine_fault_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_crisapi_timeout_log[MAX_STR_SIZE_OF_LOG]={'\0'};


//////////////////////////////////////////////////////////////////////////////////////////////////////

char  g_Deviceid[MAX_STR_SIZE_OF_LOG];

int   g_LogModeLevel;

void *g_lib_handle;

int  (*g_Ascrm_writeFileLog)(char *str,char *LogFilePathstr);

int  (*g_Ascrm_WriteLogInConsole)(char *str);

char  g_logdllpath[MAX_STR_SIZE_OF_LOG];


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int SetSoFilePath(unsigned char* SoFilePath )
{

    
    memset(g_logdllpath,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_logdllpath ,SoFilePath );
    //printf("\n[SetSoFilePath()] log dll file path=%s \n",g_logdllpath);
    return 1;

}//int  SetSoFilePath(unsigned char* SoFilePath ) end


/////////////////////////////////////////////////////////////////////////////////////////////////


int  SetGeneralFileLogPath(unsigned char* LogFilePath )
{

    #if defined(PRINT_FILE)
    memset(g_general_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_general_log ,LogFilePath );
    //printf("\n[SetGeneralFileLogPath()] ascrm log file path=%s \n",g_general_log);
    #endif

    return 1;

}


//////////////////////////////////////////////////////////////////////////////////////////////////


int  SetMachineFaultFileLogPath(unsigned char* LogFilePath )
{

    #if defined(PRINT_FILE)
    memset(g_machine_fault_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_machine_fault_log ,LogFilePath );
    printf("\n atvmlog machine fault log file path=%s \n",g_machine_fault_log);
    #endif

    return 1;

}

/////////////////////////////////////////////////////////////////////////////////////////////////

int  SetAPITimeoutFileLogPath(unsigned char* LogFilePath )
{

    #if  defined(PRINT_FILE)
    memset(g_crisapi_timeout_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_crisapi_timeout_log ,LogFilePath );
    printf("\n atvm api timeout test file path=%s \n",g_crisapi_timeout_log);
    #endif

    return 1;

}

//////////////////////////////////////////////////////////////////////////////////////////////////

//Write Trans Log
int DefaultwriteFileLog(char *file,char *str)
{
	
     #ifdef PRINT_CONSOLE
         return WriteLogInConsole(str);
     #endif
     
     #ifdef PRINT_FILE
         return writeMDSLog(str,file);
     #endif
 
     #if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE)
         return 1;
     #endif

}


////////////////////////////////////////////////////////////////////////////////////////////////////

/*

NO_LOG=0
OPERATION_COMPLETION_LOG=1
DATA_EXCHANGE_LOG_ERROR=2
DATA_EXCHANGE_LOG_ERROR_SUCCESS=3
DATA_EXCHANGE_ALL=4
ALL_LOG=5

*/

//////////////////////////////////////////////////////////////////////////////////////////////////////


//Write Trans Log
int writeFileLog(char *str)
{ 


             #ifdef PRINT_CONSOLE
		 return WriteLogInConsole(str);
	     #endif
	     
	     #ifdef PRINT_FILE
		 return writeMDSLog(str,g_general_log);
	     #endif

	     #if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE)
		 return 1;
	     #endif


}//writeFileLog() end


///////////////////////////////////////////////////////////////////////////////////////////////////


//Write Machine Log
int writeFaultLog(char *str)
{
/*


     #ifdef PRINT_CONSOLE
         return WriteLogInConsole(str);
     #endif
     
     #ifdef PRINT_FILE
         return writeMDSLog(str,g_machine_fault_log);
     #endif

     #if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE)
         return 1;
     #endif
*/

}//writeFaultLog() end


///////////////////////////////////////////////////////////////////////////////////////////////

//Write Machine Log
int writeAPITimeoutLog(char *str)
{
      
     /*
     #ifdef PRINT_CONSOLE
         return WriteLogInConsole(str);
     #endif
     
     #ifdef PRINT_FILE
         return writeMDSLog(str,g_crisapi_timeout_log);
     #endif

     #if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE)
         return 1;
     #endif
     */
     return 1;

}//writeAPITimeoutLog() end


/////////////////////////////////////////////////////////////////////////////////////////////////

void getDateTime(int *day,int *mon,int *yr,int *hr,int *min,int *sec)
{

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



///////////////////////////////////////////////////////////////////////////////////////////////

static int WriteLogInConsole(char *str)
{


            
            /////////////////////////////////////////////////////////////////////////////

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

	    return 1;

            ////////////////////////////////////////////////////////////////////////////

}//WriteLogInConsole() end


//////////////////////////////////////////////////////////////////////////////////////////////////


static int writeMDSLog(char *str, char *filename)
{


        ///////////////////////////////////////////////////////////////////////////////////////////
      
        int rtcode=0;  

        int logdllpathlegth=0;
        
        int counter=0;


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //No Log Mode Defined

        //printf("\n[writeMDSLog()] Log File Mode: %d",g_LogModeLevel);
        
        if( 0 == g_LogModeLevel )  
        {
            //printf("\n[writeMDSLog()] No Log Mode defined");
            return;

        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        pthread_mutex_lock(&g_LogFilemutex);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if(strlen (g_logdllpath) <=0 )
        {

             printf("\n[writeMDSLog()] Error Empty Log dll path");
             pthread_mutex_unlock(&g_LogFilemutex);
             return;

        }//if(strlen (g_logdllpath) <=0 ) end


        if(strlen (g_general_log) <=0 )
        {
             
             printf("\n[writeMDSLog()] Error Empty Log file name");
             pthread_mutex_unlock(&g_LogFilemutex);
             return; 
                

        }//if(strlen (g_general_log) <=0 ) end

       
        //////////////////////////////////////////////////////////////////////////////////////
        
        while (g_logdllpath[counter] != '\0')
        {
              logdllpathlegth++;
              counter++;

        }

        
        //printf("\n[SmartCard_writeMDSLog()] dllLogPath String Length: %d",logdllpathlegth);

        if(logdllpathlegth<=0)
        {
             printf("\n[SmartCard_writeMDSLog()] dllLogPath is empty");
             pthread_mutex_unlock(&g_LogFilemutex);
             return 0;
 
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //printf("\n[writeMDSLog()] dllLogPath: %s",g_logdllpath);

        if( NULL == g_lib_handle )
        {
			   g_lib_handle = dlopen(g_logdllpath, RTLD_LAZY);

			   if (NULL==g_lib_handle) 
			   {
			       printf("\n[writeMDSLog()] Get so memory pointer failed");
                               pthread_mutex_unlock(&g_LogFilemutex);
			       return false; //failed
			   }
		           else
		           {
		               //success
                               //printf("\n[writeMDSLog() if block] Get so memory pointer successfully done");
	 
		           }


        }//if( NULL == g_lib_handle ) end


        if(NULL==g_Ascrm_writeFileLog)
        {

              
              g_Ascrm_writeFileLog = dlsym(g_lib_handle, "Ascrm_writeFileLog");

              if (NULL == g_Ascrm_writeFileLog)  
	      {
	            printf("\n[writeMDSLog()] Get so memory function pointer failed");
                    pthread_mutex_unlock(&g_LogFilemutex);
                    return 0;
	      }
	      else
	      {
		   //printf("\n[writeMDSLog()] Get so memory function pointer get success");
              }

              

        }//if(NULL==g_Ascrm_writeFileLog) end
        else
        {
            //printf("\n[writeMDSLog()] Get so memory function pointer already done");

        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
        char *string = strdup(str);

        char *writemssg=NULL;

        char *token=NULL;

        if( NULL== string )
        {
             //printf("\n[SmartCard_writeMDSLog()] Duplicate String create failed");
             free(string);
             pthread_mutex_unlock(&g_LogFilemutex);
             return;

        }
        
        token=strtok(string," ");

        //Data Exchange Log
        if( ( 0 == strcmp(token,"DATA_EXCHANGE_ALL") ) &&
            ( DATA_EXCHANGE_ALL == g_LogModeLevel ) 
          ) 
        {
             //printf("\n[SmartCard_writeMDSLog()] Log Type: Data Exchange All ");

             //writemssg = replace_str(str, "DATA_EXCHANGE", " ");

             //Now Write Log through so file
             g_Ascrm_writeFileLog(str,g_general_log);

        }
        if( ( 0 == strcmp(token,"DATA_EXCHANGE_ERROR_ONLY") ) &&
            (DATA_EXCHANGE_ERROR_ONLY == g_LogModeLevel) 
          ) 
        {
             //printf("\n[SmartCard_writeMDSLog()] Log Type: Data Exchange Error ");

             //writemssg = replace_str(str, "DATA_EXCHANGE", " ");

             //Now Write Log through so file
             g_Ascrm_writeFileLog(str,g_general_log);

        }
        else if( ALL_LOG == g_LogModeLevel )          
        {

             //printf("\n[SmartCard_writeMDSLog()] Log Type: General log");

             char alllog[9600]; 
             memset(alllog,'\0',9600);
             sprintf(alllog,"[%s]: %s",g_Deviceid,str);

             if( ( 0 == strcmp(token,"DATA_EXCHANGE_ALL") ) ||
                 ( 0 == strcmp(token,"DATA_EXCHANGE_ERROR_ONLY") )
               )
             {


             }
             else
             {
		     //Now Write Log through so file
		     g_Ascrm_writeFileLog(alllog,g_general_log);
             }


        }//else if( 0 != strcmp(token,"DATA_EXCHANGE") )  end      
        else
        {

                  //printf("\n[SmartCard_writeMDSLog()] No Log mode matched");
        }

        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if(NULL != string )
        {
             free(string);
        }

        pthread_mutex_unlock(&g_LogFilemutex);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        return rtcode;



}//writeMDSLog() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//int Ascrm_writeFileLog(char *str,char *LogFilePathstr)
//int Ascrm_WriteLogInConsole(char *str)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool OpenLogdll(char* dlllogpath)
{       
 
                  
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GetFileLogDllFunctionAddress()
{

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GetConsoleLogDllFunctionAddress()
{

                   if(NULL == g_Ascrm_WriteLogInConsole)
                   {
                   
			   g_Ascrm_WriteLogInConsole = dlsym(
                               g_lib_handle, "Ascrm_WriteLogInConsole");

			   if (NULL == g_Ascrm_WriteLogInConsole)  
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

}//bool GetConsoleLogDllFunctionAddress() end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CloseLogdll()
{
		   
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////





void LogDataExchnage( int            DataExchangelevel,
                      unsigned char* Deviceid,
                      unsigned char* TransmitType,
                      unsigned char* command,
                      int            datalength
                    )
{


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
       /////////////////////////////////////////////////////////////////////////////////////////

       
       //printf("\n[LogDataExchnage()] Transmit Type: %s ",TransmitType);

       //for(counter=0;counter<datalength;counter++)
       //{
            //printf("\nTx[%d] : 0x%x ",counter,command[counter]);

       //}//for loop
       
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

       if(   (DATA_EXCHANGE_ALL == g_LogModeLevel )   ||  //3 
             (ALL_LOG           == g_LogModeLevel )       //4
         )
       {


                       strcpy(LogTypeMesssage,"DATA_EXCHANGE_ALL");

		       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
        
                       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage,
                       Deviceid,TransmitType,datalength);

                       for(counter=0;counter<datalength;counter++)
		       {
                            sprintf(CommandHexLog,"%x ",command[counter]);

                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop
                       

                       writeFileLog(MessageLog);

                       //printf("\n[LogDataExchnage()] String:%s\n", MessageLog);



       }//if block end
       else if( DATA_EXCHANGE_ERROR_ONLY == g_LogModeLevel )   //2
       {

             

             if( ( 0== strcmp("Rx",TransmitType) ) &&
                 ( datalength <= 0  )
               )
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

                       sprintf(MessageLog,"%s %s %s[%d]0x:",LogTypeMesssage,
                       Deviceid,DATA_EXCHANGE_ERROR_Type,DATA_EXCHANGE_ERROR_TxLength);

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
             //printf("\n[LogDataExchnage()] No log Mode defined");
             return ;

       }

       ///////////////////////////////////////////////////////////////////////////////////


}//void LogDataExchnage(unsigned char* command,int  datalength) 



/////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

