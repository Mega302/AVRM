#include "atvmparport.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        //Data register 
	//ioctl(fd, PPRDATA, &r); // read the data register  
	//ioctl(fd, PPWDATA, &r); // write the data register  

	//control register 
	//ioctl(fd, PPRCONTROL, &r); // read the control register 
	//ioctl(fd, PPWCONTROL, &r); // write the control register
	//ioctl(fd, PPFCONTROL, &frob); //"atomic" modify (read - write at once)

	//status register 
	//ioctl(fd, PPRSTATUS, &r); // read the status register  

        /*

           Mask Value:

           #define PARPORT_STATUS_ERROR      0x8  //1000b
           #define PARPORT_STATUS_SELECT     0x10 //10000b
           #define PARPORT_STATUS_PAPEROUT   0x20 //100000b
           #define PARPORT_STATUS_ACK        0x40 //1000000b
           #define PARPORT_STATUS_BUSY       0x80 //10000000b

               
      */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Open lp port
int OpenParPort()
{


                 int devhandle=-1,rtcode=-1;
           
                 //Open LPT Port
                 devhandle=open(DEVICE, O_RDWR);

                 if( devhandle < 0 ) 
		 { 

		      writeFileLog("[OpenParPort()] Lpt Open failed.");
		      return -1; 
		 } 
		 else
                 {
		      writeFileLog("[OpenParPort()] Lpt Open Successfully.");
                 }

                 //Get LPT Port Claim
                 rtcode=ioctl(devhandle, PPCLAIM);
		 
		 if(rtcode < 0) 
		 { 
		    writeFileLog("[OpenParPort()] Lpt port claim  failed.");
		    close(devhandle); 
		    return -1; 
		 } 
		 else
                 {
		     writeFileLog("[OpenParPort()] Lpt Claim Successfully.");
                     return devhandle;

                 }

                


}//end



//Close lp port
int CloseParPort(int devhandle)
{

          
                 int rtcode=-1;

                 //Release lpt port
		 rtcode = ioctl(devhandle, PPRELEASE); 

                 //Get LPT Port Claim
		 if(rtcode < 0) 
		 {
                     writeFileLog("[CloseParPort()] Lpt claim release failed.");
                     return -1; 
		 } 
		 else
                 {
		     writeFileLog("[CloseParPort()] Lpt claim release Successfully.");

                 }

                 rtcode = -1;
 
                 rtcode = close(devhandle); 

                 
		 if(rtcode < 0) 
		 {
                     writeFileLog("[CloseParPort()] Lpt port close failed.");
                     return (-1); 
		 } 
		 else
                 {
		     writeFileLog("[CloseParPort()]  Lpt close Successfully.");
                     return 1;

                 }



}//end



//example how to read the status lines. 
int status_pins(int devhandle) 
{ 
                   /*

                             //The ERROR, ACK and BUSY signals are active low when reading from the IO port. 
                              __
                              S7       S6      S5     S4      S3     S2      S1     S0

                                                                      X       X      X

                   */
		  
                  int val=0,rtcode=-1; 

		  rtcode = ioctl(devhandle, PPRSTATUS, &val); 
                
		  val^=PARPORT_STATUS_BUSY;  // BUSY needs to get inverted  10000000 (xor operation same bit xor make zero)
		  
                  //bit 7 when active, remote printer is busy and cannot accept data ,as its complement so when busy its output zero
                  printf("\nbit 7(bit low when busy): BUSY  = %s\n", ((val & PARPORT_STATUS_BUSY)==PARPORT_STATUS_BUSY)?"HI":"LO"); 

		  //bit 6 bit is pulsed when it ready to accept new character
                  printf("\nbit 6(bit is pulsed when it ready to accept new character): ACK = %s\n", ((val & PARPORT_STATUS_ACK)==PARPORT_STATUS_ACK)?"HI":"LO");  

                  //bit 5 bit high when paper out
                  printf("\nbit 5(high when paper out): PAPEROUT = %s\n", ((val & PARPORT_STATUS_PAPEROUT)==PARPORT_STATUS_PAPEROUT)?"HI":"LO"); 

                  //bit 4 high when printer is selected
                  printf("\nbit 4(high when printer is selected): SELECT = %s\n", ((val & PARPORT_STATUS_SELECT)==PARPORT_STATUS_SELECT)?"HI":"LO"); 

                  //bit 3 error when low
                  printf("\nbit 3(error when low) : ERROR = %s\n", ((val & PARPORT_STATUS_ERROR)==PARPORT_STATUS_ERROR)?"HI":"LO"); 
		  
		  return val; 



}//end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// example how to read 8 bit from the data lines 
static int read_data(int fd) 
{ 


	  int mode, res; 
	  unsigned char data; 
	  mode = IEEE1284_MODE_ECP; 
	  res=ioctl(fd, PPSETMODE, &mode); 
	  // ready to read 
	  mode=255; 
          res=ioctl(fd, PPDATADIR, &mode); 
	  // switch output driver off 
	  printf("ready to read data !\n"); 
	  fflush(stdout); 
	  sleep(10); 
	  res=ioctl(fd, PPRDATA, &data); 
	  // now fetch the data 
	  printf("data=%02x\n", data); 
	  fflush(stdout); 
	  sleep(10); 
	  data=0; 
	  res=ioctl(fd, PPDATADIR, data); 
	  return 0; 


}//end


