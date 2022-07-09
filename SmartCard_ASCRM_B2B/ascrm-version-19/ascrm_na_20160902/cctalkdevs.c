 /*
  All CCTALK Device Code : NRI Coin Acceptor
                           NRI Coin Escrow
                           Atvm Door/Gate/Alarm Status
                           Atvm Electronics Vault Open/Close
 */ 

 #include "cctalkdevs.h"


 #ifdef CCTALK_DEVICE


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //Global Variables List

 static int   g_cctalkhandler=-1;

 static int   g_CCTALK_OpenFlag=false;

 static volatile int   g_state_C;

 static volatile int   g_poll_flag_C;

 static unsigned char g_rcvPkt_C[COINACPTR_MAX_LEN_OF_PKT];

 static int  g_rcvPktLen_C;

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



//////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////

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

static pthread_mutex_t  g_cctalkopenflagmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_cctalkporthandlemutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_getsecuredstausmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  g_getdisablealarmmutex = PTHREAD_MUTEX_INITIALIZER;


static bool g_CADisableAcceptanceFlag;

static pthread_mutex_t g_CADisableAcceptanceFlagMutex= PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//malay add 31 jan 2014
int IsCCTALKPortOpen()
{

          int rtcode=-1;

          pthread_mutex_lock( &g_cctalkopenflagmutex );

	  if(true == g_CCTALK_OpenFlag)
          {
                rtcode = 1;

          }
	  else if(false == g_CCTALK_OpenFlag)
          {
                rtcode = 0;
          }

          pthread_mutex_unlock( &g_cctalkopenflagmutex );

          return rtcode;


}



int ActivateCCTALKPort(int portNmbr)
{


          int cctalkopenrtcode =-1,comporthandler=-1;

          cctalkopenrtcode = OpenPort_C(portNmbr,&comporthandler);



	  if( cctalkopenrtcode < 0 ) //port open failed
	  {
                  pthread_mutex_lock( &g_cctalkopenflagmutex );
                  g_CCTALK_OpenFlag=false;    
                  pthread_mutex_unlock( &g_cctalkopenflagmutex );
		  writeFileLog("[ActivateCCTALKPort()] CCTALK Open Port failed.");
                  return 0; 

	  } 
	  else if( 1 == cctalkopenrtcode ) //port open successed
	  {  

                  pthread_mutex_lock( &g_cctalkopenflagmutex );
                  g_CCTALK_OpenFlag=true;    
                  pthread_mutex_unlock( &g_cctalkopenflagmutex );

                  pthread_mutex_lock( &g_cctalkporthandlemutex);
                  g_cctalkhandler = comporthandler;
                  pthread_mutex_unlock( &g_cctalkporthandlemutex);

                  writeFileLog("[ActivateCCTALKPort()] CCTALK Open Port opened successfully.");
                  return 1; 


          }


}




int DeActivateCCTALKPort()
{


           int rtcode=-1;

           pthread_mutex_lock( &g_cctalkporthandlemutex);
	   rtcode=ClosePort_C(g_cctalkhandler);  
           pthread_mutex_unlock( &g_cctalkporthandlemutex);

	   if(SUCCESS==rtcode)
	   {

                      writeFileLog("[OpenCoinAcptr()] Close CCTALK Port successed.");

                      pthread_mutex_lock( &g_cctalkopenflagmutex );
		      g_CCTALK_OpenFlag=false;
                      pthread_mutex_unlock( &g_cctalkopenflagmutex );

                      pthread_mutex_lock( &g_cctalkporthandlemutex);
                      g_cctalkhandler = -1;
                      pthread_mutex_unlock( &g_cctalkporthandlemutex);

		      return 1;

	   }
	   else
           {
                      writeFileLog("[OpenCoinAcptr()] Close CCTALK Port failed.");
	              return 0;

           }


}



int GetCoinAcptrStatus()
{
         
       
         return ( CoinAcceptorSimplePoll() );


}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int CoinAcceptorSimplePoll()
{


         ////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	 ret = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

         pthread_mutex_unlock( &g_cctalkporthandlemutex);

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
         //#ifdef COINACCPTEOR_DEBUG

         unsigned char log[100];

         int Counter=0;

         writeFileLog("=========================================================================");

         memset(log,'\0',100); 

         sprintf(log,"[CoinAcceptorSimplePoll()] Recv Packet Length=%d .",rcvPktLen);

         writeFileLog(log);

         for(Counter;Counter<rcvPktLen;Counter++)
         {


		 memset(log,'\0',100);
 
		 sprintf(log,"[CoinAcceptorSimplePoll()] rcvPkt[%d] = 0x%x h.",Counter,rcvPkt[Counter]);

		 writeFileLog(log);


         }

         writeFileLog("=========================================================================");

         //#endif

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////

         if( (10 == rcvPktLen) && ( 0x01 == rcvPkt[5]) && (0x00 == rcvPkt[6]) && (0x02 == rcvPkt[7]) &&
             (0x00 == rcvPkt[8]) && (0xFD == rcvPkt[9] ) )
         {

               writeFileLog("[CoinAcceptorSimplePoll()] Coin Acceptor Simple Poll Success.");
               return 1;

         }
         else
         {

               writeFileLog("[CoinAcceptorSimplePoll()] Coin Acceptor Simple Poll Failed.");
               return 0;

         }
       
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////


}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Add By Malay 3 March 2013
/*

 2= already closed
 3= failed to disable coin acptr
 1= successfully closed
 0= failed to closed note acptr 

*/


