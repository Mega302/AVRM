#include "upsstatus.h"
#include "currencyreturn.h"


int CheckAPCServiceStatus() {

         
          FILE *fp;
	  int status;
	  char path[4096];
          memset(path,'\0',4096);
          char log[200];
          memset(log,'\0',200);

	  //Open the command for reading
	  //fp = popen("service apcupsd status | grep running", "r");
          fp = popen("sudo /etc/init.d/apcupsd status", "r");
	  if (fp == NULL)
	  {
                  writeFileLog("[CheckAPCServiceStatus()] Failed to run command .");
                  AVRM_writeFileLog("[CheckAPCServiceStatus()] Failed to run command .",ERROR);
		  return (-1);
	  }

          //Now Store command output to an array
	  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) 
	  {

                 memset(log,'\0',200);
		 sprintf(log,"[CheckAPCServiceStatus()] %s .", path);
                 //writeFileLog(log);
                 AVRM_writeFileLog(log,INFO);
         
          }


          char *pch=NULL;
          char mssg[100];
          memset(mssg,'\0',100);
          int rtcode=4;//by default ups service status unknown
          pch = strtok (path," ,.-\n");
	  while (pch != NULL)
	  {


		            memset(log,'\0',200);
			    sprintf(log,"[CheckAPCServiceStatus() while block] %s .",pch);
		            //writeFileLog(log);
		            AVRM_writeFileLog(log,INFO);

		            if( NULL != pch  )
		            {

		                           memset(log,'\0',200);
					   sprintf(log,"[CheckAPCServiceStatus()while if block ] %s .",pch);
				           //writeFileLog(log);
				                   AVRM_writeFileLog(log,INFO);

		                           //memset(log,'\0',200);

					   //sprintf(log,"[CheckAPCServiceStatus()while if block ] String length : %d .",strlen(pch) );
				           //writeFileLog(log);
					         

				           memset(mssg,'\0',100);

		                           RemoveSpaceFromString(mssg,pch);

		                           //memset(log,'\0',200);

					   //sprintf(log,"[CheckAPCServiceStatus()while if block ]After Space Removed String length : %d .",strlen(mssg) );
				           //writeFileLog(log);
					   

										       
				           //if( 0 == strcmp ( pch,"running" ))
                                           if( 0 == strcmp ( mssg,"running" ))
				           { 
				                //writeFileLog("[CheckAPCServiceStatus()] ups daemon is running.");
				                AVRM_writeFileLog("[CheckAPCServiceStatus()] ups daemon is running.",INFO);
				                rtcode = 1; //ups status is running
				                break;
				           }
				           //else if(0 == strcmp ( pch,"stopped" ) )
                                           else if(0 == strcmp ( mssg,"stopped" ) )
				           {
				                //writeFileLog("[CheckAPCServiceStatus()] ups daemon is stopped.");
				                AVRM_writeFileLog("[CheckAPCServiceStatus()] ups daemon is stopped.",INFO);
				                rtcode=  2; //ups status is stopped
				                break;
				           }
				           //else if(0 == strcmp ( pch,"dead" ) )
                                           else if(0 == strcmp ( mssg,"dead" ) )
				           {
				                //writeFileLog("[CheckAPCServiceStatus()] ups daemon is dead but its process id locked /var/lock/subsys folder.need to unlock it.");
				                AVRM_writeFileLog("[CheckAPCServiceStatus()] ups daemon is dead but its process id locked /var/lock/subsys folder.need to unlock it.",INFO);
				                rtcode=  3; //ups status is dead (need to unlock its locks)
				                break;
				           }
				           else 
				           {
				               //writeFileLog("[CheckAPCServiceStatus()] Still No Matching found.");
				               AVRM_writeFileLog("[CheckAPCServiceStatus()] Still No Matching found.",INFO);
				                
				           }

				           //go to next string
				           pch = strtok (NULL, " ,.-");

		            }
		            

	  }

          //close 
	  pclose(fp);
           
          return rtcode;



}//CheckAPCServiceStatus() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RemoveSpaceFromString(char *Dest,char *Source) {


                    int i=0,j=0;
                    char log[200];
                    memset(log,'\0',200);
				    for(; Source[i] != '\0'; i++)
				    {

                            //memset(log,'\0',200);
				            //sprintf(log,"[RemoveSpaceFromString()] Source[%d] = %d ",i,Source[i]);
		                    //writeFileLog(log);

							if( ( Source[i] != ' ' ) &&  ( Source[i] != '\n' ) )
												{
								 Dest[j] = Source[i];
								 //memset(log,'\0',200);
								 //sprintf(log,"[RemoveSpaceFromString()] Destination[%d] = %d ",j,Dest[j]);
								 //writeFileLog(log);
								 j++;
							}

                                       
				    }//for end

				     Dest[j]= '\0';
  
                     //memset(log,'\0',200);
				     //sprintf(log,"[RemoveSpaceFromString()] Destination String : %s ",Dest);
		             //writeFileLog(log);
                              
				     return;



}//RemoveSpaceFromString() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void removeSpaces(char *str) {
	 
    char log[200];
    memset(log,'\0',200);
          
    //To keep track of non-space character count 
    int count = 0; 
    // Traverse the given string. If current character 
    // is not space, then place it at index 'count++' 
    for (int i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i]; // here count is incremented 
        }//if end
    }//for end
    str[count] = '\0'; 
    
    memset(log,'\0',200);
	sprintf(log,"[removeSpaces()] String:%s", str);
    //writeFileLog(log);
    AVRM_writeFileLog(log,INFO);
    
} //void removeSpaces(char *str) end

