 
 // All CCTALK Device Code : NRI Coin Acceptor
 //                          NRI Coin Escrow
 //                          Atvm Door/Gate/Alarm Status
 //                          Atvm Electronics Vault Open/Close
 

#include "cctalkdevs.h"
#include "currencyreturn.h"
 
#ifdef CCTALK_DEVICE
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Request software revision = ‘CRS-F1-V1.09’
//++TX = 002 000 001 241 012
//++RX = 001 012 002 000 067 082 083 045 070 049 045 086 049 046 048 057 026

//Request comms revision = 1.4.2
//TX = 002 000 001 004 249
//RX = 001 003 002 000 001 004 002 243
/*
3.148 Header 4 - Request comms revision
Transmitted data : <none>
Received data :
[ release ] [ major revision ] [ minor revision ]
This command requests the ccTalk release number and the major / minor revision
numbers of the comms specification. This is read separately to the main software
revision number for the product which can be obtained with a 'Request software
revision' command.
The revision numbers should tie up with those at the top of this specification
document.
The ccTalk release number is used to track changes to the serial comms library in that
particular product.
For example, the first release of a product conforming to the specification document
at issue 4.6 should return [ 1 ] [ 4 ] [ 6 ]

Host sends...
[ 2 ] - destination address
[ 1 ] - 1 data byte = 1 x sub-header
[ 1 ] - source address
[ 100 ] - expansion header 100
[ 255 ] - sub-header 255 ( e.g. Request ASCII serial number )
[ 153 ] - checksum, 2 + 1 + 1 + 100 + 255 + 153 = 512 = 0

Host receives...
[ 1 ]- destination address
[ 8 ]- 8 data bytes
[ 2 ]- source address
[ 0 ]- reply header
[ 49 ]- ‘1’
[ 50 ]- ‘2’
[ 51 ]- ‘3’
[ 52 ]- ‘4’
[ 53 ]- ‘5’
[ 54 ]- ‘6’
[ 55 ]- ‘7’
[ 56 ] - ‘8’
[ 81 ] - checksum, 1 + 8 + 2 +... = 512 = 0
*/
//++Global Variables List
static int   g_cctalkhandler=-1;
static int   g_CCTALK_OpenFlag=false;
static volatile int   g_state_C;
static volatile int   g_poll_flag_C;
static unsigned char g_rcvPkt_C[COINACPTR_MAX_LEN_OF_PKT];
int  g_rcvPktLen_C=0;
int  g_cmdPktLen_C=0;
static pthread_mutex_t  CmdPktLengthMutuex = PTHREAD_MUTEX_INITIALIZER;
static int  g_prevEvent_C;
static coinType g_COIN;
static int g_lostEvent; 
static int g_nmbrOfCoinsAt_aTime;
static int g_CoinInhibitFlag=0;
//Malay Add 2 feb 2013
static unsigned int g_SpecificCoinInhibitFlag=0;
static unsigned int g_CACurrentAtvmFare=0;
static unsigned int g_CoinLowLevelPollFlagStatus=0;
static unsigned int g_CoinSkipPollFlag=0;
static unsigned int g_CloseCoinCreditPollingThreadFlag=0;
static unsigned int g_NaCoinInhibitFlag =0;
static unsigned int g_Nafaretobeaccept =0;
static bool g_AlreadyCoinPollFlag=false;

#ifdef NRI_FIRMWARE_12_03

//Created By Malay Maji Date 13 Dec 2012
//NRI Coin Acceptor Coin Table
/*
    First InhibitByte:
    ------------------------------------------------ 
    Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0
     x     x     x     x     x     x     x      x
    ------------------------------------------------
    Bit 0 =50 paisa enable if it 1 or disable if it 0
    Bit 1 =50 paisa enable if it 1 or disable if it 0
    Bit 2 =Rs1 (Type 1 year 1997) enable if it 1 or disable if it 0
    Bit 3 =Rs1 (Type 2) enable if it 1 or disable if it 0
    Bit 4 =Rs2 enable if it 1 or disable if it 0
    Bit 5 =Rs5 (Type 1 year 2002) enable if it 1 or disable if it 0
    Bit 6 =Rs5 (Type 2 year 2007) enable if it 1 or disable if it 0
    Bit 7 =Rs5 (Type 3 year 2012 Golden) enable if it 1 or disable if it 0

     Second InhibitByte:
    ------------------------------------------------ 
     Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0
      x     x     x     x     x     x      x      x
     ------------------------------------------------
     Bit 0 =Rs 10 enable if it 1 or disable if it 0
     other bits for Token so dont needed for this operation 

*/

//Coin Table for NRI Coin Acceptor Firmware Version: 12.03
static CoinTable g_CoinTable[8]={ .50,2,0x3,0x0,   //First  Byte=0b00000011
                                  1,3,  0xC,0x0,   //First  Byte=0b00001100
                                  1,4,  0xC,0x0,   //First  Byte=0b00001100
                                  2,5,  0x10,0x0,  //First  Byte=0b00010000
                                  5,6,  0xE0,0x0,  //First  Byte=0b11100000
                                  5,7,  0xE0,0x0,  //First  Byte=0b11100000
                                  5,8,  0xE0,0x0,  //First  Byte=0b11100000
                                  10,9, 0x0, 0x1   //Second Byte=0b00000001
                                };

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NRI_FIRMWARE_12_06
/*
    First InhibitByte:
    ------------------------------------------------ 
    Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0
     x     x     x     x     x     x     x      x
    ------------------------------------------------
    Bit 0 =Rs1  if it 1 enable or disable if it 0
    Bit 1 =Rs2  if it 1 enable or disable if it 0
    Bit 2 =Rs5  if it 1 enable or disable if it 0
    Bit 3 =Rs10 if it 1 enable or disable if it 0
   

     Second InhibitByte no need
    ------------------------------------------------ 
     Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0
      x     x     x     x     x     x      x      x
    ------------------------------------------------
  
*/
//Coin Table for NRI Coin Acceptor Firmware Version: 12.06
static CoinTable g_CoinTable[8]={  0,0,0x0,0x0,     
                                   1,1,  0x1,0x0,   //First  Byte=0b00000001
                                   1,1,  0x1,0x0,   //First  Byte=0b00000001
                                   2,2,  0x02,0x0,  //First  Byte=0b00000010
                                   5,3,  0x04,0x0,  //First  Byte=0b00000100
                                   5,3,  0x04,0x0,  //First  Byte=0b00000100
                                   5,3,  0x04,0x0,  //First  Byte=0b00000100
                                   10,4, 0x8, 0x0   //First  Byte=0b00001000
                                };

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static pthread_mutex_t CoinPollFlagMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t CASpecificInhibitmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t CASetSpecificFaremutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t CASetPollFlag = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t CAEscrowFlagStatusmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  CoinLowLevelPollFlagStatusmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  CloseCoinCreditPollingThreadFlagmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  NaCoinInhibitFlagmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  CoinInhibitFlagmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  RecvPktLengthMutuex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  PrevEventMutuex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  GlobalRecvpacketMutuex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_CAEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  g_CAEnableThreadCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t  g_cctalkopenflagmutex 	= PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_cctalkporthandlemutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_getsecuredstausmutex 	= PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  g_getdisablealarmmutex 	= PTHREAD_MUTEX_INITIALIZER;
static bool g_CADisableAcceptanceFlag;
static pthread_mutex_t g_CADisableAcceptanceFlagMutex= PTHREAD_MUTEX_INITIALIZER;

//#if !defined(NOTE_ACCEPTOR)
int g_StackAcceptedTime;
int g_ReturnAcceptedTime;
//#endif
static  int g_DenomMaskValue=0x00;
static  pthread_mutex_t g_DenomMaskValuemutex=PTHREAD_MUTEX_INITIALIZER;
static unsigned char  g_BCAId[MAX_STR_SIZE_OF_LOG];
static unsigned char  g_BCEId[MAX_STR_SIZE_OF_LOG];
static unsigned char  g_KSDId[MAX_STR_SIZE_OF_LOG];
static int   g_CCTALK_CoinAcceptorFlag=false;
static int   g_CCTALK_TokenDispenserFlag=false;
static int   g_CCTALK_SecurtityFlag=false; 
static bool  g_CAExactFareFlag=false;
static bool  g_CCTalkOperationFlag 					= false;
static pthread_mutex_t  g_CcTalkPortOperationFlag 	= PTHREAD_MUTEX_INITIALIZER;
static char  g_StatusByte=0x00; 

static bool  g_CCTalkPollInterValTime = 0;

int IsCCTALKPortBusy(){
	if (pthread_mutex_trylock(&g_cctalkporthandlemutex) == 0)
    {
		AVRM_writeFileLogV2("[IsCCTALKPortBusy()] Port is available",INFO,g_BCAId);
        pthread_mutex_unlock (&g_cctalkporthandlemutex);
        return 1; //Not Busy
    }
	else{
		AVRM_writeFileLogV2("[IsCCTALKPortBusy()] Port is not available now",INFO,g_BCAId);
		return 0;// Busy
	}
}//++int IsCCTALKPortBusy() end

int SetCCTalkPollInterValTime(int fnCCTalkPollInterValTime){
		g_CCTalkPollInterValTime = fnCCTalkPollInterValTime;
}//++int IsCCTALKPortBusy() end

void CCTALK_SetDenomMaskValue(int DenomMaskValue){

        unsigned char log[100];
        memset(log,'\0',100);
        pthread_mutex_lock(&g_DenomMaskValuemutex);
        g_DenomMaskValue = 0x00;
        g_DenomMaskValue = DenomMaskValue;
        memset(log,'\0',100);
        sprintf(log,"[CCTALK_SetDenomMaskValue()] Coin Denom Mask : %d",g_DenomMaskValue); 
        AVRM_writeFileLogV2(log,INFO,g_BCAId);
        pthread_mutex_unlock(&g_DenomMaskValuemutex); 

}//void CCTALK_SetDenomMaskValue() end

int SetBCADeviceId(unsigned char* fnBCADeviceID ) {
    memset(g_BCAId,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_BCAId ,fnBCADeviceID );
    //printf("\n[SetBCADeviceId()] log dll file path=%s \n",fnBCADeviceID);
    return 1;
}//int  SetBCADeviceId(unsigned char* fnBCADeviceID ) end

int SetBCEDeviceId(unsigned char* fnBCEDeviceID ) {
    memset(g_BCEId,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_BCEId ,fnBCEDeviceID );
    //printf("\n[SetBCEDeviceId()] log dll file path=%s \n",fnBCEDeviceID);
    return 1;
}//int  SetBCEDeviceId(unsigned char* fnBCEDeviceID ) end

int SetKSDDeviceId(unsigned char* fnKSDDeviceID ) {
    memset(g_KSDId,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_KSDId ,fnKSDDeviceID );
    //printf("\n[SetKSDDeviceId()] log dll file path=%s \n",fnKSDDeviceID);
    return 1;
}//int  SetKSDDeviceId(unsigned char* fnKSDDeviceID ) end

int EnableCoinAcceptor(struct coinType fnCoin) {

                        //++Issue command
                        int  rtcode =-1;
                        unsigned char log[200];
                        memset(log,'\0',200);
                        int CommandReplyLength = 0;
                        unsigned char rcvPkt[60];
                        unsigned char rcvPkt2[100];
                        memset(rcvPkt,'\0', 60 );
                        memset(rcvPkt2,'\0', 60 );
                        int rcvPktLen=0;
                        int rtcode2=0,rcvPktLen2=0;
                        struct coinType UserDefinedCoin={0,0,0,0,0};
                        
                        if( ENABLE == fnCoin.rs_1 ){
			                 UserDefinedCoin.rs_1    = 1; 
                             AVRM_writeFileLogV2("[EnableCoinAcceptor()] Rs.1 Enabled.",INFO,g_BCAId);
                        }//if end
                        
                        if( ENABLE == fnCoin.rs_2 ){
			                 UserDefinedCoin.rs_2    = 1; 
                             AVRM_writeFileLogV2("[EnableCoinAcceptor()] Rs.2 Enabled.",INFO,g_BCAId);
                        }//if end
                        
                        if( ENABLE == fnCoin.rs_5 ){
			                 UserDefinedCoin.rs_5    = 1; 
                             AVRM_writeFileLogV2("[EnableCoinAcceptor()] Rs.5 Enabled.",INFO,g_BCAId);
                        }//if end
                        
                        if( ENABLE == fnCoin.rs_10 ){
			                 UserDefinedCoin.rs_10    = 1; 
                             AVRM_writeFileLogV2("[EnableCoinAcceptor()] Rs.10 Enabled.",INFO,g_BCAId);
                        }//if end
                        
                        //++Now enable coin status
                        pthread_mutex_lock( &g_cctalkporthandlemutex);
                        
                        rtcode=-1;
                        MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(fnCoin);
                        rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId); 
                        
					    pthread_mutex_unlock( &g_cctalkporthandlemutex);
					    
                        //++Store Last Counter Value                        
                        memset(rcvPkt2,'\0', 60 );
                        rcvPktLen2 = 0;
                        
                        pthread_mutex_lock( &g_cctalkporthandlemutex);
                        
                        MONEYCONTROL_CoinAcptr_Cmd_CreditPoll(); 
                        rtcode2=-1;
                        rtcode2 = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt2,&rcvPktLen2,CCTALK_REPLY_WAIT_TIME,g_BCAId);
                        
                        pthread_mutex_unlock( &g_cctalkporthandlemutex); 
                          
					    memset(log,'\0',200);
					    sprintf(log,"[EnableCoinAcceptor()] Poll Recv PacketLength = %d .", rcvPktLen2 );
						AVRM_writeFileLogV2(log,INFO,g_BCAId);
						
						if( SUCCESS ==  rtcode2 ){ 
							                          
								pthread_mutex_lock(&RecvPktLengthMutuex);
								CommandReplyLength=g_rcvPktLen_C;
								pthread_mutex_unlock(&RecvPktLengthMutuex);
								
								memset(log,'\0',200);
								sprintf(log,"[EnableCoinAcceptor()] Global Poll Recv PacketLength = %d .", CommandReplyLength );
								AVRM_writeFileLogV2(log,INFO,g_BCAId);
								
								memset(log,'\0',200);
								sprintf(log,"[EnableCoinAcceptor()] Command Byte= 0x%x .", rcvPkt2[3] );
								AVRM_writeFileLogV2(log,INFO,g_BCAId);
								
								pthread_mutex_lock(&PrevEventMutuex);
							    if( ( 0xe5 == rcvPkt2[3] ) && ( rcvPktLen2 == CommandReplyLength) ) {

										  g_prevEvent_C = rcvPkt2[9];
										  pthread_mutex_unlock(&PrevEventMutuex);
										  AVRM_writeFileLogV2("[EnableCoinAcceptor()] Store last event counter success.",INFO,g_BCAId);
										  memset(log,'\0',200);
										  sprintf(log,"[EnableCoinAcceptor()] Event Counter = %d .", g_prevEvent_C );
										  AVRM_writeFileLogV2(log,INFO,g_BCAId);
						   
								}else{
										 pthread_mutex_unlock(&PrevEventMutuex);
										 AVRM_writeFileLogV2("[EnableCoinAcceptor()] Credit Poll Command Reply Packet mismatch error.",ERROR,g_BCAId);
										
								}//else end
								
                        }else {
							AVRM_writeFileLogV2("[EnableCoinAcceptor()] Unable to store counter value due to credit poll command issue failed.",ERROR,g_BCAId);
						}//else end
						
						if( SUCCESS == rtcode  ){
							   
							AVRM_writeFileLogV2("[EnableCoinAcceptor() Exit] Enable coin success..",INFO,g_BCAId);
							return 1;
							
						}else{
							
							AVRM_writeFileLogV2("[EnableCoinAcceptor() Exit] Enable coin failed..",ERROR,g_BCAId);
							return 0;
							
                        }//else end
                        
}//int EnableCoinAcceptor(noteType NOTE) end

/*
int MONEYCONTROL_CoinAcptr_statusRead(unsigned char* const rcvPkt,unsigned int* const rcvPktLen,int const CcTalkReplyWaitTime,unsigned char* fnDeviceId) 
{


	         unsigned char rxByte=0x00;

	         int rcvStat=0;

             unsigned int  byteRcvdCounter=0;

	         unsigned int  totalByteToRecv=0; 

	         unsigned int  totalByteMONEYCONTROL_CoinAcptr_statusReadIn=0; 

             unsigned char log[100];

             memset(log,'\0',100);

             int rtcode =0,totalByteIn=0;
             
             pthread_mutex_lock(&RecvPktLengthMutuex);

	         //Store Reply packet total bytes
             totalByteToRecv = g_rcvPktLen_C;
             
             pthread_mutex_unlock(&RecvPktLengthMutuex);

             struct timespec begints={0}, endts={0},diffts={0};
             
             //++Start Time
             //++clock_gettime(CLOCK_MONOTONIC, &begints);
             clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begints);
      
       
             //memset(log,'\0',100);
             //sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] g_cctalkhandler = %d.",g_cctalkhandler );
             //AVRM_writeFileLogV2(log,INFO,g_BCAId);
             
			//++Set CCTalk Port Operation Flag Status
			pthread_mutex_lock(&g_CcTalkPortOperationFlag);
			g_CCTalkOperationFlag = true;
			pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
                             
            for(;;)
            {        
							 rxByte      = 0x00;
                             totalByteIn = 0;
                             
							 //Read serial port received register 
							 //rcvStat = ReceiveSingERRORleByteFromSerialPort_C(g_cctalkhandler,&rxByte,&totalByteIn); 
							 totalByteIn = read(g_cctalkhandler,&rxByte,1);
							 
							 //++memset(log,'\0',100);
                             //++sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] totalByteIn = %d.",totalByteIn);
                             //++writeFileLog(log);
                             
							 //if any byte received store it and increment byte counter
							 if(1 == totalByteIn) 
							 {                                
                                rcvPkt[byteRcvdCounter] = rxByte; 
                                byteRcvdCounter +=1;
						        *rcvPktLen      = byteRcvdCounter;	 
							 }//++if end

							 //Expected No. of Bytes received.
							 if(byteRcvdCounter == totalByteToRecv)
							 { 
								  
								   //memset(log,'\0',100);
								   //sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Expected Byte received in %d seconds. ",timeelapsed );
								   //AVRM_writeFileLogV2(log,INFO,fnDeviceId);
								   
								   //++clock_gettime(CLOCK_REALTIME, &endts);
								   //++clock_gettime(CLOCK_MONOTONIC, &endts);
								   
								   //long ns = (endts.tv_nsec-begints.tv_nsec);
								   //if (begints.tv_nsec > endts.tv_nsec) { // clock underflow 
								   //   ns += 1000000000; 
								   //} //if end
								   //double timeelapsed = (double)( ns/1000000 );
								   //memset(log,'\0',100);
								   //sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Expected Byte received in %.2f miliseconds. ",timeelapsed );
								   //AVRM_writeFileLogV2(log,INFO,fnDeviceId);
								   rtcode=SUCCESS;
								   //for( unsigned inCLOCK_REALTIMEt counter=0;counter < byteRcvdCounter;counter++){
								   //printf("\n [MONEYCONTROL_CoinAcptr_statusRead()]  Rx [%d]: 0x%xh.",counter,rcvPkt[counter] );
								   //}
								   //++LogDataExchnageV2( INFO,fnDeviceId,"Rx",rcvPkt,byteRcvdCounter);   
								   break;

							}//++if(byteRcvdCounter == totalByteToRecv) end
					 
							//Check Current timer stERRORatus
							//++clock_gettime(CLOCK_MONOTONIC, &endts);
							clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endts);

							diffts.tv_sec = endts.tv_sec - begints.tv_sec;

							//++Time out and CCTALK echo date received
							if((byteRcvdCounter < totalByteToRecv)  &&  ( diffts.tv_sec >= CcTalkReplyWaitTime ) )
							{
								
									memset(log,'\0',100);
									sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and CCTALK echo data received in %d seconds.",diffts.tv_sec);
									AVRM_writeFileLogV2(log,ERROR,fnDeviceId);																		
									memset(log,'\0',100);
									sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Total Byte to Recv: %d ",totalByteToRecv );
									AVRM_writeFileLogV2(log,ERROR,fnDeviceId);	
									memset(log,'\0',100);
									sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] No of data Received : %d ",byteRcvdCounter );
									AVRM_writeFileLogV2(log,ERROR,fnDeviceId);																		
									
									//long ns = (endts.tv_nsec-begints.tv_nsec);
								    //if (begints.tv_nsec > endts.tv_nsec) { //++clock underflow 
									//   ns += 1000000000; 
								    //} //if end
								    //double timeelapsed = (double)( ns/1000000 ) ;
								    //memset(log,'\0',100);
									//sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and CCTALK in %.2f miliseconds. ",timeelapsed );
									//AVRM_writeFileLogV2(log,ERROR,fnDeviceId);
																		
								    rtcode=FAIL;
									//++LogDataExchnageV2( ERROR,fnDeviceId,"Rx",rcvPkt,byteRcvdCounter);
									break;
		
						    }//if(  (byteRcvdCounter < totalByteToRecv)  &&  ( diffts.tv_sec >= CcTalkReplyWaitTime ) ) end  
					
						    //++Time out and No data received.      
						    if((byteRcvdCounter == 0)  &&  (diffts.tv_sec >= CcTalkReplyWaitTime)  ) 
						    { 
								
									 memset(log,'\0',100);
									 sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and No data received in %d seconds.",diffts.tv_sec);
									 AVRM_writeFileLogV2(log,ERROR,fnDeviceId);
									 //long ns = (endts.tv_nsec-begints.tv_nsec);
								     //if (begints.tv_nsec > endts.tv_nsec) { //++clock underflow 
									 //  ns += 1000000000; 
								     //}//if end 
								     //double timeelapsed = (double)( ns/1000000 ) ;
								     //memset(log,'\0',100);
									 //sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and No data received in %.2f miliseconds. ",timeelapsed );
									 //AVRM_writeFileLogV2(log,ERROR,fnDeviceId);
									 rtcode=FAIL;
									 //++LogDataExchnageV2( INFO,fnDeviceId,"Rx",rcvPkt,byteRcvdCounter);
									 break;
							 }//if(  (byteRcvdCounter == 0)  &&  (diffts.tv_sec >= CcTalkReplyWaitTime)  )  end     
			 

            }//++End for(;;)
                  
            //++Write Exchange Data
			LogDataExchnageV2( INFO,fnDeviceId,"Rx",rcvPkt,byteRcvdCounter);
			  
			//++Set CCTalk Port Operation Flag Status
			pthread_mutex_lock(&g_CcTalkPortOperationFlag);
			g_CCTalkOperationFlag = false;
			pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
			return rtcode;


}//++MONEYCONTROL_CoinAcptr_statusRead(unsigned char* const rcvPkt,unsigned int* const rcvPktLen,int const CcTalkReplyWaitTime,unsigned char* fnDeviceId) end
*/