int CloseCoinAcptr()
{
          
     
           pthread_mutex_lock( &g_cctalkopenflagmutex );
  
           //check port already closed or not
           if( false == g_CCTALK_OpenFlag )
           {
                  pthread_mutex_unlock( &g_cctalkopenflagmutex );
                  writeFileLog("[OpenCoinAcptr()] Error CCTALK Port not opened !!!.");
                  return 2;

           }

           pthread_mutex_unlock( &g_cctalkopenflagmutex );

           //Disable all coin acceptance before close operation in coin acceptor[safety]
	   return( DisableAllCoins() );
	   
	   


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//function Add By Malay on 30 Jan 2013

//Disable All coins
int DisableAllCoins()
{


	  unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT];
	  memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 
	  int  rcvPktLen=0;
	  coinType COIN;
	  COIN.rs_50  = 0;
	  COIN.rs_1   = 0;
	  COIN.rs_2   = 0;
	  COIN.rs_5   = 0;
	  COIN.rs_10  = 0;
          
          #ifdef COINACCPTEOR_DEBUG
          writeFileLog("[DisableAllCoins()] Going to disable all Coin .");
          #endif

	  MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 

	  if(MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen) == SUCCESS)
	  {      

		     if((rcvPkt[3] != 0xE7) || (rcvPkt[11] !=0xFD)) 
		     {

			  //#ifdef COINACCPTEOR_DEBUG
		          writeFileLog("[DisableAllCoins()] failed to disable all Coin .");
		          //#endif
		          return FAIL;

		     }
		     else
		     {
			    //#ifdef COINACCPTEOR_DEBUG 
		            writeFileLog("[DisableAllCoins()] successfully disable all Coin .");
		            //#endif
		            return SUCCESS;
		     }


	  }
	  else
          { 
			//#ifdef COINACCPTEOR_DEBUG
		        writeFileLog("[DisableAllCoins()] MONEYCONTROL_CoinAcptr_statusRead() return failed .");
		        //#endif
		        return FAIL;

          }
  


}//end



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Run time coin inhibit 
inline int EnableSpecificCoins(int currentfare)
{

          
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

          if(currentfare > 10)
	  {
	     #ifdef COINACCPTEOR_DEBUG
             writeFileLog("[EnableSpecificCoins()]  Greater than 10.");
             #endif
             //COIN.rs_1   = 1;
	     COIN.rs_5   = 1;
	     COIN.rs_10  = 1;
	  }
	  //Range : 1-10
	  else if(10==currentfare)
	  {
	     #ifdef COINACCPTEOR_DEBUG
             writeFileLog("[EnableSpecificCoins()]  Equal to 10.");
             #endif
             //COIN.rs_1   = 1;
	     COIN.rs_5   = 1;
	     COIN.rs_10  = 1;
	  }
	  //Range: 5,6,7,8,9
	  else if (currentfare < 10 && currentfare >=5 )
	  {
	     #ifdef COINACCPTEOR_DEBUG
             writeFileLog("[EnableSpecificCoins()]  less than 10 but greter or equal to 5.");
             #endif
             //COIN.rs_1   = 1;
	     COIN.rs_5   = 1;
	     COIN.rs_10  = 0;
	  } 
	  //Range: 1,2,3,4
	  else if (currentfare < 5 && currentfare >=1 )
	  {
	     #ifdef COINACCPTEOR_DEBUG
             writeFileLog("[EnableSpecificCoins()]  less than 5 but greter or equal to 1.");
             #endif
             //COIN.rs_1   = 1;
	     COIN.rs_5   = 0;
	     COIN.rs_10  = 0;
	  } 
	  else if(0==currentfare)
          {
	      #ifdef COINACCPTEOR_DEBUG
              writeFileLog("[EnableSpecificCoins()] MONEYCONTROL_CoinAcptr_statusRead() Zero fare receieved.");
              #endif
              return FAIL;
          }
          
          #ifdef COINACCPTEOR_DEBUG
          writeFileLog("[EnableSpecificCoins()] Going to enable Specific Coin ");
          #endif

          ///////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &g_cctalkporthandlemutex);

	  MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 

          rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

          pthread_mutex_unlock( &g_cctalkporthandlemutex);

          ////////////////////////////////////////////////////////////////////////////////////

          if( SUCCESS ==  rtcode )
          {  
    
                     if((rcvPkt[3] != 0xE7) || (rcvPkt[11] !=0xFD)) 
		     {
			  #ifdef COINACCPTEOR_DEBUG
		          writeFileLog("[EnableSpecificCoins()] Specific Coin enable failed."); 
		          #endif
		          return FAIL;
		     }
		     else
		     {
			  #ifdef COINACCPTEOR_DEBUG
		          writeFileLog("[EnableSpecificCoins()] Successfully Specific Coin Enable.");
		          #endif
		          return SUCCESS;
		     }


	  }
	  else 
          {
                       
		       #ifdef COINACCPTEOR_DEBUG
		       writeFileLog("[EnableSpecificCoins()] MONEYCONTROL_CoinAcptr_statusRead() return failed");  
		       #endif
		       return FAIL;
          }

  
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Run time coin inhibit 
inline int DisableSpecificCoins(int currentfare)
{


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
          
          char log[100];
          memset(log,'\0',100);

          #ifdef  COINACCPTEOR_DEBUG
          sprintf(log,"[DisableSpecificCoins()] Current fare : Rs. %d.", currentfare);  
          writeFileLog(log);
          #endif

          //Greater than 10
	  if ( currentfare > 10)
          {
		        #ifdef  COINACCPTEOR_DEBUG
		        writeFileLog("[DisableSpecificCoins()]  Greater than 10.");
                        #endif

		        //COIN.rs_1   = 1;
			COIN.rs_5   = 1;
			COIN.rs_10  = 1;
	  }

          //Range : 1-10 
	  else if(  10 == currentfare )
	  {
		     #ifdef  COINACCPTEOR_DEBUG
		     writeFileLog("[DisableSpecificCoins()]  Equal to 10.");
                     #endif

		     //COIN.rs_1   = 1;
		     COIN.rs_5   = 1;
		     COIN.rs_10  = 1;
	  }
	  //Range: 5,6,7,8,9
	  else if (currentfare < 10 && currentfare >=5 )
	  {
		     #ifdef  COINACCPTEOR_DEBUG
                     writeFileLog("[DisableSpecificCoins()]  less than 10 but greter or equal to 5.");
                     #endif

		     //COIN.rs_1   = 1;
		     COIN.rs_5   = 1;
		     COIN.rs_10  = 0;
	  } 
	  //Range: 1,2,3,4
	  else if (currentfare < 5 && currentfare >=1 )
	  {
		     #ifdef  COINACCPTEOR_DEBUG
                     writeFileLog("[DisableSpecificCoins()]  less than 5 but greter or equal to 1.");
                     #endif
		     //COIN.rs_1   = 1;
		     COIN.rs_5   = 0;
		     COIN.rs_10  = 0;
	  } 
          //equal fare
	  else  if ( 0 == currentfare)
          {

	      //writeFileLog("[DisableSpecificCoins()]  Equal fare state found.");
              //writeFileLog("[DisableSpecificCoins()]  Now going to disable all coins.");

              //#ifdef  COINACCPTEOR_DEBUG
              writeFileLog("[DisableSpecificCoins()]  Zero fare receieved.");
              //#endif

              return;

          }
          
          #ifdef  COINACCPTEOR_DEBUG
          writeFileLog("[DisableSpecificCoins()] Going to disable Specific Coin ");
	  #endif

          /////////////////////////////////////////////////////////////////////////////////////////
          
          pthread_mutex_lock( &g_cctalkporthandlemutex);

          MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 

          rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

          pthread_mutex_unlock( &g_cctalkporthandlemutex);

          /////////////////////////////////////////////////////////////////////////////////////////

          if(  SUCCESS ==  rtcode )
	  {      
		     if((rcvPkt[3] != 0xE7) || (rcvPkt[11] !=0xFD)) 
		     {
			  #ifdef  COINACCPTEOR_DEBUG
                          writeFileLog("[DisableSpecificCoins()] Specific Coin Disable failed.");
                          #endif
		          return FAIL;
		     }
		     
		     else
		     {
			  #ifdef  COINACCPTEOR_DEBUG
                          writeFileLog("[DisableSpecificCoins()] Successfully Specific Coin Disable.");
                          #endif
		          return SUCCESS;
		     }

	  }
	  else 
          {
		     #ifdef  COINACCPTEOR_DEBUG
                     writeFileLog("[DisableSpecificCoins()] MONEYCONTROL_CoinAcptr_statusRead() return failed."); 
                     #endif 
		     return FAIL;
          }

  


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//set All Coin Inhibit status to true
inline void SetAllCoinInhibitStatus()
{

         
	   pthread_mutex_lock(&CoinInhibitFlagmutex);

	   g_CoinInhibitFlag=1;

	   pthread_mutex_unlock(&CoinInhibitFlagmutex);

           return;


}//end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//set All Coin Inhibit status to true
inline void SetSpecificCoinInhibitStatus(int fare)
{


	    pthread_mutex_lock(&CASetSpecificFaremutex);

	    g_SpecificCoinInhibitFlag=1;

	    g_CACurrentAtvmFare=fare;

	    /*

	    char log[100];

	    memset(log,'\0',100);

	    sprintf(log,"[SetSpecificCoinInhibitStatus()] fare: Rs. %d .",g_CACurrentAtvmFare); 

	    writeFileLog(log);

	    */

	    pthread_mutex_unlock(&CASetSpecificFaremutex);

	    return;


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Get Low Level Poll Flag Status
inline unsigned int GetCoinAcptrLowLevelFlagStatus()
{


	    int CoinLowLevelPollFlagStatus=0;

	    pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

	    CoinLowLevelPollFlagStatus=g_CoinLowLevelPollFlagStatus;

	    pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

	    return (CoinLowLevelPollFlagStatus);



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int WaitforCoinAcptrLowlevelflagClosed(double waittime)
{

		   
		 struct timespec begints={0}, endts={0},diffts={0};

                 int rtcode=-1;

                 #ifdef  COINACCPTEOR_DEBUG

		 writeFileLog("[WaitforCoinAcptrLowlevelflagClosed()]  Get Low level flag status starting..");

                 #endif

		 unsigned char log[100];

                 memset(log,'\0',100);
 
                 pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

		 //if polling flag is on condition (on=1)
		 if(1==g_CoinLowLevelPollFlagStatus)
		 {
		  

                           pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

			   clock_gettime(CLOCK_MONOTONIC, &begints);
			 
			   //Wait here untill polling flag is off (off=0)
			   for(;;)
			   {

				     clock_gettime(CLOCK_MONOTONIC, &endts);

			             diffts.tv_sec = endts.tv_sec - begints.tv_sec;
				     
				     //if timer is running
				     if(diffts.tv_sec<waittime)
				     {

                                                pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

						if(0==g_CoinLowLevelPollFlagStatus) 
						{

                                                          pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

							  #ifdef  COINACCPTEOR_DEBUG

				                          writeFileLog("[WaitforCoinAcptrLowlevelflagClosed()] Low level flag status is closed.");
				                          memset(log,'\0',100); 

						          sprintf(log,"[WaitforCoinAcptrLowlevelflagClosed()]  Seconds : %d.",diffts.tv_sec);

						          writeFileLog(log);

				                          #endif
							  return 1;

						}
                                                else
                                                {
						      pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

                                                }

				     }
				     //if timer time is end or timer is running
				     if(diffts.tv_sec>=waittime)
				     {

                                                pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

						if( 0==g_CoinLowLevelPollFlagStatus)
						{

                                                          pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

							  #ifdef  COINACCPTEOR_DEBUG

		                                          writeFileLog("[WaitforCoinAcptrLowlevelflagClosed()] Low level flag status is closed.");
		                                          memset(log,'\0',100); 

				                          sprintf(log,"[WaitforCoinAcptrLowlevelflagClosed()]  Seconds : %d.",diffts.tv_sec);

				                          writeFileLog(log);

		                                          #endif

							  return 1;

						}
						else
						{ 

                                                         pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

							 #ifdef  COINACCPTEOR_DEBUG

		                                         writeFileLog("[WaitforCoinAcptrLowlevelflagClosed()] Low level flag status is not closed during time delay wait.");
		                                         memset(log,'\0',100); 

				                         sprintf(log,"[WaitforCoinAcptrLowlevelflagClosed()]  Seconds : %d.",diff);

				                         writeFileLog(log);

		                                         #endif

							 return 0;
                                           
						}

						

				     }//timer if block end here
					      

			   }//for loop end
		   
		 }//if thread status start end here

                 pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

                 //////////////////////////////////////////////////////////////////////////////////////////////////////

                 pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

		 if(0==g_CoinLowLevelPollFlagStatus)
		 {

                      pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

		      #ifdef  COINACCPTEOR_DEBUG
                      writeFileLog("[WaitforCoinAcptrLowlevelflagClosed()] Already Low level flag status is closed.");
                      #endif
		      return 1;

		 }

                 pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////




}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void ClearCCTALKBuffer()
{

 
      pthread_mutex_lock( &g_cctalkporthandlemutex);

      FlushSerialPortOutBuff_C(g_cctalkhandler);  

      FlushSerialPortInBuff_C(g_cctalkhandler);  

      pthread_mutex_unlock( &g_cctalkporthandlemutex);

      return;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int enableSpecificCoinsAndMakeIdle(int fare,int maxCoinQtyAllowed)
{

	  
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
          writeFileLog(log);

          //#endif

	 //Analysis if exact fare is given
         #ifdef  COINACCPTEOR_DEBUG
         writeFileLog("[enableSpecificCoinsAndMakeIdle()] Before Exact fare analysis block.");
         #endif

	 if( 1 == maxCoinQtyAllowed )
	 {

		   //#ifdef  COINACCPTEOR_DEBUG
                   writeFileLog("[enableSpecificCoinsAndMakeIdle()] Exact fare block Active [Coin:1].");
                   //#endif

                   switch(fare)
		   {
			   
			     case 1:
			          //COIN.rs_1   = ENABLE;
                                  //#ifdef  COINACCPTEOR_DEBUG
                                  //writeFileLog("[enableSpecificCoinsAndMakeIdle()  Exact fare block] Rs. 1 only enable.");
                                  //#endif
			          break;
                          
			     case 5:
			          COIN.rs_5   = ENABLE;
                                  //#ifdef  COINACCPTEOR_DEBUG
                                  writeFileLog("[enableSpecificCoinsAndMakeIdle() Exact fare block] Rs. 5 only enable.");
                                  //#endif
			          break;

			     case 10:
			          COIN.rs_10  = ENABLE;
                                  //#ifdef  COINACCPTEOR_DEBUG
                                  writeFileLog("[enableSpecificCoinsAndMakeIdle() Exact fare block] Rs. 10 only enable.");
                                  //#endif
			          break;

			     default: writeFileLog("[enableSpecificCoinsAndMakeIdle() Exact fare block] No Exact fare given.");
                                      break;

	      } 
	   }

           else if(  maxCoinQtyAllowed >1  )//Exact fare not given
           {
                  

                  //#ifdef  COINACCPTEOR_DEBUG
                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Max Coin Block Active [Coin:20].");
                  //#endif
                  
                  if(fare >= 1)
                  {
                        //COIN.rs_1     = ENABLE;
                        //#ifdef  COINACCPTEOR_DEBUG
                        //writeFileLog("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 1 Enable.");
                        //#endif
                  }
                 
                  if(fare >= 5)
                  {
                        COIN.rs_5     = ENABLE;
                        //#ifdef  COINACCPTEOR_DEBUG
                        writeFileLog("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 5 Enable.");
                        //#endif
                  }
                  if(fare >= 10)
                  {
                        COIN.rs_10    = ENABLE;
                        
                        //#ifdef  COINACCPTEOR_DEBUG
                        writeFileLog("[enableSpecificCoinsAndMakeIdle() Max Coin Block] Rs. 10 Enable.");
                        //#endif
                  }
                
           }
           
           #ifdef  COINACCPTEOR_DEBUG
	   writeFileLog("[enableSpecificCoinsAndMakeIdle() ] After Exact fare analysis block.");
           #endif


          ////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &g_cctalkporthandlemutex);

          //Now enable coin status

          rtcode=-1;

	  MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(COIN); 

	  rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen); 

          if( SUCCESS == rtcode  )
	  {   
               writeFileLog("[enableSpecificCoinsAndMakeIdle()] Enable coin success.");
          }
          else
          {
               writeFileLog("[enableSpecificCoinsAndMakeIdle()] Enable coin failed.");

          }


          pthread_mutex_unlock( &g_cctalkporthandlemutex);


          /////////////////////////////////////////////////////////////////////////////////
	  

          //Malay Add 29 March 2013
	  cctalk_delay_mSec(100); 
          
          //To Hold the Last Event Nmbr. & To Status Chk of acptr.

          //#ifdef  COINACCPTEOR_DEBUG
          writeFileLog("[enableSpecificCoinsAndMakeIdle()] Get last event counter block.");
          //#endif

          int CommandReplyLength=0;

          ////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &g_cctalkporthandlemutex);

	  MONEYCONTROL_CoinAcptr_Cmd_CreditPoll();  

          rtcode=-1;

          rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

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
                              writeFileLog("[enableSpecificCoinsAndMakeIdle()] Store last event counter success.");
                              //#endif
                              
                              memset(log,'\0',200);

			      sprintf(log,"[enableSpecificCoinsAndMakeIdle()] Event Counter = %d .", g_prevEvent_C );

			      writeFileLog(log);  
   
			      return(SUCCESS);

		    }
                    else
                    {
                             pthread_mutex_unlock(&PrevEventMutuex);

                             writeFileLog("[enableSpecificCoinsAndMakeIdle()] Credit Poll Command Reply Packet mismatch error.");
                             return(FAIL);

                    }


	  }
          else
          {
		       //#ifdef  COINACCPTEOR_DEBUG
		       writeFileLog("[enableSpecificCoinsAndMakeIdle()] Store last event counter failed.");
		       //#endif
		       return(FAIL);

          }


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Start Coin Acceptor Credit Polling Thread
unsigned int MONEYCONTROL_CoinAcptrCreateCreditPoll()
{

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void  CoinAcptrStartSignalCreditPollThread()
{

          writeFileLog("[CoinAcptrStartSignalCreditPollThread()] Entry.");

          ////////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

          g_CloseCoinCreditPollingThreadFlag =0;
 
          pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

          ////////////////////////////////////////////////////////////////////////////////////

 
          pthread_mutex_lock( &g_CAEnableThreadmutex );

          pthread_cond_signal( &g_CAEnableThreadCond );

          pthread_mutex_unlock( &g_CAEnableThreadmutex );

          ////////////////////////////////////////////////////////////////////////////////////

          writeFileLog("[CoinAcptrStartSignalCreditPollThread()] Exit.");

          return;


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void  CoinAcptrStopSignalCreditPollThread()
{


         writeFileLog("[CoinAcptrStopSignalCreditPollThread()] Entry.");

         ////////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

          g_CloseCoinCreditPollingThreadFlag =1;
 
          pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

          ////////////////////////////////////////////////////////////////////////////////////
 
          pthread_mutex_lock( &g_CAEnableThreadmutex );

          pthread_cond_signal( &g_CAEnableThreadCond );

          pthread_mutex_unlock( &g_CAEnableThreadmutex );

          ////////////////////////////////////////////////////////////////////////////////////

          writeFileLog("[CoinAcptrStopSignalCreditPollThread()] Exit.");

          return;


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void InitCATransModel()
{



      writeFileLog("[InitCATransModel()] Entry.");

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&CoinInhibitFlagmutex);

      g_CoinInhibitFlag = 0;

      pthread_mutex_unlock(&CoinInhibitFlagmutex);

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&CASpecificInhibitmutex);

      g_CACurrentAtvmFare=0;

      g_SpecificCoinInhibitFlag=0;

      pthread_mutex_unlock(&CASpecificInhibitmutex);

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&NaCoinInhibitFlagmutex);

      g_NaCoinInhibitFlag=0;

      pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&NaCoinInhibitFlagmutex);

      g_Nafaretobeaccept=0;

      pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&CAEscrowFlagStatusmutex);

      g_state_C = 0x00;

      pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&CoinPollFlagMutex);

      g_poll_flag_C = OFF; 
 
      pthread_mutex_unlock(&CoinPollFlagMutex);

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&GlobalRecvpacketMutuex);

      int i=0;

      for(i = 0; i<COINACPTR_MAX_LEN_OF_PKT; i++)
      {

          g_rcvPkt_C[i] = 0;

      } 

      pthread_mutex_unlock(&GlobalRecvpacketMutuex);

      ////////////////////////////////////////////////////

      pthread_mutex_lock(&RecvPktLengthMutuex);

      g_rcvPktLen_C=0;

      pthread_mutex_unlock(&RecvPktLengthMutuex);

      ////////////////////////////////////////////////////

      writeFileLog("[InitCATransModel()] Exit.");

      return;



}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//set All Coin Inhibit status to true
inline void NaCoinEnableSet(int fare)
{

    pthread_mutex_lock(&NaCoinInhibitFlagmutex);

    g_NaCoinInhibitFlag=1;

    g_Nafaretobeaccept=fare;  

    pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

    return;

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Delay or Disable all coin before doing poll
static inline void CoinIntelligentInhibit()
{


           int CACurrentAtvmFare=0;

           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           pthread_mutex_lock(&CoinInhibitFlagmutex);

	   if( 1==g_CoinInhibitFlag )
	   {

		      g_CoinInhibitFlag = 0;  

                      pthread_mutex_unlock(&CoinInhibitFlagmutex);

                      writeFileLog("[CoinIntelligentInhibit()] Before set disable coin acceptance.");

	              if (SUCCESS == DisableAllCoins() )
		      {
		                      
		                       

		                        pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );

		                        g_CADisableAcceptanceFlag =true ;

		                        pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );

		                        

		      }

		      writeFileLog("[CoinIntelligentInhibit()] After set disable coin acceptance.");

                      return;

	   }

           pthread_mutex_unlock(&CoinInhibitFlagmutex);

           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           pthread_mutex_lock(&CASpecificInhibitmutex);

	   if(1==g_SpecificCoinInhibitFlag)  
	   {

		       g_SpecificCoinInhibitFlag=0;

		       CACurrentAtvmFare = g_CACurrentAtvmFare;

		       pthread_mutex_unlock(&CASpecificInhibitmutex); 

		       DisableSpecificCoins(CACurrentAtvmFare);
		       
		       return;

	   }

           pthread_mutex_unlock(&CASpecificInhibitmutex); 

           ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           pthread_mutex_lock(&NaCoinInhibitFlagmutex);

           if(1==g_NaCoinInhibitFlag)  
	   {
                         
			 g_NaCoinInhibitFlag=0;     

                         CACurrentAtvmFare=g_Nafaretobeaccept;

		         pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

		         EnableSpecificCoins(CACurrentAtvmFare);

		         return;


	   }

	   pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           return;


}


