#include "atvmlog.h"

#include <pthread.h>

static pthread_mutex_t g_LogFilemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_LogConsolemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getDateTimeprocmutex = PTHREAD_MUTEX_INITIALIZER;

char g_general_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_machine_fault_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_crisapi_timeout_log[MAX_STR_SIZE_OF_LOG]={'\0'};


//////////////////////////////////////////////////////////////////////////////////////////////////////

int   g_LogModeLevel;

void *g_lib_handle;

int  (*g_Ascrm_writeFileLog)(char *str,char *LogFilePathstr);

int  (*g_Ascrm_WriteLogInConsole)(char *str);

char  g_logdllpath[MAX_STR_SIZE_OF_LOG];

char  g_Deviceid[MAX_STR_SIZE_OF_LOG];


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int SetSoFilePath(unsigned char* SoFilePath )
{

    
    memset(g_logdllpath,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_logdllpath ,SoFilePath );
    //printf("\n[SetSoFilePath()] log dll file path=%s \n",g_logdllpath);
    return 1;

}//int  SetSoFilePath(unsigned char* SoFilePath ) end



int  SetGeneralFileLogPath(unsigned char* LogFilePath )
{

    #if defined(PRINT_FILE)
    memset(g_general_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_general_log ,LogFilePath );
    //printf("\n[SetGeneralFileLogPath()] ascrm log file path=%s \n",g_general_log);
    #endif

    return 1;

}


int  SetMachineFaultFileLogPath(unsigned char* LogFilePath )
{

    #if defined(PRINT_FILE)
    memset(g_machine_fault_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_machine_fault_log ,LogFilePath );
    printf("\n atvmlog machine fault log file path=%s \n",g_machine_fault_log);
    #endif

    return 1;

}


int  SetAPITimeoutFileLogPath(unsigned char* LogFilePath )
{

    #if  defined(PRINT_FILE)
    memset(g_crisapi_timeout_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_crisapi_timeout_log ,LogFilePath );
    printf("\n atvm api timeout test file path=%s \n",g_crisapi_timeout_log);
    #endif

    return 1;

}



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

     
	         pthread_mutex_lock(&g_LogConsolemutex);

                ////////////////////////////////////////////////////////////////////////
                

                //printf("\n[WriteLogInConsole()] so Path: %s",g_logdllpath);

		if( NULL == g_lib_handle )
		{
				   g_lib_handle = dlopen(g_logdllpath, RTLD_LAZY);

				   if (NULL==g_lib_handle) 
				   {
				       printf("\n[writeMDSLog()] Get so memory pointer failed");
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

		      //GetFileLogDllFunctionAddress();

		      g_Ascrm_WriteLogInConsole = dlsym(g_lib_handle, "Ascrm_WriteLogInConsole");

		      if (NULL == g_Ascrm_WriteLogInConsole)  
		      {
			    printf("\n[writeMDSLog() if block] Get so memory function pointer failed");
		            return 0;
		      }
		      else
		      {
			   //printf("\n[writeMDSLog() if block] Get so memory function pointer get success");
		      }

		      

		}//if(NULL==g_Ascrm_writeFileLog) end
		else
		{
		    //printf("\n[writeMDSLog() if block] Get so memory function pointer already done");

		}

		//Now Write Log through so file
	        (*g_Ascrm_WriteLogInConsole)(str);
         
            /////////////////////////////////////////////////////////////////////////////////

	       pthread_mutex_unlock(&g_LogConsolemutex);

	       return 1;


}//WriteLogInConsole() end


//////////////////////////////////////////////////////////////////////////////////////////////////


static int writeMDSLog(char *str, char *filename)
{

      
        int rtcode=0;  
           
        pthread_mutex_lock(&g_LogFilemutex);

        ///////////////////////////////////////////////////////////////////////////////////////////

        //printf("\n[writeMDSLog()] dllLogPath: %s",g_logdllpath);

        if( NULL == g_lib_handle )
        {
			   g_lib_handle = dlopen(g_logdllpath, RTLD_LAZY);

			   if (NULL==g_lib_handle) 
			   {
			       printf("\n[writeMDSLog()] Get so memory pointer failed");
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
	            printf("\n[writeMDSLog() if block] Get so memory function pointer failed");
                    return 0;
	      }
	      else
	      {
		   //printf("\n[writeMDSLog() if block] Get so memory function pointer get success");
              }

              

        }//if(NULL==g_Ascrm_writeFileLog) end
        else
        {
            //printf("\n[writeMDSLog() if block] Get so memory function pointer already done");

        }

        //Now Write Log through so file
        g_Ascrm_writeFileLog(str,g_general_log);

        /////////////////////////////////////////////////////////////////////////////////

        pthread_mutex_unlock(&g_LogFilemutex);

        return rtcode;



}//writeMDSLog() end


////////////////////////////////////////////////////////////////////////////////////////////////////

//int Ascrm_writeFileLog(char *str,char *LogFilePathstr)
//int Ascrm_WriteLogInConsole(char *str)

//////////////////////////////////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////////////////////////

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



//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

#define NO_LOG                              0

#define OPERATION_COMPLETE_LOG              1

#define DATA_EXCHANGE_ERROR_ONLY            2

#define DATA_EXCHANGE_ERROR_SUCCESS_ONLY    3 //No Monitoring Data

#define DATA_EXCHANGE_ALL                   4 

#define ALL_LOG                             5 


*/

void LogDataExchnage( unsigned char* Deviceid,
                      unsigned char* TransmitType,
                      unsigned char* command,
                      int            datalength
                    )
{

       int counter=0;

       unsigned char MessageLog[LOG_ARRAY_SIZE];

       unsigned char CommandHexLog[LOG_ARRAY_SIZE];

       memset(MessageLog,'\0',LOG_ARRAY_SIZE);

       memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);


       ///////////////////////////////////////////////////////////////////////////////////////

       if(   (DATA_EXCHANGE_ALL == g_LogModeLevel) || 
             (DATA_EXCHANGE_ALL == g_LogModeLevel) ||
             (ALL_LOG == g_LogModeLevel) )
       {

		       memset(MessageLog,'\0',LOG_ARRAY_SIZE);
        
                       sprintf(MessageLog,"%s %s[%d]:",Deviceid,TransmitType,datalength);

                       for(counter=0;counter<datalength;counter++)
		       {
                            sprintf(CommandHexLog,"0x%x ",command[counter]);
                            strcat(MessageLog,CommandHexLog);
                             
                       }//for loop
                       

                       writeFileLog(MessageLog);

                       //printf("\n[LogDataExchnage()] String: %s\n", MessageLog);

       }//if block end
       else
       {
             return ;

       }

       ///////////////////////////////////////////////////////////////////////////////////

}//void LogDataExchnage(unsigned char* command,int  datalength) 



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

