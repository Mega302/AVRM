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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pthread_mutex_t g_NAEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  g_NAEnableThreadCond = PTHREAD_COND_INITIALIZER;

static  pthread_mutex_t g_NoteLowLevelPollFlagStatusmutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_stopThreadmutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_PollFlagmutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_AlreadyCoinDisablemutex=PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Generic Note Acceptor Initilzation 
int OpenCashAcptr(int noteAcptrPortNmbr)
{

      return (JCM_Activate(noteAcptrPortNmbr));


}



int Deactivate_JCM_NoteAcptr()
{

     return (JCM_Deactivate());


}


//Generic Note Acceptor Device Status
int GetNoteAcptrStatus()
{

      return (JCM_GetStatus());

}


int UnloadCashAcptr()
{

    //InDirect Call to unload operation
    return( JCM_Unload() );


}


int  DispenseCashAcptr()
{

     return ( JCM_Dispense() );

}


//Start Note Acceptor Credit Poll
unsigned int  NoteAcptrStartCreditPollThread()
{

                  
      return ( JCM_NoteAcptrCreatePollThread());


}


inline void  CloseNoteAcceptorPollThread()
{
       
       JCM_CloseNoteAcceptorPollThread();

       return;

}


inline void InitNATransModel()
{
 
        JCM_Atvm_InitCashInStartThread();

        return;

}


