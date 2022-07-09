
#include "RejectCard.h"


/*
* Function Name : RejectCard_c 
* Return Type   : int 
                  0 = Operation Successful
                  1 = Communication Failure
                  2 = Rejection Bin Full
                  3 = No Card in the Channel
                  4 = Operation timeout Occurred 
                  5 = Other Error
                

* Parameters    : Name                           Type          Description 
                  
                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

*/

/*
* To get Current Report status of st0, st1, st2
*/

static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};

/*
* To Move to Error Bin
*/

static unsigned char g_moveToErrorBin[9]={0xF2,0x00,0x00,0x03,0x43,0x32,0x33,0x03,0x00};



int RejectCard_c(int Timeout)
{

        char bcc=0x00;
        unsigned char rcvPkt[25];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,noCardInRF_IC = 0,rejectionBinFull = 0;
        unsigned int rcvPktLen =0x00; 

        printf("\n[RejectCard_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = bcc;
        printf("\n[RejectCard_c()] bcc value is 0x%xh",bcc);

        /* Before Send Command clear all serial buffer */
        ClearReceiveBuffer (h_commport);
        ClearTransmitBuffer(h_commport);
       
        for(i=0;i<9;i++)
	{
		 printf("\n[RejectCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[RejectCard_c()] Failed to Send Report status Command ");
                    /* 1 = Communication Failure */
                    return 1;
                 }
	}

        printf("\n[RejectCard_c()] Status Command send Successfully");
        printf("\n[RejectCard_c()] Now Going to read Acknowledgement");

        /* Now going to Check Acknowledgement */  

        totalByteToRecv = 1;
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

        printf("\n[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        if(0 == rtcode)
        {
                 /* 1 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 1;
        }
        else
	{
		 printf("\n[RejectCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
				
	}

        /* If Return Data is 06h then Going to Read 25byte Data */

        if(0x06 == rcvPkt[0])
        {
                      printf("\n[RejectCard_c()] Acknowledgement Received");
                      totalByteToRecv = 12;
                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\n[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen); 
                      if(0 == rtcode)
		      {
				 /* 1 = Communication Failure */
				 ClearReceiveBuffer (h_commport);
				 ClearTransmitBuffer(h_commport);
				 return 1;
		      }             
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[RejectCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                                /* Card Status Code st0 */
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[RejectCard_c()] No Card in MTK-571");
                                           /* 3 = No Card in the Channel */ 
                                           noCardInRF_IC = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                        
                                           printf("\n[RejectCard_c()] One Card in Gate");
                                                                              
                  			 
                                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[RejectCard_c()] One Card on RF/IC Card Position");
                                            
                                           
		                        } 
                                }
                                /* Card Status Code st1 */
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[RejectCard_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[RejectCard_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[RejectCard_c()] Enough Card in the Box");
		                        }   
                                } 
                                /* Card Status Code st2 */
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[RejectCard_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[RejectCard_c()] Error Card Bin Full");
                                           /* 2 = Rejection Bin Full */ 
                                           rejectionBinFull = 1;
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
		            printf("\n[RejectCard_c()] Failed to send total Acknowledgement Command ");
                            /* 1 = Communication Failure */ 
                            return 1;
		      }
                      if(1 == noCardInRF_IC)
                      {
                           /* 3 = No Card in the Channel */ 
                           return 3;
                      }
                      if(1 == rejectionBinFull)
                      {
                           /* 2 = Rejection Bin Full */ 
                           return 2;

                      }
                      
                      
                       
         }
         /* If Return Data is 15h then No need to read Data */
         else if (0x15 == rcvPkt[0])
         {
             printf("\n[RejectCard_c()] Nak Reply Received");
             /* 5 = Other Error */
             return 5;  
         }

         /* All status ok and Going to issue Reject Card */

        printf("\n[RejectCard_c()] Going to send Reject Card Comamnd");
        bcc = getBcc(9,g_moveToErrorBin);
        g_moveToErrorBin[8] = bcc;
        printf("\n[RejectCard_c()] bcc value is 0x%xh",bcc);
        
        /* Before Send Command clear all serial buffer */

        ClearReceiveBuffer (h_commport);
        ClearTransmitBuffer(h_commport);
       
        for(i=0;i<9;i++)
	{
		 printf("\n[RejectCard_c()] Reject Card Command[%d] = 0x%xh",i,g_moveToErrorBin[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,g_moveToErrorBin[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[RejectCard_c()] Failed to Send Reject Card Command ");
                    /* 1 = Communication Failure */
                    return 1;
                 }
	}

        printf("\n[RejectCard_c()] Reject Card Command send Successfully");
        printf("\n[RejectCard_c()] Now Going to read Acknowledgement");

        /* Now going to Check Acknowledgement */  

        totalByteToRecv = 1;
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

        printf("\n[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        if(0 == rtcode)
        {
                 /* 1 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 1;
        }
        else
	{
		 printf("\n[RejectCard_c()] Acknowledgement against Reject Card Command [0] = 0x%xh.\n",rcvPkt[0]);
                  /* Acknowledgement received and going to return 0 and have to check return data and have to send ack */
                 return 0;
                
				
	}
        /* This function does not end here because Rejection and collection bin are not same */
       
      


}