/////////////////////////////////////////////////////////////////////////////////////////////////////////



bool GetCADisableAcceptanceSignal()
{

       bool rtcode=false;

       pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );

       rtcode = g_CADisableAcceptanceFlag ;

       pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );

       return rtcode;


}


//Delay Between two consecutive coin acceptor credit polling command 
static inline void  CoinIntelligentDelay(unsigned int delay)
{


      struct timespec req = {0};

      req.tv_sec = 0;

      //req.tv_nsec = 1* 1000000L; //1ms delay

      req.tv_nsec = 10* 1000000L; //10ms delay

      int CACurrentAtvmFare=0;

      for(;delay>=1; delay--)
      {



                   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

                   pthread_mutex_lock(&CoinInhibitFlagmutex);

		   if(1==g_CoinInhibitFlag)
		   {
 
			      g_CoinInhibitFlag = 0; 
	 
		              pthread_mutex_unlock(&CoinInhibitFlagmutex);

		              writeFileLog("[CoinIntelligentDelay()] Before set disable coin acceptance.");

			      if (SUCCESS == DisableAllCoins() )
		              {
		                      
		                       

		                        pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );

		                        g_CADisableAcceptanceFlag =true ;

		                        pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );

		                        

		              }

		              writeFileLog("[CoinIntelligentDelay()] After set disable coin acceptance.");

			      return;

		   }

                   pthread_mutex_unlock(&CoinInhibitFlagmutex);

                   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

                   pthread_mutex_lock(&CASpecificInhibitmutex);

		   if( 1 == g_SpecificCoinInhibitFlag )  
		   {
		       
		       g_SpecificCoinInhibitFlag=0;

		       CACurrentAtvmFare = g_CACurrentAtvmFare;

                       pthread_mutex_unlock(&CASpecificInhibitmutex);

                       DisableSpecificCoins(CACurrentAtvmFare);
                      
                       
                       clock_nanosleep( CLOCK_MONOTONIC,
				        0,
				        &req,
				        NULL
				       );
                  
                       continue;

		   }

                   pthread_mutex_unlock(&CASpecificInhibitmutex);

                   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

                   pthread_mutex_lock(&NaCoinInhibitFlagmutex);

		   if(1==g_NaCoinInhibitFlag)  
		   {

			 g_NaCoinInhibitFlag=0;  
                       
                         pthread_mutex_unlock(&NaCoinInhibitFlagmutex);    
    
                         EnableSpecificCoins(g_Nafaretobeaccept);

                        
                         clock_nanosleep(CLOCK_MONOTONIC,
				         0,
				         &req,
				         NULL
				        );
                         continue;
 
		   }

		   pthread_mutex_unlock(&NaCoinInhibitFlagmutex);

                   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		   clock_nanosleep( CLOCK_MONOTONIC,
				    0,
				    &req,
				    NULL
				  );


       } 

       return;


}//end




//malay 8 april 2013
void GetCoinDetailInEscrow (int value[], int *coinStatus)
{


	   #ifdef  COINACCPTEOR_DEBUG 
           writeFileLog("[GetCoinDetailInEscrow()] Entry.");
           #endif

           char log[100];

           memset(log,'\0',100);

	   int i=0,amnt[5];

           memset(amnt,0,5);

           memset(value,0,5);
            
           //init status
	   *coinStatus  = 0x00; 
 
           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           //writeFileLog("[GetCoinDetailInEscrow()] Before Check Escrow State.");

           pthread_mutex_lock(&CAEscrowFlagStatusmutex);

           if( COINACPTR_RES_ESCROW == g_state_C )
	   { 

                       pthread_mutex_lock(&GlobalRecvpacketMutuex);

                       MONEYCONTROL_CoinAcptr_coinAmountReceived(g_rcvPkt_C,amnt);     

                       pthread_mutex_unlock(&GlobalRecvpacketMutuex);
  
		       for(i = 0;i<5;i++)
                       {
		            value[i] = 0; 
                       }

		       i = 0;

		       while(g_nmbrOfCoinsAt_aTime>0)
		       {   


				  value[i] =  amnt[i];

				  memset(log,'\0',100);

		                  sprintf(log,"[GetCoinDetailInEscrow() Escrow] value[%d] : %d .", i,value[i]);

		                  writeFileLog(log);

		                  i++; 

				  g_nmbrOfCoinsAt_aTime = g_nmbrOfCoinsAt_aTime-1;   
		                          
                               
		       }

                       //set escrow state
                       *coinStatus =   g_state_C;

                       //clear escrow state for again poll
	               g_state_C=0x00;

                       pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

                       return;

	  }    
          else
          {
           
                      pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

          }

          //writeFileLog("[GetCoinDetailInEscrow()] After Check Escrow State.");

          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          #ifdef  COINACCPTEOR_DEBUG 

          writeFileLog("[GetCoinDetailInEscrow()] Exit.");

          #endif

	  return;   
 
	   
}//GetCoinDetailInEscrow() end