int Cstrcmp(char* s1, char* s2) {  
		
		int i;  
		for (i = 0; '\0'!=s1[i]; ++i)  {  
		     
		     printf("[Cstrcmp()] %c", s1[i] );
			 //printf("[Cstrcmp()] %c", s2[i] );
			    
		     if (s1[i] != s2[i]) {
			    //printf("[Cstrcmp()] %c", s1[i] );
			    //printf("[Cstrcmp()] %c", s2[i] );
                return 0;
			 } //if end
			 
			 printf("\n" );
			 
		} //for end
	    
	    return 1;  
		
}  //int ic_strcmp(string s1, string s2)   end

//++Get APC Service Status
int GetAPCServiceStatus() {
         
          FILE *fp;
		  int status;
		  char path[20];
          memset(path,'\0',20);
          char output[20];
          memset(output,'\0',20);
          int rtcode=-1;
          char log[200];
          memset(log,'\0',200);
          
          AVRM_writeFileLog("[GetAPCServiceStatus()] Entry." ,INFO);

		  //++Open the command for reading
		  //++fp = popen("sudo apcaccess status 127.0.0.1:3551 | grep STATUS", "r");
		  fp = popen(" sudo apcaccess status 127.0.0.1:3551 | grep STATUS | awk '{print $3}' ", "r");
		  if (fp == NULL){
			  //writeFileLog("[GetAPCServiceStatus()] Failed to run command." );
			  AVRM_writeFileLog("[GetAPCServiceStatus()] Failed to run command.",INFO);
			  AVRM_writeFileLog("[GetAPCServiceStatus()] Exit." ,INFO);
			  return (COMMUNICATION_FAILURE);
		  }//if end
		  
		  //++Now Store command output to an array
		  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) {
                                
                 
                 RemoveSpaceFromString(output,path);
                 
                 memset(log,'\0',200);
				 sprintf(log,"[GetAPCServiceStatus()] Command OutPut:%s", output);
                 //writeFileLog(log);
                 AVRM_writeFileLog(log,INFO);
                 
                 //memset(log,'\0',200);
				 //sprintf(log,"[GetAPCServiceStatus()] ONLINE Comapre:%d", strcmp ( output,"ONLINE" ) );
                 //writeFileLog(log);
                 
                 //memset(log,'\0',200);
				 //sprintf(log,"[GetAPCServiceStatus()] ONBATT Comapre:%d", Cstrcmp ( path,"ONBATT" ) );
                 //writeFileLog(log);
                 
                 //memset(log,'\0',200);
				 //sprintf(log,"[GetAPCServiceStatus()] OFFLINE Comapre:%d", Cstrcmp ( path,"OFFLINE" ) );
                 //writeFileLog(log);
                 
                 if( strlen(output) <= 0 ){
					 memset(log,'\0',200);
		             sprintf(log,"[GetAPCBatteryCharge()] Empty Status String Found");
                     //writeFileLog(log);
                     AVRM_writeFileLog(log,INFO);
                     break;
			     }//if end
			     
                 if( 0 == strcmp ( output,"ONLINE" ) ) { 
					rtcode = 0; //ac power on
					break;
                 }else if(0 == strcmp ( output,"OFFLINE" ) || 0 == strcmp ( output,"ONBATT" ) ) {
                    rtcode=  1; //ac power off
                    break;
                 }//else end
                 
		  }//while end
                           
          //close 
	      pclose(fp);
	      AVRM_writeFileLog("[GetAPCServiceStatus()] Exit." ,INFO);
          return rtcode;


}//GetAPCServiceStatus() end here