int MONEYCONTROL_CoinAcptr_statusRead(unsigned char* const rcvPkt,unsigned int* const rcvPktLen,int const CcTalkReplyWaitTime,unsigned char* fnDeviceId) 
{
			 unsigned char rxByte=0x00;
	         int rcvStat=0;
			 unsigned int  byteRcvdCounter=0;
			 unsigned int  totalByteToRecv=0; 
			 unsigned int  totalByteMONEYCONTROL_CoinAcptr_statusReadIn=0; 
			 unsigned char log[100];
             memset(log,'\0',100);
             int rtcode =0,totalByteIn=0;
             pthread_mutex_lock(&RecvPktLengthMutuex);
             //++Store Reply packet total bytes
             totalByteToRecv = g_rcvPktLen_C;
             pthread_mutex_unlock(&RecvPktLengthMutuex);
             struct timespec begints={0}, endts={0},diffts={0};
             //++Start Time
             clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begints);
			//++Set CCTalk Port Operation Flag Status
			pthread_mutex_lock(&g_CcTalkPortOperationFlag);
			g_CCTalkOperationFlag = true;
			pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
			memset(log,'\0',100);
            sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()]  Total Bytes to Recvd = %d.",totalByteToRecv );
            AVRM_writeFileLogV2(log,INFO,fnDeviceId);
            int  bytesAvailable= 0 ;
            char read_buf [2048];
            memset(&read_buf, '\0', sizeof(read_buf));
            for(;;)
            {        
							 rxByte      = 0x00;
                             totalByteIn = 0;
                             bytesAvailable= 0 ;
                             //++Call Device Driver
                             ioctl(g_cctalkhandler, FIONREAD, &bytesAvailable);
                             if( bytesAvailable == totalByteToRecv )
                             {
								    //++Read serial port received data 
								    totalByteIn = 0;
									totalByteIn = read(g_cctalkhandler,&read_buf,totalByteToRecv);
						     }//if end
							//++Check if data there
							if( totalByteIn > 0 )
							{
								  memset(log,'\0',100);
								  sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()]  Read Data Bytes Recvd = %d.",totalByteIn );
								  AVRM_writeFileLogV2(log,INFO,fnDeviceId);	
							}//if end
							 //++Expected No. of Bytes received.
							 if(totalByteToRecv == totalByteIn)
							 {  
								   memset(log,'\0',100);
								   sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()]  Expected byte Recvd...");
								   AVRM_writeFileLogV2(log,INFO,fnDeviceId);	
								   rtcode=SUCCESS;
								   break;
							 }//if end
							//++Check Current timer 
							clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endts);
							diffts.tv_sec = endts.tv_sec - begints.tv_sec;
							//++Timer End
							if( diffts.tv_sec >= CcTalkReplyWaitTime )
							{									    
								         //++Read Serial Port Data if timeout 
								         bytesAvailable = 0;
										 ioctl(g_cctalkhandler, FIONREAD, &bytesAvailable);
										 if( bytesAvailable > 0 )
										 {
												//++Read serial port received data after timeout
												totalByteIn = 0;
												totalByteIn = read(g_cctalkhandler,&read_buf,bytesAvailable);
										 }//if end
								        
								        //++Time out and CCTALK echo date received
										if( (totalByteIn < totalByteToRecv)  &&  ( diffts.tv_sec >= CcTalkReplyWaitTime ) )
										{
											
												memset(log,'\0',100);
												sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and CCTALK echo data received in %d seconds.",diffts.tv_sec);
												AVRM_writeFileLogV2(log,ERROR,fnDeviceId);																		
												memset(log,'\0',100);
												sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Total Byte to Recv: %d ",totalByteToRecv );
												AVRM_writeFileLogV2(log,ERROR,fnDeviceId);	
												memset(log,'\0',100);
												sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] No of data Received : %d ",totalByteIn );
												AVRM_writeFileLogV2(log,ERROR,fnDeviceId);																		
												rtcode=FAIL;
												break;
					
										}					
										//++Time out and No data received.      
										else if(  (totalByteIn == 0)  &&  (diffts.tv_sec >= CcTalkReplyWaitTime)   ) 
										{ 								
												 memset(log,'\0',100);
												 sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and No data received in %d seconds.",diffts.tv_sec);
												 AVRM_writeFileLogV2(log,ERROR,fnDeviceId);
												 rtcode=FAIL;
												 break;
										 }else{
											     memset(log,'\0',100);
												 sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Status Read End in %d seconds.",diffts.tv_sec);
												 AVRM_writeFileLogV2(log,ERROR,fnDeviceId);
												 rtcode=FAIL;
												 break;
										 }//else end
							}//if end
            }//++End for(;;)
           
            //Return Total Return data
            *rcvPktLen  = totalByteIn;	 
             
			for( int counter=0;counter<totalByteIn;counter++)
			{
				 //memset(log,'\0',100);
				 //sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()]  read_buf[%d]= 0x%x.",counter, read_buf[counter] );
				 //AVRM_writeFileLogV2(log,INFO,fnDeviceId);	
				 rcvPkt[counter] = (unsigned char)read_buf[counter];
			}//for end
			
			/*for( int counter=0;counter<totalByteIn;counter++)
			{
				 memset(log,'\0',100);
				 sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()]  rcvPkt[%d]= 0x%x.",counter, rcvPkt[counter] );
				 AVRM_writeFileLogV2(log,INFO,fnDeviceId);	
			}//for end
			*/
			
			 //++Write Exchange Data
			LogDataExchnageV2( INFO,fnDeviceId,"Rx",rcvPkt,totalByteIn);
			
		   //++Set CCTalk Port Operation Flag Status
			pthread_mutex_lock(&g_CcTalkPortOperationFlag);
			g_CCTalkOperationFlag = false;
			pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
            return rtcode;

}//++MONEYCONTROL_CoinAcptr_statusRead(unsigned char* const rcvPkt,unsigned int* const rcvPktLen,int const CcTalkReplyWaitTime,unsigned char* fnDeviceId) end

int IsCCTALKPortOpen() {

		  int rtcode=-1;

		  pthread_mutex_lock( &g_cctalkopenflagmutex );

		  if(true == g_CCTALK_OpenFlag)
		  {
		        rtcode = 1;
		        ////writeFileLog("[IsCCTALKPortOpen()] g_CCTALK_OpenFlag is set to true");
		        AVRM_writeFileLogV2("[IsCCTALKPortOpen()] g_CCTALK_OpenFlag is set to true",INFO,g_BCAId);

		  }
		  else if(false == g_CCTALK_OpenFlag)
		  {
		        rtcode = 0;
		        ////writeFileLog("[IsCCTALKPortOpen()] g_CCTALK_OpenFlag is set to false");
		        AVRM_writeFileLogV2("[IsCCTALKPortOpen()] g_CCTALK_OpenFlag is set to false",INFO,g_BCAId);
		        
		  }//else if end
          
		  pthread_mutex_unlock( &g_cctalkopenflagmutex );
		 
		  return rtcode;


}//int IsCCTALKPortOpen() end

//++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
int ActivateCCTALKPortV2(int DeviceFlag,int portNmbr) {

          unsigned char log[100];
          memset(log,'\0',100);
          int cctalkopenrtcode =-1,comporthandler=-1;
          
          if( 1 == IsCCTALKPortOpen ){
			    
			    memset(log,'\0',100);
			    sprintf(log,"[ActivateCCTALKPortV2() IsCCTALKPortOpen Block] PortHandle ID: %d",g_cctalkhandler);
				AVRM_writeFileLogV2(log,INFO,g_BCAId);
                AVRM_writeFileLogV2("[ActivateCCTALKPortV2() IsCCTALKPortOpen Block] Already CCTALK Port opened.",INFO,g_BCAId);
                return 0;  //++Successfully Connected 
		  
		  }//++if end
		  
		  cctalkopenrtcode = OpenPort_C(portNmbr,&comporthandler);
          if( cctalkopenrtcode < 0 ) { //++port open failed
		  
					  pthread_mutex_lock( &g_cctalkopenflagmutex );
					  g_CCTALK_OpenFlag=false;    
					  pthread_mutex_unlock( &g_cctalkopenflagmutex );
			          AVRM_writeFileLogV2(log,ERROR,g_BCAId);
					  return 28; //++Communication failure 

		  }else if( 1 == cctalkopenrtcode ) { //++port open successed  

					  pthread_mutex_lock( &g_cctalkopenflagmutex );
					  g_CCTALK_OpenFlag=true;    
					  pthread_mutex_unlock( &g_cctalkopenflagmutex );
					  pthread_mutex_lock( &g_cctalkporthandlemutex);
					  g_cctalkhandler = comporthandler;
					  pthread_mutex_unlock( &g_cctalkporthandlemutex);
					  memset(log,'\0',100);
					  sprintf(log,"[ActivateCCTALKPortV2() Succeess Block] PortHandle ID: %d",g_cctalkhandler);
					  AVRM_writeFileLogV2(log,INFO,g_BCAId);
					  AVRM_writeFileLogV2("[ActivateCCTALKPortV2() Succeess Block] CCTALK Open Port opened successfully.",INFO,g_BCAId);
					  return 0; //++Successfully Connected 


		  }else{
			 AVRM_writeFileLogV2("[ActivateCCTALKPortV2()] CCTALK Open Port failed due to unknown reason.",ERROR,g_BCAId);
			 return 31; //+other error
		 }//else end
		
}//int ActivateCCTALKPortV2(int DeviceFlag,int portNmbr) end

int DeActivateCCTALKPortV2( int DeviceFlag ) {


           int rtcode=-1;
           unsigned char log[100];
           memset(log,'\0',100);
          
           
           pthread_mutex_lock( &g_cctalkporthandlemutex);
           
           memset(log,'\0',100);
		   sprintf(log,"[DeActivateCCTALKPortV2()] PortHandle ID: %d",g_cctalkhandler);
		   AVRM_writeFileLogV2(log,INFO,g_BCAId);
		   
		   //++rtcode=ClosePort_C(g_cctalkhandler);  
		   rtcode = close(g_cctalkhandler); //++Close Serial Port
		   
		   pthread_mutex_unlock( &g_cctalkporthandlemutex);
				  
		   if( 0 ==rtcode){
			   
					  AVRM_writeFileLogV2("[DeActivateCCTALKPort()] Close CCTALK Port successed.",INFO,g_BCAId);

				      pthread_mutex_lock( &g_cctalkopenflagmutex );
				      g_CCTALK_OpenFlag=false;
					  pthread_mutex_unlock( &g_cctalkopenflagmutex );

					  pthread_mutex_lock( &g_cctalkporthandlemutex);
					  g_cctalkhandler = -1;
					  pthread_mutex_unlock( &g_cctalkporthandlemutex);

				      return 0; //++Operation Success

		   }else {
                      AVRM_writeFileLogV2("[DeActivateCCTALKPort()] Close CCTALK Port failed.",ERROR,g_BCAId);
					  return 28; //++Communication Failure

           }//else end
          
}//int DeActivateCCTALKPort() end

int ActivateCCTALKPort(int portNmbr) {

          unsigned char log[100];
          memset(log,'\0',100);
          
          int cctalkopenrtcode =-1,comporthandler=-1;
          
          if( 1 == IsCCTALKPortOpen() ){
			    
			    memset(log,'\0',100);
			    sprintf(log,"[ActivateCCTALKPort()] PortHandle ID: %d",g_cctalkhandler);
				////writeFileLog(log);
				AVRM_writeFileLogV2(log,INFO,g_BCAId);

                AVRM_writeFileLogV2("[ActivateCCTALKPort()] Already CCTALK Port opened.",INFO,g_BCAId);
                ////writeFileLog("[ActivateCCTALKPort()] Already CCTALK Port opened.");
			    return 1; 
		  
		  }//if end

          cctalkopenrtcode = OpenPort_C(portNmbr,&comporthandler);

          if( cctalkopenrtcode < 0 ) //++port open failed
		  {
					  pthread_mutex_lock( &g_cctalkopenflagmutex );
					  g_CCTALK_OpenFlag=false;    
					  pthread_mutex_unlock( &g_cctalkopenflagmutex );
			          ////writeFileLog("[ActivateCCTALKPort()] CCTALK Open Port failed.");
			          AVRM_writeFileLogV2("[ActivateCCTALKPort()] CCTALK Open Port failed.",ERROR,g_BCAId);
					  return 0; 

		  } 
		  else if( 1 == cctalkopenrtcode ) //++port open successed
		  {  

					  pthread_mutex_lock( &g_cctalkopenflagmutex );
					  g_CCTALK_OpenFlag=true;    
					  pthread_mutex_unlock( &g_cctalkopenflagmutex );

					  pthread_mutex_lock( &g_cctalkporthandlemutex);
					  g_cctalkhandler = comporthandler;
					  pthread_mutex_unlock( &g_cctalkporthandlemutex);
					  
					  memset(log,'\0',100);
					  sprintf(log,"[ActivateCCTALKPort()] PortHandle ID: %d",g_cctalkhandler);
					  //writeFileLog(log);
					  AVRM_writeFileLogV2(log,INFO,g_BCAId);

					  ////writeFileLog("[ActivateCCTALKPort()] CCTALK Open Port opened successfully.");
					  AVRM_writeFileLogV2("[ActivateCCTALKPort()] CCTALK Open Port opened successfully.",INFO,g_BCAId);
					  
					  return 1; 


		 }//else if end


}//int ActivateCCTALKPort(int portNmbr) end

int DeActivateCCTALKPort() {


           int rtcode=-1;

           pthread_mutex_lock( &g_cctalkporthandlemutex);
	       rtcode = close(g_cctalkhandler); 
           pthread_mutex_unlock( &g_cctalkporthandlemutex);

		   if( 0 ==rtcode ){

						  AVRM_writeFileLogV2("[DeActivateCCTALKPort()] Close CCTALK Port successed.",INFO,g_BCAId);

						  pthread_mutex_lock( &g_cctalkopenflagmutex );
				          g_CCTALK_OpenFlag=false;
						  pthread_mutex_unlock( &g_cctalkopenflagmutex );

						  pthread_mutex_lock( &g_cctalkporthandlemutex);
						  g_cctalkhandler = -1;
						  pthread_mutex_unlock( &g_cctalkporthandlemutex);

				          return 1;

		   }else {
                      ////writeFileLog("[DeActivateCCTALKPort()] Close CCTALK Port failed.");
                      AVRM_writeFileLogV2("[DeActivateCCTALKPort()] Close CCTALK Port failed.",INFO,g_BCAId);
					  return 0;

           }//else end


}//int DeActivateCCTALKPort() end

int GetCoinAcptrStatus(){
         return ( CoinAcceptorSimplePoll() );
}//int GetCoinAcptrStatus() end

int GetReplyFromCCTalkPortV2( unsigned char* const fncommand,unsigned char* const fnrecvpackt,unsigned int   fnCommandpacktLength,unsigned int*  const fnrecvlength,unsigned int   fnbytestoberead,int WaitBeforeReadReply,int const CcTalkReplyWaitTime) {

                        
									int rtcode =-1;

									pthread_mutex_lock( &g_cctalkporthandlemutex);
									
									//clear output and input buffer
									//++printf("\n [GetReplyFromCCTalkPort()] Clear CCTALK buffer.");
									FlushSerialPortOutBuff_C(g_cctalkhandler);
									FlushSerialPortInBuff_C(g_cctalkhandler);

									pthread_mutex_lock(&RecvPktLengthMutuex);
									g_rcvPktLen_C = fnbytestoberead;
									pthread_mutex_unlock(&RecvPktLengthMutuex);

									int counter=0;
									
									//for( counter=0;counter < fnrecvpacktLength;counter++){
										 //++printf("\n [GetReplyFromCCTalkPort()] fncommand[%d]: 0x%xh.",counter,fncommand[counter] );
									//}
									
									//++Print Data
									LogDataExchnageV2( INFO,"TKN","Tx",fncommand,fnCommandpacktLength);  
									
									cctalk_delay_mSec(50) ; //50 ms 
									
									/*for( counter=0;counter < fnCommandpacktLength;counter++){
										 SendSingleByteToSerialPort_C(g_cctalkhandler,fncommand[counter] );
									}//for end
									*/
									
									if(  -1 != write(g_cctalkhandler, fncommand, fnCommandpacktLength )  )
									{
										AVRM_writeFileLogV2("[GetReplyFromCCTalkPortV2()]  Write Command Success.",INFO,g_BCAId);
									}else{
										AVRM_writeFileLogV2("[GetReplyFromCCTalkPortV2()]  Write Command  Failed.",INFO,g_BCAId);
									}

									///////////////////////////////////////////////////////////////////////////////////////////////////////
									
									//Wait Before Read Reply Bytes
									cctalk_delay_mSec(WaitBeforeReadReply) ;

								   ///////////////////////////////////////////////////////////////////////////////////////////////////////

									//printf("\n Byte to be read: %d",fnbytestoberead );

									rtcode = MONEYCONTROL_CoinAcptr_statusRead(fnrecvpackt,fnrecvlength,CcTalkReplyWaitTime,"TKN");

									//printf("\n Recv Packet Length: %d",*fnrecvlength );

									pthread_mutex_unlock( &g_cctalkporthandlemutex);

									return rtcode;


}//function GetReplyFromCCTalkPortV2 end

int GetReplyFromCCTalkPort( unsigned char* const fncommand,unsigned char* const fnrecvpackt,unsigned int   fnCommandpacktLength,unsigned int*  const fnrecvlength,unsigned int   fnbytestoberead,int const CcTalkReplyWaitTime) {

                        
                        int rtcode =-1;

                        pthread_mutex_lock( &g_cctalkporthandlemutex);
                        
                        cctalk_delay_mSec(50) ; //50 ms 

                        //clear output and input buffer
                        //++printf("\n [GetReplyFromCCTalkPort()] Clear CCTALK buffer.");
                        FlushSerialPortOutBuff_C(g_cctalkhandler);
                        FlushSerialPortInBuff_C(g_cctalkhandler);

						pthread_mutex_lock(&RecvPktLengthMutuex);
						g_rcvPktLen_C = fnbytestoberead;
						pthread_mutex_unlock(&RecvPktLengthMutuex);

                        int counter=0;
                        
                        //for( counter=0;counter < fnrecvpacktLength;counter++){
							//++printf("\n [GetReplyFromCCTalkPort()] fncommand[%d]: 0x%xh.",counter,fncommand[counter] );
						//}
						
						//++Print Data
						LogDataExchnageV2( INFO,"TKN","Tx",fncommand,fnCommandpacktLength);  
						
						/*for( counter=0;counter < fnCommandpacktLength;counter++)
						{
							 SendSingleByteToSerialPort_C(g_cctalkhandler,fncommand[counter] );
						}*/
						
						if(  -1 != write(g_cctalkhandler, fncommand, fnCommandpacktLength )  )
						{
							AVRM_writeFileLogV2("[GetReplyFromCCTalkPort()]  Write Command Success.",INFO,g_BCAId);
						}else{
							AVRM_writeFileLogV2("[GetReplyFromCCTalkPort()]  Write Command  Failed.",INFO,g_BCAId);
						}
						
                        //printf("\n Byte to be read: %d",fnbytestoberead );
                        rtcode = MONEYCONTROL_CoinAcptr_statusRead(fnrecvpackt,fnrecvlength,CcTalkReplyWaitTime,"TKN");

                        //printf("\n Recv Packet Length: %d",*fnrecvlength );

                        pthread_mutex_unlock( &g_cctalkporthandlemutex);

                        return rtcode;


}//function GetReplyFromCCTalkPort end

int CoinAcceptorSimplePoll() {


         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 

         memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 

		 unsigned int  rcvPktLen=0;

		 int ret=0;

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //malay add 29 march 2013
         ClearCCTALKBuffer();

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////

         cctalk_delay_mSec(100);

         pthread_mutex_lock( &g_cctalkporthandlemutex);

         MONEYCONTROL_CoinAcptr_Cmd_Poll();

	     ret = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId);

         pthread_mutex_unlock( &g_cctalkporthandlemutex);

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
         //#ifdef COINACCPTEOR_DEBUG

         unsigned char log[100];

         int Counter=0;

         ////writeFileLog("=========================================================================");

         //memset(log,'\0',100); 

         //sprintf(log,"[CoinAcceptorSimplePoll()] Recv Packet Length=%d .",rcvPktLen);

         ////writeFileLog(log);
         
         //for(Counter;Counter<rcvPktLen;Counter++){


			 //memset(log,'\0',100);
	 
			 //sprintf(log,"[CoinAcceptorSimplePoll()] rcvPkt[%d] = 0x%x h.",Counter,rcvPkt[Counter]);

			 ////writeFileLog(log);

         //}
         

         ////writeFileLog("=========================================================================");

         //#endif

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////

         if( (10 == rcvPktLen) && ( 0x01 == rcvPkt[5]) && (0x00 == rcvPkt[6]) && (0x02 == rcvPkt[7]) &&(0x00 == rcvPkt[8]) && (0xFD == rcvPkt[9] ) ){

               ////writeFileLog("[CoinAcceptorSimplePoll()] Coin Acceptor Simple Poll Success.");
               AVRM_writeFileLogV2("[CoinAcceptorSimplePoll()] Coin Acceptor Simple Poll Success.",INFO,g_BCAId);
               return 0; //++Success

         }else{

               ////writeFileLog("[CoinAcceptorSimplePoll()] Coin Acceptor Simple Poll Failed.");
               AVRM_writeFileLogV2("[CoinAcceptorSimplePoll()] Coin Acceptor Simple Poll Failed.",ERROR,g_BCAId);
               return 1; //++Failed

         }//else end
       
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////


}//int CoinAcceptorSimplePoll() end

//++2= already closed
//++3= failed to disable coin acptr
//++1= successfully closed
//++0= failed to closed note acptr 

int CloseCoinAcptr() {
          
     
           pthread_mutex_lock( &g_cctalkopenflagmutex );
  
           //check port already closed or not
           if( false == g_CCTALK_OpenFlag )
           {
                  pthread_mutex_unlock( &g_cctalkopenflagmutex );
                  ////writeFileLog("[OpenCoinAcptr()] Error CCTALK Port not opened !!!.");
                  AVRM_writeFileLogV2("[OpenCoinAcptr()] Error CCTALK Port not opened !!!.",ERROR,g_BCAId);
                  return 2;

           }

           pthread_mutex_unlock( &g_cctalkopenflagmutex );

           //Disable all coin acceptance before close operation in coin acceptor[safety]
	   return( DisableAllCoins() );
	  
}//int CloseCoinAcptr() end

//++Disable All coins
int DisableAllCoins() {


	  unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT];
	  memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 
	  int  rcvPktLen=0,rtcode =-1;
	  coinType COIN;
	  COIN.rs_50  = 0;
	  COIN.rs_1   = 0;
	  COIN.rs_2   = 0;
	  COIN.rs_5   = 0;
	  COIN.rs_10  = 0;
          
      AVRM_writeFileLogV2("[DisableAllCoins()] Going to disable all Coin ..",INFO,g_BCAId);
      
	  pthread_mutex_lock( &g_cctalkporthandlemutex);
	  
	  MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 
	  
	  rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId);
	  
	  pthread_mutex_unlock( &g_cctalkporthandlemutex);
	  
	  if( SUCCESS == rtcode )
	  {      
		     if((rcvPkt[3] != 0xE7) || (rcvPkt[11] !=0xFD)) 
		     {
		          AVRM_writeFileLogV2("[DisableAllCoins()] failed to disable all Coin  ..",INFO,g_BCAId);
		          return FAIL;
		     }
		     else
		     {
		         AVRM_writeFileLogV2("[DisableAllCoins()] successfully disable all Coin .",INFO,g_BCAId);
		         return SUCCESS;
		     }

	  } else{ 
		  AVRM_writeFileLogV2("[DisableAllCoins()] MONEYCONTROL_CoinAcptr_statusRead() return failed .",INFO,g_BCAId);
		  return FAIL;
      }//else end
  
}//end

//Coin Mask Values
//0 0 0 0 0 0 0 1  : 0x01 : Rs 1
//0 0 0 0 0 0 1 0  : 0x02 : Rs 2
//0 0 0 0 0 1 0 0  : 0x04 : Rs 5
//0 0 0 0 1 0 0 0  : 0x08 : Rs 10