inline void  CloseCoinCreditPollingThread()
{


            //writeFileLog("[CloseCoinCreditPollingThread()] Entry .");

            /////////////////////////////////////////////////////////////////////////////////////

            //writeFileLog("[CloseCoinCreditPollingThread()] Before Poll Flag Close .");

	    pthread_mutex_lock(&CoinPollFlagMutex);

	    g_poll_flag_C = OFF;

	    pthread_mutex_unlock(&CoinPollFlagMutex);

            //writeFileLog("[CloseCoinCreditPollingThread()] After Poll Flag Close .");

            /////////////////////////////////////////////////////////////////////////////////////

            //writeFileLog("[CloseCoinCreditPollingThread()] Before Signal Close Poll Thared .");
 
            pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

	    g_CloseCoinCreditPollingThreadFlag=1;

	    pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

            //writeFileLog("[CloseCoinCreditPollingThread()] After Signal Close Poll Thared .");

            /////////////////////////////////////////////////////////////////////////////////////


            //writeFileLog("[CloseCoinCreditPollingThread()] Exit .");

	    return;


}//CloseCoinCreditPollingThread() end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Coin Acceptor Credit Polling Thread
static void* ThreadProc1_MONEYCONTROL_CoinAcptrPoll(void *ptr)
{   

              

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
      
             writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Before Wait Signal.");             

             //wait for signal
             pthread_mutex_lock( &g_CAEnableThreadmutex );

             pthread_cond_wait( &g_CAEnableThreadCond  , &g_CAEnableThreadmutex  );

             pthread_mutex_unlock( &g_CAEnableThreadmutex );

             writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] After Wait Signal.");

           //////////////////////////////////////////////////////////////////////////////////////////////////////////////

            pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);
            
            //if credit poll flag is set exit from thread
            if( 1 == g_CloseCoinCreditPollingThreadFlag )
            {
                     
                    g_CloseCoinCreditPollingThreadFlag=0;

                    pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

                    writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Thread Exit.");
				                   
                    pthread_exit(0);

	    }
            else
            {

                     pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

                     writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Thread Running.");


            }
 

          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 

          memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 

	  int  rcvPktLen=0,state=0,amnt=0;

          char log[100];

          memset(log,'\0',100);

          g_AlreadyCoinPollFlag=false;

          int rtcode=-1;

          /////////////////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &g_CADisableAcceptanceFlagMutex );

          g_CADisableAcceptanceFlag = false;

          pthread_mutex_unlock( &g_CADisableAcceptanceFlagMutex );

          /////////////////////////////////////////////////////////////////////////////////////

	  for(;;)
	  {         

		       
		     CoinIntelligentInhibit(); 

                     ///////////////////////////////////////////////////////////////////////////

                     pthread_mutex_lock(&CoinPollFlagMutex);

                     pthread_mutex_lock(&CAEscrowFlagStatusmutex);

		     if( ( ON == g_poll_flag_C ) &&  ( COINACPTR_RES_ESCROW != g_state_C ) )
		     {

		                //writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Credit Polling.");

                                ///////////////////////////////////////////////////////////////////////////

                                pthread_mutex_unlock(&CoinPollFlagMutex);

                                pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

                                ///////////////////////////////////////////////////////////////////////////
                                
                                g_AlreadyCoinPollFlag=false;
    
                                //Delay Between continous poll
                                CoinIntelligentDelay(COIN_ACPTR_POLL_DELAY);
                          
                                ///////////////////////////////////////////////////////////////////////////
       
                                pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

                                //Set Coin Low Level flag is running state
                                g_CoinLowLevelPollFlagStatus=1;

                                pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

                                //////////////////////////////////////////////////////////////////////////////
                                
                                //clear command reply array 
                                memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT);
         
                                //////////////////////////////////////////////////////////////////////////////

                                pthread_mutex_lock( &g_cctalkporthandlemutex);

                                rtcode =-1;

                                MONEYCONTROL_CoinAcptr_Cmd_CreditPoll(); 

                                rtcode = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

                                pthread_mutex_unlock( &g_cctalkporthandlemutex);

                                ///////////////////////////////////////////////////////////////////////////////
                    
                                if(  SUCCESS ==  rtcode )
				{ 
					            
                                                    #ifdef COINACCPTEOR_DEBUG

                                                    int Counter=0;

                                                    for(Counter=0;Counter<rcvPktLen;Counter++)
                                                    {
							    memset(log,'\0',100); 

							    sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] rcvPkt[%d]:0x%x h .",Counter,rcvPkt[Counter]);
							    writeFileLog(log);   
    
                                                    }

                                                    #endif

		                                   //Copy into Global var
						   CoinAcptrCopyReceivedPacketGlobally(rcvPkt,rcvPktLen);
		                                  
		                                   if( ( 0x01 == rcvPkt[5] )  && ( 0xB == rcvPkt[6] ) && 
		                                       ( 0x02 == rcvPkt[7] )  && ( 0x00 == rcvPkt[8] ) 
		                                     )
						   {

                                                              pthread_mutex_lock(&PrevEventMutuex);

							      //compare with previouse event counter with current event counter
		                                              if(rcvPkt[9]  != g_prevEvent_C)
							      {          
									
                                                                             
									//g_lostEvent = 0; 

									g_nmbrOfCoinsAt_aTime = 0; 
		                                                        
		                                                        //#ifdef  COINACCPTEOR_DEBUG

		                                                        writeFileLog("\n");

		                                                        memset(log,'\0',100);

		                                                        sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Previous Event : %d .", g_prevEvent_C);
		                                                        writeFileLog(log);

		                                                        memset(log,'\0',100);

		                                                        sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Current Event  : %d .", rcvPkt[9]);
		                                                        writeFileLog(log);
		                                                        
		                                                        memset(log,'\0',100);

		                                                        sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Current  Credit Code: %d .", rcvPkt[10]);
		                                                        writeFileLog(log);

		                                                        //#endif
		                                                        
		                                                        
		                                                       
		                                                        //check number of coins in credit polling
		                                                        //current event greater than prev event
		                                                        if( (rcvPkt[9] > g_prevEvent_C) && ((rcvPkt[9]-g_prevEvent_C) == 2) )
		                                                        {
		                                                             writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss event 2 (Current Event Greater than prev event block).");
		                                                             g_nmbrOfCoinsAt_aTime=2;
		                                                             
		                                                             memset(log,'\0',100);
		                                                             sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss Event Credit Code: %d", rcvPkt[12]);
		                                                             writeFileLog(log);

		                                                        }

		                                                        //prev event greater than current event
		                                                        else if( (rcvPkt[9] < g_prevEvent_C ) &&
		                                                                 ( ( (255-g_prevEvent_C) + rcvPkt[9] ) ==2 )
		                                                               )
		                                                        {
		                                                           
		                                                              writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss Event 2 (Current Event less than prev event block) .");
		                                                              g_nmbrOfCoinsAt_aTime=2;

		                                                              memset(log,'\0',100);

		                                                              sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Miss Event Credit Code: %d.", rcvPkt[12]);
		                                                              writeFileLog(log);

		                                                        }
		                                                        else
                                                                        {

		                                                           g_nmbrOfCoinsAt_aTime=1;

                                                                        }
		                                                       
		                                                        memset(log,'\0',100);

		                                                        sprintf(log,"[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Number of Coins in single poll: %d.", g_nmbrOfCoinsAt_aTime);
		                                                        writeFileLog(log);

		                                                       
		                                                        //store current event counter
								        g_prevEvent_C = rcvPkt[9];  

                                                                        pthread_mutex_unlock(&PrevEventMutuex);

                                                                        /////////////////////////////////////////////////

                                                                        pthread_mutex_lock(&CAEscrowFlagStatusmutex);

                                                                        //set coin acceptor escrow state and store coin information
		                                                        g_state_C = COINACPTR_RES_ESCROW;

                                                                        pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

                                                                        /////////////////////////////////////////////////



		                                                   } //event compare if block
                                                                   else
                                                                   {

                                                                         pthread_mutex_unlock(&PrevEventMutuex);

                                                                   }                                                                 


					     } //reply packet if block
                                             else
                                             {

                                                writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Coin Credit Reply Packet mismatch error!!!.");

                                             }


				  }//read success if block
				 
                                                                    
		      } //if(g_poll_flag_C == ON)
                      else 
                      { 

				        
                                         pthread_mutex_unlock(&CoinPollFlagMutex);

                                         pthread_mutex_unlock(&CAEscrowFlagStatusmutex);

                                         //////////////////////////////////////////////////////////////////////////////////////////

                                         pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

		                         //Set Coin low level flag state is closed
		                         g_CoinLowLevelPollFlagStatus=0; 

                                         if( ( 0 == g_CoinLowLevelPollFlagStatus ) && ( false == g_AlreadyCoinPollFlag ) )
		                         {

                                                     pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

				                     #ifdef  COINACCPTEOR_DEBUG
				                     writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Credit Poll Off.");
				                     #endif

				                     g_AlreadyCoinPollFlag=true;


		                         }
                                         else
                                         {
                                                    pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

                                         }

                                        
                                         //////////////////////////////////////////////////////////////////////////////////////////

                                         pthread_mutex_lock(&CloseCoinCreditPollingThreadFlagmutex);

				         //if credit poll flag is set exit from thread
				         if( 1== g_CloseCoinCreditPollingThreadFlag )
				         {
                                                      
						     g_CloseCoinCreditPollingThreadFlag=0;

                                                     //Set Coin low level flag state is closed
                                                     pthread_mutex_lock(&CoinLowLevelPollFlagStatusmutex);

                                                     g_CoinLowLevelPollFlagStatus=0; 

                                                     pthread_mutex_unlock(&CoinLowLevelPollFlagStatusmutex);

                                                     pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

				                     //#ifdef  COINACCPTEOR_DEBUG
				                     writeFileLog("[ThreadProc1_MONEYCONTROL_CoinAcptrPoll()] Thread Exit.");
				                     //#endif

						     pthread_exit(0);

				         }
                                         else
                                         {
                                                pthread_mutex_unlock(&CloseCoinCreditPollingThreadFlagmutex);

                                         }


                                         //////////////////////////////////////////////////////////////////////////////////////////


                      }
		      
                     
		     

	   } // End for(;;)



} // End function 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//Malay modified 29 march
static int MONEYCONTROL_CoinAcptr_statusRead(unsigned char* const rcvPkt,unsigned int* const rcvPktLen)
{




             //////////////////////////////////////////////////////////////////////////////////////////////////////////////

	     unsigned char rxByte=0x00;

	     int rcvStat=0;

             unsigned int  byteRcvdCounter=0;

	     unsigned int  totalByteToRecv=0; 

	     unsigned int  totalByteMONEYCONTROL_CoinAcptr_statusReadIn=0; 

             unsigned char log[100];

             memset(log,'\0',100);

             int rtcode =0,totalByteIn=0;

             ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

             pthread_mutex_lock(&RecvPktLengthMutuex);

	     //Store Reply packet total bytes
             totalByteToRecv = g_rcvPktLen_C;

             pthread_mutex_unlock(&RecvPktLengthMutuex);

             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

             struct timespec begints={0}, endts={0},diffts={0};

             //Start Time
             clock_gettime(CLOCK_MONOTONIC, &begints);

             for(;;)
	     {        


                         rxByte=0x00;

                         totalByteIn=0;

                         //Read serial port received register 
		         //rcvStat = ReceiveSingleByteFromSerialPort_C(g_cctalkhandler,&rxByte,&totalByteIn); 
                         totalByteIn = read(g_cctalkhandler,&rxByte,1);

                          /*

                          memset(log,'\0',100);

		          sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] totalByteIn = %d.",totalByteIn);

		          writeFileLog(log);

                          */

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

                               

                               //memset(log,'\0',100);

		               //sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Expected Byte received in %d seconds.",diffts.tv_sec);
		               //writeFileLog(log);

                               rtcode=SUCCESS;   
                               
                               break;

                         }

                         //////////////////////////////////////////////////////////////////////////////////////////////////////


                         //Check Current timer status
                         clock_gettime(CLOCK_MONOTONIC, &endts);

			 diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                         //Time out and CCTALK echo date received
			 if(  (byteRcvdCounter < totalByteToRecv)  &&  ( diffts.tv_sec >= CCTALK_REPLY_WAIT_TIME ) )
			 {


                                memset(log,'\0',100);

		                sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and CCTALK echo date received in %d seconds.",diffts.tv_sec);
		                writeFileLog(log);

                                rtcode=FAIL;   

                                break;
     


			 }  
				
                         //Time out and No data received.      
			 if(  (byteRcvdCounter == 0)  &&  (diffts.tv_sec >= CCTALK_REPLY_WAIT_TIME)  )
			 { 

			         memset(log,'\0',100);

		                 sprintf(log,"[MONEYCONTROL_CoinAcptr_statusRead()] Time out and No data received in %d seconds.",diffts.tv_sec);
		                 writeFileLog(log);

                                 rtcode=FAIL; 
  
                                 break;
       


			 }       
      	 

                         //////////////////////////////////////////////////////////////////////////////////////////////////////





		}//End for(;;)


                return rtcode;




}//MONEYCONTROL_CoinAcptr_statusRead() end





