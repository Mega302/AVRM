/*
 * Copyright © Mega Designs Pvt. Ltd.
 *
 * This source code is not part of any free software and so
 * not avilable for free usage and distribution. Only the
 * specified authorties of CRIS, India can use it for integration
 * of Currency Operated Ticket Vending Machine and related projects.
 *
 *
 * File : serial.c
 *
 * Usage: Source to make physical data exchange from/to the serial port
 *
 */

///////////////////////////////////////////Start:Serial.c////////////////////////////////////////////////////////////

#include "serialc.h"

/*
Function name:		OpenPort
Functionality:		Opens the serial port
Input parameters:	port number, get the port handler, which will be used for read,write function.
Return value: 		success or fail, 
*/
int  OpenPort_C(const int port_nmbr, int* const h_commport)
{

	int rtcode=1;
        if(port_nmbr <0 )
        {

             return(CCTALK_FAIL); //Error Code
 
        }
        struct termios portParams;
	char com_port[30];
        memset(com_port,'\0',30);
        #if defined(BOARDUART)
        rtcode=sprintf(com_port,"/dev/ttyS%d",port_nmbr);
        #endif
        #if defined(USBUART)
        rtcode=sprintf(com_port,"/dev/ttyUSB%d",port_nmbr);
        #endif
        unsigned char log[100];
        memset(log,'\0',100);
        sprintf(log,"[OpenPort()] OpenPort_C : %s .",com_port);
        writeFileLog(log);

        if(rtcode <0 ) //sprintf return 0 on failure ,On success, the total number of characters written is returned
        {

             return(CCTALK_FAIL); //Error Code

        }
        
        *h_commport = open(com_port,O_RDWR | O_NOCTTY | O_NDELAY);
  	if(*h_commport < 0)
	{
            return(CCTALK_FAIL); //Error Code
	}
      /*if there is no char in buff then read() function will not block(wait) the program,    	               
       rather when it gets char in buff then it will autometically read by read() function, 
       like interrupt or event. Upon successful completion for F_SETFL return Value other than -1,
       Otherwise, -1 shall be returned and errno set to indicate the error.
      */       
                
        rtcode=fcntl(*h_commport,F_SETFL,FNDELAY);   
       
        if(-1==rtcode)
        return(CCTALK_FAIL);
        /*tcgetattr RETURN VALUE:Upon successful completion, 0 shall be returned. Otherwise, 
         -1 shall be returned and errno set to indicate the error.
         */
        rtcode=tcgetattr(*h_commport,&portParams);          // read serial port parameters
        
        if(-1==rtcode)
        {
            return(CCTALK_FAIL);
        }
         
        /*
           Set the baud rates to 9600bps
           cfsetispeed && cfsetospeed RETURN VALUE:Upon successful completion, 0 shall be returned. Otherwise, 
         -1 shall be returned and errno set to indicate the error.
         
        */
        rtcode=cfsetispeed(&portParams, B9600);
        if(-1==rtcode)
        {
             return(CCTALK_FAIL);
        }
        rtcode=cfsetospeed(&portParams, B9600);
        if(-1==rtcode)
        {
            return(CCTALK_FAIL);
        }
        
         /*
          * Enable the receiver and set local mode...
         */

        portParams.c_cflag |= (CLOCAL | CREAD);
       
        //DataBits 8 ,No parity ,1 Stop Bits (8N1) :
        portParams.c_cflag &= ~PARENB;
        portParams.c_cflag &= ~CSTOPB;
        portParams.c_cflag &= ~CSIZE;
        portParams.c_cflag |= CS8;
        //Other Option
        portParams.c_lflag &= ~(ICANON | ECHO | ISIG);  //raw data input 
        portParams.c_oflag &= ~OPOST;                   //raw output
        portParams.c_cflag &= ~CRTSCTS;                 //Disable hardware flow control  
        portParams.c_iflag &= ~(IXON | IXOFF | IXANY);  //Disable software flow control


        //Input flags - Turn off input processing
        //convert break to null byte, no CR to NL translation,
        //no NL to CR translation, don't mark parity errors or breaks
        //no input parity check, don't strip high bit off,
        //no XON/XOFF software flow control
        //
        portParams.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

	//
	// Output flags - Turn off output processing
	// no CR to NL translation, no NL to CR-NL translation,
	// no NL to CR translation, no column 0 CR suppression,
	// no Ctrl-D suppression, no fill characters, no case mapping,
	// no local output processing
	//
        // config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
        //                     ONOCR | ONOEOT| OFILL | OLCUC | OPOST);


        portParams.c_oflag &= ~(OPOST);



	//
	// No line processing:
	// echo off, echo newline off, canonical mode off, 
	// extended input processing off, signal chars off
	//
        portParams.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);


        /*

	//
	// One input byte is enough to return from read()
	// Inter-character timer off
	//
        //portParams.c_cc[VMIN]  = 1;
        //portParams.c_cc[VTIME] = 0;

           portParams.c_cc[VINTR]    = 0;     // Ctrl-c 
	   portParams.c_cc[VQUIT]    = 0;     // Ctrl-\ 
	   portParams.c_cc[VERASE]   = 0;     // del 
	   portParams.c_cc[VKILL]    = 0;     // @ 
	   portParams.c_cc[VEOF]     = 0;     // Ctrl-d 
	   portParams.c_cc[VTIME]    = 0;     // inter-character timer unused 
	   portParams.c_cc[VMIN]     = 0;     // blocking read until 1 character arrives 
	   portParams.c_cc[VSWTC]    = 0;     // '\0' 
	   portParams.c_cc[VSTART]   = 0;     // Ctrl-q 
	   portParams.c_cc[VSTOP]    = 0;     // Ctrl-s 
	   portParams.c_cc[VSUSP]    = 0;     // Ctrl-z 
	   portParams.c_cc[VEOL]     = 0;     // '\0' 
	   portParams.c_cc[VREPRINT] = 0;     // Ctrl-r 
	   portParams.c_cc[VDISCARD] = 0;     // Ctrl-u 
	   portParams.c_cc[VWERASE]  = 0;     // Ctrl-w 
	   portParams.c_cc[VLNEXT]   = 0;     // Ctrl-v 
	   portParams.c_cc[VEOL2]    = 0;     // '\0' 

       */


        /*
         tcsetattr: Return Value
         Upon successful completion, 0 is returned. Otherwise, -1 is returned and errno is set to indicate the error.
       */
        rtcode=tcsetattr(*h_commport,TCSANOW,&portParams);  // write serial port parameters. TCSANOW will make data instantly available when sending & receiving.
        if(-1==rtcode)
        {
            return(CCTALK_FAIL);
        }   
 
        //If all settings done return SUCCESS
        return(CCTALK_SUCCESS);


}



