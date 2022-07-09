
#include "ConnectDevice.h"


int h_commport;
static unsigned char g_resetData_Retain_Channel[9]={0xF2,0x00,0x00,0x03,0x43,0x30,0x33,0x03,0x00}; 
static unsigned char g_resetData_Capture_Card_Bin[9]={0xF2,0x00,0x00,0x03,0x43,0x30,0x31,0x03,0x00}; 
static unsigned char g_resetData_CardHolding_Position[9]={0xF2,0x00,0x00,0x03,0x43,0x30,0x30,0x03,0x00}; 


/*
* Function Name : ConnectDevice_c 
* Return Type   : int 
                  0 = Device Connected Successfully
                  1 = Port Does Not Exits 
                  2 = Communication Failure
                  3 = Channel Clearance failed due to rejection bin full   [ Not Tested] 
                  4 = Channel Clearance failed due to return mouth blocked [ Not implemented]
                  5 = Channel Clearance failed due to unknown reason
                  6 = Operation timeout occurred
                  7 = Other Error 
* Parameters    : Name                           Type          Description 

                  PortId                         int           Serial Port number to which the Device is Connected.

                  ChannelClearanceMode           int           If there is a card in the device Channel either it will be sent to 		                                                       rejection bin or return from the mouth of the device or kept in the 								       position. 
                                                               0 = Retain in the Channel.
                                                               1 = Send to Rejection bin.
                                                               2 = Return from the mouth of the Device.
                    
                  Timeout                         int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

*/
int ConnectDevice_c(int PortId, int ChannelClearanceMode, int Timeout)
{
         
        char bcc=0x00;
        unsigned char initialized_Data[9],rcvPkt[25];
        int i=0,openPortStatus = -1,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,cardBinFullFlag = 0;
        unsigned int rcvPktLen =0x00;
         
         

         
        /*
        * Check COM port Here
        */

        printf("\n[ConnectDevice_c()] Going to Open Port");
        if(PortId < 0)
        {
           printf("\n[ConnectDevice_c()] Fail to Open Port ");
           /* 1 = Port Does Not Exits */
           ClearReceiveBuffer (h_commport);
           ClearTransmitBuffer(h_commport);
           return 1;
        } 
        /* Now Call Open Port Function */ 
         
	openPortStatus = OpenPort(PortId,&h_commport);
        
        if(1 == openPortStatus)
        {
           printf("\n[ConnectDevice_c()] Open Port Successfully");
        }
        else
        {
           printf("\n[ConnectDevice_c()] Fail to Open Port ");
           /* 1 = Port Does Not Exits */
           ClearReceiveBuffer (h_commport);
           ClearTransmitBuffer(h_commport);   
           return 1; 
        }

        /* 
         * ChannelClearanceMode Check Here
        */

         if(ChannelClearanceMode == 0)
         {
            /* Retain in the Channel */
            /* Now going to get bcc */
            printf("\n[ConnectDevice_c()] Initialise with Retain in the Channel");
            bcc = getBcc(9,g_resetData_Retain_Channel);
            g_resetData_Retain_Channel[8] = bcc;
            printf("\n[ConnectDevice_c()] bcc value is 0x%xh",bcc); 
            memset(initialized_Data,'\0',9);
            for(i=0;i<9;i++)
	    {
                     initialized_Data[i]=g_resetData_Retain_Channel[i];
	    }
         }
         else if(ChannelClearanceMode == 1)
         {
            /* Send to Rejection bin */
            /* Now going to get bcc */
            printf("\n[ConnectDevice_c()] Initialise with Send to Rejection bin");
            bcc = getBcc(9,g_resetData_Capture_Card_Bin);
            g_resetData_Capture_Card_Bin[8] = bcc;
            printf("\n[ConnectDevice_c()] bcc value is 0x%xh",bcc);
            memset(initialized_Data,'\0',9);
            for(i=0;i<9;i++)
	    {
                     initialized_Data[i] = g_resetData_Capture_Card_Bin[i];
            }    
         }
         else if(ChannelClearanceMode == 2)
         {
            /* Return from the mouth of the Device */
            /* Now going to get bcc */
            printf("\n[ConnectDevice_c()] Initialise with Return from the mouth of the Device");
            bcc = getBcc(9,g_resetData_CardHolding_Position);
            g_resetData_CardHolding_Position[8] = bcc;
            printf("\n[ConnectDevice_c()] bcc value is 0x%xh",bcc); 
            memset(initialized_Data,'\0',9);
            for(i=0;i<9;i++)
	    {
                     initialized_Data[i] = g_resetData_CardHolding_Position[i];
            }  
         }
         else
         {  
            /* ChannelClearanceMode does not match so return from this function */
            printf("\n[ConnectDevice_c()] Initialise Failed");
            printf("\n[ConnectDevice_c()] Invalid ChannelClearanceMode found from Parameters");
            printf("\n[ConnectDevice_c()] Now going to Return");
            /* 5 = Channel Clearance failed due to unknown reason */
            ClearReceiveBuffer (h_commport);
            ClearTransmitBuffer(h_commport);
            return 5;

         }
         
         printf("\n[ConnectDevice_c()] ChannelClearanceMode ok Now going to Send Initialize Data");
         /* Before Send Command clear all serial buffer */

         ClearReceiveBuffer (h_commport);
         ClearTransmitBuffer(h_commport);

         for(i=0;i<9;i++)
	 {
		 printf("\n[ConnectDevice_c()] Initialize Command[%d] = 0x%xh",i,initialized_Data[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,initialized_Data[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[ConnectDevice_c()] Failed to send total Initialize Command ");
                    return 2;
                 }
	 }
         printf("\n[ConnectDevice_c()] Initialize Data send Successfully");
         printf("\n[ConnectDevice_c()] Now Going to read Acknowledgement");
         
         /* Now going to Check Acknowledgement*/  

        
          
         totalByteToRecv = 1;
         memset(rcvPkt,'\0',24); 
         
         rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

         printf("\n[ConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
         if(0 == rtcode)
         {
                 /* 2 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 2;
         }
         else
	 {
		 printf("\n[ConnectDevice_c()] Acknowledgement against Initialize Command [0] = 0x%xh.\n",rcvPkt[0]);
				
	 }
         
         /* If Return Data is 06h then Going to Read 25byte Data */

         if(0x06 == rcvPkt[0])
         {
                      printf("\n[ConnectDevice_c()] Acknowledgement Received");
                      totalByteToRecv = 25;
                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                      printf("\n[ConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen); 
                      if(0 == rtcode)
		      {
				 /* 2 = Communication Failure */
				 ClearReceiveBuffer (h_commport);
				 ClearTransmitBuffer(h_commport);
				 return 2;
		      }             
                      for(i=0;i<rcvPktLen;i++)
	              {
				printf("\n[ConnectDevice_c()] Initialize Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                                /* Card Status Code st0 */
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] No Card in MTK-571");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] One Card in Gate");
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] One Card on RF/IC Card Position");
		                        } 
                                }
                                /* Card Status Code st1 */
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] Enough Card in the Box");
		                        }   
                                } 
                                /* Card Status Code st2 */
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           printf("\n[ConnectDevice_c()] Error Card Bin Full");
                                           /* 3 = Channel Clearance failed due to rejection bin full */
                                           cardBinFullFlag = 1;
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
		            printf("\n[ConnectDevice_c()] Failed to send total Acknowledgement Command ");
                            /* 2 = Communication Failure */
                            return 2;
		      }
                       
                      if(1 == cardBinFullFlag)
                      {
                        /* 3 = Channel Clearance failed due to rejection bin full */
                        return 3;  
                      }
                      /* 0 = Device Connected Successfully */
                      return 0;
                       
         }
         /* If Return Data is 15h then No need to read Data */
         else if (0x15 == rcvPkt[0])
         {
             printf("\n[ConnectDevice_c()] Nak Reply Received");
             /* 7 = Other Error */
             return 7;  
         }
         
         
}


/*

void main()
{
                
		int i = ConnectDevice_c(0,0,0);  // (PortId,ChannelClearanceMode,Timeout)
		
		if(0 == i)
		{
			printf("\nDevice Connected Successfully");
		}
		else if(1 == i)
		{
			printf("\nPort Does Not Exits");
		}
                else if(2 == i)
		{
			printf("\nCommunication Failure");
		}
                else if(3 == i)
		{
			printf("\nChannel Clearance failed due to rejection bin full");
		}
                else if(4 == i)
		{
			printf("\nChannel Clearance failed due to return mouth blocked");
		}
                else if(5 == i)
		{
			printf("\nChannel Clearance failed due to unknown reason");
		}
                else if(6 == i)
		{
			printf("\nOperation timeout occurred");
		}
                else if(7 == i)
		{
			printf("\nOther Error");
		}
		printf("\n"); 
}

*/









