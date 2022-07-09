
#include "ReturnCard.h"


/*
* Function Name : ReturnCard_c 
* Return Type   : int 
                  0 = Operation Successful
                  1 = Communication Failure
                  2 = Return mouth block
                  3 = No Card in the Channel
                  4 = Operation timeout Occurred 
                  5 = Other Error
                

* Parameters    : Name                           Type          Description 
                  
                  DispenseMode                   int           0 = Hold at the mouth of the device until it taken by the Customer 
                                                               1 = Dispense it immediately  
                            
                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

*/

/*
* To get Current Report status of st0, st1, st2
*/

static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};

/*
* To Hold at Mouth
*/

static unsigned char g_holdAtMouth[9]={0xF2,0x00,0x00,0x03,0x43,0x32,0x30,0x03,0x00};

/*
* To Move to Error Bin
*/

static unsigned char g_moveToErrorBin[9]={0xF2,0x00,0x00,0x03,0x43,0x32,0x33,0x03,0x00};



int ReturnCard_c(int DispenseMode,int Timeout)
{
        char bcc=0x00;
        unsigned char rcvPkt[25],DispenseModeData[9];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,returnMouthFlag = 0,noCardInChannelFlag = 0;
        int oneCardInRF_IC = 0,ErrorBinFull = 0;
        unsigned int rcvPktLen =0x00; 

        printf("\n[ReturnCard_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = bcc;
        printf("\n[ReturnCard_c()] bcc value is 0x%xh",bcc);

        /* Before Send Command clear all serial buffer */
        ClearReceiveBuffer (h_commport);
        ClearTransmitBuffer(h_commport);
       
        for(i=0;i<9;i++)
	{
		 printf("\n[ReturnCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[ReturnCard_c()] Failed to Send Report status Command ");
                    /* 1 = Communication Failure */
                    return 1;
                 }
	}

        printf("\n[ReturnCard_c()] Status Command send Successfully");
        printf("\n[ReturnCard_c()] Now Going to read Acknowledgement");

        /* Now going to Check Acknowledgement */  

         totalByteToRecv = 1;
         memset(rcvPkt,'\0',24); 
         
         rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

         printf("\n[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
         if(0 == rtcode)
         {
                 /* 1 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 1;
         }
         else
	 {
		 printf("\n[ReturnCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
				
	 }

         /* If Return Data is 06h then Going to Read 25byte Data */

         if(0x06 == rcvPkt[0])
         {
                      printf("\n[ReturnCard_c()] Acknowledgement Received");
                      totalByteToRecv = 12;
                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\n[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen); 
                      if(0 == rtcode)
		      {
				 /* 1 = Communication Failure */
				 ClearReceiveBuffer (h_commport);
				 ClearTransmitBuffer(h_commport);
				 return 1;
		      }             
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[ReturnCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                                /* Card Status Code st0 */
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] No Card in MTK-571");
                                           /* 4 = No Card in the Channel */ 
                                           noCardInChannelFlag = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] One Card in Gate");
                                           /* 2 = Return mouth block */
                                           returnMouthFlag = 1;
                  			   
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] One Card on RF/IC Card Position");
                                           oneCardInRF_IC = 1;
                                           
		                        } 
                                }
                                /* Card Status Code st1 */
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] Enough Card in the Box");
		                        }   
                                } 
                                /* Card Status Code st2 */
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[ReturnCard_c()] Error Card Bin Full");
                                           ErrorBinFull = 1; 
                                           
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
		            printf("\n[ReturnCard_c()] Failed to send total Acknowledgement Command ");
                            /* 1 = Communication Failure */
                            return 1;
		      }
                      if(1 == noCardInChannelFlag)
                      {
                           /* 3 = No Card in the Channel */ 
                           printf("\n[ReturnCard_c()] No Card in Channel ");
                           return 3; 
                        
                      }
                      if(1 == returnMouthFlag)
                      {
                           /* 2 = Return mouth block */
                           printf("\n[ReturnCard_c()] Return Mouth Block ");
                           return 2; 
                           
                      }
                      
                       
         }
         /* If Return Data is 15h then No need to read Data */
         else if (0x15 == rcvPkt[0])
         {
             printf("\n[ReturnCard_c()] Nak Reply Received");
             /* 5 = Other Error */
             return 5;  
         }

         /* Return Mouth not blocked and No card in Channel So going to check one card in One Card on RF/IC Card Position or Not */ 
         if(0 == DispenseMode)
         {
            
            printf("\n[ReturnCard_c()] Hold at the Mouth untill it is taken by the customer");
            if(1 == oneCardInRF_IC)
            {
                /* Found one Card in RF/IC Reader */
                printf("\n[ReturnCard_c()] Got One Card in RF/IC Reader and Going to return from Mouth ");
                /* Going to issue command to send the card to Mouth */ 
                
                    /* Now going to get bcc */
		    printf("\n[ReturnCard_c()] Going to get bcc for Hold at Mouth ");
		    bcc = getBcc(9,g_holdAtMouth);
		    g_holdAtMouth[8] = bcc;
		    printf("\n[ReturnCard_c()] bcc value is 0x%xh",bcc); 
		    memset(DispenseModeData,'\0',9);
		    for(i=0;i<9;i++)
		    {
		             DispenseModeData[i]=g_holdAtMouth[i];
		    }

            }
         } 
         else if(1 == DispenseMode)
         {
            printf("\n[ReturnCard_c()] Dispense it immediately");
            if((1 == ErrorBinFull) || (0 == oneCardInRF_IC))
            {
               /* 5 = Other Error */
               printf("\n[ReturnCard_c()] Unable to Dispense Error bin Full or No Card in RF/IC ");    
               return 5;
              
            }
            
            else
            {
               /* Found one Card in RF/IC Reader */
               printf("\n[ReturnCard_c()] Got One Card in RF/IC Reader and Going to Send to Error bin ");
               /* Going to issue command to send the card to error bin */
               
                    /* Now going to get bcc */
		    printf("\n[ReturnCard_c()] Going to get bcc for Send to Error bin ");
		    bcc = getBcc(9,g_moveToErrorBin);
		    g_moveToErrorBin[8] = bcc;
		    printf("\n[ReturnCard_c()] bcc value is 0x%xh",bcc); 
		    memset(DispenseModeData,'\0',9);
		    for(i=0;i<9;i++)
		    {
		             DispenseModeData[i]=g_moveToErrorBin[i];
		    }
            }

         }
         else
         {
            printf("\n[ReturnCard_c()] Function parameter DispenseMode not Ok ");
            /* 5 = Other Error */
            return 5;
         }
        
        /* All status Ok Now going to issue Dispense Mode Command */
        for(i=0;i<9;i++)
	{
		 printf("\n[ReturnCard_c()] Dispense Mode Command[%d] = 0x%xh",i,DispenseModeData[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,DispenseModeData[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[ReturnCard_c()] Failed to Send Dispense Mode Command ");
                    /* 1 = Communication Failure */
                    return 1;
                 }
	}

        printf("\n[ReturnCard_c()] Dispense Mode Command send Successfully");
        printf("\n[ReturnCard_c()] Now Going to read Acknowledgement");
        
        /* Now going to Check Acknowledgement */  

        totalByteToRecv = 1; 
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

        printf("\n[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

        if(0 == rtcode)
        {
                 /* 1 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 1;
        }
        else
	{
		 printf("\n[ReturnCard_c()] Acknowledgement against Dispense Mode Command [0] = 0x%xh.\n",rcvPkt[0]);
				
	} 

        /* If Return Data is 06h then Going to Read 25byte Data */

        if(0x06 == rcvPkt[0])
        {
                      printf("\n[ReturnCard_c()] Acknowledgement Received");
                      totalByteToRecv = 12;
                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\n[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen); 
                      if(0 == rtcode)
		      {
				 /* 1 = Communication Failure */
				 ClearReceiveBuffer (h_commport);
				 ClearTransmitBuffer(h_commport);
				 return 1;
		      }             
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[ReturnCard_c()] Initialize Command Dispense Mode Data[%d] = 0x%xh",i,rcvPkt[i]);
                      }
                      /* 0 = Operation Successful */
                      return 0;
        }
        /* If Return Data is 15h then No need to read Data */
        else if (0x15 == rcvPkt[0])
        {
             printf("\n[ReturnCard_c()] Nak Reply Received");
             /* 5 = Other Error */
             return 5;  
        }

}