/*
Function name:		Make_RTS_DTR_High
Functionality:		Make High DTR and RTS Line
Input parameters:	Serial Port Handle
Return value: 		success or fail

int Make_RTS_DTR_High(int *h_commport)
{
    int serstat;
    // Read the RTS pin status.
    ioctl(*h_commport, TIOCMGET, &serstat);
   //On DTR and RTS (Means Positive Voltage in Port pin respect to ground)   (Logic 1)
    serstat |=   TIOCM_DTR;
    serstat |=   TIOCM_RTS;

   //Set New Settings
   if (ioctl(*h_commport, TIOCMSET, &serstat))
   return(FAIL);
   
   else
   return(SUCCESS);

}

*/

/*
Function name:		ClosePort
Functionality:		Closes an open serial port
Input parameters:	Serial port handle
Return value:		Always return SUCCESS
*/
int  ClosePort_C(const int h_commport)
{

        /*close() returns zero on success. On error,
          -1 is returned, and errno is set appropriately.
        */
        int rtcode=1;
        rtcode=close(h_commport);
        if(0==rtcode)
        {
	    return (CCTALK_SUCCESS);
        }
        else
        {
           return CCTALK_FAIL;
        }


}


/*
Function name:		SendSingleByteToSerialPort
Functionality:		Sends a byte to serial port
Input parameters:	Serial port handle, Byte to send
Return value:		Returns SUCCESS in case of successful write to port, otherwise returns FAIL
*/
int SendSingleByteToSerialPort_C(const int h_commport, const unsigned char bdata)
{

        int rtcode =-1;
        unsigned char log[100];
        memset(log,'\0',100);

	if(h_commport < 0)
        {
               //writeFileLog("[SendSingleByteToSerialPort_C() wrong handle] Com port write failed.");
               return CCTALK_FAIL; //return Error

        }

        rtcode = write(h_commport,&bdata,1);
        
        //memset(log,'\0',100);
        //sprintf(log,"[SendSingleByteToSerialPort_C()] write() return code =%d .",rtcode);
        //writeFileLog(log);

	if(  -1 == rtcode   )
        {
               writeFileLog("[SendSingleByteToSerialPort_C() write failed] Com port write failed.");
               return(CCTALK_FAIL); //return Error 
        }
        else
        {
               return(CCTALK_SUCCESS);

        }


}


