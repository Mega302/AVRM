
#include "delaytime.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

/*

void  delay_mSec(const int milisec)  
{

 
        struct timespec req = {0};
        req.tv_sec = 0;
        req.tv_nsec = milisec * 1000000L;
        if( 0 <  nanosleep( &req,(struct timespec*) NULL) )
        {
            printf("\n[delay_mSec] Delay failed");

        }
        
        return;


}//delay_mSec(const int milisec) end

*/

void delay_mSec(const int milisec) 
{
        

        unsigned char log[100];

        memset(log,'\0',100);

        struct timespec req = {0},rim={0};

	req.tv_sec = 0;

	req.tv_nsec = milisec * 1000000L;

        int rtcode =0;

        rtcode = nanosleep(&req, &rim);
        
        if( rtcode < 0 )   
	{
	      
              memset(log,'\0',100);
              sprintf(log,"[delay_mSec()] nanosleep() system call failed with return code  %d .",rtcode); 
              writeFileLog(log);

              return;
	      

	}
        else
        {
              //memset(log,'\0',100);

              //sprintf(log,"[delay_mSec()] nanosleep() system call success with return code  %d .",rtcode); 

              //writeFileLog(log);

              return;

        }

      
            
}//delay_mSec() end


/////////////////////////////////////////////////////////////////////////////////////////////////

int  Wait(unsigned int Second)
{
           

                  if(Second < 0)
                  {

                     return 0;

                  }
                  
                  struct timespec begints, endts,diffts;;
                  clock_gettime(CLOCK_MONOTONIC, &begints);
                  while(1)
                  {

                          clock_gettime(CLOCK_MONOTONIC, &endts);
                          diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                          if(diffts.tv_sec >= Second)
                          {
         			 return 1;

                          }


                 }


}//Wait(unsigned int Second) end

//////////////////////////////////////////////////////////////////////////////////////////////////