/*

static void CoinAcptrdisplayReceivedPacket(const unsigned char* const rcvPkt,const int  rcvPktLen) 
{

	  int i;  
	  for(i = 0; i<rcvPktLen; i++)
	  {
		  

	  } 

 
}


*/



static void CoinAcptrCopyReceivedPacketGlobally(const unsigned char* const rcvPkt,const unsigned int  rcvPktLen)
{


          //writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] Entry.");

	  int i=0;  

          ////////////////////////////////////////////////////////////////

          //writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] Before Recv Packet Length.");

          pthread_mutex_lock(&RecvPktLengthMutuex);

          g_rcvPktLen_C=0;

          g_rcvPktLen_C = rcvPktLen;

          pthread_mutex_unlock(&RecvPktLengthMutuex);

          //writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] After Recv Packet Length.");

          ////////////////////////////////////////////////////////////////

          //writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] Before Global Reply Packet Set.");

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

          //writeFileLog("[CoinAcptrCopyReceivedPacketGlobally()] After Global Reply Packet Set.");

          ////////////////////////////////////////////////////////////////

          //writeFileLog("[CoinAcptrCopyReceivedPacketGlobally] Exit.");

          return;
 

}




static void MONEYCONTROL_CoinAcptr_coinAmountReceived(const unsigned char* const rcvPkt,int* const  amnt)    
{

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


//Milliseconds delay [value must be 0-900]
static void cctalk_delay_mSec(const int milisec) 
{

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
              writeFileLog(log);
              return;
	      

	}
        else
        {
              memset(log,'\0',100);
              sprintf(log,"[cctalk_delay_mSec()] nanosleep() system call success with return code  %d .",rtcode); 
              writeFileLog(log);
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

		writeFileLog(log);


        }


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



static  void MONEYCONTROL_CoinAcptr_Cmd_inhibitStat()
{
	
        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 7;
        pthread_mutex_unlock(&RecvPktLengthMutuex);

        

        SendSingleByteToSerialPort_C(g_cctalkhandler,0x02);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xE6); // command
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xEA); // checksum

        
       
        return;

}


static  void MONEYCONTROL_CoinAcptr_Cmd_CreditPoll()
{

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 21;
        pthread_mutex_unlock(&RecvPktLengthMutuex);

        
        
        SendSingleByteToSerialPort_C(g_cctalkhandler,0x02); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xE5); //command
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x18); //checksum

        

        return;

}


static  void MONEYCONTROL_CoinAcptr_Cmd_Poll()
{

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10;
        pthread_mutex_unlock(&RecvPktLengthMutuex);
        
        SendSingleByteToSerialPort_C(g_cctalkhandler,0x02); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xFE); //command
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xFF); //checksum

        

        return;

}


static  void MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow()
{
	
        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10; 
        pthread_mutex_unlock(&RecvPktLengthMutuex);
 
        SendSingleByteToSerialPort_C(g_cctalkhandler,0x53);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xDF);  //command
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xCD);  //checksum

        

        return;
 
}


static void MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow()
{

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10;
        pthread_mutex_unlock(&RecvPktLengthMutuex);

        SendSingleByteToSerialPort_C(g_cctalkhandler,0x53);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xE0); //command
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xCC); //chekcsum

        

        return;

}

static void MONEYCONTROL_CoinAcptr_Cmd_PollFromEscrow()
{

        pthread_mutex_lock(&RecvPktLengthMutuex);
        g_rcvPktLen_C = 10;
        pthread_mutex_unlock(&RecvPktLengthMutuex);

        SendSingleByteToSerialPort_C(g_cctalkhandler,0x53);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xFE); //command
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xAE); //chekcsum

        

        return;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


         //Created By Malay Maji Data 13 Dec 2012
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
	      x     x     x     x        x    x     x      x
	     ------------------------------------------------
	     Bit 0 =Rs 10 enable if it 1 or disable if it 0
	     other bits for Token so dont needed for this operation 
	     
	static CoinTable g_CoinTable[8]={ .50,2,0b00000011,0b0,  //First InhibitByte
		                          1,3,  0b00001100,0b0,  //First InhibitByte
		                          1,4,  0b00001100,0b0,  //First InhibitByte
		                          2,5,  0b00010000,0b0,  //First InhibitByte
		                          5,6,  0b11100000,0b0,  //First InhibitByte
		                          5,7,  0b11100000,0b0,  //First InhibitByte
		                          5,8,  0b11100000,0b0,  //First InhibitByte
		                          10,9, 0b0,0b11111110   //Second InhibitByte
		                        };


	*/

