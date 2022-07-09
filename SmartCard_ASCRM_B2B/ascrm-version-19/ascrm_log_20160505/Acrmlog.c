#include "Acrmlog.h"

#include <pthread.h>

static pthread_mutex_t g_LogFilemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_LogConsolemutex=PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_getDateTimeprocmutex = PTHREAD_MUTEX_INITIALIZER;

static char g_general_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_machine_fault_log[MAX_STR_SIZE_OF_LOG]={'\0'};

static char g_crisapi_timeout_log[MAX_STR_SIZE_OF_LOG]={'\0'};

bool g_LogModeType=false;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Write Trans Log
static int Ascrm_DefaultwriteFileLog(char *file,char *str)
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


///////////////////////////////////////////////////////////////////////////////////////////////////


//Write Machine Log
static int Ascrm_writeFaultLog(char *str)
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


///////////////////////////////////////////////////////////////////////////////////////////////////

//Write Machine Log
static int Ascrm_writeAPITimeoutLog(char *str)
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


///////////////////////////////////////////////////////////////////////////////////////////////////

static void Ascrm_getDateTime(int *day,int *mon,int *yr,int *hr,int *min,int *sec)
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



////////////////////////////////////////////////////////////////////////////////////////////////////


int Ascrm_WriteLogInConsole(char *str)
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

	    Ascrm_getDateTime(&day,&mon,&yr,&hr,&min,&sec);
	  
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


///////////////////////////////////////////////////////////////////////////////////////////////////



//Write Trans Log
int Ascrm_writeFileLog(char *str,char *LogFilePathstr)
{ 

     //return 
     //printf("\n[Ascrm_writeFileLog()] LogMessage: %s",str);
    
     //printf("\n[Ascrm_writeFileLog()] LogFilePath: %s",LogFilePathstr);

     int rtcode =0;

     rtcode = Ascrm_writeMDSLog(str,LogFilePathstr);

     //printf("\n[Ascrm_writeFileLog()] FileWrite return Code: %d", rtcode);

     return 1;
	   

}//writeFileLog() end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int Ascrm_writeMDSLog(char *str, char *filename)
{

        //printf("\n[Ascrm_writeMDSLog()] Entry");
      
        int rtcode=0;  
           
        pthread_mutex_lock(&g_LogFilemutex);
        
        FILE * fp=NULL;

        if((str != NULL) && (filename != NULL))
	{
		
                //printf("\n[Ascrm_writeMDSLog()] Before file open");

                fp = fopen(filename,"a");

                if(fp)
		{
                        

			int day=0,mon=0,yr=0,hr=0,min=0,sec=0;

			char * outString = (char *)malloc(MAX_STR_SIZE_OF_LOG*2);

                        
			if(outString == NULL)
                        {
                               //printf("\n[Ascrm_writeMDSLog()] string create failed");
                               pthread_mutex_unlock(&g_LogFilemutex);
			       return 0;

                        }

                      
                        //printf("\n[Ascrm_writeMDSLog()] string create success");

                        Ascrm_getDateTime(&day,&mon,&yr,&hr,&min,&sec);

			sprintf(outString,"\n[%02d/%02d/%02d] [%02d:%02d:%02d] ",day,mon,yr,hr,min,sec);

			strcat(outString,str);

		        fprintf(fp,outString);

			fclose(fp);

			free(outString);
                        
                        rtcode= 1;

		}
		else
                {
		       //printf("\n[Ascrm_writeMDSLog()] file pointer get failed");
                       rtcode= 0;
                }

                
               
	}
	else
        { 
                //printf("\n[Ascrm_writeMDSLog()] all parameters are null");
                rtcode= 0; 
        }

        pthread_mutex_unlock(&g_LogFilemutex);

        return rtcode;

}//writeMDSLog() end
*/



static int Ascrm_writeMDSLog(char *str, char *filename)
{

        //printf("\n[Ascrm_writeMDSLog()] Entry");
      
        pthread_mutex_lock(&g_LogFilemutex);

        int rtcode=0;  
        
        int FileHandle =0,CharCounter=0,totalbuffersize=0,writebuffersize=0;

        ssize_t buffersize=0;

        int day=0,mon=0,yr=0,hr=0,min=0,sec=0;


        if((str != NULL) && (filename != NULL))
	{
			
                        //return the new file descriptor, or -1 if an error occurred 
		        FileHandle=open(filename,O_APPEND|O_CREAT|O_RDWR);
		        
		        if( -1 == FileHandle ){
		           printf("\n[Ascrm_writeMDSLog()] Get File Handle Failed\n");
		           pthread_mutex_unlock(&g_LogFilemutex);
		           return 0;
		        }

                        
			unsigned char * outString = (unsigned char *)malloc(MAX_STR_SIZE_OF_LOG*2);

                        if(outString == NULL)
                        {
                               printf("\n[Ascrm_writeMDSLog()] string create failed\n");
                               close(FileHandle);
                               FileHandle =-1;
                               pthread_mutex_unlock(&g_LogFilemutex);
			       return 0;

                        }

                        //printf("\n[Ascrm_writeMDSLog()] string create success");

                        Ascrm_getDateTime(&day,&mon,&yr,&hr,&min,&sec);

			sprintf(outString,"\n[%02d/%02d/%02d] [%02d:%02d:%02d] ",day,mon,yr,hr,min,sec);

			strcat(outString,str);

                        while( '\0'!= outString[CharCounter] ){

                               totalbuffersize++;
                               CharCounter++;
                        }

                        CharCounter=0;
                        while( '\0'!= outString[CharCounter] ){

			      buffersize = -1;
                              buffersize =  write( FileHandle,&outString[CharCounter], 1 );
                              //printf("\n[Ascrm_writeMDSLog()] FileHandle: %d Char: %c RtCode: %d ",FileHandle,outString[CharCounter] ,buffersize);
                              CharCounter++;
                              
                              if(-1!=buffersize){
                               writebuffersize++;
                              }
                        }

                        if(totalbuffersize != writebuffersize){
                               
                              printf("\n[Ascrm_writeMDSLog()] Write Failed\n");
                              rtcode= 0;

                        }
                        else{

                              rtcode= 1;
                        }

                        close(FileHandle);
                        free(outString);
                        FileHandle = -1; 
             
	}
	else
        { 
                printf("\n[Ascrm_writeMDSLog()] all parameters are null\n");
                rtcode= 0; 
        }

        pthread_mutex_unlock(&g_LogFilemutex);

        return rtcode;

}//writeMDSLog() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
int main(){

        Ascrm_writeFileLog("HelloWorld","/home1/megalog/sample.txt");
        return 0;


}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