/*
Function name:		ReceiveSingleByteFromSerialPort
Functionality:		Receives a byte from serial port
Input parameters:	Serial port handle, Space for received byte, Total byte received
Return value:		Returns SUCCESS in case of successful single byte or no byte read from port, otherwise returns FAIL
*/
int ReceiveSingleByteFromSerialPort_C(const int h_commport, unsigned char* const rbyte,int* const totalByteIn)
{

	int              nbytes;                    // Number of bytes read
	unsigned char    rxByte = '\0';
        //unsigned char log[100];
        //memset(log,'\0',100);

	if(h_commport < 1 || NULL==rbyte || NULL==totalByteIn)
        {
              return CCTALK_FAIL; //Error Code
        }

       *rbyte      = rxByte;
       *totalByteIn = 0;                           // Initially , 0 byte received from Port.

       *totalByteIn = read(h_commport,&rxByte,1);

        //memset(log,'\0',100);
        //sprintf(log,"[SendSingleByteToSerialPort_C()] read() return code =%d .",*totalByteIn);
        //writeFileLog(log);

	if( 1 == *totalByteIn )
	{
		  
                 *rbyte =  rxByte;                   // This data byte received from port.
		  return(CCTALK_SUCCESS);            // Successfully Received data byte from port.
	}
        else if ( 1 != *totalByteIn )
        {
                 *totalByteIn = 0;
                  //writeFileLog("[ReceiveSingleByteFromSerialPort_C()] Com port read failed.");
                  return(CCTALK_FAIL); //return Error 

        }
	

}


//Clear Transmit Buffer of Pc Serail Port
int  FlushSerialPortOutBuff_C(const int SerialPortHwnd)
{
    if(SerialPortHwnd < 1 )
    return CCTALK_FAIL; //Error Code

    if(0==tcflush(SerialPortHwnd,TCOFLUSH)) //Clears the output buffer 
	                                     //(if the device driver has one)
	    return CCTALK_SUCCESS;
    else
	    return CCTALK_FAIL;

}


//Clear Receive Buffer of Pc Serail Port
int FlushSerialPortInBuff_C(const int SerialPortHwnd)
{
        if(SerialPortHwnd < 1 )
        return CCTALK_FAIL; //Error Code

	if(0==tcflush(SerialPortHwnd,TCIFLUSH)) //Clears the input buffer 
		                                 //(if the device driver has one).
            return CCTALK_SUCCESS;
      else
	    
           return CCTALK_FAIL;

}


///////////////////////////////////////////End:Serial.c////////////////////////////////////////////////////////////
