//++Battery status
int GetAPCBatteryCharge() {

          
          FILE *fp;
		  int status;
		  char path[20];
          memset(path,'\0',20);
          char log[200];
          memset(log,'\0',200);
          char output[20];
          memset(output,'\0',20);
          int charge=-1;
          
          AVRM_writeFileLog("[GetAPCBatteryCharge()] Entry." ,INFO);

		  //Open the command for reading
		  fp = popen("sudo apcaccess status 127.0.0.1:3551 | grep BCHARGE | awk '{print $3}'", "r");
		  if (fp == NULL){
			  //writeFileLog("[GetAPCBatteryCharge()] Failed to run command." );
			  AVRM_writeFileLog("[GetAPCBatteryCharge()] Failed to run command." ,INFO);
			  AVRM_writeFileLog("[GetAPCBatteryCharge()] Exit." ,INFO);
			  return (COMMUNICATION_FAILURE);
		  }//if end

		  //Now Store command output to an array
		  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) {
			                  
                 RemoveSpaceFromString(output,path);
                 
                 memset(log,'\0',200);
				 sprintf(log,"[GetAPCBatteryCharge()] Command OutPut: %s", output);
                 //writeFileLog(log);
                 AVRM_writeFileLog(log ,INFO);
                 
                 if( strlen(output) <= 0 ){
					 memset(log,'\0',200);
		             sprintf(log,"[GetAPCBatteryCharge()] Empty Charge String Found");
                     //writeFileLog(log);
                     AVRM_writeFileLog(log ,INFO);
                     break;
			     }//if end
                 charge=-1;
                 charge=atoi(output);
                 memset(log,'\0',200);
		         sprintf(log,"[GetAPCBatteryCharge()] Charge= '%d' .",charge);
                 //writeFileLog(log);
                 AVRM_writeFileLog(log ,INFO);
                 break;
 
          }//++while end

          //++close 
          pclose(fp);
          AVRM_writeFileLog("[GetAPCBatteryCharge()] Exit." ,INFO);
          return ( charge );


}//GetAPCBatteryCharge() end here

int StartAPCServiceStatus() {

        
          FILE *fp;
	  int status;
	  char path[4096];
          memset(path,'\0',4096);
          char log[200];
          memset(log,'\0',200);

	  //Open the command for reading
	  fp = popen("sudo /etc/init.d/apcupsd restart | grep 'Starting UPS monitoring:'", "r");

	  if (fp == NULL)
	  {
                  //writeFileLog("[StartAPCServiceStatus()]Failed to run command." );
                   AVRM_writeFileLog("[StartAPCServiceStatus()]Failed to run command." ,INFO);
		  return (-1);
	  }

          

	  //Now Store command output to an array
	  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) 
	  {

                
                 memset(log,'\0',200);
		 sprintf(log,"[StartAPCServiceStatus()] %s .", path);
                 //writeFileLog(log);
                 AVRM_writeFileLog(log ,INFO);
				     
         
          }

          

          char *pch=NULL;
          int rtcode=2;//abnormal service restart status read
          pch = strtok (path," ,.-");
	  while (pch != NULL)
	  {

		
                    memset(log,'\0',200);
		    sprintf(log,"[StartAPCServiceStatus() while block] %s .",pch);
                    //writeFileLog(log);
                    AVRM_writeFileLog(log ,INFO);

		    pch = strtok (NULL, " ,.-");

                    if(pch != NULL)
                    {
                           
		           memset(log,'\0',200);
			   sprintf(log,"[StartAPCServiceStatus() while if block] %s .",pch);
		           //writeFileLog(log);
		           AVRM_writeFileLog(log ,INFO);

                           if( 0 == strcmp ( pch,"OK" ))
                           { 
                                rtcode = 1; //successfully started ups service
                                break;
                           }
                           else if(0 == strcmp ( pch,"FAILED" ) )
                           {
                                rtcode=  0; //unsuccessfull to start  ups service
                                break;
                           }
                         

                    }


	  }

          //close 
	  pclose(fp);
          return rtcode;


}//StartAPCServiceStatus() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ups_delay_mSec(int milisec)  {

        struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = milisec * 1000000L;
        //nanosleep(&req, (struct timespec *)NULL);
        clock_nanosleep(CLOCK_MONOTONIC,
                        0,
                        &req,
                        NULL
                       );


}//ups_delay_mSec() end here

#ifdef UPSSTATUS_MAIN

int main() {

	   
	   //printf("\nCheckAPCServiceStatus() Return Code=%d",CheckAPCServiceStatus());

           //printf("\nGetAPCServiceStatus() Return Code=%d",StartAPCServiceStatus());
           while(1)
           {
	         printf("\n\n\n[main()]GetAPCServiceStatus() Return Code=%d",GetAPCServiceStatus());
                 //GetAPCBatteryCharge();
                 //ups_delay_mSec(500);
           }

          /*

	  FILE *fp;
	  int status;
	  char path[1035];

	  //Open the command for reading
	  fp = popen("apcaccess", "r");
	  if (fp == NULL)
	  {
	  
		  printf("Failed to run command\n" );
		  exit;
	  }

	  //Read the output a line at a time - output it.
	  while (fgets(path, sizeof(path)-1, fp) != NULL) 
	  {

	      printf("%s", path);


	  }

	  //close 
	  pclose(fp);

          */


          return 0;


}

#endif
