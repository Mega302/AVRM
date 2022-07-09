#include "Acrmlog.h"
#include <pthread.h>

static pthread_mutex_t g_LogFilemutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_LogConsolemutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_getDateTimeprocmutex = PTHREAD_MUTEX_INITIALIZER;
static char g_general_log[MAX_STR_SIZE_OF_LOG]={'\0'};
static char g_machine_fault_log[MAX_STR_SIZE_OF_LOG]={'\0'};
static char g_crisapi_timeout_log[MAX_STR_SIZE_OF_LOG]={'\0'};
bool g_LogModeType=false;
static pthread_mutex_t g_LogLevelSetmutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_SetLogLevelProcMutex= PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_WriteLogProcMutex= PTHREAD_MUTEX_INITIALIZER;
#define HOST_NAME_MAX  200
//++Set Default LogLevel to ALL
static int g_LogLevel = ALL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL Java_Cris_AscrmLog_SetLogLevel(JNIEnv *env, jclass ascrmlog, jint fnLogLevel){
              
              pthread_mutex_lock( &g_SetLogLevelProcMutex );

              pthread_mutex_lock(&g_LogLevelSetmutex);
	
	          g_LogLevel = fnLogLevel;
	          
	          pthread_mutex_unlock(&g_LogLevelSetmutex);
	          
	          printf("[Java_Cris_AscrmLog_SetLogLevel()] LogLevel: %d",g_LogLevel);
	
              pthread_mutex_unlock( &g_SetLogLevelProcMutex );
              
}//JNIEXPORT void JNICALL Java_Cris_AscrmLog_SetLogLevel(JNIEnv *, jclass, jint) end

