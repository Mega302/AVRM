#include "jcm.h"

#ifdef JCM_NOTE_ACCEPTOR

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Global Variables
static volatile unsigned int g_ThreadCurrentfare=0;
static unsigned int g_JCM_Current_State; 
static unsigned int g_JCM_Accepted_Moneyvalue;
static bool g_ExactFareFlag; 
static volatile bool g_JCM_CashInStart; 
static JCM_ESCROW_NOTE g_currentnote;
static volatile bool g_PollFlag;
static volatile bool g_EscrowEvent;
static volatile bool g_EscrowEventReadComplete;
static volatile bool g_RejectEvent;
static volatile bool g_stopThread;
static int  g_HANDLE=-1;
static bool g_DisableAcceptance;
static bool g_ThreadFinish;
static pthread_mutex_t g_NAEscrowStateComplete= PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_NASetCurrentFare= PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_FaretobeacceptMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_NAEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  g_NAEnableThreadCond = PTHREAD_COND_INITIALIZER;
static  pthread_mutex_t g_NoteLowLevelPollFlagStatusmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t g_stopThreadmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t g_PollFlagmutex=PTHREAD_MUTEX_INITIALIZER;
static  pthread_mutex_t g_AlreadyCoinDisablemutex=PTHREAD_MUTEX_INITIALIZER;
//ATVM API VARIABLES [COMMON]
static pthread_mutex_t g_JCM_State_Mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_JCM_Escrow_State_Mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_NASetSpecificFaremutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_NASetDisableAcceptance = PTHREAD_MUTEX_INITIALIZER;
static volatile unsigned int g_CoinDisableflag = 0;
static volatile unsigned int g_faretobeaccept  = 0;
static bool g_SingleNoteAcceptEvent=false;
static bool g_AlreadyCoinDisable=false;
static bool g_AlreadyNotePollFlag=false;
static unsigned int g_state;
static unsigned int g_NACurrentAtvmFare;
static unsigned int g_SpecificNoteInhibitFlag;
static unsigned int g_AllNoteInhibitFlag;
static unsigned int g_NoteLowLevelPollFlagStatus=-1;
static bool g_NADisableAcceptanceFlag;
static pthread_mutex_t g_NADisableAcceptanceFlagMutex= PTHREAD_MUTEX_INITIALIZER;
static bool g_NA_NoteAcceptanceFlag;
static pthread_mutex_t g_NA_NoteAcceptanceFlagMutex= PTHREAD_MUTEX_INITIALIZER;
static int g_JcmPortNumber=-1;
int g_StackAcceptedTime;
int g_ReturnAcceptedTime;
int g_NoteAcceptingFlag;
static unsigned char  g_BNAId[MAX_STR_SIZE_OF_LOG];

static pthread_mutex_t g_RejectEventFlagMutex= PTHREAD_MUTEX_INITIALIZER;
static bool g_ExactFare_RejectEvent;

bool getNARejectEventFlagStatus(){
       
       //AVRM_writeFileLogV2("[getNARejectEventFlagStatus()] Entry.",INFO,g_BNAId);
       bool rejectEventFlag=false;
       pthread_mutex_lock(&g_RejectEventFlagMutex);
       rejectEventFlag = g_ExactFare_RejectEvent ;
       pthread_mutex_unlock(&g_RejectEventFlagMutex);	
       //AVRM_writeFileLogV2("[getNARejectEventFlagStatus()] Exit.",INFO,g_BNAId);
       return rejectEventFlag;
       
}//bool getRejectEventFlagStatus() end

void setNARejectEventFlagStatus(fnrejectEventFlag){
       
       AVRM_writeFileLogV2("[setNARejectEventFlagStatus()] Entry.",INFO,g_BNAId);
       pthread_mutex_lock(&g_RejectEventFlagMutex);
       g_ExactFare_RejectEvent = fnrejectEventFlag;
       pthread_mutex_unlock(&g_RejectEventFlagMutex);	
       AVRM_writeFileLogV2("[setNARejectEventFlagStatus()] Exit.",INFO,g_BNAId);
       
}//bool setRejectEventFlagStatus(fnrejectEventFlag) end

int SetBNADeviceId(unsigned char* fnBNADeviceID ) {
    memset(g_BNAId,'\0',MAX_STR_SIZE_OF_LOG);
    strcpy( g_BNAId ,fnBNADeviceID );
    //printf("\n[SetBNADeviceId()] log dll file path=%s \n",fnBNADeviceID);
    return 1;
}//int  SetBNADeviceId(unsigned char* fnBNADeviceID ) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int IssueGetRomVersion(unsigned char* Response ){
      
        int rtcode =-1,counter=0;
        char log[200];
        memset(log,'\0',200);
        int ResponseLength=30;
        unsigned char Commands[JCM_CMD_CMN_LENGTH +2]=JCM_ROMVERSION_CMD;
        rtcode=JCM_GetReply(JCM_CMD_CMN_LENGTH+2,ResponseLength,Response,Commands,4);
        if( 1 == rtcode ){
           //WriteFormattedDataExchange(Response,"Rx",ResponseLength );
           return 1;
        }else{
           AVRM_writeFileLogV2("[IssueGetRomVersion()] No reply bytes receieved.",ERROR,g_BNAId);
           return 0;
        }
        
}//unsigned char* IssueGetRomVersion() end

//Generic Note Acceptor Initilzation 
int OpenCashAcptr(int noteAcptrPortNmbr){
   return (JCM_Activate(noteAcptrPortNmbr));
}//int OpenCashAcptr(int noteAcptrPortNmbr) end

int Deactivate_JCM_NoteAcptr(){
   return (JCM_Deactivate());
}//int Deactivate_JCM_NoteAcptr() end

//Generic Note Acceptor Device Status
int GetNoteAcptrStatus(){
  return (JCM_GetStatus());
}//int GetNoteAcptrStatus() end

int GetNoteAcptrStatusV2(){
	return (JCM_GetStatusV2());
}//int GetNoteAcptrStatusV2() end

int UnloadCashAcptr(){
   //++InDirect Call to unload operation
    return( JCM_Unload() );
}//int UnloadCashAcptr() end

int  DispenseCashAcptr(){
   return ( JCM_Dispense() );
}

//Start Note Acceptor Credit Poll
unsigned int  NoteAcptrStartCreditPollThread(){
	return ( JCM_NoteAcptrCreatePollThread());
}

void  CloseNoteAcceptorPollThread(){
       JCM_CloseNoteAcceptorPollThread();
       return;
}

 void InitNATransModel(){
	    JCM_Atvm_CashInStart();
		JCM_Atvm_InitCashInStartThread();
		return;
}

 void IssuedisableAllBillCmd(){
       //JCM_delay_miliseconds(200);
       DisableAcceptance();
       return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//New ASCRM API
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
      -4 : Any other exception
      
      -3 : Note rejected due to insertion of invalid note

      -2 : Note Rejected due to insertion of disallow denomination 
 
      -1 : Communication failure

      0 : Operation timeout

      1 : Note of correct denomaination validated

*/

/*

int  JCMSingleNote_GetValidNote(int Denom,int Timeout)
{

        unsigned char log[200];

        memset(log,'\0',200);

        if( SUCCESS != B2BSingleNote_EnableNoteAcceptor(Denom) )
        {
             //writeFileLog"[B2BSingleNote_GetValidNote()] B2B bill enable failed.");

             return (-4);//Any other exception

        }
        else
        {
             //writeFileLog"[B2BSingleNote_GetValidNote()] Successfully enable b2b note accpetor.");

        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////

        int state =0x00,amount=0,rtcode=0;

        int rcvPktLen =0;

        unsigned char rcvPkt[100];

        memset(rcvPkt,'\0',100);

        struct timespec begints, endts,diffts;

        clock_gettime(CLOCK_MONOTONIC, &begints);

        while(1)
        {

                            state =0x00;

                            memset(rcvPkt,'\0',100);

                            rcvPktLen =0;
                         
                            //issue delay command between two poll command
                            delay_mSec(200);  

                            //Issue delay
                            CASHCODE_NoteAcptr_sendPollCmd();
		        
                            if(  SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen)  )
			    {
                                    

					    //if successfullt get poll reply bytes then inform b2b with ack status  

		                            CASHCODE_NoteAcptr_sendACK(); 

		                            state =rcvPkt[3] ;	
		  
					  
					    switch(state)
					    {
							 
		                                         case CASHCODE_RES_ACK:
							      break;
						         
							 case CASHCODE_RES_POWERUP:

		                                              //writeFileLog"[B2BSingleNote_GetValidNote()] Powerup State.");			 
							      break;
		                                         
							 case CASHCODE_RES_INITIALIZE:      
							      //writeFileLog"[B2BSingleNote_GetValidNote()] Initialize state.");
							      break;

							 case CASHCODE_RES_DISABLED: 
		                                              //writeFileLog"[B2BSingleNote_GetValidNote()] Disable state.");         
		                                              break;


							 case CASHCODE_RES_IDLING:
		                                              #ifdef B2B_LOGLEVEL_ONE          
							      //writeFileLog"[B2BSingleNote_GetValidNote()] Idling state.");
							      #endif 
							      break;

							 case CASHCODE_RES_ACCEPTING:    		
							      #ifdef B2B_LOGLEVEL_ONE

							      ////writeFileLog"[B2BSingleNote_GetValidNote()] Accepting state.");
							      #endif 
							      break;

							 case CASHCODE_RES_ESCROW:
							     
							      //writeFileLog"[B2BSingleNote_GetValidNote()] Escrow state.");

                                                             
		                                              //issue hold command
                                                              CASHCODE_NoteAcptr_hold();

                                                              //Get Denom info
		                                              CASHCODE_NoteAcptr_noteAmountReceived(rcvPkt,&amount); 

		                                              //Set Denom Info
		                                              SetGlobalDenomInfo( amount );

                                                              ////////////////////////////////////////////////////////////////

                                                              if( amount > Denom )
                                                              {
                                                                  //writeFileLog"[B2BSingleNote_GetValidNote()] More Denom Accepted.");          
                                                                  rtcode = amount;
                                                                    
                                                              }
                                                              else if( amount == Denom )
                                                              {
                                                                  //writeFileLog"[B2BSingleNote_GetValidNote()] Equal Denom Accepted.");          
                                                                  rtcode = 1;
                                                      
                                                              }
                                                              else if( amount < Denom )
                                                              {
                                                                  //writeFileLog"[B2BSingleNote_GetValidNote()] Less Denom Accepted.");          
                                                                  rtcode = amount;
                                                      
                                                              }

                                                              ////////////////////////////////////////////////////////////////
                                                              //Now Exit
                                                              
                                                              clock_gettime(CLOCK_MONOTONIC, &endts);
 
                                                              diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                                              memset(log,'\0',200);

		                                              sprintf(log,"[B2BSingleNote_GetValidNote()] Denom value =%d withing %d Second.",amount,diffts.tv_sec ); 

		                                              //writeFileLoglog);

		                                              return rtcode ; //Note of correct denomaination validated
							      
							      break;


							 case CASHCODE_RES_RETURNING:  
	     
							      #ifdef B2B_LOGLEVEL_ONE

							      //writeFileLog"[B2BSingleNote_GetValidNote()] Returning state.");          
		                                              #endif  
							      break;

							      
							 case CASHCODE_RES_INVALID_BILL_NMBR:
		                                              #ifdef B2B_LOGLEVEL_ONE
							      //writeFileLog"[B2BSingleNote_GetValidNote()] Invalid bill number state.");          
		                                              #endif    
					  	              
							      break;
							      

							 case CASHCODE_RES_HOLDING:                 
							      #ifdef B2B_LOGLEVEL_ONE
							      ////writeFileLog"[B2BSingleNote_GetValidNote()] Holding state.");
							      #endif  
							      break;

							 case CASHCODE_RES_REJECTING: 
                                                              //issue disable command
                                                              IssuedisableAllBillCmd();
 
		                                              //Reject Notes cause return
		                                              return ( B2BSingleNote_GetDenomRejectCode( rcvPkt[4] ) );

							      break;


							 case CASHCODE_RES_STACKING:   
							      #ifdef B2B_LOGLEVEL_ONE
							      ////writeFileLog"[B2BSingleNote_GetValidNote()] Stacking state.");
							      #endif
							      break;

							 case CASHCODE_RES_STACKED: 
		                                              //writeFileLog"[B2BSingleNote_GetValidNote()] Stacked state.");
		                                              break;


							 case CASHCODE_RES_DISPENSING:      
							      break;

							 
							 case CASHCODE_RES_DISPESED:
							      break;
							 

							 case CASHCODE_RES_UNLOADING:       
							      break;
							 
							 case CASHCODE_RES_UNLOADED:       
							      break;
							 
							 case CASHCODE_RES_INVALID_CMD:    
		                                              #ifdef B2B_LOGLEVEL_ONE 
							      //writeFileLog"[B2BSingleNote_GetValidNote()] Invalid command state.");     
		                                              #endif
							      break;

							 
							 case CASHCODE_RES_RETURNED:
		                                              #ifdef B2B_LOGLEVEL_ONE
		                                              //writeFileLog"[B2BSingleNote_GetValidNote()] Returned state.");
		                                              #endif
							      
							      break;

							 case CASHCODE_RES_ACPTR_JAM:      
							     //#ifdef B2B_LOGLEVEL_ONE
		                                             //writeFileLog"[B2BSingleNote_GetValidNote()] Accepter Jam State.");
		                                             //#endif
							     
							     break; 

							 case CASHCODE_RES_STKR_JAM: 
							     //#ifdef B2B_LOGLEVEL_ONE
		                                             //writeFileLog"[B2BSingleNote_GetValidNote()] Stacker Jam State");
		                                             //#endif
							          
							     break;        

							 case CASHCODE_RES_CSTFULL:
							      //#ifdef B2B_LOGLEVEL_ONE
		                                              //writeFileLog"[B2BSingleNote_GetValidNote()] Drop Cassette full state."); 
		                                              //#endif
							               
							      break; 

							 case  CASHCODE_RES_CSTREMOVED:  
							     //#ifdef B2B_LOGLEVEL_ONE
		                                             //writeFileLog"[B2BSingleNote_GetValidNote()] Drop Cassette removed state.");
		                                             //#endif

							                 
							     break; 		 

							 case CASHCODE_RES_GENERIC_FAIL:
							      //#ifdef B2B_LOGLEVEL_ONE
		                                              //writeFileLog"[B2BSingleNote_GetValidNote()] Note Acceptor Generic Fail");
		                                              //#endif
			                    		                        
							      break; 

							  case CASHCODE_RES_CHEATED:
							       #ifdef B2B_LOGLEVEL_ONE
		                                               //writeFileLog"[B2BSingleNote_GetValidNote()] Cheated State");
		                                               #endif
							       break;	
		  
							 
							 case  CASHCODE_RES_RCYCL_CST_STAT:  
		                                               #ifdef B2B_LOGLEVEL_ONE
							       //writeFileLog"[B2BSingleNote_GetValidNote()] Rcycl Cst Stat");
		                                               #endif
		                                               break; 

		                                         default:		 
							         //writeFileLog"[B2BSingleNote_GetValidNote()] No Valid b2b state found.");
								 break; 


				    }  // End switch(state)         
                            
                            }//if(  SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen)  ) block

		            
                            //Timeout Check
                            clock_gettime(CLOCK_MONOTONIC, &endts);
 
                            diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                            if( (diffts.tv_sec*1000) >= Timeout )
                            {

                                     //issue disable command
                                     IssuedisableAllBillCmd();

                                     memset(log,'\0',200);

		                     sprintf(log,"[B2BSingleNote_GetValidNote()] Timeout Happened =%d .",diffts.tv_sec ); 

		                     //writeFileLoglog);

                                     return 0; //Timeout happened

                           }


                     
         }//while loop


}//int  B2B_GetValidNote(int Denom,int Timeout) END


int  JCMSingleNote_AcceptCurrentNote(int Denom,int Timeout)
{

     //Issue stack comamnd
     if( SUCCESS == CASHCODE_NoteAcptr_stack() )
     {
           //writeFileLog"[B2BSingleNote_AcceptCurrentNote()] B2B stack command successfully done.");

           if( 1 == WaitforState(CASHCODE_RES_STACKED,WAIT_TIME_STACKED))
	   {    
		 //writeFileLog"[B2BSingleNote_AcceptCurrentNote()] Note Stacked State found successfully.");

                 //issue disable command
                 IssuedisableAllBillCmd();

                 return 1; 
 
	   }
	   else
	   {
		 //writeFileLog"[B2BSingleNote_AcceptCurrentNote()] Note Stack Command issue successfully but unable to fund any stacked state.");

		 return 0; //Operation timeout

	   }
           
     }
     else
     {
         //writeFileLog"[B2BSingleNote_AcceptCurrentNote()] B2B stack command failed to do.");
         return (-3);//Any other exception 
     }



}//int  B2B_AcceptCurrentNote(int Denom,int Timeout)


int  JCMSingleNote_ReturnCurrentNote(int Timeout)
{
 
     //issue return command

     if( SUCCESS == CASHCODE_NoteAcptr_return() )
     {
           //writeFileLog"[B2BSingleNote_ReturnCurrentNote()] B2B stack command successfully done.");

           if( 1 == WaitforState( CASHCODE_RES_RETURNED, WAIT_TIME_RETURNED ))
	   {    
		 //writeFileLog"[B2BSingleNote_AcceptCurrentNote()] Note Returned State found successfully.");

                 //issue disable command
                 IssuedisableAllBillCmd();

                 return 1; 
 
	   }
	   else
	   {
		 //writeFileLog"[B2BSingleNote_AcceptCurrentNote()] Note Returned Command issue successfully but unable to fund any returned state.");

		 return 0; //Operation timeout

	   }  
     }
     else
     {
         //writeFileLog"[B2BSingleNote_ReturnCurrentNote()] B2B stack command failed to do.");
         return (-3);//Any other exception 
     }


}//int  B2B_ReturnCurrentNote(int Timeout)


int JCMSingleNote_EnableNoteAcceptor(int amount)
{

                              
		                    noteType NOTE;

		                    unsigned char rcvPkt[100]; 

		                    memset(rcvPkt,'\0',100);

			            unsigned int  rcvPktLen=0;

		                    int state = -1; 

                                    ///////////////////////////////////////////////////////////////////////
                                    
                                
                                    
                                    //////////////////////////////////////////////////////////////////////


                                    NOTE.rs_5 =ENABLE ;

                                    NOTE.rs_10 =ENABLE ;

                                    NOTE.rs_20 =ENABLE ;

                                    NOTE.rs_50 =ENABLE ;

                                    NOTE.rs_100 =ENABLE ;

                                    NOTE.rs_500 =ENABLE ;

                                    NOTE.rs_1000 =ENABLE ;
                                  
                                    //////////////////////////////////////////////////////////////////////
                               
		                     B2BSingleNote_IssueCommand(NOTE);
				       
		                     CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);      
				       
		                     CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);  
		                       
		                     //b2b not ack       
				     if( CASHCODE_RES_ACK != state )
				     {
					   //writeFileLog"[B2BSingleNote_EnableNoteAcceptor()] Bill enable command not ack by b2b.");
		                           return FAIL;
		                           
				     }
				     else //if b2b ack bill enable command
				     {
					   
		                            //writeFileLog"[B2BSingleNote_EnableNoteAcceptor()] Bill enable successfully ack by b2b.");
		                            return SUCCESS;
				     }



}//int EnableNoteAcceptor(noteType NOTE) end


bool JCMSingleNote_IssueCommand(noteType NOTE)
{

	    unsigned int i=0;

	    unsigned char crc1=0x00, crc2=0x00;

            char noteEnableByte = 0x00; 
      
	    unsigned char escrow = 0;

	    unsigned char sbytes[20]; 

            memset(sbytes,'\0',20);

	    int ret = SUCCESS;

	    if( ENABLE == NOTE.rs_5 )
            {
                 noteEnableByte = noteEnableByte | 0x01; //0b00000001;
                 #ifdef B2B_LOGLEVEL_TWO
                 //writeFileLog"[B2BSingleNote_IssueCommand()] Rs. 5 Enable. "); 
                 #endif
            }
            if( ENABLE == NOTE.rs_10 )
	    {
		  noteEnableByte = noteEnableByte | 0x02;//0b00000010;
                  #ifdef B2B_LOGLEVEL_TWO
                  //writeFileLog"[B2BSingleNote_IssueCommand()] Rs. 10 Enable. "); 
                  #endif
            }
            if( ENABLE == NOTE.rs_20 )
            {
		  noteEnableByte = noteEnableByte | 0x04;//0b00000100;
                  #ifdef B2B_LOGLEVEL_TWO
                  //writeFileLog"[B2BSingleNote_IssueCommand()] Rs. 20 Enable. ");  
                  #endif
	    }
            if( ENABLE == NOTE.rs_50 )
	    {
                  noteEnableByte = noteEnableByte | 0x08;//0b00001000;
                  #ifdef B2B_LOGLEVEL_TWO
                  //writeFileLog"[B2BSingleNote_IssueCommand()] Rs. 50 Enable. "); 
                  #endif
	    }
            if( ENABLE == NOTE.rs_100 )
            {
		  noteEnableByte = noteEnableByte | 0x10;//0b00010000;
                  #ifdef B2B_LOGLEVEL_TWO
                  //writeFileLog"[B2BSingleNote_IssueCommand()] Rs. 100 Enable. "); 
                  #endif
	    }
            if( ENABLE == NOTE.rs_500 )
	    {
		  noteEnableByte = noteEnableByte | 0x20;//0b00100000;
                  #ifdef B2B_LOGLEVEL_TWO
                  //writeFileLog"[B2BSingleNote_IssueCommand()] Rs. 500 Enable. "); 
                  #endif
	    }
            if( ENABLE == NOTE.rs_1000 )
            {
		  noteEnableByte = noteEnableByte | 0x40;//0b01000000;
                  #ifdef B2B_LOGLEVEL_TWO
                  //writeFileLog"[B2BSingleNote_IssueCommand()] Rs. 1000 Enable. ");  
                  #endif
            }
          
             
        
           ////////////////////////////////////////////////////////////////////////////////////////////////////

           sbytes[0] = CASHCODE_SYNC;  

	   sbytes[1] = DEVICE_ADDRESS;
 
	   sbytes[2] = 0x0C;              //hole command Length

	   sbytes[3] = CASHCODE_CMD_ENABLE_BILL_TYPE; 
	   
           sbytes[4] = 0x00;              //Y1
	   sbytes[5] = 0x00;              //Y2
	   sbytes[6] = noteEnableByte;    //Y3
	   //Escrow bytes
           sbytes[7] = 0x00;              //Y4
	   sbytes[8] = 0x00;              //Y5
	   sbytes[9] = 0xFF;              //Y6

           GetCRC16(sbytes,10,&crc1,&crc2);

           sbytes[10]  = crc1;

	   sbytes[11]  = crc2;
            
           for(i=0;i< 12; i++)
           {
	      ret = SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
           }

           if(SUCCESS==ret)
           {
                #ifdef B2B_LOGLEVEL_TWO
                //writeFileLog"[B2BSingleNote_IssueCommand()] Enable bill type Cmd sent with success status."); 
                #endif
                return true;
           }
           else
           {
                #ifdef B2B_LOGLEVEL_TWO
                //writeFileLog"[B2BSingleNote_IssueCommand()] Enable bill type Cmd sent with fail status");
                #endif 
                return false;
           
           }

          //////////////////////////////////////////////////////////////////////////////////////////////////////////



}//B2BSingleNote_IssueCommand() end

*/

void enableNaExactFareFlag(){
	  AVRM_Currency_writeFileLog("[enableExactFareFlag()] Single Note Enable",INFO);
      g_ExactFareFlag=true;
}//void enableExactFareFlag() end

void disableNAExactFareFlag(){
	 AVRM_Currency_writeFileLog("[enableExactFareFlag()] Single Note Disabled",INFO);
     g_ExactFareFlag=false;
}//++void enableExactFareFlag() end

static NOTE g_CurrentNOTE={0,0,0,0,0,0,0,0,0};
static bool g_CurrentDenomEnableFlag=false;
static pthread_mutex_t g_DenomStructmutex=PTHREAD_MUTEX_INITIALIZER;

void SetDenomStruct(noteType fnNOTE)
{

						pthread_mutex_lock(&g_DenomStructmutex);
						
						g_CurrentNOTE.INR_5    		= 0; 
						g_CurrentNOTE.INR_10    	= 0; 
						g_CurrentNOTE.INR_20    	= 0; 
						g_CurrentNOTE.INR_50    	= 0; 
						g_CurrentNOTE.INR_100    	= 0; 
						g_CurrentNOTE.INR_200    	= 0; 
						g_CurrentNOTE.INR_500    	= 0; 
						g_CurrentNOTE.INR_1000    	= 0;
						g_CurrentNOTE.INR_2000    	= 0;
                                  
                
						if( 1 == fnNOTE.rs_5 )
						{
			                 g_CurrentNOTE.INR_5    = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.5  is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_10 )
                        {
			                 g_CurrentNOTE.INR_10    = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.10 is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_20 )
                        {
			                 g_CurrentNOTE.INR_20    = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.20  is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_50 )
                        {
			                 g_CurrentNOTE.INR_50    = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.50  is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_100 )
                        {
			                 g_CurrentNOTE.INR_100   = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.100  is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_200 )
                        {
			                 g_CurrentNOTE.INR_200   = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.200  is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_500 )
                        {
			                 g_CurrentNOTE.INR_500  = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.500  is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_1000 )
                        {
			                 g_CurrentNOTE.INR_1000    = 1; 
                             AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.1000  is enable.",INFO,g_BNAId);
                        }
                        
                        if( 1 == fnNOTE.rs_2000 )
                        {
			                g_CurrentNOTE.INR_2000  = 1; 
                            AVRM_writeFileLogV2("[ SetDenomStruct() ] Rs.2000 is enable.",INFO,g_BNAId);
                        }
        
						g_CurrentDenomEnableFlag = true;
						
						pthread_mutex_unlock(&g_DenomStructmutex); 
						

}//void SetDenomStruct() end

int EnableDenomDuringCreditPolling(int fare,int flag) 
{
                

					int rtcode=-1,CmdLength=0,Counter=0;

					unsigned char DefaultEnablebyte=0x00;
					unsigned char DefaultEnablebyte2=0x00;

					unsigned char Response[100];

					memset(Response,'\0',100);

					char log[200];
					
					NOTE UserdefEnableNotes={0,0,0,0,0,0,0,0,0};

					memset(log,'\0',200);

					unsigned char DittoCommands[ JCM_CMD_DITTO_INSERTION_AUTH_LENGTH ]	= JCM_INSERTION_AUTHORISED_DITTO_CMD;
					unsigned char Commands[ JCM_CMD_INSERTION_AUTH_LENGTH ]				= JCM_INSERTION_AUTHORISED_CMD;

					if(true == g_ExactFareFlag ) 
					{
					
						   AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()]  Exact Fare Block Active.",INFO,g_BNAId);

						   switch(fare) {
						   
								   case 5:
									     AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 5 Enable.",INFO,g_BNAId);
									     DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
								         break;
					

								   case 10:
									     AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 10 Enable.",INFO,g_BNAId);
									     DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
									     break;
								

								   case 20:
									     AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 20 Enable.",INFO,g_BNAId);
									     DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
									     break;
					

								   case 50:
										 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 50 Enable.",INFO,g_BNAId);
										 DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
										 break;
									

									case 100:
										AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 100 Enable.",INFO,g_BNAId);
										DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
										break;

									case 200:
										AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 200 Enable.",INFO,g_BNAId);
										DefaultEnablebyte2 = DefaultEnablebyte2 | 0x1; //0b00000001;
										break;
									

									case 500:
										AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 500 Enable.",INFO,g_BNAId);
										//Soomit - 25/11/2016 - Enable 500 Starts
										DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
										//Soomit - 25/11/2016 - Enable 500 Ends
										break;
									

									case 1000:
										AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 1000 Enable.",INFO,g_BNAId);
										//DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
										break;

								   case 2000:
										AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 2000 Enable.",INFO,g_BNAId);
										DefaultEnablebyte = DefaultEnablebyte | 0x80; //0b10000000;
										break;
												 
								  };//switch end
					
					}
					else 
					{				
								AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()]  Exact Fare Block InActive.",INFO,g_BNAId);
								
								/*
								if( fare>=5 ){
									 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 5 Enable.",INFO,g_BNAId);
									 DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
								}//if end

								if( fare>=10 ) {
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 10 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
								}//if end

								if( fare>=20 ){
									 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 20 Enable.",INFO,g_BNAId);
									 DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
								
								}//if end

								if( fare>=50 ){
									 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 50 Enable.",INFO,g_BNAId);
									 DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
							    }//if end

								if( fare>=100 ){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 100 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
								}//if end

								if( fare>=200 ){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 200 Enable.",INFO,g_BNAId);
									DefaultEnablebyte2 = DefaultEnablebyte2 | 0x1; //0b00000001;
								}//if end

								if( fare>=500 ){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 500 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
								}//if end

								if( fare>= 1000){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 1000 Enable.",INFO,g_BNAId);
									//DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
								}//if end
								
								if( fare>= 2000){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 2000 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x80; //0b10000000;
								}//if end
								*/
								
								DefaultEnablebyte=0x00;
								DefaultEnablebyte2=0x00;
																
								pthread_mutex_lock(&g_DenomStructmutex);
								UserdefEnableNotes = g_CurrentNOTE;
								pthread_mutex_unlock(&g_DenomStructmutex); 
								
								/*
								if( fare>=5 ){
									 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 5 Enable.",INFO,g_BNAId);
									 DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
								}//if end

								if( fare>=10 ) {
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 10 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
								}//if end

								if( fare>=20 ){
									 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 20 Enable.",INFO,g_BNAId);
									 DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
								
								}//if end

								if( fare>=50 ){
									 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 50 Enable.",INFO,g_BNAId);
									 DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
							    }//if end

								if( fare>=100 ){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 100 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
								}//if end

								if( fare>=200 ){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 200 Enable.",INFO,g_BNAId);
									DefaultEnablebyte2 = DefaultEnablebyte2 | 0x1; //0b00000001;
								}//if end

								if( fare>=500 ){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 500 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
								}//if end

								if( fare>= 1000){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 1000 Enable.",INFO,g_BNAId);
									//DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
								}//if end
								
								if( fare>= 2000){
									AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Rs 2000 Enable.",INFO,g_BNAId);
									DefaultEnablebyte = DefaultEnablebyte | 0x80; //0b10000000;
								}//if end
								*/
								
								if( 1 == UserdefEnableNotes.INR_5 )
								{

									 DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
									 AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.5  is enable.",INFO,g_BNAId);
								
								}//if end

								if( 1 == UserdefEnableNotes.INR_10 ) 
								{

									 DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
									 AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.10  is enable.",INFO,g_BNAId);

								}//if end

								if( 1 == UserdefEnableNotes.INR_20 ) 
								{

									 DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
									 AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.20  is enable.",INFO,g_BNAId);

								}//if end

								if( 1 == UserdefEnableNotes.INR_50 ) 
								{

									 DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
									 AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.50  is enable.",INFO,g_BNAId);

								}//if end

								if( 1 == UserdefEnableNotes.INR_100 ) 
								{

									DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
									AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.100  is enable",INFO,g_BNAId);

								}//if end

								if( 1 == UserdefEnableNotes.INR_200 )
								{

									DefaultEnablebyte2 = DefaultEnablebyte2 | 0x01; //0b00010000;
									AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.200  is enable",INFO,g_BNAId);

								}//if end
								
								if( 1 == UserdefEnableNotes.INR_500 ) 
								{

									DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
									AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.500  is enable.",INFO,g_BNAId);

								}//if end
								
								if( 1 == UserdefEnableNotes.INR_1000 )
								{

									//++DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
									AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.1000  is enable.",INFO,g_BNAId);

								}//if end
								
								if( 1 == UserdefEnableNotes.INR_2000 ) 
								{

									DefaultEnablebyte = DefaultEnablebyte | 0x80; //0b10000000;
									AVRM_writeFileLogV2("[ EnableDenomDuringCreditPolling() User Defined] Rs.2000  is enable.",INFO,g_BNAId);

								}//if end
								
											
					}//++ else end

                    //#ifdef JCM_DEBUG

		            memset(log,'\0',100);
		            sprintf(log,"[EnableDenomDuringCreditPolling()] DefaultEnablebyte =0x%xh.",DefaultEnablebyte);
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
		            
		            memset(log,'\0',100);
		            sprintf(log,"[EnableDenomDuringCreditPolling()] DefaultEnablebyte2 =0x%xh.",DefaultEnablebyte2);
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);

                    //#endif
	
					if(0x10 == DefaultEnablebyte)
					{
			    
								//++Set MoneyCode byte
								DittoCommands[ 3 ]= DefaultEnablebyte;
								DittoCommands[ 5 ]= DefaultEnablebyte2;
								CmdLength=JCM_CMD_DITTO_INSERTION_AUTH_LENGTH;
								//++Prepare BCC 
								for(Counter=2;Counter<= (CmdLength-2);Counter++) 
								{
									 if(4 == Counter)
									 {
										continue;
									 }
									 else
									 {
										DittoCommands[CmdLength-1]=  DittoCommands[CmdLength-1] ^ DittoCommands[Counter];
									 }//++else end

								}//for end

								//++#ifdef JCM_DEBUG 

								for(Counter=0;Counter<CmdLength;Counter++)
								{
										 memset(log,'\0',100);
										 sprintf(log,"[EnableDenomDuringCreditPolling()] DittoCommands[%d] = 0x%xh.",Counter,DittoCommands[Counter]);
										 AVRM_writeFileLogV2(log,INFO,g_BNAId);

								}//for end

								memset(log,'\0',100);
								sprintf(log,"[EnableDenomDuringCreditPolling()] DittoComamnd Packet BCC =0x%xh.",DittoCommands[CmdLength-1]);
								AVRM_writeFileLogV2(log,INFO,g_BNAId);
							
								//++#endif

			    
		            }
		            else if(0x10!= DefaultEnablebyte) 
		            {
                     

						//Set MoneyCode byte
						Commands[ 3 ]=DefaultEnablebyte;
						Commands[ 4 ]=DefaultEnablebyte2;
						CmdLength=JCM_CMD_INSERTION_AUTH_LENGTH;

						//Prepare BCC 
						for(Counter=2;Counter<= (CmdLength-2);Counter++)
						{
						  Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
						}

						#ifdef JCM_DEBUG
						for(Counter=0;Counter<CmdLength;Counter++)
						{
							memset(log,'\0',100);
							sprintf(log,"[EnableDenomDuringCreditPolling()] Commands[%d] = 0x%xh.",Counter,Commands[Counter]);
							//writeFileLoglog);
						}
						//memset(log,'\0',100);
						//sprintf(log,"[EnableDenomDuringCreditPolling()] Comamnd Packet BCC =0x%xh.",Commands[CmdLength-1]);
						//writeFileLoglog);
						#endif
						
		            }//else if end
		            
		            
		            //delay before command transmission
		            //++JCM_delay_miliseconds(80);

                    if(0x10== DefaultEnablebyte) 
                    {

                           //#ifdef JCM_DEBUG
			               ////writeFileLog"[EnableDenomDuringCreditPolling()] Enablebyte 0x10 found.");
                           //#endif
                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,DittoCommands,CmdLength);

					}
					else if(0x10 != DefaultEnablebyte)
					{

                            //#ifdef JCM_DEBUG
			                //writeFileLog"[EnableDenomDuringCreditPolling()] Enablebyte 0x10 not found.");
                            //#endif
			                rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);

		            }//else if end

					if( JCM_EXTERNAL_COMMAND_REPLY_READ == flag ) 
					{
							//++Receieved Reply
							rtcode = JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_INSERTION_AUTHORIZING_REPLY_SIZE);
							 
							//++Display Enable Denom Packet
							memset(log,'\0',200);
							sprintf(log,"[EnableDenomDuringCreditPolling()] Command = 0x%x h",Response[2]);
							AVRM_writeFileLogV2(log,INFO,g_BNAId);

							memset(log,'\0',200);
							sprintf(log,"[EnableDenomDuringCreditPolling()] State = 0x%x h",Response[3]);
							AVRM_writeFileLogV2(log,INFO,g_BNAId);

							#ifdef JCM_DEBUG
							int i=0;
							for(i=0;i<JCM_INSERTION_AUTHORIZING_REPLY_SIZE;i++)
							{
								 memset(log,'\0',200);
								 sprintf(log,"[EnableDenomDuringCreditPolling()] Response[%d]= 0x%x h",i,Response[i]);
								 AVRM_writeFileLogV2(log,INFO,g_BNAId);
							}
							#endif
							
							//Log Tx Data
							//++LogDataExchnage( DATA_EXCHANGE_ALL,g_BNAId, "Tx",JCM_Cmd,JCM_NumberofBytestobeWrite);

						   //if(   ( ( 0x50 == Response[2] ) && ( 0x03 == Response[3] ) )  ||
						   //		 ( ( 0x50 == Response[2] ) && ( 0x13 == Response[3] ) )  ||
						   //		 ( ( 0x50 == Response[2] ) && ( 0x05 == Response[3] ) )  ||
						   //		 ( ( 0x50 == Response[2] ) && ( 0x08 == Response[3] ) ) 
						   //	  )
						   //	{
						   //		 AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Enable Denom Success .",INFO,g_BNAId); 
						   //		 return 1;
							
							//}
							//else
							//{
								//  AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Enable Denom not possible due to no matching with any denomination.",INFO,ERROR); 
								  //return 0;
							//}
							
							
							//AVRM_writeFileLogV2("[EnableDenomDuringCreditPolling()] Enable Denom Success .",INFO,g_BNAId); 
							return 1;

				    }
				    else if( JCM_INTERNAL_COMMAND_REPLY_READ == flag)
				    {
					    //writeFileLog"[EnableDenomDuringCreditPolling()] Enable Denom recv not read due to flag not set it will be read any other recv function ."); 
						return 1;
				    } //else if end               
					

}//++int EnableDenomDuringCreditPolling(int fare,int flag) end

int EnableDenom(NOTE UserdefEnablebyte) {
                
                int rtcode=-1;

                char log[200];

                memset(log,'\0',200);

                int i=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                unsigned char DefaultEnablebyte=0x00;
                
                unsigned char DefaultEnablebyte2=0x00;

                if( JCM_ENABLE == UserdefEnablebyte.INR_5 ){

                     DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
                     AVRM_writeFileLogV2("[ EnableDenom() ] Rs.5  is enable.",INFO,g_BNAId);
                
                }//if end

                if( JCM_ENABLE == UserdefEnablebyte.INR_10 ) {

		             DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
                     AVRM_writeFileLogV2("[ EnableDenom() ] Rs.10  is enable.",INFO,g_BNAId);

                }//if end

                if( JCM_ENABLE == UserdefEnablebyte.INR_20 ) {

		             DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
                     AVRM_writeFileLogV2("[ EnableDenom() ] Rs.20  is enable.",INFO,g_BNAId);

	            }//if end

                if( JCM_ENABLE == UserdefEnablebyte.INR_50 ) {

                     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
                     AVRM_writeFileLogV2("[ EnableDenom() ] Rs.50  is enable.",INFO,g_BNAId);

	            }//if end

                if( 1 == UserdefEnablebyte.INR_100 ) {

		            DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
                    AVRM_writeFileLogV2("[ EnableDenom() ] Rs.100  is enable",INFO,g_BNAId);

	            }//if end

                if( JCM_ENABLE == UserdefEnablebyte.INR_200 ){

		            DefaultEnablebyte2 = DefaultEnablebyte2 | 0x01; //0b00010000;
                    AVRM_writeFileLogV2("[ EnableDenom() ] Rs.200  is enable",INFO,g_BNAId);

	            }//if end
	            
                if( JCM_ENABLE == UserdefEnablebyte.INR_500 ) {

		            DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
                    AVRM_writeFileLogV2("[ EnableDenom() ] Rs.500  is enable.",INFO,g_BNAId);

	            }//if end
	            
                if( JCM_ENABLE == UserdefEnablebyte.INR_1000 ){

		            //++DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
                    AVRM_writeFileLogV2("[ EnableDenom() ] Rs.1000  is enable.",INFO,g_BNAId);

                }//if end
                
                if( JCM_ENABLE == UserdefEnablebyte.INR_2000 ) {

		            DefaultEnablebyte = DefaultEnablebyte | 0x80; //0b10000000;
                    AVRM_writeFileLogV2("[ EnableDenom() ] Rs.2000  is enable.",INFO,g_BNAId);

	            }//if end
	            
                AVRM_writeFileLogV2("[ EnableDenom() ] Going To issue insertion auth command.",INFO,g_BNAId);
                
                memset(log,'\0',200);
                sprintf(log,"[ EnableDenom() ] DefaultEnablebyte = 0x%x h",DefaultEnablebyte);
                AVRM_writeFileLogV2(log,INFO,g_BNAId);
                
                memset(log,'\0',200);
                sprintf(log,"[ EnableDenom() ] DefaultEnablebyte2 = 0x%x h",DefaultEnablebyte2);
                AVRM_writeFileLogV2(log,INFO,g_BNAId);

                //++Now issue insertion authorised command to jcm
                JCM_delay_miliseconds(200);
                rtcode = JCM_Cmd_InsertionAuthorizedV2( DefaultEnablebyte ,DefaultEnablebyte2 ,Response,JCM_INSERTION_AUTHORIZING_REPLY_SIZE,JCM_RECV_DELAY);               
                
                #ifdef JCM_DEBUG 
                
                //Display Enable Denom Packet
     
                //for(i=0;i<JCM_INSERTION_AUTHORIZING_REPLY_SIZE;i++){
                     //memset(log,'\0',200);
                     //sprintf(log,"[ EnableDenom() ] Response[%d]= 0x%x h",i,Response[i]);
	                 //writeFileLoglog);
	            //}
                
                #endif
                
                if( Response[2]> 0 ) {
                    //++LogDataExchnage( "Rx",Response,JCM_INSERTION_AUTHORIZING_REPLY_SIZE);
                }//if end

                if(  ( ( 0x50 == Response[2] ) && ( 0x03 == Response[3] ) )  ||( ( 0x50 == Response[2] ) && ( 0x13 == Response[3] ) )){
                     AVRM_writeFileLogV2("[ EnableDenom() ] Enable Denom Success .",INFO,g_BNAId);
                     return 1;
                
                }else{
                     AVRM_writeFileLogV2("[ EnableDenom() ] Enable Denom failed .",INFO,g_BNAId);
                     return (-1);

                }//else end


}//int EnableDenom(NOTE UserdefEnablebyte) end

int EnableDenomV2(noteType UserdefEnablebyte) {
                

                int rtcode=-1;

                char log[200];

                memset(log,'\0',200);

                int i=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                unsigned char DefaultEnablebyte=0x00;
                
                unsigned char DefaultEnablebyte2=0x00;

                if( 1 == UserdefEnablebyte.rs_5 ){

                     DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
                     //writeFileLog"[ EnableDenom() ] Rs.5  is enable.");
                
                }//if end

                if( 1 == UserdefEnablebyte.rs_10 ) {

		             DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
                     //writeFileLog"[ EnableDenomV2() ] Rs.10  is enable.");

                }//if end

                if( 1 == UserdefEnablebyte.rs_20 ) {

		             DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
                     //writeFileLog"[ EnableDenomV2() ] Rs.20  is enable.");

	            }//if end

                if( 1 == UserdefEnablebyte.rs_50 ) {

                     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
                     //writeFileLog"[ EnableDenomV2() ] Rs.50  is enable.");

	            }//if end

                if( 1 == UserdefEnablebyte.rs_100 ) {

		            DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
                    //writeFileLog"[ EnableDenomV2() ] Rs.100  is enable");

	            }//if end

                if( 1 == UserdefEnablebyte.rs_200 ){

		            DefaultEnablebyte2 = DefaultEnablebyte2 | 0x01; //0b00010000;
                    //writeFileLog"[ EnableDenomV2() ] Rs.200  is enable");

	            }//if end
	            
                if( 1 == UserdefEnablebyte.rs_500 ) {

		            DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
                    //writeFileLog"[ EnableDenomV2() ] Rs.500  is enable.");

	            }//if end
	            
                if( 1 == UserdefEnablebyte.rs_1000 ){

		            DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
                    //writeFileLog"[ EnableDenomV2() ] Rs.1000  is enable.");

                }//if end
                
                if( 1 == UserdefEnablebyte.rs_2000 ) {

		            DefaultEnablebyte = DefaultEnablebyte | 0x80; //0b10000000;
                    //writeFileLog"[ EnableDenomV2() ] Rs.2000  is enable.");

	            }//if end
	            
                //writeFileLog"[ EnableDenomV2() ] Going To issue insertion auth command.");
                memset(log,'\0',200);
                sprintf(log,"[ EnableDenomV2() ] DefaultEnablebyte = 0x%x h",DefaultEnablebyte);
                //writeFileLoglog);
                
                memset(log,'\0',200);
                sprintf(log,"[ EnableDenomV2() ] DefaultEnablebyte2 = 0x%x h",DefaultEnablebyte2);
                //writeFileLoglog);

                //++Now issue insertion authorised command to jcm
                rtcode = JCM_Cmd_InsertionAuthorizedV2( DefaultEnablebyte ,DefaultEnablebyte2 ,Response,JCM_INSERTION_AUTHORIZING_REPLY_SIZE,JCM_RECV_DELAY);               
                
                #ifdef JCM_DEBUG 
                
                //Display Enable Denom Packet
     
                //for(i=0;i<JCM_INSERTION_AUTHORIZING_REPLY_SIZE;i++)
                //{

                   
                     //memset(log,'\0',200);
                     //sprintf(log,"[ EnableDenomV2() ] Response[%d]= 0x%x h",i,Response[i]);
	             ////writeFileLoglog);


                //}
                
                #endif
                

                if( Response[2]> 0 ) {
                    //++LogDataExchnage( "Rx",Response,JCM_INSERTION_AUTHORIZING_REPLY_SIZE);
                }


                if(  ( ( 0x50 == Response[2] ) && ( 0x03 == Response[3] ) )  ||( ( 0x50 == Response[2] ) && ( 0x13 == Response[3] ) )){
                     //writeFileLog"[ EnableDenomV2() ] Enable Denom Success ."); 
                     return 1;
                
                }else{
                      //writeFileLog"[ EnableDenomV2() ] Enable Denom failed ."); 
                      return (-1);

                }//else end

}//int EnableDenomV2(noteType UserdefEnablebyte) end

int JCM_Cmd_InsertionAuthorizedV2( unsigned char DefaultEnablebyte,unsigned char DefaultEnablebyte2,unsigned char *Response,const int ResponseLength,const unsigned int Delay) { 

                    int rtcode=-1,Counter=0;

		    int CmdLength=0;
        
                    unsigned char log[100];

                    memset(log,'\0',100);

		    unsigned int NumberofBytestobeRead=ResponseLength;

                    unsigned char Commands[ JCM_CMD_INSERTION_AUTH_LENGTH ]=JCM_INSERTION_AUTHORISED_CMD;

		    unsigned char DittoCommands[ JCM_CMD_DITTO_INSERTION_AUTH_LENGTH ]=JCM_INSERTION_AUTHORISED_DITTO_CMD;
                
                    #ifdef JCM_DEBUG
		    memset(log,'\0',100);

                    sprintf(log,"[JCM_Cmd_InsertionAuthorizedV2()] DefaultEnablebyte =0x%xh.",DefaultEnablebyte);

                    //writeFileLoglog);
                    #endif
	
		    memset(Response,'\0',ResponseLength);
		    

		    if(0x10 == DefaultEnablebyte)
		    {
			    
				    //Set MoneyCode byte
		                    DittoCommands[ 3 ]=DefaultEnablebyte;
                                    DittoCommands[ 5 ]=DefaultEnablebyte2;

				    CmdLength=JCM_CMD_DITTO_INSERTION_AUTH_LENGTH;

				    //Prepare BCC 
				    for(Counter=2;Counter<= (CmdLength-2);Counter++)
				    {
					 if(4 == Counter)
					 continue;
					 else
					 DittoCommands[CmdLength-1]=  DittoCommands[CmdLength-1] ^ DittoCommands[Counter];
				    }
                                    #ifdef JCM_DEBUG 
                                    for(Counter=0;Counter<CmdLength;Counter++)
				    {
					 memset(log,'\0',100);

                                         sprintf(log,"[JCM_Cmd_InsertionAuthorizedV2()] DittoCommands[%d] = 0x%xh.",Counter,DittoCommands[Counter]);
                                         //writeFileLoglog);
				    }

                                    memset(log,'\0',100);

                                    sprintf(log,"[JCM_Cmd_InsertionAuthorizedV2()] DittoComamnd Packet BCC =0x%xh.",DittoCommands[CmdLength-1]);
                                    //writeFileLoglog);
				
				    #endif
			    
		    }

		    else if(0x10!= DefaultEnablebyte)
		    {
                     
				    //Set MoneyCode byte
		                    Commands[ 3 ]=DefaultEnablebyte;
                                    Commands[ 4 ]=DefaultEnablebyte2;
				    
                                    CmdLength=JCM_CMD_INSERTION_AUTH_LENGTH;

				    //Prepare BCC 
				    for(Counter=2;Counter<= (CmdLength-2);Counter++)
				    {
					 
					 Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
				    
				    }
                                    #ifdef JCM_DEBUG
				    for(Counter=0;Counter<CmdLength;Counter++)
				    {
					  
                                          memset(log,'\0',100);

                                          sprintf(log,"[JCM_Cmd_InsertionAuthorizedV2()] Commands[%d] = 0x%xh.",Counter,Commands[Counter]);
                                          //writeFileLoglog);

				    }

				
                                    memset(log,'\0',100);

                                    sprintf(log,"[JCM_Cmd_InsertionAuthorizedV2()] Comamnd Packet BCC =0x%xh.",Commands[CmdLength-1]);

                                    //writeFileLoglog);
				    #endif
		    
		    }

                    if(0x10== DefaultEnablebyte)
                    {
                           #ifdef JCM_DEBUG
			   //writeFileLog"[JCM_Cmd_InsertionAuthorizedV2()] Enablebyte 0x10 found.");
                           #endif
                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,DittoCommands,CmdLength);

	            }
	            else if(0x10 != DefaultEnablebyte)
	            {
                            #ifdef JCM_DEBUG
			    //writeFileLog"[JCM_Cmd_InsertionAuthorizedV2()] Enablebyte 0x10 not found.");
                            #endif
			    rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);

		    }

		    if(1==rtcode)
		    {
           
				  JCM_delay_miliseconds(Delay); 
				  rtcode=-1;
				  rtcode=JCM_ReceieveCmdReplySequence(g_HANDLE,Response,NumberofBytestobeRead);
				  if(1==rtcode)
				  {
					  return 1; //success
				  }
				  else
				  {
					  return 3; //recv sequence failed
                          
				  }
 
			}
			else
			{
				return 2; //transmit sequence failed
			} 


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  int g_DenomMaskValue=0x00;
static  pthread_mutex_t g_DenomMaskValuemutex=PTHREAD_MUTEX_INITIALIZER;

void SetDenomMaskValue(int DenomMaskValue){

        pthread_mutex_lock(&g_DenomMaskValuemutex);
        
        //++g_DenomMaskValue = 0x00;
        g_DenomMaskValue = DenomMaskValue;
        
        pthread_mutex_unlock(&g_DenomMaskValuemutex); 

}//void SetDenomMaskValue() end

/*
int EnableDenom(NOTE UserdefEnablebyte) {
                

                int rtcode=-1;

                char log[200];

                memset(log,'\0',200);

                int i=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                unsigned char DefaultEnablebyte=0x00;

                if( JCM_ENABLE == UserdefEnablebyte.INR_5 ){

                     DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
                     //writeFileLog"[ EnableDenom() ] Rs.5  is enable.");
                
                }

                if( JCM_ENABLE == UserdefEnablebyte.INR_10 ){

		             DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
                     //writeFileLog"[ EnableDenom() ] Rs.10  is enable.");

                }

                if( JCM_ENABLE == UserdefEnablebyte.INR_20 ){

		             DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
                     //writeFileLog"[ EnableDenom() ] Rs.20  is enable.");

	            }

                if( JCM_ENABLE == UserdefEnablebyte.INR_50 ){

                     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
                     //writeFileLog"[ EnableDenom() ] Rs.50  is enable.");

	            }

                if( JCM_ENABLE == UserdefEnablebyte.INR_100 ){

		            DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
                    //writeFileLog"[ EnableDenom() ] Rs.100  is enable");

	            }

                if( JCM_ENABLE == UserdefEnablebyte.INR_500 ){

		            DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
                    //writeFileLog"[ EnableDenom() ] Rs.500  is enable.");

	            }

                if( JCM_ENABLE == UserdefEnablebyte.INR_1000 ){

		            DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
                    //writeFileLog"[ EnableDenom() ] Rs.1000  is enable.");

                }

                
                //writeFileLog"[ EnableDenom() ] Going To issue insertion auth command.");
                memset(log,'\0',200);
                sprintf(log,"[ EnableDenom() ] DefaultEnablebyte = 0x%x h",DefaultEnablebyte);
                //writeFileLoglog);


                //Now issue insertion authorised command to jcm
                rtcode = JCM_Cmd_InsertionAuthorized( DefaultEnablebyte,
                                                      Response,
                                                      JCM_INSERTION_AUTHORIZING_REPLY_SIZE,
                                                      JCM_RECV_DELAY
                                                   );
                 
                
                #ifdef JCM_DEBUG 

                //Display Enable Denom Packet
     
                for(i=0;i<JCM_INSERTION_AUTHORIZING_REPLY_SIZE;i++){
                     memset(log,'\0',200);
                     sprintf(log,"[ EnableDenom() ] Response[%d]= 0x%x h",i,Response[i]);
	                 //writeFileLoglog);

                }
                
                #endif
                



                if(  ( ( 0x50 == Response[2] ) && ( 0x03 == Response[3] ) )  ||
                     ( ( 0x50 == Response[2] ) && ( 0x13 == Response[3] ) )
                  ){
                     //writeFileLog"[ EnableDenom() ] Enable Denom Success ."); 
                     return 1;
                
                }else{
                     //writeFileLog"[ EnableDenom() ] Enable Denom failed ."); 
                     return (-1);

                }

}//EnableDenom() end
*/

int DisableSpecificNotes(int fare){

                         char log[100];
                         memset(log,'\0',100);
                         int GlobalDenomMaskValue = 0;
                         
                         pthread_mutex_lock(&g_DenomMaskValuemutex);
                         GlobalDenomMaskValue = g_DenomMaskValue ;
                         pthread_mutex_unlock(&g_DenomMaskValuemutex); 

                         memset(log,'\0',100);
                         sprintf(log,"[DisableSpecificNotes(()] DenomMask Value: %d",GlobalDenomMaskValue); 
                         //writeFileLoglog);
 

			             int rtcode=-1;
					     NOTE UserDefinedNote={0,0,0,0,0,0,0};

			             if(fare>=5)
                         {
                             if( 0x01 == (GlobalDenomMaskValue&0x01) )
                             {
			                       UserDefinedNote.INR_5    = 1;
                                   //writeFileLog"[DisableSpecificNotes()] Rs.5 Enabled."); 
                             }
                         }

			             if(fare>=10)
                         {
                            if( 0x02 == (GlobalDenomMaskValue&0x02) )
                            {
			                   UserDefinedNote.INR_10   = 1;
                               //writeFileLog"[DisableSpecificNotes()] Rs.10 Enabled."); 
                            }
                         }

			             if(fare>=20)
                         {
                            if( 0x04 == (GlobalDenomMaskValue&0x04) )
			                { 
                                UserDefinedNote.INR_20   = 1;
                                //writeFileLog"[DisableSpecificNotes()] Rs.20 Enabled."); 
                            }
                         }
			 
                         if(fare>=50)
                         {
                            if( 0x08 == (GlobalDenomMaskValue&0x08) )
                            {
			                    UserDefinedNote.INR_50   = 1;
                                //writeFileLog"[DisableSpecificNotes()] Rs.50 Enabled."); 
                            }
                         }

			             if(fare>=100)
                         {
                            if( 0x10 == (GlobalDenomMaskValue&0x10) )
                            {
			                    UserDefinedNote.INR_100  = 1;
                                //writeFileLog"[DisableSpecificNotes()] Rs.100 Enabled."); 
                            }
                         }

			             if(fare>=500)
                         {
                            if( 0x20 == (GlobalDenomMaskValue&0x20) )
                            {
			                   UserDefinedNote.INR_500  = 1;
                               //writeFileLog"[DisableSpecificNotes()] Rs.500 Enabled."); 
                            }
                         }

			             if(fare>=1000) 
                         {
                            if( 0x40 == (GlobalDenomMaskValue&0x40) )
                            {
			                     UserDefinedNote.INR_1000 = 1;
                                 //writeFileLog"[DisableSpecificNotes()] Rs.1000 Enabled."); 
                            }
                         }

			            //equal fare
						if( 0 == fare)
					    {
						      //writeFileLog"[DisableSpecificNotes()] Zero fare receieved.");
												   
					    }
							   
					    //++issue command
					    rtcode=EnableDenom(UserDefinedNote); 

						if( 1 == rtcode){
								
						     //writeFileLog"[DisableSpecificNotes()] Specific Notes Disable success."); 
							 return 1;
						}else {
							 //writeFileLog"[DisableSpecificNotes()] Specific Notes Disable failed."); 
							 return 0;
                        }

	       
}//int DisableSpecificNotes(int fare) end

int EnableNoteAcceptor(noteType fnNOTE) {

                        //++issue command
                        int  rtcode =-1;
                        NOTE UserDefinedNote={0,0,0,0,0,0,0,0,0};
                        if( ENABLE == fnNOTE.rs_5 ){
			                 UserDefinedNote.INR_5    = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.5 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_10 ){
			                 UserDefinedNote.INR_10    = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.10 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_20 ){
			                 UserDefinedNote.INR_20    = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.20 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_50 ){
			                 UserDefinedNote.INR_50    = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.50 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_100 ){
			                 UserDefinedNote.INR_100   = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.100 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_200 ){
			                 UserDefinedNote.INR_200   = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.200 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_500 ){
			                 UserDefinedNote.INR_500  = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.500 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_1000 ){
			                 UserDefinedNote.INR_1000    = 1; 
                             //writeFileLog"[EnableNoteAcceptor()] Rs.1000 Enabled."); 
                        }
                        
                        if( ENABLE == fnNOTE.rs_2000 ){
			                UserDefinedNote.INR_2000  = 1; 
                            //writeFileLog"[EnableNoteAcceptor()] Rs.2000 Enabled."); 
                        }
					    
					    rtcode=EnableDenom(UserDefinedNote); 
                        if( 1 == rtcode){
							 //writeFileLog"[EnableNoteAcceptor()] JCM Notes Enable success."); 
							 return 1;
						}else {
							 //writeFileLog"[EnableNoteAcceptor()] JCM Notes Enable failed."); 
							 return 0;
                        }//else end

}//int EnableNoteAcceptor(noteType NOTE) end

int* GetEscrowMoneyPatchV3(unsigned char *Response,int EscrowIndex){
         
         int Counter=0,DoubleDle=0,DoubleDleIndex=0,MoneyCounter=0;
         unsigned char Money[7];
         memset(Money,'\0',7);
         bool errordetect=false;
         unsigned char log[100];
         int TotalMoney=0;
         int *arr = (char *)malloc( 9* sizeof(int));
         for(Counter=0;Counter< 9  ;Counter++){
		    arr[Counter]=0x00;
	     }//for end

           //EscrowIndex   = Rs 5
           //EscrowIndex+1 = Rs 10
           //EscrowIndex+2 = Rs 20
           //EscrowIndex+3 = Rs 50
           //EscrowIndex+4 = Rs 100
           //EscrowIndex+5 = Rs 500
           //EscrowIndex+6 = Rs 1000


           //user defined Money[] index
           //Index 0 = Rs 5
           //Index 1 = Rs 10
           //Index 2 = Rs 20
           //Index 3 = Rs 50
           //Index 4 = Rs 100
           //Index 5 = Rs 200
           //Index 6 = Rs 500
           //Index 7 = Rs 1000
           //Index 8 = Rs 2000
    
         //Detect Error
         for(Counter=EscrowIndex;Counter<= (EscrowIndex+6) ;Counter++){
             if( 0x10 == Response[Counter] ){
                   DoubleDle++;
                   if(DoubleDle>=2)
                   {
                        errordetect=true;
                        DoubleDleIndex=Counter;
                        memset(log,'\0',100);
                        sprintf(log,"[GetEscrowMoneyPatchV3()] Error Double Byte Recvd. Index Value = %d",DoubleDleIndex);
                        //writeFileLoglog);

                   }//if end
             }//if end
          
         }//for end

         if( false == errordetect ){
                 
                arr[0] = (int)Response[EscrowIndex];
                arr[1] = (int)Response[EscrowIndex+1];
                arr[2] = (int)Response[EscrowIndex+2];
                arr[3] = (int)Response[EscrowIndex+3];
                arr[4] = (int)Response[EscrowIndex+4];
                arr[5] = (int)Response[EscrowIndex+5];
                arr[6] = (int)Response[EscrowIndex+6];
                arr[7] = (int)Response[EscrowIndex+7];
                arr[8] = (int)Response[EscrowIndex+8];
                
                //++DisplayEscrowTableV2(Response,EscrowIndex);
                return arr;
          
         }else if( true == errordetect ){

                for(Counter=EscrowIndex;Counter<= (EscrowIndex+8) ;Counter++){
                       if(DoubleDleIndex!=Counter)
                       {
                             Money[MoneyCounter]=Response[Counter];
                             MoneyCounter++;
                       }//if end

                }//for end
                 
                arr[0] = (int)Money[0]; 
                arr[1] = (int)Money[1];
                arr[2] = (int)Money[2];
                arr[3] = (int)Money[3];
                arr[4] = (int)Money[4];
                arr[5] = (int)Money[5];
                arr[6] = (int)Money[6];
                arr[7] = (int)Money[7];
                arr[8] = (int)Money[8];
                
                //++DisplayEscrowTableV2(Money,0);
                //++TotalMoney = Money[0]+Money[1]+Money[2]+Money[3]+Money[4]+Money[5]+Money[6];
                return arr;

         } else{
			 return NULL; 
		 }


}//GetEscrowMoneyPatchV3() end

int* GetNoteDetailsInRecycleCstV1() {

           
                if( -1 != g_HANDLE ) {

	                                      unsigned char Wait_State[4]={0x02,0x03,0x12,0x13};

                                          unsigned int  Wait_State_Length=4; 

                                          unsigned char StateFound=0x00;

                                          /*

                                          //Wait for 5 Second if any enq byte recv from jcm
                                          if(  1 == ReadEnqFromJcm() )
                                          {

                                                //writeFileLog"[GetNoteDetailsInRecycleCst()] Enq Byte receieved from jcm.");

                                                //writeFileLog"[GetNoteDetailsInRecycleCst()] Before go for wait jcm idle session.");

                                                WaitForState( 5,Wait_State,Wait_State_Length, &StateFound);

                                                //writeFileLog"[GetNoteDetailsInRecycleCst()] After go for wait jcm idle session.");


                                          }

                                          */
        
                                          int rtcode=-1;

                                          unsigned char Response[100];
                                          memset(Response,'\0',100);

                                          unsigned char log[200];
                                          memset(log,'\0',200);

	                                      //issue sense command
		                                  rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

		                          
		                                 if( ( Response[3] > 0x00 ) && (0x40 == Response[2]) ) {
					                 
                                                 //++#ifdef JCM_DEBUG 

				                                 int i=0;

		                                         for(i=0;i<JCM_SENSE_REPLY_SIZE;i++) {
						
		                                               //memset(log,'\0',200);
                                                       //sprintf(log,"[GetNoteDetailsInRecycleCstV1()] Sense Response[%d] = 0x%x h. ",i,Response[i]);
		                                               //++//writeFileLoglog);
	 
		                                         }
                                                        
                                                 //++#endif

                                                 //++Log Current State 
                                                 memset(log,'\0',200);
                                                 sprintf(log,"[GetNoteDetailsInRecycleCstV1()] Current State = 0x%xh. ",Response[3]);
		                                         //writeFileLoglog);
                                    
                                                 //Log Current Sensor State
                                                 //++JCM_LogSensorState(Response[7],Response[8],Response[9]);

                                                 //calculate escrow notes
                                                                 
				                                 int* EscrowNotesArray=NULL;

                                                 EscrowNotesArray=GetEscrowMoneyPatchV3(Response,10);

							                     return EscrowNotesArray;

                                                         
							    	                     
										} else {
													
																			  
							                     //writeFileLog"[GetNoteDetailsInRecycleCstV1()] JCM is not reply against sense command.");
											     int i=0;
                                                 for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)  {
													 memset(log,'\0',200);
													 sprintf(log,"[GetNoteDetailsInRecycleCstV1()] Sense Response[%d] = 0x%x h. ",i,Response[i]);
													 //writeFileLoglog);
								 
												 }

												 return NULL;
                                       }


			   }else {
					//writeFileLog"[GetNoteDetailsInRecycleCstV1()] JCM port open not done.");
					return NULL;

			   }
           
}//GetNoteDetailsInRecycleCstV1() end

int JCMGetEscrowNotesNumbers( int* RS5NoteQuanity,int* RS10NoteQuanity,int* RS20NoteQuanity,int* RS50NoteQuanity ,int* RS100NoteQuanity ,int* RS200NoteQuanity ,int* RS500NoteQuanity, int* RS1000NoteQuanity,int* RS2000NoteQuanity,const unsigned int Seconds){
                                   
                                   

											     unsigned char rcvPkt[30];
											     int rcvPktLen=0,i=0,counter=1,totalnotes=0,StartNoteIndex=4;
											     unsigned char log[100];
											     memset(rcvPkt,'\0',30);
											     memset(log,'\0',100);
											     int* Money=NULL;

											     *RS5NoteQuanity=0;
											     *RS10NoteQuanity=0;
											     *RS20NoteQuanity=0;
											     *RS50NoteQuanity=0;
											     *RS100NoteQuanity=0;
											     *RS200NoteQuanity=0;
											     *RS500NoteQuanity=0;
											     *RS1000NoteQuanity=0;
											     *RS2000NoteQuanity=0;
											     
											      //++Disable JCM Note Acceptor
											      IssuedisableAllBillCmd();
                                   
                                                  //++Now Get Escorw notes number
                                                  Money =(int*) GetNoteDetailsInRecycleCstV1();
                                                  
                                                  if( NULL == Money ){
													  memset(log,'\0',100);
                                                      sprintf(log,"[JCMGetEscrowNotesNumbers()] Unable to get Escow Notes");
		                                              //writeFileLoglog);
													  return 1; //++Failed
                                                  }//if end
                                                  
                                                  totalnotes = Money[0]+Money[1]+Money[2]+Money[3]+Money[4]+Money[5]+Money[6]+Money[7]+Money[8];
                                                   
                                                  memset(log,'\0',100);

                                                  sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Notes = %d .",totalnotes);

		                                          //writeFileLoglog);
		                                           
		                                      
                                                  if( totalnotes >= 1 ){
													  
                                                      for(counter=1;counter<=totalnotes;counter++){
														  
                                                           switch(rcvPkt[StartNoteIndex]){
															   
                                                                case 0: *RS5NoteQuanity=
*RS5NoteQuanity+1;
                                                                        break;

                                                                case 1: *RS10NoteQuanity = *RS10NoteQuanity+1;
                                                                         break;

                                                                case 2: *RS20NoteQuanity= *RS20NoteQuanity+1;
                                                                         break;

                                                                case 3:  *RS50NoteQuanity=
*RS50NoteQuanity+1;
                                                                         break;

                                                                case 4: *RS100NoteQuanity=
*RS100NoteQuanity+1;
                                                                         break;
                                                                         
                                                                case 5: *RS200NoteQuanity=
*RS200NoteQuanity+1;
                                                                         break;

                                                                case 6: *RS500NoteQuanity=
*RS500NoteQuanity+1;
                                                                         break;

                                                                case 7: *RS1000NoteQuanity=
*RS1000NoteQuanity+1;
                                                                        break;
                                                                        
                                                                case 8: *RS2000NoteQuanity=
*RS2000NoteQuanity+1;
                                                                        break;
                                                                        
                                                                default:
                                                                        break;
                                                           };
 
                                                           StartNoteIndex++;

                                                      }//for loop end

                                                   }//if block end

                                                   ///////////////////////////////////////////////////////////////////////////////
                                                   

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 5 Notes = %d .",*RS5NoteQuanity);
		                                           //writeFileLoglog);

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 10 Notes = %d .",*RS10NoteQuanity);
		                                           //writeFileLoglog);
 

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 20 Notes = %d .",*RS20NoteQuanity);
		                                           //writeFileLoglog);

                                                   
                                                   memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 50 Notes = %d .",*RS50NoteQuanity);
		                                           //writeFileLoglog);

                                                   
                                                   memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 100 Notes = %d .",*RS100NoteQuanity);
		                                           //writeFileLoglog);
		                                           
		                                           memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 200 Notes = %d .",*RS200NoteQuanity);
		                                           //writeFileLoglog);

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 500 Notes = %d .",*RS500NoteQuanity);
		                                           //writeFileLoglog);

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 1000 Notes = %d .",*RS1000NoteQuanity);
		                                           //writeFileLoglog);
		                                           
		                                           memset(log,'\0',100);
                                                   sprintf(log,"[JCMGetEscrowNotesNumbers()] Total Rs 2000 Notes = %d .",*RS2000NoteQuanity);
		                                           //writeFileLoglog);
                                                   
                                                   free(Money);
                                                   
                                                   return 0; //++Success
                                                     
									  

}//int JCMGetEscrowNotesNumbers( int* const NoteQuanity,const unsigned int Seconds) end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//wait for note acceptor low level poll thread closed status
unsigned int WaitforNoteAcptrLowlevelflagClosed( double waittime ) {

	 
	 time_t start,end,diff;
	 int rtcode=-1;
         unsigned char log[100];
         memset(log,'\0',100); 
	 
         //#ifdef JCM_DEBUG
         //writeFileLog"[WaitforNoteAcptrLowlevelflagClosed()]  Get Low level flag status starting..");
         //#endif
	 
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

	 //if polling flag is on condition ( on=1 )
	 if(1==g_NoteLowLevelPollFlagStatus)
	 {
	  
                   pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

		   time(&start);  
		 
		   //Wait here untill polling flag is off ( off=0 )
		   for(;;)
		   {

			     time(&end); 
			     diff=end-start;
			     
			     //if timer is running
			     if(diff<waittime)
			     {
                 
                                        pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

					if( 0==g_NoteLowLevelPollFlagStatus ) 
					{

                                             pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

					     //#ifdef JCM_DEBUG

                                             memset(log,'\0',100); 

                                             sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Already Low level flag status is closed in %d seconds.",diff);
					                         
					                         AVRM_writeFileLogV2(log,INFO,g_BNAId);

                                             //#endif

                                             return 1;

					}
					else
                                        {
					      pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
 
                                        }


			     }

			     //if timer time is end or timer is running
			     if(diff>=waittime)
			     {

                                        pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

					if( 0==g_NoteLowLevelPollFlagStatus)
					{
					  //#ifdef JCM_DEBUG
                                          memset(log,'\0',100);
                                          sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Low level flag status is closed %d seconds.",diff);
                                          AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                          //#endif	
				  
                                          rtcode=1;
					}
					else
					{ 
					   //#ifdef JCM_DEBUG

                                           memset(log,'\0',100);

                                           sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Low level flag status is not closed during time delay wait. %d .",diff);
                                           
                                           AVRM_writeFileLogV2(log,INFO,g_BNAId);

                                           //#endif

					   rtcode=0;

					}

                                        pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

					return rtcode;


			     }//timer if block end here
			      

		   }//for loop end
	   
	 }//if thread status start end here

	 pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

         //////////////////////////////////////////////////////////////////////////////////////////////////////

         pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

	 if(0==g_NoteLowLevelPollFlagStatus)
	 {
              pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

	      //#ifdef JCM_DEBUG
              AVRM_writeFileLogV2("[WaitforNoteAcptrLowlevelflagClosed()]  Already Low level flag status is closed.",INFO,g_BNAId);
              //#endif
	      return 1;

	 }

         pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

       ////////////////////////////////////////////////////////////////////////////////////////////////////////

         

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int  JCM_Atvm_CashInStart() {
                 
                                  
                                  int rtcode=-1;
                                  int i=0;
                                  char log[200];
                                  memset(log,'\0',200);
                                  
                                  //++clear global var for escrow notes
								  g_currentnote.AcceptedNumber_INR_5 =   0;
								  g_currentnote.AcceptedNumber_INR_10 =  0;
								  g_currentnote.AcceptedNumber_INR_20 =  0;
								  g_currentnote.AcceptedNumber_INR_50 =  0;
								  g_currentnote.AcceptedNumber_INR_100 = 0;
								  g_currentnote.AcceptedNumber_INR_200 = 0;
								  g_currentnote.AcceptedNumber_INR_500 = 0;
								  g_currentnote.AcceptedNumber_INR_1000= 0;
								  g_currentnote.AcceptedNumber_INR_2000= 0;
		                          
		                          //++JCMGetEscrowNotesNumbers( &RS5NoteQuanity,&RS10NoteQuanity,&RS20NoteQuanity,&RS50NoteQuanity ,&RS100NoteQuanity ,&RS200NoteQuanity ,&RS500NoteQuanity, &RS1000NoteQuanity,&RS2000NoteQuanity,2);
		                          
		                          //++Get Current Escrow Tables
                                  unsigned char Response[100];
				                  memset(Response,'\0',100);
	                              rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);
                               
                                  #ifdef JCM_DEBUG

                                  if( Response[3] > 0 ){
				                      for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   {
						                   memset(log,'\0',200);
                                           sprintf(log,"[JCM_Atvm_CashInStart()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
			                          }//for end
					    	                     
			                      }//if end

                                  #endif

                                  if( ( (0x40 == Response[2]) && (0x02 == Response[3]) ) || 
                                      ( (0x40 == Response[2]) && (0x03 == Response[3]) ) ||
                                      ( (0x40 == Response[2]) && (0x13 == Response[3]) ) ||
                                      ( (0x40 == Response[2]) && (0x12 == Response[3]) )
                                    )
								  {
										   AVRM_writeFileLogV2("[JCM_Atvm_CashInStart()] Get Escrow Table success.",INFO,g_BNAId);
										   GetEscrowMoneyPatch(&g_currentnote,Response,10);
									       return 1;
																							   
								  }
								  else
								  {
										  AVRM_writeFileLogV2("[JCM_Atvm_CashInStart()] Get Escrow Table failed.",INFO,g_BNAId);
										  return 0;
										  
								  }//else end

}//unsigned int  JCM_Atvm_CashInStart()  end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//API ENABLE =1 and DISABLE=0
int enableSpecificBillsAndMakeIdle(  int fare, int maxNoteQtyAllowed ){

	 
         //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Entry:",INFO);
		                
         //Disable all notes by default
		 NOTE notevar={0,0,0,0,0,0,0};

	 
         //Analysis if exact fare is given
         #ifdef JCM_DEBUG
         //writeFileLog"[enableSpecificBillsAndMakeIdle()] Before Exact fare analysis block.");
         #endif


         #ifdef JCM_DEBUG

         //Make a log about fare and max no of cash
         char log[200];
         memset(log,'\0',200);
         sprintf(log,"[enableSpecificBillsAndMakeIdle()] Max No of Cash = %d And fare = %d",maxNoteQtyAllowed,fare);       
         //AVRM_writeFileLogV2(log,INFO); 

         #endif

		 if(1 == maxNoteQtyAllowed)
		 {

                   g_ExactFareFlag=true;
                   //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Exact fare block Active [Cash:1].",INFO);
                   switch(fare)
				   {

						 case 5:
							  notevar.INR_5    = 1;
										  #ifdef JCM_DEBUG
										  //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 5 only enable.");
										  #endif
							  break;

						 case 10:
										  
							  notevar.INR_10   = 1;
										  #ifdef JCM_DEBUG
										  //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 10 only enable.");
										  #endif
							  break;

						 case 20:
							  notevar.INR_20  = 1;
										  #ifdef JCM_DEBUG
										  //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 20 only enable.");
										  #endif
							  break;

						 case 50:
							  notevar.INR_50   = 1;
										  #ifdef JCM_DEBUG
										  //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 50 only enable.");
										  #endif
							  break;

						 case 100:
							  notevar.INR_100  = 1;
										  #ifdef JCM_DEBUG
										  //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 100 only enable.");
										  #endif 
							  break;

						 case 500:
							  notevar.INR_500  = 1;
										  #ifdef JCM_DEBUG
										  //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 500 only enable.");
										  #endif
							  break;

						 case 1000:
							 notevar.INR_1000 = 1;
										 #ifdef JCM_DEBUG
										 //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 1000 only enable.");
										 #endif
							 break;

									 default: //writeFileLog"[enableSpecificBillsAndMakeIdle()] No fare given.");
											  break;

					   } 


		 }else if( maxNoteQtyAllowed > 1 ) {//Exact fare not given
           
                  
                  g_ExactFareFlag=false;
                  //AVRM_Currency_writeFileLog("[enableSpecificBillsAndMakeIdle()] Max Cash Block Active [Cash:20].",INFO);
                  if(fare >= 5)
                  {
                        
                        notevar.INR_5      = 1;

                        #ifdef JCM_DEBUG
                        //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 5 Enable.");
                        #endif
                  }

                  if(fare >= 10)
                  {
                        notevar.INR_10    = 1;

                        #ifdef JCM_DEBUG
                        //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 10 Enable.");
                        #endif
                  }

                  if(fare >= 20)
                  {
                       notevar.INR_20     = 1;

                       #ifdef JCM_DEBUG
                       //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 20 Enable.");
                       #endif
                  }

                  if(fare >= 50)
                  {
                      notevar.INR_50     = 1;

                      #ifdef JCM_DEBUG
                      //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 50 Enable.");
                      #endif
                  }

                  if(fare >= 100)
                  {
                      notevar.INR_100    = 1;

                      #ifdef JCM_DEBUG
                      //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 100 Enable.");
                      #endif

                  }

                  if(fare >= 500)
                  {
                      
                      notevar.INR_500    = 1;
                      #ifdef JCM_DEBUG
                      //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif

                  }
                  if(fare >= 1000)
                  {
                      
                      notevar.INR_1000   = 1;

                      #ifdef JCM_DEBUG
                      //writeFileLog"[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif

                  }

           }


           #ifdef JCM_DEBUG
		   //writeFileLog"[enableSpecificBillsAndMakeIdle()] After Exact fare analysis block.");
           #endif
        
           //Now Disable Notes
           #ifdef JCM_DEBUG
           //writeFileLog"[enableSpecificBillsAndMakeIdle()] Now going to enable note type.");
           #endif
           
           //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] After Exact fare analysis block.",INFO);
           //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Now going to enable note type.",INFO);
	  
           if( ( maxNoteQtyAllowed > 1 ) || ( 1 == maxNoteQtyAllowed  ) )
           {
                   //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] maxNoteQtyAllowed in between range 1-20 so going to enable jcm",INFO);

				   //store current jcm escrow table
				   //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Before Call JCM_Atvm_CashInStart",INFO);
				   JCM_Atvm_CashInStart();
				   //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] After Call JCM_Atvm_CashInStart",INFO);

				   //now going to enable jcm
				   int rtcode_enable = 0;
				   //++AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Before Call EnableDenom",INFO);
				   rtcode_enable = EnableDenom(notevar);
				   //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Before Call EnableDenom",INFO);
				   //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] enableSpecificBillsAndMakeIdle Exit",INFO);
				   return (  rtcode_enable );

           }
           else if( ( maxNoteQtyAllowed < 0 ) || ( 0 == maxNoteQtyAllowed  ) )
           {
				   //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] maxNoteQtyAllowed is not between range 1-20 so going to disable jcm",INFO);
                   int rtcode=-1;
                   rtcode=DisableAcceptance();
                   if( (1 == rtcode) || (2 == rtcode) ){
					 //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Enable Success Exit:",INFO);
                     return 1;
				   }
                   else{
					 //AVRM_writeFileLogV2("[enableSpecificBillsAndMakeIdle()] Enable Failed Exit:",INFO);
                     return (-1);
                    } 

           }//else if end


}

int SetCurrentfare(int Remainbalance) {

     ///////////////////////////////////////////////////////////

     pthread_mutex_lock(&g_FaretobeacceptMutex);

     g_faretobeaccept=0;
     
     g_faretobeaccept=Remainbalance;

     pthread_mutex_unlock(&g_FaretobeacceptMutex);
 
     //////////////////////////////////////////////////////////

     pthread_mutex_lock(&g_NASetCurrentFare);

     g_ThreadCurrentfare=0;

     g_ThreadCurrentfare=Remainbalance;

     pthread_mutex_unlock(&g_NASetCurrentFare);
 
     //////////////////////////////////////////////////////////

     return 0;



}// int SetCurrentfare(int Remainbalance) end

void SetSpecificNoteInhibitStatus(int fare){
	  

           pthread_mutex_lock(&g_NASetSpecificFaremutex);
 
           //writeFileLog"[SetSpecificNoteInhibitStatus()] Set External fare.");

           g_NACurrentAtvmFare=0;

	   g_SpecificNoteInhibitFlag=1;
	   
           g_NACurrentAtvmFare=fare;

           pthread_mutex_unlock(&g_NASetSpecificFaremutex);

           return;

	      
}// void SetSpecificNoteInhibitStatus(int fare) end

void SetCoinFullDisableFlag(int maxNoOfCash , int maxNoOfCoin){
     
          
          if(maxNoOfCash >= 1 && maxNoOfCash <= 20 ) //if note acceptor enable then going to set or unset of coin acceptor control
          {
		  if( 0 == maxNoOfCoin )//coin acceptor is diable due to maxnocoin is zero
		  {
		       g_CoinDisableflag=1; //set (user predefined control / out of control )
		       ////writeFileLog"[SetCoinFullDisableFlag()] Runtime Coin Control Disabled due to maxnoofcoin is zero.");
		  }
		  else if( maxNoOfCoin >= 1 && maxNoOfCoin <= 20 ) //coin acceptor is enable due to maxnocoin range is valid(1-20)
		  {
		       g_CoinDisableflag=0; //default (user runtime control in control )
		       ////writeFileLog"[SetCoinFullDisableFlag()] Runtime Coin Control Enabled.");
		  }

                  return;
          }
          else if( maxNoOfCash <= 0) //if note acceptor disable then no need to control coin acceptor
          {
                g_CoinDisableflag=1; //set (user predefined control / out of control )
		////writeFileLog"[SetCoinFullDisableFlag()] Runtime Coin Control Disabled due to maxnoofcash value less than 1.");
       
                return;
          }


}// void SetCoinFullDisableFlag(int maxNoOfCash , int maxNoOfCoin) end

int SetAlreadyCoinInhibit(){
   
     g_AlreadyCoinDisable=false;

     return 0;


}// int SetAlreadyCoinInhibit() end

int SetCurrent_JCM_State(int CurrentState){
   
   
   pthread_mutex_lock(&g_JCM_State_Mutex);

   g_JCM_Current_State=0x00;
 
   g_JCM_Current_State=CurrentState;

   pthread_mutex_unlock(&g_JCM_State_Mutex);

   return 0;
   


}// int SetCurrent_JCM_State(int CurrentState) end

unsigned int GetCurrent_JCM_State(){
   
   
	   int CurrentState=0x00;

	   unsigned char log[100];

	   pthread_mutex_lock(&g_JCM_State_Mutex);
	 
	   CurrentState=g_JCM_Current_State;

           #ifdef JCM_DEBUG

	   memset(log,'\0',100);

	   sprintf(log,"[GetCurrent_JCM_State()] Current State = 0x%x h.",g_JCM_Current_State);

	   //writeFileLoglog);

           #endif

	   g_JCM_Current_State=0x00;

	   pthread_mutex_unlock(&g_JCM_State_Mutex);

	   return CurrentState;



}// unsigned int GetCurrent_JCM_State() end

unsigned int GetCurrent_JCM_Accepted_Denom(){
       
	       
               int AcceptedDenom=0;

	       unsigned char log[100];
		     
	       pthread_mutex_lock(&g_JCM_Escrow_State_Mutex);

	       if( true == g_EscrowEvent)
	       {
		     
		     g_EscrowEvent=false;
		     AcceptedDenom=g_JCM_Accepted_Moneyvalue;
                     #ifdef JCM_DEBUG
		     memset(log,'\0',100);
		     sprintf(log,"[GetCurrent_JCM_Accepted_Denom()] AcceptedDenom =%d",AcceptedDenom);
		     //writeFileLoglog);
                     #endif

	       }

	       pthread_mutex_unlock(&g_JCM_Escrow_State_Mutex);
	     
	      
	      
	       return  AcceptedDenom;
	      
 
}// unsigned int GetCurrent_JCM_Accepted_Denom() end

//++API
int GetNoteDetailInEscrow() {


		  unsigned int Current_state=0x00;

		  unsigned int Current_Accepted_Denom=0x00;

	   
		  //make a log about to be accept fare
		  char log[200];
		  memset(log,'\0',200);

		  #ifdef JCM_DEBUG
		  pthread_mutex_lock(&g_FaretobeacceptMutex);
		  memset(log,'\0',200);
		  sprintf(log,"[GetNoteDetailInEscrow()] Fare to be accept Rs. : %d .", g_faretobeaccept);
		  //writeFileLoglog);
		  pthread_mutex_unlock(&g_FaretobeacceptMutex);
		  #endif
		  

                  /////////////////////////////////////////////////////////////////////////////////////////////

		  pthread_mutex_lock(&g_FaretobeacceptMutex);

		  if(  (5    == g_faretobeaccept)  ||
		       (10   == g_faretobeaccept)  ||
		       (20   == g_faretobeaccept)  ||
		       (50   == g_faretobeaccept)  ||
		       (100  == g_faretobeaccept)  ||
		       (200  == g_faretobeaccept)  ||
		       (500  == g_faretobeaccept)  ||
		       (1000 == g_faretobeaccept)  ||
		       (2000  == g_faretobeaccept)   
		    )
		 {
		        
		        g_SingleNoteAcceptEvent=true;
		        //#ifdef JCM_DEBUG
		        ////writeFileLog"[GetNoteDetailInEscrow()] Single Note Event Found.");  
		        //#endif
		 }
		 else
		 {
		        
		        g_SingleNoteAcceptEvent=false;
		        //#ifdef JCM_DEBUG
		        ////writeFileLog"[GetNoteDetailInEscrow()] Single Note Event Not Found."); 
		        //#endif

		 }
	       
		 pthread_mutex_unlock(&g_FaretobeacceptMutex);

                 /////////////////////////////////////////////////////////////////////////////////////////////

                    //Get State and Escrow Money
                    Current_state=GetCurrent_JCM_State();
         


         
                     /*
		                         10   = 5,10
		                         20   = 5,10,20
		                         50   = 5,10,20,50
		                         100  = 5,10,20,50,100
		                         500  = 5,10,20,50,100,500
		                         1000 = 5,10,20,50,100,500,1000

                     */

                        
                       //pthread_mutex_lock(&g_AlreadyCoinDisablemutex);

                       
                       
                       //if note already in processing disable coin acceptor (no need to accept coin as single note event)
                       if( ( JCM_ACCEPTING == Current_state)       && 
                           ( false == g_AlreadyCoinDisable)        && 
                           ( true  == g_SingleNoteAcceptEvent)     && 
                           ( 0     == g_CoinDisableflag )
                        )
                       {
		                    
                                    //#ifdef JCM_DEBUG
                                    //writeFileLog"[GetNoteDetailInEscrow() while loop Accepting] Accepting State.");
                                    //writeFileLog"[GetNoteDetailInEscrow() while loop Accepting] Start Coin Inhibit block.");
                                    //#endif

                                    #ifdef COIN_ACCEPTOR
		                    //Lock coin acceptor                            
		                    SetAllCoinInhibitStatus();
		                    g_AlreadyCoinDisable=true;
                                    #endif

                                    //#ifdef JCM_DEBUG
				    //writeFileLog"[GetNoteDetailInEscrow() while loop Accepting] End Coin Inhibit block.");                          
                                    //#endif

                                    //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);

                                    return 0;

                  
                       } 
                      
                     
                     //if any below state found Again Enable Coin Acceptor
                     else if( (JCM_ALARM                      == Current_state )  ||
                              (JCM_SENSE_TROUBLE              == Current_state )  ||
	                      (JCM_REJECTION                  == Current_state )  ||
                              (JCM_RETURN_WAITING             == Current_state )  ||
                              (JCM_DISABLED_WITHOUT_ESCROW    == Current_state )  ||
                              (JCM_ENABLED_WITHOUT_ESCROW     == Current_state )  
                       )
                 
                       {
		               
                              
		              if(  (true == g_AlreadyCoinDisable    )  && 
                                   (true == g_SingleNoteAcceptEvent )  && 
                                   ( 0   == g_CoinDisableflag       )  
                                )
		              {
		                     //#ifdef JCM_DEBUG
                                     //writeFileLog"[GetNoteDetailInEscrow() while loop] Start Release Coin Acceptor block.");
                                     //#endif

                                     #ifdef COIN_ACCEPTOR
		                     NaCoinEnableSet(g_faretobeaccept);
		                     g_AlreadyCoinDisable=false;
                                     #endif

                                     //#ifdef JCM_DEBUG
		                     //writeFileLog"[GetNoteDetailInEscrow() while loop] End Release Coin Acceptor block.");
                                     //#endif

		              }   
 
                              //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);
                              return 0;
                          
                           }
			   //check note stacked or not stacked
			   else if( 0x0F == Current_state ) 
			   { 
				     
				     //#ifdef JCM_DEBUG
				     //writeFileLog"[GetNoteDetailInEscrow() Stacked if block] Note Stacked.");  
				     //#endif

				     //get current accepted denom        
				     Current_Accepted_Denom=GetCurrent_JCM_Accepted_Denom();

				     //make a log about accepted money

				     //#ifdef JCM_DEBUG
				     memset(log,'\0',200);
				     sprintf(log,"[GetNoteDetailInEscrow() if block] Accepted Note Rs. : %d .", Current_Accepted_Denom);
				     //writeFileLoglog);
				     //#endif
				   
				  
				     //if accept money is remaining balance then no need to enable coin so return with success
				     if(  (Current_Accepted_Denom == g_faretobeaccept )        && 
				          (true == g_SingleNoteAcceptEvent)                    && 
				          (true == g_AlreadyCoinDisable)                       && 
				          ( 0   == g_CoinDisableflag )
				      )
				     {
					       
				                //#ifdef JCM_DEBUG
				                //writeFileLog"[GetNoteDetailInEscrow() Stacked if block] Start Accepted Note Amount is equal to fare to be accepted block.");
				                //#endif
				                #ifdef COIN_ACCEPTOR
								g_AlreadyCoinDisable=false;
				                #endif

				                //#ifdef JCM_DEBUG
				                //writeFileLog"[GetNoteDetailInEscrow() Stacked if block] End Accepted Note Amount is equal to fare to be accepted block.."); 
				                //#endif

				                //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);
				                return Current_Accepted_Denom ; //success

				                     
				     }
				     
				     //release coin acceptor if accepted note amount less than fare to be accept
				     else if( (g_faretobeaccept > Current_Accepted_Denom)           && 
				              (true == g_SingleNoteAcceptEvent)                     && 
				              (true == g_AlreadyCoinDisable)                        && 
				              ( 0   == g_CoinDisableflag )
				            )
				     {
				          
				           
				           //#ifdef JCM_DEBUG
				           //writeFileLog"[GetNoteDetailInEscrow() Stacked if block] Start Free Coin Poll flag block.");
				           //#endif
				              
				           #ifdef COIN_ACCEPTOR
				           NaCoinEnableSet(g_faretobeaccept- Current_Accepted_Denom); 
				           g_AlreadyCoinDisable=false; 
				           #endif

				           //#ifdef JCM_DEBUG
				           //writeFileLog"[GetNoteDetailInEscrow() Stacked if block] End Free Coin Poll flag block.");
				           //#endif

				           //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);
				           return Current_Accepted_Denom ; //success


				     }
				     else 
				     {
				           
				           //#ifdef JCM_DEBUG
				           //writeFileLog"[GetNoteDetailInEscrow() Stacked if block] Normal path Now time to return from here.");
				           //#endif
				           //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);
					   return Current_Accepted_Denom ; //suceess

				     }
				                                   

			  }    
			  else //by default return zero
			  {
				   //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);
				   return 0;

			  }
          


}//GetNoteDetailInEscrow() end

unsigned int InhibitNoteAcptr(){
    
    ////writeFileLog"[InhibitNoteAcptr()] Before Set Disable Acceptance.");
    pthread_mutex_lock(&g_NASetDisableAcceptance);
    g_DisableAcceptance=true;
    pthread_mutex_unlock(&g_NASetDisableAcceptance);
    ////writeFileLog"[InhibitNoteAcptr()] After Set Disable Acceptance.");
    return 0;


}//InhibitNoteAcptr() end

//Call this function to set thread close status
void  JCM_CloseNoteAcceptorPollThread(){

      
      //writeFileLog"[JCM_CloseNoteAcceptorPollThread()] Before set stop jcm credit polling.");

      pthread_mutex_lock( &g_stopThreadmutex );

      pthread_mutex_lock( &g_PollFlagmutex );

      g_stopThread=true;

      g_PollFlag=false;

      pthread_mutex_unlock( &g_stopThreadmutex );

      pthread_mutex_unlock( &g_PollFlagmutex );

      //writeFileLog"[JCM_CloseNoteAcceptorPollThread()] After set stop jcm credit polling.");

      return;
                

}//JCM_CloseNoteAcceptorPollThread() end

unsigned int  JCM_NoteAcptrCreatePollThread(){
		  
                  
                  pthread_t thread1;
		  int  iret1;
		  int returnVal=-1;  
		  pthread_attr_t attr;
		  returnVal = pthread_attr_init(&attr);
		  if(0!=returnVal)
		  return 0;
		  returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		  if(0!=returnVal)
		  return 0;
		  iret1     = pthread_create( &thread1, &attr,JCM_RSP_Poll,NULL);
		  if(0!= iret1)
		  return 0;
		  returnVal = pthread_attr_destroy(&attr);
              
                  /////////////////////////////////////////////////////////////////////////////
  
                  pthread_mutex_lock( &g_stopThreadmutex );

                  pthread_mutex_lock( &g_PollFlagmutex );

                  //now enable to read rsp state of JCM
                  g_PollFlag=true;

                  g_stopThread=false;

                  pthread_mutex_unlock( &g_stopThreadmutex );

                  pthread_mutex_unlock( &g_PollFlagmutex );
 
                 /////////////////////////////////////////////////////////////////////////////

		  return 1;



}//JCM_NoteAcptrCreatePollThread() end

unsigned int  JCM_Atvm_InitCashInStartThread() {
                 
                
                                  //Set Pre Credit Polling Flag
                                  pthread_mutex_lock(&g_NASetDisableAcceptance);
                                  g_DisableAcceptance=false;
                                  pthread_mutex_unlock(&g_NASetDisableAcceptance);

                                  pthread_mutex_lock( &g_stopThreadmutex );
                                  g_stopThread=false;
                                  pthread_mutex_unlock( &g_stopThreadmutex );

                                  pthread_mutex_lock( &g_PollFlagmutex );
                                  g_PollFlag=false;
                                  pthread_mutex_unlock( &g_PollFlagmutex );

                                  pthread_mutex_lock(&g_JCM_Escrow_State_Mutex);
                                  g_EscrowEvent=false;
                                  pthread_mutex_unlock(&g_JCM_Escrow_State_Mutex);

                                  pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);
                                  g_NoteLowLevelPollFlagStatus=-1;
                                  pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
 
                                  pthread_mutex_lock(&g_NASetSpecificFaremutex);
                                  g_SpecificNoteInhibitFlag=0;
                                  pthread_mutex_unlock(&g_NASetSpecificFaremutex);
    
                                  pthread_mutex_lock(&g_JCM_State_Mutex);

                                  g_JCM_Current_State=0x00;

                                  g_JCM_Accepted_Moneyvalue=0;

                                  pthread_mutex_unlock(&g_JCM_State_Mutex);

                                  g_ExactFareFlag=false;
                                  
                                  pthread_mutex_lock(&g_RejectEventFlagMutex);
                                  g_ExactFare_RejectEvent=false;
                                  pthread_mutex_unlock(&g_RejectEventFlagMutex);
                                  
                                  /*pthread_mutex_lock(&g_DenomStructmutex);
								  g_CurrentNOTE.INR_5    	= 0; 
								  g_CurrentNOTE.INR_10    	= 0; 
								  g_CurrentNOTE.INR_20    	= 0; 
								  g_CurrentNOTE.INR_50    	= 0; 
								  g_CurrentNOTE.INR_100    	= 0; 
								  g_CurrentNOTE.INR_200    	= 0; 
								  g_CurrentNOTE.INR_500    	= 0; 
								  g_CurrentNOTE.INR_1000    = 0;
								  g_CurrentNOTE.INR_2000    = 0;
                                  pthread_mutex_unlock(&g_DenomStructmutex);*/
                                  
                                  return 0;

}//JCM_Atvm_InitCashInStartThread() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  NoteAcptrStartSignalCreditPollThread(){


                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_stopThreadmutex );

                g_stopThread =false;
		
                pthread_mutex_unlock( &g_stopThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                //Send Start Credit Poll Signal
                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_signal( &g_NAEnableThreadCond );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                return;


}//NoteAcptrStartSignalCreditPollThread() end

void  NoteAcptrStopSignalCreditPollThread() {


                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_stopThreadmutex );

                g_stopThread =true;
		
                pthread_mutex_unlock( &g_stopThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////
                
                //Send Stop Credit Poll Signal
                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_signal( &g_NAEnableThreadCond );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                return;


}//NoteAcptrStopSignalCreditPollThread() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

static int JCM_Read_ENQ()
{


                       int counter=1,rtcode=-1,totalByteRead=0;
                       unsigned char log[100],ENQ=0x00;
                       memset(log,'\0',100);
                       int loopvar=6;
                       while( counter <= loopvar )  //wait for enq 
                       {

				    
				    #ifdef JCM_DEBUG
				    memset(log,'\0',100);
                                    sprintf(log,"[JCM_Read_ENQ()()] Now go for ENQ read from JCM  loop counter=%d ." ,counter); 
				    //writeFileLoglog);
				    #endif
				    rtcode=-1;
                                    totalByteRead=0;
                                    ENQ=0x00;
                                    rtcode=BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&ENQ,&totalByteRead);

				    //ENQ byte recv success
				    if( JCM_ENQ == ENQ)
				    {
					 
					 #ifdef JCM_RECV_TIME_DEBUG 
					 
				         memset(log,'\0',200);
				         sprintf(log,"[JCM_Read_ENQ()()] read ENQ success ENQ=0x%xh .",ENQ);
				         //writeFileLoglog); 
                                         clock_gettime(CLOCK_MONOTONIC, &enqendts);
				         enqdiffts.tv_nsec = enqendts.tv_nsec - enqbegints.tv_nsec;
		                         memset(log,'\0',100);
				         sprintf(log,"[JCM_Read_ENQ()] Enq Recv in millisecond=%f .",(enqdiffts.tv_nsec/1000000) ); 
				         //writeFileLoglog);
                                         #endif
				         return 1;     
			

				    }
		                    else if( loopvar == counter ) //ENQ byte recv failed
				    {
				           
                                           #ifdef JCM_DEBUG
					   memset(log,'\0',200);
				           sprintf(log,"[JCM_Read_ENQ()] read ENQ failed ENQ=0x%xh .",ENQ);
				           //writeFileLoglog);
                                           #endif
                                           if( JCM_ENQ == ENQ)
                                           {
                                                 return 1;
                                           }
                                           else
                                           {
                                                 return 0;
                                           }
					 
				    }

                                    JCM_delay_miliseconds(10); //10ms delay

                                    counter++;
                                
             

            }//enq while end


          
}

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GetNADisableAcceptanceSignal() {

       bool rtcode=false;

       pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );

       rtcode = g_NADisableAcceptanceFlag ;

       pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );

       return rtcode;


}//bool GetNADisableAcceptanceSignal() end

bool GetNANoteAcceptanceStatus(){

       bool rtcode=false;

       pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

       rtcode = g_NA_NoteAcceptanceFlag ;
 
       pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );

       return rtcode;


}//end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_Wait(unsigned int Second){
		   

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
		           
		                  sprintf(log,"[JCM_Wait()] Seconds Elapsed=%d",diffts.tv_sec); 

		                  //writeFileLoglog);

		                  #endif

		                  if(diffts.tv_sec>=Second)
		                  {

		                     return 1;

		                  }


                 }//while loop end


}//Wait() end here

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int IssueEscrowCommand(){
         
            int rtcode=-1;

            int CmdLength=JCM_CMD_CMN_LENGTH;

            unsigned char Commands  [JCM_CMD_CMN_LENGTH]=JCM_ESCROW_CMD;

            rtcode= -1;

            unsigned char Response[100];

            unsigned char log[100];

            memset(Response,'\0',100);

            Commands[JCM_CMD_CMN_LENGTH-1]= GetBCC(Commands,
						   2,
						   JCM_CMD_CMN_LENGTH);
			  
            rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);

            memset(log,'\0',100);

            sprintf(log,"[IssueEscrowCommand()]  JCM_TransmitCmdSequence return code= %d .",rtcode);

            //writeFileLoglog);

            if(1==rtcode)
            {
									   
	        //writeFileLog"[IssueEscrowCommand()] Escrow Command Transmission sequence success.");

                //writeFileLog"[IssueEscrowCommand()] Now go for Escrow Command reply bytes.");

                rtcode= -1;

                rtcode= JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_ESCROW_REPLY_SIZE) ;

                memset(log,'\0',100);

                sprintf(log,"[IssueEscrowCommand()]  JCM_ReceieveCmdReplySequence return code= %d .",rtcode);

                //writeFileLoglog);


                if( Response[2] > 0x00 )
                {

                       memset(log,'\0',100);

		       sprintf(log,"[IssueEscrowCommand()] Credit Polling Command = 0x%xh .",Response[2]);

		       //writeFileLoglog);

		       memset(log,'\0',100);

		       sprintf(log,"[IssueEscrowCommand()] Credit Polling State = 0x%xh .",Response[3]);

		       //writeFileLoglog);

                       if( (0x55 == Response[2]) && (0x12 == Response[3]) )
                       {
                            return 1;

                       }
                       else
                       {

                            return 0;
                       }


                }//if( Response[2] > 0x00 ) end
                else
                {



                }

            }//if(1==rtcode) end
            else 
            {
                //writeFileLog"[IssueEscrowCommand()] Escrow Command Transmission sequence failed due to eot byte receieved.");
                return 0;
                                                                                
            }


}//IssueEscrowCommand() end

static int EscrowIssueEOTPatch() {


      unsigned char Response[100];

      memset(Response,'\0',100);

      unsigned char log[200];

      memset(log,'\0',200);
      
      int rtcode=-1;

      //writeFileLog"[EscrowIssueEOTPatch()] Before close com port for jcm note acceptor."); 

      
      //first disconnect com port now 
      if( 1 == JCM_Deactivate() )
      {
           
           //writeFileLog"[EscrowIssueEOTPatch()] close com port successfully done."); 

           g_HANDLE=-1;
 
           //writeFileLog"[EscrowIssueEOTPatch()] Before open com port for jcm note acceptor."); 

           rtcode=BNA_OpenPort(g_JcmPortNumber, &g_HANDLE);

           //writeFileLog"[EscrowIssueEOTPatch()] After open com port for jcm note acceptor."); 
           
	   if( (1==rtcode) && (-1!=g_HANDLE) )
	   {
	                           
                                 //writeFileLog"[EscrowIssueEOTPatch()] open com port for jcm successfully done."); 
	                                 
                                 //writeFileLog"[EscrowIssueEOTPatch()] Before issue jcm sense command ."); 

                                 //issue sense command
                                 rtcode =-1;

                                 memset(Response,0xf,100);

		                 rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                 //writeFileLog"[EscrowIssueEOTPatch()] After issue jcm sense command ."); 
                                         
                                 if( Response[3] >= 0x00 )
			         {
                                      //writeFileLog"[EscrowIssueEOTPatch()] Sense issue  command successfully run."); 

	                         }//if( Response[3] >= 0x00 )end
                                 else
                                 {

                                      //writeFileLog"[EscrowIssueEOTPatch()] jcm sense command issue failed."); 

                                      //writeFileLog"[EscrowIssueEOTPatch()] Now Wait for 20 second."); 

                                      //wait for 1 minute
                                      JCM_Wait(20);

                                      rtcode =-1;

                                      memset(Response,'\0',100);

                                      //writeFileLog"[EscrowIssueEOTPatch()]  Before now again issue sense command."); 

		                      rtcode = JCM_Cmd_Sense(Response,
                                                     JCM_SENSE_REPLY_SIZE,
                                                     JCM_RECV_DELAY);   

                                      //writeFileLog"[EscrowIssueEOTPatch()]  After issue sense command."); 
  
                                  }
 
                                  //finaly check sense command reply bytes                                  
                                  if( Response[3] >= 0x00 )
                                  {

		                          int i=0;

				          for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
					  {
						
				              memset(log,'\0',200);

				              sprintf(log,"[EscrowIssueEOTPatch()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
				              //writeFileLoglog);

		 
				          }

                                          ////////////////////////////////////////////////////////////////
		                           
		                          memset(log,'\0',200);


				          sprintf(log,"[EscrowIssueEOTPatch()] Sense Command = 0x%xh. Current State = 0x%xh.",Response[2],Response[3]);

				          //writeFileLoglog);


                                          ////////////////////////////////////////////////////////////////

		                          switch(Response[3])
		                          {
		                                                                                                                                                                                          
		                                case 0x05: //issue escrow command
                                                           return 1;
		                                           break;

		                                case 0x00: //issue reset command and go to remaining state 
                                                           if( 1 == JCM_Reset() )
                                                           {
                                                              return 2;
                                                           }
                                                           else
                                                           {
                                                              return 3;

                                                           }
		                                           break;

                                                default:   return 3;
                                                           break;
		                          };


                                }
                                else
                                {
                                    //writeFileLog"[EscrowIssueEOTPatch()] Sense command issue failed.");

                                    return 0; 

                                }



	   }
	   else
	   {
	        //writeFileLog"[EscrowIssueEOTPatch()]  open com port for jcm note acceptor failed.");
                return 0; 

           }

     }//if( 1 == JCM_Deactivate() ) end
     else
     {

          //writeFileLog"[EscrowIssueEOTPatch()] close com port failed to done."); 
          return 0;

     }


}//end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void JCM_StoreEscrowState(JCM_ESCROW_NOTE *SourceNotes,JCM_ESCROW_NOTE *DestNotes) {
                                                  
             
             AVRM_writeFileLogV2("[JCM_StoreEscrowState()] Entry.",TRACE,g_BNAId);
             
             AVRM_writeFileLogV2("[JCM_StoreEscrowState()] Source Notes",INFO,g_BNAId);  
             DisplayEscrowTable(SourceNotes);
             
             DestNotes->AcceptedNumber_INR_5   =  SourceNotes->AcceptedNumber_INR_5;
             DestNotes->AcceptedNumber_INR_10  =  SourceNotes->AcceptedNumber_INR_10;
             DestNotes->AcceptedNumber_INR_20  =  SourceNotes->AcceptedNumber_INR_20;
             DestNotes->AcceptedNumber_INR_50  =  SourceNotes->AcceptedNumber_INR_50;
             DestNotes->AcceptedNumber_INR_100 =  SourceNotes->AcceptedNumber_INR_100;
             DestNotes->AcceptedNumber_INR_200 =  SourceNotes->AcceptedNumber_INR_200;
             DestNotes->AcceptedNumber_INR_500 =  SourceNotes->AcceptedNumber_INR_500;
             DestNotes->AcceptedNumber_INR_1000 = SourceNotes->AcceptedNumber_INR_1000;
             DestNotes->AcceptedNumber_INR_2000 = SourceNotes->AcceptedNumber_INR_2000;
             
             AVRM_writeFileLogV2("[JCM_StoreEscrowState()] Dest Notes",INFO,g_BNAId);  
             DisplayEscrowTable(&DestNotes);

}//static void JCM_StoreEscrowState() end


//++Credit Poll
static void* JCM_RSP_Poll(void *ptr) {

                //wait for signal

                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_wait( &g_NAEnableThreadCond , &g_NAEnableThreadmutex );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );
                
                pthread_mutex_lock( &g_stopThreadmutex );

                 //++STOP CREDIT POLLING THREAD
				 if( true ==  g_stopThread )
				 {

		                   
                                   pthread_mutex_unlock(&g_stopThreadmutex);

								   pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

								   g_NoteLowLevelPollFlagStatus=0;

                                   pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

                                   //writeFileLog"[JCM_RSP_Poll() AtvmAPI Exit] JCM_RSP_Poll Thread now exit."); 
                                   
                                   AVRM_writeFileLogV2("[JCM_RSP_Poll() AtvmAPI Exit] JCM_RSP_Poll Thread now exit.",ERROR,g_BNAId);
		                    
		                           pthread_exit(0);

 
                 }
                 else
                 {

                                 pthread_mutex_unlock(&g_stopThreadmutex);

                                 //writeFileLog"[JCM_RSP_Poll()] JCM_RSP_Poll Thread Running."); 
                                 
                                 AVRM_writeFileLogV2("[JCM_RSP_Poll()] JCM_RSP_Poll Thread Running.",INFO,g_BNAId);

                 }//else end
               
		         bool fnrejectEventFlag = false;
		         
		         int RejectState=0x00,fare=0,DisableFlag=0;

                 int Counter=0;

                 int rtcode=-1;

                 unsigned char Response[100];

		         memset(Response,'\0',100);

                 JCM_ESCROW_NOTE currentnote={0,0,0,0,0,0,0,0,0};

                 unsigned char log[100];

                 memset(Response,'\0',100);

                 unsigned int NoteAcceptSequenceFlag=0x00;

                 bool Disable_Acceptance_RSP_ReadFlag=false;

                 bool Escrow_Note_Flag=false,Reject_Note_Flag=false;

                 char enq=0x00;

                 int totalByteRead=1;

                 bool EscrowStateEnableFlag=false,SpecificEnable=false;

                 /*
                 //THREAD PRIORITY WITH PAGE LOCK
                 if(0 == geteuid() )
                 {
		             struct sched_param sp;
		             memset(&sp,0,sizeof(sp));
		             sp.sched_priority=sched_get_priority_max(SCHED_FIFO);
		             if( 0 == sched_setscheduler(0,SCHED_FIFO,&sp)) //success zero return
		             {
		                  //writeFileLog"[JCM_RSP_Poll()] Realtime scheduling set success.");
		             }
		             else
		             {
		                  //writeFileLog"[JCM_RSP_Poll()] Realtime scheduling set failed.");

		             }

                             
		             if( 0 == mlockall(MCL_CURRENT|MCL_FUTURE))
		             {
		                  //writeFileLog"[JCM_RSP_Poll()] Paging disable success.");
		             }
		             else
		             {
		                  //writeFileLog"[JCM_RSP_Poll()] Paging disable failed.");

		             }
		             //writeFileLog"[JCM_RSP_Poll()] running with real time priority");
                             

                 }
                 else
                 {
                             //writeFileLog"[JCM_RSP_Poll()] not running with real time priority");
                 }

                 */

                  //++Set Escrow table
		          currentnote.AcceptedNumber_INR_5=g_currentnote.AcceptedNumber_INR_5;
                  currentnote.AcceptedNumber_INR_10=g_currentnote.AcceptedNumber_INR_10;
                  currentnote.AcceptedNumber_INR_20=g_currentnote.AcceptedNumber_INR_20;
                  currentnote.AcceptedNumber_INR_50=g_currentnote.AcceptedNumber_INR_50;
                  currentnote.AcceptedNumber_INR_100=g_currentnote.AcceptedNumber_INR_100;
                  currentnote.AcceptedNumber_INR_200=g_currentnote.AcceptedNumber_INR_200;
                  currentnote.AcceptedNumber_INR_500=g_currentnote.AcceptedNumber_INR_500;
                  currentnote.AcceptedNumber_INR_1000=g_currentnote.AcceptedNumber_INR_1000;
                  currentnote.AcceptedNumber_INR_2000=g_currentnote.AcceptedNumber_INR_2000;
                  
                  DisplayEscrowTable(&currentnote);
                 
                  pthread_mutex_lock(&g_JCM_State_Mutex);

                  g_JCM_Current_State=0x00;

                  g_JCM_Accepted_Moneyvalue=0;

                  pthread_mutex_unlock(&g_JCM_State_Mutex);

                  RejectState=0x00;

                  fare=0;

                  DisableFlag=0;

                  int replysize=0;

                  int EscrowMoney=0;

                  int InternalReadEnableReplyFlag=0;

                  //++DEFAULT NOTE ACCEPTANCE SEQUENCE FLAG SET TO ZERO
                  NoteAcceptSequenceFlag=0;

                  unsigned char dle0[2],jcmenq=0x05;

                  memset(dle0,'\0',2);

                  int counter=1;

                  unsigned char replychecksum=0x00;

                  //++Get user selected fare
                  int userfare=0;

                  userfare =  GetFare();

                  JCM_ESCROW_NOTE tempcurrentnote={0,0,0,0,0,0,0};

                  int CurrentAcceptedAmount=0,CurrenteEscrowAmount=0;

                  g_EscrowEvent=false;

                  pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );

                  g_NADisableAcceptanceFlag=false;

                  pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );

                  pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

                  g_NA_NoteAcceptanceFlag = false;

                  pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );

                  bool DisableCommandIssue=false;
                  
                  bool CurrentDenomEnableFlag=false;
                  
                  bool fnEnableDenomEnableFlag=false;
                  
                  //++First Time Disable Outer Denom Enabled
                  pthread_mutex_lock(&g_DenomStructmutex);
				  g_CurrentDenomEnableFlag = false;
				  pthread_mutex_unlock(&g_DenomStructmutex);
                  
                  while(1) 
                  {

                       pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);
                       //++Credit Polling Started
                       g_NoteLowLevelPollFlagStatus=1;
                       pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
                       
                       pthread_mutex_lock( &g_PollFlagmutex );
                       pthread_mutex_lock( &g_stopThreadmutex );

                       //++Start Read RSP Packet Polling
                       if( (true == g_PollFlag ) && ( false == g_stopThread ) )
                       {
		                       
                                       pthread_mutex_unlock( &g_PollFlagmutex );
                                       pthread_mutex_unlock( &g_stopThreadmutex );
                                
                                       //++Disable JCM Note Acceptor after credit poll
									   pthread_mutex_lock(&g_NASetDisableAcceptance);

									   if( true == g_DisableAcceptance  )  
									   { 
											    //DisableFlag=1;

											    if( ( 0 == NoteAcceptSequenceFlag )  && ( 0 == InternalReadEnableReplyFlag ) && (0 == DisableFlag) ) 
											    {

																	   ////writeFileLog"[JCM_RSP_Poll() Disable Acceptance] Before issue Disable Acceptance command.");
																	   
																	   AVRM_writeFileLogV2("[JCM_RSP_Poll() Disable Acceptance] Before issue Disable Acceptance command.",INFO,g_BNAId);

																		//issue disable acceptance command
																	   if( 1 == Disable_Acceptance_During_RSPPoll() )
																	   {

																		   Disable_Acceptance_RSP_ReadFlag=true;

																		   DisableCommandIssue=true;

																		   //Now set disable flag that we already disable note acceptor no need to do that again
																		   DisableFlag=1;

																		   ////writeFileLog"[JCM_RSP_Poll()] successfully issue disable acceptance command.");
																		   
																		   AVRM_writeFileLogV2("[JCM_RSP_Poll()] successfully issue disable acceptance command.",INFO,g_BNAId);
																	   }
																	   else 
																	   {
																		  ////writeFileLog"[JCM_RSP_Poll()] Failed to issue disable acceptance command.");
																		  
																		  AVRM_writeFileLogV2("[JCM_RSP_Poll()] Failed to issue disable acceptance command.",INFO,g_BNAId);

																	   }//else end

																	  ////writeFileLog"[JCM_RSP_Poll() Disable Acceptance] After issue Disable Acceptance command.");
																	  AVRM_writeFileLogV2("[JCM_RSP_Poll() Disable Acceptance] After issue Disable Acceptance command.",INFO,g_BNAId);
												   
												}//if( ( 0 == NoteAcceptSequenceFlag )  && ( 0 == InternalReadEnableReplyFlag ) && (0 == DisableFlag) ) end
											 
												pthread_mutex_unlock(&g_NASetDisableAcceptance);
									   }
									   else
									   {
									       pthread_mutex_unlock(&g_NASetDisableAcceptance); 
									       
									       ///////////////////////////////////////////////////////////////////////////////
										   	
										   //++Run time EnableThisDenom Change
										   pthread_mutex_lock(&g_DenomStructmutex);
										   CurrentDenomEnableFlag = false;
										   CurrentDenomEnableFlag = g_CurrentDenomEnableFlag;
										   if( true == g_CurrentDenomEnableFlag )
										   {
											   g_CurrentDenomEnableFlag = false;
										   }//++if end		
										   pthread_mutex_unlock(&g_DenomStructmutex);
										   
										   if( true == CurrentDenomEnableFlag )
										   {
											   CurrentDenomEnableFlag  = false;
											   EnableDenomDuringCreditPolling(0,JCM_EXTERNAL_COMMAND_REPLY_READ);
										   }//++if end
										   
										   ///////////////////////////////////////////////////////////////////////////////
								       
									   }//else end
					    
					                   /*
					                   //Exit Thread if exact fare with reject state found
					                   fnrejectEventFlag = false;
					                   fnrejectEventFlag = getNARejectEventFlagStatus();
						               if( (true == g_ExactFareFlag) && (true == fnrejectEventFlag) ){
											 
											 //++Set Thread Low Level Flag Status
										     AVRM_writeFileLogV2("[JCM_RSP_Poll()] Exact Fare With Reject State Found Now Exit Thread",INFO,g_BNAId);
										     pthread_mutex_lock( &g_PollFlagmutex );
											 g_PollFlag=false;
											 pthread_mutex_unlock( &g_PollFlagmutex );
											 pthread_mutex_lock( &g_stopThreadmutex );
											 g_stopThread=true;
											 pthread_mutex_lock( &g_stopThreadmutex );
											 continue;
								       
								       }//if end
								       */
								      
                                       rtcode=-1;
		                               memset(Response,'\0',100);
                              
                                       //++Set Current Reply Packet size
									   if( true == Disable_Acceptance_RSP_ReadFlag ){
											  
											  replysize=JCM_INSERTION_AUTHORIZING_REPLY_SIZE;
                                              Disable_Acceptance_RSP_ReadFlag=false;
										   
									   }else if( 1 == InternalReadEnableReplyFlag ){
											  
											  replysize=JCM_INSERTION_AUTHORIZING_REPLY_SIZE;
                                              InternalReadEnableReplyFlag=0;
										   
									   }else if ( true == Escrow_Note_Flag ){
                                              replysize=JCM_ESCROW_REPLY_SIZE ;

                                       }else if ( true == Reject_Note_Flag ){
                                             replysize=JCM_REJECT_REPLY_SIZE ;

                                       }else{
										     replysize=JCM_RSP_REPLY_SIZE;
									   }//else end
                                       
                                       //++read rsp packet
		                               rtcode = JCM_Receieve_RSP_Packet(Response,replysize);
                                       if( Response[3]> 0 ) 
                                       {
                                         //++LogDataExchnage( "Rx",Response,replysize);
                                       }//if end

                                       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                       //check rsp checksum value
                                       //replychecksum=0x00;
                                       //replychecksum=GetBCC(Response,2,replysize);
                                       //if(replychecksum!=Response[replysize-1])
                                       //{

                                          // //writeFileLog"[JCM_RSP_Poll()] RSP packet reply byte checksum value not matched."); 
                                           //memset(log,'\0',100);
                                           //sprintf(log,"[JCM_RSP_Poll()] replychecksum= 0x%xh. Response[%d] = 0x%xh.",replysize,replysize-1,Response[replysize-1]);
                                           ////writeFileLoglog);
                                           //int Counter=0;
                                           //for(Counter=0;Counter<replysize;Counter++){
						                       //memset(log,'\0',100);
						                       //sprintf(log,"[JCM_RSP_Poll()] Response[%d] = 0x%xh .",Counter,Response[Counter]);
		                                       ////writeFileLoglog);
		                                   //}
		                                   //continue;
		                                //}
                                       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                       if( Response[3]> 0 ) 
                                       {
						
		                               
												 #ifdef JCM_DEBUG   
												 int Counter=0 ;
                                                 for(Counter=0;Counter<replysize;Counter++)
                                                 {
								                         memset(log,'\0',100);
														 sprintf(log,"[JCM_RSP_Poll()] Response[%d] = 0x%xh .",Counter,Response[Counter]);
														 AVRM_writeFileLogV2(log,INFO,g_BNAId);
												  }//for end
												 #endif
												  
												 memset(log,'\0',100);
												 sprintf(log,"[JCM_RSP_Poll()] Credit Polling Command = 0x%xh .",Response[2]);
												 AVRM_writeFileLogV2(log,INFO,g_BNAId);
												 memset(log,'\0',100);
												 sprintf(log,"[JCM_RSP_Poll()] Credit Polling State = 0x%xh .",Response[3]);
												 AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                 //++SET CURRENT STATE
												 pthread_mutex_lock(&g_JCM_State_Mutex);
												 g_JCM_Current_State=Response[3];
												 pthread_mutex_unlock(&g_JCM_State_Mutex);
                 
            				                 switch( Response[3] ) 
            				                 {     
     
                                              //++State:Identification
				                              case 0x04 :

						                              NoteAcceptSequenceFlag=1;
                                                      pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );
													  g_NA_NoteAcceptanceFlag = true;
													  pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );
													  if( (0x04 == Response[3]) &&(0x50 == Response[2])  ) 
													  {
														     AVRM_writeFileLogV2("[JCM_RSP_Poll() 0x04] Before Set Disable acceptance signal.",INFO,g_BNAId);
                                                             pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );
                                                             g_NADisableAcceptanceFlag=true;
                                                             pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );
                                                             AVRM_writeFileLogV2("[JCM_RSP_Poll() 0x04] After Set Disable acceptance signal.",INFO,g_BNAId);
                                                      }//if end

                                                      break; 
                                                       
                                               //++State:Acceptance
                                               case 0x05:

                                                       NoteAcceptSequenceFlag=1; 
                                                       pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );
                                                       g_NA_NoteAcceptanceFlag = true;
                                                       pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );
				                                       AVRM_writeFileLogV2("[JCM_RSP_Poll()] Entry Acceptance Block.",INFO,g_BNAId);  
				                                       tempcurrentnote.AcceptedNumber_INR_5=0; 
                                                       tempcurrentnote.AcceptedNumber_INR_10=0; 
				                                       tempcurrentnote.AcceptedNumber_INR_20=0;
				                                       tempcurrentnote.AcceptedNumber_INR_50=0;
				                                       tempcurrentnote.AcceptedNumber_INR_100=0; 
				                                       tempcurrentnote.AcceptedNumber_INR_200=0; 
				                                       tempcurrentnote.AcceptedNumber_INR_500=0;
				                                       tempcurrentnote.AcceptedNumber_INR_1000=0;
				                                       tempcurrentnote.AcceptedNumber_INR_2000=0;
				                                       CurrentAcceptedAmount=0;
				                                       CurrenteEscrowAmount=0;
				                                       
				                                       //++Store Temporary Denomination Information
				                                       tempcurrentnote.AcceptedNumber_INR_5=currentnote.AcceptedNumber_INR_5; 
				                                       tempcurrentnote.AcceptedNumber_INR_10=currentnote.AcceptedNumber_INR_10; 
				                                       tempcurrentnote.AcceptedNumber_INR_20=currentnote.AcceptedNumber_INR_20;
				                                       tempcurrentnote.AcceptedNumber_INR_50=currentnote.AcceptedNumber_INR_50; 
				                                       tempcurrentnote.AcceptedNumber_INR_100=currentnote.AcceptedNumber_INR_100; 
				                                       tempcurrentnote.AcceptedNumber_INR_200=currentnote.AcceptedNumber_INR_200; 
				                                       tempcurrentnote.AcceptedNumber_INR_500=currentnote.AcceptedNumber_INR_500; 
				                                       tempcurrentnote.AcceptedNumber_INR_1000=currentnote.AcceptedNumber_INR_1000; 
                                                       tempcurrentnote.AcceptedNumber_INR_2000=currentnote.AcceptedNumber_INR_2000;
                                                       
                                                       AVRM_writeFileLogV2("[JCM_RSP_Poll()] State:Acceptance Credit Polling Stores Escrow Table: ",INFO,g_BNAId); 
                                                       DisplayEscrowTable(&currentnote); 
                                                       
                                                       //++Get current escrow money information
				                                       CurrenteEscrowAmount	 =  JCM_GetDenom_From_EscrowState(Response,&tempcurrentnote);
				                                       CurrentAcceptedAmount =  GetCurrentAcceptedAmount();
				                                       memset(log,'\0',100);
				                                       sprintf(log,"[JCM_RSP_Poll()] fare = %d  CurrentAcceptedAmount = %d . CurrenteEscrowAmount= %d .",userfare,CurrentAcceptedAmount,CurrenteEscrowAmount);
				                                       AVRM_writeFileLogV2(log,INFO,g_BNAId);  
		                                               //++Accept Amount
				                                       if( (CurrentAcceptedAmount+CurrenteEscrowAmount) <= userfare ) 
				                                       {
														   
														   AVRM_writeFileLogV2("[JCM_RSP_Poll()] Start:Now Going to accept Money.",INFO,g_BNAId);  
                                                           //++Update Coin Acceptor Table
                                                           #if defined(COIN_ACCEPTOR)
                                                           SetSpecificCoinInhibitStatus(CurrentAcceptedAmount+CurrenteEscrowAmount);
                                                           #endif
                                                           //++issue escrow command
				                                           int rtcode=-1;
				                                           int CmdLength=JCM_CMD_CMN_LENGTH;
				                                           unsigned char Commands  [JCM_CMD_CMN_LENGTH]=JCM_ESCROW_CMD;
				                                           rtcode= -1;
				                                           Commands[JCM_CMD_CMN_LENGTH-1]=GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
				                                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);
								                           memset(log,'\0',100);
								                           sprintf(log,"[JCM_RSP_Poll()]  JCM_TransmitCmdSequence return code= %d .",rtcode);
                                                           AVRM_writeFileLogV2(log,INFO,g_BNAId);  
                                                           if(1==rtcode)
                                                           {
									                           AVRM_writeFileLogV2("[JCM_RSP_Poll()] Escrow Command Transmission sequence success.",INFO,g_BNAId);  
									                           //++JCM_StoreEscrowState(tempcurrentnote,currentnote);
                                                           }
                                                           else 
                                                           {
															   AVRM_writeFileLogV2("[JCM_RSP_Poll()] Escrow Command Transmission sequence failed due to eot byte receieved.",INFO,g_BNAId);  
                                                               AVRM_writeFileLogV2("[JCM_RSP_Poll()] Before go for jcm transmission sequent patch function.",INFO,g_BNAId);  
                                                               rtcode=-1;
                                                               rtcode = EscrowIssueEOTPatch();
                                                               AVRM_writeFileLogV2("[JCM_RSP_Poll()] After go for jcm transmission sequent patch function.",INFO,g_BNAId);
                                                               if( 1 == rtcode )
                                                               {
																   
                                                                   AVRM_writeFileLogV2("[JCM_RSP_Poll()] jcm transmission sequent patch function successfully run.",INFO,g_BNAId);
                                                                   AVRM_writeFileLogV2("[JCM_RSP_Poll()] Before Now going to again run escrow command.",INFO,g_BNAId);
                                                                   rtcode= -1;
                                                                   Commands[JCM_CMD_CMN_LENGTH-1]=GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
											                       rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);
		                                                           AVRM_writeFileLogV2("[JCM_RSP_Poll()] After Now going to again run escrow command.",INFO,g_BNAId);
		                                                           //++JCM_StoreEscrowState(tempcurrentnote,currentnote);

                                                               }
                                                               else
                                                               {
																   
                                                                   AVRM_writeFileLogV2("[JCM_RSP_Poll()] After Now going to again run escrow command.",INFO,g_BNAId);
                                                                   NoteAcceptSequenceFlag=0;
                                                                   Escrow_Note_Flag=false;
                                                                   continue;
                                                               }//else end

                                                           }//main else block 

                                                          Escrow_Note_Flag=true;
                                                          AVRM_writeFileLogV2("[JCM_RSP_Poll()] End:Now Going to accept Money.",INFO,g_BNAId);
                                                          
													   } //if( (CurrentAcceptedAmount+CurrenteEscrowAmount) <= userfare ) Escrow Block end
				                                       //++Reject Amount when amount is equal or greater than   
                                                       else if( (CurrentAcceptedAmount+CurrenteEscrowAmount) > userfare  ){

                                                           //++issue reject command
                                                           Reject_Note_Flag=true;
                                                           replysize= JCM_REJECT_REPLY_SIZE ;
                                                           int rtcode=-1;
                                                           int CmdLength=JCM_CMD_CMN_LENGTH;
				                                           unsigned char Commands [JCM_CMD_CMN_LENGTH]=JCM_REJECT_CMD;
                                                           Commands[JCM_CMD_CMN_LENGTH-1]=GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
                                                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);
									                       if(1==rtcode){
																 
																AVRM_writeFileLogV2("[JCM_RSP_Poll()] Reject Command Transmission sequence success.",INFO,g_BNAId);
														   }else{
															    
																AVRM_writeFileLogV2("[JCM_RSP_Poll()] Reject Command Transmission sequence failed.",WARN,g_BNAId);
														   }//else end 

				                                      }//else if( (CurrentAcceptedAmount+CurrenteEscrowAmount) > userfare  ) Reject Block end
				                                      break;

                                                            
				                            //++State:REJECT STATE WITH WAITING TO BE RECEIEVED
						                    case 0x09:
                                                      NoteAcceptSequenceFlag=1;
						                              RejectState=Response[3];
						                              fnrejectEventFlag = true;
						                              
											          if( true == g_ExactFareFlag )
											          {
														  
														  AVRM_writeFileLogV2("[JCM_RSP_Poll() Stop Thread RejectEvent & ExactFare] Start: Found Exact fare With Reject State",INFO,g_BNAId);
														  //++Set SingleFareRejectEvent
						                                  setNARejectEventFlagStatus(fnrejectEventFlag);
														  RejectState=0x00;
														  pthread_mutex_lock(&g_NASetDisableAcceptance);
                                                          g_DisableAcceptance=true;
                                                          pthread_mutex_unlock(&g_NASetDisableAcceptance);
                                                          AVRM_writeFileLogV2("[JCM_RSP_Poll() Stop Thread RejectEvent & ExactFare] End: Found Exact fare With Reject State",INFO,g_BNAId);
                                                          
                                                      }//if end
													  
													  break;

		                                    //++State: Rejection state
				                            case 0x08: 
                                                      NoteAcceptSequenceFlag=1;
                                                      pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );
                                                      g_NA_NoteAcceptanceFlag = true;            
													  pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );
					                                  break;

							               case 0x03: //++Ready state without escrow notes
				                           case 0x13: //++Ready state with    escrow notes
				                                      NoteAcceptSequenceFlag=0;
				                                      if( true == DisableCommandIssue )
				                                      {
                                                            if( ( (0x13 == Response[3]) && (0x50 == Response[2]) ) || ( (0x03 == Response[3])&& (0x50 == Response[2]) ) ) 
                                                            { 
                                                                        
                                                                        //writeFileLog"[JCM_RSP_Poll()] Before Set Disable acceptance signal.");
                                                                        AVRM_writeFileLogV2("[JCM_RSP_Poll()] Before Set Disable acceptance signal.",INFO,g_BNAId);
                                                                        pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );
                                                                        g_NADisableAcceptanceFlag=true;
																	    pthread_mutex_unlock(&g_NADisableAcceptanceFlagMutex );
                                                                        AVRM_writeFileLogV2("[JCM_RSP_Poll()] After Set Disable acceptance signal.",INFO,g_BNAId);
                                                                        AVRM_writeFileLogV2("[JCM_RSP_Poll()] Before Set Current Note acceptance signal.",INFO,g_BNAId);
                                                                        //writeFileLog"[JCM_RSP_Poll()] After Set Disable acceptance signal.");
																	    //writeFileLog"[JCM_RSP_Poll()] Before Set Current Note acceptance signal.");
		                                                                pthread_mutex_lock(&g_NA_NoteAcceptanceFlagMutex );
													                    g_NA_NoteAcceptanceFlag = false;
                                                                	    pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );					            
															            //writeFileLog"[JCM_RSP_Poll()] After Set Current Note acceptance signal.");
		                                                                AVRM_writeFileLogV2("[JCM_RSP_Poll()] After Set Current Note acceptance signal.",INFO,g_BNAId);
															}//if( ( (0x13 == Response[3]) && (0x50 == Response[2]) ) || ( (0x03 == Response[3])&& (0x50 == Response[2]) ) )  END
                                                      }
                                                      else
                                                      {
                                                            pthread_mutex_lock(  &g_NA_NoteAcceptanceFlagMutex );
                                                            g_NA_NoteAcceptanceFlag = false;          
														    pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );
					                                  }//else end

				                                      break; 

						                   case 0x02:
				                           case 0x12: 
				                                      NoteAcceptSequenceFlag=0;
													  pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );
                                                      g_NA_NoteAcceptanceFlag = false;
                                                      pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex ); 
                                                           
													  //++Signal Disable acceptance signal
					                                  if( ( (0x12 == Response[3]) && (0x50 == Response[2]) ) || ((0x02 == Response[3]) && (0x50 == Response[2]) ) )
					                                  {
														  
														  
														  AVRM_writeFileLogV2("[JCM_RSP_Poll() 0x12] Before Set Disable acceptance signal.",INFO,g_BNAId);
                                                          pthread_mutex_lock(  &g_NADisableAcceptanceFlagMutex );
                                                          g_NADisableAcceptanceFlag=true;               
													      pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );
                                                          AVRM_writeFileLogV2("[JCM_RSP_Poll() 0x12] After Set Disable acceptance signal.",INFO,g_BNAId); 
                                                                     
                                                      }//if end
                                                      
                                                      //++Note is now stacked
				                                      if( (0x12 == Response[3]) && (0x55 == Response[2]) &&  (false == g_EscrowEvent) ) 
				                                      { 
                                                          
                                                          AVRM_writeFileLogV2("[JCM_RSP_Poll()] Entry Escrow Block.",INFO,g_BNAId);
                                                          Escrow_Note_Flag=false;
				                                          //++Record Escrow Money
						                                  EscrowMoney=JCM_GetDenom_From_EscrowState( Response,&currentnote);
                                                          if( (EscrowMoney>=5 ) &&   (EscrowMoney<=2000) ) 
                                                          {
															             
															  EscrowStateEnableFlag=true;
															  pthread_mutex_lock(&g_JCM_Escrow_State_Mutex);
															  g_EscrowEvent=true;
															  g_JCM_Accepted_Moneyvalue = EscrowMoney;
															  EscrowMoney=0;
															  pthread_mutex_unlock(&g_JCM_Escrow_State_Mutex);
															  AVRM_writeFileLogV2("[JCM_RSP_Poll() Credit Polling Escrow State ] Escrow State happened.",INFO,g_BNAId);
															  pthread_mutex_lock(&g_JCM_State_Mutex);
															  //++User defined escrow state complete
								                              g_JCM_Current_State=0x0f;
								                              pthread_mutex_unlock(&g_JCM_State_Mutex);
								                              
								                          }//if end
								                          AVRM_writeFileLogV2("[JCM_RSP_Poll()] Exit Escrow Block.",INFO,g_BNAId);
								                          continue;
								                          
								                      }//if( (0x12 == Response[3]) && (0x55 == Response[2]) &&  (false == g_EscrowEvent) ) end
								                      
								                      //++Reject state enable
							                          if(((0x09 == RejectState) && (0x12 == Response[3]) ) ||((0x09 == RejectState) && (0x02 == Response[3]) ))
							                          {
														  
															  RejectState=0x00;
															  pthread_mutex_lock( &g_stopThreadmutex );
															  if( (false == g_stopThread ) && ( 0 == DisableFlag ) ) 
															  {
																 
																 pthread_mutex_unlock( &g_stopThreadmutex );
																 bool InternalEnableflag=false;
																 //Run time inhibit
																 pthread_mutex_lock(&g_NASetSpecificFaremutex);
																 if( 1 == g_SpecificNoteInhibitFlag ) 
																 {  
																	 AVRM_writeFileLogV2("[JCM_RSP_Poll() Reject Event] External Enable Event lock.",INFO,g_BNAId);
																	 fare=0;
																	 fare=g_NACurrentAtvmFare;
																	 g_SpecificNoteInhibitFlag=0;
																	 AVRM_writeFileLogV2("[JCM_RSP_Poll() Reject Event] External Enable Event unlock.",INFO,g_BNAId);
																 }
																 else
																 {
																	 InternalEnableflag=true;
																 }//else end
																 pthread_mutex_unlock(&g_NASetSpecificFaremutex);
																 if( true == InternalEnableflag)
																 {
																	pthread_mutex_lock(&g_NASetCurrentFare);
																	fare=g_ThreadCurrentfare;
																	pthread_mutex_unlock(&g_NASetCurrentFare);
																 }//if end
																 memset(log,'\0',100);
																 sprintf(log,"[JCM_RSP_Poll() Reject Event]  Current Fare = %d ",fare);
																 AVRM_writeFileLogV2(log,INFO,g_BNAId);
																 RejectState=0x00;
																 //note accepting process finished
																 NoteAcceptSequenceFlag=0; 
																 AVRM_writeFileLogV2("[JCM_RSP_Poll() Reject Event]  Entry Enable",INFO,g_BNAId);
																 pthread_mutex_lock(&g_NASetDisableAcceptance);
																 if( true == g_DisableAcceptance  ) 
																 {
																		pthread_mutex_unlock(&g_NASetDisableAcceptance);
																		AVRM_writeFileLogV2("[JCM_RSP_Poll() Reject Event] Unable to do enable note acceptor as inhibit signal is on.",INFO,g_BNAId);
																		continue;
																 }//if end
																 pthread_mutex_unlock(&g_NASetDisableAcceptance);
																 if( true == fnEnableDenomEnableFlag )
																 {
																	AVRM_writeFileLogV2("[JCM_RSP_Poll() Reject Event] outer denom enable active.",INFO,g_BNAId);
																	fnEnableDenomEnableFlag = false; //++Set External Denom flag false
																 }
													             else
															     {
																	 AVRM_writeFileLogV2("[JCM_RSP_Poll() Reject Event] outer denom enable inactive so active from poll.",INFO,g_BNAId);  
																	 EnableDenomDuringCreditPolling(fare,JCM_EXTERNAL_COMMAND_REPLY_READ);
																 }//else end
																 
																 //++Enable Note Acceptor
																 //++EnableDenomDuringCreditPolling(fare, JCM_EXTERNAL_COMMAND_REPLY_READ);
																 
																 AVRM_writeFileLogV2("[JCM_RSP_Poll() Reject Event]  Exit Enable",INFO,g_BNAId);
																 
														  } else {
																 pthread_mutex_unlock( &g_stopThreadmutex );
														  }//else end
														  continue;
				                                      
				                                      }else{
				                                         AVRM_writeFileLogV2("[JCM_RSP_Poll() Stop Thread RejectEvent & ExactFare] No Need to enable note acceptor",INFO,g_BNAId);
												      }
				                                      
				                                      break;

											default: memset(log,'\0',100);
													 sprintf(log,"[JCM_RSP_Poll()] Unknown State = 0x%xh. ",Response[3]);
													 AVRM_writeFileLogV2(log,INFO,g_BNAId);
													 break;

                                         }; //switch block exit
                          
                                     }//if(Response[3]>0) block

                                     pthread_mutex_lock( &g_stopThreadmutex );

									//++Enable from Note and Coin Acceptor can be done here 
									if( ( 0 == NoteAcceptSequenceFlag ) && ( false == g_stopThread ) && ( 0 == DisableFlag ) )
									{

															 
															 pthread_mutex_unlock( &g_stopThreadmutex );
															 //if already get disable signal then 
															 //dont need to enable it again
															 pthread_mutex_lock(&g_NASetDisableAcceptance);

															 if( true == g_DisableAcceptance )
															 {
                                                                 AVRM_writeFileLogV2("[JCM_RSP_Poll() Enable block] Unable to do enable note acceptor as inhibit Signal Receieved.",INFO,g_BNAId);
																 //writeFileLog"[JCM_RSP_Poll() Enable block] Unable to do enable note acceptor as inhibit Signal Receieved.");  
																 pthread_mutex_unlock(&g_NASetDisableAcceptance);
																 continue;

															 }//if end
				 
															 pthread_mutex_unlock(&g_NASetDisableAcceptance);
															 //++Run time inhibit
															 pthread_mutex_lock( &g_NASetSpecificFaremutex );
															 if( 1 == g_SpecificNoteInhibitFlag )  
															 {
														       fare=0;
															   fare=g_NACurrentAtvmFare;
															   g_SpecificNoteInhibitFlag=0;
															   SpecificEnable=true;
													         }//if end
													         pthread_mutex_unlock(&g_NASetSpecificFaremutex);
															 if(true == SpecificEnable) 
															 {
																	  
																	  AVRM_writeFileLogV2("[JCM_RSP_Poll() Enable block] Enable Signal Receieved.",INFO,g_BNAId);
																	  SpecificEnable=false;
																	  memset(log,'\0',100);
																	  sprintf(log,"[JCM_RSP_Poll() Common Enable block]  Current Fare = %d .",fare);
																	  AVRM_writeFileLogV2(log,INFO,g_BNAId);
																	  if(true == EscrowStateEnableFlag) 
																	  {
					                                                          
					                                                          AVRM_writeFileLogV2("[JCM_RSP_Poll() With Escrow Enable block] Found Escrow Enable State.",INFO,g_BNAId);
																			  EscrowStateEnableFlag=false;
																			  AVRM_writeFileLogV2("[JCM_RSP_Poll() With Escrow Enable block] Start Enable.",INFO,g_BNAId);
																			  pthread_mutex_lock(&g_NASetDisableAcceptance);
																			  if( true == g_DisableAcceptance ) 
																			  {
																				 AVRM_writeFileLogV2("[JCM_RSP_Poll() With Escrow Enable block] No need to do enable note acceptor as inhibit signal is on.",INFO,g_BNAId);
																				 pthread_mutex_unlock(&g_NASetDisableAcceptance);
																				 continue;
																			  }//if end
																			  pthread_mutex_unlock(&g_NASetDisableAcceptance);
																			  
																			  if( true == fnEnableDenomEnableFlag )
																			  {
																				  AVRM_writeFileLogV2("[JCM_RSP_Poll() With Escrow  Enable block] outer denom enable active.",INFO,g_BNAId);
																				  fnEnableDenomEnableFlag = false; //++Set External Denom flag false
																			  }
																			  else
																			  {
																				AVRM_writeFileLogV2("[JCM_RSP_Poll() With Escrow  Enable block] outer denom enable inactive so active from poll.",INFO,g_BNAId);  
																				EnableDenomDuringCreditPolling(fare,JCM_EXTERNAL_COMMAND_REPLY_READ);
																			  }//++else end
																			  
																			  //++Enable Note Acceptor
																			  //++EnableDenomDuringCreditPolling(fare,JCM_EXTERNAL_COMMAND_REPLY_READ);
																			  
																			  AVRM_writeFileLogV2("[JCM_RSP_Poll() With Escrow  Enable block] End Enable.",INFO,g_BNAId);

																	  }
																	  else if( false == EscrowStateEnableFlag )
																	  {
																		  AVRM_writeFileLogV2("[JCM_RSP_Poll() Without Escrow Enable block] Found Without Escrow Enable State.",INFO,g_BNAId);
																		  totalByteRead=1;
																		  enq=0x00;
																		  BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);
																		  if( 0x05 != enq ) 
																		  {
																			  AVRM_writeFileLogV2("[JCM_RSP_Poll() Without Escrow Enable block] Start Enable.",INFO,g_BNAId);
																			  pthread_mutex_lock(&g_NASetDisableAcceptance);
																			  if( true == g_DisableAcceptance ) 
																			  {
																				 AVRM_writeFileLogV2("[JCM_RSP_Poll() Without Escrow Enable block] No need to do enable note acceptor as inhibit signal is on.",INFO,g_BNAId);
																				 ////writeFileLog"[JCM_RSP_Poll() Without Escrow Enable block] No need to do enable note acceptor as inhibit signal is on.");
																				 pthread_mutex_unlock(&g_NASetDisableAcceptance);
																				 continue;
																			  }//IF END
																			  pthread_mutex_unlock(&g_NASetDisableAcceptance);
																			  //++Enable Note Acceptor
																			  EnableDenomDuringCreditPolling(fare, JCM_INTERNAL_COMMAND_REPLY_READ);
																			  InternalReadEnableReplyFlag=1;   
																			  AVRM_writeFileLogV2("[JCM_RSP_Poll() Without Escrow  Enable block] End Enable.",INFO,g_BNAId);
																		  }
																		  else
																		  {
																			  AVRM_writeFileLogV2("[JCM_RSP_Poll() Without Escrow  Enable block] Unable to accept update denomination change request due to get jcm state change enq byte receieved.",INFO,g_BNAId);
																			  ////writeFileLog"[JCM_RSP_Poll() Without Escrow  Enable block] Unable to accept update denomination change request due to get jcm state change enq byte receieved.");
																		  }//else end
																	 }//++else if( false == EscrowStateEnableFlag ) end
															 }//if(true == SpecificEnable) end
									}
									else 
									{                                      
										pthread_mutex_unlock( &g_stopThreadmutex );
									}//else end

									 /*

									 //Disable JCM Note Acceptor after credit poll
									 pthread_mutex_lock(&g_NASetDisableAcceptance);

									 if( true == g_DisableAcceptance  ) 
									 { 

											   DisableFlag=1;

											   if( ( 0 == NoteAcceptSequenceFlag ) && ( 0 == InternalReadEnableReplyFlag ) )
											   {

											   //writeFileLog"[JCM_RSP_Poll() Disable Acceptance] Before Disable Acceptance.");

													   Disable_Acceptance_RSP_ReadFlag=true;

													   //issue disable acceptance command
											   Disable_Acceptance_During_RSPPoll();

													   g_DisableAcceptance=false;

													   //writeFileLog"[JCM_RSP_Poll() Disable Acceptance] After Disable Acceptance.");

											   }
											 
											   pthread_mutex_unlock(&g_NASetDisableAcceptance);

											   continue;

											   

									}    
									else
									{
											   pthread_mutex_unlock(&g_NASetDisableAcceptance); 

									}

									*/

       }
       else  
       {
                             pthread_mutex_unlock( &g_PollFlagmutex );
                             //Check Kill Signal from upper level
							 if( true ==  g_stopThread ){
							       pthread_mutex_unlock( &g_stopThreadmutex );
								   //writeFileLog"[JCM_RSP_Poll() Stop Thread] RSP Poll Thread now exit.");
								   AVRM_writeFileLogV2("[JCM_RSP_Poll() Stop Thread] RSP Poll Thread now exit.",INFO,g_BNAId);
								   pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);
								   g_NoteLowLevelPollFlagStatus=0;
								   pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
								   pthread_exit(0); 
							 }else {
				                   pthread_mutex_unlock( &g_stopThreadmutex );

							 }//else end
                                      
        }//++else end

    }//++while loop end   

}//void* JCM_RSP_Poll(void *ptr) thread end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
int EnableDenomDuringCreditPolling(int fare,int flag){
                

                int rtcode=-1,CmdLength=0,Counter=0;

                unsigned char DefaultEnablebyte=0x00;

                unsigned char Response[100];

                memset(Response,'\0',100);

                char log[200];

                memset(log,'\0',200);

                unsigned char DittoCommands[ JCM_CMD_DITTO_INSERTION_AUTH_LENGTH ]=JCM_INSERTION_AUTHORISED_DITTO_CMD;

                unsigned char Commands[ JCM_CMD_INSERTION_AUTH_LENGTH ]=JCM_INSERTION_AUTHORISED_CMD;

                if(true == g_ExactFareFlag )
                {

                       //writeFileLog"[EnableDenomDuringCreditPolling()]  Exact Fare Block Active.");

                       switch(fare)
                       {

                               case 5:
                                      #ifdef JCM_DEBUG 
				      //writeFileLog"[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 5 Enable.");
                                      #endif
				      DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
				      break;
				

				case 10:
                                     #ifdef JCM_DEBUG 
				     //writeFileLog"[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 10 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
				     break;
				

				case 20:
                                     #ifdef JCM_DEBUG 
				     //writeFileLog"[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 20 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
			             break;
				

				case 50 :
                                     #ifdef JCM_DEBUG 
				     //writeFileLog"[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 50 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
				     break;
				

				case 100:
                                    #ifdef JCM_DEBUG 
				    //writeFileLog"[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 100 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
				    break;
				

				case 500:
                                    #ifdef JCM_DEBUG 
				    //writeFileLog"[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 500 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
				    break;
				

				case 1000:
                                    #ifdef JCM_DEBUG 
				    //writeFileLog"[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 1000 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
				    break;
                              

                              };
				


                }
                else
                {
			
                                //writeFileLog"[EnableDenomDuringCreditPolling()]  Exact Fare Block InActive.");
                        	if( fare>=5 )
				{

                                     #ifdef JCM_DEBUG 
				     //writeFileLog"[EnableDenomDuringCreditPolling() Max Cash Block] Rs 5 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
				
				}

				if( fare>=10 )
				{
                                     #ifdef JCM_DEBUG 
                                     //writeFileLog"[EnableDenomDuringCreditPolling() Max Cash Block] Rs 10 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
				  
				}

				if( fare>=20 )
				{
                                     #ifdef JCM_DEBUG 
                                     //writeFileLog"[EnableDenomDuringCreditPolling() Max Cash Block] Rs 20 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
				
				}

				if( fare>=50 )
				{
                                     #ifdef JCM_DEBUG 
                                     //writeFileLog"[EnableDenomDuringCreditPolling() Max Cash Block] Rs 50 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
				  
				}

				if( fare>=100 )
				{
                                    #ifdef JCM_DEBUG 
                                    //writeFileLog"[EnableDenomDuringCreditPolling() Max Cash Block] Rs 100 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
				
				}

				if( fare>=500 )
				{
                                    #ifdef JCM_DEBUG 
                                    //writeFileLog"[EnableDenomDuringCreditPolling() Max Cash Block] Rs 500 Enable.");
				    #endif
                                    DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
				 
				}

				if( fare>= 1000)
				{
                                    #ifdef JCM_DEBUG 
                                    //writeFileLog"[EnableDenomDuringCreditPolling() Max Cash Block] Rs 1000 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
				 
				}


                }

               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    #ifdef JCM_DEBUG

		    memset(log,'\0',100);

                    sprintf(log,"[EnableDenomDuringCreditPolling()] DefaultEnablebyte =0x%xh.",DefaultEnablebyte);

                    //writeFileLoglog);

                    #endif
	
		    if(0x10 == DefaultEnablebyte)
		    {
			    
				    //Set MoneyCode byte
		                    DittoCommands[ 3 ]=DefaultEnablebyte;

				    CmdLength=JCM_CMD_DITTO_INSERTION_AUTH_LENGTH;

				    //Prepare BCC 
				    for(Counter=2;Counter<= (CmdLength-2);Counter++)
				    {
					 if(4 == Counter)
					    continue;
					 else
					    DittoCommands[CmdLength-1]=  DittoCommands[CmdLength-1] ^ DittoCommands[Counter];
				    }

                                    #ifdef JCM_DEBUG 

                                    for(Counter=0;Counter<CmdLength;Counter++)
				    {

					 memset(log,'\0',100);

                                         sprintf(log,"[EnableDenomDuringCreditPolling()] DittoCommands[%d] = 0x%xh.",Counter,DittoCommands[Counter]);
                                         //writeFileLoglog);

				    }

                                    memset(log,'\0',100);

                                    sprintf(log,"[EnableDenomDuringCreditPolling()] DittoComamnd Packet BCC =0x%xh.",DittoCommands[CmdLength-1]);
                                    //writeFileLoglog);
				
				    #endif

			    
		    }
                    else if(0x10!= DefaultEnablebyte)
		    {
                     

				    //Set MoneyCode byte
		                    Commands[ 3 ]=DefaultEnablebyte;

				    CmdLength=JCM_CMD_INSERTION_AUTH_LENGTH;

				    //Prepare BCC 
				    for(Counter=2;Counter<= (CmdLength-2);Counter++)
				    {
					 
					 Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
				    
				    }

                                    #ifdef JCM_DEBUG

				    for(Counter=0;Counter<CmdLength;Counter++)
				    {
					  
                                          memset(log,'\0',100);

                                          sprintf(log,"[EnableDenomDuringCreditPolling()] Commands[%d] = 0x%xh.",Counter,Commands[Counter]);
                                          //writeFileLoglog);

				    }

				
                                    memset(log,'\0',100);

                                    sprintf(log,"[EnableDenomDuringCreditPolling()] Comamnd Packet BCC =0x%xh.",Commands[CmdLength-1]);

                                    //writeFileLoglog);

				    #endif

		    
		    }

                    if(0x10== DefaultEnablebyte)
                    {

                           #ifdef JCM_DEBUG
			   //writeFileLog"[EnableDenomDuringCreditPolling()] Enablebyte 0x10 found.");
                           #endif
                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,DittoCommands,CmdLength);

	            }
	            else if(0x10 != DefaultEnablebyte)
	            {

                            #ifdef JCM_DEBUG
			    //writeFileLog"[EnableDenomDuringCreditPolling()] Enablebyte 0x10 not found.");
                            #endif
			    rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);

		    }


                if( JCM_EXTERNAL_COMMAND_REPLY_READ == flag )
                {


		        //Receieved Reply
		        rtcode = JCM_ReceieveCmdReplySequence( g_HANDLE,
		                                               Response,
		                                               JCM_INSERTION_AUTHORIZING_REPLY_SIZE
		                                             );
		         
		        

		        //Display Enable Denom Packet
                        memset(log,'\0',200);
		        sprintf(log,"[EnableDenomDuringCreditPolling()] Command = 0x%x h",Response[2]);
		        //writeFileLoglog);

		        memset(log,'\0',200);
		        sprintf(log,"[EnableDenomDuringCreditPolling()] State = 0x%x h",Response[3]);
		        //writeFileLoglog);

		        
                        #ifdef JCM_DEBUG

		        int i=0;
		        for(i=0;i<JCM_INSERTION_AUTHORIZING_REPLY_SIZE;i++)
		        {

		             
                             memset(log,'\0',200);
		             sprintf(log,"[EnableDenomDuringCreditPolling()] Response[%d]= 0x%x h",i,Response[i]);
		             //writeFileLoglog);


		        }

		        #endif

		       if(   ( ( 0x50 == Response[2] ) && ( 0x03 == Response[3] ) )  ||
		             ( ( 0x50 == Response[2] ) && ( 0x13 == Response[3] ) )  ||
                             ( ( 0x50 == Response[2] ) && ( 0x05 == Response[3] ) )  ||
                             ( ( 0x50 == Response[2] ) && ( 0x08 == Response[3] ) ) 
		          )
		        {
		             //writeFileLog"[EnableDenomDuringCreditPolling()] Enable Denom Success ."); 
		             return 1;
		        
		        }
		        else
		        {
		              //writeFileLog"[EnableDenomDuringCreditPolling()] Enable Denom not possible due to no matching with any denomination."); 
		              return 0;

		        }

               }
               else if( JCM_INTERNAL_COMMAND_REPLY_READ == flag)
               {

                    ////writeFileLog"[EnableDenomDuringCreditPolling()] Enable Denom recv not read due to flag not set it will be read any other recv function ."); 
                    return 1;


               }                


}//EnableDenomDuringCreditPolling(int fare,int flag) end
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Make reset JCM
int JCM_Reset(){
 
            
                unsigned char Response[100];
                memset(Response,'\0',100);
                unsigned char log[200];
                memset(log,'\0',200);
                unsigned char Commands[ JCM_CMD_CMN_LENGTH ]=JCM_RESET_CMD;
                int Counter=0;
                int rtcode=0;
                int CmdLength = JCM_CMD_CMN_LENGTH;
                
                //Create checksum value for command byte
                for(Counter=2;Counter<= (CmdLength-2);Counter++){
					Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
	            }//for end
             
               //Step 1:Transmit Command
               rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,JCM_CMD_CMN_LENGTH);
               if(1==rtcode){
				   
                            //wait for reset operation done [Patch]
		                    ////writeFileLog"[JCM_Reset()]  Waiting for enq byte for reset reply ");
		                    AVRM_writeFileLogV2("[JCM_Reset()]  Waiting for enq byte for reset reply ",DEBUG,g_BNAId);
					        unsigned char rbyte=0x00;
                            int totalByteIn=0,counter=1;
                            while(1) {
                                 
                                  rbyte=0x00;
                                  totalByteIn=0;
                                  //wait for enq byte
                                  BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&rbyte,&totalByteIn);
                                  //memset(log,'\0',200);
                                  //sprintf(log,"[JCM_Reset()]  rbyte = 0x%xh",rbyte);
					              ////writeFileLoglog);

 
                                  if( ( rbyte > 0x00 ) && ( 0x05 == rbyte ) ){
                          
                                       ////writeFileLog"[JCM_Reset()]  Found enq byte for reset reply ");
                                       AVRM_writeFileLogV2("[JCM_Reset()]  Found enq byte for reset reply ",DEBUG,g_BNAId);
                                       DelayInSeconds( 1 );
                                       break;

                                  }//if end
                                  
                                  
                                  if( counter > JCM_ENQ_WAIT_FOR_RESET ){
                                       ////writeFileLog"[JCM_Reset()]  Unable to found enq byte for reset reply .");
                                       AVRM_writeFileLogV2("[JCM_Reset()]  Unable to found enq byte for reset reply .",DEBUG,g_BNAId);
                                       return 0;
                                  }//if end

                                  //Then delay for 1 second
                                  DelayInSeconds( 1 );
                                  counter++;

                            }//while end


                            ///////////////////////////////////////////////////////////////////////

							rtcode=-1;

							//Step 2:Receieve Reply
                            rtcode=JCM_ReceieveCmdReplySequence(g_HANDLE,Response,JCM_RESET_REPLY_SIZE);

							if(1!=rtcode){

										 ////writeFileLog"[JCM_Reset()] Receieve sequence failed.");
                                         AVRM_writeFileLogV2("[JCM_Reset()] Receieve sequence failed.",DEBUG,g_BNAId);
										//#ifdef JCM_DEBUG  
			 
										//Display Reset Response Packet
										//for(Counter=0;Counter<JCM_RESET_REPLY_SIZE;Counter++)
										//{

											//memset(log,'\0',200);

											//sprintf(log,"[JCM_Reset()] Response[%d] = 0x%xh",Counter,Response[Counter]);
											  
											////writeFileLoglog);

										//}
										   
										//#endif

										return 0; //recv sequence failed
								  
							}//if end
			 
				}else{
							 ////writeFileLog"[JCM_Reset()] Transmission sequence failed.");
							 AVRM_writeFileLogV2("[JCM_Reset()] Transmission sequence failed.",DEBUG,g_BNAId);
							 return 0; //transmit sequence failed
				} //else end

               
                 //#ifdef JCM_DEBUG  
	 
	             //Display Reset Response Packet
				 //for(Counter=0;Counter<JCM_RESET_REPLY_SIZE;Counter++){
					  //memset(log,'\0',200);
					  //sprintf(log,"[JCM_Reset()] Response[%d] = 0x%xh",Counter,Response[Counter]);
					  ////writeFileLoglog);

				 //}//for end
				       
		         //#endif

                //check reply byte checksum value
                int replybytechecksum=0x00;
                for(Counter=2;Counter<= (JCM_RESET_REPLY_SIZE-2);Counter++){
				   replybytechecksum =  replybytechecksum ^ Response[Counter];
				}//for end

                ////////////////////////////////////////////////////////////////////////////

                //Log Reply byte checksum
                //memset(log,'\0',200);
                //sprintf(log,"[JCM_Reset()] replybytechecksum = 0x%xh.  Response[%d]= 0x%xh .",replybytechecksum,JCM_RESET_REPLY_SIZE-1,Response[JCM_RESET_REPLY_SIZE-1]);
		        ////writeFileLoglog);

                ////////////////////////////////////////////////////////////////////////////
                                                                                                                                     
                if(Response[JCM_RESET_REPLY_SIZE-1]!= replybytechecksum ){
                    ////writeFileLog"[JCM_Reset()] Reply byte checksum is not matched");
                    AVRM_writeFileLogV2("[JCM_Reset()] Reply byte checksum is not matched",DEBUG,g_BNAId);
                    return 0;
                    
                }//if end

                if(Response[3]>0x00) {


                                  //++Log Current State
                                  memset(log,'\0',200);
                                  sprintf(log,"[JCM_Reset()] Current State: 0x%xh",Response[3]);
		                          ////writeFileLoglog);
		                          
		                          AVRM_writeFileLogV2(log,DEBUG,g_BNAId);

		                          if( 0x0D == Response[3] ) {
										////writeFileLog"[JCM_Reset()] Found Standby with escrow notes State.");
                                        AVRM_writeFileLogV2("[JCM_Reset()] Found Standby with escrow notes State.",INFO,g_BNAId);
									    return 1;
								  }
                                  else if( 0x02 == Response[3] ) 
                                  {
                                        ////writeFileLog"[JCM_Reset()] Found Standby without escrow notes State.");
                                        AVRM_writeFileLogV2("[JCM_Reset()] Found Standby without escrow notes State.",INFO,g_BNAId);
					                    return 1;

                                  }
                                  else if( 0x81 == Response[3] ) 
                                  {
                                        ////writeFileLog"[JCM_Reset()] Found Alarm State.");
                                        AVRM_writeFileLogV2("[JCM_Reset()] Found Alarm State.",INFO,g_BNAId);
					                    return 0;
			 
			                      }// else if( 0x81 == Response[3] )
                                  else if ( 0x80 == Response[3] ) 
                                  {
                                        ////writeFileLog"[JCM_Reset()] Found Sense trouble State.");
                                        AVRM_writeFileLogV2("[JCM_Reset()] Found Sense trouble State.",INFO,g_BNAId);
					                    return 0;
			 
			                      }// else if( 0x81 == Response[3] ) block


                }//if(Response[3]>0x00) block
                else
                {
                    ////writeFileLog"[JCM_Reset()] unknown jcm state found.");
                    AVRM_writeFileLogV2("[JCM_Reset()] unknown jcm state found.",INFO,g_BNAId);
                    return 0;

                }//else end
                

}//JCM_Reset() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Reinit After Com Port Open
int JCM_InitAfterPortOpen() {
           

       if( -1 != g_HANDLE ) {

	       

                                          int rtcode=-1;

                                          unsigned char Response[100];

                                          memset(Response,'\0',100);

                                          unsigned char log[200];

                                          memset(log,'\0',200);

	                                      //++//writeFileLog"[JCM_InitAfterPortOpen()] Before Going to issue sense command.");
	                                      AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Before Going to issue sense command.",INFO,g_BNAId);

		                                  rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                          //++//writeFileLog"[JCM_InitAfterPortOpen()] After issued sense command.");
                                          AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] After issued sense command.",INFO,g_BNAId);

		                                  if( ( Response[3] >= 0 ) &&  ( 0x40 == Response[2] ) ) {
					                 
														 #ifdef JCM_DEBUG 

														 //int i=0;

														 //for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   {
								                               //memset(log,'\0',200);
															   //sprintf(log,"[JCM_InitAfterPortOpen()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
															   ////writeFileLoglog);
			 
														 //}//for end
                                                        
                                                         #endif

                                                         //++Log Current State 
                                                         memset(log,'\0',200);
                                                         sprintf(log,"[JCM_InitAfterPortOpen()] Current State = 0x%xh. ",Response[3]);
		                                                 //++//writeFileLoglog);
		                                                 AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                                 
                                                         //Log Current Sensor State
                                                         //++//writeFileLog"[JCM_InitAfterPortOpen()] Start Record Current JCM Sensor State.");
                                                         //++JCM_LogSensorState(Response[7],Response[8],Response[9]);
                                                         //++//writeFileLog"[JCM_InitAfterPortOpen()] End Record Current JCM Sensor State.");

                                                         switch(Response[3]) {
                                                             
                                                                     //reset found after power up  
		                                                     case 0x00:////writeFileLog"[JCM_InitAfterPortOpen()] Found After Power on state.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found After Power on state.",INFO,g_BNAId);
                                                                       //issue reset command 
                                                                       return ( JCM_Reset() );

		                                                               break;

		                                                     //reset found after power up  
		                                                     case 0x01:////writeFileLog"[JCM_InitAfterPortOpen()] Found In Reset State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found In Reset State.",INFO,g_BNAId);
                                                                       return 1;
		                                                               break;

		                                                     //normal state
		                                                     case 0x02:////writeFileLog"[JCM_InitAfterPortOpen()] Found Standby without note present in escrow State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found Standby without note present in escrow State.",INFO,g_BNAId);
                                                                       return 1;                          
		                                                               break;

		                                                     //Disable here
		                                                     case 0x03:////writeFileLog"[JCM_InitAfterPortOpen()] Found  Enable without note present in escrow State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found  Enable without note present in escrow State.",INFO,g_BNAId);
                                                                       return ( DisableAcceptance() );
		                                                               break;

		                                                     //normal state with notes in escrow
		                                                     case 0x12:////writeFileLog"[JCM_InitAfterPortOpen()] Found Standby with note present in escrow State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found Standby with note present in escrow State.",INFO,g_BNAId);
                                                                       return 1;
		                                                               break;

		                                                     //Disable here
		                                                     case 0x13:        ////writeFileLog"[JCM_InitAfterPortOpen()] Found  Enable with note present in escrow State.");
		                                                                       AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found  Enable with note present in escrow State.",INFO,g_BNAId);
                                                                               
                                                                               //Make Disable it and accept all money to freevault
                                                                               ////writeFileLog"[JCM_InitAfterPortOpen()] No going to first inhibit note acceptor");
                                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] No going to first inhibit note acceptor",INFO,g_BNAId);
                                                                               if( 1 == DisableAcceptance() ){

                                                                                    ////writeFileLog"[JCM_InitAfterPortOpen()] inhibit note acceptor successfully done.");
                                                                                    ////writeFileLog"[JCM_InitAfterPortOpen()] Now going to accept all escrow money to vault.");
                                                                                    AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] inhibit note acceptor successfully done.",INFO,g_BNAId);
                                                                                    AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Now going to accept all escrow money to vault.",INFO,g_BNAId);
                                                                                    
                                                                                    //if(1==JCM_DefaultCommit()){
                                                                                        ////writeFileLog"[JCM_InitAfterPortOpen()] Accept escrow money successfully done."); 
                                                                                        //AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Accept escrow money successfully done.",INFO,g_BNAId);
                                                                                        //return 1;
                                                                                    //}else{
                                                                                        ////writeFileLog"[JCM_InitAfterPortOpen()] Accept escrow money failed.");
                                                                                        //AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Accept escrow money failed.",INFO,g_BNAId);
                                                                                        //return 0;
                                                                                    //}//else end
                                                                                    
                                                                               }else{
                                                                                        ////writeFileLog"[JCM_InitAfterPortOpen()] Inhibit note acceptor failed.");
                                                                                        AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Inhibit note acceptor failed.",INFO,g_BNAId);
                                                                                        return 0;

                                                                               } //else end

		                                                               break;

		                                                     //Sense trouble
		                                                     case 0x80:////writeFileLog"[JCM_InitAfterPortOpen()] Found  Sense Trouble State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found  Sense Trouble State.",INFO,g_BNAId);
                                                                       return 0;
		                                                               break;

		                                                     //Jam or Alarm State reset here
		                                                     case 0x81:////writeFileLog"[JCM_InitAfterPortOpen()] Found Alarm/Jamming State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found Alarm/Jamming State.",INFO,g_BNAId);
                                                                       return(JCM_ReInit(30));
		                                                               break;

		                                                     //Wait for Rejection state complete 
		                                                     case 0x09:////writeFileLog"[JCM_InitAfterPortOpen()] Found Rejection State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found Rejection State.",INFO,g_BNAId);
                                                                       return 1;
		                                                               break;

		                                                     //Wait for Return state complete
		                                                     case 0x0A:////writeFileLog"[JCM_InitAfterPortOpen()] Found Wait For return State.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found Wait For return State.",INFO,g_BNAId);
                                                                       return 1;
		                                                               break;

                                                                     //Remaining notes escrow state
		                                                     case 0x0D:////writeFileLog"[JCM_InitAfterPortOpen()] Found Remaining notes in escrow state.");
		                                                               AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] Found Remaining notes in escrow state.",INFO,g_BNAId);
                                                                       return 1;
                                                                       break;


		                                                     //No Suitable State found
		                                                     default:////writeFileLog"[JCM_InitAfterPortOpen()] No State Found.");
		                                                             AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] No State Found.",INFO,g_BNAId);
                                                                     return 0;
                                                                     break;


                                                         };//switch end

                                                         
							    	                     
					 }else{
						
                                                  //Log Current JCM Reply Data
                                                  //int i=0;
                                                  //for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   {
								                       //memset(log,'\0',200);
													   //sprintf(log,"[JCM_InitAfterPortOpen()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
													   ////writeFileLoglog);
			                                      //}//for end
                                                 ////writeFileLog"[JCM_InitAfterPortOpen()] JCM is not reply against sense command.");
                                                 AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] JCM is not reply against sense command.",FATAL,g_BNAId);
		                                         return 0;


					 }//else end
	   }else{
	        ////writeFileLog"[JCM_InitAfterPortOpen()] JCM port open not done.");
	        AVRM_writeFileLogV2("[JCM_InitAfterPortOpen()] JCM port open not done.",FATAL,g_BNAId);
            return 0;

	   }//else end
           
}//JCM_InitAfterPortOpen() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_AcceptEscrowMoney() {

            int rtcode=-1;
            ////writeFileLog"[JCM_AcceptEscrowMoney()] Found Remaining notes state.");
            ////writeFileLog"[JCM_AcceptEscrowMoney)] Before going for Accept escrow notes.");
            AVRM_writeFileLogV2("[JCM_AcceptEscrowMoney()] Found Remaining notes state.",INFO,g_BNAId);
            AVRM_writeFileLogV2("[JCM_AcceptEscrowMoney)] Before going for Accept escrow notes.",INFO,g_BNAId);
            rtcode = JCM_Receipt();
            ////writeFileLog"[JCM_AcceptEscrowMoney()] After Accept escrow notes");
            AVRM_writeFileLogV2("[JCM_AcceptEscrowMoney()] After Accept escrow notes",INFO,g_BNAId);
            if(1 == rtcode ){
		         ////writeFileLog"[JCM_AcceptEscrowMoney()] Accept escrow notes at startup successfully.");
		         AVRM_writeFileLogV2("[JCM_AcceptEscrowMoney()] Accept escrow notes at startup successfully.",INFO,g_BNAId);
				 return 1;

            }else{
				 ////writeFileLog"[JCM_AcceptEscrowMoney()] Accept escrow notes at startup failed.");
				 AVRM_writeFileLogV2("[JCM_AcceptEscrowMoney()] Accept escrow notes at startup failed.",ERROR,g_BNAId);
				 return 0;
		    }//else end

}//JCM_AcceptEscrowMoney() end

//commit notes at startup
int JCM_DefaultCommit() {

       if( -1 != g_HANDLE ){

	       

                                          int rtcode=-1;

                                          unsigned char Response[100];

                                          memset(Response,'\0',100);

                                          unsigned char log[200];

                                          memset(log,'\0',200);

	                                      //writeFileLog"[JCM_DefaultCommit()] Before Going to issue sense command.");

		                                  rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                          //writeFileLog"[JCM_DefaultCommit()] After issued sense command.");

                                          int replychecksum=0x00;

                                          replychecksum=GetBCC(Response,2,JCM_SENSE_REPLY_SIZE);

                                          //Log reply byte checksum value
                                          memset(log,'\0',200);

										  sprintf(log,"[JCM_DefaultCommit()] Response[%d] = 0x%xh. replychecksum =0x%xh ",JCM_SENSE_REPLY_SIZE-1,Response[JCM_SENSE_REPLY_SIZE-1],replychecksum);

										  //writeFileLoglog);
	 

                                          if( replychecksum !=  Response[JCM_SENSE_REPLY_SIZE -1])
                                          {

                                                //writeFileLog"[JCM_DefaultCommit()] Reply byte checksum is not matched.");                                 
                                                int i=0;

												for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   {
								
													  memset(log,'\0',200);

													  sprintf(log,"[JCM_DefaultCommit()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
													  //writeFileLoglog);
			 
												}
                                                        
                                                
                                          } 

		                          if( ( Response[3] >= 0 ) &&  ( 0x40 == Response[2] ) )
					              {
					                 
                                                         //#ifdef JCM_DEBUG 

				                         int i=0;

		                                         for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                         {
						
		                                               memset(log,'\0',200);

		                                               sprintf(log,"[JCM_DefaultCommit()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                               //writeFileLoglog);
	 
		                                         }
                                                        
                                                         //#endif

                                                         //Log Current State 
                                                         memset(log,'\0',200);

		                                         sprintf(log,"[JCM_DefaultCommit()] Current State = 0x%xh. ",Response[3]);
		                                         //writeFileLoglog);
                                                      
                                                         
                                                         switch( Response[3]  )
                                                         {


                                                                 ///////////////////////////////////////////////

                                                                 case 0x12:
		                                                 case 0x0D: return (JCM_AcceptEscrowMoney() );
		                                                            break;

                                                                 case 0x13: if( 1 == DisableAcceptance() )
                                                                            {
                                                                                return (JCM_AcceptEscrowMoney() );
                                                                            }
                                                                            else
                                                                            {
                                                                                return 0;

                                                                            }
		                                                            break;

                                                                 ///////////////////////////////////////////////


		                                                 default:
		                                                      //writeFileLog"[JCM_DefaultCommit()] Not Found Remaining notes state.");
		                                                
		                                                      return 0;

                                                                      break;

                                                           
                                                  }; //switch end
  
							    	                     
					  }
		                          else
					  {
						
                                                  //Log Current JCM Reply Data
                                                  int i=0;

		                                  for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                  {
						
		                                        memset(log,'\0',200);

		                                        sprintf(log,"[JCM_DefaultCommit()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                        //writeFileLoglog);
	 
		                                  }
                                 
		                                  //writeFileLog"[JCM_DefaultCommit()] JCM is not reply against sense command.");

		                                  return 0;


					  }


                             


	   }
	   else
	   {
	            //writeFileLog"[JCM_DefaultCommit()] JCM port open not done.");

                return 0;

	   }
     
}//JCM_DefaultCommit() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_RejectEscrowMoney() {


              int rtcode=-1;
              
              unsigned char returnstate=0x00;

	      //writeFileLog"[JCM_DefaultCancel()] Found Remaining notes state.");
		                                                     
              //writeFileLog"[JCM_DefaultCancel()] Before going for Return escrow notes.");

              rtcode = JCM_Return(&returnstate);

              //writeFileLog"[JCM_DefaultCancel()] After Return escrow notes");
	
              if( 0x0A == returnstate )
              {
		     //writeFileLog"[JCM_DefaultCancel()] Return escrow notes at startup successfully but user not taken notes from outlet.");
		     
                     return 1;

	      }
	      else if( 0x02 == returnstate )
              {
		     //writeFileLog"[JCM_DefaultCancel()] Return escrow notes at startup successfully and user also taken notes from outlet.");

		     return 1;

	      }
	      else if( 0x81 == returnstate )
              {
		     //writeFileLog"[JCM_DefaultCancel()] Return escrow notes at startup failed due to alarm state .");

		     return 0;

	      }
	      else
	      {
		     //writeFileLog"[JCM_DefaultCancel()] Return escrow notes at startup failed due to unknown reason.");
		     return 0;

              }

  
}//JCM_RejectEscrowMoney() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Return note at startup
int JCM_DefaultCancel(){


           if( -1 != g_HANDLE )
	   {

	       

                                          int rtcode=-1;

                                          unsigned char Response[100];

                                          memset(Response,'\0',100);

                                          unsigned char log[200];

                                          memset(log,'\0',200);

	                                  //writeFileLog"[JCM_DefaultCancel()] Before Going to issue sense command.");

		                          rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                          //writeFileLog"[JCM_DefaultCancel()] After issued sense command.");

                                          int replychecksum=0x00;

                                          replychecksum=GetBCC(Response,2,JCM_SENSE_REPLY_SIZE);

                                          //Log reply byte checksum value
                                          memset(log,'\0',200);

		                          sprintf(log,"[JCM_DefaultCancel()] Response[%d] = 0x%xh. replychecksum =0x%xh ",JCM_SENSE_REPLY_SIZE-1,Response[JCM_SENSE_REPLY_SIZE-1],replychecksum);

		                          //writeFileLoglog);
	 

                                          if( replychecksum !=  Response[JCM_SENSE_REPLY_SIZE -1])
                                          {

                                                //writeFileLog"[JCM_DefaultCancel()] Reply byte checksum is not matched.");
                                                return 0;
                                          }

		                          if( ( Response[3] >= 0 ) &&  ( 0x40 == Response[2] ) )
					  {
					                 
                                                         //#ifdef JCM_DEBUG 

				                         int i=0;

		                                         for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                         {
						
		                                               memset(log,'\0',200);

		                                               sprintf(log,"[JCM_DefaultCancel()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                               //writeFileLoglog);
	 
		                                         }
                                                        
                                                         //#endif

                                                         //Log Current State 
                                                         memset(log,'\0',200);

		                                         sprintf(log,"[JCM_DefaultCancel()] Current State = 0x%xh. ",Response[3]);
		                                         //writeFileLoglog);

                                                         switch( Response[3]  )
                                                         {


                                                                 ///////////////////////////////////////////////

                                                                 case 0x12:
		                                                 case 0x0D: return (JCM_RejectEscrowMoney() );
		                                                            break;

                                                                 case 0x13: if( 1 == DisableAcceptance() )
                                                                            {
                                                                                return (JCM_RejectEscrowMoney() );
                                                                            }
                                                                            else
                                                                            {
                                                                                return 0;

                                                                            }
		                                                            break;

                                                                 ///////////////////////////////////////////////


		                                                 default:
		                                                      //writeFileLog"[JCM_DefaultCancel()] Not Found Remaining notes state.");
		                                                
		                                                      return 0;

                                                                      break;

                                                           
                                                         }; //switch end

                                                       
							    	                     
					  }
		                          else
					  {
						
                                                  //Log Current JCM Reply Data
                                                  int i=0;

		                                  for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                  {
						
		                                        memset(log,'\0',200);

		                                        sprintf(log,"[JCM_DefaultCancel()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                        //writeFileLoglog);
	 
		                                  }
                                 
		                                  //writeFileLog"[JCM_DefaultCancel()] JCM is not reply against sense command.");

		                                  return 0;


					  }


                             


	   }
	   else
	   {
	        //writeFileLog"[JCM_DefaultCancel()] JCM port open not done.");

                return 0;

	   }
     




}//JCM_DefaultCancel() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_Activate(const int ComPortno){
	  
           
           g_HANDLE=-1;

	       int  rtcode=-1;

           //////////////////////////////////////////////////////

           //++Now store com port numnber
           g_JcmPortNumber=ComPortno;
   
           //////////////////////////////////////////////////////

           rtcode=BNA_OpenPort(ComPortno, &g_HANDLE);
           
		   if( 1 == rtcode){
			   
	            ////writeFileLog"[JCM_Activate()] JCM Note Acceptor Open Successfully.");
	            AVRM_writeFileLogV2("[JCM_Activate()] JCM Note Acceptor Open Successfully.",INFO,g_BNAId);
                rtcode=-1;
                rtcode=JCM_InitAfterPortOpen();
                if( 1 == rtcode)
                {
                    ////writeFileLog"[JCM_Activate()] JCM Note Acceptor Init Successfully Done.");
                    AVRM_writeFileLogV2("[JCM_Activate()] JCM Note Acceptor Init Successfully Done.",INFO,g_BNAId);
                    return 1;
                }else{ 
                    ////writeFileLog"[JCM_Activate()] JCM Note Acceptor Init Failed to Done.");
                    ////writeFileLog"[JCM_Activate()] Now going to close JCM Note Acceptor ComPort.");
                    AVRM_writeFileLogV2("[JCM_Activate()] JCM Note Acceptor Init Failed to Done.",FATAL,g_BNAId);
                    AVRM_writeFileLogV2("[JCM_Activate()] Now going to close JCM Note Acceptor ComPort.",INFO,g_BNAId);
                    JCM_Deactivate();
                    return 0;    
                }//else end
                
		   }else{
			    AVRM_writeFileLogV2("[JCM_Activate()] JCM Note Acceptor Open failed!!.",FATAL,g_BNAId);
				//++//writeFileLog"[JCM_Activate()] JCM Note Acceptor Open failed!!.");
			    return 0;
		   }//else end
           
          


}//JCM_Activate(const int ComPortno) end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_Deactivate(){
     
	   
       int  rtcode=-1;
	   rtcode=BNA_ClosePort(g_HANDLE);
	   if( 1 == rtcode){
	        g_HANDLE=-1;
            ////writeFileLog"[JCM_Deactivate()] JCM com port successfully released.");
            AVRM_writeFileLogV2("[JCM_Deactivate()] JCM com port successfully released.",INFO,g_BNAId);
            return 1;
	   }else{
            ////writeFileLog"[JCM_Deactivate()] JCM com port  failed to closed.");
            AVRM_writeFileLogV2("[JCM_Deactivate()] JCM com port  failed to closed.",ERROR,g_BNAId);
	        return 0;
	   }//else end

}//end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_LogSensorState(unsigned char Sensor_first,unsigned char Sensor_second,unsigned char Sensor_third){

             
             JCM_SENSOR_STATE sensorstate; 
             unsigned char log[200];
             memset(log,'\0',200);
           

             //Log Sensor one
             sensorstate.sensor_validation_ptc=  Sensor_first & 0b00000001;

             sensorstate.sensor_validation_ptr = ((Sensor_first & 0b00000010)>>1);

             sensorstate.sensor_acceptor_lid_detection = ((Sensor_first & 0b00100000)>>5);

             sensorstate.sensor_insertion_detection= ((Sensor_first & 0b10000000)>>7);

             switch( sensorstate.sensor_validation_ptc )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTC NO Note");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTC With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTC unknown status");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

             switch( sensorstate.sensor_validation_ptr )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTR NO Note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTR With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTR unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

             switch( sensorstate.sensor_acceptor_lid_detection )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Acceptor lid detection Sensor closed.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Acceptor lid detection Sensor open.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Acceptor lid detection Sensor  unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

             switch( sensorstate.sensor_insertion_detection )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion detection Sensor No Note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Insertion detection Sensor With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion detection Sensor unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

             //Log Sensor two
             sensorstate.sensor_validation_sensor_ptl=((Sensor_second & 0b00000010)>>1);

             sensorstate.sensor_outlet_shutter=((Sensor_second & 0b01000000)>>6);

             sensorstate.sensor_freefall=((Sensor_second & 0b10000000)>>7);

             switch( sensorstate.sensor_validation_sensor_ptl )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTL No Note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTL With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTL unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

             switch( sensorstate.sensor_outlet_shutter )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Outlet Shutter Sensor Closed.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Outlet Shutter Sensor Opened.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Outlet Shutter Sensor unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

            
             switch( sensorstate.sensor_freefall )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Free Fall Sensor No Note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Free Fall Sensor With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Free Fall Sensor unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };


             //Log Sensor three
             sensorstate.sensor_position_s13=(Sensor_third & 0b00000001);

             sensorstate.sensor_position_s14=((Sensor_third & 0b00000010)>>1);

             sensorstate.sensor_position_s15=((Sensor_third & 0b00000100)>>2);

             sensorstate.sensor_stacking_control=((Sensor_third & 0b00010000)>>4);

             sensorstate.sensor_insertion_detection_right=((Sensor_third & 0b10000000)>>7);

             switch( sensorstate.sensor_position_s13 )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S13 Position Sensor No Note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] S13 Position Sensor With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S13 Position Sensor unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

             switch( sensorstate.sensor_position_s14 )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S14 Position Sensor No Note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] S14 Position Sensor With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S14 Position Sensor unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };
             
             switch( sensorstate.sensor_position_s15 )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S15 Position Sensor No Note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] S15 Position Sensor With Note.");
		             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S15 Position Sensor unknown status.");
		           AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };
               
             switch( sensorstate.sensor_stacking_control )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Stacking Control Sensor Normal Position.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Stacking Control Sensor UnNormal Position.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Stacking Control Sensor unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };

             switch( sensorstate.sensor_insertion_detection_right )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion Detection Sensor(Right) No note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Insertion Detection Sensor(Right) with note.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion Detection Sensor(Right) unknown status.");
		            AVRM_writeFileLogV2(log,INFO,g_BNAId);
                            break;

             };




}//JCM_LogSensorState() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_ReInit(int WaitSecond){


           
            int rtcode=-1;
            unsigned char Response[100];
            memset(Response,'\0',100);
            unsigned char log[200];
            memset(log,'\0',200);
            int Counter=0;

            //issue reset command

	    rtcode = JCM_Cmd_Reset(g_HANDLE); 
 
            //now wait for reset response
            struct timespec start,end,diff;

            //Start Timer
            clock_gettime(CLOCK_MONOTONIC, &start);

	    for(;;)
            {        
			
                        //JCM_RSP_REPLY_SIZE JCM_RESET_REPLY_SIZE
                        JCM_ReceieveCmdReplySequence( g_HANDLE, Response , JCM_RESET_REPLY_SIZE  ); 

                        if( Response[3]> 0x00 )
                        {
		                     
                                     #ifdef JCM_DEBUG

		                     for( Counter=0; Counter< JCM_RESET_REPLY_SIZE ; Counter++)
				     {
				           memset(log,'\0',200);
		                           sprintf(log,"[JCM_ReInit()] Response[%d]= 0x%xh",Counter,Response[Counter]);
		                           //writeFileLoglog);
				     }

                                     #endif

                                     memset(log,'\0',200);
		                     sprintf(log,"[JCM_ReInit()] Command = 0x%xh State = 0x%xh .",Response[2],Response[3]);
		                     AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                           
                        }

                        if( (0x02 == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset Success in %d Second with standby state found.",diff.tv_sec);
		               AVRM_writeFileLogV2(log,INFO,g_BNAId);
                               return 1;

                        }    

                        else if( (0x0D == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset Success in %d Second with Remaining state found.",diff.tv_sec);
		               AVRM_writeFileLogV2(log,INFO,g_BNAId);
                               return 3;

                        }      

                        else if( (0x80 == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset failed in %d Second with sense trouble state found.",diff.tv_sec);
		               AVRM_writeFileLogV2(log,INFO,g_BNAId);
                               return 3;

                        } 

                        else if( (0x81 == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset failed in %d Second with Alarm state found.",diff.tv_sec);
		               AVRM_writeFileLogV2(log,INFO,g_BNAId);
                               return 4;

                        }                             
                       
                        //Check timer status
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        diff.tv_sec = end.tv_sec - start.tv_sec;

                        if( diff.tv_sec >= WaitSecond )
                        {

                                memset(log,'\0',200);
		                sprintf(log,"[JCM_ReInit()] Reset Timeout in %d Second .",diff.tv_sec);
		                AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                return 2;
		                   
                        
                        }



           }// End for(;;)


	 
		                                                    

}//JCM_ReInit() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetNoteDetailsInRecycleCst( int *CastQuanity ) {

             
       *CastQuanity=-1;

       if( -1 != g_HANDLE ) {

	                                      unsigned char Wait_State[4]={0x02,0x03,0x12,0x13};

                                          unsigned int  Wait_State_Length=4; 

                                          unsigned char StateFound=0x00;

                                          //Wait for 5 Second if any enq byte recv from jcm
                                          //if(  1 == ReadEnqFromJcm() )
                                          //{
                                                ////writeFileLog"[GetNoteDetailsInRecycleCst()] Enq Byte receieved from jcm.");
                                                ////writeFileLog"[GetNoteDetailsInRecycleCst()] Before go for wait jcm idle session.");
                                                //WaitForState( 5,Wait_State,Wait_State_Length, &StateFound);
                                                ////writeFileLog"[GetNoteDetailsInRecycleCst()] After go for wait jcm idle session.");
                                          //}

                                          int rtcode=-1;

                                          unsigned char Response[100];
                                          memset(Response,'\0',100);

                                          unsigned char log[200];
                                          memset(log,'\0',200);

	                                      //++issue sense command
		                                  rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

		                          
										  if( ( Response[3] > 0x00 ) && (0x40 == Response[2]) ) {
					                 
                                                         #ifdef JCM_DEBUG 

														 //int i=0;

														 //for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   {
							                               //memset(log,'\0',200);
                                                           //sprintf(log,"[GetNoteDetailsInRecycleCst()] Sense Response[%d] = 0x%x h. ",i,Response[i]);
														   ////writeFileLoglog);
		 
														 //}//for end
                                                        
                                                         #endif

                                                         //++Log Current State 
                                                         memset(log,'\0',200); 
                                                         sprintf(log,"[GetNoteDetailsInRecycleCst()] Current State = 0x%xh. ",Response[3]);
		                                                 ////writeFileLoglog);
		                                                 AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                    
                                                         //Log Current Sensor State
                                                         //++JCM_LogSensorState(Response[7],Response[8],Response[9]);

                                                         //calculate escrow notes
                                                                 
				                                         int totalnotes=0;

                                                         totalnotes=GetEscrowMoneyPatchV2(Response,10);

							                             *CastQuanity=totalnotes;

                                                         memset(log,'\0',200);

			                                             sprintf(log,"[GetNoteDetailsInRecycleCst()] Total Escrow Notes= %d",   totalnotes);

						                                 ////writeFileLoglog);
						                                 
						                                 AVRM_writeFileLogV2(log,INFO,g_BNAId);

                                                         return 1;

										  }else {
						
                                                  
		                                          ////writeFileLog"[GetNoteDetailsInRecycleCst()] JCM is not reply against sense command.");
		                                          AVRM_writeFileLogV2("[GetNoteDetailsInRecycleCst()] JCM is not reply against sense command.",ERROR,g_BNAId);
                                                  //int i=0;
                                                  //for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)  {
									
														   //memset(log,'\0',200);
                                                           //sprintf(log,"[GetNoteDetailsInRecycleCst()] Sense Response[%d] = 0x%x h. ",i,Response[i]);
														   ////writeFileLoglog);
				 
												  //}//for end
                                                  return 0;


					                       }//else end

	   }else {
	            ////writeFileLog"[GetNoteDetailsInRecycleCst()] JCM port open not done.");
	            AVRM_writeFileLogV2("[GetNoteDetailsInRecycleCst()] JCM port open not done.",ERROR,g_BNAId);
                return 0;

	   }//else end
           
}//GetNoteDetailsInRecycleCst() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Get Curent JCM status
int JCM_GetStatus() {
           

              
       if( -1 != g_HANDLE )
	   {

	       
                                   
                                         unsigned char Wait_State[4] ={0x02,0x03,0x12,0x13};

                                         unsigned int  Wait_State_Length=4; 

                                         unsigned char StateFound=0x00;

                                         int rtcode=-1;

                                         unsigned char Response[100];

                                         memset(Response,'\0',100);

                                         unsigned char log[200];

                                         memset(log,'\0',200);

	                         
                                         //issue sense command
		                         rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                         int replychecksum=0x00;

                                         replychecksum=GetBCC(Response,2,JCM_SENSE_REPLY_SIZE);

                                         if(replychecksum!=Response[JCM_SENSE_REPLY_SIZE-1])
                                         {

                                             //writeFileLog"[JCM_GetStatus()] Not matched with reply byte checksum value");


                                         }
		                          
		                         if( Response[3] >= 0x00 )
					 {

					                  
                                                         //#ifdef JCM_DEBUG

                                                         //writeFileLog"=========================================================================");

				                         int i=0;

		                                         for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                         {
						
		                                               //memset(log,'\0',200);
                                                               //sprintf(log,"[JCM_GetStatus()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                               ////writeFileLoglog);

	 
		                                          }

                                                          //writeFileLog"=========================================================================");

                                                          //#endif  

                                                          memset(log,'\0',200);
                                                          sprintf(log,"[JCM_GetStatus()] Sense Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);
		                                          //writeFileLoglog);

                                                          //Log Current JCM Sensor Status
                                                          //++JCM_LogSensorState(Response[7],Response[8],Response[9]);


                                                          if(   (0x40 == Response[2]) && 
                                                                (0x00 == Response[3]) 
                                                             
                                                            )
                                                          {

                                                             //writeFileLog"[JCM_GetStatus() Found After power on state.");
                                                             //writeFileLog"[JCM_GetStatus() So going for reset jcm note acceptor.");
                                                             return (JCM_Reset());

                                                          }

                                                          //State 1:post transaction rejection/returning state
		                                          if( ( (0x40 == Response[2]) && (0x09 == Response[3]) ) ||
                                                              ( (0x40 == Response[2]) && (0x0A == Response[3]) ) 
                                                            )
						          {
								
                                                                
		                                                        StateFound=0x00;

		                                                        //writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] Now wait for any standby state.");

		                                                        //wait for standby without escrow state [0x02]  [30min]
		                                                        WaitForState( JCM_STANDBY_WAIT_TIME , Wait_State, Wait_State_Length, &StateFound);

		                                                        //standby without escrow state found
		                                                        if( 0x02 == StateFound )
		                                                        {
                                                                     //writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] Standby mode found.");    
		                                                             return 1;

		                                                        }
		              
		                                                        //notes with esrow state found
		                                                        else if( 0x12 == StateFound )
		                                                        {
		                                                             
		                                                             int CastQuanity=-1,rtcode=-1;

		                                                             GetNoteDetailsInRecycleCst( &CastQuanity );

		                                                             memset(log,'\0',100);

		                                                             sprintf(log,"[JCM_GetStatus() post transaction rejection/returning state] Current Recycle Quantity : %d",CastQuanity);

		                                                             //writeFileLoglog);    
	       
		                                                             if(CastQuanity>0)
		                                                             {
		                                                                   
		                                                                   //writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] Before Going to unload all cash in free fall vault.");    
		                                                                   rtcode = JCM_Unload();
		                                                                   
		                                                                   //writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] After Going to unload all cash in free fall vault.");
		                                                                   return rtcode;


		                                                             }
		                                                             else
		                                                             {
		                                                                    //writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] Before Going to unload unknown recycle quanity found so return error here.");    
		                                                                    return 0;

		                                                             }
	   
		                                                        }

		                                                        //sensor state error
		                                                        else if( 0x80 == StateFound )
		                                                        {
		                                                            //writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] Sensor problem state found.");    
                                                                            //log current sensor status  
		                                                            JCM_LogSensorState(Response[7],Response[8],Response[9]);
		                                                            return 0;

		                                                        }

		                                                        //alarm state error
		                                                        else if( 0x81 == StateFound )
		                                                        {
		                                                            //writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] Alarm state found Going to reset.");    
		                                                            //if reset issue success    
		                                                            return(JCM_ReInit(20));

		                                                        }

                     		                   
							  }
                                        
                                                          //State 2:Sensor Trouble log all sensor status here
		                                          else if( (0x40 == Response[2]) && (0x80 == Response[3]) ) 
						          {
								//writeFileLog"[JCM_GetStatus() [0x80] ] Found Sensor Trouble state."); 
                                                                //wait for [standby with/without escrow] /[ready]/[reset] state 
                                                                JCM_LogSensorState(Response[7],Response[8],Response[9]);
                                                                return 0;
						 	  		                 		                   
							  }

                                                          //State 3:Reset here Jamming state
		                                          else if( (0x40 == Response[2]) && (0x81 == Response[3]) ) 
						          {
							      //writeFileLog"[JCM_GetStatus() [0x81] ] Found Jammed State.");   
                                                              //issue reset command and wait for standby without escrow state
                                                              return(JCM_ReInit(20));
						 	  		                 		                   
							  }
                                                          //State 4:Free fall all notes in escrow (with escrow)
				                          else if( ( (0x40 == Response[2]) && (0x12 == Response[3]) )  ||
                                                                   ( (0x40 == Response[2]) && (0x13 == Response[3]) )
                                                                 )
                                                                  
						          {
                                                                     if( (0x40 == Response[2]) && (0x13 == Response[3]) ) 
                                                                     {
                                                                         //writeFileLog"[JCM_GetStatus() [0x13] ] Found Ready State with escorw notes.");
                                                                     }

                                                                     if( (0x40 == Response[2]) && (0x12 == Response[3]) ) 
                                                                     {
                                                                         //writeFileLog"[JCM_GetStatus() [0x12] ] Found Standby State with escorw notes.");
                                                                     }

                                                                     if(0x13 == Response[3])
                                                                     {
                                                                         //writeFileLog"[JCM_GetStatus() [0x13] ] Going to disabled jcm note acceptor.");
								         DisableAcceptance();

                                                                     }

                                                                     int CastQuanity=-1,rtcode=-1;

                                                                     GetNoteDetailsInRecycleCst( &CastQuanity );

                                                                     memset(log,'\0',100);

                                                                     sprintf(log,"[JCM_GetStatus() [0x12/0x13] ] Current Recycle Quantity : %d",CastQuanity);
                                                                     //writeFileLoglog);    
       
                                                                     if(CastQuanity>0)
                                                                     {
                                                                           
                                                                           //writeFileLog"[JCM_GetStatus() [0x12/0x13] ] Before Going to unload all cash in free fall vault.");    
                                                                           rtcode = JCM_Unload();
                                                                           
                                                                           //writeFileLog"[JCM_GetStatus() [0x12/0x13] ] After Going to unload all cash in free fall vault.");
                                                                           return rtcode;


                                                                     }
                                                                     else
                                                                     {
                                                                            //writeFileLog"[JCM_GetStatus() [0x12/0x13] ] Before unload unable to get current recycle quanity.so return error here.");
                                                                            return 0;

                                                                     }

						 	  		                 		                   
							  }

                                                          //State 5:everything is ok for start a new transaction
						          else if(  (0x40 == Response[2]) && (0x02 == Response[3]) ) 
						          {

								   //writeFileLog"[JCM_GetStatus() [0x02] ] Found Standby State without escrow notes."); 
						                   //writeFileLog"[JCM_GetStatus() [0x02] ] JCM is ok.");

						                   return 1;
						 	  		                 		                   
							  }

                                                          //State 6:everything is ok for start a new transaction
						          else if(  (0x40 == Response[2]) && (0x03 == Response[3]) ) 
						          {
								   
                                                                   //writeFileLog"[JCM_GetStatus() [0x03] ] Found Ready for insertion state.");
                                                                   int rtcode=-1;

                                                                   rtcode=DisableAcceptance();

                                                                   if(1==rtcode)
                                                                   {
                                                                           //writeFileLog"[JCM_GetStatus() [0x03] ] Disable Success.");
                                                                           return 1;

                                                                   }
                                                                   else
                                                                   {
                                                                           //writeFileLog"[JCM_GetStatus() [0x03] ] Disable Failed.");
						                           return 0;

                                                                   }
						 	  		                 		                   
							  }
                                                          else
                                                          {
							         //writeFileLog"[JCM_GetStatus()] Found Unknown JCM State .");

		                                                 return 0;

                                                          }


					  }
		                          else
					  {
						
                                                  //writeFileLog"[JCM_GetStatus()] Sense Command issue failed.");

		                                  //writeFileLog"[JCM_GetStatus()] JCM is not ok.");

		                                  return 0;


					  }

	   }
	   else
	   {
	        //writeFileLog"[JCM_GetStatus()] JCM port open not done.");
            return 0;

	   }//else end
           



}//JCM_GetStatus() end

int JCM_GetStatusV2(){
          
       if( -1 != g_HANDLE ) {

	       
                                   
                                         unsigned char Wait_State[4] ={0x02,0x03,0x12,0x13};

                                         unsigned int  Wait_State_Length=4; 

                                         unsigned char StateFound=0x00;

                                         int rtcode=-1;

                                         unsigned char Response[100];

                                         unsigned char log[200];

                                         memset(log,'\0',200);

	                         
                                         //issue sense command
		                                 rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                         int replychecksum=0x00;

                                         replychecksum=GetBCC(Response,2,JCM_SENSE_REPLY_SIZE);

                                         if(replychecksum!=Response[JCM_SENSE_REPLY_SIZE-1]){
                                              ////writeFileLog"[JCM_GetStatusV2()] Not matched with reply byte checksum value");
                                              AVRM_writeFileLogV2("[JCM_GetStatusV2()] Not matched with reply byte checksum value",ERROR,g_BNAId);
                                         }//if(replychecksum!=Response[JCM_SENSE_REPLY_SIZE-1]) end
		                          
		                                 if( Response[3] >= 0x00 ) {

					                  
                                                         #ifdef JCM_DEBUG

                                                         ////writeFileLog"=========================================================================");

				                                         //int i=0;

		                                                 //for(i=0;i<JCM_SENSE_REPLY_SIZE;i++) {
						
		                                                        //memset(log,'\0',200);
                                                                //sprintf(log,"[JCM_GetStatusV2()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                                        ////writeFileLoglog);
                                                         //}//for end

                                                         ////writeFileLog"=========================================================================");

                                                         #endif  

                                                          memset(log,'\0',200);
                                                          sprintf(log,"[JCM_GetStatusV2()] Sense Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);
		                                                  ////writeFileLoglog);
		                                                  AVRM_writeFileLogV2(log,DEBUG,g_BNAId);

                                                          //Log Current JCM Sensor Status
                                                          //++JCM_LogSensorState(Response[7],Response[8],Response[9]);

                                                          if( (0x40 == Response[2]) && (0x00 == Response[3]) ){

                                                             ////writeFileLog"[JCM_GetStatusV2() Found After power on state.");
                                                             ////writeFileLog"[JCM_GetStatusV2() So going for reset jcm note acceptor.");
                                                             AVRM_writeFileLogV2("[JCM_GetStatusV2() Found After power on state.",INFO,g_BNAId);
                                                             AVRM_writeFileLogV2("[JCM_GetStatusV2() So going for reset jcm note acceptor.",INFO,g_BNAId);
                                                             return (JCM_Reset());

                                                          }//if end

                                                          //State 1:post transaction rejection/returning state
		                                                  if( ( (0x40 == Response[2]) && (0x09 == Response[3]) ) ||( (0x40 == Response[2]) && (0x0A == Response[3]) ) ){
								
                                                                
		                                                        StateFound=0x00;

		                                                        ////writeFileLog"[JCM_GetStatusV2() post transaction rejection/returning state] Now wait for any standby state.");
                                                                AVRM_writeFileLogV2("[JCM_GetStatusV2() post transaction rejection/returning state] Now wait for any standby state.",INFO,g_BNAId);

		                                                        //wait for standby without escrow state [0x02]  [30min]
		                                                        WaitForState( JCM_STANDBY_WAIT_TIME , Wait_State, Wait_State_Length, &StateFound);

		                                                        //standby without escrow state found
		                                                        if( 0x02 == StateFound ){
                                                                     ////writeFileLog"[JCM_GetStatusV2() post transaction rejection/returning state] Standby mode found.");
                                                                     AVRM_writeFileLogV2("[JCM_GetStatusV2() post transaction rejection/returning state] Standby mode found.",INFO,g_BNAId);    
		                                                             return 1;
		                                                        }//if( 0x02 == StateFound ) end
		                                                        //notes with esrow state found
		                                                        else if( 0x12 == StateFound )
		                                                        {
		                                                             
																		 int CastQuanity=-1,rtcode=-1;

																		 GetNoteDetailsInRecycleCst( &CastQuanity );

																		 memset(log,'\0',100);

																		 sprintf(log,"[JCM_GetStatusV2() post transaction rejection/returning state] Current Recycle Quantity : %d",CastQuanity);

																		 ////writeFileLoglog); 
																		 
																		 AVRM_writeFileLogV2(log,INFO,g_BNAId);     
				
																		 rtcode = 0;
																		 
																		 return 1;
																		 
																		 //if(CastQuanity>0){
																			   
																			   ////writeFileLog"[JCM_GetStatusV2() post transaction rejection/returning state] Before Going to unload all cash in free fall vault.");    
																			   //++rtcode = JCM_Unload();
																			   ////writeFileLog"[JCM_GetStatusV2() post transaction rejection/returning state] After Going to unload all cash in free fall vault.");
																			   //return rtcode;


																		 //}else {
																				////writeFileLog"[JCM_GetStatusV2() post transaction rejection/returning state] Before Going to unload unknown recycle quanity found so return error here.");    
																				//return 0;

																		 //}//else end
	   
																	}//else if( 0x12 == StateFound ) end

																	//sensor state error
																	else if( 0x80 == StateFound )
																	{
																		////writeFileLog"[JCM_GetStatusV2() post transaction rejection/returning state] Sensor problem state found.");
																		AVRM_writeFileLogV2("[JCM_GetStatusV2() post transaction rejection/returning state] Sensor problem state found.",INFO,g_BNAId);        
																		//log current sensor status  
																		JCM_LogSensorState(Response[7],Response[8],Response[9]);
																		return 0;

																	}
																	//alarm state error
																	else if( 0x81 == StateFound )
																	{
																		////writeFileLog"[JCM_GetStatus() post transaction rejection/returning state] Alarm state found Going to reset."); 
																		AVRM_writeFileLogV2("[JCM_GetStatus() post transaction rejection/returning state] Alarm state found Going to reset.",INFO,g_BNAId);   
																		//if reset issue success    
																		return(JCM_ReInit(20));

																	}//else if

                     		                   
							                                }//State 1:post transaction rejection/returning state
		                                                     
                                                           //State 2:Sensor Trouble log all sensor status here
		                                                   else if( (0x40 == Response[2]) && (0x80 == Response[3]) ) 
													       {
													             ////writeFileLog"[JCM_GetStatus() [0x80] ] Found Sensor Trouble state."); 
													             AVRM_writeFileLogV2("[JCM_GetStatus() [0x80] ] Found Sensor Trouble state.",INFO,g_BNAId);
																 //wait for [standby with/without escrow] /[ready]/[reset] state 
																 JCM_LogSensorState(Response[7],Response[8],Response[9]);
																 return 0;
																								   
												           }
                                                           //State 3:Reset here Jamming state
		                                                   else if( (0x40 == Response[2]) && (0x81 == Response[3]) ) 
											               {
											                       ////writeFileLog"[JCM_GetStatus() [0x81] ] Found Jammed State."); 
											                       AVRM_writeFileLogV2("[JCM_GetStatus() [0x81] ] Found Jammed State.",INFO,g_BNAId);  
																   //issue reset command and wait for standby without escrow state
																   return(JCM_ReInit(20));
																						   
										                   }
                                                           //State 4:Free fall all notes in escrow (with escrow)
				                                           else if( ( (0x40 == Response[2]) && (0x12 == Response[3]) )  || ( (0x40 == Response[2]) && (0x13 == Response[3]) ) )
                                                           {
																		 if( (0x40 == Response[2]) && (0x13 == Response[3]) ) 
																		 {
																			 ////writeFileLog"[JCM_GetStatus() [0x13] ] Found Ready State with escorw notes.");
																			 AVRM_writeFileLogV2("[JCM_GetStatus() [0x13] ] Found Ready State with escorw notes.",INFO,g_BNAId);  
																		 }

																		 if( (0x40 == Response[2]) && (0x12 == Response[3]) ) 
																		 {
																			 ////writeFileLog"[JCM_GetStatus() [0x12] ] Found Standby State with escorw notes.");
																			 AVRM_writeFileLogV2("[JCM_GetStatus() [0x12] ] Found Standby State with escorw notes.",INFO,g_BNAId);  
																		 }

																		 if(0x13 == Response[3])
																		 {
																			 ////writeFileLog"[JCM_GetStatus() [0x13] ] Going to disabled jcm note acceptor.");
																			 AVRM_writeFileLogV2("[JCM_GetStatus() [0x13] ] Going to disabled jcm note acceptor.",INFO,g_BNAId); 
																			 DisableAcceptance();

																		 }

																		 int CastQuanity=-1,rtcode=-1;

																		 GetNoteDetailsInRecycleCst( &CastQuanity );

																		 memset(log,'\0',100);

																		 sprintf(log,"[JCM_GetStatus() [0x12/0x13] ] Current Recycle Quantity : %d",CastQuanity);
																		 
																		 ////writeFileLoglog);   
																		 
																		 AVRM_writeFileLogV2(log,INFO,g_BNAId); 
																		 
																		 rtcode = 0;
																		 
																		 return 1;
																		 
																		 //if(CastQuanity>0)
																		 //{
																			   
																			   ////writeFileLog"[JCM_GetStatus() [0x12/0x13] ] Before Going to unload all cash in free fall vault.");    
																			   //++rtcode = JCM_Unload();
																			   ////writeFileLog"[JCM_GetStatus() [0x12/0x13] ] After Going to unload all cash in free fall vault.");
																			   //return 1;


																		 //}
																		 //else
																		 //{
																				////writeFileLog"[JCM_GetStatus() [0x12/0x13] ] Before unload unable to get current recycle quanity.so return error here.");
																				//return 0;

																		 //}//else end
						 	  		                 		                   
							                                 }//else if( ( (0x40 == Response[2]) && (0x12 == Response[3]) )  || end
                                                              
                                                             //State 5:everything is ok for start a new transaction
															 else if(  (0x40 == Response[2]) && (0x02 == Response[3]) ) 
															 {

															           ////writeFileLog"[JCM_GetStatus() [0x02] ] Found Standby State without escrow notes."); 
																	   ////writeFileLog"[JCM_GetStatus() [0x02] ] JCM is ok.");
																	   AVRM_writeFileLogV2("[JCM_GetStatus() [0x02] ] Found Standby State without escrow notes.",INFO,g_BNAId); 
																	   AVRM_writeFileLogV2("[JCM_GetStatus() [0x02] ] JCM is ok.",INFO,g_BNAId); 
																	   return 1;
																										   
														     }//else if end

                                                             //State 6:everything is ok for start a new transaction
															 else if(  (0x40 == Response[2]) && (0x03 == Response[3]) ) 
															 {
								   
                                                                   ////writeFileLog"[JCM_GetStatus() [0x03] ] Found Ready for insertion state.");
                                                                   AVRM_writeFileLogV2("[JCM_GetStatus() [0x03] ] Found Ready for insertion state.",INFO,g_BNAId); 
                                                                   
                                                                   int rtcode=-1;
                                                                   
                                                                   rtcode=DisableAcceptance();

                                                                   if(1==rtcode)
                                                                   {
                                                                           ////writeFileLog"[JCM_GetStatus() [0x03] ] Disable Success.");
                                                                           AVRM_writeFileLogV2("[JCM_GetStatus() [0x03] ] Disable Success.",INFO,g_BNAId); 
                                                                           return 1;

                                                                   }
                                                                   else
                                                                   {
                                                                           ////writeFileLog"[JCM_GetStatus() [0x03] ] Disable Failed.");
                                                                           AVRM_writeFileLogV2("[JCM_GetStatus() [0x03] ] Disable Failed.",INFO,g_BNAId); 
						                                                   return 0;

                                                                   }//else end
						 	  		                 		                   
							                                 }
															 else
															 {
												                     ////writeFileLog"[JCM_GetStatus()] Found Unknown JCM State .");
                                                                     AVRM_writeFileLogV2("[JCM_GetStatus()] Found Unknown JCM State .",INFO,g_BNAId); 
																	 return 0;

															 }//else end


									  }else{
										
														////writeFileLog"[JCM_GetStatus()] Sense Command issue failed.");
                                                        ////writeFileLog"[JCM_GetStatus()] JCM is not ok.");
                                                        AVRM_writeFileLogV2("[JCM_GetStatus()] Sense Command issue failed.",INFO,g_BNAId); 
                                                        AVRM_writeFileLogV2("[JCM_GetStatus()] JCM is not ok.",INFO,g_BNAId); 
														return 0;


									  }//else end

	   }
	   else
	   {
	        ////writeFileLog"[JCM_GetStatus()] JCM port open not done.");
	        AVRM_writeFileLogV2("[JCM_GetStatus()] JCM port open not done.",INFO,g_BNAId);
            return 0;

	   }//else end
           



}//JCM_GetStatus() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
int DisableSpecificNotes(int fare){
                         
                         
                       int rtcode=-1;
		       NOTE UserDefinedNote={0,0,0,0,0,0,0};

		       if(fare>=5)
		       UserDefinedNote.INR_5    = 1;

		       if(fare>=10)
		       UserDefinedNote.INR_10   = 1;

		       if(fare>=20)
		       UserDefinedNote.INR_20   = 1;

		       if(fare>=50)
		       UserDefinedNote.INR_50  = 1;

		       if(fare>=100)
		       UserDefinedNote.INR_100  = 1;

		       if(fare>=500)
		       UserDefinedNote.INR_500  = 1;

		       if(fare>=1000)
		       UserDefinedNote.INR_1000 = 1;

                       //equal fare
	               if( 0 == fare)
		       {
			      //writeFileLog"[DisableSpecificNotes()] Zero fare receieved.");
                            		       
                       }
                       
                       //issue command
		       rtcode=EnableDenom(UserDefinedNote); 

		       if( 1 == rtcode)
		       {
		            
                            //writeFileLog"[DisableSpecificNotes()] Specific Notes Disable success."); 
                            return 1;
		       }
                       else 
		       {
                            //writeFileLog"[DisableSpecificNotes()] Specific Notes Disable failed."); 
                            return 0;

		       }

       
	       
}//DisableSpecificNotes() end
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Disable all acceptance
int DisableAcceptance() {
        
 
             int rtcode=-1;

             unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

             memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

             int ResponseLength=JCM_INSERTION_INHIBITING_REPLY_SIZE;

             unsigned int Delay=JCM_RECV_DELAY ;
 
             unsigned char log[200];
  
             //issue disable acceptance command
             rtcode=JCM_Cmd_InsertionInhibited( Response,ResponseLength,Delay ); 

             //display response packet
             //#ifdef JCM_DEBUG 
             //int Counter=0;
             //for(Counter=0;Counter<ResponseLength;Counter++){
		             //memset(log,'\0',200);
                     //sprintf(log,"[DisableAcceptance()] DisableAcceptance[%d] = 0x%xh",Counter,Response[Counter]);
                     ////writeFileLoglog);

             //}//for end
             //#endif

             if( Response[3]> 0x00 ){
		     
                    
					 if( 0x02 ==Response[3]){
						 ////writeFileLog"[DisableAcceptance()] 0x02 Means Standby Without Escrow mode found.");
						 AVRM_writeFileLogV2("[DisableAcceptance()] 0x02 Means Standby Without Escrow mode found.",DEBUG,g_BNAId);
						 return 1;
                     }else if( 0x12 ==Response[3]){
						  ////writeFileLog"[DisableAcceptance()] 0x12 Means Standby With Escrow mode found.");
						  AVRM_writeFileLogV2("[DisableAcceptance()] 0x12 Means Standby With Escrow mode found.",DEBUG,g_BNAId);
						  return 2;
					 }else {
						  memset(log,'\0',200);
				          sprintf(log,"[DisableAcceptance()] No Standby mode found Command = x%xh State= x%xh", Response[2],Response[3]);
                          ////writeFileLoglog);
                          AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                          return 0;
                     }//else end

          }else{
                
                memset(log,'\0',200);
                sprintf(log,"[DisableAcceptance()] Current State = 0x%xh",Response[3]);
                ////writeFileLoglog);
                AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                ////writeFileLog"[DisableAcceptance()] DisableAcceptance command issue failed."); 
                AVRM_writeFileLogV2("[DisableAcceptance()] DisableAcceptance command issue failed.",WARN,g_BNAId);     
                return (-1);    
          }//else end    

         
            
}//DisableAcceptance() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int WaitForState( const unsigned int  Wait_time,unsigned char*  Wait_State,  const unsigned int  Wait_State_Length, unsigned char* StateFound) {
         
                int rtcode=-1,Counter=0;

                *StateFound= 0x00;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                struct timespec begints, endts,diffts;

                clock_gettime(CLOCK_MONOTONIC, &begints);

                unsigned char log[100];

                int retry=0;

                while(1) {
		               
						   rtcode=-1;
						   
						   memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);
								 
						   rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_RSP_REPLY_SIZE);

						   if(Response[3]>0x00)
						   {
									  
											  
											   *StateFound= Response[3];

											   for(Counter=0;Counter<Wait_State_Length;Counter++) {
												   
								                             if( Response[3] == Wait_State[Counter])
															 {
																	 clock_gettime(CLOCK_MONOTONIC, &endts);

																	 diffts.tv_sec = endts.tv_sec - begints.tv_sec;

																	 memset(log,'\0',100);

																	 sprintf(log,"[WaitForState()] Found State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
																	 
																	 //writeFileLoglog);
																	 
																	 AVRM_writeFileLogV2(log,INFO,g_BNAId);
											
																	 return 1; //if given state found return 1

															 }// if( Response[3] == Wait_State[Counter]) end


											}//for end

						   }//if(Response[3]>0x00) end

							   
						   clock_gettime(CLOCK_MONOTONIC, &endts);

						   diffts.tv_sec = endts.tv_sec - begints.tv_sec;

						   if(diffts.tv_sec >= Wait_time) {
									   
									   
										   memset(log,'\0',100);

										   sprintf(log,"[WaitForState()] Timeout Wait_time =%d diffts.tv_sec =%d.",Wait_time,diffts.tv_sec);
											
										   ////writeFileLoglog);
										   
										   AVRM_writeFileLogV2(log,INFO,g_BNAId);
												
										   for(Counter=0;Counter<Wait_State_Length;Counter++){
										 
												 *StateFound= Response[3];

												 if( Response[3] == Wait_State[Counter])
												 {
												 
																 clock_gettime(CLOCK_MONOTONIC, &endts);

																 diffts.tv_sec = endts.tv_sec - begints.tv_sec;
												
																 memset(log,'\0',100);

																 sprintf(log,"[WaitForState()] Found State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
											
																 ////writeFileLoglog);
																 
																 AVRM_writeFileLogV2(log,INFO,g_BNAId);

																 return 1; //if given state found return 1

												 }//if( Response[3] == Wait_State[Counter]) end

										  }//for end

										   memset(log,'\0',100);

										   sprintf(log,"[WaitForState()] Timeout Current State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
											
										   ////writeFileLoglog);
										   
										   AVRM_writeFileLogV2(log,INFO,g_BNAId);

										   return 2;  //if given state not found return 2 (timeout)


									}//if(diffts.tv_sec >= Wait_time) END



                 }//while block

}//WaitForState() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_Make_StandBy(int OperationCode) {
         
        
       unsigned char log[100];
       memset(log,'\0',100);
       unsigned char Wait_State[4]={0x02,0x03,0x12,0x13};
       unsigned int  Wait_State_Length=4; 
       unsigned char StateFound=0x00;
       int rtcode=1;
       //wait for standby state
       if(  1 != ReadEnqFromJcm() ){  
            
                     ////writeFileLog"[JCM_Make_StandBy() Sense] No Enq Byte recv from JCM.Now go for issue sense command.");
                     AVRM_writeFileLogV2("[JCM_Make_StandBy() Sense] No Enq Byte recv from JCM.Now go for issue sense command.",INFO,g_BNAId);
                     unsigned char Response[JCM_SENSE_REPLY_SIZE];
                     memset(Response,'\0',JCM_SENSE_REPLY_SIZE);
                     rtcode=JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);
	                 #ifdef JCM_DEBUG 
					 if(Response[3] > 0){
						int i=0;
						for(i=0;i<JCM_SENSE_REPLY_SIZE;i++) {  
							memset(log,'\0',100);
							sprintf(log,"[JCM_Make_StandBy()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
							//writeFileLoglog); 
					    }//for end
					 }//if end
                     #endif
                     memset(log,'\0',100);
                     sprintf(log,"[JCM_Make_StandBy()] Sense Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);
		             ////writeFileLoglog);
                     AVRM_writeFileLogV2(log,INFO,g_BNAId);
					 if( Response[3] > 0 ){

                                  if( 0x0D == Response[3]  ){
										////writeFileLog"[JCM_Make_StandBy()] Found Remaining notes states 0x0D.");
										AVRM_writeFileLogV2("[JCM_Make_StandBy()] Found Remaining notes states 0x0D.",INFO,g_BNAId);
										return 1;
								  }//if end
								  //normal state to do operation
		                          else if( (0x12 == Response[3] ) || (0x02 == Response[3] ) ){
				        
                                        if( 0x80 == Response[2] ){
                                            ////writeFileLog"[JCM_Make_StandBy() RSP State Found ] RSP State Found Now wait for few seconds for reactivate JCM.");
                                            AVRM_writeFileLogV2("[JCM_Make_StandBy() RSP State Found ] RSP State Found Now wait for few seconds for reactivate JCM.",INFO,g_BNAId);
                                            DelayInSeconds( 6 );
                                        }else{
                                            ////writeFileLog"[JCM_Make_StandBy()] Standby mode found.");
                                            AVRM_writeFileLogV2("[JCM_Make_StandBy()] Standby mode found.",INFO,g_BNAId);
                                        }//else end

		                                return 1;

				                  }//else if end
				                  //DISABLE ACCEPTANCE
								  else if( (0x03 == Response[3] ) || (0x13 == Response[3] ) ) {
								         AVRM_writeFileLogV2("[JCM_Make_StandBy() Enable State Found ] Enable State Found Going for disable JCM.",INFO,g_BNAId);
								         ////writeFileLog"[JCM_Make_StandBy() Enable State Found ] Enable State Found Going for disable JCM.");
								         int Dis_rtcode=-1;
								         Dis_rtcode = DisableAcceptance();
								         if( (1 == Dis_rtcode ) || (2 == Dis_rtcode ) ){
											return 1;
									     }else{
											return 1;
									     }
								  }//else if end
								  //Sensor Problem State
								  else if( 0x80 == Response[3] ) {
									  StateFound=0x00;         
									  ////writeFileLog"[JCM_Make_StandBy() Sensor Trouble] Sensor Problem State Found.");
									  ////writeFileLog"[JCM_Make_StandBy() Sensor Trouble] Going for sensor trouble state over checking.");
									  AVRM_writeFileLogV2("[JCM_Make_StandBy() Sensor Trouble] Sensor Problem State Found.",INFO,g_BNAId);
									  AVRM_writeFileLogV2("[JCM_Make_StandBy() Sensor Trouble] Going for sensor trouble state over checking.",INFO,g_BNAId);
									  WaitForState( JCM_SENSOR_OVER_WAIT_TIME ,Wait_State ,Wait_State_Length , &StateFound );                        
                                      if( (0x03 == StateFound  ) || (0x13 == StateFound  ) ) {
										  int Dis_rtcode=-1;
										  Dis_rtcode = DisableAcceptance();
										  return Dis_rtcode;
                                      }else if ( 0x80 == StateFound ) {
                                          return (-1);
                                      }else if ( 0x12 == StateFound ){
										  return 1;
                                      }else if ( 0x02 == StateFound ){
										  return (-1);
                                      }//else if end
		                          }//else if end
		                          //Alarm Problem State
		                          else if( 0x81 == Response[3] ) {
									  StateFound=0x00;         
                                      ////writeFileLog"[JCM_Make_StandBy() Alarm/Jamm Trouble] Alarm/Jamm Problem State Found.");
                                      ////writeFileLog"[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for Reset JCM.");
                                      AVRM_writeFileLogV2("[JCM_Make_StandBy() Alarm/Jamm Trouble] Alarm/Jamm Problem State Found.",INFO,g_BNAId);
                                      AVRM_writeFileLogV2("[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for Reset JCM.",INFO,g_BNAId);
                                      if(JCM_UNLOAD_OPERATION == OperationCode) {
                                          ////writeFileLog"[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for Reset JCM for unload operation.");
                                          AVRM_writeFileLogV2("[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for Reset JCM for unload operation.",INFO,g_BNAId);
                                          JCM_ReInit(30);
                                          return (-1);
                                      }//if end
                                      if(JCM_DISPENSE_OPERATION == OperationCode){
                                           ////writeFileLog"[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for return here dispense operation.");
                                           AVRM_writeFileLogV2("[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for return here dispense operation.",INFO,g_BNAId);
                                           return (-1);
                                      }//if end

		                          }//else if end
		                          //wait for reject state over
		                          else if( (0x08 == Response[3] ) || (0x09 == Response[3] ) ){
				                      ////writeFileLog"[JCM_Make_StandBy()] Now Wait for Reject State.");
				                      AVRM_writeFileLogV2("[JCM_Make_StandBy()] Now Wait for Reject State.",INFO,g_BNAId);
				                      rtcode = -1;
				                      rtcode = WaitForState( JCM_REJECTION_OVER_WAIT_TIME,Wait_State,Wait_State_Length, &StateFound);
				                      return rtcode;
				                  }//else if end
								  else{
									  return (-1);
								  }//else end
		             }else if ( 0x00 == Response[3] ) { //if sense command issue failed
                             ////writeFileLog"[JCM_Make_StandBy() sense command issue failed] Now Wait RSP State Change Packet as sense command issue failed.");
                             AVRM_writeFileLogV2("[JCM_Make_StandBy() sense command issue failed] Now Wait RSP State Change Packet as sense command issue failed.",ERROR,g_BNAId);
		                     rtcode = -1;
		                     ////writeFileLog"[JCM_Make_StandBy() sense command issue failed ] Now wait for some seconds for jcm again reactive.");
		                     AVRM_writeFileLogV2("[JCM_Make_StandBy() sense command issue failed ] Now wait for some seconds for jcm again reactive.",INFO,g_BNAId);
                             DelayInSeconds( 6 );
                             return 1;
                    }else{
						 return (-1);
					}//else end 

	  } else {    
		         ////writeFileLog"[JCM_Make_StandBy() RSP State] Enq Byte recv from JCM,now go for issue read rsp packet.");
		         AVRM_writeFileLogV2("[JCM_Make_StandBy() RSP State] Enq Byte recv from JCM,now go for issue read rsp packet.",INFO,g_BNAId);
				 WaitForState( JCM_STANDBY_STATE_WAIT_TIME,Wait_State,Wait_State_Length, &StateFound);
				 return 1;
      }//else end

}//JCM_Make_StandBy() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_Unload() {
      
       int rtcode=-1;
       unsigned char Wait_State[4]= {0x02,0x03,0x12,0x13} ;
       unsigned int  Wait_State_Length=4; 
       unsigned char StateFound=0x00;
       unsigned char log[100];
       memset(log,'\0',100);
       rtcode=-1;
       rtcode= JCM_Make_StandBy(JCM_UNLOAD_OPERATION);
       if( 1 == rtcode) {

               rtcode=-1;
               rtcode = JCM_Receipt();
               //++wait for note fall in free fall box
			   if(1 ==rtcode) {
				   ////writeFileLog"[JCM_Unload()] Unload Successfully done.");
				   AVRM_writeFileLogV2("[JCM_Unload()] Unload Successfully done.",INFO,g_BNAId);
				   return 1;
               }else{
                   ////writeFileLog"[JCM_Unload()] Unload Command execute failed..");
                   AVRM_writeFileLogV2("[JCM_Unload()] Unload Command execute failed..",ERROR,g_BNAId);
				   return 0;
			   }//else end

      }else{
              ////writeFileLog"[JCM_Unload()] Unload Command execute failed due to no standby state found!!!.");
              AVRM_writeFileLogV2("[JCM_Unload()] Unload Command execute failed due to no standby state found!!!.",ERROR,g_BNAId);
              return (-1);
      }//else end
       	
}//JCM_Unload() end

//++accept money from escrow to vault ( Standby or ready for insertion state required for this operation 
int JCM_Receipt() {
                
                
                int rtcode=-1,Counter=0;
                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];
                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);
                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_RECEIPT_CMD;
                unsigned char bcc = GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
                unsigned char Replybcc =0x00;
                unsigned char log[100];
                memset(log,'\0',100);
                memset(log,'\0',100);
                sprintf(log,"[JCM_Receipt()] GetBCC CheckSum = 0x%xh",bcc);
                ////writeFileLoglog);  
                AVRM_writeFileLogV2(log,INFO,g_BNAId);
                Commands[5]=bcc;
                #ifdef JCM_DEBUG
                for(Counter=0;Counter<6;Counter++) {

                       memset(log,'\0',100);
                       sprintf(log,"[JCM_Receipt()] Command[%d] = 0x%xh",Counter,Commands[Counter]);
                       //writeFileLoglog);  
	            }
	            #endif
       
                //Transmit receipt Command
                rtcode=-1;
                rtcode=JCM_TransmitCmdSequence( g_HANDLE, Commands ,JCM_CMD_CMN_LENGTH);
                if(1==rtcode) {
		                
		                struct timespec begints, endts,diffts;
                        clock_gettime(CLOCK_MONOTONIC, &begints);
                        while(1) {
                                //Read RSP Packet from JCM
                                rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_RECEIPT_REPLY_SIZE);
                                //check reply bytes checksum value
                                int replychecksum=0x00;
                                replychecksum = GetBCC(Response,2,JCM_RECEIPT_REPLY_SIZE);
                                ////////////////////////////////////////////////////////////////////
                                //Log reply bytes checksum value
                                //memset(log,'\0',100);
                                //sprintf(log,"[JCM_Receipt()] Command[%d] = 0x%xh, replychecksum =0x%xh. ",JCM_RECEIPT_REPLY_SIZE-1,Commands[JCM_RECEIPT_REPLY_SIZE-1],replychecksum);
                                ////writeFileLoglog); 
                                ///////////////////////////////////////////////////////////////////
                                if( Response[JCM_RECEIPT_REPLY_SIZE-1] != replychecksum ) {
								   ////writeFileLog"[JCM_Receipt()] Reply byte checksum value not matched .");
								   AVRM_writeFileLogV2("[JCM_Receipt()] Reply byte checksum value not matched .",ERROR,g_BNAId);
                                   return 0; 
                                }//if end
                                if( Response[3] > 0x00  )  	 {

		                           
                                           #ifdef JCM_DEBUG
                                           //writeFileLog"[JCM_JCM_Receipt()] ---------------------------------------");
                                           for(Counter=0;Counter<JCM_RECEIPT_REPLY_SIZE;Counter++)   {
                                                 memset(log,'\0',100);
                                                 sprintf(log,"[JCM_Receipt()] Response[%d] = 0x%xh",Counter,Response[Counter]);
                                                 //writeFileLoglog); 
                                           }
                                           //writeFileLog"[JCM_Receipt()] -------------------------------------------");
                                           #endif

                                           memset(log,'\0',100);
 
                                           sprintf(log,"[JCM_Receipt()] Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);

                                           ////writeFileLoglog); 
                                           
                                           AVRM_writeFileLogV2(log,INFO,g_BNAId);

                                           //calculate time
                                           clock_gettime(CLOCK_MONOTONIC, &endts);

			                               diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                           //Standby mode found
					                       if(  0x02  == Response[3]  ) {
						 
                                                 memset(log,'\0',100);
                                                 sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                 ////writeFileLoglog); 
                                                 AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                 return 1;

					                       }
                                           //Alarm state found
					                       else if(  0x81  == Response[3]  )		                             
		                                   {
						                         memset(log,'\0',100);
                                                 sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                 ////writeFileLoglog); 
                                                 AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                 return 3;

					                      }//else if end
					                      
		                         }//if( Response[3] > 0x00  ) end
		               
                                clock_gettime(CLOCK_MONOTONIC, &endts);

								diffts.tv_sec = endts.tv_sec - begints.tv_sec;

								if(diffts.tv_sec >= JCM_UNLOAD_TIME) {


											//++Standby mode found
											if(  0x02  == Response[3]  ) 
											{
									 
														 memset(log,'\0',100);
                                                         sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
														 ////writeFileLoglog); 
														 AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                         return 1;

											 }
											 //alarm state found
											 else if(  0x81  == Response[3]  )		                             
											 {
								  
														 memset(log,'\0',100);
                                                         sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
														 ////writeFileLoglog); 
														 AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                         return 3;

											}   
                                            else
                                            {
                                                   
                                                  memset(log,'\0',100);
                                                  sprintf(log,"[JCM_Receipt()] Timeout for receipt operation. State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                  //writeFileLoglog); 
                                                  AVRM_writeFileLogV2(log,WARN,g_BNAId);
                                                  return 2; //Timeout

                                            }//else end



                                }//if(diffts.tv_sec >= JCM_UNLOAD_TIME)  end



                      }//++while loop

		        
                }else {

                        ////writeFileLog"[JCM_Receipt()] Unload command transmit failed.");
                        AVRM_writeFileLogV2("[JCM_Receipt()] Unload command transmit failed.",ERROR,g_BNAId);
                        return 0;
                 
                 }//else end        

}//JCM_Receipt() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int WaitForStateV2(  const unsigned int  Wait_time,unsigned char* Wait_State,const unsigned int  Wait_State_Length, unsigned char command,unsigned char* StateFound) {
         
                int rtcode=-1,Counter=0;

                *StateFound= 0x00;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                struct timespec begints, endts,diffts;

                clock_gettime(CLOCK_MONOTONIC, &begints);

                unsigned char log[100];

                int retry=0;

                while(1)
		        {
		               
                               rtcode=-1;
		               
                               memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);
                             
                               rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_RSP_REPLY_SIZE);

							   if(Response[3]>0x00) {
		                          
                                          
                                           *StateFound= Response[3];

										   for(Counter=0;Counter<Wait_State_Length;Counter++) {
											    
												  if( ( Response[3] == Wait_State[Counter]) && (Response[2] == command) ){
														
														clock_gettime(CLOCK_MONOTONIC, &endts);
														diffts.tv_sec = endts.tv_sec - begints.tv_sec;
														memset(log,'\0',100);
														sprintf(log,"[WaitForStateV2()] Found State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
														//writeFileLoglog);
														return 1; //if given state found return 1

												 }//if( Response[3] == Wait_State[Counter]) end
				                         
				                          }//for(Counter=0;Counter<Wait_State_Length;Counter++) end


		                       }//if(Response[3]>0x00) end

                           
                               clock_gettime(CLOCK_MONOTONIC, &endts);

							   diffts.tv_sec = endts.tv_sec - begints.tv_sec;

							   if(diffts.tv_sec >= Wait_time) {
                                   
                                   
									   memset(log,'\0',100);

									   sprintf(log,"[WaitForStateV2()] Timeout Wait_time =%d diffts.tv_sec =%d.",Wait_time,diffts.tv_sec);
										
									   //writeFileLoglog);
											
									   for(Counter=0;Counter<Wait_State_Length;Counter++) {
									 
													 
													 *StateFound= Response[3];

													 if( (Response[3] == Wait_State[Counter]) && (Response[2] == command) )  {
											 
															 clock_gettime(CLOCK_MONOTONIC, &endts);

															 diffts.tv_sec = endts.tv_sec - begints.tv_sec;
											
															 memset(log,'\0',100);

															 sprintf(log,"[WaitForStateV2()] Found State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
										
															 //writeFileLoglog);

															 return 1; //++if given state found return 1

													}//if( Response[3] == Wait_State[Counter]) end

									   }//for(Counter=0;Counter<Wait_State_Length;Counter++) end

									   
									   memset(log,'\0',100);

									   sprintf(log,"[WaitForStateV2()] Timeout Current State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
										
									   //writeFileLoglog);

									   return 2;  //if given state not found return 2 (timeout)

                                }//if(diffts.tv_sec >= Wait_time) end

                 }//while block

}//WaitForStateV2() end

int IsNoteRemoved(int timeout) {
									     									     
											 AVRM_writeFileLogV2("[IsNoteRemoved()] Entry.",TRACE,g_BNAId);
											 
											 unsigned char Wait_State[4] ={0x02};
                                             unsigned int  Wait_State_Length=1; 
                                             unsigned char StateFound=0x00;
                                             int rtcode=-1;
                                             unsigned char Response[100];
                                             memset(Response,'\0',100);
                                             unsigned char log[200];
                                             memset(log,'\0',200);
											 unsigned char Response2[100];
											 memset(Response2,'\0',100);
											 int replysize=0;
											 rtcode = 0;
											 memset(Response,'\0',100);
											 unsigned char command = 0x40;
											 
											 //++issue sense command
                                             JCM_delay_miliseconds(200); 
											 rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

											 int replychecksum=0x00;

											 replychecksum=GetBCC(Response,2,JCM_SENSE_REPLY_SIZE);

											 if( ( 0x80 != Response[2] ) && ( 0x40 == Response[2] ) ) {
												 
												 if( replychecksum !=Response[JCM_SENSE_REPLY_SIZE-1] ){
													 
													 AVRM_writeFileLogV2("[IsNoteRemoved()] Not matched with reply byte checksum value",ERROR,g_BNAId);
													 return 31; //other error
													 
												 }//++if end
												 
											 }else if( ( 0x80 == Response[2] ) && ( 0x40 != Response[2] ) ) {
												  
												  //++wait for standby without escrow state [0x02]  [30min]
		                                          WaitForStateV2( JCM_RETURN_WAIT_TIME , Wait_State, Wait_State_Length,command, &StateFound);

										     }//else if( ( 0x80 == Response[2] ) && ( 0x40 != Response[2] ) ) end 
		                                 
									     //++} //if( 1!= rtcode ) trying to read any rsp packet if no rsp packet then isssue sense command
									 
		                                 if( Response[3] >= 0) {
											
                                                    
													 
													 //int i=0;

												     //for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)  {
						
		                                               //memset(log,'\0',200);
                                                       //sprintf(log,"[IsNoteRemoved()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                               ////writeFileLoglog);
		                                               
		                                             //}//for end
		                                             
		                                         
                                                     memset(log,'\0',200);
                                                     sprintf(log,"[IsNoteRemoved()] Sense Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);
                                                     //writeFileLoglog);
                                                     
							                         if( (0x40 == Response[2]) && (0x0A == Response[3])  ) {
								                           
								                           AVRM_writeFileLogV2("[IsNoteRemoved() [0x0A] ] Found Note Return State.",INFO,g_BNAId); 
                                                           return 1; //++Note yet removed
                     		                   
							                         } else if( ( (0x40 == Response[2]) && (0x02 == Response[3]) ) || ( (0x80 == Response[2]) && (0x02 == Response[3]) )  ) { 
														 
														  AVRM_writeFileLogV2("[IsNoteRemoved() [0x02] ] Found Standby State without escrow notes.",INFO,g_BNAId); 
														  AVRM_writeFileLogV2("[IsNoteRemoved() [0x02] ] Notes Removed.",INFO,g_BNAId);
														  
														  /*
														  if( (0x80 == Response[2]) && (0x02 == Response[3]) )  {
															  
															  //writeFileLog"[IsNoteRemoved() [0x02] ] Found Standby RSP State."); 
															  JCM_delay_miliseconds(200); 
															  rtcode = 0;
															  rtcode = JCM_Receieve_RSP_Packet(Response,replysize);
															  memset(log,'\0',200);
															  sprintf(log,"[IsNoteRemoved()] JCM_Receieve_RSP_Packet return code = %d.",rtcode );
															  //writeFileLoglog);
															  
														  }//if end
														  */
														  
                                                          return 0; //++ Note removed
																									   
													 } else {
														 
							                              AVRM_writeFileLogV2("[IsNoteRemoved()] Found Other state .",WARN,g_BNAId);
							                              return 31; //++Other error

                                                     }//++else end


									     } else {
									
											  AVRM_writeFileLogV2("[IsNoteRemoved()] Communication Failure.",ERROR,g_BNAId);
											  return 28; //++Communication Failure
											  
									     }//else end
                        
}//int IsNoteRemoved(int timeout) end

//++Return money to customer
int JCM_Dispense() {


	       int rtcode=-1;
	       unsigned char Wait_State[4] ={0x02,0x03,0x12,0x13};
	       unsigned int  Wait_State_Length=4; 
	       unsigned char StateFound=0x00;
	       unsigned char log[100];
           memset(log,'\0',100); 
           
	       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               
           //++Wait for Standby mode

	       rtcode=-1;
           rtcode= JCM_Make_StandBy(JCM_DISPENSE_OPERATION);

	       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
	       
	       if( 1 == rtcode) {
		       
		       unsigned char State=0x00;

		       //Issue return command to jcm
		       rtcode=-1;
               rtcode=JCM_Return(&State);
		       
		       //++wait for note recv by customer
		       if( 0x0a == State  ) {

                       ////writeFileLog"[JCM_Dispense() Wait for Receieve block] Note dispense successfully executed by JCM but note still is not taken back by customer.");
                       AVRM_writeFileLogV2("[JCM_Dispense() Wait for Receieve block] Note dispense successfully executed by JCM but note still is not taken back by customer.",INFO,g_BNAId);
                       return 1;
                       
		       }else if( 0x02 == State  ) {

		              memset(log,'\0',100); 
                      sprintf(log,"[JCM_Dispense() Standby State ] Return Command execute success State Found= 0x%xh",State);
		              ////writeFileLoglog);
		              AVRM_writeFileLogV2(log,INFO,g_BNAId);
                      return 1;

		       }else if( 0x81 == State  ) {
                      
                      memset(log,'\0',100);
		              sprintf(log,"[JCM_Dispense() Notes Jamming State ] Return Command execute success State Found= 0x%xh",State);
		              ////writeFileLoglog);
		              AVRM_writeFileLogV2(log,INFO,g_BNAId);
                      return 0;

		       }else if( 0x00 == State  ){
		              
		              memset(log,'\0',100); 
                      sprintf(log,"[JCM_Dispense()] Return Command execute failed and State Found= 0x%xh",State);
		              ////writeFileLoglog);
		              AVRM_writeFileLogV2(log,ERROR,g_BNAId);
		              return 0;
		            
		       } else {
                      memset(log,'\0',100); 
                      sprintf(log,"[JCM_Dispense()] No Suitable State Found= 0x%xh",State);
		              ////writeFileLoglog);
		              AVRM_writeFileLogV2(log,WARN,g_BNAId);
		              return 0;
		       }//else end
	      }else {
		       ////writeFileLog"[JCM_Dispense()] Return Command execute failed due to no standby state found!!!.");
		       AVRM_writeFileLogV2("[JCM_Dispense()] Return Command execute failed due to no standby state found!!!.",ERROR,g_BNAId);
		       return (-1);

	      }//else end
	      

	     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}//int JCM_Dispense() end

//++return money from escrow to customer ( Standby state required for this operation [0x02/0x12/0x03/0x13] )
int JCM_Return(unsigned char *ReturnState) {

 
                int rtcode=-1,Counter=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);
              
                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_RETURN_CMD;
                  
                unsigned char bcc = GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
       
                unsigned char Calculatebcc=0x00;

                unsigned char log[100];

                memset(log,'\0',100);

                //sprintf(log,"[JCM_Return()] GetBCC CheckSum = 0x%xh",bcc);

                ////writeFileLoglog); 

                Commands[5]=bcc;

              
                #ifdef JCM_DEBUG

                for(Counter=0;Counter<6;Counter++)
                {
                      
                       memset(log,'\0',100);

                       sprintf(log,"[JCM_Return()] Command[%d] = 0x%xh.",Counter,Commands[Counter]);

                       //writeFileLoglog); 

	            }//for end
                
                #endif

                //++Transmit Command
                rtcode=JCM_TransmitCmdSequence( g_HANDLE, Commands ,JCM_CMD_CMN_LENGTH);
                       
                //Wait for standby state 
                if(1==rtcode) {

		                        *ReturnState=0x00;

                                
                                memset(log,'\0',100);

                                sprintf(log,"[JCM_Return()] Dispense Wait Time = %d Seconds.",JCM_RETURN_TIME);

                                ////writeFileLoglog); 
                                
                                AVRM_writeFileLogV2(log,INFO,g_BNAId);

                                

                                struct timespec begints, endts,diffts;

                                clock_gettime(CLOCK_MONOTONIC, &begints);

                                while(1) {
				               
                                           //++get return response reply
										   rtcode=-1;

										   memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

				                           //++Read RSP Packet from JCM
										   rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_RETURN_REPLY_SIZE);

                                           #ifdef JCM_DEBUG

                                           if(Response[3] > 0) {
		                                    
                                                int Counter=0;

		                                        for(Counter=0;Counter<JCM_RETURN_REPLY_SIZE;Counter++)  { 

		                                                  memset(log,'\0',100);

                                                          sprintf(log,"[JCM_Return()] Response[%d] = 0x%xh. ",Counter,Response[Counter]);

                                                          //writeFileLoglog); 

                                                 }//for end
                                                 
		                                    }//if end

                                            #endif

                                               //////////////////////////////////////////////////////////////
                                               
                                               //Calculate reply byte checksum value
                                               //int replychecksum=0x00;
 
                                               //replychecksum = GetBCC(Response,2,JCM_RETURN_REPLY_SIZE);

                                               //memset(log,'\0',100);

											   //sprintf(log,"[JCM_Return()] Response[%d]= 0x%xh. replychecksum  = 0x%xh.",JCM_RETURN_REPLY_SIZE-1,Response[JCM_RETURN_REPLY_SIZE-1],replychecksum);

											   ////writeFileLoglog); 

                                               //if(replychecksum != Response[JCM_RETURN_REPLY_SIZE-1] )
                                               //{

                                                     ////writeFileLog"[JCM_Return()] reply byte checksum not mmatched."); 

                                                     //continue;


                                               //}
                                               

                                               //////////////////////////////////////////////////////////////

                                               if( Response[3] > 0x00 ) {

												   memset(log,'\0',100);
												   sprintf(log,"[JCM_Return()] Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);
												   ////writeFileLoglog);
												   AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                               }//if end

											   if(Response[3] > 0 ) {
						       

                                                       *ReturnState=Response[3];   
    
                                                        memset(log,'\0',100);

                                                        sprintf(log,"[JCM_Return()] Current State Found = 0x%xh.",*ReturnState);

                                                        ////writeFileLoglog); 
                                                        AVRM_writeFileLogV2(log,INFO,g_BNAId);
						       
                                                       //++calculate time
                                                       clock_gettime(CLOCK_MONOTONIC, &endts);

			                                           diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                                       //++note is taken back by customer
						                               if( 0x02 == Response[3]  ){
							     
                                                              memset(log,'\0',100);
                                                              sprintf(log,"[JCM_Return()] State Found = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
                                                              ////writeFileLoglog); 
                                                              AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                              return 1;

													   }
													   //++note infront of note acceptor
													   else if( 0x0A == Response[3] )  
                                                       {
							      
                                                              memset(log,'\0',100);
                                                              sprintf(log,"[JCM_Return()] State Found = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
                                                              ////writeFileLoglog); 
                                                              AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                              return 1;

						                               }
													   //++jamming state
													   else if( 0x81 == Response[3] )  
                                                       {
							      
                                                              memset(log,'\0',100);
                                                              sprintf(log,"[JCM_Return()] State Found = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
                                                              ////writeFileLoglog); 
                                                              AVRM_writeFileLogV2(log,INFO,g_BNAId);
                                                              return 3;

						                               }//else 


                                              }//if(Response[3] > 0 ) end

                                              clock_gettime(CLOCK_MONOTONIC, &endts);

											  diffts.tv_sec = endts.tv_sec - begints.tv_sec;

											  if(diffts.tv_sec >= JCM_RETURN_TIME)
                                              {
		                                       

                                                           memset(log,'\0',100);

                                                           sprintf(log,"[JCM_Return()] Timeout Current State = 0x%xh. Previous State = 0x%xh.  Time =%d Seconds.",Response[3],*ReturnState,diffts.tv_sec);

                                                           ////writeFileLoglog); 
                                                           
                                                           AVRM_writeFileLogV2(log,INFO,g_BNAId);


														   //standby mode found
														   if(  0x02 == *ReturnState  )
														   {

																return 1;

														   }
														   //waiting state
														   else if(  0x0a == *ReturnState )
														   {

																return 1;

														   }

														   //Alarm state
														   else if( 0x81 == *ReturnState ) 
														   {

																return 3;

														   }

														   else //timeout but not any state recvd
														   {

																return 2;

														   }//else end
		                                          

                                              }//if(diffts.tv_sec >= JCM_RETURN_TIME) end

				           } //while(1) end                 

                }else{
                    
                     ////writeFileLog"[JCM_Return()] return command transmit failed!!...");
                     AVRM_writeFileLogV2("[JCM_Return()] return command transmit failed!!...",ERROR,g_BNAId);
                     return 0;
              
                }//else end
                
}//JCM_Return() end

//++accept money again which is not taken by customer ( Standby state required for this operation) 
int JCM_Intake() {
               
                int rtcode=-1,Counter=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);
              
                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_INTAKE_CMD;
                  
                unsigned char bcc = GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
       
                unsigned char Calculatebcc=0x00;

                unsigned char log[100];

                memset(log,'\0',100);

                sprintf(log,"[JCM_Intake()] GetBCC CheckSum = 0x%xh",bcc);

                //writeFileLoglog); 

                Commands[5]=bcc;

                #ifdef JCM_DEBUG

                for(Counter=0;Counter<6;Counter++)
                {
                      

                       memset(log,'\0',100);

                       sprintf(log,"[JCM_Intake()] Command[%d] = 0x%xh",Counter,Commands[Counter]);

                       //writeFileLoglog); 


	            }

                #endif

                //Transmit Command
                rtcode=JCM_TransmitCmdSequence( g_HANDLE, Commands ,JCM_CMD_CMN_LENGTH);

                                    
                //Wait for standby state 
                if(1==rtcode) {
		                
                                struct timespec begints, endts,diffts;
                                clock_gettime(CLOCK_MONOTONIC, &begints);
                                while(1) {
								   
										   //++Check RSP State reply size
										   rtcode=-1;
                                           memset(Response,'\0',JCM_INTAKE_REPLY_SIZE);
                                           rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_INTAKE_REPLY_SIZE);

										   if(Response[3] >0) {
							                                   
						                               //++Response[6]=0x0d;
                                                       Calculatebcc=GetBCC(Response,2,JCM_INTAKE_REPLY_SIZE);

                                                       memset(log,'\0',100);
                                                       sprintf(log,"[JCM_Intake()] CalculateBCC = 0x%x h  ReplyBcc= 0x%x h",Calculatebcc,Response[24]);
                                                       //writeFileLoglog); 
                                                       
													   if(Response[3] >0){
														   
														          int i=0;
														          for(i=0;i<JCM_INTAKE_REPLY_SIZE;i++) {  
															   
                                                                       memset(log,'\0',100);

																	   sprintf(log,"[JCM_Intake()] Response[%d] = 0x%xh. ",i,Response[i]);

																	   //writeFileLoglog); 

																  }//for end

													 
													   }//for end
                                                       
					                                   clock_gettime(CLOCK_MONOTONIC, &endts);
			                                           diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                                       //waiting to be receieved
						                               if( 0x02 == Response[3] )  
                                                       {
                                                              
                                                              memset(log,'\0',100);

                                                              sprintf(log,"[JCM_Intake()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);

                                                              //writeFileLoglog); 

                                                              return 1;

						                               }
                                                       else if( 0x81 == Response[3] ) // jamming state 
                                                       {
		                                              
                                                              memset(log,'\0',100);

                                                              sprintf(log,"[JCM_Intake()] Jammimg State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);

                                                              //writeFileLoglog); 
                                                              
                                                              return 3;

		                                                }//else if( 0x81 == Response[3] )  end


                                               }//if(Response[3] >0) end

                                               clock_gettime(CLOCK_MONOTONIC, &endts);
											   diffts.tv_sec = endts.tv_sec - begints.tv_sec;
											   if(diffts.tv_sec >= JCM_RETURN_TIME) {
		                                           
                                                           memset(log,'\0',100);


                                                           sprintf(log,"[JCM_Intake()] Timeout Current State = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);

                                                           //writeFileLoglog);

														   //waiting to be receieved
														   if( (0x02 == Response[3] ) || (0x81 == Response[3] ) )
														   {
																return 1;

														   }
                                                           else if( 0x81 == Response[3] ) // jamming state 
                                                           {
		                                                
                                                                 return 3;

														   }
														   else //timeout but not any state recvd
														   {
																return 2;
														   }
                                                  

                                              }//if(diffts.tv_sec >= JCM_RETURN_TIME) end

				              } //while(1) end            

                }else {
                    //writeFileLog"[JCM_Intake()] Dispense copmmand transmit failed!!...");
              
                }



}//JCM_Intake() end

//++delay in seconds
void DelayInSeconds( int seconds ) {
		  
                  
                  int oldsecond=0;
                  struct timespec begints, endts,diffts;
                  clock_gettime(CLOCK_MONOTONIC, &begints);
                  unsigned char log[100];
		  while(1)
		  {
                          oldsecond = diffts.tv_sec;
			  clock_gettime(CLOCK_MONOTONIC, &endts);
			  diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                          if(oldsecond != diffts.tv_sec)
                          {
                                 //memset(log,'\0',100);

                                 //sprintf(log,"[DelayInSeconds()] Seconds =%d",diffts.tv_sec);

                                 ////writeFileLoglog);

                          }
			  if(diffts.tv_sec >= seconds)
                          {
                               return;

                          }

                 }



}//DelayInSeconds() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Make_JCM_StandBy_Mode(unsigned int WaitTime) {

      
       unsigned int  Wait_time=WaitTime;
       unsigned char Wait_State[4] ={0x02,0x03,0x12,0x13};
       unsigned int  Wait_State_Length=4; 
       unsigned char StateFound=0x00;
       int rtcode=-1;

       //wait for standby state
       if( 1 != ReadEnqFromJcm())
       {  
		     printf("\n[JCM_Dispense() Sense] No Enq Byte recv from JCM");
		     unsigned char Response[JCM_SENSE_REPLY_SIZE] ;
		     memset(Response,'\0',JCM_SENSE_REPLY_SIZE);
		     rtcode=JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY); 
                     
		     if(Response[3] >0)
		     {
			    int i=0;
			    for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
			    printf("\n [Make_JCM_StandBy_Mode()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
				                             
		     }
                     
		     if( 1 == rtcode)
		     {
		          if( ( 0x12 == Response[3] ) || ( 0x02 == Response[3] ) )
		          {
		               return 1;
		          }
                          else if( ( 0x13 == Response[3] ) || ( 0x03 == Response[3] ) )
		          {
		               return 2;
		          }
		          else
		          {
		               return 0;
		          }
		     }
		     else
		     {
		         return (-1);
		     } 
                     

       }                        
       else
       {
              
              printf("\n[Make_JCM_StandBy_Mode() RSP State] Enq Byte recv from JCM");
              rtcode = -1; 
              rtcode = WaitForState( Wait_time ,Wait_State ,Wait_State_Length , &StateFound );
              return rtcode;
              

       }



}//Make_JCM_StandBy_Mode() end

/*
int GetEscrowMoneyPatchV2(unsigned char *Response,int EscrowIndex){
         
         int Counter=0,DoubleDle=0,DoubleDleIndex=0,MoneyCounter=0;
         unsigned char Money[7];
         memset(Money,'\0',7);
         bool errordetect=false;

         
           
           //EscrowIndex   = Rs 5
           //EscrowIndex+1 = Rs 10
           //EscrowIndex+2 = Rs 20
           //EscrowIndex+3 = Rs 50
           //EscrowIndex+4 = Rs 100
           //EscrowIndex+5 = Rs 500
           //EscrowIndex+6 = Rs 1000

           //user defined Money[] index
           //Index 0 = Rs 5
           //Index 1 = Rs 10
           //Index 2 = Rs 20
           //Index 3 = Rs 50
           //Index 4 = Rs 100
           //Index 5 = Rs 500
           //Index 6 = Rs 1000


        unsigned char log[100];
        int TotalMoney=0;

         //Detect Error
         for(Counter=EscrowIndex;Counter<= (EscrowIndex+6) ;Counter++)
         {
             if( 0x10 == Response[Counter] )
             {
                   DoubleDle++;
                   if(DoubleDle>=2)
                   {
                        errordetect=true;
                        DoubleDleIndex=Counter;
                        memset(log,'\0',100);
                        sprintf(log,"[GetEscrowMoneyPatchV2()] Error Double Byte Recvd. Index Value = %d",DoubleDleIndex);
                        //writeFileLoglog);

                   }
             }
          
         }

         if( false == errordetect )
         {
               
                TotalMoney    = Response[EscrowIndex]+ Response[EscrowIndex+1] + Response[EscrowIndex+2]+ Response[EscrowIndex+3]
+ Response[EscrowIndex+4] + Response[EscrowIndex+5] + Response[EscrowIndex+6];
                DisplayEscrowTableV2(Response,EscrowIndex);
                return TotalMoney;
                            
 

         }
         else if( true == errordetect )
         {

                for(Counter=EscrowIndex;Counter<= (EscrowIndex+7) ;Counter++)
                {
                       if(DoubleDleIndex!=Counter)
                       {
                             Money[MoneyCounter]=Response[Counter];
                             MoneyCounter++;
                       }

                }
                
                 TotalMoney = Money[0]+Money[1]+Money[2]+Money[3]+Money[4]+Money[5]+Money[6];
                 DisplayEscrowTableV2(Money,0);
                 return TotalMoney;

         }


}//GetEscrowMoneyPatchV2() end
*/

void DisplayEscrowTable(JCM_ESCROW_NOTE *currentnote) {

                  unsigned char log[200];
                  memset(log,'\0',200);
                  sprintf(log,"[DisplayEscrowTable()] INR 5=%d INR 10=%d INR 20=%d INR 50=%d INR 100=%d INR 200=%d INR 500=%d INR 1000=%d INR 2000=%d",  currentnote->AcceptedNumber_INR_5,
					 currentnote->AcceptedNumber_INR_10,
					 currentnote->AcceptedNumber_INR_20,
					 currentnote->AcceptedNumber_INR_50,
					 currentnote->AcceptedNumber_INR_100,
					 currentnote->AcceptedNumber_INR_200,
					 currentnote->AcceptedNumber_INR_500,
					 currentnote->AcceptedNumber_INR_1000,
					 currentnote->AcceptedNumber_INR_2000
                 );

                 AVRM_writeFileLogV2(log,INFO,g_BNAId);

                 
}//DisplayEscrowTable(JCM_ESCROW_NOTE *currentnote) end

void DisplayEscrowTableV2(unsigned char *currentnote,int EscrowIndex){

          /*
           
		   EscrowIndex+0 = Rs 5
		   EscrowIndex+1 = Rs 10
		   EscrowIndex+2 = Rs 20
		   EscrowIndex+3 = Rs 50
		   EscrowIndex+4 = Rs 100
		   EscrowIndex+5 = Rs 500
		   EscrowIndex+6 = Rs 1000
		   EscrowIndex+7 = Rs 2000
		   EscrowIndex+8 = Rs 200

		   //user defined Money[] index
		   Index 0 = Rs 5
		   Index 1 = Rs 10
		   Index 2 = Rs 20
		   Index 3 = Rs 50
		   Index 4 = Rs 100
		   Index 5 = Rs 500
		   Index 6 = Rs 1000
		   Index 7 = Rs 2000
		   Index 8 = Rs 2000
       
         */
                 

                 unsigned char log[200];
                 memset(log,'\0',200);
                 sprintf(log,"[DisplayEscrowTableV2()] INR 5=%d INR 10=%d INR 20=%d INR 50=%d INR 100=%d INR 200=%d INR 500=%d INR 1000=%d INR 2000=%d ",       currentnote[EscrowIndex+0], 
                 currentnote[EscrowIndex+1],
                 currentnote[EscrowIndex+2],
                 currentnote[EscrowIndex+3],
                 currentnote[EscrowIndex+4],
                 currentnote[EscrowIndex+8],
                 currentnote[EscrowIndex+5],
                 currentnote[EscrowIndex+6],
                 currentnote[EscrowIndex+7]
                 );

                 AVRM_writeFileLogV2(log,INFO,g_BNAId);

}//inline void DisplayEscrowTableV2() end

int GetEscrowMoneyPatchV2(unsigned char *Response,int EscrowIndex) {
         
         int Counter=0,DoubleDle=0,DoubleDleIndex=0,MoneyCounter=0;
         unsigned char Money[7];
         memset(Money,'\0',7);
         bool errordetect=false;

         /*
           
           EscrowIndex+0 = Rs 5
		   EscrowIndex+1 = Rs 10
		   EscrowIndex+2 = Rs 20
		   EscrowIndex+3 = Rs 50
		   EscrowIndex+4 = Rs 100
		   EscrowIndex+5 = Rs 500
		   EscrowIndex+6 = Rs 1000
		   EscrowIndex+7 = Rs 2000
		   EscrowIndex+8 = Rs 200

		   //user defined Money[] index
		   Index 0 = Rs 5
		   Index 1 = Rs 10
		   Index 2 = Rs 20
		   Index 3 = Rs 50
		   Index 4 = Rs 100
		   Index 5 = Rs 500
		   Index 6 = Rs 1000
		   Index 7 = Rs 2000
		   Index 8 = Rs 200
       
        */

        unsigned char log[100];
        int TotalMoney=0;

         //Detect Error
         for(Counter=EscrowIndex;Counter<= (EscrowIndex+6) ;Counter++)
         {
             if( 0x10 == Response[Counter] )
             {
                   DoubleDle++;
                   if(DoubleDle>=2)
                   {
                        errordetect=true;
                        DoubleDleIndex=Counter;
                        memset(log,'\0',100);
                        sprintf(log,"[GetEscrowMoneyPatchV2()] Error Double Byte Recvd. Index Value = %d",DoubleDleIndex);
                        //writeFileLoglog);

                   }
             }
          
         }

         if( false == errordetect )
         {
               
                TotalMoney    = Response[EscrowIndex]+       //Rs 5
                                Response[EscrowIndex+1] +    //Rs 10
                                Response[EscrowIndex+2] +    //Rs 20
                                Response[EscrowIndex+3] +    //Rs 50
                                Response[EscrowIndex+4] +    //Rs 100
                                Response[EscrowIndex+8] +    //Rs 200
                                Response[EscrowIndex+5] +    //Rs 500
                                Response[EscrowIndex+6] +    //Rs 1000
                                Response[EscrowIndex+7] ;    //Rs 2000

                //++DisplayEscrowTableV2(Response,EscrowIndex);
                return TotalMoney;
                            
 

         }
         else if( true == errordetect )
         {

                for(Counter=EscrowIndex;Counter<= (EscrowIndex+8) ;Counter++)
                {
                       if(DoubleDleIndex!=Counter)
                       {
                             Money[MoneyCounter]=Response[Counter];
                             MoneyCounter++;
                       }

                }
                
                 TotalMoney =  Money[0] + //Rs  5
                               Money[1] + //Rs  10
                               Money[2] + //Rs  20
                               Money[3] + //Rs  50
                               Money[4] + //Rs  100
                               Money[5] + //Rs  500
                               Money[6] + //Rs  1000
                               Money[7] + //Rs  2000
                               Money[8] ; //Rs  200
                           

                 //++DisplayEscrowTableV2(Money,0);
                 return TotalMoney;

         }


}//GetEscrowMoneyPatchV2() end

/*
void DisplayEscrowTableV2(unsigned char *currentnote,int EscrowIndex){


                 

                 unsigned char log[200];
                 memset(log,'\0',200);
                 sprintf(log,"[DisplayEscrowTableV2()] INR 5=%d INR 10=%d INR 20=%d INR 50=%d INR 100=%d INR 500=%d INR 1000=%d",  currentnote[EscrowIndex+0],
                 currentnote[EscrowIndex+1],
                 currentnote[EscrowIndex+2],
                 currentnote[EscrowIndex+3],
                 currentnote[EscrowIndex+4],
                 currentnote[EscrowIndex+5],
                 currentnote[EscrowIndex+6]
                 );

                 //writeFileLoglog);

                 




}//DisplayEscrowTableV2() end
*/

/*
void GetEscrowMoneyPatch(JCM_ESCROW_NOTE *currentnote,unsigned char *Response,int EscrowIndex){
         
         int Counter=0,DoubleDle=0,DoubleDleIndex=0,MoneyCounter=0;
         unsigned char Money[7];
         memset(Money,'\0',7);
         bool errordetect=false;

         
           
           //EscrowIndex   = Rs 5
           //EscrowIndex+1 = Rs 10
           //EscrowIndex+2 = Rs 20
           //EscrowIndex+3 = Rs 50
           //EscrowIndex+4 = Rs 100
           //EscrowIndex+5 = Rs 500
           //EscrowIndex+6 = Rs 1000

           //user defined Money[] index
           //Index 0 = Rs 5
           //Index 1 = Rs 10
           //Index 2 = Rs 20
           //Index 3 = Rs 50
           //Index 4 = Rs 100
           //Index 5 = Rs 500
           //Index 6 = Rs 1000
       
       

         unsigned char log[100];


         //Detect Error
         for(Counter=EscrowIndex;Counter<= (EscrowIndex+6) ;Counter++)
         {

             if( 0x10 == Response[Counter] )
             {
                   DoubleDle++;
                   if(DoubleDle>=2)
                   {
                        errordetect=true;
                        DoubleDleIndex=Counter;
                        memset(log,'\0',100);
                        sprintf(log,"[GetEscrowMoneyPatch()] Error Double Byte Recvd. Index Value = %d",DoubleDleIndex);
                        //writeFileLoglog);

                   }
             }
          
         }

         if( false == errordetect )
         {
               
                currentnote->AcceptedNumber_INR_5    = Response[EscrowIndex];
                currentnote->AcceptedNumber_INR_10   = Response[EscrowIndex+1];
                currentnote->AcceptedNumber_INR_20   = Response[EscrowIndex+2];
                currentnote->AcceptedNumber_INR_50   = Response[EscrowIndex+3];
	        currentnote->AcceptedNumber_INR_100  = Response[EscrowIndex+4];
	        currentnote->AcceptedNumber_INR_500  = Response[EscrowIndex+5];
	        currentnote->AcceptedNumber_INR_1000 = Response[EscrowIndex+6];
                DisplayEscrowTable(currentnote);
                return;
                            
 

         }
         else if( true == errordetect )
         {

                for(Counter=EscrowIndex;Counter<= (EscrowIndex+7) ;Counter++)
                {
                       if(DoubleDleIndex!=Counter)
                       {
                             Money[MoneyCounter]=Response[Counter];
                             MoneyCounter++;
                       }

                }
                
                currentnote->AcceptedNumber_INR_5 = Money[0];
                currentnote->AcceptedNumber_INR_10 = Money[1];
                currentnote->AcceptedNumber_INR_20 = Money[2];
                currentnote->AcceptedNumber_INR_50 = Money[3];
	        currentnote->AcceptedNumber_INR_100 = Money[4];
	        currentnote->AcceptedNumber_INR_500 = Money[5];
	        currentnote->AcceptedNumber_INR_1000= Money[6];
                DisplayEscrowTable(currentnote);
                return;

         }



}//GetEscrowMoneyPatch() end
*/

void GetEscrowMoneyPatch(JCM_ESCROW_NOTE *currentnote,unsigned char *Response,int EscrowIndex) {
         
         int Counter=0,DoubleDle=0,DoubleDleIndex=0,MoneyCounter=0;
         unsigned char Money[9];
         memset(Money,'\0',9);
         bool errordetect=false;

         //EscrowIndex   = Rs 5
         //EscrowIndex+1 = Rs 10
         //EscrowIndex+2 = Rs 20
         //EscrowIndex+3 = Rs 50
         //EscrowIndex+4 = Rs 100
         //EscrowIndex+5 = Rs 500
         //EscrowIndex+6 = Rs 1000
         //EscrowIndex+7 = Rs 2000
         //EscrowIndex+8 = Rs 200
         //++user defined Money[] index
         //Index 0 = Rs 5
         //Index 1 = Rs 10
         //Index 2 = Rs 20
         //Index 3 = Rs 50
         //Index 4 = Rs 100
         //Index 5 = Rs 500
         //Index 6 = Rs 1000
         //Index 7 = Rs 2000
         //Index 8 = Rs 200
       
         unsigned char log[100];

         //++Detect Error
         for(Counter=EscrowIndex;Counter<= (EscrowIndex+8) ;Counter++){

             if( 0x10 == Response[Counter] ){
				 
                   DoubleDle++;
                   if(DoubleDle>=2){
					   
                        errordetect=true;
                        DoubleDleIndex=Counter;
                        memset(log,'\0',100);
                        sprintf(log,"[GetEscrowMoneyPatch()] Error Double Byte Recvd. Index Value = %d",DoubleDleIndex);
                        AVRM_writeFileLogV2(log,INFO,g_BNAId);

                   }//if end
             }//if end
          
         }//if end

         if( false == errordetect ){
             AVRM_writeFileLogV2("[GetEscrowMoneyPatch()] No Error Detect",INFO,g_BNAId);

         }else{
             AVRM_writeFileLogV2("[GetEscrowMoneyPatch()] Error Detect",INFO,g_BNAId);
         }//else end

         if( false == errordetect ){
               
                currentnote->AcceptedNumber_INR_5    = Response[EscrowIndex];
                currentnote->AcceptedNumber_INR_10   = Response[EscrowIndex+1];
                currentnote->AcceptedNumber_INR_20   = Response[EscrowIndex+2];
                currentnote->AcceptedNumber_INR_50   = Response[EscrowIndex+3];
				currentnote->AcceptedNumber_INR_100  = Response[EscrowIndex+4];
				currentnote->AcceptedNumber_INR_500  = Response[EscrowIndex+5];
				currentnote->AcceptedNumber_INR_1000 = Response[EscrowIndex+6];
                currentnote->AcceptedNumber_INR_2000 = Response[EscrowIndex+7];
                currentnote->AcceptedNumber_INR_200  = Response[EscrowIndex+8];
                AVRM_writeFileLogV2("[GetEscrowMoneyPatch()] Escrow Table for No Error Detect",INFO,g_BNAId);
                DisplayEscrowTable(currentnote);
                return;
                           
         }else if( true == errordetect ) {

                for(Counter=EscrowIndex;Counter<= (EscrowIndex+8) ;Counter++){
					
                       if(DoubleDleIndex!=Counter){
						   
                             Money[MoneyCounter]=Response[Counter];
                             memset(log,'\0',100);
                             sprintf(log,"[GetEscrowMoneyPatch()] Money[MoneyCounter] = %d",MoneyCounter,Money[MoneyCounter]);
                             AVRM_writeFileLogV2(log,INFO,g_BNAId);
                             MoneyCounter++;
                             
                       }//if end

                }//for end
                
                currentnote->AcceptedNumber_INR_5 = Money[0];
                currentnote->AcceptedNumber_INR_10 = Money[1];
                currentnote->AcceptedNumber_INR_20 = Money[2];
                currentnote->AcceptedNumber_INR_50 = Money[3];
				currentnote->AcceptedNumber_INR_100 = Money[4];
				currentnote->AcceptedNumber_INR_500 = Money[5];
				currentnote->AcceptedNumber_INR_1000= Money[6];
                currentnote->AcceptedNumber_INR_2000= Money[7];
                currentnote->AcceptedNumber_INR_200= Money[8];
                AVRM_writeFileLogV2("[GetEscrowMoneyPatch()] Escrow Table for Error Detect",INFO,g_BNAId);
                DisplayEscrowTable(currentnote);
                return;

         }//else if end

}//GetEscrowMoneyPatch() end

/*
void DisplayEscrowTable(JCM_ESCROW_NOTE *currentnote){


             

                  unsigned char log[200];
                  memset(log,'\0',200);
                  sprintf(log,"[DisplayEscrowTable()] INR 5=%d INR 10=%d INR 20=%d INR 50=%d INR 100=%d INR 500=%d INR 1000=%d",  currentnote->AcceptedNumber_INR_5,
                 currentnote->AcceptedNumber_INR_10,
                 currentnote->AcceptedNumber_INR_20,
                 currentnote->AcceptedNumber_INR_50,
                 currentnote->AcceptedNumber_INR_100,
                 currentnote->AcceptedNumber_INR_500,
                 currentnote->AcceptedNumber_INR_1000
                 );

                 //writeFileLoglog);

                 




}//DisplayEscrowTable(JCM_ESCROW_NOTE *currentnote) end
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

//start cash polling
unsigned int  JCM_CashInStart()
{
                 
                  
		  //if any cashin operation not started then start it 
                  if( false == g_JCM_CashInStart )
                  {
				  
                                  int rtcode=-1;
                                  int i=0;
                                  
                                  //Reset JCM
                                  JCM_Reset(20);

                                  //Set Pre Credit Polling Flag
                                  g_EscrowEvent=false;
                                  g_RejectEvent=false;
                                  g_DisableAcceptance=false;
                                  g_stopThread=false;
                                  g_EscrowEventReadComplete=false;
                              
                                                              
				  //clear global var for escrow notes
				  g_currentnote.AcceptedNumber_INR_5 = 0;
				  g_currentnote.AcceptedNumber_INR_10 = 0;
				  g_currentnote.AcceptedNumber_INR_20 = 0;
				  g_currentnote.AcceptedNumber_INR_50 = 0;
				  g_currentnote.AcceptedNumber_INR_100 = 0;
				  g_currentnote.AcceptedNumber_INR_500 = 0;
				  g_currentnote.AcceptedNumber_INR_1000= 0;
                                  
                                  unsigned char Response[100];
				  memset(Response,'\0',100);
	                          rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                  if( Response[3] > 0 )
			          {
				     
		                             printf("\n\n");
					     for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
		                             {
						 printf("\n [JCM_CashInStart()]] Sense Response[%d] = 0x%xh. ",i,Response[i]);

					     }
					    	                     
			          }
                                  

				  //if(1 == rtcode)
                                  if( ( 0x02 == Response[3] ) || ( 0x12 == Response[3] ) )
				  {
				           printf("\n[JCM_CashInStart()] Get Escrow Table success.");
                                           GetEscrowMoneyPatch(&g_currentnote,Response,10);
                                           
                         	  		                 		                   
				  }
				  else
				  {
				          printf("\n[JCM_CashInStart()] Get Escrow Table failed.");
                                          return 0;
				  }

				  //create credit polling thread
				  pthread_t thread1;
				  int  iret1;
				  int returnVal=-1;  
				  pthread_attr_t attr;
				  returnVal = pthread_attr_init(&attr);
				  if(0!=returnVal)
				  return 0;
				  returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
				  if(0!=returnVal)
				  return 0;
				  iret1     = pthread_create( &thread1, &attr,JCM_RSP_Poll,NULL); 
				  if(0!= iret1)
				  return 0;

				  g_JCM_CashInStart=true;
                                  g_PollFlag=false;

				  returnVal = pthread_attr_destroy(&attr);
                                  return 1;
                    }
                    else
                    {

                                 return -1;

                    }

}



//stop cash polling
unsigned int  JCM_CashInEnd(int *LastFare)
{
	       
              
               *LastFare=0;

               //if any cash in operation start then stop it
	       if( true == g_JCM_CashInStart)
	       {
			     
                             g_JCM_CashInStart=false;

                             g_DisableAcceptance=true;
			    
		             printf("\n[JCM_CashInEnd()] Before close poll thread.");
                                                   
                             struct timespec begints, endts,diffts;
                             clock_gettime(CLOCK_MONOTONIC, &begints);

		             while(1)
		             {
		                          
                                          if( true == g_EscrowEvent )
		                          {
		                              clock_gettime(CLOCK_MONOTONIC, &endts);
					      diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                                              //printf("\n\n\n[JCM_CashInEnd()] State found = 0x%xh Seconds = %d.",g_JCM_Current_State,diffts.tv_sec);
                                              printf("\n\n\n[JCM_CashInEnd() if block] Escrow State Found.");
		                              break;


		                          }

                                          clock_gettime(CLOCK_MONOTONIC, &endts);
					  diffts.tv_sec = endts.tv_sec - begints.tv_sec;
					  if( diffts.tv_sec >= 4 )
		                          {
				               //printf("\n\n\n[JCM_CashInEnd()] State found = 0x%xh Seconds = %d.",g_JCM_Current_State,diffts.tv_sec);
                                               printf("\n\n\n[JCM_CashInEnd() Timeout] No Escrow State Found When Stop Credit Polling.");
                                               break;
		                          }


                             }
                             
                             g_PollFlag=false;
		             g_stopThread=true;                              
                             
                             DelayInSeconds( 2 );
		             
                             printf("\n[JCM_CashInEnd()] After close poll thread.");

                             if( true == g_EscrowEvent)
                             {
                                   g_EscrowEvent=false;
                                  *LastFare=g_JCM_Accepted_Moneyvalue;
                                   printf("\n\n\n[JCM_CashInEnd()] Last Accepted Money = %d.",g_JCM_Accepted_Moneyvalue);
                                   
                             }

                             return 1;


	       }
	       else if( true != g_JCM_CashInStart)
	       {

		            return -1;

	       }


}


*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ReadReceptibleState(int Seconds){


	     
            int rtcode=-1;
	    int totalByteRead=1;
	    unsigned char DLE[2]={0x00,0x00};
            unsigned char EOT=0x04;
            unsigned char ENQ=0x05;
            unsigned char ScratchPad=0x00;
            unsigned char log[200];
            memset(log,'\0',200);


	    struct timespec start,end,diff;

            //Send ENQ byte
            //writeFileLog"[ReadReceptibleState()] Now Sending ENQ to JCM .");

            BNA_SendSingleByteToSerialPort(g_HANDLE,ENQ);
         
            //writeFileLog"[ReadReceptibleState()] Now Going to read response byte from JCM. ");

            //Start Timer
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            for(;;)
            {        
			
                         

                         BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&DLE[0],&totalByteRead);

                         BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&DLE[1],&totalByteRead);

		         if( 0x03 == DLE[0] )
		         {
		             
                             memset(log,'\0',200);
                             sprintf(log,"[ReadReceptibleState()] EOT Recieved= 0x%x h",DLE[0]);
                             //writeFileLoglog);  
		             return 0;

		         }

                         if( (0x10 == DLE[0] ) && (0x30 == DLE[1] ) )
		         {
		             
                             memset(log,'\0',200);
                             sprintf(log,"[ReadReceptibleState()] DLE0 Receieved DLE0[0] = 0x%x h  DLE0[1] = 0x%x h",DLE[0],DLE[1]);
                             //writeFileLoglog);  

                             BNA_SendSingleByteToSerialPort(g_HANDLE,EOT);

                             BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&ScratchPad,&totalByteRead);

                             memset(log,'\0',200);
                             sprintf(log,"[ReadReceptibleState()] ScratchPad  = 0x%x h",ScratchPad);
                             //writeFileLoglog); 
                             ScratchPad=0x00;

		             return 1;

		         }

                         //Check timer status
                         clock_gettime(CLOCK_MONOTONIC, &end);
                         diff.tv_sec = end.tv_sec - start.tv_sec;
                         if( diff.tv_sec >= Seconds )
                         {
		                 if( 0x03 == DLE[0] )
				 {
				     
		                     memset(log,'\0',200);
		                     sprintf(log,"[ReadReceptibleState()] EOT Recieved= 0x%x h",DLE[0]);
		                     //writeFileLoglog);  
				     return 0;

				 }
		                 else if( (0x10 == DLE[0] ) && (0x30 == DLE[1] ) )
				 {
				     
		                     memset(log,'\0',200);
		                     sprintf(log,"[ReadReceptibleState()] DLE0 Receieved DLE0[0] = 0x%x h  DLE0[1] = 0x%x h",DLE[0],DLE[1]);
		                     //writeFileLoglog);  
		                     
		                     BNA_SendSingleByteToSerialPort(g_HANDLE,EOT);

                                     BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&ScratchPad,&totalByteRead);
		                     memset(log,'\0',200);
		                     sprintf(log,"[ReadReceptibleState()] ScratchPad  = 0x%x h.",ScratchPad);
		                     //writeFileLoglog); 
                                     ScratchPad=0x00;
				     return 1;

				 }
                                else
                                {
                                      //writeFileLog"[ReadReceptibleState()] Timeout State.");
		                      return 2;    
                                }

                         }



          }

	 


}//ReadReceptibleState() end

static int ReadEnqFromJcmWithinTime(int Seconds){

	     
            int rtcode=-1;
	    int totalByteRead=1;
	    unsigned char enq=0x00;
             
            unsigned char log[200];
            memset(log,'\0',200);
	    struct timespec start,end,diff;

            //Start Timer
            clock_gettime(CLOCK_MONOTONIC, &start);

            for(;;)
            {        
			
                         rtcode=BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);
		         if( 0x05 == enq )
		         {
		             
                             memset(log,'\0',200);
                             sprintf(log,"[ReadEnqFromJcm()] enq= 0x%x h",enq);
                             AVRM_writeFileLogV2(log,DEBUG,g_BNAId);  
		             return 1;
		         }

                         //Check timer status
                         clock_gettime(CLOCK_MONOTONIC, &end);
                         diff.tv_sec = end.tv_sec - start.tv_sec;
                         if( diff.tv_sec >= Seconds )
                         {
                             if( 0x05 == enq )
                                return 1;
                             else
		                return 2;    
                        
                         }



          }

	 


}//ReadEnqFromJcmWithinTime() end

static int ReadEnqFromJcm(){

	     
			 int rtcode=-1;
			 int totalByteRead=1;
			 unsigned char enq=0x00;

             /*
             int try=0;
			 for(try=0;try<1;try++)
			 {
				 
				 rtcode=BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);
				 if( 0x05 == enq )
				 {
					  printf("\n[ReadEnqFromJcm()] enq= 0x%x h",enq);  
					  return 1;
				 }
			  
			} 

            */

            unsigned char log[200];
            memset(log,'\0',200);
	        struct timespec start,end,diff;

            //Start Timer
            clock_gettime(CLOCK_MONOTONIC, &start);

            for(;;)
            {        
			
                         rtcode=BNA_ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);
						 if( 0x05 == enq )
						 {
							 memset(log,'\0',200);
							 sprintf(log,"[ReadEnqFromJcm()] enq= 0x%x h",enq);
							 ////writeFileLoglog);
							 AVRM_writeFileLogV2(log,INFO,g_BNAId);   
							 return 1;
						 }
                         //Check timer status
                         clock_gettime(CLOCK_MONOTONIC, &end);
                         diff.tv_sec = end.tv_sec - start.tv_sec;
                         if( diff.tv_sec >= 1 )
                         {
                             if( 0x05 == enq )
                                return 1;
                             else
								return 2;    
                        
                         }//if end



          }//for end

	 


}//ReadEnqFromJcm() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Get current accepted cashvalue
int JCM_CashIn(int fare,int *State,int *Credit) {
              
	      if( true !=  g_JCM_CashInStart)
	      {
		   //printf("\n[JCM_CashIn()] Creditpolling not started.");
		   return -1;

	      }
	      else if( true ==  g_JCM_CashInStart)
	      {
                      
                      //Escrow Event happened [ g_EscrowEvent=true &&  g_PollFlag=false ]
                      if( (true == g_EscrowEvent ) &&  (false == g_PollFlag) ) 
                      {
                            printf("\n[JCM_CashIn() Escrow State] fare = %d",fare);
                           *Credit=g_JCM_Accepted_Moneyvalue;
                            g_EscrowEvent=false;
                            printf("\n[JCM_CashIn() Escrow State] Now Going to Enable JCM.");
                            printf("\n[JCM_CashIn() Escrow State] Before Enable JCM fare = %d",fare-*Credit);

                            while(false != g_PollFlag);

                            DisableSpecificNotes(fare-*Credit);

                            g_PollFlag=true;

                            return 1;

                      }

                      //Normal State means no escrow state happened [ g_EscrowEvent=false &&  g_PollFlag=false false = g_RejectEvent]
                      if( (false == g_PollFlag) &&  (false == g_EscrowEvent) && (false == g_RejectEvent) )
                      { 
                           printf("\n[JCM_CashIn() Normal State] fare = %d",fare);
                           printf("\n[JCM_CashIn() Normal State] Now Going to Enable JCM.");
                           printf("\n[JCM_CashIn() Normal State]  Before Enable JCM fare = %d",fare);

                           DisableSpecificNotes(fare);

                           g_PollFlag=true;
                           return 1;
                          
                      }
                                                                       

                      //Reject state to standby state and no escrow state happened [ g_RejectEvent=true &&  g_PollFlag=false ]
                      if( (true == g_RejectEvent) &&  (false == g_PollFlag) &&  (false == g_EscrowEvent) )
                      { 
                           printf("\n[JCM_CashIn() Reject To StandBy State] fare = %d",fare);
                           printf("\n[JCM_CashIn() Reject To StandBy State] Now Going to Enable JCM.");
                           g_RejectEvent=false;
                           printf("\n[JCM_CashIn() Reject To StandBy State] Before Enable JCM fare = %d",fare);

                           if( 1 !=ReadEnqFromJcm())
                           DisableSpecificNotes(fare);

                           g_PollFlag=true;
                           return 1;
                          
                      }
                      

	      }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Disable_Acceptance_During_RSPPoll() {

		     int rtcode=-1;
                    
                     unsigned char Commands[JCM_CMD_INSERTION_AUTH_LENGTH]=JCM_INSERTION_AUTHORISED_CMD;

                     //Step 1:Prepare BCC for inhibit command
                     int Counter=0;
					 for(Counter=2; Counter<= (JCM_CMD_INSERTION_AUTH_LENGTH-2) ;Counter++)
					 {
						 
								  Commands[JCM_CMD_INSERTION_AUTH_LENGTH-1]=  Commands [JCM_CMD_INSERTION_AUTH_LENGTH-1] ^ Commands[Counter];

					 }

                     //issue inhibit command
                     rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,JCM_CMD_INSERTION_AUTH_LENGTH);
                     if(1==rtcode)
                     {
           
                              AVRM_writeFileLogV2("[Disable_Acceptance_During_RSPPoll()] Command Issue success.",INFO,g_BNAId);
                             return 1;
                            
                   
                     }
					 //issue disable acceptance command
					 else
                     {
                             AVRM_writeFileLogV2("[Disable_Acceptance_During_RSPPoll()] Command Issue failed.",ERROR,g_BNAId);
                            return 0;
                     }



          

}//Disable_Acceptance_During_RSPPoll() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetDeonm(JCM_ESCROW_NOTE *currentnote,JCM_ESCROW_NOTE *oldnote) {
        
                
                AVRM_writeFileLogV2("[GetDeonm()] oldnote",INFO,g_BNAId);  
                DisplayEscrowTable(oldnote);
                
                AVRM_writeFileLogV2("[GetDeonm()] currentnote",INFO,g_BNAId);  
                DisplayEscrowTable(currentnote);
             
                if( currentnote->AcceptedNumber_INR_5 != oldnote->AcceptedNumber_INR_5 ){
                    AVRM_writeFileLogV2("[GetDeonm()] State Rs 5 Accepted.",INFO,g_BNAId);
					return 5;
                }else if( currentnote->AcceptedNumber_INR_10 != oldnote->AcceptedNumber_INR_10){
                    AVRM_writeFileLogV2("[GetDeonm()] State Rs 10 Accepted.",INFO,g_BNAId);
					return 10;
                }else if( currentnote->AcceptedNumber_INR_20 != oldnote->AcceptedNumber_INR_20) {
					AVRM_writeFileLogV2("[GetDeonm()] State Rs 20 Accepted.",INFO,g_BNAId);
				    return 20;
                }else if( currentnote->AcceptedNumber_INR_50 != oldnote->AcceptedNumber_INR_50){
					AVRM_writeFileLogV2("[GetDeonm()] State Rs 50 Accepted.",INFO,g_BNAId);
					return 50;
               }else if( currentnote->AcceptedNumber_INR_100 != oldnote->AcceptedNumber_INR_100) {
				    AVRM_writeFileLogV2("[GetDeonm()] State Rs 100 Accepted.",INFO,g_BNAId);
		            return 100;
               }else if( currentnote->AcceptedNumber_INR_200 != oldnote->AcceptedNumber_INR_200) {
				    AVRM_writeFileLogV2("[GetDeonm()] State Rs 200 Accepted.",INFO,g_BNAId);
		            return 200;
               }else if( currentnote->AcceptedNumber_INR_500 != oldnote->AcceptedNumber_INR_500){
					AVRM_writeFileLogV2("[GetDeonm()] State Rs 500 Accepted.",INFO,g_BNAId);
		            return 500;
	           }else if( currentnote->AcceptedNumber_INR_1000 != oldnote->AcceptedNumber_INR_1000){
                    AVRM_writeFileLogV2("[GetDeonm()] State Rs 1000 Accepted.",INFO,g_BNAId);
		            return 1000;
               }else if( currentnote->AcceptedNumber_INR_2000 != oldnote->AcceptedNumber_INR_2000){
                    AVRM_writeFileLogV2("[GetDeonm()] State Rs 2000 Accepted.",INFO,g_BNAId);
		            return 2000;
               }else{
                    AVRM_writeFileLogV2("[GetDeonm()] No money accepted.",INFO,g_BNAId);
		            return 0;
               }//else if	
               
}//JCM_GetDenom_From_Escrow() end

/*
//g_JCM_Accepted_Moneyvalue
static unsigned int JCM_GetDenom_From_EscrowState(unsigned char* Response, JCM_ESCROW_NOTE *currentnote){
                                                  
                                                   
             JCM_ESCROW_NOTE oldnote={0,0,0,0,0,0,0};
                                                 
             //save old values
             oldnote.AcceptedNumber_INR_5   =  currentnote->AcceptedNumber_INR_5;
 
             oldnote.AcceptedNumber_INR_10  =  currentnote->AcceptedNumber_INR_10;

             oldnote.AcceptedNumber_INR_20  =  currentnote->AcceptedNumber_INR_20;

             oldnote.AcceptedNumber_INR_50  =  currentnote->AcceptedNumber_INR_50;

             oldnote.AcceptedNumber_INR_100 =  currentnote->AcceptedNumber_INR_100;

             oldnote.AcceptedNumber_INR_500 =  currentnote->AcceptedNumber_INR_500;

             oldnote.AcceptedNumber_INR_1000 = currentnote->AcceptedNumber_INR_1000;
								                   
                                                   
             //Disable state
             if( ( 0x51 == Response[2]  ) &&  ( 0x05 == Response[3]) )
             {
		                                           
                   GetEscrowMoneyPatch(currentnote,Response,7);
             }

             //RSP Standby state with note in escrow 
             else if( ( 0x80 == Response[2]  ) &&  (0x12 == Response[3]) )
             {
                                                          
                   GetEscrowMoneyPatch(currentnote,Response,8);
                                                           
             }
             //Escrow State note present in validator head
             else if( ( 0x55 == Response[2]  ) &&  ( 0x12 == Response[3]) )
             {
                                                          
                   GetEscrowMoneyPatch(currentnote,Response,7);
                                                           
             }
             //RSP Acceptance State note present in validator head
             else if( ( 0x80 == Response[2]  ) && ( 0x05 == Response[3]) )
             {
                                                          
                   GetEscrowMoneyPatch(currentnote,Response,8);
             }

             return ( GetDeonm(currentnote,&oldnote) );
	



}//JCM_GetDenom_From_EscrowState()
*/

static unsigned int JCM_GetDenom_From_EscrowState(unsigned char* Response, JCM_ESCROW_NOTE *currentnote) {
                                                  
             
             AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] Entry.",TRACE,g_BNAId);
                                     
             JCM_ESCROW_NOTE oldnote={0,0,0,0,0,0,0,0,0};
                                                 
             //save old values
             AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] New State Notes",INFO,g_BNAId);  
             DisplayEscrowTable(currentnote);
             
             oldnote.AcceptedNumber_INR_5   =  currentnote->AcceptedNumber_INR_5;
             
             oldnote.AcceptedNumber_INR_10  =  currentnote->AcceptedNumber_INR_10;

             oldnote.AcceptedNumber_INR_20  =  currentnote->AcceptedNumber_INR_20;

             oldnote.AcceptedNumber_INR_50  =  currentnote->AcceptedNumber_INR_50;

             oldnote.AcceptedNumber_INR_100 =  currentnote->AcceptedNumber_INR_100;

             oldnote.AcceptedNumber_INR_200 =  currentnote->AcceptedNumber_INR_200;

             oldnote.AcceptedNumber_INR_500 =  currentnote->AcceptedNumber_INR_500;

             oldnote.AcceptedNumber_INR_1000 = currentnote->AcceptedNumber_INR_1000;

             oldnote.AcceptedNumber_INR_2000 = currentnote->AcceptedNumber_INR_2000;
             
             AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] Old State Notes",INFO,g_BNAId);  
             DisplayEscrowTable(&oldnote);
								                   
                                                   
             //++Disable state
             if( ( 0x51 == Response[2]  ) &&  ( 0x05 == Response[3]) )
             {
				   AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] Disable state .",INFO,g_BNAId);     
				   GetEscrowMoneyPatch(currentnote,Response,7);
             }
             //++RSP Standby state with note in escrow 
             else if( ( 0x80 == Response[2]  ) &&  (0x12 == Response[3]) )
             {
                   AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] RSP Standby state with note in escrow .",INFO,g_BNAId);                                  
                   GetEscrowMoneyPatch(currentnote,Response,8);
                                                           
             }
             //++Escrow State note present in validator head
             else if( ( 0x55 == Response[2]  ) &&  ( 0x12 == Response[3]) )
             {
                   AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] Escrow State note present in validator head.",INFO,g_BNAId);                                     
                   GetEscrowMoneyPatch(currentnote,Response,7);
                                                           
             }
             //++RSP Acceptance State note present in validator head
             else if( ( 0x80 == Response[2]  ) && ( 0x05 == Response[3]) )
             {
                   AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] RSP Acceptance State note present in validator head.",INFO,g_BNAId);                                    
                   GetEscrowMoneyPatch(currentnote,Response,8);
             }//else if end

             AVRM_writeFileLogV2("[JCM_GetDenom_From_Escrow()] Exit.",TRACE,g_BNAId);

             return ( GetDeonm(currentnote,&oldnote) );
	



}//JCM_GetDenom_From_EscrowState()

///////////////////////////Jcm Transmit and Receieve Patterns////////////////////////////////////////////////////////////////////////////////////////////////////

/*


return code:

1 = Success
2 = ENQ send failed
3 = DLE0 recv failed
4 = Command write failed
5 = DLE1 recv failed
6 = EOT write failed


*/

/*

//Make JCM Note acceptor receptible state
static int JCM_TransmitCmdSequence( const int HANDLE,
                                    unsigned char* const JCM_Cmd,
                                    const unsigned int JCM_NumberofBytestobeWrite
                                  )
{
                    
                    int recvbyte=0x00,totalByteRead=0,i=0;

                    unsigned char DLE0[2];
                    memset(DLE0,'\0',2);

                    unsigned char DLE1[2];
                    memset(DLE1,'\0',2);

                    unsigned char log[200];
                    memset(log,'\0',200);
                     
                    int rtcode=-1; 
                    
                    #ifdef JCM_DEBUG
                    for(i=0;i<JCM_NumberofBytestobeWrite;i++)
	            {
	                printf("\n [JCM_TransmitCmdSequence()] JCM_Cmd[%d] = 0x%xh",i,JCM_Cmd[i]);
	            }
                    printf("\n [JCM_TransmitCmdSequence()] ________________________________________________________________");

		    printf("\n [JCM_TransmitCmdSequence()] Transmission Start.");

		    printf("\n [JCM_TransmitCmdSequence()]  Sending enq command .");
                    #endif

                    //clear comport before doing any operation
                    
                    //clear transmit register
                    rtcode=-1;
                    rtcode=ClearTransmitBuffer(HANDLE);
                    if( 1 == rtcode ) //clear transmit register success
                    {
                         #ifdef JCM_DEBUG
                         //writeFileLog"[JCM_TransmitCmdSequence()] clear transmit register success.");
                         #endif

                    }
                    else //clear transmit register failed
                    {
                         #ifdef JCM_DEBUG
                         //writeFileLog"[JCM_TransmitCmdSequence()] clear transmit register failed.");
                         #endif

                    }

                    //clear receive register    
                    rtcode=-1;                           
                    rtcode=ClearReceiveBuffer (HANDLE);
                    if( 1 == rtcode ) //clear receieve register success
                    {
                         #ifdef JCM_DEBUG
                         //writeFileLog"[JCM_TransmitCmdSequence()] clear receieve register success.");
                         #endif

                    }
                    else //clear receieve register failed
                    {
                         #ifdef JCM_DEBUG
                         //writeFileLog"[JCM_TransmitCmdSequence()] clear receieve register failed.");
                         #endif

                    }

		    //Step 1:Send ENQ character
                    rtcode=-1;
		    rtcode=SendSingleByteToSerialPort( HANDLE , JCM_ENQ );
		    if( 1 == rtcode ) //send enq success
                    {
                         #ifdef JCM_DEBUG
                         //writeFileLog"[JCM_TransmitCmdSequence()] Send ENQ Success.");
                         #endif

                    }
                    else //send enq failed now abort
                    {
                         //#ifdef JCM_DEBUG
                         //writeFileLog"[JCM_TransmitCmdSequence()] Send ENQ Failed."); 
                         //#endif
                         return 2;
                      
                    }

                    //Step 2:Delay between after ENQ transmission character send
		    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
		  
                    #ifdef JCM_DEBUG
		    //writeFileLog"[JCM_TransmitCmdSequence()] Read dle0 status.");
                    #endif

		    //Step 3:Wait for DLE0 to start sending command
                    recvbyte=0x00;
                    totalByteRead=0;
                    

                    //Read Only 2 byte
                    for(i=0;i<2;i++)
                    {
                            
		                    rtcode=-1;

		                    rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&DLE0[i],&totalByteRead);

                                    if( 1 == rtcode ) //read com port success
				    {
                                         #ifdef JCM_DEBUG
                                         
                                         memset(log,'\0',200);
                                         sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh success.",i,DLE0[i]);
                                         //writeFileLoglog); 
                                         #endif

				    }
				    else //send com port failed
				    {
				         #ifdef JCM_DEBUG
                                         
                                         memset(log,'\0',200);
                                         sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh failed.",i,DLE0[i]);
                                         //writeFileLoglog); 
                                         #endif
                                      
				    }

		    }
                   
                    //Check DLE0 bytes
                    if( (JCM_DLE0_FIRST_BYTE == DLE0[0]) &&  (JCM_DLE0_SECOND_BYTE == DLE0[1]) )
                    {
                           #ifdef JCM_DEBUG
                           //writeFileLog"[JCM_TransmitCmdSequence()] Read DLE0  success.");
                           #endif
                       
                    }

                    else if( 0x04 == DLE0[0] )
                    {
                           //#ifdef JCM_DEBUG
                           //writeFileLog"[JCM_TransmitCmdSequence()] Read EOT Bytes any comand transmission now aborted.");
                           return 3;
                           //#endif
                       
                    }

                    else //if not DLE0 bytes not recv now abort
                    {
                          //#ifdef JCM_DEBUG
                          //writeFileLog"[JCM_TransmitCmdSequence()] Read DLE0  failed.");
                          //#endif
                          return 3;
                    }
                    
                    #ifdef JCM_DEBUG
		    //writeFileLog"[JCM_TransmitCmdSequence()] send command now.");
                    #endif
                   
		    //Step 4:Now Send Command to jcm
                    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
		    for(i=0;i<JCM_NumberofBytestobeWrite;i++)
		    {
				 
                                  rtcode=-1;
                                  rtcode=SendSingleByteToSerialPort(HANDLE, JCM_Cmd[i]);
                                  
		                  if( 1 == rtcode ) //write com port success
				  {


				  }
				  else //write com port failed
				  {
				     return 4;  
                                     
				  }
                                  

		    }

		    //Step 5: wait for DLE1 for ack recv from jcm
                    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
		    totalByteRead=0;
		    for(i=0;i<2;i++)
                    {
		                  rtcode=-1;

		                  rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&DLE1[i],&totalByteRead);

		                  if( 1 == rtcode ) //read com port success
				  {
                                      #ifdef JCM_DEBUG 

                                      memset(log,'\0',200);
                                      sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh success.",i,DLE1[i]);
                                      //writeFileLoglog); 
                                      
                                      #endif

				  }
				  else //send com port failed
				  {
				       //#ifdef JCM_DEBUG 
                                       
                                       memset(log,'\0',200);
                                       sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh failed.",i,DLE1[i]);
                                       //writeFileLoglog); 

                                       //#endif

				  }
                    }

		    //Check DLE1 bytes
                    if( (JCM_DLE1_FIRST_BYTE == DLE1[0]) &&  (JCM_DLE1_SECOND_BYTE == DLE1[1]) )
                    {
                          #ifdef JCM_DEBUG
                          //writeFileLog"[JCM_TransmitCmdSequence()] Read DLE1 success.");
                          #endif

                    }
                    else //if not DLE0 bytes recv failed abort
                    {
                          //#ifdef JCM_DEBUG
                          //writeFileLog"[JCM_TransmitCmdSequence()] Read DLE1 failed.");
                          //#endif
                          return 5;
                    }

		    #ifdef JCM_DEBUG
		    //writeFileLog"[JCM_TransmitCmdSequence()] Sending eot command .");
                    #endif

		    //Step 6:Now send eot to terminate command sending sequence
                    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
                    rtcode=-1;
		    rtcode=SendSingleByteToSerialPort(HANDLE,JCM_EOT);
                    if( 1 == rtcode ) //write com port success
		    {
                        #ifdef JCM_DEBUG 
                        //writeFileLog"[JCM_TransmitCmdSequence()] Send EOT success.");
                        #endif

		    }
		    else //write com port failed
		    {
		        //#ifdef JCM_DEBUG      
                        //writeFileLog"[JCM_TransmitCmdSequence()] Send EOT failed.");
                        //#endif
                        return 6;
		    }
                    
                    
                    #ifdef JCM_DEBUG
                    //writeFileLog"[JCM_TransmitCmdSequence()] Transmission End.");
		    //writeFileLog"[JCM_TransmitCmdSequence()] ________________________________________________________________");
                    #endif
                  
                    return 1;


}


*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

return code:

1 = Success
2 = ENQ send failed
3 = DLE0 recv failed
4 = Command write failed
5 = DLE1 recv failed
6 = EOT write failed

*/

//++Make JCM Note acceptor receptible state
static int JCM_TransmitCmdSequence( const int HANDLE,unsigned char* const JCM_Cmd,const unsigned int JCM_NumberofBytestobeWrite){
                    
                    int recvbyte=0x00,totalByteRead=0,i=0;

                    unsigned char DLE0[2];
                    memset(DLE0,'\0',2);

                    unsigned char DLE1[2];
                    memset(DLE1,'\0',2);

                    unsigned char log[200];
                    memset(log,'\0',200);
                     
                    int rtcode=-1,counter=1; 
                    bool complete[2];
                    
                    #ifdef JCM_TRANS_TIME_DEBUG
                    struct timespec begints, endts,diffts;
                    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begints);
                    #endif

                    #ifdef JCM_DEBUG
                    for(i=0;i<JCM_NumberofBytestobeWrite;i++){
	                     
                          memset(log,'\0',200);

		                  sprintf(log,"[JCM_TransmitCmdSequence()] JCM_Cmd[%d] = 0x%xh.",i,JCM_Cmd[i]);

		                  AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 

	                }//for end
	                
                    AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] ________________________________________________________________",DEBUG,g_BNAId);

					AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Transmission Start.",DEBUG,g_BNAId);

					AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()]  Sending enq command .",DEBUG,g_BNAId);
                    #endif
                    
                    //Log Tx Data
			        LogDataExchnage( DATA_EXCHANGE_ALL,
				                     g_BNAId, 
				                     "Tx",
					                 JCM_Cmd,
					                 JCM_NumberofBytestobeWrite
				                   );

                    //clear comport before doing any operation
                    
                    //clear transmit register
                    rtcode=-1;
                    rtcode=BNA_ClearTransmitBuffer(HANDLE);
                    if( 1 == rtcode ) //clear transmit register success
                    {
                         #ifdef JCM_DEBUG
                         AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] clear transmit register success.",DEBUG,g_BNAId);
                         #endif

                    }
                    else //clear transmit register failed
                    {
                         #ifdef JCM_DEBUG
                         AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] clear transmit register failed.",DEBUG,g_BNAId);
                         #endif

                    }

                    //clear receive register    
                    rtcode=-1;                           
                    rtcode=BNA_ClearReceiveBuffer (HANDLE);
                    if( 1 == rtcode ) //clear receieve register success
                    {
                         #ifdef JCM_DEBUG
                         AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] clear receieve register success.",DEBUG,g_BNAId);
                         #endif

                    }
                    else //clear receieve register failed
                    {
                         #ifdef JCM_DEBUG
                         AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] clear receieve register failed.",DEBUG,g_BNAId);
                         #endif

                    }

		   
                     //Step 1:Send ENQ character
                     rtcode=-1;
					 rtcode=BNA_SendSingleByteToSerialPort( HANDLE , JCM_ENQ );
					 if( 1 == rtcode ) //send enq success
					 {
							#ifdef JCM_DEBUG
							AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Send ENQ Success.",DEBUG,g_BNAId);
							#endif

					 }
					 else //send enq failed now abort
					 {
							//#ifdef JCM_DEBUG
							AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Send ENQ Failed.",DEBUG,g_BNAId); 
							//#endif
							return 2;
							  
					 }
		    
                    //Step 2:Delay between after ENQ transmission character send
		            counter=1;

                    complete[0]=false,complete[1]=false;

                    while( counter<= JCM_MAX_TRY_FOR_DLE0 ) //total 1 second delay [ 4 times with 250 ms delay]
                    {

                                   
                                JCM_delay_miliseconds(100);

		 	                    //Step 3:Wait for DLE0 to start sending command
								recvbyte=0x00;

								totalByteRead=0;
											
												//Read Only 2 byte
								for(i=0;i<2;i++)
								{
										
										rtcode=-1;

                                        DLE0[i]=0x00;

						                rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&DLE0[i],&totalByteRead);

										if( ( 1 == rtcode ) &&  (DLE0[i] > 0x00) ) //read com port success
										{

														 #ifdef JCM_DEBUG
														 
														 //memset(log,'\0',200);
														 //sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh success.",i,DLE0[i]);
														 ////writeFileLoglog); 

														 #endif
                                                         complete[i]=true;

										}
										else //send com port failed
										{
													 #ifdef JCM_DEBUG
													 
													 //memset(log,'\0',200);
													 //sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh failed.",i,DLE0[i]);
													 ////writeFileLoglog); 

													 #endif

                                                     complete[i]=false;
				                      
						   }


				    }//for dle0 end
                                    
                    if( (true == complete[0]) &&  (true == complete[1]) )
				    {
				           #ifdef JCM_DEBUG
				           AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Read DLE0 two bytes complete.",DEBUG,g_BNAId);
				           #endif
                                           break;
				       
				    }
                                    else //send enq byte again
                                    {

		                               rtcode=-1;
					       rtcode=BNA_SendSingleByteToSerialPort( HANDLE , JCM_ENQ );
					       if( 1 == rtcode ) //send enq success
					       { 
						    #ifdef JCM_DEBUG
						    AVRM_writeFileLogV2("[JCM_TransmitCmdSequence() DLE0 Block] Send ENQ Success.",DEBUG,g_BNAId);
						    #endif

					       }
					       else //send enq failed now abort
					       {
						    //#ifdef JCM_DEBUG
						    AVRM_writeFileLogV2("[JCM_TransmitCmdSequence() DLE0 Block] Send ENQ Failed.",DEBUG,g_BNAId); 
						    //#endif
						    
					       }


                                    }

                                    
                                     
                                    counter++;


                    }//while dle0 end
				   
                    //Check DLE0 bytes
                    if( (JCM_DLE0_FIRST_BYTE == DLE0[0]) &&  (JCM_DLE0_SECOND_BYTE == DLE0[1]) )
                    {
                           #ifdef JCM_DEBUG
                           AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Read DLE0  success.",DEBUG,g_BNAId);
                           #endif
                       
                    }
                    else if( 0x04 == DLE0[0] )
                    {
                           //#ifdef JCM_DEBUG
                           AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Read EOT Bytes any comand transmission now aborted.",DEBUG,g_BNAId);
                           return 3;
                           //#endif
                       
                    }

                    else //if not DLE0 bytes not recv now abort
                    {
                          //#ifdef JCM_DEBUG
                          AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Read DLE0  failed.",DEBUG,g_BNAId);
                          //#endif
                          return 4;
                    }
                    
                    #ifdef JCM_DEBUG
		            AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] send command now.",DEBUG,g_BNAId);
                    #endif
                   
		            //Step 4:Now Send Command to jcm
                   
                  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                  
                 

                   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

                   for(i=0;i<JCM_NumberofBytestobeWrite;i++)
	           {
					 
		                          rtcode=-1;

		                          rtcode=BNA_SendSingleByteToSerialPort(HANDLE, JCM_Cmd[i]);
		                          
				          if( 1 == rtcode ) //write com port success
					  {
                                               continue;

					  }
					  else //write com port failed
					  {
                                               AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] write command byte failed!!.",DEBUG,g_BNAId);
					       return 4;  
		                             
					  }
		                          

	            }

		    //Step 5: wait for DLE1 for ack recv from jcm
                    counter=1;
                    complete[0]=false,complete[1]=false;
                    while(counter<=JCM_MAX_TRY_FOR_DLE1)
                    {
                     
                                    totalByteRead=0;

				    for(i=0;i<2;i++)
				    {

						  rtcode=-1;
                                                
                                                  DLE1[i]=0x00;
						  
                                                  rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&DLE1[i],&totalByteRead);

                                                  if( (1== rtcode) && (DLE1[i] > 0x00) ) //read com port success
						  {

				                      #ifdef JCM_DEBUG 

				                      memset(log,'\0',200);
                                                      sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh success.",i,DLE1[i]);
				                      AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 
				                      
				                      #endif
                                                       
                                                      complete[i]=true;

						  }
						  else //send com port failed
						  {
						       
                                                       //#ifdef JCM_DEBUG 
				                       
				                       //memset(log,'\0',200);
                                                       //sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh failed.",i,DLE1[i]);
				                       ////writeFileLoglog); 

				                       //#endif

						  }

				    }//for() dle1

                                    if( (true == complete[0]) &&  (true == complete[1]) )
				    {
				           #ifdef JCM_DEBUG
				           AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Read DLE1 two bytes complete.",DEBUG,g_BNAId);
				           #endif
                                           break;
				       
				    }

                                    
                                    JCM_delay_miliseconds(100); //10ms delay

                                    counter++;


                    }//while dle1 end

		    //Check DLE1 bytes
                    if( (JCM_DLE1_FIRST_BYTE == DLE1[0]) &&  (JCM_DLE1_SECOND_BYTE == DLE1[1]) )
                    {
                          #ifdef JCM_DEBUG
                          AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Read DLE1 success.",DEBUG,g_BNAId);
                          #endif

                    }
                    else //if not DLE0 bytes recv failed abort
                    {
                          //#ifdef JCM_DEBUG
                          AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Read DLE1 failed.",DEBUG,g_BNAId);
                          //#endif
                          return 5;
                    }

		    #ifdef JCM_DEBUG
		    //writeFileLog"[JCM_TransmitCmdSequence()] Sending eot command .");
                    #endif

		    //Step 6:Now send eot to terminate command sending sequence
                    //JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
                    complete[0]=false,complete[1]=false;
		    while(counter<=8)
                    {
		            rtcode=-1;
			    rtcode=BNA_SendSingleByteToSerialPort(HANDLE,JCM_EOT);
		            if( 1 == rtcode ) //write com port success
			    {
		                #ifdef JCM_DEBUG 
		                AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Send EOT success.",DEBUG,g_BNAId);
		                #endif
                                complete[0]=true;
                                
			    }
			    else //write com port failed
			    {
				//#ifdef JCM_DEBUG      
		                AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Send EOT failed.",DEBUG,g_BNAId);
		                //#endif
		                return 6;
			    }
                            if( true == complete[0]  )
		            {
				           #ifdef JCM_DEBUG
				           AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Send EOT success.",DEBUG,g_BNAId);
				           #endif 
                                           break;
				       
		            }

                            JCM_delay_miliseconds(10); //10ms delay

                            counter++;
                    
                    }//while() eot byte

                    #ifdef JCM_DEBUG
                    AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] Transmission End.",DEBUG,g_BNAId);
		            AVRM_writeFileLogV2("[JCM_TransmitCmdSequence()] ________________________________________________________________",DEBUG,g_BNAId);
                    #endif
                    #ifdef JCM_TRANS_TIME_DEBUG
                    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endts);
                    diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                    diffts.tv_nsec = endts.tv_nsec - begints.tv_nsec;
                    memset(log,'\0',100);
		            sprintf(log,"[JCM_TransmitCmdSequence()] Cycle complete in Seconds Elapsed=%d microsecond=%ld .",diffts.tv_sec,(diffts.tv_nsec/1000UL ) ); 
                    AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                    #endif
                    return 1;


}//JCM_TransmitCmdSequence() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
return code:

1 = Success

2 = ENQ send failed

3 = ENQ recv failed

4 = DLE0 send failed

5 = Command Response recv failed

6 = DLE1 send failed

6 = EOT recv failed

*/

/*

static int JCM_ReceieveCmdReplySequence( const int HANDLE,
                                         unsigned char*  Response,
                                         const unsigned int NumberofBytestobeRead
                                       )
{
       
             
            int i=0,totalByteRead=0;
            unsigned char ENQ=0x00;
            unsigned char EOT=0x00;
            int  Orgtotalbytesread=0;
            unsigned char DLE0[2]={ JCM_DLE0_FIRST_BYTE , JCM_DLE0_SECOND_BYTE };
            unsigned char DLE1[2]={ JCM_DLE1_FIRST_BYTE , JCM_DLE1_SECOND_BYTE };
            int rtcode=-1;
            unsigned char log[200];
            
            #ifdef JCM_DEBUG
            //writeFileLog"[JCM_ReceieveCmdReplySequence()] ________________________________________________________________");
            //writeFileLog"[JCM_ReceieveCmdReplySequence()] Recv command Start.");
	    #endif

        
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            //Step 1: Now wait for ENQ reply from JCM
            int retry=0;
            while(retry <=1) //1 default
            {
		    retry++;
                    #ifdef JCM_DEBUG
                    //writeFileLog"[JCM_ReceieveCmdReplySequence()] Now go for ENQ read from JCM ");
                    #endif
		    rtcode=-1;
		    rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&ENQ,&totalByteRead);

                    

		    //if( 1 == rtcode ) //ENQ byte recv success
                    if( JCM_ENQ == ENQ)
		    {
		         
		         #ifdef JCM_DEBUG
		         
                         memset(log,'\0',200);
                         sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ success ENQ=0x%xh .",ENQ);
                         //writeFileLoglog); 

		         #endif
                         break;      
			

		    }
                    
		    else if( (JCM_ENQ != ENQ ) || (retry >=1) )//ENQ byte recv failed
                    {
                           #ifdef JCM_DEBUG
		           
                           memset(log,'\0',200);
                           sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ failed ENQ=0x%xh .",ENQ);
                           //writeFileLoglog); 

		           #endif
                           return 3;
		         
		    }
             

            }

      
            //Step 2: Send DLE0 Bytes to JCM 
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            #ifdef JCM_DEBUG
            //writeFileLog"[JCM_ReceieveCmdReplySequence()] Now Sending DLE0 bytes to JCM");
            #endif

	    for(i=0;i<2;i++)
            {
                          rtcode=-1;
                          rtcode=SendSingleByteToSerialPort(HANDLE, DLE0[i]);
                          if( 1 == rtcode ) //send DLE0 byte success
		          {
                               #ifdef JCM_DEBUG
                               
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveCmdReplySequence()] Write DLE0[%d] =0x%xh success. ",i,DLE0[i]);
                               //writeFileLoglog); 

                               #endif

		          }
		          else //send DLE0 byte failed
		          {
		               //#ifdef JCM_DEBUG 
                              
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveCmdReplySequence()] Write DLE0[%d] =0x%xh failed. ",i,DLE0[i]);
                               //writeFileLoglog); 

                               //#endif
                               return 4;
		          }
            }
            

            //Step 3: Now Wait for response against any command
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
            retry=0;
            while(retry<=1)
            {
		    
                    retry++;
                    Orgtotalbytesread=0;
		    rtcode=-1;

		    //Read command response packet
		    rtcode=JCM_ReadResponse(HANDLE,Response,NumberofBytestobeRead,&Orgtotalbytesread);
                    break;
 
                    /*

                    int Counter=0;
                    unsigned char BCC=0x00;
                    for( Counter=2; Counter<=(NumberofBytestobeRead-2) ; Counter++)
	            {
                          BCC =   BCC ^ Response[Counter];
                    }
            
                    #ifdef JCM_DEBUG
		    printf("\n[JCM_ReceieveCmdReplySequence() Checksum calculation] Return Packet BCC = 0x%xh Response[%d]= 0x%xh, retry =%d", BCC,NumberofBytestobeRead-1,Response[NumberofBytestobeRead-1],retry);
                    #endif

		    if(BCC !=Response[NumberofBytestobeRead-1])
		    {
				
		                #ifdef JCM_DEBUG
                                printf("\n[JCM_ReceieveCmdReplySequence()] Checksum error retry =%d",retry);
				#endif
                                break;

		    }
		    else if(BCC == Response[NumberofBytestobeRead-1])
                    {
		                #ifdef JCM_DEBUG
                                printf("\n[JCM_ReceieveCmdReplySequence()] Checksum ok BCC =0x%xh Response[%d]= 0x%xh",BCC,NumberofBytestobeRead-1,Response[NumberofBytestobeRead-1]);
                                #endif
		                break;
             
                    }
                    */
		   
            //}//while loop           
/*
	    //Step 4: Now send DLE1 for ack that we receievce response packet properly

            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            #ifdef JCM_DEBUG
            //writeFileLog"[JCM_ReceieveCmdReplySequence()] Now Sending DLE1 bytes to JCM");
            #endif

	    for(i=0;i<2;i++)
            {
                          rtcode=-1;
                          rtcode=SendSingleByteToSerialPort(HANDLE, DLE1[i]);
                          if( 1 == rtcode ) //send DLE1 byte success
		          {


		          }
		          else //send DLE1 byte failed
		          {
		              return 6;
		          }
            }
           
            //delay
	    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            //Step 5: Now Read EOT status from JCM

            #ifdef JCM_DEBUG
            //writeFileLog"[JCM_ReceieveCmdReplySequence()] Now go for EOT read from JCM ");
            #endif
            rtcode=-1;
            totalByteRead=0;
            EOT=0x00;
            for(retry=1;retry <=1;retry ++)
            {           
			    EOT=0x00;
                            rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
			    if( 1 == rtcode ) //read com port success
			    {
				          if( JCM_EOT == EOT ) //read eot status
					  {
				             #ifdef JCM_DEBUG

				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh  success retry=%d. ",EOT,retry);
                                             //writeFileLoglog); 
 
				             #endif
                                             break;
					  }
					  else //read eot status failed
					  {
					     
                                             #ifdef JCM_DEBUG  
				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh  failed. retry=%d",EOT,retry);
                                             //writeFileLoglog); 
 
				             #endif
				             continue; 
 
					  }

			    }
            }
         
            #ifdef JCM_DEBUG
            //writeFileLog"[JCM_ReceieveCmdReplySequence()] Recv command End.");
	    //writeFileLog"[JCM_ReceieveCmdReplySequence()] ________________________________________________________________");
            #endif
          
            return 1;
  
            

}



*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

return code:

1 = Success

2 = ENQ send failed

3 = ENQ recv failed

4 = DLE0 send failed

5 = Command Response recv failed

6 = DLE1 send failed

6 = EOT recv failed

*/

static int JCM_ReceieveCmdReplySequence( const int  HANDLE,unsigned char* Response,const unsigned int NumberofBytestobeRead ) {
       
             
            int i=0,totalByteRead=0;
            unsigned char ENQ=0x00;
            unsigned char EOT=0x00;
            int  Orgtotalbytesread=0;
            unsigned char DLE0[2]={ JCM_DLE0_FIRST_BYTE , JCM_DLE0_SECOND_BYTE };
            unsigned char DLE1[2]={ JCM_DLE1_FIRST_BYTE , JCM_DLE1_SECOND_BYTE };
            int rtcode=-1;
            unsigned char log[200];
            int counter=1;

            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_ReceieveCmdReplySequence()] ________________________________________________________________",DEBUG,g_BNAId);
            AVRM_writeFileLogV2("[JCM_ReceieveCmdReplySequence()] Recv command Start.",DEBUG,g_BNAId);
	        #endif

            //begin time
            #ifdef JCM_RECV_TIME_DEBUG 
            struct timespec begints,endts,diffts;
            struct timespec enqbegints,enqendts,enqdiffts;
            struct timespec resbegints,resendts,resdiffts;
            struct timespec eotbegints,eotendts,eotdiffts;
         
            //start cycle time
            clock_gettime(CLOCK_MONOTONIC, &begints);

            #endif

            //Step 1: Now wait for ENQ reply from JCM
            #ifdef JCM_RECV_TIME_DEBUG 
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &enqbegints);
            #endif
            counter=1;
            while( counter <= 8 )  //wait for enq 80ms
            {

				    #ifdef JCM_DEBUG
				    memset(log,'\0',100);
                    sprintf(log,"[JCM_ReceieveCmdReplySequence()] Now go for ENQ read from JCM  loop counter=%d ." ,counter); 
				    AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
				    #endif
				    rtcode=-1;
                                    ENQ=0x00;
                                    totalByteRead=0;
				    rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&ENQ,&totalByteRead);

				    //ENQ byte recv success
				    if( JCM_ENQ == ENQ)
				    {
					 
					 #ifdef JCM_RECV_TIME_DEBUG 
					 
				         memset(log,'\0',200);
				         sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ success ENQ=0x%xh .",ENQ);
				         AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 

					     clock_gettime(CLOCK_MONOTONIC, &enqendts);
				         enqdiffts.tv_nsec = enqendts.tv_nsec - enqbegints.tv_nsec;
		                         memset(log,'\0',100);
				         sprintf(log,"[JCM_ReceieveCmdReplySequence()] Enq Recv in millisecond=%ld .",(enqdiffts.tv_nsec/1000000) ); 
				         AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                                         #endif
				         break;      
			

				    }
		                    else if( (JCM_ENQ != ENQ ) && ( 8 == counter ) ) //ENQ byte recv failed
				    {
				           #ifdef JCM_DEBUG
					   
				           memset(log,'\0',200);
				           sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ failed ENQ=0x%xh .",ENQ);
				           AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 

					   #endif
				           return 3;
					 
				    }

                                    JCM_delay_miliseconds(10); //10ms delay

                                    counter++;
                                
             

            }//enq while end

      
            //Step 2: Send DLE0 Bytes to JCM 
            #ifdef JCM_RECV_TIME_DEBUG 
            AVRM_writeFileLogV2("[JCM_ReceieveCmdReplySequence()] Now Sending DLE0 bytes to JCM",DEBUG,g_BNAId);
            #endif

	    for(i=0;i<2;i++)
            {

                          rtcode=-1;
                          rtcode=BNA_SendSingleByteToSerialPort(HANDLE, DLE0[i]);
                          if( 1 == rtcode ) //send DLE0 byte success
		          {
                               #ifdef JCM_RECV_TIME_DEBUG 
                               
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveCmdReplySequence()] Write DLE0[%d] =0x%xh success. ",i,DLE0[i]);
                               AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 

                               #endif

		          }
		          else //send DLE0 byte failed
		          {
		               //#ifdef JCM_DEBUG 
                              
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveCmdReplySequence()] Write DLE0[%d] =0x%xh failed. ",i,DLE0[i]);
                               AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 

                               //#endif
                               return 4;
		          }



            }//DLE0 for loop
            

            //Step 3: Now Wait for response against any command
            counter=1;
            rtcode=JCM_ReadResponse(HANDLE,Response,NumberofBytestobeRead,&Orgtotalbytesread);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                  
                  //++Log Rx Data
			LogDataExchnage( DATA_EXCHANGE_ALL,
				             g_BNAId, 
				             "Rx",
					         Response,
					         Orgtotalbytesread
				           );

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

            if( (NumberofBytestobeRead == Orgtotalbytesread) && (1==rtcode) )
            {
           
            }
            else if( (NumberofBytestobeRead != Orgtotalbytesread) && (1 !=rtcode) )
            {
		                  
                        memset(log,'\0',100);
		           
		        sprintf(log,"[JCM_ReceieveCmdReplySequence()] response reply bytes recv failed due to NumberofBytestobeRead and Orgtotalbytesread not matched."); 
                        AVRM_writeFileLogV2(log,DEBUG,g_BNAId);

		        return 5; //command response recv failed

            }
         
            //Step 4: Now send DLE1 for ack that we receievce response packet properly
            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_ReceieveCmdReplySequence()] Now Sending DLE1 bytes to JCM",DEBUG,g_BNAId);
            #endif

	    for(i=0;i<2;i++)
            {

                          rtcode=-1;
                          rtcode=BNA_SendSingleByteToSerialPort(HANDLE, DLE1[i]);
                          if( 1 == rtcode ) //send DLE1 byte success
		          {
                               continue;
                              
		          }
		          else //send DLE1 byte failed
		          {
                               
                               memset(log,'\0',100);
		           
		               sprintf(log,"[JCM_ReceieveCmdReplySequence()] DLE1[%d] = 0x%xh. write failed.",i,DLE1[i]); 

		               AVRM_writeFileLogV2(log,DEBUG,g_BNAId);

		               return 6;

		          }


            }//for dle1 write
           
            
            //Step 5: Now Read EOT status from JCM

            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_ReceieveCmdReplySequence()] Now go for EOT read from JCM ",DEBUG,g_BNAId);
            #endif
            rtcode=-1;
            totalByteRead=0;
            EOT=0x00;
            counter=1;
            #ifdef JCM_RECV_TIME_DEBUG 
            clock_gettime(CLOCK_MONOTONIC, &eotbegints);
            #endif
            while(counter<=100) //EOT WAIT MAX 1 SECOND AFTER DLE1 SEND
            { 
          
			    EOT=0x00;
                            rtcode=-1;
                            totalByteRead=0;
                            rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
                            
                            //read com port success
			    if( ( 1 == rtcode )  && (JCM_EOT == EOT) )
			    {

                                           #ifdef JCM_RECV_TIME_DEBUG 
                                           memset(log,'\0',200);
                                           sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh .",EOT);
                                           AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                                           
                                           clock_gettime(CLOCK_MONOTONIC, &eotendts);
				           eotdiffts.tv_nsec = eotendts.tv_nsec - eotbegints.tv_nsec;
		                           memset(log,'\0',100);
				           sprintf(log,"[JCM_ReceieveCmdReplySequence()] eot bytes Recv in millisecond=%ld .",(eotdiffts.tv_nsec/1000000) ); 
                                           AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                                           #endif
                                           break;
                           }
                           else if( ( 100 == counter )  && (JCM_EOT != EOT) ) 
                           {
					     
                                             //#ifdef JCM_DEBUG  
				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh  failed.",EOT);
                                             AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
 
				             //#endif

				             return 6; //eot read failed


                          }
                          
                          JCM_delay_miliseconds(10); //10ms delay
                          counter++; 
                          continue;

                           
			    


            }//eot read while block
         
            #ifdef JCM_RECV_TIME_DEBUG 
            clock_gettime(CLOCK_MONOTONIC, &endts);
            diffts.tv_sec = endts.tv_sec - begints.tv_sec;
            diffts.tv_nsec = endts.tv_nsec - begints.tv_nsec;
            memset(log,'\0',100);
            sprintf(log,"[JCM_ReceieveCmdReplySequence()] Cycle complete in Seconds Elapsed=%d millisecond=%ld .",diffts.tv_sec,(diffts.tv_nsec/1000000UL ) ); 
            AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
            #endif

            #ifdef JCM_DEBUG
            //writeFileLog"[JCM_ReceieveCmdReplySequence()] Recv command End.",DEBUG,g_BNAId);
	        //writeFileLog"[JCM_ReceieveCmdReplySequence()] ________________________________________________________________",DEBUG,g_BNAId);
            #endif
          
            return 1;
  
            

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int JCM_ReceieveReplyWithoutReadENQ( const int HANDLE,unsigned char*  Response,const unsigned int NumberofBytestobeRead) {
       
            unsigned char log[200];
            int i=0,totalByteRead=0;
            unsigned char ENQ=0x00;
            unsigned char EOT=0x00;
            int  Orgtotalbytesread=0;
            unsigned char DLE0[2]={ JCM_DLE0_FIRST_BYTE , JCM_DLE0_SECOND_BYTE };
            unsigned char DLE1[2]={ JCM_DLE1_FIRST_BYTE , JCM_DLE1_SECOND_BYTE };
            int rtcode=-1;
            
            #ifdef JCM_DEBUG
             AVRM_writeFileLogV2("[JCM_ReceieveReplyWithoutReadENQ()] ________________________________________________________________",DEBUG,g_BNAId);
             AVRM_writeFileLogV2("[JCM_ReceieveReplyWithoutReadENQ()] Recv command Start.",DEBUG,g_BNAId);
	    #endif

        
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 
      
            //Step 1: Send DLE0 Bytes to JCM 
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_ReceieveReplyWithoutReadENQ()] Now Sending DLE0 bytes to JCM",DEBUG,g_BNAId);
            #endif

	    for(i=0;i<2;i++)
            {
                          rtcode=-1;
                          rtcode=BNA_SendSingleByteToSerialPort(HANDLE, DLE0[i]);
                          if( 1 == rtcode ) //send DLE0 byte success
		          {
                               #ifdef JCM_DEBUG
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveReplyWithoutReadENQ()] Write DLE0[%d] =0x%xh success. ",i,DLE0[i]);
                               AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 
 
                               #endif

		          }
		          else //send DLE0 byte failed
		          {
		               //#ifdef JCM_DEBUG 
                               
                               memset(log,'\0',200);
                               sprintf(log,"\n[JCM_ReceieveReplyWithoutReadENQ()] Write DLE0[%d] =0x%xh failed. ",i,DLE0[i]);
                               AVRM_writeFileLogV2(log,DEBUG,g_BNAId); 
 
                               //#endif
                               return 4;
		          }
            }
            

            //Step 2: Now Wait for response against any command
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
            int retry=0;
            while(retry<=1)
            {
		    
                    retry++;
                    Orgtotalbytesread=0;
		    rtcode=-1;

		    //Read command response packet
		    rtcode=JCM_ReadResponse(HANDLE,Response,NumberofBytestobeRead,&Orgtotalbytesread);
                    break;
 
            }


	    //Step 3: Now send DLE1 for ack that we receievce response packet properly

            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_ReceieveReplyWithoutReadENQ()] Now Sending DLE1 bytes to JCM",DEBUG,g_BNAId);
            #endif

	    for(i=0;i<2;i++)
            {
                          rtcode=-1;
                          rtcode=BNA_SendSingleByteToSerialPort(HANDLE, DLE1[i]);
                          if( 1 == rtcode ) //send DLE1 byte success
		          {


		          }
		          else //send DLE1 byte failed
		          {
		              return 6;
		          }
            }
           
            //delay
	    JCM_delay_miliseconds(50); 

            //Step 4: Now Read EOT status from JCM

            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_ReceieveReplyWithoutReadENQ()] Now go for EOT read from JCM ",DEBUG,g_BNAId);
            #endif
            rtcode=-1;
            totalByteRead=0;
            EOT=0x00;
            for(retry=1;retry <=2;retry ++)
            {           
			    EOT=0x00;
                            rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
			    if( 1 == rtcode ) //read com port success
			    {
				          if( JCM_EOT == EOT ) //read eot status
					  {
				             #ifdef JCM_DEBUG
				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveReplyWithoutReadENQ()] Read EOT status =0x%xh  success retry=%d.",EOT,retry);
                                             AVRM_writeFileLogV2(log,DEBUG,g_BNAId);

				             #endif
                                             break;

					  }
					  else //read eot status failed
					  {
					     
                                             #ifdef JCM_DEBUG  
				            
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveReplyWithoutReadENQ()] Read EOT status =0x%xh  failed. retry=%d",EOT,retry);
                                             AVRM_writeFileLogV2(log,DEBUG,g_BNAId);

				             #endif

				             continue;  

					  }

			    }
            }
         
            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_ReceieveReplyWithoutReadENQ()] Recv command End.",DEBUG,g_BNAId);
	        AVRM_writeFileLogV2("[JCM_ReceieveReplyWithoutReadENQ()] ________________________________________________________________",DEBUG,g_BNAId);
            #endif
          
            return 1;
  
            

}

//++Complete cycle if byte recv not properly
static int JCM_Complete_ReceieveCmdReplySequence(int HANDLE){
            
            

            int rtcode=-1,i=0;
            int totalByteRead=0;
            unsigned char EOT=0x00;
            unsigned char DLE0[2]={ JCM_DLE0_FIRST_BYTE , JCM_DLE0_SECOND_BYTE };
            unsigned char DLE1[2]={ JCM_DLE1_FIRST_BYTE , JCM_DLE1_SECOND_BYTE };

            //Step 1: Now send DLE1 for ack that we receievce response packet properly

            #ifdef JCM_DEBUG
            //printf("\n[JCM_Complete_ReceieveCmdReplySequence()] Now Sending DLE1 bytes to JCM");
            #endif

	    for(i=0;i<2;i++)
            {
                          
                          rtcode=-1;
                          rtcode=BNA_SendSingleByteToSerialPort(HANDLE, DLE1[i]);
                          if( 1 == rtcode ) //send DLE1 byte success
		          {


		          }
		          else //send DLE1 byte failed
		          {
		              return 6;
		          }

            }
           
            //delay
	    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            //Step 6: Now Read EOT status from JCM
            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("[JCM_Complete_ReceieveCmdReplySequence()] Now go for EOT read from JCM ",DEBUG,g_BNAId);
            #endif
            rtcode=-1;
            totalByteRead=0;
            EOT=0x00;
            rtcode=BNA_ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
            if( 1 == rtcode ) //read com port success
            {
                          if( JCM_EOT == EOT ) //read eot status
		          {
                             #ifdef JCM_DEBUG
                             //AVRM_writeFileLogV2("\n[JCM_Complete_ReceieveCmdReplySequence()] Read EOT status =0x%xh  success. ",EOT);
                             #endif
		          }
		          else //read eot status failed
		          {
		             //#ifdef JCM_DEBUG  
                             //printf("\n[JCM_Complete_ReceieveCmdReplySequence()] Read EOT status =0x%xh  failed. ",EOT);
                             //#endif
                             return 7;  
		          }
            }
            else //read port failed
            {
		return 7;                

            }

            #ifdef JCM_DEBUG
            AVRM_writeFileLogV2("\n[JCM_Complete_ReceieveCmdReplySequence()] Recv command End.",DEBUG,g_BNAId);
	    AVRM_writeFileLogV2("\n[JCM_Complete_ReceieveCmdReplySequence()] ________________________________________________________________",DEBUG,g_BNAId);
            #endif
          
            return 1;
 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Milliseconds delay [value must be 0-900]
static void JCM_delay_miliseconds(const long long int milisec) {
        
        
        unsigned char log[100];

        memset(log,'\0',100);

        struct timespec req = {0},rim={0};

	req.tv_sec = 0;

	req.tv_nsec = milisec * 1000000L; //[0 .. 999999999] otherwise return error

        int rtcode =0;

        /*

        clock_nanosleep return 0 on success

        CLOCK_MONOTONIC  A nonsettable, monotonically increasing clock that
                        measures time since some unspecified point in the
                        past that does not change after system startup.'

        */

        rtcode =clock_nanosleep( CLOCK_MONOTONIC,
                                 0,
                                 &req,
                                 NULL
                               );
        
        if( 0 == rtcode )   
	{
	      
                //memset(log,'\0',100);
                //sprintf(log,"[JCM_delay_miliseconds()] nanosleep() system call successed with return code  %d .",rtcode); 
                ////writeFileLoglog);
                return;
	      

	}
        else
        {
	      
                memset(log,'\0',100);
                sprintf(log,"[JCM_delay_miliseconds()] nanosleep() system call failed with return code  %d .",rtcode); 
                AVRM_writeFileLogV2(log,INFO,g_BNAId);
                return;
	      

	}
        

        

         
}//JCM_delay_miliseconds() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++read jcm command response packet
static int JCM_ReadResponse( int HANDLE,unsigned char* recvbyte,const int  totalBytestoberead,int* const Orgtotalbytesread) {
	   
            

            struct timespec start,end,diff;

            int Counter=0;
            unsigned char log[200];             
            int rcvstat=-1;
            int bytecounter=0;
            int totalByteread=0,totalbytein=0;
            char Recvbyte=0x00;
            

            //Start Timer
            clock_gettime(CLOCK_MONOTONIC, &start);

	    for(;;)
            {        
			
                        rcvstat=-1;
                        totalByteread=0;
                        Recvbyte=0x00;

                        //Read com port
                        totalByteread = read(HANDLE,&Recvbyte,sizeof(char));

                        if( 1 == totalByteread )
                        {
                             
                             recvbyte[bytecounter]=Recvbyte;
                             bytecounter++;

                             //Set bytes original read by this function
                             *Orgtotalbytesread=bytecounter;

                             #ifdef JCM_DEBUG
                             memset(log,'\0',200);
                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Recvbyte= 0x%xh",Recvbyte);
                             AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                             #endif
                        }
                        
                        if(totalBytestoberead == bytecounter)
                        {
                             clock_gettime(CLOCK_MONOTONIC, &end);
                             diff.tv_sec = end.tv_sec - start.tv_sec;
                             #ifdef JCM_DEBUG
                             memset(log,'\0',200);
                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Expected byte read complete %d Seconds",diff.tv_sec);
                             AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                             for( Counter=0; Counter< bytecounter ; Counter++)
			     {
		                   memset(log,'\0',100);
                                   sprintf(log,"[JCM_ReceieveCmdReplySequence()] Response[%d]= 0x%xh",Counter,recvbyte[Counter]);
                                   AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
		             }
		    
                             #endif
                             return 1;
                        }

                        //Check timer status
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        diff.tv_sec = end.tv_sec - start.tv_sec;

                        if( diff.tv_sec >= JCM_RESPONSE_TIME )
                        {
                            
		                    if(totalBytestoberead == bytecounter)
		                    {
		                         #ifdef JCM_DEBUG
                                        
                                         memset(log,'\0',200);
                                         sprintf(log,"[JCM_ReceieveCmdReplySequence()] Expected byte read complete  time elapsed %d Seconds",diff);
                                         AVRM_writeFileLogV2(log,DEBUG,g_BNAId);
                                         #endif
		                         return 1;
		                    }
		                    else
		                    {
		                         #ifdef JCM_DEBUG
                                         
                                         memset(log,'\0',200);
                                         sprintf(log,"[JCM_ReceieveCmdReplySequence()] Expected byte read failed time elapsed %d Seconds",diff);
                                        AVRM_writeFileLogV2(log,DEBUG,g_BNAId);

                                         #endif

                                         return 2;
		                    }
		                    
                        
                       }



           }// End for(;;)

     


}//end


//1= success
//or return code by transmit or recv reply
int JCM_GetReply( unsigned int CmdLength, unsigned int Resposnesize, unsigned char *Response, unsigned char *Commands, int Delay){

            
            int Counter=0,rtcode=-1;

            memset(Response,'\0',Resposnesize);

            unsigned int NumberofBytestobeRead=Resposnesize;

            //Step 1:Prepare BCC for command
			for(Counter=2;Counter<= (CmdLength-2);Counter++)
			{
			   Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
			}
             
          
            #ifdef JCM_DEBUG
            for(Counter=0;Counter<CmdLength;Counter++)
			{
				//printf("\n [JCM_GetReply()] Commands[%d] = 0x%xh",Counter,Commands[Counter]);
			}

			//printf("\n[JCM_GetReply()] Comamnd Packet BCC =0x%xh",Commands[CmdLength-1]);
            #endif         
            //Step 2:Transmit Command
			rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);
            if(1==rtcode)
            {
           
                    JCM_delay_miliseconds(Delay); 
                    rtcode=-1;
                    //Step 3:Receieve Reply
                    rtcode=JCM_ReceieveCmdReplySequence(g_HANDLE,Response,NumberofBytestobeRead);
                    if(1==rtcode)
                    {
                          return 1; //success
                    }
                    else
                    {
                         AVRM_writeFileLogV2("[JCM_GetReply()] Receieve sequence failed.",DEBUG,g_BNAId);
                         return 3; //recv sequence failed
                          
                    }
 
            }
            else
            {
                     AVRM_writeFileLogV2("[JCM_GetReply()] Transmission sequence failed.",DEBUG,g_BNAId);
                     return 2; //transmit sequence failed
            } 


}

//++Return BCC of any packet
unsigned char GetBCC(unsigned char* Packet,int StartIndex,unsigned int Packetlength){
        
           
            int Counter=0;

            unsigned char BCC=0x00;
            
            for( Counter=StartIndex; Counter<=(Packetlength-2) ; Counter++)
	    {
                  //printf("\n [CheckReplyPacketBCC()] Packet[%d] = 0x%x h",Counter,Packet[Counter]);
 	          BCC =   BCC ^ Packet[Counter];
            }
            
            return BCC;

        
}

int JCM_Receieve_RSP_Packet( unsigned char*  Response,const unsigned int NumberofBytestobeRead){
            int rtcode=-1;
            rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,NumberofBytestobeRead);
            return rtcode;
}//

///////////////////////////////////////////  Start: JCM Command Section ////////////////////////////////////////////////////////////////////////////////////////////////

//JCM_RESET_CMD                   
//JCM_SENSE_CMD                   
//JCM_INSERTION_AUTHORISED_CMD     
//JCM_INSERTION_INHIBIT_CMD       
//JCM_RECEIPT_CMD                 
//JCM_RETURN_CMD		       
//JCM_INTAKE_CMD       

int JCM_Cmd_Reset(int HANDLE){ 
                

                int rtcode=-1,Counter=0;

                int CmdLength=JCM_CMD_CMN_LENGTH;

                unsigned char Commands[ JCM_CMD_CMN_LENGTH ]=JCM_RESET_CMD;

                //Prepare BCC for reset command
				for(Counter=2;Counter<= (CmdLength-2);Counter++)
				{
					   Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
				}
		     

                rtcode = JCM_TransmitCmdSequence( HANDLE, Commands,JCM_CMD_CMN_LENGTH);

                return rtcode;

  
}

int JCM_Cmd_Sense( unsigned char *Response,const int ResponseLength,const unsigned int Delay ){ 
                
                int rtcode=-1;

                unsigned char Commands[ JCM_CMD_CMN_LENGTH ]=JCM_SENSE_CMD;
                
 
                rtcode=JCM_GetReply(JCM_CMD_CMN_LENGTH ,ResponseLength,Response,Commands,Delay);
                
                return rtcode;
  
}

int JCM_Cmd_InsertionAuthorized( unsigned char DefaultEnablebyte ,unsigned char *Response,const int ResponseLength,const unsigned int Delay ){ 

                    int rtcode=-1,Counter=0;

		    int CmdLength=0;
        
                    unsigned char log[100];

                    memset(log,'\0',100);

		    unsigned int NumberofBytestobeRead=ResponseLength;

                    unsigned char Commands[ JCM_CMD_INSERTION_AUTH_LENGTH ]=JCM_INSERTION_AUTHORISED_CMD;

		    unsigned char DittoCommands[ JCM_CMD_DITTO_INSERTION_AUTH_LENGTH ]=JCM_INSERTION_AUTHORISED_DITTO_CMD;
                
                    #ifdef JCM_DEBUG
		    memset(log,'\0',100);

                    sprintf(log,"[JCM_Cmd_InsertionAuthorized()] DefaultEnablebyte =0x%xh.",DefaultEnablebyte);

                    //writeFileLoglog);
                    #endif
	
		    memset(Response,'\0',ResponseLength);
		    

		    if(0x10 == DefaultEnablebyte)
		    {
			    
				    //Set MoneyCode byte
		                    DittoCommands[ 3 ]=DefaultEnablebyte;

				    CmdLength=JCM_CMD_DITTO_INSERTION_AUTH_LENGTH;

				    //Prepare BCC 
				    for(Counter=2;Counter<= (CmdLength-2);Counter++)
				    {
					 if(4 == Counter)
					 continue;
					 else
					 DittoCommands[CmdLength-1]=  DittoCommands[CmdLength-1] ^ DittoCommands[Counter];
				    }
                                    #ifdef JCM_DEBUG 
                                    for(Counter=0;Counter<CmdLength;Counter++)
				    {
					 memset(log,'\0',100);

                                         sprintf(log,"[JCM_Cmd_InsertionAuthorized()] DittoCommands[%d] = 0x%xh.",Counter,DittoCommands[Counter]);
                                         //writeFileLoglog);
				    }

                                    memset(log,'\0',100);

                                    sprintf(log,"[JCM_Cmd_InsertionAuthorized()] DittoComamnd Packet BCC =0x%xh.",DittoCommands[CmdLength-1]);
                                    //writeFileLoglog);
				
				    #endif
			    
		    }

		    else if(0x10!= DefaultEnablebyte)
		    {
                     
				    //Set MoneyCode byte
		                    Commands[ 3 ]=DefaultEnablebyte;

				    CmdLength=JCM_CMD_INSERTION_AUTH_LENGTH;

				    //Prepare BCC 
				    for(Counter=2;Counter<= (CmdLength-2);Counter++)
				    {
					 
					 Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
				    
				    }
                                    #ifdef JCM_DEBUG
				    for(Counter=0;Counter<CmdLength;Counter++)
				    {
					  
                                          memset(log,'\0',100);

                                          sprintf(log,"[JCM_Cmd_InsertionAuthorized()] Commands[%d] = 0x%xh.",Counter,Commands[Counter]);
                                          //writeFileLoglog);

				    }

				
                                    memset(log,'\0',100);

                                    sprintf(log,"[JCM_Cmd_InsertionAuthorized()] Comamnd Packet BCC =0x%xh.",Commands[CmdLength-1]);

                                    //writeFileLoglog);
				    #endif
		    
		    }

                    if(0x10== DefaultEnablebyte)
                    {
                           #ifdef JCM_DEBUG
			   //writeFileLog"[JCM_Cmd_InsertionAuthorized()] Enablebyte 0x10 found.");
                           #endif
                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,DittoCommands,CmdLength);

	            }
	            else if(0x10 != DefaultEnablebyte)
	            {
                            #ifdef JCM_DEBUG
			    //writeFileLog"[JCM_Cmd_InsertionAuthorized()] Enablebyte 0x10 not found.");
                            #endif
			    rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,CmdLength);

		    }

		    if(1==rtcode)
		    {
           
				  JCM_delay_miliseconds(Delay); 
				  rtcode=-1;
				  rtcode=JCM_ReceieveCmdReplySequence(g_HANDLE,Response,NumberofBytestobeRead);
				  if(1==rtcode)
				  {
					  return 1; //success
				  }
				  else
				  {
					  return 3; //recv sequence failed
                          
				  }
 
			}
			else
			{
				return 2; //transmit sequence failed
			} 


               
              
}

int JCM_Cmd_InsertionInhibited( unsigned char *Response,const int ResponseLength,const unsigned int Delay ){ 
            
                
                int rtcode=-1;

                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_INSERTION_INHIBIT_CMD;

                rtcode=JCM_GetReply(JCM_CMD_CMN_LENGTH,ResponseLength,Response,Commands,Delay);

                return rtcode;


}

int JCM_Cmd_Receipt( unsigned char *Response,const int ResponseLength,const unsigned int Delay ){ 
               
               
 
                //unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                //memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                //unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_RECEIPT_CMD;

                //JCM_GetReply(JCM_CMD_CMN_LENGTH,30,Response,Commands,JCM_CMD_DELAY);
              
}

int JCM_Cmd_Return( unsigned char *Response,const int ResponseLength,const unsigned int Delay ){ 
                
                int rtcode=-1;

                int CmdLength=JCM_CMD_CMN_LENGTH;

                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_RETURN_CMD;
              
                rtcode = JCM_GetReply(JCM_CMD_CMN_LENGTH,ResponseLength,Response,Commands,Delay);
               
                return rtcode;


}

int JCM_Cmd_Intake( unsigned char *Response,const int ResponseLength,const unsigned int Delay ){ 
                
                //unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                //memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                //unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_INTAKE_CMD;

                //JCM_GetReply(JCM_CMD_CMN_LENGTH,30,Response,Commands,JCM_CMD_DELAY);
               
}//int JCM_Cmd_Intake(unsigned char *Response,const int ResponseLength,const unsigned int Delay) end

///////////////////////////////////////////  End: JCM Command Section ////////////////////////////////////////////////////////////////////////////////////////////////


#endif


