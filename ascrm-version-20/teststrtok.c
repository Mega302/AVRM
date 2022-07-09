#include <stdio.h> 
#include <string.h> 
 int GetAPCServiceStatus() ;
int main() 
{ 
	/*
    char str[] = "STATUS   : ONLINE"; 
  
    // Returns first token 
    char* token = strtok(str, ":"); 
  
    // Keep printing tokens while one of the 
    // delimiters present in str[]. 
    while (token != NULL) { 
        printf("%s\n", token); 
        token = strtok(NULL, "-"); 
    } 
    */
    GetAPCServiceStatus();
    return 0; 
} 


int GetAPCServiceStatus() {

         
          FILE *fp;
		  int status;
		  char path[4096];
          memset(path,'\0',4096);
          int rtcode=-1;
          char log[200];
          memset(log,'\0',200);

		  //Open the command for reading
		  fp = popen("sudo apcaccess status 127.0.0.1:3551 | grep STATUS", "r");
		  if (fp == NULL){
			  printf("[GetAPCServiceStatus()] Failed to run command." );
			  return (-1);
		  }//if end
		  
		  //++Now Store command output to an array
		  while ( NULL!= fgets(path, sizeof(path)-1, fp) ) {
                 memset(log,'\0',200);
				 sprintf(log,"[GetAPCServiceStatus() Store command output block] %s .", path);
                 printf("\n%s",log);
		  }//while end

          
          char *pch=NULL;
          pch = strtok (path,":");
		  while (pch != NULL) {

		            memset(log,'\0',200);
		            sprintf(log,"[GetAPCServiceStatus() while block] String = '%s' .",pch);
                    printf("\n%s",log);
				    pch = strtok (NULL, ":");
                    if(pch != NULL) {

                           memset(log,'\0',200);
		                   sprintf(log,"[GetAPCServiceStatus() while if block ] String = '%s' .",pch);
                           printf("\n%s",log);

                           if( 0 == strcmp ( pch,"ONLINE" )) { 
                                rtcode = 1; //ac power on
                                printf("\n[GetAPCServiceStatus() while if block ] String Matched ONLINE");
                                break;
                           }else if(0 == strcmp ( pch,"OFFLINE" ) || 0 == strcmp ( pch,"ONBATT" ) ) {
							    printf("\n[GetAPCServiceStatus() while if block ] String Matched OFFLINE or ONBATT");
                                rtcode=  0; //ac power off
                                break;
                           }//else end
                           
                    }//if end


	      }//while end
          
          
          //close 
	      pclose(fp);
          return rtcode;


}//GetAPCServiceStatus() end here



