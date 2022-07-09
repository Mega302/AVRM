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

///////////////////////////////////////////Start:Serial.c///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "serial.h"
#include <time.h>

//int g_LogLevel;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
Function name:		OpenPort
Functionality:		Opens the serial port
Input parameters:	port number, get the port handler, which will be used for read,write function.
Return value: 		success or fail, 
*/
int  OpenPort(const int port_nmbr, int baudrate,int* const h_commport){

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

	int rtcode=1;
        if(port_nmbr <0 )
        return(FAIL); //Error Code
 
        struct termios portParams;
	char com_port[30];
        memset(com_port,'\0',30);
        rtcode=sprintf(com_port,"/dev/ttyS%d",port_nmbr);
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[OpenPort()] Com Port Id: %s",com_port);
        AVRM_SCRD_writeFileLog(log,"DEBUG");
        if(rtcode <0 ) //sprintf return 0 on failure ,On success, the total number of characters written is returned
        {
             return(FAIL); //Error Code
        }
        
        *h_commport = open(com_port,O_RDWR | O_NOCTTY | O_NDELAY);
  	if(*h_commport < 0)
	{
         return(FAIL); //Error Code
	}
      /*if there is no char in buff then read() function will not block(wait) the program,    	               
       rather when it gets char in buff then it will autometically read by read() function, 
       like interrupt or event. Upon successful completion for F_SETFL return Value other than -1,
       Otherwise, -1 shall be returned and errno set to indicate the error.
      */       
                
        rtcode=fcntl(*h_commport,F_SETFL,FNDELAY);   
       /*
	Set the file status flags to the value specified by arg. File access mode (O_RDONLY, O_WRONLY, O_RDWR) and file creation 	flags (i.e., O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC) in arg are ignored. On Linux this command can change only the O_APPEND, 	O_ASYNC, O_DIRECT, O_NOATIME, and O_NONBLOCK flags. 
       */
        if(-1==rtcode)
        return(FAIL);
        /*tcgetattr RETURN VALUE:Upon successful completion, 0 shall be returned. Otherwise, 
         -1 shall be returned and errno set to indicate the error.
         */
        rtcode=tcgetattr(*h_commport,&portParams);          // read serial port parameters
        
        if(-1==rtcode)
        return(FAIL);
        
        /*
           Set the baud rates to 9600bps
           cfsetispeed && cfsetospeed RETURN VALUE:Upon successful completion, 0 shall be returned. Otherwise, 
         -1 shall be returned and errno set to indicate the error.
         
        */
        
        //////////////////////////////////////////////////////////////////////////////
        /*
        
        rtcode=cfsetispeed(&portParams, B9600);
        if(-1==rtcode)
          return(FAIL);
        rtcode=cfsetospeed(&portParams, B9600);
        if(-1==rtcode)
          return(FAIL);
        
        */
        //////////////////////////////////////////////////////////////////////////////
        
        speed_t comportbaudrate=B9600;

        switch(baudrate)
        {
                 case 0: comportbaudrate=B0;
                       break;

                 case 50: comportbaudrate=B50;
                       break;

                 case 75: comportbaudrate=B75;
                       break;

                 case 110: comportbaudrate=B110;
                       break;
                 case 134: comportbaudrate=B134;
                       break;
                 case 150: comportbaudrate=B150;
                       break;
                 case 200: comportbaudrate=B200;
                       break;
                 case 300: comportbaudrate=B300;
                       break;
                 case 600: comportbaudrate=B600;
                       break;
                 case 1200: comportbaudrate=B1200;
                       break;
                 case 1800: comportbaudrate=B1800;
                       break;
                 case 2400: comportbaudrate=B2400;
                       break;
                 case 4800: comportbaudrate=B4800;
                       break;
                 case 9600: comportbaudrate=B9600;
                       break;
                 case 19200: comportbaudrate=B19200;
                       break;
                 case 38400: comportbaudrate=B38400;
                       break;
                       
                 case 57600: comportbaudrate=B57600;
                       break;      
                       
                 default : comportbaudrate=B9600;
                           break;
        };
        rtcode=cfsetispeed(&portParams, comportbaudrate);
        if(-1==rtcode)
        return(FAIL);
        rtcode=cfsetospeed(&portParams, comportbaudrate);
        if(-1==rtcode)
        return(FAIL);
        
        //////////////////////////////////////////////////////////////////////////////



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
        portParams.c_lflag &= ~(ICANON | ECHO | ISIG);  //Disable raw data input 
        portParams.c_oflag &= ~OPOST;                   //Disable raw output
        portParams.c_cflag &= ~CRTSCTS;                 //Disable hardware flow control  
        portParams.c_iflag &= ~(IXON | IXOFF | IXANY);  //Disable software flow control   
    
 
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
        portParams.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

	//
	// No line processing:
	// echo off, echo newline off, canonical mode off, 
	// extended input processing off, signal chars off
	//
        portParams.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
        

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /*
                VMIN and VTIME defined

		VMIN is a character count ranging from 0 to 255 characters, and VTIME is time measured in 0.1 second intervals, (0 to 25.5 seconds). The value of "zero" is special to both of these parameters, and this suggests four combinations that we'll discuss below. In every case, the question is when a read() system call is satisfied, and this is our prototype call:

		int n = read(fd, buffer, nbytes);

		Keep in mind that the tty driver maintains an input queue of bytes already read from the serial line and not passed to the user, so not every read() call waits for actual I/O - the read may very well be satisfied directly from the input queue.

		VMIN = 0 and VTIME = 0
		    This is a completely non-blocking read - the call is satisfied immediately directly from the driver's input queue. If data are available, it's transferred to the caller's buffer up to nbytes and returned. Otherwise zero is immediately returned to indicate "no data". We'll note that this is "polling" of the serial port, and it's almost always a bad idea. If done repeatedly, it can consume enormous amounts of processor time and is highly inefficient. Don't use this mode unless you really, really know what you're doing. 

		VMIN = 0 and VTIME > 0
		    This is a pure timed read. If data are available in the input queue, it's transferred to the caller's buffer up to a maximum of nbytes, and returned immediately to the caller. Otherwise the driver blocks until data arrives, or when VTIME tenths expire from the start of the call. If the timer expires without data, zero is returned. A single byte is sufficient to satisfy this read call, but if more is available in the input queue, it's returned to the caller. Note that this is an overall timer, not an intercharacter one. 

		VMIN > 0 and VTIME > 0
		    A read() is satisfied when either VMIN characters have been transferred to the caller's buffer, or when VTIME tenths expire between characters. Since this timer is not started until the first character arrives, this call can block indefinitely if the serial line is idle. This is the most common mode of operation, and we consider VTIME to be an intercharacter timeout, not an overall one. This call should never return zero bytes read. 

		VMIN > 0 and VTIME = 0
		    This is a counted read that is satisfied only when at least VMIN characters have been transferred to the caller's buffer - there is no timing component involved. This read can be satisfied from the driver's input queue (where the call could return immediately), or by waiting for new data to arrive: in this respect the call could block indefinitely. We believe that it's undefined behavior if nbytes is less then VMIN. 

        */

         
	 // One input byte is enough to return from read()
	 // Inter-character timer off
	 //https://ez.analog.com/dsp/software-and-development-tools/linux-blackfin/f/q-a/83410/read-termios-vmin-vtime-no-effect
	 portParams.c_cc[VMIN]  = 0; // read doesn't block (blocking read until 1 chars received)
	 portParams.c_cc[VTIME] = 0; //Wait for timeout= (value*1/10) Second 

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /*
         tcsetattr: Return Value
         Upon successful completion, 0 is returned. Otherwise, -1 is returned and errno is set to indicate the error.
       */
        rtcode=tcsetattr(*h_commport,TCSANOW,&portParams);  // write serial port parameters. TCSANOW will make data instantly available when sending & receiving.
        if(-1==rtcode)
        return(FAIL);

        //If all settings done return SUCCESS
        return(SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int  ClosePort(const int h_commport){
        /*close() returns zero on success. On error,
          -1 is returned, and errno is set appropriately.
        */
        int rtcode=1;
        rtcode=close(h_commport);
        if(0==rtcode)
	return (SUCCESS);
        else
        return FAIL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Function name:		SendSingleByteToSerialPort
Functionality:		Sends a byte to serial port
Input parameters:	Serial port handle, Byte to send
Return value:		Returns SUCCESS in case of successful write to port, otherwise returns FAIL
*/
int SendSingleByteToSerialPort(const int h_commport, const unsigned char bdata){
	if(h_commport < 1)
        return FAIL; //return Error

	if(write(h_commport,&bdata,1) < 0)
        return(FAIL); //return Error 

        return(SUCCESS);

}//int SendSingleByteToSerialPort(const int h_commport, const unsigned char bdata)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Function name:		ReceiveSingleByteFromSerialPort
Functionality:		Receives a byte from serial port
Input parameters:	Serial port handle, Space for received byte, Total byte received
Return value:		Returns SUCCESS in case of successful single byte or no byte read from port, otherwise returns FAIL
*/
int ReceiveSingleByteFromSerialPort(  const int h_commport, unsigned char* const rbyte,int* const totalByteIn)
{
	int              nbytes;                    // Number of bytes read
	unsigned char    rxByte = '\0';

	if(h_commport < 1 || NULL==rbyte || NULL==totalByteIn)
        return FAIL; //Error Code

	*rbyte      = rxByte;
	*totalByteIn = 0;                           // Initially , 0 byte received from Port.

	*totalByteIn = read(h_commport,&rxByte,1);
        
	if(*totalByteIn > 0)
	{
		  
                 *rbyte =  rxByte;                   // This data byte received from port.
		  return(SUCCESS);               // Successfully Received data byte from port.
	}

	*totalByteIn = 0;
	return(SUCCESS);                            // atleast port had not any error.
}//int ReceiveSingleByteFromSerialPort(  const int h_commport, 
//                                      unsigned char* const rbyte,
//                                      int* const totalByteIn)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Clear Transmit Buffer of Pc Serail Port
int  ClearTransmitBuffer(const int SerialPortHwnd)
{
    if(SerialPortHwnd < 1 )
    {
      return FAIL; //Error Code
    }

    if(0==tcflush(SerialPortHwnd,TCOFLUSH)) //Clears the output buffer 
    {	                                     //(if the device driver has one)
	    return SUCCESS;
    }
    else
    {
	    return FAIL;
    }

}//int  ClearTransmitBuffer(const int SerialPortHwnd)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Clear Receive Buffer of Pc Serail Port
int ClearReceiveBuffer (const int SerialPortHwnd)
{
        if(SerialPortHwnd < 1 )
        {
              return FAIL; //Error Code
        }

	if(0==tcflush(SerialPortHwnd,TCIFLUSH)) //Clears the input buffer 
	{	                                 //(if the device driver has one).
            return SUCCESS;
        }
        else
	{    
           return FAIL;
        }

}//int ClearReceiveBuffer (const int SerialPortHwnd)

unsigned int GetCRC16(unsigned char* bufData, unsigned int sizeData)
{
		unsigned int CRC=0x00, i=0;
		unsigned char j=0x00;
		CRC = 0;
		for(i=0; i < sizeData; i++)
		{
		        CRC ^= bufData[i];
			for(j=0; j < 8; j++)
			{
			       if(CRC & 0x0001) 
		               {
		                   CRC >>= 1; 
		                   CRC ^= POLYNOMIAL;
		               }
			       else CRC >>= 1;
			}

		}

		return CRC;


}//GetCRC16() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static pthread_mutex_t StatusReadProcMutex = PTHREAD_MUTEX_INITIALIZER;
int statusRead( int pcomhandler, unsigned char* const rcvPkt, unsigned int* const  rcvPktLen,int totalByteToRecv )
{
		pthread_mutex_lock(&StatusReadProcMutex);	
		int rtcode = statusReadV0(  pcomhandler, rcvPkt, rcvPktLen, totalByteToRecv );
		pthread_mutex_unlock(&StatusReadProcMutex);
		return rtcode;
}//++int statusRead( int pcomhandler, unsigned char* const rcvPkt, unsigned int* const  rcvPktLen,int totalByteToRecv ) end

static pthread_mutex_t StatusReadV0ProcMutex = PTHREAD_MUTEX_INITIALIZER;
int statusReadV0( int pcomhandler, unsigned char* const rcvPkt, unsigned int* const  rcvPktLen,int totalByteToRecv )
{
             //printf("\n[statusReadV0()] Entry .");
             char log[LOG_ARRAY_SIZE];
             unsigned char MessageLog[LOG_ARRAY_SIZE];
             unsigned char CommandHexLog[LOG_ARRAY_SIZE];
             unsigned char rxByte=0x00;
	         unsigned int  byteRcvdCounter=0;
	         int rtcode =0,totalByteIn=0,byteslength=0;            
             int RceieveTextLength =0x00;
             struct timespec begints={0}, endts={0},diffts={0};
             memset(log,'\0',LOG_ARRAY_SIZE);  
             memset(MessageLog,'\0',LOG_ARRAY_SIZE);
             memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);           
             unsigned char deviceID[MAX_STR_SIZE_OF_LOG];
             memset(deviceID,'\0',MAX_STR_SIZE_OF_LOG);
             GetSCRDDeviceID(deviceID);
             //Start Time
             clock_gettime(CLOCK_MONOTONIC, &begints);             
             for(;;)
	         {        
                         rxByte=0x00;
                         totalByteIn=0;
                         //Read serial port received register 
						 //rcvStat = ReceiveSingleByteFromSerialPort_C(
                         //          g_cctalkhandler,
                         //          &rxByte,
                         //          &totalByteIn); 
                         totalByteIn = read(pcomhandler,&rxByte,1);
						 /*
                         //if any byte received store it and increment byte counter
						 //if(1 == totalByteIn) 
						 //{
												//rcvPkt[byteRcvdCounter] = rxByte; 
												//byteRcvdCounter +=1;
												//*rcvPktLen      = byteRcvdCounter;


									 //}//if(1 == totalByteIn)

									 //Expected No. of Bytes received.
							 //if(byteRcvdCounter == totalByteToRecv)
									 //{ 

										  // clock_gettime(CLOCK_MONOTONIC, &endts);

										   //diffts.tv_sec = endts.tv_sec - begints.tv_sec;

										   //memset(log,'\0',LOG_ARRAY_SIZE);

										   //sprintf(log,"[statusRead()] Expected Byte received in %d seconds .",diffts.tv_sec);
			 
										  // writeFileLog(log);

										   //printf("\n[statusRead()] Expected Byte received in %d seconds .",diffts.tv_sec);

										   ///////////////////////////////////////////////////////////////////////////////////////////////////

										  // byteslength = *rcvPktLen;

										  // SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
														   // g_Deviceid,
														   // "Rx",
														   // rcvPkt,
														   // byteslength
														 // );

										   ////////////////////////////////////////////////////////////////////////////////////////////////////

								  // rtcode=1;  
			 
										   //break;


									// }//if(byteRcvdCounter == totalByteToRecv) end
                          */						
                         /////////////////////////////////////////////////////////////////////////////////////////////////////////
                      
                         //if any byte received store it and increment byte counter
						 if(1 == totalByteIn) 
						 {
                                    rcvPkt[byteRcvdCounter] = rxByte;
                                    byteRcvdCounter +=1;
                                    *rcvPktLen      = byteRcvdCounter;  
                                    //memset(log,'\0',LOG_ARRAY_SIZE);
                                    //sprintf(log,"[statusRead()] rcvPkt[%d]= 0x%xh.",byteRcvdCounter,rcvPkt[byteRcvdCounter]);
                                    //writeFileLog(log); 
                                    if(byteRcvdCounter == totalByteToRecv)
									{ 
											   clock_gettime(CLOCK_MONOTONIC, &endts);
                                               diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                                               memset(log,'\0',LOG_ARRAY_SIZE);
                                               sprintf(log,"[statusReadV0()] Expected Byte received in %d seconds .",diffts.tv_sec);
                                               AVRM_SCRD_writeFileLog(log,"DEBUG");
                                               //printf("\n[statusReadV0()] Expected Byte received in %d seconds .",diffts.tv_sec);
                                               byteslength = *rcvPktLen;
                                               SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,deviceID,"Rx",rcvPkt,byteslength);
                                               rtcode=1; 
                                               break;
								     }//if(byteRcvdCounter == totalByteToRecv) end
                                    //Get Receieve Text Length 
                                    else if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )
                                    {
                                              RceieveTextLength = (rcvPkt[2]<<8)+rcvPkt[3]+6;//6 Byte: STX+ADDR+LENH+LENL+ETX+BCC                                             
                                              if( byteRcvdCounter >= RceieveTextLength )
                                              {     
                                                      memset(log,'\0',LOG_ARRAY_SIZE);
                                                      sprintf(log,"[statusReadV0() Text length calculate block] RceieveTextLength=%d and byteRcvdCounte=%d.",RceieveTextLength,byteRcvdCounter);                                    
                                                      AVRM_SCRD_writeFileLog(log,"DEBUG");
                                                      clock_gettime(CLOCK_MONOTONIC, &endts);
                                                      diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                                                      memset(log,'\0',LOG_ARRAY_SIZE);
                                                      sprintf(log,"[statusReadV0() Text length calculate block ] Expected byte receieve in %d second.",diffts.tv_sec);
                                                      AVRM_SCRD_writeFileLog(log,"DEBUG");
                                                      byteslength = *rcvPktLen;
                                                      SCRD_LogDataExchnage(  DATA_EXCHANGE_ALL,deviceID,"Rx",rcvPkt, byteslength );
		                                              return 1;
											   }//if( byteRcvdCounter >= RceieveTextLength ) end                                        
                                      }//else if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )                                      
                         }//if(1 == totalByteIn) 

                         //Check Current timer status
                         clock_gettime(CLOCK_MONOTONIC, &endts);
                         diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                         //Time out and No data received. //Default : 3 Second
						 //if(  (byteRcvdCounter == 0)  &&  (diffts.tv_sec >= 1 )  )
                         if( diffts.tv_sec >= MAX_WAIT_TIME_COMPORT  )
                         {      
                                 //printf("\n[statusReadV0()] Expected Byte not received in %d seconds .",diffts.tv_sec);
                                 rtcode=0; 
                                 //if( (ALL_LOG == g_LogModeLevel)               ||
                                 //    (OPERATION_COMPLETE_LOG == g_LogModeLevel) 
                                 //) 
                                 //{
                                   memset(log,'\0',LOG_ARRAY_SIZE);
                                   sprintf(log,"[statusReadV0()] Time out in %d seconds",diffts.tv_sec);
                                   AVRM_SCRD_writeFileLog(log,"DEBUG");
                                 //}
                                 byteslength = *rcvPktLen;
                                 SCRD_LogDataExchnage( DATA_EXCHANGE_ERROR_ONLY, deviceID,"Rx",rcvPkt, byteslength );
                                 break;
                         } //if end
      	        }//End for(;;)
                return rtcode;
}//statusReadV0() end