inline void IssuedisableAllBillCmd()
{
 
       //JCM_delay_miliseconds(200);
       DisableAcceptance();
       return;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//wait for note acceptor low level poll thread closed status
unsigned int WaitforNoteAcptrLowlevelflagClosed( double waittime )
{

	 
	 time_t start,end,diff;
	 int rtcode=-1;
         unsigned char log[100];
         memset(log,'\0',100); 
	 
         //#ifdef JCM_DEBUG
         writeFileLog("[WaitforNoteAcptrLowlevelflagClosed()]  Get Low level flag status starting..");
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
					     writeFileLog(log);

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
                                          writeFileLog(log);
                                          //#endif	
				  
                                          rtcode=1;
					}
					else
					{ 
					   //#ifdef JCM_DEBUG

                                           memset(log,'\0',100);

                                           sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Low level flag status is not closed during time delay wait. %d .",diff);
                                           writeFileLog(log);

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
              writeFileLog("[WaitforNoteAcptrLowlevelflagClosed()]  Already Low level flag status is closed.");
              //#endif
	      return 1;

	 }

         pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

       ////////////////////////////////////////////////////////////////////////////////////////////////////////

         

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int  JCM_Atvm_CashInStart()
{
                 
                                  
                                  int rtcode=-1;
                                  int i=0;
                                  char log[200];
                                  memset(log,'\0',200);
                                  
                                  
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

                                  #ifdef JCM_DEBUG

                                  if( Response[3] > 0 )
			          {
				     
		                             
					     for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
		                             {
						
                                                 memset(log,'\0',200);
                                                 sprintf(log,"[JCM_Atvm_CashInStart()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
			                         writeFileLog(log);
			       
					     }
					    	                     
			          }

                                  #endif

                                  if( ( (0x40 == Response[2]) && (0x02 == Response[3]) ) || 
                                      ( (0x40 == Response[2]) && (0x03 == Response[3]) ) ||
                                      ( (0x40 == Response[2]) && (0x13 == Response[3]) ) ||
                                      ( (0x40 == Response[2]) && (0x12 == Response[3]) )
                                    )
				  {
				           writeFileLog("[JCM_Atvm_CashInStart()] Get Escrow Table success.");
                                           GetEscrowMoneyPatch(&g_currentnote,Response,10);
                                           return 1;
                         	  		                 		                   
				  }
				  else
				  {
				          writeFileLog("[JCM_Atvm_CashInStart()] Get Escrow Table failed.");
                                          return 0;
				  }



                              
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//api ENABLE =1 and DISABLE=0
int enableSpecificBillsAndMakeIdle(  int fare,                  //API fare (in)
                                     int maxNoteQtyAllowed      //API Max no of cash allow number (in)
                                  )
{

	 
         
         //Disable all notes by default
	 NOTE notevar={0,0,0,0,0,0,0};

	 
         //Analysis if exact fare is given
         #ifdef JCM_DEBUG
         writeFileLog("[enableSpecificBillsAndMakeIdle()] Before Exact fare analysis block.");
         #endif


         #ifdef JCM_DEBUG

         //Make a log about fare and max no of cash
         char log[200];
         memset(log,'\0',200);
         sprintf(log,"[enableSpecificBillsAndMakeIdle()] Max No of Cash = %d And fare = %d",maxNoteQtyAllowed,fare);       
         writeFileLog(log);

         #endif

	 if(1 == maxNoteQtyAllowed)
	 {

                   g_ExactFareFlag=true;

		   //#ifdef JCM_DEBUG
                   writeFileLog("[enableSpecificBillsAndMakeIdle()] Exact fare block Active [Cash:1].");
                   //#endif

                   switch(fare)
		   {

			     case 5:
			          notevar.INR_5    = 1;
                                  #ifdef JCM_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 5 only enable.");
                                  #endif
			          break;

			     case 10:
                                  
			          notevar.INR_10   = 1;
                                  #ifdef JCM_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 10 only enable.");
                                  #endif
			          break;

			     case 20:
			          notevar.INR_20  = 1;
                                  #ifdef JCM_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 20 only enable.");
                                  #endif
			          break;

			     case 50:
			          notevar.INR_50   = 1;
                                  #ifdef JCM_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 50 only enable.");
                                  #endif
			          break;

			     case 100:
			          notevar.INR_100  = 1;
                                  #ifdef JCM_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 100 only enable.");
                                  #endif 
			          break;

			     case 500:
			          notevar.INR_500  = 1;
                                  #ifdef JCM_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 only enable.");
                                  #endif
			          break;

			     case 1000:
			         notevar.INR_1000 = 1;
                                 #ifdef JCM_DEBUG
                                 writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 1000 only enable.");
                                 #endif
			         break;

                             default: writeFileLog("[enableSpecificBillsAndMakeIdle()] No fare given.");
                                      break;

	           } 


	   }

           else if( maxNoteQtyAllowed > 1 ) //Exact fare not given
           {
                  
                  g_ExactFareFlag=false;

                  //#ifdef JCM_DEBUG
                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Max Cash Block Active [Cash:20].");
                  //#endif

           
                  if(fare >= 5)
                  {
                        
                        notevar.INR_5      = 1;

                        #ifdef JCM_DEBUG
                        writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 5 Enable.");
                        #endif
                  }

                  if(fare >= 10)
                  {
                        notevar.INR_10    = 1;

                        #ifdef JCM_DEBUG
                        writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 10 Enable.");
                        #endif
                  }

                  if(fare >= 20)
                  {
                       notevar.INR_20     = 1;

                       #ifdef JCM_DEBUG
                       writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 20 Enable.");
                       #endif
                  }

                  if(fare >= 50)
                  {
                      notevar.INR_50     = 1;

                      #ifdef JCM_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 50 Enable.");
                      #endif
                  }

                  if(fare >= 100)
                  {
                      notevar.INR_100    = 1;

                      #ifdef JCM_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 100 Enable.");
                      #endif

                  }

                  if(fare >= 500)
                  {
                      
                      notevar.INR_500    = 1;
                      #ifdef JCM_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif

                  }
                  if(fare >= 1000)
                  {
                      
                      notevar.INR_1000   = 1;

                      #ifdef JCM_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif

                  }

           }


           #ifdef JCM_DEBUG
	   writeFileLog("[enableSpecificBillsAndMakeIdle()] After Exact fare analysis block.");
           #endif
        
           //Now Disable Notes
           #ifdef JCM_DEBUG
           writeFileLog("[enableSpecificBillsAndMakeIdle()] Now going to enable note type.");
           #endif
	  
           if( ( maxNoteQtyAllowed > 1 ) || ( 1 == maxNoteQtyAllowed  ) )
           {
             
                   writeFileLog("[enableSpecificBillsAndMakeIdle()] maxNoteQtyAllowed in between range 1-20 so going to enable jcm.");

		   //store current jcm escrow table
		   JCM_Atvm_CashInStart();

		   //now going to enable jcm
		   return ( EnableDenom(notevar) );

           }
           else if( ( maxNoteQtyAllowed < 0 ) || ( 0 == maxNoteQtyAllowed  ) )
           {

                   writeFileLog("[enableSpecificBillsAndMakeIdle()] maxNoteQtyAllowed is not between range 1-20 so going to disable jcm.");

                   int rtcode=-1;
                   rtcode=DisableAcceptance();
                   if( (1 == rtcode) || (2 == rtcode) )
                     return 1;
                   else
                     return (-1);

           }





}


inline int SetCurrentfare(int Remainbalance)
{

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



}//inline int SetCurrentfare(int Remainbalance) end




inline void SetSpecificNoteInhibitStatus(int fare)
{
	  

           pthread_mutex_lock(&g_NASetSpecificFaremutex);
 
           writeFileLog("[SetSpecificNoteInhibitStatus()] Set External fare.");

           g_NACurrentAtvmFare=0;

	   g_SpecificNoteInhibitFlag=1;
	   
           g_NACurrentAtvmFare=fare;

           pthread_mutex_unlock(&g_NASetSpecificFaremutex);

           return;

	      
}//inline void SetSpecificNoteInhibitStatus(int fare) end



inline void SetCoinFullDisableFlag(int maxNoOfCash , int maxNoOfCoin)
{
     
          
          if(maxNoOfCash >= 1 && maxNoOfCash <= 20 ) //if note acceptor enable then going to set or unset of coin acceptor control
          {
		  if( 0 == maxNoOfCoin )//coin acceptor is diable due to maxnocoin is zero
		  {
		       g_CoinDisableflag=1; //set (user predefined control / out of control )
		       //writeFileLog("[SetCoinFullDisableFlag()] Runtime Coin Control Disabled due to maxnoofcoin is zero.");
		  }
		  else if( maxNoOfCoin >= 1 && maxNoOfCoin <= 20 ) //coin acceptor is enable due to maxnocoin range is valid(1-20)
		  {
		       g_CoinDisableflag=0; //default (user runtime control in control )
		       //writeFileLog("[SetCoinFullDisableFlag()] Runtime Coin Control Enabled.");
		  }

                  return;
          }
          else if( maxNoOfCash <= 0) //if note acceptor disable then no need to control coin acceptor
          {
                g_CoinDisableflag=1; //set (user predefined control / out of control )
		//writeFileLog("[SetCoinFullDisableFlag()] Runtime Coin Control Disabled due to maxnoofcash value less than 1.");
       
                return;
          }


}//inline void SetCoinFullDisableFlag(int maxNoOfCash , int maxNoOfCoin) end





inline int SetAlreadyCoinInhibit()
{
   
     g_AlreadyCoinDisable=false;

     return 0;


}//inline int SetAlreadyCoinInhibit() end



inline int SetCurrent_JCM_State(int CurrentState)
{
   
   
   pthread_mutex_lock(&g_JCM_State_Mutex);

   g_JCM_Current_State=0x00;
 
   g_JCM_Current_State=CurrentState;

   pthread_mutex_unlock(&g_JCM_State_Mutex);

   return 0;
   


}//inline int SetCurrent_JCM_State(int CurrentState) end


inline unsigned int GetCurrent_JCM_State()
{
   
   
	   int CurrentState=0x00;

	   unsigned char log[100];

	   pthread_mutex_lock(&g_JCM_State_Mutex);
	 
	   CurrentState=g_JCM_Current_State;

           #ifdef JCM_DEBUG

	   memset(log,'\0',100);

	   sprintf(log,"[GetCurrent_JCM_State()] Current State = 0x%x h.",g_JCM_Current_State);

	   writeFileLog(log);

           #endif

	   g_JCM_Current_State=0x00;

	   pthread_mutex_unlock(&g_JCM_State_Mutex);

	   return CurrentState;



}//inline unsigned int GetCurrent_JCM_State() end



inline unsigned int GetCurrent_JCM_Accepted_Denom()
{
       
	       
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
		     writeFileLog(log);
                     #endif

	       }

	       pthread_mutex_unlock(&g_JCM_Escrow_State_Mutex);
	     
	      
	      
	       return  AcceptedDenom;
	      
 
}//inline unsigned int GetCurrent_JCM_Accepted_Denom() end



//api
int GetNoteDetailInEscrow()
{


		  unsigned int Current_state=0x00;

		  unsigned int Current_Accepted_Denom=0x00;

	   
		  //make a log about to be accept fare
		  char log[200];
		  memset(log,'\0',200);

		  #ifdef JCM_DEBUG
		  pthread_mutex_lock(&g_FaretobeacceptMutex);
		  memset(log,'\0',200);
		  sprintf(log,"[GetNoteDetailInEscrow()] Fare to be accept Rs. : %d .", g_faretobeaccept);
		  writeFileLog(log);
		  pthread_mutex_unlock(&g_FaretobeacceptMutex);
		  #endif
		  

                  /////////////////////////////////////////////////////////////////////////////////////////////

		  pthread_mutex_lock(&g_FaretobeacceptMutex);

		  if(  (5    == g_faretobeaccept)  ||
		       (10   == g_faretobeaccept)  ||
		       (20   == g_faretobeaccept)  ||
		       (50   == g_faretobeaccept)  ||
		       (100  == g_faretobeaccept)  ||
		       (500  == g_faretobeaccept)  ||
		       (1000 == g_faretobeaccept)   
		    )
		 {
		        
		        g_SingleNoteAcceptEvent=true;
		        //#ifdef JCM_DEBUG
		        //writeFileLog("[GetNoteDetailInEscrow()] Single Note Event Found.");  
		        //#endif
		 }
		 else
		 {
		        
		        g_SingleNoteAcceptEvent=false;
		        //#ifdef JCM_DEBUG
		        //writeFileLog("[GetNoteDetailInEscrow()] Single Note Event Not Found."); 
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
                                    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] Accepting State.");
                                    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] Start Coin Inhibit block.");
                                    //#endif

                                    #ifdef COIN_ACCEPTOR
		                    //Lock coin acceptor                            
		                    SetAllCoinInhibitStatus();
		                    g_AlreadyCoinDisable=true;
                                    #endif

                                    //#ifdef JCM_DEBUG
				    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] End Coin Inhibit block.");                          
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
                                     writeFileLog("[GetNoteDetailInEscrow() while loop] Start Release Coin Acceptor block.");
                                     //#endif

                                     #ifdef COIN_ACCEPTOR
		                     NaCoinEnableSet(g_faretobeaccept);
		                     g_AlreadyCoinDisable=false;
                                     #endif

                                     //#ifdef JCM_DEBUG
		                     writeFileLog("[GetNoteDetailInEscrow() while loop] End Release Coin Acceptor block.");
                                     //#endif

		              }   
 
                              //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);
                              return 0;
                          
                           }
			   //check note stacked or not stacked
			   else if( 0x0F == Current_state ) 
			   { 
				     
				     //#ifdef JCM_DEBUG
				     writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Note Stacked.");  
				     //#endif

				     //get current accepted denom        
				     Current_Accepted_Denom=GetCurrent_JCM_Accepted_Denom();

				     //make a log about accepted money

				     //#ifdef JCM_DEBUG
				     memset(log,'\0',200);
				     sprintf(log,"[GetNoteDetailInEscrow() if block] Accepted Note Rs. : %d .", Current_Accepted_Denom);
				     writeFileLog(log);
				     //#endif
				   
				  
				     //if accept money is remaining balance then no need to enable coin so return with success
				     if(  (Current_Accepted_Denom == g_faretobeaccept )        && 
				          (true == g_SingleNoteAcceptEvent)                    && 
				          (true == g_AlreadyCoinDisable)                       && 
				          ( 0   == g_CoinDisableflag )
				      )
				     {
					       
				                //#ifdef JCM_DEBUG
				                writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Start Accepted Note Amount is equal to fare to be accepted block.");
				                //#endif
				                #ifdef COIN_ACCEPTOR
						g_AlreadyCoinDisable=false;
				                #endif

				                //#ifdef JCM_DEBUG
				                writeFileLog("[GetNoteDetailInEscrow() Stacked if block] End Accepted Note Amount is equal to fare to be accepted block.."); 
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
				           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Start Free Coin Poll flag block.");
				           //#endif
				              
				           #ifdef COIN_ACCEPTOR
				           NaCoinEnableSet(g_faretobeaccept- Current_Accepted_Denom); 
				           g_AlreadyCoinDisable=false; 
				           #endif

				           //#ifdef JCM_DEBUG
				           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] End Free Coin Poll flag block.");
				           //#endif

				           //pthread_mutex_unlock(&g_AlreadyCoinDisablemutex);
				           return Current_Accepted_Denom ; //success


				     }
				     else 
				     {
				           
				           //#ifdef JCM_DEBUG
				           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Normal path Now time to return from here.");
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





inline unsigned int InhibitNoteAcptr()
{
    
    //writeFileLog("[InhibitNoteAcptr()] Before Set Disable Acceptance.");
    pthread_mutex_lock(&g_NASetDisableAcceptance);
    g_DisableAcceptance=true;
    pthread_mutex_unlock(&g_NASetDisableAcceptance);
    //writeFileLog("[InhibitNoteAcptr()] After Set Disable Acceptance.");
    return 0;


}//InhibitNoteAcptr() end


//Call this function to set thread close status
inline void  JCM_CloseNoteAcceptorPollThread()
{

      
      writeFileLog("[JCM_CloseNoteAcceptorPollThread()] Before set stop jcm credit polling.");

      pthread_mutex_lock( &g_stopThreadmutex );

      pthread_mutex_lock( &g_PollFlagmutex );

      g_stopThread=true;

      g_PollFlag=false;

      pthread_mutex_unlock( &g_stopThreadmutex );

      pthread_mutex_unlock( &g_PollFlagmutex );

      writeFileLog("[JCM_CloseNoteAcceptorPollThread()] After set stop jcm credit polling.");

      return;
                

}//JCM_CloseNoteAcceptorPollThread() end



unsigned int  JCM_NoteAcptrCreatePollThread()
{
		  
                  
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



unsigned int  JCM_Atvm_InitCashInStartThread()
{
                 
                
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

                                  return 0;


                              

}//JCM_Atvm_InitCashInStartThread() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void  NoteAcptrStartSignalCreditPollThread()
{


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



inline void  NoteAcptrStopSignalCreditPollThread()
{


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






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
				    writeFileLog(log);
				    #endif
				    rtcode=-1;
                                    totalByteRead=0;
                                    ENQ=0x00;
                                    rtcode=ReceiveSingleByteFromSerialPort(g_HANDLE,&ENQ,&totalByteRead);

				    //ENQ byte recv success
				    if( JCM_ENQ == ENQ)
				    {
					 
					 #ifdef JCM_RECV_TIME_DEBUG 
					 
				         memset(log,'\0',200);
				         sprintf(log,"[JCM_Read_ENQ()()] read ENQ success ENQ=0x%xh .",ENQ);
				         writeFileLog(log); 
                                         clock_gettime(CLOCK_MONOTONIC, &enqendts);
				         enqdiffts.tv_nsec = enqendts.tv_nsec - enqbegints.tv_nsec;
		                         memset(log,'\0',100);
				         sprintf(log,"[JCM_Read_ENQ()] Enq Recv in millisecond=%f .",(enqdiffts.tv_nsec/1000000) ); 
				         writeFileLog(log);
                                         #endif
				         return 1;     
			

				    }
		                    else if( loopvar == counter ) //ENQ byte recv failed
				    {
				           
                                           #ifdef JCM_DEBUG
					   memset(log,'\0',200);
				           sprintf(log,"[JCM_Read_ENQ()] read ENQ failed ENQ=0x%xh .",ENQ);
				           writeFileLog(log);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool GetNADisableAcceptanceSignal()
{

       bool rtcode=false;

       pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );

       rtcode = g_NADisableAcceptanceFlag ;

       pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );

       return rtcode;


}


bool GetNANoteAcceptanceStatus()
{

       bool rtcode=false;

       pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

       rtcode = g_NA_NoteAcceptanceFlag ;
 
       pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );

       return rtcode;


}//end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline int JCM_Wait(unsigned int Second)
{
		   

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

		                  writeFileLog(log);

		                  #endif

		                  if(diffts.tv_sec>=Second)
		                  {

		                     return 1;

		                  }


                 }//while loop end


}//Wait() end here


///////////////////////////////////////////////////////////////////////////////////////////////////////////


static int IssueEscrowCommand()
{
         
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

            writeFileLog(log);

            if(1==rtcode)
            {
									   
	        writeFileLog("[IssueEscrowCommand()] Escrow Command Transmission sequence success.");

                writeFileLog("[IssueEscrowCommand()] Now go for Escrow Command reply bytes.");

                rtcode= -1;

                rtcode= JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_ESCROW_REPLY_SIZE) ;

                memset(log,'\0',100);

                sprintf(log,"[IssueEscrowCommand()]  JCM_ReceieveCmdReplySequence return code= %d .",rtcode);

                writeFileLog(log);


                if( Response[2] > 0x00 )
                {

                       memset(log,'\0',100);

		       sprintf(log,"[IssueEscrowCommand()] Credit Polling Command = 0x%xh .",Response[2]);

		       writeFileLog(log);

		       memset(log,'\0',100);

		       sprintf(log,"[IssueEscrowCommand()] Credit Polling State = 0x%xh .",Response[3]);

		       writeFileLog(log);

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
                writeFileLog("[IssueEscrowCommand()] Escrow Command Transmission sequence failed due to eot byte receieved.");
                return 0;
                                                                                
            }


}//IssueEscrowCommand() end



static int EscrowIssueEOTPatch()
{


      unsigned char Response[100];

      memset(Response,'\0',100);

      unsigned char log[200];

      memset(log,'\0',200);
      
      int rtcode=-1;

      writeFileLog("[EscrowIssueEOTPatch()] Before close com port for jcm note acceptor."); 

      
      //first disconnect com port now 
      if( 1 == JCM_Deactivate() )
      {
           
           writeFileLog("[EscrowIssueEOTPatch()] close com port successfully done."); 

           g_HANDLE=-1;
 
           writeFileLog("[EscrowIssueEOTPatch()] Before open com port for jcm note acceptor."); 

           rtcode=OpenPort(g_JcmPortNumber, &g_HANDLE);

           writeFileLog("[EscrowIssueEOTPatch()] After open com port for jcm note acceptor."); 
           
	   if( (1==rtcode) && (-1!=g_HANDLE) )
	   {
	                           
                                 writeFileLog("[EscrowIssueEOTPatch()] open com port for jcm successfully done."); 
	                                 
                                 writeFileLog("[EscrowIssueEOTPatch()] Before issue jcm sense command ."); 

                                 //issue sense command
                                 rtcode =-1;

                                 memset(Response,0xf,100);

		                 rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                 writeFileLog("[EscrowIssueEOTPatch()] After issue jcm sense command ."); 
                                         
                                 if( Response[3] >= 0x00 )
			         {
                                      writeFileLog("[EscrowIssueEOTPatch()] Sense issue  command successfully run."); 

	                         }//if( Response[3] >= 0x00 )end
                                 else
                                 {

                                      writeFileLog("[EscrowIssueEOTPatch()] jcm sense command issue failed."); 

                                      writeFileLog("[EscrowIssueEOTPatch()] Now Wait for 20 second."); 

                                      //wait for 1 minute
                                      JCM_Wait(20);

                                      rtcode =-1;

                                      memset(Response,'\0',100);

                                      writeFileLog("[EscrowIssueEOTPatch()]  Before now again issue sense command."); 

		                      rtcode = JCM_Cmd_Sense(Response,
                                                     JCM_SENSE_REPLY_SIZE,
                                                     JCM_RECV_DELAY);   

                                      writeFileLog("[EscrowIssueEOTPatch()]  After issue sense command."); 
  
                                  }
 
                                  //finaly check sense command reply bytes                                  
                                  if( Response[3] >= 0x00 )
                                  {

		                          int i=0;

				          for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
					  {
						
				              memset(log,'\0',200);

				              sprintf(log,"[EscrowIssueEOTPatch()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
				              writeFileLog(log);

		 
				          }

                                          ////////////////////////////////////////////////////////////////
		                           
		                          memset(log,'\0',200);


				          sprintf(log,"[EscrowIssueEOTPatch()] Sense Command = 0x%xh. Current State = 0x%xh.",Response[2],Response[3]);

				          writeFileLog(log);


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
                                    writeFileLog("[EscrowIssueEOTPatch()] Sense command issue failed.");

                                    return 0; 

                                }



	   }
	   else
	   {
	        writeFileLog("[EscrowIssueEOTPatch()]  open com port for jcm note acceptor failed.");
                return 0; 

           }

     }//if( 1 == JCM_Deactivate() ) end
     else
     {

          writeFileLog("[EscrowIssueEOTPatch()] close com port failed to done."); 
          return 0;

     }


}//end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//cash polling thread
static void* JCM_RSP_Poll(void *ptr)
{



 
                /////////////////////////////////////////////////////////////////////////////////////////////

                //wait for signal

                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_wait( &g_NAEnableThreadCond , &g_NAEnableThreadmutex );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );


                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_stopThreadmutex );


                //STOP CREDIT POLLING THREAD
		if( true ==  g_stopThread )
		{

		                   
                                   pthread_mutex_unlock(&g_stopThreadmutex);

		                   pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

		                   g_NoteLowLevelPollFlagStatus=0;

                                   pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

                                   writeFileLog("[JCM_RSP_Poll() AtvmAPI Exit] JCM_RSP_Poll Thread now exit."); 
		                    
		                   pthread_exit(0);

 
                }
                else
                {

                                 pthread_mutex_unlock(&g_stopThreadmutex);

                                 writeFileLog("[JCM_RSP_Poll()] JCM_RSP_Poll Thread Running."); 

                }
               
               /////////////////////////////////////////////////////////////////////////////////////////////////////

		 int RejectState=0x00,fare=0,DisableFlag=0;

                 int Counter=0;

                 int rtcode=-1;

                 unsigned char Response[100];

		 memset(Response,'\0',100);

                 JCM_ESCROW_NOTE currentnote={0,0,0,0,0,0};

                 unsigned char log[100];

                 memset(Response,'\0',100);

                 unsigned int NoteAcceptSequenceFlag=0x00;

                 bool Disable_Acceptance_RSP_ReadFlag=false;

                 bool Escrow_Note_Flag=false,Reject_Note_Flag=false;

                 char enq=0x00;

                 int totalByteRead=1;

                 bool EscrowStateEnableFlag=false,SpecificEnable=false;

                 /////////////////////////////////////////////////////////////////////////////////////////////////////

                 /*

                 //THREAD PRIORITY WITH PAGE LOCK
                 if(0 == geteuid() )
                 {
		             struct sched_param sp;
		             memset(&sp,0,sizeof(sp));
		             sp.sched_priority=sched_get_priority_max(SCHED_FIFO);
		             if( 0 == sched_setscheduler(0,SCHED_FIFO,&sp)) //success zero return
		             {
		                  writeFileLog("[JCM_RSP_Poll()] Realtime scheduling set success.");
		             }
		             else
		             {
		                  writeFileLog("[JCM_RSP_Poll()] Realtime scheduling set failed.");

		             }

                             
		             if( 0 == mlockall(MCL_CURRENT|MCL_FUTURE))
		             {
		                  writeFileLog("[JCM_RSP_Poll()] Paging disable success.");
		             }
		             else
		             {
		                  writeFileLog("[JCM_RSP_Poll()] Paging disable failed.");

		             }
		             writeFileLog("[JCM_RSP_Poll()] running with real time priority");
                             

                 }
                 else
                 {
                             writeFileLog("[JCM_RSP_Poll()] not running with real time priority");
                 }

                 */

                 /////////////////////////////////////////////////////////////////////////////////////////////////////

                 //Set Escrow table
		 currentnote.AcceptedNumber_INR_5=g_currentnote.AcceptedNumber_INR_5;
                 currentnote.AcceptedNumber_INR_10=g_currentnote.AcceptedNumber_INR_10;
                 currentnote.AcceptedNumber_INR_20=g_currentnote.AcceptedNumber_INR_20;
                 currentnote.AcceptedNumber_INR_50=g_currentnote.AcceptedNumber_INR_50;
                 currentnote.AcceptedNumber_INR_100=g_currentnote.AcceptedNumber_INR_100;
                 currentnote.AcceptedNumber_INR_500=g_currentnote.AcceptedNumber_INR_500;
                 currentnote.AcceptedNumber_INR_1000=g_currentnote.AcceptedNumber_INR_1000;
                 
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

                 //DEFAULT NOTE ACCEPTANCE SEQUENCE FLAG SET TO ZERO
                 NoteAcceptSequenceFlag=0;

                 unsigned char dle0[2],jcmenq=0x05;

                 memset(dle0,'\0',2);

                 int counter=1;

                 unsigned char replychecksum=0x00;

                 //Get user selected fare
                 int userfare=0;

                 userfare =  GetFare();

                 ///////////////////////////////////////////////////////////////////////////////////////


                 JCM_ESCROW_NOTE tempcurrentnote={0,0,0,0,0,0};

                 int CurrentAcceptedAmount=0,CurrenteEscrowAmount=0;

                 g_EscrowEvent=false;

                 /////////////////////////////////////////////////////////////////////////////////////////


                  pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );

                  g_NADisableAcceptanceFlag=false;

                  pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );


                ///////////////////////////////////////////////////////////////////////////////////////////

                 pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

                 g_NA_NoteAcceptanceFlag = false;

                 pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );

 
                ///////////////////////////////////////////////////////////////////////////////////////////

                bool DisableCommandIssue=false;

                ////////////////////////////////////////////////////////////////////////////////////////////

                 while(1)
                 {

                       
                       //////////////////////////////////////////////////////////////////////////////////////////////////

                       pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

                       //credit polling started
                       g_NoteLowLevelPollFlagStatus=1;

                       pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

                       //////////////////////////////////////////////////////////////////////////////////////////////////
                       
                       pthread_mutex_lock( &g_PollFlagmutex );

                       pthread_mutex_lock( &g_stopThreadmutex );

                       //Start Read RSP Packet Polling
                       if( (true == g_PollFlag ) && ( false == g_stopThread ) )
                       {
		                       
                                       pthread_mutex_unlock( &g_PollFlagmutex );

                                       pthread_mutex_unlock( &g_stopThreadmutex );
                                
                                       /////////////////////////////////////////////////////////

                                       //Disable JCM Note Acceptor after credit poll
				       pthread_mutex_lock(&g_NASetDisableAcceptance);

				       if( true == g_DisableAcceptance  ) 
				       { 

						       //DisableFlag=1;

						       if( ( 0 == NoteAcceptSequenceFlag )  && 
                                                       ( 0 == InternalReadEnableReplyFlag ) &&
                                                       (0 == DisableFlag))
						       {

							       writeFileLog("[JCM_RSP_Poll() Disable Acceptance] Before issue Disable Acceptance command.");

						               //issue disable acceptance command
							       if( 1 == Disable_Acceptance_During_RSPPoll() )
                                                               {

                                                                   Disable_Acceptance_RSP_ReadFlag=true;

						                   DisableCommandIssue=true;

                                                                   //Now set disable flag that we already disable note acceptor no need to do that again
                                                                   DisableFlag=1;

                                                                   writeFileLog("[JCM_RSP_Poll()] successfully issue disable acceptance command.");
                                                               }
                                                               else
                                                               {
                                                                  writeFileLog("[JCM_RSP_Poll()] Failed to issue disable acceptance command.");

                                                               }

						               writeFileLog("[JCM_RSP_Poll() Disable Acceptance] After issue Disable Acceptance command.");

						      }
						     
						      pthread_mutex_unlock(&g_NASetDisableAcceptance);

						    
                                      }    
				      else
				      {
						       pthread_mutex_unlock(&g_NASetDisableAcceptance); 

				      }

					    
                                       ///////////////////////////////////////////////////////////////////////

                                       rtcode=-1;
		                       
		                       memset(Response,'\0',100);
                              
                                       //Set Current Reply Packet size
		                       if( true == Disable_Acceptance_RSP_ReadFlag )
		                       {
		                              
                                              replysize=JCM_INSERTION_AUTHORIZING_REPLY_SIZE;

                                              Disable_Acceptance_RSP_ReadFlag=false;
		                           
		                       }
		                       else if( 1 == InternalReadEnableReplyFlag )
		                       {
		                              
                                              replysize=JCM_INSERTION_AUTHORIZING_REPLY_SIZE;

		                              InternalReadEnableReplyFlag=0;
		                           
		                       }
                                       else if ( true == Escrow_Note_Flag )
                                       {

                                            replysize=JCM_ESCROW_REPLY_SIZE ;

                                       }
                                       else if ( true == Reject_Note_Flag )
                                       {

                                            replysize=JCM_REJECT_REPLY_SIZE ;

                                       }
                                       else
		                       {

		                            replysize=JCM_RSP_REPLY_SIZE;

		                       }

                                      

                                       //read rsp packet
		                       rtcode = JCM_Receieve_RSP_Packet(Response,replysize);


                                      ////////////////////////////////////////////////////////////////////////////
                                       
                                       /*

                                       //check rsp checksum value
                                       replychecksum=0x00;

                                       replychecksum=GetBCC(Response,2,replysize);

                                       if(replychecksum!=Response[replysize-1])
                                       {

                                           writeFileLog("[JCM_RSP_Poll()] RSP packet reply byte checksum value not matched."); 
                                           memset(log,'\0',100);

		                           sprintf(log,"[JCM_RSP_Poll()] replychecksum= 0x%xh. Response[%d] = 0x%xh.",replysize,replysize-1,Response[replysize-1]);

		                           writeFileLog(log);

                                           int Counter=0 ;

				           for(Counter=0;Counter<replysize;Counter++)
				           {
						     
                                                     memset(log,'\0',100);

		                                     sprintf(log,"[JCM_RSP_Poll()] Response[%d] = 0x%xh .",Counter,Response[Counter]);
		                                     writeFileLog(log);


				          }


                                           //continue;

                                       }
 
                                       */

                                       ////////////////////////////////////////////////////////////////////////////

                                       if( Response[3]> 0 ) 
				       {
						
		                               
                                                 #ifdef JCM_DEBUG   

				                 int Counter=0 ;

						 for(Counter=0;Counter<replysize;Counter++)
						 {
						     
                                                     memset(log,'\0',100);

		                                     sprintf(log,"[JCM_RSP_Poll()] Response[%d] = 0x%xh .",Counter,Response[Counter]);
		                                     writeFileLog(log);


						 }

                                                 #endif

                                                 ////////////////////////////////////////////////////////////////////////////////
				                 
		                                 memset(log,'\0',100);

		                                 sprintf(log,"[JCM_RSP_Poll()] Credit Polling Command = 0x%xh .",Response[2]);
		                                 writeFileLog(log);

		                                 memset(log,'\0',100);

		                                 sprintf(log,"[JCM_RSP_Poll()] Credit Polling State = 0x%xh .",Response[3]);
		                                 writeFileLog(log);
                                                 
                                                 ////////////////////////////////////////////////////////////////////////////

		                                 //SET CURRENT STATE
		                                 pthread_mutex_lock(&g_JCM_State_Mutex);

		                                 
		                                 g_JCM_Current_State=Response[3];


		                                 pthread_mutex_unlock(&g_JCM_State_Mutex);

                                             ////////////////////////////////////////////////////////////////////////////
                                             
                                                                   
            				     switch( Response[3] )
                                             {     
     
                                              
				                              case 0x04 :

						                      NoteAcceptSequenceFlag=1;
                                                                      
								      pthread_mutex_lock( 
                                                                      &g_NA_NoteAcceptanceFlagMutex );

								      g_NA_NoteAcceptanceFlag = true;

								      pthread_mutex_unlock( 
                                                                      &g_NA_NoteAcceptanceFlagMutex );

                                                                      if( (0x04 == Response[3]) &&
                                                                          (0x50 == Response[2])  
                                                                        ) 
                                                                      {

                                                                            writeFileLog("[JCM_RSP_Poll() 0x04] Before Set Disable acceptance signal.");
                                                                                                                                                                        									    pthread_mutex_lock
                                                                            ( 
                                                                            &g_NADisableAcceptanceFlagMutex );

                                                                            g_NADisableAcceptanceFlag=true;

                                                                            pthread_mutex_unlock
                                                                            ( 
                                                                            &g_NADisableAcceptanceFlagMutex );

                                                                            writeFileLog("[JCM_RSP_Poll() 0x04] After Set Disable acceptance signal.");
                                         
                                                                      }

                                                                      break; 

                                                              case 0x05:

                                                                      NoteAcceptSequenceFlag=1; 

                                                                      pthread_mutex_lock( 
                                                                      &g_NA_NoteAcceptanceFlagMutex );

								      g_NA_NoteAcceptanceFlag = true;

								      pthread_mutex_unlock( 
                                                                      &g_NA_NoteAcceptanceFlagMutex );
				                                     
				                                      writeFileLog("[JCM_RSP_Poll()] Entry Acceptance Block.");	   
				                                      /////////////////////////////////////////////////////

                                                                      tempcurrentnote.AcceptedNumber_INR_5=
				                                      0; 

				                                      tempcurrentnote.AcceptedNumber_INR_10=
				                                      0; 

		     
				                                      tempcurrentnote.AcceptedNumber_INR_20=
				                                      0; 

				                                      tempcurrentnote.AcceptedNumber_INR_50=
				                                      0;

				
				                                      tempcurrentnote.AcceptedNumber_INR_100=
				                                      0; 

				                                      tempcurrentnote.AcceptedNumber_INR_500=
				                                      0; 


				                                      tempcurrentnote.AcceptedNumber_INR_1000=
				                                      0;

				                                      CurrentAcceptedAmount=0;

                                                                      CurrenteEscrowAmount=0;
				                                      
                                                                      //////////////////////////////////////////

				                                      tempcurrentnote.AcceptedNumber_INR_5=
				                                      currentnote.AcceptedNumber_INR_5; 

				                                      tempcurrentnote.AcceptedNumber_INR_10=
				                                      currentnote.AcceptedNumber_INR_10; 

		     
				                                      tempcurrentnote.AcceptedNumber_INR_20=
				                                      currentnote.AcceptedNumber_INR_20; 

				                                      tempcurrentnote.AcceptedNumber_INR_50=
				                                      currentnote.AcceptedNumber_INR_50; 

				
				                                      tempcurrentnote.AcceptedNumber_INR_100=
				                                      currentnote.AcceptedNumber_INR_100; 

				                                      tempcurrentnote.AcceptedNumber_INR_500=
				                                      currentnote.AcceptedNumber_INR_500; 


				                                      tempcurrentnote.AcceptedNumber_INR_1000=
				                                      currentnote.AcceptedNumber_INR_1000; 

                                                                      //Get current escrow money information
				                                      CurrenteEscrowAmount=
                                                                      JCM_GetDenom_From_EscrowState(Response,
                                                                      &tempcurrentnote);


				                                      /////////////////////////////////////////////////////
                                                                     
						                      CurrentAcceptedAmount =  

                                                                      GetCurrentAcceptedAmount();

                                                                      memset(log,'\0',100);

		                                                      sprintf(log,"[JCM_RSP_Poll()] fare = %d  CurrentAcceptedAmount = %d . CurrenteEscrowAmount= %d .",userfare,CurrentAcceptedAmount,CurrenteEscrowAmount);

		                                                      writeFileLog(log);

                                                                      
				                                      //////////////////////////////////////////////////////////

                             				              //Accept Amount
				                                      if( (CurrentAcceptedAmount+
                                                                      CurrenteEscrowAmount) <= userfare )
				                                      {

                                                                            writeFileLog("[JCM_RSP_Poll()] Start:Now Going to accept Money.");
                                                                            //////////////////////////////////
                                                                            //Update Coin Acceptor Table
                                                                            #if defined(COIN_ACCEPTOR)
                                                                            SetSpecificCoinInhibitStatus(
                                                                            CurrentAcceptedAmount+
                                                                            CurrenteEscrowAmount);
                                                                            #endif
                                                                            //////////////////////////////////
				                                            //issue escrow command
				                                            int rtcode=-1;

				                                            int CmdLength=JCM_CMD_CMN_LENGTH;

				                                            unsigned char Commands  [JCM_CMD_CMN_LENGTH]=JCM_ESCROW_CMD;

                                                                            rtcode= -1;

				                                            Commands
                                                                            [JCM_CMD_CMN_LENGTH-1]=
										GetBCC(
										Commands,
										2,
										JCM_CMD_CMN_LENGTH);
			      
				                                            rtcode=JCM_TransmitCmdSequence(
                                                                            g_HANDLE,
								            Commands,
								            CmdLength);

                                                                            memset(log,'\0',100);

		                                                            sprintf(log,"[JCM_RSP_Poll()]  JCM_TransmitCmdSequence return code= %d .",rtcode);
                                                                            writeFileLog(log);

									    if(1==rtcode)
									    {
									   
										writeFileLog("[JCM_RSP_Poll()] Escrow Command Transmission sequence success.");
                                                                            }
									    else 
                                                                            {

										writeFileLog("[JCM_RSP_Poll()] Escrow Command Transmission sequence failed due to eot byte receieved.");

                                                                                writeFileLog("[JCM_RSP_Poll()] Before go for jcm transmission sequent patch function.");
 
                                                                                rtcode=-1;

                                                                                rtcode = EscrowIssueEOTPatch();
                                                                                writeFileLog("[JCM_RSP_Poll()] After go for jcm transmission sequent patch function.");
                                                                                if( 1 == rtcode )
                                                                                {

                                                                                   writeFileLog("[JCM_RSP_Poll()] jcm transmission sequent patch function successfully run.");

                                                                                   ////////////////////////////////

                                                                                    writeFileLog("[JCM_RSP_Poll()] Before Now going to again run escrow command.");
                                                                                    rtcode= -1;

						                                    Commands
		                                                                    [JCM_CMD_CMN_LENGTH-1]=
											GetBCC(
											Commands,
											2,
											JCM_CMD_CMN_LENGTH);
				      
						                                    rtcode=
                                                                                    JCM_TransmitCmdSequence(
		                                                                    g_HANDLE,
										    Commands,
										    CmdLength);


                                                                                    writeFileLog("[JCM_RSP_Poll()] After Now going to again run escrow command.");

                                                                                  ///////////////////////////////////
                                                                                }
                                                                                else
                                                                                {

                                                                                    writeFileLog("[JCM_RSP_Poll()] jcm transmission sequent patch function failed to do job.");

                                                                                    NoteAcceptSequenceFlag=0;

                                                                                    Escrow_Note_Flag=false;

                                                                                    continue;

                                                                                }

                                                                               


                                                                            } //main else block 

                                                                            
				                                            Escrow_Note_Flag=true;
                                                                           
                                                                            writeFileLog("[JCM_RSP_Poll()] End:Now Going to accept Money.");

				                                            writeFileLog("[JCM_RSP_Poll()] Exit Acceptance Block.");	   

				                                      } //Escrow Block

				                                      //Reject Amount when amount is equal or greater than   
                                                                      else if( (CurrentAcceptedAmount+
                                                                      CurrenteEscrowAmount) > userfare  )
				                                      {

                                                                           

				                                            //issue reject command
                                                                           
                                                                            Reject_Note_Flag=true;
                                       
                                                                            replysize= JCM_REJECT_REPLY_SIZE ;

                                                                            int rtcode=-1;

				                                            int CmdLength=JCM_CMD_CMN_LENGTH;

				                                            unsigned char Commands 
                                                                            [JCM_CMD_CMN_LENGTH]=
                                                                            JCM_REJECT_CMD;

				                                            Commands[JCM_CMD_CMN_LENGTH-1]=
                                                                            GetBCC(
                                                                            Commands,2,
                                                                            JCM_CMD_CMN_LENGTH);
			      
				                                            rtcode=JCM_TransmitCmdSequence(
                                                                            g_HANDLE,Commands,CmdLength);
									    
                                                                            if(1==rtcode)
									    {
									   
										writeFileLog("[JCM_RSP_Poll()] Reject Command Transmission sequence success.");	   
								 
									    }
									    else
									    {
										 writeFileLog("[JCM_RSP_Poll()] Reject Command Transmission sequence failed.");
										     
									    } 


				                                      } //Reject Block end
                                                                      
                                                                      
				                                       
                                                                      break;

                                                            
				                            //REJECT STATE WITH WAITING TO BE RECEIEVED
						            case 0x09:
                                                                       NoteAcceptSequenceFlag=1;
						                       RejectState=Response[3];
				                                       break;

		 
				                            case 0x08: //Start Rejection state
                                                                       NoteAcceptSequenceFlag=1;

                                                                       pthread_mutex_lock( 
                                                                       &g_NA_NoteAcceptanceFlagMutex );

					                               g_NA_NoteAcceptanceFlag = true;

							               pthread_mutex_unlock( 
                                                                       &g_NA_NoteAcceptanceFlagMutex );

                                                                       break;

				                            case 0x03: //Ready state without escrow notes
				                            case 0x13: //Ready state with    escrow notes
				                                       NoteAcceptSequenceFlag=0;

                                                                       ///////////////////////////////////

                                                                       if( true == DisableCommandIssue )
                                                                       {
                                                                         if( ( (0x13 == Response[3])   &&
                                                                               (0x50 == Response[2]) ) ||
                                                                              ((0x03 == Response[3])&& 
                                                                               (0x50 == Response[2]) ) 

                                                                            )
                                                                            {

                                                                            writeFileLog("[JCM_RSP_Poll()] Before Set Disable acceptance signal.");
                                                                                                                                                                        									    pthread_mutex_lock
                                                                            ( 
                                                                            &g_NADisableAcceptanceFlagMutex );

                                                                            g_NADisableAcceptanceFlag=true;

                                                                            pthread_mutex_unlock
                                                                            ( 
                                                                            &g_NADisableAcceptanceFlagMutex );

                                                                            writeFileLog("[JCM_RSP_Poll()] After Set Disable acceptance signal.");

		                                                               
                                                                            writeFileLog("[JCM_RSP_Poll()] Before Set Current Note acceptance signal.");
                                                                								            pthread_mutex_lock(
 
		                                                            &g_NA_NoteAcceptanceFlagMutex );

							                    g_NA_NoteAcceptanceFlag = false;

								            pthread_mutex_unlock( 
		                                                            &g_NA_NoteAcceptanceFlagMutex );
                                                                              
                                                                            writeFileLog("[JCM_RSP_Poll()] After Set Current Note acceptance signal.");

                                                                            }

                                                                            /////////////////////////////////
                                                                            

                                                                       }
                                                                       else
                                                                       {
                                                                       pthread_mutex_lock( 
                                                                       &g_NA_NoteAcceptanceFlagMutex );

					                               g_NA_NoteAcceptanceFlag = false;

							               pthread_mutex_unlock( 
                                                                       &g_NA_NoteAcceptanceFlagMutex );
                                                                       }

				                                       break; 

						            case 0x02:
				                            case 0x12:

				                             NoteAcceptSequenceFlag=0;

                                                             pthread_mutex_lock( 
                                                             &g_NA_NoteAcceptanceFlagMutex );

					                     g_NA_NoteAcceptanceFlag = false;

							     pthread_mutex_unlock( 
                                                             &g_NA_NoteAcceptanceFlagMutex );

				                             //Signal DIsable acceptance signal
                                                             if( ( (0x12 == Response[3])   &&
                                                                   (0x50 == Response[2]) ) ||
                                                                  ((0x02 == Response[3])   && 
                                                                   (0x50 == Response[2]) ) 

                                                               )
                                                             {

                                                                            writeFileLog("[JCM_RSP_Poll() 0x12] Before Set Disable acceptance signal.");
                                                                                                                                                                        									    pthread_mutex_lock
                                                                            ( 
                                                                            &g_NADisableAcceptanceFlagMutex );

                                                                            g_NADisableAcceptanceFlag=true;

                                                                            pthread_mutex_unlock
                                                                            ( 
                                                                            &g_NADisableAcceptanceFlagMutex );

                                                                            writeFileLog("[JCM_RSP_Poll() 0x12] After Set Disable acceptance signal.");
                                         
                                                             }

				                             //note is now stacked
				                          
				                             if( (0x12 == Response[3])    && 
                                                                 (0x55 == Response[2])    && 
                                                                 (false == g_EscrowEvent)
                                                                
						               )

				                           
						             {

				                                          writeFileLog("[JCM_RSP_Poll()] Entry Escrow Block.");

				                                          /////////////////////////////////////////////////////////////////////

				                                          Escrow_Note_Flag=false;
				                                          
				                                          /////////////////////////////////////////////////////////////////////////		                                        

						                          //Record Escrow Money
						                          EscrowMoney=
                                                                          JCM_GetDenom_From_EscrowState(
                                                                          Response,
                                                                          &currentnote);

				                                          if( (EscrowMoney>=5 ) && 
 	                                                                      (EscrowMoney<=1000) 
                                                                            )
						                          {
								                  
				                                                  EscrowStateEnableFlag=true;

				                                                  pthread_mutex_lock(&g_JCM_Escrow_State_Mutex);

								                  g_EscrowEvent=true;

								                  g_JCM_Accepted_Moneyvalue =

                                                                                  EscrowMoney;

						                                  EscrowMoney=0;

								                  pthread_mutex_unlock(
                                                                                  &g_JCM_Escrow_State_Mutex);

								                  writeFileLog("[JCM_RSP_Poll() Credit Polling Escrow State ] Escrow State happened.");
						                                  
						                                  pthread_mutex_lock(
                                                                                  &g_JCM_State_Mutex);

						                                    
						                                  //user defined escrow state complete
                                                                                  g_JCM_Current_State=0x0f;

						                                  pthread_mutex_unlock(
                                                                                  &g_JCM_State_Mutex);

						                          }


				                                          writeFileLog("[JCM_RSP_Poll()] Exit Escrow Block.");

				                                          continue;

				           
							     }

                                                             ///////////////////////////////////////////////////

                                                             //reject state enable
							     if(
                                                                ((0x09 == RejectState) && (0x12 == Response[3]) ) ||
								((0x09 == RejectState) && (0x02 == Response[3]) )
							       )
							      {

				                                 RejectState=0x00;

                                                                
                                                               
				                                 pthread_mutex_lock( &g_stopThreadmutex );

								 if( (false == g_stopThread ) && 
                                                                     ( 0 == DisableFlag ) 
                                                                   )
				                                 {     
				                                             
			 
						                  pthread_mutex_unlock( &g_stopThreadmutex );

						                  bool InternalEnableflag=false;

						                  //Run time inhibit
			                                          pthread_mutex_lock(
                                                                  &g_NASetSpecificFaremutex);
	
	                                                          if( 1 == g_SpecificNoteInhibitFlag )  
								  {
													     
				                                    writeFileLog("[JCM_RSP_Poll() Reject Event] External Enable Event lock.");
											   
				                                    fare=0;

				                                    fare=g_NACurrentAtvmFare;
											  			                                                    g_SpecificNoteInhibitFlag=0;
											   
				                                    writeFileLog("[JCM_RSP_Poll() Reject Event] External Enable Event unlock.");

							          }
								  else
						                  {
						                                             									     InternalEnableflag=true;

						                  }

							          pthread_mutex_unlock(
                                                                  &g_NASetSpecificFaremutex);
													 
							          if( true == InternalEnableflag)
								  {
											    
							             pthread_mutex_lock(&g_NASetCurrentFare);

								     fare=g_ThreadCurrentfare;

								     pthread_mutex_unlock(&g_NASetCurrentFare);
																				  
								  }
												  
				                                  memset(log,'\0',100);

								  sprintf(log,"[JCM_RSP_Poll() Reject Event]  Current Fare = %d ",fare);

								  writeFileLog(log);

								  RejectState=0x00;

						                  //note accepting process finished

						                  NoteAcceptSequenceFlag=0; 

				                                  writeFileLog("[JCM_RSP_Poll() Reject Event]  Entry Enable" );

                                                                  ////////////////////////////////////////////

                                                                  pthread_mutex_lock(
                                                                  &g_NASetDisableAcceptance);

                                                                  if( true == g_DisableAcceptance  ) 
                                                                  {
 
                                                                    pthread_mutex_unlock(
                                                                    &g_NASetDisableAcceptance);

                                                                    writeFileLog("[JCM_RSP_Poll() Reject Event] Unable to do enable note acceptor as inhibit signal is on.");

                                                                    continue;


                                                                  }

                                                                  pthread_mutex_unlock(
                                                                  &g_NASetDisableAcceptance);

                                                                  ////////////////////////////////////////////

						                  EnableDenomDuringCreditPolling(fare,
                                                                  JCM_EXTERNAL_COMMAND_REPLY_READ);

				                                  writeFileLog("[JCM_RSP_Poll() Reject Event]  Exit Enable" );

				                                        

				                                 }//if( (false == g_stopThread ) && 
                                                                  //( 0 == DisableFlag ) )
                                                                 else
						                 {
						                   pthread_mutex_unlock( &g_stopThreadmutex );

						                 }

				                                 continue;
				                                         

										               
							   }//if(
                                                            //((0x09 == RejectState) && (0x12 == Response[3]) ) ||
							    //((0x09 == RejectState) && (0x02 == Response[3]) )
							    //) block

                                                           ///////////////////////////////////////////////////

				                           break;


				             default: memset(log,'\0',100);

			                              sprintf(log,"[JCM_RSP_Poll()] Unknown State = 0x%xh. ",Response[3]);

					              writeFileLog(log);

				                      break;



                          }; //switch block exit



                       }//if(Response[3]>0) block

                      /////////////////////////////////////////////////////////////////////////////////////////////////////

                      pthread_mutex_lock( &g_stopThreadmutex );

                      //Enable from Note and Coin Acceptor can be done here 
                      if( ( 0 == NoteAcceptSequenceFlag ) && ( false == g_stopThread ) && 
                          ( 0 == DisableFlag ) )
                      {

                                             
                                             pthread_mutex_unlock( &g_stopThreadmutex );
                                                   				                           
                                       
                                             ///////////////////////////////////////////////////////////
                                             //if already get disable signal then 
                                             //dont need to enable it again
                                             pthread_mutex_lock(&g_NASetDisableAcceptance);

                                             if( true == g_DisableAcceptance )
                                             {

                                                 writeFileLog("[JCM_RSP_Poll() Enable block] Unable to do enable note acceptor as inhibit Signal Receieved.");  

                                                 pthread_mutex_unlock(&g_NASetDisableAcceptance);

                                                 continue;

                                             }
 
                                             pthread_mutex_unlock(&g_NASetDisableAcceptance);

                                            
                                             //////////////////////////////////////////////////////////              
                 
                                             //Run time inhibit
		                             pthread_mutex_lock( &g_NASetSpecificFaremutex );

		                             if( 1 == g_SpecificNoteInhibitFlag )  
				             {
									     
		                                       fare=0;

						       fare=g_NACurrentAtvmFare;

						       g_SpecificNoteInhibitFlag=0;

                                                       SpecificEnable=true;
		                            
		                             }

		                             pthread_mutex_unlock(&g_NASetSpecificFaremutex);

                                             ////////////////////////////////////////////////////////////

                                             if(true == SpecificEnable)
                                             {

                                                       
                                                      writeFileLog("[JCM_RSP_Poll() Enable block] Enable Signal Receieved.");  

                                                      SpecificEnable=false;
           
		                                      memset(log,'\0',100);

						      sprintf(log,"[JCM_RSP_Poll() Common Enable block]  Current Fare = %d .",fare);

						      writeFileLog(log);

		                                      if(true == EscrowStateEnableFlag)
		                                      {


                                                              writeFileLog("[JCM_RSP_Poll() With Escrow Enable block] Found Escrow Enable State.");
                                                              EscrowStateEnableFlag=false;

		                                              writeFileLog("[JCM_RSP_Poll() With Escrow Enable block] Start Enable.");

                                                              //////////////////////////////////////////////////

                                                              pthread_mutex_lock(&g_NASetDisableAcceptance);

                                                              if( true == g_DisableAcceptance )
                                                              {
                                                               
                                                                 writeFileLog("[JCM_RSP_Poll() With Escrow Enable block] No need to do enable note acceptor as inhibit signal is on.");

                                                                 pthread_mutex_unlock(
                                                                 &g_NASetDisableAcceptance);
                                                                 
                                                                 continue;

                                                              }
 
                                                              pthread_mutex_unlock(&g_NASetDisableAcceptance);

                                                              //////////////////////////////////////////////////

                                                              EnableDenomDuringCreditPolling(fare,
                                                              JCM_EXTERNAL_COMMAND_REPLY_READ);


                                                              writeFileLog("[JCM_RSP_Poll() With Escrow  Enable block] End Enable.");


		                                      }
		                                      else if( false == EscrowStateEnableFlag )
		                                      {


                                                              writeFileLog("[JCM_RSP_Poll() Without Escrow Enable block] Found Without Escrow Enable State.");
                                                              totalByteRead=1;
				                             
				                              enq=0x00;

				                              ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);

						              if( 0x05 != enq )
				                              {
						                      
                                                                      writeFileLog("[JCM_RSP_Poll() Without Escrow Enable block] Start Enable.");

                                                              //////////////////////////////////////////////////

                                                              pthread_mutex_lock(&g_NASetDisableAcceptance);

                                                              if( true == g_DisableAcceptance )
                                                              {
                                                               
                                                                 writeFileLog("[JCM_RSP_Poll() Without Escrow Enable block] No need to do enable note acceptor as inhibit signal is on.");

                                                                 pthread_mutex_unlock(
                                                                 &g_NASetDisableAcceptance);
                                                                 
                                                                 continue;

                                                              }
 
                                                              pthread_mutex_unlock(&g_NASetDisableAcceptance);

                                                              //////////////////////////////////////////////////

		                                                      //Enable Note Acceptor
								      EnableDenomDuringCreditPolling(fare,
                                                                      JCM_INTERNAL_COMMAND_REPLY_READ);


                                                                      InternalReadEnableReplyFlag=1;
				                                                                
				                                      writeFileLog("[JCM_RSP_Poll() Without Escrow  Enable block] End Enable.");

				                             }
                                                             else
                                                             {

                                                                     writeFileLog("[JCM_RSP_Poll() Without Escrow  Enable block] Unable to accept update denomination change request due to get jcm state change enq byte receieved.");

                                                             }
		                                             

		                                     }

                                        }

           
                                        


                     }//End:if( ( 0 == NoteAcceptSequenceFlag ) && (false == g_stopThread ) && ( 0 == DisableFlag ) ) 
                     else                                       
                     {
                                pthread_mutex_unlock( &g_stopThreadmutex );

                     }


                     ////////////////////////////////////////////////////////////////////////////////////////////////////////

                     /*

                     //Disable JCM Note Acceptor after credit poll
                     pthread_mutex_lock(&g_NASetDisableAcceptance);

                     if( true == g_DisableAcceptance  ) 
                     { 

                               DisableFlag=1;

                               if( ( 0 == NoteAcceptSequenceFlag ) && ( 0 == InternalReadEnableReplyFlag ) )
                               {

		                       writeFileLog("[JCM_RSP_Poll() Disable Acceptance] Before Disable Acceptance.");

                                       Disable_Acceptance_RSP_ReadFlag=true;

                                       //issue disable acceptance command
		                       Disable_Acceptance_During_RSPPoll();

                                       g_DisableAcceptance=false;

                                       writeFileLog("[JCM_RSP_Poll() Disable Acceptance] After Disable Acceptance.");

                               }
                             
                               pthread_mutex_unlock(&g_NASetDisableAcceptance);

                               continue;

                               

                    }    
                    else
                    {
                               pthread_mutex_unlock(&g_NASetDisableAcceptance); 

                    }

                    */

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

       }//if( (true == g_PollFlag ) && ( false == g_stopThread ) ) block
       else       
       {

                          
                
                             pthread_mutex_unlock( &g_PollFlagmutex );

                             ///////////////////////////////////////////////////////////////////////////////////////////

                             //Check Kill Signal from upper level
		             if( true ==  g_stopThread )
		             {
		                  
		                   pthread_mutex_unlock( &g_stopThreadmutex );

		                   writeFileLog("[JCM_RSP_Poll() Stop Thread] RSP Poll Thread now exit.");
		                 
		                   pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

		                   g_NoteLowLevelPollFlagStatus=0;

		                   pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

		                   pthread_exit(0);

		                     
		             }
		             else
		             {
		                   pthread_mutex_unlock( &g_stopThreadmutex );

		             }
                           
                             //////////////////////////////////////////////////////////////////////////////////////////////



        }

 
      

    } //while loop end   

            

 

}//void* JCM_RSP_Poll(void *ptr) thread end



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int EnableDenomDuringCreditPolling(int fare,int flag)
{
                

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

                       writeFileLog("[EnableDenomDuringCreditPolling()]  Exact Fare Block Active.");

                       switch(fare)
                       {

                               case 5:
                                      #ifdef JCM_DEBUG 
				      writeFileLog("[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 5 Enable.");
                                      #endif
				      DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
				      break;
				

				case 10:
                                     #ifdef JCM_DEBUG 
				     writeFileLog("[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 10 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
				     break;
				

				case 20:
                                     #ifdef JCM_DEBUG 
				     writeFileLog("[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 20 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
			             break;
				

				case 50 :
                                     #ifdef JCM_DEBUG 
				     writeFileLog("[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 50 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
				     break;
				

				case 100:
                                    #ifdef JCM_DEBUG 
				    writeFileLog("[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 100 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
				    break;
				

				case 500:
                                    #ifdef JCM_DEBUG 
				    writeFileLog("[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 500 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
				    break;
				

				case 1000:
                                    #ifdef JCM_DEBUG 
				    writeFileLog("[EnableDenomDuringCreditPolling() Exact Fare Block] Only Rs 1000 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
				    break;
                              

                              };
				


                }
                else
                {
			
                                writeFileLog("[EnableDenomDuringCreditPolling()]  Exact Fare Block InActive.");
                        	if( fare>=5 )
				{

                                     #ifdef JCM_DEBUG 
				     writeFileLog("[EnableDenomDuringCreditPolling() Max Cash Block] Rs 5 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
				
				}

				if( fare>=10 )
				{
                                     #ifdef JCM_DEBUG 
                                     writeFileLog("[EnableDenomDuringCreditPolling() Max Cash Block] Rs 10 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
				  
				}

				if( fare>=20 )
				{
                                     #ifdef JCM_DEBUG 
                                     writeFileLog("[EnableDenomDuringCreditPolling() Max Cash Block] Rs 20 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
				
				}

				if( fare>=50 )
				{
                                     #ifdef JCM_DEBUG 
                                     writeFileLog("[EnableDenomDuringCreditPolling() Max Cash Block] Rs 50 Enable.");
                                     #endif
				     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
				  
				}

				if( fare>=100 )
				{
                                    #ifdef JCM_DEBUG 
                                    writeFileLog("[EnableDenomDuringCreditPolling() Max Cash Block] Rs 100 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
				
				}

				if( fare>=500 )
				{
                                    #ifdef JCM_DEBUG 
                                    writeFileLog("[EnableDenomDuringCreditPolling() Max Cash Block] Rs 500 Enable.");
				    #endif
                                    DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
				 
				}

				if( fare>= 1000)
				{
                                    #ifdef JCM_DEBUG 
                                    writeFileLog("[EnableDenomDuringCreditPolling() Max Cash Block] Rs 1000 Enable.");
                                    #endif
				    DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
				 
				}


                }

               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    #ifdef JCM_DEBUG

		    memset(log,'\0',100);

                    sprintf(log,"[EnableDenomDuringCreditPolling()] DefaultEnablebyte =0x%xh.",DefaultEnablebyte);

                    writeFileLog(log);

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
                                         writeFileLog(log);

				    }

                                    memset(log,'\0',100);

                                    sprintf(log,"[EnableDenomDuringCreditPolling()] DittoComamnd Packet BCC =0x%xh.",DittoCommands[CmdLength-1]);
                                    writeFileLog(log);
				
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
                                          writeFileLog(log);

				    }

				
                                    memset(log,'\0',100);

                                    sprintf(log,"[EnableDenomDuringCreditPolling()] Comamnd Packet BCC =0x%xh.",Commands[CmdLength-1]);

                                    writeFileLog(log);

				    #endif

		    
		    }

                    if(0x10== DefaultEnablebyte)
                    {

                           #ifdef JCM_DEBUG
			   writeFileLog("[EnableDenomDuringCreditPolling()] Enablebyte 0x10 found.");
                           #endif
                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,DittoCommands,CmdLength);

	            }
	            else if(0x10 != DefaultEnablebyte)
	            {

                            #ifdef JCM_DEBUG
			    writeFileLog("[EnableDenomDuringCreditPolling()] Enablebyte 0x10 not found.");
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
		        writeFileLog(log);

		        memset(log,'\0',200);
		        sprintf(log,"[EnableDenomDuringCreditPolling()] State = 0x%x h",Response[3]);
		        writeFileLog(log);

		        
                        #ifdef JCM_DEBUG

		        int i=0;
		        for(i=0;i<JCM_INSERTION_AUTHORIZING_REPLY_SIZE;i++)
		        {

		             
                             memset(log,'\0',200);
		             sprintf(log,"[EnableDenomDuringCreditPolling()] Response[%d]= 0x%x h",i,Response[i]);
		             writeFileLog(log);


		        }

		        #endif

		       if(   ( ( 0x50 == Response[2] ) && ( 0x03 == Response[3] ) )  ||
		             ( ( 0x50 == Response[2] ) && ( 0x13 == Response[3] ) )  ||
                             ( ( 0x50 == Response[2] ) && ( 0x05 == Response[3] ) )  ||
                             ( ( 0x50 == Response[2] ) && ( 0x08 == Response[3] ) ) 
		          )
		        {
		             writeFileLog("[EnableDenomDuringCreditPolling()] Enable Denom Success ."); 
		             return 1;
		        
		        }
		        else
		        {
		              writeFileLog("[EnableDenomDuringCreditPolling()] Enable Denom not possible due to no matching with any denomination."); 
		              return 0;

		        }

               }
               else if( JCM_INTERNAL_COMMAND_REPLY_READ == flag)
               {

                    //writeFileLog("[EnableDenomDuringCreditPolling()] Enable Denom recv not read due to flag not set it will be read any other recv function ."); 
                    return 1;


               }                


}//EnableDenomDuringCreditPolling(int fare,int flag) end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Make reset JCM
int JCM_Reset()
{
 
            
                unsigned char Response[100];

                memset(Response,'\0',100);

                unsigned char log[200];

                memset(log,'\0',200);


                unsigned char Commands[ JCM_CMD_CMN_LENGTH ]=JCM_RESET_CMD;


                int Counter=0;
                int rtcode=0;
                int CmdLength = JCM_CMD_CMN_LENGTH;
                
                //Create checksum value for command byte
                for(Counter=2;Counter<= (CmdLength-2);Counter++)
	        {
	           Commands[CmdLength-1]=  Commands[CmdLength-1] ^ Commands[Counter];
	        }
             

                //Step 1:Transmit Command
                
		rtcode=JCM_TransmitCmdSequence(g_HANDLE,Commands,JCM_CMD_CMN_LENGTH);

		if(1==rtcode)
		{

		   
                            ///////////////////////////////////////////////////////////////////////

                            //wait for reset operation done [Patch]
		            
                       

	                    writeFileLog("[JCM_Reset()]  Waiting for enq byte for reset reply ");
					      
			   

                            unsigned char rbyte=0x00;

                            int totalByteIn=0,counter=1;

                            while(1)
                            {
                                 
                                  rbyte=0x00;

                                  totalByteIn=0;

                                  //wait for enq byte
                                  ReceiveSingleByteFromSerialPort(g_HANDLE,&rbyte,&totalByteIn);


                                  //memset(log,'\0',200);

	                          //sprintf(log,"[JCM_Reset()]  rbyte = 0x%xh",rbyte);
					      
			          //writeFileLog(log);

 
                                  if( ( rbyte > 0x00 ) && ( 0x05 == rbyte ) )
                                  {
                          
                                       writeFileLog("[JCM_Reset()]  Found enq byte for reset reply ");

                                       DelayInSeconds( 1 );

                                       break;

                                  } 
                                  
                                  
                                  if( counter > JCM_ENQ_WAIT_FOR_RESET )
                                  {
                                       
                                       writeFileLog("[JCM_Reset()]  Unable to found enq byte for reset reply .");

                                       return 0;
                                      

                                  }

                                  //Then delay for 1 second
                                  DelayInSeconds( 1 );
                                
                                  counter++;

                            }


                            ///////////////////////////////////////////////////////////////////////

		            rtcode=-1;

		            //Step 2:Receieve Reply

		            rtcode=JCM_ReceieveCmdReplySequence(g_HANDLE,Response,JCM_RESET_REPLY_SIZE);

		            if(1!=rtcode)
		            {

				        writeFileLog("[JCM_Reset()] Receieve sequence failed.");

		                        //#ifdef JCM_DEBUG  
	 
					//Display Reset Response Packet
					for(Counter=0;Counter<JCM_RESET_REPLY_SIZE;Counter++)
					{

					    memset(log,'\0',200);

					    sprintf(log,"[JCM_Reset()] Response[%d] = 0x%xh",Counter,Response[Counter]);
					      
					    writeFileLog(log);

					}
				       
					//#endif

				        return 0; //recv sequence failed
		                  
		            }
	 
		}
		else
		{
		             writeFileLog("[JCM_Reset()] Transmission sequence failed.");
		             return 0; //transmit sequence failed
		} 

               
                 //#ifdef JCM_DEBUG  
	 
	         //Display Reset Response Packet
		 for(Counter=0;Counter<JCM_RESET_REPLY_SIZE;Counter++)
		 {
                           memset(log,'\0',200);
                           
                           sprintf(log,"[JCM_Reset()] Response[%d] = 0x%xh",Counter,Response[Counter]);
					      
		           writeFileLog(log);

		 }
				       
		//#endif

                //check reply byte checksum value
                int replybytechecksum=0x00;

                for(Counter=2;Counter<= (JCM_RESET_REPLY_SIZE-2);Counter++)
	        {
	           replybytechecksum =  replybytechecksum ^ Response[Counter];
	        }

                ////////////////////////////////////////////////////////////////////////////

                //Log Reply byte checksum
                //memset(log,'\0',200);

		//sprintf(log,"[JCM_Reset()] replybytechecksum = 0x%xh.  Response[%d]= 0x%xh .",replybytechecksum,JCM_RESET_REPLY_SIZE-1,Response[JCM_RESET_REPLY_SIZE-1]);
		      
                //writeFileLog(log);

                ////////////////////////////////////////////////////////////////////////////
                                                                                                                                     
                if(Response[JCM_RESET_REPLY_SIZE-1]!= replybytechecksum )
                {
                    writeFileLog("[JCM_Reset()] Reply byte checksum is not matched");
                    return 0;
                    
                }


                if(Response[3]>0x00)
                {


                                  //Log Current State
                                  memset(log,'\0',200);

		                  sprintf(log,"[JCM_Reset()] Current State: 0x%xh",Response[3]);
		      
                                  writeFileLog(log);

		                  if( 0x0D == Response[3] ) 
				  {
				        writeFileLog("[JCM_Reset()] Found Standby with escrow notes State.");

					return 1;
				  }
                                  else if( 0x02 == Response[3] ) 
                                  {
                                        writeFileLog("[JCM_Reset()] Found Standby without escrow notes State.");
					return 1;

                                  }
                                  else if( 0x81 == Response[3] ) 
                                  {
                                        writeFileLog("[JCM_Reset()] Found Alarm State.");
					return 0;
			 
			          }// else if( 0x81 == Response[3] )
                                  else if ( 0x80 == Response[3] ) 
                                  {
                                        writeFileLog("[JCM_Reset()] Found Sense trouble State.");
					return 0;
			 
			          }// else if( 0x81 == Response[3] ) block


                }//if(Response[3]>0x00) block
                else
                {
                    writeFileLog("[JCM_Reset()] unknown jcm state found.");
                    return 0;

                }
                

}//JCM_Reset() end



///////////////////////////////////////////////////////////////////////////////////////////////////////////


//Reinit After Com Port Open
int JCM_InitAfterPortOpen()
{
           

           if( -1 != g_HANDLE )
	   {

	       

                                          int rtcode=-1;

                                          unsigned char Response[100];

                                          memset(Response,'\0',100);

                                          unsigned char log[200];

                                          memset(log,'\0',200);

	                                  writeFileLog("[JCM_Activate()] Before Going to issue sense command.");

		                          rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                          writeFileLog("[JCM_Activate()] After issued sense command.");

		                          if( ( Response[3] >= 0 ) &&  ( 0x40 == Response[2] ) )
					  {
					                 
                                                         #ifdef JCM_DEBUG 

				                         int i=0;

		                                         for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                         {
						
		                                               memset(log,'\0',200);

		                                               sprintf(log,"[JCM_InitAfterPortOpen()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                               writeFileLog(log);
	 
		                                         }
                                                        
                                                         #endif

                                                         //Log Current State 
                                                         memset(log,'\0',200);

		                                         sprintf(log,"[JCM_InitAfterPortOpen()] Current State = 0x%xh. ",Response[3]);
		                                         writeFileLog(log);
                                 
                                                         //Log Current Sensor State

                                                         //++writeFileLog("[JCM_InitAfterPortOpen()] Start Record Current JCM Sensor State.");
                                                         //++JCM_LogSensorState(Response[7],Response[8],Response[9]);

                                                         //++writeFileLog("[JCM_InitAfterPortOpen()] End Record Current JCM Sensor State.");

                                                         switch(Response[3])
                                                         {
                                                             
                                                                     //reset found after power up  
		                                                     case 0x00:writeFileLog("[JCM_InitAfterPortOpen()] Found After Power on state.");
                                                                               //issue reset command 
                                                                               return ( JCM_Reset() );

		                                                               break;

		                                                     //reset found after power up  
		                                                     case 0x01:writeFileLog("[JCM_InitAfterPortOpen()] Found In Reset State.");
                                                                               return 1;
		                                                               break;

		                                                     //normal state
		                                                     case 0x02:writeFileLog("[JCM_InitAfterPortOpen()] Found Standby without note present in escrow State.");
                                                                               return 1;                          
		                                                               break;

		                                                     //Disable here
		                                                     case 0x03:writeFileLog("[JCM_InitAfterPortOpen()] Found  Enable without note present in escrow State.");
                                                                               return ( DisableAcceptance() );
		                                                               break;

		                                                     //normal state with notes in escrow
		                                                     case 0x12:writeFileLog("[JCM_InitAfterPortOpen()] Found Standby with note present in escrow State.");
                                                                               return 1;
		                                                               break;

		                                                     //Disable here
		                                                     case 0x13:writeFileLog("[JCM_InitAfterPortOpen()] Found  Enable with note present in escrow State.");
                                                                               //Make Disable it and accept all money to freevault
                                                                               writeFileLog("[JCM_InitAfterPortOpen()] No going to first inhibit note acceptor");
                                                                               if( 1 == DisableAcceptance() )
                                                                               {

                                                                                    writeFileLog("[JCM_InitAfterPortOpen()] inhibit note acceptor successfully done.");
                                                                                    writeFileLog("[JCM_InitAfterPortOpen()] Now going to accept all escrow money to vault.");
                                                                                    if(1==JCM_DefaultCommit())
                                                                                    {
                                                                                        writeFileLog("[JCM_InitAfterPortOpen()] Accept escrow money successfully done."); 
                                                                                        return 1;

                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        writeFileLog("[JCM_InitAfterPortOpen()] Accept escrow money failed.");
                                                                                        return 0;

                                                                                    }
                                                                               }
                                                                               else
                                                                               {
                                                                                        writeFileLog("[JCM_InitAfterPortOpen()] Inhibit note acceptor failed.");
                                                                                        return 0;

                                                                               } 

		                                                               break;

		                                                     //Sense trouble
		                                                     case 0x80:writeFileLog("[JCM_InitAfterPortOpen()] Found  Sense Trouble State.");
                                                                               return 0;
		                                                               break;

		                                                     //Jam or Alarm State reset here
		                                                     case 0x81:writeFileLog("[JCM_InitAfterPortOpen()] Found Alarm/Jamming State.");
                                                                               return(JCM_ReInit(30));
		                                                               break;

		                                                     //Wait for Rejection state complete 
		                                                     case 0x09:writeFileLog("[JCM_InitAfterPortOpen()] Found Rejection State.");
                                                                               return 1;
		                                                               break;

		                                                     //Wait for Return state complete
		                                                     case 0x0A:writeFileLog("[JCM_InitAfterPortOpen()] Found Wait For return State.");
                                                                               return 1;
		                                                               break;

                                                                     //Remaining notes escrow state
		                                                     case 0x0D:writeFileLog("[JCM_InitAfterPortOpen()] Found Remaining notes in escrow state.");
                                                                               return 1;

		                                                               break;


		                                                     //No Suitable State found
		                                                     default:writeFileLog("[JCM_InitAfterPortOpen()] No State Found.");
                                                                             return 0;

		                                                             break;


                                                         };

                                                         
							    	                     
					  }
		                          else
					  {
						
                                                  //Log Current JCM Reply Data
                                                  int i=0;

		                                  for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                  {
						
		                                        memset(log,'\0',200);

		                                        sprintf(log,"[JCM_InitAfterPortOpen()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                        writeFileLog(log);
	 
		                                  }
                                 
		                                  writeFileLog("[JCM_InitAfterPortOpen()] JCM is not reply against sense command.");

		                                  return 0;


					  }


                             


	   }
	   else
	   {
	        writeFileLog("[JCM_InitAfterPortOpen()] JCM port open not done.");
                return 0;

	   }
           


}//JCM_InitAfterPortOpen() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int JCM_AcceptEscrowMoney()
{

            int rtcode=-1;

            writeFileLog("[JCM_AcceptEscrowMoney()] Found Remaining notes state.");

            writeFileLog("[JCM_AcceptEscrowMoney)] Before going for Accept escrow notes.");

            rtcode = JCM_Receipt();

            writeFileLog("[JCM_AcceptEscrowMoney()] After Accept escrow notes");

            if(1 == rtcode )
            {
		  
                 writeFileLog("[JCM_AcceptEscrowMoney()] Accept escrow notes at startup successfully.");
		 return 1;

            }
            else
            {
		 writeFileLog("[JCM_AcceptEscrowMoney()] Accept escrow notes at startup failed.");
		 return 0;

            }



}//JCM_AcceptEscrowMoney() end




//commit notes at startup
int JCM_DefaultCommit()
{

           if( -1 != g_HANDLE )
	   {

	       

                                          int rtcode=-1;

                                          unsigned char Response[100];

                                          memset(Response,'\0',100);

                                          unsigned char log[200];

                                          memset(log,'\0',200);

	                                  writeFileLog("[JCM_DefaultCommit()] Before Going to issue sense command.");

		                          rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                          writeFileLog("[JCM_DefaultCommit()] After issued sense command.");

                                          int replychecksum=0x00;

                                          replychecksum=GetBCC(Response,2,JCM_SENSE_REPLY_SIZE);

                                          //Log reply byte checksum value
                                          memset(log,'\0',200);

		                          sprintf(log,"[JCM_DefaultCommit()] Response[%d] = 0x%xh. replychecksum =0x%xh ",JCM_SENSE_REPLY_SIZE-1,Response[JCM_SENSE_REPLY_SIZE-1],replychecksum);

		                          writeFileLog(log);
	 

                                          if( replychecksum !=  Response[JCM_SENSE_REPLY_SIZE -1])
                                          {

                                                writeFileLog("[JCM_DefaultCommit()] Reply byte checksum is not matched.");                                 
                                                int i=0;

		                                for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                {
						
		                                      memset(log,'\0',200);

		                                      sprintf(log,"[JCM_DefaultCommit()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                      writeFileLog(log);
	 
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
		                                               writeFileLog(log);
	 
		                                         }
                                                        
                                                         //#endif

                                                         //Log Current State 
                                                         memset(log,'\0',200);

		                                         sprintf(log,"[JCM_DefaultCommit()] Current State = 0x%xh. ",Response[3]);
		                                         writeFileLog(log);
                                                      
                                                         
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
		                                                      writeFileLog("[JCM_DefaultCommit()] Not Found Remaining notes state.");
		                                                
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
		                                        writeFileLog(log);
	 
		                                  }
                                 
		                                  writeFileLog("[JCM_DefaultCommit()] JCM is not reply against sense command.");

		                                  return 0;


					  }


                             


	   }
	   else
	   {
	        writeFileLog("[JCM_DefaultCommit()] JCM port open not done.");

                return 0;

	   }
     



}//JCM_DefaultCommit() end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////



int JCM_RejectEscrowMoney()
{


              int rtcode=-1;
              
              unsigned char returnstate=0x00;

	      writeFileLog("[JCM_DefaultCancel()] Found Remaining notes state.");
		                                                     
              writeFileLog("[JCM_DefaultCancel()] Before going for Return escrow notes.");

              rtcode = JCM_Return(&returnstate);

              writeFileLog("[JCM_DefaultCancel()] After Return escrow notes");
	
              if( 0x0A == returnstate )
              {
		     writeFileLog("[JCM_DefaultCancel()] Return escrow notes at startup successfully but user not taken notes from outlet.");
		     
                     return 1;

	      }
	      else if( 0x02 == returnstate )
              {
		     writeFileLog("[JCM_DefaultCancel()] Return escrow notes at startup successfully and user also taken notes from outlet.");

		     return 1;

	      }
	      else if( 0x81 == returnstate )
              {
		     writeFileLog("[JCM_DefaultCancel()] Return escrow notes at startup failed due to alarm state .");

		     return 0;

	      }
	      else
	      {
		     writeFileLog("[JCM_DefaultCancel()] Return escrow notes at startup failed due to unknown reason.");
		     return 0;

              }

  
}//JCM_RejectEscrowMoney() end


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//return note at startup
int JCM_DefaultCancel()
{


           if( -1 != g_HANDLE )
	   {

	       

                                          int rtcode=-1;

                                          unsigned char Response[100];

                                          memset(Response,'\0',100);

                                          unsigned char log[200];

                                          memset(log,'\0',200);

	                                  writeFileLog("[JCM_DefaultCancel()] Before Going to issue sense command.");

		                          rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

                                          writeFileLog("[JCM_DefaultCancel()] After issued sense command.");

                                          int replychecksum=0x00;

                                          replychecksum=GetBCC(Response,2,JCM_SENSE_REPLY_SIZE);

                                          //Log reply byte checksum value
                                          memset(log,'\0',200);

		                          sprintf(log,"[JCM_DefaultCancel()] Response[%d] = 0x%xh. replychecksum =0x%xh ",JCM_SENSE_REPLY_SIZE-1,Response[JCM_SENSE_REPLY_SIZE-1],replychecksum);

		                          writeFileLog(log);
	 

                                          if( replychecksum !=  Response[JCM_SENSE_REPLY_SIZE -1])
                                          {

                                                writeFileLog("[JCM_DefaultCancel()] Reply byte checksum is not matched.");
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
		                                               writeFileLog(log);
	 
		                                         }
                                                        
                                                         //#endif

                                                         //Log Current State 
                                                         memset(log,'\0',200);

		                                         sprintf(log,"[JCM_DefaultCancel()] Current State = 0x%xh. ",Response[3]);
		                                         writeFileLog(log);

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
		                                                      writeFileLog("[JCM_DefaultCancel()] Not Found Remaining notes state.");
		                                                
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
		                                        writeFileLog(log);
	 
		                                  }
                                 
		                                  writeFileLog("[JCM_DefaultCancel()] JCM is not reply against sense command.");

		                                  return 0;


					  }


                             


	   }
	   else
	   {
	        writeFileLog("[JCM_DefaultCancel()] JCM port open not done.");

                return 0;

	   }
     




}//JCM_DefaultCancel() end




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int JCM_Activate(const int ComPortno)
{
	  
           
           g_HANDLE=-1;

	   int  rtcode=-1;

           //////////////////////////////////////////////////////

           //Now store com port numnber
           g_JcmPortNumber=ComPortno;
   
           //////////////////////////////////////////////////////

           rtcode=OpenPort(ComPortno, &g_HANDLE);
           
	   if( 1 == rtcode)
	   {
	        writeFileLog("[JCM_Activate()] JCM Note Acceptor Open Successfully.");
                rtcode=-1;
                rtcode=JCM_InitAfterPortOpen();
                if( 1 == rtcode)
                {
                    writeFileLog("[JCM_Activate()] JCM Note Acceptor Init Successfully Done.");
                    return 1;
                }
                else
                { 
                    writeFileLog("[JCM_Activate()] JCM Note Acceptor Init Failed to Done.");
                    writeFileLog("[JCM_Activate()] Now going to close JCM Note Acceptor ComPort.");
                    JCM_Deactivate();
                    return 0;    
                }
                
	   }
	   else
	   {
	        writeFileLog("[JCM_Activate()] JCM Note Acceptor Open failed!!.");
                return 0;

	   }
           
          


}//JCM_Activate(const int ComPortno) end



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int JCM_Deactivate()
{
     
	   
           int  rtcode=-1;
	   rtcode=ClosePort(g_HANDLE);
	   if( 1 == rtcode)
	   {
	        g_HANDLE=-1;
                writeFileLog("[JCM_Deactivate()] JCM com port successfully released.");
                return 1;
	   }
	   else
	   {
                writeFileLog("[JCM_Deactivate()] JCM com port  failed to closed.");
	        return 0;

	   }



}//end



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int JCM_LogSensorState(unsigned char Sensor_first,unsigned char Sensor_second,unsigned char Sensor_third)
{

             
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
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTC With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTC unknown status");
		            writeFileLog(log);
                            break;

             };

             switch( sensorstate.sensor_validation_ptr )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTR NO Note.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTR With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTR unknown status.");
		            writeFileLog(log);
                            break;

             };

             switch( sensorstate.sensor_acceptor_lid_detection )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Acceptor lid detection Sensor closed.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Acceptor lid detection Sensor open.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Acceptor lid detection Sensor  unknown status.");
		            writeFileLog(log);
                            break;

             };

             switch( sensorstate.sensor_insertion_detection )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion detection Sensor No Note.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Insertion detection Sensor With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion detection Sensor unknown status.");
		            writeFileLog(log);
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
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTL With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Validation Sensor PTL unknown status.");
		            writeFileLog(log);
                            break;

             };

             switch( sensorstate.sensor_outlet_shutter )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Outlet Shutter Sensor Closed.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Outlet Shutter Sensor Opened.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Outlet Shutter Sensor unknown status.");
		            writeFileLog(log);
                            break;

             };

            
             switch( sensorstate.sensor_freefall )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Free Fall Sensor No Note.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Free Fall Sensor With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Free Fall Sensor unknown status.");
		            writeFileLog(log);
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
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] S13 Position Sensor With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S13 Position Sensor unknown status.");
		            writeFileLog(log);
                            break;

             };

             switch( sensorstate.sensor_position_s14 )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S14 Position Sensor No Note.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] S14 Position Sensor With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S14 Position Sensor unknown status.");
		            writeFileLog(log);
                            break;

             };
             
             switch( sensorstate.sensor_position_s15 )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S15 Position Sensor No Note.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] S15 Position Sensor With Note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] S15 Position Sensor unknown status.");
		            writeFileLog(log);
                            break;

             };
               
             switch( sensorstate.sensor_stacking_control )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Stacking Control Sensor Normal Position.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Stacking Control Sensor UnNormal Position.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Stacking Control Sensor unknown status.");
		            writeFileLog(log);
                            break;

             };

             switch( sensorstate.sensor_insertion_detection_right )
             {

                    case 0: memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion Detection Sensor(Right) No note.");
		            writeFileLog(log);
                            break;

                    case 1:  memset(log,'\0',200);
		             sprintf(log,"[JCM_LogSensorState()] Insertion Detection Sensor(Right) with note.");
		             writeFileLog(log);
                             break;

                    default:memset(log,'\0',200);
		            sprintf(log,"[JCM_LogSensorState()] Insertion Detection Sensor(Right) unknown status.");
		            writeFileLog(log);
                            break;

             };




}//JCM_LogSensorState() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////