// trivial example how to write data 
int write_data(int fd, unsigned char data) 
{
    
    return(ioctl(fd, PPWDATA, &data)); 

}



/* 
      example how to use frob ... 
      toggle STROBE on and off without messing around the other lines 
*/ 
static int strobe_blink(int fd) 
{ 

		    
		    struct ppdev_frob_struct frob; 
		    frob.mask = PARPORT_CONTROL_STROBE; 
		    //change only this pin 
		    while(1) 
		    { 
			  frob.val = PARPORT_CONTROL_STROBE; 
			  // set STROBE  
			  ioctl(fd, PPFCONTROL, &frob); 
			  usleep(500); 
			  frob.val = 0; 
			  // and clear again
			  ioctl(fd, PPFCONTROL, &frob); 
			  usleep(500); 
		    } 



}//end





static void PrintTestPage(int fd)
{

			unsigned char command[830];
			int commandSize = 0;
	
			memcpy(command, "\x1b\x1d\x61\x01", 4); //Center Alignment - Refer to Pg. 3-29
			commandSize += 4;

			memcpy(command + commandSize, "\x1b\x1c\x70\x02\x00\r\n", 7); //Stored Logo Printing - Refer to Pg. 3-38
			commandSize += 7;

			memcpy(command + commandSize, "Star Clothing Boutique\r\n",24 ); 
			commandSize += 24;

			memcpy(command + commandSize,"\x1b\x1d\x74\x03",4);
			commandSize += 4;

			memcpy(command + commandSize, "1150 King Georges Post Rd.\r\nEdison, NJ, 08837\r\n\r\n",49);
			commandSize += 49;

			memcpy(command + commandSize, "\x1b\x44\x02\x10\x22\x00", 6);  //Setting Horizontal Tab - Pg. 3-27
			commandSize += 6;

			memcpy(command + commandSize, "Date: 5/25/2011", 15);
			commandSize += 15;

			memcpy(command + commandSize, " \x09 ", 3);  //Moving Horizontal Tab - Pg. 3-26
			commandSize += 3;

			memcpy(command + commandSize, "Time: 9:10 AM\r\n------------------------------------------------\r\n\r\n", 67);
			commandSize += 67;

			memcpy(command + commandSize, "\x1b\x45", 2);
			commandSize += 2;

			memcpy(command + commandSize, "SALE \r\n", 7);
			commandSize += 7;

			memcpy(command + commandSize, "\x1b\x46", 2);
			commandSize += 2;

			memcpy(command + commandSize, "SKU \x09", 5);
			commandSize += 5;

			memcpy(command + commandSize, " Description   \x09         Total\r\n",32);
			commandSize += 32;

			memcpy(command + commandSize, "300678566 \x09  PLAN T-SHIRT\x09         10.99\r\n",42);
			commandSize += 42;

			memcpy(command + commandSize, "300692003 \x09  BLACK DENIM\x09         29.99\r\n",41);
			commandSize += 41;

			memcpy(command + commandSize, "300651148 \x09  BLUE DENIM\x09         29.99\r\n",40);
			commandSize += 40;

			memcpy(command + commandSize, "300642980 \x09  STRIPE DRESS\x09         49.99\r\n",42 );
			commandSize += 42;

			memcpy(command + commandSize, "300638471 \x09  BLACK BOOT\x09         35.99\r\n\r\n", 43);
			commandSize += 43;

			memcpy(command + commandSize, "Subtotal \x09\x09        156.95\r\n", 27);
			commandSize += 27;

			memcpy(command + commandSize, "Tax \x09\x09         00.00\r\n", 22);
			commandSize += 22;

			memcpy(command + commandSize, "------------------------------------------------\r\n", 50);
			commandSize += 50;

			memcpy(command + commandSize, "Total", 5);
			commandSize += 5;

			memcpy(command + commandSize, "\x06\x09\x1b\x69\x01\x01", 6);
			commandSize += 6;
	
			memcpy(command + commandSize, "        $156.95\r\n", 17);
			commandSize += 17;

			memcpy(command + commandSize, "\x1b\x69\x00\x00", 4);  //Cancel Character Expansion - Pg. 3-10
			commandSize += 4;

			memcpy(command + commandSize, "------------------------------------------------\r\n\r\n", 52);
			commandSize += 52;

			memcpy(command + commandSize, "Charge\r\n159.95\r\n", 16);
			commandSize += 16;

			memcpy(command + commandSize, "Visa XXXX-XXXX-XXXX-0123\r\n\r\n", 28);
			commandSize += 28;

			//Specify/Cancel White/Black Invert - Pg. 3-16
			memcpy(command + commandSize, "\x1b\x34Returns and Exchanges\x1b\x35\r\n", 27);
			commandSize += 27;

			memcpy(command + commandSize, "Within \x1b\x2d\x01", 10);
			commandSize += 10;

			memcpy(command + commandSize, "30 days", 7);
			commandSize += 7;

			memcpy(command + commandSize, "\x1b\x2d\x00", 3);  //Specify/Cancel Underline Printing - Pg. 3-15
			commandSize += 3;

			memcpy(command + commandSize, " with receipt\r\n", 15);
			commandSize += 15;

			memcpy(command + commandSize, "And tags attached\r\n\r\n", 21);
			commandSize += 21;

			memcpy(command + commandSize, "\x1b\x1d\x61\x01", 4);
			commandSize += 4;

			memcpy(command + commandSize, "\x1b\x62\x06\x02\x02", 5);
			commandSize += 5;

			memcpy(command + commandSize, " 12ab34cd56\x1e\r\n", 14);
			commandSize += 14;

			memcpy(command + commandSize, "\x1b\x64\x02", 3);  //Cut  - Pg. 3-41
			commandSize += 3;

			memcpy(command + commandSize, "\r\n\r\n", 4);  //Cut  - Pg. 3-41
			commandSize += 4;
		
			//Now print 
			write_printer(fd, command ,commandSize);




}