//++Run time coin inhibit 
int EnableSpecificCoins(int currentfare) {

			unsigned char log[100];
			memset(log,'\0',100);
			unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 
			memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);
			int  rcvPktLen=0,rtcode=-1;
			coinType COIN;
			//These field always fixed
			COIN.rs_50  = 0;
			COIN.rs_2   = 0;
			//These field always change
			COIN.rs_1   = 0;
			COIN.rs_5   = 0;
			COIN.rs_10  = 0;

			  /*
			  if(currentfare > 10)
			  {
				 #ifdef COINACCPTEOR_DEBUG
					 //writeFileLog("[EnableSpecificCoins()]  Greater than 10.");
					 #endif
					 //COIN.rs_1   = 1;
					 COIN.rs_5   = 1;
					 COIN.rs_10  = 1;
			  }
			  //Range : 1-10
			  else if(10==currentfare)
			  {
				 #ifdef COINACCPTEOR_DEBUG
					 //writeFileLog("[EnableSpecificCoins()]  Equal to 10.");
					 #endif
					 //COIN.rs_1   = 1;
				 COIN.rs_5   = 1;
				 COIN.rs_10  = 1;
			  }
			  //Range: 5,6,7,8,9
			  else if (currentfare < 10 && currentfare >=5 )
			  {
				 #ifdef COINACCPTEOR_DEBUG
					 //writeFileLog("[EnableSpecificCoins()]  less than 10 but greter or equal to 5.");
					 #endif
					 //COIN.rs_1   = 1;
				 COIN.rs_5   = 1;
				 COIN.rs_10  = 0;
			  } 
			  //Range: 1,2,3,4
			  else if (currentfare < 5 && currentfare >=1 )
			  {
				 #ifdef COINACCPTEOR_DEBUG
					 //writeFileLog("[EnableSpecificCoins()]  less than 5 but greter or equal to 1.");
					 #endif
					 //COIN.rs_1   = 1;
				 COIN.rs_5   = 0;
				 COIN.rs_10  = 0;
			  } 
			  else if(0==currentfare)
				  {
				  #ifdef COINACCPTEOR_DEBUG
					  //writeFileLog("[EnableSpecificCoins()] MONEYCONTROL_CoinAcptr_statusRead() Zero fare receieved.");
					  #endif
					  return FAIL;
				  }
				  
				  #ifdef COINACCPTEOR_DEBUG
				  //writeFileLog("[EnableSpecificCoins()] Going to enable Specific Coin ");
				  #endif
				  
				  ///////////////////////////////////////////////////////////////////////////////////

				  pthread_mutex_lock( &g_cctalkporthandlemutex);

				  MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 

				  rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

				  pthread_mutex_unlock( &g_cctalkporthandlemutex);
				  */
          
          
            unsigned char DenomMaskValue=0x00;
          
            pthread_mutex_lock(&g_DenomMaskValuemutex);
          
            DenomMaskValue = (unsigned char)g_DenomMaskValue ;
            
            memset(log,'\0',100);
        
			sprintf(log,"[EnableSpecificCoins()] Coin Denom Mask : %d",DenomMaskValue); 
        
			AVRM_writeFileLogV2(log,INFO,g_BCAId);
          
            pthread_mutex_unlock(&g_DenomMaskValuemutex); 
            
            struct coinType CurrentCoin;
            
            CurrentCoin.rs_50    = DISABLE;
            CurrentCoin.rs_1     = DISABLE;
            CurrentCoin.rs_2     = DISABLE;
            CurrentCoin.rs_5     = DISABLE;
            CurrentCoin.rs_10    = DISABLE;
            
		    if( 0x01 == (DenomMaskValue&0x01) ) {
				CurrentCoin.rs_1      = ENABLE;
				AVRM_writeFileLogV2("[EnableSpecificCoins()] Coin Rs.5 Enable",INFO,g_BCAId);
		    }//if end
			   
			if( 0x02 == (DenomMaskValue&0x02) ) {
				CurrentCoin.rs_2      = ENABLE;
			    AVRM_writeFileLogV2("[EnableSpecificCoins()] Coin Rs.10 Enable",INFO,g_BCAId);
			}//if end
				
			if( 0x04 == (DenomMaskValue&0x04) ) {
			    CurrentCoin.rs_5      = ENABLE;
				AVRM_writeFileLogV2("[EnableSpecificCoins()] Coin Rs.5 Enable",INFO,g_BCAId);
			}//if end
			   
		    if( 0x08 == (DenomMaskValue&0x08) ) {
				CurrentCoin.rs_10      = ENABLE;
				AVRM_writeFileLogV2("[EnableSpecificCoins()] Coin Rs.10 Enable",INFO,g_BCAId);
		    }//if end
		    
		    /*
		    if(true == g_CAExactFareFlag ) {
					
						   AVRM_writeFileLogV2("[EnableSpecificCoins()]  Exact Fare Block Active.",INFO,g_BCAId);

						   switch(currentfare) {
						   
								   case 1:
								      AVRM_writeFileLogV2("[EnableSpecificCoins() Exact Fare Block] Only Rs 1 Enable.",INFO,g_BCAId);
								      CurrentCoin.rs_1      = ENABLE;
								      break;
					
								   case 2:
									 AVRM_writeFileLogV2("[EnableSpecificCoins() Exact Fare Block] Only Rs 2 Enable.",INFO,g_BCAId);
									 CurrentCoin.rs_2      = ENABLE;
									 break;
								
								   case 5:
									  AVRM_writeFileLogV2("[EnableSpecificCoins() Exact Fare Block] Only Rs 5 Enable.",INFO,g_BCAId);
									  CurrentCoin.rs_5      = ENABLE;
									  break;	
								  
								   case 10:
									  AVRM_writeFileLogV2("[EnableSpecificCoins() Exact Fare Block] Only Rs 5 Enable.",INFO,g_BCAId);
									  CurrentCoin.rs_10      = ENABLE;
									  break;		  

						  };
					
			}else {
				
								AVRM_writeFileLogV2("[EnableSpecificCoins()]  Exact Fare Block InActive.",INFO,g_BCAId);
								
								if( currentfare>=1 ){
									AVRM_writeFileLogV2("[EnableSpecificCoins() Max Cash Block] Rs 1 Enable.",INFO,g_BCAId);
									CurrentCoin.rs_1      = ENABLE;
								}//if end

								if( currentfare>=2 ) {
									AVRM_writeFileLogV2("[EnableSpecificCoins() Max Cash Block] Rs 2 Enable.",INFO,g_BCAId);
									CurrentCoin.rs_2      = ENABLE;
								}//if end

								if( currentfare>=5 ){
									 AVRM_writeFileLogV2("[EnableSpecificCoins() Max Cash Block] Rs 5 Enable.",INFO,g_BCAId);
									 CurrentCoin.rs_5      = ENABLE;
								}//if end

								if( currentfare>=10 ){
									 AVRM_writeFileLogV2("[EnableSpecificCoins() Max Cash Block] Rs 10 Enable.",INFO,g_BCAId);
									 CurrentCoin.rs_10      = ENABLE;
								}//if end

			}//++ else end
			*/
			
			pthread_mutex_lock( &g_cctalkporthandlemutex);

			MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(CurrentCoin); 

		    rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId);

			pthread_mutex_unlock( &g_cctalkporthandlemutex);
        
			if( SUCCESS ==  rtcode ) 
			{
			       
				 if((rcvPkt[3] != 0xE7) || (rcvPkt[11] !=0xFD)) 
				 {
				 
					  //#ifdef COINACCPTEOR_DEBUG
					  AVRM_writeFileLogV2("[EnableSpecificCoins()] Specific Coin enable failed.",ERROR,g_BCAId);
					  //#endif
					  return FAIL;
				 }
				 else
				 {
					  //#ifdef COINACCPTEOR_DEBUG
					  AVRM_writeFileLogV2("[EnableSpecificCoins()] Successfully Specific Coin Enable.",INFO,g_BCAId);
					  //#endif
					  return SUCCESS;
				 }//else end


	        }
	        else 
	        {
	           //#ifdef COINACCPTEOR_DEBUG
		       AVRM_writeFileLogV2("[EnableSpecificCoins()] MONEYCONTROL_CoinAcptr_statusRead() return failed",ERROR,g_BCAId);
		       //#endif
		       return FAIL;
            }//else end

}//int EnableSpecificCoins(int currentfare) end

//++Run time coin inhibit 
int DisableSpecificCoins(int currentfare) {

			    unsigned char log[100];
			    memset(log,'\0',100);
			    unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 
			    memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);
			    int  rcvPktLen=0,rtcode=-1;
			    coinType COIN;
			  
			    //These field always fixed
			    COIN.rs_50  = 0;
			    COIN.rs_2   = 0;
			    //These field always change
			    COIN.rs_1   = 0;
			    COIN.rs_5   = 0;
			    COIN.rs_10  = 0;

			    memset(log,'\0',100);
			    sprintf(log,"[DisableSpecificCoins()] Current fare : Rs. %d.", currentfare);  
			    AVRM_writeFileLogV2(log,INFO,g_BCAId);
			   
				unsigned char DenomMaskValue=0x00;
			  
				pthread_mutex_lock(&g_DenomMaskValuemutex);
			  
				DenomMaskValue = (unsigned char)g_DenomMaskValue ;
				
				memset(log,'\0',100);
			
				sprintf(log,"[DisableSpecificCoins()] Coin Denom Mask : %d",DenomMaskValue); 
			
				AVRM_writeFileLogV2(log,INFO,g_BCAId);
			  
				pthread_mutex_unlock(&g_DenomMaskValuemutex); 
				
				struct coinType CurrentCoin;
			
				CurrentCoin.rs_50    = DISABLE;
				CurrentCoin.rs_1     = DISABLE;
				CurrentCoin.rs_2     = DISABLE;
				CurrentCoin.rs_5     = DISABLE;
				CurrentCoin.rs_10    = DISABLE;
				
				if( 0x01 == (DenomMaskValue&0x01) ) {
				   CurrentCoin.rs_1      = ENABLE;
				   AVRM_writeFileLogV2("[DisableSpecificCoins()] Coin Rs.5 Enable",INFO,g_BCAId);
					 
				}//if end
			   
				if( 0x02 == (DenomMaskValue&0x02) ) {
				   CurrentCoin.rs_2      = ENABLE;
				   AVRM_writeFileLogV2("[DisableSpecificCoins()] Coin Rs.10 Enable",INFO,g_BCAId);
				}//if end
				
				if( 0x04 == (DenomMaskValue&0x04) ) {
			       CurrentCoin.rs_5      = ENABLE;
				   AVRM_writeFileLogV2("[DisableSpecificCoins()] Coin Rs.5 Enable",INFO,g_BCAId);
					 
				}//if end
			   
				if( 0x08 == (DenomMaskValue&0x08) ) {
				   CurrentCoin.rs_10      = ENABLE;
				   AVRM_writeFileLogV2("[DisableSpecificCoins()] Coin Rs.10 Enable",INFO,g_BCAId);
				}//if end
				
				/*
				if(true == g_CAExactFareFlag ) {
					
						   AVRM_writeFileLogV2("[DisableSpecificCoins()]  Exact Fare Block Active.",INFO,g_BCAId);

						   switch(currentfare) {
						   
								   case 1:
								      AVRM_writeFileLogV2("[DisableSpecificCoins() Exact Fare Block] Only Rs 1 Enable.",INFO,g_BCAId);
								      CurrentCoin.rs_1      = ENABLE;
								      break;
					
								   case 2:
									 AVRM_writeFileLogV2("[DisableSpecificCoins() Exact Fare Block] Only Rs 2 Enable.",INFO,g_BCAId);
									 CurrentCoin.rs_2      = ENABLE;
									 break;
								
								   case 5:
									  AVRM_writeFileLogV2("[DisableSpecificCoins() Exact Fare Block] Only Rs 5 Enable.",INFO,g_BCAId);
									  CurrentCoin.rs_5      = ENABLE;
									  break;	
								  
								   case 10:
									  AVRM_writeFileLogV2("[DisableSpecificCoins() Exact Fare Block] Only Rs 5 Enable.",INFO,g_BCAId);
									  CurrentCoin.rs_10      = ENABLE;
									  break;		  

						  };
					
				}else {
					
									AVRM_writeFileLogV2("[DisableSpecificCoins()]  Exact Fare Block InActive.",INFO,g_BCAId);
									
									if( currentfare>=1 ){
										AVRM_writeFileLogV2("[DisableSpecificCoins() Max Cash Block] Rs 1 Enable.",INFO,g_BCAId);
										CurrentCoin.rs_1      = ENABLE;
									}//if end

									if( currentfare>=2 ) {
										AVRM_writeFileLogV2("[DisableSpecificCoins() Max Cash Block] Rs 2 Enable.",INFO,g_BCAId);
										CurrentCoin.rs_2      = ENABLE;
									}//if end

									if( currentfare>=5 ){
										 AVRM_writeFileLogV2("[DisableSpecificCoins() Max Cash Block] Rs 5 Enable.",INFO,g_BCAId);
										 CurrentCoin.rs_5      = ENABLE;
									}//if end

									if( currentfare>=10 ){
										 AVRM_writeFileLogV2("[DisableSpecificCoins() Max Cash Block] Rs 10 Enable.",INFO,g_BCAId);
										 CurrentCoin.rs_10      = ENABLE;
									}//if end

				}//++ else end
				*/
				
			    pthread_mutex_lock( &g_cctalkporthandlemutex);

				MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(CurrentCoin); 

				rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId);

				pthread_mutex_unlock( &g_cctalkporthandlemutex);
			
			    if(  SUCCESS ==  rtcode )
			    {
						
					 if((rcvPkt[3] != 0xE7) || (rcvPkt[11] !=0xFD)) 
					 {
							//#ifdef  COINACCPTEOR_DEBUG
							AVRM_writeFileLogV2("[DisableSpecificCoins()] Specific Coin Disable failed.",ERROR,g_BCAId);
							//#endif
							return FAIL;
					 }
					 else
					 {
							//#ifdef  COINACCPTEOR_DEBUG
							AVRM_writeFileLogV2("[DisableSpecificCoins()] Successfully Specific Coin Disable.",INFO,g_BCAId);
							//#endif
							return SUCCESS;
					 }//else end

			    }
			    else 
			    {
						 //#ifdef  COINACCPTEOR_DEBUG
						 AVRM_writeFileLogV2("[DisableSpecificCoins()] MONEYCONTROL_CoinAcptr_statusRead() return failed.",ERROR,g_BCAId);
						 //#endif 
						 return FAIL;
			    }//else end

			 
}//++int DisableSpecificCoins(int currentfare)  end

//++set All Coin Inhibit status to true
void SetAllCoinInhibitStatus() {

         
	   pthread_mutex_lock(&CoinInhibitFlagmutex);

	   g_CoinInhibitFlag=1;

	   pthread_mutex_unlock(&CoinInhibitFlagmutex);

       return;


}//end

//++set All Coin Inhibit status to true
void SetSpecificCoinInhibitStatus(int fare) {

        unsigned char log[100];
        memset(log,'\0',100);
	    pthread_mutex_lock(&CASetSpecificFaremutex);
	    g_SpecificCoinInhibitFlag=1;
	    g_CACurrentAtvmFare=fare;
	    AVRM_writeFileLogV2("[SetSpecificCoinInhibitStatus()] Enable Specific Coin inhibit.",INFO,g_BCAId);
	    memset(log,'\0',100);
        sprintf(log,"[SetSpecificCoinInhibitStatus()] Fare To Be Accepted : %d",fare); 
        AVRM_writeFileLogV2(log,INFO,g_BCAId);
        //char log[100];
        //memset(log,'\0',100);
        //sprintf(log,"[SetSpecificCoinInhibitStatus()] fare: Rs. %d .",g_CACurrentAtvmFare); 
        //AVRM_writeFileLogV2(log,INFO,g_BCAId);
        pthread_mutex_unlock(&CASetSpecificFaremutex);
        return;

}//void SetSpecificCoinInhibitStatus(int fare)  end

//++Get Low Level Poll Flag Status
unsigned int GetCoinAcptrLowLevelFlagStatus() {


	    int CoinLowLevelPollFlagStatus=0;

	    pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

	    CoinLowLevelPollFlagStatus=g_CoinLowLevelPollFlagStatus;

	    pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

	    return (CoinLowLevelPollFlagStatus);



}

unsigned int WaitforCoinAcptrLowlevelflagClosed(double waittime) {
	             
	             struct timespec begints={0}, endts={0},diffts={0};
		         int rtcode=-1;
                 #ifdef  COINACCPTEOR_DEBUG
                 AVRM_writeFileLogV2("[WaitforCoinAcptrLowlevelflagClosed()]  Get Low level flag status starting..",INFO,g_BCAId);
                 #endif
                 unsigned char log[100];
		         memset(log,'\0',100);
                 pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

				 //++if polling flag is on condition (on=1)
				 if(1==g_CoinLowLevelPollFlagStatus) {
				 
		               pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
		               clock_gettime(CLOCK_MONOTONIC, &begints);
					   //++Wait here untill polling flag is off (off=0)
					   for(;;) {
						   	
								clock_gettime(CLOCK_MONOTONIC, &endts);
								diffts.tv_sec = endts.tv_sec - begints.tv_sec;
								//++if timer is running
								if(diffts.tv_sec<waittime) {
															
											pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);
											if(0==g_CoinLowLevelPollFlagStatus) {
												  pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
												  #ifdef  COINACCPTEOR_DEBUG
												  AVRM_writeFileLogV2("[WaitforCoinAcptrLowlevelflagClosed()] Low level flag status is closed.",INFO,g_BCAId);
												  memset(log,'\0',100);
												  sprintf(log,"[WaitforCoinAcptrLowlevelflagClosed()]  Seconds : %d.",diffts.tv_sec);
												  AVRM_writeFileLogV2(log,INFO,g_BCAId);
												  #endif
												  return 1;
											}else{
												  pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
											}//else end

								}
								//++if timer time is end or timer is running
								if(diffts.tv_sec>=waittime) {
										
										pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);
										if( 0==g_CoinLowLevelPollFlagStatus) {
												pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
												AVRM_writeFileLogV2("[WaitforCoinAcptrLowlevelflagClosed()] Low level flag status is closed",INFO,g_BCAId);		                                          
												memset(log,'\0',100); 
												sprintf(log,"[WaitforCoinAcptrLowlevelflagClosed()]  Seconds : %d.",diffts.tv_sec);
												AVRM_writeFileLogV2(log,INFO,g_BCAId);	
												return;
										}else { 

												 pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
												 //#ifdef  COINACCPTEOR_DEBUG
												 AVRM_writeFileLogV2("[WaitforCoinAcptrLowlevelflagClosed()] Low level flag status is not closed during time delay wait.",WARN,g_BCAId);	
												 memset(log,'\0',100); 
												 sprintf(log,"[WaitforCoinAcptrLowlevelflagClosed()]  Seconds : %d.",diffts.tv_sec);
												 AVRM_writeFileLogV2(log,INFO,g_BCAId);
												 //#endif
												 return 0;
											   
										}//else end

							

							 }//timer if block end here
					      
			          }//++for loop end
		   
		         }//++if thread status start end here

                 pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
                 pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);
                 if(0==g_CoinLowLevelPollFlagStatus) {
				       pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
				       //#ifdef  COINACCPTEOR_DEBUG
                       AVRM_writeFileLogV2("[WaitforCoinAcptrLowlevelflagClosed()] Already Low level flag status is closed.",INFO,g_BCAId);
                       //#endif
		               return 1;
		         }//if end

                 pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

}//unsigned int WaitforCoinAcptrLowlevelflagClosed(double waittime) end

void ClearCCTALKBuffer(){
	  
	  pthread_mutex_lock( &g_cctalkporthandlemutex);
	  FlushSerialPortOutBuff_C(g_cctalkhandler);  
	  FlushSerialPortInBuff_C(g_cctalkhandler);  
	  pthread_mutex_unlock( &g_cctalkporthandlemutex);
	  return;

}

int enableSpecificCoinsAndMakeIdle(int fare,int maxCoinQtyAllowed){

	  
          unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT];
          memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);  
	  unsigned int  rcvPktLen=0;
          int rtcode=-1;
          //Clear flag
          g_CoinInhibitFlag = 0;        
	  g_SpecificCoinInhibitFlag=0;
          g_NaCoinInhibitFlag =0;

	       
	  //Coin struct
	  coinType COIN;
	  COIN.rs_50  = 0; //by default disable alltime
	  COIN.rs_1   = 0;
	  COIN.rs_2   = 0; //by default disable alltime
	  COIN.rs_5   = 0;
	  COIN.rs_10  = 0;
	  
          //#ifdef  COINACCPTEOR_DEBUG

          //Make a log about fare and max no of cash
          char log[200];
          memset(log,'\0',200);
          sprintf(log,"[enableSpecificCoinsAndMakeIdle()] Max No of Coin = %d And fare = %d",maxCoinQtyAllowed,fare);       
          ////writeFileLog(log);
          AVRM_writeFileLogV2(log,INFO,g_BCAId);

          //#endif

	 //Analysis if exact fare is given
         #ifdef  COINACCPTEOR_DEBUG
         ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Before Exact fare analysis block.");
         AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Before Exact fare analysis block.",INFO,g_BCAId);
         #endif

	 if( 1 == maxCoinQtyAllowed )
	 {

		   //#ifdef  COINACCPTEOR_DEBUG
                   ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Exact fare block Active [Coin:1].");
                   AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Exact fare block Active [Coin:1].",INFO,g_BCAId);
                   //#endif

                   switch(fare)
		   {
			   
			     case 1:
			          //COIN.rs_1   = ENABLE;
                                  //#ifdef  COINACCPTEOR_DEBUG
                                  ////writeFileLog("[enableSpecificCoinsAndMakeIdle()  Exact fare block] Rs. 1 only enable.");
                                  //#endif
			          break;
                          
			     case 5:
			          COIN.rs_5   = ENABLE;
                                  //#ifdef  COINACCPTEOR_DEBUG
                                  ////writeFileLog("[enableSpecificCoinsAndMakeIdle() Exact fare block] Rs. 5 only enable.");
                                  AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle() Exact fare block] Rs. 5 only enable.",INFO,g_BCAId);
                                  //#endif
			          break;

			     case 10:
			          COIN.rs_10  = ENABLE;
                                  //#ifdef  COINACCPTEOR_DEBUG
                                  ////writeFileLog("[enableSpecificCoinsAndMakeIdle() Exact fare block] Rs. 10 only enable.");
                                  AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle() Exact fare block] Rs. 10 only enable.",INFO,g_BCAId);
                                  //#endif
			          break;

			     default: ////writeFileLog("[enableSpecificCoinsAndMakeIdle() Exact fare block] No Exact fare given.");
			              AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle() Exact fare block] No Exact fare given.",INFO,g_BCAId);
                                      break;

	      } 
	   }

           else if(  maxCoinQtyAllowed >1  )//Exact fare not given
           {
                  

                  //#ifdef  COINACCPTEOR_DEBUG
                  ////writeFileLog("[enableSpecificBillsAndMakeIdle()] Max Coin Block Active [Coin:20].");
                  AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Max Coin Block Active [Coin:20].",INFO,g_BCAId);
                  //#endif
                  
                  if(fare >= 1)
                  {
                        //COIN.rs_1     = ENABLE;
                        //#ifdef  COINACCPTEOR_DEBUG
                        ////writeFileLog("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 1 Enable.");
                        //#endif
                  }
                 
                  if(fare >= 5)
                  {
                        COIN.rs_5     = ENABLE;
                        //#ifdef  COINACCPTEOR_DEBUG
                        ////writeFileLog("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 5 Enable.");
                        AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 5 Enable.",INFO,g_BCAId);
                        //#endif
                  }
                  if(fare >= 10)
                  {
                        COIN.rs_10    = ENABLE;
                        
                        //#ifdef  COINACCPTEOR_DEBUG
                        ////writeFileLog("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 10 Enable.");
                        AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 10 Enable.",INFO,g_BCAId);
                        //#endif
                  }
                
           }
           
           #ifdef  COINACCPTEOR_DEBUG
	   ////writeFileLog("[enableSpecificCoinsAndMakeIdle() ] After Exact fare analysis block.");
           #endif


          ////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &g_cctalkporthandlemutex);

          //Now enable coin status

          rtcode=-1;

	  MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 

	  rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId); 

          if( SUCCESS == rtcode  )
	  {   
               ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Enable coin success.");
               AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Enable coin success.",INFO,g_BCAId);
          }
          else
          {
               ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Enable coin failed.");
               AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Enable coin failed.",ERROR,g_BCAId);

          }


          pthread_mutex_unlock( &g_cctalkporthandlemutex);


          /////////////////////////////////////////////////////////////////////////////////
	  

           //Malay Add 29 March 2013
	      cctalk_delay_mSec(100); 
          
          //To Hold the Last Event Nmbr. & To Status Chk of acptr.

          //#ifdef  COINACCPTEOR_DEBUG
          ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Get last event counter block.");
          AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Get last event counter block.",INFO,g_BCAId);
          //#endif

          int CommandReplyLength=0;

          ////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &g_cctalkporthandlemutex);

	      MONEYCONTROL_CoinAcptr_Cmd_CreditPoll();  

          rtcode=-1;

          rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId);

          pthread_mutex_unlock( &g_cctalkporthandlemutex);   

          ////////////////////////////////////////////////////////////////////////////////
     
		  if( SUCCESS ==  rtcode )
		  {   

                    /////////////////////////////////////////////////////

                    pthread_mutex_lock(&RecvPktLengthMutuex);

                    CommandReplyLength=g_rcvPktLen_C;

                    pthread_mutex_unlock(&RecvPktLengthMutuex);

                    ///////////////////////////////////////////////////////

                    pthread_mutex_lock(&PrevEventMutuex);

                    if( ( 0xE5 == rcvPkt[3] ) && (rcvPktLen == CommandReplyLength) )
		            {

                              g_prevEvent_C = rcvPkt[9];

                              pthread_mutex_unlock(&PrevEventMutuex);

                              //#ifdef  COINACCPTEOR_DEBUG
                              ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Store last event counter success.");
                              AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Store last event counter success.",INFO,g_BCAId);
                              //#endif
                              
                              memset(log,'\0',200);

							  sprintf(log,"[enableSpecificCoinsAndMakeIdle()] Event Counter = %d .", g_prevEvent_C );

							  ////writeFileLog(log);  
							  
							  AVRM_writeFileLogV2(log,INFO,g_BCAId);
			   
							  return(SUCCESS);

		            }
                    else
                    {
                             pthread_mutex_unlock(&PrevEventMutuex);

                             ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Credit Poll Command Reply Packet mismatch error.");
                             AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Credit Poll Command Reply Packet mismatch error.",ERROR,g_BCAId);
                             
                             return(FAIL);

                    }


	  }
          else
          {
		       //#ifdef  COINACCPTEOR_DEBUG
		       ////writeFileLog("[enableSpecificCoinsAndMakeIdle()] Store last event counter failed.");
		       AVRM_writeFileLogV2("[enableSpecificCoinsAndMakeIdle()] Store last event counter failed.",ERROR,g_BCAId);
		       //#endif
		       return(FAIL);

          }//else end


}

//++Start Coin Acceptor Credit Polling Thread
unsigned int MONEYCONTROL_CoinAcptrCreateCreditPoll(){

	  pthread_t thread1;

	  int  iret1;   

	  int returnVal=-1;

	  pthread_attr_t attr;

          //Intilize thread attribute
	  returnVal = pthread_attr_init(&attr);
          
          if(0!=returnVal)
          {
               return 0;
 
          }
          
          //Set thread attribute as detached so thats system can cleanup thread after thread exit
          returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
          
          if(0!=returnVal)
          return 0;

          //create thread
	  iret1     = pthread_create( &thread1, 
		                      &attr,
		                      ThreadProc1_MONEYCONTROL_CoinAcptrPoll,
		                      NULL
		                    );
          if(0!=returnVal)
          {
              return 0;
          }
          
          //Destroy thread attribute
          returnVal = pthread_attr_destroy(&attr);

          ////////////////////////////////////////////////////////////////////////////////////

          //Now Start Coin Acceptor Credit Polling

          pthread_mutex_lock(&CoinPollFlagMutex);
          g_poll_flag_C = ON; 
          pthread_mutex_unlock(&CoinPollFlagMutex);

          pthread_mutex_lock(&CAEscrowFlagStatusmutex);
          g_state_C=0x00;
          pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

          ////////////////////////////////////////////////////////////////////////////////////

          return 1;


}

