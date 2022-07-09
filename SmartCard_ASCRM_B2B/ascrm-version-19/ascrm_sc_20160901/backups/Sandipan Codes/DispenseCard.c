
#include "DispenseCard.h"

/*
* Function Name : DispenseCard_c 
* Return Type   : int 
                  0 = Operation Successful
                  1 = Communication Failure
                  2 = Channel blocked 
                  3 = Insert/return mouth block
                  4 = Stacker Empty
                  5 = Operational timeout occurred
                  6 = Other Error 

* Parameters    : Name                           Type          Description 

                            
                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

*/
 
/*
* To get Current Report status of st0, st1, st2
*/

static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};

/*
* To Dispense Card   
*/
static unsigned char g_dispenseCard[9]={0xF2,0x00,0x00,0x03,0x43,0x32,0x32,0x03,0x00};



int DispenseCard_c(int Timeout)
{

        char bcc=0x00;
        unsigned char rcvPkt[25];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,returnMouthFlag = 0,channelBlockFlag = 0,stackerEmptyFlag = 0;
        unsigned int rcvPktLen =0x00; 

        printf("\n[DispenseCard_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = bcc;
        printf("\n[DispenseCard_c()] bcc value is 0x%xh",bcc);

        /* Before Send Command clear all serial buffer */
        ClearReceiveBuffer (h_commport);
        ClearTransmitBuffer(h_commport);

        for(i=0;i<9;i++)
	{
		 printf("\n[DispenseCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[DispenseCard_c()] Failed to Send Report status Command ");
                    return 1;
                 }
	}

        printf("\n[DispenseCard_c()] Status Command send Successfully");
        printf("\n[DispenseCard_c()] Now Going to read Acknowledgement");


        /* Now going to Check Acknowledgement*/  
 
        totalByteToRecv = 1;
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

        printf("\n[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        if(0 == rtcode)
        {
                 /* 1 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 1;
        }
        else
        {
                 printf("\n[DispenseCard_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        }

        /* If Return Data is 06h then Going to Read 25byte Data */

        if(0x06 == rcvPkt[0])
        {
                      printf("\n[DispenseCard_c()] Acknowledgement Received");
                      totalByteToRecv = 12;
                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\n[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen); 
                      if(0 == rtcode)
		      {
				 /* 1 = Communication Failure */
				 ClearReceiveBuffer (h_commport);
				 ClearTransmitBuffer(h_commport);
				 return 1;
		      }
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[DispenseCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                                /* Card Status Code st0 */
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] No Card in MTK-571");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] One Card in Gate");
                                           /* 3 = Insert/return mouth block */
                                           returnMouthFlag = 1; 
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] One Card on RF/IC Card Position");
                                           /* 2 = Channel blocked */
                                           channelBlockFlag = 1;
                                  
		                        } 
                                }
                                /* Card Status Code st1 */
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] No Card in Stacker");
                                           /* 4 = Stacker Empty */
                                           stackerEmptyFlag = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] Enough Card in the Box");
		                        }   
                                } 
                                /* Card Status Code st2 */
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[DispenseCard_c()] Error Card Bin Full");
                                           
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
		            printf("\n[DispenseCard_c()] Failed to send total Acknowledgement Command ");
                            /* 1 = Communication Failure */
                            return 1;
		      }
                      if(1 == returnMouthFlag)
                      { 
                      	    /* 3 = Insert/return mouth block */
                            return 3; 
                      }
                      if(1 == channelBlockFlag)
                      {    
                            /* 2 = Channel blocked */
                            return 2;
                      }
                      if(1 == stackerEmptyFlag)
                      {
                            /* 4 = Stacker Empty */
                            return 4;  
                      }
       }
       /* If Return Data is 15h then No need to read Data */
       else if (0x15 == rcvPkt[0])
       {
             printf("\n[DispenseCard_c()] Nak Reply Received");
             /* 6 = Other Error */
             return 6;  
       }

       printf("\n[DispenseCard_c()] All Status OK Now Going to Dispense Card");
       printf("\n[DispenseCard_c()] Going to send Dispense Card Command ");
       bcc = getBcc(9,g_dispenseCard);
       g_dispenseCard[8] = bcc;
       printf("\n[DispenseCard_c()] bcc value is 0x%xh",bcc);

       /* Before Send Command clear all serial buffer */
       ClearReceiveBuffer (h_commport);
       ClearTransmitBuffer(h_commport);

       for(i=0;i<9;i++)
       {
		 printf("\n[DispenseCard_c()] Dispense Card Command[%d] = 0x%xh",i,g_dispenseCard[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,g_dispenseCard[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[DispenseCard_c()] Failed to Send Dispense Card Command ");
                    /* 1 = Communication Failure */ 
                    return 1;
                 }
	}
        
        /* Now going to Check Acknowledgement*/  
        totalByteToRecv = 1;
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

        printf("\n[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        if(0 == rtcode)
        {
                 /* 1 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 1;
        }
        else
        {
                 printf("\n[DispenseCard_c()] Acknowledgement against Dispense Card Command[0] = 0x%xh.\n",rcvPkt[0]);
        }

        /* If Return Data is 06h then Going to Read 12byte Data */

        if(0x06 == rcvPkt[0])
        {
                 printf("\n[DispenseCard_c()] Acknowledgement Received");
                 totalByteToRecv = 12;
                 memset(rcvPkt,'\0',24);
                      
                 rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                 printf("\n[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
                 for(i=0;i<rcvPktLen;i++)
	         {
				printf("\n[DispenseCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                 }
                 /* 0 = Operation Successful */
                 return 0;
        }
        else if (0x15 == rcvPkt[0])
        {
             printf("\n[DispenseCard_c()] Nak Reply Received");
             /* 6 = Other Error */
             return 6;  
        }

}