static ssize_t write_printer (int fd, const char *ptr, size_t count)
{


	   ssize_t wrote = 0;

	   while (wrote < count) 
	   {
		unsigned char status, control, data;
		unsigned char mask = (PARPORT_STATUS_ERROR | PARPORT_STATUS_BUSY);
		unsigned char val = (PARPORT_STATUS_ERROR  | PARPORT_STATUS_BUSY);
		struct ppdev_frob_struct frob;
		struct timespec ts;

		// Wait for printer to be ready 
		for (;;) 
		{
		    ioctl (fd, PPRSTATUS, &status);

		    if ((status & mask) == val)
		    {
		         printf("\n[write_printer()] Printer is ok");
		         break;
		    }
		    printf("\n[write_printer()] Printer either busy or in error condition");
		    ioctl (fd, PPRELEASE);
		    sleep (1);
		    ioctl (fd, PPCLAIM);
		}

		// Set the data lines 
		data =*ptr;
		ptr++;
		ioctl (fd, PPWDATA, &data);

		// Delay for a bit 
		ts.tv_sec = 0;
		ts.tv_nsec = 1000;
		nanosleep (&ts, NULL);

		// Pulse strobe 
		frob.mask = PARPORT_CONTROL_STROBE;
		frob.val = PARPORT_CONTROL_STROBE;
		ioctl (fd, PPFCONTROL, &frob);
		nanosleep (&ts, NULL);

		// End the pulse 
		frob.val = 0;
		ioctl (fd, PPFCONTROL, &frob);
		nanosleep (&ts, NULL);

		wrote++;
	    }

	    return wrote;



}//end
    




#ifdef PARPORT_MAIN

int main(int argc, char **argv) 
{ 


		 struct ppdev_frob_struct frob; 
		 int fd; int mode; 
		 
		 unsigned char test[300];
		 strcpy(test,"\n MegaDesigns Pvt. Ltd. Smaple HelloWorld \n Testing Programme for write data in Printer");
		 printf("%s",test);
		

		 //Open LPT Port
		 if((fd=open(DEVICE, O_RDWR)) < 0) 
		 { 
		     fprintf(stderr, "can not open %s\n", DEVICE); 
		     return 10; 
		 } 
		 else
		   printf("\n[Main()] Lpt Open Successfully");

		 //Get LPT Port Claim
		 if(ioctl(fd, PPCLAIM)) 
		 { 
		    perror("[Main()] PPCLAIM"); 
		    close(fd); 
		    return 10; 
		 } 
		 else
		   printf("\n[Main()] Lpt Claim Successfully");

		 
		 //read the status register 
		 //Tested
                 while(1)
                 {
                      system("clear");
		      status_pins(fd);
                 } 
		 
		 /*//Tested
		   int i=0;
		   write(fd,test, strlen(test)); 
		 */

		 //PrintTestPage(fd);

		 //Release lpt port
		 ioctl(fd, PPRELEASE); 
		 close(fd); 
		 
		 return 0; 



}//end



#endif