void  CoinAcptrStartSignalCreditPollThread() {

          ////writeFileLog("[CoinAcptrStartSignalCreditPollThread()] Entry.");
          
          AVRM_writeFileLogV2("[CoinAcptrStartSignalCreditPollThread()] Entry.",TRACE,g_BCAId);

          ////////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

          g_CloseCoinCreditPollingThreadFlag =0;
 
          pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

          ////////////////////////////////////////////////////////////////////////////////////

 
          pthread_mutex_lock( &g_CAEnableThreadmutex );

          pthread_cond_signal( &g_CAEnableThreadCond );

          pthread_mutex_unlock( &g_CAEnableThreadmutex );

          ////////////////////////////////////////////////////////////////////////////////////

         
          AVRM_writeFileLogV2("[CoinAcptrStartSignalCreditPollThread()] Exit.",TRACE,g_BCAId);

          return;


}

void  CoinAcptrStopSignalCreditPollThread(){

         ////writeFileLog("[CoinAcptrStopSignalCreditPollThread()] Entry.");

         ////////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

          g_CloseCoinCreditPollingThreadFlag =1;
 
          pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

          ////////////////////////////////////////////////////////////////////////////////////
 
          pthread_mutex_lock( &g_CAEnableThreadmutex );

          pthread_cond_signal( &g_CAEnableThreadCond );

          pthread_mutex_unlock( &g_CAEnableThreadmutex );

          ////////////////////////////////////////////////////////////////////////////////////

          ////writeFileLog("[CoinAcptrStopSignalCreditPollThread()] Exit.");

          return;

}// void  CoinAcptrStopSignalCreditPollThread() end

//++Set All Coin Inhibit status to true
void NaCoinEnableSet(int fare){

    pthread_mutex_lock(&NaCoinInhibitFlagmutex);

    g_NaCoinInhibitFlag=1;

    g_Nafaretobeaccept=fare;  

    pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

    return;

}// void NaCoinEnableSet(int fare) end

//++Delay or Disable all coin before doing poll
static  void CoinIntelligentInhibit(){


           int CACurrentAtvmFare=0;

           pthread_mutex_lock(&CoinInhibitFlagmutex);

		   if( 1==g_CoinInhibitFlag ) {

					  g_CoinInhibitFlag = 0;  

                      pthread_mutex_unlock(&CoinInhibitFlagmutex);

                      AVRM_writeFileLogV2("[CoinIntelligentInhibit()] Before set disable coin acceptance.",TRACE,g_BCAId);

					  if (SUCCESS == DisableAllCoins() ) {
									  
							pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );

							g_CADisableAcceptanceFlag =true ;

							pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );

					  }//if end 

				      AVRM_writeFileLogV2("[CoinIntelligentInhibit()] After set disable coin acceptance.",INFO,g_BCAId);

					  return;

		   }//if end

           pthread_mutex_unlock(&CoinInhibitFlagmutex);

           pthread_mutex_lock(&CASpecificInhibitmutex);

		   if( 1==g_SpecificCoinInhibitFlag )  {
			   
		           AVRM_writeFileLogV2("[CoinIntelligentInhibit()] Going to Enable Specific Coin.",INFO,g_BCAId);

				   g_SpecificCoinInhibitFlag=0;

				   CACurrentAtvmFare = g_CACurrentAtvmFare;

				   pthread_mutex_unlock(&CASpecificInhibitmutex); 

				   DisableSpecificCoins(CACurrentAtvmFare);
				   
				   return;

		   }//if end

           pthread_mutex_unlock(&CASpecificInhibitmutex); 

           pthread_mutex_lock(&NaCoinInhibitFlagmutex);

           if( 1==g_NaCoinInhibitFlag ) {  
		   
					 g_NaCoinInhibitFlag=0;     

					 CACurrentAtvmFare=g_Nafaretobeaccept;

					 pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

					 EnableSpecificCoins(CACurrentAtvmFare);

					 return;

		   }//++if end

	       pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

           return;


}//static  void CoinIntelligentInhibit() end

bool GetCADisableAcceptanceSignal(){

       bool rtcode=false;
       pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );
       rtcode = g_CADisableAcceptanceFlag ;
       pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );
       return rtcode;
       
}//bool GetCADisableAcceptanceSignal() end

//++Delay Between two consecutive coin acceptor credit polling command 
static  void  CoinIntelligentDelay(unsigned int delay){

      struct timespec req = {0};
      req.tv_sec = 0;
      req.tv_nsec = 10* 1000000L; //10ms delay
      int CACurrentAtvmFare=0;
      for(;delay>=1; delay--) {

                   pthread_mutex_lock(&CoinInhibitFlagmutex);

				   if(1==g_CoinInhibitFlag) {
				              g_CoinInhibitFlag = 0; 
			                  pthread_mutex_unlock(&CoinInhibitFlagmutex);
							  AVRM_writeFileLogV2("[CoinIntelligentDelay()] Before set disable coin acceptance.",INFO,g_BCAId);
							  if (SUCCESS == DisableAllCoins() ) {
							        pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );
							        g_CADisableAcceptanceFlag =true ;
							        pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );
							  }//if end
							  AVRM_writeFileLogV2("[CoinIntelligentDelay()] After set disable coin acceptance.",INFO,g_BCAId);
							  return;

				   }//if end

                   pthread_mutex_unlock(&CoinInhibitFlagmutex);
                   pthread_mutex_lock(&CASpecificInhibitmutex);

				   if( 1 == g_SpecificCoinInhibitFlag )  {
				         
				         g_SpecificCoinInhibitFlag=0;
						 CACurrentAtvmFare = g_CACurrentAtvmFare;
						 pthread_mutex_unlock(&CASpecificInhibitmutex);
						 DisableSpecificCoins(CACurrentAtvmFare);
						 clock_nanosleep( CLOCK_MONOTONIC,0,&req,NULL);
						 continue;

				   }//if end

                   pthread_mutex_unlock(&CASpecificInhibitmutex);
                   pthread_mutex_lock(&NaCoinInhibitFlagmutex);
                   if(1==g_NaCoinInhibitFlag)  {
					     g_NaCoinInhibitFlag=0;  
                         pthread_mutex_unlock(&NaCoinInhibitFlagmutex);    
                         EnableSpecificCoins(g_Nafaretobeaccept);
                         clock_nanosleep(CLOCK_MONOTONIC,0,&req,NULL );
				         continue;
				   }//if end

				   pthread_mutex_unlock(&NaCoinInhibitFlagmutex);
				   clock_nanosleep( CLOCK_MONOTONIC,0,&req,NULL);
							
       }//++for end
      return;
       
}//end

//++Malay 8 april 2013
void GetCoinDetailInEscrow (int value[], int *coinStatus){

           char log[100];

           memset(log,'\0',100);

		   int i=0,amnt[5];

           memset(amnt,0,5);

           memset(value,0,5);
            
           //init status
		   *coinStatus  = 0x00; 
 
           pthread_mutex_lock(&CAEscrowFlagStatusmutex);

           if( COINACPTR_RES_ESCROW == g_state_C ){ 

                       pthread_mutex_lock(&GlobalRecvpacketMutuex);

                       MONEYCONTROL_CoinAcptr_coinAmountReceived(g_rcvPkt_C,amnt);     

                       pthread_mutex_unlock(&GlobalRecvpacketMutuex);
  
					   for(i = 0;i<5;i++){
                          value[i] = 0; 
                       }//for end

					   i = 0;

					   while(g_nmbrOfCoinsAt_aTime>0) {
					     
								  value[i] =  amnt[i];

								  memset(log,'\0',100);

								  sprintf(log,"[GetCoinDetailInEscrow() Escrow] value[%d] : %d .", i,value[i]);

								  AVRM_writeFileLogV2(log,INFO,g_BCAId);

								  i++; 

								  g_nmbrOfCoinsAt_aTime = g_nmbrOfCoinsAt_aTime-1;   
										  
									   
					   }//while end

                       //set escrow state
                       *coinStatus =   g_state_C;

                       //clear escrow state for again poll
	                   g_state_C=0x00;

                       pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

                       return;

	       } else {   
              pthread_mutex_unlock(&CAEscrowFlagStatusmutex);
           }//else end

           //++AVRM_writeFileLogV2("[GetCoinDetailInEscrow()] After Check Escrow State.",INFO,g_BCAId);
	       return;   
 
	   
}//GetCoinDetailInEscrow() end

void  CloseCoinCreditPollingThread(){


            AVRM_writeFileLogV2("[CloseCoinCreditPollingThread()] Entry .",TRACE,g_BCAId);

            AVRM_writeFileLogV2("[CloseCoinCreditPollingThread()] Before Poll Flag Close .",INFO,g_BCAId);

			pthread_mutex_lock(&CoinPollFlagMutex);

			g_poll_flag_C = OFF;

			pthread_mutex_unlock(&CoinPollFlagMutex);

            AVRM_writeFileLogV2("[CloseCoinCreditPollingThread()] After Poll Flag Close .",INFO,g_BCAId);

            AVRM_writeFileLogV2("[CloseCoinCreditPollingThread()] Before Signal Close Poll Thared .",INFO,g_BCAId);
 
            pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

			g_CloseCoinCreditPollingThreadFlag=1;

			pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

            AVRM_writeFileLogV2("[CloseCoinCreditPollingThread()] After Signal Close Poll Thared .",INFO,g_BCAId);

            AVRM_writeFileLogV2("[CloseCoinCreditPollingThread()] Exit .",TRACE,g_BCAId);

	        return;


}//CloseCoinCreditPollingThread() end

void enableCAExactFareFlag(){
	  AVRM_Currency_writeFileLog("[enableCAExactFareFlag()] Single Coin Enable",INFO);
      g_CAExactFareFlag=true;
}//void enableExactFareFlag() end

void disableCAExactFareFlag(){
	 AVRM_Currency_writeFileLog("[disableCAExactFareFlag()] Single Coin Disabled",INFO);
     g_CAExactFareFlag=false;
}//void enableExactFareFlag() end

static pthread_mutex_t g_RejectEventFlagMutex= PTHREAD_MUTEX_INITIALIZER;
static bool g_ExactFare_RejectEvent;
bool getCARejectEventFlagStatus(){
       
       //AVRM_writeFileLogV2("[getCARejectEventFlagStatus()] Entry.",INFO,g_BCAId);
       bool rejectEventFlag=false;
       pthread_mutex_lock(&g_RejectEventFlagMutex);
       rejectEventFlag = g_ExactFare_RejectEvent ;
       pthread_mutex_unlock(&g_RejectEventFlagMutex);	
       //AVRM_writeFileLogV2("[getCARejectEventFlagStatus()] Exit.",INFO,g_BCAId);
       return rejectEventFlag;
       
}//bool getCARejectEventFlagStatus() end

void setCARejectEventFlagStatus(bool fnrejectEventFlag){
       
       AVRM_writeFileLogV2("[setCARejectEventFlagStatus()] Entry.",INFO,g_BCAId);
       pthread_mutex_lock(&g_RejectEventFlagMutex);
       g_ExactFare_RejectEvent = fnrejectEventFlag;
       pthread_mutex_unlock(&g_RejectEventFlagMutex);	
       AVRM_writeFileLogV2("[setCARejectEventFlagStatus()] Exit.",INFO,g_BCAId);
       
}//bool setCARejectEventFlagStatus(fnrejectEventFlag) end

void InitCATransModel() {

      AVRM_writeFileLogV2("[InitCATransModel()] Entry.",TRACE,g_BCAId);
      pthread_mutex_lock(&CoinInhibitFlagmutex);
      g_CoinInhibitFlag = 0;
      pthread_mutex_unlock(&CoinInhibitFlagmutex);
      pthread_mutex_lock(&CASpecificInhibitmutex);
      g_CACurrentAtvmFare=0;
      g_SpecificCoinInhibitFlag=0;
      pthread_mutex_unlock(&CASpecificInhibitmutex);
      pthread_mutex_lock(&NaCoinInhibitFlagmutex);
      g_NaCoinInhibitFlag=0;
      pthread_mutex_unlock(&NaCoinInhibitFlagmutex);
      pthread_mutex_lock(&NaCoinInhibitFlagmutex);
      g_Nafaretobeaccept=0;
      pthread_mutex_unlock(&NaCoinInhibitFlagmutex);
      pthread_mutex_lock(&CAEscrowFlagStatusmutex);
      g_state_C = 0x00;
      pthread_mutex_unlock(&CAEscrowFlagStatusmutex);
      pthread_mutex_lock(&CoinPollFlagMutex);
      g_poll_flag_C = OFF; 
      pthread_mutex_unlock(&CoinPollFlagMutex);
      pthread_mutex_lock(&GlobalRecvpacketMutuex);
      int i=0;
      for(i = 0; i<COINACPTR_MAX_LEN_OF_PKT; i++){
		  g_rcvPkt_C[i] = 0;
	  } //for end
	  pthread_mutex_unlock(&GlobalRecvpacketMutuex);
	  pthread_mutex_lock(&RecvPktLengthMutuex);
	  g_rcvPktLen_C=0;
	  pthread_mutex_unlock(&RecvPktLengthMutuex);
	  //++Disable Exact fare Block
	  disableCAExactFareFlag();
	  bool fnrejectEventFlag = false;
	  //++Disable Reject Flag status to false
	  setCARejectEventFlagStatus( fnrejectEventFlag);
      AVRM_writeFileLogV2("[InitCATransModel()] Exit.",TRACE,g_BCAId);
      return;


}//void InitCATransModel() end

//++Coin Acceptor Credit Polling Thread
static void* ThreadProc1_MONEYCONTROL_CoinAcptrPoll(void *ptr){   

           AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Before Wait Signal.",INFO,g_BCAId);           

           //++wait for signal
           pthread_mutex_lock( &g_CAEnableThreadmutex );

           pthread_cond_wait( &g_CAEnableThreadCond  , &g_CAEnableThreadmutex  );

           pthread_mutex_unlock( &g_CAEnableThreadmutex );

           AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] After Wait Signal.",INFO,g_BCAId);
             
           pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);
           
           bool fnrejectEventFlag = false;
            
           //++if credit poll flag is set exit from thread
           if( 1 == g_CloseCoinCreditPollingThreadFlag ) 
           {
                     
                    g_CloseCoinCreditPollingThreadFlag=0;
                    pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);
                    AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Thread Exit.",INFO,g_BCAId);
				    pthread_exit(0);
	       }
	       else
	       {
			        pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);
                    AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Thread Running.",INFO,g_BCAId);
          }//else end
 
          unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 

          memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 

	      int  rcvPktLen=0,state=0,amnt=0;

          char log[100];

          memset(log,'\0',100);

          g_AlreadyCoinPollFlag=false;

          int rtcode=-1;

          pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );

          g_CADisableAcceptanceFlag = false;

          pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );

		  for(;;) {

		             CoinIntelligentInhibit(); 
		             pthread_mutex_lock(&CoinPollFlagMutex);
		             pthread_mutex_lock(&CAEscrowFlagStatusmutex);
		             if( ( ON == g_poll_flag_C ) &&  ( COINACPTR_RES_ESCROW != g_state_C ) ){
						 
                                //++AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Credit Poll On.",INFO,g_BCAId);
                                
                                pthread_mutex_unlock(&CoinPollFlagMutex);
                                pthread_mutex_unlock(&CAEscrowFlagStatusmutex);
                                
                                g_AlreadyCoinPollFlag=false;
                                
                                //++Delay Between continous poll
                                CoinIntelligentDelay(COIN_ACPTR_POLL_DELAY);
                          
                                pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

                                //++Set Coin Low Level flag is running state
                                g_CoinLowLevelPollFlagStatus=1;

                                pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
                                
                                //++clear command reply array 
                                memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);
         
								/////////////////////////////////////////////////////////////////////////////////////////////////////////////
								
                                pthread_mutex_lock( &g_cctalkporthandlemutex);
                                
                                rtcode =-1;
                                
                                MONEYCONTROL_CoinAcptr_Cmd_CreditPoll(); 
                                
                                rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId);

                                pthread_mutex_unlock( &g_cctalkporthandlemutex);
                                
                                /////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                if(  SUCCESS ==  rtcode ) {
				                					            
										   //++Copy into Global var
										   CoinAcptrCopyReceivedPacketGlobally(rcvPkt,rcvPktLen);
		                                  
		                                   if( ( 0x01 == rcvPkt[5] )  && ( 0xB == rcvPkt[6] ) && ( 0x02 == rcvPkt[7] )  && ( 0x00 == rcvPkt[8] ) ) 
		                                   {
		                                     
						                                      pthread_mutex_lock(&PrevEventMutuex);

															  //++compare with previouse event counter with current event counter
															  if(rcvPkt[9]  != g_prevEvent_C) {
																	 
																		//g_lostEvent = 0; 

																		g_nmbrOfCoinsAt_aTime = 0; 
																		
																		//#ifdef  COINACCPTEOR_DEBUG

																		memset(log,'\0',100);
																		sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Previous Event : %d .", g_prevEvent_C);
																		AVRM_writeFileLogV2(log,INFO,g_BCAId);

																		memset(log,'\0',100);
																		sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Current Event  : %d .", rcvPkt[9]);
																		AVRM_writeFileLogV2(log,INFO,g_BCAId);
																		
																		memset(log,'\0',100);
																		sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Current  Credit Code: %d .", rcvPkt[10]);
																		AVRM_writeFileLogV2(log,INFO,g_BCAId);

																		//#endif
																		
																		//++check number of coins in credit polling
																		//++current event greater than prev event
																		if( (rcvPkt[9] > g_prevEvent_C) && ((rcvPkt[9]-g_prevEvent_C) == 2) ) {
																		
																			 AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss event 2 (Current Event Greater than prev event block).",WARN,g_BCAId);
																			 g_nmbrOfCoinsAt_aTime=2;
																			 memset(log,'\0',100);
																			 sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss Event Credit Code: %d", rcvPkt[12]);
																			 AVRM_writeFileLogV2(log,WARN,g_BCAId);

																		}else if( (rcvPkt[9] > g_prevEvent_C) && ( 0 == rcvPkt[10] ) ) {
																			  
																			  AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Found Reject Event .",INFO,g_BCAId);
																			  
																			  if( true == g_CAExactFareFlag ) {
																				  
																				  pthread_mutex_unlock(&PrevEventMutuex);
																				  
																				  AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Before Enable Reject Event .",INFO,g_BCAId);
																				  fnrejectEventFlag = true;
																				  setCARejectEventFlagStatus( fnrejectEventFlag );
																				  AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] After Enable Reject Event .",INFO,g_BCAId);
																				  AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Before Disable Coin Reject Event .",INFO,g_BCAId);
																				  
																				  //++Inhibit Coin
																				  pthread_mutex_lock(&CoinInhibitFlagmutex);
																				  g_CoinInhibitFlag =1; 
																				  pthread_mutex_unlock(&CoinInhibitFlagmutex);
																				  
                                                                                  //++Clear Escrow State
                                                                                  pthread_mutex_lock(&CAEscrowFlagStatusmutex);
                                                                                  g_poll_flag_C = OFF;
		                                                                          pthread_mutex_unlock(&CAEscrowFlagStatusmutex);
                                                                                  
																				  AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] After Disable Coin Reject Event .",INFO,g_BCAId);
																				  
																				  continue; //++Again Run Loop
																				  
																			  }//if end
																	
																		}else if( (rcvPkt[9] < g_prevEvent_C ) && ( ( (255-g_prevEvent_C) + rcvPkt[9] ) == 2 ) ) { //++prev event greater than current event
																		
																		      AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss Event 2 (Current Event less than prev event block) .",WARN,g_BCAId);
																			  g_nmbrOfCoinsAt_aTime=2;
																			  memset(log,'\0',100);
																			  sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss Event Credit Code: %d.", rcvPkt[12]);
																			  AVRM_writeFileLogV2(log,WARN,g_BCAId);

																		}else {
																		    AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Found Coin in Credit Poll.",INFO,g_BCAId);
																			g_nmbrOfCoinsAt_aTime=1;
																		}//else end
		                                                       
		                                                                memset(log,'\0',100);
                                                                        sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Number of Coins in single poll: %d.", g_nmbrOfCoinsAt_aTime);
		                                                                AVRM_writeFileLogV2(log,INFO,g_BCAId);
		                                                                
		                                                                //++store current event counter
		                                                                g_prevEvent_C = rcvPkt[9];  
								                                        pthread_mutex_unlock(&PrevEventMutuex);
                                                                        
                                                                        pthread_mutex_lock(&CAEscrowFlagStatusmutex);
                                                                        //++set coin acceptor escrow state and store coin information
		                                                                g_state_C = COINACPTR_RES_ESCROW;
		                                                                pthread_mutex_unlock(&CAEscrowFlagStatusmutex);
		                                                          
		                                                      }else { //event compare if block
                                                                pthread_mutex_unlock(&PrevEventMutuex);
                                                              }//else end                                                              


					                       }else { //reply packet not ok
                                                AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Coin Credit Reply Packet mismatch error!!!.",ERROR,g_BCAId);
                                           }//else end  
                                                                        
		                        } else { //++Read Communication Error
                                       
                                         //++Clear Escrow Values
                                         //pthread_mutex_lock(&CAEscrowFlagStatusmutex);
                                         //g_state_C = 0x00;
                                         //pthread_mutex_unlock(&CAEscrowFlagStatusmutex);
                                         
                                         AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Coin Credit Reply Packet Read Error.",ERROR,g_BCAId);
                                         
                                         pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

										 //if credit poll flag is set exit from thread
										 if( 1== g_CloseCoinCreditPollingThreadFlag ) 
										 {
										             g_CloseCoinCreditPollingThreadFlag=0;
						                             //++Set Coin low level flag state is closed
                                                     pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);
                                                     g_CoinLowLevelPollFlagStatus=0; 
                                                     pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
                                                     pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);
                                                     AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Thread Exit.",INFO,g_BCAId);
											         pthread_exit(0);
				                         } 
				                         else 
				                         {
                                            pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);
                                         }//else end

                              }//else end
		      
		             } else { //if Credit Poll flag is off
		                
		                                 pthread_mutex_unlock(&CoinPollFlagMutex);
		                                 pthread_mutex_unlock(&CAEscrowFlagStatusmutex);
                                         
                                         pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);
                                         //++Set Coin low level flag state is closed
										 g_CoinLowLevelPollFlagStatus=0; 
										 if( ( 0 == g_CoinLowLevelPollFlagStatus ) && ( false == g_AlreadyCoinPollFlag ) ) 
										 {
		                                      pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
                                              AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Credit Poll Off.",INFO,g_BCAId);
										      g_AlreadyCoinPollFlag=true;
										 } 
										 else 
										 {
                                             pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
                                         }//else end

                                         pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);
                                         //if credit poll flag is set exit from thread
										 if( 1== g_CloseCoinCreditPollingThreadFlag ) 
										 {
										         
										         g_CloseCoinCreditPollingThreadFlag=0;
						                         
						                         //++Set Coin low level flag state is closed
                                                 pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);
                                                 g_CoinLowLevelPollFlagStatus=0; 
                                                 pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);
                                                 
                                                 pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);
                                                 
                                                 AVRM_writeFileLogV2("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Thread Exit.",INFO,g_BCAId);
											     pthread_exit(0);
											     
				                         } 
				                         else 
				                         {
                                                pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);
                                         }//++else end
		                                
			      }//++else end
		      
	   }//++End for(;;)

       //++Default Thread Exit
       pthread_exit(0); 

}//End function 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static void CoinAcptrdisplayReceivedPacket(const unsigned char* const rcvPkt,const int  rcvPktLen) {

	  int i;  
	  for(i = 0; i<rcvPktLen; i++)
	  {
		  

	  } 

 
}//static void CoinAcptrdisplayReceivedPacket(const unsigned char* const rcvPkt,const int  rcvPktLen)  end
*/

static void CoinAcptrCopyReceivedPacketGlobally(const unsigned char* const rcvPkt,const unsigned int  rcvPktLen){


          ////writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] Entry.");

	      int i=0;  

          ////////////////////////////////////////////////////////////////

          ////writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] Before Recv Packet Length.");

          pthread_mutex_lock(&RecvPktLengthMutuex);

          g_rcvPktLen_C=0;

          g_rcvPktLen_C = rcvPktLen;

          pthread_mutex_unlock(&RecvPktLengthMutuex);

          ////writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] After Recv Packet Length.");

          ////////////////////////////////////////////////////////////////

          ////writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] Before Global Reply Packet Set.");

          pthread_mutex_lock(&GlobalRecvpacketMutuex);

          for(i = 0; i< COINACPTR_MAX_LEN_OF_PKT ; i++)
	  {

	     g_rcvPkt_C[i] = 0;

	  } 

          for(i = 0; i<rcvPktLen; i++)
	  {

	     g_rcvPkt_C[i] = rcvPkt[i];

	  } 

          pthread_mutex_unlock(&GlobalRecvpacketMutuex);

          ////writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] After Global Reply Packet Set.");

          ////////////////////////////////////////////////////////////////

          ////writeFileLog("[CoinAcptrCopyReceivedPacketGlobally] Exit.");

          return;
 

}

