#include "atvmlog.h"

#include <pthread.h>

static pthread_mutex_t g_LogFilemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_LogConsolemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getDateTimeprocmutex = PTHREAD_MUTEX_INITIALIZER;

static char g_general_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_machine_fault_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_crisapi_timeout_log[MAX_STR_SIZE_OF_LOG]={'\0'};

bool g_LogModeType=false;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int  SetGeneralFileLogPath(unsigned char* LogFilePath )
{

    #if  defined(PRINT_FILE)
    memset(g_general_log,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_general_log ,LogFilePath );
    printf("\n atvm log file path=%s \n",g_general_log);
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


//Write Trans Log
int writeFileLog(char *str)
{ 

	     if( true == g_LogModeType )
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
	  
	     }
	     else if( false == g_LogModeType )
	     {
		     return 0;

	     }




}//writeFileLog() end


//Write Machine Log
int writeFaultLog(char *str)
{

     #ifdef PRINT_CONSOLE
         return WriteLogInConsole(str);
     #endif
     
     #ifdef PRINT_FILE
         return writeMDSLog(str,g_machine_fault_log);
     #endif

     #if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE)
         return 1;
     #endif


}//writeFaultLog() end


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


static int WriteLogInConsole(char *str)
{

     
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


}//WriteLogInConsole() end



static int writeMDSLog(char *str, char *filename)
{
      
        int rtcode=0;  
           
        pthread_mutex_lock(&g_LogFilemutex);
        
        if((str != NULL) && (filename != NULL))
	{
		
                

		FILE * fp = fopen(filename,"a");

		if(fp)
		{
			int day=0,mon=0,yr=0,hr=0,min=0,sec=0;

			char * outString = (char *)malloc(MAX_STR_SIZE_OF_LOG*2);

			if(outString == NULL)
                        {

                               pthread_mutex_unlock(&g_LogFilemutex);
			       return 0;

                        }

                        getDateTime(&day,&mon,&yr,&hr,&min,&sec);

			sprintf(outString,"\n[%02d/%02d/%02d] [%02d:%02d:%02d] ",day,mon,yr,hr,min,sec);

			strcat(outString,str);

		        fprintf(fp,outString);

			fclose(fp);

			free(outString);
                
                        
			rtcode= 1;

		}
		else
                {
		       
                       rtcode= 0;
                }
               
	}
	else
        { 
                
		rtcode= 0;
        }

        pthread_mutex_unlock(&g_LogFilemutex);
        return rtcode;

}//writeMDSLog() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