static void MONEYCONTROL_CoinAcptr_Cmd_InhibitAndRply(coinType COIN)
{

	     
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

		for(i=0;i< 7; i++)
		{ 
                   SendSingleByteToSerialPort_C(g_cctalkhandler, sbytes[i]);
		} 
                 
 
                pthread_mutex_lock(&RecvPktLengthMutuex);
		g_rcvPktLen_C = 12;
                pthread_mutex_unlock(&RecvPktLengthMutuex);

		MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);


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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes(coinType COIN)
{


	     unsigned char sbytes[MAX_CMD_LEN]; 
	     unsigned int i=0;
	     unsigned char coinEnableByte2=0x00;
             unsigned char coinEnableByte1=0x00;
	     g_COIN.rs_50 = COIN.rs_50; 
	     g_COIN.rs_1  = COIN.rs_1; 
	     g_COIN.rs_2  = COIN.rs_2;
	     g_COIN.rs_5  = COIN.rs_5;
	     g_COIN.rs_10 = COIN.rs_10;
	   

           ///////////////////////////////////////////////////////////////////////////////////////
          
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
                    #ifdef  COINACCPTEOR_DEBUG 
		    writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] 0.50 paisa enable.");
                    #endif
            }
           
            if( ENABLE == COIN.rs_1 )
            {
		    coinEnableByte1 = coinEnableByte1 | g_CoinTable[1].FirstCreditByteEnableMask; 
                    #ifdef  COINACCPTEOR_DEBUG 
		    writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 1 enable.");
                    #endif
            }
            
            if( ENABLE == COIN.rs_2 )
	    {
		    coinEnableByte1 = coinEnableByte1 | g_CoinTable[3].FirstCreditByteEnableMask;
                    #ifdef  COINACCPTEOR_DEBUG 
		    writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 2 enable."); 
                    #endif
	    }
          
            if( ENABLE == COIN.rs_5 )
	    {
		    coinEnableByte1 = coinEnableByte1 | g_CoinTable[4].FirstCreditByteEnableMask; 
                    #ifdef  COINACCPTEOR_DEBUG 
		    writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 5 enable.");
                    #endif
	    }
           
            if( ENABLE == COIN.rs_10 )
            {
		   coinEnableByte2 = coinEnableByte2 | g_CoinTable[7].SecondCreditByteEnableMask; 
                   #ifdef  COINACCPTEOR_DEBUG 
		   writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 10 enable.");
                   #endif
	    }
          
            

            #endif

            
            ///////////////////////////////////////////////////////////////////////////////////////
            /*
            static CoinTable g_CoinTable[8]={  0,0,0x0,0x0,     

                                  1,1,  0x1,0x0,   //First  Byte=0b00000001
                                  1,2,  0x1,0x0,   //First  Byte=0b00000001

                                  2,3,  0x02,0x0,  //First  Byte=0b00000010

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
                    #ifdef  COINACCPTEOR_DEBUG 
		    writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 1 enable.");
                    #endif
            }
            
            if( ENABLE == COIN.rs_2 )
	    {
		    coinEnableByte1 = coinEnableByte1 | g_CoinTable[3].FirstCreditByteEnableMask;
                    #ifdef  COINACCPTEOR_DEBUG 
		    writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 2 enable."); 
                    #endif
	    }
          
            if( ENABLE == COIN.rs_5 )
	    {
		    coinEnableByte1 = coinEnableByte1 | g_CoinTable[4].FirstCreditByteEnableMask; 
                    #ifdef  COINACCPTEOR_DEBUG 
		    writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 5 enable.");
                    #endif
	    }
           
            if( ENABLE == COIN.rs_10 )
            {
		   coinEnableByte1 = coinEnableByte1 | g_CoinTable[7].FirstCreditByteEnableMask; 
                   #ifdef  COINACCPTEOR_DEBUG 
		   writeFileLog("[MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes] Rs 10 enable.");
                   #endif
	    }
          
            #endif


            ///////////////////////////////////////////////////////////////////////////////////////

	    sbytes[0] = 0x02;
            sbytes[1] = 0x02;
	    sbytes[2] = 0x01; 
	    sbytes[3] = 0xE7;            //command byte
	    sbytes[4] = coinEnableByte1; //coin inhibit byte 1
	    sbytes[5] = coinEnableByte2; //coin inhibit byte 2         
	    sbytes[6] = GetCheckSum(sbytes,6);

            for(i=0;i< 7; i++)
	    {
               SendSingleByteToSerialPort_C(g_cctalkhandler, sbytes[i]);
            } 

            pthread_mutex_lock(&RecvPktLengthMutuex);

            g_rcvPktLen_C = 12;

            pthread_mutex_unlock(&RecvPktLengthMutuex);

            return;


}//MONEYCONTROL_CoinAcptr_Cmd_enableCoinTypes() end



//////////////////////////////////////////////////End :Coin Acceptor///////////////////////////////////////////////////////////////


///////////////////////////////////////////////Start :Coin Escrow ////////////////////////////////////////////////////////////////

#ifdef COIN_ESCROW

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int cctalk_Wait(unsigned int Second)
{
		   

                  if(Second < 0)
                  {

                          writeFileLog("[cctalk_Wait()] Function parameter not ok.");
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

		                  writeFileLog(log);

		                  #endif

		                  if(diffts.tv_sec>=Second)
		                  {

		                          memset(log,'\0',100);
				   
				          sprintf(log,"[cctalk_Wait()] Seconds Elapsed = %d ",diffts.tv_sec); 

				          writeFileLog(log);

                                          return 1;

		                  }


                 }


}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int PollFromCoinEscrow()
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
                     
                //////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_cctalkporthandlemutex);

		//clear serial port transmit and rece buffer 
		FlushSerialPortOutBuff_C(g_cctalkhandler);

		FlushSerialPortInBuff_C(g_cctalkhandler);

                //Send accept command to escrow device
		MONEYCONTROL_CoinAcptr_Cmd_PollFromEscrow();
                 
                //Then read command reply bytes
		ret =  MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);


                pthread_mutex_unlock( &g_cctalkporthandlemutex);

                //////////////////////////////////////////////////////////////////////////


		
                if(                       ( 0x01 == rcvPkt[5] )                              && 
                                          ( 0x00 == rcvPkt[6] )                              && 
                                          ( 0x53 == rcvPkt[7] )                              &&
                                          ( 0x00 == rcvPkt[8] )                              && 
                                          ( 0xAC == rcvPkt[9] ) 
                 )
		{


				writeFileLog("[PollFromCoinEscrow()] Coin Escrow poll succcessed.");

		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[PollFromCoinEscrow()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return SUCCESS;

		}
		else
		{
				writeFileLog("[PollFromCoinEscrow()] Coin Escrow poll  failed.");
		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[PollFromCoinEscrow()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return FAIL;

		}



}//PollFromCoinEscrow() end here




int AcceptFromCoinEscrow()
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
                     

                /////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_cctalkporthandlemutex);

		//clear serial port transmit and rece buffer // wait for lst pkt to complete the recv/trnsmt
		FlushSerialPortOutBuff_C(g_cctalkhandler);

		FlushSerialPortInBuff_C(g_cctalkhandler);

                //Send accept command to escrow device
		MONEYCONTROL_CoinAcptr_Cmd_acceptFromEscrow();

                //wait for solenoid work complete
		//cctalk_Wait(COIN_ESCROW_REPLY_WAIT_TIME); 
		cctalk_delay_mSec(500);

                //Then read command reply bytes
		ret =  MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

                pthread_mutex_unlock( &g_cctalkporthandlemutex);

                /////////////////////////////////////////////////////////////////////////////////

		
                if(                       ( 0x01 == rcvPkt[5])                               && 
                                          ( 0x00 == rcvPkt[6] )                              && 
                                          ( 0x53 == rcvPkt[7] )                              &&
                                          ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )    && 
                                          ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                 )
		{


				writeFileLog("[AcceptFromCoinEscrow()] Accept coin Successfully done.");

		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[AcceptFromCoinEscrow()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return SUCCESS;

		}
		else
		{
				writeFileLog("[AcceptFromCoinEscrow()] Accept coin failed.");
		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[AcceptFromCoinEscrow()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return FAIL;

		}



}//AcceptFromCoinEscrow() end here




int RejectFromCoinEscrow()
{

	
                     unsigned char rcvPkt[COINACPTR_MAX_LEN_OF_PKT]; 
                     memset(rcvPkt,'\0',COINACPTR_MAX_LEN_OF_PKT); 
	             unsigned int  rcvPktLen=0;
                     int ret=0;
                     unsigned char log[100];
                     memset(log,'\0',100);
                     int i=0;

                     //////////////////////////////////////////////////////////////////////////////

                     pthread_mutex_lock( &g_cctalkporthandlemutex);

                     //clear serial port transmit and recv buffer	
		     FlushSerialPortOutBuff_C(g_cctalkhandler); 
 
		     FlushSerialPortInBuff_C(g_cctalkhandler);   

                     writeFileLog("[RejectFromCoinEscrow()] Before Issue Reject Command.");

                     //Send reject command to escrow device
		     MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow();

                     writeFileLog("[RejectFromCoinEscrow()] After Issue Reject Command.");
                    
                     //memset(log,'\0',100);

                     //sprintf(log,"[RejectFromCoinEscrow()] Wait Time = %d Seconds.",COIN_ESCROW_REPLY_WAIT_TIME);

                     //writeFileLog(log);
					 
                     //wait for solenoid work complete
                     //cctalk_Wait( COIN_ESCROW_REPLY_WAIT_TIME ); 
                     cctalk_delay_mSec(500);

                     //Then read command reply bytes
		     ret = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

                     pthread_mutex_unlock( &g_cctalkporthandlemutex);

                     //////////////////////////////////////////////////////////////////////////////

		     
                     if(                       ( 0x01 == rcvPkt[5] )                             && 
                                               ( 0x00 == rcvPkt[6] )                             && 
                                               ( 0x53 == rcvPkt[7] )                             &&
                                               ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )   && 
                                               ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                       )
	             {

				 writeFileLog("[RejectFromCoinEscrow()] Reject Successfully done.");

		                 for(i=0;i<10;i++)
		                 {

                                         memset(log,'\0',100);

				         sprintf(log,"[RejectFromCoinEscrow()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);

					 writeFileLog(log);

					 
		                 }

		                 return SUCCESS; 

			 
	             }
		     else
		     {

				 writeFileLog("[RejectFromCoinEscrow()] Reject Coin failed. ");

		                 for(i=0;i<10;i++)
		                 {

                                         memset(log,'\0',100);

				         sprintf(log,"[RejectFromCoinEscrow()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);

					 writeFileLog(log);
					 
		                 }

		                 return FAIL;  


		     }   


       
}//RejectFromCoinEscrow() end here

#endif

///////////////////////////////////////////// End :Coin Escrow ////////////////////////////////////////////////////////////////


static char GetCheckSum(const unsigned char* const  bufData, const unsigned int sizeData)
{

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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



static void Delay(const unsigned int MilliSeconds)
{

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

		writeFileLog(log);


        }


}//delay() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
static  void SendATVMGetStatusCommand()
{


	        pthread_mutex_lock(&RecvPktLengthMutuex);

		g_rcvPktLen_C = COMMANDREPLYLENGTH; //Return Byte : echo byte(9)+reply byte(5)

		pthread_mutex_unlock(&RecvPktLengthMutuex);

	        int counter =0;
 
                char checksum =0x00;

                unsigned char log[100];

                memset(log,'\0',100);
               
                //clear checksum value
                g_AtvmAlarmCommands[0].Command[COMMANDLENGTH -1] = 0x00;

                checksum = GetCheckSum(g_AtvmAlarmCommands[0].Command, COMMANDLENGTH);

                //update checksum value 
                g_AtvmAlarmCommands[0].Command[COMMANDLENGTH -1] = checksum;

               
                /////////////////////////////////////////////////////////////////////////////////////
                
                

                //Print getstatus command reply bytes
                for(counter=0;counter<COMMANDLENGTH;counter++)
                {

	               memset(log,'\0',100);

                       sprintf(log,"[SendATVMGetStatusCommand()] g_AtvmAlarmCommands[0].Command[%d] = 0x%xh .",counter,g_AtvmAlarmCommands[0].Command[counter]); 

		       writeFileLog(log);

                }

                

                ////////////////////////////////////////////////////////////////////////////////////

                //send command to cctalk board
                for(counter=0;counter<COMMANDLENGTH;counter++)
                {

	           SendSingleByteToSerialPort_C(g_cctalkhandler,g_AtvmAlarmCommands[0].Command[counter]);

                }

                

	        return;



}//SendATVMGetStatusCommand()


static  void SendATVMElectronicsCommand()
{

	
		pthread_mutex_lock(&RecvPktLengthMutuex);

		g_rcvPktLen_C = COMMANDREPLYLENGTH; //Return Byte : echo byte(17)+reply byte(5)

		pthread_mutex_unlock(&RecvPktLengthMutuex);

                int counter=0;

                unsigned char log[100];

                memset(log,'\0',100);

                //////////////////////////////////////////////////////////////////////////////

                for(counter=0;counter<COMMANDLENGTH;counter++)
                {

	           memset(log,'\0',100);
				   
		   sprintf(log,"[SendATVMElectronicsCommand()]  g_AtvmAlarmCommands[1].Command[%d]: 0x %x h .",counter,g_AtvmAlarmCommands[1].Command[counter]); 

		   writeFileLog(log);

                }
                
                 //////////////////////////////////////////////////////////////////////////////

                for(counter=0;counter<COMMANDLENGTH;counter++)
                {

	           SendSingleByteToSerialPort_C(g_cctalkhandler,g_AtvmAlarmCommands[1].Command[counter]);

                }
	       
		return;



}//SendATVMElectronicsCommand() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int setDisableAlarmState( const int doorOpenTimeFlag,const int doorOpenexpiryTime ,
                          const int cashboxOpenTimeFlag, const int cashboxOpenexpiryTime
                        )  
                         
{

           pthread_mutex_lock( &g_getdisablealarmmutex );

           writeFileLog("[setDisableAlarmState()] Entry.");

           if(1 != doorOpenTimeFlag)
           {
                pthread_mutex_unlock( &g_getdisablealarmmutex );
                
                writeFileLog("[setDisableAlarmState()] Vault Opentime is not set by use by default return zero.");

                writeFileLog("[setDisableAlarmState()] Exit.");

	        return 0;

           }


           unsigned char log[200];
           memset(log,'\0',200);	
           unsigned char GetStatusReplyPackt[100];
           memset(GetStatusReplyPackt,'\0',100);
           unsigned int GetStatusReplyPacktLength=20;

           if(-1==g_cctalkhandler)
           {
	         writeFileLog("[setDisableAlarmState()] Could Not Obtain Com port handle.");
                 writeFileLog("[setDisableAlarmState()] Exit.");
                 pthread_mutex_unlock( &g_getdisablealarmmutex );
	         return 0;
	   } 
	   
	   unsigned int tempdoorOpenexpiryTimeMSB=0,
           tempdoorOpenexpiryTimeLSB=0,
           tempcashboxOpenexpiryTimeMSB=0,
           tempcashboxOpenexpiryTimeLSB=0,rtcode=-1,electronicstime=-1;

           /////////////////////////////////////////////////////////////////////////////////////////

           //Init all command bytes
 
           //Clear door open allow time in second 
           
           g_AtvmAlarmCommands[1].Command[4]= 0x00;   //16 BIT VAULT OPEN TIME MSB

           g_AtvmAlarmCommands[1].Command[5]= 0x00;   //16 BIT VAULT OPEN TIME LSB
          
           g_AtvmAlarmCommands[1].Command[6]= 0x00;  //8 BIT ELECTRONIC LOCK ENABLE TIME IN MINUTE

           ////////////////////////////////////////////////////////////////////////////////////////////

           //read ini file electronics time
           electronicstime = GetElectronicsLockTime();

           //only use valid time from ini file
           if( -1!= electronicstime )
           {

               if( (electronicstime >=0 ) &&  (electronicstime <= 255) )
               { 

                      memset(log,'\0',100);

		      sprintf(log,"[setDisableAlarmState()] electronicstime = %d .",electronicstime); 

                      writeFileLog(log);

                      g_AtvmAlarmCommands[1].Command[6]= electronicstime;

               }// if( (electronicstime >=0 ) &&  (electronicstime <= 255) ) end

              
           }//if( -1!= electronicstime ) end
           else
           {
                 writeFileLog("[setDisableAlarmState()] Unable to read electronics time from ini file so it by default set by zero.");


           }

           //////////////////////////////////////////////////////////////////////////////////////////////////

           g_AtvmAlarmCommands[1].Command[7]= 0x01;  //ALARM LOGIC BIT BY DEFAULT SET TO 0X01


           //////////////////////////////////////////////////////////////////////////////////////////////////

           //clear checksum value
           g_AtvmAlarmCommands[1].Command[8]=0x00; //checksum value



           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           

		    if(1==doorOpenTimeFlag)
		    {
		               
		               writeFileLog("[setDisableAlarmState()] Door Opentime flag is set by user.");

		               /*
		               tempdoorOpenexpiryTimeMSB=(doorOpenexpiryTime & 0xffff0000);  //get only MSB 2byte from 4 byte
		               tempdoorOpenexpiryTimeMSB =( tempdoorOpenexpiryTimeMSB >>16 );  //shift bit position
		               g_AtvmAlarmCommands[1].Command[4]=(tempdoorOpenexpiryTimeMSB>> 8);   //msb of Upper 2byte
			       g_AtvmAlarmCommands[1].Command[5]=(tempdoorOpenexpiryTimeMSB & 0x00ff);//lsb of Upper 2byte 
		               */
		         
		               tempdoorOpenexpiryTimeLSB=0;

			       tempdoorOpenexpiryTimeLSB=(doorOpenexpiryTime & 0x0000ffff);  //get only LSB 2byte from 4 byte
			       g_AtvmAlarmCommands[1].Command[4]=(tempdoorOpenexpiryTimeLSB>> 8);   //msb of Lower 2byte
			       g_AtvmAlarmCommands[1].Command[5]=(tempdoorOpenexpiryTimeLSB & 0x00ff);//lsb of Lower 2byte

                       }//if(1==doorOpenTimeFlag)
           
                       //set checksum value	
                       g_AtvmAlarmCommands[1].Command[8] = GetCheckSum(g_AtvmAlarmCommands[1].Command,9);

                      ///////////////////////////////////////////////////////////////////////////////////////
           
               
                       rtcode= -1 ;

                       pthread_mutex_lock( &g_cctalkporthandlemutex);

		       //Step 1 :First Send getstatus command
		       SendATVMElectronicsCommand();
		          
		       //Step 2 :Now go for read reply bytes
		       rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength);
                 
		          
                       pthread_mutex_unlock( &g_cctalkporthandlemutex);

                       ////////////////////////////////////////////////////////////////////////////////////

                       //Print atvm electronics lock command reply bytes
                       int counter=0;

		       for(counter=0;counter<GetStatusReplyPacktLength;counter++)
		       {

			       memset(log,'\0',100);

		               sprintf(log,"[setDisableAlarmState()] GetStatusReplyPackt[%d] = 0x%xh .",counter,GetStatusReplyPackt[counter]); 

                               writeFileLog(log);

		        }

                       ////////////////////////////////////////////////////////////////////
                       if( SUCCESS == rtcode )
                       {
                            writeFileLog("[setDisableAlarmState()] setDisableAlarm command successfully executed.");
                            rtcode= 1;
                            
                       }
		       else
                       {
                            writeFileLog("[setDisableAlarmState()] setDisableAlarm command failed to executed.");
                            rtcode= 0;

                       }
 
                       writeFileLog("[setDisableAlarmState()] Exit.");

                       pthread_mutex_unlock( &g_getdisablealarmmutex );

                       return rtcode;



}//setDisableAlarmState() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int getDoorOpenStatus(void) //returns 0 when door is in close state, returns 1 when door is in open state, returns -1 when error reading the status
{
	  int doorOpenStatus=2;
	  getSecuredState(&doorOpenStatus, NULL, NULL,NULL);
	  return doorOpenStatus;
}