static void MONEYCONTROL_CoinAcptr_coinAmountReceived(const unsigned char* const rcvPkt,int* const  amnt)    {

  	int i_flag=0,j_count=0,i=0,denom=0,j=0;

        int count=0;

        for(count=0;count<5;count++)
	{

            amnt[count]=0;

        }
    
        for(i=10; i<=18; i+=2)
	{
		denom = rcvPkt[i];
		i_flag=0;
		for(j_count=0; j_count<8; j_count++)
		{
			if(denom == g_CoinTable[j_count].CreditCode)
			{
				amnt[j] = g_CoinTable[j_count].Credit;
				i_flag=1;
				break;
			}
			if(i_flag)
				break;
		}
		j++;
	}

        return;

}

//++Milliseconds delay [value must be 0-900]
static void cctalk_delay_mSec(const int milisec) {

        unsigned char log[100];

        memset(log,'\0',100);

        struct timespec req = {0},rim={0};

	req.tv_sec = 0;

	req.tv_nsec = milisec * 1000000L;

        /*

        int rtcode =0;

        rtcode = nanosleep(&req, &rim);
        
        if( rtcode < 0 )   
	{
	      
              memset(log,'\0',100);
              sprintf(log,"[cctalk_delay_mSec()] nanosleep() system call failed with return code  %d .",rtcode); 
              //writeFileLog(log);
              return;
	      

	}
        else
        {
              memset(log,'\0',100);
              sprintf(log,"[cctalk_delay_mSec()] nanosleep() system call success with return code  %d .",rtcode); 
              //writeFileLog(log);
              return;

        }

        */

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int rtcode=-1;
       
        rtcode=clock_nanosleep( CLOCK_MONOTONIC,
                                0,
                                &req,
                                NULL
                              );
        

        if(rtcode<0)
        {

			memset(log,'\0',100);
					   
			sprintf(log,"[delay_mSec()] clock_nanosleep failed with return code %d .",rtcode); 

			////writeFileLog(log);
			
			AVRM_writeFileLogV2(log,INFO,g_BCAId);


        }


}

static  void MONEYCONTROL_CoinAcptr_Cmd_inhibitStat(){
	
        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 7;
        pthread_mutex_unlock(&RecvPktLengthMutuex);
        
        pthread_mutex_lock(&CmdPktLengthMutuex);
        g_cmdPktLen_C = 5;
        pthread_mutex_unlock(&CmdPktLengthMutuex);
        
        //++Write Exchange Data
        char Command[5]={0x02,0x00,0x01,0xE6,0xEA};
        LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCA","Tx", Command, 5);
        
        //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = true;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
        //++Clear Buffer
        FlushSerialPortOutBuff_C(g_cctalkhandler);
        FlushSerialPortInBuff_C(g_cctalkhandler);
        
		SendSingleByteToSerialPort_C(g_cctalkhandler,0x02);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xE6); // command
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xEA); // checksum
	    
	    //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = false;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
	    return;

}//static  void MONEYCONTROL_CoinAcptr_Cmd_inhibitStat() end

static  void MONEYCONTROL_CoinAcptr_Cmd_CreditPoll(){

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 21;
        pthread_mutex_unlock(&RecvPktLengthMutuex);
        
        pthread_mutex_lock(&CmdPktLengthMutuex);
        g_cmdPktLen_C = 5;
        pthread_mutex_unlock(&CmdPktLengthMutuex);
        
        //++Write Exchange Data
        char Command[5]={0x02,0x00,0x01,0xE5,0x18};
		LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCA","Tx", Command, 5);
		
		//++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = true;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
        //++Clear Buffer
        FlushSerialPortOutBuff_C(g_cctalkhandler);
        FlushSerialPortInBuff_C(g_cctalkhandler);
        
		SendSingleByteToSerialPort_C(g_cctalkhandler,0x02); 
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x00); 
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x01); 
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xE5); //command
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x18); //checksum
	    
	    //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = false;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
        return;

}//static  void MONEYCONTROL_CoinAcptr_Cmd_CreditPoll() end

static  void MONEYCONTROL_CoinAcptr_Cmd_Poll(){

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10;
        pthread_mutex_unlock(&RecvPktLengthMutuex);
        
        pthread_mutex_lock(&CmdPktLengthMutuex);
        g_cmdPktLen_C = 5;
        pthread_mutex_unlock(&CmdPktLengthMutuex);
        
        //++Write Exchange Data
        char Command[5]={0x02,0x00,0x01,0xFE,0xFF};
        LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCA","Tx", Command, 5);
        
        //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = true;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
        //++Clear Buffer
        FlushSerialPortOutBuff_C(g_cctalkhandler);
        FlushSerialPortInBuff_C(g_cctalkhandler);
        
		SendSingleByteToSerialPort_C(g_cctalkhandler,0x02); 
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x00); 
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x01); 
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xFE); //command
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xFF); //checksum
	    
	    //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = false;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
	    return;

}//static  void MONEYCONTROL_CoinAcptr_Cmd_Poll() end

static  void MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow(){
	
        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10; 
        pthread_mutex_unlock(&RecvPktLengthMutuex);
        
        pthread_mutex_lock(&CmdPktLengthMutuex);
        g_cmdPktLen_C = 5;
        pthread_mutex_unlock(&CmdPktLengthMutuex);
        
        //++Write Exchange Data
        char Command[5]={0x53,0x00,0x01,0xDF,0xCD};
        LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCA","Tx", Command, 5);
        
        //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = true;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
        /*
		SendSingleByteToSerialPort_C(g_cctalkhandler,0x53);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xDF);  //command
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xCD);  //checksum
	    */
	     int fnCommandpacktLength = 5;
	     if(  -1 != write(g_cctalkhandler, Command, fnCommandpacktLength )  )
		 {	
			 AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow()]  Write Command Success.",INFO,g_BCAId);
		  }else{
			 AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow()]  Write Command  Failed.",INFO,g_BCAId);
		  } 
	    
	    //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = false;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
	    
	    return;
 
}//static  void MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow() end

static void MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow(){

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10;
        pthread_mutex_unlock(&RecvPktLengthMutuex);
        
        pthread_mutex_lock(&CmdPktLengthMutuex);
        g_cmdPktLen_C = 5;
        pthread_mutex_unlock(&CmdPktLengthMutuex);
        
        //++Write Exchange Data
        char Command[5]={0x53,0x00,0x01,0xE0,0xCC};
        LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCA","Tx", Command, 5);
         
        //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = true;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
        /*
		SendSingleByteToSerialPort_C(g_cctalkhandler,0x53);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xE0); //command
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xCC); //chekcsum
	    */
	    
	    int fnCommandpacktLength = 5;
	     if(  -1 != write(g_cctalkhandler, Command, fnCommandpacktLength )  )
		 {	
			 AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow()]  Write Command Success.",INFO,g_BCAId);
		  }else{
			 AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow()]  Write Command  Failed.",INFO,g_BCAId);
		  } 
	    
	    //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = false;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
	    
	    return;

}//static void MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow() end

static void MONEYCONTROL_CoinAcptr_Cmd_PollFromEscrow(){

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10;
        pthread_mutex_unlock(&RecvPktLengthMutuex);
        
        pthread_mutex_lock(&CmdPktLengthMutuex);
        g_cmdPktLen_C = 5;
        pthread_mutex_unlock(&CmdPktLengthMutuex);
        
        char Command[5]={0x53,0x00,0x01,0xFE,0xAE};
        LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCA","Tx", Command, 5);
        
        //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = true;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
        
        //++Clear Buffer
        FlushSerialPortOutBuff_C(g_cctalkhandler);
        FlushSerialPortInBuff_C(g_cctalkhandler);
        
        SendSingleByteToSerialPort_C(g_cctalkhandler,0x53);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xFE); //command
	    SendSingleByteToSerialPort_C(g_cctalkhandler,0xAE); //chekcsum
	    
	    //++Set CCTalk Port Operation Flag Status
        pthread_mutex_lock(&g_CcTalkPortOperationFlag);
        g_CCTalkOperationFlag = false;
        pthread_mutex_unlock(&g_CcTalkPortOperationFlag);

        return;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Created By Malay  Data 13 Dec 2012
//++NRI Coin Acceptor Coin Table
/*
	    First InhibitByte:
	    ------------------------------------------------ 
	    Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0
	     x     x     x     x     x     x     x      x
	    ------------------------------------------------
	    Bit 0 =50 paisa enable if it 1 or disable if it 0
	    Bit 1 =50 paisa enable if it 1 or disable if it 0
	    Bit 2 =Rs1 (Type 1 year 1997) enable if it 1 or disable if it 0
	    Bit 3 =Rs1 (Type 2) enable if it 1 or disable if it 0
	    Bit 4 =Rs2 enable if it 1 or disable if it 0
	    Bit 5 =Rs5 (Type 1 year 2002) enable if it 1 or disable if it 0
	    Bit 6 =Rs5 (Type 2 year 2007) enable if it 1 or disable if it 0
	    Bit 7 =Rs5 (Type 3 year 2012 Golden) enable if it 1 or disable if it 0

	     Second InhibitByte:
	    ------------------------------------------------ 
	     Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0
	      x     x     x     x        x    x     x      x
	     ------------------------------------------------
	     Bit 0 =Rs 10 enable if it 1 or disable if it 0
	     other bits for Token so dont needed for this operation 
	     
	     static CoinTable g_CoinTable[8]={   .50,2,0b00000011,0b0,  //First InhibitByte
											  1,3,  0b00001100,0b0,  //First InhibitByte
											  1,4,  0b00001100,0b0,  //First InhibitByte
											  2,5,  0b00010000,0b0,  //First InhibitByte
											  5,6,  0b11100000,0b0,  //First InhibitByte
											  5,7,  0b11100000,0b0,  //First InhibitByte
											  5,8,  0b11100000,0b0,  //First InhibitByte
											  10,9, 0b0,0b11111110   //Second InhibitByte
										 };

*/

static void MONEYCONTROL_CoinAcptr_Cmd_InhibitAndRply(coinType COIN) {

	     
				unsigned int i=0;
				unsigned char coinEnableByte1=0x00;
				unsigned char coinEnableByte2=0x00;
				unsigned char sbytes[MAX_CMD_LEN];
				unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 
				memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 
				unsigned int  rcvPktLen=0;

				if(COIN.rs_50 == ENABLE)
				{
					coinEnableByte1 = coinEnableByte1 | g_CoinTable[0].FirstCreditByteEnableMask; 
				}
				  
				if(COIN.rs_1 == ENABLE)
				{   
					coinEnableByte1 = coinEnableByte1 | g_CoinTable[1].FirstCreditByteEnableMask; 
				}
				 
				if(COIN.rs_2 == ENABLE)
				{
					coinEnableByte1 = coinEnableByte1 | g_CoinTable[3].FirstCreditByteEnableMask;
				}
				   
				if(COIN.rs_5 == ENABLE)
				{
					coinEnableByte1 = coinEnableByte1 | g_CoinTable[4].FirstCreditByteEnableMask; 
				}
					
				if(COIN.rs_10 == ENABLE)
				{
				   coinEnableByte2 = coinEnableByte2 | g_CoinTable[7].SecondCreditByteEnableMask; 
				}
				   
				sbytes[0] = 0x02;
				sbytes[1] = 0x02;
				sbytes[2] = 0x01; 
				sbytes[3] = 0xE7;            //Command
				sbytes[4] = coinEnableByte1; //Coin Inhibit byte1
				sbytes[5] = coinEnableByte2; //Coin Inhibit byte2
				sbytes[6] = GetCheckSum(sbytes,6);

                ///////////////////////////////////////////////////////////////////////////////
                
                pthread_mutex_lock( &g_cctalkporthandlemutex);
                
                //++Set CCTalk Port Operation Flag Status
				pthread_mutex_lock(&g_CcTalkPortOperationFlag);
				g_CCTalkOperationFlag = true;
				pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
				
				pthread_mutex_lock(&CmdPktLengthMutuex);
				g_cmdPktLen_C = 7;
				pthread_mutex_unlock(&CmdPktLengthMutuex);
				
				//++Clear Buffer
				FlushSerialPortOutBuff_C(g_cctalkhandler);
				FlushSerialPortInBuff_C(g_cctalkhandler);
        

				for(i=0;i< 7; i++)
				{ 
                   SendSingleByteToSerialPort_C(g_cctalkhandler, sbytes[i]);
				} 
				
                pthread_mutex_lock(&RecvPktLengthMutuex);
				g_rcvPktLen_C = 12;
                pthread_mutex_unlock(&RecvPktLengthMutuex);
                
				MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCAId);
				
				//++Set CCTalk Port Operation Flag Status
				pthread_mutex_lock(&g_CcTalkPortOperationFlag);
				g_CCTalkOperationFlag = false;
				pthread_mutex_unlock(&g_CcTalkPortOperationFlag);


                pthread_mutex_unlock( &g_cctalkporthandlemutex);

                ////////////////////////////////////////////////////////////////////////////////

                return;

                 /*
				printf("\n Count: %d , data: %x  %x  %x  %x   %x %x  %x\
				   %x  %x  %x   %x   %x ",rcvPktLen,rcvPkt[0],
				   rcvPkt[1],rcvPkt[2],rcvPkt[3],rcvPkt[4],rcvPkt[5],
				   rcvPkt[6],rcvPkt[7],rcvPkt[8],rcvPkt[9],rcvPkt[10], 
				   rcvPkt[11]);
                */
	 
}//end

static void MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(coinType COIN) {


			 unsigned char sbytes[MAX_CMD_LEN]; 
			 unsigned int i=0;
			 unsigned char coinEnableByte2=0x00;
			 
			 unsigned char coinEnableByte1=0x00;
			 g_COIN.rs_50 = COIN.rs_50; 
			 g_COIN.rs_1  = COIN.rs_1; 
			 g_COIN.rs_2  = COIN.rs_2;
			 g_COIN.rs_5  = COIN.rs_5;
			 g_COIN.rs_10 = COIN.rs_10;
	   
             #ifdef NRI_FIRMWARE_12_03 

		      //Table 
			  /*
		  static CoinTable g_CoinTable[8]={ .50,2,0b00000011,0b0,  //First  Byte
									  1,3,  0b00001100,0b0,  //First  Byte
									  1,4,  0b00001100,0b0,  //First  Byte
									  2,5,  0b00010000,0b0,  //First  Byte
									  5,6,  0b11100000,0b0,  //First  Byte
									  5,7,  0b11100000,0b0,  //First  Byte
									  5,8,  0b11100000,0b0,  //First  Byte
									  10,9, 0b0,0b11111111   //Second Byte
									};
			

				*/

	        if( ENABLE == COIN.rs_50 )
            {
		            coinEnableByte1 = coinEnableByte1 | g_CoinTable[0].FirstCreditByteEnableMask;
                    //#ifdef  COINACCPTEOR_DEBUG 
		            ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] 0.50 paisa enable.");
		            AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] 0.50 paisa enable.",INFO,g_BCAId);
                    //#endif
            }
           
            if( ENABLE == COIN.rs_1 )
            {
		            coinEnableByte1 = coinEnableByte1 | g_CoinTable[1].FirstCreditByteEnableMask; 
                    #ifdef  COINACCPTEOR_DEBUG 
		            ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 1 enable.");
		            AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 1 enable.",INFO,g_BCAId);
                    #endif
            }
            
            if( ENABLE == COIN.rs_2 )
			{
				        coinEnableByte1 = coinEnableByte1 | g_CoinTable[3].FirstCreditByteEnableMask;
						#ifdef  COINACCPTEOR_DEBUG 
				        ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 2 enable."); 
				        AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 2 enable.",INFO,g_BCAId);
						#endif
			}
          
            if( ENABLE == COIN.rs_5 )
			{
				        coinEnableByte1 = coinEnableByte1 | g_CoinTable[4].FirstCreditByteEnableMask; 
						//++#ifdef  COINACCPTEOR_DEBUG 
				        ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 5 enable.");
				        AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 5 enable..",INFO,g_BCAId);
						//++#endif
			}
           
            if( ENABLE == COIN.rs_10 )
            {
		           coinEnableByte2 = coinEnableByte2 | g_CoinTable[7].SecondCreditByteEnableMask; 
                   //++#ifdef  COINACCPTEOR_DEBUG 
		           ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 10 enable.");
		           AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 10 enable..",INFO,g_BCAId);
                   //++#endif
	        }
          
           
            #endif

            
            /*
            static CoinTable g_CoinTable[8]={  0,0,0x0,0x0,     

                                  1,1,  0x1,0x0,   //First  Byte=0b00000001
                                  1,2,  0x1,0x0,   //First  Byte=0b00000001

                                  2,3,  0x02,0x0,  //First  Byte=0b00000010
id range maxnoofcoin.
AVRMAPI_<KESP81>LOG<2022>.<28May>.<15:16:19:1653731179215>[API]: [InitAcceptFaresModel()] Runti
                                  5,4,  0x04,0x0,  //First  Byte=0b00000100
                                  5,5,  0x04,0x0,  //First  Byte=0b00000100
                                  5,6,  0x04,0x0,  //First  Byte=0b00000100

                                  10,7, 0x8, 0x0   //First Byte=0b00001000
                                };
            */

            #ifdef NRI_FIRMWARE_12_06

	        if( ENABLE == COIN.rs_1 )
            {
		            coinEnableByte1 = coinEnableByte1 | g_CoinTable[1].FirstCreditByteEnableMask; 
                    //#ifdef  COINACCPTEOR_DEBUG 
		            ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 1 enable.");
		            AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 1 enable.",INFO,g_BCAId);
                    //#endif
            }
            
            if( ENABLE == COIN.rs_2 )
			{
				        coinEnableByte1 = coinEnableByte1 | g_CoinTable[3].FirstCreditByteEnableMask;
						//#ifdef  COINACCPTEOR_DEBUG 
				        ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 2 enable."); 
				        AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 2 enable.",INFO,g_BCAId);
						//#endif
			}
          
            if( ENABLE == COIN.rs_5 )
			{
				        coinEnableByte1 = coinEnableByte1 | g_CoinTable[4].FirstCreditByteEnableMask; 
						//#ifdef  COINACCPTEOR_DEBUG 
				        ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 5 enable.");
				        AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 5 enable.",INFO,g_BCAId);
						//#endif
			}
           
            if( ENABLE == COIN.rs_10 )
			{
			           coinEnableByte1 = coinEnableByte1 | g_CoinTable[7].FirstCreditByteEnableMask; 
					   //#ifdef  COINACCPTEOR_DEBUG 
			           ////writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 10 enable.");
			           AVRM_writeFileLogV2("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 10 enable.",INFO,g_BCAId);
					   //#endif
			}
          
            #endif

			sbytes[0] = 0x02;
			sbytes[1] = 0x02;
			sbytes[2] = 0x01; 
			sbytes[3] = 0xE7;            //command byte
			sbytes[4] = coinEnableByte1; //coin inhibit byte 1
			sbytes[5] = coinEnableByte2; //coin inhibit byte 2         
			sbytes[6] = GetCheckSum(sbytes,6);
			
			//Write Exchange Data
			/*
					LogDataExchnageV2( DATA_EXCHANGE_ALL,
					   "BCA", 
					   "Tx",
					   sbytes,
					   7
					);
             */
            
            
            //++Set CCTalk Port Operation Flag Status
			pthread_mutex_lock(&g_CcTalkPortOperationFlag);
			g_CCTalkOperationFlag = true;
			pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
			
			pthread_mutex_lock(&CmdPktLengthMutuex);
			g_cmdPktLen_C = 7;
			pthread_mutex_unlock(&CmdPktLengthMutuex);
			
			pthread_mutex_lock(&RecvPktLengthMutuex);
            g_rcvPktLen_C = 12;
            pthread_mutex_unlock(&RecvPktLengthMutuex);
			
			//++Clear Buffer
			FlushSerialPortOutBuff_C(g_cctalkhandler);
			FlushSerialPortInBuff_C(g_cctalkhandler);
        
            for(i=0;i< 7; i++)
	        {
               SendSingleByteToSerialPort_C(g_cctalkhandler, sbytes[i]);
            } //for end
            
            //++Set CCTalk Port Operation Flag Status
			pthread_mutex_lock(&g_CcTalkPortOperationFlag);
			g_CCTalkOperationFlag = false;
			pthread_mutex_unlock(&g_CcTalkPortOperationFlag);

            return;


}//MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes() end

//////////////////////////////////////////////////End :Coin Acceptor//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////Start :Coin Escrow ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef COIN_ESCROW

int cctalk_Wait(unsigned int Second){
		   

                  if(Second < 0)
                  {

                          ////writeFileLog("[cctalk_Wait()] Function parameter not ok.");
                          AVRM_writeFileLogV2("[cctalk_Wait()] Function parameter not ok.",INFO,g_BCAId);
		          return 0;

                  }
                  unsigned char log[100]; 
                  memset(log,'\0',100);
                  struct timespec begints, endts,diffts;;
                  clock_gettime(CLOCK_MONOTONIC, &begints);
		  while(1)
		  {


				  clock_gettime(CLOCK_MONOTONIC, &endts);

				  diffts.tv_sec = endts.tv_sec - begints.tv_sec;

				  #ifdef COINESCROW_DEBUG

		                  memset(log,'\0',100);
		           
		                  sprintf(log,"[cctalk_Wait()] Seconds Elapsed=%d",diffts.tv_sec); 

		                  ////writeFileLog(log);
		                  
		                  AVRM_Currency_writeFileLog(log,INFO,"API");

		                  #endif

		                  if(diffts.tv_sec>=Second)
		                  {

		                          memset(log,'\0',100);
				   
				          sprintf(log,"[cctalk_Wait()] Seconds Elapsed = %d ",diffts.tv_sec); 

				          ////writeFileLog(log);
				                   AVRM_writeFileLogV2(log,INFO,"NA");

                                          return 1;

		                  }


                 }


}

int PollFromCoinEscrow() {

		
                unsigned char * rcvPkt = (unsigned char *)malloc(COINACPTR_MAX_LEN_OF_PKT);
				if(rcvPkt == NULL)
				return FAIL;
				memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);
				unsigned int  rcvPktLen=0;
				int ret=0;
                int i=0;
                unsigned char log[100];
                memset(log,'\0',100);
                     
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_cctalkporthandlemutex);

				//++clear serial port transmit and rece buffer 
				FlushSerialPortOutBuff_C(g_cctalkhandler);
                FlushSerialPortInBuff_C(g_cctalkhandler);

                //++Send accept command to escrow device
				MONEYCONTROL_CoinAcptr_Cmd_PollFromEscrow();
                //++Then read command reply bytes
				ret =  MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME,g_BCEId);

                pthread_mutex_unlock( &g_cctalkporthandlemutex);
                
                LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCE", "Tx",rcvPkt,rcvPktLen);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                		
                if( ( 0x01 == rcvPkt[5] ) && ( 0x00 == rcvPkt[6] ) && ( 0x53 == rcvPkt[7] ) && ( 0x00 == rcvPkt[8] ) && ( 0xAC == rcvPkt[9] ) ) {
                        AVRM_writeFileLogV2("[PollFromCoinEscrow()] Coin Escrow poll succcessed.",INFO,g_BCEId);
						LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCE","Rx", rcvPkt, 10);
						free(rcvPkt);
						return 0; //++Success

				} else {
						AVRM_writeFileLogV2("[PollFromCoinEscrow()] Coin Escrow poll  failed.",ERROR,g_BCEId);
					    LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCE","Rx", rcvPkt, 10);
						free(rcvPkt);
						return 1; //++Failed

			  }//++else end
				

}//PollFromCoinEscrow() end here

int WaitBeforeCoinEscrowRead = 2000; //Milisecond
int CoinEscrowReadTimeout = 4; //Second

int AcceptFromCoinEscrow() {
	
				unsigned char * rcvPkt = (unsigned char *)malloc(COINACPTR_MAX_LEN_OF_PKT);
				if(rcvPkt == NULL)
				return FAIL;
				memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);
				unsigned int  rcvPktLen=0;
				int ret=0;
				int i=0;
				unsigned char log[100];
				memset(log,'\0',100);
							 
				pthread_mutex_lock( &g_cctalkporthandlemutex);

				//clear serial port transmit and rece buffer // wait for lst pkt to complete the recv/trnsmt
				FlushSerialPortOutBuff_C(g_cctalkhandler);
				FlushSerialPortInBuff_C(g_cctalkhandler);

				//Send accept command to escrow device
				MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow();

				//wait for solenoid work complete
				cctalk_delay_mSec(WaitBeforeCoinEscrowRead);

				//Then read command reply bytes
				ret =  MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CoinEscrowReadTimeout,g_BCAId);

                pthread_mutex_unlock( &g_cctalkporthandlemutex);

                /////////////////////////////////////////////////////////////////////////////////

		
                if(                       ( 0x01 == rcvPkt[5])                               && 
                                          ( 0x00 == rcvPkt[6] )                              && 
                                          ( 0x53 == rcvPkt[7] )                              &&
                                          ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )    && 
                                          ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                 )
				{


					
								AVRM_writeFileLogV2("[AcceptFromCoinEscrow()] Accept coin Successfully done.",INFO,g_BCEId);
								LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCE","Rx", rcvPkt, 10);
								free(rcvPkt);
								return SUCCESS;

				}else {
						
								AVRM_writeFileLogV2("[AcceptFromCoinEscrow()] Accept coin failed.",ERROR,g_BCEId);
								LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCE","Rx", rcvPkt, 10);
								free(rcvPkt);
								return FAIL;

				}//else end



}//AcceptFromCoinEscrow() end here