static pthread_mutex_t StatusReadV1ProcMutex = PTHREAD_MUTEX_INITIALIZER;
int statusReadV1( int pcomhandler,  unsigned char* const rcvPkt, unsigned int* const  rcvPktLen,int totalByteToRecv )
{
			 pthread_mutex_lock(&StatusReadV1ProcMutex);	
	         char log[LOG_ARRAY_SIZE];
             unsigned char MessageLog[LOG_ARRAY_SIZE];
             unsigned char CommandHexLog[LOG_ARRAY_SIZE];
             unsigned char rxByte=0x00;
	         unsigned int  byteRcvdCounter=0;
	         int rtcode =0,totalByteIn=0,byteslength=0;            
             int RceieveTextLength =0x00;
             struct timespec begints={0}, endts={0},diffts={0};
             memset(log,'\0',LOG_ARRAY_SIZE);  
             memset(MessageLog,'\0',LOG_ARRAY_SIZE);
             memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);            
             unsigned char deviceID[MAX_STR_SIZE_OF_LOG];
             memset(deviceID,'\0',MAX_STR_SIZE_OF_LOG);
             GetSCRDDeviceID(deviceID);
             char read_buf [2048];
             memset(&read_buf, '\0', sizeof(read_buf));        
             int bytesAvailable =0;   
             int counter=0;  
             memset(log,'\0',LOG_ARRAY_SIZE);
             sprintf(log,"[statusReadV1()] Expected Byte Length  : %d  .",totalByteToRecv);
             AVRM_SCRD_writeFileLog(log,"DEBUG");
             //++Start Time
             clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begints);              
            for(;;)
	        { 
				         rxByte=0x00;
                         totalByteIn=0;
                         bytesAvailable= 0 ;
                         //++Call Device Driver
                         ioctl(pcomhandler, FIONREAD, &bytesAvailable);
                         //memset(log,'\0',LOG_ARRAY_SIZE);
						 //sprintf(log,"[statusReadV1()]  Byte Available : %d  .",bytesAvailable);
						 //AVRM_SCRD_writeFileLog(log,"DEBUG");
                         if( bytesAvailable == totalByteToRecv )
                         {
							     //++Read serial port received data 
								 totalByteIn = 0;
								 totalByteIn = read(pcomhandler,&read_buf,totalByteToRecv);
						 }//++if end
                         //++if any byte received store it 
						 if( totalByteIn > 0 ) 
						 {
                                    if( totalByteIn == totalByteToRecv )
									{ 
											  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endts);
                                              diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                                              memset(log,'\0',LOG_ARRAY_SIZE);
                                              sprintf(log,"[statusReadV1()] Expected Byte received in %d seconds .",diffts.tv_sec);
                                              AVRM_SCRD_writeFileLog(log,"DEBUG");
                                              for( counter=0;counter<totalByteIn;counter++)
											  {
													 //memset(log,'\0',100);
													 //sprintf(log,"[statusReadV1()]  read_buf[%d]= 0x%x.",counter, read_buf[counter] );
													 //AVRM_SCRD_writeFileLog(log,"DEBUG");	
													 rcvPkt[counter] = (unsigned char)read_buf[counter];
											  }//for end
											 *rcvPktLen  = totalByteIn;	 
                                              byteslength = *rcvPktLen;
                                              SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,deviceID,"Rx",rcvPkt,byteslength);
                                              rtcode=1; 
                                              break;
				                    }//if(byteRcvdCounter == totalByteToRecv) end
                                    //++Get Receieve Text Length 
                                    else if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )
                                    {
                                              RceieveTextLength = (rcvPkt[2]<<8)+rcvPkt[3]+6;//++6 Byte: STX+ADDR+LENH+LENL+ETX+BCC                                            
                                              if( totalByteIn >= RceieveTextLength )
                                              {     
                                                      memset(log,'\0',LOG_ARRAY_SIZE);
                                                      sprintf(log,"[statusReadV1() Text length calculate block] RceieveTextLength=%d and totalByteIn=%d.",RceieveTextLength,totalByteIn);                                   
                                                      AVRM_SCRD_writeFileLog(log,"DEBUG");
													  clock_gettime(CLOCK_MONOTONIC, &endts);
													  diffts.tv_sec = endts.tv_sec - begints.tv_sec;
													  memset(log,'\0',LOG_ARRAY_SIZE);
													  sprintf(log,"[statusReadV1() Text length calculate block ] Expected byte receieve in %d second.",diffts.tv_sec);
													  AVRM_SCRD_writeFileLog(log,"DEBUG");
													  *rcvPktLen = totalByteIn;
													  byteslength = *rcvPktLen;													  
													  SCRD_LogDataExchnage(  DATA_EXCHANGE_ALL, deviceID, "Rx", rcvPkt, byteslength );
		                                    		  return 1;
                                              }//++if( byteRcvdCounter >= RceieveTextLength ) end                                        
                                      }//++else if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )                                     
                         }//++if( totalByteIn > 0 ) 
                         
                         //++Check Current timer status
                         clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endts);
                         diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                         //++Time out and No data received. //Default : 3 Second
                         if( diffts.tv_sec >= MAX_WAIT_TIME_COMPORT  )
                         {
							        //++Read Serial Port Data if timeout 
								    bytesAvailable = 0;
								    ioctl(pcomhandler, FIONREAD, &bytesAvailable);
								    if( bytesAvailable > 0 )
								    {
										   //++Read serial port received data after timeout
										   totalByteIn = 0;
										   totalByteIn = read(pcomhandler,&read_buf,bytesAvailable);
								   }//++if end
								   for( counter=0;counter<totalByteIn;counter++)
								   {
									       //memset(log,'\0',100);
										   //sprintf(log,"[statusReadV1()]  read_buf[%d]= 0x%x.",counter, read_buf[counter] );
										   //AVRM_SCRD_writeFileLog(log,INFO,fnDeviceId);	
										   rcvPkt[counter] = (unsigned char)read_buf[counter];
									}//++for end
								   *rcvPktLen  = totalByteIn;	 
                                   byteslength = *rcvPktLen;
                                   if( totalByteIn == totalByteToRecv )
								   {
									    rtcode=1; 
									    memset(log,'\0',LOG_ARRAY_SIZE);
                                        sprintf(log,"[statusReadV1()] Expected Byte received in %d seconds .",diffts.tv_sec);
                                        AVRM_SCRD_writeFileLog(log,"DEBUG");
									    SCRD_LogDataExchnage(  DATA_EXCHANGE_ALL, deviceID, "Rx", rcvPkt, byteslength );
								   } else{
									   rtcode=0; 
									   memset(log,'\0',LOG_ARRAY_SIZE);
                                       sprintf(log,"[statusReadV1()] Time out in %d seconds",diffts.tv_sec);
                                       AVRM_SCRD_writeFileLog(log,"DEBUG");                                 
                                       SCRD_LogDataExchnage( DATA_EXCHANGE_ERROR_ONLY,deviceID,"Rx",rcvPkt,byteslength);
								   }//++else end
                                   break;
                         }//++if end
      	   }//++End for(;;)
      	   pthread_mutex_unlock(&StatusReadV1ProcMutex);	
           return rtcode;
}//++statusRead() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static pthread_mutex_t StatusReadV2ProcMutex = PTHREAD_MUTEX_INITIALIZER;
int statusRead_V2(   int  pcomhandler, unsigned char*  rcvPkt, unsigned int*   rcvPktLen)
{ 
             //printf("\n[statusRead_V2()] Entry .");
             pthread_mutex_lock(&StatusReadV2ProcMutex);            
             AVRM_SCRD_writeFileLog("[statusRead_V2()] Entry","INFO");             	
             char log[LOG_ARRAY_SIZE];
             unsigned char MessageLog[LOG_ARRAY_SIZE];
             unsigned char CommandHexLog[LOG_ARRAY_SIZE];
             char rxByte=0x00;
			 unsigned int  byteRcvdCounter=0;
			 int rtcode =0,totalByteIn=0,byteslength=0;
             char bcc=-1;
             int RceieveTextLength =0x00,EOTIndex=-1;
             struct timespec begints={0}, endts={0},diffts={0};
             memset(MessageLog,'\0',LOG_ARRAY_SIZE);
             memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);
             memset(log,'\0',LOG_ARRAY_SIZE);              
             unsigned char deviceID[MAX_STR_SIZE_OF_LOG];
             memset(deviceID,'\0',MAX_STR_SIZE_OF_LOG);
             GetSCRDDeviceID(deviceID);
             //++Start Time
             clock_gettime(CLOCK_MONOTONIC, &begints);
             for(;;)
	         {        
                         rxByte=0x00;
                         totalByteIn=0;
                         //Read serial port received register 
						 totalByteIn = read(pcomhandler,&rxByte,1);
                         //if any byte received store it and increment byte counter
						 if(1 == totalByteIn) 
						 {
                                    rcvPkt[byteRcvdCounter] = rxByte;                                    
                                    //memset(log,'\0',LOG_ARRAY_SIZE);
                                    //sprintf(log,"[statusRead_V2()] rcvPkt[%d]= 0x%xh.",byteRcvdCounter,rcvPkt[byteRcvdCounter]);                                  
                                    //AVRM_SCRD_writeFileLog(log,"DEBUG");
                                    byteRcvdCounter +=1;
                                    *rcvPktLen      = byteRcvdCounter;
                                    //Get Receieve Text Length 
                                    if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )
                                    {
                                         RceieveTextLength = 0;
                                         RceieveTextLength = (rcvPkt[2]<<8)+rcvPkt[3]+6;//6 Byte: STX+ADDR+LENH+LENL+ETX+BCC                                         
                                         if( byteRcvdCounter >= RceieveTextLength   )
                                         {                                              
                                              memset(log,'\0',LOG_ARRAY_SIZE);
                                              sprintf(log,"[statusRead_V2() Text length calculate block] RceieveTextLength=%d and byteRcvdCounte=%d.",RceieveTextLength,byteRcvdCounter);                                    
                                              AVRM_SCRD_writeFileLog(log,"DEBUG");                                              
                                              clock_gettime(CLOCK_MONOTONIC, &endts);
                                              diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                                              memset(log,'\0',LOG_ARRAY_SIZE);
                                              sprintf(log,"[statusRead_V2() Text length calculate block ] Expected byte receieve in %d second.",diffts.tv_sec);
                                              AVRM_SCRD_writeFileLog(log,"DEBUG");                                                                                           
                                              //++long long int delta_us = (endts.tv_sec - begints.tv_sec)*1000 + (endts.tv_nsec - begints.tv_nsec)/1000000;                                             
                                              //++memset(log,'\0',LOG_ARRAY_SIZE);
                                              //++sprintf(log,"[statusRead_V2() Text length calculate block ] Expected byte receieve in %lld milisecond",delta_us);
                                              //++AVRM_SCRD_writeFileLog(log,"DEBUG");
                                              byteslength = *rcvPktLen;											  			
                                              SCRD_LogDataExchnage(  DATA_EXCHANGE_ALL,deviceID,"Rx",rcvPkt,byteslength);											  
											  AVRM_SCRD_writeFileLog("[statusRead_V2() Success] Exit ","INFO");											  
											  pthread_mutex_unlock(&StatusReadV2ProcMutex);												  
                                              return 1;                                        
                                         }
                                    }//if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )                                  
                         }//if(1 == totalByteIn) 
                       
                         /*
                         //Now Check if it ETX byte
		                 if( 0x03 == rxByte )
                         { 

                               //Sure that we receive a eot byte and already we receieve 4 byte (STX+ADDR+LENH+LENL)
                               if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )
                               {

                                       EOTIndex = (rcvPkt[2]<<8)+rcvPkt[3]+5; //5 Byte: STX+ADDR+LENH+LENL+ETX

                                       memset(log,'\0',LOG_ARRAY_SIZE);

                                       sprintf(log,"[statusRead_V2() Eot Check Block] EOTIndex = %d .",EOTIndex);
                                       
                                       AVRM_SCRD_writeFileLog(log,"DEBUG");

                                       memset(log,'\0',LOG_ARRAY_SIZE);

                                       sprintf(log,"[statusRead_V2() Eot Check Block] EOTByte = %d .",rcvPkt[EOTIndex-1]);
                                       
                                       writeFileLog(log);

                                       if( 0x03 == rcvPkt[EOTIndex-1] )
                                       {

                                                       rxByte =-1;
													   read(pcomhandler,&rxByte,1);
													   if( rxByte > -1 )
													   {
															rcvPkt[byteRcvdCounter] = rxByte; 
															byteRcvdCounter +=1;
															*rcvPktLen      = byteRcvdCounter;
															//Now check bcc data 
															bcc = getBcc(*rcvPktLen-1 , rcvPkt );
															clock_gettime(CLOCK_MONOTONIC, &endts);
															diffts.tv_sec = endts.tv_sec - begints.tv_sec;
															if( bcc == rxByte )
															{
																
																memset(log,'\0',LOG_ARRAY_SIZE);

																sprintf(log,"[statusRead_V2() Eot Check Block] Expected byte receieve in %d second with checksum matched success.",diffts.tv_sec);
																writeFileLog(log);

																return 1;
															}
															else
															{

																					   memset(log,'\0',LOG_ARRAY_SIZE);

																					   sprintf(log,"[statusRead_V2() Eot Check Block] Read CheckSum Byte = 0x%xh and calculated checksum value=0x%xh.",rcvPkt[byteRcvdCounter-1],bcc);
															   
																					   writeFileLog(log);
															  
																memset(log,'\0',LOG_ARRAY_SIZE);

																sprintf(log,"[statusRead_V2() Eot Check Block] Expected byte receieve failed in %d second with checksum matched failed.",diffts.tv_sec);
																writeFileLog(log);

																return 0;
															}
                                                    
                                                    }//if( rxByte > -1 ) block

                                      }//if( 0x03 == rcvPkt[EOTIndex-1] ) block
                               }//if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) &&  block end
                               //   ('\0'!=rcvPkt[2])&& ('\0'!=rcvPkt[3]) )
                               //{      
                         }//if( 0x03 == rxByte ) block end          
                        */
                                                  
                         //++Check Current timer status
                         clock_gettime(CLOCK_MONOTONIC, &endts);
                         diffts.tv_sec = endts.tv_sec - begints.tv_sec;                        
                         //++Time out and No data received. //Default : 5 Second
						 if( diffts.tv_sec >= MAX_WAIT_TIME_COMPORT  )
                         {      
                                 rtcode=0;
                                 memset(log,'\0',LOG_ARRAY_SIZE);
                                 sprintf(log,"[statusRead_V2()] Time out in %d seconds",diffts.tv_sec);
                                 AVRM_SCRD_writeFileLog(log,"DEBUG");
                                 byteslength = *rcvPktLen;
                                 SCRD_LogDataExchnage( DATA_EXCHANGE_ERROR_ONLY,deviceID, "Rx", rcvPkt, byteslength );
                                 if( (0xF2 == rcvPkt[0]) && (MTK_DEVICE_ADDRESS == rcvPkt[1]) && (byteRcvdCounter>=4) )
                                 {                                        
                                         RceieveTextLength = (rcvPkt[2]<<8)+rcvPkt[3]+6;//6 Byte: STX+ADDR+LENH+LENL+ETX+BCC                                        
                                         if( byteRcvdCounter >= RceieveTextLength   )
                                         {
                                                   rtcode =1; //Success data
                                         }
                                         else if( byteRcvdCounter < RceieveTextLength   ){                                               
                                                   rtcode =0; //Less data
                                         }     
                                 }                               
                                 break;
                         }//if( diffts.tv_sec >= MAX_WAIT_TIME_COMPORT  ) end                          
      	      }//++End for(;;)
      	      
      	     AVRM_SCRD_writeFileLog("[statusRead_V2() Normal] Exit ","INFO");
      	     pthread_mutex_unlock(&StatusReadV2ProcMutex);	
             return rtcode;
}//++statusRead_V2() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned char getBcc(int length , unsigned char* const rcvPkt )
{
     unsigned char bcc=0x00;
     int counter=0;

     for(counter=0;counter< length ;counter++)
     {  
               bcc= rcvPkt[counter] ^ bcc;
               
     }
     
     return bcc;

}//getBcc() end

/////////////////////////////////////End:Serial.c//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////













