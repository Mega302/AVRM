
#include "DisConnectDevice.h"

/*
* Function Name : DisConnectDevice_c 
* Return Type   : int 
                  0 = DisConnected Successfully
                  1 = DisConnected Successfully but a Card in the Channel
                  2 = Communication Failure
                  3 = Operation timeout occurred
                  4 = Other Error 

* Parameters    : Name                           Type          Description 

                            
                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

*/

/*
* To get Current Report status of st0, st1, st2
*/

static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};

int DisConnectDevice_c(int Timeout)
{
        char bcc=0x00;
        unsigned char rcvPkt[25];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,oneCradInChannel = 0,closePortStatus = 0;
        unsigned int rcvPktLen =0x00; 

        /* If there was any card block or not */

        printf("\n[DisConnectDevice_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = bcc;
        printf("\n[DisConnectDevice_c()] bcc value is 0x%xh",bcc);

        /* Before Send Command clear all serial buffer */
        ClearReceiveBuffer (h_commport);
        ClearTransmitBuffer(h_commport);

        for(i=0;i<9;i++)
	{
		 printf("\n[DisConnectDevice_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[DisConnectDevice_c()] Failed to Send Report status Command ");
                    /* 2 = Communication Failure */
                    return 2;
                 }
	}

        printf("\n[DisConnectDevice_c()] Status Command send Successfully");
        printf("\n[DisConnectDevice_c()] Now Going to read Acknowledgement");

        /* Now going to Check Acknowledgement */  

        totalByteToRecv = 1;
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

        printf("\n[DisConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

        if(0 == rtcode)
        {
                 /* 2 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 2;
        }
        else
        {
                 printf("\n[DisConnectDevice_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        }

        /* If Return Data is 06h then Going to Read 25byte Data */

        if(0x06 == rcvPkt[0])
        {
                      printf("\n[DisConnectDevice_c()] Acknowledgement Received");
                      totalByteToRecv = 12;
                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\n[DisConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen); 
                      if(0 == rtcode)
		      {
				 /* 2 = Communication Failure */
				 ClearReceiveBuffer (h_commport);
				 ClearTransmitBuffer(h_commport);
				 return 2;
		      }
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[DisConnectDevice_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                                /* Card Status Code st0 */
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] No Card in MTK-571");
                                             
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] One Card in Gate");
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] One Card on RF/IC Card Position");
                                           /* 1 = Card found in the Channel */
                                           oneCradInChannel = 1;  
                                  
		                        } 
                                }
                                /* Card Status Code st1 */
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] Enough Card in the Box");
		                        }   
                                } 
                                /* Card Status Code st2 */
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[DisConnectDevice_c()] Error Card Bin Full");
                                           
		                        }
                                }
                                
				
	              }
                      /* Now time to send Acknowledgement */
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(h_commport,0x06);
                      ClearReceiveBuffer (h_commport);
                      ClearTransmitBuffer(h_commport);
                      if(0 == send_rvalue) 
		      {
		            printf("\n[DisConnectDevice_c()] Failed to send total Acknowledgement Command ");
                            /* 2 = Communication Failure */
                            return 2;
		      }
                       
       }
       /* If Return Data is 15h then No need to read Data */
       else if (0x15 == rcvPkt[0])
       {
             printf("\n[DisConnectDevice_c()] Nak Reply Received");
             /* 4 = Other Error */
             return 4;  
       }

       /* Status Read Successfull Now Going to Close the Serial Port */
       
       closePortStatus = ClosePort(h_commport);
       
       /* Port close and going to return */
       if(1 == closePortStatus)    
       {
               /* Port Close Successfull */ 
               if(1 == oneCradInChannel)
               {
                      /* One Card in Channel */
                      /* 1 = DisConnected Successfully but a Card in the Channel */
                      return 1;
               }
               else if(0 == oneCradInChannel)
               {
                      /* No Card in Channel */
                      /* 0 = DisConnected Successfully */
                      return 0;
               }
       }
       else if(0 == closePortStatus)
       {
           /* Port Close UnSuccessfull */
           /* 2 = Communication Failure */
           return 2;

       }
       else 
       {
           /* 4 = Other Error */
           return 4;
       }
 

}