int JCM_ReInit(int WaitSecond)
{


           
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
		                           writeFileLog(log);
				     }

                                     #endif

                                     memset(log,'\0',200);
		                     sprintf(log,"[JCM_ReInit()] Command = 0x%xh State = 0x%xh .",Response[2],Response[3]);
		                     writeFileLog(log);
                           
                        }

                        if( (0x02 == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset Success in %d Second with standby state found.",diff.tv_sec);
		               writeFileLog(log);
                               return 1;

                        }    

                        else if( (0x0D == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset Success in %d Second with Remaining state found.",diff.tv_sec);
		               writeFileLog(log);
                               return 3;

                        }      

                        else if( (0x80 == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset failed in %d Second with sense trouble state found.",diff.tv_sec);
		               writeFileLog(log);
                               return 3;

                        } 

                        else if( (0x81 == Response[3]) && (0x30 == Response[2]) )
                        {

                               clock_gettime(CLOCK_MONOTONIC, &end);
                               diff.tv_sec = end.tv_sec - start.tv_sec;
                               memset(log,'\0',200);
		               sprintf(log,"[JCM_ReInit()] Reset failed in %d Second with Alarm state found.",diff.tv_sec);
		               writeFileLog(log);
                               return 4;

                        }                             
                       
                        //Check timer status
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        diff.tv_sec = end.tv_sec - start.tv_sec;

                        if( diff.tv_sec >= WaitSecond )
                        {

                                memset(log,'\0',200);
		                sprintf(log,"[JCM_ReInit()] Reset Timeout in %d Second .",diff.tv_sec);
		                writeFileLog(log);
                                return 2;
		                   
                        
                        }



           }// End for(;;)


	 
		                                                    

}//JCM_ReInit() end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int GetNoteDetailsInRecycleCst( int *CastQuanity )
{

             
          *CastQuanity=-1;

           if( -1 != g_HANDLE )
	   {

	                                  unsigned char Wait_State[4]={0x02,0x03,0x12,0x13};

                                          unsigned int  Wait_State_Length=4; 

                                          unsigned char StateFound=0x00;

                                          /*

                                          //Wait for 5 Second if any enq byte recv from jcm
                                          if(  1 == ReadEnqFromJcm() )
                                          {

                                                writeFileLog("[GetNoteDetailsInRecycleCst()] Enq Byte receieved from jcm.");

                                                writeFileLog("[GetNoteDetailsInRecycleCst()] Before go for wait jcm idle session.");

                                                WaitForState( 5,Wait_State,Wait_State_Length, &StateFound);

                                                writeFileLog("[GetNoteDetailsInRecycleCst()] After go for wait jcm idle session.");


                                          }

                                          */
        
                                          int rtcode=-1;

                                          unsigned char Response[100];
                                          memset(Response,'\0',100);

                                          unsigned char log[200];
                                          memset(log,'\0',200);

	                                  //issue sense command
		                          rtcode = JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);

		                          
		                          if( ( Response[3] > 0x00 ) && (0x40 == Response[2]) )
					  {
					                 
                                                         #ifdef JCM_DEBUG 

				                         int i=0;

		                                         for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                         {
						
		                                               memset(log,'\0',200);

		                                               sprintf(log,"[GetNoteDetailsInRecycleCst()] Sense Response[%d] = 0x%x h. ",i,Response[i]);
		                                               writeFileLog(log);
	 
		                                         }
                                                        
                                                         #endif

                                                         //Log Current State 
                                                         memset(log,'\0',200);

		                                         sprintf(log,"[GetNoteDetailsInRecycleCst()] Current State = 0x%xh. ",Response[3]);
		                                         writeFileLog(log);
                                    
                                                         //Log Current Sensor State
                                                         //++JCM_LogSensorState(Response[7],Response[8],Response[9]);

                                                         //calculate escrow notes
                                                                 
				                         int totalnotes=0;

                                                         totalnotes=GetEscrowMoneyPatchV2(Response,10);

							*CastQuanity=totalnotes;

                                                         memset(log,'\0',200);

			                                 sprintf(log,"[GetNoteDetailsInRecycleCst()] Total Escrow Notes= %d",   totalnotes);

						         writeFileLog(log);

                                                         return 1;

                                                         
							    	                     
					  }
		                          else
					  {
						
                                                  
		                                  writeFileLog("[GetNoteDetailsInRecycleCst()] JCM is not reply against sense command.");
                                                  int i=0;

		                                  for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                  {
						
		                                       memset(log,'\0',200);

		                                       sprintf(log,"[GetNoteDetailsInRecycleCst()] Sense Response[%d] = 0x%x h. ",i,Response[i]);
		                                       writeFileLog(log);
	 
		                                  }

		                                  return 0;


					  }


                             


	   }
	   else
	   {
	        writeFileLog("[GetNoteDetailsInRecycleCst()] JCM port open not done.");
                return 0;

	   }
           





}//GetNoteDetailsInRecycleCst() end