int getAlarmOnStatus(void) //returns 0 when alarm is off, returns 1 when alarm is on, returns -1 when error reading the status
{
	  int getAlarmOnStatus=2;
	  getSecuredState(NULL,&getAlarmOnStatus, NULL,NULL);
	  return getAlarmOnStatus;

}

int getCashboxOpenStatus(void) //returns 0 when cashbox is in close state, returns 1 when cashbox is in open state, returns -1 when error reading the status
{
	  int getCashboxOpenStatus=2;
	  getSecuredState(NULL,NULL, &getCashboxOpenStatus,NULL);
	  return getCashboxOpenStatus;

}



int getButtomDoorStatus(int *ButtomDoorStatus) 
{

      
	          pthread_mutex_lock( &g_getsecuredstausmutex);
                  
                  writeFileLog("[getButtomDoorStatus()] Entry.");

		  if(-1==g_cctalkhandler)
		  {
			writeFileLog("[getButtomDoorStatus() Exit] Could Not Obtain Com port handle.");
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
                  rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength);

                  pthread_mutex_unlock( &g_cctalkporthandlemutex);

                  ///////////////////////////////////////////////////////////////////////////
                
                  if( SUCCESS != rtcode )
		  {
		      writeFileLog("[[getButtomDoorStatus() Exit] atvm getstatus command execution failed.");

                      pthread_mutex_unlock( &g_getsecuredstausmutex);

		      return 0;
		  }
		  else
		  {


                                  int  StatusBytePosition=0;

                                  StatusBytePosition=GetStatusReplyPacktLength-2;

                                  char StatusByte=0;

                                  StatusByte = GetStatusReplyPackt[ StatusBytePosition] ;

                                  //High if open Low if close
                                  bottomdoor = StatusByte & 0b00000001;
                                 
			         *ButtomDoorStatus = bottomdoor ;

                                  memset(log,'\0',100); 

				  sprintf(log,"[getButtomDoorStatus() Exit] Bottomdoor status: %d.", *ButtomDoorStatus);

                                  writeFileLog(log);

                                  
                                   if( 0 == *ButtomDoorStatus )
                                   {
                                        writeFileLog("[getButtomDoorStatus()] Bottom door close.");
                                   }
                                   else if( 1 == *ButtomDoorStatus )
                                   {
                                        writeFileLog("[getButtomDoorStatus()] Bottom door open.");
                                   }
                      
                                  rtcode= 1;

                  }

                  pthread_mutex_unlock( &g_getsecuredstausmutex);

                  return rtcode;
                        
}//getButtomDoorStatus() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//New Version cctalk style
int getSecuredState(  int* const doorOpenStatus, 
                      int* const alarmOnStatus,
                      int* const cashboxOpenStatus,
                      int* const PrinterCoverstatus
                   ) //Returns all status values in a single call
{

                  
                  pthread_mutex_lock( &g_getsecuredstausmutex);

                  writeFileLog("[getSecuredState()] Entry.");

		  if(-1==g_cctalkhandler)
		  {
			writeFileLog("[getSecuredState() Exit] Could Not Obtain Com port handle.");
                        
                        pthread_mutex_unlock( &g_getsecuredstausmutex);
			
                        return 0;
		  }
		 
                  

		  //if all pointer have null value return then
		  if( (NULL== doorOpenStatus) && ( NULL== alarmOnStatus) && 
                      (NULL== cashboxOpenStatus) && (NULL== PrinterCoverstatus) )
                  {
                           writeFileLog("[getSecuredState() Exit] Function parameters not ok.");  

                           pthread_mutex_unlock( &g_getsecuredstausmutex);

                           return 0;

                  }
 
                  writeFileLog("[getSecuredState()] Going to issue atvm getstatus command.");

                  int rtcode=-1; 

                  unsigned char log[100];

                  memset(log,'\0',100);

		  unsigned char GetStatusReplyPackt[100];

                  memset(GetStatusReplyPackt,'\0',100);

                  unsigned int GetStatusReplyPacktLength=0;

                  int StatusBytePosition=0;

                  /////////////////////////////////////////////////////////////////////////////
                  

                  cctalk_delay_mSec(100);

                  pthread_mutex_lock( &g_cctalkporthandlemutex);

                  //Step 1 :First Send getstatus command
                  SendATVMGetStatusCommand();
                  
                  //Step 2 :Now go for read reply bytes
                  rtcode = MONEYCONTROL_CoinAcptr_statusRead(GetStatusReplyPackt,&GetStatusReplyPacktLength);


                  pthread_mutex_unlock( &g_cctalkporthandlemutex);

                  
                  ///////////////////////////////////////////////////////////////////////////////

                  int counter=0;

                  memset(log,'\0',100); 

		  sprintf(log,"[getSecuredState()] Reply Packet Length = %d ", GetStatusReplyPacktLength);

		  writeFileLog(log);

                  for(counter=0;counter<GetStatusReplyPacktLength;counter++)
                  {                

		          memset(log,'\0',100); 

		          sprintf(log,"[getSecuredState()] GetStatusReplyPackt[%d] = 0x%xh", counter,GetStatusReplyPackt[counter]);

		          writeFileLog(log);
   
                  }

                  //////////////////////////////////////////////////////////////////////////////////
                
                  if( SUCCESS != rtcode )
		  {
		      writeFileLog("[getSecuredState() Exit] atvm getstatus command execution failed.");

                      pthread_mutex_unlock( &g_getsecuredstausmutex);

		      return 0;

		  }
		  else
		  {

                             StatusBytePosition= GetStatusReplyPacktLength - 2 ;
                        
                             writeFileLog("[getSecuredState()] atvm getstatus command executed successfully.");
                             memset(log,'\0',100); 

		             sprintf(log,"[getSecuredState()] StatusByte: 0x%xh.", 
                             GetStatusReplyPackt[StatusBytePosition]);

                             writeFileLog(log); 

                             char StatusByte=0x00;

                             StatusByte= GetStatusReplyPackt[StatusBytePosition] ;

                           

                             ////////////////////////////////////////////////////////////////////////////////////

                             //Bit 0
                             if(NULL!=doorOpenStatus)
                             {

                                  unsigned char  bottomdoor=0x00;
                                  
                                  bottomdoor = StatusByte & 0x01 ;


                                  *doorOpenStatus= bottomdoor ;

                                  
                                   if( 0 == bottomdoor)
                                   {
                                        writeFileLog("[getSecuredState()] Bottom door close.");
                                   }
                                   else if( 1 == bottomdoor)
                                   {
                                        writeFileLog("[getSecuredState()] Bottom door open.");
                                   }
                                   

			     }
                             
                             

                             //////////////////////////////////////////////////////////////////////////////////

                             //Bit 1
                             if(NULL!=cashboxOpenStatus)
                             {
                                    //High if open Low if closed
                                    
			           *cashboxOpenStatus=( (GetStatusReplyPackt[StatusBytePosition] & 0b00000010) >> 1);

                                    if( 0 == *cashboxOpenStatus)
                                    {
                                        writeFileLog("[getSecuredState()] Cash Vault close.");
                                    }
                                    else if( 1 == *cashboxOpenStatus )
                                    {
                                        writeFileLog("[getSecuredState()] Cash Vault open.");
                                    }

                             }
                             else if(NULL == cashboxOpenStatus)
                             {

                                  writeFileLog("[getSecuredState()] cashbox open status  get failed.");

			     }
                            
                             
                           
                             ////////////////////////////////////////////////////////////////////////////////////

                             //Bit 2
                             if(NULL!=alarmOnStatus)
                             {
                                   //High if alarm on Low if alarm off

                                   *alarmOnStatus=(StatusByte & 0b00000100)>>2 ;

                                   if( 0 == *alarmOnStatus)
                                   {
                                        writeFileLog("[getSecuredState()] Alarm is off.");
                                   }
                                   else if( 1 == *cashboxOpenStatus )
                                   {
                                        writeFileLog("[getSecuredState()] Alarm is on");
                                   }
 
                             }
			    
                             ////////////////////////////////////////////////////////////////////////////////////


                             writeFileLog("[getSecuredState()] Exit.");

                             pthread_mutex_unlock( &g_getsecuredstausmutex);

			     return 1;


		  }

                  

                  return 1;

}//getSecuredState() end here


