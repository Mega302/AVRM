
#include "upsstatus.h"


int CheckAPCServiceStatus()
{

         
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
		  return (-1);
	  }

          //Now Store command output to an array
	  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) 
	  {

                 memset(log,'\0',200);
		 sprintf(log,"[CheckAPCServiceStatus()] %s .", path);
                 writeFileLog(log);
         
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
		            writeFileLog(log);

		            if( NULL != pch  )
		            {

		                           memset(log,'\0',200);
					   sprintf(log,"[CheckAPCServiceStatus()while if block ] %s .",pch);
				           writeFileLog(log);

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
				                writeFileLog("[CheckAPCServiceStatus()] ups daemon is running.");
				                rtcode = 1; //ups status is running
				                break;
				           }
				           //else if(0 == strcmp ( pch,"stopped" ) )
                                           else if(0 == strcmp ( mssg,"stopped" ) )
				           {
				                writeFileLog("[CheckAPCServiceStatus()] ups daemon is stopped.");
				                rtcode=  2; //ups status is stopped
				                break;
				           }
				           //else if(0 == strcmp ( pch,"dead" ) )
                                           else if(0 == strcmp ( mssg,"dead" ) )
				           {
				                writeFileLog("[CheckAPCServiceStatus()] ups daemon is dead but its process id locked /var/lock/subsys folder.need to unlock it.");
				                rtcode=  3; //ups status is dead (need to unlock its locks)
				                break;
				           }
				           else 
				           {
				               writeFileLog("[CheckAPCServiceStatus()] Still No Matching found.");
				                
				           }

				           //go to next string
				           pch = strtok (NULL, " ,.-");

		            }
		            

	  }

          //close 
	  pclose(fp);
           
          return rtcode;



}//CheckAPCServiceStatus() end here


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void RemoveSpaceFromString(char *Dest,char *Source)
{


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

                                       
				    }

				     Dest[j]= '\0';
  
                                     //memset(log,'\0',200);
				     //sprintf(log,"[RemoveSpaceFromString()] Destination String : %s ",Dest);
		                     //writeFileLog(log);
                              
				     return;



}//RemoveSpaceFromString() end here


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//COMMLOST  
int GetAPCServiceStatus()
{

          writeFileLog("[GetAPCServiceStatus()] Entry." );

          FILE *fp;
	  int status;
	  char path[4096];
          memset(path,'\0',4096);
          char log[200];
          memset(log,'\0',200);

	  //Open the command for reading
	  fp = popen("apcaccess status 127.0.0.1:3551 | grep STATUS", "r");
	  if (fp == NULL)
	  {
                  writeFileLog("[GetAPCServiceStatus()] Failed to run command." );
		  return (-1);
	  }

          

	  //Now Store command output to an array
	  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) 
	  {

                
                 memset(log,'\0',200);
		 sprintf(log,"[GetAPCServiceStatus() Store command output block] %s .", path);
                 writeFileLog(log);
				       
         
          }

          char *pch=NULL;
          int rtcode=-1;
          pch = strtok (path," ,.-");
	  while (pch != NULL)
	  {

		
                    memset(log,'\0',200);
		    sprintf(log,"[GetAPCServiceStatus() while block] String = '%s' .",pch);
                    writeFileLog(log);
				     
		    pch = strtok (NULL, " ,.-");
                    
                    if(pch != NULL)
                    {

                           memset(log,'\0',200);
		           sprintf(log,"[GetAPCServiceStatus() while if block ] String = '%s' .",pch);
                           writeFileLog(log);

                           if( 0 == strcmp ( pch,"ONLINE" ))
                           { 
                                rtcode = 0; //main power supply on
                                break;
                           }
                           else if(0 == strcmp ( pch,"OFFLINE" ) || 0 == strcmp ( pch,"ONBATT" ) )
                           {
                                rtcode=  1; //main power is down and ups on
                                break;
                           }
                           
                    }


	  }

          //close 
	  pclose(fp);
          writeFileLog("[GetAPCServiceStatus()] Exit." );
          return rtcode;


}//GetAPCServiceStatus() end here


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Battery status
int GetAPCBatteryCharge()
{

         
          FILE *fp;
	  int status;
	  char path[4096];
          memset(path,'\0',4096);
          char log[200];
          memset(log,'\0',200);
          int charge=-1;

	  //Open the command for reading
	  fp = popen("apcaccess status 127.0.0.1:3551 | grep BCHARGE", "r");
	  if (fp == NULL)
	  {
                  writeFileLog("[GetAPCBatteryCharge()] Failed to run command." );
		  return (-1);
	  }

          

	  //Now Store command output to an array
	  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) 
	  {

                
                 memset(log,'\0',200);
		 sprintf(log,"[GetAPCBatteryCharge() store command output] %s.", path);
                 writeFileLog(log);
				       
         
          }

          char *pch=NULL;
          int rtcode=-1;
          pch = strtok (path," ,.-");
	  while (pch != NULL)
	  {

		
                    memset(log,'\0',200);
		    sprintf(log,"[GetAPCBatteryCharge() while block] String = '%s' .",pch);
                    writeFileLog(log);
				     
		    if(pch != NULL)
                    {

                           memset(log,'\0',200);
		           sprintf(log,"[GetAPCBatteryCharge() while if block ] String = '%s' .",pch);
                           writeFileLog(log);

                           if( 0 == strcmp ( pch,"BCHARGE" ))
                           { 
                                writeFileLog("[GetAPCBatteryCharge()] BCHARGE string found. " );
                                pch = strtok (NULL, " ,-:");
                                charge=-1;
                                charge=atoi(pch);
                                memset(log,'\0',200);
		                sprintf(log,"[GetAPCBatteryCharge() while if block ] Charge= '%d' .",charge);
                                writeFileLog(log);
                                pclose(fp);
                                return charge;
 
                           }
                         

                    }

                    pch = strtok (NULL, " ,-");


	  }

          //close 
          writeFileLog("[GetAPCBatteryCharge()] Unable to find BCHARGE string Exception found. " );
	  pclose(fp);
          return charge;


}//GetAPCBatteryCharge() end here


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int StartAPCServiceStatus()
{

        
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
                  writeFileLog("[StartAPCServiceStatus()]Failed to run command." );
		  return (-1);
	  }

          

	  //Now Store command output to an array
	  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) 
	  {

                
                 memset(log,'\0',200);
		 sprintf(log,"[StartAPCServiceStatus()] %s .", path);
                 writeFileLog(log);
				     
         
          }

          

          char *pch=NULL;
          int rtcode=2;//abnormal service restart status read
          pch = strtok (path," ,.-");
	  while (pch != NULL)
	  {

		
                    memset(log,'\0',200);
		    sprintf(log,"[StartAPCServiceStatus() while block] %s .",pch);
                    writeFileLog(log);

		    pch = strtok (NULL, " ,.-");

                    if(pch != NULL)
                    {
                           
		           memset(log,'\0',200);
			   sprintf(log,"[StartAPCServiceStatus() while if block] %s .",pch);
		           writeFileLog(log);

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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ups_delay_mSec(int milisec) 
{

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

int main()
{

	   
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