///////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Get Curent JCM status
int JCM_GetStatus()
{
           

              
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

                                             writeFileLog("[JCM_GetStatus()] Not matched with reply byte checksum value");


                                         }
		                          
		                         if( Response[3] >= 0x00 )
					 {

					                  
                                                         //#ifdef JCM_DEBUG

                                                         writeFileLog("=========================================================================");

				                         int i=0;

		                                         for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
				                         {
						
		                                               memset(log,'\0',200);

		                                               sprintf(log,"[JCM_GetStatus()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
		                                               writeFileLog(log);

	 
		                                          }

                                                          writeFileLog("=========================================================================");

                                                          //#endif  

                                                          memset(log,'\0',200);

		                                          sprintf(log,"[JCM_GetStatus()] Sense Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);
		                                          writeFileLog(log);

                                                          //Log Current JCM Sensor Status
                                                          //++JCM_LogSensorState(Response[7],Response[8],Response[9]);


                                                          if(   (0x40 == Response[2]) && 
                                                                (0x00 == Response[3]) 
                                                             
                                                            )
                                                          {

                                                             writeFileLog("[JCM_GetStatus() Found After power on state.");
                                                                
                                                             writeFileLog("[JCM_GetStatus() So going for reset jcm note acceptor.");
                                                             return (JCM_Reset());

                                                          }

                                                          //State 1:post transaction rejection/returning state
		                                          if( ( (0x40 == Response[2]) && (0x09 == Response[3]) ) ||
                                                              ( (0x40 == Response[2]) && (0x0A == Response[3]) ) 
                                                            )
						          {
								
                                                                
		                                                        StateFound=0x00;

		                                                        writeFileLog("[JCM_GetStatus() post transaction rejection/returning state] Now wait for any standby state.");

		                                                        //wait for standby without escrow state [0x02]  [30min]
		                                                        WaitForState( JCM_STANDBY_WAIT_TIME , Wait_State, Wait_State_Length, &StateFound);

		                                                        //standby without escrow state found
		                                                        if( 0x02 == StateFound )
		                                                        {
                                                                             writeFileLog("[JCM_GetStatus() post transaction rejection/returning state] Standby mode found.");    
		                                                             return 1;

		                                                        }
		              
		                                                        //notes with esrow state found
		                                                        else if( 0x12 == StateFound )
		                                                        {
		                                                             
		                                                             int CastQuanity=-1,rtcode=-1;

		                                                             GetNoteDetailsInRecycleCst( &CastQuanity );

		                                                             memset(log,'\0',100);

		                                                             sprintf(log,"[JCM_GetStatus() post transaction rejection/returning state] Current Recycle Quantity : %d",CastQuanity);

		                                                             writeFileLog(log);    
	       
		                                                             if(CastQuanity>0)
		                                                             {
		                                                                   
		                                                                   writeFileLog("[JCM_GetStatus() post transaction rejection/returning state] Before Going to unload all cash in free fall vault.");    
		                                                                   rtcode = JCM_Unload();
		                                                                   
		                                                                   writeFileLog("[JCM_GetStatus() post transaction rejection/returning state] After Going to unload all cash in free fall vault.");
		                                                                   return rtcode;


		                                                             }
		                                                             else
		                                                             {
		                                                                    writeFileLog("[JCM_GetStatus() post transaction rejection/returning state] Before Going to unload unknown recycle quanity found so return error here.");    
		                                                                    return 0;

		                                                             }
	   
		                                                        }

		                                                        //sensor state error
		                                                        else if( 0x80 == StateFound )
		                                                        {
		                                                            writeFileLog("[JCM_GetStatus() post transaction rejection/returning state] Sensor problem state found.");    
		                                                            //log current sensor status  
		                                                            JCM_LogSensorState(Response[7],Response[8],Response[9]);
		                                                            return 0;

		                                                        }

		                                                        //alarm state error
		                                                        else if( 0x81 == StateFound )
		                                                        {
		                                                            writeFileLog("[JCM_GetStatus() post transaction rejection/returning state] Alarm state found Going to reset.");    
		                                                            //if reset issue success    
		                                                            return(JCM_ReInit(20));

		                                                        }

                     		                   
							  }
                                        
                                                          //State 2:Sensor Trouble log all sensor status here
		                                          else if( (0x40 == Response[2]) && (0x80 == Response[3]) ) 
						          {
								writeFileLog("[JCM_GetStatus() [0x80] ] Found Sensor Trouble state."); 
                                                                //wait for [standby with/without escrow] /[ready]/[reset] state 
                                                                JCM_LogSensorState(Response[7],Response[8],Response[9]);

                                                                return 0;
						 	  		                 		                   
							  }

                                                          //State 3:Reset here Jamming state
		                                          else if( (0x40 == Response[2]) && (0x81 == Response[3]) ) 
						          {
							      writeFileLog("[JCM_GetStatus() [0x81] ] Found Jammed State.");   
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
                                                                         writeFileLog("[JCM_GetStatus() [0x13] ] Found Ready State with escorw notes.");
                                                                     }

                                                                     if( (0x40 == Response[2]) && (0x12 == Response[3]) ) 
                                                                     {
                                                                         writeFileLog("[JCM_GetStatus() [0x12] ] Found Standby State with escorw notes.");
                                                                     }

                                                                     if(0x13 == Response[3])
                                                                     {
                                                                         writeFileLog("[JCM_GetStatus() [0x13] ] Going to disabled jcm note acceptor.");
								         DisableAcceptance();

                                                                     }

                                                                     int CastQuanity=-1,rtcode=-1;

                                                                     GetNoteDetailsInRecycleCst( &CastQuanity );

                                                                     memset(log,'\0',100);

                                                                     sprintf(log,"[JCM_GetStatus() [0x12/0x13] ] Current Recycle Quantity : %d",CastQuanity);
                                                                     writeFileLog(log);    
       
                                                                     if(CastQuanity>0)
                                                                     {
                                                                           
                                                                           writeFileLog("[JCM_GetStatus() [0x12/0x13] ] Before Going to unload all cash in free fall vault.");    
                                                                           rtcode = JCM_Unload();
                                                                           
                                                                           writeFileLog("[JCM_GetStatus() [0x12/0x13] ] After Going to unload all cash in free fall vault.");
                                                                           return rtcode;


                                                                     }
                                                                     else
                                                                     {
                                                                            writeFileLog("[JCM_GetStatus() [0x12/0x13] ] Before unload unable to get current recycle quanity.so return error here.");
                                                                            return 0;

                                                                     }

						 	  		                 		                   
							  }

                                                          //State 5:everything is ok for start a new transaction
						          else if(  (0x40 == Response[2]) && (0x02 == Response[3]) ) 
						          {

								   writeFileLog("[JCM_GetStatus() [0x02] ] Found Standby State without escrow notes."); 
						                   writeFileLog("[JCM_GetStatus() [0x02] ] JCM is ok.");

						                   return 1;
						 	  		                 		                   
							  }

                                                          //State 6:everything is ok for start a new transaction
						          else if(  (0x40 == Response[2]) && (0x03 == Response[3]) ) 
						          {
								   
                                                                   writeFileLog("[JCM_GetStatus() [0x03] ] Found Ready for insertion state.");
                                                                   int rtcode=-1;

                                                                   rtcode=DisableAcceptance();

                                                                   if(1==rtcode)
                                                                   {
                                                                           writeFileLog("[JCM_GetStatus() [0x03] ] Disable Success.");
                                                                           return 1;

                                                                   }
                                                                   else
                                                                   {
                                                                           writeFileLog("[JCM_GetStatus() [0x03] ] Disable Failed.");
						                           return 0;

                                                                   }
						 	  		                 		                   
							  }
                                                          else
                                                          {
							         writeFileLog("[JCM_GetStatus()] Found Unknown JCM State .");

		                                                 return 0;

                                                          }


					  }
		                          else
					  {
						
                                                  writeFileLog("[JCM_GetStatus()] Sense Command issue failed.");

		                                  writeFileLog("[JCM_GetStatus()] JCM is not ok.");

		                                  return 0;


					  }

	   }
	   else
	   {
	        writeFileLog("[JCM_GetStatus()] JCM port open not done.");
                return 0;

	   }
           



}//JCM_GetStatus() end



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int DisableSpecificNotes(int fare)
{
                         
                         
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
			      writeFileLog("[DisableSpecificNotes()] Zero fare receieved.");
                            		       
                       }
                       
                       //issue command
		       rtcode=EnableDenom(UserDefinedNote); 

		       if( 1 == rtcode)
		       {
		            
                            writeFileLog("[DisableSpecificNotes()] Specific Notes Disable success."); 
                            return 1;
		       }
                       else 
		       {
                            writeFileLog("[DisableSpecificNotes()] Specific Notes Disable failed."); 
                            return 0;

		       }

       
	       
}//DisableSpecificNotes() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int EnableDenom(NOTE UserdefEnablebyte)
{
                

                int rtcode=-1;

                char log[200];

                memset(log,'\0',200);

                int i=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                unsigned char DefaultEnablebyte=0x00;

                if( JCM_ENABLE == UserdefEnablebyte.INR_5 )
                {

                     DefaultEnablebyte = DefaultEnablebyte | 0x01; //0b00000001;
                     writeFileLog("[ EnableDenom() ] Rs.5  is enable.");
                
                }

                if( JCM_ENABLE == UserdefEnablebyte.INR_10 )
	        {

		     DefaultEnablebyte = DefaultEnablebyte | 0x02; //0b00000010;
                     writeFileLog("[ EnableDenom() ] Rs.10  is enable.");

                }

                if( JCM_ENABLE == UserdefEnablebyte.INR_20 )
                {

		     DefaultEnablebyte = DefaultEnablebyte | 0x04; //0b00000100;
                     writeFileLog("[ EnableDenom() ] Rs.20  is enable.");

	        }

                if( JCM_ENABLE == UserdefEnablebyte.INR_50 )
	        {

                     DefaultEnablebyte = DefaultEnablebyte | 0x08; //0b00001000;
                     writeFileLog("[ EnableDenom() ] Rs.50  is enable.");

	        }

                if( JCM_ENABLE == UserdefEnablebyte.INR_100 )
                {

		    DefaultEnablebyte = DefaultEnablebyte | 0x10; //0b00010000;
                    writeFileLog("[ EnableDenom() ] Rs.100  is enable");

	        }

                if( JCM_ENABLE == UserdefEnablebyte.INR_500 )
	        {

		    DefaultEnablebyte = DefaultEnablebyte | 0x20; //0b00100000;
                    writeFileLog("[ EnableDenom() ] Rs.500  is enable.");

	        }

                if( JCM_ENABLE == UserdefEnablebyte.INR_1000 )
                {

		    DefaultEnablebyte = DefaultEnablebyte | 0x40; //0b01000000;
                    writeFileLog("[ EnableDenom() ] Rs.1000  is enable.");

                }

                
                writeFileLog("[ EnableDenom() ] Going To issue insertion auth command.");

                
                memset(log,'\0',200);
                sprintf(log,"[ EnableDenom() ] DefaultEnablebyte = 0x%x h",DefaultEnablebyte);


                //Now issue insertion authorised command to jcm
                rtcode = JCM_Cmd_InsertionAuthorized( DefaultEnablebyte,
                                                      Response,
                                                      JCM_INSERTION_AUTHORIZING_REPLY_SIZE,
                                                      JCM_RECV_DELAY
                                                   );
                 
                
                #ifdef JCM_DEBUG 

                //Display Enable Denom Packet
     
                for(i=0;i<JCM_INSERTION_AUTHORIZING_REPLY_SIZE;i++)
                {

                   
                     memset(log,'\0',200);
                     sprintf(log,"[ EnableDenom() ] Response[%d]= 0x%x h",i,Response[i]);
	             writeFileLog(log);


                }
                
                #endif
                



                if(  ( ( 0x50 == Response[2] ) && ( 0x03 == Response[3] ) )  ||
                     ( ( 0x50 == Response[2] ) && ( 0x13 == Response[3] ) )
                  )
                {
                     writeFileLog("[ EnableDenom() ] Enable Denom Success ."); 
                     return 1;
                
                }
                else
                {
                      writeFileLog("[ EnableDenom() ] Enable Denom failed ."); 
                      return (-1);

                }



}//EnableDenom() end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Disable all acceptance
int DisableAcceptance()
{
        
 
             int rtcode=-1;

             unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

             memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

             int ResponseLength=JCM_INSERTION_INHIBITING_REPLY_SIZE;

             unsigned int Delay=JCM_RECV_DELAY ;
 
             unsigned char log[200];
  
             //issue disable acceptance command
             rtcode=JCM_Cmd_InsertionInhibited( Response,ResponseLength,Delay ); 

             //display response packet
             #ifdef JCM_DEBUG 

             int Counter=0;

             for(Counter=0;Counter<ResponseLength;Counter++)
	     {
		
                     memset(log,'\0',200);
                     sprintf(log,"[DisableAcceptance()] DisableAcceptance[%d] = 0x%xh",Counter,Response[Counter]);
                     writeFileLog(log);

             }


             #endif

             if( Response[3]> 0x00 )
             {
		     
                    
                     if( 0x02 ==Response[3])
		     {
		         writeFileLog("[DisableAcceptance()] 0x02 Means Standby Without Escrow mode found.");

                         return 1;

		     }
	             else if( 0x12 ==Response[3])
		     {
		          writeFileLog("[DisableAcceptance()] 0x12 Means Standby With Escrow mode found.");

                          return 2;
		     }
		     else 
		     {
		          
                           memset(log,'\0',200);
	       
                           sprintf(log,"[DisableAcceptance()] No Standby mode found Command = x%xh State= x%xh", Response[2],Response[3]);

                           writeFileLog(log);

                           return 0;

		     }

          }
          else
          {
                
                memset(log,'\0',200);

                sprintf(log,"[DisableAcceptance()] Current State = 0x%xh",Response[3]);

                writeFileLog(log);

                writeFileLog("[DisableAcceptance()] DisableAcceptance command issue failed.");      
    
                return (-1);    

          }    

         
            
}//DisableAcceptance() end