/*
JNIEXPORT jint JNICALL Java_Cris_AscrmLog_WriteLog(JNIEnv *env, jclass ascrmlog, jbyteArray logstr){

						pthread_mutex_lock( &g_WriteLogProcMutex );
						unsigned char *strmssg=NULL;
						int counter = 0;
                        //++Get Message Array     
                        jsize strlength =  (*env)->GetArrayLength(env,logstr);
						strmssg= (char*) malloc(strlength*sizeof(char) );
						//++Get ByteArray Element from java byte array
                        jbyte *bytearray = (*env)->GetByteArrayElements(env, logstr, 0);
                        for (counter = 0; counter < strlength ; counter++) {
                           strmssg[counter] = bytearray[counter] ;
                        }//++for end
                        int rtcode =0;
                        rtcode = Ascrm_writeFileLog(cstrmssg,LogFilePathstr);
                        //Now release byte array elements
                        (*env)->ReleaseByteArrayElements(env, Reply,bytearray, 0 );
                        if( NULL !=strmssg ){
                            free(strmssg);
                        }//if end
                        pthread_mutex_unlock( &g_WriteLogProcMutex );
						return rtcode;

}//JNIEXPORT jint JNICALL Java_Cris_AscrmLog_WriteLog(JNIEnv *env, jclass ascrmlog, jbyteArray logstr) end
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Write Trans Log
static int Ascrm_DefaultwriteFileLog(char *file,char *str) {
	
     #ifdef PRINT_CONSOLE
         return WriteLogInConsole(str);
     #endif
     
     #ifdef PRINT_FILE
         return writeMDSLog(str,file);
     #endif
 
     #if !defined(PRINT_CONSOLE) && !defined(PRINT_FILE)
         return 1;
     #endif

}//static int Ascrm_DefaultwriteFileLog(char *file,char *str) end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Write Machine Log
static int Ascrm_writeFaultLog(char *str) {

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Write Machine Log
static int Ascrm_writeAPITimeoutLog(char *str){
      
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Ascrm_getDateTime(int *day,int *mon,int *yr,int *hr,int *min,int *sec,long long *milliseconds) {


      pthread_mutex_lock(&g_getDateTimeprocmutex);

	  time_t tm; 
      struct tm dttm;
	  struct timeval te; 
	  tm = time(NULL);
	  dttm = *localtime(&tm);
	  gettimeofday(&te, NULL); // get current time

	  *day =  dttm.tm_mday;
	  *mon =  dttm.tm_mon + 1;
	  *yr  =  dttm.tm_year+1900;
      *hr  =  dttm.tm_hour;
	  *min =  dttm.tm_min; 
	  *sec =  dttm.tm_sec;
	  //printf("\n dd/mm/yyyy %d / %d / %d  hh:mm:ss= %d : %d : %d",*day,*mon,*yr,*hr,*min,*sec);
      long long milliseconds2 = te.tv_sec*1000LL + te.tv_usec/1000;
      //++printf("[Ascrm_getDateTime()] Milisecond: %llu",milliseconds2 );
      *milliseconds  = milliseconds2;
      
        /*
        struct timeval tvTime;

		gettimeofday(&tvTime, NULL);

		int iTotal_seconds = tvTime.tv_sec;
		struct tm *ptm = localtime((const time_t *) & iTotal_seconds);

		int iHour = ptm->tm_hour;;
		int iMinute = ptm->tm_min;
		int iSecond = ptm->tm_sec;
		int iMilliSec = tvTime.tv_usec / 1000;
		int iMicroSec = tvTime.tv_usec;
		
		*hr  =  iHour;
	    *min =  iMinute; 
	    *sec =  iSecond;
	    *milliseconds  = iMilliSec;
	    */
      
      pthread_mutex_unlock(&g_getDateTimeprocmutex);


}//getDateTime() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int Ascrm_WriteLogInConsole(char *str) {

     
					pthread_mutex_lock(&g_LogConsolemutex);

					int day=0,mon=0,yr=0,hr=0,min=0,sec=0;
					long long milisec=0;
					int counter=0;
					char * outString = (char *)malloc(MAX_STR_SIZE_OF_LOG*2);

					if(outString == NULL){
						pthread_mutex_unlock(&g_LogConsolemutex); 
						return 0;
					}//if end

					    Ascrm_getDateTime(&day,&mon,&yr,&hr,&min,&sec,&milisec);
				  
					   //sprintf(outString,"\n[%02d/%02d/%02d] [%02d:%02d:%02d] ",day,mon,yr,hr,min,sec);
					
					    char hostname[HOST_NAME_MAX + 1];
                        
                        memset ( hostname,'\0',HOST_NAME_MAX + 1);
                        
                        size_t len = HOST_NAME_MAX + 1;
                        
                        int hostnamertcode = gethostname( hostname, len);

						//sprintf(outString,"\n[%02d/%02d/%02d] [%02d:%02d:%02d] ",day,mon,yr,hr,min,sec);
						char MonthName[20];
						memset ( MonthName,'\0', 20);
						switch(mon){
							
							case 1: strcpy(MonthName, "January");
							        break;
							
							case 2: strcpy(MonthName, "February");
							        break;
							        
							case 3: strcpy(MonthName, "March");
							        break;
							        
							case 4: strcpy(MonthName, "April");
							        break;
							        
							case 5: strcpy(MonthName, "May");
							        break;
							        
							case 6: strcpy(MonthName, "June");
							        break;
							
							case 7: strcpy(MonthName, "July");
							        break;
							        
						    case 8: strcpy(MonthName, "August");
							        break;
							        
							case 9: strcpy(MonthName, "September");
							        break;
							        
							case 10: strcpy(MonthName, "October");
							        break;
							        
							case 11: strcpy(MonthName, "November");
							        break;
							
							case 12: strcpy(MonthName, "December");
							        break;
							
					    };

						sprintf(outString,"\nAVRMAPI_<%s>LOG<%02d>.<%02d%s>.<%02d:%02d:%02d:%llu>",hostname,yr,day,MonthName,hr,min,sec,milisec);
						
						strcat(outString,str);

						for(counter=0;outString[counter]!='\0';counter++){
							printf("%c",outString[counter]);
						}//for end
						
						free(outString);

						pthread_mutex_unlock(&g_LogConsolemutex);

						return 1;


}//WriteLogInConsole() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Write Trans Log
int Ascrm_writeFileLog(char *str,char *LogFilePathstr) { 

     int rtcode =0,CurrentLogLevel=0;
     //return 
     //printf("\n[Ascrm_writeFileLog()] LogMessage: %s",str);
     //printf("\n[Ascrm_writeFileLog()] LogFilePath: %s",LogFilePathstr);
     //++rtcode = Ascrm_writeMDSLog(str,LogFilePathstr);
     
     pthread_mutex_lock(&g_LogLevelSetmutex);
	 CurrentLogLevel = g_LogLevel ;
	 pthread_mutex_unlock(&g_LogLevelSetmutex);
	 
	 //++printf("\n[Ascrm_writeFileLog()] LogLevel: %s",CurrentLogLevel);
	 rtcode = AVRM_writeFileLog(str,LogFilePathstr,CurrentLogLevel);
	 
     //printf("\n[Ascrm_writeFileLog()] FileWrite return Code: %d", rtcode);
     return 1;
	   
}//writeFileLog() end

static int AVRM_SetLogPriority(int fnLogLevel) { 
	
	pthread_mutex_lock(&g_LogLevelSetmutex);
	g_LogLevel = fnLogLevel;
	pthread_mutex_unlock(&g_LogLevelSetmutex);
	
}//++int AVRM_SetLogPriority(int fnLogLevel) end