int RejectFromCoinEscrow() {

	
                     unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 
                     memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 
	                 unsigned int  rcvPktLen=0;
                     int ret=0;
                     unsigned char log[100];
                     memset(log,'\0',100);
                     int i=0;
                     
                     pthread_mutex_lock( &g_cctalkporthandlemutex);

                     //clear serial port transmit and recv buffer	
					 FlushSerialPortOutBuff_C(g_cctalkhandler); 
					 FlushSerialPortInBuff_C(g_cctalkhandler);   

                     AVRM_writeFileLogV2("[RejectFromCoinEscrow()] Before Issue Reject Command.",INFO,g_BCEId);

                     //Send reject command to escrow device
					 MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow();

                     AVRM_writeFileLogV2("[RejectFromCoinEscrow()] After Issue Reject Command.",INFO,g_BCEId);
                    
         
                     //wait for solenoid work complete
                     cctalk_delay_mSec(WaitBeforeCoinEscrowRead);

                     //Then read command reply bytes
					 ret = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CoinEscrowReadTimeout,g_BCEId);

                     pthread_mutex_unlock( &g_cctalkporthandlemutex);

		     
                     if(                       ( 0x01 == rcvPkt[5] )                             && 
                                               ( 0x00 == rcvPkt[6] )                             && 
                                               ( 0x53 == rcvPkt[7] )                             &&
                                               ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )   && 
                                               ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                       )
					 {

							 
							 AVRM_writeFileLogV2("[RejectFromCoinEscrow()] Reject Successfully done.",INFO,g_BCEId);
							 LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCE","Rx", rcvPkt, 10);
							 return SUCCESS; 

				 
					 }else{			 
						     AVRM_writeFileLogV2("[RejectFromCoinEscrow()] Reject Coin failed. ",ERROR,g_BCEId);
							 LogDataExchnageV2( DATA_EXCHANGE_ALL,"BCE","Rx", rcvPkt, 10);
							 return FAIL;  

					 } //else end

}//RejectFromCoinEscrow() end here

#endif

/////////////////////////////////////////////End :Coin Escrow ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char GetCheckSum(const unsigned char* const  bufData, const unsigned int sizeData) {

          /*

	   LOGIC: 1. Add bytes. 
                  2. Complement the byte. 
                  3. Add 1 with the complemented byte.
	
          */

	  char CHKSUM;
	  int i;
	  CHKSUM = 0;
          for(i=0; i < sizeData; i++)
	  {	 
	    CHKSUM += bufData[i];   
	  } 
	  CHKSUM = ~CHKSUM;    // Complement the byte.
	  CHKSUM = CHKSUM + 1; // Final Byte.
	  return CHKSUM;


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ATVM_ALARM_VAULT

//Coin Acceptor Unit General Commands Table
static AtvmAlarmCommands g_AtvmAlarmCommands[3]=  {  

		//0 Get Current Status                                                   
		0x55,0x04,0x01,0x80,0x00,0x00,0x00,0x00,0x00,

		//1 Set Disable Alarm
		0x55,0x04,0x01,0x40,0x00,0x00,0x00,0x00,0x00,

		//2 Ack Command
		0x02,0x01,0x06,0x00,0x00,0x00,0x00,0x00,0x00


};

static void Delay(const unsigned int MilliSeconds){

        unsigned char log[100];
        struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = MilliSeconds * 1000000L;
        //nanosleep(&req, (struct timespec *)NULL);
        int rtcode=-1;
        rtcode=clock_nanosleep( CLOCK_MONOTONIC,
                                0,
                                &req,
                                NULL
                              );
        
        if(rtcode<0)
        {

		memset(log,'\0',100);
				   
		sprintf(log,"[Delay()] clock_nanosleep failed with return code %d .",rtcode); 

		////writeFileLog(log);
		
		AVRM_writeFileLogV2(log,INFO,"NA");


        }


}//delay() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Getatvmstatus status command structure in ATVM Machine
//Command : Disable alarm
/*

These are command bytes:
Command[0]=0x55 h.
Command[1]=0x04 h.
Command[2]=0x01 h.
Command[3]=0x80 h.
Command[4]=0x00 h. 
Command[5]=0x00 h. 
Command[6]=0x00 h.  
Command[7]=0x00 h.  
Command[8]=0xXY h. //checksum value

These are reply bytes toatal 5 bytes:
Reply[17]=0x01 h.
Reply[18]=0x00 h.
Reply[19]=0x55 h.
Reply[20]=0x37 h. //Sample Status Byte Variable Value
Reply[21]=0x73 h. //Sample CheckSum Value

*/

//getstatus command 
static  void SendATVMGetStatusCommand() {


	            pthread_mutex_lock(&RecvPktLengthMutuex);

		        g_rcvPktLen_C = COMMANDREPLYLENGTH; //Return Byte : echo byte(9)+reply byte(5)

		        pthread_mutex_unlock(&RecvPktLengthMutuex);

	            int counter =0;
 
                char checksum =0x00;

                unsigned char log[100];

                memset(log,'\0',100);
               
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                int CommandLength = 9;
                char CommandPackt[CommandLength];
                memset(CommandPackt,'\0',CommandLength);
                
                CommandPackt[0] =  0x55;
                CommandPackt[1] =  0x04;
                CommandPackt[2] =  0x01;
                CommandPackt[3] =  0x80;
                
                CommandPackt[4] =  0x00;
                CommandPackt[5] =  0x00;
                CommandPackt[6] =  0x00;
                CommandPackt[7] =  0x00;
                CommandPackt[8] =  0x00; //Chksum 0x26
                
                //clear checksum value
                checksum = GetCheckSum( CommandPackt, CommandLength );
                
                memset(log,'\0',100); 
                sprintf(log,"[SendATVMGetStatusCommand()] Chksum = 0x%xh", checksum );
                AVRM_writeFileLogV2(log,DEBUG,g_KSDId);

                //update checksum value 
                CommandPackt[8] = checksum;
                
                LogDataExchnageV2( ALL,"BCA", "Tx",CommandPackt,CommandLength );
                
                pthread_mutex_lock(&CmdPktLengthMutuex);
				g_cmdPktLen_C = 9;
				pthread_mutex_unlock(&CmdPktLengthMutuex);
					             
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                /*for(counter=0;counter<CommandLength;counter++)
                {
                    SendSingleByteToSerialPort_C(g_cctalkhandler, CommandPackt[counter]);
                }//for end*/
                
                //++Write Data to Serial Port
                //On success, the number of bytes written is returned (zero indicates nothing was written). On error, -1 is returned, and errno is set appropriately. 
                if(  -1 != write(g_cctalkhandler, CommandPackt, CommandLength )  )
                {
					AVRM_writeFileLogV2("[SendATVMGetStatusCommand()]  Write Command Success.",INFO,g_BCAId);
				}else{
					AVRM_writeFileLogV2("[SendATVMGetStatusCommand()]  Write Command  Failed.",INFO,g_BCAId);
				}
                return;

}//SendATVMGetStatusCommand()

static  void SendATVMVersionCommand() {


	            pthread_mutex_lock(&RecvPktLengthMutuex);

		        g_rcvPktLen_C = COMMANDREPLYLENGTH; //Return Byte : echo byte(9)+reply byte(5)

		        pthread_mutex_unlock(&RecvPktLengthMutuex);

	            int counter =0;
 
                char checksum =0x00;

                unsigned char log[100];

                memset(log,'\0',100);
               
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                int CommandLength = 9;
                
                char CommandPackt[CommandLength];
                
                memset(CommandPackt,'\0',CommandLength);
                
                CommandPackt[0] =  0x55;
                CommandPackt[1] =  0x04;
                CommandPackt[2] =  0x01;
                CommandPackt[3] =  0x80;
                
                CommandPackt[4] =  0xFF;
                CommandPackt[5] =  0x00;
                CommandPackt[6] =  0x00;
                CommandPackt[7] =  0x00;
                CommandPackt[8] =  0x00; //Chksum
                
                //++clear checksum value
                checksum = GetCheckSum( CommandPackt, CommandLength );
                
                CommandPackt[8] =  checksum;
                
                memset(log,'\0',100); 
                
                sprintf(log,"[SendATVMVersionCommand()] Chksum = 0x%xh", checksum );
                
                ////writeFileLog(log);
                
                AVRM_writeFileLogV2(log,DEBUG,g_KSDId);
                
                LogDataExchnageV2( DATA_EXCHANGE_ALL,
								   "BCA", 
								   "Tx",
								   CommandPackt,
								   CommandLength
					             );

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


				 pthread_mutex_lock(&CmdPktLengthMutuex);
				 g_cmdPktLen_C = 9;
				 pthread_mutex_unlock(&CmdPktLengthMutuex);
				
                 pthread_mutex_lock(&RecvPktLengthMutuex);

				 //Store Reply packet total bytes
				 g_rcvPktLen_C = 18;
				 
				 pthread_mutex_unlock(&RecvPktLengthMutuex);


                //send command to cctalk board
                for(counter=0;counter<CommandLength;counter++){
                    
                    SendSingleByteToSerialPort_C(g_cctalkhandler,CommandPackt[counter]);
                    //memset(log,'\0',100); 
                    //sprintf(log,"[SendATVMVersionCommand()] Byte = 0x%xh", CommandPackt[counter] );
                    ////writeFileLog(log);
                
                }//for end
                
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                return;

}//SendATVMGetStatusCommand()

static  void SendATVMElectronicsCommand() {

	
				pthread_mutex_lock(&RecvPktLengthMutuex);

				g_rcvPktLen_C = COMMANDREPLYLENGTH; //Return Byte : echo byte(17)+reply byte(5)

				pthread_mutex_unlock(&RecvPktLengthMutuex);

                int counter=0;

                unsigned char log[100];

                memset(log,'\0',100);
                
                char CommandPackt[COMMANDLENGTH];
                
                memset(CommandPackt,'\0',COMMANDLENGTH);

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                for(counter=0;counter<COMMANDLENGTH;counter++) {

				   CommandPackt[counter] =  g_AtvmAlarmCommands[1].Command[counter];
				   
				   //memset(log,'\0',100);
						   
				   //sprintf(log,"[SendATVMElectronicsCommand()]  g_AtvmAlarmCommands[1].Command[%d]: 0x %x h .",counter,g_AtvmAlarmCommands[1].Command[counter]); 

				   ////writeFileLog(log);

                }//for end
                
                LogDataExchnageV2( DATA_EXCHANGE_ALL,
								   "BCA", 
								   "Tx",
								   CommandPackt,
								   COMMANDLENGTH
					             );

                
                 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                for(counter=0;counter<COMMANDLENGTH;counter++)
                {

					SendSingleByteToSerialPort_C(g_cctalkhandler,g_AtvmAlarmCommands[1].Command[counter]);
                }
	       
		       return;

}//SendATVMElectronicsCommand() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Set Disable Alarm command structure in ATVM Machine
//Command : Disable alarm
/*

These are command bytes:
Command[0]=0x55 h.
Command[1]=0x04 h.
Command[2]=0x01 h.
Command[3]=0x40 h.
Command[4]=0xC1 h. //16 BIT VAULT OPEN TIME MSB
Command[5]=0xC0 h. //16 BIT VAULT OPEN TIME LSB
Command[6]=0x00 h. //8 BIT ELECTRONIC LOCK ENABLE TIME IN MINUTE
Command[7]=0x00 h. //ALARM LOGIC BIT BY DEFAULT SET TO 0X01
Command[8]=0x00 h. //checksum value

These are reply bytes toatal 5 bytes:
Reply[17]=0x01 h.
Reply[18]=0x00 h.
Reply[19]=0x55 h.
Reply[20]=0x37 h. //Sample Status Byte Value
Reply[21]=0x73 h. //Sample CheckSum  Value

*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Opern Cash Vault
int setDisableAlarmState(const int doorOpenTimeFlag,const int doorOpenexpiryTime ,const int cashboxOpenTimeFlag, const int cashboxOpenexpiryTime ,const char alarmLogicBits )  {

           
					   pthread_mutex_lock( &g_getdisablealarmmutex );

					   ////writeFileLog("[setDisableAlarmState()] Entry.");
					   AVRM_writeFileLogV2("[setDisableAlarmState()] Entry.",TRACE,g_KSDId);


					   unsigned char log[200];
					   memset(log,'\0',200);	
					   unsigned char GetStatusReplyPackt[100];
					   memset(GetStatusReplyPackt,'\0',100);
					   unsigned int GetStatusReplyPacktLength=20;

					   if(-1==g_cctalkhandler){
						   
							 ////writeFileLog("[setDisableAlarmState()] Could Not Obtain Com port handle.");
							 ////writeFileLog("[setDisableAlarmState()] Exit.");
							 AVRM_writeFileLogV2("[setDisableAlarmState()] Could Not Obtain Com port handle.",ERROR,g_KSDId);
							 AVRM_writeFileLogV2("[setDisableAlarmState()] Exit.",ERROR,g_KSDId);
							 pthread_mutex_unlock( &g_getdisablealarmmutex );
							 return COMMUNICATION_FAILURE;
							 
					   } //if end
	   
					   unsigned int tempdoorOpenexpiryTimeMSB=0,
					                tempdoorOpenexpiryTimeLSB=0,
									tempcashboxOpenexpiryTimeMSB=0,
									tempcashboxOpenexpiryTimeLSB=0,rtcode=-1,electronicstime=-1;

					   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					   //Init all command bytes
			 
					   //Clear door open allow time in second 
					   g_AtvmAlarmCommands[1].Command[4]= 0x00;   //16 BIT ONLY ALARM DISABLE TIME MSB
                       g_AtvmAlarmCommands[1].Command[5]= 0x00;   //16 BIT ONLY ALARM DISABLE TIME LSB
					   g_AtvmAlarmCommands[1].Command[6]= 0x00;   //++8 BIT ELECTRONIC LOCK ENABLE TIME IN MINUTE [Vault Open Session Time]

					   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					   //read ini file electronics time
					   //++electronicstime = GetElectronicsLockTime();
					   electronicstime = 1;

					   //only use valid time from ini file
					   if( -1!= electronicstime ) {

						   if( (electronicstime >=0 ) &&  (electronicstime <= 255) ) { 

								  memset(log,'\0',100);

								  sprintf(log,"[setDisableAlarmState()] electronicstime = %d .",electronicstime); 

								  ////writeFileLog(log);
								  
								  AVRM_writeFileLogV2(log,INFO,g_KSDId);

								  g_AtvmAlarmCommands[1].Command[6]= electronicstime;

						   }// if( (electronicstime >=0 ) &&  (electronicstime <= 255) ) end

						  
					   }else{
							 //++//writeFileLog("[setDisableAlarmState()] Unable to read electronics time from ini file so it by default set by zero.");
					   }//else end

					   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                        memset(log,'\0',100);
                        sprintf(log,"[setDisableAlarmState()] alarmLogicBits = 0x%xh .",alarmLogicBits ); 
                        ////writeFileLog(log);
                        AVRM_writeFileLogV2(log,INFO,g_KSDId);
                        g_AtvmAlarmCommands[1].Command[7]= (char)alarmLogicBits;  //ALARM LOGIC BIT BY DEFAULT 
                        
					   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					   //clear checksum value
					   g_AtvmAlarmCommands[1].Command[8]=0x00; //checksum value
					   
					   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					   if(  1 == doorOpenTimeFlag ){ //++Door open
		               
							   ////writeFileLog("[setDisableAlarmState()] Door Opentime flag is set by user.");
							   AVRM_writeFileLogV2("[setDisableAlarmState()] Door Opentime flag is set by user.",INFO,g_KSDId);
							   
							   tempdoorOpenexpiryTimeLSB=0;
							   tempdoorOpenexpiryTimeLSB=(doorOpenexpiryTime & 0x0000ffff);  //get only LSB 2byte from 4 byte
							   g_AtvmAlarmCommands[1].Command[4]=(tempdoorOpenexpiryTimeLSB>> 8);   //msb of Lower 2byte
							   g_AtvmAlarmCommands[1].Command[5]=(tempdoorOpenexpiryTimeLSB & 0x00ff);//lsb of Lower 2byte

                       }else if( 2 == cashboxOpenTimeFlag ){ //++Vault open
		               
							   ////writeFileLog("[setDisableAlarmState()] Vault Opentime flag is set by user.");
							   AVRM_writeFileLogV2("[setDisableAlarmState()] Vault Opentime flag is set by user.",INFO,g_KSDId);
                               tempcashboxOpenexpiryTimeLSB=0;
							   tempcashboxOpenexpiryTimeLSB=(cashboxOpenTimeFlag & 0x0000ffff);  //get only LSB 2byte from 4 byte
							   g_AtvmAlarmCommands[1].Command[4]=(tempcashboxOpenexpiryTimeLSB>> 8);     //msb of Lower 2byte
							   g_AtvmAlarmCommands[1].Command[5]=(tempcashboxOpenexpiryTimeLSB & 0x00ff); //lsb of Lower 2byte

                       }//else if
                       
                       //++set checksum value	
                       g_AtvmAlarmCommands[1].Command[8] = GetCheckSum(g_AtvmAlarmCommands[1].Command,9);

                      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
           
               
                       rtcode= -1 ;

                       pthread_mutex_lock( &g_cctalkporthandlemutex);

					   //Step 1 :First Send getstatus command
					   SendATVMElectronicsCommand();
						  
					   //Step 2 :Now go for read reply bytes
					   rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength,CCTALK_REPLY_WAIT_TIME,g_KSDId);
                 
		               pthread_mutex_unlock( &g_cctalkporthandlemutex);

                       /////////////////////////////////////////////////////////////////////////////////////////////////

                       //Print atvm electronics lock command reply bytes
                       int counter=0;
                       
					   //for(counter=0;counter<GetStatusReplyPacktLength;counter++) {

			              //memset(log,'\0',100);

		                  //sprintf(log,"[setDisableAlarmState()] GetStatusReplyPackt[%d] = 0x%xh .",counter,GetStatusReplyPackt[counter]); 

                          ////writeFileLog(log);

		               //}//for end
                       
                       
                        LogDataExchnageV2( DATA_EXCHANGE_ALL,
								   "BCA", 
								   "Rx",
								   GetStatusReplyPackt,
								   GetStatusReplyPacktLength
					             );

                       
                       //////////////////////////////////////////////////////////////////////////////////////////////////
                       if( SUCCESS == rtcode )
                       {
                            ////writeFileLog("[setDisableAlarmState()] setDisableAlarm command successfully executed.");
                            AVRM_writeFileLogV2("[setDisableAlarmState()] setDisableAlarm command successfully executed.",INFO,g_KSDId);
                            rtcode= 0;  //++Success 0 Alarm disabled
                            
                       }else{
                            //writeFileLog("[setDisableAlarmState()] setDisableAlarm command failed to executed.");
                            AVRM_writeFileLogV2("[setDisableAlarmState()] setDisableAlarm command failed to executed.",ERROR,g_KSDId);
                            rtcode= 1; //++Fail Alarm could'nt be disabled

                       }//else end
 
                       ////writeFileLog("[setDisableAlarmState()] Exit.");
                       AVRM_writeFileLogV2("[setDisableAlarmState()] Exit.",TRACE,g_KSDId);

                       pthread_mutex_unlock( &g_getdisablealarmmutex );

                       return rtcode;



}//setDisableAlarmState() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int getDoorOpenStatus(void) //returns 0 when door is in close state, returns 1 when door is in open state, returns -1 when error reading the status
{
	  int doorOpenStatus=2;
	  //getSecuredState(&doorOpenStatus, NULL, NULL,NULL);
	  return doorOpenStatus;
}

int getAlarmOnStatus(void) //returns 0 when alarm is off, returns 1 when alarm is on, returns -1 when error reading the status
{
	  int getAlarmOnStatus=2;
	  //getSecuredState(NULL,&getAlarmOnStatus, NULL,NULL);
	  return getAlarmOnStatus;

}

int getCashboxOpenStatus(void) //returns 0 when cashbox is in close state, returns 1 when cashbox is in open state, returns -1 when error reading the status
{
	  int getCashboxOpenStatus=2;
	  //getSecuredState(NULL,NULL, &getCashboxOpenStatus,NULL);
	  return getCashboxOpenStatus;

}

int getButtomDoorStatus(int *ButtomDoorStatus) {

      
	              pthread_mutex_lock( &g_getsecuredstausmutex);
                  
                  AVRM_writeFileLogV2("[getButtomDoorStatus()] Entry.",TRACE,g_KSDId);

				  if(-1==g_cctalkhandler)
				  {
						////writeFileLog("[getButtomDoorStatus() Exit] Could Not Obtain Com port handle.");
						AVRM_writeFileLogV2("[getButtomDoorStatus() Exit] Could Not Obtain Com port handle.",ERROR,g_KSDId);
						pthread_mutex_unlock( &g_getsecuredstausmutex);
						return 0;
				  }
		 
                  int rtcode=-1; 
                  unsigned char log[100],bottomdoor=-1;
                  memset(log,'\0',100);
				  unsigned char GetStatusReplyPackt[100];
                  memset(GetStatusReplyPackt,'\0',100);
		          unsigned int GetStatusReplyPacktLength=20;

                  ////////////////////////////////////////////////////////////////////////////

                  cctalk_delay_mSec(100);

                  pthread_mutex_lock( &g_cctalkporthandlemutex);

                  //Step 1 :First Send getstatus command
                  SendATVMGetStatusCommand();
                  
                  //Step 2 :Now go for read reply bytes
                  rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength,CCTALK_REPLY_WAIT_TIME,g_KSDId);

                  pthread_mutex_unlock( &g_cctalkporthandlemutex);

                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                  if( SUCCESS != rtcode ){
			
					  AVRM_writeFileLogV2("[[getButtomDoorStatus() Exit] atvm getstatus command execution failed.",ERROR,g_KSDId);

                      pthread_mutex_unlock( &g_getsecuredstausmutex);

		              return 0;
				  
				  }else{
                                  int  StatusBytePosition=0;

                                  StatusBytePosition=GetStatusReplyPacktLength-2;

                                  char StatusByte=0;

                                  StatusByte = GetStatusReplyPackt[StatusBytePosition] ;

                                  //High if open Low if close
                                  bottomdoor = StatusByte & 0b00000001;
                                 
								  *ButtomDoorStatus = bottomdoor ;

                                  memset(log,'\0',100); 

				                  sprintf(log,"[getButtomDoorStatus() Exit] Bottomdoor status: %d.", *ButtomDoorStatus);

                                  AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                  
                                   if( 0 == *ButtomDoorStatus )
                                   {
                                        ////writeFileLog("[getButtomDoorStatus()] Bottom door close.");
                                        AVRM_writeFileLogV2("[getButtomDoorStatus()] Bottom door close.",INFO,g_KSDId);
                                   }
                                   else if( 1 == *ButtomDoorStatus )
                                   {
                                        ////writeFileLog("[getButtomDoorStatus()] Bottom door open.");
                                        AVRM_writeFileLogV2("[getButtomDoorStatus()] Bottom door open.",INFO,g_KSDId);
                                        
                                   }else{
                                         ////writeFileLog("[getButtomDoorStatus()] Bottom door unknown status.");
                                         AVRM_writeFileLogV2("[getButtomDoorStatus()] Bottom door unknown status.",INFO,g_KSDId);
							       }
                                   rtcode= 1;

                  }//else end

                  pthread_mutex_unlock( &g_getsecuredstausmutex);

                  return rtcode;
                        
}//getButtomDoorStatus() end

static pthread_mutex_t  g_getSecurityVersionNumbermutex = PTHREAD_MUTEX_INITIALIZER;

int getSecurityVersionNumber(unsigned char *SecurityVersionNumber)  {

                  
                  pthread_mutex_lock( &g_getSecurityVersionNumbermutex);

                  ////writeFileLog("[getSecurityVersionNumber()] Entry.");
                  AVRM_writeFileLogV2("[getSecurityVersionNumber()] Entry.",TRACE,g_KSDId);

				  if(-1==g_cctalkhandler){
					  
						  ////writeFileLog("[getSecurityVersionNumber() Exit] Could Not Obtain Com port handle.");
						  
						  AVRM_writeFileLogV2("[getSecurityVersionNumber() Exit] Could Not Obtain Com port handle.",ERROR,g_KSDId);
								
						  pthread_mutex_unlock( &g_getsecuredstausmutex);
					
						  return 0;
						  
				  }//if end
		 
                  ////writeFileLog("[getSecurityVersionNumber()] Going to issue atvm get version command.");
                  
                  AVRM_writeFileLogV2("[getSecurityVersionNumber()] Going to issue atvm get version command.",INFO,g_KSDId);

                  int rtcode=-1; 

                  unsigned char log[100];

                  memset(log,'\0',100);

		          unsigned char GetStatusReplyPackt[100];

                  memset(GetStatusReplyPackt,'\0',100);

                  unsigned int GetStatusReplyPacktLength=0;

                  int StatusBytePosition=0;

                  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                  

                  cctalk_delay_mSec(100);

                  pthread_mutex_lock( &g_cctalkporthandlemutex);

                  //++Step 1 :First Send getstatus command
                  SendATVMVersionCommand();
                  
                  //++Step 2 :Now go for read reply bytes
                  rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength,CCTALK_REPLY_WAIT_TIME,g_KSDId);

                  pthread_mutex_unlock( &g_cctalkporthandlemutex);
                
                  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                  int counter=0;

                  memset(log,'\0',100); 

				  sprintf(log,"[getSecuredState()] Reply Packet Length = %d ", GetStatusReplyPacktLength);

		          ////writeFileLog(log);
		          
		          AVRM_writeFileLogV2V2(log,INFO,g_KSDId);
		          
		          //++Write Exchange Data
				  LogDataExchnageV2( DATA_EXCHANGE_ALL,
									 "KSD", 
									 "Rx",
									 GetStatusReplyPackt,
									 GetStatusReplyPacktLength
					               );

                  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                  if( SUCCESS != rtcode ){
					  
							 ////writeFileLog("[getSecurityVersionNumber() Exit] atvm getstatus command execution failed.");
							 
							 AVRM_writeFileLogV2("[getSecurityVersionNumber() Exit] atvm getstatus command execution failed.",ERROR,g_KSDId);

							 pthread_mutex_unlock( &g_getSecurityVersionNumbermutex);

							 return 0;

				  } else {

                             
                             pthread_mutex_unlock( &g_getSecurityVersionNumbermutex);
                             
                             StatusBytePosition= GetStatusReplyPacktLength - 2 ;
                        
                             ////writeFileLog("[getSecurityVersionNumber()] atvm getstatus command executed successfully.");
                             
                             AVRM_writeFileLogV2("[getSecurityVersionNumber()] atvm getstatus command executed successfully.",INFO,g_KSDId);
                             
                             memset(log,'\0',100); 

		                     sprintf(log,"[getSecurityVersionNumber()] StatusByte: 0x%xh.", GetStatusReplyPackt[StatusBytePosition]);

                             ////writeFileLog(log);
                             
                             AVRM_writeFileLogV2(log,DEBUG,g_BCAId);
                             
                             memset(log,'\0',100); 

                             /*
		                     sprintf(log,"[getSecurityVersionNumber()] Version Byte1: 0x%xh.", GetStatusReplyPackt[13]);

                             //writeFileLog(log); 
                             
                             memset(log,'\0',100); 

		                     sprintf(log,"[getSecurityVersionNumber()] Version Byte2: 0x%xh.", GetStatusReplyPackt[14]);

                             //writeFileLog(log); 
                             
                             memset(log,'\0',100); 

		                     sprintf(log,"[getSecurityVersionNumber()] Version Byte3: 0x%xh.", GetStatusReplyPackt[15]);

                             //writeFileLog(log); 
                             
                             memset(log,'\0',100); 

		                     sprintf(log,"[getSecurityVersionNumber()] Version Byte4: 0x%xh.", GetStatusReplyPackt[16]);

                             //writeFileLog(log);  
                             */ 

                             //char StatusByte=0x00;

                             //StatusByte= GetStatusReplyPackt[StatusBytePosition] ;
                             
                             SecurityVersionNumber[0] = GetStatusReplyPackt[13];
                             SecurityVersionNumber[1] = GetStatusReplyPackt[13];
                             SecurityVersionNumber[2] = ".";
                             SecurityVersionNumber[3] = GetStatusReplyPackt[15];
                             SecurityVersionNumber[4] = GetStatusReplyPackt[16];

                             ////writeFileLog("[getSecurityVersionNumber()] Exit.");
                             AVRM_writeFileLogV2("[getSecurityVersionNumber()] Exit.",TRACE,g_KSDId);

                             pthread_mutex_unlock( &g_getsecuredstausmutex);

							 return 1;


					  } //else end

                     
}//getSecuredState() end here