///////////////////////////////////////////////////////////////////////////////////////////////////////////



int WaitForState(  const unsigned int  Wait_time,
                   unsigned char*      Wait_State,  
                   const unsigned int  Wait_State_Length, 
                   unsigned char*      StateFound
                 )
{
         
         
                
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

		               if(Response[3]>0x00)
		               {
		                          
                                          
                                           *StateFound= Response[3];

				           for(Counter=0;Counter<Wait_State_Length;Counter++)
				           {
						       
  
                                                         if( Response[3] == Wait_State[Counter])
				                         {
						                 clock_gettime(CLOCK_MONOTONIC, &endts);

								 diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                                                 memset(log,'\0',100);

                                                                 sprintf(log,"[WaitForState()] Found State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
                                                                 writeFileLog(log);
						                
								 return 1; //if given state found return 1

				                         }


				           }


		               }

                           
                               clock_gettime(CLOCK_MONOTONIC, &endts);

			       diffts.tv_sec = endts.tv_sec - begints.tv_sec;

			       if(diffts.tv_sec >= Wait_time)
                               {
                                   
                                   
		                           memset(log,'\0',100);

		                           sprintf(log,"[WaitForState()] Timeout Wait_time =%d diffts.tv_sec =%d.",Wait_time,diffts.tv_sec);
		                            
		                           writeFileLog(log);
								        
		                           for(Counter=0;Counter<Wait_State_Length;Counter++)
				           {
						         
		                                         
		                                         *StateFound= Response[3];

				                         if( Response[3] == Wait_State[Counter])
				                         {
						                 
		                                                 clock_gettime(CLOCK_MONOTONIC, &endts);

								 diffts.tv_sec = endts.tv_sec - begints.tv_sec;
						                
		                                                 memset(log,'\0',100);

		                                                 sprintf(log,"[WaitForState()] Found State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
		                            
		                                                 writeFileLog(log);

								 return 1; //if given state found return 1

				                         }

				           }

		                           
		                           memset(log,'\0',100);

		                           sprintf(log,"[WaitForState()] Timeout Current State = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);
		                            
		                           writeFileLog(log);

		                           return 2;  //if given state not found return 2 (timeout)

                                }



                 }//while block




}//WaitForState() end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int JCM_Make_StandBy(int OperationCode)
{
         
        
        unsigned char log[100];

        memset(log,'\0',100);

        unsigned char Wait_State[4]={0x02,0x03,0x12,0x13};

        unsigned int  Wait_State_Length=4; 

        unsigned char StateFound=0x00;

        int rtcode=1;

     
       //wait for standby state
       if(  1 != ReadEnqFromJcm() )
       {  
             
		     
                     writeFileLog("[JCM_Make_StandBy() Sense] No Enq Byte recv from JCM.Now go for issue sense command.");

		     unsigned char Response[JCM_SENSE_REPLY_SIZE];

		     memset(Response,'\0',JCM_SENSE_REPLY_SIZE);

		     rtcode=JCM_Cmd_Sense(Response,JCM_SENSE_REPLY_SIZE,JCM_RECV_DELAY);
	             
                     
                     #ifdef JCM_DEBUG 
		     if(Response[3] > 0)
		     {
			    
                            int i=0;

			    for(i=0;i<JCM_SENSE_REPLY_SIZE;i++)   
                            {
			           memset(log,'\0',100);
                                   sprintf(log,"[JCM_Make_StandBy()] Sense Response[%d] = 0x%xh. ",i,Response[i]);
                                   writeFileLog(log); 

                            }
				                             
		     }
                     #endif
                     

                     memset(log,'\0',100);

		     sprintf(log,"[JCM_Make_StandBy()] Sense Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);
		     writeFileLog(log);

		     if( Response[3] > 0 )
		     {

                                  if( 0x0D == Response[3]  )
                                  {
				         writeFileLog("[JCM_Make_StandBy()] Found Remaining notes states 0x0D.");

                                         return 1;

                                  }

		                  //normal state to do operation
		                  else if( (0x12 == Response[3] ) || (0x02 == Response[3] ) )
				  {
				        
                                        if( 0x80 == Response[2] )
                                        {
                                            
                                            writeFileLog("[JCM_Make_StandBy() RSP State Found ] RSP State Found Now wait for few seconds for reactivate JCM.");
                                            DelayInSeconds( 6 );

                                        }
                                        else
                                        {
                                              writeFileLog("[JCM_Make_StandBy()] Standby mode found.");
                                        }

		                        return 1;

				  }

		                  //DISABLE ACCEPTANCE
		                  else if( (0x03 == Response[3] ) || (0x13 == Response[3] ) )
		                  {
				               
                                               writeFileLog("[JCM_Make_StandBy() Enable State Found ] Enable State Found Going for disable JCM.");

                                               int Dis_rtcode=-1;

				               Dis_rtcode = DisableAcceptance();

				               if( (1 == Dis_rtcode ) || (2 == Dis_rtcode ) )
				               {
				                     
                                                     return 1;

				               }
                                               else
                                               {

                                                     return 1;

                                               }


		                  }

                                  //Sensor Problem State
		                  else if( 0x80 == Response[3] )
		                  {
				      
                                      StateFound=0x00;         

                                      writeFileLog("[JCM_Make_StandBy() Sensor Trouble] Sensor Problem State Found.");
                  
                                      writeFileLog("[JCM_Make_StandBy() Sensor Trouble] Going for sensor trouble state over checking.");

                                      WaitForState( JCM_SENSOR_OVER_WAIT_TIME ,Wait_State ,Wait_State_Length , &StateFound );                        
                                      if( (0x03 == StateFound  ) || (0x13 == StateFound  ) )
                                      {

		                              int Dis_rtcode=-1;

					      Dis_rtcode = DisableAcceptance();

                                              return Dis_rtcode;

					      
		                              
                                      }
                                      else if ( 0x80 == StateFound )
                                      {

                                             return (-1);
                                      }

                                      else if ( 0x12 == StateFound )
                                      {

                                             return 1;
                                      }

                                      else if ( 0x02 == StateFound )
                                      {

                                             return (-1);
                                      }
               

		                  }

                                  //Alarm Problem State
		                  else if( 0x81 == Response[3] )
		                  {
				      
                                      StateFound=0x00;         

                                      writeFileLog("[JCM_Make_StandBy() Alarm/Jamm Trouble] Alarm/Jamm Problem State Found.");
                  
                                      writeFileLog("[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for Reset JCM.");

                                      if(JCM_UNLOAD_OPERATION == OperationCode)
                                      {
                                             writeFileLog("[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for Reset JCM for unload operation.");
                                             JCM_ReInit(30);

                                             return (-1);

                                      }

                                      if(JCM_DISPENSE_OPERATION == OperationCode)
                                      {
                                             writeFileLog("[JCM_Make_StandBy() Alarm/Jamm Trouble] Going for return here dispense operation.");
                                             return (-1);

                                      }

		                  }

		                  //wait for reject state over
		                  else if( (0x08 == Response[3] ) || (0x09 == Response[3] ) )
				  {
				      
		                      writeFileLog("[JCM_Make_StandBy()] Now Wait for Reject State.");

				      rtcode = -1;

				      rtcode = WaitForState( JCM_REJECTION_OVER_WAIT_TIME,Wait_State,Wait_State_Length, &StateFound);

				      return rtcode;

				  }
				  else
				  {

                                        return (-1);



				  }

		     
		     }
                     else if ( 0x00 == Response[3] ) //if sense command issue failed
                     {
                            
                            writeFileLog("[JCM_Make_StandBy() sense command issue failed] Now Wait RSP State Change Packet as sense command issue failed.");
		            rtcode = -1;

		            writeFileLog("[JCM_Make_StandBy() sense command issue failed ] Now wait for some seconds for jcm again reactive.");
                            DelayInSeconds( 6 );

                            return 1;

                     }
		     else
		     {
		         return (-1);
		     } 

       }                        
       else
       {
              
              writeFileLog("[JCM_Make_StandBy() RSP State] Enq Byte recv from JCM,now go for issue read rsp packet.");
             
              WaitForState( JCM_STANDBY_STATE_WAIT_TIME,Wait_State,Wait_State_Length, &StateFound);
              
              return 1;

       }

      
       


}//JCM_Make_StandBy() end




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int JCM_Unload()
{
       
       

       int rtcode=-1;
       unsigned char Wait_State[4]= {0x02,0x03,0x12,0x13} ;
       unsigned int  Wait_State_Length=4; 
       unsigned char StateFound=0x00;
       unsigned char log[100];
       memset(log,'\0',100);

       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       rtcode=-1;
       
       rtcode= JCM_Make_StandBy(JCM_UNLOAD_OPERATION);

       
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       if( 1 == rtcode)
       {

               rtcode=-1;
               
               rtcode = JCM_Receipt();

               //wait for note fall in free fall box
	       if(1 ==rtcode)
	       {
		       
                       
		       writeFileLog("[JCM_Unload()] Unload Successfully done.");
		       return 1;

	       }
	       else
	       {

                      writeFileLog("[JCM_Unload()] Unload Command execute failed..");
                      return 0;
	       }

      }
      else
      {
              writeFileLog("[JCM_Unload()] Unload Command execute failed due to no standby state found!!!.");
              return (-1);

      }
       	

     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



}//JCM_Unload() end




//accept money from escrow to vault ( Standby or ready for insertion state required for this operation 

int JCM_Receipt()
{
                
                
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

                writeFileLog(log);  

                Commands[5]=bcc;

                #ifdef JCM_DEBUG

                for(Counter=0;Counter<6;Counter++)
                {

                       memset(log,'\0',100);
                       sprintf(log,"[JCM_Receipt()] Command[%d] = 0x%xh",Counter,Commands[Counter]);
                       writeFileLog(log);  


	        }

                #endif
       
                //Transmit receipt Command
                rtcode=-1;

                rtcode=JCM_TransmitCmdSequence( g_HANDLE, Commands ,JCM_CMD_CMN_LENGTH);

                if(1==rtcode)
                {
		        

                        struct timespec begints, endts,diffts;

                        clock_gettime(CLOCK_MONOTONIC, &begints);

                        while(1)
                        {


		                //Read RSP Packet from JCM
                                rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_RECEIPT_REPLY_SIZE);

                                //check reply bytes checksum value

                                int replychecksum=0x00;

                                replychecksum = GetBCC(Response,2,JCM_RECEIPT_REPLY_SIZE);

                                ////////////////////////////////////////////////////////////////////

                                //Log reply bytes checksum value

                                //memset(log,'\0',100);

                                //sprintf(log,"[JCM_Receipt()] Command[%d] = 0x%xh, replychecksum =0x%xh. ",JCM_RECEIPT_REPLY_SIZE-1,Commands[JCM_RECEIPT_REPLY_SIZE-1],replychecksum);
                                
                                //writeFileLog(log); 


                                ///////////////////////////////////////////////////////////////////

                                if( Response[JCM_RECEIPT_REPLY_SIZE-1] != replychecksum )
                                {

                                   writeFileLog("[JCM_Receipt()] Reply byte checksum value not matched .");
                                   
                                   return 0; 

                                }

		                if( Response[3] > 0x00  )  	
		                {

		                           
                                           //#ifdef JCM_DEBUG

                                           
                                           writeFileLog("[JCM_JCM_Receipt()] ---------------------------------------");
                                           
                                           for(Counter=0;Counter<JCM_RECEIPT_REPLY_SIZE;Counter++)   
                                           {
                                                 
                                                 memset(log,'\0',100);

                                                 sprintf(log,"[JCM_Receipt()] Response[%d] = 0x%xh",Counter,Response[Counter]);
                                                 writeFileLog(log); 

                                           }

				           writeFileLog("[JCM_Receipt()] -------------------------------------------");

                                           //#endif

                                           memset(log,'\0',100);
 
                                           sprintf(log,"[JCM_Receipt()] Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);

                                           writeFileLog(log); 

                                           //calculate time
                                           clock_gettime(CLOCK_MONOTONIC, &endts);

			                   diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                           //Standby mode found
					   if(  0x02  == Response[3]  )		                             
		                           {
						 
                                                 
                                                 memset(log,'\0',100);

                                                 sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                 writeFileLog(log); 

                                                 return 1;

					   }

                                           //Alarm state found
					   else if(  0x81  == Response[3]  )		                             
		                           {
						 
                                                 memset(log,'\0',100);

                                                 sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                 writeFileLog(log); 

                                                 return 3;

					   }


		                }
		               
                                clock_gettime(CLOCK_MONOTONIC, &endts);

			        diffts.tv_sec = endts.tv_sec - begints.tv_sec;

			        if(diffts.tv_sec >= JCM_UNLOAD_TIME)
                                {


                                            //Standby mode found
					    if(  0x02  == Response[3]  ) 
		                            {
					         
                                                 memset(log,'\0',100);

                                                 sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                 writeFileLog(log); 

                                                 return 1;

					    }

                                            //alarm state found
					    else if(  0x81  == Response[3]  )		                             
		                            {
						  
                                                   memset(log,'\0',100);

                                                   sprintf(log,"[JCM_Receipt()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                   writeFileLog(log); 

                                                   return 3;

					    }   
                                            else
                                            {
                                                   
                                                  memset(log,'\0',100);

                                                  sprintf(log,"[JCM_Receipt()] Timeout for receipt operation. State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);
                                                  writeFileLog(log); 

                                                  return 2; //Timeout

                                            }



                                }



                      }//while loop

		        
                 }
                 else
                 {

                        writeFileLog("[JCM_Receipt()] Unload command transmit failed.");

                        return 0;
                 
                }        





}//JCM_Receipt() end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Return money to customer
int JCM_Dispense()
{


	       int rtcode=-1;
	       unsigned char Wait_State[4] ={0x02,0x03,0x12,0x13};
	       unsigned int  Wait_State_Length=4; 
	       unsigned char StateFound=0x00;
	       unsigned char log[100];
               memset(log,'\0',100); 
	       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               
               //Wait for Standby mode

	       rtcode=-1;

	       rtcode= JCM_Make_StandBy(JCM_DISPENSE_OPERATION);

	     
	      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
	       
	       if( 1 == rtcode)
	       {
		       
		       unsigned char State=0x00;

		       //Issue return command to jcm
		       rtcode=-1;

		       rtcode=JCM_Return(&State);
		       
		       //wait for note recv by customer
		       if( 0x0a == State  )
		       {

                                       
				       writeFileLog("[JCM_Dispense() Wait for Receieve block] Note dispense successfully executed by JCM but note still is not taken back by customer.");
                              
                                       return 4;

	 

		       }
		       else if( 0x02 == State  )
		       {

		              memset(log,'\0',100); 

		              sprintf(log,"[JCM_Dispense() Standby State ] Return Command execute success State Found= 0x%xh",State);
		              writeFileLog(log);

		              return 1;

		       }
                       else if( 0x81 == State  )
		       {

		             
		              memset(log,'\0',100); 

		              sprintf(log,"[JCM_Dispense() Notes Jamming State ] Return Command execute success State Found= 0x%xh",State);
		              writeFileLog(log);

		              return 0;

		       }
		       else if( 0x00 == State  )
		       {
		              memset(log,'\0',100); 

		              sprintf(log,"[JCM_Dispense()] Return Command execute failed and State Found= 0x%xh",State);
		              writeFileLog(log);

		              return 0;
		            
		       }


	      }
	      else
	      {
		       writeFileLog("[JCM_Dispense()] Return Command execute failed due to no standby state found!!!.");
		       return (-1);

	      }
	      

	     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



}




//return money from escrow to customer ( Standby state required for this operation [0x02/0x12/0x03/0x13] )
int JCM_Return(unsigned char *ReturnState)
{

 
                int rtcode=-1,Counter=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);
              
                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_RETURN_CMD;
                  
                unsigned char bcc = GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
       
                unsigned char Calculatebcc=0x00;

                unsigned char log[100];

                memset(log,'\0',100);

                //sprintf(log,"[JCM_Return()] GetBCC CheckSum = 0x%xh",bcc);

                //writeFileLog(log); 

                Commands[5]=bcc;

              
                #ifdef JCM_DEBUG

                for(Counter=0;Counter<6;Counter++)
                {
                      
                       memset(log,'\0',100);

                       sprintf(log,"[JCM_Return()] Command[%d] = 0x%xh.",Counter,Commands[Counter]);

                       writeFileLog(log); 

	        }
                
                #endif

                //Transmit Command
                rtcode=JCM_TransmitCmdSequence( g_HANDLE, Commands ,JCM_CMD_CMN_LENGTH);
                       
             
                //Wait for standby state 
                if(1==rtcode)
                {

		                *ReturnState=0x00;

                                ////////////////////////////////////////////////////////////////////////////////////
                                memset(log,'\0',100);


                                sprintf(log,"[JCM_Return()] Dispense Wait Time = %d Seconds.",JCM_RETURN_TIME);

                                writeFileLog(log); 

                                ///////////////////////////////////////////////////////////////////////////////////

                                struct timespec begints, endts,diffts;

                                clock_gettime(CLOCK_MONOTONIC, &begints);

                                while(1)
				{
				               
                                               //get return response reply
				               rtcode=-1;

					       memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                                               //Read RSP Packet from JCM
					       rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_RETURN_REPLY_SIZE);

                                               #ifdef JCM_DEBUG

                                               if(Response[3] > 0)
		                               {
		                                    
                                                     int Counter=0;

		                                     for(Counter=0;Counter<JCM_RETURN_REPLY_SIZE;Counter++)  
                                                     { 

		                                          memset(log,'\0',100);

                                                          sprintf(log,"[JCM_Return()] Response[%d] = 0x%xh. ",Counter,Response[Counter]);

                                                          writeFileLog(log); 

                                                     }

		                                     
		                               }

                                               #endif

                                               //////////////////////////////////////////////////////////////
                                               /*
                                               //Calculate reply byte checksum value
                                               int replychecksum=0x00;
 
                                               replychecksum = GetBCC(Response,2,JCM_RETURN_REPLY_SIZE);

                                               //memset(log,'\0',100);


		                               //sprintf(log,"[JCM_Return()] Response[%d]= 0x%xh. replychecksum  = 0x%xh.",JCM_RETURN_REPLY_SIZE-1,Response[JCM_RETURN_REPLY_SIZE-1],replychecksum);

		                               //writeFileLog(log); 

                                               if(replychecksum != Response[JCM_RETURN_REPLY_SIZE-1] )
                                               {

                                                     writeFileLog("[JCM_Return()] reply byte checksum not mmatched."); 

                                                     //continue;


                                               }
                                               */

                                               //////////////////////////////////////////////////////////////

                                               if( Response[3] > 0x00 )
                                               {

		                                       memset(log,'\0',100);


		                                       sprintf(log,"[JCM_Return()] Command = 0x%xh. State = 0x%xh.",Response[2],Response[3]);

		                                       writeFileLog(log); 


                                               }

					       if(Response[3] > 0 )
					       {
						       

                                                       *ReturnState=Response[3];   
    
                                                        memset(log,'\0',100);

                                                        sprintf(log,"[JCM_Return()] Current State Found = 0x%xh.",*ReturnState);

                                                        writeFileLog(log); 
                      
						       
                                                       //calculate time
                                                       clock_gettime(CLOCK_MONOTONIC, &endts);

			                               diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                                       //note is taken back by customer
						       if( 0x02 == Response[3]  )
                                                       {
							     
                                                              memset(log,'\0',100);


                                                              sprintf(log,"[JCM_Return()] State Found = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);

                                                              writeFileLog(log); 

                                                              return 1;

						       }

                                                       //jamming state
						       else if( 0x81 == Response[3] )  
                                                       {
							      
                                                              memset(log,'\0',100);


                                                              sprintf(log,"[JCM_Return()] State Found = 0x%xh. Time =%d Seconds.",Response[3],diffts.tv_sec);

                                                              writeFileLog(log); 

                                                              return 3;

						       }


                                              }

                                              clock_gettime(CLOCK_MONOTONIC, &endts);

			                      diffts.tv_sec = endts.tv_sec - begints.tv_sec;

			                      if(diffts.tv_sec >= JCM_RETURN_TIME)
                                              {
		                                       

                                                           memset(log,'\0',100);


                                                           sprintf(log,"[JCM_Return()] Timeout Current State = 0x%xh. Previous State = 0x%xh.  Time =%d Seconds.",Response[3],*ReturnState,diffts.tv_sec);

                                                           writeFileLog(log); 


		                                           //standby mode found
		                                           if(  0x02 == *ReturnState  )
		                                           {

		                                                return 1;

		                                           }
                                                           //waiting state
                                                           else if(  0x0a == *ReturnState )
		                                           {

		                                                return 4;

		                                           }

		                                           //Alarm state
		                                           else if( 0x81 == *ReturnState ) 
		                                           {

		                                                return 3;

		                                           }

		                                           else //timeout but not any state recvd
		                                           {

		                                                return 2;

		                                           }
		                                          

                                              }

				}                   

                }

                else
                {
                    
                     writeFileLog("[JCM_Return()] return copmmand transmit failed!!...");
                     return 0;
              
                }



}//JCM_Return() end





//accept money again which is not taken by customer ( Standby state required for this operation) 
int JCM_Intake()
{

        
                
                int rtcode=-1,Counter=0;

                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);
              
                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_INTAKE_CMD;
                  
                unsigned char bcc = GetBCC(Commands,2,JCM_CMD_CMN_LENGTH);
       
                unsigned char Calculatebcc=0x00;

                unsigned char log[100];

                memset(log,'\0',100);

                sprintf(log,"[JCM_Intake()] GetBCC CheckSum = 0x%xh",bcc);

                writeFileLog(log); 

                Commands[5]=bcc;

                #ifdef JCM_DEBUG

                for(Counter=0;Counter<6;Counter++)
                {
                      

                       memset(log,'\0',100);

                       sprintf(log,"[JCM_Intake()] Command[%d] = 0x%xh",Counter,Commands[Counter]);

                       writeFileLog(log); 


	        }

                #endif

                //Transmit Command
                rtcode=JCM_TransmitCmdSequence( g_HANDLE, Commands ,JCM_CMD_CMN_LENGTH);

                                    
                //Wait for standby state 
                if(1==rtcode)
                {
		                
                                struct timespec begints, endts,diffts;
                                clock_gettime(CLOCK_MONOTONIC, &begints);
                                while(1)
				{
				               //Check RSP State reply size
				               rtcode=-1;

					       memset(Response,'\0',JCM_INTAKE_REPLY_SIZE);

					       rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,JCM_INTAKE_REPLY_SIZE);


					       if(Response[3] >0)
					       {
							                                   
						       //Response[6]=0x0d;
                                                       Calculatebcc=GetBCC(Response,2,JCM_INTAKE_REPLY_SIZE);

                                                       memset(log,'\0',100);

                                                       sprintf(log,"[JCM_Intake()] CalculateBCC = 0x%x h  ReplyBcc= 0x%x h",Calculatebcc,Response[24]);

                                                       writeFileLog(log); 
                                                       
		                                       if(Response[3] >0)
		                                       {
		                                          int i=0;
		                                          for(i=0;i<JCM_INTAKE_REPLY_SIZE;i++) 
                                                          {  
		                                               

                                                               memset(log,'\0',100);

                                                               sprintf(log,"[JCM_Intake()] Response[%d] = 0x%xh. ",i,Response[i]);

                                                               writeFileLog(log); 

                                                          }

		                                     
		                                       }
                                                       
					               clock_gettime(CLOCK_MONOTONIC, &endts);
			                               diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                                       //waiting to be receieved
						       if( 0x02 == Response[3] )  
                                                       {
                                                              
                                                              memset(log,'\0',100);

                                                              sprintf(log,"[JCM_Intake()] State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);

                                                              writeFileLog(log); 

                                                              return 1;

						       }

                                                       else if( 0x81 == Response[3] ) // jamming state 
                                                       {
		                                              
                                                              memset(log,'\0',100);

                                                              sprintf(log,"[JCM_Intake()] Jammimg State Found = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);

                                                              writeFileLog(log); 
                                                              
                                                              return 3;

		                                       }


                                               }

                                               clock_gettime(CLOCK_MONOTONIC, &endts);
			                       diffts.tv_sec = endts.tv_sec - begints.tv_sec;
			                       if(diffts.tv_sec >= JCM_RETURN_TIME)
                                               {
		                                           
                                                           memset(log,'\0',100);


                                                           sprintf(log,"[JCM_Intake()] Timeout Current State = 0x%xh. Time =%d Seconds",Response[3],diffts.tv_sec);

                                                           writeFileLog(log);

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
                                                  

                                              }

				}                   

                }

                else
                {
                    writeFileLog("[JCM_Intake()] Dispense copmmand transmit failed!!...");
              
                }



}//JCM_Intake() end




//delay in seconds
void DelayInSeconds( int seconds )
{
		  
                  
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

                                 //writeFileLog(log);

                          }
			  if(diffts.tv_sec >= seconds)
                          {
                               return;

                          }

                 }



}//DelayInSeconds() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int Make_JCM_StandBy_Mode(unsigned int WaitTime)
{

      
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



int GetEscrowMoneyPatchV2(unsigned char *Response,int EscrowIndex)
{
         
         int Counter=0,DoubleDle=0,DoubleDleIndex=0,MoneyCounter=0;
         unsigned char Money[7];
         memset(Money,'\0',7);
         bool errordetect=false;

         /*
           
           EscrowIndex   = Rs 5
           EscrowIndex+1 = Rs 10
           EscrowIndex+2 = Rs 20
           EscrowIndex+3 = Rs 50
           EscrowIndex+4 = Rs 100
           EscrowIndex+5 = Rs 500
           EscrowIndex+6 = Rs 1000

           //user defined Money[] index
           Index 0 = Rs 5
           Index 1 = Rs 10
           Index 2 = Rs 20
           Index 3 = Rs 50
           Index 4 = Rs 100
           Index 5 = Rs 500
           Index 6 = Rs 1000
       
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
                        writeFileLog(log);

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



inline void DisplayEscrowTableV2(unsigned char *currentnote,int EscrowIndex)
{


                 

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

                 writeFileLog(log);

                 




}//DisplayEscrowTableV2() end



void GetEscrowMoneyPatch(JCM_ESCROW_NOTE *currentnote,unsigned char *Response,int EscrowIndex)
{
         
         int Counter=0,DoubleDle=0,DoubleDleIndex=0,MoneyCounter=0;
         unsigned char Money[7];
         memset(Money,'\0',7);
         bool errordetect=false;

         /*
           
           EscrowIndex   = Rs 5
           EscrowIndex+1 = Rs 10
           EscrowIndex+2 = Rs 20
           EscrowIndex+3 = Rs 50
           EscrowIndex+4 = Rs 100
           EscrowIndex+5 = Rs 500
           EscrowIndex+6 = Rs 1000

           //user defined Money[] index
           Index 0 = Rs 5
           Index 1 = Rs 10
           Index 2 = Rs 20
           Index 3 = Rs 50
           Index 4 = Rs 100
           Index 5 = Rs 500
           Index 6 = Rs 1000
       
        */

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
                        writeFileLog(log);

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





inline void DisplayEscrowTable(JCM_ESCROW_NOTE *currentnote)
{


             

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

                 writeFileLog(log);

                 




}//DisplayEscrowTable(JCM_ESCROW_NOTE *currentnote) end


////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////


static int ReadReceptibleState(int Seconds)
{


	     
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
            writeFileLog("[ReadReceptibleState()] Now Sending ENQ to JCM .");

            SendSingleByteToSerialPort(g_HANDLE,ENQ);
         
            writeFileLog("[ReadReceptibleState()] Now Going to read response byte from JCM. ");

            //Start Timer
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            for(;;)
            {        
			
                         

                         ReceiveSingleByteFromSerialPort(g_HANDLE,&DLE[0],&totalByteRead);

                         ReceiveSingleByteFromSerialPort(g_HANDLE,&DLE[1],&totalByteRead);

		         if( 0x03 == DLE[0] )
		         {
		             
                             memset(log,'\0',200);
                             sprintf(log,"[ReadReceptibleState()] EOT Recieved= 0x%x h",DLE[0]);
                             writeFileLog(log);  
		             return 0;

		         }

                         if( (0x10 == DLE[0] ) && (0x30 == DLE[1] ) )
		         {
		             
                             memset(log,'\0',200);
                             sprintf(log,"[ReadReceptibleState()] DLE0 Receieved DLE0[0] = 0x%x h  DLE0[1] = 0x%x h",DLE[0],DLE[1]);
                             writeFileLog(log);  

                             SendSingleByteToSerialPort(g_HANDLE,EOT);

                             ReceiveSingleByteFromSerialPort(g_HANDLE,&ScratchPad,&totalByteRead);

                             memset(log,'\0',200);
                             sprintf(log,"[ReadReceptibleState()] ScratchPad  = 0x%x h",ScratchPad);
                             writeFileLog(log); 
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
		                     writeFileLog(log);  
				     return 0;

				 }
		                 else if( (0x10 == DLE[0] ) && (0x30 == DLE[1] ) )
				 {
				     
		                     memset(log,'\0',200);
		                     sprintf(log,"[ReadReceptibleState()] DLE0 Receieved DLE0[0] = 0x%x h  DLE0[1] = 0x%x h",DLE[0],DLE[1]);
		                     writeFileLog(log);  
		                     
		                     SendSingleByteToSerialPort(g_HANDLE,EOT);

                                     ReceiveSingleByteFromSerialPort(g_HANDLE,&ScratchPad,&totalByteRead);
		                     memset(log,'\0',200);
		                     sprintf(log,"[ReadReceptibleState()] ScratchPad  = 0x%x h.",ScratchPad);
		                     writeFileLog(log); 
                                     ScratchPad=0x00;
				     return 1;

				 }
                                else
                                {
                                      writeFileLog("[ReadReceptibleState()] Timeout State.");
		                      return 2;    
                                }

                         }



          }

	 


}//ReadReceptibleState() end



static int ReadEnqFromJcmWithinTime(int Seconds)
{

	     
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
			
                         rtcode=ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);
		         if( 0x05 == enq )
		         {
		             
                             memset(log,'\0',200);
                             sprintf(log,"[ReadEnqFromJcm()] enq= 0x%x h",enq);
                             writeFileLog(log);  
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



static int ReadEnqFromJcm()
{

	     
             int rtcode=-1;
	     int totalByteRead=1;
	     unsigned char enq=0x00;

             /*
             int try=0;
	     for(try=0;try<1;try++)
	     {
		     
		     rtcode=ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);
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
			
                         rtcode=ReceiveSingleByteFromSerialPort(g_HANDLE,&enq,&totalByteRead);
		         if( 0x05 == enq )
		         {
		             memset(log,'\0',200);
                             sprintf(log,"[ReadEnqFromJcm()] enq= 0x%x h",enq);
                             writeFileLog(log);  
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
                        
                         }



          }

	 


}//ReadEnqFromJcm() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////


//get current accepted cashvalue
int  JCM_CashIn(int fare,int *State,int *Credit)
{
              
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Disable_Acceptance_During_RSPPoll()
{

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
           
                             writeFileLog("[Disable_Acceptance_During_RSPPoll()] Command Issue success.");
                             return 1;
                            
                   
                     }
	             //issue disable acceptance command
		     else
                     {
                            writeFileLog("[Disable_Acceptance_During_RSPPoll()] Command Issue failed.");
                            return 0;
                     }



          

}//Disable_Acceptance_During_RSPPoll() end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////



int GetDeonm(JCM_ESCROW_NOTE *currentnote,JCM_ESCROW_NOTE *oldnote)
{
        
               
                if( currentnote->AcceptedNumber_INR_5 != oldnote->AcceptedNumber_INR_5 )
                {

		    writeFileLog("[JCM_GetDenom_From_Escrow()] State Rs 5 Accepted.");
		    return 5;
                }
                else if( currentnote->AcceptedNumber_INR_10 != oldnote->AcceptedNumber_INR_10)
                {

		    writeFileLog("[JCM_GetDenom_From_Escrow()] State Rs 10 Accepted.");
		    return 10;
               }
               else if( currentnote->AcceptedNumber_INR_20 != oldnote->AcceptedNumber_INR_20)
	       {
								
		    writeFileLog("[JCM_GetDenom_From_Escrow()] State Rs 20 Accepted.");
		    return 20;
               }
               else if( currentnote->AcceptedNumber_INR_50 != oldnote->AcceptedNumber_INR_50)
	       {
								
		    writeFileLog("[JCM_GetDenom_From_Escrow()] State Rs 50 Accepted.");
		    return 50;
               }
               else if( currentnote->AcceptedNumber_INR_100 != oldnote->AcceptedNumber_INR_100)
	       {
								
	            writeFileLog("[JCM_GetDenom_From_Escrow()] State Rs 100 Accepted.");
		    return 100;
               }
               else if( currentnote->AcceptedNumber_INR_500 != oldnote->AcceptedNumber_INR_500)
	       {
								
	            writeFileLog("[JCM_GetDenom_From_Escrow()] State Rs 500 Accepted.");
		    return 500;
	       }
               else if( currentnote->AcceptedNumber_INR_1000 != oldnote->AcceptedNumber_INR_1000)
	       {
                    writeFileLog("[JCM_GetDenom_From_Escrow()] State Rs 1000 Accepted.");
		    return 1000;
               }
               else
               {
                    writeFileLog("[JCM_GetDenom_From_Escrow()] No money accepted.");
		    return 0;
               }	
                                                            


}//JCM_GetDenom_From_Escrow() end


//g_JCM_Accepted_Moneyvalue
static unsigned int JCM_GetDenom_From_EscrowState(unsigned char* Response, JCM_ESCROW_NOTE *currentnote)
{
                                                  
                                                   
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


///////////////////////////Jcm Transmit and Receieve Patterns//////////////////////////////////////////////


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
                         writeFileLog("[JCM_TransmitCmdSequence()] clear transmit register success.");
                         #endif

                    }
                    else //clear transmit register failed
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] clear transmit register failed.");
                         #endif

                    }

                    //clear receive register    
                    rtcode=-1;                           
                    rtcode=ClearReceiveBuffer (HANDLE);
                    if( 1 == rtcode ) //clear receieve register success
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] clear receieve register success.");
                         #endif

                    }
                    else //clear receieve register failed
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] clear receieve register failed.");
                         #endif

                    }

		    //Step 1:Send ENQ character
                    rtcode=-1;
		    rtcode=SendSingleByteToSerialPort( HANDLE , JCM_ENQ );
		    if( 1 == rtcode ) //send enq success
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] Send ENQ Success.");
                         #endif

                    }
                    else //send enq failed now abort
                    {
                         //#ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] Send ENQ Failed."); 
                         //#endif
                         return 2;
                      
                    }

                    //Step 2:Delay between after ENQ transmission character send
		    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
		  
                    #ifdef JCM_DEBUG
		    writeFileLog("[JCM_TransmitCmdSequence()] Read dle0 status.");
                    #endif

		    //Step 3:Wait for DLE0 to start sending command
                    recvbyte=0x00;
                    totalByteRead=0;
                    

                    //Read Only 2 byte
                    for(i=0;i<2;i++)
                    {
                            
		                    rtcode=-1;

		                    rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&DLE0[i],&totalByteRead);

                                    if( 1 == rtcode ) //read com port success
				    {
                                         #ifdef JCM_DEBUG
                                         
                                         memset(log,'\0',200);
                                         sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh success.",i,DLE0[i]);
                                         writeFileLog(log); 
                                         #endif

				    }
				    else //send com port failed
				    {
				         #ifdef JCM_DEBUG
                                         
                                         memset(log,'\0',200);
                                         sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh failed.",i,DLE0[i]);
                                         writeFileLog(log); 
                                         #endif
                                      
				    }

		    }
                   
                    //Check DLE0 bytes
                    if( (JCM_DLE0_FIRST_BYTE == DLE0[0]) &&  (JCM_DLE0_SECOND_BYTE == DLE0[1]) )
                    {
                           #ifdef JCM_DEBUG
                           writeFileLog("[JCM_TransmitCmdSequence()] Read DLE0  success.");
                           #endif
                       
                    }

                    else if( 0x04 == DLE0[0] )
                    {
                           //#ifdef JCM_DEBUG
                           writeFileLog("[JCM_TransmitCmdSequence()] Read EOT Bytes any comand transmission now aborted.");
                           return 3;
                           //#endif
                       
                    }

                    else //if not DLE0 bytes not recv now abort
                    {
                          //#ifdef JCM_DEBUG
                          writeFileLog("[JCM_TransmitCmdSequence()] Read DLE0  failed.");
                          //#endif
                          return 3;
                    }
                    
                    #ifdef JCM_DEBUG
		    writeFileLog("[JCM_TransmitCmdSequence()] send command now.");
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

		                  rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&DLE1[i],&totalByteRead);

		                  if( 1 == rtcode ) //read com port success
				  {
                                      #ifdef JCM_DEBUG 

                                      memset(log,'\0',200);
                                      sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh success.",i,DLE1[i]);
                                      writeFileLog(log); 
                                      
                                      #endif

				  }
				  else //send com port failed
				  {
				       //#ifdef JCM_DEBUG 
                                       
                                       memset(log,'\0',200);
                                       sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh failed.",i,DLE1[i]);
                                       writeFileLog(log); 

                                       //#endif

				  }
                    }

		    //Check DLE1 bytes
                    if( (JCM_DLE1_FIRST_BYTE == DLE1[0]) &&  (JCM_DLE1_SECOND_BYTE == DLE1[1]) )
                    {
                          #ifdef JCM_DEBUG
                          writeFileLog("[JCM_TransmitCmdSequence()] Read DLE1 success.");
                          #endif

                    }
                    else //if not DLE0 bytes recv failed abort
                    {
                          //#ifdef JCM_DEBUG
                          writeFileLog("[JCM_TransmitCmdSequence()] Read DLE1 failed.");
                          //#endif
                          return 5;
                    }

		    #ifdef JCM_DEBUG
		    writeFileLog("[JCM_TransmitCmdSequence()] Sending eot command .");
                    #endif

		    //Step 6:Now send eot to terminate command sending sequence
                    JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
                    rtcode=-1;
		    rtcode=SendSingleByteToSerialPort(HANDLE,JCM_EOT);
                    if( 1 == rtcode ) //write com port success
		    {
                        #ifdef JCM_DEBUG 
                        writeFileLog("[JCM_TransmitCmdSequence()] Send EOT success.");
                        #endif

		    }
		    else //write com port failed
		    {
		        //#ifdef JCM_DEBUG      
                        writeFileLog("[JCM_TransmitCmdSequence()] Send EOT failed.");
                        //#endif
                        return 6;
		    }
                    
                    
                    #ifdef JCM_DEBUG
                    writeFileLog("[JCM_TransmitCmdSequence()] Transmission End.");
		    writeFileLog("[JCM_TransmitCmdSequence()] ________________________________________________________________");
                    #endif
                  
                    return 1;


}


*/



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*

return code:

1 = Success
2 = ENQ send failed
3 = DLE0 recv failed
4 = Command write failed
5 = DLE1 recv failed
6 = EOT write failed

*/





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
                     
                    int rtcode=-1,counter=1; 
                    bool complete[2];
                    
                    #ifdef JCM_TRANS_TIME_DEBUG
                    struct timespec begints, endts,diffts;
                    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begints);
                    #endif

                    #ifdef JCM_DEBUG
                    for(i=0;i<JCM_NumberofBytestobeWrite;i++)
	            {
	                     
                             memset(log,'\0',200);

		             sprintf(log,"[JCM_TransmitCmdSequence()] JCM_Cmd[%d] = 0x%xh.",i,JCM_Cmd[i]);

		             writeFileLog(log); 

	            }
                    writeFileLog("[JCM_TransmitCmdSequence()] ________________________________________________________________");

		    writeFileLog("[JCM_TransmitCmdSequence()] Transmission Start.");

		    writeFileLog("[JCM_TransmitCmdSequence()]  Sending enq command .");
                    #endif

                    //clear comport before doing any operation
                    
                    //clear transmit register
                    rtcode=-1;
                    rtcode=ClearTransmitBuffer(HANDLE);
                    if( 1 == rtcode ) //clear transmit register success
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] clear transmit register success.");
                         #endif

                    }
                    else //clear transmit register failed
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] clear transmit register failed.");
                         #endif

                    }

                    //clear receive register    
                    rtcode=-1;                           
                    rtcode=ClearReceiveBuffer (HANDLE);
                    if( 1 == rtcode ) //clear receieve register success
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] clear receieve register success.");
                         #endif

                    }
                    else //clear receieve register failed
                    {
                         #ifdef JCM_DEBUG
                         writeFileLog("[JCM_TransmitCmdSequence()] clear receieve register failed.");
                         #endif

                    }

		   
                     //Step 1:Send ENQ character
                     rtcode=-1;
		     rtcode=SendSingleByteToSerialPort( HANDLE , JCM_ENQ );
		     if( 1 == rtcode ) //send enq success
		     {
		            #ifdef JCM_DEBUG
		            writeFileLog("[JCM_TransmitCmdSequence()] Send ENQ Success.");
		            #endif

		     }
		     else //send enq failed now abort
		     {
		            //#ifdef JCM_DEBUG
		            writeFileLog("[JCM_TransmitCmdSequence()] Send ENQ Failed."); 
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

						    rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&DLE0[i],&totalByteRead);

				                    if( ( 1 == rtcode ) &&  (DLE0[i] > 0x00) ) //read com port success
						    {

				                         #ifdef JCM_DEBUG
				                         
				                         //memset(log,'\0',200);

				                         //sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh success.",i,DLE0[i]);
				                         //writeFileLog(log); 

				                         #endif
                                                         complete[i]=true;

						    }
						    else //send com port failed
						    {
							 #ifdef JCM_DEBUG
				                         
				                         //memset(log,'\0',200);

				                         //sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE0[%d]= 0x%xh failed.",i,DLE0[i]);
				                         //writeFileLog(log); 

				                         #endif

                                                         complete[i]=false;
				                      
						   }


				    }//for dle0 end
                                    
                                    if( (true == complete[0]) &&  (true == complete[1]) )
				    {
				           #ifdef JCM_DEBUG
				           writeFileLog("[JCM_TransmitCmdSequence()] Read DLE0 two bytes complete.");
				           #endif
                                           break;
				       
				    }
                                    else //send enq byte again
                                    {

		                               rtcode=-1;
					       rtcode=SendSingleByteToSerialPort( HANDLE , JCM_ENQ );
					       if( 1 == rtcode ) //send enq success
					       { 
						    #ifdef JCM_DEBUG
						    writeFileLog("[JCM_TransmitCmdSequence() DLE0 Block] Send ENQ Success.");
						    #endif

					       }
					       else //send enq failed now abort
					       {
						    //#ifdef JCM_DEBUG
						    writeFileLog("[JCM_TransmitCmdSequence() DLE0 Block] Send ENQ Failed."); 
						    //#endif
						    
					       }


                                    }

                                    
                                     
                                    counter++;


                    }//while dle0 end
				   
                    //Check DLE0 bytes
                    if( (JCM_DLE0_FIRST_BYTE == DLE0[0]) &&  (JCM_DLE0_SECOND_BYTE == DLE0[1]) )
                    {
                           #ifdef JCM_DEBUG
                           writeFileLog("[JCM_TransmitCmdSequence()] Read DLE0  success.");
                           #endif
                       
                    }
                    else if( 0x04 == DLE0[0] )
                    {
                           //#ifdef JCM_DEBUG
                           writeFileLog("[JCM_TransmitCmdSequence()] Read EOT Bytes any comand transmission now aborted.");
                           return 3;
                           //#endif
                       
                    }

                    else //if not DLE0 bytes not recv now abort
                    {
                          //#ifdef JCM_DEBUG
                          writeFileLog("[JCM_TransmitCmdSequence()] Read DLE0  failed.");
                          //#endif
                          return 4;
                    }
                    
                    #ifdef JCM_DEBUG
		    writeFileLog("[JCM_TransmitCmdSequence()] send command now.");
                    #endif
                   
		   //Step 4:Now Send Command to jcm
                   for(i=0;i<JCM_NumberofBytestobeWrite;i++)
	           {
					 
		                          rtcode=-1;

		                          rtcode=SendSingleByteToSerialPort(HANDLE, JCM_Cmd[i]);
		                          
				          if( 1 == rtcode ) //write com port success
					  {
                                               continue;

					  }
					  else //write com port failed
					  {
                                               writeFileLog("[JCM_TransmitCmdSequence()] write command byte failed!!.");
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
						  
                                                  rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&DLE1[i],&totalByteRead);

                                                  if( (1== rtcode) && (DLE1[i] > 0x00) ) //read com port success
						  {

				                      #ifdef JCM_DEBUG 

				                      memset(log,'\0',200);

				                      sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh success.",i,DLE1[i]);
				                      writeFileLog(log); 
				                      
				                      #endif
                                                       
                                                      complete[i]=true;

						  }
						  else //send com port failed
						  {
						       
                                                       //#ifdef JCM_DEBUG 
				                       
				                       //memset(log,'\0',200);

				                       //sprintf(log,"[JCM_TransmitCmdSequence()] Read DLE1[%d]= 0x%xh failed.",i,DLE1[i]);
				                       //writeFileLog(log); 

				                       //#endif

						  }

				    }//for() dle1

                                    if( (true == complete[0]) &&  (true == complete[1]) )
				    {
				           #ifdef JCM_DEBUG
				           writeFileLog("[JCM_TransmitCmdSequence()] Read DLE1 two bytes complete.");
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
                          writeFileLog("[JCM_TransmitCmdSequence()] Read DLE1 success.");
                          #endif

                    }
                    else //if not DLE0 bytes recv failed abort
                    {
                          //#ifdef JCM_DEBUG
                          writeFileLog("[JCM_TransmitCmdSequence()] Read DLE1 failed.");
                          //#endif
                          return 5;
                    }

		    #ifdef JCM_DEBUG
		    writeFileLog("[JCM_TransmitCmdSequence()] Sending eot command .");
                    #endif

		    //Step 6:Now send eot to terminate command sending sequence
                    //JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY);
                    complete[0]=false,complete[1]=false;
		    while(counter<=8)
                    {
		            rtcode=-1;
			    rtcode=SendSingleByteToSerialPort(HANDLE,JCM_EOT);
		            if( 1 == rtcode ) //write com port success
			    {
		                #ifdef JCM_DEBUG 
		                writeFileLog("[JCM_TransmitCmdSequence()] Send EOT success.");
		                #endif
                                complete[0]=true;
                                
			    }
			    else //write com port failed
			    {
				//#ifdef JCM_DEBUG      
		                writeFileLog("[JCM_TransmitCmdSequence()] Send EOT failed.");
		                //#endif
		                return 6;
			    }
                            if( true == complete[0]  )
		            {
				           #ifdef JCM_DEBUG
				           writeFileLog("[JCM_TransmitCmdSequence()] Send EOT success.");
				           #endif 
                                           break;
				       
		            }

                            JCM_delay_miliseconds(10); //10ms delay

                            counter++;
                    
                    }//while() eot byte

                    #ifdef JCM_DEBUG
                    writeFileLog("[JCM_TransmitCmdSequence()] Transmission End.");
		    writeFileLog("[JCM_TransmitCmdSequence()] ________________________________________________________________");
                    #endif
                    #ifdef JCM_TRANS_TIME_DEBUG
                    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endts);
                    diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                    diffts.tv_nsec = endts.tv_nsec - begints.tv_nsec;
                    memset(log,'\0',100);
		    sprintf(log,"[JCM_TransmitCmdSequence()] Cycle complete in Seconds Elapsed=%d microsecond=%ld .",diffts.tv_sec,(diffts.tv_nsec/1000UL ) ); 
                    writeFileLog(log);
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
            writeFileLog("[JCM_ReceieveCmdReplySequence()] ________________________________________________________________");
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Recv command Start.");
	    #endif

        
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            //Step 1: Now wait for ENQ reply from JCM
            int retry=0;
            while(retry <=1) //1 default
            {
		    retry++;
                    #ifdef JCM_DEBUG
                    writeFileLog("[JCM_ReceieveCmdReplySequence()] Now go for ENQ read from JCM ");
                    #endif
		    rtcode=-1;
		    rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&ENQ,&totalByteRead);

                    

		    //if( 1 == rtcode ) //ENQ byte recv success
                    if( JCM_ENQ == ENQ)
		    {
		         
		         #ifdef JCM_DEBUG
		         
                         memset(log,'\0',200);
                         sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ success ENQ=0x%xh .",ENQ);
                         writeFileLog(log); 

		         #endif
                         break;      
			

		    }
                    
		    else if( (JCM_ENQ != ENQ ) || (retry >=1) )//ENQ byte recv failed
                    {
                           #ifdef JCM_DEBUG
		           
                           memset(log,'\0',200);
                           sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ failed ENQ=0x%xh .",ENQ);
                           writeFileLog(log); 

		           #endif
                           return 3;
		         
		    }
             

            }

      
            //Step 2: Send DLE0 Bytes to JCM 
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Now Sending DLE0 bytes to JCM");
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
                               writeFileLog(log); 

                               #endif

		          }
		          else //send DLE0 byte failed
		          {
		               //#ifdef JCM_DEBUG 
                              
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveCmdReplySequence()] Write DLE0[%d] =0x%xh failed. ",i,DLE0[i]);
                               writeFileLog(log); 

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
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Now Sending DLE1 bytes to JCM");
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
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Now go for EOT read from JCM ");
            #endif
            rtcode=-1;
            totalByteRead=0;
            EOT=0x00;
            for(retry=1;retry <=1;retry ++)
            {           
			    EOT=0x00;
                            rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
			    if( 1 == rtcode ) //read com port success
			    {
				          if( JCM_EOT == EOT ) //read eot status
					  {
				             #ifdef JCM_DEBUG

				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh  success retry=%d. ",EOT,retry);
                                             writeFileLog(log); 
 
				             #endif
                                             break;
					  }
					  else //read eot status failed
					  {
					     
                                             #ifdef JCM_DEBUG  
				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh  failed. retry=%d",EOT,retry);
                                             writeFileLog(log); 
 
				             #endif
				             continue; 
 
					  }

			    }
            }
         
            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Recv command End.");
	    writeFileLog("[JCM_ReceieveCmdReplySequence()] ________________________________________________________________");
            #endif
          
            return 1;
  
            

}