static int AVRM_CheckPriority(int fnLogLevel) { 
	
	int CurrentLogLevel = ALL;
	pthread_mutex_lock(&g_LogLevelSetmutex);
	CurrentLogLevel = g_LogLevel ;
	pthread_mutex_unlock(&g_LogLevelSetmutex);
	if( ALL == CurrentLogLevel   ){
	   //printf("\n[AVRM_CheckPriority()] ALL Loglevel");
	   return 1; //++Success	
	}else if( OFF == CurrentLogLevel   ){
	   //printf("\n[AVRM_CheckPriority()] OFF Loglevel");
	   return 0; //++Success	
	}else if( fnLogLevel >= CurrentLogLevel ){
		//printf("\n[AVRM_CheckPriority()] fnLogLevel[%d], g_LogLevel[%d] is eligible for log write",fnLogLevel,g_LogLevel);
		return 1; //++Success
    } else {
	    //printf("\n[AVRM_CheckPriority()] fnLogLevel is not eligible for log write");
		return 0; //++Fail
    }//else end
    
}//++int AVRM_CheckPriority(int fnLogLevel) end

int AVRM_writeFileLog(char *str,char *LogFilePathstr,int fnLogLevel) { 

     //++printf("\n[AVRM_writeFileLog()] LogLevel: %d",fnLogLevel);
     //++printf("\n[AVRM_writeFileLog()] LogMessage: %s",str);
     //++printf("\n[AVRM_writeFileLog()] LogFilePath: %s",LogFilePathstr);
     int rtcode =0;
     rtcode = AVRM_CheckPriority(fnLogLevel);
     if( 1 == rtcode ){
		
		rtcode = 0;
		
		#ifdef PRINT_CONSOLE
		rtcode = Ascrm_WriteLogInConsole(str);
		#endif
		 
		#ifdef PRINT_FILE
		rtcode = Ascrm_writeMDSLog(str,LogFilePathstr);
		#endif

	 }else {
		 
	 }//else end
     //++printf("\n[AVRM_writeFileLog()] FileWrite return Code: %d", rtcode);
     return 1;
	   
}//int AVRM_writeFileLog(char *str,char *LogFilePathstr,int fnLogLevel) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int Ascrm_writeMDSLog(char *str, char *filename) {

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
static int Ascrm_writeMDSLog(char *str, char *filename) {

        //printf("\n[Ascrm_writeMDSLog()] Entry");
      
        pthread_mutex_lock(&g_LogFilemutex);

        int rtcode=0;  
        
        int FileHandle =0,CharCounter=0,totalbuffersize=0,writebuffersize=0;

        ssize_t buffersize=0;

        int day=0,mon=0,yr=0,hr=0,min=0,sec=0;
        long long milliseconds=0;


        if((str != NULL) && (filename != NULL) ) {
			
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

                        Ascrm_getDateTime(&day,&mon,&yr,&hr,&min,&sec,&milliseconds);
                        
                        char hostname[HOST_NAME_MAX + 1];
                        
                        memset ( hostname,'\0',HOST_NAME_MAX + 1);
                        
                        size_t len = HOST_NAME_MAX + 1;
                        
                        int hostnamertcode = gethostname( hostname, len);

						//sprintf(outString,"\n[%02d/%02d/%02d] [%02d:%02d:%02d] ",day,mon,yr,hr,min,sec);
						char MonthName[20];
						memset ( MonthName,'\0', 20);
						switch(mon){
							
							case 1: strcpy(MonthName, "January");
							        break;
							
							case 2: strcpy(MonthName, "February");
							        break;
							        
							case 3: strcpy(MonthName, "March");
							        break;
							        
							case 4: strcpy(MonthName, "April");
							        break;
							        
							case 5: strcpy(MonthName, "May");
							        break;
							        
							case 6: strcpy(MonthName, "June");
							        break;
							
							case 7: strcpy(MonthName, "July");
							        break;
							        
						    case 8: strcpy(MonthName, "August");
							        break;
							        
							case 9: strcpy(MonthName, "September");
							        break;
							        
							case 10: strcpy(MonthName, "October");
							        break;
							        
							case 11: strcpy(MonthName, "November");
							        break;
							
							case 12: strcpy(MonthName, "December");
							        break;
							
					    };
						
						sprintf(outString,"\nAVRMAPI_<%s>LOG<%02d>.<%02d%s>.<%02d:%02d:%02d:%llu>",hostname,yr,day,MonthName,hr,min,sec,milliseconds);

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
             
	} else { 
                printf("\n[Ascrm_writeMDSLog()] all parameters are null\n");
                rtcode= 0; 
    }

    pthread_mutex_unlock(&g_LogFilemutex);

    return rtcode;

}//writeMDSLog() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int main(){

        Ascrm_writeFileLog("HelloWorld","/home1/megalog/sample.txt");
        return 0;
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