#endif   //ATVM_ALARM_VAULT endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#ifdef ATVMCA_MAIN

void Creditpolling();

int EnableAllCoins();

void SendSoftwareversionCommand();

void SendSoftwareversionCommand()
{
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x02);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x00);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x01);
	SendSingleByteToSerialPort_C(g_cctalkhandler,0xF1); 
	SendSingleByteToSerialPort_C(g_cctalkhandler,0x0C); 
        g_rcvPktLen_C = 15;

}

int EnableAllCoins()
{

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
	  if(MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen) == SUCCESS)
	  {      
		    if(( 0xE5 == rcvPkt[3] ) && (rcvPktLen == g_rcvPktLen_C) )
		    {
			      g_prevEvent_C = rcvPkt[9];
                    }
          }
  
}




static void CAGetCreditCodeDenomValue(const unsigned char* const rcvPkt,int* const  amnt)    
{

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


void Creditpolling()
{
       
           unsigned char rcvPkt[40]; 
           int Denom[5];
           memset(Denom,0,5);

           memset(rcvPkt,'\0',40); 
           int  rcvPktLen=0;
           int totalcoins=0;
           int Coins=0;
           int Counter=0;

           writeFileLog("[Creditpolling()] Now going to enable all coins.. ");

           EnableAllCoins();
           
             
           for(;;)
           {                     
                
                
	        rcvPktLen=0;
                
                memset(rcvPkt,'\0',40); 

                Counter=0;

                cctalk_delay_mSec(200); 

                MONEYCONTROL_CoinAcptr_Cmd_CreditPoll();      
                  

	        if( SUCCESS == MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen) )
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
				writeFileLog("[Creditpolling()] Coin acceptor poll status read failed..");   
                           }

                       
                   }

                 
 
               } //for loop




}//end



int main()
{


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
	     if(MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen) == SUCCESS) 
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

#endif  //ATVMCA_MAIN endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ATVMALARM_MAIN


static   int g_Mcashboxopenexpirytime=0;

static   int g_Mdooropenexpirytime=0;


int WaitForVaultOpen(int second)
{

          int VaultStatus=-1,rtcode=-1;

          struct timespec begints, endts,diffts;

          clock_gettime(CLOCK_MONOTONIC, &begints);

          while(1)
          {

                writeFileLog("\n\n=========================================");

                rtcode =-1;
 
                VaultStatus=-1;
							     
                getSecuredState(NULL,NULL,&VaultStatus,NULL);

                                                           
                //0 closed 1 open
                if(  0 == VaultStatus  )
                {
                      writeFileLog("[WaitForVaultOpen()] ATVM Volt is locked");

                      


		} 
                else if(  1 == VaultStatus  )
                {

                     writeFileLog("[WaitForVaultOpen()] ATVM Volt is still opened");

                     return 1;

                } 

                writeFileLog("[WaitForVaultOpen()] Now Checking for Vault lock open condition.");
	
					     
                clock_gettime(CLOCK_MONOTONIC, &endts);
 
                diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                if(diffts.tv_sec>= second ) //Given door open allow time check only
                {

		         writeFileLog("[WaitForVaultOpen()] Timeout for cashbox vault close status check.");

	                 return 0;

                }

			     
            }// while(1) end here




}//int WaitForVaultOpen(int second) end


int WaitForVaultLocked()
{

          int VaultStatus=-1,rtcode=-1;

          struct timespec begints, endts,diffts;

          clock_gettime(CLOCK_MONOTONIC, &begints);

          while(1)
          {

                writeFileLog("\n\n=========================================");

                rtcode =-1;
 
                VaultStatus=-1;
							     
                getSecuredState(NULL,NULL,&VaultStatus,NULL);

                                                           
                //0 closed 1 open
                if(  0 == VaultStatus  )
                {
                      writeFileLog("[WaitForVaultLocked()] ATVM Volt is locked");
                      
                      return 1;
                      


		} 
                else if(  1 == VaultStatus  )
                {

                     writeFileLog("[WaitForVaultLocked()] ATVM Volt is still opened");

                     

                } 

                writeFileLog("[WaitForVaultLocked()] Now Checking for Vault lock close condition.");
	
					     
                clock_gettime(CLOCK_MONOTONIC, &endts);
 
                diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                if(diffts.tv_sec>= 3600 ) //1 Hour check
                {
		         writeFileLog("[WaitForVaultLocked()] Timeout for cashbox vault close status check.");

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
		                           writeFileLog("\n[main()] setDisableAlarmState() executed for door open successfully.");
		                       }
		                       else
		                       {
		                           writeFileLog("[main()] setDisableAlarmState() executed for door open failed to execute.");
                                           break;
		                       }	

                                       writeFileLog("[main()] Now checking for vault open.");

                                       if( 1 ==  WaitForVaultOpen(ApiAllowdooropenExpirytime) )
                                       {
                           
                                             writeFileLog("[main()] Now checking for vault close .");

                                             if( 1 ==  WaitForVaultLocked() )
                                             {
                                                writeFileLog("[main()] vault closed by user.");
                                             }
                                             else
                                             {
                                                writeFileLog("[main()] Warning vault is not closed by user.");
                                             }


                                       }
                                       else
                                       {

                                              writeFileLog("[main()] vault do not opened by user withing given time.");

                                       }

			            }//if end here

                                    break;

                               case  4:  
                                         while(1)
                                         {
                                                writeFileLog(" \n\n\n\n\n ");
                                                
                                                alarmOnStatus=-1;

                                                cashboxOpenStatus=-1;
                                                
                                                getSecuredState(NULL,
                                                                &alarmOnStatus,
                                                                &cashboxOpenStatus,
                                                                NULL );

                                                

                                         }

                                         break;  
                               default:  writeFileLog(" Again choice ");
                                         break;

			 
			}//switch() end
		      
		       
		
		  }//while end

		  return 0;

}



#endif //ATVMALARM_MAIN endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
		ret =  MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);

		
                if(                       ( 0x01 == rcvPkt[5])                               && 
                                          ( 0x00 == rcvPkt[6] )                              && 
                                          ( 0x53 == rcvPkt[7] )                              &&
                                          ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )    && 
                                          ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                 )
		{


				writeFileLog("[AcceptFromCoinEscrow_Ver2()] Accept coin Successfully done.");

		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[AcceptFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return SUCCESS;

		}
		else
		{

				writeFileLog("[AcceptFromCoinEscrow_Ver2()] Accept coin failed.");
		                for(i=0;i<10;i++)
		                {
				         sprintf(log,"[AcceptFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);
					 writeFileLog(log);
					 memset(log,'\0',100);
		                }
				free(rcvPkt);
				return FAIL;

		}



}//AcceptFromCoinEscrow_Ver2() end here




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
                     
                     writeFileLog("[RejectFromCoinEscrow_Ver2()] Before Issue Reject Command.");

                     //Send reject command to escrow device
		     MONEYCONTROL_CoinAcptr_Cmd_rejectFromEscrow();

                     writeFileLog("[RejectFromCoinEscrow_Ver2] After Issue Reject Command.");
                    
                     //memset(log,'\0',100);

                     //sprintf(log,"[RejectFromCoinEscrow_Ver2()] Wait Time = %d Seconds.",COIN_ESCROW_REPLY_WAIT_TIME);

                     //writeFileLog(log);
					 
                     //wait for solenoid work complete
                     //cctalk_Wait( delay ); 

                     //Then read command reply bytes
		     ret = MONEYCONTROL_CoinAcptr_statusRead(rcvPkt,&rcvPktLen);
		     
                     if(                       ( 0x01 == rcvPkt[5] )                             && 
                                               ( 0x00 == rcvPkt[6] )                             && 
                                               ( 0x53 == rcvPkt[7] )                             &&
                                               ( (0x00 == rcvPkt[8] ) || (0xFA== rcvPkt[8] ) )   && 
                                               ( (0xAC == rcvPkt[9] ) || (0xB2== rcvPkt[9] ) )
                       )
	             {

				 writeFileLog("[RejectFromCoinEscrow_Ver2()] Reject Successfully done.");

		                 for(i=0;i<10;i++)
		                 {

                                         memset(log,'\0',100);

				         sprintf(log,"[RejectFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);

					 writeFileLog(log);

					 
		                 }

		                 return SUCCESS; 

			 
	             }
		     else
		     {

				 writeFileLog("[RejectFromCoinEscrow_Ver2)] Reject Coin failed. ");

		                 for(i=0;i<10;i++)
		                 {

                                         memset(log,'\0',100);

				         sprintf(log,"[RejectFromCoinEscrow_Ver2()] rcvPkt[%d] = 0x%xh.",i,rcvPkt[i]);

					 writeFileLog(log);
					 
		                 }

		                 return FAIL;  


		     }   


       
}//RejectFromCoinEscrow_Ver2() end here


#endif  //ATVM_ESCORW_MAIN endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif  //cctalk endif