*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
            int counter=1;

            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveCmdReplySequence()] ________________________________________________________________");
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Recv command Start.");
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
				    writeFileLog(log);
				    #endif
				    rtcode=-1;
                                    ENQ=0x00;
                                    totalByteRead=0;
				    rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&ENQ,&totalByteRead);

				    //ENQ byte recv success
				    if( JCM_ENQ == ENQ)
				    {
					 
					 #ifdef JCM_RECV_TIME_DEBUG 
					 
				         memset(log,'\0',200);
				         sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ success ENQ=0x%xh .",ENQ);
				         writeFileLog(log); 

					 clock_gettime(CLOCK_MONOTONIC, &enqendts);
				         enqdiffts.tv_nsec = enqendts.tv_nsec - enqbegints.tv_nsec;
		                         memset(log,'\0',100);
				         sprintf(log,"[JCM_ReceieveCmdReplySequence()] Enq Recv in millisecond=%ld .",(enqdiffts.tv_nsec/1000000) ); 
				         writeFileLog(log);
                                         #endif
				         break;      
			

				    }
		                    else if( (JCM_ENQ != ENQ ) && ( 8 == counter ) ) //ENQ byte recv failed
				    {
				           #ifdef JCM_DEBUG
					   
				           memset(log,'\0',200);
				           sprintf(log,"[JCM_ReceieveCmdReplySequence()] read ENQ failed ENQ=0x%xh .",ENQ);
				           writeFileLog(log); 

					   #endif
				           return 3;
					 
				    }

                                    JCM_delay_miliseconds(10); //10ms delay

                                    counter++;
                                
             

            }//enq while end

      
            //Step 2: Send DLE0 Bytes to JCM 
            #ifdef JCM_RECV_TIME_DEBUG 
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Now Sending DLE0 bytes to JCM");
            #endif

	    for(i=0;i<2;i++)
            {

                          rtcode=-1;
                          rtcode=SendSingleByteToSerialPort(HANDLE, DLE0[i]);
                          if( 1 == rtcode ) //send DLE0 byte success
		          {
                               #ifdef JCM_RECV_TIME_DEBUG 
                               
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveCmdReplySequence()] Write DLE0[%d] =0x%xh success. ",i,DLE0[i]);
                               writeFileLog(log); 

                               #endif

		          }
		          else //send DLE0 byte failed
		          {
		               //#ifdef JCM_DEBUG 
                              
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveCmdReplySequence()] Write DLE0[%d] =0x%xh failed. ",i,DLE0[i]);
                               writeFileLog(log); 

                               //#endif
                               return 4;
		          }



            }//DLE0 for loop
            

            //Step 3: Now Wait for response against any command
            counter=1;
            rtcode=JCM_ReadResponse(HANDLE,Response,NumberofBytestobeRead,&Orgtotalbytesread);
            if( (NumberofBytestobeRead == Orgtotalbytesread) && (1==rtcode) )
            {
          
            }
            else if( (NumberofBytestobeRead != Orgtotalbytesread) && (1 !=rtcode) )
            {
		                  
                        memset(log,'\0',100);
		           
		        sprintf(log,"[JCM_ReceieveCmdReplySequence()] response reply bytes recv failed due to NumberofBytestobeRead and Orgtotalbytesread not matched."); 

		        writeFileLog(log);

		        return 5; //command response recv failed

            }
         
            //Step 4: Now send DLE1 for ack that we receievce response packet properly
            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Now Sending DLE1 bytes to JCM");
            #endif

	    for(i=0;i<2;i++)
            {

                          rtcode=-1;
                          rtcode=SendSingleByteToSerialPort(HANDLE, DLE1[i]);
                          if( 1 == rtcode ) //send DLE1 byte success
		          {
                               continue;
                              
		          }
		          else //send DLE1 byte failed
		          {
                               
                               memset(log,'\0',100);
		           
		               sprintf(log,"[JCM_ReceieveCmdReplySequence()] DLE1[%d] = 0x%xh. write failed.",i,DLE1[i]); 

		               writeFileLog(log);

		               return 6;

		          }


            }//for dle1 write
           
            
            //Step 5: Now Read EOT status from JCM

            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Now go for EOT read from JCM ");
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
                            rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
                            
                            //read com port success
			    if( ( 1 == rtcode )  && (JCM_EOT == EOT) )
			    {

                                           #ifdef JCM_RECV_TIME_DEBUG 
                                           memset(log,'\0',200);
                                           sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh .",EOT);
                                           writeFileLog(log);
                                           
                                           clock_gettime(CLOCK_MONOTONIC, &eotendts);
				           eotdiffts.tv_nsec = eotendts.tv_nsec - eotbegints.tv_nsec;
		                           memset(log,'\0',100);
				           sprintf(log,"[JCM_ReceieveCmdReplySequence()] eot bytes Recv in millisecond=%ld .",(eotdiffts.tv_nsec/1000000) ); 
                                           writeFileLog(log);
                                           #endif
                                           break;
                           }
                           else if( ( 100 == counter )  && (JCM_EOT != EOT) ) 
                           {
					     
                                             //#ifdef JCM_DEBUG  
				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Read EOT status =0x%xh  failed.",EOT);
                                             writeFileLog(log); 
 
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
            writeFileLog(log);
            #endif

            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveCmdReplySequence()] Recv command End.");
	    writeFileLog("[JCM_ReceieveCmdReplySequence()] ________________________________________________________________");
            #endif
          
            return 1;
  
            

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



