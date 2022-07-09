
#include "DisableCardAcceptance.h"

/*
* Function Name : DisableCardAcceptance_c 
* Return Type   : int 
                  0 = Operation Successful
                  1 = Communication Failure
                  2 = Operation timeout occurred
                  3 = Other Error 

* Parameters    : Name                           Type          Description 

                            
                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

*/

/*
* To Disable Card Acceptance 
*/

static unsigned char g_DisableCardAcceptance[9]={0xF2,0x00,0x00,0x03,0x43,0x33,0x31,0x03,0x00};


int DisableCardAcceptance_c(int Timeout)
{
        char bcc=0x00;
        unsigned char rcvPkt[11];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,returnMouthFlag = 0,channelBlockFlag = 0;
        unsigned int rcvPktLen =0x00; 

        printf("\n[DisableCardAcceptance_c()] Going to Send Disable Acceptance Command");
        bcc = getBcc(9,g_DisableCardAcceptance);
        g_DisableCardAcceptance[8] = bcc;
        printf("\n[DisableCardAcceptance_c()] bcc value is 0x%xh",bcc);


        /* Before Send Command clear all serial buffer */
        ClearReceiveBuffer (h_commport);
        ClearTransmitBuffer(h_commport);

        for(i=0;i<9;i++)
	{
		 printf("\n[DisableCardAcceptance_c()] Disable Acceptance Command[%d] = 0x%xh",i,g_DisableCardAcceptance[i]);
		 send_rvalue = SendSingleByteToSerialPort(h_commport,g_DisableCardAcceptance[i]);
                 if(0 == send_rvalue) 
                 {
                    printf("\n[DisableCardAcceptance_c()] Failed to Send Report status Command ");
                    /* 1 = Communication Failure */
                    return 1;
                 }
	}

        printf("\n[DisableCardAcceptance_c()] Disable Acceptance Command send Successfully");
        printf("\n[DisableCardAcceptance_c()] Now Going to read Acknowledgement");


        /* Now going to Check Acknowledgement*/  
  
        totalByteToRecv = 1;
        memset(rcvPkt,'\0',11); 
         
        rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);

        printf("\n[DisableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        if(0 == rtcode)
        {
                 /* 1 = Communication Failure */
                 ClearReceiveBuffer (h_commport);
                 ClearTransmitBuffer(h_commport);
                 return 1;
        }
        else
        {
                 printf("\n[DisableCardAcceptance_c()] Acknowledgement against Disable Acceptance Command[0] = 0x%xh.\n",rcvPkt[0]);
        }

        /* If Return Data is 06h then Going to Read 12byte Data */

        if(0x06 == rcvPkt[0])
        {
                 printf("\n[DisableCardAcceptance_c()] Acknowledgement Received");
                 totalByteToRecv = 12;
                 memset(rcvPkt,'\0',11);
                      
                 rtcode = statusRead(h_commport,rcvPkt,&rcvPktLen,totalByteToRecv);
                 printf("\n[DisableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
                 for(i=0;i<rcvPktLen;i++)
	         {
				printf("\n[DisableCardAcceptance_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                 }
                 /* 0 = Operation Successful */
                 return 0;
        }
        else if (0x15 == rcvPkt[0])
        {
             printf("\n[DisableCardAcceptance_c()] Nak Reply Received");
             /* 3 = Other Error */
             return 3;  
        }

}
