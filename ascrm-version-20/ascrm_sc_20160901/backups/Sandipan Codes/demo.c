#include "delaytime.h"
#include "serial.h"

// HANDLE ComHandle,BYTE TxAddr,BYTE TxCmCode,BYTE TxPmCode,int TxDataLen,BYTE TxData[],BYTE *RxReplyType,BYTE *RxStCode2,BYTE *RxStCode1,BYTE *RxStCode0,int *RxDataLen,BYTE RxData[]
/*
static int statusRead(int pcomhandler,unsigned char* const rcvPkt,unsigned int* const rcvPktLen,int totalByteToRecv)
{ 

             //////////////////////////////////////////////////////////////////////////////////////////////////////////////

	     unsigned char rxByte=0x00;

	     unsigned int  byteRcvdCounter=0;

	     unsigned char log[100];

             memset(log,'\0',100);

             int rtcode =0,totalByteIn=0;

            
             struct timespec begints={0}, endts={0},diffts={0};

             //Start Time
             clock_gettime(CLOCK_MONOTONIC, &begints);

             for(;;)
	     {        


                         rxByte=0x00;

                         totalByteIn=0;

                         //Read serial port received register 
		         //rcvStat = ReceiveSingleByteFromSerialPort_C(g_cctalkhandler,&rxByte,&totalByteIn); 
                         totalByteIn = read(pcomhandler,&rxByte,1);

                   
                         //////////////////////////////////////////////////////////////////////////////////////////////////////

                         //if any byte received store it and increment byte counter
			 if(1 == totalByteIn) 
			 {


				    rcvPkt[byteRcvdCounter] = rxByte; 

				    byteRcvdCounter +=1;
               
				    *rcvPktLen      = byteRcvdCounter;
 

		         }

                         //Expected No. of Bytes received.
		         if(byteRcvdCounter == totalByteToRecv)
                         { 
                               printf("\n[statusRead()] Expected Byte received in %d seconds.\n",diffts.tv_sec);

		               rtcode=1;   
                               
                               break;

                         }

                         //////////////////////////////////////////////////////////////////////////////////////////////////////


                         //Check Current timer status
                         clock_gettime(CLOCK_MONOTONIC, &endts);

			 diffts.tv_sec = endts.tv_sec - begints.tv_sec;

				
                         //Time out and No data received.      
			 if(  (byteRcvdCounter == 0)  &&  (diffts.tv_sec >= 2)  )
			 {      
                                 rtcode=0; 
  
                                 break;
                         }      
 
      	        }//End for(;;)


                return rtcode;

 
}//statusRead() end
*/

 
void main()
{
	int h_commport,i,send_rvalue = 0;
	int comPortNumber = 0;
	int openPortFlag = -1;

        unsigned char CmData[1024];
        memset(CmData,0x00,sizeof(CmData));
        /*

        STX (F2H)  = Representing the start of text in a command or a response
        ADDR = Representing the address of MTK-571
        LENH(1 byte) = Length of high byte of text
        LENL(1 byte) = Length of low byte of text 
        CMT = Command head („C‟ , 43H )
        CM = Specify as command
        PM = Command parameter
        DATA = Transmission data ( N byte, N=0~512)
        ETX (03H) = End of text
        BCC = XOR

         AND: 0.x = 0   1.x=x
         OR : 1+x = 1   0+x=x
 
         AND: 
         xxxx xxxx
         1111 1111 
        -----------------
         xxxx xxxx

         AND:
         xxxx xxxx
         0000 1111 
        -----------------
         0000 xxxx
        
        */
        unsigned char g_resetData[9]={0xF2,0x00,0x00,0x03,0x43,0x30,0x33,0x03,0x00}; // Reset command 
        //unsigned char g_resetData[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00}; // Reset command 
        unsigned char g_statusData[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x31,0x03,0x00}; // Reset command 
        char bcc=0x00;
        int counter=0;
        unsigned char rbyte=0x00;
        int totalByteIn=0x00;
        unsigned char rcvPkt[100];
        memset(rcvPkt,'\0',100);
        unsigned int rcvPktLen =0x00;
        int totalByteToRecv =0x00;
        /*
        for(counter=0;counter< 9 ;counter++)
        {  
               bcc= g_resetData[counter] ^ bcc;
               
        }
        */
        
        //printf("\n[main()] bcc: 0x%x",bcc);
        g_resetData[8]=getBcc(9,g_resetData);//bcc;
        /*
        for(counter = 0; counter< 9; counter++)
        {
              printf("\n[main()] g_resetData[%d]: 0x%x",counter,g_resetData[counter]);

        }
        */
             //unsigned char g_resetData[9]={0xF2,0x00,0x00,0x03,0x43,0x30,0x33,0x03,0x00}; // Reset command 

        printf("\n[main()] Going to Open Port");
	openPortFlag = OpenPort(comPortNumber,&h_commport);
        
        if(1 == openPortFlag)
        {
		        printf("\n[main()] Port open Successful, Status = %d",openPortFlag);
                        printf("\n[main()] Now going to Issue Initialize Command\n");
                        
                        //Step 1: send command with clear all serial buffer
                        ClearReceiveBuffer (h_commport);
                        ClearTransmitBuffer(h_commport);
                        for(i=0;i<9;i++)
			{
				printf("\n[main()] Initialize Command[%d] = %x",i,g_resetData[i]);
				send_rvalue=SendSingleByteToSerialPort(h_commport,g_resetData[i]);
			}
                        
                        /*
                        int j = ClearReceiveBuffer (h_commport);
                        while(i<26)
                        {
                               rbyte=0x00;
                               totalByteIn=0;
                               ReceiveSingleByteFromSerialPort(h_commport,&rbyte,&totalByteIn);
                               if( totalByteIn >= 1 )
                               {
                                  printf("[main()] rbyte[%d] = %x\n",i,rbyte);
                                  i++;
                               }
                               
                               
                        }
                         
	                */
                      /////////////////////////////////////////////////////////////////////////////////

                      //Step 2: after send command now time to read ack byte
                      memset(rcvPkt,'\0',100);
                      totalByteToRecv = 1;
                      int rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\nReturn Code of status read = %d",rtcode);
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[main()] Ack against Initialize Command [%d] = 0x%xh.\n",i,rcvPkt[i]);
				
	              }
                      
                      //////////////////////////////////////////////////////////////////////////////////
                      //Step 3: if we receieved ack byte then we go for read reply byte
                      printf("\n\n\n");
                       
                      memset(rcvPkt,'\0',100);
                      totalByteToRecv = 25;
                      rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\nReturn Code of status read = %d",rtcode);
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[main()] Initialize Command Reply Data[%d] = 0x%xh.\n",i,rcvPkt[i]);
				
	              }
                      /////////////////////////////////////////////////////////////////////////////////////
                      //Step 4: if we receieved reply byte properly then we go for reply with ack byte
                      SendSingleByteToSerialPort(h_commport,0x06);
                      
                      /////////////////////////////////////////////////////////////////////////////////////
                      //Step 5: Now going to issue second command for status
                      
                      ClearReceiveBuffer (h_commport);
                      ClearTransmitBuffer(h_commport);
                      bcc=0x00;
                      for(counter=0;counter< 9 ;counter++)
		      {  
		       bcc= g_statusData[counter] ^ bcc;
		       
		      }
                      g_statusData[8]=bcc;
                      for(i=0;i<9;i++)
		      {
				printf("\n[main()] Send Data to get Status[%d] = %x\n",i,g_statusData[i]);
				send_rvalue=SendSingleByteToSerialPort(h_commport,g_statusData[i]);
		      }
                      //Step 6: after send command now time to read ack byte
                      memset(rcvPkt,'\0',100);
                      totalByteToRecv = 1;
                      statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[main()] Reply ACK Data[%d] = 0x%xh.\n",i,rcvPkt[i]);
				
	              }
                      //Step 7: after send command now time to read total data
                      
                      printf("\n\n\n");
                       
                      memset(rcvPkt,'\0',100);
                      //totalByteToRecv = 24;
                      totalByteToRecv = 24;
                      statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("[main()] Reply Data[%d] = 0x%xh.\n",i,rcvPkt[i]);
				
	              }
                     
                      
                      ClosePort(h_commport);
                        
                         
        }
        else
        {
                printf("\n[main()] openPortFlag = %d",openPortFlag);
        }

}