static int JCM_ReceieveReplyWithoutReadENQ( const int HANDLE,
                                            unsigned char*  Response,
                                            const unsigned int NumberofBytestobeRead
                                          )
{
       
            unsigned char log[200];
            int i=0,totalByteRead=0;
            unsigned char ENQ=0x00;
            unsigned char EOT=0x00;
            int  Orgtotalbytesread=0;
            unsigned char DLE0[2]={ JCM_DLE0_FIRST_BYTE , JCM_DLE0_SECOND_BYTE };
            unsigned char DLE1[2]={ JCM_DLE1_FIRST_BYTE , JCM_DLE1_SECOND_BYTE };
            int rtcode=-1;
            
            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveReplyWithoutReadENQ()] ________________________________________________________________");
            writeFileLog("[JCM_ReceieveReplyWithoutReadENQ()] Recv command Start.");
	    #endif

        
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 
      
            //Step 1: Send DLE0 Bytes to JCM 
            JCM_delay_miliseconds(JCM_HANDSHAKE_DELAY); 

            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveReplyWithoutReadENQ()] Now Sending DLE0 bytes to JCM");
            #endif

	    for(i=0;i<2;i++)
            {
                          rtcode=-1;
                          rtcode=SendSingleByteToSerialPort(HANDLE, DLE0[i]);
                          if( 1 == rtcode ) //send DLE0 byte success
		          {
                               #ifdef JCM_DEBUG
                               memset(log,'\0',200);
                               sprintf(log,"[JCM_ReceieveReplyWithoutReadENQ()] Write DLE0[%d] =0x%xh success. ",i,DLE0[i]);
                               writeFileLog(log); 
 
                               #endif

		          }
		          else //send DLE0 byte failed
		          {
		               //#ifdef JCM_DEBUG 
                               
                               memset(log,'\0',200);
                               sprintf(log,"\n[JCM_ReceieveReplyWithoutReadENQ()] Write DLE0[%d] =0x%xh failed. ",i,DLE0[i]);
                               writeFileLog(log); 
 
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
            writeFileLog("[JCM_ReceieveReplyWithoutReadENQ()] Now Sending DLE1 bytes to JCM");
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
	    JCM_delay_miliseconds(50); 

            //Step 4: Now Read EOT status from JCM

            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveReplyWithoutReadENQ()] Now go for EOT read from JCM ");
            #endif
            rtcode=-1;
            totalByteRead=0;
            EOT=0x00;
            for(retry=1;retry <=2;retry ++)
            {           
			    EOT=0x00;
                            rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
			    if( 1 == rtcode ) //read com port success
			    {
				          if( JCM_EOT == EOT ) //read eot status
					  {
				             #ifdef JCM_DEBUG
				             
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveReplyWithoutReadENQ()] Read EOT status =0x%xh  success retry=%d.",EOT,retry);
                                             writeFileLog(log);

				             #endif
                                             break;

					  }
					  else //read eot status failed
					  {
					     
                                             #ifdef JCM_DEBUG  
				            
                                             memset(log,'\0',200);
                                             sprintf(log,"[JCM_ReceieveReplyWithoutReadENQ()] Read EOT status =0x%xh  failed. retry=%d",EOT,retry);
                                             writeFileLog(log);

				             #endif

				             continue;  

					  }

			    }
            }
         
            #ifdef JCM_DEBUG
            writeFileLog("[JCM_ReceieveReplyWithoutReadENQ()] Recv command End.");
	    writeFileLog("[JCM_ReceieveReplyWithoutReadENQ()] ________________________________________________________________");
            #endif
          
            return 1;
  
            

}