/*
int returnReservedSecuredState(){
	
							 AVRM_writeFileLogV2("[returnReservedSecuredState()] Entry.",TRACE,g_KSDId);
							 
							 int StatusByte = g_StatusByte;
                             
                             //++Bit 0 Top Door
                             if(NULL!=topdoorOpenStatus)
                             {

                                   unsigned char  topdoor=0x00;
                                  
                                   topdoor = StatusByte & 0b00000001 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[returnReservedSecuredState()] Top Door Status: 0x%xh.", topdoor );
  
                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   *topdoorOpenStatus= topdoor ;

                                   if( 0 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[returnReservedSecuredState()] Top door close.",INFO,g_KSDId);
                                   }
                                   else if( 1 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[returnReservedSecuredState()] Top door open.",INFO,g_KSDId);
                                   }
                                   
			                 }else if(NULL == topdoorOpenStatus){

                                  //writeFileLog("[returnReservedSecuredState()] Top Door status  get failed.");

			                 }//else if end
                             
                             //++Bit 1 CashBox Door
                             if(NULL!=cashboxOpenStatus)
                             {
                                    //++High if open Low if closed
                                    *cashboxOpenStatus=( (GetStatusReplyPackt[StatusBytePosition] & 0b00000010) >> 1);
                                    
                                    memset(log,'\0',100); 

		                            sprintf(log,"[returnReservedSecuredState()] Vault Status: 0x%xh.", *cashboxOpenStatus );

                                    ////writeFileLog(log); 
                                    
                                    AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                    if( 0 == *cashboxOpenStatus)
                                    {
                                        AVRM_writeFileLogV2("[returnReservedSecuredState()] Cash Vault close.",INFO,g_KSDId);
                                    }
                                    else if( 1 == *cashboxOpenStatus )
                                    {
                                        AVRM_writeFileLogV2("[returnReservedSecuredState()] Cash Vault open.",INFO,g_KSDId);
                                    }

                             }else if(NULL == cashboxOpenStatus){

                                  writeFileLog("[returnReservedSecuredState()] cashbox open status  get failed.");

			                 }//else if end
                                                      
                             //++Bit 2 Alarm Status
                             if(NULL!=alarmOnStatus){
								 
                                   //High if alarm on Low if alarm off

                                   *alarmOnStatus=(StatusByte & 0b00000100)>>2 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[returnReservedSecuredState()] Alarm Status: 0x%xh.", *alarmOnStatus );

                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   if( 0 == *alarmOnStatus)
                                   {
                                        AVRM_writeFileLogV2("[returnReservedSecuredState()] Alarm is off.",INFO,g_KSDId);
                                   }
                                   else if( 1 == *alarmOnStatus )
                                   {
                                        AVRM_writeFileLogV2("[returnReservedSecuredState()] Alarm is on",INFO,g_KSDId);
                                   }
 
                             }else if(NULL == alarmOnStatus){

                                  writeFileLog("[returnReservedSecuredState()] alarm status  get failed.");

			                 }//else if end
			    			                 			                 
							 AVRM_writeFileLogV2("[returnReservedSecuredState()] Exit.",TRACE,g_KSDId);

}*/


//0x55 0x04 0x01 0x80 0x00 0x00 0x00 0x00 0x26 :: 0x1 0x0 0x55 0x0 0xaa
int checkSecuredReplyBytes(unsigned char *GetStatusReplyPackt){
		
		        if(                       (0x55   == GetStatusReplyPackt[0] )    && 
                                          (0x04   == GetStatusReplyPackt[1] )    && 
                                          (0x01   == GetStatusReplyPackt[2] )    &&
                                          (0x80   == GetStatusReplyPackt[3] )    && 
                                          (0x00   == GetStatusReplyPackt[4] ) 	 &&
                                          (0x00   == GetStatusReplyPackt[5] )    && 
                                          (0x00   == GetStatusReplyPackt[6] )    &&
                                          (0x00   == GetStatusReplyPackt[7] )    &&
                                          (0x26   == GetStatusReplyPackt[8] )    
                 )
				{


					
								AVRM_writeFileLogV2("[checkSecuredReplyBytes()] Return data is valid.",INFO,g_KSDId);
								return 1;

				}else {
						
								AVRM_writeFileLogV2("[checkSecuredReplyBytes()] Return data is not valid.",ERROR,g_KSDId);
								return 0;

				}//else end
	
}//int checkSecuredReplyBytes(unsigned char *GetStatusReplyPackt) end

//++New Version cctalk style ++Returns all status values in a single call
int getSecuredState(  int* const topdoorOpenStatus, int* const alarmOnStatus,int* const cashboxOpenStatus,int* const bottomdoorOpenStatus, int* const PrinterCoverstatus)  {

                  
                  bool currentCCTalkOperationFlag = false;
                  char StatusByte=0x00;
                  
                  int rtcode=-1; 

                  unsigned char log[100];

                  memset(log,'\0',100);

		          unsigned char GetStatusReplyPackt[100];

                  memset(GetStatusReplyPackt,'0x00',100);

                  unsigned int GetStatusReplyPacktLength=0;

                  int StatusBytePosition=0;
                  
                  pthread_mutex_lock( &g_getsecuredstausmutex);

                  AVRM_writeFileLogV2("[getSecuredState()] Entry.",TRACE,g_KSDId);
                  
                  
				  if(-1==g_cctalkhandler){
					  
						  AVRM_writeFileLogV2("[getSecuredState() Exit] Could Not Obtain Com port handle.",ERROR,g_KSDId);
								
						  pthread_mutex_unlock( &g_getsecuredstausmutex);
					
						  return COMMUNICATION_FAILURE;
						  
				  }//if end
		 
		          //++if all pointer have null value return then
		          if( (NULL== topdoorOpenStatus) && (NULL == bottomdoorOpenStatus) && ( NULL== alarmOnStatus) && (NULL== cashboxOpenStatus) && (NULL== PrinterCoverstatus) ) {
                           
                           AVRM_writeFileLogV2("[getSecuredState() Exit] Function parameters not ok.",WARN,g_KSDId);

                           pthread_mutex_unlock( &g_getsecuredstausmutex);

                           return OTHER_ERROR;

                  }//if end
 
				  //++Return Previous Data
				  pthread_mutex_lock(&g_CcTalkPortOperationFlag);
				  currentCCTalkOperationFlag = g_CCTalkOperationFlag ;
				  pthread_mutex_unlock(&g_CcTalkPortOperationFlag);
				  
				  //++Check if Serial Port busy then return stored data		
				  if( true == currentCCTalkOperationFlag ) 
				  {
					 
                             StatusByte = g_StatusByte;
                             
                             //++Bit 0 Top Door
                             if(NULL!=topdoorOpenStatus)
                             {

                                   unsigned char  topdoor=0x00;
                                  
                                   topdoor = StatusByte & 0b00000001 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()] Top Door Status: 0x%xh.", topdoor );
  
                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   *topdoorOpenStatus= topdoor ;

                                   if( 0 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Top door close.",INFO,g_KSDId);
                                   }
                                   else if( 1 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Top door open.",INFO,g_KSDId);
                                   }
                                   
			                 }else if(NULL == topdoorOpenStatus){

                                  //writeFileLog("[getSecuredState()] Top Door status  get failed.");

			                 }//else if end
                             
                             //++Bit 1 CashBox Door
                             if(NULL!=cashboxOpenStatus)
                             {
                                    //++High if open Low if closed
                                    *cashboxOpenStatus=( (GetStatusReplyPackt[StatusBytePosition] & 0b00000010) >> 1);
                                    
                                    memset(log,'\0',100); 

		                            sprintf(log,"[getSecuredState()] Vault Status: 0x%xh.", *cashboxOpenStatus );

                                    ////writeFileLog(log); 
                                    
                                    AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                    if( 0 == *cashboxOpenStatus)
                                    {
                                        AVRM_writeFileLogV2("[getSecuredState()] Cash Vault close.",INFO,g_KSDId);
                                    }
                                    else if( 1 == *cashboxOpenStatus )
                                    {
                                        AVRM_writeFileLogV2("[getSecuredState()] Cash Vault open.",INFO,g_KSDId);
                                    }

                             }else if(NULL == cashboxOpenStatus){

                                  //writeFileLog("[getSecuredState()] cashbox open status  get failed.");

			                 }//else if end
                                                      
                             //++Bit 2 Alarm Status
                             if(NULL!=alarmOnStatus){
								 
                                   //High if alarm on Low if alarm off

                                   *alarmOnStatus=(StatusByte & 0b00000100)>>2 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()] Alarm Status: 0x%xh.", *alarmOnStatus );

                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   if( 0 == *alarmOnStatus)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Alarm is off.",INFO,g_KSDId);
                                   }
                                   else if( 1 == *alarmOnStatus )
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Alarm is on",INFO,g_KSDId);
                                   }
 
                             }else if(NULL == alarmOnStatus){

                                  //writeFileLog("[getSecuredState()] alarm status  get failed.");

			                 }//else if end
			    
                             //++Bit 3 Bottom Door
                             if(NULL!=bottomdoorOpenStatus){

                                   unsigned char  bottomdoor=0x00;
                                  
                                   bottomdoor = (StatusByte & 0b00001000)>>3 ;

                                   *bottomdoorOpenStatus= bottomdoor ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()] Bottom Door Status: 0x%xh.", *bottomdoorOpenStatus );

                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   if( 0 == *bottomdoorOpenStatus)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Bottom door close.",INFO,g_KSDId);
                                   }
                                   else if( 1 == *bottomdoorOpenStatus)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Bottom door open.",INFO,g_KSDId);
                                   }
                                   
			                 }else if(NULL == bottomdoorOpenStatus){

                                  //writeFileLog("[getSecuredState()] Bottom Door status  get failed.");

			                 }//else if end
			                 			                 
							 AVRM_writeFileLogV2("[getSecuredState()] Exit with Res Data && CCTALK Operation Flag On.",TRACE,g_KSDId);

                             pthread_mutex_unlock( &g_getsecuredstausmutex);

							 return 1;
							  
				  }//if end
                  
                  AVRM_writeFileLogV2("[getSecuredState()] Going to issue atvm getstatus command.",INFO,g_KSDId);
                  
                  //++ 0x55 0x04 0x01 0x80 0x00 0x00 0x00 0x00 0x26 :: 0x1 0x0 0x55 0x0 0xaa
                  
                  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                  
                  pthread_mutex_lock( &g_cctalkporthandlemutex);
                                    
                  FlushSerialPortOutBuff_C(g_cctalkhandler);
                  FlushSerialPortInBuff_C(g_cctalkhandler);

                  //++Step 1 :First Send getstatus command
                  cctalk_delay_mSec(50);
                   
                  SendATVMGetStatusCommand();
                  
                  //++Step 2 :Now go for read reply bytes
                  int replyTime = 4;
                  rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength,replyTime,g_KSDId);
                  
                  /*
                  //++ Wrong Data
                  //++ 0xaa 0x55 0x55 0x04 0x01 0x80 0x00 0x00 0x00 0x00 0x26 0x1 0x0 0x55 0x0
                  
                  GetStatusReplyPackt[0] = 0xaa;   
                  GetStatusReplyPackt[1] = 0x55;   
                  GetStatusReplyPackt[2] = 0x04;   
                  GetStatusReplyPackt[3] = 0x01;   
                  GetStatusReplyPackt[4] = 0x80;  
                  GetStatusReplyPackt[5] = 0x00;   
                  GetStatusReplyPackt[6] = 0x00;   
                  GetStatusReplyPackt[7] = 0x00;  
                  GetStatusReplyPackt[8] = 0x00;   
                  GetStatusReplyPackt[9] = 0x26;   
                  
                  GetStatusReplyPackt[10] = 0x01;
                  GetStatusReplyPackt[11] = 0x00;
                  GetStatusReplyPackt[12] = 0x55;
                  GetStatusReplyPackt[13] = 0x00;
                  
                  rtcode = SUCCESS;
                  */
                
                  //++Retry 2nd Time if Fail Status
                  if( FAIL == rtcode ){
						AVRM_writeFileLogV2("[getSecuredState()] Going to retry..",TRACE,g_KSDId);
						FlushSerialPortOutBuff_C(g_cctalkhandler);
						FlushSerialPortInBuff_C(g_cctalkhandler);
						memset(GetStatusReplyPackt,'\0',100);
						//++Send Again Command
						SendATVMGetStatusCommand();
						rtcode = -1;
						rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength,replyTime,g_KSDId);
						//++if again fail return reserved state
						if( FAIL == rtcode ){							
							 //++Release Com Port	
							 pthread_mutex_unlock( &g_cctalkporthandlemutex); 
							 
							 StatusByte = g_StatusByte;
							 
                             //++Bit 0 Top Door
                             if(NULL!=topdoorOpenStatus)
                             {
                                   unsigned char  topdoor=0x00;
                                  
                                   topdoor = StatusByte & 0b00000001 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()] Top Door Status: 0x%xh.", topdoor );
  
                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   *topdoorOpenStatus= topdoor ;

                                   if( 0 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Top door close.",INFO,g_KSDId);
                                   }
                                   else if( 1 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[returnReservedSecuredState()] Top door open.",INFO,g_KSDId);
                                   }
                                   
			                 }else if(NULL == topdoorOpenStatus){
                                  //AVRM_writeFileLogV2("[getSecuredState()] Door status  get failed",INFO,g_KSDId);
			                 }//else if end
                             
                             //++Bit 1 CashBox Door
                             if(NULL!=cashboxOpenStatus)
                             {
                                    //++High if open Low if closed
                                    *cashboxOpenStatus=( (GetStatusReplyPackt[StatusBytePosition] & 0b00000010) >> 1);
                                    
                                    memset(log,'\0',100); 

		                            sprintf(log,"[getSecuredState()] Vault Status: 0x%xh.", *cashboxOpenStatus );

                                    AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                    if( 0 == *cashboxOpenStatus)
                                    {
                                        AVRM_writeFileLogV2("[getSecuredState()] Cash Vault close.",INFO,g_KSDId);
                                    }
                                    else if( 1 == *cashboxOpenStatus )
                                    {
                                        AVRM_writeFileLogV2("[getSecuredState()] Cash Vault open.",INFO,g_KSDId);
                                    }

                             }else if(NULL == cashboxOpenStatus){
                                  //AVRM_writeFileLogV2("[getSecuredState()] ChashBox status  get failed",INFO,g_KSDId);
			                 }//else if end
                                                      
                             //++Bit 2 Alarm Status
                             if(NULL!=alarmOnStatus){
								 
                                   //High if alarm on Low if alarm off

                                   *alarmOnStatus=(StatusByte & 0b00000100)>>2 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()] Alarm Status: 0x%xh.", *alarmOnStatus );

                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   if( 0 == *alarmOnStatus)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Alarm is off.",INFO,g_KSDId);
                                   }
                                   else if( 1 == *alarmOnStatus )
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Alarm is on",INFO,g_KSDId);
                                   }
 
                             }else if(NULL == alarmOnStatus){
                                  //AVRM_writeFileLogV2("[getSecuredState()] Alarm status  get failed",INFO,g_KSDId);
			                 }//else if end
			    			 
			    			 AVRM_writeFileLogV2("[getSecuredState()] Exit With Res Data.",TRACE,g_KSDId);
			    			 pthread_mutex_unlock( &g_getsecuredstausmutex);
			    			 return 1;           			                 
						}//++if end
				  }//++if end
                  
                  pthread_mutex_unlock( &g_cctalkporthandlemutex);
                  
                  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                  int counter=0;
                  memset(log,'\0',100); 
				  sprintf(log,"[getSecuredState()] Reply Packet Length = %d ", GetStatusReplyPacktLength);
		          AVRM_writeFileLogV2(log,INFO,g_KSDId);
		          
		          /*g_StatusByte = 0x05;
		          if( NULL != topdoorOpenStatus ){
					*topdoorOpenStatus = 0 ;
			      }
			      if( NULL != cashboxOpenStatus ){
					*cashboxOpenStatus = 0;
			      }
		          if( NULL != alarmOnStatus ){
						*alarmOnStatus=0;
			      }
		          pthread_mutex_unlock( &g_getsecuredstausmutex);
		          return 1;*/
		          		         		          
                  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                  if( SUCCESS != rtcode )
                  {
						AVRM_writeFileLogV2("[getSecuredState() Exit] atvm getstatus command execution failed.",ERROR,g_KSDId);
						pthread_mutex_unlock( &g_getsecuredstausmutex);
						return COMMUNICATION_FAILURE;
				  }
				  else 
				  {
                             //Check Data Validitiy
                             if( 1 == checkSecuredReplyBytes(GetStatusReplyPackt) ){
								 memset(log,'\0',100); 
								 sprintf(log,"[getSecuredState()] Va.. Data", GetStatusReplyPackt[StatusBytePosition]);
								 AVRM_writeFileLogV2(log,DEBUG,g_KSDId);
							 }else{
								 
								 memset(log,'\0',100); 
								 sprintf(log,"[getSecuredState()] InVa.. Data", GetStatusReplyPackt[StatusBytePosition]);
								 AVRM_writeFileLogV2(log,DEBUG,g_KSDId);
								 
								 StatusByte = g_StatusByte;
							 
								 //++Bit 0 Top Door
								 if(NULL!=topdoorOpenStatus)
								 {
									   unsigned char  topdoor=0x00;
									  
									   topdoor = StatusByte & 0b00000001 ;
									   
									   memset(log,'\0',100); 

									   sprintf(log,"[getSecuredState()] Top Door Status: 0x%xh.", topdoor );
	  
									   AVRM_writeFileLogV2(log,INFO,g_KSDId);

									   *topdoorOpenStatus= topdoor ;

									   if( 0 == topdoor)
									   {
											AVRM_writeFileLogV2("[getSecuredState()] Top door close.",INFO,g_KSDId);
									   }
									   else if( 1 == topdoor)
									   {
											AVRM_writeFileLogV2("[returnReservedSecuredState()] Top door open.",INFO,g_KSDId);
									   }
									   
								 }else if(NULL == topdoorOpenStatus){
									  //AVRM_writeFileLogV2("[getSecuredState()] Door status  get failed",INFO,g_KSDId);
								 }//else if end
								 
								 //++Bit 1 CashBox Door
								 if(NULL!=cashboxOpenStatus)
								 {
										//++High if open Low if closed
										*cashboxOpenStatus=( (GetStatusReplyPackt[StatusBytePosition] & 0b00000010) >> 1);
										
										memset(log,'\0',100); 

										sprintf(log,"[getSecuredState()] Vault Status: 0x%xh.", *cashboxOpenStatus );

										AVRM_writeFileLogV2(log,INFO,g_KSDId);

										if( 0 == *cashboxOpenStatus)
										{
											AVRM_writeFileLogV2("[getSecuredState()] Cash Vault close.",INFO,g_KSDId);
										}
										else if( 1 == *cashboxOpenStatus )
										{
											AVRM_writeFileLogV2("[getSecuredState()] Cash Vault open.",INFO,g_KSDId);
										}

								 }else if(NULL == cashboxOpenStatus){
									  //AVRM_writeFileLogV2("[getSecuredState()] ChashBox status  get failed",INFO,g_KSDId);
								 }//else if end
														  
								 //++Bit 2 Alarm Status
								 if(NULL!=alarmOnStatus){
									 
									   //High if alarm on Low if alarm off

									   *alarmOnStatus=(StatusByte & 0b00000100)>>2 ;
									   
									   memset(log,'\0',100); 

									   sprintf(log,"[getSecuredState()] Alarm Status: 0x%xh.", *alarmOnStatus );

									   AVRM_writeFileLogV2(log,INFO,g_KSDId);

									   if( 0 == *alarmOnStatus)
									   {
											AVRM_writeFileLogV2("[getSecuredState()] Alarm is off.",INFO,g_KSDId);
									   }
									   else if( 1 == *alarmOnStatus )
									   {
											AVRM_writeFileLogV2("[getSecuredState()] Alarm is on",INFO,g_KSDId);
									   }
	 
								 }else if(NULL == alarmOnStatus){
									  //AVRM_writeFileLogV2("[getSecuredState()] Alarm status  get failed",INFO,g_KSDId);
								 }//else if end
								 
								 AVRM_writeFileLogV2("[getSecuredState()] Exit With Res Data InVa...",TRACE,g_KSDId);
								 pthread_mutex_unlock( &g_getsecuredstausmutex);
								 return 1;       
								 
							 }//else end
                             
                             StatusBytePosition= GetStatusReplyPacktLength - 2 ; 
                                                         
                             AVRM_writeFileLogV2("[getSecuredState()] atvm getstatus command executed successfully.",INFO,g_KSDId);         
                                                 
                             memset(log,'\0',100); 

		                     sprintf(log,"[getSecuredState()] StatusByte: 0x%xh.", GetStatusReplyPackt[StatusBytePosition]);
                             
                             AVRM_writeFileLogV2(log,DEBUG,g_KSDId);

                             StatusByte= GetStatusReplyPackt[StatusBytePosition] ;
                             
                             //++Store Current Status Byte
                             g_StatusByte= StatusByte; 

                             /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                             //++Bit 0 Top Door
                             if(NULL!=topdoorOpenStatus)
                             {

                                   unsigned char  topdoor=0x00;
                                  
                                   topdoor = StatusByte & 0b00000001 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()]  Door Status: 0x%xh.", topdoor );
                                   
                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   *topdoorOpenStatus= topdoor ;

                                   if( 0 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()]  door close.",INFO,g_KSDId);
                                   }
                                   else if( 1 == topdoor)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()]  door open.",INFO,g_KSDId);
                                   }
                                   
			                 }else if(NULL == topdoorOpenStatus){
                                  //AVRM_writeFileLogV2("[getSecuredState()] Top Door status  get failed.",INFO,g_KSDId);
			                 }//else if end
                             
                             ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                             //++Bit 1 CashBox Door
                             if(NULL!=cashboxOpenStatus)
                             {
                                    //++High if open Low if closed
                                    *cashboxOpenStatus=( (GetStatusReplyPackt[StatusBytePosition] & 0b00000010) >> 1);
                                    
                                    memset(log,'\0',100); 

		                            sprintf(log,"[getSecuredState()] Vault Status: 0x%xh.", *cashboxOpenStatus );

                                    AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                    if( 0 == *cashboxOpenStatus)
                                    {
                                        ////writeFileLog("[getSecuredState()] Cash Vault close.");
                                        AVRM_writeFileLogV2("[getSecuredState()] Cash Vault close.",INFO,g_KSDId);
                                    }
                                    else if( 1 == *cashboxOpenStatus )
                                    {
                                        ////writeFileLog("[getSecuredState()] Cash Vault open.");
                                        AVRM_writeFileLogV2("[getSecuredState()] Cash Vault open.",INFO,g_KSDId);
                                    }

                             }else if(NULL == cashboxOpenStatus){
                                  //AVRM_writeFileLogV2("[getSecuredState()] cashbox open status  get failed.",INFO,g_KSDId);
			                 }//else if end
                                                      
                             /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                             //++Bit 2 Alarm Status
                             if(NULL!=alarmOnStatus){
								 
                                   //High if alarm on Low if alarm off

                                   *alarmOnStatus=(StatusByte & 0b00000100)>>2 ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()] Alarm Status: 0x%xh.", *alarmOnStatus );

                                   ////writeFileLog(log); 
                                   
                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   if( 0 == *alarmOnStatus)
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Alarm is off.",INFO,g_KSDId);
                                   }
                                   else if( 1 == *alarmOnStatus )
                                   {
                                        AVRM_writeFileLogV2("[getSecuredState()] Alarm is on",INFO,g_KSDId);
                                   }
 
                             }else if(NULL == alarmOnStatus){
                                  //AVRM_writeFileLogV2("[getSecuredState()] alarm status  get failed",INFO,g_KSDId);
			                 }//else if end
			    
                             //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                             
                             //++Bit 3 Bottom Door
                             if(NULL!=bottomdoorOpenStatus){

                                   unsigned char  bottomdoor=0x00;
                                  
                                   bottomdoor = (StatusByte & 0b00001000)>>3 ;

                                   *bottomdoorOpenStatus= bottomdoor ;
                                   
                                   memset(log,'\0',100); 

		                           sprintf(log,"[getSecuredState()] Bottom Door Status: 0x%xh.", *bottomdoorOpenStatus );

                                   ////writeFileLog(log); 
                                   
                                   AVRM_writeFileLogV2(log,INFO,g_KSDId);

                                   if( 0 == *bottomdoorOpenStatus)
                                   {
                                        ////writeFileLog("[getSecuredState()] Bottom door close.");
                                        AVRM_writeFileLogV2("[getSecuredState()] Bottom door close.",INFO,g_KSDId);
                                   }
                                   else if( 1 == *bottomdoorOpenStatus)
                                   {
                                        ////writeFileLog("[getSecuredState()] Bottom door open.");
                                         AVRM_writeFileLogV2("[getSecuredState()] Bottom door open.",INFO,g_KSDId);
                                   }
                                   
			                 }else if(NULL == bottomdoorOpenStatus){
                                  //AVRM_writeFileLogV2("[getSecuredState()] Bottom Door status  get failed.",INFO,g_KSDId);
			                 }//else if end
			                 			                 
                             //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                             AVRM_writeFileLogV2("[getSecuredState()] Exit With Current Data.",TRACE,g_KSDId);

                             pthread_mutex_unlock( &g_getsecuredstausmutex);

							 return 1;

				  }//++else end

}//getSecuredState() end here

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif   //ATVM_ALARM_VAULT endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ATVMCA_MAIN

