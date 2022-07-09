#include <errno.h>
#include <time.h>
#include <stdio.h>
int better_sleep (double sleep_time)
{

                 printf("[better_sleep()] Enter in better_sleep.");
		 struct timespec tv;
		 /* Construct the timespec from the number of whole seconds... */
		 tv.tv_sec = (time_t) sleep_time;
		 /* ... and the remainder in nanoseconds. */
		 tv.tv_nsec = (long) ((sleep_time - tv.tv_sec) * 1e+9);


                  printf("[better_sleep()]Going to sleep");
		  /* Sleep for the time specified in tv. If interrupted by a
		    signal, place the remaining time left to sleep back into tv. */
		  int rval = nanosleep (&tv, NULL);
                  printf("[better_sleep()] After call  nanosleep().");
		  if (rval == 0)
                  {
		        //Completed the entire sleep time; all done. 
		        return 0;
                  }
		  else if (errno == EINTR)
                  {
		        //Interrupted by a signal. Try again. 
		        return -1;
                  }
		  else
                  { 
		       // Some other error; bail out. 
		       return rval;

                  }

 

	
}

int main()
{

   struct timespec tim={0}, tim2={0};
   tim.tv_sec = 0;
   tim.tv_nsec = 500*1000000L;
   int rtcode=0;
   /*

   rtcode = nanosleep(&tim , &tim2);
   
   if( rtcode < 0 )   
   {
      printf("Nano sleep system call failed nanosleep rtcode = %d \n",rtcode);
      return -1;
   }
 
   printf("Nano sleep successfull \n");

   */

   rtcode = clock_nanosleep (CLOCK_MONOTONIC, 
                             0,
                             &tim,
                             NULL); 
   
   if( rtcode > 0 )   
   {
      printf("clock_nanosleep system call failed nanosleep rtcode = %d \n",rtcode);
      return -1;
   }
   else
      printf("clock_nanosleep sleep successfull \n");
 

}