//Complete cycle if byte recv not properly
static int JCM_Complete_ReceieveCmdReplySequence(int HANDLE)
{
            
            

            int rtcode=-1,i=0;
            int totalByteRead=0;
            unsigned char EOT=0x00;
            unsigned char DLE0[2]={ JCM_DLE0_FIRST_BYTE , JCM_DLE0_SECOND_BYTE };
            unsigned char DLE1[2]={ JCM_DLE1_FIRST_BYTE , JCM_DLE1_SECOND_BYTE };

            //Step 1: Now send DLE1 for ack that we receievce response packet properly

            #ifdef JCM_DEBUG
            printf("\n[JCM_Complete_ReceieveCmdReplySequence()] Now Sending DLE1 bytes to JCM");
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

            //Step 6: Now Read EOT status from JCM
            #ifdef JCM_DEBUG
            writeFileLog("[JCM_Complete_ReceieveCmdReplySequence()] Now go for EOT read from JCM ");
            #endif
            rtcode=-1;
            totalByteRead=0;
            EOT=0x00;
            rtcode=ReceiveSingleByteFromSerialPort(HANDLE,&EOT,&totalByteRead);
            if( 1 == rtcode ) //read com port success
            {
                          if( JCM_EOT == EOT ) //read eot status
		          {
                             #ifdef JCM_DEBUG
                             printf("\n[JCM_Complete_ReceieveCmdReplySequence()] Read EOT status =0x%xh  success. ",EOT);
                             #endif
		          }
		          else //read eot status failed
		          {
		             //#ifdef JCM_DEBUG  
                             printf("\n[JCM_Complete_ReceieveCmdReplySequence()] Read EOT status =0x%xh  failed. ",EOT);
                             //#endif
                             return 7;  
		          }
            }
            else //read port failed
            {
		return 7;                

            }

            #ifdef JCM_DEBUG
            printf("\n[JCM_Complete_ReceieveCmdReplySequence()] Recv command End.");
	    printf("\n[JCM_Complete_ReceieveCmdReplySequence()] ________________________________________________________________");
            #endif
          
            return 1;
 

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Milliseconds delay [value must be 0-900]
static void JCM_delay_miliseconds(const long long int milisec) 
{
        
        
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
                //writeFileLog(log);
                return;
	      

	}
        else
        {
	      
                memset(log,'\0',100);
                sprintf(log,"[JCM_delay_miliseconds()] nanosleep() system call failed with return code  %d .",rtcode); 
                writeFileLog(log);
                return;
	      

	}
        

        

         
}//JCM_delay_miliseconds() end



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//read jcm command response packet
static int JCM_ReadResponse( int HANDLE,
                             unsigned char* recvbyte,
                             const int  totalBytestoberead,
                             int* const Orgtotalbytesread
                            )
{
	   
            

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
                             writeFileLog(log);
                             #endif
                        }
                        
                        if(totalBytestoberead == bytecounter)
                        {
                             clock_gettime(CLOCK_MONOTONIC, &end);
                             diff.tv_sec = end.tv_sec - start.tv_sec;
                             #ifdef JCM_DEBUG
                             memset(log,'\0',200);
                             sprintf(log,"[JCM_ReceieveCmdReplySequence()] Expected byte read complete %d Seconds",diff.tv_sec);
                             writeFileLog(log);
                             for( Counter=0; Counter< bytecounter ; Counter++)
			     {
		                   memset(log,'\0',100);
                                   sprintf(log,"[JCM_ReceieveCmdReplySequence()] Response[%d]= 0x%xh",Counter,recvbyte[Counter]);
                                   writeFileLog(log);
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
                                         writeFileLog(log);
                                         #endif
		                         return 1;
		                    }
		                    else
		                    {
		                         #ifdef JCM_DEBUG
                                         
                                         memset(log,'\0',200);
                                         sprintf(log,"[JCM_ReceieveCmdReplySequence()] Expected byte read failed time elapsed %d Seconds",diff);
                                         writeFileLog(log);

                                         #endif

		                         return 2;
		                    }
		                    
                        
                       }



           }// End for(;;)

     


}//end




/*
 
 1= success
 or return code by transmit or recv reply
*/

int JCM_GetReply( unsigned int CmdLength, unsigned int Resposnesize, unsigned char *Response, unsigned char *Commands, int Delay)
{

            
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
	        printf("\n [JCM_GetReply()] Commands[%d] = 0x%xh",Counter,Commands[Counter]);
	    }

	    printf("\n[JCM_GetReply()] Comamnd Packet BCC =0x%xh",Commands[CmdLength-1]);
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
                         writeFileLog("[JCM_GetReply()] Receieve sequence failed.");
                         return 3; //recv sequence failed
                          
                    }
 
            }
            else
            {
                     writeFileLog("[JCM_GetReply()] Transmission sequence failed.");
                     return 2; //transmit sequence failed
            } 


}

//Return BCC of any packet
unsigned char GetBCC(unsigned char* Packet,int StartIndex,unsigned int Packetlength)
{
        
           
            int Counter=0;

            unsigned char BCC=0x00;
            
            for( Counter=StartIndex; Counter<=(Packetlength-2) ; Counter++)
	    {
                  //printf("\n [CheckReplyPacketBCC()] Packet[%d] = 0x%x h",Counter,Packet[Counter]);
 	          BCC =   BCC ^ Packet[Counter];
            }
            
            return BCC;

        
}




int JCM_Receieve_RSP_Packet( unsigned char*  Response,
                             const unsigned int NumberofBytestobeRead
                           )
{
  

            int rtcode=-1;
            rtcode=JCM_ReceieveCmdReplySequence( g_HANDLE,Response,NumberofBytestobeRead);
            return rtcode;



}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  JCM Command Section /////////////////////////////////////////////////////////////

/*
		JCM_RESET_CMD                   
		JCM_SENSE_CMD                   
		JCM_INSERTION_AUTHORISED_CMD     
		JCM_INSERTION_INHIBIT_CMD       
		JCM_RECEIPT_CMD                 
		JCM_RETURN_CMD		       
		JCM_INTAKE_CMD       
*/  

         


int JCM_Cmd_Reset(int HANDLE)
{ 
                

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


int JCM_Cmd_Sense(unsigned char *Response,const int ResponseLength,const unsigned int Delay)
{ 
                
                int rtcode=-1;

                unsigned char Commands[ JCM_CMD_CMN_LENGTH ]=JCM_SENSE_CMD;
                
 
                rtcode=JCM_GetReply(JCM_CMD_CMN_LENGTH ,ResponseLength,Response,Commands,Delay);
                
                return rtcode;
  
}


int JCM_Cmd_InsertionAuthorized( unsigned char DefaultEnablebyte ,unsigned char *Response,const int ResponseLength,const unsigned int Delay)

{ 

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

                    writeFileLog(log);
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
                                         writeFileLog(log);
				    }

                                    memset(log,'\0',100);

                                    sprintf(log,"[JCM_Cmd_InsertionAuthorized()] DittoComamnd Packet BCC =0x%xh.",DittoCommands[CmdLength-1]);
                                    writeFileLog(log);
				
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
                                          writeFileLog(log);

				    }

				
                                    memset(log,'\0',100);

                                    sprintf(log,"[JCM_Cmd_InsertionAuthorized()] Comamnd Packet BCC =0x%xh.",Commands[CmdLength-1]);

                                    writeFileLog(log);
				    #endif
		    
		    }

                    if(0x10== DefaultEnablebyte)
                    {
                           #ifdef JCM_DEBUG
			   writeFileLog("[JCM_Cmd_InsertionAuthorized()] Enablebyte 0x10 found.");
                           #endif
                           rtcode=JCM_TransmitCmdSequence(g_HANDLE,DittoCommands,CmdLength);

	            }
	            else if(0x10 != DefaultEnablebyte)
	            {
                            #ifdef JCM_DEBUG
			    writeFileLog("[JCM_Cmd_InsertionAuthorized()] Enablebyte 0x10 not found.");
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


int JCM_Cmd_InsertionInhibited( unsigned char *Response,const int ResponseLength,const unsigned int Delay )
{ 
            
                
                int rtcode=-1;

                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_INSERTION_INHIBIT_CMD;

                rtcode=JCM_GetReply(JCM_CMD_CMN_LENGTH,ResponseLength,Response,Commands,Delay);

                return rtcode;


}


int JCM_Cmd_Receipt(unsigned char *Response,const int ResponseLength,const unsigned int Delay)
{ 
               
               /*
 
                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_RECEIPT_CMD;

                JCM_GetReply(JCM_CMD_CMN_LENGTH,30,Response,Commands,JCM_CMD_DELAY);
              
              
               */

}




int JCM_Cmd_Return(unsigned char *Response,const int ResponseLength,const unsigned int Delay)
{ 
                
                int rtcode=-1;

                int CmdLength=JCM_CMD_CMN_LENGTH;

                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_RETURN_CMD;
              
                rtcode = JCM_GetReply(JCM_CMD_CMN_LENGTH,ResponseLength,Response,Commands,Delay);
               
                return rtcode;


}


int JCM_Cmd_Intake(unsigned char *Response,const int ResponseLength,const unsigned int Delay)
{ 
                
               /*
                
                unsigned char Response[JCM_RESPONSE_BUFFER_SIZE];

                memset(Response,'\0',JCM_RESPONSE_BUFFER_SIZE);

                unsigned char Commands[JCM_CMD_CMN_LENGTH]=JCM_INTAKE_CMD;

                JCM_GetReply(JCM_CMD_CMN_LENGTH,30,Response,Commands,JCM_CMD_DELAY);
               
               */


}


#endif