void Creditpolling();

int EnableAllCoins();

void SendSoftwareversionCommand();

void SendSoftwareversionCommand(){
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x02);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xF1); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x0C); 
    g_rcvPktLen_C = 15;
}

int EnableAllCoins(){

	  unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT];
	  memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 
	  int  rcvPktLen=0;
	  coinType COIN;
	  COIN.rs_50  = 1;
	  COIN.rs_1   = 1;
	  COIN.rs_2   = 1;
	  COIN.rs_5   = 1;
	  COIN.rs_10  = 1;
          

	  MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 

	  if(MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen) == SUCCESS)
	  {      
	     if((rcvPkt[3] != 0xE7) || (rcvPkt[11] !=0xFD)) 
             {
	        printf("[EnableAllCoins()] failed to enable all Coin ");
                //return FAIL;
	     }
             else
             {
	        printf("[EnableAllCoins()] successfully enable all Coin ");
                //return SUCCESS;
             }
	  }
	  else
          { 
	      printf("[EnableAllCoins()] MONEYCONTROL_CoinAcptr_statusRead() return failed ");
              //return FAIL;
          }
          int i=0;
          for(i=0; i< 12 ; i++)
	  {

             printf("\n [EnableAllCoins()] Recv packet[%d]= 0x%x h \n",i,rcvPkt[i]);

          }
          cctalk_delay_mSec(100); 
          MONEYCONTROL_CoinAcptr_Cmd_CreditPoll();          
	  if(MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME) == SUCCESS)
	  {      
		    if(( 0xE5 == rcvPkt[3] ) && (rcvPktLen == g_rcvPktLen_C) )
		    {
			      g_prevEvent_C = rcvPkt[9];
                    }
      }
  
}//if end

static void CAGetCreditCodeDenomValue(const unsigned char* const rcvPkt,int* const  amnt)  {

  	int i_flag=0,j_count=0,i=0,denom=0,j=0;

        int count=0;
        for(count=0;count<5;count++)
	{
            amnt[count]=0;
        }
    
        for(i=10; i<=18; i+=2)
	{
		denom = rcvPkt[i];
		i_flag=0;
		for(j_count=0; j_count<8; j_count++)
		{
			if(denom == g_CoinTable[j_count].CreditCode)
			{
				amnt[j] = g_CoinTable[j_count].Credit;
				i_flag=1;
				break;
			}
			if(i_flag)
				break;
		}
		j++;
	}

}//end

void Creditpolling() {
       
           unsigned char rcvPkt[40]; 
           int Denom[5];
           memset(Denom,0,5);

           memset(rcvPkt,'\0',40); 
           int  rcvPktLen=0;
           int totalcoins=0;
           int Coins=0;
           int Counter=0;

           //writeFileLog("[Creditpolling()] Now going to enable all coins.. ");

           EnableAllCoins();
           
             
           for(;;)
           {                     
                
                
	        rcvPktLen=0;
                
                memset(rcvPkt,'\0',40); 

                Counter=0;

                cctalk_delay_mSec(200); 

                MONEYCONTROL_CoinAcptr_Cmd_CreditPoll();      
                  

	        if( SUCCESS == MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME) )
	        { 

                           if( (rcvPkt[5]== 0x01) && (rcvPkt[6]==0xB) && (rcvPkt[7]==0x02) && (rcvPkt[8]==0x00) ) 
			   {
				  if(rcvPkt[9]  != g_prevEvent_C)
				  {          
		                        //system("clear");
                                        printf("\n");
                                        
                                        //old event counter
                                        printf("\n [Creditpolling()] Old Event  = %d ",g_prevEvent_C);
                                         
                                        //Event Counter
                                        printf("\n [Creditpolling()] New Event  = %d ",rcvPkt[9]);

                                        //Credit code
                                        printf("\n [Creditpolling()] New Event Credit Code = %d ",rcvPkt[10]);
                                        
                                        
                                        if( rcvPkt[9] != g_prevEvent_C  )
                                        {
                                            
                                           
                                             Coins=1;

                                             memset(Denom,0,5);

                                             CAGetCreditCodeDenomValue(rcvPkt,Denom);
                                           
                                             if(rcvPkt[10] >0)
                                             {
                                                totalcoins +=1;
                                                printf("\n [Creditpolling()] total coins  = %d ",totalcoins);                                 
                                             }
                                             while(Coins > 0)
                                             {
                                                 printf( "\n Denom[%d]=%d",Counter,Denom[Counter]);
                                                 Counter++;
                                                 Coins-=1;
                                             }

                                             Coins=0;
                                                 
                                               
                                        }//if( rcvPkt[9] != g_prevEvent_C  ) end

                                       
				  } 

			          g_prevEvent_C = rcvPkt[9];  

                                 
			   } 
		           else 
		           {
				//writeFileLog("[Creditpolling()] Coin acceptor poll status read failed..");   
                           }

                       
                   }

                 
 
               } //for loop




}//end

int main() {


	     int portno=-1;
	     printf("\n[main()] Enter Serial Port Number :");
	     scanf("%d",&portno);
	     printf("\n[main()] Going to open port for coin acceptor.");
	     g_cctalkhandler=-1;

	     

	     OpenPort_C(portno,&g_cctalkhandler);
	     
	     if (-1 != g_cctalkhandler)
	      printf("\n[Creditpolling()] open port for coin acceptor success.");
	     else
	      printf("\n[Creditpolling()] open port for coin acceptor failed."); 

	     Creditpolling();

	     //////////////////////////////////////////////////////////////////////////////

	     char rcvPkt[100];
	     memset(rcvPkt,0,100);
	     int rcvPktLen=0,counter=0;
	     SendSoftwareversionCommand();
	     if(MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME) == SUCCESS) 
	     {      
		 printf("\n[main()] Recv software version command success ");
		 for(counter=0;counter<rcvPktLen;counter++)
		 { 
		   
		   printf("\n[main()] rcvPkt[%d] = 0x%xh. ",counter,rcvPkt[counter]);

		 }
	     }
	     else
	     { 
		 printf("\n[main()] Recv software version command failed ");
		      
	     }

	     //////////////////////////////////////////////////////////////////////////////
    
     
}//main end

#endif  //++ATVMCA_MAIN endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ATVMALARM_MAIN


static   int g_Mcashboxopenexpirytime=0;

static   int g_Mdooropenexpirytime=0;


int WaitForVaultOpen(int second) {

          int VaultStatus=-1,rtcode=-1;

          struct timespec begints, endts,diffts;

          clock_gettime(CLOCK_MONOTONIC, &begints);

          while(1)
          {

                //writeFileLog("\n\n=========================================");

                rtcode =-1;
 
                VaultStatus=-1;
							     
                getSecuredState(NULL,NULL,&VaultStatus,NULL);

                                                           
                //0 closed 1 open
                if(  0 == VaultStatus  )
                {
                      //writeFileLog("[WaitForVaultOpen()] ATVM Volt is locked");

                      


		} 
                else if(  1 == VaultStatus  )
                {

                     //writeFileLog("[WaitForVaultOpen()] ATVM Volt is still opened");

                     return 1;

                } 

                //writeFileLog("[WaitForVaultOpen()] Now Checking for Vault lock open condition.");
	
					     
                clock_gettime(CLOCK_MONOTONIC, &endts);
 
                diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                if(diffts.tv_sec>= second ) //Given door open allow time check only
                {

		         //writeFileLog("[WaitForVaultOpen()] Timeout for cashbox vault close status check.");

	                 return 0;

                }

			     
            }// while(1) end here




}//int WaitForVaultOpen(int second) end


int WaitForVaultLocked(){

          int VaultStatus=-1,rtcode=-1;

          struct timespec begints, endts,diffts;

          clock_gettime(CLOCK_MONOTONIC, &begints);

          while(1)
          {

                //writeFileLog("\n\n=========================================");

                rtcode =-1;
 
                VaultStatus=-1;
							     
                getSecuredState(NULL,NULL,&VaultStatus,NULL);

                                                           
                //0 closed 1 open
                if(  0 == VaultStatus  )
                {
                      //writeFileLog("[WaitForVaultLocked()] ATVM Volt is locked");
                      
                      return 1;
                      


		} 
                else if(  1 == VaultStatus  )
                {

                     //writeFileLog("[WaitForVaultLocked()] ATVM Volt is still opened");

                     

                } 

                //writeFileLog("[WaitForVaultLocked()] Now Checking for Vault lock close condition.");
	
					     
                clock_gettime(CLOCK_MONOTONIC, &endts);
 
                diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                if(diffts.tv_sec>= 3600 ) //1 Hour check
                {
		         //writeFileLog("[WaitForVaultLocked()] Timeout for cashbox vault close status check.");

	                 return 0;
                }

			     
            }// while(1) end here




}//int WaitForVaultLocked() end


int main()
{
   
                  int  iret1=-1;
                  int returnVal=-1; 
                  int VaultStatus=-1,rtcode=-1,ButtomDoorStatus=-1;
                  unsigned int JniRtCode=0;
                  unsigned int ApiAllowdooropenExpirytime=0;
                  unsigned int ApiAllowCashBoxTimeExpirytime=0;
		  int handle=-1;
		  int portno=-1;

		  printf("\(main) Enter Serial Port Number :");

		  scanf("%d",&portno);

		  printf("\(main) handle=%d",handle);

		  OpenPort_C(portno,&handle);
		  printf("\(main) handle=%d",handle);

		  g_cctalkhandler=handle;

		  int doorOpenStatus=-1,alarmOnStatus=-1,cashboxOpenStatus=-1,Prinercoverstatus=-1;

		  int choice =-1;

		  int Index=-1;

		  while(1)
		  {

			printf("\n Atvm Alarm Menu ");
			printf("\n 1.Get Current Status");
			printf("\n 2.Set Disable Alarm"); 
			printf("\n 3.Cris API Style Vault Open.");
                        printf("\n 4.Getstatus Loop Style.");
                        printf("\n 5.Exit");
			printf("\n Enter your choice in number: ");
			scanf("%d",&choice);

			if( choice > 5 )
			{
			  printf("\n Wrong Input Again Choice");
			  continue;
			}
		
			if(5==choice)
			{
			  printf("\n Program now exit");
			  break;
			}

			switch(choice)
			{

			  case 1: Index=0;
                                  //cctalk_delay_mSec(100);
				  getSecuredState(NULL,
                                  &alarmOnStatus,
                                  &cashboxOpenStatus,
                                  NULL );
				  printf("\n---------------------------------------------------------------------\n");
				  
				  
				  if(1==alarmOnStatus)
				     printf("\nAlarm is on");
				  else if(0==alarmOnStatus)
				     printf("\nAlarm is off");
				  
				  if(1==cashboxOpenStatus)
				     printf("\nCashBox is open");
				  else if(0==cashboxOpenStatus)
				     printf("\nCashBox is close");
				 
				  printf("\n-------------------------------------------------------------------\n");
				  break;

			  case 2:  
                                   Index=1;
				   unsigned int AllowdooropenExpirytime=0;
				   unsigned int AllowCashBoxTimeExpirytime=0;
				   printf("\n Enter Vault open expiray time in seconds :");
				   scanf("%d",&AllowdooropenExpirytime);

				   if(AllowdooropenExpirytime> 65535)
				   {
				     printf("\nwrong input expiray time max value is 65535 seconds.");
				     
				   }
                                  
				   setDisableAlarmState(1,AllowdooropenExpirytime,0,0);
                                   
				   break;

                            case 3:  
                                   
                                   iret1=-1;
                                   returnVal=-1; 
                                   JniRtCode=0;
                                   ApiAllowdooropenExpirytime=0;
				   ApiAllowCashBoxTimeExpirytime=0;
				   printf("\n Enter Vault expiray time in seconds :");
				   scanf("%d",&ApiAllowdooropenExpirytime);

				   if(ApiAllowdooropenExpirytime> 65535)
				   {
				      printf("\nwrong input expiray time max value is 65535 seconds.");

                                      break;
				     
				   }
				  
                   	           printf("\nApiAllowdooropenExpirytime = %d ",ApiAllowdooropenExpirytime);
                                  
                                   if(ApiAllowdooropenExpirytime > 0)
			           {

                                       JniRtCode=setDisableAlarmState(1,
                                       ApiAllowdooropenExpirytime ,0,0);

                                       if(1==JniRtCode)
		                       {
		                           //writeFileLog("\n[main()] setDisableAlarmState() executed for door open successfully.");
		                       }
		                       else
		                       {
		                           //writeFileLog("[main()] setDisableAlarmState() executed for door open failed to execute.");
                                           break;
		                       }	

                                       //writeFileLog("[main()] Now checking for vault open.");

                                       if( 1 ==  WaitForVaultOpen(ApiAllowdooropenExpirytime) )
                                       {
                           
                                             //writeFileLog("[main()] Now checking for vault close .");

                                             if( 1 ==  WaitForVaultLocked() )
                                             {
                                                //writeFileLog("[main()] vault closed by user.");
                                             }
                                             else
                                             {
                                                //writeFileLog("[main()] Warning vault is not closed by user.");
                                             }


                                       }
                                       else
                                       {

                                              //writeFileLog("[main()] vault do not opened by user withing given time.");

                                       }

			            }//if end here

                                    break;

                               case  4:  
                                         while(1)
                                         {
                                                //writeFileLog(" \n\n\n\n\n ");
                                                
                                                alarmOnStatus=-1;

                                                cashboxOpenStatus=-1;
                                                
                                                getSecuredState(NULL,
                                                                &alarmOnStatus,
                                                                &cashboxOpenStatus,
                                                                NULL );

                                                

                                         }

                                         break;  
                               default:  //writeFileLog(" Again choice ");
                                         break;

			 
			}//switch() end
		      
		       
		
		  }//while end

		  return 0;

}

#endif //ATVMALARM_MAIN endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef  ATVM_ESCORW_MAIN

int main()
{

 
			  int handle=-1;
			  int portno=-1;
			  printf("\(main) Enter Serial Port Number :");
			  scanf("%d",&portno);
			  printf("\(main) Before open serial port handle value =%d",handle);
			  OpenPort_C(portno,&handle);
			  printf("\(main) After open serial port handle value =%d",handle);
			  g_cctalkhandler=handle;
			  int choice =-1;
			  int Index=-1;
			  int delay=0;
			  while(1)
			  {
				printf("\n Atvm Coin Escrow Menu ");
                                printf("\n 1.Poll .");
				printf("\n 2.Accept .");
				printf("\n 3.Reject ."); 
				printf("\n 4.Polling Excrow");
                                printf("\n 5.Exit");
				printf("\n Enter your choice in number: ");
				scanf("%d",&choice);
				if(choice >5)
				{
				  printf("\n Wrong Input Again Choice");
				  continue;
				}
		
				if(5==choice)
				{
				  printf("\n Program now exit");
				  break;
				}

				switch(choice)
				{
					   
					  case 1:  /*
                                                   printf("\n Enter time delay for accept operation of coin escrow in seconds in numeric form [must be greater than zero]: ");
						   scanf("%d",&delay);
						   if(delay>=0)
						      PollFromCoinEscrow();
						   else
						      printf("\n Wrong delay time input try again. "); 
                                                    
                                                   */
                                                   PollFromCoinEscrow();
						   break;

					  case 2:  
                                                   /*

                                                   printf("\n Enter time delay for accept operation of coin escrow in seconds in numeric form [must be greater than zero]: ");
						   scanf("%d",&delay);
						   if(delay>=0)
						      //AcceptFromCoinEscrow_Ver2(delay);
                                                      
						   else
						      printf("\n Wrong delay time input try again. "); 
                                                   
                                                   */
                                                   AcceptFromCoinEscrow();
						   break;

					  case 3:  /*
                                                   printf("\n Enter time delay for reject operation coin escrow in seconds in numeric form [must be greater than zero]: ");
						   scanf("%d",&delay);
						   if(delay>=0)
						      //RejectFromCoinEscrow_Ver2(delay);
                                                      
						   else
						      printf("\n Wrong delay time input try again. "); 
                                                   */
                                                   RejectFromCoinEscrow();
						   break;

				          case 4:  /*
                                                   printf("\n Enter time delay for accept operation of coin escrow in seconds in numeric form [must be greater than zero]: ");
						   scanf("%d",&delay);
						   if(delay>=0)
						      PollFromCoinEscrow();
						   else
						      printf("\n Wrong delay time input try again. "); 
                                                    
                                                   */
                                                   while(1)
                                                   {
                                                        PollFromCoinEscrow();
                                                        cctalk_delay_mSec(200);
                                                   }
						   break;
					  default: printf("\n Wrong choice again input.");
						   break;

				}
			      
			       
		
			  }//while end

			  return 0;



}

int AcceptFromCoinEscrow_Ver2(int delay)
{

		
                

                unsigned char * rcvPkt = (unsigned char *)malloc(COINACPTR_MAX_LEN_OF_PKT);
		if(rcvPkt == NULL)
		return FAIL;
		memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);
		unsigned int  rcvPktLen=0;
		int ret=0;
                int i=0;
                unsigned char log[100];
                memset(log,'\0',100);
                     
		//clear serial port transmit and rece buffer // wait for lst pkt to complete the recv/trnsmt
		FlushSerialPortOutBuff_C(g_cctalkhandler);

		FlushSerialPortInBuff_C(g_cctalkhandler);

		//Send accept command to escrow device
		MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow();

		//cctalk_Wait(delay); //wait for solenoid work complete
		
                //Then read command reply bytes
		ret =  MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME);

		
                if(                       ( 0x01 == rcvPkt[5])                               && 
                                          ( 0x00 == rcvPkt[6] )                              && 
                                          ( 0x53 == rcvPkt[7] )                              &&
                                          ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )    && 
                                          ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                 )
		{


				//writeFileLog("[AcceptFromCoinEscrow_Ver2()] Accept coin Successfully done.");

		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[AcceptFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 //writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return SUCCESS;

		}
		else
		{

				//writeFileLog("[AcceptFromCoinEscrow_Ver2()] Accept coin failed.");
		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[AcceptFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 //writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return FAIL;

		}



}//AcceptFromCoinEscrow_Ver2() end here

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int RejectFromCoinEscrow_Ver2(int delay)
{

	
                     unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 
                     memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 
	             unsigned int  rcvPktLen=0;
                     int ret=0;
                     unsigned char log[100];
                     memset(log,'\0',100);
                     int i=0;

                     //clear serial port transmit and recv buffer	
		     FlushSerialPortOutBuff_C(g_cctalkhandler);  
		     FlushSerialPortInBuff_C(g_cctalkhandler);   
                     
                     //writeFileLog("[RejectFromCoinEscrow_Ver2()] Before Issue Reject Command.");

                     //Send reject command to escrow device
		     MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow();

                     //writeFileLog("[RejectFromCoinEscrow_Ver2] After Issue Reject Command.");
                    
                     //memset(log,'\0',100);

                     //sprintf(log,"[RejectFromCoinEscrow_Ver2()] Wait Time = %d Seconds.",COIN_ESCROW_REPLY_WAIT_TIME);

                     ////writeFileLog(log);
					 
                     //wait for solenoid work complete
                     //cctalk_Wait( delay ); 

                     //Then read command reply bytes
		     ret = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen,CCTALK_REPLY_WAIT_TIME);
		     
                     if(                       ( 0x01 == rcvPkt[5] )                             && 
                                               ( 0x00 == rcvPkt[6] )                             && 
                                               ( 0x53 == rcvPkt[7] )                             &&
                                               ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )   && 
                                               ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                       )
	             {

				 //writeFileLog("[RejectFromCoinEscrow_Ver2()] Reject Successfully done.");

		                 for(i=0;i<10;i++)
		                 {

                                         memset(log,'\0',100);

				         sprintf(log,"[RejectFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);

					 //writeFileLog(log);

					 
		                 }

		                 return SUCCESS; 

			 
	             }
		     else
		     {

				 //writeFileLog("[RejectFromCoinEscrow_Ver2)] Reject Coin failed. ");

		                 for(i=0;i<10;i++)
		                 {

                                         memset(log,'\0',100);

				         sprintf(log,"[RejectFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);

					 //writeFileLog(log);
					 
		                 }

		                 return FAIL;  


		     }   


       
}//RejectFromCoinEscrow_Ver2() end here


#endif  //ATVM_ESCORW_MAIN endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //++CCTALK endif
