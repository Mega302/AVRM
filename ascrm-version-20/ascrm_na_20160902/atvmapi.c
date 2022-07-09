#include "atvmapi.h"
#include "currencyreturn.h"  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Share by note and coin acceptior source 
static volatile int g_totalNoteAmountAccepted;
static volatile int g_totalCoinAmountAccepted;
static volatile int g_ttlNmbrOfNote;
static volatile int g_ttlNmbrOfCoin;
static volatile int g_totalAmountAccepted;
static int g_Fare;
static int g_maxNoOfCash;
static int g_maxNoOfCoin;
static noteType g_NOTE={0,0,0,0,0,0,0,0,0,0};
static coinType g_COIN={0,0,0,0,0}; //Rs .50,1,2,5,10

static  volatile int g_noteAcptrStartFlag;
static  volatile int g_coinAcptrStartFlag;
static  volatile unsigned int g_RuntimeInhibitFlag;
static  volatile unsigned int g_ExternalKillAcptFareThread;
static  volatile unsigned int g_AcptFareThreadStatus;
static  volatile unsigned int g_CoinFullDisable;
static  volatile unsigned int g_NoteFullDisable;

static  int  g_acpted_CurrencyDetail[2][MAX_DENOM_NUMBER];
static  int  g_denomIndex;
static  int  g_NotedenomIndex=0;
static  int  g_CoindenomIndex=0;

static  unsigned int g_NoteCounter;
static  unsigned int g_DropNotesNumber;
static  unsigned int g_total_transaction;
static  volatile bool g_transactiontimeout;
static  unsigned int g_PrevCastQuanity;
static  unsigned int g_CurrentCastQuanity;
static  bool g_PrevCastFlag;
static  bool g_CurrentCastFlag;
static  bool g_AllAlreadyDisableFlag;
static  bool g_CoinEventFlag;
static  unsigned int g_TransTimeOut;
static bool  g_CloseAcceptFareThreadFlag =false;
static pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t Inhibitmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t ExternalInhibitmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t AcptFareThreadStatusmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t noteAcptrStartFlagmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t coinAcptrStartFlagmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t RuntimeInhibitFlagmutex=PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static  pthread_mutex_t CloseAcceptFareThreadFlagmutex = PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t g_AcceptFareEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;
static  pthread_cond_t  g_AcceptFareEnableThreadCond  = PTHREAD_COND_INITIALIZER;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static  pthread_mutex_t g_NAULEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;
static  pthread_cond_t  g_NAULEnableThreadCond  = PTHREAD_COND_INITIALIZER;
static  pthread_mutex_t g_CAULEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;
static  pthread_cond_t  g_CAULEnableThreadCond  = PTHREAD_COND_INITIALIZER;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static  CoinsCounter g_CoinsCounter;
static  NotesCounter g_NotesCounter;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static  CoinsCounter g_AllCoinsCounter;
static  NotesCounter g_AllNotesCounter;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static  int g_NACommitStatus=false;
static  int g_NACancelStatus=false;
static  int g_CACommitStatus=false;
static  int g_CACancelStatus=false;
static  pthread_mutex_t g_NACommitStatusmutex = PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t g_CACommitStatusmutex = PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t g_NACancelStatusmutex = PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t g_CACancelStatusmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_NACompleteThreadmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_NACompleteThreadCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t g_CACompleteThreadmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_CACompleteThreadCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t g_NAComplteOperationmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_CAComplteOperationmutex = PTHREAD_MUTEX_INITIALIZER;
static  bool g_NAComplteOperation=false;
static  bool g_CAComplteOperation=false;
static pthread_mutex_t g_APIEnableTheseDenominationsmutex = PTHREAD_MUTEX_INITIALIZER;
static  bool g_APIEnableTheseDenominationsFlag=false;
static bool g_SingleNoteEvent=false;
static pthread_mutex_t g_SingleNoteEventMutex= PTHREAD_MUTEX_INITIALIZER;
static int g_SingleNoteValue=0;
static pthread_mutex_t g_SingleNoteValueMutex= PTHREAD_MUTEX_INITIALIZER;
static int g_CommitTransTime=0;
static bool g_getDenomtransactiontimeout=false;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool SetupNoteAcceptorLogFile() {

          /*

          bool rtcode = false;

          //Set SmartCard General LOG File path
         
          unsigned char LogFilePath[8192];
      
          memset(LogFilePath,'\0',8192);
           
          int day=0,mon=0,yr=0,hr=0,min=0,sec=0;

          char *destStr=NULL,*file=NULL;

	  getDateTime(&day,&mon,&yr,&hr,&min,&sec);

	  destStr = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

          file = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

	  sprintf(destStr,"/Mega_Smartcard_Dt_%d%d%d_Tm_%d_%d.log",yr,mon,day,hr,min,sec);

          ////////////////////////////////////////////////////////////////////////////////////////////////////////

	  if( -1 == GetLogFilePath(file) )
	  {

	          printf("\n/[Mega Designs Pvt. Ltd.] Error Create smartcard general log file.");
                  
                  rtcode = false;

	  } 
          else
          {

		 //Copy trans logfile name
                 strcat(file,destStr);
		 
                 strcat(LogFilePath,file);
                 
                 SetGeneralFileLogPath( LogFilePath );

                 printf("\n[Mega Designs Pvt. Ltd.] successfully created smartcard general log file.");

                 rtcode = true;
                 
         }

         ////////////////////////////////////////////////////////////////////////////////////////////////////////
          
          char logfilemode[LOG_ARRAY_SIZE];

	  memset(logfilemode,'\0',LOG_ARRAY_SIZE);

          if( READ_INI_SUCCESS == GetLogFileMode(logfilemode) )
          {
                 if( 0 == strcmp( logfilemode, "true" ) )
                 {
                        g_LogModeType=true; //now logmode enable
                        printf("\n[Mega Designs Pvt. Ltd.] Log mode enable.");
                 }
                 else if( 0 == strcmp( logfilemode, "false" ) )
                 {
                        g_LogModeType=false; //now logmode disable
                         printf("\n[Mega Designs Pvt. Ltd.] Log mode disable.");
                 }
                 else
                 {
                        g_LogModeType=false; //now logmode disable
                        printf("\n[Mega Designs Pvt. Ltd.] Log mode disable.");
                 }
          }
          else
          {
              g_LogModeType=false;
              printf("\n[Mega Designs Pvt. Ltd.] Log mode disable.");

          }

         ////////////////////////////////////////////////////////////////////////////////////////////////////////
        
         if(NULL!= destStr)
         {
             free(destStr);
         }
        
         if(NULL!=file)
         {
	     free(file);
         }


         return rtcode;
        
         */



}//bool SetupNoteAcceptorLogFile() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetGlobalDenomInfo(int noteValue) {

      pthread_mutex_lock(&g_SingleNoteValueMutex);
 
      g_SingleNoteValue=noteValue;

      pthread_mutex_unlock(&g_SingleNoteValueMutex);
    

}//SetGlobalDenomInfo() end

void SetDenomInfo(int noteValue ) {

                            unsigned char log[200];

                            memset(log,'\0',200);

                            if( (noteValue >= 5)  &&  (noteValue <= 1000) ) 
		            {

				       pthread_mutex_lock(&mut);   

				       g_totalNoteAmountAccepted += noteValue;
		                       
		                       g_totalAmountAccepted += noteValue;

				       g_ttlNmbrOfNote +=1; 

                                       memset(log,'\0',200);

		                       sprintf(log,"[SetDenomInfo()] Note value Rs. %d .",noteValue );

				       //writeFileLoglog);
				      
				       g_acpted_CurrencyDetail[g_denomIndex][0] = NOTE; 
                                       
                                       /////////////////////////////////////////////////////////////
                  
                                       switch(noteValue)
                                       {

                                          case 5: 
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 0;
                                                 break;
					      
                                          case 10:
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 1;
                                                 break;

                                          case 20:
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 2;
                                                 break;

                                          case 50:
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 3;
                                                 break;

                                          case 100:
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 4;
                                                 break;

                                          case 500:
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 5;
                                                 break;

                                          case 1000:
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 6;
                                                 break;

					   default:
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= -1;
                                                 break;

                                       };

                                       g_denomIndex+=1;    
		                       
                                      
		                       memset(log,'\0',200);
		                       
                                       sprintf(log,"[SetDenomInfo()] Accepted Note : Rs. %d  Current Accepted Ammount : Rs. %d .", noteValue,g_totalAmountAccepted);
				       
                                       //writeFileLoglog);

		                       //clear array
		                       memset(log,'\0',200);

		                       sprintf(log,"[SetDenomInfo()] Total Notes Number : %d .", g_ttlNmbrOfNote);

				       //writeFileLoglog);
				       
		                       pthread_mutex_unlock(&mut);


                         
                            }//if( (noteValue >= 5)  &&  (noteValue <= 1000) ) 



}//void SetDenomInfo(int noteVale ) end

void InitSingleNoteData(int fare) {



                          int i=0,j=0;
                         
                          //Init All global variables    

			  pthread_mutex_lock(&mut);

			  g_Fare        = 0;

			  g_maxNoOfCash = 0;

			  g_maxNoOfCoin = 0;

			  g_totalNoteAmountAccepted = 0;

			  g_totalCoinAmountAccepted = 0;

			  g_totalAmountAccepted     = 0;

			  g_ttlNmbrOfNote           = 0;

			  g_ttlNmbrOfCoin           = 0;

			  g_denomIndex = 0;

                          //clear denomation array
			  for( i=0; i< MAX_NMBR_OF_NOTE; i++)  
			  {

				       for(j=0; j<2; j++) 
		                       {

					    g_acpted_CurrencyDetail[i][j] = -1; 

		                       }

			  }

                          g_Fare        = fare;

                          g_maxNoOfCash = 1;

                          g_maxNoOfCoin = 0;

			  pthread_mutex_unlock(&mut);
         


}//void InitSingleNoteData(int fare)

void ResetTransData() {


                          //writeFileLog"[ResetTransData()]  Entry.");

                          int i=0,j=0;
                         
                          //Init All global variables    

			  pthread_mutex_lock(&mut);

			  g_Fare        = 0;

			  g_maxNoOfCash = 0;

			  g_maxNoOfCoin = 0;

			  g_totalNoteAmountAccepted = 0;

			  g_totalCoinAmountAccepted = 0;

			  g_totalAmountAccepted     = 0;

			  g_ttlNmbrOfNote           = 0;

			  g_ttlNmbrOfCoin           = 0;

			  g_denomIndex = 0;

                          //clear denomation array
			  for( i=0; i< MAX_NMBR_OF_NOTE; i++)  
			  {

				       for(j=0; j<2; j++) 
		                       {

					    g_acpted_CurrencyDetail[i][j] = -1; 

		                       }//second for loop

			  }//first for loop

                          g_Fare        = 0;

                          g_maxNoOfCash = 0;

                          g_maxNoOfCoin = 0;

			  pthread_mutex_unlock(&mut);

                          //writeFileLog"[ResetTransData()]  Exit.");
         


}//void InitSingleNoteData(int fare)

//++Single Note API
 #ifdef B2B_NOTE_ACCEPTOR
 
int  GetValidNote(int Denom,int Timeout) {

	      pthread_mutex_lock(&g_SingleNoteEventMutex);

	      if( false == g_SingleNoteEvent ){
		   
			   g_SingleNoteEvent = true ;

			   InitSingleNoteData(Denom);

			   pthread_mutex_lock(&g_SingleNoteValueMutex);
		 
			   g_SingleNoteValue= 0;

			   pthread_mutex_unlock(&g_SingleNoteValueMutex);
		  
	      }else{
			  
		      pthread_mutex_lock(&mut);

		      g_Fare        = Denom;

		      g_maxNoOfCash = 1;

		      g_maxNoOfCoin = 0;

		      pthread_mutex_unlock(&mut);

		      pthread_mutex_lock(&g_SingleNoteValueMutex);
	 
		      g_SingleNoteValue=0;

		      pthread_mutex_unlock(&g_SingleNoteValueMutex);
		     
	      }//ELSE END

	      pthread_mutex_unlock(&g_SingleNoteEventMutex);

	      return( B2BSingleNote_GetValidNote(Denom,Timeout) );


}//int  GetValidNote(int Denom,int Timeout)

int  AcceptCurrentNote(int Denom,int Timeout) {

      //Issue stack comamnd
      
      int notevalue =0 ;

      pthread_mutex_lock(&g_SingleNoteValueMutex);
 
      notevalue  = g_SingleNoteValue ;

      pthread_mutex_unlock(&g_SingleNoteValueMutex);

      int rtcode = 0;

      rtcode = B2BSingleNote_AcceptCurrentNote( Denom,Timeout);
    
      if( 1 ==rtcode )
      { 
           SetDenomInfo( notevalue );
      }
     
      return rtcode ;

}//int  AcceptCurrentNote(int Denom,int Timeout)

int  ReturnCurrentNote(int Timeout){
 
      //issue return command
      return( B2BSingleNote_ReturnCurrentNote( Timeout ) );

}//int  ReturnCurrentNote(int Timeout)

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
#define OFF           40
#define TRACE         41
#define DEBUG         42
#define INFO          43
#define WARN          44
#define ERROR         45
#define FATAL         46
#define ALL           47

-3:  Any other exception
-2:  Return mouth blocked
-1:  Communication failure
 0:  Operation timeout occurred
 1:  Returned

*/

#ifdef B2B_NOTE_ACCEPTOR
int cancelTran_Ver3(int timeout) {

                       
                               //Issue unload command escrow to collection bin
     
                               unsigned char log[200];

                               memset(log,'\0',200); 

                               int value=0,noteStatus=0,try=1,ret2=0,ret3=0,retry=0;

                               //////////////////////////////////////////////////////////////

                              
				  if( g_ttlNmbrOfNote > 0 )
				  {
					   //Check b2b failure state
					   #ifdef NOTE_ACCEPTOR  

					   #ifdef B2B_NOTE_ACCEPTOR
						int rtcode = 0;
						rtcode = CheckB2BFailureState();
						if( 0!=rtcode  )
						{
						  return rtcode;
						     
						}

					   #endif

					   #endif

				  }
				  
                               /////////////////////////////////////////////////////////////

                               pthread_mutex_lock(&mut);  

                               if( g_ttlNmbrOfNote > 0 )
                               {

                                       pthread_mutex_unlock(&mut);
		               
		                       B2BFinishRejectingState();

		                       delay_mSec(200);

		                       unsigned int totalnotes=0;

		                       totalnotes=CalculateCurrentBillQuantity();

		                       for(retry = 1; retry<=1;retry++)  
				       {

						 //writeFileLog"[cancelTran_Ver3()] before reject from escrow.");

		                                 ret2 = RejectFromEscrow(24,g_ttlNmbrOfNote);

		                                 if( SUCCESS == ret2 )
		                                 {
		                                        //writeFileLog"[cancelTran_Ver3()] RejectFromEscrow() return with success.");
		                                        //writeFileLog"[cancelTran_Ver3()] Now no need to retry here.");

		                                        //writeFileLog"[cancelTran_Ver3()] before GetReturnDetail().");

		                                        for(retry = 1; retry<=2;retry++) 
		                                        {

														ret3 = GetReturnDetail(&value,&noteStatus);

														if( SUCCESS == ret3 )  
		                                                {
														   //writeFileLog"[cancelTran_Ver3()] GetReturnDetail() return with success status.");
		                                                   //writeFileLog"[cancelTran_Ver3()] Now no retry here.");

		                                                   break;
		                                                }
														else
		                                                {
				                                   //writeFileLog"[cancelTran_Ver3()] GetReturnDetail() return with fail status.");
		                                                   memset(log,'\0',100); 

		                                                   sprintf(log,"[cancelTran_Ver3()] GetReturnDetail() Retry: %d.",retry);
		                                                   //writeFileLoglog);

		                                                   continue; 
		                                                }
		                                         }//internal for loop end

		                                         //writeFileLog"[cancelTran_Ver3()] after GetReturnDetail().");

		                                         break;
		                                 }
		                                 else
		                                 {
		                                       //writeFileLog"[cancelTran_Ver3()] RejectFromEscrow() return with fail.");

		                                       memset(log,'\0',200);
 
		                                       sprintf(log,"[cancelTran_Ver3()] RejectFromEscrow() Retry: %d.",retry);

		                                       //writeFileLoglog);

		                                       continue;

		                                 }


		                           }//external for loop end

		                           
		                           
		                           //writeFileLog"[cancelTran_Ver3()] after reject from escrow.");

		                           //clear global note number
                                           pthread_mutex_lock(&mut);  

		                           g_ttlNmbrOfNote=0;

                                           pthread_mutex_unlock(&mut);  

                                           if( SUCCESS == ret3 )  
		                           {
                                              //writeFileLog"[cancelTran_Ver3()] Successfully cancel transactions.");

                                              return true;
                                           }
                                           else
                                           {
                                              //writeFileLog"[cancelTran_Ver3()] Failed to do cancel transactions.");

                                              return false;
                                           }
                            
                         }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
                         else
                         {

                                pthread_mutex_unlock(&mut);  

                                //writeFileLog"[cancelTran_Ver3()] No need to cancel transaction due to no  ");

                                return true;


                         }


}//bool cancelTran_Ver3()
#endif

#if defined(JCM_NOTE_ACCEPTOR) 
int cancelTran_Ver3(int timeout){
        return 0;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Return Value: An int indicating one of the following
//-4: Any other exception
//-3: Transport Channel blocked
//-2: Stacker/Cash box full
//-1: Communication failure
// 0: Operation timeout occurred
//1: Stacked

#ifdef B2B_NOTE_ACCEPTOR
int commitTran_Ver3(int timeout) {


                                        unsigned char log[200];

                                        memset(log,'\0',200); 

                                        int value=0,noteStatus=0,try=1,ret2=0,ret3=0;

                                        //////////////////////////////////////////////////////////////

                              
					  if( g_ttlNmbrOfNote > 0 )
					  {
						   //Check b2b failure state
						   #ifdef NOTE_ACCEPTOR  

							        #ifdef B2B_NOTE_ACCEPTOR
								int rtcode = 0;
								rtcode = CheckB2BFailureState();
								if( 0!=rtcode  )
								{
								     return rtcode;
								}

							        #endif

						   #endif

					  }
				  
                                        /////////////////////////////////////////////////////////////

                                        pthread_mutex_lock(&mut);  

                                        if( g_ttlNmbrOfNote > 0 )
                                        {
      
                                                pthread_mutex_unlock(&mut);  

		                                //Issue dispense command escrow to collection bin
		                                
		                                B2BFinishRejectingState();

		                                delay_mSec(200);

		                                unsigned int Currenttotalnotes=0;

		                                Currenttotalnotes=CalculateCurrentBillQuantity();
		                               
		                                //writeFileLog"[commitTran_Ver3()] Before AcceptFromEscrow().");

		                                for(try=1;try<=1;try++)
		                                {  
		                                     
		                                     //start b2b unloading state
		                                     ret2 = AcceptFromEscrow(1,g_ttlNmbrOfNote); 

		                                     if( SUCCESS == ret2 )  
		                                     {

				                                  //writeFileLog"[commitTran_Ver3()] AcceptFromEscrow() return with success.");
				                                  //writeFileLog"[commitTran_Ver3()] Now no retry needed.");

				                                  //writeFileLog"[commitTran_Ver3()] Before GetAcceptedNoteDetail().");

				                                  for(try=1;try<=1;try++)
				                                  {
				                                     
						                             //wait for b2b unloaded state and return when b2b finish with disabled state
		                                                             memset(log,'\0',200);

						                             sprintf(log,"[commitTran_Ver3()] Commit Single Note Timeout: %d. Seconds",(timeout/1000));
						                             //writeFileLoglog);

						                             ret3 = GetAcceptedNoteDetailWithTime(&value,&noteStatus, (timeout/1000) ); 
		                                                             
						                             if(SUCCESS == ret3)
						                             {
						                                  //writeFileLog"[commitTran_Ver3()] GetAcceptedNoteDetail() return with success."); 
						                                  //writeFileLog"[commitTran_Ver3()] No retry needed."); 

						                                  break;

						                              }
						                              else
						                              {
						                                   //writeFileLog"[commitTran_Ver3()] GetAcceptedNoteDetail() return with fail."); 
						                                   memset(log,'\0',200);
                                                                                   sprintf(log,"[commitTran_Ver3()] Retry due to fail return %d.",try);
						                                   //writeFileLoglog);

						                                   continue;
						                              }

						                       }//for(try=1;try<=1;try++) loop end

						                        if(SUCCESS == ret3)
				                                        {
						                            //writeFileLog"[commitTran_Ver3()] GetAcceptedNoteDetail() return with success.");
		                                                        }
						                        else
		                                                        {
						                            //writeFileLog"[commitTran_Ver3()] GetAcceptedNoteDetail() return with fail.");
		                                                        }

						                        //writeFileLog"[commitTran()] After GetAcceptedNoteDetail().");

						                       break;

				                                 }//if( SUCCESS == ret2 )
				                                 else
				                                 {
				                                     memset(log,'\0',200);

				                                     sprintf(log,"[commitTran_Ver3()] AcceptFromEscrow() return with fail %d . Communication Failure.",try);
				                                     //writeFileLoglog);  

				                                     return (-1); //Communication Failure
				                                 }

		                                      }//external foor loop end

		                                      //writeFileLog"[commitTran_Ver3()] After AcceptFromEscrow().");

                                                       
		                                      

                                                      if(SUCCESS == ret3)
                                                      {
                                                              //writeFileLog"[commitTran_Ver3()] Commit successfully done.");
                                                              //clear global note number
		                                              pthread_mutex_lock(&mut); 

				                              g_ttlNmbrOfNote=0;

		                                              pthread_mutex_unlock(&mut); 

                                                              return 1; //Stacked

                                                      }
                                                      else
                                                      {
                                                          //writeFileLog"[commitTran_Ver3()] Commit failed to .");

                                                          return 0; //Operation timeout occurred

                                                      }

         

                                 }//if( g_ttlNmbrOfNote > 0 )
                                 else
                                 {
                                        pthread_mutex_unlock(&mut);  

                                        //writeFileLog"[commitTran_Ver3()] No need to commit here as total notes is less or equal to zero.");

                                        return (-4); //Any other exception; 
 
						
                                 }
 
}//bool commitTran_Ver3()
#endif

#if defined(JCM_NOTE_ACCEPTOR) 
int commitTran_Ver3(int timeout){
        return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int activateDevice( int fnComPort,int fnEscrowClearanceMode,int fntimeout ){
     activateDevice2( fnComPort,1,fnEscrowClearanceMode,fntimeout );
}//int activateDevice( int fnComPort,int fnEscrowClearanceMode,int fntimeout ) end

//++MetroASCRM API AVRM API
int activateDevice2( int fnPortId, int fnDeviceType, int fnEscrowClearanceMode, int fntimeout ){
         
         //++To activate all  cash/coin devices to be ready for accepting cash and coin.
         int ret1=-1,ret2=-1,ret3=-1,ret4=-1;
	     int noteQtyInRcyclCst = 0,coinQtyInEscrow=0;;
	     unsigned char log[200];
         int TransTimeout=0;

         //++Now fully activate device entry
         //++//writeFileLog"[activateDevice()] Entry.");
         AVRM_Currency_writeFileLog("[activateDevice()] Entry.",TRACE);

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //++Check Port Existence

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         if( ( ALL != fnDeviceType ) &&  ( BNA != fnDeviceType ) && ( BCA != fnDeviceType ) ) {
             //++//writeFileLog"[activateDevice() Exit]  Error No note or coin Acceptor mentioned for activate");
             AVRM_Currency_writeFileLog("[activateDevice() Exit]  Error No note or coin Acceptor mentioned for activate",FATAL);
             return -1;
         }//if end

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //++DeviceType 0: All Device 1: NoteAcceptor 2:CoinAcceptor

         //++All device or note acceptor 
         if( ( ALL == fnDeviceType ) ||  ( BNA == fnDeviceType ) ) {
                 
                 #if defined(NOTE_ACCEPTOR) && (defined(B2B_NOTE_ACCEPTOR) || defined(JCM_NOTE_ACCEPTOR))
				 ret1 = OpenCashAcptr(fnPortId);  //++Initialize Note Acceptor
				 #endif

				 //#if defined(NOTE_ACCEPTOR) && defined(BNA_NOTE_ACCEPTOR)
				 //ret1 = OpenCashAcptr();  //++Initialize Note Acceptor
				 //#endif

				 #if !defined(NOTE_ACCEPTOR) 
				 ret1 = SUCCESS;  //++Success
				 #endif

				 #if !defined(CCTALK_DEVICE)
				 //by default CCTALK port open return code set to 1(success) if CCTALK not defined in atvmdevice headers
				 ret2 = SUCCESS;  //++Success
				 #endif

                 if( 1 != ret1 ){
                       AVRM_Currency_writeFileLog("[activateDevice()] Note Acceptor Communication Failure.",FATAL);
                       return COMMUNICATION_FAILURE_NOTE_ACCEPTOR;
                 }//++if end
 
         }//if( ( ALL == fnDeviceType ) ||  ( BNA == fnDeviceType ) )  end

         //++All device or coin acceptor 
         if( ( ALL == fnDeviceType ) ||  ( BCA == fnDeviceType ) ) {

                 #if !defined(NOTE_ACCEPTOR) 
				 ret1 = SUCCESS;  //Success
				 #endif

				 #if defined(CCTALK_DEVICE)
				 
				 if( 0 == IsCCTALKPortOpen() ){
					   //++ret2 = ActivateCCTALKPort( fnPortId);
					   //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
					   ret2 = ActivateCCTALKPortV2( 0,fnPortId );
				 }else{
					   ret2 = SUCCESS;  
				 }
				 
				 #endif

				 #if !defined(CCTALK_DEVICE)
				 ret2 = SUCCESS;  //Success
				 #endif
 
                 if( 0 != ret2 ){
                       ////writeFileLog"[activateDevice()] Coin Acceptor Communication Failure.");
                       AVRM_Currency_writeFileLog("[activateDevice()] Coin Acceptor Communication Failure.",FATAL);
                       return (COMMUNICATION_FAILURE_COIN_ACCEPTOR);
                 }//if end
     
         }//if end

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          //++Get  escrow notes
          if( ( ALL == fnDeviceType ) ||  ( BNA == fnDeviceType ) ) {

				 //++Get Note Acceptor current recycle quantity
				 #ifdef NOTE_ACCEPTOR
				 noteQtyInRcyclCst=0;
				 #ifdef B2B_NOTE_ACCEPTOR
				 //if( CASHCODE_RES_DISABLED == ret1  )
				 if( 1 == ret1  )
				 #endif
				 #ifdef JCM_NOTE_ACCEPTOR
				 if( 1 == ret1 )
				 #endif
				 #ifdef BNA_NOTE_ACCEPTOR
				 if( 1 == ret1 )
				 #endif
				 {
					  if( 1 == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) ){
						 
						 memset(log,'\0',100);
			             sprintf(log,"[activateDevice()] Note present in recycle cassette: %d.",noteQtyInRcyclCst);
						 AVRM_Currency_writeFileLog(log,INFO);

						 
					  }//if( 1 == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) )
					  else
					  {
						  memset(log,'\0',100); 
						  sprintf(log,"[activateDevice()] Unable to find any note in note acceptor");
						  ////writeFileLoglog);
						  AVRM_Currency_writeFileLog(log,WARN);

					  }//else end

				 }//if end

				 #endif
         
         }//if end

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //++Operation of escrow notes and coins
         if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) || ( BCA == fnDeviceType )) {

		          memset(log,'\0',200);
		          sprintf(log,"[activateDevice()] Escrow Clearance Mode: %d.",fnEscrowClearanceMode);
			      //writeFileLoglog);
			      AVRM_Currency_writeFileLog(log,INFO);
			      //1: Send the escrowed note(s)/ coin(s) in the collection bin.
			      if( 1 == fnEscrowClearanceMode ) { //++Accept Notes and Coins
			      				        
		                        AVRM_Currency_writeFileLog("[activateDevice()] Escrow Accept Clearance Mode is defined.",INFO);
		                        
		                        if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ){
										//Send money to collection bin
										if( noteQtyInRcyclCst > 0 ){
											if( true == defaultCommit2(fntimeout,BNA) ){
												AVRM_Currency_writeFileLog("[activateDevice()] Escrow Clearance successfully done.",INFO);
												noteQtyInRcyclCst = 0;
												ret3 = 1; //Device Connected Successfully without escrow notes [return code: 1 ]
											}else{
												AVRM_Currency_writeFileLog("[activateDevice()] Escrow Clearance failed to do.",ERROR);
												ret3 = 0; //Device Connected Successfully with escrow clearance failed [return code: 5 ]
											}//++else end
										}else{
										   AVRM_Currency_writeFileLog("[activateDevice()] No notes in escrow .",INFO);
										   ret3 = 2; //No notes in escrow
										}//++else end
		                        }//if end
		                        
		                        if( ( ALL == fnDeviceType ) || ( BCA == fnDeviceType ) ){
									
									//Send coin to collection bin
									if( true == defaultCommit2(fntimeout,BCE) ){
											////writeFileLog"[activateDevice()] Escrow Clearance successfully done.");
											AVRM_Currency_writeFileLog("[activateDevice()] Escrow Clearance successfully done.",INFO);
											noteQtyInRcyclCst = 0;
											ret4 = SUCCESS; //Device Connected Successfully without escrow notes [return code: 1 ]
									}else{
											////writeFileLog"[activateDevice()] Escrow Clearance failed to do.");
											AVRM_Currency_writeFileLog("[activateDevice()] Escrow Clearance failed to do.",ERROR);
											ret4 = ESCROW_CLEARANCE_FAILED_AS_UNKNOWN; //Device Connected Successfully with escrow clearance failed [return code: 5 ]
									}//else end
		                       
		                        }//if end

		                        if(  ALL == fnDeviceType  ){
		                              

		                        }else if( BNA == fnDeviceType ) {

		                             if( ( 1 == ret1) && ( 1 == ret3) ){
		                                  AVRM_Currency_writeFileLog("[activateDevice()] NoteAcceptor Activated with escrow clearnce success.",INFO);
		                                  return SUCCESS;
		     
		                             }else if( ( 1 == ret1) && ( 0 == ret3) ){
										  
										  noteQtyInRcyclCst = 0;
		                                  if( 1 == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) ) {
												 memset(log,'\0',100); 
												 sprintf(log,"[activateDevice()] After Operation Note present in recycle cassette: %d.",noteQtyInRcyclCst);
												 AVRM_Currency_writeFileLog(log,INFO);
										  }else{
												AVRM_Currency_writeFileLog("[activateDevice()] Read recycling cassette status read failed.",INFO);
										  }  //else end
										  
										  if( 0 == noteQtyInRcyclCst ){
											  AVRM_Currency_writeFileLog("[activateDevice()] NoteAcceptor Activated with escrow empty.",ERROR);
											  return SUCCESS;
									      }else{
											 AVRM_Currency_writeFileLog("[activateDevice()] NoteAcceptor Activated with escrow clearnce failed.",ERROR);
											 return ESCROW_CLEARANCE_FAILED_AS_UNKNOWN;
										  }//else end
										  
		                             }else if( ( 1 == ret1) && ( 2 == ret3) ){
										  AVRM_Currency_writeFileLog("[activateDevice()] NoteAcceptor Activated successfully With Empty Escrow.",INFO);
		                                  return SUCCESS;
									 }else {
		                                  AVRM_Currency_writeFileLog("[activateDevice()] NoteAcceptor Activated successfully.",INFO);
		                                  return ESCROW_CLEARANCE_FAILED_AS_UNKNOWN;
		                             }//else end

		                        }else if( BCA == fnDeviceType ) {

		                              if( ( 0 == ret2) && ( 1 == ret4) ){
		                                  ////writeFileLog"[activateDevice()] CoinAcceptor Activated with escrow clearnce success.");
		                                  AVRM_Currency_writeFileLog("[activateDevice()] CoinAcceptor Activated with escrow clearnce success.",INFO);
		                                  return SUCCESS;
		     
		                             }else if( ( 0 == ret1) && ( 5 == ret4) ){
		                                  ////writeFileLog"[activateDevice()] CoinAcceptor Activated with escrow clearnce failed.");
		                                  AVRM_Currency_writeFileLog("[activateDevice()] CoinAcceptor Activated with escrow clearnce failed.",ERROR);
		                                  return ESCROW_CLEARANCE_FAILED_AS_UNKNOWN;
		                             }else {
		                                  ////writeFileLog"[activateDevice()] CoinAcceptor Activated successfully.");
		                                  AVRM_Currency_writeFileLog("[activateDevice()] CoinAcceptor Activated successfully.",INFO);
		                                  return SUCCESS;
		                             }//else end

		                       }//else if end
		                      

			      }else if( 0 == fnEscrowClearanceMode ) { //0: Retain escrowed note(s)/coin(s) in the escrow
					  ////writeFileLog"[activateDevice()] Escrow Clearance Retain defined.");
					  AVRM_Currency_writeFileLog("[activateDevice()] Escrow Clearance Retain defined.",INFO);
				      return SUCCESS; //++Device Connected Successfully 
			      }else{
					  ////writeFileLog"[activateDevice()] No Escrow Clearance Mode defined.");
					  AVRM_Currency_writeFileLog("[activateDevice()] No Escrow Clearance Mode defined.",INFO);
					  return SUCCESS; //++Device Connected Successfully 
			      }

	     }//if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) || ( BCA == fnDeviceType )) end
	     
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
}//++int activateDevice( int NoteAcceprtorComPort,int EscrowClearanceMode ) end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int deactivateDevice( int fnDeviceType ) {

             //++//writeFileLog"[deactivateDevice()] Entry.");

			 unsigned int rtcode=-1; 
			 
             if( (1 == fnDeviceType) || (0==fnDeviceType) ){
				 
				 //++Deactivate Note Acceptor
				 rtcode=DeactivateNoteAcptr();

				 if(1!=rtcode){
					   ////writeFileLog"[deactivateDevice() Exit] Note acptr deactivate failed!!.");
					   AVRM_Currency_writeFileLog("[deactivateDevice() Exit] Note acptr deactivate failed!!.",ERROR);
					   return OTHER_ERROR;
				 }else{
					   ////writeFileLog"[deactivateDevice() Exit] Note acptr deactivate successfully executed.");
					   AVRM_Currency_writeFileLog("[deactivateDevice() Exit] Note acptr deactivate successfully executed.",INFO);
					   return 0; //++Success

				 }//ELSE END

				
             }//if end
             
             if( (2 == fnDeviceType) || (0==fnDeviceType)  ){
				 
				 
				 //++Deactivate Coin Acceptor
				 rtcode=-1;
                 //++rtcode=DeActivateCCTALKPort();
                 //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
				 rtcode=DeActivateCCTALKPortV2( 0 ); 
                 if(0!=rtcode){
					   ////writeFileLog"[deactivateDevice() Exit] CCTALK port close operation failed.!!");
					   AVRM_Currency_writeFileLog("[deactivateDevice() Exit] CCTALK port close operation failed.!!",ERROR);
					   return OTHER_ERROR;
				 }else{
				       ////writeFileLog"[deactivateDevice() Exit] CCTALK port close operation successfully executed.");
				       AVRM_Currency_writeFileLog("[deactivateDevice() Exit] CCTALK port close operation successfully executed.",INFO);
				       return 0; //Success
				 }//else end

	          } //if end
	          
	         //++//writeFileLog"[deactivateDevice()] Exit.");  
	              
             return 0; //Success


}//deactivateDevice() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Return values are 
//++0001-  decimal 1,  Ready
//++0010 - decimal 2 , Cash acceptor device not ready
//++0100 - decimal 4 , Coin acceptor device not ready
//++1000 – decimal 8 , both device fault

int deviceStatusV2(int fnDeviceType ,int TransTime) {

         
         int  Rs50escrowerror=0,
              Rs100escrowerror=0,
              Rs500escrowerror=0;

         char log[LOG_ARRAY_SIZE];

         int noteAcptrRet=0;

	     int coinAcptrRet=0;

	     int DSCURet=0;
	     
	     int CoinEscrowReturnCode=0;

         char Byte0=0b00000101,
              Byte1=0x00,
              Byte2=0x00,
              Byte3=0x00;

         int  rtcode=0x00;

	     /*
	     #if  defined(B2B_NOTE_ACCEPTOR)
         int RS5NoteQuanity=0,
             RS10NoteQuanity=0,
             RS20NoteQuanity=0,
             RS50NoteQuanity=0,
             RS100NoteQuanity=0,
             RS500NoteQuanity=0, 
             RS1000NoteQuanity=0;
         #endif
		 */
		 	
         //++#if  defined(JCM_NOTE_ACCEPTOR)
         int RS5NoteQuanity=0,
             RS10NoteQuanity=0,
             RS20NoteQuanity=0,
             RS50NoteQuanity=0,
             RS100NoteQuanity=0,
             RS200NoteQuanity=0,
             RS500NoteQuanity=0, 
             RS1000NoteQuanity=0,
             RS2000NoteQuanity=0;
         //++#endif

         //writeFileLog"=========================================================================");  
         //writeFileLog"[deviceStatusV2()] Entry.");

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         if( 1 == fnDeviceType ){
			 
			 //++get note acceptor status
			 #if defined(NOTE_ACCEPTOR)
			 noteAcptrRet = GetNoteAcptrStatus();
			 #endif
			 
         }//if end
         
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         if( 2 == fnDeviceType ){
			 
			 //++get coin acceptor status
			 #if defined(COIN_ACCEPTOR)
			 
			 coinAcptrRet = GetCoinAcptrStatus();
			 delay_mSec(100);
			 CoinEscrowReturnCode = PollFromCoinEscrow();
			 
			 #endif
			 
			 #if !defined(COIN_ACCEPTOR)
			 
			 coinAcptrRet = 1;
			 CoinEscrowReturnCode = 1;
			 
			 #endif
			 
			
			 
         }//if end
         
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         //++Coin Acceptor Status
         
         #if defined(COIN_ACCEPTOR) 
         
         if( 2 == fnDeviceType ){
			 
					 if( 1 == coinAcptrRet ) {
						
							
							/*
							   Byte 0: bit0:Serial Communication 0: Not ok 1:ok
									   bit1:device is ready 0: not ready 1; ready
									   bit2:security door status 0: opened 1 : closed
									   bit3:collection box full or not 0: not full 1 :full
									   bit4:insertion slot blocked is blocked by particle 0:not blocked 1:blocked
									   bit5:transaportchanel is blocked by particle 0:not blocked 1:blocked

							*/
							
							if( ( SUCCESS == CoinEscrowReturnCode ) && (SUCCESS == coinAcptrRet) ){
								
								Byte0=0b00000111;
								
                            } else {
								
								Byte0=0b00000101;
							}
							
							/////////////////////////////////////////////////////////////////////////////////////

							if( ( ( RS10NoteQuanity>=0 ) || ( RS10NoteQuanity <=100) )  && 
								( ( RS20NoteQuanity>=0 ) || ( RS20NoteQuanity <=100) ) 
							  )
							{
									Byte1= (RS20NoteQuanity<<4)+RS10NoteQuanity;

							}

							if( ( ( RS50NoteQuanity>=0 )  || ( RS50NoteQuanity <=100)  )  && 
								( ( RS100NoteQuanity>=0 ) || ( RS100NoteQuanity <=100) ) 
							  )
							{
									Byte2= (RS100NoteQuanity<<4)+RS50NoteQuanity;

							}

							if( ( ( RS500NoteQuanity>=0 )  || ( RS500NoteQuanity <=100)  )  && 
								( ( RS1000NoteQuanity>=0 ) || ( RS1000NoteQuanity <=100) ) 
							  )
							{
									Byte3= (RS1000NoteQuanity<<4)+RS500NoteQuanity;

							}
					
							rtcode = (Byte3<<24)+(Byte2<<16)+(Byte1<<8)+Byte0;

							memset(log,'\0',LOG_ARRAY_SIZE);
                            sprintf(log,"[deviceStatusV2()] Return Code =0x%xh.",rtcode);
							//writeFileLoglog);
                            //writeFileLog"[deviceStatusV2() Exit] Coin Acceptor is  ready.");
							//writeFileLog"=========================================================================");  

							return(rtcode); 


					 }//if( SUCCESS == coinAcptrRet )  end

					 if( 1 != coinAcptrRet  ) {
				   
							 //writeFileLog"[deviceStatusV2() Exit] Coin acceptor fault.");
							 //writeFileLog"=========================================================================");  

							 /*
							   Byte 0: 
									   bit0:Serial Communication 0: Not ok 1:ok

									   bit1:device is ready 0: not ready 1; ready

									   bit2:security door status 0: opened 1 : closed

									   bit3:collection box full or not 0: not full 1 :full

									   bit4:insertion slot blocked is blocked by particle 
											0:not blocked 1:blocked

									   bit5:transaportchanel is blocked by particle 0:not blocked 1:blocked

									   bit6 and bit7 Escrow Exceeds 

							*/

							 Byte0=0b00000100;
							 Byte3=0b00000000;
							 Byte2=0b00000000;
							 Byte1=0b00000000;
							 rtcode = (Byte3<<24)+(Byte2<<16)+(Byte1<<8)+Byte0;
							 return(rtcode); 

						
	                     }//if( SUCCESS != coinAcptrRet )
	                     
         
	     }//if( 2 == fnDeviceType ) end
         
         #endif

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         //++Note Acceptor Status
         
         #if defined(NOTE_ACCEPTOR) 
         
         if( 1 == fnDeviceType ){
			 
					 #if defined(B2B_NOTE_ACCEPTOR)
					 if( ( CASHCODE_RES_DISABLED ==  noteAcptrRet  ) ||( CASHCODE_RES_HOLDING  ==  noteAcptrRet  ) || ( CASHCODE_RES_REJECTING == noteAcptrRet  ) ) 
					 #endif
					 #if defined(JCM_NOTE_ACCEPTOR) 
					 if( 1 == noteAcptrRet )
					 #endif
					 #if defined(BNA_NOTE_ACCEPTOR) 
					 if(  1 == noteAcptrRet )
					 #endif
					 {
					   
							/*
							#if defined(B2B_NOTE_ACCEPTOR)

							RS5NoteQuanity=-1,
							RS10NoteQuanity=-1,
							RS20NoteQuanity=-1,
							RS50NoteQuanity=-1,
							RS100NoteQuanity=-1,
							RS500NoteQuanity=-1, 
							RS1000NoteQuanity=-1;
							
							unsigned char PollReplyPacket[30];
							
							int PollBufferLength=30;
							
							int PollReplyPacketlength=0;

							rtcode=GetCurrentB2BState( PollReplyPacket,
													   PollBufferLength,
													   &PollReplyPacketlength);

							//if poll success
							if(1==rtcode)
							{
								  if( CASHCODE_RES_GENERIC_FAIL != PollReplyPacket[3] )
								  {
										  if(SUCCESS ==  B2BGetEscrowNotesNumbers( &RS5NoteQuanity,
														   &RS10NoteQuanity,
														   &RS20NoteQuanity,
														   &RS50NoteQuanity ,
														   &RS100NoteQuanity ,
														   &RS500NoteQuanity, 
														   &RS1000NoteQuanity,
														   2
																			))
										 {

											  //writeFileLog"[deviceStatus() Exit] B2B Extended cassette status get successfully.");

										 }// if(SUCCESS ==  B2BGetEscrowNotesNumbers)

										 }//if( CASHCODE_RES_GENERIC_FAIL != PollReplyPacket[3] )
										
							}//if(1==rtcode)
									
							#endif
							
							/*
							#if defined(B2B_NOTE_ACCEPTOR)
							
							RS5NoteQuanity=-1,
							RS10NoteQuanity=-1,
							RS20NoteQuanity=-1,
							RS50NoteQuanity=-1,
							RS100NoteQuanity=-1,
							RS500NoteQuanity=-1, 
							RS1000NoteQuanity=-1;
							
							if(SUCCESS ==  B2BGetEscrowNotesNumbers(  &RS5NoteQuanity,
																	   &RS10NoteQuanity,
																	   &RS20NoteQuanity,
																	   &RS50NoteQuanity ,
																	   &RS100NoteQuanity ,
																	   &RS500NoteQuanity, 
																	   &RS1000NoteQuanity,
																	   2 ))
							{

									  //writeFileLog"[deviceStatus() Exit] B2B Extended cassette status get successfully.");

							 }// if(SUCCESS ==  B2BGetEscrowNotesNumbers)

							 
							 #endif
							 */
							 
							 #if defined(JCM_NOTE_ACCEPTOR)
							 
							 
							 if(SUCCESS == JCMGetEscrowNotesNumbers(   &RS5NoteQuanity,
																	   &RS10NoteQuanity,
																	   &RS20NoteQuanity,
																	   &RS50NoteQuanity ,
																	   &RS100NoteQuanity ,
																	   &RS200NoteQuanity ,
																	   &RS500NoteQuanity, 
																	   &RS1000NoteQuanity,
																	   &RS2000NoteQuanity,
																	   2
																   ))
							 {

								 //writeFileLog"[deviceStatusV2() Exit] JCM Escrow Notes get successfully.");

							 }// if(SUCCESS ==  JCMGetEscrowNotesNumbers) end

							 #endif
							
							//////////////////////////////////////////////////////////////////////////
							  Byte0=0b00000111;
							//////////////////////////////////////////////////////////////////////////

							/*
							   Byte 0: bit0:Serial Communication 0: Not ok 1:ok
									   bit1:device is ready 0: not ready 1; ready
									   bit2:security door status 0: opened 1 : closed
									   bit3:collection box full or not 0: not full 1 :full
									   bit4:insertion slot blocked is blocked by particle 0:not blocked 1:blocked
									   bit5:transaportchanel is blocked by particle 0:not blocked 1:blocked

							*/

							#if defined(NOTE_ACCEPTOR) && defined(B2B_NOTE_ACCEPTOR)
							//Insertion slot is blocked 1:blocked
							if(( CASHCODE_RES_HOLDING  ==  noteAcptrRet  ) ||
							   ( CASHCODE_RES_REJECTING == noteAcptrRet  )
							)
							{
								   Byte0=Byte0 | 0b00001000;

							}
							#endif

							/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

							//++Error Checking rupees more than 15

							if(  RS10NoteQuanity >= 15 ) {
								  //writeFileLog"[deviceStatusV2() Exit] Rs 10 Escorw Number exceed than 15.");
								  RS10NoteQuanity = 15;
								  
							}

							if(  RS20NoteQuanity >= 15 ) {
								  //writeFileLog"[deviceStatusV2() Exit] Rs 20 Escorw Number exceed than 15.");
								  RS20NoteQuanity = 15;
							}

							if(  RS50NoteQuanity >= 15 ) {
								  //writeFileLog"[deviceStatusV2() Exit] Rs 50 Escorw Number exceed than 15.");
								  RS50NoteQuanity = 15;
								  Rs50escrowerror= 1;
							}

							if(  RS100NoteQuanity >= 15 ) {
								  //writeFileLog"[deviceStatusV2() Exit] Rs 100 Escorw Number exceed than 15.");
								  RS100NoteQuanity = 15;
								  Rs100escrowerror=1;
							}
			 
							if(  RS500NoteQuanity >= 15 ) {
								  //writeFileLog"[deviceStatusV2() Exit] Rs 500 Escorw Number exceed than 15.");
								  RS500NoteQuanity = 15;
								  Rs500escrowerror=1;
							}
						   

							//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

							if( ( ( RS10NoteQuanity>=0 ) || ( RS10NoteQuanity <=100) )  && 
								( ( RS20NoteQuanity>=0 ) || ( RS20NoteQuanity <=100) ) 
							  )
							{
									Byte1= (RS20NoteQuanity<<4)+RS10NoteQuanity;

							}

							if( ( ( RS50NoteQuanity>=0 )  || ( RS50NoteQuanity <=100)  )  && 
								( ( RS100NoteQuanity>=0 ) || ( RS100NoteQuanity <=100) ) 
							  )
							{
									Byte2= (RS100NoteQuanity<<4)+RS50NoteQuanity;

							}

							if( ( ( RS500NoteQuanity>=0 )  || ( RS500NoteQuanity <=100)  )  && 
								( ( RS1000NoteQuanity>=0 ) || ( RS1000NoteQuanity <=100) ) 
							  )
							{
									Byte3= (RS1000NoteQuanity<<4)+RS500NoteQuanity;

							}
							
							//Set Escrow bit 
							if( 1 == Rs50escrowerror ){
								  //writeFileLog"[deviceStatusV2() Exit] Rs 50 Escorw error bit set.");
								  Byte0=Byte0+0b01000000; 
							} else if( 1 == Rs100escrowerror ){
								  //writeFileLog"[deviceStatusV2() Exit] Rs 100 Escorw error bit set.");
								  Byte0=Byte0+0b10000000; 
							}else if( 1 == Rs500escrowerror ){
								  //writeFileLog"[deviceStatusV2() Exit] Rs 500 Escorw error bit set.");
								  Byte0=Byte0+0b11000000; 
							}
							rtcode = (Byte3<<24)+(Byte2<<16)+(Byte1<<8)+Byte0;

							memset(log,'\0',LOG_ARRAY_SIZE);

							sprintf(log,"[deviceStatusV2()] Return Code =0x%xh.",rtcode);
												
							//writeFileLoglog);

							//writeFileLog"[deviceStatusV2() Exit] Note Acceptor is  ready.");
							//writeFileLog"=========================================================================");  

							return(rtcode); 


					 }//if( ( CASHCODE_RES_DISABLED == noteAcptrRet ) ) 

					 #if  defined(B2B_NOTE_ACCEPTOR)
					 if( CASHCODE_RES_DISABLED != noteAcptrRet    )
						 #endif
						 #if  defined(JCM_NOTE_ACCEPTOR) 
					 if(  1 != noteAcptrRet   )
						 #endif
						 #if  defined(BNA_NOTE_ACCEPTOR) 
					 if( 1 != noteAcptrRet  )
						 #endif
					 {
				   
						 //writeFileLog"[deviceStatusV2() Exit] Note acceptor fault.");
						 //writeFileLog"=========================================================================");  

						 /*
						   Byte 0: 
								   bit0:Serial Communication 0: Not ok 1:ok

								   bit1:device is ready 0: not ready 1; ready

								   bit2:security door status 0: opened 1 : closed

								   bit3:collection box full or not 0: not full 1 :full

								   bit4:insertion slot blocked is blocked by particle 
										0:not blocked 1:blocked

								   bit5:transaportchanel is blocked by particle 0:not blocked 1:blocked

								   bit6 and bit7 Escrow Exceeds 

						*/

						 Byte0=0b00000100;
						 Byte3=0b00000000;
						 Byte2=0b00000000;
						 Byte1=0b00000000;
						 rtcode = (Byte3<<24)+(Byte2<<16)+(Byte1<<8)+Byte0;
						 return(rtcode); 

						 ///////////////////////////////////////////////////////////////////////////////////////////
                 
	         }//if( CASHCODE_RES_DISABLED != noteAcptrRet    )
         
	     }//if( 1 == fnDeviceType ) end
         
         #endif
         
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         
         
}//deviceStatus() end here

//++AVRM API
void deviceStatusV3(char *rtArrray , int fnDeviceType ,int TransTime) {

         
         int  Rs50escrowerror=0,
              Rs100escrowerror=0,
              Rs500escrowerror=0;
         char log[LOG_ARRAY_SIZE];
         int noteAcptrRet=0;
         int coinAcptrRet=0;
         int DSCURet=0;
	     int CoinEscrowReturnCode=0;
         char Byte0=0b00000101,
              Byte1=0x00,
              Byte2=0x00,
              Byte3=0x00,
              Byte4=0x00,NAStatusByte=0x00,CAStatusByte=0x00;
         int  rtcode=0x00;
         
         int RS5NoteQuanity=0,
             RS10NoteQuanity=0,
             RS20NoteQuanity=0,
             RS50NoteQuanity=0,
             RS100NoteQuanity=0,
             RS200NoteQuanity=0,
             RS500NoteQuanity=0, 
             RS1000NoteQuanity=0,
             RS2000NoteQuanity=0;
         
         AVRM_Currency_writeFileLog("[deviceStatusV3()] Entry.",TRACE);

         if( 1 == fnDeviceType ) {
			 
			 //++get note acceptor status
			 #if defined(NOTE_ACCEPTOR)
			 noteAcptrRet = GetNoteAcptrStatusV2();
			 #endif
			 
         }//if end
         
         if( 2 == fnDeviceType ) {
			 
			 //++get coin acceptor status
			 #if defined(COIN_ACCEPTOR)
			 
			 delay_mSec(100);
			 coinAcptrRet = GetCoinAcptrStatus();
			 
			 delay_mSec(100);
			 CoinEscrowReturnCode = PollFromCoinEscrow();
			 
			 #endif
			 
			 #if !defined(COIN_ACCEPTOR)
			 
			 coinAcptrRet = 1; //++Failed
			 CoinEscrowReturnCode = 1; //++Failed
			 
			 #endif

         }//if end
         
         char doorstatusbyte=0b00000000;
         int topdoorOpenStatus=0,cashboxOpenStatus=0,bottomdoorOpenStatus=0;
         
         //++Get Door Status (Start From Bit0) Bit2:security door status 0: opened 1 : closed
         if( ( 1 == fnDeviceType ) || ( 2 == fnDeviceType )  ) {
			              
                            //++getSecuredState(  int* const topdoorOpenStatus, int* const alarmOnStatus,int* const cashboxOpenStatus,int* const bottomdoorOpenStatus, int* const PrinterCoverstatus)  
					        if( 1 == getSecuredState(  &topdoorOpenStatus, NULL,&cashboxOpenStatus,&bottomdoorOpenStatus, NULL )  ){
								   //++ Bit 2
								   //++ C:    0: Close  1:Open
								   //++ Java: 0: Open   1:Close
								   if( ( 0 == topdoorOpenStatus) && (0==cashboxOpenStatus) && (0==bottomdoorOpenStatus) ) {
									    AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Door Close [Top/bottom/vault all door closed].",INFO);
                                        doorstatusbyte = 0b00000100; //++Door Close
                                   } else if( ( 1 == topdoorOpenStatus) || (1==bottomdoorOpenStatus) || (1==cashboxOpenStatus) ) {
										AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Door Open [Top/bottom/vault any door opened].",INFO);
                                        doorstatusbyte = 0b00000000; //++Door Open
								   }else {
									    AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Door Open.",INFO);
                                        doorstatusbyte = 0b00000000; //++Door Open
                                   }//++else end 
                                   
						     }//++if end
         
	     }//if end
	     
	     memset(log,'\0',LOG_ARRAY_SIZE);
         sprintf(log,"[deviceStatusV3()] Door Status Byte=0x%xh.",doorstatusbyte );
	     AVRM_Currency_writeFileLog(log,INFO);
	     
         //++Coin Acceptor Status
         #if defined(COIN_ACCEPTOR) 
         
         if( 2 == fnDeviceType ) {
			 
			         //++Coin Acceptor And Coin Escrow Success Block
					 if( ( 0 == coinAcptrRet ) && ( 0 == CoinEscrowReturnCode) ) {
						
							
							///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            //   Byte 1:  bit0:Serial Communication 0: ok 1: notok
						    //		      bit1:device is ready 0: ready 1; not ready
							//		      bit2:security door status 0: opened 1 : closed
							//		      bit3:Escrow status 0: Empty 1 : Not Empty
							//		      bit4:collection box full or not 0: full 1 : not full
							//		      bit5:insertion slot blocked is blocked by particle 0:clear 1:blocked
							//		      bit6:transaportchanel is blocked by particle 0:Clear 1:not blocked
							////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							
							//++ Bit 0 & 1: Communication  and Ready Status Success: 0
							if( ( 0 == CoinEscrowReturnCode ) && (0 == coinAcptrRet) ){
								CAStatusByte=0b00010000; //Default (bit2)Door Status Opened/(bit3)Escrow Status Empty/collection box not full 
                            } else {
								CAStatusByte=0b00010011; //Default (bit2)Door Status Opened/(bit3)Escrow Status Empty/collection box not full 
							}//else end
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] CAStatusByte=0x%xh.",NAStatusByte );
							////writeFileLoglog);
							AVRM_Currency_writeFileLog(log,INFO);
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] doorstatusbyte=0x%xh.",doorstatusbyte );
							AVRM_Currency_writeFileLog(log,INFO);
							
							//++Bit 2: Door Status
							CAStatusByte = CAStatusByte| doorstatusbyte;
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] After Set doorstatusbyte CAStatusByte=0x%xh.",CAStatusByte );
							AVRM_Currency_writeFileLog(log,INFO);
							
		                    //++Byte 0: Execution Status of API
							rtArrray[0] = 0x00; //++Operation Success
							
							//++Note Acceptor Status
							//++Byte 1: Note Accepter status
							//++Byte1:0 Note Accepter Communication status
							//++Byte1:1 Note Accepter Readiness
							//++Byte1:2 Security Door status
							//++Byte1:3 Escrow status
							//++Byte1:4 Collection Box status
							//++Byte1:5 Insertion Slot status
							//++Byte1:6 Transport Channel status
							//++Byte1:7 RFU
							rtArrray[1] = 0b00000000; //++Note Acceptor Status
							
														
							//++Coin Acceptor Status
							//++Byte2:Bit 0 Coin Accepter And Escrow Communication status
							//++Byte2:Bit 1 Coin Accepter Readiness
							//++Byte2:Bit 2 Security Door status
							//++Byte2:Bit 3 Escrow status
							//++Byte2:Bit 4 Collection Box status
							//++Byte2:Bit 5 Insertion Slot status
							//++Byte2:Bit 6 Transport Channel status
							//++Byte2:Bit 7 RFU
                            rtArrray[2] = CAStatusByte;  //++Coin Acceptor Status
                            
                            //++Escrow Notes
                            rtArrray[3] = 0b00000000; //++INR 5   and 10
                            rtArrray[4] = 0b00000000; //++INR 20  and 50
                            rtArrray[5] = 0b00000000; //++INR 100 and 200
                            rtArrray[6] = 0b00000000; //++INR 500 and 1000
                            rtArrray[7] = 0b00000000; //++INR 2000
                            rtArrray[8] = 0b00000000; //++RFU
                            
                            //++Escrow Coins
                            //++Byte10:0-3 Indicates no of escrowed INR 5 Coins
                            //++Byte10:4-7 Indicates no of escrowed INR 10 Coins
                            rtArrray[9]  = 0b00000000; //++RFU
                            rtArrray[10] = Byte1; 
                            rtArrray[11] = 0b00000000; //++RFU
                            
							memset(log,'\0',LOG_ARRAY_SIZE);
                            sprintf(log,"[deviceStatusV3()] Coin Acceeptor Operation Status Bytes =0x%xh.",rtArrray[0]);
							AVRM_Currency_writeFileLog(log,INFO);
							
							memset(log,'\0',LOG_ARRAY_SIZE);
                            sprintf(log,"[deviceStatusV3()] Coin Acceeptor Status Bytes =0x%xh.",rtArrray[2]);
							AVRM_Currency_writeFileLog(log,INFO);
							
							
							AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Coin Acceptor success block.",INFO);
							AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Coin Acceptor is ready.",INFO);
							 
					 }//if( SUCCESS == coinAcptrRet )  end

                     //++Coin Acceptor And Coin Escrow Failed Block
					 if( ( 1 == coinAcptrRet ) && ( 1 == CoinEscrowReturnCode) ) {
				   
							 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                             //   Byte 0: bit0:Serial Communication 0: ok 1: notok
						     //		      bit1:device is ready 0: ready 1; not ready
							 //		      bit2:security door status 0: opened 1 : closed
							 //		      bit3:Escrow status 0: Empty 1 : Not Empty
							 //		      bit3:collection box full or not 0: full 1 : not full
							 //		      bit4:insertion slot blocked is blocked by particle 0:clear 1:blocked
							 //		      bit5:transaportchanel is blocked by particle 0:Clear 1:not blocked
							 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							
                             //++Byte 0: Execution Status of API
                             rtArrray[0] = 0x01; //++Operation Failed
                             
                             //++Note Acceptor Status
							 //++Byte 1: Note Accepter status
							 //++Byte1:0 Note Accepter Communication status
							 //++Byte1:1 Note Accepter Readiness
							 //++Byte1:2 Security Door status
							 //++Byte1:3 Escrow status
							 //++Byte1:4 Collection Box status
							 //++Byte1:5 Insertion Slot status
							 //++Byte1:6 Transport Channel status
							 //++Byte1:7 RFU
							 rtArrray[1] = 0b00000111; //++Note Acceptor Status
							 
							 //++Coin Acceptor Status
							 //++Byte2:0 Coin Accepter And Escrow Communication status
							 //++Byte2:1 Coin Accepter Readiness
							 //++Byte2:2 Security Door status
							 //++Byte2:3 Escrow status
							 //++Byte2:4 Collection Box status
							 //++Byte2:5 Insertion Slot status
							 //++Byte2:6 Transport Channel status
							 //++Byte2:7 RFU
                             rtArrray[2] = 0b00000011; //++Coin Acceptor Status
                             
                             //++Bit 2: Door Status
							 rtArrray[2] = rtArrray[2]|doorstatusbyte;
                            
                             //++Escrow Notes
                             rtArrray[3] = 0b00000000; //++INR 5   and 10
                             rtArrray[4] = 0b00000000; //++INR 20  and 50
                             rtArrray[5] = 0b00000000; //++INR 100 and 200
                             rtArrray[6] = 0b00000000; //++INR 500 and 1000
                             rtArrray[7] = 0b00000000; //++INR 2000
                             rtArrray[8] = 0b00000000; //++RFU
                             
                             //++Escrow Coins
                             //++Byte10:0-3 Indicates no of escrowed INR 5 Coins
                             //++Byte10:4-7 Indicates no of escrowed INR 10 Coins
                             rtArrray[9]  = 0b00000000; //++RFU
                             rtArrray[10] = 0b00000000; 
                             rtArrray[11] = 0b00000000; //++RFU
                             
                             memset(log,'\0',LOG_ARRAY_SIZE);
                             sprintf(log,"[deviceStatusV3()] Coin Acceeptor Operation Status Bytes =0x%xh.",rtArrray[0]);
							 AVRM_Currency_writeFileLog(log,INFO);
							
							 memset(log,'\0',LOG_ARRAY_SIZE);
                             sprintf(log,"[deviceStatusV3()] Coin Acceeptor Status Bytes =0x%xh.",rtArrray[2]);
							 AVRM_Currency_writeFileLog(log,INFO);
                             
							 
							 AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Coin Acceptor failed block.",INFO);
							 AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Coin Acceptor is not ready.",INFO);
							 
	                     }//if( SUCCESS != coinAcptrRet )
	                     
         
	     }//if( 2 == fnDeviceType ) end
         
         #endif

         //++Note Acceptor Status
         #if defined(NOTE_ACCEPTOR) 
         
         if( 1 == fnDeviceType ) {
			 
			         //++Note Acceptor Status is Ok			 
					 #if defined(B2B_NOTE_ACCEPTOR)
					 if( ( CASHCODE_RES_DISABLED ==  noteAcptrRet  ) ||( CASHCODE_RES_HOLDING  ==  noteAcptrRet  ) || ( CASHCODE_RES_REJECTING == noteAcptrRet  ) ) 
					 #endif
					 #if defined(JCM_NOTE_ACCEPTOR) 
					 if( 1 == noteAcptrRet )
					 #endif
					 #if defined(BNA_NOTE_ACCEPTOR) 
					 if(  1 == noteAcptrRet )
					 #endif
					 {
						 
							  //++Get Note Acceoptor Escrow Notes
							 #if defined(B2B_NOTE_ACCEPTOR)
							 if(0 == B2BGetEscrowNotesNumbersV2( &RS5NoteQuanity,&RS10NoteQuanity,&RS20NoteQuanity,&RS50NoteQuanity ,&RS100NoteQuanity,&RS200NoteQuanity,&RS500NoteQuanity, &RS1000NoteQuanity,&RS2000NoteQuanity,2)) {
								 AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] B2B Escrow Notes get successfully.",INFO);
							 }else{
								AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] B2B Escrow Notes get successfully.",INFO);
						     }//else end
                             #endif
					   
					         //++Get Note Acceoptor Escrow Notes
							 #if defined(JCM_NOTE_ACCEPTOR)
							 if(0 == JCMGetEscrowNotesNumbers( &RS5NoteQuanity,&RS10NoteQuanity,&RS20NoteQuanity,&RS50NoteQuanity ,&RS100NoteQuanity,&RS200NoteQuanity,&RS500NoteQuanity, &RS1000NoteQuanity,&RS2000NoteQuanity,2)) {
								 AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] JCM Escrow Notes get successfully.",INFO);
							 }else{
								AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] JCM Escrow Notes get successfully.",INFO);
						     }//else end
                             #endif
						     
						    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            //   Byte 1:  bit0:Serial Communication 0: ok 1: notok
						    //		      bit1:device is ready 0: ready 1; not ready
							//		      bit2:security door status 0: opened 1 : closed
							//		      bit3:Escrow status 0: Empty 1 : Not Empty
							//		      bit4:collection box full or not 0: full 1 : not full
							//		      bit5:insertion slot blocked is blocked by particle 0:clear 1:blocked
							//		      bit6:transaportchanel is blocked by particle 0:Clear 1:not blocked
							////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							 						
							NAStatusByte=0b00010000; //++Default Door Status Open/collection box not full
							
							char escrowbytes=0b00000000;
							if( (RS5NoteQuanity > 0 )  && (RS10NoteQuanity>0)  && (RS20NoteQuanity>0)  && (RS50NoteQuanity>0)   && 
							    (RS100NoteQuanity > 0) && (RS200NoteQuanity>0) && (RS500NoteQuanity>0) && (RS1000NoteQuanity>0) && (RS2000NoteQuanity>0) ) {
							      escrowbytes=0b00001000;	 
							}//++if end
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] NAStatusByte=0x%xh.",NAStatusByte );
							AVRM_Currency_writeFileLog(log,INFO);
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] doorstatusbyte=0x%xh.",doorstatusbyte );
							AVRM_Currency_writeFileLog(log,INFO);
	      
							//++Bit 2: Set Door Status
							NAStatusByte=NAStatusByte|doorstatusbyte;
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] After Set Door Status NAStatusByte=0x%xh.",NAStatusByte );
							AVRM_Currency_writeFileLog(log,INFO);
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] escrowbytes=0x%xh.",escrowbytes );
							AVRM_Currency_writeFileLog(log,INFO);
							
							//++Bit 3: Set Note Acceptor Escrow Bytes Status
							NAStatusByte=NAStatusByte|escrowbytes;
							
							memset(log,'\0',LOG_ARRAY_SIZE);
							sprintf(log,"[deviceStatusV3()] After Set Escrow Byte Status NAStatusByte=0x%xh.",NAStatusByte );
							AVRM_Currency_writeFileLog(log,INFO);
							
							//++Error Checking rupees more than 15
                            
							if(  RS5NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 5 Escorw Number exceed than 15.",INFO);
								  RS5NoteQuanity = 15;
							}//if end
							
							if(  RS10NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 10 Escorw Number exceed than 15.",INFO);
								  RS10NoteQuanity = 15;
								  
							}//if end

							if(  RS20NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 20 Escorw Number exceed than 15.",INFO);
								  RS20NoteQuanity = 15;
							}//if end

							if(  RS50NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 50 Escorw Number exceed than 15.",INFO);
								  RS50NoteQuanity = 15;
								  Rs50escrowerror= 1;
							}//if end

							if(  RS100NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 100 Escorw Number exceed than 15.",INFO);
								  RS100NoteQuanity = 15;
								  Rs100escrowerror=1;
							}//if end
							
							if(  RS200NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 200 Escorw Number exceed than 15.",INFO);
								  RS200NoteQuanity = 15;
							}//if end
			 
							if(  RS500NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 500 Escorw Number exceed than 15.",INFO);
								  RS500NoteQuanity = 15;
								  Rs500escrowerror=1;
							}//if end
							
							if(  RS2000NoteQuanity >= 15 ) {
								  AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Rs 2000 Escorw Number exceed than 15.",INFO);
								  RS2000NoteQuanity = 15;
							}//if end
						   
														
							//++INR 5 and 10
							if( ( ( RS5NoteQuanity>=0 )  ||  ( RS5NoteQuanity <=100) )  && 
								( ( RS10NoteQuanity>=0 ) ||  ( RS10NoteQuanity <=100) ) 
							  )
							{
									Byte0= (RS10NoteQuanity<<4)+RS5NoteQuanity;

							}

                            //++INR 20 and 50
							if( ( ( RS20NoteQuanity>=0 )  ||  ( RS20NoteQuanity <=100)  )  && 
								( ( RS50NoteQuanity>=0 )  ||  ( RS50NoteQuanity <=100) ) 
							  )
							{
									Byte1= (RS50NoteQuanity<<4)+RS20NoteQuanity;

							}
							
							//++INR 100 and 200
							if( ( ( RS100NoteQuanity>=0 )  ||  ( RS100NoteQuanity <=100)  )  && 
								( ( RS200NoteQuanity>=0 )  ||  ( RS200NoteQuanity <=100) ) 
							  )
							{
									Byte2= (RS200NoteQuanity<<4)+RS100NoteQuanity;

							}

                            //++INR 500 and 1000
							if( ( ( RS500NoteQuanity>=0 )  || ( RS500NoteQuanity <=100)  )  && 
								( ( RS1000NoteQuanity>=0 ) || ( RS1000NoteQuanity <=100) ) 
							  )
							{
									Byte3= (RS1000NoteQuanity<<4)+RS500NoteQuanity;

							}
							
							//++INR 2000
							if( ( RS2000NoteQuanity>=0 )  || ( RS2000NoteQuanity <=100) )
							{
									Byte4= RS2000NoteQuanity;

							}//if end
					
							//++Byte 0: Execution Status of API
							rtArrray[0] = 0x00; //++Operation successful
							
							//++Byte 1: Note Accepter status
							//++Byte1:0 Note Accepter Communication status
							//++Byte1:1 Note Accepter Readiness
							//++Byte1:2 Security Door status
							//++Byte1:3 Escrow status
							//++Byte1:4 Collection Box status
							//++Byte1:5 Insertion Slot status
							//++Byte1:6 Transport Channel status
							//++Byte1:7 RFU
							rtArrray[1] = NAStatusByte;
                            
                            //++Byte 2: Coin Accepter status
                            //++Byte2:0 Coin Accepter And Escrow Communication status
							//++Byte2:1 Coin Accepter Readiness
							//++Byte2:2 Security Door status
							//++Byte2:3 Escrow status
							//++Byte2:4 Collection Box status
							//++Byte2:5 Insertion Slot status
							//++Byte2:6 Transport Channel status
							//++Byte2:7 RFU
                            rtArrray[2] = 0b00000000; 
                            
                            //++Byte3-7 : Escrowed Notes 
                            rtArrray[3] = Byte0; //++INR 5   and 10
                            rtArrray[4] = Byte1; //++INR 20  and 50
                            rtArrray[5] = Byte2; //++INR 100 and 200
                            rtArrray[6] = Byte3; //++INR 500 and 1000
                            rtArrray[7] = Byte4; //++INR 2000
                            rtArrray[8] = 0b00000000; //++RFU
                            
                            //++Byte9-11 :Escrowed Coins
                            //++Byte10:0-3 Indicates no of escrowed INR 5 Coins
                            //++Byte10:4-7 Indicates no of escrowed INR 10 Coins
                            rtArrray[9]  = 0b00000000; //++RFU
                            rtArrray[10] = 0b00000000; 
                            rtArrray[11] = 0b00000000; //++RFU
                            
                            memset(log,'\0',LOG_ARRAY_SIZE);
                            sprintf(log,"[deviceStatusV3()] Note Acceeptor Operation Status Bytes =0x%xh.",rtArrray[0]);
							//writeFileLoglog);
							AVRM_Currency_writeFileLog(log,INFO);
							
							memset(log,'\0',LOG_ARRAY_SIZE);
                            sprintf(log,"[deviceStatusV3()] Note Acceeptor Status Bytes =0x%xh.",rtArrray[1]);
							//writeFileLoglog);
							AVRM_Currency_writeFileLog(log,INFO);
							
							AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Note Acceptor is ready.",INFO);
							AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Note Acceptor success block.",INFO);
                            ////writeFileLog"[deviceStatusV3() Exit] Note Acceptor is ready.");
						    ////writeFileLog"[deviceStatusV3() Exit] Note Acceptor success block.");
														
					 }//if( ( CASHCODE_RES_DISABLED == noteAcptrRet ) ) 

                     //++Note Acceptor Status is Not Ok		
					 #if  defined(B2B_NOTE_ACCEPTOR)
					 if( CASHCODE_RES_DISABLED != noteAcptrRet    )
					 #endif
					 #if  defined(JCM_NOTE_ACCEPTOR) 
					 if(  1 != noteAcptrRet   )
					 #endif
					 #if  defined(BNA_NOTE_ACCEPTOR) 
					 if( 1 != noteAcptrRet  )
					 #endif
					 {
					   
							///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            //   Byte 0: bit0:Serial Communication 0: ok 1: notok
						    //		      bit1:device is ready 0: ready 1; not ready
							//		      bit2:security door status 0: opened 1 : closed
							//		      bit3:Escrow status 0: Empty 1 : Not Empty
							//		      bit3:collection box full or not 0: full 1 : not full
							//		      bit4:insertion slot blocked is blocked by particle 0:clear 1:blocked
							//		      bit5:transaportchanel is blocked by particle 0:Clear 1:not blocked
							////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

							Byte0=0b00000100;
							Byte3=0b00000000;
							Byte2=0b00000000;
							Byte1=0b00000000;
							Byte4=0b00000000;
							
						    //++Byte 0: Execution Status of API
							rtArrray[0] = 0x1; //++Operation Failed
							
							//++Byte 1: Note Accepter status
							//++Byte1:0 Note Accepter Communication status
							//++Byte1:1 Note Accepter Readiness
							//++Byte1:2 Security Door status
							//++Byte1:3 Escrow status
							//++Byte1:4 Collection Box status
							//++Byte1:5 Insertion Slot status
							//++Byte1:6 Transport Channel status
							//++Byte1:7 RFU
							rtArrray[1] = 0b00000011;
							rtArrray[1] = rtArrray[1]|doorstatusbyte; //++Bit2
							
							//++Byte 2: Coin Accepter status
                            //++Byte2:0 Coin Accepter And Escrow Communication status
							//++Byte2:1 Coin Accepter Readiness
							//++Byte2:2 Security Door status
							//++Byte2:3 Escrow status
							//++Byte2:4 Collection Box status
							//++Byte2:5 Insertion Slot status
							//++Byte2:6 Transport Channel status
							//++Byte2:7 RFU
							rtArrray[2] = 0b00000000; //++Coin Acceptor Status
                            
                            //++Byte3-7 : Escrowed Notes 
                            rtArrray[3] = 0b00000000; //++INR 5   and 10
                            rtArrray[4] = 0b00000000; //++INR 20  and 50
                            rtArrray[5] = 0b00000000; //++INR 100 and 200
                            rtArrray[6] = 0b00000000; //++INR 500 and 1000
                            rtArrray[7] = 0b00000000; //++INR 2000
                            rtArrray[8] = 0b00000000; //++RFU
                            
                            //++Byte9-11 :Escrowed Coins
                            //++Byte10:0-3 Indicates no of escrowed INR 5 Coins
                            //++Byte10:4-7 Indicates no of escrowed INR 10 Coins
                            rtArrray[9]  = 0b00000000; //++RFU
                            rtArrray[10] = 0b00000000; 
                            rtArrray[11] = 0b00000000; //++RFU
                            
                            memset(log,'\0',LOG_ARRAY_SIZE);
                            sprintf(log,"[deviceStatusV3()] Note Acceeptor Operation Status Bytes =0x%xh.",rtArrray[0]);
							////writeFileLoglog);
							AVRM_Currency_writeFileLog(log,INFO);
							
							memset(log,'\0',LOG_ARRAY_SIZE);
                            sprintf(log,"[deviceStatusV3()] Note Acceeptor Status Bytes =0x%xh.",rtArrray[1]);
							////writeFileLoglog);
							AVRM_Currency_writeFileLog(log,INFO);
						    
						    AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Note Acceptor is not Ready.",INFO);
						    AVRM_Currency_writeFileLog("[deviceStatusV3() Exit] Note Acceptor failed block.",INFO);
						    
						    ////writeFileLog"[deviceStatusV3() Exit] Note Acceptor is not Ready.");
						    ////writeFileLog"[deviceStatusV3() Exit] Note Acceptor failed block.");
						    
						   
	         }//if( CASHCODE_RES_DISABLED != noteAcptrRet    )
         
	     }//if( 1 == fnDeviceType ) end
         
         #endif
         
         
}//deviceStatus() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int deviceStatus(int TransTime) {

         int  Rs50escrowerror=0,
              Rs100escrowerror=0,
              Rs500escrowerror=0;

         char log[LOG_ARRAY_SIZE];

         int noteAcptrRet=0;

	 int coinAcptrRet=0;

	 int DSCURet=0;

         char Byte0=0b00000101,
              Byte1=0x00,
              Byte2=0x00,
              Byte3=0x00;

         int  rtcode=0x00;

	 #if  defined(B2B_NOTE_ACCEPTOR)
         int RS5NoteQuanity=0,
             RS10NoteQuanity=0,
             RS20NoteQuanity=0,
             RS50NoteQuanity=0,
             RS100NoteQuanity=0,
             RS500NoteQuanity=0, 
             RS1000NoteQuanity=0;
         #endif

         #if  defined(JCM_NOTE_ACCEPTOR)
         int RS5NoteQuanity=0,
             RS10NoteQuanity=0,
             RS20NoteQuanity=0,
             RS50NoteQuanity=0,
             RS100NoteQuanity=0,
             RS200NoteQuanity=0,
             RS500NoteQuanity=0, 
             RS1000NoteQuanity=0,
             RS2000NoteQuanity=0;
         #endif

         //writeFileLog"=========================================================================");  
         //writeFileLog"[deviceStatus()] Entry.");



         ///////////////////////////////////////////////////////////////////////////////////////

	     //get note acceptor status
         #if defined(NOTE_ACCEPTOR)
         noteAcptrRet = GetNoteAcptrStatus();
         #endif
         
         ///////////////////////////////////////////////////////////////////////////////////////

         //get coin acceptor status
         #if defined(COIN_ACCEPTOR)
         coinAcptrRet = GetCoinAcptrStatus();
         #endif

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         #if !defined(COIN_ACCEPTOR)
	     coinAcptrRet = SUCCESS;
         #endif

       
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
        
         /*
          
         //Note Acceptor Active and Coin Acceptor Active
         
         #if defined(NOTE_ACCEPTOR) &&  defined(COIN_ACCEPTOR)
         

		         #if defined(B2B_NOTE_ACCEPTOR)
				 if( ( CASHCODE_RES_DISABLED == noteAcptrRet ) && ( SUCCESS == coinAcptrRet ) )
				 #endif
				 #if defined(JCM_NOTE_ACCEPTOR) 
				 if( ( 1 == noteAcptrRet ) && ( SUCCESS == coinAcptrRet ) )
				 #endif
				 #if defined(BNA_NOTE_ACCEPTOR) 
				 if( ( 1 == noteAcptrRet ) && ( SUCCESS == coinAcptrRet ) )
				 #endif
				 {

						//writeFileLog"[deviceStatus() Exit] Note acptr and coin acptr is ready.");
						//writeFileLog"=========================================================================");  
						return(1); // ret code as per CRIS

				 }
				 
				 

                 #if  defined(B2B_NOTE_ACCEPTOR)
				 if( ( CASHCODE_RES_DISABLED != noteAcptrRet   )  && (SUCCESS == coinAcptrRet) )
			     #endif
			     #if  defined(JCM_NOTE_ACCEPTOR) 
				 if( ( 1 != noteAcptrRet  )  && (SUCCESS == coinAcptrRet) )
				 #endif
				 #if  defined(BNA_NOTE_ACCEPTOR) 
				 if( ( 1 != noteAcptrRet  )  && (SUCCESS == coinAcptrRet) )
				 #endif
				 {
					   
							 //writeFileLog"[deviceStatus() Exit] Note acceptor fault.");
							 //writeFileLog"=========================================================================");  
							 return(2); // ret code as per CRIS

				 }

				 

				 #if defined(B2B_NOTE_ACCEPTOR)
				 if( ( CASHCODE_RES_DISABLED == noteAcptrRet )  && ( FAIL == coinAcptrRet ))
				 #endif
				 #if defined(JCM_NOTE_ACCEPTOR) 
				 if( ( 1 == noteAcptrRet )  && (FAIL == coinAcptrRet))
				 #endif
				 #if defined(BNA_NOTE_ACCEPTOR) 
				 if( ( 1 == noteAcptrRet )  && (FAIL == coinAcptrRet))
				 #endif
				 {
					 //writeFileLog"[deviceStatus() Exit] Coin acceptor fault.");
					 //writeFileLog"=========================================================================");  
					 return(4); // ret code as per CRIS

				 }

				 
				 #if  defined(B2B_NOTE_ACCEPTOR)
				 if( ( CASHCODE_RES_DISABLED != noteAcptrRet) && (FAIL == coinAcptrRet))
				 #endif
			     #if defined(JCM_NOTE_ACCEPTOR) 
				 if( ( 1!=noteAcptrRet) && ( FAIL == coinAcptrRet))
				 #endif
				 #if defined(BNA_NOTE_ACCEPTOR) 
				 if( ( 1!=noteAcptrRet) && ( FAIL == coinAcptrRet))
				 #endif
				 {
							//writeFileLog"[deviceStatus() Exit] Note acceptor and Coin acceptor fault.");
							//writeFileLog"=========================================================================");  
					        return(8); // ret code as per CRIS

				 }

         #endif
          
          */
          
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         //++Note Acceptor Active Coin Acceptor InActive

         //++#if defined(NOTE_ACCEPTOR) && !defined(COIN_ACCEPTOR)
         
         
         #if defined(NOTE_ACCEPTOR) 

         #if defined(B2B_NOTE_ACCEPTOR)
	     if( ( CASHCODE_RES_DISABLED ==  noteAcptrRet  ) ||( CASHCODE_RES_HOLDING  ==  noteAcptrRet  ) || ( CASHCODE_RES_REJECTING == noteAcptrRet  ) ) 
         #endif
         #if defined(JCM_NOTE_ACCEPTOR) 
	     if( 1 == noteAcptrRet )
         #endif
         #if defined(BNA_NOTE_ACCEPTOR) 
	     if(  1 == noteAcptrRet )
         #endif
	     {
	       
                
                #if defined(B2B_NOTE_ACCEPTOR)

                RS5NoteQuanity=-1,
				RS10NoteQuanity=-1,
				RS20NoteQuanity=-1,
				RS50NoteQuanity=-1,
				RS100NoteQuanity=-1,
				RS500NoteQuanity=-1, 
				RS1000NoteQuanity=-1;
                
                unsigned char PollReplyPacket[30];
                
                int PollBufferLength=30;
                
                int PollReplyPacketlength=0;

                rtcode=GetCurrentB2BState( PollReplyPacket,
                                           PollBufferLength,
                                           &PollReplyPacketlength);

				//if poll success
				if(1==rtcode)
				{
					  if( CASHCODE_RES_GENERIC_FAIL != PollReplyPacket[3] )
					  {
							  if(SUCCESS ==  B2BGetEscrowNotesNumbers( &RS5NoteQuanity,
											   &RS10NoteQuanity,
											   &RS20NoteQuanity,
											   &RS50NoteQuanity ,
											   &RS100NoteQuanity ,
											   &RS500NoteQuanity, 
											   &RS1000NoteQuanity,
											   2
																))
							 {

								  //writeFileLog"[deviceStatus() Exit] B2B Extended cassette status get successfully.");

							 }// if(SUCCESS ==  B2BGetEscrowNotesNumbers)

							 }//if( CASHCODE_RES_GENERIC_FAIL != PollReplyPacket[3] )
							
				}//if(1==rtcode)
						
                #endif
                
                /*
                #if defined(B2B_NOTE_ACCEPTOR)
                
                RS5NoteQuanity=-1,
	            RS10NoteQuanity=-1,
	            RS20NoteQuanity=-1,
	            RS50NoteQuanity=-1,
		        RS100NoteQuanity=-1,
		        RS500NoteQuanity=-1, 
	            RS1000NoteQuanity=-1;
	            
                if(SUCCESS ==  B2BGetEscrowNotesNumbers(  &RS5NoteQuanity,
														   &RS10NoteQuanity,
														   &RS20NoteQuanity,
														   &RS50NoteQuanity ,
														   &RS100NoteQuanity ,
														   &RS500NoteQuanity, 
														   &RS1000NoteQuanity,
														   2 ))
				{

					      //writeFileLog"[deviceStatus() Exit] B2B Extended cassette status get successfully.");

				 }// if(SUCCESS ==  B2BGetEscrowNotesNumbers)

                 
                 #endif
                 */
                 
                 #if defined(JCM_NOTE_ACCEPTOR)
                 
                 
                 if(SUCCESS == JCMGetEscrowNotesNumbers(   &RS5NoteQuanity,
														   &RS10NoteQuanity,
														   &RS20NoteQuanity,
														   &RS50NoteQuanity ,
														   &RS100NoteQuanity ,
														   &RS200NoteQuanity ,
														   &RS500NoteQuanity, 
														   &RS1000NoteQuanity,
														   &RS2000NoteQuanity,
														   2
													   ))
				 {

					 //writeFileLog"[deviceStatus() Exit] JCM Escrow Notes get successfully.");

				 }// if(SUCCESS ==  JCMGetEscrowNotesNumbers) end

                 #endif
                
                //////////////////////////////////////////////////////////////////////////
                  Byte0=0b00000111;
                //////////////////////////////////////////////////////////////////////////

                /*
                   Byte 0: bit0:Serial Communication 0: Not ok 1:ok
                           bit1:device is ready 0: not ready 1; ready
                           bit2:security door status 0: opened 1 : closed
                           bit3:collection box full or not 0: not full 1 :full
                           bit4:insertion slot blocked is blocked by particle 0:not blocked 1:blocked
                           bit5:transaportchanel is blocked by particle 0:not blocked 1:blocked

                */

                #if defined(NOTE_ACCEPTOR) && defined(B2B_NOTE_ACCEPTOR)
                //Insertion slot is blocked 1:blocked
                if(( CASHCODE_RES_HOLDING  ==  noteAcptrRet  ) ||
                   ( CASHCODE_RES_REJECTING == noteAcptrRet  )
                )
                {
                       Byte0=Byte0 | 0b00001000;

                }
                #endif

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                //++Error Checking rupees more than 15

                if(  RS10NoteQuanity >= 15 ) {
                      //writeFileLog"[deviceStatus() Exit] Rs 10 Escorw Number exceed than 15.");
                      RS10NoteQuanity = 15;
                      
                }

                if(  RS20NoteQuanity >= 15 ) {
                      //writeFileLog"[deviceStatus() Exit] Rs 20 Escorw Number exceed than 15.");
                      RS20NoteQuanity = 15;
                }

                if(  RS50NoteQuanity >= 15 ) {
                      //writeFileLog"[deviceStatus() Exit] Rs 50 Escorw Number exceed than 15.");
                      RS50NoteQuanity = 15;
                      Rs50escrowerror= 1;
                }

                if(  RS100NoteQuanity >= 15 ) {
                      //writeFileLog"[deviceStatus() Exit] Rs 100 Escorw Number exceed than 15.");
                      RS100NoteQuanity = 15;
                      Rs100escrowerror=1;
                }
 
                if(  RS500NoteQuanity >= 15 ) {
                      //writeFileLog"[deviceStatus() Exit] Rs 500 Escorw Number exceed than 15.");
                      RS500NoteQuanity = 15;
                      Rs500escrowerror=1;
                }
               

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                if( ( ( RS10NoteQuanity>=0 ) || ( RS10NoteQuanity <=100) )  && 
                    ( ( RS20NoteQuanity>=0 ) || ( RS20NoteQuanity <=100) ) 
                  )
                {
                        Byte1= (RS20NoteQuanity<<4)+RS10NoteQuanity;

                }

                if( ( ( RS50NoteQuanity>=0 )  || ( RS50NoteQuanity <=100)  )  && 
                    ( ( RS100NoteQuanity>=0 ) || ( RS100NoteQuanity <=100) ) 
                  )
                {
                        Byte2= (RS100NoteQuanity<<4)+RS50NoteQuanity;

                }

                if( ( ( RS500NoteQuanity>=0 )  || ( RS500NoteQuanity <=100)  )  && 
                    ( ( RS1000NoteQuanity>=0 ) || ( RS1000NoteQuanity <=100) ) 
                  )
                {
                        Byte3= (RS1000NoteQuanity<<4)+RS500NoteQuanity;

                }
                
                //Set Escrow bit 
                if( 1 == Rs50escrowerror ){
                      //writeFileLog"[deviceStatus() Exit] Rs 50 Escorw error bit set.");
                      Byte0=Byte0+0b01000000; 
                } else if( 1 == Rs100escrowerror ){
                      //writeFileLog"[deviceStatus() Exit] Rs 100 Escorw error bit set.");
                      Byte0=Byte0+0b10000000; 
                }else if( 1 == Rs500escrowerror ){
                      //writeFileLog"[deviceStatus() Exit] Rs 500 Escorw error bit set.");
                      Byte0=Byte0+0b11000000; 
                }
                rtcode = (Byte3<<24)+(Byte2<<16)+(Byte1<<8)+Byte0;

                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[deviceStatus()] Return Code =0x%xh.",rtcode);
                                    
                //writeFileLoglog);

                //writeFileLog"[deviceStatus() Exit] Note Acceptor is  ready.");
                //writeFileLog"=========================================================================");  

                return(rtcode); 


         }//if( ( CASHCODE_RES_DISABLED == noteAcptrRet ) ) 

         #if  defined(B2B_NOTE_ACCEPTOR)
		 if( CASHCODE_RES_DISABLED != noteAcptrRet    )
			 #endif
			 #if  defined(JCM_NOTE_ACCEPTOR) 
		 if(  1 != noteAcptrRet   )
			 #endif
			 #if  defined(BNA_NOTE_ACCEPTOR) 
		 if( 1 != noteAcptrRet  )
			 #endif
		 {
		   
                 //writeFileLog"[deviceStatus() Exit] Note acceptor fault.");
                 //writeFileLog"=========================================================================");  

                 /*
                   Byte 0: 
                           bit0:Serial Communication 0: Not ok 1:ok

                           bit1:device is ready 0: not ready 1; ready

                           bit2:security door status 0: opened 1 : closed

                           bit3:collection box full or not 0: not full 1 :full

                           bit4:insertion slot blocked is blocked by particle 
                                0:not blocked 1:blocked

                           bit5:transaportchanel is blocked by particle 0:not blocked 1:blocked

                           bit6 and bit7 Escrow Exceeds 

                */

                 Byte0=0b00000100;
                 Byte3=0b00000000;
                 Byte2=0b00000000;
                 Byte1=0b00000000;
                 rtcode = (Byte3<<24)+(Byte2<<16)+(Byte1<<8)+Byte0;
                 return(rtcode); 

                 ///////////////////////////////////////////////////////////////////////////////////////////
                 
	 }//if( CASHCODE_RES_DISABLED != noteAcptrRet    )
         
         #endif

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         /*

         //Note Acceptor InActive Coin Acceptor InActive

         #if !defined(NOTE_ACCEPTOR) &&  defined(COIN_ACCEPTOR)

			 if( SUCCESS == coinAcptrRet )
			 {
					//writeFileLog"[deviceStatus() Exit] Coin acceptor is ready.");
					//writeFileLog"=========================================================================");  
					return(1); 

			 }

			 else if(  FAIL == coinAcptrRet )
			 {
			     //writeFileLog"[deviceStatus() Exit]  Coin acceptor fault.");
				 //writeFileLog"=========================================================================");  
			     return(4); 

	         }


         #endif
 
         ////////////////////////////////////////////////////////////////////////////////////////////
         */


}//deviceStatus() end here

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int defaultCommit( int timeout) {
      return 0;
}//int defaultCommit( int timeout) end

//++AVRM API
int defaultCommit2( int timeout,int fnDeviceType ) {
	
         //0 : All Device 1: NoteAcceptor 2: CoinEscrow
         AVRM_Currency_writeFileLog("[defaultCommit2()] Entry.",TRACE);
         if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) {
			AVRM_Currency_writeFileLog("[defaultCommit2()] DeviceType Coin Acceptor.",INFO);
		 }//if end
		 if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) {
			AVRM_Currency_writeFileLog("[defaultCommit2()] DeviceType Note Acceptor.",INFO);
		 }//if end
         int value=0, noteStatus=0;
		 int retry=1,try=1;
		 int ret=0, ret1=0, ret2=0,ret3=0; 
		 int noteQtyInRcyclCst=0;
         ret1 = FAIL; 
		 ret2 = FAIL; 
		 ret  = FAIL;
         unsigned char log[100];
         memset(log,'\0',100); 
        
         //++Disable all acceptance for money accept machine

         if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) {

				 #ifdef COIN_ACCEPTOR
				 //Now Disable all note and coin acceptor
				 delay_mSec(200);

				 AVRM_Currency_writeFileLog("[defaultCommit2()] Before Disable note and coin section.",INFO);

				 if( SUCCESS == DisableAllCoins() ){
					  AVRM_Currency_writeFileLog("[defaultCommit2()] Disable coin acceptor success.",INFO);
				 }else{
					  AVRM_Currency_writeFileLog("[defaultCommit2()] Disable coin acceptor failed.",INFO);
				 }//else end
                 #endif
                 
         }//if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) )  end

         if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) {
				 
					 #ifdef B2B_NOTE_ACCEPTOR
					 if(SUCCESS == IssuedisableAllBillCmd()){
						  AVRM_Currency_writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Disable note acceptor success.",INFO);
					 }else{
						  AVRM_Currency_writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Disable note acceptor failed.",INFO);
					 }//else end
					 #endif
					
					 #ifdef JCM_NOTE_ACCEPTOR
					 if(SUCCESS == IssuedisableAllBillCmd()){
						  AVRM_Currency_writeFileLog("[defaultCommit2() JCM_NOTE_ACCEPTOR] Disable note acceptor success.",INFO);
					 }else{
						  AVRM_Currency_writeFileLog("[defaultCommit2() JCM_NOTE_ACCEPTOR] Disable note acceptor failed.",INFO);
					 }//else end
					 #endif
					 
         }//if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) end
        
         if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) {

			 //++Coin Escrow Accept Operation
			 #ifdef COIN_ESCROW

			 //++Accept Coin
			 AVRM_Currency_writeFileLog("[defaultCommit2()] Going to accept coin...",INFO);

			 for(retry = 1; retry<=1;retry++) { 

					 ret2 = AcceptFromCoinEscrow();

					 if(0==ret2){
						    AVRM_Currency_writeFileLog("[defaultCommit2()] Accept coin successfully.",INFO);
						    break;
					 }else{
						    AVRM_Currency_writeFileLog("[defaultCommit2()] Accept coin failed.",INFO);
						    memset(log,'\0',100); 
						    sprintf(log,"[defaultCommit2()] Accept Coin Retry : %d.",retry);
						    AVRM_Currency_writeFileLog(log,INFO);
						    continue;
					 }//else end
					 
			 }//for end

			 #endif
             #if !defined(COIN_ESCROW)
		     ret1=SUCCESS;
		     #endif

        }//if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) )  end

         if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) {
			       
                    //Note Acceptor Unload Operation
			        #ifdef  NOTE_ACCEPTOR
						 
						  //++Step 1: Get current notes number from escrow
						  noteQtyInRcyclCst = 0;

						  if( 1 == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) ) {
							     memset(log,'\0',100); 
							     sprintf(log,"[defaultCommit()] Note present in recycle cassette: %d.",noteQtyInRcyclCst);
							     AVRM_Currency_writeFileLog(log,INFO);
						  }else{

								AVRM_Currency_writeFileLog("[defaultCommit()] Read recycling cassette status read failed.",INFO);
								return(FAIL);
						  }  //else end
						  
						  //++Step 2: Accept current notes from escrow to vault
						  if( noteQtyInRcyclCst > 0 ) {

								  AVRM_Currency_writeFileLog("[defaultCommit2()] Going to accept notes from note acceptor device.",INFO);
								  
								  
								  #ifdef B2B_NOTE_ACCEPTOR
								  
								  if(noteQtyInRcyclCst > 20) //max 20 note can be acprd at a time.
								  { 
										noteQtyInRcyclCst = 20;
								  }
								  AVRM_Currency_writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Before AcceptFromEscrow().",INFO);								 
								  AVRM_Currency_writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Before Unload Operation Calling.",INFO);
								  int b2brtcode=-1;
								  for(retry = 1; retry<=2;retry++) {
										 b2brtcode=AcceptFromEscrow(1,noteQtyInRcyclCst);
										 if(1==b2brtcode){

												AVRM_Currency_writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Unload operation successfully executed.",INFO);
												ret3=-1;
												ret3 = GetAcceptedNoteDetail_V2(&value,&noteStatus,timeout); 
												if(ret3 == 1){
													   writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Accept notes successfully.");
												}else{	 
													   writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Accept notes failed.");
												}
												break;
										 }else{

												AVRM_Currency_writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] Accept note failed.",INFO);
												memset(log,'\0',100); 
												sprintf(log,"[defaultCommit2() B2B_NOTE_ACCEPTOR]  Accept note Retry : %d.",retry);
												AVRM_Currency_writeFileLog(log,INFO);
												ret3 = FAIL;
												delay_mSec(100);
												continue;
										 }//else end

								  }//for end

								  AVRM_Currency_writeFileLog("[defaultCommit2() B2B_NOTE_ACCEPTOR] After Unload Operation Completed.",INFO);

								  #endif

								  #ifdef JCM_NOTE_ACCEPTOR
								 
								  AVRM_Currency_writeFileLog("[defaultCommit2() JCM_NOTE_ACCEPTOR] Before Unload Operation Calling.",INFO);

								  int jcmrtcode=-1;

								  for(retry = 1; retry<=2;retry++) {

										 jcmrtcode=JCM_DefaultCommit();

										 if(1==jcmrtcode){

												AVRM_Currency_writeFileLog("[defaultCommit2() JCM_NOTE_ACCEPTOR] Unload operation successfully executed.",INFO);
												ret3 = SUCCESS;
												break;
										 }else{

												AVRM_Currency_writeFileLog("[defaultCommit2() JCM_NOTE_ACCEPTOR] Accept note failed.",INFO);
												memset(log,'\0',100); 
												sprintf(log,"[defaultCommit2() JCM_NOTE_ACCEPTOR]  Accept note Retry : %d.",retry);
												AVRM_Currency_writeFileLog(log,INFO);
												ret3 = FAIL;
												delay_mSec(100);
												continue;
										 }//else end

								  }//for end

								  AVRM_Currency_writeFileLog("[defaultCommit2() JCM_NOTE_ACCEPTOR] After Unload Operation Completed.",INFO);

								  #endif

								  AVRM_Currency_writeFileLog("[defaultCommit2()] after accept notes.",INFO);
								  //++Now again read recycle quantity
								  int CurrentCastQuanity=0;
								  GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
								  memset(log,'\0',100);
								  sprintf(log,"[defaultCommit2()] After Unload Recycle Quantity = %d.",CurrentCastQuanity);
								  AVRM_Currency_writeFileLog(log,INFO);
								  
								  if( 0 == CurrentCastQuanity ){
									  AVRM_Currency_writeFileLog("[defaultCommit2()] note acceptor escrow clearance success ",INFO);
									  return true;
								  }else{
									  AVRM_Currency_writeFileLog("[defaultCommit2()] note acceptor escrow clearance failed ",INFO);
									  return false;
								  }
					      }else{
							  AVRM_Currency_writeFileLog("[defaultCommit2()] No Note present in note acceptor escrow",INFO);
							  return true;
						  }//else end
			  #endif

			  #if !defined(NOTE_ACCEPTOR)
			  //AVRM_Currency_writeFileLog("[defaultCommit()] No Note Acceptor device is present in ATVM system.",INFO);
			  ret3 = SUCCESS ;
			  #endif
          
          }// if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) )  end
  
  /*
         if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) {

			  if( noteQtyInRcyclCst > 0 ){
				  
				  if(( SUCCESS == ret1  ) && ( SUCCESS == ret3  )){
					 AVRM_Currency_writeFileLog("[defaultCommit2() Exit] Accept notes finished with success.",INFO);
					 return 0; //AVRM New API SUCCESS
				  }else{
					 AVRM_Currency_writeFileLog("[defaultCommit2() Exit] Accept notes finished with failed.",INFO);
					 return 31; //Other Error
				  }//else end
				  
			 }else{
				   AVRM_Currency_writeFileLog("[defaultCommit2() Exit] No action takes dues No Note present in note acceptor escrow .",INFO);
				   return 31; //Other Error
			 }//else end

        }//if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) end

         if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) {
			
			  if( SUCCESS == ret2  ) {
			     AVRM_Currency_writeFileLog("[defaultCommit2() Exit] Accept coin finished with success.",INFO);
			     return 0; //AVRM New API SUCCESS
			  }else{
			     AVRM_Currency_writeFileLog("[defaultCommit2() Exit] Accept coin finished with failed.",INFO);
			     return 31; //AVRM New API Other Error
			  }//else end
		 
          }// if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) )  end
*/

}//int defaultCommit2( int timeout,int fnDeviceType ) end here

//++AVRM API
int defaultCancel2(int timeout,int fnDeviceType) {

                  
	     AVRM_Currency_writeFileLog("[defaultCancel2()] Entry.",TRACE);

		 int value=0, noteStatus=0;
		 int retry=1;
		 int ret1=0, ret2=0, ret=0,ret3=0;
		 int noteQtyInRcyclCst=0;
		 ret1 = FAIL; 
		 ret2 = FAIL; 
		 ret  = FAIL;

		 unsigned char log[100];
		 memset(log,'\0',100); 

         if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) {
			 
				 #ifdef COIN_ACCEPTOR

				 //Now Disable all note and coin acceptor
				 delay_mSec(200);
				 AVRM_Currency_writeFileLog("[defaultCancel2()] Before Disable note and coin section.",INFO);
				 if(SUCCESS == DisableAllCoins() ){
					  AVRM_Currency_writeFileLog("[defaultCancel2()] Disable coin acceptor success.",INFO);
				 }else{
					  AVRM_Currency_writeFileLog("[defaultCancel2()] Disable coin acceptor failed.",INFO);
				 }//else end

				 #endif
				 
		 }//if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) )
 
         if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) {
			 
					 #ifdef NOTE_ACCEPTOR

						 #ifdef B2B_NOTE_ACCEPTOR

						 if(SUCCESS == IssuedisableAllBillCmd())
						 {
							  AVRM_Currency_writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Disable note acceptor success.",INFO);
						 }
						 else
						 {
							  AVRM_Currency_writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Disable note acceptor failed.",INFO);
						 }

						 #endif
                         
                         #ifdef JCM_NOTE_ACCEPTOR
						 if(SUCCESS == IssuedisableAllBillCmd()){
							  AVRM_Currency_writeFileLog("[defaultCancel2() JCM_NOTE_ACCEPTOR] Disable note acceptor success.",INFO);
						 }else{
							  AVRM_Currency_writeFileLog("[defaultCancel2() JCM_NOTE_ACCEPTOR] Disable note acceptor failed.",INFO);
						 }//else end
						 #endif
									
					 #endif
					 
		 }//if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) end
		 
		 AVRM_Currency_writeFileLog("[defaultCancel2()] After Disable note and coin section.",INFO);
	 
	     if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) {
			  
				 #ifdef COIN_ESCROW
				 //++Reject Coin
				 AVRM_Currency_writeFileLog("[defaultCancel2()] Going to reject coin.",INFO);
				 AVRM_Currency_writeFileLog("[defaultCancel2()] Before RejectFromCoinEscrow().",INFO);
				 for(retry = 1; retry<=2;retry++) {

						 ret1 = RejectFromCoinEscrow(); 
						 if(SUCCESS==ret1){
							   AVRM_Currency_writeFileLog("[defaultCancel2()] Successfully reject coin.",INFO);
							   break;
						 }else{
							   AVRM_Currency_writeFileLog("[defaultCancel2()] failed to reject coin.",INFO);
							   memset(log,'\0',100); 
							   sprintf(log,"[defaultCancel2()] Reject Coin retry: %d.",retry);
							   AVRM_Currency_writeFileLog(log,INFO);
							   continue;
						 }//else end

				 }//for end
				 AVRM_Currency_writeFileLog("[defaultCancel2()] After RejectFromCoinEscrow().",INFO);
				 #endif

                 #if !defined(COIN_ESCROW)   
                 ret1 =SUCCESS;
                 #endif

         }//if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) end
         
         if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) {
			 
							 #ifdef NOTE_ACCEPTOR
								 noteQtyInRcyclCst = 0;
								 if( SUCCESS == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) ) {
									  memset(log,'\0',100); 
									  sprintf(log,"[defaultCancel2()] Note present in recycle cassette: %d.",noteQtyInRcyclCst);
									  AVRM_Currency_writeFileLog(log,INFO);
								 }else{
									  AVRM_Currency_writeFileLog("[defaultCancel2()] note acceptor recycling cassette status read failed so return from here.",INFO);
									  return(FAIL);
								 }//else end
								 if( noteQtyInRcyclCst  > 0 ) {
								 
										  AVRM_Currency_writeFileLog("[defaultCancel2()] Going to Dispense notes.",INFO);
										  
										  #ifdef B2B_NOTE_ACCEPTOR
										  AVRM_Currency_writeFileLog("[defaultCancel2() B2B_NOTE_ACCEPTOR] Before Dispense Operation Calling.",INFO);
										  int b2brtcode=-1;
										  for(retry = 1; retry<=2;retry++) {
												 b2brtcode= RejectFromEscrow(24,noteQtyInRcyclCst);
												 if(1==b2brtcode) {
													  AVRM_Currency_writeFileLog("[defaultCancel2() B2B_NOTE_ACCEPTOR] Dispense operation successfully executed.",INFO);
													  ret3=-1;
													  ret3 = GetAcceptedNoteDetail_V2(&value,&noteStatus,timeout); 
													  if(ret3 == 1){
														   writeFileLog("[defaultCancel2() B2B_NOTE_ACCEPTOR] Accept notes successfully.");
													  }else{	 
														   writeFileLog("[defaultCancel2() B2B_NOTE_ACCEPTOR] Accept notes failed.");
													  }
													  break;
												 }else{
													  AVRM_Currency_writeFileLog("[defaultCancel2() B2B_NOTE_ACCEPTOR] Reject note failed.",INFO);
													  memset(log,'\0',100); 
													  sprintf(log,"[defaultCancel() B2B_NOTE_ACCEPTOR]  Reject note Retry : %d.",retry);
													  AVRM_Currency_writeFileLog(log,INFO);
													  ret3 = FAIL;
													  delay_mSec(100);
													  continue;
												 }//else end

										   }//else end
										   AVRM_Currency_writeFileLog("[defaultCancel2() B2B_NOTE_ACCEPTOR] After Dispense Operation Completed.",INFO);
										  #endif
										  
										  #ifdef JCM_NOTE_ACCEPTOR
										  AVRM_Currency_writeFileLog("[defaultCancel2() JCM_NOTE_ACCEPTOR] Before Dispense Operation Calling.",INFO);
										  int jcmrtcode=-1;
										  for(retry = 1; retry<=2;retry++) {
												 jcmrtcode= JCM_DefaultCancel();
												 if(1==jcmrtcode) {
													  AVRM_Currency_writeFileLog("[defaultCancel() JCM_NOTE_ACCEPTOR] Dispense operation successfully executed.",INFO);
													  ret3 = SUCCESS;
													  break;
												 }else{
													  AVRM_Currency_writeFileLog("[defaultCancel() JCM_NOTE_ACCEPTOR] Reject note failed.",INFO);
													  memset(log,'\0',100); 
													  sprintf(log,"[defaultCancel() JCM_NOTE_ACCEPTOR]  Reject note Retry : %d.",retry);
													  AVRM_Currency_writeFileLog(log,INFO);
													  ret3 = FAIL;
													  delay_mSec(100);
													  continue;
												 }//else end

										   }//else end
										   AVRM_Currency_writeFileLog("[defaultCancel2() JCM_NOTE_ACCEPTOR] After Dispense Operation Completed.",INFO);
										   #endif
										   
										   AVRM_Currency_writeFileLog("[defaultCancel2()] after Dispense notes.",INFO);
										   int CurrentCastQuanity=0;
										   GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
										   memset(log,'\0',100);
										   sprintf(log,"[defaultCancel2()] After Dispense Recycle Quantity = %d.",CurrentCastQuanity);
										   AVRM_Currency_writeFileLog(log,INFO);

							  }else{
								   AVRM_Currency_writeFileLog("[defaultCancel2()] No Note present in recycle cassette.",INFO);
							  }//else end
							 
						     #endif

							 #if !defined(NOTE_ACCEPTOR)   
							 ret3 =SUCCESS;
							 #endif
				
	     }//if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) end

         if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) {

			  if( noteQtyInRcyclCst > 0 ){
				  
				  if(( SUCCESS == ret1  ) && ( SUCCESS == ret3  )){
					 AVRM_Currency_writeFileLog("[defaultCancel2() Exit] Accept notes finished with success.",INFO);
					 return 0; //AVRM New API SUCCESS
				  }else{
					 AVRM_Currency_writeFileLog("[defaultCancel2() Exit] Accept notes finished with failed.",INFO);
					 return 31; //Other Error
				  }//else end
				  
			 }else{
				   AVRM_Currency_writeFileLog("[defaultCancel2() Exit] No action takes dues No Note present in note acceptor escrow .",INFO);
				   return 31; //Other Error
			 }//else end

         }//if( ( ALL == fnDeviceType ) || ( BNA == fnDeviceType ) ) end

         if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) ) {
			
			  if( SUCCESS == ret1  ) {
			     AVRM_Currency_writeFileLog("[defaultCancel2() Exit] Accept coin finished with success.",INFO);
			     return 0; //AVRM New API SUCCESS
			  }else{
			     AVRM_Currency_writeFileLog("[defaultCancel2() Exit] Accept coin finished with failed.",INFO);
			     return 31; //AVRM New API Other Error
			  }//else end
		 
          }// if( ( ALL == fnDeviceType ) || ( BCE == fnDeviceType ) )  end

	   
}//++int defaultCancel2(int timeout,int fnDeviceType) end here

int defaultCancel(int timeout) {

                  
	     //writeFileLog"[defaultCancel()] Entry.");

		 int value=0, noteStatus=0;
		 int retry=1;
		 int ret1=0, ret2=0, ret=0,ret3=0;
		 int noteQtyInRcyclCst=0;
		 ret1 = FAIL; 
		 ret2 = FAIL; 
		 ret  = FAIL;

		 unsigned char log[100];

		 memset(log,'\0',100); 

		 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		 
                 #ifdef COIN_ACCEPTOR

		 //Now Disable all note and coin acceptor

		 delay_mSec(200);

		 //writeFileLog"[defaultCancel()] Before Disable note and coin section.");

		 if(SUCCESS == DisableAllCoins() )
		 {
		      //writeFileLog"[defaultCancel()] Disable coin acceptor success.");
		 }
		 else
		 {
		      //writeFileLog"[defaultCancel()] Disable coin acceptor failed.");
		 }

		 #endif
 
                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		 #ifdef NOTE_ACCEPTOR

			 #ifdef B2B_NOTE_ACCEPTOR

			 if(SUCCESS == IssuedisableAllBillCmd())
			 {
			      //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Disable note acceptor success.");
			 }
			 else
			 {
			      //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Disable note acceptor failed.");
			 }

			 #endif

                        
		 #endif

		 //writeFileLog"[defaultCancel()] After Disable note and coin section.");
	 
		 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 

		 #ifdef COIN_ESCROW

		 //Reject Coin
		 //writeFileLog"[defaultCancel()] Going to reject coin.");

		 //writeFileLog"[defaultCancel()] Before RejectFromCoinEscrow().");

		 for(retry = 1; retry<=2;retry++) 
		 {

			 ret1 = RejectFromCoinEscrow(); 

			 if(SUCCESS==ret1)
		         {
			       //writeFileLog"[defaultCancel()] Successfully reject coin.");
		               break;
		         }
			 else
		         {
			       //writeFileLog"[defaultCancel()] failed to reject coin.");
		               memset(log,'\0',100); 
		               sprintf(log,"[defaultCancel()] Reject Coin retry: %d.",retry);
		               //writeFileLoglog);
		               continue;
		         }

		 }

		 //writeFileLog"[defaultCancel()] After RejectFromCoinEscrow().");

		 #endif

                 
	 
		 #if !defined(COIN_ESCROW)   
	   
		 ret1 =SUCCESS;

		 #endif

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		 #ifdef NOTE_ACCEPTOR

                         noteQtyInRcyclCst = 0;

			 if( SUCCESS == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) )
			 {
				   memset(log,'\0',100); 
				   sprintf(log,"[defaultCancel()] Note present in recycle cassette: %d.",noteQtyInRcyclCst);
				   //writeFileLoglog);

			 }
			 else
			 {
				  //writeFileLog"[defaultCancel()] note acceptor recycling cassette status read failed so return from here.");
				  return(FAIL);
			 }  

                         
                 

                     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                     if( noteQtyInRcyclCst  > 0 )
                     {

                                  //writeFileLog"[defaultCancel()] Going to Dispense notes.");

                                  
		                  #ifdef JCM_NOTE_ACCEPTOR
			 
				  //writeFileLog"[defaultCancel() JCM_NOTE_ACCEPTOR] Before Dispense Operation Calling.");

				  int jcmrtcode=-1;

				  for(retry = 1; retry<=2;retry++) 
				  {

				         jcmrtcode= JCM_DefaultCancel();

				         if(1==jcmrtcode)
					 {

				              //writeFileLog"[defaultCancel() JCM_NOTE_ACCEPTOR] Dispense operation successfully executed.");

				              ret3 = SUCCESS;

				              break;

				         }   
				         else
				         {

				                //writeFileLog"[defaultCancel() JCM_NOTE_ACCEPTOR] Reject note failed.");

						memset(log,'\0',100); 

						sprintf(log,"[defaultCancel() JCM_NOTE_ACCEPTOR]  Reject note Retry : %d.",retry);

						//writeFileLoglog);

				                ret3 = FAIL;

				                delay_mSec(100);

						continue;


				         }


				  }

		                  //writeFileLog"[defaultCancel() JCM_NOTE_ACCEPTOR] After Dispense Operation Completed.");

		                  #endif

                                  
		                 ////////////////////////////////////////////////////////////////////////////////////


		                 #ifdef B2B_NOTE_ACCEPTOR

				 if(noteQtyInRcyclCst > 20) //max 20 note can be dispensed at a time.
				 { 
				      noteQtyInRcyclCst = 20;
				 }
				 
		                 //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Before RejectFromEscrow().");

				 for(retry = 1; retry<=1;retry++) 
				 {

					   //issue dispense command
					   
					   ret2 = RejectFromEscrow(24,noteQtyInRcyclCst);

					   if(SUCCESS == ret2)
					   {   
						   //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Dispence command issue success.");

						   //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Before GetReturnDetail().");

						   ret3 = GetReturnDetail_V2(&value,&noteStatus,timeout);  

						   if(SUCCESS == ret3) 
		                                   {
							//writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Reject notes success."); 
		                                   }   
						   else
		                                   {     
							//writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Reject notes failed.");
		                                   }

						   //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] After GetReturnDetail().");

						   break;
						    
					   } 
					   else
					   {
						 //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] Dispence command issue failed.");

						 memset(log,'\0',100); 

						 sprintf(log,"[defaultCancel() B2B_NOTE_ACCEPTOR] Note Dispense Retry: %d.",retry);

						 //writeFileLoglog);

						 continue;
					   } 
					   
				
				  } 

				  //writeFileLog"[defaultCancel() B2B_NOTE_ACCEPTOR] After RejectFromEscrow().");

				  #endif

		                 ////////////////////////////////////////////////////////////////////////////////////////


		                  //writeFileLog"[defaultCancel()] after Dispense notes.");
		                   
		                  int CurrentCastQuanity=0;

		                  GetNoteDetailsInRecycleCst( &CurrentCastQuanity );

		                  memset(log,'\0',100);

		                  sprintf(log,"[defaultCancel()] After Dispense Recycle Quantity = %d.",CurrentCastQuanity);

		                  //writeFileLoglog);
	 

			 
                 }//if( noteQtyInRcyclCst  > 0 )
                 else
                 {
                         //writeFileLog"[defaultCancel()] No Note present in recycle cassette.");
                 }
                 #endif

		 #if !defined(NOTE_ACCEPTOR)   
	   
		 ret3 =SUCCESS;

		 #endif

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
                 if( noteQtyInRcyclCst  > 0 )
                 {
			  if(( SUCCESS == ret1 ) && ( SUCCESS == ret3  ))
			  {
				   //writeFileLog"[defaultCancel() Exit] Reject notes finished with success.");

				   return 1;
			  }
			  else
			  {

				  //writeFileLog"[defaultCancel() Exit] Reject notes finished with failed.");

				  return 2;

			  }

                 }
                 else
                 {
                     //writeFileLog"[defaultCancel() Exit] No action takes dues No Note present in note acceptor escrow .");
                     return 2;
                 }

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

		  
	

}//defaultCancel() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOTE_ACCEPTOR 

//++Start Note Acceptor Transaction Complete Thread if success return 1 or fail 0
unsigned int  atvm_NACreateTransCompleteThread( unsigned int Transtype ) {


				  pthread_t thread;
				  int  iret;
				  int returnVal=-1;  
				  pthread_attr_t attr;
				  returnVal = pthread_attr_init(&attr);
				  if(0!=returnVal)
                  {
                       return 0;
                  }

				  returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

				  if(0!=returnVal)
                  {
					return 0;
                  }

                  void *arg = malloc(sizeof(int));

                  *((int*)arg) = Transtype;

				  iret     = pthread_create( &thread, &attr,NACompleteTransactionThread,arg);

				  if(0!= iret)
                  {
					return 0;
                  }

				  returnVal = pthread_attr_destroy(&attr);
            
                  return 1;

}//++unsigned int  atvm_NACreateTransCompleteThread( unsigned int Transtype ) end

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef COIN_ESCROW

//++Start Coin Acceptor Transaction Complete Thread if success return 1 or fail 0
unsigned int  atvm_CACreateTransCompleteThread( unsigned int Transtype ) {

		  
                  
          pthread_t thread;
		  int  iret;
		  int returnVal=-1;  
		  pthread_attr_t attr;
		  returnVal = pthread_attr_init(&attr);
		  if(0!=returnVal)
          {
               return 0;
          }

		  returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		  if(0!=returnVal)
          {
		       return 0;
          }

          void *arg = malloc(sizeof(int));
          *((int*)arg) = Transtype;
		  iret     = pthread_create( &thread, &attr,CACompleteTransactionThread,arg);

		  if(0!= iret)
          {
		      return 0;
          }

		  returnVal = pthread_attr_destroy(&attr);
            
          return 1;

}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOTE_ACCEPTOR 

//AVRM_Currency_writeFileLog("[activateDevice()] NoteAcceptor Activated successfully.",INFO);
static void* NACompleteTransactionThread(void *argptr) {



                        //writeFileLog"[NACompleteTransactionThread()] Entry .");

                        int try=1,ret3=FAIL,ret2=FAIL;
                        unsigned char log[100];   
                        memset(log,'\0',100); 
                        unsigned int value=0,noteStatus=0;

                        unsigned int *fnarg=(int *) argptr;
                        sprintf(log,"[NACompleteTransactionThread()] fnarg = %d.",*fnarg);
                        //writeFileLoglog);
                        
                        pthread_mutex_lock(&g_NAComplteOperationmutex );
                        g_NAComplteOperation=false;
                        pthread_mutex_unlock(&g_NAComplteOperationmutex );
                        
                        // COMMIT_TRANS start here
                        if( COMMIT_TRANS ==  *fnarg ) {
		                     
				             #ifdef B2B_NOTE_ACCEPTOR

				             if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
				             {
				                        
				                        //++B2BFinishRejectingState();
				                        
				                        if( SUCCESS != B2BFinishRejectingState() )
									    {
																	   //++Check for communication Failure,Return Mouth Blocked
																	   unsigned char PollReplyPacket[30];

																	   unsigned int  PollBufferLength=30;

																	   unsigned int  PollReplyPacketlength=0;

																	   int rtcode=0 ;

																	   rtcode=GetCurrentB2BState(PollReplyPacket,PollBufferLength,&PollReplyPacketlength);

																	   if(1==rtcode)
																	   {

																		   memset(log,'\0',100);

																		   sprintf(log,"[NACompleteTransactionThread() Cancel] B2B Current State: 0x%x .",PollReplyPacket[3]);

																		   AVRM_Currency_writeFileLog(log,INFO);
															  
																		   //Rejecting State : 0x1C
																		   //Jam in BillPath : 0x43
																		   //Returning State : 0x18
																		   if( (0x1C== PollReplyPacket[3])  || (0x43 == PollReplyPacket[3])) 
																		   {
																					 //free thread arguments pointer
																					 if(NULL != fnarg)
																					 {
																						free(fnarg);

																					 }//if end
																					 pthread_mutex_lock(&g_NACancelStatusmutex);

																					 g_NACancelStatus= OTHER_ERROR ; //Return Mouth Blocked
																					 
																					 pthread_mutex_unlock(&g_NACancelStatusmutex);
									   
																					 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] Return Mouth Blocked Status Found.",INFO);
																					 
																					 pthread_mutex_lock (&g_NAComplteOperationmutex );

																					 g_NAComplteOperation=true;

																					 pthread_mutex_unlock(&g_NAComplteOperationmutex );
																			
																					 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .",INFO);

																					 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Exit .",INFO);

																					 pthread_exit(0);
																		   

																	     }else if(0x19== PollReplyPacket[3]){
																	      //++Disable state found
																	       AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Normal Disabled State Found.",INFO);
																	     }else{
																			  
																			 //++free thread arguments pointer
																			 if(NULL != fnarg)
																			 {
																					free(fnarg);
																			 }//++if end
																			 pthread_mutex_lock(&g_NACancelStatusmutex );
																			 
																			 g_NACancelStatus= OTHER_ERROR ; //Any Other Exception 
																			 
																			 pthread_mutex_unlock(&g_NACancelStatusmutex );

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] read b2b current state failed.",INFO);

																			 pthread_mutex_lock (&g_NAComplteOperationmutex );
																			 
																			 g_NAComplteOperation=true;

																			 pthread_mutex_unlock(&g_NAComplteOperationmutex );
																		
																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .",INFO);

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Exit .",INFO);

																			 pthread_exit(0);

																		  }//++else block
																	   
																	   }//if(1==rtcode) block
																	   else 
																	   {
																			 //++free thread arguments pointer
																			 if(NULL != fnarg)
																			 {
																					free(fnarg);
																			 }//++if end
																			 pthread_mutex_lock(&g_NACancelStatusmutex );
																			 
																			 g_NACancelStatus= COMMUNICATION_FAILURE_NOTE_ACCEPTOR ; //Communication Failure
																			 
																			 pthread_mutex_unlock(&g_NACancelStatusmutex );

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] GetCurrentB2BState return comm failure.",INFO);

																			 pthread_mutex_lock (&g_NAComplteOperationmutex );
																			 
																			 g_NAComplteOperation=true;

																			 pthread_mutex_unlock(&g_NAComplteOperationmutex );
																		
																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .",INFO);

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Exit .",INFO);

																			 pthread_exit(0);

																	   }//++else block end
																	  
								        }//++if( SUCCESS != B2BFinishRejectingState() )
																   
				                        delay_mSec(200);

				                        unsigned int Currenttotalnotes=0;

				                        Currenttotalnotes=CalculateCurrentBillQuantity();
				                   
                                        AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] Before AcceptFromEscrow().",INFO);

				                        for(try=1;try<=1;try++)
				                        {  
				                             
				                             //start b2b unloading state
				                             ret2 = AcceptFromEscrow(1,g_ttlNmbrOfNote); 

				                             if( 1 == ret2 )  
				                             {

				                                  AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] AcceptFromEscrow() return with success.",INFO);
				                                  AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] Now no retry needed.",INFO);
				                                  AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] Before GetAcceptedNoteDetail().",INFO);
				                                  for(try=1;try<=1;try++)
				                                  {
				                                     
				                                      //wait for b2b unloaded state and return when b2b finish with disabled state
                                                      memset(log,'\0',100);
                                                      sprintf(log,"[NACompleteTransactionThread() Commit] Multi Note Commit Timeout: %d.",(g_CommitTransTime/1000));
				                                      AVRM_Currency_writeFileLog(log,INFO);

				                                      ret3 = GetAcceptedNoteDetailWithTime(&value,&noteStatus,(g_CommitTransTime/1000) ); 

				                                      if(1 == ret3)
				                                      {
				                                          //AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with success.",INFO); 
				                                          //AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] No retry needed.",INFO); 
				                                          break;

				                                      }
				                                      else
				                                      {
				                                           //AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with fail.",INFO); 
				                                           memset(log,'\0',100);
				                                           sprintf(log,"[NACompleteTransactionThread() Commit] Retry due to fail return %d.",try);
				                                           //AVRM_Currency_writeFileLog(log,INFO);

                                                           pthread_mutex_lock(&g_NACommitStatusmutex);

				                                           g_NACommitStatus= OTHER_ERROR; //Operation timeout occured        

                                                           pthread_mutex_unlock(&g_NACommitStatusmutex);

				                                           continue;
				                                      }//else end

				                                    }//internal foor loop end
				                                    if(SUCCESS == ret3) {
				                                        //AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with success.",INFO);
                                                        pthread_mutex_lock(&g_NACommitStatusmutex);
                                                        g_NACommitStatus= STACKED_NOTE_COIN; //Success Operation       
                                                        pthread_mutex_unlock(&g_NACommitStatusmutex);
                                                    }
				                                    else
                                                    {

				                                        //AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with fail.",INFO);

                                                        pthread_mutex_lock(&g_NACommitStatusmutex);

				                                        g_NACommitStatus= OTHER_ERROR; //++Operation timeout occured        

                                                        pthread_mutex_unlock(&g_NACommitStatusmutex);

                                                    }//else block

				                                    //AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] After GetAcceptedNoteDetail().",INFO);
				                                    break;

				                                 }//if( SUCCESS == ret2 )  
				                                 else
				                                 {

				                                     memset(log,'\0',100);

				                                     sprintf(log,"[NACompleteTransactionThread() Commit] AcceptFromEscrow() return with fail %d .",try);

				                                     AVRM_Currency_writeFileLog(log,INFO);

                                                     pthread_mutex_lock(&g_NACommitStatusmutex);

				                                     g_NACommitStatus= OTHER_ERROR; //++Communication Failure       

                                                     pthread_mutex_unlock(&g_NACommitStatusmutex);  

				                                     continue;

				                                 }//else end

				                              }//external foor loop end

				                        //writeFileLog"[NACompleteTransactionThread() Commit] After AcceptFromEscrow().");

				                 }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
				                 else
				                 {

				                    //AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Commit] Note Acceptor low level thread is not starting so no accept escrow can be done now!!.",INFO);
                                       
                                    pthread_mutex_lock(&g_NACommitStatusmutex);

				                    g_NACommitStatus= OTHER_ERROR; //Any Other Exception             

                                    pthread_mutex_unlock(&g_NACommitStatusmutex);

				                 }//else end
				                 
				              #endif
                              
                               
                              #ifdef JCM_NOTE_ACCEPTOR

							  if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD)) {

													 //Step 1: Wait for if any note rejected by note acceptor is not taken out by customer
													 //Step 2: then calculate recycle quanity
													 //Step 3: then issue unload/dispense command
													 //Step 4: calculate recycle quanity after operation
													 //Step 5: if equal to dispensed/unloaded quanity then exit or not eqaul go to retry

													 int CurrentCastQuanity=0,AfterCastQuanity=0;
													 GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
													 memset(log,'\0',100);
													 sprintf(log,"[NACompleteTransactionThread()] JCM Before Unload Recycle Quantity = %d.",CurrentCastQuanity);
													 //writeFileLoglog);
													 int rtcode=-1;

													 //writeFileLog"[NACompleteTransactionThread()] Before Unload Operation Calling.");

													 for(try=1;try<=2;try++) {
													
														   //++unload all notes from jcm escrow
														   
														   rtcode=UnloadCashAcptr();
														  
														   if(1 == rtcode){

																   //writeFileLog"[NACompleteTransactionThread()] Unload successfully executed.");
																   break;

														   }else{
																   //writeFileLog"[NACompleteTransactionThread()]  Unload Operation failed.");
																   memset(log,'\0',100); 
												                   sprintf(log,"[NACompleteTransactionThread()]  retry: %d.",try);
												                   //writeFileLoglog);
																   delay_mSec(200);
																   rtcode=-1;
																   continue;
																   
                                                           }//else end


													 }//for end

													 //writeFileLog"[NACompleteTransactionThread()] After Unload Operation Calling.");
                                                     //writeFileLog"[NACompleteTransactionThread()] After Unload Operation now going to read jcm escrow notes number.");
                                                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );

                                                     memset(log,'\0',100);
                                                     sprintf(log,"[NACompleteTransactionThread()] JCM After Unload Recycle Quantity = %d.",AfterCastQuanity );
                                                     //writeFileLoglog);

													 if(1 == rtcode){
													   ret3=SUCCESS;
													   g_NACommitStatus= STACKED_NOTE_COIN; //++Success
													 }
													 else{
													   ret3=FAIL;
													   g_NACommitStatus= OTHER_ERROR; //++other error
													 }//else end

							  }//if end

                              #endif

                                             
                         }// COMMIT_TRANS end here

		         
                         //////////////////////////////////////////////////////////////////////////////////////////////////////////////

                         // CANCEL_TRANS  start here
                         if( CANCEL_TRANS ==  *fnarg ) {

										   
								   #ifdef B2B_NOTE_ACCEPTOR
										   if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
										   {
																   if( SUCCESS != B2BFinishRejectingState() )
																   {
																	   //++Check for communication Failure,Return Mouth Blocked
																	   unsigned char PollReplyPacket[30];

																	   unsigned int  PollBufferLength=30;

																	   unsigned int  PollReplyPacketlength=0;

																	   int rtcode=0 ;

																	   rtcode=GetCurrentB2BState(PollReplyPacket,PollBufferLength,&PollReplyPacketlength);

																	   if(1==rtcode)
																	   {

																		   memset(log,'\0',100);

																		   sprintf(log,"[NACompleteTransactionThread() Cancel] B2B Current State: 0x%x .",PollReplyPacket[3]);

																		   AVRM_Currency_writeFileLog(log,INFO);
															  
																		   //Rejecting State : 0x1C
																		   //Jam in BillPath : 0x43
																		   //Returning State : 0x18
																		   if( (0x1C== PollReplyPacket[3])  || (0x43 == PollReplyPacket[3])) 
																		   {
																					 //free thread arguments pointer
																					 if(NULL != fnarg)
																					 {
																						free(fnarg);

																					 }//if end
																					 pthread_mutex_lock(&g_NACancelStatusmutex);

																					 g_NACancelStatus= OTHER_ERROR ; //Return Mouth Blocked
																					 
																					 pthread_mutex_unlock(&g_NACancelStatusmutex);
									   
																					 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] Return Mouth Blocked Status Found.",INFO);
																					 
																					 pthread_mutex_lock (&g_NAComplteOperationmutex );

																					 g_NAComplteOperation=true;

																					 pthread_mutex_unlock(&g_NAComplteOperationmutex );
																			
																					 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .",INFO);

																					 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Exit .",INFO);

																					 pthread_exit(0);
																		   

																	     }else if(0x19== PollReplyPacket[3]){
																	      //++Disable state found
																	       AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Normal Disabled State Found.",INFO);
																	     }else{
																			  
																			 //++free thread arguments pointer
																			 if(NULL != fnarg)
																			 {
																					free(fnarg);
																			 }//++if end
																			 pthread_mutex_lock(&g_NACancelStatusmutex );
																			 
																			 g_NACancelStatus= OTHER_ERROR ; //Any Other Exception 
																			 
																			 pthread_mutex_unlock(&g_NACancelStatusmutex );

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] read b2b current state failed.",INFO);

																			 pthread_mutex_lock (&g_NAComplteOperationmutex );
																			 
																			 g_NAComplteOperation=true;

																			 pthread_mutex_unlock(&g_NAComplteOperationmutex );
																		
																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .",INFO);

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Exit .",INFO);

																			 pthread_exit(0);

																		  }//++else block
																	   
																	   }//if(1==rtcode) block
																	   else 
																	   {
																			 //++free thread arguments pointer
																			 if(NULL != fnarg)
																			 {
																					free(fnarg);
																			 }//++if end
																			 pthread_mutex_lock(&g_NACancelStatusmutex );
																			 
																			 g_NACancelStatus= COMMUNICATION_FAILURE_NOTE_ACCEPTOR ; //Communication Failure
																			 
																			 pthread_mutex_unlock(&g_NACancelStatusmutex );

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] GetCurrentB2BState return comm failure.",INFO);

																			 pthread_mutex_lock (&g_NAComplteOperationmutex );
																			 
																			 g_NAComplteOperation=true;

																			 pthread_mutex_unlock(&g_NAComplteOperationmutex );
																		
																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .",INFO);

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread()] Exit .",INFO);

																			 pthread_exit(0);

																	   }//++else block end
																	  
																   }//++if( SUCCESS != B2BFinishRejectingState() )
																   
																   delay_mSec(200);

																   unsigned int totalnotes=0,retry = 1;

																   totalnotes=CalculateCurrentBillQuantity();

																	for(try = 1; try<=1;try++)  
																	{

																			 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] before reject from escrow.",INFO);

																			 ret2 = RejectFromEscrow(24,g_ttlNmbrOfNote);

																			 if( 1 == ret2 )
																			 {

																					AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] RejectFromEscrow() return with success.",INFO);
																					AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] Now no need to retry here.",INFO);
																					AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] before GetReturnDetail().",INFO);   

																					for(retry=1; retry<=1;retry++) 
																					{
																							ret3 = GetReturnDetail(&value,&noteStatus);

																							if( 1 == ret3 )  
																							{
																											  AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] GetReturnDetail() return with success status.",INFO);

																											  pthread_mutex_lock(&g_NACancelStatusmutex);

																											  g_NACancelStatus= 1;//++Return Success

																											  pthread_mutex_unlock(&g_NACancelStatusmutex);

																											  AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] Now no retry here.",INFO);

																											  break;

																							}// if( SUCCESS == ret3 )  
																							else
																							{
																											   AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] GetReturnDetail() return with fail status.",INFO);

																											   memset(log,'\0',100); 

																											   sprintf(log,"[NACompleteTransactionThread() Cancel] GetReturnDetail() Retry: %d.",retry);

																											   //writeFileLoglog);
																											   
																											   pthread_mutex_lock(&g_NACancelStatusmutex);

																											   g_NACancelStatus= OTHER_ERROR ;//Operation Timeout Occured

																											   pthread_mutex_unlock(&g_NACancelStatusmutex);

																											  continue; 
																							
																							}//else end

																					 }//++for loop end

																					 AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] after GetReturnDetail().",INFO);

																					 break;

																			 }//++if( SUCCESS == ret2 )
																			 else
																			 {
																					AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] RejectFromEscrow() return with fail.",INFO);
																					memset(log,'\0',100); 
																					sprintf(log,"[NACompleteTransactionThread() Cancel]  RejectFromEscrow() Retry: %d.",retry);
																					AVRM_Currency_writeFileLog(log,INFO);
																					pthread_mutex_lock(&g_NACancelStatusmutex);
																					g_NACancelStatus= OTHER_ERROR ;//Communication Failure
																					pthread_mutex_unlock(&g_NACancelStatusmutex);
																					continue;
																			 }//++else end
																			 
																   }//++external for loop end
																   													   
																   AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] after reject from escrow.",INFO);

											 }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
										   else
										   {
													AVRM_Currency_writeFileLog("[NACompleteTransactionThread() Cancel] Note Acceptor low level thread is not closed so no reject escrow can be done now!!.",INFO);

													pthread_mutex_lock(&g_NACancelStatusmutex);

													g_NACancelStatus= OTHER_ERROR ;//Any Other Exception
																	
													pthread_mutex_unlock(&g_NACancelStatusmutex);

											}//++else end

								   #endif
								                                        
                                   #ifdef JCM_NOTE_ACCEPTOR

								   if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD) ){
                                     
											 //Step 1: Wait for if any note rejected by note acceptor is not taken out by customer
											 //Step 2: then calculate recycle quanity
											 //Step 3: then issue unload/dispense command
											 //Step 4: calculate recycle quanity after operation
											 //Step 5: if equal to dispensed/unloaded quanity then exit or not eqaul go to retry
									 
											 int CurrentCastQuanity=0,AfterCastQuanity=0;
											 GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
											 memset(log,'\0',100);
											 sprintf(log,"[NACompleteTransactionThread()] JCM Before Dispense Recycle Quantity = %d.",CurrentCastQuanity);
											 //writeFileLoglog);
											 int rtcode=-1,try=1;

											 //writeFileLog"[NACompleteTransactionThread()] Before Dispense Operation Calling.");
										 
											 for(try=1;try<=2;try++) {
											
												   //dispense all notes
												   rtcode=DispenseCashAcptr();
                                                   if( (1 == rtcode) || (4 == rtcode) ){ 

														 //writeFileLog"[NACompleteTransactionThread()] Dispense Operation successfully executed.");
														 break;

												   }else{

														  //writeFileLog"[NACompleteTransactionThread()]  Dispense notes failed.");
														  memset(log,'\0',100); 
												          sprintf(log,"[NACompleteTransactionThread()]  Retry: %d.",try);
												          //writeFileLoglog);
														  delay_mSec(200);
														  rtcode=-1;
														  continue;
														  
                                                   }//else end
                                                   
                                             }//for end

											 //writeFileLog"[NACompleteTransactionThread()] After Dispense Operation Calling.");

											 GetNoteDetailsInRecycleCst( &AfterCastQuanity );

											 memset(log,'\0',100);

											 sprintf(log,"[NACompleteTransactionThread()] JCM After Dispensed Recycle Quantity = %d.",AfterCastQuanity );

											 //writeFileLoglog);

											 if( (1 == rtcode)|| (4 == rtcode) ) {
											   ret3=SUCCESS;
											   g_NACancelStatus= 1; //Success
											 }
											 else {
											   ret3=FAIL;
											   g_NACancelStatus= 0; //Fail
                                             } //else end

							   } //if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD) ) end

							       #endif


                         }// CANCEL_TRANS  end here

                         //free thread arguments pointer
                         if(NULL != fnarg)
                         {
                                free(fnarg);

                         }

                         //writeFileLog"[NACompleteTransactionThread()] Before Set Note acceptor cancel work complete signal .");

                         //Send Signal That note acceptor work [accept or reject ] is complete

                         //pthread_mutex_lock( &g_NACompleteThreadmutex );

						 //pthread_cond_signal( &g_NACompleteThreadCond );

						 //pthread_mutex_unlock( &g_NACompleteThreadmutex );

                         pthread_mutex_lock(&g_NAComplteOperationmutex );

                         g_NAComplteOperation=true;

                         pthread_mutex_unlock(&g_NAComplteOperationmutex );
                        
                         //writeFileLog"[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .");

                         //writeFileLog"[NACompleteTransactionThread()] Exit .");

                         pthread_exit(0);

}//[NACompleteTransactionThread()] end

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef COIN_ESCROW

static void* CACompleteTransactionThread(void *argptr) {


                      //writeFileLog"[CACompleteTransactionThread()] Entry .");
                      unsigned char log[100];
                      memset(log,'\0',100); 
                      int try=1,ret1=FAIL;
             
             
                      unsigned int *fnarg=(int *) argptr;
                      sprintf(log,"[CACompleteTransactionThread() Commit] myptr = %d.",*fnarg);
                      //writeFileLoglog);

                      pthread_mutex_lock(&g_CAComplteOperationmutex );
                      g_CAComplteOperation=false;
                      pthread_mutex_unlock(&g_CAComplteOperationmutex );

                       //COMMIT_TRANS start here
                       if(COMMIT_TRANS==  *fnarg )
                       {

								  //if coin acceptor low level flag is closed then do operation (1 means off and 0 means on)
								  if(1==WaitforCoinAcptrLowlevelflagClosed(WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD))
								  {

										   for(try=1;try<=2;try++)
										   {      
						 
												   ret1 = AcceptFromCoinEscrow();

												   if( SUCCESS == ret1 )
												   {
													   //writeFileLog"[CACompleteTransactionThread() Commit] Accept Coin Successfully.");
													   memset(log,'\0',100); 

													   sprintf(log,"[CACompleteTransactionThread() Commit]  Retry: %d.",try);
													   //writeFileLoglog);

													   break;
												   }
												   else
												   {
													   //writeFileLog"[CACompleteTransactionThread() Commit] Accept Coin failed.");
													   memset(log,'\0',100); 

													   sprintf(log,"[CACompleteTransactionThread() Commit]  Retry: %d.",try);
													   //writeFileLoglog);

													   continue;

												   }

										   }//for block end

									  }
									  else
									  {
											   //writeFileLog"[CACompleteTransactionThread() Commit] Coin Acceptor Low Level flag is not closed so no accept escrow can be done now!!. ");
											   ret1=FAIL;
									  }

		                              //set commit transaction status

                                      pthread_mutex_lock(&g_CACommitStatusmutex);

                                      if( SUCCESS == ret1 )
                                      {
											g_CACommitStatus=SUCCESS;

                                      }
                                      else
                                      {
                                            g_CACommitStatus=FAIL;

                                      }

                                      pthread_mutex_unlock(&g_CACommitStatusmutex); 

 
                         }//COMMIT_TRANS end here

		         
                         //CANCEL_TRANS  start here
                         if( CANCEL_TRANS ==  *fnarg )
                         {

								   if(1==WaitforCoinAcptrLowlevelflagClosed(WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD))
								   {

										   for(try = 1; try<=2;try++)  
										   {

												   ret1 = RejectFromCoinEscrow(); 

												   if(SUCCESS == ret1)
												   {

															   //writeFileLog"[CACompleteTransactionThread() Cancel] Reject Coin Successfully.");
															   memset(log,'\0',100); 

															   sprintf(log,"[CACompleteTransactionThread() Cancel]  Retry: %d.",try);
															   //writeFileLoglog);

															   break;
												   }
												   else
												   {
															  //writeFileLog"[CACompleteTransactionThread() Cancel]  Reject Coin failed.");
															  memset(log,'\0',100); 

															  sprintf(log,"[CACompleteTransactionThread() Cancel]  Retry: %d.",try);
															  //writeFileLoglog);

															  continue;

													}

											}

									 }
									 else
									 {
										   //writeFileLog"[CACompleteTransactionThread() Cancel] Coin Acceptor Low Level flag is not closed so no reject escrow can be done now!!.");
										   ret1=FAIL;
									 }


								     //set cancel transaction status

								     pthread_mutex_lock(&g_CACancelStatusmutex);

							         if( SUCCESS == ret1 )
									 {
										 g_CACancelStatus=SUCCESS;

						             }
									 else
									 {

										 g_CACancelStatus=FAIL;

									 }

									 pthread_mutex_unlock(&g_CACancelStatusmutex); 


                           }// CANCEL_TRANS  end here

                           //free thread arguments pointer
                           if(NULL != fnarg)
                           {
                                free(fnarg);

                           }

                           //Send Signal That coin acceptor work [accept or reject] is complete

                           //pthread_mutex_lock(   &g_CACompleteThreadmutex );
                           //pthread_cond_signal(  &g_CACompleteThreadCond );
                           //pthread_mutex_unlock( &g_CACompleteThreadmutex );

                           pthread_mutex_lock(&g_CAComplteOperationmutex );
                           g_CAComplteOperation=true;
                           pthread_mutex_unlock(&g_CAComplteOperationmutex );

                           //writeFileLog"[CACompleteTransactionThread()] Exit .");

                           pthread_exit(0);


}//static void* CACompleteTransactionThread(void *argptr) end

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ACCEPTING_STATE_UNKNOWN  -1
#define ACCEPTING_STATE           0
#define COMMIT_STATE              1
#define CANCEL_STATE              2
#define TIMEOUT_STATE             3

static pthread_mutex_t g_AcceptanceStateMutex= PTHREAD_MUTEX_INITIALIZER;
static bool g_AcceptanceState= ACCEPTING_STATE_UNKNOWN;

//++AVRM API
int cancelTranInterface(int timeout) {
     
		      int rtcode = 0;

		      pthread_mutex_lock(&g_SingleNoteEventMutex);

              //g_ReturnAcceptedTime = 0;

              //g_ReturnAcceptedTime = timeout ; //Set Timeout for CancelTrans
              
              //Set Cancel State
              pthread_mutex_lock(&g_AcceptanceStateMutex);
              g_AcceptanceState=COMMIT_STATE; 
              pthread_mutex_unlock(&g_AcceptanceStateMutex);

		      if( true == g_SingleNoteEvent )
		      {
				   pthread_mutex_unlock(&g_SingleNoteEventMutex);

				   //Now Call our Single Note Cancel Trans function

				   rtcode = cancelTran_Ver3(timeout) ;

				   if( true == rtcode )
				   { 
						ResetTransData();
				   }

			   
		      }//if block
		      else
		      {
				  pthread_mutex_unlock(&g_SingleNoteEventMutex);     
				  
				  //Now Call our Multi Note Cancel Trans function

				  rtcode =  cancelTran_Ver2();

				  if( true == rtcode )
				  { 
						ResetTransData();
				  }

		      }//else

		      return rtcode ;


}//++cancelTranInterface() end

//++AVRM API
int commitTranInterface(int timeout) {


	      int rtcode = 0;

	      pthread_mutex_lock(&g_SingleNoteEventMutex);
 
          //g_StackAcceptedTime = 0;

          //g_StackAcceptedTime = timeout; //Set timeout for multinote
          
          //Set Commit State
          pthread_mutex_lock(&g_AcceptanceStateMutex);
          g_AcceptanceState=CANCEL_STATE; 
          pthread_mutex_unlock(&g_AcceptanceStateMutex);

	      if( true == g_SingleNoteEvent )
	      {
			   pthread_mutex_unlock(&g_SingleNoteEventMutex);

			   //Now Call our Single Note Commit Trans function
			   rtcode = commitTran_Ver3(timeout) ;

			   if( true == rtcode )
			   { 
			         ResetTransData();

			   }
		
	      }else {
			  
			  pthread_mutex_unlock(&g_SingleNoteEventMutex);     
			  
			  //Now Call our MUlti Note Commit Trans function

              g_CommitTransTime = timeout;

			  rtcode =  commitTran_Ver2() ;

			  if( true == rtcode )
			  { 
			        ResetTransData();
              }


	      }//else block
	      
	      return rtcode ;
	      
}//++commitTranInterface() end

//++Accept Transaction Version 2 //++ASCRM API
int commitTran_Ver2() {

 
         bool CAThreadStatus=false,NAThreadStatus=false;

         int ComitStatus=0,ComitStatus1=0,ComitStatus2=0;
	
         int totalnotes=0,rtcode=0;
         
         unsigned char log[100];   
         
         memset(log,'\0',100); 
         
         AVRM_Currency_writeFileLog("[commitTran_Ver2()] Entry:",INFO);
         
         AVRM_Currency_writeFileLog("[commitTran_Ver2()]  Commit Transaction issue by system......",INFO);
         
         bool already_unlock=false;
    
         pthread_mutex_lock(&AcptFareThreadStatusmutex);

         //if all acpt fare thread not stop then stop them before doing commit trans
		 if(THREAD_START==g_AcptFareThreadStatus) {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	          
	          AVRM_Currency_writeFileLog("[commitTran_Ver2()] Commit Transaction issue by system before close all acceptfare threads.",INFO);
       
	          rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);

              already_unlock=true;                                              

	          AVRM_Currency_writeFileLog("[commitTran_Ver2()] after return KillAllRunningThread().",INFO);

	     }//IF END
         
         if( false == already_unlock ) {

               pthread_mutex_unlock(&AcptFareThreadStatusmutex);

         }//if end

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	     if(THREAD_CLOSE==g_AcptFareThreadStatus) {

                  
                  pthread_mutex_unlock(&AcptFareThreadStatusmutex);
 
                  #ifdef COIN_ACCEPTOR  
                  memset(log,'\0',100); 
                  sprintf(log,"[commitTran_Ver2()] Total Coins : %d .",g_ttlNmbrOfCoin);
                  AVRM_Currency_writeFileLog(log,INFO);  
                  #endif

                  #ifdef NOTE_ACCEPTOR 
                  memset(log,'\0',100);
                  sprintf(log,"[commitTran_Ver2()] Total Notes : %d .",g_ttlNmbrOfNote);
                  ////writeFileLoglog);
                  AVRM_Currency_writeFileLog(log,INFO);  
                  #endif

                  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                  if( g_ttlNmbrOfNote > 0 ) {
					  
						   //Check b2b failure state
						   #ifdef NOTE_ACCEPTOR  

								#ifdef B2B_NOTE_ACCEPTOR
								int rtcode = 0;
								rtcode = CheckB2BFailureState();
								if( 0!=rtcode  )
								{
								   return rtcode;
								}
								#endif

						   #endif

                  }//IF END

                  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #ifdef COIN_ESCROW
                  
                  ////writeFileLog"[commitTran_Ver2()] Before Accept Coin.");
                  
                  AVRM_Currency_writeFileLog("[commitTran_Ver2()] Before Accept Coin.",INFO); 
                  
				  //if(g_ttlNmbrOfCoin>0){
								   
								   pthread_mutex_lock(&g_CAComplteOperationmutex );

								   g_CAComplteOperation=false;

								   pthread_mutex_unlock(&g_CAComplteOperationmutex );

								   rtcode=0;

								   rtcode=atvm_CACreateTransCompleteThread( COMMIT_TRANS );

								   //Thread create successfully
								   if(1 == rtcode){
										AVRM_Currency_writeFileLog("[commitTran_Ver2()] Successfully started atvm_CACreateTransCompleteThread with commit mode.",INFO);
										CAThreadStatus=true;
								   }
								   else //thread create failed
								   {
									   AVRM_Currency_writeFileLog("[commitTran_Ver2()] failed to  start atvm_CACreateTransCompleteThread with commit mode.",ERROR);
								   }

				  /*} else {
						
								   pthread_mutex_lock( &g_CACommitStatusmutex );
								   g_CACommitStatus=SUCCESS;
								   pthread_mutex_unlock( &g_CACommitStatusmutex );

				  }//else end
				  */	
                  ////writeFileLog"[commitTran_Ver2()] After Accept Coin.");
                  
                  #endif
               
                  #ifndef COIN_ESCROW
                  
                  pthread_mutex_lock( &g_CACommitStatusmutex );
                  g_CACommitStatus=SUCCESS;
                  pthread_mutex_unlock( &g_CACommitStatusmutex );
                  
                  #endif

                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #ifdef NOTE_ACCEPTOR 
                   
                  pthread_mutex_lock(&mut);
           
                  //if(g_ttlNmbrOfNote>0) {      
                   
                           pthread_mutex_unlock(&mut);  

                           pthread_mutex_lock(&g_NAComplteOperationmutex );

                           g_NAComplteOperation=false;

                           pthread_mutex_unlock(&g_NAComplteOperationmutex );

                           //store global note number
                           totalnotes=g_ttlNmbrOfNote;
                           rtcode=0;
                           rtcode=atvm_NACreateTransCompleteThread( COMMIT_TRANS );

                           //Thread create successfully
                           if(1 == rtcode){
                                AVRM_Currency_writeFileLog("[commitTran_Ver2()] Successfully started atvm_NACreateTransCompleteThread with commit mode.",INFO);
                                NAThreadStatus=true;
                           }
                           else //thread create failed
                           {
                                AVRM_Currency_writeFileLog("[commitTran_Ver2()] failed to  start atvm_NACreateTransCompleteThread with commit mode.",INFO);
                           }//else end
                            
  
                  /*}else{
                            pthread_mutex_unlock(&mut);
							pthread_mutex_lock( &g_NACommitStatusmutex );
							g_NACommitStatus = SUCCESS;
							pthread_mutex_unlock( &g_NACommitStatusmutex );
                  }//else end
                  */
                 #endif

                 #ifndef NOTE_ACCEPTOR
                 
                 pthread_mutex_lock( &g_NACommitStatusmutex );
			     g_NACommitStatus=SUCCESS;
		         pthread_mutex_unlock( &g_NACommitStatusmutex );
		         
                 #endif
                 
                 #ifndef COIN_ACCEPTOR
                 
                 pthread_mutex_lock( &g_CACommitStatusmutex );
			     g_CACommitStatus=SUCCESS;
		         pthread_mutex_unlock( &g_CACommitStatusmutex );
		         
                 #endif

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                //wait for coin acceptor commit status signal
                struct timespec begints, endts,diffts;
                  
                #ifdef COIN_ACCEPTOR  

                if(true == CAThreadStatus) {

                       //pthread_mutex_lock( &g_CACompleteThreadmutex );
                       //pthread_cond_wait( &g_CACompleteThreadCond , &g_CACompleteThreadmutex );
                       //pthread_mutex_unlock( &g_CACompleteThreadmutex );
                       
                       clock_gettime(CLOCK_MONOTONIC, &begints);

                       while(1){
								   pthread_mutex_lock(&g_CAComplteOperationmutex );

								   if ( true == g_CAComplteOperation )
                                   {
                                           
                                           clock_gettime(CLOCK_MONOTONIC, &endts);
 
                                           diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                           memset(log,'\0',100);

										   sprintf(log,"[commitTran_Ver2()] coin acceptor operation complete in Seconds =%d .",diffts.tv_sec ); 

										   AVRM_Currency_writeFileLog(log,INFO);

										   pthread_mutex_unlock(&g_CAComplteOperationmutex );

                                           break;

                                   }//if end

                                   pthread_mutex_unlock(&g_CAComplteOperationmutex );

                       }//while end

                }//if end
                
                #endif
                

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //++wait for note acceptor commit status signal

				  #ifdef NOTE_ACCEPTOR  

					if( true == NAThreadStatus ) {

							   //pthread_mutex_lock( &g_NACompleteThreadmutex );

							   //pthread_cond_wait( &g_NACompleteThreadCond , &g_NACompleteThreadmutex );

							   //pthread_mutex_unlock( &g_NACompleteThreadmutex );
							   clock_gettime(CLOCK_MONOTONIC, &begints);

							   while(1)
							   {
								           pthread_mutex_lock(&g_NAComplteOperationmutex );

								           if ( true == g_NAComplteOperation )
										   {
												   clock_gettime(CLOCK_MONOTONIC, &endts);
		 
												   diffts.tv_sec = endts.tv_sec - begints.tv_sec;

												   memset(log,'\0',100);

												   sprintf(log,"[commitTran_Ver2()] note acceptor operation complete in Seconds =%d .",diffts.tv_sec ); 
												   
												   AVRM_Currency_writeFileLog(log,INFO);

												   pthread_mutex_unlock(&g_NAComplteOperationmutex );

												   break;

										   }//if end

										   pthread_mutex_unlock(&g_NAComplteOperationmutex );


							   }//while end

					  }//if( true == NAThreadStatus ) end

				  #endif

                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #ifdef COIN_ESCROW
                  pthread_mutex_lock( &g_CACommitStatusmutex );
                  #endif

                  #ifdef NOTE_ACCEPTOR  
                  pthread_mutex_lock( &g_NACommitStatusmutex );
                  #endif

                  /*if( (g_ttlNmbrOfNote>0) && (g_maxNoOfCash>0) ) {
					ComitStatus1 = g_NACommitStatus;
			      }else{
					 ComitStatus1 = SUCCESS;
				  }//else end
                 
                  if( (g_ttlNmbrOfCoin>0) && (g_maxNoOfCoin>0) ){
					ComitStatus2 = g_CACommitStatus; 
				  }else{
					ComitStatus2 = SUCCESS; 
				  }//else end
				  */
				  
				  ComitStatus1 = g_NACommitStatus;
				  ComitStatus2 = g_CACommitStatus; 
				  
                  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                  
                  ComitStatus  = 0;
                  ComitStatus1 = g_NACommitStatus;
                  ComitStatus2 = g_CACommitStatus;

                  if(  ( SUCCESS == ComitStatus1 ) || ( SUCCESS == ComitStatus2) ) {


                              pthread_mutex_lock(&mut);

                              g_ttlNmbrOfNote =0;

                              pthread_mutex_unlock(&mut);
		                      
		                      AVRM_Currency_writeFileLog(log,INFO);

                              memset(log,'\0',100);

		                      sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d committed With Success Status.",g_total_transaction);
		                      
		                      AVRM_Currency_writeFileLog(log,INFO);
		                      
		                      ComitStatus = SUCCESS;

		              
                  }
		          else if(  (OPERATION_TIMEOUT_OCCURRED == ComitStatus1) || (OPERATION_TIMEOUT_OCCURRED == ComitStatus2) )  //++Operation timeout
                  {
                             
                          ComitStatus = OPERATION_TIMEOUT_OCCURRED;
                                                        
			              AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Operation timeout Status.",INFO);

                          memset(log,'\0',100);

		                  sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);

		                  AVRM_Currency_writeFileLog(log,INFO);

		          }
                  else if(  COMMUNICATION_FAILURE_NOTE_ACCEPTOR == ComitStatus1 ) //++ -1 Communication failure Note
                  {
                               
                         ComitStatus = COMMUNICATION_FAILURE_NOTE_ACCEPTOR;
                         
			             AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Communication failure Status.",INFO);

                         memset(log,'\0',100);

		                 sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);

		                 AVRM_Currency_writeFileLog(log,INFO);

		          }
		          else if(  COMMUNICATION_FAILURE_COIN_ACCEPTOR == ComitStatus2 ) //++ -1 Communication failure Coin
                  {
                               
                         ComitStatus = COMMUNICATION_FAILURE_COIN_ACCEPTOR;
                        
			             AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Communication failure Status.",INFO);

                         memset(log,'\0',100);

		                 sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);
		                 
		                 AVRM_Currency_writeFileLog(log,INFO);

		          }
                  else if(  (OTHER_ERROR == ComitStatus1) ||  (OTHER_ERROR == ComitStatus2) ) //++Any Other exception
                  {
                               
                          ComitStatus = OTHER_ERROR;
                          
			              AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Anyother Exception Status.",INFO);

                          memset(log,'\0',100);

		                  sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);
		                  
		                  AVRM_Currency_writeFileLog(log,INFO);

		          }
                  else if(  OTHER_ERROR == ComitStatus1 ) //++No Notes in stacker
                  {
                               
                         ComitStatus = OTHER_ERROR;
                           
			             ////writeFileLog"[commitTran_Ver2() Exit] Return With No Notes in stacker Status.");
			             
			             AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With No Notes in stacker Status.",INFO);

                         memset(log,'\0',100);

		                 sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);

		                 ////writeFileLoglog);
		                 
		                 AVRM_Currency_writeFileLog(log,INFO);

		          }            
                  else if(  STACKED_NOTE_TRANSPORT_CHANEL_BLOCKED == ComitStatus1 ) //++Transport Chanel blocked Note
                  {
                               
                          ComitStatus = STACKED_NOTE_TRANSPORT_CHANEL_BLOCKED;
                           
			              ////writeFileLog"[commitTran_Ver2() Exit] Return With Transport chanel blocked Status.");
			              
			              AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Transport chanel blocked Status.",INFO);

                          memset(log,'\0',100);

		                  sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);

		                  ////writeFileLoglog);
		                  
		                  AVRM_Currency_writeFileLog(log,INFO);

		          } 
		          else if(  STACKED_COIN_TRANSPORT_CHANEL_BLOCKED == ComitStatus2 ) //++Transport Chanel blocked Coin
                  {
                               
                          ComitStatus = STACKED_COIN_TRANSPORT_CHANEL_BLOCKED;
                          
			              ////writeFileLog"[commitTran_Ver2() Exit] Return With Transport chanel blocked Status.");
			              
			              AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Transport chanel blocked Status.",INFO);

                          memset(log,'\0',100);

		                  sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);

		                  ////writeFileLoglog);
		                  
		                  AVRM_Currency_writeFileLog(log,INFO);

		          } 
                  else if(  STACKED_NOTE_CASH_BOX_FULL == ComitStatus1 ) //++Stacker Cash Box Full Note
                  {  
			              
			              ComitStatus = STACKED_NOTE_CASH_BOX_FULL;
			              
			              ////writeFileLog"[commitTran_Ver2() Exit] Return With Stacker Cash Box full Status.");
			              
			              AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Stacker Cash Box full Status.",INFO);

                          memset(log,'\0',100);

		                  sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);

		                  //writeFileLoglog);
		                  
		                  AVRM_Currency_writeFileLog(log,INFO);

		         }//else if end
		          else if(  STACKED_COIN_CASH_BOX_FULL == ComitStatus2 ) //++Stacker Cash Box Full Coin
                  {  
			              
			              ComitStatus= STACKED_COIN_CASH_BOX_FULL;
			              
			              ////writeFileLog"[commitTran_Ver2() Exit] Return With Stacker Cash Box full Status.");
			              
			              AVRM_Currency_writeFileLog("[commitTran_Ver2() Exit] Return With Stacker Cash Box full Status.",INFO);

                          memset(log,'\0',100);

		                  sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);

		                  ////writeFileLoglog);
		                  
		                  AVRM_Currency_writeFileLog(log,INFO);

		          }//else if end


                  #ifdef COIN_ESCROW
                  pthread_mutex_unlock( &g_CACommitStatusmutex );
                  #endif
                  #ifdef NOTE_ACCEPTOR  
                  pthread_mutex_unlock( &g_NACommitStatusmutex );
                  #endif

                  memset(log,'\0',100);

		          sprintf(log,"[commitTran_Ver2() Exit] Return Code: %d.",ComitStatus);

		          AVRM_Currency_writeFileLog(log,INFO);

                  return ComitStatus;
                  
		  
         } else {
			  pthread_mutex_unlock(&AcptFareThreadStatusmutex);
			  //writeFileLog"[commitTran_Ver2() Exit] No acceptfare thread stated so committrans() return Any other exception status.");
			  return (31); //Any Other Exception
		 }//else end

}//commitTran_Ver2() end here

//++Reject Transaction Version 2 //++ASCRM API
int cancelTran_Ver2() {

	
         int rtcode=0;
         unsigned char log[100];
         memset(log,'\0',100);
         bool already_unlock=false;
         int  cancelrtcode=0,cancelrtcode1=0,cancelrtcode2=0;
	     bool CAThreadStatus=false,NAThreadStatus=false;
	     
         ////writeFileLog" \n\n\n\n ");
         ////writeFileLog"[cancelTran_Ver2() Entry] ");
         ////writeFileLog"[cancelTran_Ver2() Entry]  Cancel Transaction issue by system...");
         
         AVRM_Currency_writeFileLog("\n\n\n\n ",INFO);
         AVRM_Currency_writeFileLog("[cancelTran_Ver2() Entry] ",INFO);
         AVRM_Currency_writeFileLog("[cancelTran_Ver2() Entry]  Cancel Transaction issue by system...",INFO);

         //++if all acpt fare thread not stop then stop them before doing cancel trans

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	     if( THREAD_START== g_AcptFareThreadStatus ) {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);
     
              already_unlock=true;

			  //Malay Add 19 Jan 2013
			  AVRM_Currency_writeFileLog("[cancelTran_Ver2] Cancel Transaction issue by system before close all acceptfare threads.",INFO);
              rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);
              AVRM_Currency_writeFileLog("[cancelTran_Ver2] after return KillAllRunningThread().",INFO);


	     }//if end
 
         if( false == already_unlock ) {

               pthread_mutex_unlock(&AcptFareThreadStatusmutex);

         }//if end

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	     if(THREAD_CLOSE==g_AcptFareThreadStatus) {
                 
                  pthread_mutex_unlock(&AcptFareThreadStatusmutex);
                  
                  #ifdef COIN_ACCEPTOR

                  memset(log,'\0',100);
                  sprintf(log,"[cancelTran_Ver2] Total Coins : %d .",g_ttlNmbrOfCoin);
                  ////writeFileLoglog);
                  AVRM_Currency_writeFileLog(log,INFO);
                  #endif

                  #ifdef NOTE_ACCEPTOR
                  
                  memset(log,'\0',100);
                  sprintf(log,"[cancelTran_Ver2()] Total Notes : %d .",g_ttlNmbrOfNote);
                  ////writeFileLoglog);
                  AVRM_Currency_writeFileLog(log,INFO);
                  #endif

                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                  
                  if( g_ttlNmbrOfNote > 0 ) {

					   //Check b2b failure state
					   #ifdef NOTE_ACCEPTOR  

					   #ifdef B2B_NOTE_ACCEPTOR
							int rtcode = 0;
							rtcode = CheckB2BFailureState();
							if( 0!=rtcode  )
							{
							     //writeFileLog"[cancelTran_Ver2()] Found B2B Failure State.");
								 return (-4); //Any Other Exception
								 
							}else {
								//writeFileLog"[cancelTran_Ver2()] No B2B Failure State found.");
							}

					   #endif

					   #endif
					   
                  }//if end
                  
                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		          //Malay Add 19 Jan 2013
                  #ifdef COIN_ESCROW

					  AVRM_Currency_writeFileLog("[cancelTran_Ver2()] before start reject coin thread.",INFO);

					  //if(g_ttlNmbrOfCoin>0) {  
										  
									   pthread_mutex_lock(&g_CAComplteOperationmutex );
									   g_CAComplteOperation=false;
									   pthread_mutex_unlock(&g_CAComplteOperationmutex );
									
									   rtcode=0;
									   rtcode=atvm_CACreateTransCompleteThread( CANCEL_TRANS );
									   //++Thread create successfully
									   if(1 == rtcode) {
											AVRM_Currency_writeFileLog("[cancelTran_Ver2()] Successfully started atvm_CACreateTransCompleteThread with commit mode.",INFO);
											CAThreadStatus=true;

									   }
									   else //thread create failed
									   {
                                            AVRM_Currency_writeFileLog("[cancelTran_Ver2()] failed to  start atvm_CACreateTransCompleteThread with commit mode.",ERROR);
									   }
										

					  /*} else {
							
									pthread_mutex_lock( &g_CACancelStatusmutex );
									g_CACancelStatus = SUCCESS ; 
									pthread_mutex_unlock( &g_CACancelStatusmutex );

					  }//else end*/
							  
					  AVRM_Currency_writeFileLog("[cancelTran_Ver2()] after finish reject coin thread.",INFO);

                   #endif
                  
                  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #ifndef COIN_ESCROW
                  
                  pthread_mutex_lock( &g_CACancelStatusmutex );
                  g_CACancelStatus = SUCCESS ; 
                  pthread_mutex_unlock( &g_CACancelStatusmutex );
                  
                  #endif
                 
				  #ifdef NOTE_ACCEPTOR

		         
                  pthread_mutex_lock(&mut);

			      //if(g_ttlNmbrOfNote>0) {  
                           
                           pthread_mutex_unlock(&mut);
                           pthread_mutex_lock(&g_NAComplteOperationmutex );
                           g_NAComplteOperation=false;
                           pthread_mutex_unlock(&g_NAComplteOperationmutex );
                           rtcode=0;
                           rtcode=atvm_NACreateTransCompleteThread( CANCEL_TRANS );
                           //++Thread create successfully
                           if(1 == rtcode)
                           {
                                AVRM_Currency_writeFileLog("[cancelTran_Ver2()] Successfully started atvm_NACreateTransCompleteThread with cancel mode.",INFO);
                                NAThreadStatus=true;

                           }
                           else //++thread create failed
                           {
                                AVRM_Currency_writeFileLog("[cancelTran_Ver2()] failed to  start atvm_NACreateTransCompleteThread with cancel mode.",INFO);
                                
                           }//else end
               
                  /*} //if(g_ttlNmbrOfNote>0) block
		          else
                  {

                       pthread_mutex_unlock(&mut);
					   AVRM_Currency_writeFileLog("[cancelTran_Ver2()] No Notes in stacker.",INFO);
                       pthread_mutex_lock( &g_NACancelStatusmutex );
                       g_NACancelStatus = -3 ; //No Notes in stacker
                       pthread_mutex_unlock( &g_NACancelStatusmutex );

                       
                 }//else end*/

                 AVRM_Currency_writeFileLog("[cancelTran_Ver2()] after reject notes thread .",INFO);

                 #endif

                 #ifndef NOTE_ACCEPTOR
                 
                 pthread_mutex_lock( &g_NACancelStatusmutex );
                 g_NACancelStatus = SUCCESS ; 
                 pthread_mutex_unlock( &g_NACancelStatusmutex );
                 
                 #endif

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 //++wait for coin acceptor cancel status signal

                  #ifdef COIN_ACCEPTOR  
                
					////writeFileLog"[cancelTran_Ver2()] Before wait for coin acceptor cancel status signal."); 
					AVRM_Currency_writeFileLog("[cancelTran_Ver2()] Before wait for coin acceptor cancel status signal.",INFO);
					
					if(true == CAThreadStatus)
					{

						   //pthread_mutex_lock( &g_CACompleteThreadmutex );
						   //pthread_cond_wait( &g_CACompleteThreadCond , &g_CACompleteThreadmutex );
						   //pthread_mutex_unlock( &g_CACompleteThreadmutex );
						   
						   while(1){
							   
									  pthread_mutex_lock(&g_CAComplteOperationmutex );

									   if ( true == g_CAComplteOperation )
									   {
											   pthread_mutex_unlock(&g_CAComplteOperationmutex );

											   break;
									   }//if end

									   pthread_mutex_unlock(&g_CAComplteOperationmutex );

						   }//while end

					}//++if end
					
					////writeFileLog"[cancelTran_Ver2()] After get  coin acceptor cancel status signal."); 
					AVRM_Currency_writeFileLog("[cancelTran_Ver2()] After get  coin acceptor cancel status signal.",INFO);
                
                  #endif

                 //++wait for note acceptor cancel status signal

                 #ifdef NOTE_ACCEPTOR  

                 ////writeFileLog"[cancelTran_Ver2()] Before wait for note acceptor cancel status signal."); 
                 AVRM_Currency_writeFileLog("[cancelTran_Ver2()] Before wait for note acceptor cancel status signal.",INFO);
                
                 if(true == NAThreadStatus) {

                       //++pthread_mutex_lock( &g_NACompleteThreadmutex );
                       //++pthread_cond_wait( &g_NACompleteThreadCond , &g_NACompleteThreadmutex );
                       //++pthread_mutex_unlock( &g_NACompleteThreadmutex );

                       while(1){
						   
		                           pthread_mutex_lock(&g_NAComplteOperationmutex );

		                           if ( true == g_NAComplteOperation ){
		                                  pthread_mutex_unlock(&g_NAComplteOperationmutex );
                                          break;
                                   }//if end

                                   pthread_mutex_unlock(&g_NAComplteOperationmutex );

                       }//++while end

                 } //if end

                 ////writeFileLog"[cancelTran_Ver2()] After get note acceptor cancel status signal."); 
                 
                 AVRM_Currency_writeFileLog("[cancelTran_Ver2()] After get note acceptor cancel status signal.",INFO);

                 #endif

                 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 #ifdef COIN_ESCROW
                 pthread_mutex_lock( &g_CACancelStatusmutex );
                 #endif
                 #ifdef NOTE_ACCEPTOR
                 pthread_mutex_lock( &g_NACancelStatusmutex );
                 #endif
                  
                 if( (g_ttlNmbrOfNote>0) && (g_maxNoOfCash>0) ) {
					 
					 memset(log,'\0',100);
					 sprintf(log,"[cancelTran_Ver2] g_NACancelStatus : %d .",g_NACancelStatus);
					 AVRM_Currency_writeFileLog(log,INFO);
					 
                 }//++if end
                 
                 if( (g_ttlNmbrOfCoin>0) && (g_maxNoOfCoin>0) ){
					 
					 memset(log,'\0',100);
					 sprintf(log,"[cancelTran_Ver2] g_CACancelStatus : %d .",g_CACancelStatus);
					 AVRM_Currency_writeFileLog(log,INFO);
					 
                 }//++if end
                 
                 
                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
                 /*if( (g_ttlNmbrOfNote>0) && (g_maxNoOfCash>0) ) {
					cancelrtcode1 = g_NACancelStatus;
			     }else{
					 cancelrtcode1 = RETURNED_NOTE_COIN;
				 }//else end
                 
                 if( (g_ttlNmbrOfCoin>0) && (g_maxNoOfCoin>0) ){
					cancelrtcode2 = g_CACancelStatus; 
				 }else{
					cancelrtcode2 = RETURNED_NOTE_COIN; 
				 }//else end
                 
                 if( RETURNED_NOTE_MOUTH_BLOCKED == cancelrtcode1 ){
					 cancelrtcode1 = RETURNED_NOTE_COIN;
					 g_NACancelStatus = RETURNED_NOTE_COIN;
				 }//if end
				 */
				 
				  memset(log,'\0',100);
				  sprintf(log,"[cancelTran_Ver2] g_NACancelStatus : %d .",g_NACancelStatus);
				  AVRM_Currency_writeFileLog(log,INFO);
				  
				  memset(log,'\0',100);
				  sprintf(log,"[cancelTran_Ver2] g_CACancelStatus : %d .",g_CACancelStatus);
				  AVRM_Currency_writeFileLog(log,INFO);
					 
				  cancelrtcode1 = g_NACancelStatus;
				  cancelrtcode2 = g_CACancelStatus; 
				 
				 
				 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				 if( ( RETURNED_NOTE_COIN == cancelrtcode1) || (RETURNED_NOTE_COIN == cancelrtcode2)  )  //++Return Successfully
                 {
                        cancelrtcode = RETURNED_NOTE_COIN;
                        pthread_mutex_lock(&mut);
                        g_ttlNmbrOfNote=0;
                        g_ttlNmbrOfCoin=0;
                        pthread_mutex_unlock(&mut);
		                AVRM_Currency_writeFileLog("[cancelTran_Ver2() Exit] finished with success.",INFO);
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with success status.",g_total_transaction);
                        AVRM_Currency_writeFileLog(log,INFO);
                       
                 }
                 else if( (OPERATION_TIMEOUT_OCCURRED == cancelrtcode1) ||  (OPERATION_TIMEOUT_OCCURRED == cancelrtcode2) ) //++Timeout occured
                 {
                        cancelrtcode = OPERATION_TIMEOUT_OCCURRED;
                        ////writeFileLog"[cancelTran_Ver2() Exit] finished with timeout occured.");
                        AVRM_Currency_writeFileLog("[cancelTran_Ver2() Exit] finished with timeout occured.",INFO);
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with failed status.",g_total_transaction);
                        AVRM_Currency_writeFileLog(log,INFO);
                       
		         }
                 else if( COMMUNICATION_FAILURE_NOTE_ACCEPTOR == cancelrtcode1 ) //++Communication failed occured Note
                 {
                        cancelrtcode = COMMUNICATION_FAILURE_NOTE_ACCEPTOR;
                        AVRM_Currency_writeFileLog("[cancelTran_Ver2() Exit] finished with Communication failed.",INFO);
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with failed status.",g_total_transaction);
                        AVRM_Currency_writeFileLog(log,INFO);
		         }
		         else if( COMMUNICATION_FAILURE_COIN_ACCEPTOR == cancelrtcode2  ) //++Communication failed occured Coin
                 {
                        cancelrtcode = COMMUNICATION_FAILURE_COIN_ACCEPTOR;
                        AVRM_Currency_writeFileLog("[cancelTran_Ver2() Exit] finished with Communication failed.",INFO);
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with failed status.",g_total_transaction);
                        AVRM_Currency_writeFileLog(log,INFO);
                        
		         }
		         else if( RETURNED_NOTE_MOUTH_BLOCKED == cancelrtcode1  ) //++Return Mouth blocked
                 {
					    cancelrtcode = RETURNED_NOTE_MOUTH_BLOCKED;
                        AVRM_Currency_writeFileLog("[cancelTran_Ver2() Exit] finished with Note Return Mouth blocked.",INFO);
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with failed status.",g_total_transaction);
                        AVRM_Currency_writeFileLog(log,INFO);
		         }
		         else if( -3 == cancelrtcode1  ) //++No Notes in stacker
                 {
                        cancelrtcode = 31; //++Other error
                        AVRM_Currency_writeFileLog("[cancelTran_Ver2() Exit] finished with no notes in stacker.",INFO);
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with failed status.",g_total_transaction);
                        AVRM_Currency_writeFileLog(log,INFO);
		         }
                 else if( (OTHER_ERROR == cancelrtcode1) || (OTHER_ERROR == cancelrtcode2)  ) //++Any other exception
                 {
                        cancelrtcode = OTHER_ERROR; //++Other error
                        AVRM_Currency_writeFileLog("[cancelTran_Ver2() Exit] finished with Any other exception.",INFO);
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with failed status.",g_total_transaction);
                        AVRM_Currency_writeFileLog(log,INFO);
               
		         }//else if end
                
                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                
                 #ifdef COIN_ESCROW
	             pthread_mutex_unlock( &g_CACancelStatusmutex );
                 #endif

                 #ifdef NOTE_ACCEPTOR
                 pthread_mutex_unlock( &g_NACancelStatusmutex );
                 #endif

                 return cancelrtcode;
                 
      } else  {
           
           pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	       AVRM_Currency_writeFileLog("[cancelTran_Ver2()] No acceptfare thread stated so canceltrans() return fail status.",INFO);
	       return (31); //Any Other exception status

      }//++else end
	 
}//cancelTran_Ver2() end
static pthread_mutex_t  g_getDoorstatusprocmutex 		      = PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Accept Transaction Version 1
bool commitTran() {
	
 
         
         //MODULE:Finally accept all cash,coin from escrow to cst.
         int value, noteStatus;
         unsigned int totalnotes=0;
	 int retry;
	 int ret, ret1, ret2,ret3; 
	 ret1 = FAIL; 
	 ret2 = FAIL; 
	 ret3 = FAIL; 
	 ret  = FAIL;
         int rtcode=0;
         unsigned int try=1;
         unsigned char log[100];
         memset(log,'\0',100); 
         //writeFileLog"[commitTran()] Commit Transaction issue by system......");
         
         bool already_unlock=false;
    
         pthread_mutex_lock(&AcptFareThreadStatusmutex);

         //if all acpt fare thread not stop then stop them before doing commit trans
	 if(THREAD_START==g_AcptFareThreadStatus)
	 {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);

	      //Malay Add 19 Jan 2013
	      //writeFileLog"[commitTran()] Attempt to kill all running threads.");

	      rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);

              already_unlock=true;                                              

	      //writeFileLog"[commitTran()] after return KillAllRunningThread().");


	 }
         
         if( false == already_unlock )
         {

               pthread_mutex_unlock(&AcptFareThreadStatusmutex);

         }

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	 if(THREAD_CLOSE==g_AcptFareThreadStatus)
         {
                  
                  pthread_mutex_unlock(&AcptFareThreadStatusmutex);

                  sprintf(log,"[commitTran()] Total Coins : %d",g_ttlNmbrOfCoin);
                  //writeFileLoglog);
                  memset(log,'\0',100);
                  sprintf(log,"[commitTran()] Total Notes : %d",g_ttlNmbrOfNote);
                  //writeFileLoglog);  

                  

                  #ifdef COIN_ESCROW
                  
                  //writeFileLog"[commitTran()] Before Accept Coin.");
                  
		  if(g_ttlNmbrOfCoin>0)
		  {
                       
                      //clear coin number
                      g_ttlNmbrOfCoin=0;
            

                      //if coin acceptor low level flag is closed then do operation (1 means off and 0 means on)
                      if(1==WaitforCoinAcptrLowlevelflagClosed(WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD))
                      {

                               Wait(2);

		               for(try=1;try<=4;try++)
		               {      
 
                                       ret1 = AcceptFromCoinEscrow();

		                       if( SUCCESS == ret1 )
		                       {
				                   //writeFileLog"[commitTran()] Accept Coin Successfully.");
				                   memset(log,'\0',100); 
				                   sprintf(log,"[commitTran()]  try: %d.",try);
				                   //writeFileLoglog);
				                   break;
		                       }
		                       else
		                       {
				                   //writeFileLog"[commitTran()] Accept Coin failed.");
				                   memset(log,'\0',100); 
				                   sprintf(log,"[commitTran()]  try: %d.",try);
				                   //writeFileLoglog);
				                   continue;
		                       }

                               }
                      }
                      else
                      {
                               //writeFileLog"[commitTran()] Coin Acceptor Low Level flag is not closed so no accept escrow can be done now!!. ");
                               ret1=FAIL;
                       }

                       

		  } 
		  else
                  {
		      ret1=SUCCESS;

                  }

                  //writeFileLog"[commitTran()] After Accept Coin.");
                  
                  #endif
               
                  #ifndef COIN_ESCROW
                  ret1=SUCCESS;
                  #endif

                  #ifdef NOTE_ACCEPTOR  
                               
                  //writeFileLog"[commitTran()] Before Accept Notes.");

		  if(g_ttlNmbrOfNote>0)
		  {  
                         
                            //store global note number
                            totalnotes=g_ttlNmbrOfNote;
                            g_ttlNmbrOfNote=0;
  
                            
                             
                            #ifdef JCM_NOTE_ACCEPTOR

		            if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
		            {

                                     //Step 1: Wait for if any note rejected by note acceptor is not taken out by customer
                                     //Step 2: then calculate recycle quanity
                                     //Step 3: then issue unload/dispense command
                                     //Step 4: calculate recycle quanity after operation
                                     //Step 5: if equal to dispensed/unloaded quanity then exit or not eqaul go to retry

                                     int CurrentCastQuanity=0,AfterCastQuanity=0;
                                     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
                                     memset(log,'\0',100);
                                     sprintf(log,"[commitTran()] JCM Before Unload Recycle Quantity = %d.",CurrentCastQuanity);
                                     //writeFileLoglog);
                                     int rtcode=-1;

                                     //writeFileLog"[commitTran()] Before Unload Operation Calling.");

                                     for(try=1;try<=2;try++)
                                     {
		                            
		                                   //unload all notes from jcm escrow
		                                   
		                                   rtcode=UnloadCashAcptr();
		                                  
		                                   if(1 == rtcode)
		                                   {

		                                           //writeFileLog"[commitTran()] Unload successfully executed.");
		                                           break;

		                                   }
		                                   else
		                                   {
                                                           //writeFileLog"[commitTran()]  Unload Operation failed.");
		                                           memset(log,'\0',100); 
						           sprintf(log,"[commitTran()]  retry: %d.",try);
						           //writeFileLoglog);
                                                           delay_mSec(200);
                                                           rtcode=-1;
		                                           continue;

		                                   }


                                     }

                                     //writeFileLog"[commitTran()] After Unload Operation Calling.");

                                      
                                     //writeFileLog"[commitTran()] After Unload Operation now going to read jcm escrow notes number.");

                                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );


                                     memset(log,'\0',100);

                                     sprintf(log,"[commitTran()] JCM After Unload Recycle Quantity = %d.",AfterCastQuanity );

                                     //writeFileLoglog);

                                     if(1 == rtcode)
                                       ret3=SUCCESS;
                                     else
                                       ret3=FAIL;

		            }

                            #endif
                            
                            #ifdef BNA_NOTE_ACCEPTOR

		            if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
		            {

                                     //Step 1: Wait for if any note rejected by note acceptor is not taken out by customer
                                     //Step 2: then calculate recycle quanity
                                     //Step 3: then issue unload/dispense command
                                     //Step 4: calculate recycle quanity after operation
                                     //Step 5: if equal to dispensed/unloaded quanity then exit or not eqaul go to retry


                                     
                                     int CurrentCastQuanity=0,AfterCastQuanity=0;

                                     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );

                                     memset(log,'\0',100);
                                     sprintf(log,"[commitTran()] BNA Before Unload Recycle Quantity = %d.",CurrentCastQuanity);
                                     //writeFileLoglog);
                                     int rtcode=-1;

                                     //writeFileLog"[commitTran()] Before Unload Operation Calling.");

                                     for(try=1;try<=2;try++)
                                     {
		                            
		                                   //unload all notes from jcm escrow
		                                   
		                                   rtcode=UnloadCashAcptr();
		                                  
		                                   if(1 == rtcode)
		                                   {

		                                           //writeFileLog"[commitTran()] Unload successfully executed.");
		                                           break;

		                                   }
		                                   else
		                                   {
                                                           //writeFileLog"[commitTran()]  Unload Operation failed.");
		                                           memset(log,'\0',100); 
						           sprintf(log,"[commitTran()]  retry: %d.",try);
						           //writeFileLoglog);
                                                           delay_mSec(200);
                                                           rtcode=-1;
		                                           continue;

		                                   }


                                     }

                                     //writeFileLog"[commitTran()] After Unload Operation Calling.");

                                      
                                     //writeFileLog"[commitTran()] After Unload Operation now going to read bna escrow notes number.");

                                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );


                                     memset(log,'\0',100);

                                     sprintf(log,"[commitTran()] BNA After Unload Recycle Quantity = %d.",AfterCastQuanity );

                                     //writeFileLoglog);

                                     if(1 == rtcode)
                                       ret3=SUCCESS;
                                     else
                                       ret3=FAIL;
                                   
 
		            }

                            #endif
                                                         
 
                            #ifdef B2B_NOTE_ACCEPTOR

                             if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
                             {
                                        

                                        B2BFinishRejectingState();

                                        delay_mSec(200);

                                        unsigned int Currenttotalnotes=0;

                                        Currenttotalnotes=CalculateCurrentBillQuantity();
                                       
                                        //delay_mSec(200);
                                       
                                        //Wait(WAIT_BEFORE_TRANS_END);

                                        //writeFileLog"[commitTran()] Before AcceptFromEscrow().");

                                        for(try=1;try<=4;try++)
                                        {  
                                             
                                             
                                             //start b2b unloading state
                                             ret2 = AcceptFromEscrow(1,g_ttlNmbrOfNote); 

                                             if( SUCCESS == ret2 )  
                                             {

                                                  //writeFileLog"[commitTran()] AcceptFromEscrow() return with success.");
                                                  //writeFileLog"[commitTran()] Now no retry needed.");
                                                  //writeFileLog"[commitTran()] Before GetAcceptedNoteDetail().");
                                                  for(try=1;try<=2;try++)
                                                  {
                                                     
                                                     //wait for b2b unloaded state and return when b2b finish with disabled state
                                                     ret3 = GetAcceptedNoteDetail(&value,&noteStatus); 

                                                     if(SUCCESS == ret3)
                                                     {
                                                          //writeFileLog"[commitTran()] GetAcceptedNoteDetail() return with success."); 
                                                          //writeFileLog"[commitTran()] No retry needed."); 

                                                          break;

                                                      }
                                                      else
                                                      {
                                                           //writeFileLog"[commitTran()] GetAcceptedNoteDetail() return with fail."); 
                                                           memset(log,'\0',100);
                                                           sprintf(log,"[commitTran()] Retry due to fail return %d.",try);
                                                           //writeFileLoglog);
                                                           continue;
                                                      }

                                                    }//internal foor loop end
                                                    if(SUCCESS == ret3){
                                                        //writeFileLog"[commitTran()] GetAcceptedNoteDetail() return with success.");
                                                    }else{
                                                        //writeFileLog"[commitTran()] GetAcceptedNoteDetail() return with fail.");
													}
                                                    //writeFileLog"[commitTran()] After GetAcceptedNoteDetail().");
                                                    break;
                                                 }
                                                 else
                                                 {
                                                     memset(log,'\0',100);
                                                     sprintf(log,"[commitTran()] AcceptFromEscrow() return with fail %d .",try);
                                                     //writeFileLoglog);  
                                                     continue;
                                                 }

                                              }//external foor loop end

                                              //writeFileLog"[commitTran()] After AcceptFromEscrow().");

                                              //clear global note number
                                              totalnotes=g_ttlNmbrOfNote;
                                              g_ttlNmbrOfNote=0;

						
                                 }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
                                 else
                                 {
                                    //writeFileLog"[commitTran()] Note Acceptor low level thread is not starting so no accept escrow can be done now!!.");
                                    ret3=FAIL;
                                 }
                                 #endif

                                 

		  }//if(g_ttlNmbrOfNote>0) block

		  else
                  {

		        ret3=SUCCESS;

                  }
                  
                  //writeFileLog"[commitTran()] After Accept Notes.");
                  

                  #endif

                 #ifndef NOTE_ACCEPTOR
                  ret3=SUCCESS;
                 #endif
                   
                   #if defined(B2B_TRAP_GATE)

                   
		    //Start:Malay 1 Jan 2013 add
		    //Store Note Counts in global variable and drop it in big cashbox if threshold value cross
		   
                   //writeFileLog"[commitTran()] Before Drop notes.");
		                    
                   
		   //Store Notes accepted in each transactions
		   
                   g_NoteCounter=g_NoteCounter+totalnotes;
                   
                   memset(log,'\0',100);
                   sprintf(log,"[commitTran()] Still Accepted Notes = %d .",g_NoteCounter);
                   //writeFileLoglog);
                   memset(log,'\0',100);
                   sprintf(log,"[commitTran()] Threshold value of Notes = %d .",g_DropNotesNumber);
                   //writeFileLoglog);

		   //Check if it greater than or equal to notes drop threshold value if greater found drop it
		   if(g_NoteCounter >= g_DropNotesNumber)
		   {
			    //Drop Notes
                            int rtcode=-1;
			    rtcode=DropNotes();
			    if(1==rtcode || 4==rtcode) //Successfully drop notes
		            {
			       //writeFileLog"[commitTran()] Drop notes successfully.");
		               g_NoteCounter=0;
		            }
		            else
                            {
		               //writeFileLog"[commitTran()] Drop notes failed.");
                            }
                            
                            
		   }
                   else
                      //writeFileLog"[commitTran()] Threshold value not match so no drop note operation canbe done now.");

                   //writeFileLog"[commitTran()] After Drop notes.");

		  #endif

                  
                  if( ( SUCCESS == ret1 ) && ( SUCCESS == ret3 ) )
                  {
		              //writeFileLog"[commitTran()] Return With Success Status.");
                              memset(log,'\0',100);
		              sprintf(log,"[commitTran()] Transaction Number %d committed With Success Status.",g_total_transaction);
		              //writeFileLoglog);
		              return true;
                  }
		  else
                  {

			      //writeFileLog"[commitTran()] Return With Fail Status.");
                              memset(log,'\0',100);
		              sprintf(log,"[commitTran()] Transaction Number %d not committed With Fail Status.",g_total_transaction);
		              //writeFileLoglog);
		              return false;

                  }
                  
                  
		  
      }//if(THREAD_CLOSE==g_AcptFareThreadStatus) block 

      else 
      {
              pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	      //writeFileLog"[commitTran()] No acceptfare thread stated so committrans() return fail status.");
	      return false;
      }


 
}//commitTran() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Reject Transaction Version 1
bool cancelTran() {
	

	 int value, noteStatus;
	 int retry=1;
	 int ret1 , ret2, ret3,ret,rtcode=0;
         ret1 = FAIL; 
	 ret2 = FAIL; 
	 ret3 = FAIL;
	 ret  = FAIL;
         unsigned char log[100];
         memset(log,'\0',100);

 
         bool already_unlock=false;

	 //if all acpt fare thread not stop then stop them before doing cancel trans

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	 if( THREAD_START== g_AcptFareThreadStatus )
	 {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);
     
              already_unlock=true;

	      //Malay Add 19 Jan 2013
	      //writeFileLog"[cancelTran()] Cancel Transaction issue by system...");

	      rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);

	      //writeFileLog"[cancelTran()] after return KillAllRunningThread().");


	 }

         if( false == already_unlock )
         {

               pthread_mutex_unlock(&AcptFareThreadStatusmutex);

         }

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	 if(THREAD_CLOSE==g_AcptFareThreadStatus)
         {
                 
                  pthread_mutex_unlock(&AcptFareThreadStatusmutex);

                  sprintf(log,"[cancelTran()] Total Coins : %d",g_ttlNmbrOfCoin);
                  //writeFileLoglog);
                  memset(log,'\0',100);
                  sprintf(log,"[cancelTran()] Total Notes : %d",g_ttlNmbrOfNote);
                  //writeFileLoglog);

		  //Malay Add 19 Jan 2013
                  #ifdef COIN_ESCROW

		  //writeFileLog"[cancelTran()] before reject coin.");

		  if(g_ttlNmbrOfCoin>0)
		  {  

                         //clear global coin
                         g_ttlNmbrOfCoin = 0;

                         

                         if(1==WaitforCoinAcptrLowlevelflagClosed(WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD))
                         {

                               Wait(2);

                               for(retry = 1; retry<=4;retry++)  
			       {

		                       ret1 = RejectFromCoinEscrow(); 

		                       if(SUCCESS == ret1)
                                       {

				                   //writeFileLog"[cancelTran()] Reject Coin Successfully.");
		                                   memset(log,'\0',100); 
		                                   sprintf(log,"[cancelTran()]  Retry: %d.",retry);
		                                   //writeFileLoglog);
		                                   break;
                                       }
		                       else
                                       {
				                  //writeFileLog"[cancelTran()]  Reject Coin failed.");
		                                  memset(log,'\0',100); 
		                                  sprintf(log,"[cancelTran()]  Retry: %d.",retry);
		                                  //writeFileLoglog);
		                                  continue;

                                       }

                                }

			 }
                         else
                         {
                               //writeFileLog"[cancelTran()] Coin Acceptor Low Level flag is not closed so no reject escrow can be done now!!.");
                               ret1=FAIL;
                         }
                           
                         
                         

		  }
		  else
		   ret1=SUCCESS;
                  
		  //writeFileLog"[cancelTran()] after reject coin.");
                  #endif

                  #ifndef COIN_ESCROW
                  ret1=SUCCESS;
                  #endif
                 
		  #ifdef NOTE_ACCEPTOR

		  //writeFileLog"[cancelTran()] before reject notes.");

		  if(g_ttlNmbrOfNote>0)
		  {  
                      

                       #ifdef JCM_NOTE_ACCEPTOR

		       if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
		       {
                                     
                                     //Step 1: Wait for if any note rejected by note acceptor is not taken out by customer
                                     //Step 2: then calculate recycle quanity
                                     //Step 3: then issue unload/dispense command
                                     //Step 4: calculate recycle quanity after operation
                                     //Step 5: if equal to dispensed/unloaded quanity then exit or not eqaul go to retry
		                     

                                                          
                                     int CurrentCastQuanity=0,AfterCastQuanity=0;
                                     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
                                     memset(log,'\0',100);
                                     sprintf(log,"[cancelTran()] JCM Before Dispense Recycle Quantity = %d.",CurrentCastQuanity);
                                     //writeFileLoglog);
                                     int rtcode=-1,try=1;

                                     //writeFileLog"[cancelTran()] Before Dispense Operation Calling.");
                                 
                                     for(try=1;try<=2;try++)
                                     {
		                            
                                           //dispense all notes
                                           
                                           rtcode=DispenseCashAcptr();

                                           if( (1 == rtcode) || (4 == rtcode) )
                                           { 

                                                 //writeFileLog"[cancelTran()] Dispense Operation successfully executed.");
                                                 break;

                                           }
                                           else
                                           {

                                                  //writeFileLog"[cancelTran()]  Dispense notes failed.");
		                                  memset(log,'\0',100); 
		                                  sprintf(log,"[cancelTran()]  Retry: %d.",try);
		                                  //writeFileLoglog);
                                                  delay_mSec(200);
                                                  rtcode=-1;
                                                  continue;

                                            }



                                     }

                                     //writeFileLog"[cancelTran()] After Dispense Operation Calling.");

                                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );

                                     memset(log,'\0',100);

                                     sprintf(log,"[cancelTran()] JCM After Dispensed Recycle Quantity = %d.",AfterCastQuanity );

                                     //writeFileLoglog);

                                     if( (1 == rtcode)|| (4 == rtcode) )
                                       ret3=SUCCESS;
                                     else
                                       ret3=FAIL;



		       }

                       #endif

                       #ifdef BNA_NOTE_ACCEPTOR

		       if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
		       {
                                     
                                     //Step 1: Wait for if any note rejected by note acceptor is not taken out by customer
                                     //Step 2: then calculate recycle quanity
                                     //Step 3: then issue unload/dispense command
                                     //Step 4: calculate recycle quanity after operation
                                     //Step 5: if equal to dispensed/unloaded quanity then exit or not eqaul go to retry

		                     int CurrentCastQuanity=0,AfterCastQuanity=0,rtcode=0;

                                     //writeFileLog"[cancelTran()] Before Wait for any reject state completion.");

                                     rtcode=WaitforNoteTakenBackByUser(3600); //1hr

                                     //writeFileLog"[cancelTran()] After Wait for any reject state completion.");

                                     if(1 == rtcode )
                                     {
                                            
                                   
				                     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );

				                     memset(log,'\0',100);

				                     sprintf(log,"[cancelTran()] BNA Before Dispense Recycle Quantity = %d.",CurrentCastQuanity);
				                     //writeFileLoglog);

				                     int rtcode=-1,try=1;

				                     //writeFileLog"[cancelTran()] Before Dispense Operation Calling.");
				                 
				                     for(try=1;try<=2;try++)
				                     {
						            
				                           //dispense all notes
				                           
				                           rtcode=DispenseCashAcptr();

				                           if( (1 == rtcode) )
				                           { 

				                                 //writeFileLog"[cancelTran()] Dispense Operation successfully executed.");
				                                 break;

				                           }
				                           else
				                           {

				                                  //writeFileLog"[cancelTran()]  Dispense notes failed.");
						                  memset(log,'\0',100); 
						                  sprintf(log,"[cancelTran()]  Retry: %d.",try);
						                  //writeFileLoglog);
				                                  delay_mSec(200);
				                                  rtcode=-1;
				                                  continue;

				                            }



				                     }

				                     //writeFileLog"[cancelTran()] After Dispense Operation Calling.");

                                                     delay_mSec(200);

				                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );

				                     memset(log,'\0',100);

				                     sprintf(log,"[cancelTran()] BNA After Dispensed Recycle Quantity = %d.",AfterCastQuanity );

				                     //writeFileLoglog);

				                     if( (1 == rtcode) )
				                       ret3=SUCCESS;
				                     else
				                       ret3=FAIL;


                                    }//if(1 == rtcode ) block
                                   
                                    

		       }

                       #endif

                       #ifdef B2B_NOTE_ACCEPTOR
                       if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
                       {
		               
                               B2BFinishRejectingState();

                               delay_mSec(200);

                               unsigned int totalnotes=0;
                               totalnotes=CalculateCurrentBillQuantity();

                               //delay_mSec(200);
                               
                               //Wait(WAIT_BEFORE_TRANS_END);

                               for(retry = 1; retry<=4;retry++)  
			       {

					 //writeFileLog"[cancelTran()] before reject from escrow.");

                                         ret2 = RejectFromEscrow(24,g_ttlNmbrOfNote);

                                         //ret2 = RejectFromEscrow(24,totalnotes);

                                         if( SUCCESS == ret2 )
                                         {
                                                //writeFileLog"[cancelTran()] RejectFromEscrow() return with success.");
                                                //writeFileLog"[cancelTran()] Now no need to retry here.");
                                                //writeFileLog"[cancelTran()] before GetReturnDetail().");
                                                for(retry = 1; retry<=2;retry++) 
                                                {
							ret3 = GetReturnDetail(&value,&noteStatus);
		                                        if( SUCCESS == ret3 )  
                                                        {
		                                           //writeFileLog"[cancelTran()] GetReturnDetail() return with success status.");
                                                           //writeFileLog"[cancelTran()] Now no retry here.");
                                                           break;
                                                        }
		                                        else
                                                        {
		                                           //writeFileLog"[cancelTran()] GetReturnDetail() return with fail status.");
                                                           memset(log,'\0',100); 
                                                           sprintf(log,"[cancelTran()] GetReturnDetail() Retry: %d.",retry);
                                                           //writeFileLoglog);
                                                           continue; 
                                                        }
                                                 }//internal for loop end
                                                 //writeFileLog"[cancelTran()] after GetReturnDetail().");
                                                 break;
                                         }
                                         else
                                         {
                                               //writeFileLog"[cancelTran()] RejectFromEscrow() return with fail.");
                                               memset(log,'\0',100); 
                                               sprintf(log,"[cancelTran()] RejectFromEscrow() Retry: %d.",retry);
                                               //writeFileLoglog);
                                               continue;

                                         }


                                   }//external for loop end

                                   
                                   
                                   //writeFileLog"[cancelTran()] after reject from escrow.");

                                   //clear global note number
                                   g_ttlNmbrOfNote=0;

                            
                         }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
                         else
                         {
                                //writeFileLog"[cancelTran()] Note Acceptor low level thread is not closed so no reject escrow can be done now!!.");
                                ret3=FAIL;
                         }

                         #endif

                    
		  } //if(g_ttlNmbrOfNote>0) block
		  else
                  {

		      ret3=SUCCESS;
                      
                      #ifdef BNA_NOTE_ACCEPTOR

                      

                      if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
		      {

                                     

                                     int rtcode=0;

                                     //writeFileLog"[cancelTran()] Before Wait for any reject state completion.");

                                     rtcode=WaitforNoteTakenBackByUser(1);

                                     if( 1 != rtcode )
                                     {

                                            //writeFileLog"[cancelTran()] Before Going to close reject state operation.");

                                            StopRejectProcess();

                                            //writeFileLog"[cancelTran()] After Going to close reject state operation.");
  

                                     }

                                     //writeFileLog"[cancelTran()] After Wait for any reject state completion.");

                                     //writeFileLog"[cancelTran()] Before  close all BNA CashIn Process.");      
    
                                     CompleteCashInProcess(); //++

                                     //writeFileLog"[cancelTran()] After  close all BNA CashIn Process."); 

                                     


                      }

                      

                      #endif
                      

                  }
                  //writeFileLog"[cancelTran()] after reject notes.");

                  #endif

                 #ifndef NOTE_ACCEPTOR
                 ret3=SUCCESS;
                 #endif

		  if(( SUCCESS == ret1 ) && ( SUCCESS == ret3 ))
                  {

		        //writeFileLog"[canceltrans()] finished with success.");
                        memset(log,'\0',100); 
                        sprintf(log,"[canceltrans()] Transaction Number %d canceled with success status.",g_total_transaction);
                        //writeFileLoglog);
                        return true;

                  }
		  else
                  {
		        
                        //writeFileLog"[canceltrans()] finished with failed.");
                        memset(log,'\0',100); 
                        sprintf(log,"[canceltrans()] Transaction Number %d canceled with failed status.",g_total_transaction);
                        //writeFileLoglog);
                        return false;

		  
		  }
		  
                  
		  
      
      }//if(THREAD_CLOSE==g_AcptFareThreadStatus) block
      else 
      {
           pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	   //writeFileLog"[cancelTran()] No acceptfare thread stated so canceltrans() return fail status.");
	   return false;
      }
	 


}//cancelTran() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
#ifdef B2B_NOTE_ACCEPTOR

int EnableTheseDenomination( int DenomMask,int Timeout) {
   
           

            g_NOTE.rs_5      = DISABLE;
            g_NOTE.rs_10     = DISABLE;
            g_NOTE.rs_20     = DISABLE;
            g_NOTE.rs_50     = DISABLE;
            g_NOTE.rs_100    = DISABLE;
            g_NOTE.rs_500    = DISABLE;
            g_NOTE.rs_1000   = DISABLE;

            char log[200];
		          
            memset(log,'\0',200);

            sprintf(log,"[EnableTheseDenomination()] Mask value : 0x%x h",DenomMask);

            //writeFileLoglog);

            int localDenomMask =  DenomMask&0x7F ;


            memset(log,'\0',200);

            sprintf(log,"[EnableTheseDenomination()] After local Mask value : 0x%x h",localDenomMask);

            //writeFileLoglog);
            
            if( 0x01 == (localDenomMask&0x01) )
            {
               g_NOTE.rs_5      = ENABLE;
               //writeFileLog"[EnableTheseDenomination()] Rs.5 Enable");
                 
            }
           
            if( 0x02 == (localDenomMask&0x02) )
            {
               g_NOTE.rs_10      = ENABLE;
               //writeFileLog"[EnableTheseDenomination()] Rs.10 Enable");
            }
 
            if( 0x04 == (localDenomMask&0x04) )
            {
               g_NOTE.rs_20      = ENABLE;
               //writeFileLog"[EnableTheseDenomination()] Rs.20 Enable");
            }


            if( 0x08 == (localDenomMask&0x08) )
            {
               g_NOTE.rs_50      = ENABLE;
               //writeFileLog"[EnableTheseDenomination()] Rs.50 Enable");
                 
            }

            if( 0x10 == (localDenomMask&0x10) )
            {
               g_NOTE.rs_100      = ENABLE;
               //writeFileLog"[EnableTheseDenomination()] Rs.100 Enable");
                 
            }

            if( 0x20 == (localDenomMask&0x20) )
            {
               g_NOTE.rs_500     = ENABLE;
               //writeFileLog"[EnableTheseDenomination()] Rs.500 Enable");
                 
            }

            if( 0x40 == (localDenomMask&0x40) )
            {
               g_NOTE.rs_1000     = ENABLE;
               //writeFileLog"[EnableTheseDenomination()] Rs.1000 Enable");
                 
            }

            SetDenomMaskValue(DenomMask);

            pthread_mutex_lock(&g_APIEnableTheseDenominationsmutex );

            g_APIEnableTheseDenominationsFlag = true;

            pthread_mutex_unlock(&g_APIEnableTheseDenominationsmutex );

            return 1;

}//int EnableTheseDenomination( int DenomMask,int Timeout)
#endif
*/ 

//Byte 0:
//0 0 0 0 0 0 0 1  : 0x01 : Rs 5
//0 0 0 0 0 0 1 0  : 0x02 : Rs 10
//0 0 0 0 0 1 0 0  : 0x04 : Rs 20
//0 0 0 0 1 0 0 0  : 0x08 : Rs 50
//0 0 0 1 0 0 0 0  : 0x10 : Rs 100
//0 0 1 0 0 0 0 0  : 0x20 : Rs 200
//0 1 0 0 0 0 0 0  : 0x40 : Rs 500
//1 0 0 0 0 0 0 0  : 0x80 : Rs 1000
//Byte 1:
//0 0 0 0 0 0 0 1  : 0x01 : Rs 2000
//Byte 2:
//0 0 0 0 0 0 0 1  : 0x01 : Rs 1
//0 0 0 0 0 0 1 0  : 0x02 : Rs 2
//0 0 0 0 0 1 0 0  : 0x04 : Rs 5
//0 0 0 0 1 0 0 0  : 0x08 : Rs 10

//All Note and Coin Enable
//11110000000111111111
//983551

int EnableTheseDenomination( int CurrencyType ,int DenomMask,int Timeout) 
{
   
            char log[200];
            char NoteDenomMask   =  0x00,
				 NoteDenomMask2  =  0x00;
            char CoinDenomMask   =  0x00 ;
            
            int tempMask    = DenomMask&0x000000ff ;
            
            //++Byte 0
            NoteDenomMask   = tempMask ;     //++most significant bytes  of Lower Firstbyte [Right] 
            
            //++Byte 1
            NoteDenomMask2   = (DenomMask&0x00000100)>>8 ;     //++most significant bytes  of Lower Firstbyte [Right] 
            
            //++Byte 2
            CoinDenomMask  = (DenomMask&0x00ff0000)>>16;//++lower significant bytes of Lower 2byte [Left]  Byte 0
            
            memset(log,'\0',200);
		    sprintf(log,"[EnableTheseDenomination()] CurrencyType : %d",CurrencyType);
            AVRM_Currency_writeFileLog(log,INFO);
            
            memset(log,'\0',200);
		    sprintf(log,"[EnableTheseDenomination()] API DenomMask : 0x%xh",DenomMask);
            AVRM_Currency_writeFileLog(log,INFO);
            
            memset(log,'\0',200);
		    sprintf(log,"[EnableTheseDenomination()] Note DenomMask : 0x%xh",(unsigned char)NoteDenomMask);
            AVRM_Currency_writeFileLog(log,INFO);
            
            memset(log,'\0',200);
			sprintf(log,"[EnableTheseDenomination()] Note DenomMask2 Value : 0x%x h",(unsigned char)NoteDenomMask2);
            AVRM_Currency_writeFileLog(log,INFO);
           
            memset(log,'\0',200);
		    sprintf(log,"[EnableTheseDenomination()] Coin DenomMask : 0x%xh",CoinDenomMask);
            AVRM_Currency_writeFileLog(log,INFO);
            
            //++Note Acceptor Device
            if( ( 1 ==  CurrencyType ) || (0 ==  CurrencyType) ) 
            {
				
                    #ifdef NOTE_ACCEPTOR 
                    
                    g_NOTE.rs_5      = 0;
					g_NOTE.rs_10     = 0;
					g_NOTE.rs_20     = 0;
					g_NOTE.rs_50     = 0;
					g_NOTE.rs_100    = 0;
					g_NOTE.rs_200    = 0;
					g_NOTE.rs_500    = 0;
					g_NOTE.rs_1000   = 0;
					g_NOTE.rs_2000   = 0;
					
					memset(log,'\0',200);
					sprintf(log,"[EnableTheseDenomination()] Mask value : 0x%x h",DenomMask);
                    AVRM_Currency_writeFileLog(log,INFO);
                    
                    memset(log,'\0',200);
					sprintf(log,"[EnableTheseDenomination()] Note DenomMask1 Value : 0x%x h",(unsigned char)NoteDenomMask);
                    AVRM_Currency_writeFileLog(log,INFO);
                    
                    memset(log,'\0',200);
					sprintf(log,"[EnableTheseDenomination()] Note DenomMask2 Value : 0x%x h",(unsigned char)NoteDenomMask2);
                    AVRM_Currency_writeFileLog(log,INFO);
                    
                    
					//++Byte 0 : Rs 5
					if( 0x01 == (NoteDenomMask&0x01) )
					{
					   g_NOTE.rs_5      = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.5 Enable",INFO);
				    }
				   
				    //++Byte 0 : Rs 10
					if( 0x02 == (NoteDenomMask&0x02) )
					{
					   g_NOTE.rs_10      = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.10 Enable",INFO);
					}
		 
		            //++Byte 0 : Rs 20
					if( 0x04 == (NoteDenomMask&0x04) )
					{
					   g_NOTE.rs_20      = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.20 Enable",INFO);
					}

                    //++Byte 0 : Rs 50
					if( 0x08 == (NoteDenomMask&0x08) )
					{
					   g_NOTE.rs_50      = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.50 Enable",INFO);
					}

                    //++Byte 0 : Rs 100
					if( 0x10 == (NoteDenomMask&0x10) )
					{
					   g_NOTE.rs_100      = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.100 Enable",INFO);
					}
					
					//++Byte 0 : Rs 200
					if( 0x20 == (NoteDenomMask&0x20) )
					{
					    g_NOTE.rs_200      = 1;
					    AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.200 Enable",INFO);
				    }

                    //++Byte 0 : Rs 500
					if( 0x40 == (NoteDenomMask&0x40) )
					{
					   g_NOTE.rs_500     = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.500 Enable",INFO);
					}

                    //++Byte 0 : Rs 1000
					if( 0x80 == (NoteDenomMask&0x80) )
					{
					   g_NOTE.rs_1000     = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.1000 Enable",INFO);
					}

                    //++Byte 1 : Rs 2000				
					if( 0x01 == (NoteDenomMask2&0x01) )
					{
					   g_NOTE.rs_2000     = 1;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] NOTE Rs.2000 Enable",INFO);
					}

					SetDenomMaskValue(NoteDenomMask);
					SetDenomStruct(g_NOTE);
					
					#endif

		    }//if end
		    
		    //++Coin Acceptor Device
		    if( ( 2 ==  CurrencyType ) || (0 ==  CurrencyType) )
		    {
            
					#if defined(CCTALK_DEVICE) 
					
					AVRM_Currency_writeFileLog("[EnableTheseDenomination()] Coin Enable Block",INFO);
					
					memset(log,'\0',200);
					sprintf(log,"[EnableTheseDenomination()] Coin Denom Mask Value : 0x%x h",(unsigned char)CoinDenomMask);
                    AVRM_Currency_writeFileLog(log,INFO);
					
					g_COIN.rs_50    = DISABLE;
					g_COIN.rs_1     = DISABLE;
					g_COIN.rs_2     = DISABLE;
					g_COIN.rs_5     = DISABLE;
					g_COIN.rs_10    = DISABLE;
					
					if( 0x01 == (CoinDenomMask&0x01) ) {
					   g_COIN.rs_1      = ENABLE;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] Coin Rs.1 Enable",INFO);
					}//if end
				   
					if( 0x02 == (CoinDenomMask&0x02) ) {
					   g_COIN.rs_2      = ENABLE;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] Coin Rs.2 Enable",INFO);
					}//if end
					
					if( 0x04 == (CoinDenomMask&0x04) ) {
					   g_COIN.rs_5      = ENABLE;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] Coin Rs.5 Enable",INFO);
					}//if end
				   
					if( 0x08 == (CoinDenomMask&0x08) ) {
					   g_COIN.rs_10      = ENABLE;
					   AVRM_Currency_writeFileLog("[EnableTheseDenomination()] Coin Rs.10 Enable",INFO);
					}//if end
					
					CCTALK_SetDenomMaskValue(CoinDenomMask);
					
					#endif
					      
            }//++else if end

            pthread_mutex_lock(&g_APIEnableTheseDenominationsmutex );

            g_APIEnableTheseDenominationsFlag = true;

            pthread_mutex_unlock(&g_APIEnableTheseDenominationsmutex );
            
            return 0; //++Success Return

}//++int EnableTheseDenomination( int DenomMask,int Timeout)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool g_NASingleFareRejectEvent=false;
static pthread_mutex_t g_NASingleFareRejectEventMutex= PTHREAD_MUTEX_INITIALIZER;

static void setNASingleFareRejectEventFlag(bool fnSingleFareRejectEvent){
	 
	 AVRM_Currency_writeFileLog("[setNASingleFareRejectEventFlag()] Entry.",TRACE);
	 pthread_mutex_lock(&g_NASingleFareRejectEventMutex);
	 g_NASingleFareRejectEvent=fnSingleFareRejectEvent;
	 pthread_mutex_unlock(&g_NASingleFareRejectEventMutex);
	 AVRM_Currency_writeFileLog("[setNASingleFareRejectEventFlag()] Exit.",TRACE);
	 
}//static void setSingleFareRejectEventFlag() end

static bool getNASingleFareRejectEventFlag(){
	 
	 pthread_mutex_lock(&g_NASingleFareRejectEventMutex);
	 bool SingleFareRejectEvent=false;
	 SingleFareRejectEvent = g_NASingleFareRejectEvent;
	 pthread_mutex_unlock(&g_NASingleFareRejectEventMutex);
	 return SingleFareRejectEvent;
	 
}//static void setSingleFareRejectEventFlag() end

static bool g_CASingleFareRejectEvent=false;
static pthread_mutex_t g_CASingleFareRejectEventMutex= PTHREAD_MUTEX_INITIALIZER;

static void setCASingleFareRejectEventFlag(bool fnSingleFareRejectEvent){
	 
	 AVRM_Currency_writeFileLog("[setCASingleFareRejectEventFlag()] Entry.",TRACE);
	 pthread_mutex_lock(&g_CASingleFareRejectEventMutex);
	 g_CASingleFareRejectEvent=fnSingleFareRejectEvent;
	 pthread_mutex_unlock(&g_CASingleFareRejectEventMutex);
	 AVRM_Currency_writeFileLog("[setCASingleFareRejectEventFlag()] Exit.",TRACE);
	 
}//static void setSingleFareRejectEventFlag() end

static bool getCASingleFareRejectEventFlag(){
	 
	 //++AVRM_Currency_writeFileLog("[getCASingleFareRejectEventFlag()] Entry.",TRACE);
	 pthread_mutex_lock(&g_CASingleFareRejectEventMutex);
	 bool SingleFareRejectEvent=false;
	 SingleFareRejectEvent = g_CASingleFareRejectEvent;
	 pthread_mutex_unlock(&g_CASingleFareRejectEventMutex);
	 //++AVRM_Currency_writeFileLog("[getCASingleFareRejectEventFlag()] Exit.",TRACE);
	 return SingleFareRejectEvent;
	 
}//static void setSingleFareRejectEventFlag() end

static bool g_EqualFareEventFlag=false;
static pthread_mutex_t g_EqualFareFlagEventMutex= PTHREAD_MUTEX_INITIALIZER;

static void setEqualEventFlag(bool fnEqualFareEventFlag){
	
	 AVRM_Currency_writeFileLog("[setEqualEventFlag()] Entry.",TRACE);
	 pthread_mutex_lock(&g_EqualFareFlagEventMutex);
	 g_EqualFareEventFlag=fnEqualFareEventFlag;
	 pthread_mutex_unlock(&g_EqualFareFlagEventMutex);
	 AVRM_Currency_writeFileLog("[setEqualEventFlag()] Exit.",TRACE);
	 
}//static void setSingleFareRejectEventFlag() end

static bool getEqualEventFlag(){
	 
	 //AVRM_Currency_writeFileLog("[getEqualEventFlag()] Entry.",TRACE);
	 pthread_mutex_lock(&g_EqualFareFlagEventMutex);
	 bool EqualFareEventFlag=false;
	 EqualFareEventFlag = g_EqualFareEventFlag;
	 pthread_mutex_unlock(&g_EqualFareFlagEventMutex);
	 //AVRM_Currency_writeFileLog("[getEqualEventFlag()] Exit.",TRACE);
	 return EqualFareEventFlag;
	 
}//static void getEqualEventFlag() end

static pthread_mutex_t g_AcceptanceStateFlagMutex= PTHREAD_MUTEX_INITIALIZER;
static bool g_AcceptanceStateFlag=false;

bool acceptFare(int Fare,int maxNoOfCash,int maxNoOfCoin,int TransTime) {
	    
					    char log[200];
					    
					    AVRM_Currency_writeFileLog("[acceptFare()] Entry ",TRACE);
					    
					    //++Set Accepting State Flag Status
                        //++pthread_mutex_lock(&g_AcceptanceStateFlagMutex);
                        //++g_AcceptanceStateFlag=false;
                        //++pthread_mutex_unlock(&g_AcceptanceStateFlagMutex);
                        
                        //++Reset Accepting State Status TO UNKNOWN
                        pthread_mutex_lock(&g_AcceptanceStateMutex);
                        g_AcceptanceState=ACCEPTING_STATE_UNKNOWN;
                        pthread_mutex_unlock(&g_AcceptanceStateMutex);
					    
						memset(log,'\0',200);
					    sprintf(log,"[acceptFare()] Parameter Max Numbers of Cash= %d  .",maxNoOfCash);
				        AVRM_Currency_writeFileLog(log,INFO); 
					    memset(log,'\0',200);
					    sprintf(log,"[acceptFare()] Parameter Max Numbers of Coins= %d  .",maxNoOfCoin);
					    AVRM_Currency_writeFileLog(log,INFO); 
						   
					    if( Fare <  0) {
							memset(log,'\0',200);
							sprintf(log,"[acceptFare()] Negative fare not accepeted. Given fare : %d ",Fare);
							AVRM_Currency_writeFileLog(log,INFO);
							return false;
					   }//if end

					    //++Check transtime
			            if( TransTime <= 0 ){

						    memset(log,'\0',200);

							sprintf(log,"[acceptFare()] Trans time is zero or less than zero given : %d ",TransTime);
							
							AVRM_Currency_writeFileLog(log,INFO); 

							return (-3);
		  

					    }else if( TransTime > 0 ) {

						    memset(log,'\0',200);

							sprintf(log,"[acceptFare()] Trans time : %d ",TransTime);

							AVRM_Currency_writeFileLog(log,INFO); 

							g_TransTimeOut=TransTime;

						  }//else if end

                         
			              #if defined(NOTE_ACCEPTOR)

                           if(  maxNoOfCash < 0 ) {
							    memset(log,'\0',200);
                                sprintf(log,"[acceptFare() Exit] negative number of maximum number of note not accepeted. Given maxNoOfCash : %d ",maxNoOfCash);
								AVRM_Currency_writeFileLog(log,INFO); 
								return false;
						   }else if( 1== maxNoOfCash ) { //++Single Note Fare
						        AVRM_Currency_writeFileLog("[acceptFare()] Single Note Fare given",INFO); 
						   }else if(  maxNoOfCash <= 20 ) {
							    AVRM_Currency_writeFileLog("[acceptFare()] Max Cash or Less given",INFO);
						   }else if(  maxNoOfCash > MAX_NMBR_OF_NOTE ) {
							    memset(log,'\0',200);
                                sprintf(log,"[acceptFare()] maximum number of note cris cross threshold value . Given maxNoOfCash : %d .",maxNoOfCash);
								AVRM_Currency_writeFileLog(log,INFO); 
			                    maxNoOfCash = MAX_NMBR_OF_NOTE;
			                    disableNAExactFareFlag();
						   }//else if end

                          #endif

                          #if defined(COIN_ACCEPTOR)
                          if(  maxNoOfCoin < 0 ) {
							    memset(log,'\0',200);
                                sprintf(log,"[acceptFare() Exit] negative number of maximum number of coin accepeted. Given maxNoOfcoin : %d .",maxNoOfCoin);
                                AVRM_Currency_writeFileLog(log,INFO); 
                                return false;
                                
						  }else if(  maxNoOfCoin > MAX_NMBR_OF_COIN ) {
							    memset(log,'\0',200);
                                sprintf(log,"[acceptFare() Exit] maximum number of cris coin threshold value cross. Given maxNoOfcoin : %d .",maxNoOfCoin);
                                AVRM_Currency_writeFileLog(log,INFO); 
                                return false;
						  }//else if end
						  #endif

                          //++Set Single Note Event to false

                          pthread_mutex_lock(&g_SingleNoteEventMutex);

                          g_SingleNoteEvent = false ;

                          pthread_mutex_unlock(&g_SingleNoteEventMutex);

                          pthread_t threadAcptFare;

						  int  iretAcptFare=-1;

						  int i=0,j=0;
						  
						  int day=0,mon=0,yr=0,hr=0,min=0,sec=0;
						  
						  //++Init All global variables    

						  pthread_mutex_lock(&mut);

						  g_Fare        = 0;

						  g_maxNoOfCash = 0;

						  g_maxNoOfCoin = 0;

						  g_totalNoteAmountAccepted = 0;

						  g_totalCoinAmountAccepted = 0;

						  g_totalAmountAccepted     = 0;

						  g_ttlNmbrOfNote           = 0;

						  g_ttlNmbrOfCoin           = 0;

						  g_denomIndex = 0;
						  
						  g_NotedenomIndex=0;
						  
						  g_CoindenomIndex=0;

					      g_getDenomtransactiontimeout=false;

						  pthread_mutex_unlock(&mut);
						  
						   //++Add By Malay on 31 Jan 2013
						   pthread_attr_t attr;

						   int returnVal=-1;

                           pthread_mutex_lock(&mut);

                           g_Fare        = Fare;

                           g_maxNoOfCash = maxNoOfCash;

                           g_maxNoOfCoin = maxNoOfCoin;

                           //++clear denomation array
						   for( i=0; i< 2; i++)  {
								for(j=0; j<MAX_DENOM_NUMBER; j++) {
									g_acpted_CurrencyDetail[i][j] = 0; 
                                }//for end
						   }//for end

						   //++Set previous state of this flag(Malay add)
						   g_ExternalKillAcptFareThread=THREAD_NO_KILL;

                           //++Set SINGLEFARE Reject Event to False
                           bool fnSingleFareRejectEvent =false;
                           
                           setNASingleFareRejectEventFlag(fnSingleFareRejectEvent);
                           
                           setCASingleFareRejectEventFlag(fnSingleFareRejectEvent);
                           
                           //++Set SINGLEFARE Accept Event to False
                           bool fnEqualFareEventFlag=false;
                           
                           setEqualEventFlag(fnEqualFareEventFlag);
                           
						   g_transactiontimeout=false; 

                           g_AllAlreadyDisableFlag=false;

                           pthread_mutex_unlock(&mut);

                           #if defined(NOTE_ACCEPTOR) &&  defined(B2B_NOTE_ACCEPTOR)
                           ResetNoteAcceptanceFlag();
                           #endif

                           #if defined(COIN_ACCEPTOR)
                           //if(  maxNoOfCoin > 0 ) {
							ClearCoinsCounter();
						   //}//if end
                           #endif

                           #if defined(NOTE_ACCEPTOR)
                           //if( maxNoOfCash > 0 ){
							ClearNotesCounter();
						   //}
                           #endif
                           
                           #if defined(NOTE_ACCEPTOR)
                           //if( maxNoOfCash > 0 ){
							SetAlreadyCoinInhibit();
						   //}
						   #endif

						   #if defined(COIN_ACCEPTOR)
						   if(  maxNoOfCoin > 0 ) {
								InitCATransModel();
								if( 1== maxNoOfCoin ){
								   //++enableCAExactFareFlag();
								}//++if end
						   }//++if end
						   #endif
						   
						   #if defined(NOTE_ACCEPTOR)
						   if( maxNoOfCash > 0 ){
								InitNATransModel();
								if( 1== maxNoOfCash ){
								   enableNaExactFareFlag();
							    }else{
								   disableNAExactFareFlag();		
								}//else end
						   }//++if end
						   #endif

						   #ifdef NOTE_ACCEPTOR
						   //++30 May 2013 add by malay Read recycle status before transaction
						   if( maxNoOfCash > 0 ){
								GetRecycleStatusBeforeStartAcceptfare();
						   }//++IF END
						   #endif
                         
                           bool StartNoteAcceptorRtcode=false,
                           StartCoinAcceptorRtcode=false,
                           StartManagerRtcode=false,
                           ActivateAllRtcode=false;
                           
                           pthread_mutex_lock( &CloseAcceptFareThreadFlagmutex );

                           g_CloseAcceptFareThreadFlag =false;
                           
                           pthread_mutex_unlock( &CloseAcceptFareThreadFlagmutex );
                           
                           InitAcceptFaresModel();

                           StartNoteAcceptorRtcode = StartNoteAcceptorAllThreads(); //++Level 1
                           
                           AVRM_Currency_writeFileLog("[acceptFare()] Before Start All Acceptor Devices Threads",INFO);

                           if( true ==  StartNoteAcceptorRtcode) {
                                                                   
                                   AVRM_Currency_writeFileLog("[acceptFare()] Start NoteAcceptor All Threads started successfully.",INFO); 

                                   StartCoinAcceptorRtcode=StartCoinAcceptorAllThreads(); //++Level 2

                                    if( true ==  StartCoinAcceptorRtcode ){
                                              
                                       AVRM_Currency_writeFileLog("[acceptFare()] Start CoinAcceptor All Threads started successfully.",INFO); 
                                       
		                               StartManagerRtcode = StartManagerThread(); //++Level 3

		                               if( true ==  StartManagerRtcode) {

		                                            ActivateAllRtcode=ActivateAllAcceptors(); //++Level 4

		                                            if( true ==  ActivateAllRtcode){
														
														
                                                        AVRM_Currency_writeFileLog("[acceptFare()] Start Acceptfare Manager Thread started successfully.",INFO); 
                                                        AVRM_Currency_writeFileLog("[acceptFare()] Before Start All Credit Polling Threads.",INFO); 
                                                        StartAllAcceptProcessThreads();
														
														AVRM_Currency_writeFileLog("[acceptFare()] After Start All Credit Polling Threads.",INFO); 

                                                    }else if( false ==  ActivateAllRtcode){

														
														AVRM_Currency_writeFileLog("[acceptFare()] Start Acceptfare Manager thread failed to start.",INFO); 
                                                        StopAllAcceptProcessThreads(LEVEL4);
                                                        return false;
                                                        
		                                            }//else if
		                               }else if( false ==  StartManagerRtcode){

                                                      
                                                      AVRM_Currency_writeFileLog("[acceptFare()] Start Acceptfare Manager thread failed to start.",INFO); 
                                                      
                                                      StopAllAcceptProcessThreads(LEVEL3);

                                                      return false;
 
 
                                       }//else if end         
                                       }else if( false ==  StartCoinAcceptorRtcode ){
                                          
                                           
                                           AVRM_Currency_writeFileLog("[acceptFare()] Start CoinAcceptor All Threads failed to start.",INFO); 
                                           
                                           StopAllAcceptProcessThreads(LEVEL2);

                                           return false;


                                   }//else if end

                           }else if( false ==  StartNoteAcceptorRtcode){
                               
                                AVRM_Currency_writeFileLog("[acceptFare()] Start CoinAcceptor All Threads failed to start.",INFO); 
                                
                                StopAllAcceptProcessThreads(LEVEL1);

                                return false;

                           } //else if end      
                    
						   AVRM_Currency_writeFileLog("[acceptFare()] After Start All Acceptor Devices Threads",INFO);
                    
                           //++make a log about current fare

						   g_total_transaction++; 

                           memset(log,'\0',200);

						   sprintf(log,"[acceptFare()] Transaction Number : %d.",g_total_transaction);

						   AVRM_Currency_writeFileLog(log,INFO); 

                           //++Make a log about fare
						   getDateTime(&day,&mon,&yr,&hr,&min,&sec);

                           memset(log,'\0',200);

						   sprintf(log,"[acceptFare()] Transaction Date(dd/mm/yyyy)= %d/%d/%d Start Time(hh:mm:ss)= %d:%d:%d.",day,mon,yr,hr,min,sec);
						   
                           AVRM_Currency_writeFileLog(log,INFO); 
                           
                           pthread_mutex_lock(&mut);

						   memset(log,'\0',200);

						   sprintf(log,"[acceptFare()] Transaction Fare= %d.",g_Fare);

                           AVRM_Currency_writeFileLog(log,INFO);

                           memset(log,'\0',200);

						   sprintf(log,"[acceptFare() Exit] Max Numbers of Cash= %d  .",g_maxNoOfCash);

						   AVRM_Currency_writeFileLog(log,INFO); 
						   
						   memset(log,'\0',200);

						   sprintf(log,"[acceptFare() Exit] Max Numbers of Coin= %d  .",g_maxNoOfCoin);
						   
						   AVRM_Currency_writeFileLog(log,INFO);
						   
						   memset(log,'\0',200);
						   sprintf(log,"[acceptFare() Exit] TotalNotes= %d  .",g_ttlNmbrOfNote);
						   AVRM_Currency_writeFileLog(log,INFO);
						     
                           memset(log,'\0',200);
						   sprintf(log,"[acceptFare() Exit] g_totalAmountAccepted= %d  .",g_totalAmountAccepted);
						   AVRM_Currency_writeFileLog(log,INFO);

                           pthread_mutex_unlock(&mut);
                           
                           //++Now Signal Acceptance State is running
                           AVRM_Currency_writeFileLog("[acceptFare() Exit] Before Going to Enable Acceptance State Running Flag On",INFO);
                           
                           //++Set Accepting State Flag Status
                           pthread_mutex_lock(&g_AcceptanceStateFlagMutex);
                           g_AcceptanceStateFlag=true;
                           pthread_mutex_unlock(&g_AcceptanceStateFlagMutex);
                           
                           AVRM_Currency_writeFileLog("[acceptFare() Exit] After Going to Enable Acceptance State Running Flag On",INFO);
                           AVRM_Currency_writeFileLog("[acceptFare()] Exit ",TRACE);
                           
                           return true;

}//++acceptFare() end here

int  GetFare() 
{
         
          int fareamount=0;

          pthread_mutex_lock(&mut);

          fareamount = g_Fare ;
        
          pthread_mutex_unlock(&mut);

          return fareamount;

}//int  GetFare()  end

int  GetCurrentAcceptedAmount() 
{
         
          int acceptedamount=0;

          pthread_mutex_lock(&mut);

          acceptedamount = g_totalAmountAccepted ;
        
          pthread_mutex_unlock(&mut);

          return acceptedamount;

}//int  GetCurrentAcceptedAmount() end

int getDenomination(int arry[][MAX_DENOM_NUMBER])
{
	
				  int i = 0, j = 0;
				  int fare=0,totalAmountAccepted=0;
                  bool acceptStateStaus=false;
                  char log[200];
                  memset(log,'\0',200);

                  //++malay add 29 Jan 2014
				  pthread_mutex_lock(&mut);
                  //#define MAX_DENOM_NUMBER  40                    
                  for(i=0; i<2; i++){
                         for(j = 0; j<MAX_DENOM_NUMBER; j++){
							 arry[i][j] = g_acpted_CurrencyDetail[i][j];
						 }//++for end
				  }//++for end

			      if( (g_ttlNmbrOfNote > g_maxNoOfCash) && (g_maxNoOfCash >0)   ) { //++Max no Cash accepted 

							 pthread_mutex_unlock(&mut);
							 
							 memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] TotalNotes= %d  .",g_ttlNmbrOfNote);
						     AVRM_Currency_writeFileLog(log,INFO);
						     
						     memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] g_maxNoOfCash= %d  .",g_maxNoOfCash);
						     AVRM_Currency_writeFileLog(log,INFO);
						     
						     memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] g_Fare= %d  .",g_Fare);
						     AVRM_Currency_writeFileLog(log,INFO);

                             memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] g_totalAmountAccepted= %d  .",g_totalAmountAccepted);
						     AVRM_Currency_writeFileLog(log,INFO);
						     
							 AVRM_Currency_writeFileLog("[getDenomination() Exit] Note Escrow Full.",INFO);

							 return (STATE_NOTE_ESCROW_FULL);   

                  }else if( (g_ttlNmbrOfCoin > g_maxNoOfCoin) && (g_maxNoOfCoin >0)   )  { //++Max no Coins accepted 
                 

							 pthread_mutex_unlock(&mut);
							 
							 memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] TotalCoins= %d  .",g_ttlNmbrOfCoin);
						     AVRM_Currency_writeFileLog(log,INFO);
						     
						     memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] g_maxNoOfCoin= %d  .",g_maxNoOfCoin);
						     AVRM_Currency_writeFileLog(log,INFO);
						     
						     memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] g_Fare= %d  .",g_Fare);
						     AVRM_Currency_writeFileLog(log,INFO);

                             memset(log,'\0',200);
							 sprintf(log,"[getDenomination() Exit] g_totalAmountAccepted= %d  .",g_totalAmountAccepted);
						     AVRM_Currency_writeFileLog(log,INFO);

							 AVRM_Currency_writeFileLog("[getDenomination() Exit] Coin Escrow Full.",INFO);

							 return (STATE_COIN_ESCROW_FULL);   

                  }else if(  true == getEqualEventFlag()  ) { //++Accepting State stopped and accepted fare equal to fare
				  
                              pthread_mutex_unlock(&mut);

                              AVRM_Currency_writeFileLog("[getDenomination() Exit] Accepted total fare.",INFO);
			                    
			                  return STATE_EXACT_AMOUNT_ACCEPTED;  //++Exact Amount

				  }else if(  g_totalAmountAccepted > g_Fare   ) { //++Excess Amount Accepted
                              pthread_mutex_unlock(&mut);

                              AVRM_Currency_writeFileLog("[getDenomination() Exit] Excess Amount Accepted.",INFO);
			                    
			                  return STATE_EXCESS_AMOUNT_ACCEPTED;  //Excess Amount
			                  
				  }else if( ( 1 == g_maxNoOfCoin ) || ( 1 == g_maxNoOfCash ) ) { //++Single Fare Check
								 
								        //++AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE BLOCK.",INFO);
								        //++Check Note Acceptor Reject State
								        if( ( 1 == g_maxNoOfCash ) && (true == getNASingleFareRejectEventFlag()) ){
									        
									        pthread_mutex_unlock(&mut);
									        AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE NOTE_REJECTED_DUE_TO_INHIBITED_NOTE.",INFO);
									        return (NOTE_REJECTED_DUE_TO_INHIBITED_NOTE); 
									        
									    }else if( ( 1 == g_maxNoOfCoin ) && (true == getCASingleFareRejectEventFlag()) ){
									        
									        pthread_mutex_unlock(&mut);
									        AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE COIN_REJECTED_DUE_TO_INHIBITED_COIN.",INFO);
									        return (COIN_REJECTED_DUE_TO_INHIBITED_COIN);
									       
									    }else if(  g_Fare == g_totalAmountAccepted   ) {
                                            
                                            pthread_mutex_unlock(&mut);
                                            AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Accepted total fare.",INFO);
			                                return STATE_EXACT_AMOUNT_ACCEPTED;  //++Exact Amount
			                                
			                            }else if(  true == getEqualEventFlag()  ) {
                                            
                                            pthread_mutex_unlock(&mut);
                                            AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Accepted total fare.",INFO);
			                                return STATE_EXACT_AMOUNT_ACCEPTED;  //++Exact Amount
			                                
			                            }else if(  true == g_getDenomtransactiontimeout  ) { 

											  pthread_mutex_unlock(&mut);
											  AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Transaction timeout from api.",INFO);
											  if(  g_Fare > g_totalAmountAccepted  ){
										         AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Still No fare Amount receieved.",INFO);
										         return OPERATION_TIMEOUT_OCCURRED;  
										      }else if(  g_totalAmountAccepted > g_Fare    ){
										         AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Still Excess Amount receieved.",INFO);
										         return STATE_EXCESS_AMOUNT_ACCEPTED; 
										      }else if(  g_Fare == g_totalAmountAccepted  ){
												 AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Exact Amount receieved.",INFO);
										         return STATE_EXACT_AMOUNT_ACCEPTED; 
											  } else{	  
												AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Transaction Out By Default.",INFO);
												return OPERATION_TIMEOUT_OCCURRED;  
											  }//else end

										}else if(  g_Fare > g_totalAmountAccepted  ){
											
											pthread_mutex_unlock(&mut);
											//++AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Accepting State running.",INFO);
										    return STATE_ACCEPTING; //++Accepting State
										    
										}else {
											
											pthread_mutex_unlock(&mut);
											acceptStateStaus=false;
											
											//++Get Accepting State Flag Status
                                            pthread_mutex_lock(&g_AcceptanceStateFlagMutex);
                                            acceptStateStaus = g_AcceptanceStateFlag;
                                            pthread_mutex_unlock(&g_AcceptanceStateFlagMutex);
                                            
                                            if( true == acceptStateStaus ){
												  //++AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE Accepting State",INFO);
												  return STATE_ACCEPTING; //++Accepting State
											}else {
												  AVRM_Currency_writeFileLog("[getDenomination() Exit] SINGLEFARE No Accepting sequence matched!!!.",INFO);
												  int currentState = -1;
												  pthread_mutex_lock(&g_AcceptanceStateMutex);
												  currentState = g_AcceptanceState; 
												  pthread_mutex_unlock(&g_AcceptanceStateMutex);
												  if( COMMIT_STATE == currentState ){
													  return (8); //++Commit Status
												  }else if( CANCEL_STATE == currentState ){
													  return (7); //++Cancel Status
												  }else{
													  return (OTHER_ERROR); //++Exception State
												  }//else end
												  
											}//else end
										
										}//else end
										
		          }else if(  true == g_getDenomtransactiontimeout  ) {  //++API timeout happened (Internally and Externally) and accepting state closed by forcefully

							 pthread_mutex_unlock(&mut);
							 
							 AVRM_Currency_writeFileLog("[getDenomination() Exit] Transaction timeout from api.",INFO);

							 return OPERATION_TIMEOUT_OCCURRED;  

				  }/*else if(  g_Fare > g_totalAmountAccepted  ) {  //++Accepting State running 

                                //++AVRM_Currency_writeFileLog("[getDenomination() Exit] Accepting State running.",INFO);

                                pthread_mutex_unlock(&mut);

                                return STATE_ACCEPTING; //Accepting State
 
					      
				  }else {

                                 pthread_mutex_unlock(&mut);
                                 acceptStateStaus=false;
                                 
                                 //++Get Accepting State Flag Status
                                 pthread_mutex_lock(&g_AcceptanceStateFlagMutex);
                                 acceptStateStaus = g_AcceptanceStateFlag;
                                 pthread_mutex_unlock(&g_AcceptanceStateFlagMutex);
                                 
                                 if( true == acceptStateStaus ){
								      //++AVRM_Currency_writeFileLog("[getDenomination() Exit] Accepting State",INFO);
								      return STATE_ACCEPTING; //++Accepting State
							     }else {
									  
									  AVRM_Currency_writeFileLog("[getDenomination() Exit] No Accepting sequence matched!!!.",INFO);
									  int currentState = -1;
									  pthread_mutex_lock(&g_AcceptanceStateMutex);
									  currentState = g_AcceptanceState; 
									  pthread_mutex_unlock(&g_AcceptanceStateMutex);
									  if( COMMIT_STATE == currentState ){
										  return (8); //++Commit Status
									  }else if( CANCEL_STATE == currentState ){
										  return (7); //++Cancel Status
									  }else{
										  return (OTHER_ERROR); //++Exception State
									  }//else end
								 }//++else end
                                 
				  }//++else end
				  */
				  else{
								pthread_mutex_unlock(&mut);

                                return STATE_ACCEPTING; //Accepting State
                  }
				  	
}//++getDenomination() end here

void* ThreadProc_acceptFare(void *ptr) 
{

             
            AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Entry.",TRACE);

            //++wait for signal
            pthread_mutex_lock( &g_AcceptFareEnableThreadmutex );

            pthread_cond_wait( &g_AcceptFareEnableThreadCond  , &g_AcceptFareEnableThreadmutex  );

            pthread_mutex_unlock( &g_AcceptFareEnableThreadmutex );

            AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] After Wait Signal.",INFO);
             
            pthread_mutex_lock( &CloseAcceptFareThreadFlagmutex );
      
            //++if credit poll flag is set exit from thread
            if( true == g_CloseAcceptFareThreadFlag ){
                     
                    g_CloseAcceptFareThreadFlag=false;
                    pthread_mutex_unlock (&CloseAcceptFareThreadFlagmutex );
                    pthread_mutex_lock(&AcptFareThreadStatusmutex);
                    g_AcptFareThreadStatus=THREAD_CLOSE;
					pthread_mutex_unlock(&AcptFareThreadStatusmutex);
					AVRM_Currency_writeFileLog("[ThreadProc_acceptFare() Exit] Thread Exit.",ERROR);
                    pthread_exit(0);

	        }else {

                   pthread_mutex_unlock( &CloseAcceptFareThreadFlagmutex );
                   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Thread Running.",ERROR);

            }//else end

          //Intimate device to start accepting currency for the total ticket fare with max number of notes and coins to be accepted for    single one transaction . API will return true if all the devices are ready to accept fare. 
	  
          struct timespec begints={0},endts={0},diffts={0};

          pthread_t thread1,thread2;

	      int  iret1=-1,iret2=-1,CoinCreditThreadRtcode=-1,NotePollThreadRtcode=-1,returnVal=-1;

          unsigned char log[200];

          memset(log,'\0',200);
          
          AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Now Going to monitor accepting process.",INFO);

	      //++Start Transtime
	      clock_gettime(CLOCK_MONOTONIC, &begints);
	     
		  while(1) {
			                 
			                 //++Get Current Time
                             clock_gettime(CLOCK_MONOTONIC, &endts);

                             //++Differnce between start and end time
			                 diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                             
                             //++Lock
                             pthread_mutex_lock(&mut);
                             
                             //++External Anytime Accept process close 
							 if( THREAD_KILL == g_ExternalKillAcptFareThread ) {
				                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start External Kill Thread Block.",INFO);
                                       pthread_mutex_unlock(&mut);
                                       StopAcceptingState();
                                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] End External Kill Thread Block.",INFO);
                                       break;
                             }//if( THREAD_KILL == g_ExternalKillAcptFareThread )
                             
                             //++Check Single Fare and Reject State
                             if( ( 1 == g_maxNoOfCoin ) || ( 1 == g_maxNoOfCash ) ) {
								 
								        pthread_mutex_unlock(&mut);
								        //++AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start SINGLEFARE Fare Block.",INFO);
								        bool fnSingleFareRejectEvent=true;
								        							        
								        //++Check Note Acceptor Reject State
								        if( ( 1 == g_maxNoOfCash ) && (true == getNARejectEventFlagStatus()) ){//++Check Notes Reject States
									           AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start SINGLEFARE Note Fare With Reject Block.",INFO);
                                               StopAcceptingState();
                                               setNASingleFareRejectEventFlag(fnSingleFareRejectEvent);
						                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] End SINGLEFARE Note Fare With Reject Block.",INFO);
											   break; 
									    }else if( ( 1 == g_maxNoOfCoin ) && (true == getCARejectEventFlagStatus()) ){//++Check Coins Reject States
									           AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start SINGLEFARE Coin Fare With Reject  Block.",INFO);
											   StopAcceptingState();
											   fnSingleFareRejectEvent=true;
											   setCASingleFareRejectEventFlag(fnSingleFareRejectEvent);
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] End SINGLEFARE Coin Fare With Reject Block.",INFO);
											   break; 
									    }else if( g_Fare == g_totalAmountAccepted ){//++Check Equal Fare States
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] SINGLEFARE Start Equal fare Block.",INFO);
											   bool fnEqualFareEventFlag=true;
								               StopAcceptingState();
											   setEqualEventFlag(fnEqualFareEventFlag);
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] SINGLEFARE End Equal fare accepted.",INFO);
											   break;											   
			                            }else if( g_totalAmountAccepted > g_Fare  ){//++Check Excess Fare States
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] SINGLEFARE Start Excess fare Block.",INFO);
											   StopAcceptingState();
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] SINGLEFARE End Excess fare accepted.",INFO);
											   break;
											   
			                            }else if( (diffts.tv_sec*1000) >= (g_TransTimeOut-(2*1000)) ) {//++Check Timeout States
                                               
                                               memset(log,'\0',200);
											   sprintf(log,"[ThreadProc_acceptFare()] SINGLEFARE Calculated MilliSecond= %d And  Given Transtime= %d.",(diffts.tv_sec*1000),g_TransTimeOut);
											   AVRM_Currency_writeFileLog(log,INFO);
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] SINGLEFARE Start Transaction Timeout Block.",INFO);
											   pthread_mutex_lock(&mut);
											   //++Signal Low Level Note And Coin Credit Polling Thread to closed their operation
											   g_transactiontimeout=true;
											   pthread_mutex_unlock(&mut);
											   StopAcceptingState();
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] SINGLEFARE Before Signal Transaction Timeout .",INFO);
											   pthread_mutex_lock(&mut);
											   g_getDenomtransactiontimeout=true;
											   pthread_mutex_unlock(&mut);
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] SINGLEFARE After Signal Transaction Timeout .",INFO);
											   AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()]  SINGLEFARE End Transaction Timeout Block.",INFO);
											   break;
											   
                                        }else {
										      continue; //++Loop Continue
										}//++else end
                                        break; 
								 
							 }//if( ( 1 == g_maxNoOfCoin ) || ( 1 == g_maxNoOfCash >0) ) end
							 
                             //++Accepted money greater than fare selected
							 if( g_totalAmountAccepted > g_Fare  ){

                                       pthread_mutex_unlock(&mut);

				                       //writeFileLog"[ThreadProc_acceptFare()] Start Greater than fare Block.");
				                       
				                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start Greater than fare Block.",INFO);
                                      
                                       StopAcceptingState();

				                       //writeFileLog"[ThreadProc_acceptFare()] End Greater than fare Block.");
				                       
				                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] End Greater than fare Block.",INFO);
 
                                       break;

				       
			                 }//if( g_totalAmountAccepted > g_Fare  ) end
                         
						     //++Max no of cash and Max no Coin limit crossed [default cash and coin max is 20]
						     if( ( (g_ttlNmbrOfNote >= g_maxNoOfCash) && (g_maxNoOfCash >0) ) || ( (g_ttlNmbrOfCoin >= g_maxNoOfCoin) && (g_maxNoOfCoin >0) ) ) {
                                     
                                        pthread_mutex_unlock(&mut);
					
                                        //writeFileLog"[ThreadProc_acceptFare()] Start Maximum Cash or Coin Equal Block.");
                                        
                                        AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start Maximum Cash or Coin Equal Block.",INFO);
                                     
                                        StopAcceptingState();
				       
			                            memset(log,'\0',200);

		                                sprintf(log,"[ThreadProc_acceptFare()]  Accepted Total Notes= %d Accepted Total Coins= %d.", g_ttlNmbrOfNote,g_ttlNmbrOfCoin);

                                        //writeFileLoglog);
                                        
                                        AVRM_Currency_writeFileLog(log,INFO);

                                        //writeFileLog"[ThreadProc_acceptFare()] End Maximum Cash or Coin Equal Block.");
                                        
                                        AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] End Maximum Cash or Coin Equal Block.",INFO);

                                        break; 

			
			                }
     
		                     //++Accepted money equal to fare selected
							 if( g_Fare == g_totalAmountAccepted ){

                                       pthread_mutex_unlock(&mut);

				                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start Equal fare Block.",INFO);
                                       
                                       StopAcceptingState();
                                       
                                       bool fnEqualFareEventFlag=true;
                           
                                       setEqualEventFlag(fnEqualFareEventFlag);
                                       
				                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] End Equal fare accepted.",INFO);
 
                                       break;

					   
			                } //if( g_Fare == g_totalAmountAccepted )
							 
							 //++Unlock
                             pthread_mutex_unlock(&mut);
                             
                             //++API Transaction Timeout
                             if( (diffts.tv_sec*1000) >= (g_TransTimeOut-(2*1000)) ) {

                                       memset(log,'\0',200);
                                       sprintf(log,"[ThreadProc_acceptFare()] Calculated MilliSecond= %d And  Given Transtime= %d.",(diffts.tv_sec*1000),g_TransTimeOut);
		                               AVRM_Currency_writeFileLog(log,INFO);
                                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Start Transaction Timeout Block.",INFO);
                                       pthread_mutex_lock(&mut);
                                       //Signal Low Level Note And Coin Credit Polling Thread to closed their operation
                                       g_transactiontimeout=true;
                                       pthread_mutex_unlock(&mut);
                                       StopAcceptingState();
                                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Before Signal Transaction Timeout .",INFO);
                                       pthread_mutex_lock(&mut);
                                       g_getDenomtransactiontimeout=true;
                                       pthread_mutex_unlock(&mut);
                                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] After Signal Transaction Timeout .",INFO);
                                       AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] End Transaction Timeout Block.",INFO);
                                       break;
                                       
                            }//++if( (diffts.tv_sec*1000) >= (g_TransTimeOut-(2*1000)) ) end	   
                                 
    }//++End while


    //++Log Trans Data
    //++LogThisTransData();

    //++Now Close Thread
    AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] Now set flag to close status.",INFO);
    
    pthread_mutex_lock(&AcptFareThreadStatusmutex);
    g_AcptFareThreadStatus=THREAD_CLOSE;
    pthread_mutex_unlock(&AcptFareThreadStatusmutex);
    
    AVRM_Currency_writeFileLog("[ThreadProc_acceptFare() Exit] End ThreadProc_acceptFare Thread.",INFO);
    
    //++Now Set Signal Acceptance State is stopped
    pthread_mutex_lock(&g_AcceptanceStateFlagMutex);
    g_AcceptanceStateFlag=false;
    pthread_mutex_unlock(&g_AcceptanceStateFlagMutex);
    
    pthread_exit(0);
				       
}//++End ThreadProc_acceptFare function

static  void InitAcceptFaresModel() 
{

          
          ////writeFileLog"[InitAcceptFaresModel()] Entry.");
          AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Entry.",TRACE);

          #ifdef NOTE_ACCEPTOR

          //check range of cash and coin (valid range 0-20 and other any invalid range)
          if( (g_maxNoOfCash >= 0) && (g_maxNoOfCash <= MAX_NMBR_OF_NOTE))
          {
                ////writeFileLog"[InitAcceptFaresModel()] Valid range maxnoofcash.");
                AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Valid range maxnoofcash.",INFO);
          }else{
                
                //g_maxNoOfCash is negative number
                g_maxNoOfCash=0;
                
                ////////////////////////////////////////////////////////////////////////

	            //if( g_Fare > 0)
                //{
                    //g_maxNoOfCash = 20;
                //}
                //else
                //{  
                    //g_maxNoOfCash = 0;

                //}

                //////////////////////////////////////////////////////////////////////////

                ////writeFileLog"[InitAcceptFaresModel()] Invalid range maxnoofcash.");
                AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Invalid range maxnoofcash.",INFO);

	      }//else end
    
          #endif

          #ifdef COIN_ACCEPTOR

		  if( (g_maxNoOfCoin >= 0) && (g_maxNoOfCoin <=  MAX_NMBR_OF_COIN)){
				////writeFileLog"[InitAcceptFaresModel()] Valid range maxnoofcoin.");
				AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Valid range maxnoofcoin.",INFO);
		  }else{

                 //g_maxNoOfCoin is negative number
                 g_maxNoOfCoin=0;

                 ///////////////////////////////////////////////////////////////////////////

	             //if(g_Fare > 0)
                 //{
                     //g_maxNoOfCoin = 20;
                 //}
                 //else
                 //{ 
                     //g_maxNoOfCoin = 0;

                 //}

                 ///////////////////////////////////////////////////////////////////////////

                 ////writeFileLog"[InitAcceptFaresModel()] Invalid range maxnoofcoin.");
                 AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Invalid range maxnoofcoin.",INFO);

          }

          #endif

          ////////////////////////////////////////////////////////////////////////////////////////

          #ifdef COIN_ACCEPTOR

          if( 0 == g_maxNoOfCoin)
          {
               g_CoinFullDisable=1; //set (runtime control disabled)
               ////writeFileLog"[InitAcceptFaresModel()] Runtime Coin Control Disabled.");
               AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Runtime Coin Control Disabled.",INFO);
          }
          else
          {
               g_CoinFullDisable=0;//default (runtime control enabled)
               ////writeFileLog"[InitAcceptFaresModel()] Runtime Coin Control Enabled.");
               AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Runtime Coin Control Enabled.",INFO);
          }

          #endif

          #ifdef NOTE_ACCEPTOR

          if( 0 == g_maxNoOfCash)
          {
               g_NoteFullDisable=1; //set (runtime control disabled)
               ////writeFileLog"[InitAcceptFaresModel()] Runtime Note Control Disabled.");
               AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Runtime Note Control Disabled.",INFO);
          }
          else
          {
               g_NoteFullDisable=0; //default (user runtime control enabled)
               ////writeFileLog"[InitAcceptFaresModel()] Runtime Note Control Enabled.");
               AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Runtime Note Control Enabled.",INFO);
          }
           
          #endif
          
          #ifdef NOTE_ACCEPTOR
          SetCoinFullDisableFlag(g_maxNoOfCash,g_maxNoOfCoin);
	      #endif

          pthread_mutex_lock(&RuntimeInhibitFlagmutex);

          //malay add 3 feb 2013
          g_RuntimeInhibitFlag=1;

          pthread_mutex_unlock(&RuntimeInhibitFlagmutex);
	   
	      AVRM_Currency_writeFileLog("[InitAcceptFaresModel()] Exit.",TRACE);
	      
          ////writeFileLog"[InitAcceptFaresModel()] Exit.");
          
          return;



}//InitAcceptFaresModel() end here

static  bool ActivateAllAcceptors() 
{


           int rtcode=FAIL;
 
           rtcode = makeAccepters_at_ActiveteMode(g_Fare,g_maxNoOfCash,g_maxNoOfCoin);

           //Now time to activate note and coin acceptor	   
		   if( FAIL == rtcode ) 
		   {     
                    AVRM_Currency_writeFileLog("[ActivateAllAcceptors()] makeAccepters_at_ActiveteMode() return fail.",ERROR);
					return false;  
	  

		   }else if( SUCCESS == rtcode ) {

                     AVRM_Currency_writeFileLog("[ActivateAllAcceptors()] makeAccepters_at_ActiveteMode() return success.",INFO);
                     return true;

           }//else if end

}//static  bool ActivateAllAcceptors() end

static  bool StartManagerThread() 
{

        
	                              
               pthread_t threadAcptFare;
          
			   int iretAcptFare=-1,returnVal=-1;

			   pthread_attr_t attr;

               //Create the detached thread using POSIX routines add by malay on 31 Jan 2013

			   returnVal = pthread_attr_init(&attr);

			   if(0!=returnVal)
			   {

			       ////writeFileLog"[StartManagerThread()] acceptFareThread attribute create failed so return here from.");
                   AVRM_Currency_writeFileLog("[StartManagerThread()] acceptFareThread attribute create failed so return here from.",INFO);
			       return false;

			   }

			   returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			   if(0!=returnVal)
			   {
                   AVRM_Currency_writeFileLog("[StartManagerThread()] acceptFareThread thread attribute detached create failed so return here from.",INFO);
			       ////writeFileLog"[StartManagerThread()] acceptFareThread thread attribute detached create failed so return here from.");
			       return false;

			   }

			   //Thread Create here
			   iretAcptFare= pthread_create(&threadAcptFare, &attr,&ThreadProc_acceptFare, NULL);

			   //Malay Add 19 Jan 2013
			   //Signal External Function that acpt fare thread start now
			   if(0==iretAcptFare)
			   {

			     g_AcptFareThreadStatus=THREAD_START;
                 AVRM_Currency_writeFileLog("[StartManagerThread()] acceptFareThread Creation Success.",INFO);
			     ////writeFileLog"[StartManagerThread()] acceptFareThread Creation Success.");
                             
			   }
			   else
			   {

			      g_AcptFareThreadStatus=THREAD_CLOSE;
                  
                  AVRM_Currency_writeFileLog("[StartManagerThread()] acceptFareThread Creation Failed!.",INFO);
                   
			      ////writeFileLog"[StartManagerThread()] acceptFareThread Creation Failed!.");

			      return false;

			   }

			   returnVal = pthread_attr_destroy(&attr);

			   if(0!=returnVal)
			   {
                 AVRM_Currency_writeFileLog("[StartManagerThread()] acceptFareThread thread attribute destroy failed!.",INFO);
			     ////writeFileLog"[StartManagerThread()] acceptFareThread thread attribute destroy failed!.");

			   }
			   
               return true;


}//++ static  bool StartManagerThread() end

//++Level 1
static  bool StartNoteAcceptorAllThreads() 
{
       
        #if defined(NOTE_ACCEPTOR)
           
        if( g_maxNoOfCash > 0 ) {

				   bool lowlevelthread=false; 

				   pthread_t thread1;
				  
				   int returnVal=-1,iret1=-1;

				   pthread_attr_t attr1;

				   returnVal = pthread_attr_init(&attr1);

				   if(0!=returnVal)
				   {
                       AVRM_Currency_writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart Thread attribute create failed so return here from.",INFO);
					   
				   }

				   returnVal = pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);

				   if(0!=returnVal)
				   {
                       AVRM_Currency_writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart thread attribute detached create failed so return here from.",INFO);
					
				   }
				   
				   iret1     = pthread_create(&thread1, &attr1,&ThreadProc_NoteAcptrStart, NULL);

				   if(0==iret1)
				   {    
					    AVRM_Currency_writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart Creation Success.",INFO);
						lowlevelthread=true;
				   }
				   else
				   {
						AVRM_Currency_writeFileLog("[acceptFare()] ThreadProc_NoteAcptrStart Creation Failed!.",ERROR);
				   }

				   returnVal = pthread_attr_destroy(&attr1);

				   if(0!=returnVal)
				   {
					 AVRM_Currency_writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart thread attribute destroy failed!.",ERROR);
				   }
				   
				   
				   if(true == lowlevelthread)
				   {

						   //Start Note Acceptor Low level thread

						   unsigned int NALowlevelThreadRtcode=0;

						   NALowlevelThreadRtcode=NoteAcptrStartCreditPollThread();

						   if( 1 != NALowlevelThreadRtcode )
						   {

								AVRM_Currency_writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart thread CREATE failed!.",ERROR);
								return false;
						   }
						   else
						   {

								AVRM_Currency_writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc1_CASHCODE_NoteAcptrPoll thread create successed.",INFO);
								return true;

						   }


				 } 
				 else if( false == lowlevelthread)
				 {

						return false;

				 }
         
        }else{
	        AVRM_Currency_writeFileLog("[StartNoteAcceptorAllThreads()] No need to create ThreadProc1_CASHCODE_NoteAcptrPoll as max number of cash is zero.",INFO);
		    return true;
		}
        #endif

        #if !defined(NOTE_ACCEPTOR)
        return true;
        #endif
 
}//StartNoteAcceptorAllThreads() end here

//++Level 2
static  bool StartCoinAcceptorAllThreads() 
{

 
           #if defined(COIN_ACCEPTOR)
           
           if( g_maxNoOfCoin > 0 ) {


				   int returnVal=-1,CoinCreditThreadRtcode=-1,iret2=-1;

				   pthread_attr_t attr2;

				   pthread_t thread2;

				   bool lowlevelthread=true; 

				   //Start Coin Acceptor Upper Level Thread
				   returnVal = pthread_attr_init(&attr2);

				   if(0!=returnVal)
				   {
					   AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc_CoinAcptrStart Thread attribute create failed so return here from.",ERROR);
				   }

				   returnVal = pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);

				   if(0!=returnVal)
				   {

						 AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc_CoinAcptrStart Thread attribute create failed so return here from.",ERROR);
				   }

				   iret2     = pthread_create(&thread2, &attr2,&ThreadProc_CoinAcptrStart, NULL);

				   if(0==iret2)
				   {

						  lowlevelthread=true;
                          AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart Creation Success.",INFO);
						  
				   }
				   else
				   {
                          AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart Creation Failed!.",ERROR);
					      
				   }

				   returnVal = pthread_attr_destroy(&attr2);

				   if(0!=returnVal)
				   {
                     AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart thread attribute destroy failed!.",ERROR);
					 
				   }
				   
				   if( true == lowlevelthread )
				   {

					   //Now Start Coin acceptor low level thread
					   CoinCreditThreadRtcode=MONEYCONTROL_CoinAcptrCreateCreditPoll();

					   if(1!=CoinCreditThreadRtcode)
					   {

                            AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] MONEYCONTROL_CoinAcptrCreateCreditPoll thread create failed!!!.",ERROR);
                            
							return false;
							
					   }
					   else
					   {
                            AVRM_Currency_writeFileLog("[ThreadProc_acceptFare()] MONEYCONTROL_CoinAcptrCreateCreditPoll thread create successed.",INFO);
                            
							return true;

					   }
				   }
				   else if( false == lowlevelthread )
				   {

						 return false;

				   }//else if end
				   
		   }else {
			   
			   AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart thread dont need as .",INFO);
			   return true;
		   }
           #endif

           #if !defined(COIN_ACCEPTOR)
           return true;
           #endif

}//StartCoinAcceptorAllThreads() end here

static  void StartAllAcceptProcessThreads() 
{

         
                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Entry.",TRACE);

                             #if defined(NOTE_ACCEPTOR)

                             AVRM_Currency_writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart thread dont need as .",INFO);

                             //++Start Note Acceptort All Level Threads
                             pthread_mutex_lock(&noteAcptrStartFlagmutex);

                             g_noteAcptrStartFlag = ON ;
  
                             pthread_mutex_unlock(&noteAcptrStartFlagmutex);

                             pthread_mutex_lock( &g_NAULEnableThreadmutex );

                             pthread_cond_signal( &g_NAULEnableThreadCond );

                             pthread_mutex_unlock( &g_NAULEnableThreadmutex );

                             NoteAcptrStartSignalCreditPollThread();

                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Exit Enable all Note Acceptor Threads.",INFO);

                             #endif
                                                          
                             #if defined(COIN_ACCEPTOR)

                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Entry Enable all Coin Acceptor Threads.",INFO);

                             //++Start Coin Acceptort All Level Threads

                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Entry Coin Acceptor Upper Level Threads.",INFO);

                             pthread_mutex_lock(&coinAcptrStartFlagmutex);

			                 g_coinAcptrStartFlag = ON;

                             pthread_mutex_unlock(&coinAcptrStartFlagmutex);

                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Exit Coin Acceptor Upper Level Threads.",INFO);
                             
                             //++wait for signal

                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Entry Coin Acceptor Upper Level Threads Signal.",INFO);

							 pthread_mutex_lock(&g_CAULEnableThreadmutex);

							 pthread_cond_signal( &g_CAULEnableThreadCond  );

							 pthread_mutex_unlock(&g_CAULEnableThreadmutex);
							 
							 AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Exit Coin Acceptor Upper Level Threads Signal.",INFO);

			                 CoinAcptrStartSignalCreditPollThread();
                           
                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Exit Enable all Coin Acceptor Threads.",INFO);

                             #endif
                            
                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Entry Enable Acceptfare Manager Thread.",INFO);

                             //Start Acceptfare Thread
                             pthread_mutex_lock( &CloseAcceptFareThreadFlagmutex );

                             g_CloseAcceptFareThreadFlag =false;

                             pthread_mutex_unlock( &CloseAcceptFareThreadFlagmutex );
 
                             pthread_mutex_lock( &g_AcceptFareEnableThreadmutex );

                             pthread_cond_signal( &g_AcceptFareEnableThreadCond );

                             pthread_mutex_unlock( &g_AcceptFareEnableThreadmutex );
 
                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Exit Enable Acceptfare Manager Thread.",INFO);
                             
                             AVRM_Currency_writeFileLog("[StartAllAcceptProcessThreads()] Exit.",INFO);

                             return;

}//StartAllAcceptProcessThreads() end here

static  void StopAllAcceptProcessThreads(int Level) 
{


               switch(Level){

 
                             case LEVEL4:

                             #if defined(NOTE_ACCEPTOR)

                             //Stop Note Acceptort All Level Threads
                             pthread_mutex_lock(&noteAcptrStartFlagmutex);

                             g_noteAcptrStartFlag = OFF ;
  
                             pthread_mutex_unlock(&noteAcptrStartFlagmutex);

                             pthread_mutex_lock( &g_NAULEnableThreadmutex );

                             pthread_cond_signal( &g_NAULEnableThreadCond );

                             pthread_mutex_unlock( &g_NAULEnableThreadmutex );

                             NoteAcptrStopSignalCreditPollThread();

                             #endif

                             #if defined(COIN_ACCEPTOR)

                             //Stop Coin Acceptort All Level Threads
                             pthread_mutex_lock(&coinAcptrStartFlagmutex);

							 g_coinAcptrStartFlag = OFF;

                             pthread_mutex_unlock(&coinAcptrStartFlagmutex);

                             //wait for signal
							 pthread_mutex_lock(&g_CAULEnableThreadmutex);

							 pthread_cond_signal( &g_CAULEnableThreadCond  );

							 pthread_mutex_unlock(&g_CAULEnableThreadmutex);

							 CoinAcptrStopSignalCreditPollThread();

                             #endif
                            
                             //Stop Acceptfare Thread
                             pthread_mutex_lock( &CloseAcceptFareThreadFlagmutex );

                             g_CloseAcceptFareThreadFlag =true;

                             pthread_mutex_unlock( &CloseAcceptFareThreadFlagmutex );
 
                             pthread_mutex_lock( &g_AcceptFareEnableThreadmutex );

                             pthread_cond_signal( &g_AcceptFareEnableThreadCond );

                             pthread_mutex_unlock( &g_AcceptFareEnableThreadmutex );

                             
                             break;

                  
                             case LEVEL3:

                             #if defined(NOTE_ACCEPTOR)

                             //Stop Note Acceptort All Level Threads
                             pthread_mutex_lock(&noteAcptrStartFlagmutex);

                             g_noteAcptrStartFlag = OFF ;
  
                             pthread_mutex_unlock(&noteAcptrStartFlagmutex);

                             pthread_mutex_lock( &g_NAULEnableThreadmutex );

                             pthread_cond_signal( &g_NAULEnableThreadCond );

                             pthread_mutex_unlock( &g_NAULEnableThreadmutex );

                             NoteAcptrStopSignalCreditPollThread();

                             #endif

                             #if defined(COIN_ACCEPTOR)

                             //Stop Coin Acceptort All Level Threads
                             pthread_mutex_lock(&coinAcptrStartFlagmutex);

							 g_coinAcptrStartFlag = OFF;

                             pthread_mutex_unlock(&coinAcptrStartFlagmutex);

                             //wait for signal
							 pthread_mutex_lock(&g_CAULEnableThreadmutex);

							 pthread_cond_signal( &g_CAULEnableThreadCond  );

							 pthread_mutex_unlock(&g_CAULEnableThreadmutex);

							 CoinAcptrStopSignalCreditPollThread();

                             #endif
                                                     
                             break;

                             case LEVEL2:

                             #if defined(NOTE_ACCEPTOR)

                             //Stop Note Acceptort All Level Threads
                             pthread_mutex_lock(&noteAcptrStartFlagmutex);

                             g_noteAcptrStartFlag = OFF ;
  
                             pthread_mutex_unlock(&noteAcptrStartFlagmutex);

                             pthread_mutex_lock( &g_NAULEnableThreadmutex );

                             pthread_cond_signal( &g_NAULEnableThreadCond );

                             pthread_mutex_unlock( &g_NAULEnableThreadmutex );

                             NoteAcptrStopSignalCreditPollThread();

                             #endif

                             #if defined(COIN_ACCEPTOR)

                             //Stop Coin Acceptort All Level Threads
                             pthread_mutex_lock(&coinAcptrStartFlagmutex);

							 g_coinAcptrStartFlag = OFF;

                             pthread_mutex_unlock(&coinAcptrStartFlagmutex);

                             //wait for signal
							 pthread_mutex_lock(&g_CAULEnableThreadmutex);

							 pthread_cond_signal( &g_CAULEnableThreadCond  );

							 pthread_mutex_unlock(&g_CAULEnableThreadmutex);

							 CoinAcptrStopSignalCreditPollThread();

                             #endif
                            
                             break;

                             case LEVEL1:

                             #if defined(NOTE_ACCEPTOR)

                             //Stop Note Acceptort All Level Threads
                             pthread_mutex_lock(&noteAcptrStartFlagmutex);

                             g_noteAcptrStartFlag = OFF ;
  
                             pthread_mutex_unlock(&noteAcptrStartFlagmutex);

                             pthread_mutex_lock( &g_NAULEnableThreadmutex );

                             pthread_cond_signal( &g_NAULEnableThreadCond );

                             pthread_mutex_unlock( &g_NAULEnableThreadmutex );

                             NoteAcptrStopSignalCreditPollThread();

                             #endif

                             break;

           };//switch end

           Wait( 2 );

           return;


}//StopAllAcceptProcessThreads() end here

static  void DisableAllAcceptorV2() 
{
   
          #if defined(COIN_ACCEPTOR)
          DisableAllCoins();
	      #endif

          #if  defined (NOTE_ACCEPTOR)
          IssuedisableAllBillCmd();
          #endif

}//DisableAllAcceptorV2() end here

static  void DisableAllAcceptor() 
{
                  
           AVRM_Currency_writeFileLog("[DisableAllAcceptor()] Start.",TRACE);
           
           #if defined(COIN_ACCEPTOR)
          
           if( g_maxNoOfCoin > 0 ) {
			  
			  if( 1 == WaitforCoinAcptrLowlevelflagClosed(WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD) ) {
			     DisableAllCoins();
		      }//if end
		      
           }//if end
           #endif

           #if  defined (NOTE_ACCEPTOR) 
           if( g_maxNoOfCash > 0 ){ 
			   if( 1 == WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD)) {
			       IssuedisableAllBillCmd();
			   }//if end
			  
           }//if end
           #endif
          
           AVRM_Currency_writeFileLog("[DisableAllAcceptor()] Exit.",TRACE);

	       delay_mSec(200);
	       
}//DisableAllAcceptor() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  int makeAccepters_at_ActiveteMode(int fare,int maxNoteQtyAllowed,int maxCoinQtyAllowed) 
{
          
		  int stateNote = FAIL; 
		  int stateCoin = FAIL;
  
          pthread_mutex_lock(&g_APIEnableTheseDenominationsmutex );
          
          if( false == g_APIEnableTheseDenominationsFlag  ) {
			AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode()] g_APIEnableTheseDenominationsFlag flag set to false",INFO);
          }else  if( true == g_APIEnableTheseDenominationsFlag  ){
			AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode()] g_APIEnableTheseDenominationsFlag flag set to true.",INFO);
          }//else if end
          
          //++FOR COIN ACCEPTOR
          if( false == g_APIEnableTheseDenominationsFlag  ){
			      
			      #if defined (COIN_ACCEPTOR)
			      
			      if( g_maxNoOfCoin > 0 ) {
					  
					  if( FAIL != enableSpecificCoinsAndMakeIdle(fare,maxCoinQtyAllowed) ){
						  
						  
						  AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode()] Coin acceptor activate successed.",INFO);
						  stateCoin = SUCCESS;
						  
					  }else{
						  
						 
						 AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode()] Coin acceptor activate failed.",INFO);
						 return FAIL;
						 
					  }//else end
					  
					 
                  }else {
					 AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode()] Coin acceptor activate dont need as max coin set to zero.",INFO);
				     stateCoin = SUCCESS;
			      }//else end
			      
				  #endif
				  
				  #if !defined (COIN_ACCEPTOR)
				  stateCoin = SUCCESS;
				  #endif
				  
			  
		  } else if( true == g_APIEnableTheseDenominationsFlag  ) {

                #if defined (COIN_ACCEPTOR)
                
                if( g_maxNoOfCoin > 0 ){
					
					if(  FAIL != EnableCoinAcceptor(g_COIN) ){
						
						   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] Coin acceptor activate successed.",INFO);
						   stateCoin = SUCCESS;
						   
					}else{
						  
						  AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] Coin acceptor activate failed.",ERROR);
						  return FAIL;
						  
					}//else end
					
				} else {
					 
					 AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] Coin acceptor activate do not need as max number of coin acceptor is zero.",ERROR);
					 stateCoin = SUCCESS;
					 
				}//else end
				
				#endif
				
				#if !defined (COIN_ACCEPTOR)
				stateCoin = SUCCESS;
				#endif
				
          }//else end
          
		  //++FOR NOTE ACCEPTOR
          if( false == g_APIEnableTheseDenominationsFlag  ) {
				  
				  #if defined (NOTE_ACCEPTOR)
				    
				    #ifdef B2B_NOTE_ACCEPTOR
					  if( g_maxNoOfCash > 0 ) {
						  if(  FAIL != EnableNoteAcceptor(g_NOTE) ){
							   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] B2B Note acceptor activate successed.",INFO);
							   stateNote = SUCCESS;
						  }else
						  {
							  AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] B2B Note acceptor activate failed.",ERROR);
							  stateNote = FAIL;
						  }	
					  }	 else {
						   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] B2B Note acceptor activate do not need as max number cash is zero.",INFO);
						   stateNote = SUCCESS;
						  
					  }//else end 				  
					  #endif
					  
				    #ifdef JCM_NOTE_ACCEPTOR
					  if( g_maxNoOfCash > 0 ) {
						  if(  1 == EnableNoteAcceptor(g_NOTE) ){
							   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] JCM Note acceptor activate successed.",INFO);
							   stateNote = SUCCESS;
						  }else{
							  AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] JCM Note acceptor activate failed.",ERROR);
							  stateNote = FAIL;
						  }//else end
						  
					  } else {
						   
						   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] JCM Note acceptor activate do not need as max number cash is zero.",INFO);
						   stateNote = SUCCESS;
						  
					  }//else end
					  #endif
					  
				  #endif
				  
				  #if !defined (NOTE_ACCEPTOR)
				  stateNote = SUCCESS;
				  #endif
				  
          
          }else if( true == g_APIEnableTheseDenominationsFlag  ) {

                  #if defined (NOTE_ACCEPTOR)
                  
					  #ifdef B2B_NOTE_ACCEPTOR
					  if( g_maxNoOfCash > 0 ) {
						  if(  FAIL != EnableNoteAcceptor(g_NOTE) ){
							   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] B2B Note acceptor activate successed.",INFO);
							   stateNote = SUCCESS;
						  }else
						  {
							  AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] B2B Note acceptor activate failed.",ERROR);
							  stateNote = FAIL;
						  }	
					  }	 else {
						   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] B2B Note acceptor activate do not need as max number cash is zero.",INFO);
						   stateNote = SUCCESS;
						  
					  }//else end 				  
					  #endif
					  
					  #ifdef JCM_NOTE_ACCEPTOR
					  
					  if( g_maxNoOfCash > 0 ) {
						  
						  //++Added on 29-Oct-12
						  if(  1 == EnableNoteAcceptor(g_NOTE) ){
							   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] JCM Note acceptor activate successed.",INFO);
							   stateNote = SUCCESS;
						  }else{
							  AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] JCM Note acceptor activate failed.",ERROR);
							  stateNote = FAIL;
						  }//else end
						  
					  } else {
						   AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode() With API Enable] JCM Note acceptor activate do not need as max number cash is zero.",INFO);
						   stateNote = SUCCESS;
						  
					  }//else end
					  
					  #endif
				  
				  #endif
				  
				  #if !defined (NOTE_ACCEPTOR)
				  stateNote = SUCCESS;
				  #endif
		
         }//else if end

                         
         if( true == g_APIEnableTheseDenominationsFlag  ){         
			g_APIEnableTheseDenominationsFlag = false;
         }//if( true == g_APIEnableTheseDenominationsFlag  ) end
         
         pthread_mutex_unlock(&g_APIEnableTheseDenominationsmutex );

	     if( (SUCCESS == stateNote ) && ( SUCCESS == stateCoin )){
			AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode()] Device enable denom successed.",INFO);
	        return (SUCCESS);
         }else{
			AVRM_Currency_writeFileLog("[makeAccepters_at_ActiveteMode()] Any one or Both Device enable denom failed.",INFO);
	        return (FAIL);
         }//else end

         
}//makeAccepters_at_ActiveteMode() end here

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NOTE_ACCEPTOR)
void IncrementNotesCounter(int NoteValue) 
{

       switch(NoteValue) {

            case 5:  g_NotesCounter.N5++;
                     break;

            case 10: g_NotesCounter.N10++;
                     break;

            case 20: g_NotesCounter.N20++;
                     break;

            case 50: g_NotesCounter.N50++;
                     break;

            case 100: g_NotesCounter.N100++;
                      break;     
   
            case 500: g_NotesCounter.N500++;
                      break; 
       
            case 1000: g_NotesCounter.N1000++;
                       break;      

      } 

      return;

}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(COIN_ACCEPTOR)
void IncrementCoinsCounter(int CoinValue) 
{


       switch(CoinValue)
       {

            case 1:  g_CoinsCounter.C1++;
                     break;

            case 5:  g_CoinsCounter.C5++;
                     break;

            case 10: g_CoinsCounter.C10++;
                     break;
            
      };

      return;

}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(COIN_ACCEPTOR)

void ClearCoinsCounter(){

        g_CoinsCounter.C1=0;
  
        g_CoinsCounter.C5=0;
   
        g_CoinsCounter.C10=0;
       
        return;

}//void ClearCoinsCounter() end

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NOTE_ACCEPTOR)

void ClearNotesCounter() {
       

            g_NotesCounter.N5=0;
                    
            g_NotesCounter.N10=0;
                     
            g_NotesCounter.N20=0;
                     
            g_NotesCounter.N50=0;
                     
            g_NotesCounter.N100=0;
                           
            g_NotesCounter.N500=0;
                           
            g_NotesCounter.N1000=0;

            return;


}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LogThisTransData() {

          char log[200];

          memset(log,'\0',200);

          #if defined(NOTE_ACCEPTOR)

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.5 Notes: %d .Total Rs.5 Notes Value= %d .", g_NotesCounter.N5,(g_NotesCounter.N5*5) );
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);
          
          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.10 Notes: %d .Total Rs.10 Notes Value= %d .", g_NotesCounter.N10,(g_NotesCounter.N10*10));
          //writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);
          
          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.20 Notes: %d .Total Rs.20 Notes Value= %d .", g_NotesCounter.N20,(g_NotesCounter.N20*20));
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.50 Notes: %d .Total Rs.50 Notes Value= %d .", g_NotesCounter.N50,(g_NotesCounter.N50*50));
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.100 Notes: %d .Total Rs.100 Notes Value= %d .", g_NotesCounter.N100,(g_NotesCounter.N100*100));
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.500 Notes: %d .Total Rs.500 Notes Value= %d .", g_NotesCounter.N500,(g_NotesCounter.N500*500));
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);
         
          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.1000 Notes: %d .Total Rs.1000 Notes Value= %d .", g_NotesCounter.N1000,(g_NotesCounter.N1000*1000));
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);

          #endif

          #if defined(COIN_ACCEPTOR)

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.1 Coins: %d .Total Rs.1 Coins Value: %d .", g_CoinsCounter.C1,(g_CoinsCounter.C1*1) );
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.5 Coins: %d .Total Rs.5 Coins Value: %d .", g_CoinsCounter.C5,(g_CoinsCounter.C5*5));
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.10 Coins: %d .Total Rs.10 Coins Value: %d .", g_CoinsCounter.C10,(g_CoinsCounter.C10*10));
          ////writeFileLoglog);
          AVRM_Currency_writeFileLog(log,INFO);

          #endif

          #if defined(NOTE_ACCEPTOR)

          int TotalNoteValue=0;

          TotalNoteValue= ( (g_NotesCounter.N5*5)+(g_NotesCounter.N10*10)+(g_NotesCounter.N20*20)+(g_NotesCounter.N50*50)+
          (g_NotesCounter.N100*100)+(g_NotesCounter.N500*500)+(g_NotesCounter.N1000*1000) );

          memset(log,'\0',200);

          sprintf(log,"[LogThisTransData()] Total Notes Value= %d .", TotalNoteValue );

          ////writeFileLoglog);
          
          AVRM_Currency_writeFileLog(log,INFO);

          #endif

          #if defined(COIN_ACCEPTOR)

          int TotalCoinValue=0;

          TotalCoinValue = ( (g_CoinsCounter.C1*1)+(g_CoinsCounter.C5*5)+(g_CoinsCounter.C10*10) );

          memset(log,'\0',200);

          sprintf(log,"[LogThisTransData()] Total Coins Value= %d .", TotalCoinValue );

          ////writeFileLoglog);
          
          AVRM_Currency_writeFileLog(log,INFO);

          #endif

          return;

}//LogThisTransData() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NOTE_ACCEPTOR)

//Note Acceptor Thread
void* ThreadProc_NoteAcptrStart(void *ptr){   
 
  //wait for signal
  pthread_mutex_lock( &g_NAULEnableThreadmutex );

  pthread_cond_wait( &g_NAULEnableThreadCond , &g_NAULEnableThreadmutex );

  pthread_mutex_unlock( &g_NAULEnableThreadmutex );

  pthread_mutex_lock(&noteAcptrStartFlagmutex);

  if( OFF == g_noteAcptrStartFlag ) {

       pthread_mutex_unlock(&noteAcptrStartFlagmutex);
       AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Exit.",ERROR);
       pthread_exit(0);

  }else{
	   pthread_mutex_unlock(&noteAcptrStartFlagmutex);
       AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Running.",INFO);
  }//else end

  int ret=0,noteValue=0, noteStatus=0x00;
 
  g_totalNoteAmountAccepted = 0;

  g_totalAmountAccepted     = 0;

  int NoteEvent=0,PrevNoteEvent=0;

  //malay add 6 aprl 2013
  char log[200];
  memset(log,'\0',200);

  int NACurrentfare=0;


  while(1) 
  {

            pthread_mutex_lock(&noteAcptrStartFlagmutex);

		    if( ON == g_noteAcptrStartFlag  ) {

			    
                    pthread_mutex_unlock(&noteAcptrStartFlagmutex);

		            noteValue  =  0;
		            noteStatus = 0x00;
		            ret=0;
		             
		            #ifdef B2B_NOTE_ACCEPTOR 
			        SetCurrentfare( g_Fare - g_totalAmountAccepted );
		            #endif

		            #if defined(JCM_NOTE_ACCEPTOR) || defined(BNA_NOTE_ACCEPTOR)
		            int CurrentFare=0;
		            pthread_mutex_lock(&mut);
		            CurrentFare=g_Fare - g_totalAmountAccepted;
		            pthread_mutex_unlock(&mut); 
                    SetCurrentfare( CurrentFare );
		            #endif
		           
		            #ifdef B2B_NOTE_ACCEPTOR 
		            ret = GetNoteDetailInEscrow(&noteValue, &noteStatus);
		            if( (noteValue >= 5)  &&  (noteValue <= 2000) ) {
						memset(log,'\0',200);
						sprintf(log,"[ThreadProc_NoteAcptrStart()] Note value Rs. %d .",noteValue );
						AVRM_Currency_writeFileLog(log,INFO);
						//++memset(log,'\0',200);
						//++sprintf(log,"[ThreadProc_NoteAcptrStart()] Note  Rs. %d .",noteStatus );
						//++AVRM_Currency_writeFileLog(log,INFO);
				    }//++if end
		            #endif

		            #ifdef JCM_NOTE_ACCEPTOR 
		            noteValue=GetNoteDetailInEscrow();
		            #endif

		            #ifdef BNA_NOTE_ACCEPTOR 
		            noteValue=GetNoteDetailInEscrow();
		            #endif

		            #ifdef B2B_NOTE_ACCEPTOR 
		            //++if( ( SUCCESS == ret ) && ( CASHCODE_RES_STACKED == noteStatus ) && ( (noteValue >= 5)  &&  (noteValue <= 2000) ) )
		            if( (noteValue >= 5)  &&  (noteValue <= 2000) )     
		            #endif
		            #ifdef JCM_NOTE_ACCEPTOR 
		            if( (noteValue >= 5)  &&  (noteValue <= 2000) ) 
		            #endif
		            #ifdef BNA_NOTE_ACCEPTOR 
		            if( (noteValue >= 5)  &&  (noteValue <= 2000) ) 
		            #endif
			        {

									   pthread_mutex_lock(&mut);   

									   g_totalNoteAmountAccepted += noteValue;
											   
									   g_totalAmountAccepted += noteValue;

									   g_ttlNmbrOfNote +=1; 

									   memset(log,'\0',200);

									   sprintf(log,"[ThreadProc_NoteAcptrStart()] Note value Rs. %d .",noteValue );

									   AVRM_Currency_writeFileLog(log,INFO);
									   
									   memset(log,'\0',200);

									   sprintf(log,"[ThreadProc_NoteAcptrStart()] Before Increment Note Array Index : %d .", g_NotedenomIndex);
											               
									   AVRM_Currency_writeFileLog(log,INFO);
									   
                                       switch(noteValue) {

                                          case 5: 
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 1;
                                                 break;
					      
                                          case 10:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 2;
                                                 break;

                                          case 20:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 3;
                                                 break;

                                          case 50:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 4;
                                                 break;

                                          case 100:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 5;
                                                 break;
 
                                         case 200:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 6;
                                                 break;
                                                 
                                         case 500:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 7;
                                                 break;
                                                 
                                         case 1000:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 8;
                                                 break;
                                         
                                         case 2000:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 9;
                                                 break;

										 default:
                                                 g_acpted_CurrencyDetail[1][g_NotedenomIndex]= 0;
                                                 break;

                                       };//++switch end
                                       
                                       memset(log,'\0',200);

							           sprintf(log,"[ThreadProc_NoteAcptrStart()] Denom Array Coin Value : %d .",(int) g_acpted_CurrencyDetail[1][g_NotedenomIndex]);
											               
									   AVRM_Currency_writeFileLog(log,INFO); 
                                       
									   g_NotedenomIndex+=1;    
		                       
                                       IncrementNotesCounter(noteValue);
                                       
                                       memset(log,'\0',200);

									   sprintf(log,"[ThreadProc_NoteAcptrStart()] After Increment Note Array Index : %d .", g_NotedenomIndex);
											               
									   AVRM_Currency_writeFileLog(log,INFO);
                           
									   memset(log,'\0',200);
		                       
		                               sprintf(log,"[ThreadProc_NoteAcptrStart()] Accepted Note : Rs. %d  Current Accepted Ammount : Rs. %d .", noteValue,g_totalAmountAccepted);
									   
									   AVRM_Currency_writeFileLog(log,INFO);

									   //++clear array
									   memset(log,'\0',200);
									   sprintf(log,"[ThreadProc_NoteAcptrStart()] Total Notes Number : %d .", g_ttlNmbrOfNote);
									   AVRM_Currency_writeFileLog(log,INFO);
									   
									   //++printf("[ThreadProc_NoteAcptrStart()] Accepted Note : Rs. %d  Current Accepted Ammount : Rs. %d .", noteValue,g_totalAmountAccepted);
									   //++printf("[ThreadProc_NoteAcptrStart()] Total Notes Number : %d .", g_ttlNmbrOfNote);
				       
									   //++Store Event
									   PrevNoteEvent=NoteEvent;

									   NoteEvent=NoteEvent+1;

									   pthread_mutex_unlock(&mut); 

				      
			    }//End if : ( CASHCODE_RES_STACKED / CASHCODE_RES_ESCROW)
		    

                pthread_mutex_lock(&RuntimeInhibitFlagmutex);

			    //++Runtime Inhibit
			    if( (NoteEvent>PrevNoteEvent)  && (1==g_RuntimeInhibitFlag) )
			    {

                                       
                               pthread_mutex_unlock(&RuntimeInhibitFlagmutex);

							   //Runtime Inhibit lock
							   pthread_mutex_lock(&Inhibitmutex); //Malay add 1 feb 2013

		                       #ifdef ATVM_DEBUG

		                       //++//writeFileLog"[ThreadProc_NoteAcptrStart()] Going to disable specific coins and notes.");
		                       
		                       AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Going to disable specific coins and notes.",INFO);
		                       
		                       //++clear log array
		                       
		                       memset(log,'\0',200);	       

		                       sprintf(log,"[ThreadProc_NoteAcptrStart()]  Fare : Rs. %d  Current Accepted Fare: Rs. %d .", g_Fare,g_totalAmountAccepted);

							   //writeFileLoglog);
							   
							   AVRM_Currency_writeFileLog(log,INFO);
							   
		                       #endif

                               pthread_mutex_lock(&mut);
 
		                       //Calculate current fare to be accept
		                       NACurrentfare=g_Fare - g_totalAmountAccepted;

                               pthread_mutex_unlock(&mut);

		                       //#ifdef ATVM_DEBUG

		                       memset(log,'\0',200);	       

		                       sprintf(log,"[ThreadProc_NoteAcptrStart()] Current Fare to be accept: Rs. %d .", NACurrentfare);

							   AVRM_Currency_writeFileLog(log,INFO);

		                       //#endif
	 
                               pthread_mutex_lock(&mut);

		                       //Runtime all coin and note inhibit
		                       if( ( ( g_maxNoOfCash > 0 ) && (g_ttlNmbrOfNote >= g_maxNoOfCash) )             || 
								   ( ( g_maxNoOfCoin > 0 ) && (g_ttlNmbrOfCoin >= g_maxNoOfCoin) )             ||
								   ( THREAD_KILL == g_ExternalKillAcptFareThread )                             ||
								   ( true == g_transactiontimeout )                                            ||
								   ( g_Fare == g_totalAmountAccepted )                                         ||
								   ( g_totalAmountAccepted > g_Fare )
								) {
		                            
                                                pthread_mutex_unlock(&mut);

												if( false == g_AllAlreadyDisableFlag ){

													  g_AllAlreadyDisableFlag=true;

													  #if defined(COIN_ACCEPTOR)
													  SetAllCoinInhibitStatus(); 
													  #endif

													  #if defined(NOTE_ACCEPTOR)
													  InhibitNoteAcptr();  
													  #endif

													  //#ifdef ATVM_DEBUG  
													  ////writeFileLog"[ThreadProc_NoteAcptrStart()] Inhibit from Note Event.");
													  AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Inhibit from Note Event.",INFO);
													  //#endif

												}//if( false == g_AllAlreadyDisableFlag ) end


		                       }
		                       else //normal
		                       {
                                       pthread_mutex_unlock(&mut);

									   #if defined(COIN_ACCEPTOR)       
		                               if( 0 ==  g_CoinFullDisable )
		                               {
		                                     //++Runtime coin inhibit
											 SetSpecificCoinInhibitStatus(NACurrentfare);
		                               }
		                               #endif

		                               #if defined(NOTE_ACCEPTOR) 
		                               if( 0 ==  g_NoteFullDisable )
		                               {
						                     //++Runtime Note inhibit
											 SetSpecificNoteInhibitStatus(NACurrentfare);
		                               }
		                               #endif


		                       }

							   NoteEvent=0;
							   PrevNoteEvent=0;

		                       #ifdef ATVM_DEBUG
		                       ////writeFileLog"[ThreadProc_NoteAcptrStart()] Successfully disable specific coins and notes.");
		                       AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Successfully disable specific coins and notes.",INFO);
		                       #endif

		                       pthread_mutex_unlock(&Inhibitmutex); //Malay add 1 feb 2013


							}//if( (NoteEvent>PrevNoteEvent)  && (1==g_RuntimeInhibitFlag) )
                            else
                            {
                                    pthread_mutex_unlock(&RuntimeInhibitFlagmutex);

                            }
		           
                           //External Coin Acceptor Inhibit Singal Check
		                   pthread_mutex_lock(&ExternalInhibitmutex);

                           pthread_mutex_lock(&mut);

				   //++Runtime all coin and note inhibit
				   if(( (g_maxNoOfCash > 0 ) && (g_ttlNmbrOfNote >= g_maxNoOfCash) )               || 
				     ( ( g_maxNoOfCoin > 0 ) && (g_ttlNmbrOfCoin >= g_maxNoOfCoin) )                ||
				     (  THREAD_KILL == g_ExternalKillAcptFareThread )                               ||
				     (  true == g_transactiontimeout )                                              ||
				     (  g_Fare == g_totalAmountAccepted )                                           ||
				     (  g_totalAmountAccepted > g_Fare )
				     )
				     {
                                            pthread_mutex_unlock(&mut);
				                    
				                            if( false == g_AllAlreadyDisableFlag ) {
												
		                                            g_AllAlreadyDisableFlag=true;
	 
		                                            #if defined(COIN_ACCEPTOR)
		                                            SetAllCoinInhibitStatus(); 
		                                            #endif

		                                            #if defined(NOTE_ACCEPTOR)
						                            InhibitNoteAcptr();  
		                                            #endif
		                                        
		                                            //#ifdef ATVM_DEBUG
		                                            AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Inhibit from Note External.",INFO);
		                                            //#endif

		                                    }//if( false == g_AllAlreadyDisableFlag )   


				     }else{
                           pthread_mutex_unlock(&mut);
                     }

		             pthread_mutex_unlock(&ExternalInhibitmutex);

	     
	       }//End if g_noteAcptrStartFlag == ON   
		   else if( OFF == g_noteAcptrStartFlag )
		   {
			   pthread_mutex_unlock(&noteAcptrStartFlagmutex);
			   AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Exit.",INFO);
			   pthread_exit(0);

		   }//else if
     
  }//End While(1)    	
}//ThreadProc_NoteAcptrStart() end here

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(COIN_ACCEPTOR)

//Coin Acceptor Thread
void* ThreadProc_CoinAcptrStart(void *ptr) { 

  //wait for signal
  pthread_mutex_lock(&g_CAULEnableThreadmutex);

  pthread_cond_wait( &g_CAULEnableThreadCond , &g_CAULEnableThreadmutex );

  pthread_mutex_unlock(&g_CAULEnableThreadmutex);
 
  pthread_mutex_lock(&coinAcptrStartFlagmutex);

  if( OFF == g_coinAcptrStartFlag ) {

       pthread_mutex_unlock(&coinAcptrStartFlagmutex);

       //writeFileLog"[ThreadProc_CoinAcptrStart()] Thread Exit."); 
       
       AVRM_Currency_writeFileLog("[ThreadProc_CoinAcptrStart()] Thread Exit.",ERROR);

       pthread_exit(0);

  }else{

        pthread_mutex_unlock(&coinAcptrStartFlagmutex);
        AVRM_Currency_writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Running.",INFO);
  }//else end

  int ret=0,i=0,coinValue[5]={0},coinStatus=0x00;

  g_totalCoinAmountAccepted = 0;

  //Malay Add 30 Jan 2013  
  int currentfare=0;
  int CoinEvent=0,PrevCoinEvent=0;
  
  //malay add 6 aprl 2013
  char log[200];

  memset(log,'\0',200);
  
  int CACurrentfare=0;
  

  while(1) {
  
   
	pthread_mutex_lock(&coinAcptrStartFlagmutex);

	if( ON == g_coinAcptrStartFlag  )
	{

				   pthread_mutex_unlock(&coinAcptrStartFlagmutex);

                   for(i = 0;i<5;i++){
						coinValue[i] = 0;
                   }//for end
		   
                   coinStatus = 0x00;

                   g_CoinEventFlag=false;

				   GetCoinDetailInEscrow(coinValue,&coinStatus);

                   if(  COINACPTR_RES_ESCROW == coinStatus  ){
			 
                     pthread_mutex_lock(&mut);

					 for(i=0;i<5;i++) {
					 

							 //++store coin information
							 if(coinValue[i] > 0) {
							 														
															g_totalCoinAmountAccepted += coinValue[i] ;

															g_totalAmountAccepted     += coinValue[i] ;
																	
															memset(log,'\0',200);

									                        sprintf(log,"[ThreadProc_CoinAcptrStart()] Coin value Rs. %d .",coinValue[i] );

									                        AVRM_Currency_writeFileLog(log,INFO);
									                        
									                        memset(log,'\0',200);

														    sprintf(log,"[ThreadProc_CoinAcptrStart()] Before Increment Coin Array Index : %d .", g_CoindenomIndex);
											               
											                AVRM_Currency_writeFileLog(log,INFO);
									   
                                                            switch(coinValue[i]) {
                                                                  
                                                                  case 1: 
																		 g_acpted_CurrencyDetail[0][g_CoindenomIndex]= 1;
																		 break;
												  
																  case 2:
																		 g_acpted_CurrencyDetail[0][g_CoindenomIndex]= 2;
																		 break;
																		 
																  case 5: 
																		 g_acpted_CurrencyDetail[0][g_CoindenomIndex]= 3;
																		 break;
												  
																  case 10:
																		 g_acpted_CurrencyDetail[0][g_CoindenomIndex]= 4;
																		 break;

																  default:
																		 g_acpted_CurrencyDetail[0][g_CoindenomIndex]= 0;
																		 break;

															};//++switch end
															
															memset(log,'\0',200);

														    sprintf(log,"[ThreadProc_CoinAcptrStart()] Denom Array Coin Value : %d .",(int) g_acpted_CurrencyDetail[0][g_CoindenomIndex]);
											               
											                AVRM_Currency_writeFileLog(log,INFO); 
								 
															g_CoindenomIndex+=1;
														 
															g_ttlNmbrOfCoin +=1; 

															IncrementCoinsCounter(coinValue[i]);
																											 
															//#ifdef ATVM_DEBUG
															
															memset(log,'\0',200);

														    sprintf(log,"[ThreadProc_CoinAcptrStart()] After Increment Coin Array Index : %d .", g_CoindenomIndex);
											               
											                AVRM_Currency_writeFileLog(log,INFO);

															memset(log,'\0',200);

															sprintf(log,"[ThreadProc_CoinAcptrStart()] Current Coin Accepted: Rs. %d Current Accepted Ammount: Rs. %d .",coinValue[i],g_totalAmountAccepted); 
			  
															AVRM_Currency_writeFileLog(log,INFO);
															
															//#endif

															g_CoinEventFlag=true;

														    //++clear array
														    memset(log,'\0',200);

														    sprintf(log,"[ThreadProc_CoinAcptrStart()] Total Coins Number : %d .", g_ttlNmbrOfCoin);
											               
											                AVRM_Currency_writeFileLog(log,INFO);
											               
											                //printf("[ThreadProc_CoinAcptrStart()] Current Coin Accepted: Rs. %d Current Accepted Ammount: Rs. %d .",coinValue[i],g_totalAmountAccepted); 
											                //printf("[ThreadProc_CoinAcptrStart()] Total Coins Number : %d .", g_ttlNmbrOfCoin);
														    //clear array
														    memset(log,'\0',200);

						   
							  } //if end

					  
			          }  //for end

                             
		           
					   //Store Event
					   if( true == g_CoinEventFlag){
							  PrevCoinEvent=CoinEvent;
							  CoinEvent=CoinEvent+1;
					   }//if end

		               pthread_mutex_unlock(&mut);

	            } // End if:(coinStatus == COINACPTR_RES_ESCROW)

               pthread_mutex_lock(&RuntimeInhibitFlagmutex);

	       //Runtime Inhibit if block start
           if( (CoinEvent>PrevCoinEvent) && (1==g_RuntimeInhibitFlag) )
	       {
			       
                               pthread_mutex_unlock(&RuntimeInhibitFlagmutex);

                               //Runtime Inhibit lock
							   pthread_mutex_lock(&Inhibitmutex); 

                               #ifdef ATVM_DEBUG
                               //writeFileLog"[ThreadProc_CoinAcptrStart()] Going to disable specific coins and notes.");
                               
                               //clear log array
                               memset(log,'\0',200);	       

                               sprintf(log,"[ThreadProc_CoinAcptrStart()]  Fare : Rs. %d  Current Accepted Fare: Rs. %d .", g_Fare,g_totalAmountAccepted);

							   AVRM_Currency_writeFileLog(log,INFO);
							   
                               #endif

                               pthread_mutex_lock(&mut);

                               //Calculate current fare to be collect
                               CACurrentfare= g_Fare - g_totalAmountAccepted;

                               pthread_mutex_unlock(&mut);

                               //#ifdef ATVM_DEBUG

                               memset(log,'\0',200);	       

                               sprintf(log,"[ThreadProc_CoinAcptrStart()] Fare to be accept: Rs. %d .",CACurrentfare );

							   AVRM_Currency_writeFileLog(log,INFO);

                               //#endif

                               pthread_mutex_lock(&mut);

                               //Runtime all coin and note inhibit
							   if(   ((g_maxNoOfCash > 0 ) && (g_ttlNmbrOfNote >= g_maxNoOfCash) )               || 
									 (( g_maxNoOfCoin > 0 ) && (g_ttlNmbrOfCoin >= g_maxNoOfCoin) )           	 ||
									 ( THREAD_KILL == g_ExternalKillAcptFareThread )                             ||
									 ( true == g_transactiontimeout )                                            ||
									 ( g_Fare == g_totalAmountAccepted )                                         ||
									 ( g_totalAmountAccepted > g_Fare )     
								)
                               {
                                    
                                    pthread_mutex_unlock(&mut);

                                    if( false == g_AllAlreadyDisableFlag )
                                    {
                                            g_AllAlreadyDisableFlag=true;

                                            #if defined(COIN_ACCEPTOR)
                                            SetAllCoinInhibitStatus(); 
                                            #endif

                                            #if defined(NOTE_ACCEPTOR)
											InhibitNoteAcptr();  
                                            #endif

                                            //#ifdef ATVM_DEBUG  
                                            ////writeFileLog"[ThreadProc_CoinAcptrStart()] Inhibit from Coin Event.");
                                            AVRM_Currency_writeFileLog("[ThreadProc_CoinAcptrStart()] Inhibit from Coin Event.",INFO);
                                            //#endif

                                    }

                               }
                               else
                               {

                                       pthread_mutex_unlock(&mut);

                                       #if defined(COIN_ACCEPTOR)
                                       if( 0 ==  g_CoinFullDisable )
                                       {
                                             //++Runtime coin inhibit
                                             SetSpecificCoinInhibitStatus(CACurrentfare);
                                       }
                                       #endif

                                       #if defined(NOTE_ACCEPTOR)
                                       if( 0 ==  g_NoteFullDisable )
                                       {
											 //++Runtime Note inhibit
											 SetSpecificNoteInhibitStatus(CACurrentfare);
                                       }
                                       #endif

			       }      
 
			       CoinEvent=0;

			       PrevCoinEvent=0;

                   #ifdef ATVM_DEBUG
			       ////writeFileLog"[ThreadProc_CoinAcptrStart()] Successfully disable specific coins and notes.");
			       AVRM_Currency_writeFileLog("[ThreadProc_CoinAcptrStart()] Successfully disable specific coins and notes.",INFO);
                   #endif

			       pthread_mutex_unlock(&Inhibitmutex); 
			       
			       
            } //Runtime Inhibit if block end
            else
            {
                  pthread_mutex_unlock(&RuntimeInhibitFlagmutex);
      
            }

            //External Note Acceptor Inhibit Singal Check
            pthread_mutex_lock(&ExternalInhibitmutex);

            pthread_mutex_lock(&mut);

		    //Runtime all coin and note inhibit
		    if((( g_maxNoOfCash > 0 ) && (g_ttlNmbrOfNote >= g_maxNoOfCash) )               || 
		      ( ( g_maxNoOfCoin > 0 ) && (g_ttlNmbrOfCoin >= g_maxNoOfCoin) )               ||
		      (  THREAD_KILL == g_ExternalKillAcptFareThread )                              ||
		      (  true == g_transactiontimeout )                                             ||
		      (  g_Fare == g_totalAmountAccepted )                                          ||
		      (  g_totalAmountAccepted > g_Fare )
		     )
		     {
		          pthread_mutex_unlock(&mut);

		          if( false == g_AllAlreadyDisableFlag )
                  {
                                g_AllAlreadyDisableFlag=true;

                                #if defined(COIN_ACCEPTOR)
                                SetAllCoinInhibitStatus(); 
                                #endif

                                #if defined(NOTE_ACCEPTOR)
								InhibitNoteAcptr();    
                                #endif

                                //#ifdef ATVM_DEBUG
                                ////writeFileLog"[ThreadProc_CoinAcptrStart()] Inhibit from External Coin Event."); 
                                AVRM_Currency_writeFileLog("[ThreadProc_CoinAcptrStart()] Inhibit from External Coin Event.",INFO);    
                                //#endif             
                          }

		          }
                  else
                  {
                      pthread_mutex_unlock(&mut);
                  }

                 pthread_mutex_unlock(&ExternalInhibitmutex); 


	      } // End if: g_coinAcptrStartFlag == ON
          else if ( OFF == g_coinAcptrStartFlag )
          { 
              pthread_mutex_unlock(&coinAcptrStartFlagmutex);
              ////writeFileLog"[ThreadProc_CoinAcptrStart()] Thread exit.");
              AVRM_Currency_writeFileLog("[ThreadProc_CoinAcptrStart()] Thread exit.",INFO); 
              pthread_exit(0);

          }

  } // End while(1)

}//end

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//++UTILITY FUNCTION

/*
void  StopAcceptingState() {



                                       //writeFileLog"[StopAcceptingState()] Start.");

									   Wait(DELAY_FOR_TRANS_COMPLETE);
  
                                       pthread_mutex_lock(&RuntimeInhibitFlagmutex);

                                       //Stop all accepting process
                                       g_RuntimeInhibitFlag=0;

                                       pthread_mutex_unlock(&RuntimeInhibitFlagmutex);

                                       //off noteacptr credit polling flag
                                       #ifdef NOTE_ACCEPTOR
                                       pthread_mutex_lock(&noteAcptrStartFlagmutex);
                                       g_noteAcptrStartFlag = OFF; 
                                       pthread_mutex_unlock(&noteAcptrStartFlagmutex);
                                       #endif


                                       //off coinacptr credit polling flag
                                       #ifdef COIN_ACCEPTOR
                                       pthread_mutex_lock(&coinAcptrStartFlagmutex);
                                       g_coinAcptrStartFlag = OFF; 
                                       pthread_mutex_unlock(&coinAcptrStartFlagmutex);
                                       #endif

                                       //close coin acceptor low level thread 
                                       #ifdef COIN_ACCEPTOR
                                       CloseCoinCreditPollingThread(); 
                                       #endif
 
									   //close note acptr low level thread
									   #ifdef NOTE_ACCEPTOR
                                       //writeFileLog"[StopAcceptingState()] Before Signal Note Acceptor Thread Stop.");
                                       CloseNoteAcceptorPollThread();
                                       //writeFileLog"[StopAcceptingState()] After Signal Note Acceptor Thread Stop.");
                                       delay_mSec(NOTE_ACPTR_END_DELAY); 
                             
                                       #endif

                                       //Safety
                                       DisableAllAcceptor(); 

                                       //writeFileLog"[StopAcceptingState()] End.");
                
                                       return;





}//StopAcceptingState() end here
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StopNAUpperCreditPollThread() {


       //off noteacptr credit polling flag
       pthread_mutex_lock(&noteAcptrStartFlagmutex);
       g_noteAcptrStartFlag = OFF; 
       pthread_mutex_unlock(&noteAcptrStartFlagmutex);
       

}//end

void StopCAUpperCreditPollThread() {
	
      AVRM_Currency_writeFileLog("[StopCAUpperCreditPollThread()] Entry .",TRACE);
      //off coinacptr credit polling flag
      #ifdef COIN_ACCEPTOR
      pthread_mutex_lock(&coinAcptrStartFlagmutex);
      g_coinAcptrStartFlag = OFF; 
      pthread_mutex_unlock(&coinAcptrStartFlagmutex);
      #endif
      AVRM_Currency_writeFileLog("[StopCAUpperCreditPollThread()] Exit .",TRACE);

}//end

void StopCALowerCreditPollThread() {

      AVRM_Currency_writeFileLog("[StopCALowerCreditPollThread()] Entry .",TRACE);
      #ifdef COIN_ACCEPTOR
      //close coin acceptor low level thread 
      CloseCoinCreditPollingThread(); 
      #endif
      AVRM_Currency_writeFileLog("[StopCALowerCreditPollThread()] Exit .",TRACE);
 
}//end

void StopNALowerCreditPollThread() {

      #if defined(NOTE_ACCEPTOR) 
      //close note acptr low level thread
      CloseNoteAcceptorPollThread();
      delay_mSec(NOTE_ACPTR_THREAD_END_DELAY);
      #endif 
     
 
}//end

void  StopAcceptingState() {

										   AVRM_Currency_writeFileLog("[StopAcceptingState()] Start.",TRACE);
										   
										   //++Now Set Signal Acceptance State is stopped
										   pthread_mutex_lock(&g_AcceptanceStateFlagMutex);
										   g_AcceptanceStateFlag=false;
										   pthread_mutex_unlock(&g_AcceptanceStateFlagMutex);

										   int logsize=1000;
								 
										   char log[logsize];

										   memset(log,'\0',logsize);
									  
										   struct timespec begints, endts,diffts;

										   #if defined(NOTE_ACCEPTOR) 
										   
                                           if( g_maxNoOfCash > 0 ) {
											   
												       clock_gettime(CLOCK_MONOTONIC, &begints);
			 
												       //++wait for note acceptor disable signal status
												       while(1) {

															if( true == GetNADisableAcceptanceSignal() ) {
															
																		clock_gettime(CLOCK_MONOTONIC, &endts);

																		diffts.tv_sec = endts.tv_sec - begints.tv_sec;

																		memset(log,'\0',100);

																		sprintf(log,"[StopAcceptingState() Note Disable Status] Get Disable acceptance signal get in  %d milisecond.",(int)diffts.tv_sec);

																		AVRM_Currency_writeFileLog(log,INFO); 

																		break;
															}//if end
											 
															//++Timeout exit [abnormal exit]
															clock_gettime(CLOCK_MONOTONIC, &endts);
															diffts.tv_sec = endts.tv_sec - begints.tv_sec;
															if(diffts.tv_sec >DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE ) {
															
																
																memset(log,'\0',100);

																sprintf(log,"[StopAcceptingState() Note Disable Status Timeout] unable to Get Disable acceptance signal get in  %d  milisecond.",(int)diffts.tv_sec);

																AVRM_Currency_writeFileLog(log,INFO); 

																break;
															  
															}//if end
										  
													   }//while loop
													  
													   //++delay between two checking
													   delay_mSec(NOTE_ACPTR_STATUS_CHECK_DELAY);
													   
													   //++wait for note acceptor note acceptance complete signal
													   while(1) {
													  
															if( false == GetNANoteAcceptanceStatus() ) {
															
																		clock_gettime(CLOCK_MONOTONIC, &endts);

																		diffts.tv_sec = endts.tv_sec - begints.tv_sec;

																		memset(log,'\0',logsize);

																		sprintf(log,"[StopAcceptingState() Note Acceptance Status] Get Note acceptance sequence finish signal get in  %d milisecond.",(int)diffts.tv_sec);

																		AVRM_Currency_writeFileLog(log,INFO); 

																		break;
															}//if end
															
															//Timeout exit [abnormal exit]
															clock_gettime(CLOCK_MONOTONIC, &endts);
															diffts.tv_sec = endts.tv_sec - begints.tv_sec;
															if(diffts.tv_sec > DELAY_FOR_NOTE_ACCEPTANCE_COMPLETE) {
															
																
																memset(log,'\0',logsize);

																sprintf(log,"[StopAcceptingState() Note Acceptance Status Timeout] unable to Get Note acceptance sequence finish signal get in  %d milisecond.",(int)diffts.tv_sec);

																AVRM_Currency_writeFileLog(log,INFO); 

																break;
															  
															}//if end
										  
											          }//while loop

								           }//++if( g_maxNoOfCash > 0 ) end
								       
                                           #endif
                                                                               
                                           #ifdef COIN_ACCEPTOR

                                           if( g_maxNoOfCoin > 0 ) {
											
													clock_gettime(CLOCK_MONOTONIC, &begints);
				 
													//++wait for coin acceptor disable signal status
													while(1) { //++while loop
													
																if( true == GetCADisableAcceptanceSignal() ) {
																	  clock_gettime(CLOCK_MONOTONIC, &endts);
																	  diffts.tv_sec = endts.tv_sec - begints.tv_sec;
																	  memset(log,'\0',logsize);
																	  sprintf(log,"[StopAcceptingState() Coin Disable Status] Get Disable acceptance signal get in  %d second.",(int)diffts.tv_sec);
																	  AVRM_Currency_writeFileLog(log,INFO); 
																	  break;
																}//if end
														
																//++Timeout exit [abnormal exit]
																clock_gettime(CLOCK_MONOTONIC, &endts);
																diffts.tv_sec = endts.tv_sec - begints.tv_sec;
																if(diffts.tv_sec > DELAY_FOR_COIN_DENOM_DISABLE_COMPLETE ){
																	memset(log,'\0',logsize);
																	sprintf(log,"[StopAcceptingState() Coin Disable Status Timeout] unable to Get Disable acceptance signal get in  %d second.",(int)diffts.tv_sec);
																	AVRM_Currency_writeFileLog(log,INFO); 
																	break;
																  
																}//if end
											  
												    }//while loop
                                       
                                           }//if( g_maxNoOfCoin > 0 ) end
                                           
                                           #endif
                                       
										   if( g_maxNoOfCoin > 0 ) {
											   
											   #if defined(COIN_ACCEPTOR) &&  !defined(NOTE_ACCEPTOR)
											   //Delay for coin acceptance sequene
											   Wait(DELAY_FOR_COIN_ACCEPTANCE_COMPLETE);
											   #endif
													  
										   } //if end
                                     
										   //++Now Close all thread
										   AVRM_Currency_writeFileLog("[StopAcceptingState()] Before: Now Going to Inhibit All Accepting Process.",TRACE); 
										   
										   pthread_mutex_lock(&RuntimeInhibitFlagmutex);
										   //++Stop all accepting process
										   g_RuntimeInhibitFlag=0;
										   pthread_mutex_unlock(&RuntimeInhibitFlagmutex);
										   
										   AVRM_Currency_writeFileLog("[StopAcceptingState()] After: Now Going to Inhibit All Accepting Process.",TRACE); 
										   
										   AVRM_Currency_writeFileLog("[StopAcceptingState()] Before: Now Going to Close Upper and Lower Level Note Acceptor Thread.",TRACE); 
										   
										   if( g_maxNoOfCash > 0 ) {
											   
											   #ifdef NOTE_ACCEPTOR
											   StopNAUpperCreditPollThread();
											   StopNALowerCreditPollThread();
											   Wait(1);
											   #endif
											   
										   }//if end
										   
										   AVRM_Currency_writeFileLog("[StopAcceptingState()] After: Now Going to Close Upper and Lower Level Note Acceptor Thread..",TRACE); 
										   
										   AVRM_Currency_writeFileLog("[StopAcceptingState()] Before: Now Going to Close Upper and Lower Level Coin Acceptor Thread.",TRACE);
										   
										   if( g_maxNoOfCoin > 0 ) {
											   
											   #ifdef COIN_ACCEPTOR
											   StopCAUpperCreditPollThread();
											   StopCALowerCreditPollThread();
											   #endif
											   
										   }//if end
										   
										   AVRM_Currency_writeFileLog("[StopAcceptingState()] After: Now Going to Close Upper and Lower Level Coin Acceptor Thread.",TRACE);
										   
										   //Safety purpose disable all accptor
										   //++DisableAllAcceptor(); 

										   AVRM_Currency_writeFileLog("[StopAcceptingState()] End.",TRACE); 
					
										   return;


}//++StopAcceptingState() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Millisecond delay
static void delay_mSec(int milisec)  {


        struct timespec req = {0};
		req.tv_sec = 0;
		req.tv_nsec = milisec * 1000000L;

       
        int rtcode=-1;
        rtcode=clock_nanosleep( CLOCK_MONOTONIC,
                                0,
                                &req,
                                NULL
                              );
        
        unsigned char log[100];

        if(rtcode<0)
        {

		memset(log,'\0',100);
				   
		sprintf(log,"[delay_mSec()] clock_nanosleep failed with return code %d .",rtcode); 

		//writeFileLoglog);
		

        }


}//end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Wait(unsigned int Second){
		   

                  if(Second<=0)
                  {
		      return 0;
                  }
                  unsigned char log[100]; 
                  memset(log,'\0',100);
                  struct timespec begints, endts,diffts;
                  clock_gettime(CLOCK_MONOTONIC, &begints);
		  while(1)
		  {


				  clock_gettime(CLOCK_MONOTONIC, &endts);

				  diffts.tv_sec = endts.tv_sec - begints.tv_sec;

				  #ifdef ATVM_DEBUG

		                  memset(log,'\0',100);
		           
		                  sprintf(log,"[Wait()] Seconds Elapsed=%d",diffts.tv_sec); 

		                  ////writeFileLoglog);
		                  
		                  AVRM_Currency_writeFileLog(log,INFO); 

		                  #endif

		                  if(diffts.tv_sec>=Second)
		                  {

		                     return 1;

		                  }


                 }


}//Wait() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Deactivate Note Acceptor
#if defined(NOTE_ACCEPTOR)

static  int DeactivateNoteAcptr() {


	    #ifdef B2B_NOTE_ACCEPTOR
	    return ( DeactivateB2BUnit() );
	    #endif
	    
	    #ifdef JCM_NOTE_ACCEPTOR
	    return (Deactivate_JCM_NoteAcptr());
	    #endif
	    
	    #ifdef BNA_NOTE_ACCEPTOR
	    return (Deactivate_BNA_NoteAcptr());
	    #endif
	    return 0;


}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(CCTALK_DEVICE) && defined(COIN_ACCEPTOR)

//Deactivate Coin Acceptor 
static  int DeactivateCoinAcptr(){
       return ( CloseCoinAcptr() );
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TRAP_GATE_DEVICE) && defined(B2B_TRAP_GATE) 

//Deactivate Trap Gate 
static  int  DeactivateTrapGate(){

      return ( CloseDropandSealUnit() );
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  void GetRecycleStatusBeforeStartAcceptfare() {
                                    
                   unsigned char log[100];

                   memset(log,'\0',100);

                   //writeFileLog"[GetRecycleStatusBeforeAcceptfare()] Before Going to read current recycle cassette status.");

				   g_PrevCastQuanity=0;

                   g_PrevCastFlag=false;

                   #ifdef NOTE_ACCEPTOR

		           //Get Current quantity
                   #ifdef JCM_NOTE_ACCEPTOR
                   if( 1 == GetNoteDetailsInRecycleCst(&g_PrevCastQuanity) )
                   #endif
                   #ifdef BNA_NOTE_ACCEPTOR
                   if( 1 == GetNoteDetailsInRecycleCst(&g_PrevCastQuanity) )
                   #endif
                   #ifdef B2B_NOTE_ACCEPTOR
		           if(SUCCESS == GetNoteDetailsInRecycleCst(&g_PrevCastQuanity))
                   #endif
                   //if(SUCCESS == B2BGetExtendedRecycleStatus(&g_PrevCastQuanity,0))
                   {
                        //writeFileLog"[GetRecycleStatusBeforeAcceptfare()] Read note acceptor recycle cassette status success.");
                        g_PrevCastFlag=true;
                   }
                   else
                   {
                        //writeFileLog"[GetRecycleStatusBeforeAcceptfare()] Read note acceptor recycle cassette status failed.");
                        g_PrevCastFlag=false;
                   }

                   #endif

                   sprintf(log,"[CalculateCurrentBillQuantity()] Previous Recycle Quantity : %d",g_PrevCastQuanity);
                   //writeFileLoglog); 

                   //writeFileLog"[GetRecycleStatusBeforeAcceptfare()] After read current recycle cassette status.");
                   


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  void GetRecycleStatusAfterFinishAcceptfare() {
         
                  

                   unsigned char log[100];
                   memset(log,'\0',100);

                   //writeFileLog"[GetRecycleStatusAfterFinishAcceptfare()] Before Going to read current recycle cassette status.");

				   g_CurrentCastQuanity=0;
                   g_CurrentCastFlag=false;
           
                   #ifdef NOTE_ACCEPTOR

				   //Get Current quantity
                   #ifdef JCM_NOTE_ACCEPTOR
                   if( 1 == GetNoteDetailsInRecycleCst(&g_CurrentCastQuanity) )
                   #endif
                   #ifdef BNA_NOTE_ACCEPTOR
                   if( 1 == GetNoteDetailsInRecycleCst(&g_CurrentCastQuanity) )
                   #endif
				   #ifdef B2B_NOTE_ACCEPTOR
                   //if( SUCCESS == getNoteDetailsInRecycleCst(&g_CurrentCastQuanity) )
                   if( SUCCESS == B2BGetExtendedRecycleStatus(&g_CurrentCastQuanity,2) )
                   #endif
                   {
                        //writeFileLog"[GetRecycleStatusAfterFinishAcceptfare()] Read b2b recycle cassette status success.");
                        g_CurrentCastFlag=true;
                   }
                   else
                   {
                        //writeFileLog"[GetRecycleStatusAfterFinishAcceptfare()] Read b2b recycle cassette status failed.");
                        g_CurrentCastFlag=false;
                   }
                   
                   #endif

                   sprintf(log,"[CalculateCurrentBillQuantity()] Current Recycle Quantity : %d",g_CurrentCastQuanity);
                   //writeFileLoglog);    
       
                   //writeFileLog"[GetRecycleStatusAfterFinishAcceptfare()] After read current recycle cassette status.");

                   



}//GetRecycleStatusAfterFinishAcceptfare() end here

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  unsigned int CalculateCurrentBillQuantity() {

           
           /*

           unsigned char log[100];
           memset(log,'\0',100);

           
	   GetRecycleStatusAfterFinishAcceptfare();

           memset(log,'\0',100);
           sprintf(log,"[CalculateCurrentBillQuantity()] Current Recycle Quantity : %d",g_CurrentCastQuanity);
           //writeFileLoglog);    
       
           memset(log,'\0',100);
           sprintf(log,"[CalculateCurrentBillQuantity()] Previous Recycle Quantity : %d",g_PrevCastQuanity);
           //writeFileLoglog); 

           memset(log,'\0',100);
           sprintf(log,"[CalculateCurrentBillQuantity()] Current Counted Note Number : %d",g_ttlNmbrOfNote);
           //writeFileLoglog);   

       
           if( (true == g_CurrentCastFlag) &&  ( true == g_PrevCastFlag ) ) 
           {

		           if( g_CurrentCastQuanity!= (g_PrevCastQuanity+g_ttlNmbrOfNote) )
			   {
				   ////writeFileLog"[CalculateCurrentBillQuantity() current read block ] Note count error happens.");
				   return ( g_CurrentCastQuanity - g_PrevCastQuanity );
			   }  
			   else
			   {
				   ////writeFileLog"[CalculateCurrentBillQuantity() current read block ] No Note count error.");
				   return ( g_ttlNmbrOfNote );

			   }

         }

         else
         {
	        //writeFileLog"[CalculateCurrentBillQuantity()] current read failed.");
		return (g_ttlNmbrOfNote);

         }
         */

          return ( g_ttlNmbrOfNote );
         

}//CalculateCurrentBillQuantity() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 Return Code:
 1=Successfully all close running thread
 0=Unable to close all thread
 2=thread already closed

*/

static int KillAllRunningThread(double waittime) {		 
				 
				 time_t start,end,diff;

				 int rtcode=-1;

                 unsigned char log[200];

                 memset(log,'\0',200);

				 //writeFileLog"[KillAllRunningThread()] External Thread Termination in processing.....");
		 
		         //if Acpt Fare Thread in Started Condition
                 pthread_mutex_lock(&AcptFareThreadStatusmutex);

				 if(THREAD_START==g_AcptFareThreadStatus){

                           pthread_mutex_unlock(&AcptFareThreadStatusmutex);

                           //writeFileLog"[KillAllRunningThread()] Before Signal Close AcceptFareThread.");

                           pthread_mutex_lock(&mut);

						   //Signal Acpt fare thread to closed or kill
						   g_ExternalKillAcptFareThread=THREAD_KILL;

                           pthread_mutex_unlock(&mut);

                           //writeFileLog"[KillAllRunningThread()] After Signal Close AcceptFareThread.");

                           time(&start);  
			 
						   //Wait here untill all thread not in closed
						   for(;;)
						   {

									 time(&end); 

									 diff=end-start;
                                     
                                     //////////////////////////////////////////////////////////////////////////////

                                     /*

                                     memset(log,'\0',200);
                   
                                     sprintf(log,"[KillAllRunningThread()] Time = %d Seconds.",diff); 

                                     //writeFileLoglog);

                                    */
 
                                     //////////////////////////////////////////////////////////////////////////////

									 //if timer time is end or timer is running
									 if(diff<waittime)
									 {
 
                                                pthread_mutex_lock(&AcptFareThreadStatusmutex);

												if(THREAD_CLOSE==g_AcptFareThreadStatus) 
												{

                                                    pthread_mutex_unlock(&AcptFareThreadStatusmutex);

													//memset(log,'\0',200);
                   
                                                    //sprintf(log,"[KillAllRunningThread() diff<waittime block] All thread termination completed within %d Seconds.",diff); 

                                                    ////writeFileLoglog);

													return 1;

												}
                                                else
                                                {
                                                    pthread_mutex_unlock(&AcptFareThreadStatusmutex);
                                       
                                                }
						

									 }//if end
                                     //++if timer time is end or timer is running
									 if(diff>=waittime)
									 {

												pthread_mutex_lock(&AcptFareThreadStatusmutex);

												if( THREAD_CLOSE==g_AcptFareThreadStatus)
												{
													pthread_mutex_unlock(&AcptFareThreadStatusmutex);

													//memset(log,'\0',200);
										   
													//sprintf(log,"[KillAllRunningThread() diff>=waittime block] All thread termination completed within %d Seconds.",diff); 

													////writeFileLoglog);
																			
													return 1;

												}
												else
												{ 
											        pthread_mutex_unlock(&AcptFareThreadStatusmutex);

													//memset(log,'\0',200);
										   
													//sprintf(log,"[KillAllRunningThread() diff>=waittime block] All thread termination failed: %d Seconds.",diff); 
																			
													////writeFileLoglog);

													return 0;

												}

					                   }//++timer if block end here
				      

			            }//++for loop end
		   
		 }else if(THREAD_CLOSE==g_AcptFareThreadStatus) {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);

		      //writeFileLog"[KillAllRunningThread()] All thread already terminated.");

              return 2;

         }//++else if end



}//KillAllRunningThread() end here

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ASCRMAPIMAIN


int main()
{
  
      //++TestNewAscrm(0);
	  //++TestAscrm(0);
      //TestSingleNoteAscrm(0);
      //TestAscrmCassetteStatus(0);
      TestAscrm(0);
	  return 0;


} //End main


void TestNewAscrm(int portno)
{


		   
                  //Log Test
                  
                  int devStatus1=0,
                      devStatus2=0;
                  
                  char *deviceid="JCM";
                  strcpy(g_Deviceid,deviceid);
                  g_LogModeLevel = (int)ALL_LOG;
                  
                  int counter=0,ret=0,rtcode=0,
                       UserAmountInput = 0,
                       prevfare=0,row=0 ,
                       col=0,arry[MAX_NMBR_OF_NOTE][2]={0},
                       acptfarertcode=0,
                       fare1=0,transtime=60,maxcash=0,maxcoin=0;
                   
                   double diff=0;
                   time_t start=0,end=0;
                   unsigned char log[100];
                   memset(log,'\0',100);
                   int EscrowMode=0;
                   int DeviceType=0;
                  
                   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
                   {
                        for( col=0;col<2;col++)
                        {
                               arry[row][col]=0;
                        }

                   }
        
                   ///////////////////////////////////////////////////////////////////////////
                   
                   ////writeFileLog"[TestAscrm()] ENTER DeviceType[0-ALL 1:NoteAcceptor 2:CoinAcceptor+CoinEscrow]: ");
	               //scanf("%d",&DeviceType);


                   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
                   DeviceType = 2;
                   portno = 1;
                   EscrowMode =0;
                   
                   #if defined(COIN_ACCEPTOR)
                   
                   ////writeFileLog"[TestAscrm()] ENTER Coin Acceptor COM PORT NUMBER[0-255]: ");
	               //scanf("%d",&portno);
	               
                   ////writeFileLog"[TestAscrm()] ENTER ESCROW MODE FOR [ 0: Retain Note/Coins 1: Collec Note/Coin to CollectionBin ]: ");
	               //scanf("%d",&EscrowMode);

                   ret =  activateDevice2(portno,DeviceType,EscrowMode,5000);
		           memset(log,'\0',100);
                   sprintf(log,"[TestAscrm()] ACTIVATE Coin Acceptor STATUS: %d",ret ); 
                   //writeFileLoglog);
                   
                   #endif
                   
                   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
                   DeviceType = 1;
                   EscrowMode=0;
                   portno=0;
                   
                   #if defined(NOTE_ACCEPTOR)
                   
                   ////writeFileLog"[TestAscrm()] ENTER Note Acceptor COM PORT NUMBER[0-255]: ");
	               //scanf("%d",&portno);

                   ////writeFileLog"[TestAscrm()] ENTER ESCROW MODE FOR [ 0: Retain Note/Coins 1: Collec Note/Coin to CollectionBin ]: ");
	               //scanf("%d",&EscrowMode);

                   ret =  activateDevice2(portno,DeviceType,EscrowMode,5000);
		           memset(log,'\0',100);
                   sprintf(log,"[TestAscrm()] ACTIVATE Note Acceptor STATUS: %d",ret ); 
                   //writeFileLoglog);
                   
                   #endif
                   
                   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   

				   if( ret < 1 )
				   {
					   
								memset(log,'\0',100);

								sprintf(log,"[TestAscrm()] ACTIVATE DEV STATUS: %d thats why system cannot run now ",ret ); 

								//writeFileLoglog);

								return;

				  }else if( 2 == ret ) //activated successfully but few notes in escrow
                  {

                       printf("\n[TestAscrm()] Press 1 for accept escrow notes ,\n 2 for return escrow notes and press any other key to go for system default choice which is accept escrow notes");

                       char ch=0;

					   ch=getchar();

					   switch(ch)
					   {
									  case '1': defaultCommit(1000); 
												break;

									  case '2': defaultCancel(1000);
												break;

									  default : defaultCommit(1000); 
												break;
						  
					   }

                       
                       

                  }//else if end
		  
                  for( ;; )
		          {
			

									  acptfarertcode=0;

									  prevfare=0;
						   
									  //writeFileLog"\n\n\n\n\n\n[TestAscrm()] Press any key to continue or Press 1 for exit : ");
									  char ch=0;
									  ch=getchar();
									  if( '1' == ch)
									  {
										  //writeFileLog"[TestAscrm()]) Program now exit by user. ");
										  return;
									  }
									  
									  #if defined(NOTE_ACCEPTOR)
									  devStatus1 =deviceStatusV2( 1 ,0);
									  #endif
									  
									  #if defined(COIN_ACCEPTOR)
									  devStatus2= deviceStatusV2( 2 ,0);
									  #endif
									   
									  ret=0x7;
									  
									  memset(log,'\0',100);

									  sprintf(log,"[TestAscrm()] Before And Device Status Anding Code : 0x%x",ret); 
									   
									  //writeFileLoglog);

									  ret = (ret&0x02)>>1;
									   
									  memset(log,'\0',100);
									   
									  sprintf(log,"[TestAscrm()] After And Device Status Code : %d",ret); 
									   
									  //writeFileLoglog);

									  if( 0 ==  ret )
									  {
												//writeFileLog"[TestAscrm()] Press any key to continue: ");
												char ch=0;
												ch=getchar();
												continue;
									  }

									  memset(log,'\0',100);

									  sprintf(log,"[TestAscrm()] Both Device Status Code: %d",ret); 
									   
									  //writeFileLoglog);

									  fare1=0;
						   
									  //writeFileLog"[TestAscrm()] ENTER FARE: ");
						   
									  scanf("%d",&fare1);

									  //writeFileLog"[TestAscrm()] ENTER FARE TIME: ");
						   
									  scanf("%d",&transtime);
									   
									  maxcash=100;
									   
									  maxcoin=0;
						   
									  //Clear Array
									  for( row=0;row<MAX_NMBR_OF_NOTE;row++)
									  {
											for( col=0;col<2;col++)
											{
												   arry[row][col]=0;
											}

									  }
					
									  //transtime = GetTransTimeout();
									  
									  memset(log,'\0',100);
									  
									  sprintf(log,"[TestAscrm()] Trans Time: %d",transtime  ); 
									  
									  //writeFileLoglog);

									  //////////////////////////////////////////////////////////////////////////////////////////////
									  
									  int DenomMask = 0x00;

									  //writeFileLog"[TestAscrm()] ENTER Denom Mask Value: ");
						   
									  scanf("%d",&DenomMask);

									  memset(log,'\0',100);
									  
									  sprintf(log,"[TestAscrm()] Denom Mask Value: %d",DenomMask  ); 
									  
									  //writeFileLoglog);
									  
									  /*

									  EnableTheseDenomination( DenomMask,0);

									  DenomMask = 0x1F;

									  EnableTheseDenomination( DenomMask,0);

									  DenomMask = 0x43;

									  EnableTheseDenomination( DenomMask,0);

									  DenomMask = 0x71;
								   
									  EnableTheseDenomination( DenomMask,0);

									  DenomMask = 0x4F;
									  
									  EnableTheseDenomination( DenomMask,0);


									  DenomMask = 0x7F;
									  
									  EnableTheseDenomination( DenomMask,0);


									  DenomMask = 0x40;
									  
									  EnableTheseDenomination( DenomMask,0);

									  */

									  EnableTheseDenomination( 1, DenomMask,0);
									  EnableTheseDenomination( 2, DenomMask,0);
                        
                          /////////////////////////////////////////////////////////////////////////////////////////////

	                      acptfarertcode=acceptFare(fare1,maxcash,maxcoin,transtime*1000);

						  if( false == acptfarertcode)
						  {
							  //writeFileLog"[TestAscrm()]) acceptFare() return false again try!! ");
							  
										  continue;

						  }  
									  else if( false == acptfarertcode)
						  {
							  //writeFileLog"[TestAscrm()]) acceptFare() return true.");
							  
						  }  
			  
                          rtcode = 0;
			  
                          UserAmountInput = 0;
			  
                          row=0;col=0;
			      
			              prevfare = 0;

              /////////////////////////////////////////////////////////////////////////////////////////////////
 
			  time(&start);

			  while(1)
			  {   

			   
				    time(&end);

				    diff=end-start;

                    if(diff>=transtime)
				    {
                                               

					       //writeFileLog"[TestAscrm()] >>  Transtimeout by system");  
					       rtcode = getDenomination(arry);

					       memset(log,'\0',100);
                           sprintf(log,"[TestAscrm()]getDenomination Return code=%d",rtcode); 
                           //writeFileLoglog);

                           UserAmountInput=0; 

					       for( row=0;row<MAX_NMBR_OF_NOTE;row++)
					       {
								  if(arry[row][1]>0) {
									UserAmountInput+=GetDenomValue(arry[row][1]); 
								  }//if end

						   }//for end

                           memset(log,'\0',100);
                           sprintf(log,"[TestAscrm()] >> User Given Currency:%d",UserAmountInput); 
                           //writeFileLoglog);

					       if(fare1==UserAmountInput)
                                               {
					            //writeFileLog"[TestAscrm()] >>  System now commit your transaction");
                                commitTran_Ver2();

					       }
					       else
                           {
					                               //writeFileLog"[TestAscrm()] >>  System now cancel your session");

                                                   cancelTran_Ver2();

                                                   //writeFileLog"[TestAscrm()] >>  Before check customer take note from outlet");
                                                   if(UserAmountInput > 0 )
                                                   {
                                                        #ifdef B2B_NOTE_ACCEPTOR
					                                    IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                                        #endif
                                                   }

                                                    //writeFileLog"[TestAscrm()] >>  After check customer take note from outlet");
                           }
                                               
					       break;


				    }//if(diff>=transtime)

                    rtcode =-1;

				    rtcode = getDenomination(arry);
			           
                    ///////////////////////////////////////////////////////////////////////////////////////////

									//Accepting State return code 1
									if(1==rtcode)
									{
															  
										  UserAmountInput=0;
										  
										  for( row=0;row<MAX_NMBR_OF_NOTE;row++)
										  {
												if(arry[row][1]>0) 
												{
												  UserAmountInput += GetDenomValue( arry[row][1] ); 
												}
										  }
															  
										  //system("clear");
										  if(UserAmountInput>prevfare)
										  {
												   memset(log,'\0',100);
                                                   sprintf(log,"[TestNewAscrm()] >> User Given Fare = %d",UserAmountInput); 
												   //writeFileLoglog);
                                                   prevfare=UserAmountInput;

										   }

										 
															   
									}//if(1==rtcode) end

                                    ///////////////////////////////////////////////////////////////////////////////////////////

									//Exact fare accepted return code 2
									if(2==rtcode)
									{
                                               
                                       //writeFileLog"\n =========================================================================");            
                                       memset(log,'\0',100);
		                               sprintf(log,"[TestNewAscrm()] getDenomination return code %d",rtcode); 
		                               //writeFileLoglog);
                                       //writeFileLog"\n =========================================================================");      
					                   //writeFileLog"[TestAscrm()] >> ACCEPTED FARE GIVEN");  
					       
                                       commitTran_Ver2();
                                              
					                   break;

                                    }//if(2==rtcode)

                                    
                                    ///////////////////////////////////////////////////////////////////////////////////////////

									//Transactions Timeout. return code 0
									if (0==rtcode)
									{ 
									           //writeFileLog"\n =========================================================================");      
                                               memset(log,'\0',100);
		                                       sprintf(log,"[TestAscrm()] getDenomination return code %d",rtcode); 
		                                       //writeFileLoglog);
                                               //writeFileLog"\n =========================================================================");      

					                           //writeFileLog"[TestAscrm()] >>  Transtimeout by user");  

                                               UserAmountInput=0;
					      
											   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
											   {
													if(arry[row][1]>0) 
																		{
													UserAmountInput+=GetDenomValue(arry[row][1]); 
																		}

											   }
                                              
                                               cancelTran_Ver2();

                                               if(UserAmountInput > 0 )
                                               {
                                                     #ifdef B2B_NOTE_ACCEPTOR
					                                 IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                                     #endif
                                               }

					                           break;    
                                             
                                             
         
				                    }//if (0==rtcode)

                                    
                                    ////////////////////////////////////////////////////////////////////////////////////////// 
                                    
                                    // More qty return code 3
									if( 3==rtcode )
									{ 

                                               //writeFileLog"\n =========================================================================");      
                                               memset(log,'\0',100);
                                               sprintf(log,"[TestAscrm()] getDenomination return code = %d",rtcode); 
                                               //writeFileLoglog);

                                               //writeFileLog"\n =========================================================================");      

					                           //writeFileLog"[TestAscrm()] >> More note and coin inserted its time for cancel by system");            

                                               UserAmountInput=0;
					      
											   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
											   {
													if(arry[row][1]>0) {
														UserAmountInput+=GetDenomValue(arry[row][1]); 
													}

											   }

                                               cancelTran_Ver2();

                                               if(UserAmountInput > 0 )
                                               {
                                                   #ifdef B2B_NOTE_ACCEPTOR
                                                    IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                                   #endif
                                               }
					                           
					                           break;


                                              
                                    }//if( 3==rtcode )
				   
				    
                                    ///////////////////////////////////////////////////////////////////////////////////////////

			                } // End: while loop

                          //////////////////////////////////////////////////////////////////////////////////////////////////
			 
		   } // End:  for loop




}//TestNewAscrm() end


int GetDenomValue(int notecode) {


                                       int denomvalue=0;

                                       switch(notecode)
                                       {

											  case 0: 
													 denomvalue= 5;
													 break;
							  
											  case 1:
													 denomvalue= 10;
													 break;

											  case 2:
													 denomvalue= 20;
													 break;

											  case 3:
													 denomvalue= 50;
													 break;

											  case 14:
													 denomvalue= 100;
													 break;

											  case 5:
													 denomvalue= 500;
													 break;

											  case 6:
													 denomvalue= 1000;
													 break;

											 default:
													 denomvalue= 0;
													 break;

                                       };

                                      return denomvalue;


}//int GetDenomValue(int notecode) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* reads from keypress, doesn't echo */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

/* reads from keypress, echoes */
int getche(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef B2B_NOTE_ACCEPTOR

static void TestSingleNoteAscrm(int portno)
{


		   int counter=0,ret=0,rtcode=0,
                       UserAmountInput = 0,
                       prevfare=0,row=0 ,
                       col=0,arry[MAX_NMBR_OF_NOTE][2]={0},acptfarertcode=0,fare1=0,transtime=60,maxcash=0,maxcoin=0;
                   
                   double diff=0;
                   time_t start=0,end=0;
                   unsigned char log[100];
                   memset(log,'\0',100);


                   int NoteChoice =0;

                   int totalnotes=0,TransChoice=0,MaxNote=0;


                   int input=0;

                  
                   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
                   {
                        for( col=0;col<2;col++)
                        {
                               arry[row][col]=0;
                        }

                   }
        
                   ret =  activateDevice(portno,0,5000);

		   memset(log,'\0',100);

                   sprintf(log,"[TestSingleNoteAscrm()] ACTIVATE DEV STATUS: %d",ret ); 

                   //writeFileLoglog);

		   if(-7==ret)
		   {
		       
                        memset(log,'\0',100);

                        sprintf(log,"[TestSingleNoteAscrm()] ACTIVATE DEV STATUS: %d thats why system cannot run now ",ret ); 

                        //writeFileLoglog);

                        return;

		  }
                  else if( 2 == ret ) //activated successfully but few notes in escrow
                  {
                       

                       printf("\n[TestSingleNoteAscrm()] Going to cancel all momney.");
                       //defaultCancel();
                       

                  }
		  
                  for( ;; )
		  {
			

                          acptfarertcode=0;

		          prevfare=0;
			   
			  //writeFileLog"\n\n\n\n\n\n[TestSingleNoteAscrm()] Press any key to continue or Press 1 for exit : ");

			  char ch=0;

			  ch=getchar();

			  if( '1' == ch)
			  {
			      //writeFileLog"[TestSingleNoteAscrm()]) Program now exit by user. ");
			      return;
			  }
                           
                          ret = deviceStatus(0);
                          
                          memset(log,'\0',100);

                          sprintf(log,"[TestSingleNoteAscrm()] Before And Device Status Anding Code : %d",ret); 
                           
                          //writeFileLoglog);

                          ret = (ret&0x02)>>1;
                           
                          memset(log,'\0',100);
                           
                          sprintf(log,"[TestSingleNoteAscrm()] After And Device Status Code : %d",ret); 
                           
                          //writeFileLoglog);

                          if( 0 ==  ret )
		          {
		                    //writeFileLog"[TestSingleNoteAscrm()] Press any key to continue: ");
				    scanf("%c",&input);
                                    continue;
		          }

 

                          memset(log,'\0',100);

                          sprintf(log,"[TestSingleNoteAscrm()] Both Device Status Code: %d",ret); 
                           
                          //writeFileLoglog);


                          //writeFileLog"[TestSingleNoteAscrm()] ENTER MAX NOTE FOR COMMIT OR CANCEL OPERATION: ");
			   
                          scanf("%d",&MaxNote);


                          //////////////////////////////////////////////////////////////////////////////////////////

                          //Clear Array
                          for( row=0;row<MAX_NMBR_OF_NOTE;row++)
		          {
		                for( col=0;col<2;col++)
		                {
		                       arry[row][col]=0;
		                }

		          }
		 
                          
                          rtcode = 0;
			  
                          UserAmountInput = 0;
			  
                          row=0;col=0;
			      
			  prevfare = 0;
 
                          //////////////////////////////////////////////////////////////////////////////////////////
                          
			   
                          ////////////////////////////////////////////////////////////////////////////////////////////////
 
			  time(&start);

			  while(1)
			  {   


                                    ////////////////////////////////////////////////////////////////////////////////

                                    if( totalnotes >=MaxNote )
                                    {
                                                       //writeFileLog"[TestSingleNoteAscrm()] Now Press 1 for Commit or 2 for Cancel : ");
			                               scanf("%d",&TransChoice);

                                                       if( 1 == TransChoice)
                                                       {
                                                             if( true == commitTranInterface(30) )
                                                             {
                                                                totalnotes=0;
                                                             }

                                                       }
                                                       else if( 2 == TransChoice)
                                                       {
                                                              if( true == cancelTranInterface(30) )
                                                              {
                                                                totalnotes=0;
                                                              }

                                                       }

                                                       //writeFileLog"[TestSingleNoteAscrm()]  Press any key to continue(Press 1): ");
				                 
				                       scanf("%d",&input);
                                             
                                                       

                                    }


                                    ///////////////////////////////////////////////////////////////////////////////

                                    
		                    fare1=0;
				   
		                    //writeFileLog"[TestSingleNoteAscrm()] ENTER FARE: ");
				   
		                    scanf("%d",&fare1);
		                   
		                    //writeFileLog"[TestSingleNoteAscrm()] ENTER TRANSACTION TIME IN SECOND: ");
				   
		                    scanf("%d",&transtime);
				   
                                   ////////////////////////////////////////////////////////////////////////////////////////////////
 

                                    rtcode=0;

                                    rtcode = GetValidNote(fare1,transtime*1000);

			            if( rtcode >=1 )
                                    {

                                            //writeFileLog"[TestSingleNoteAscrm()] GetValidNote() return successfully. ");

                                            /*

                                            rtcode =-1;

				            rtcode = getDenomination(arry);
                                           
                                            for( row=0;row<MAX_NMBR_OF_NOTE;row++)
					    {
						 if(arry[row][1]>0) 
                                                 {
						     UserAmountInput =GetDenomValue(arry[row][1]); 
                                                 }

					    }
                                           
                                            */ 

                                            if( rtcode > 1 )
                                            {
		                                    memset(log,'\0',100);

				                    sprintf(log,"[TestSingleNoteAscrm()] >> User Given Fare = %d",rtcode); 
				                    
		                                    //writeFileLoglog);

                                            }
                                            else if( 1 == rtcode  )
                                            {
		                                    memset(log,'\0',100);

				                    sprintf(log,"[TestSingleNoteAscrm()] >> User Given Fare = %d",fare1); 
				                    
		                                    //writeFileLoglog);

                                            }


					    ///////////////////////////////////////////////////////////////////////////////
                                            
                                            NoteChoice =0;

                                            //writeFileLog"[TestSingleNoteAscrm()] Now Press 1 for Stack or 2 for Return : ");
			   
                                            scanf("%d",&NoteChoice);
                           
                                            if( 1 == NoteChoice )
                                            {
                                                 //writeFileLog"[TestSingleNoteAscrm()] Now Going to accept notes ");
			                         
                                                 if( 1 == AcceptCurrentNote(UserAmountInput,0)  )
                                                 {
                                                        totalnotes = totalnotes+1;

                                                 }
                                                 

                                                 //writeFileLog"[TestSingleNoteAscrm()]  Press any key to continue(Press 1): ");
				                 
				                 scanf("%d",&input);
  

                                            }
                                            else if( 2 == NoteChoice )
                                            {
                                                  //writeFileLog"[TestSingleNoteAscrm()] Now Going to return notes ");
			                          
                                                  ReturnCurrentNote(0);

                                                  //writeFileLog"[TestSingleNoteAscrm()]  Press any key to continue(Press 1): ");
				                  
				                  scanf("%d",&input);

                                            }

                                           ////////////////////////////////////////////////////////////////////////////////
			           

                                    }
                                    else if( (-2==rtcode) || (-3==rtcode) )
                                    {
                                         //writeFileLog"[TestSingleNoteAscrm()]  Note Rejected. ");

                                         //writeFileLog"[TestSingleNoteAscrm()]  Press any key to continue(Press 1): ");

				         scanf("%d",&input);

                                         continue;
                                    }
                                    else if( 0 ==rtcode  )
                                    {
                                         //writeFileLog"[TestSingleNoteAscrm()]  GetValidNote() Timout. ");

                                         //writeFileLog"[TestSingleNoteAscrm()]  Press any key to continue(Press 1): ");

				         scanf("%d",&input);

                                         continue;
                                    }
				    else
                                    {
                                         //writeFileLog"[TestSingleNoteAscrm()]  GetValidNote() called failed Please retry some times later. ");
                                         break;

                                    }
				    

                                   


				   
			  } // End: while loop

                          //////////////////////////////////////////////////////////////////////////////////////////////////
			 
		   } // End:  for loop




}//TestAscrm() end

#endif

static void TestAscrmCassetteStatus(int portno)
{

                   int ret=0;
                   unsigned char log[100];
                   memset(log,'\0',100);

                   ret =  activateDevice(portno,0,5000);
		           memset(log,'\0',100);
                   sprintf(log,"[TestAscrm()] ACTIVATE DEV STATUS: %d",ret ); 
                   //writeFileLoglog);

				   if( (2==ret) || (1==ret) )
				   {
					   
								memset(log,'\0',100);

								sprintf(log,"[TestAscrm()] ACTIVATE DEV STATUS: %d  ",ret ); 

								//writeFileLoglog);

								
				  }
                  else
                  {
                        memset(log,'\0',100);

                        sprintf(log,"[TestAscrm()] ACTIVATE DEV STATUS: %d  system now exit.",ret ); 

                        //writeFileLoglog);
                      
                        return;

                  }  

                  while(1)
                  {

                      ret = 0;
                      ret = deviceStatus(0);
                
 
                  }//while loop end               
 
}//TestAscrmCassetteStatus() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void TestAscrm(int portno)
{


		   
                  //Log Test
                  char *deviceid="JCM";
                  strcpy(g_Deviceid,deviceid);
                  g_LogModeLevel = (int)ALL_LOG;
                  
                  int counter=0,ret=0,rtcode=0,
                       UserAmountInput = 0,
                       prevfare=0,row=0 ,
                       col=0,arry[MAX_NMBR_OF_NOTE][2]={0},
                       acptfarertcode=0,
                       fare1=0,transtime=60,maxcash=0,maxcoin=0;
                   
                   double diff=0;
                   time_t start=0,end=0;
                   unsigned char log[100];
                   memset(log,'\0',100);
                   int EscrowMode=0;
                   int DeviceType=0;
                  
                   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
                   {
                        for( col=0;col<2;col++)
                        {
                               arry[row][col]=0;
                        }

                   }
        
                   ///////////////////////////////////////////////////////////////////////////
                   
                   ////writeFileLog"[TestAscrm()] ENTER DeviceType[ 1:NoteAcceptor 2:CoinAcceptor+CoinEscrow ]: ");
	               //scanf("%d",&DeviceType);


                   DeviceType = 1;
                   
                   //++//writeFileLog"[TestAscrm()] ENTER Note Acceptor COM PORT NUMBER[0-255]: ");
	               //++scanf("%d",&portno);

                   //++//writeFileLog"[TestAscrm()] ENTER ESCROW MODE FOR [ 0: Retain Note/Coins 1: Collec Note/Coin to CollectionBin ]: ");
	               //++scanf("%d",&EscrowMode);
	               
	               portno = 1;
                   EscrowMode = 0;
                   
                   ret =  activateDevice2(portno,DeviceType,EscrowMode,5000);
		           memset(log,'\0',100);
                   sprintf(log,"[TestAscrm()] ACTIVATE Note Acceptor STATUS: %d",ret ); 
                   //writeFileLoglog);
                   
                   
                   DeviceType = 2;
                   int ret2=0;
                   
                   //++//writeFileLog"[TestAscrm()] ENTER Coin Acceptor COM PORT NUMBER[0-255]: ");
	               //++scanf("%d",&portno);

                   //++//writeFileLog"[TestAscrm()] ENTER ESCROW MODE FOR [ 0: Retain Note/Coins 1: Collec Note/Coin to CollectionBin ]: ");
	               //++scanf("%d",&EscrowMode);

                   portno = 0;
                   EscrowMode = 0;
                   
                   ret2 =  activateDevice2(portno,DeviceType,EscrowMode,5000);
		           memset(log,'\0',100);
                   sprintf(log,"[TestAscrm()] ACTIVATE Coin Acceptor STATUS: %d",ret2 ); 
                   //writeFileLoglog);
                   
				   if( ret < 1 )
				   {
		       
                        memset(log,'\0',100);

                        sprintf(log,"[TestAscrm()] ACTIVATE DEV STATUS: %d thats why system cannot run now ",ret ); 

                        //writeFileLoglog);

                        return;

		           }else if( 2 == ret ) { //++activated successfully but few notes in escrow
                   

                       printf("\n[TestAscrm()] Press 1 for accept escrow notes ,\n 2 for return escrow notes and press any other key to go for system default choice which is accept escrow notes");

                       char ch=0;

					   ch=getchar();

					   switch(ch)
					   {
									  case '1': defaultCommit(1000); 
												break;

									  case '2': defaultCancel(1000);
												break;

									  default : defaultCommit(1000); 
												break;
						  
					   }//switch

                  }//else if
		  
                  for( ;; )
		          {
			

                          acptfarertcode=0;

					      prevfare=0;
						   
						  //writeFileLog"\n\n\n\n\n\n[TestAscrm()] Press any key to continue or Press 1 for exit : ");
						  char ch=0;
						  ch=getchar();
						  if( '1' == ch)
						  {
							  //writeFileLog"[TestAscrm()]) Program now exit by user. ");
							  return;
						  }
									   
					      ret = deviceStatusV2( 1,0 ); 				   
                        
                          memset(log,'\0',100);
                          sprintf(log,"[TestAscrm()] Note Acceptor Before And Device Status Anding Code : 0x%x",ret); 
                          //writeFileLoglog);

                          ret = (ret&0x02)>>1;
                          memset(log,'\0',100);
                          sprintf(log,"[TestAscrm()] Note Acceptor After And Device Status Code : %d",ret); 
                          //writeFileLoglog);
                          
                          if( 0 ==  ret ){
								//writeFileLog"[TestAscrm()] Press any key to continue: ");
								char ch=0;
								ch=getchar();
								continue;
						  }
						  
                          ret = deviceStatusV2( 2,0 ); 				   
                        
                          memset(log,'\0',100);
                          sprintf(log,"[TestAscrm()] Coin Acceptor Before And Device Status Anding Code : 0x%x",ret); 
                          //writeFileLoglog);

                          ret = (ret&0x02)>>1;
                          memset(log,'\0',100);
                          sprintf(log,"[TestAscrm()] Coin Acceptor After And Device Status Code : %d",ret); 
                          //writeFileLoglog);

               
                          memset(log,'\0',100);

                          sprintf(log,"[TestAscrm()] Both Device Status Code: %d",ret); 
                           
                          //writeFileLoglog);

                          fare1=0;
			   
                          //writeFileLog"[TestAscrm()] ENTER FARE: ");
			              scanf("%d",&fare1);

                          //writeFileLog"[TestAscrm()] ENTER FARE TIME: ");
			              scanf("%d",&transtime);
                          
                          //writeFileLog"[TestAscrm()] ENTER MaxCash[0-1000]: ");
	                      scanf("%d",&maxcash);

                          //writeFileLog"[TestAscrm()] ENTER MaxCoin[ 0-1000]: ");
	                      scanf("%d",&maxcoin);
                           
                          //maxcash=100;
                          //maxcoin=0;
			   
                          //Clear Array
                          for( row=0;row<MAX_NMBR_OF_NOTE;row++)
						  {
								for( col=0;col<2;col++)
								{
									   arry[row][col]=0;
								}

						  }//++ for end
		
                          //++transtime = GetTransTimeout();
                          
                          memset(log,'\0',100);
                          
                          sprintf(log,"[TestAscrm()] Trans Time: %d",transtime  ); 
                          
                          //writeFileLoglog);

                          //////////////////////////////////////////////////////////////////////////////////////////////
                          
                          int DenomMask = 0x00;

                          ////writeFileLog"[TestAscrm()] ENTER Denom Mask Value: ");
			   
                          //scanf("%d",&DenomMask);

                          memset(log,'\0',100);
                          
                          sprintf(log,"[TestAscrm()] Denom Mask Value: %d",DenomMask  ); 
                          
                          //writeFileLoglog);
                          
                          /*

                          EnableTheseDenomination( DenomMask,0);

                          DenomMask = 0x1F;

                          EnableTheseDenomination( DenomMask,0);

                          DenomMask = 0x43;

                          EnableTheseDenomination( DenomMask,0);

                          DenomMask = 0x71;
                       
                          EnableTheseDenomination( DenomMask,0);

                          DenomMask = 0x4F;
                          
                          EnableTheseDenomination( DenomMask,0);


                          DenomMask = 0x7F;
                          
                          EnableTheseDenomination( DenomMask,0);


                          DenomMask = 0x40;
                          
                          EnableTheseDenomination( DenomMask,0);

                          */

                          DenomMask = 255;
                          EnableTheseDenomination( 1, DenomMask,0);
                        
                          /////////////////////////////////////////////////////////////////////////////////////////////

	                      acptfarertcode=acceptFare(fare1,maxcash,maxcoin,transtime*1000);

						  if( false == acptfarertcode){
							  //writeFileLog"[TestAscrm()]) acceptFare() return false again try!! ");
							  continue;
						  }  
						  else if( false == acptfarertcode){
							  //writeFileLog"[TestAscrm()]) acceptFare() return true."); 
						  }  
			  
                          rtcode = 0;
			  
                          UserAmountInput = 0;
			  
                          row=0;col=0;
			      
						  prevfare = 0;

						  /////////////////////////////////////////////////////////////////////////////////////////////////
			 
						  time(&start);

						  while(1)
						  {   

			   
							time(&end);

							diff=end-start;

							if(diff>=transtime)
							{
													   

											   //writeFileLog"[TestAscrm()] >>  Transtimeout by system");  
											   rtcode = getDenomination(arry);

											   memset(log,'\0',100);
                                               sprintf(log,"[TestAscrm()]getDenomination Return code=%d",rtcode); 
                                               //writeFileLoglog);

                                               UserAmountInput=0; 

											   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
											   {
													  if(arry[row][1]>0) 
												     {
														UserAmountInput+=GetDenomValue(arry[row][1]); 
													 }

											   }

                                               memset(log,'\0',100);
                                               sprintf(log,"[TestAscrm()] >> User Given Currency:%d",UserAmountInput); 
                                               //writeFileLoglog);

											   if(fare1==UserAmountInput)
																   {
													//writeFileLog"[TestAscrm()] >>  System now commit your transaction");
												    commitTran_Ver2();

											   }
											   else
                                               {
					                               //writeFileLog"[TestAscrm()] >>  System now cancel your session");

                                                   cancelTran_Ver2();

                                                   //writeFileLog"[TestAscrm()] >>  Before check customer take note from outlet");
                                                   if(UserAmountInput > 0 )
                                                   {
                                                        #ifdef B2B_NOTE_ACCEPTOR
					                                    IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                                        #endif
                                                   }

                                                    //writeFileLog"[TestAscrm()] >>  After check customer take note from outlet");
                                               }
                                               
					                          break;


				                    }//if(diff>=transtime)

                                    rtcode =-1;

				                    rtcode = getDenomination(arry);
			           
                                    ///////////////////////////////////////////////////////////////////////////////////////////

                                    //Accepting State return code 1
									if(1==rtcode)
									{
															  
                                              UserAmountInput=0;
					      
                                              for( row=0;row<MAX_NMBR_OF_NOTE;row++)
											  {
												if(arry[row][1]>0) 
												{
											     UserAmountInput+=GetDenomValue(arry[row][1]); 
											    }
											  }
                                              
											  //system("clear");
											  if(UserAmountInput>prevfare)
											  {
												  memset(log,'\0',100);

												  sprintf(log,"[TestAscrm()] >> User Given Fare = %d",UserAmountInput); 
												  //writeFileLoglog);

												 prevfare=UserAmountInput;

                                              }

					     
                                               
                                    }//if(1==rtcode) end

                                    ///////////////////////////////////////////////////////////////////////////////////////////

									//Exact fare accepted return code 2
									if(2==rtcode) {
															   
						                  //writeFileLog"\n =========================================================================");            
										  memset(log,'\0',100);
						                  sprintf(log,"[TestAscrm()] getDenomination return code %d",rtcode); 
										  //writeFileLoglog);
										  //writeFileLog"\n =========================================================================");      
										  //writeFileLog"[TestAscrm()] >> ACCEPTED FARE GIVEN");  
										  commitTran_Ver2();
										  break;

									}//if(2==rtcode)

                                    
                                    ///////////////////////////////////////////////////////////////////////////////////////////

									//Transactions Timeout. return code 0
									if (0==rtcode)
									{ 
                                               //writeFileLog"\n =========================================================================");      
                                               memset(log,'\0',100);
		                                       sprintf(log,"[TestAscrm()] getDenomination return code %d",rtcode); 
		                                       //writeFileLoglog);
                                               //writeFileLog"\n =========================================================================");      
											   //writeFileLog"[TestAscrm()] >>  Transtimeout by user");  

                                               

                                               UserAmountInput=0;
					      
											   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
											   {
													if(arry[row][1]>0) {
														UserAmountInput+=GetDenomValue(arry[row][1]); 
													}

											   }
                                              
                                               cancelTran_Ver2();

                                               if(UserAmountInput > 0 )
                                               {
                                                     #ifdef B2B_NOTE_ACCEPTOR
					                                 IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                                     #endif
                                               }

					                           break;    
                                             
                                             
         
				                     }//if (0==rtcode)

                                    
                                    ////////////////////////////////////////////////////////////////////////////////////////// 
                                    
                                    // More qty return code 3
									if( 3==rtcode )
									{ 

                                        //writeFileLog"\n =========================================================================");      
                                        memset(log,'\0',100);
                                        sprintf(log,"[TestAscrm()] getDenomination return code = %d",rtcode); 
                                        //writeFileLoglog);
                                        //writeFileLog"\n =========================================================================");      
                                        //writeFileLog"[TestAscrm()] >> More note and coin inserted its time for cancel by system");            
                                        UserAmountInput=0;
					                    for( row=0;row<MAX_NMBR_OF_NOTE;row++)
									    {
											if(arry[row][1]>0) 
																{
											UserAmountInput+=GetDenomValue(arry[row][1]); 
																}

									    }

                                        cancelTran_Ver2();

                                        if(UserAmountInput > 0 )
                                        {
                                                   #ifdef B2B_NOTE_ACCEPTOR
                                                   IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                                   #endif
                                        }
					                    break;


                                              
                                    }//if( 3==rtcode )
				   
				    
                                    ///////////////////////////////////////////////////////////////////////////////////////////

				   
			       } // End: while loop

                   //////////////////////////////////////////////////////////////////////////////////////////////////
			 
		   } // End:  for loop

}//TestAscrm() end


#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
