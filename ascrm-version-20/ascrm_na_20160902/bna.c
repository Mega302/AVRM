
#include "bna.h"



#ifdef BNA_NOTE_ACCEPTOR

/* 

gcc bna.c -lpthread -lBnrCtlL32

*/

static struct timespec eventTimeOut;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//unsigned long = T_BnrXfsResult


T_XfsCashOrder  g_CashOrder;

T_DenominationList g_denominationList={0};

static BOOL g_OperationCompleteflag=false;

static T_BnrXfsResult g_operationCompleteResult=0;

static T_BnrXfsResult g_operationCompleteextendedResult=0;

static T_BnrXfsResult g_statusOccuredStatus=0;

static T_BnrXfsResult g_statusOccuredResult=0;

static T_BnrXfsResult g_statusOccuredextendedResult=0;

static T_BnrXfsResult g_intermediateOccuredResult=0;

static  pthread_mutex_t operationCompleteOccured_Event_mutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t statusOccured_Event_mutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t intermediateOccured_Event_mutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t Reject_Status_Occured_EventEvent_mutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t NoteLowLevelPollFlagStatusmutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t DisableAcceptancemutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t stopThreadmutex=PTHREAD_MUTEX_INITIALIZER;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Atvm Variables

static int  g_faretobeaccept=0;

static int g_BNA_Accepted_Moneyvalue=0;

static pthread_mutex_t Reject_Event_mutex= PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  BNA_Escrow_State_Mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  BNA_Escrow_Enable_Mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  NASetSpecificFaremutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  NASetDisableAcceptance = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t  NASetCurrentFare = PTHREAD_MUTEX_INITIALIZER;

static unsigned int g_NACurrentAtvmFare;

static unsigned int g_SpecificNoteInhibitFlag;

static volatile bool g_EscrowEnableFlag;

static volatile bool g_EscrowEvent;

static volatile bool g_RejectEvent;

static volatile bool g_Reject_Status_Occured_Event;

static volatile bool g_stopThread;

static bool g_DisableAcceptance;

static unsigned int g_NoteLowLevelPollFlagStatus=-1;

static bool g_DenomTableUpdateFlag=false;


//#define BNA_ACCEPTING                          0x01

//#define BNA_GENERIC_FAIL                       0x02
         
//#define BNA_JAMMING                            0x03

//#define BNA_DISABLED_WITHOUT_ESCROW            0x04

#define BNA_REJECTING                          0x05

#define BNA_ESCROW                             0x0F

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


pthread_mutex_t g_NAEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  g_NAEnableThreadCond = PTHREAD_COND_INITIALIZER;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int g_CoinDisableflag;


int OpenCashAcptr()
{

     return ( BNA_Activate() );


}



int Deactivate_BNA_NoteAcptr()
{

       
       return ( BNA_DeActivate() );


}



int GetNoteAcptrStatus()
{
      
       return( BNA_GetStatus() );

}



int WaitforNoteTakenBackByUser(int waittime)
{

       
	  
		   time_t start,end,diff;

		   int rtcode=-1;

		   unsigned char log[100];

		   memset(log,'\0',100); 
		 
		   //#ifdef BNA_DEBUG
		   writeFileLog("[WaitforNoteTakenBackByUser()]  Get Reject State Complete read status starting..");
		   //#endif
	 
	
	  
		   time(&start);  
		 
		   //Wait here untill reject state complete
		   for(;;)
		   {

			     time(&end); 

			     diff=end-start;

			     
                             //memset(log,'\0',100);

		             //sprintf(log,"[WaitforNoteTakenBackByUser()] Time = %d .",diff);

		             //writeFileLog(log);

                             

			     //if timer is running
			     if(diff<waittime)
			     {

                                        //writeFileLog("[WaitforNoteTakenBackByUser()] if(diff<waittime) block .");

                                        pthread_mutex_lock(&Reject_Status_Occured_EventEvent_mutex);

					if( false == g_Reject_Status_Occured_Event ) 
					{

					     //#ifdef BNA_DEBUG

                                             memset(log,'\0',100); 

                                             sprintf(log,"[WaitforNoteTakenBackByUser()] No Active Reject State Found .Total Time Taken= %d seconds.",diff);
					     writeFileLog(log);

                                             //#endif

                                             rtcode =1;

					}
                               
                                        pthread_mutex_unlock(&Reject_Status_Occured_EventEvent_mutex);

                                        if(1 == rtcode)
                                          return rtcode;

				
			     }

			     //if timer time is end or timer is running
			     if(diff>=waittime)
			     {

                                        //writeFileLog("[WaitforNoteTakenBackByUser()] if(diff>=waittime) block .");

                                        pthread_mutex_lock(&Reject_Status_Occured_EventEvent_mutex);

					if( false == g_Reject_Status_Occured_Event)
					{

						  //#ifdef BNA_DEBUG

		                                  memset(log,'\0',100);

		                                  sprintf(log,"[WaitforNoteTakenBackByUser()] Found Reject State Complete in = %d seconds.",diff);
		                                  writeFileLog(log);

		                                  //#endif	
					  
		                                  rtcode=1;

					}

					else if( true == g_Reject_Status_Occured_Event)
					{ 
						   
                                                   //#ifdef BNA_DEBUG

		                                   memset(log,'\0',100);

		                                   sprintf(log,"[WaitforNoteTakenBackByUser()] Reject State is not completed during time delay wait = %d .",diff);
		                                   writeFileLog(log);

		                                   //#endif

						   rtcode=0;


					}

                                        pthread_mutex_unlock(&Reject_Status_Occured_EventEvent_mutex);

					return rtcode;


			     }//timer if block end here
			      
                             

		}//for loop end
	   
	
         

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int UnloadCashAcptr()
{


            T_BnrXfsResult result=-1;

            int rtcode=-1;

            unsigned char log[100];

            memset(log,'\0',100); 

	    ClearGlobalEventState();

            writeFileLog("[UnloadCashAcptr()]  Going to issue bnr_CashInEnd() api.");

	    result = bnr_CashInEnd();

            memset(log,'\0',100); 

            sprintf(log,"[UnloadCashAcptr()] bnr_CashInEnd() return code = %ld .",result);

            writeFileLog(log);

            if(result > 0)
            {

                    writeFileLog("[UnloadCashAcptr()]  bnr_CashInEnd() api successfully executed.");

                    writeFileLog("[UnloadCashAcptr()]  Now Wait for bnr_CashInEnd() api operation complete...");

                    //Now wait for 2min
                    rtcode=WaitforMeiOperationComplete( 120 );

                    if(1 ==  rtcode)
                    {
                         writeFileLog("[UnloadCashAcptr()]  bnr_CashInEnd() successfully completed.");
                         return 1;
                    }
                    else
                    {
                         writeFileLog("[UnloadCashAcptr()]  bnr_CashInEnd() failed to complete its operation.");
                         return 0;
                    } 
 
            }
            else
            {

                   writeFileLog("[UnloadCashAcptr()]  bnr_CashInEnd() api failed to execute.");
                   return 0;

            }



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
int  DispenseCashAcptr()
{
    


            T_BnrXfsResult result=-1;

            bool DispenseFlag=false;

            int rtcode=-1;
            
            unsigned char log[100];

            memset(log,'\0',100); 

	    ClearGlobalEventState();

            //Issue escrow notes reject operation
	    result =bnr_CashInRollback();

            memset(log,'\0',100); 

            sprintf(log,"[DispenseCashAcptr()] bnr_CashInRollback() return code = %ld .",result);

            writeFileLog(log);

            if(result>0)
            {


                    DispenseFlag=true;

                    writeFileLog("[DispenseCashAcptr()] bnr_CashInRollback() api successfully executed.");

                    writeFileLog("[DispenseCashAcptr()] Now Wait for bnr_CashInRollback() api operation complete...");

                    //Now wait for 1 hr bnr_CashInRollback() operatation[default 1hr=3600 sec]

                    rtcode=WaitforMeiOperationComplete( 3600 );

                    if(1 ==  rtcode)
                    {

                         writeFileLog("[DispenseCashAcptr()]  bnr_CashInRollback() successfully completed.");

                    }
                    else
                    {

                              writeFileLog("[DispenseCashAcptr()]  Cash is still in bna front bezel.");

                              writeFileLog("[DispenseCashAcptr()]  Now Going to eject it from front bezel.");

                              rtcode=-1;

                              //Cancel waiting for cash taken by user operation
                              rtcode=bnr_CancelWaitingCashTaken();

                              if(BXR_NO_ERROR == rtcode)
                              {

                                    writeFileLog("[DispenseCashAcptr()] bnr_CancelWaitingCashTaken() called successfully.");
                              }
		              else
		              {
                                    writeFileLog("[BNA_Credit_Poll() Disable] bnr_CancelWaitingCashTaken() called failed.");

                              }

                              
                    }

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
                  
                    bna_delay_mSec(200);

                    //Stop bnr cashin operation
                    result=-1;

	            result=bnr_CashInEnd();

	            memset(log,'\0',100); 

	            sprintf(log,"[DispenseCashAcptr()] bnr_CashInEnd() return code = %ld .",result);

	            writeFileLog(log);

	            if(result>0)
	            {

				    writeFileLog("[DispenseCashAcptr()]  bnr_CashInEnd() api successfully executed.");

				    writeFileLog("[DispenseCashAcptr()]  Now Wait for bnr_CashInEnd() api operation complete...");

				    //Now wait for 3min
				    rtcode=WaitforMeiOperationComplete( 180 );

				    if(1 ==  rtcode)
				    {
				         writeFileLog("[DispenseCashAcptr()]  bnr_CashInEnd() successfully completed.");
				         
				    }
				    else
				    {
				         writeFileLog("[DispenseCashAcptr()]  bnr_CashInEnd() failed to complete its operation.");
				         
				    } 
		 
	            }
	            else
		    {

		                   writeFileLog("[DispenseCashAcptr()]  bnr_CashInEnd() api failed to execute.");
		            

		    }
                   
                    bna_delay_mSec(200);

                    if(true == DispenseFlag)
                    {
                         return 1;
                    }
                    else
                    {
                         return 0;

                    }

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////// 

 
            }
	    else
            {

                   writeFileLog("[DispenseCashAcptr()] bnr_CashInRollback() api failed to execute.");

                   writeFileLog("[DispenseCashAcptr()] Dispense operation failed.");

                   return 0;

            }


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int GetNoteDetailsInRecycleCst(int *qty)
{
 
           *qty=0;

           *qty=BNA_ListCashUnit();
       
           return 1;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int WaitforNoteAcptrLowlevelflagClosed(double waittime)
{

       
	  
	 time_t start=0,end=0,diff=0;
	 int rtcode=-1;
         unsigned char log[100];
         memset(log,'\0',100); 
	 
         //#ifdef BNA_DEBUG
         writeFileLog("[WaitforNoteAcptrLowlevelflagClosed()]  Get Low level flag status starting..");
         //#endif
	 
         pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

	 //if polling flag is on condition (on=1)
	 if(1==g_NoteLowLevelPollFlagStatus)
	 {
	           
                   pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);
		   
                   time(&start);  
		 
		   //Wait here untill polling flag is off (off=0)
		   for(;;)
		   {

			     time(&end); 
			     diff=end-start;
			     
			     //if timer is running
			     if(diff<waittime)
			     {
                                      
                                        pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

					if(0==g_NoteLowLevelPollFlagStatus) 
					{

		                                     pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);

						     //#ifdef BNA_DEBUG
		                                     memset(log,'\0',100); 
		                                     sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Already Low level flag status is closed in %d seconds.",diff);
						     writeFileLog(log);
		                                     //#endif

		                                     return 1;

					}

					pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);

			     }

			     //if timer time is end or timer is running
			     if(diff>=waittime)
			     {

                                        pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

					if( 0==g_NoteLowLevelPollFlagStatus)
					{

		                                  pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);
						  //#ifdef BNA_DEBUG
		                                  memset(log,'\0',100);
		                                  sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Low level flag status is closed %d seconds.",diff);
		                                  writeFileLog(log);
		                                  //#endif	
					  
		                                  rtcode=1;


					}
					else
					{ 

		                                   pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);
						   //#ifdef BNA_DEBUG
		                                   memset(log,'\0',100);
		                                   sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Low level flag status is not closed during time delay wait. %d .",diff);
		                                   writeFileLog(log);
		                                   //#endif
						   rtcode=0;


					}

      

					return rtcode;


			     }//timer if block end here
			      

		   }//for loop end
	   
	 }//if thread status start end here

	 pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);

         pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

         if(0==g_NoteLowLevelPollFlagStatus)
	 {

		      pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);
		      //#ifdef BNA_DEBUG
		      writeFileLog("[WaitforNoteAcptrLowlevelflagClosed()]  Already Low level flag status is closed.");
		      //#endif
		      return 1;


	 }

         pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int StopRejectProcess()
{



      T_BnrXfsResult rtcode =-1;

      rtcode = bnr_CancelWaitingCashTaken();

      if(BXR_NO_ERROR ==rtcode)
      {

               writeFileLog("[StopRejectProcess()] bnr_CancelWaitingCashTaken() called successfully.");  
               
               bna_Wait(1); //1 second delay
                
               pthread_mutex_lock(&operationCompleteOccured_Event_mutex);

               unsigned char log[100];

               memset(log,'\0',100);

	       sprintf(log,"[StopRejectProcess()] g_operationCompleteResult =  %d .",g_operationCompleteResult);

	       writeFileLog(log);

               if( ( XFS_E_CANCELLED == g_operationCompleteResult ) && ( true == g_OperationCompleteflag ) )
               {

                       pthread_mutex_unlock(&operationCompleteOccured_Event_mutex); 

                       writeFileLog("[StopRejectProcess()] bnr_CancelWaitingCashTaken() operation completed successfully.");

                       return 1;

               }
               else
               {
                        
                        pthread_mutex_unlock(&operationCompleteOccured_Event_mutex); 

                        writeFileLog("[StopRejectProcess()] bnr_CancelWaitingCashTaken() operation completed failed.");

                        return 0;

               }
  

      }
      else if(BXR_NO_ERROR !=rtcode)
      {

          writeFileLog("[StopRejectProcess()] bnr_CancelWaitingCashTaken() called failed.");  
          return 0;
      }


}



int CompleteCashInProcess()
{


            T_BnrXfsResult rtcode=-1;

            int apirtcode=-1;

	    ClearGlobalEventState();

	    rtcode=bnr_CashInEnd();

            if(rtcode>0)
            {


                  writeFileLog("[CompleteCashInProcess()] bnr_CashInEnd() called successfully.");

                  //bna_delay_mSec(1000);

                  bna_Wait(2);

                  pthread_mutex_lock(&operationCompleteOccured_Event_mutex);

                  if( true == g_OperationCompleteflag )
                  {

                         if(XFS_RC_SUCCESSFUL == g_operationCompleteResult)  
                         {

                                writeFileLog("[CompleteCashInProcess()] bnr_CashInEnd() command executed successfully .");
                                apirtcode= 1;
                         }   
                         else
                         {

                                writeFileLog("[CompleteCashInProcess()] bnr_CashInEnd() command executed failed .");
                                apirtcode = 0;
                         }    

                  }

                  pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);                           
                  
                  return apirtcode;
            }
            else
            {

                  writeFileLog("[CompleteCashInProcess()] bnr_CashInEnd() called failed.");
                  return 0;
            }

	    

	    return 0;



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int enableSpecificBillsAndMakeIdle(  int fare,                  //API fare (in)
                                     int maxNoteQtyAllowed      //API Max no of cash allow number (in)
                                  )
{


	 

	 unsigned char log[200];
         memset(log,'\0',200);
         T_BnrXfsResult rtcode=-1;


	 //Disable all notes by default
	 NOTE notevar={0,0,0,0,0,0,0};
	 
         //Analysis if exact fare is given
        
         writeFileLog("[enableSpecificBillsAndMakeIdle()] Before Exact fare analysis block.");
         

         
         //Make a log about fare and max no of cash
         memset(log,'\0',200);
         sprintf(log,"[enableSpecificBillsAndMakeIdle()] Max No of Cash = %d And fare = %d",maxNoteQtyAllowed,fare);       
         writeFileLog(log);

         
         //Enable =1 Disable=0
	 if( 1 == maxNoteQtyAllowed )
	 {

                   g_DenomTableUpdateFlag=false;

		   #ifdef BNA_DEBUG
                   writeFileLog("[enableSpecificBillsAndMakeIdle()] Max no of cash is one given(Exact fare).");
                   #endif

                   switch(fare)
		   {
			     case 5:
			          notevar.INR_5    = 1;
                                  #ifdef BNA_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 5 only enable.");
                                  #endif
			          break;

			     case 10:
                                  
			          notevar.INR_10   = 1;
                                  #ifdef BNA_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 10 only enable.");
                                  #endif
			          break;

			     case 20:
			          notevar.INR_20   = 1;
                                  #ifdef BNA_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 20 only enable.");
                                  #endif
			          break;

			     case 50:
			          notevar.INR_50   = 1;
                                  #ifdef BNA_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 50 only enable.");
                                  #endif
			          break;

			     case 100:
			          notevar.INR_100  = 1;
                                  #ifdef BNA_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 100 only enable.");
                                  #endif 
			          break;

			     case 500:
			          notevar.INR_500  = 1;
                                  #ifdef BNA_DEBUG
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 only enable.");
                                  #endif
			          break;

			     case 1000:
			         notevar.INR_1000 = 1;
                                 #ifdef BNA_DEBUG
                                 writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 1000 only enable.");
                                 #endif
			         break;

                             default: writeFileLog("[enableSpecificBillsAndMakeIdle()] No fare given.");
                                      break;

	      } 

	   }
           else if( maxNoteQtyAllowed > 1 ) //Exact fare not given
           {

                  g_DenomTableUpdateFlag=true;

                  #ifdef BNA_DEBUG
                  writeFileLog("[enableSpecificBillsAndMakeIdle()] No Exact fare given.");
                  #endif

           
                  if(fare >= 5)
                  {
                        notevar.INR_5      = 1;
                        #ifdef BNA_DEBUG
                        writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 5 Enable.");
                        #endif
                  }
                  if(fare >= 10)
                  {
                        notevar.INR_10     = 1;
                        #ifdef BNA_DEBUG
                        writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 10 Enable.");
                        #endif
                  }

                  if(fare >= 20)
                  {
                       notevar.INR_20     = 1;
                       #ifdef BNA_DEBUG
                       writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 20 Enable.");
                       #endif
                  }
                  if(fare >= 50)
                  {
                      notevar.INR_50     = 1;
                      #ifdef BNA_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 50 Enable.");
                      #endif
                  }

                  if(fare >= 100)
                  {
                      notevar.INR_100    = 1;
                      #ifdef BNA_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 100 Enable.");
                      #endif
                  }

                  if(fare >= 500)
                  {
                      notevar.INR_500    = 1;
                      #ifdef BNA_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif
                  }

                  if(fare >= 1000)
                  {
                      notevar.INR_1000   = 1;
                      #ifdef BNA_DEBUG
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif
                  }

           }
           else if( 0 == maxNoteQtyAllowed  )
           {

                  g_DenomTableUpdateFlag=false;
                  writeFileLog("[enableSpecificBillsAndMakeIdle()] maxNoteQtyAllowed is set zero so no enable denom and cashinstart operation will not start.");
                  return 1;

           }

           #ifdef BNA_DEBUG
	   writeFileLog("[enableSpecificBillsAndMakeIdle()] After Exact fare analysis block.");
           #endif
        
           //Now Disable Notes
           #ifdef BNA_DEBUG
           writeFileLog("[enableSpecificBillsAndMakeIdle()] Now going to enable note type.");
           #endif

           //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           int EnableDenomRtcode=-1;

           //Enable Denom in BNR
           EnableDenomRtcode = EnableDenom(notevar);

           //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           bool clearflag=false,CasinStartFlag=false,CasinFlag=false;

           writeFileLog("[enableSpecificBillsAndMakeIdle()] Start Going to clear all global events flags before cashinstart().");

           ClearGlobalEventState();

           writeFileLog("[enableSpecificBillsAndMakeIdle()] End Going to clear all global events flags before cashinstart().");

           //Enable BNR Note Acceptor for cash acceptance
	   rtcode = bnr_CashInStart();

           memset(log,'\0',200);

           sprintf(log,"[enableSpecificBillsAndMakeIdle()]  bnr_CashInStart() return code = %d.",rtcode);

           writeFileLog(log);

           if(rtcode > 0)
           {

                 writeFileLog("[enableSpecificBillsAndMakeIdle()] bnr_CashInStart() call is successfully.");
           }
           else
           {
                 writeFileLog("[enableSpecificBillsAndMakeIdle()] bnr_CashInStart() call failed .");

           }

           /////////////////////////////////////////////////////////////////////////////////////////////////////////////


           WaitforMeiOperationComplete( 3 ) ;

           pthread_mutex_lock(&operationCompleteOccured_Event_mutex);

           if( (XFS_RC_SUCCESSFUL == g_operationCompleteResult) && ( true == g_OperationCompleteflag ) )
           {
                  writeFileLog("[enableSpecificBillsAndMakeIdle()] bnr_CashInStart() executed successfully.");
                  CasinStartFlag=true;
                 
           }
           else
           {
                  writeFileLog("[enableSpecificBillsAndMakeIdle()] bnr_CashInStart() executed failed.");
           }
 
           pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);


           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

           if( true == CasinStartFlag  )
           {

		   writeFileLog("[enableSpecificBillsAndMakeIdle()] Start Going to clear all global events flags before cashin().");

		   ClearGlobalEventState(); 

		   writeFileLog("[enableSpecificBillsAndMakeIdle()] End Going to clear all global events flags before cashin().");

		   rtcode=-1;

		   rtcode = bnr_CashIn(NULL,NULL);

		   memset(log,'\0',200);

		   sprintf(log,"[enableSpecificBillsAndMakeIdle()]  bnr_CashIn return code = %d.",rtcode);

		   writeFileLog(log);

		   if(rtcode > 0)
		   {
	                    writeFileLog("[enableSpecificBillsAndMakeIdle()] bnr_CashIn() call is successfully.");
                            CasinFlag=true;
		   }
		   else
		   {
		            writeFileLog("[enableSpecificBillsAndMakeIdle()] bnr_CashIn() call is failed.");

		   }

           }
           else
           {

                   writeFileLog("[enableSpecificBillsAndMakeIdle()] Unable to call bnr_CashIn() function due to bnr_CashInStart() call failed.");

           }
           /////////////////////////////////////////////////////////////////////////////////////////////////////////
           
           writeFileLog("[enableSpecificBillsAndMakeIdle()] Enable Complete.");

           if( ( 1 == EnableDenomRtcode)  && (true == CasinFlag) )
           {

                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Enable Denom Operation Successfully Executed.");

                  return 1;

           }
           else
           {
                 writeFileLog("[enableSpecificBillsAndMakeIdle()] Enable Denom Operation Failed to Execute.");

                 return (-1);


           }
	       

  
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



inline void InitNATransModel()
{

         
          ///////////////////////////////////////////////////////////////////

          g_DenomTableUpdateFlag =false;

          ///////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&stopThreadmutex);

          g_stopThread=false;

          pthread_mutex_unlock(&stopThreadmutex);
          
          ///////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &BNA_Escrow_Enable_Mutex );

          g_EscrowEnableFlag=false;

          pthread_mutex_unlock( &BNA_Escrow_Enable_Mutex );

          ////////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&DisableAcceptancemutex);

          g_DisableAcceptance=false;

          pthread_mutex_unlock(&DisableAcceptancemutex);

          //////////////////////////////////////////////////////////////////////

          pthread_mutex_lock (&Reject_Event_mutex);

          g_RejectEvent=false;

          pthread_mutex_unlock (&Reject_Event_mutex);

          //////////////////////////////////////////////////////////////////////

          pthread_mutex_lock( &BNA_Escrow_State_Mutex );

          g_EscrowEvent=false;

          pthread_mutex_unlock( &BNA_Escrow_State_Mutex );

          ///////////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&NASetSpecificFaremutex);

          g_SpecificNoteInhibitFlag=0;

          g_NACurrentAtvmFare=0;
 
          pthread_mutex_unlock(&NASetSpecificFaremutex);

          //////////////////////////////////////////////////////////////////////// 

          pthread_mutex_lock(&NASetCurrentFare);

          g_faretobeaccept=0;
 
          pthread_mutex_unlock(&NASetCurrentFare);
 
          /////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

          g_NoteLowLevelPollFlagStatus=0;

          pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);
          
          ////////////////////////////////////////////////////////////////////////

          pthread_mutex_lock ( &Reject_Status_Occured_EventEvent_mutex );

          g_Reject_Status_Occured_Event=false;

          pthread_mutex_unlock ( &Reject_Status_Occured_EventEvent_mutex );

          //////////////////////////////////////////////////////////////////////// 

          pthread_mutex_lock( &BNA_Escrow_State_Mutex );

          g_EscrowEvent=false;

          g_BNA_Accepted_Moneyvalue=0;

          pthread_mutex_unlock( &BNA_Escrow_State_Mutex );

          ///////////////////////////////////////////////////////////////////////

          return;



}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void SetSpecificNoteInhibitStatus(int fare)
{
	   
           

           pthread_mutex_lock(&NASetSpecificFaremutex);


           g_NACurrentAtvmFare=0;

	   g_SpecificNoteInhibitFlag=1;

	   g_NACurrentAtvmFare=fare;

           /*

           //////////////////////////////////////////////////////////////////////////////////////////

           char log[100];

	   memset(log,'\0',100);

	   sprintf(log,"[SetSpecificNoteInhibitStatus()] fare: Rs. %d .",g_NACurrentAtvmFare); 

	   writeFileLog(log);

           //////////////////////////////////////////////////////////////////////////////////////////
	   
           */

	   pthread_mutex_unlock(&NASetSpecificFaremutex);
           
           return;
           
	      

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int  NoteAcptrStartCreditPollThread()
{
	
	  
                  pthread_t thread1;
		  int  iret1;
		  int returnVal=-1;  
		  pthread_attr_t attr;
		  returnVal = pthread_attr_init(&attr);
		  if(0!=returnVal)
		  return 0;
		  returnVal = pthread_attr_setdetachstate( &attr , PTHREAD_CREATE_DETACHED );
		  if(0!=returnVal)
		  return 0;
		  iret1     = pthread_create(&thread1, &attr,BNA_Credit_Poll,NULL); 
		  if(0!= iret1)
		  return 0;
		  returnVal = pthread_attr_destroy(&attr);
                 
                  writeFileLog( "[NoteAcptrStartCreditPollThread()] BNA Credit Poll Create successfully.");
                        
                  return 1;

                  
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void  NoteAcptrStartSignalCreditPollThread()
{


                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &stopThreadmutex );

                g_stopThread =false;
		
                pthread_mutex_unlock( &stopThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_signal( &g_NAEnableThreadCond );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                return;


}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void  NoteAcptrStopSignalCreditPollThread()
{


                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &stopThreadmutex );

                g_stopThread =true;
		
                pthread_mutex_unlock( &stopThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_signal( &g_NAEnableThreadCond );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                return;


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Milliseconds delay [value must be 0-900]
static void bna_delay_mSec(const int milisec) 
{

        

        unsigned char log[100];

        memset(log,'\0',100);

        struct timespec req = {0},rim={0};

	req.tv_sec = 0;

	req.tv_nsec = milisec * 1000000L; //[0 .. 999999999] otherwise return error

        int rtcode =0;

        //rtcode = nanosleep(&req, &rim);

        /*
        clock_nanosleep return 0 on success

        CLOCK_MONOTONIC  A nonsettable, monotonically increasing clock that
                        measures time since some unspecified point in the
                        past that does not change after system startup.
        */
         //rtcode =clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&req,&rim);
         clock_nanosleep(CLOCK_MONOTONIC,
                        TIMER_ABSTIME,
                        &req,
                        NULL
                       );
        if( rtcode < 0 )   
	{
	      
              memset(log,'\0',100);
              sprintf(log,"[bna_delay_mSec()] nanosleep() system call failed with return code  %d .",rtcode); 
              writeFileLog(log);
              return;
	      

	}
        else
        {
              memset(log,'\0',100);
              sprintf(log,"[bna_delay_mSec()] nanosleep() system call success with return code  %d .",rtcode); 
              writeFileLog(log);
              return;

        }




}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline int bna_Wait(unsigned int Second)
{
		   

                  if(Second<=0)
                  {
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

				  #ifdef BNA_DEBUG

		                  //memset(log,'\0',100);
		           
		                  //sprintf(log,"[bna_Wait()] Seconds Elapsed=%d.",diffts.tv_sec); 

		                  //writeFileLog(log);

		                  #endif

		                  if(diffts.tv_sec>=Second)
		                  {

		                             memset(log,'\0',100);
				   
				             sprintf(log,"[bna_Wait()] Seconds Elapsed=%d.",diffts.tv_sec); 

				             writeFileLog(log);

				             return 1;

		                  }


                 
                       }



}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void SetCoinFullDisableFlag(int maxNoOfCash , int maxNoOfCoin)
{


          return;

/*          
          if(maxNoOfCash >= 1 && maxNoOfCash <= 20 ) //if note acceptor enable then going to set or unset of coin acceptor control
          {
		  if( 0 == maxNoOfCoin )//coin acceptor is diable due to maxnocoin is zero
		  {
		       g_CoinDisableflag=1; //set (user predefined control / out of control )
		       writeFileLog("[SetCoinFullDisableFlag()] Runtime Coin Control Disabled due to maxnoofcoin is zero.");
		  }
		  else if( maxNoOfCoin >= 1 && maxNoOfCoin <= 20 ) //coin acceptor is enable due to maxnocoin range is valid(1-20)
		  {
		       g_CoinDisableflag=0; //default (user runtime control in control )
		       writeFileLog("[SetCoinFullDisableFlag()] Runtime Coin Control Enabled.");
		  }
          }
          else if( maxNoOfCash <= 0) //if note acceptor disable then no need to control coin acceptor
          {
                g_CoinDisableflag=1; //set (user predefined control / out of control )
		writeFileLog("[SetCoinFullDisableFlag()] Runtime Coin Control Disabled due to maxnoofcash value less than 1.");
       
          } 

          
*/



}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline unsigned int BNA_GetEscrowStateInfo()
{
   
           

	   int BNAState=0x00;

	   unsigned char log[100];

           pthread_mutex_lock (&operationCompleteOccured_Event_mutex);

           if (true == g_OperationCompleteflag )
           {

                   
                   pthread_mutex_lock( &BNA_Escrow_State_Mutex );
                   

                   if( true == g_EscrowEvent )
		   {

		         #ifdef BNA_DEBUG
                         writeFileLog( "[BNA_GetEscrowStateInfo()] Escrow State Found.");
                         #endif

		         BNAState=BNA_ESCROW;
		  
		   }         
           
           
                   pthread_mutex_unlock ( &BNA_Escrow_State_Mutex );


           }

           pthread_mutex_unlock (&operationCompleteOccured_Event_mutex);
           
           
	  
           return BNAState;

           


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline unsigned int  BNA_GetAcceptedDenomInfo()
{
       

	       int AcceptedDenom=0;

	       unsigned char log[100];
		     
	       pthread_mutex_lock( &BNA_Escrow_State_Mutex );


	       if( true == g_EscrowEvent)
	       {
		     
		     g_EscrowEvent=false;

		     AcceptedDenom=g_BNA_Accepted_Moneyvalue;

                     g_BNA_Accepted_Moneyvalue=0;

                     #ifdef BNA_DEBUG

		     memset(log,'\0',100);

		     sprintf(log,"[BNA_GetAcceptedDenomInfo()] AcceptedDenom =Rs. %d .",AcceptedDenom);

		     writeFileLog(log);

                     #endif

	      }

	      pthread_mutex_unlock( &BNA_Escrow_State_Mutex );
	     
	      return  AcceptedDenom;
	      
               

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline int SetCurrentfare(int Remainbalance)
{


     pthread_mutex_lock(&NASetCurrentFare);

     g_faretobeaccept=0;

     g_faretobeaccept=Remainbalance;
     
     pthread_mutex_unlock(&NASetCurrentFare);

     return 0;



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int GetNoteDetailInEscrow()
{
          

          unsigned int Current_state=0x00;

          unsigned int Current_Accepted_Denom=0x00;

          //make a log about to be accept fare
          char log[200];

          memset(log,'\0',200);

          /*

          memset(log,'\0',200);

          sprintf(log,"[GetNoteDetailInEscrow()] Fare to be accept Rs. : %d ", g_faretobeaccept);

	  writeFileLog(log);
          
         */

          //Get any Escrow State information
          Current_state = BNA_GetEscrowStateInfo();

          if( BNA_ESCROW == Current_state ) 
          { 
                     
                     #ifdef BNA_DEBUG
                     writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Note Stacked.");  
                     #endif

                     //get current accepted denom        
                     Current_Accepted_Denom=BNA_GetAcceptedDenomInfo();

                     //make a log about accepted money
                     #ifdef BNA_DEBUG
                     memset(log,'\0',200);
                     sprintf(log,"[GetNoteDetailInEscrow() if block] Accepted Note Rs. : %d ", Current_Accepted_Denom);
	             writeFileLog(log);
                     #endif
                        
                     #ifdef BNA_DEBUG
                     writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Normal path Now time to return from here.");
                     #endif

                     return Current_Accepted_Denom; 

                    
	  }    
          else //by default return zero
          {

                   return 0;

          }
          


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* BNA_Credit_Poll(void *ptr)
{


                          

                /////////////////////////////////////////////////////////////////////////////////////////////

                //wait for signal
                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_wait( &g_NAEnableThreadCond , &g_NAEnableThreadmutex );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &stopThreadmutex );


                //STOP CREDIT POLLING THREAD
		if( true ==  g_stopThread )
		{
		                   
                                   pthread_mutex_unlock(&stopThreadmutex);

		                   writeFileLog("[BNA_Credit_Poll() AtvmAPI Exit] BNA Credit Poll Thread now exit."); 

                                   pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

		                   g_NoteLowLevelPollFlagStatus=0;

                                   pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);
		                    
		                   pthread_exit(0);

 
                }

                pthread_mutex_unlock(&stopThreadmutex);
               
                writeFileLog("[BNA_Credit_Poll() AtvmAPI Exit] BNA Credit Poll Thread now started."); 

                /////////////////////////////////////////////////////////////////////////////////////////////////////
   
                 bool DisableFlag=false,DenomUpdateFlag=false,goforeject=false,rejectevent=false;

                 T_BnrXfsResult rtcode=-1;

                 int fare=0;

                 char log[200];

                 memset(log,'\0',200);

                 while(1)
                 {

                                   pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

                                   //credit polling started
		                   g_NoteLowLevelPollFlagStatus=1;

                                   pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);


                                   //Enable 1: Note Acceptor Enable [BNA Escrow State and External Coin Acceptor Escrow handshake State]
		                   pthread_mutex_lock(&NASetSpecificFaremutex);

                                   pthread_mutex_lock(&stopThreadmutex);

		                   if( (1 == g_SpecificNoteInhibitFlag) && (false == g_stopThread ) && ( false == DisableFlag ) )
				   {

                                                      

		                                      bool escrowenable=false;

		                                      int fare=0;

		                                      fare = g_NACurrentAtvmFare;

				                      g_SpecificNoteInhibitFlag=0;

                                                      memset(log,'\0',200);	       

						      sprintf(log,"[BNA_Credit_Poll() Escrow]  Current to be accepted fare: Rs. %d .", fare);

		                                      writeFileLog(log);

						      pthread_mutex_lock( &BNA_Escrow_Enable_Mutex );

		                                      if( true == g_EscrowEnableFlag )
		                                      {
		                                        
		                                             escrowenable=true;
		                                             g_EscrowEnableFlag=false;

		                                      }

		                                      pthread_mutex_unlock( &BNA_Escrow_Enable_Mutex );
		                                      	       
				                      if(true == escrowenable)
		                                      {
		                                            
		                                           ClearGlobalEventState(); 
		                                    
				                           DisableSpecificNotes(fare);

				                           rtcode=-1;

				                           rtcode=bnr_CashIn(NULL,NULL);

				                           if( rtcode>0 )
				                           {

				                              writeFileLog("[BNA_Credit_Poll() Escrow Enable ] bnr_CashIn called successfully.");

				                           }
				                           else
				                           {

				                               writeFileLog("[BNA_Credit_Poll() Escrow Enable] bnr_CashIn called failed.");

				                           }


		                                      }

		                                      else if(false == escrowenable)
		                                      {

				                              writeFileLog("[BNA_Credit_Poll() External Enable] External Enable Event lock."); 
		                                              //Disable current cashin operation
				                              bnr_Cancel();  

				                              WaitforMeiOperationComplete( 2 );

				                              //Enable current denomation
				                              DisableSpecificNotes(fare);
			 
				                              //again start current cashin operation

				                              rtcode=bnr_CashIn(NULL,NULL);

				                              if( rtcode>0 )
				                              {

				                                  writeFileLog("[BNA_Credit_Poll() Escrow] bnr_CashIn called successfully.");

				                              }
				                              else
				                              {

				                                  writeFileLog("[BNA_Credit_Poll() Escrow] bnr_CashIn called failed.");

				                              }

		                                     } 

		                                      

		                   }

                                   pthread_mutex_unlock(&stopThreadmutex);

		                   pthread_mutex_unlock(&NASetSpecificFaremutex);


                               //Enable 2: Reject State BNA Enable 
                            
                               pthread_mutex_lock (&Reject_Event_mutex);

                               if( ( true == g_RejectEvent ) && ( false == g_stopThread ) && ( false == DisableFlag ) )
                               {
                                               
                                                 

                                                 ///////////////////////////////////////////////////////////////////////////////

                                                 pthread_mutex_lock(&operationCompleteOccured_Event_mutex);

                                                 if( true == g_OperationCompleteflag )
                                                 {

                                                           writeFileLog("[BNA_Credit_Poll() Reject] Reject Event Complete.");

                                                           g_RejectEvent=false;


                                                 }
                                                  
                                                 pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);
                                                  
                                                 if( false == g_RejectEvent )
                                                 {

                                                       writeFileLog("[BNA_Credit_Poll() Reject] Before call cashin oepration.");

                                                       //Denom update table only max quantity of note greater than 1
                                                       if( true == g_DenomTableUpdateFlag )
                                                       {
				                                       fare=0;

				                                       DenomUpdateFlag=false;

				                                       pthread_mutex_lock(&NASetSpecificFaremutex);

					                               if( 1 == g_SpecificNoteInhibitFlag )
				                                       {

				                                             fare = g_NACurrentAtvmFare;

				                                             DenomUpdateFlag=true;

				                                             g_SpecificNoteInhibitFlag=0;

				                                       }

				                                      pthread_mutex_unlock(&NASetSpecificFaremutex);

				                                      if(false == DenomUpdateFlag)
				                                      {


				                                          pthread_mutex_lock(&NASetCurrentFare);

									  if(g_faretobeaccept>0)
				                                          {

				                                                 fare = g_faretobeaccept;

				                                                 DenomUpdateFlag=true;
				                                          }

				                                          pthread_mutex_unlock(&NASetCurrentFare);

				                                          
				                                      
								     }

								     if( true == DenomUpdateFlag )
				                                     {

								          memset(log,'\0',200);	       

									  sprintf(log,"[BNA_Credit_Poll() Reject]  Current to be accepted fare: Rs. %d .", fare);

									  writeFileLog(log);

								          DisableSpecificNotes(fare);

						                          DenomUpdateFlag=false;

				                                     }


                                                     }//if( true == g_DenomTableUpdateFlag ) block

                                                     rtcode=-1;

                                                     //start again cashin operation

		                                     rtcode=bnr_CashIn(NULL,NULL);

                                                     if( rtcode>0 )
                                                     {

                                                          writeFileLog("[BNA_Credit_Poll() Reject] bnr_CashIn called successfully.");
                                                     }
                                                     else
                                                     {

                                                          writeFileLog("[BNA_Credit_Poll() Reject] bnr_CashIn called failed.");

                                                     }

                                                     writeFileLog("[BNA_Credit_Poll() Reject] After call cashin operation.");

                                                
                                                }

                                                ///////////////////////////////////////////////////////////////////////////////

                                                 
                                                  
                                                
                              }

                              pthread_mutex_unlock ( &Reject_Event_mutex );

		             /////////////////////////////////////////////////////////////////////////////////////////////////

                             pthread_mutex_lock(&DisableAcceptancemutex);

                             //Disable Acceptance
                             if( true == g_DisableAcceptance ) 
		             { 


                                 pthread_mutex_unlock(&DisableAcceptancemutex);

                                 ///////////////////////////////////////////////////////////////////////////////////////////

                                 writeFileLog("[BNA_Credit_Poll() Disable] Start Block."); 

                                 g_DisableAcceptance=false;
                         
                                 //Now Cancel all cashin trans

                                 rtcode=-1;

		                 rtcode=bnr_Cancel();  

                                 if(BXR_NO_ERROR == rtcode)
                                 {

                                       writeFileLog("[BNA_Credit_Poll() Disable] bnr_Cancel() called Successfully.");

		                 }   
		                 else
		                 {

                                       writeFileLog("[BNA_Credit_Poll() Disable] bnr_Cancel() call Failed.");

		                 }   

                                 bna_delay_mSec(200);

                                 DisableFlag=true;

                                 writeFileLog("[BNA_Credit_Poll() Disable] End Block."); 


                                
                                continue;

                           }

                           pthread_mutex_unlock(&DisableAcceptancemutex);

                           /////////////////////////////////////////////////////////////////////////////////////////////////

                           pthread_mutex_lock(&stopThreadmutex);

		           //STOP CREDIT POLLING THREAD
		           if( true ==  g_stopThread )
		           {
		                   
                                   pthread_mutex_unlock(&stopThreadmutex);

		                   writeFileLog("[BNA_Credit_Poll() Stop Thread] BNA Credit Poll Thread now exit."); 

                                   pthread_mutex_lock(&NoteLowLevelPollFlagStatusmutex);

		                   g_NoteLowLevelPollFlagStatus=0;

                                   pthread_mutex_unlock(&NoteLowLevelPollFlagStatusmutex);
		                    
		                   pthread_exit(0);
 
                           }

                           pthread_mutex_unlock(&stopThreadmutex);

                           /////////////////////////////////////////////////////////////////////////////////////////////////


		    
             } //while loop end   

             


}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline int SetAlreadyCoinInhibit()
{

   //g_AlreadyCoinDisable=false;
   return 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void  CloseNoteAcceptorPollThread()
{

    pthread_mutex_lock(&stopThreadmutex);

    g_stopThread=true;

    pthread_mutex_unlock(&stopThreadmutex);

    return;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline unsigned int InhibitNoteAcptr()
{

       pthread_mutex_lock(&DisableAcceptancemutex);

       g_DisableAcceptance=true;

       pthread_mutex_unlock(&DisableAcceptancemutex);

       return 0;

    
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline int DisableSpecificNotes(int fare)
{
	

                         

                     
			 NOTE notevar={0,0,0,0,0,0,0};
			 if(fare>=5)
			 notevar.INR_5    = 1;
			 if(fare>=10)
			 notevar.INR_10   = 1;
			 if(fare>=20)
			 notevar.INR_20   = 1;
			 if(fare>=50)
			 notevar.INR_50   = 1;
			 if(fare>=100)
			 notevar.INR_100  = 1;
			 if(fare>=500)
			 notevar.INR_500  = 1;
			 if(fare>=1000)
			 notevar.INR_1000 = 1;

                         return (EnableDenom(notevar));

                    
       
                      

	       
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int IssuedisableAllBillCmd()
{

	
       NOTE notevar={0,0,0,0,0,0,0};
       return (EnableDenom(notevar));


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ClearGlobalEventState()
{

    #ifdef BNA_DEBUG_EVENT_CLEAR

    writeFileLog("\n\n");
 
    writeFileLog("[ClearGlobalEventState()] Start.");  

    #endif

    //////////////////////////////////////////////////////////////////////////////////////////////////

    
     pthread_mutex_lock (&operationCompleteOccured_Event_mutex);

     #ifdef BNA_DEBUG_EVENT_CLEAR

     writeFileLog("[ClearGlobalEventState()] Start Clear operationCompleteOccured flags.");  

     #endif

     g_OperationCompleteflag=false;

     g_operationCompleteResult=0;

     g_operationCompleteextendedResult=0;
 
     #ifdef BNA_DEBUG_EVENT_CLEAR

     writeFileLog("[ClearGlobalEventState()] End Clear operationCompleteOccured flags.");

     #endif

     pthread_mutex_unlock (&operationCompleteOccured_Event_mutex);


     //////////////////////////////////////////////////////////////////////////////////////////////////

     pthread_mutex_lock (&statusOccured_Event_mutex);

     #ifdef BNA_DEBUG_EVENT_CLEAR

     writeFileLog("[ClearGlobalEventState()] Start Clear statusOccured flags.");

     #endif
 
     g_statusOccuredStatus=0;

     g_statusOccuredResult=0;

     g_statusOccuredextendedResult=0;

     #ifdef BNA_DEBUG_EVENT_CLEAR

     writeFileLog("[ClearGlobalEventState()] End Clear statusOccured flags.");

     #endif

     pthread_mutex_unlock (&statusOccured_Event_mutex);

     //////////////////////////////////////////////////////////////////////////////////////////////////
 
     pthread_mutex_lock (&intermediateOccured_Event_mutex);

     #ifdef BNA_DEBUG_EVENT_CLEAR
 
     writeFileLog("[ClearGlobalEventState()] Start Clear intermediateOccured flags.");

     #endif

     g_intermediateOccuredResult=0;

     #ifdef BNA_DEBUG_EVENT_CLEAR

     writeFileLog("[ClearGlobalEventState()] End Clear intermediateOccured flags.");

     #endif

     pthread_mutex_unlock (&intermediateOccured_Event_mutex);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    #ifdef BNA_DEBUG_EVENT_CLEAR

    writeFileLog("[ClearGlobalEventState()] End.");  

    writeFileLog("\n\n");

    #endif

    return;



}



/******************************************************************************
 *              operationCompleteOccured
 ******************************************************************************
 * Called when an Operation Complete event is received
 *
 * @param identificationID job identification number
 * @param operationID J/XFS operation code
 * @param result J/XFS result code
 * @param extendedResult extended result code send by the BNR
 * @param data event data. Contents depend upon operationID.
 *
 *****************************************************************************/

static void __stdcall operationCompleteOccured( LONG32 identificationId, 
                                                LONG32 operationId, 
                                                LONG32 result,           //operation complete event status
                                                LONG32 extendedResult, 
                                                void *data
                                              )
{
		  
                 

                 unsigned char log[100];

                 bool EscrowEvent=false,RejectEventComplete=false;

                 #ifdef BNA_DEBUG

                 writeFileLog("[operationCompleteOccured()] Entry.");

                 writeFileLog("[operationCompleteOccured()] An Operation Complete event is received.");  

                 #endif
                       
                 ///////////////////////////////////////////////////////////////////////////////////////////////////////
           
                 pthread_mutex_lock(&operationCompleteOccured_Event_mutex);

                 #ifdef BNA_DEBUG

                 writeFileLog("[operationCompleteOccured()] Before set operationCompleteOccured flags.");

                 #endif

                 g_OperationCompleteflag=true;

                 g_operationCompleteResult=result;

                 g_operationCompleteextendedResult=extendedResult;

                 #ifdef BNA_DEBUG

                 writeFileLog("[operationCompleteOccured()] After set operationCompleteOccured flags.");

                 #endif

                 pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);

                 ///////////////////////////////////////////////////////////////////////////////////////////////////////

                 #ifdef BNA_DEBUG

                 writeFileLog("\n\n");


                 memset(log,'\0',100);
                 sprintf(log,"[operationCompleteOccured()] identificationId: %ld .",identificationId);
                 writeFileLog(log);

                 memset(log,'\0',100);
                 sprintf(log,"[operationCompleteOccured()] operationId: %ld .",operationId);
                 writeFileLog(log);

                 memset(log,'\0',100);
                 sprintf(log,"[operationCompleteOccured()] Result: %ld .",result);
                 writeFileLog(log);

                 memset(log,'\0',100);
                 sprintf(log,"[operationCompleteOccured()] Extended Result: %ld .",extendedResult);
                 writeFileLog(log);
                  

                 writeFileLog("\n\n");

                 #endif

                 EscrowEvent=false;

                 RejectEventComplete=false;

                 switch(operationId)
                 {


                       case  OPERATION_CASH_IN:

                                               #ifdef BNA_DEBUG
                                               writeFileLog("[operationCompleteOccured()] Before Set Cash In() Event.");
                                               #endif

                                               pthread_mutex_lock (&statusOccured_Event_mutex);

                                               if( (6153 == g_statusOccuredStatus) || (6155 ==g_statusOccuredStatus ) )
                                               {
                                                        //#ifdef BNA_DEBUG
                                                        writeFileLog("[operationCompleteOccured()] Found Escrow Event.");
                                                        //#endif
                                                        EscrowEvent=true;

                                               }
                                               else if( 6192 == g_statusOccuredStatus )
                                               {

                                                        //#ifdef BNA_DEBUG
                                                        writeFileLog("[operationCompleteOccured()] Found Reject Event Complete.");
                                                        //#endif

                                                        RejectEventComplete=true;

                                                        pthread_mutex_lock ( &Reject_Status_Occured_EventEvent_mutex );

                                                        g_Reject_Status_Occured_Event=false;

                                                        pthread_mutex_unlock ( &Reject_Status_Occured_EventEvent_mutex );

        
                                               }

                                               pthread_mutex_unlock (&statusOccured_Event_mutex);

                                               if( true == RejectEventComplete )
                                               {

                                                      pthread_mutex_lock (&Reject_Event_mutex);


                                                      g_RejectEvent=true;


                                                      pthread_mutex_unlock (&Reject_Event_mutex);


                                               }

                                               if( (NULL !=data) && ( 0 == result)  && ( true == EscrowEvent) )
                                               {

                                                      memcpy(&g_CashOrder,data,sizeof(T_XfsCashOrder));

                                                      #ifdef BNA_DEBUG

                                                      memset(log,'\0',100);

                                                      sprintf(log,"[operationCompleteOccured()] Accepted Money : %ld .", (g_CashOrder.denomination.amount/100) );

                                                      writeFileLog(log);

                                                      #endif
                                                       
                                                      pthread_mutex_lock( &BNA_Escrow_State_Mutex );

                                                      if( (6155 == g_statusOccuredStatus) ||  (6153 == g_statusOccuredStatus) )
                                                      {

		                                              g_EscrowEvent=true;
		                       
		                                              g_BNA_Accepted_Moneyvalue=0;

			                                      g_BNA_Accepted_Moneyvalue=(g_CashOrder.denomination.amount/100);

                                                              #ifdef BNA_DEBUG

		                                              memset(log,'\0',100);


		                                              sprintf(log,"[operationCompleteOccured()] g_BNA_Accepted_Moneyvalue : %d .", g_BNA_Accepted_Moneyvalue );

		                                              writeFileLog(log);

                                                              #endif

                                                      }

		                                      pthread_mutex_unlock( &BNA_Escrow_State_Mutex );

                                                      pthread_mutex_lock( &BNA_Escrow_Enable_Mutex );

                                                      g_EscrowEnableFlag=true;

                                                      pthread_mutex_unlock( &BNA_Escrow_Enable_Mutex );

                                               }
                                               
                                               #ifdef BNA_DEBUG
                                               writeFileLog("[operationCompleteOccured()] After Set Cash In() Event.");
                                               #endif
                                               break;

                       case  OPERATION_RESET :  
                                                #ifdef BNA_DEBUG
                                                writeFileLog("[operationCompleteOccured()] Reset Event Complete.");
                                                #endif
                                                break;   

                           
                       case  OPERATION_CASH_IN_START: 
                                                      //#ifdef BNA_DEBUG
                                                      writeFileLog("[operationCompleteOccured()] Cash In Start Event Complete.");
                                                      //#endif
                                                      break;
  
                    
                       case  OPERATION_CASH_IN_ROLLBACK: 
                                                         //#ifdef BNA_DEBUG
                                                         writeFileLog("[operationCompleteOccured()] Cash In Rollback Event Complete."); 
                                                         //#endif
                                                         break;  

                    
                       case  OPERATION_CASH_IN_END:  
                                                     //#ifdef BNA_DEBUG
                                                     writeFileLog("[operationCompleteOccured()] Cash In End Event Complete.");
                                                     //#endif
                                                     break;  

                    


                 };

                 #ifdef BNA_DEBUG
                 writeFileLog("[operationCompleteOccured()] Exit.");
                 #endif
                 return;



}//operationCompleteOccured



/******************************************************************************
*             statusOccured
*******************************************************************************
* Called when an Status event is received
*
* @param status XFS status code
* @param result J/XFS result code
* @param extendedResult extended result code send by the BNR
* @param details event data. Contents depend upon status.
*
********************************************************************************/

static void __stdcall statusOccured(  LONG32 status,           //Status Event Code
                                      LONG32 result, 
                                      LONG32 extendedResult, 
                                      void *data 
                                   )
{
    

         unsigned char log[100];

         #ifdef BNA_DEBUG

         writeFileLog("[statusOccured()] Entry.");

         writeFileLog("[statusOccured()] An Operation Status event is received.");

         #endif

         ///////////////////////////////////////////////////////////////////////////////////////////////////////

         pthread_mutex_lock (&statusOccured_Event_mutex);

         #ifdef BNA_DEBUG
         writeFileLog("[statusOccured()] Going to set statusOccured flags.");
         #endif

         g_statusOccuredStatus=status;

         g_statusOccuredResult= result;
          
         g_statusOccuredextendedResult= extendedResult;

         pthread_mutex_unlock (&statusOccured_Event_mutex);

         ///////////////////////////////////////////////////////////////////////////////////////////////////////

         #ifdef BNA_DEBUG

         writeFileLog("\n\n");

         memset(log,'\0',100);
         sprintf(log,"[statusOccured()] Status: %ld .",status);
         writeFileLog(log);

         memset(log,'\0',100);
         sprintf(log,"[statusOccured()] Result: %ld .",result);
         writeFileLog(log);

         memset(log,'\0',100);
         sprintf(log,"[statusOccured()] extendedResult: %ld .",extendedResult);
         writeFileLog(log); 
         
         writeFileLog("\n\n");
        
         #endif

         #ifdef BNA_DEBUG
         writeFileLog("[statusOccured()] Exit.");
         #endif

                  
         //Start Wait for note taken back by user state
         pthread_mutex_lock ( &Reject_Status_Occured_EventEvent_mutex );

         if( 6223 == status)
         {

                g_Reject_Status_Occured_Event=true;

                writeFileLog("[intermediateOccured()] Found Note Rejecting State.");

         }
         
         pthread_mutex_unlock ( &Reject_Status_Occured_EventEvent_mutex );

         return;



}//statusOccured



/******************************************************************************
*             intermediateOccured
*******************************************************************************
* Called when an Intermediate event is received
*
* @param identificationID job identification number
* @param operationID J/XFS operation code
* @param reason J/XFS reason code
* @param data event data. Contents depend upon reason
*
******************************************************************************/

static void __stdcall intermediateOccured(  LONG32 identificationId, 
                                            LONG32 operationId,          //OperationCode
                                            LONG32 reason,               //intermeditate event code
                                            void *data 
                                         )
{
       

         unsigned char log[100];

         #ifdef BNA_DEBUG

         writeFileLog("[intermediateOccured] Entry.");

         writeFileLog("[intermediateOccured] An Operation intermediateOccured event is received.");  

         #endif

         ///////////////////////////////////////////////////////////////////////////////////////////////////////

         pthread_mutex_lock (&intermediateOccured_Event_mutex);

         #ifdef BNA_DEBUG
         writeFileLog("[intermediateOccured] Going to set intermediateOccured flags.");
         #endif

         g_intermediateOccuredResult=reason;

         pthread_mutex_unlock (&intermediateOccured_Event_mutex);

         ///////////////////////////////////////////////////////////////////////////////////////////////////////
 
         #ifdef BNA_DEBUG

         writeFileLog("\n\n");

         memset(log,'\0',100);
         sprintf(log,"[intermediateOccured()] identificationId: %ld .",identificationId);
         writeFileLog(log);

         memset(log,'\0',100);
         sprintf(log,"[intermediateOccured()] operationId: %ld .",operationId);
         writeFileLog(log);

         memset(log,'\0',100);
         sprintf(log,"[intermediateOccured()] reason: %ld .",reason);
         writeFileLog(log);

         writeFileLog("\n\n");

         writeFileLog("[intermediateOccured] Exit.");

         #endif  

         ///////////////////////////////////////////////////////////////////////////////////////////////////////



         
         return;





}//intermediateOccured




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



static unsigned int WaitforMeiOperationComplete( double waittime )
{
              
                      
                   int rtcode=-1;

                   unsigned char log[100];

                   memset(log,'\0',100);

                   struct timespec begints, endts,diffts;

                   clock_gettime(CLOCK_MONOTONIC, &begints);

                   //Wait here 
		   for(;;)
		   {
			    

				             clock_gettime(CLOCK_MONOTONIC, &endts);

					     diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                             //memset(log,'\0',100);

				             //sprintf(log,"[WaitforMeiOperationComplete()] Time =  %d seconds.",diffts.tv_sec);

				             //writeFileLog(log);
					     
					     //if timer is running
					     if(diffts.tv_sec < waittime)
					     {         
 
                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime ] Before Lock operation complete flag.");

                                                        pthread_mutex_lock(&operationCompleteOccured_Event_mutex);
				                       
                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime ] After Lock operation complete flag.");
                                                        if( true == g_OperationCompleteflag ) 
							{
 
                                                                    memset(log,'\0',100);

				                                    sprintf(log,"[WaitforMeiOperationComplete() diffts.tv_sec < waittime] Operation Completed %d seconds.",diffts.tv_sec);

				                                    writeFileLog(log);

                                                                    pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);

					                            return 1;

							}
							
                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime] Before Unlock operation complete flag.");

                                                        pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);

                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime] After Unlock operation complete flag.");
							            
				                        

					     }

					     //if timer time is end or timer is running
					     if(diffts.tv_sec >= waittime)
					     {

					
                                                        memset(log,'\0',100);


				                        sprintf(log,"[WaitforMeiOperationComplete() diffts.tv_sec >= waittime] Operation Timeout %d seconds.",diffts.tv_sec);

				                        writeFileLog(log);

                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime] Before Unlock operation complete flag.");

                                                        pthread_mutex_lock (&operationCompleteOccured_Event_mutex);

                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime] After Unlock operation complete flag.");

				                        if( true == g_OperationCompleteflag )
							{

							    rtcode=1;

							}
							else if( true != g_OperationCompleteflag )
							{ 

							    rtcode=0;

							}
                                                        else 
							{ 

							    rtcode=2; //timeout

							}

                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime] Before Unlock operation complete flag.");

                                                        pthread_mutex_unlock (&operationCompleteOccured_Event_mutex);

                                                        //writeFileLog("[WaitforMeiOperationComplete() diffts.tv_sec < waittime] After Unlock operation complete flag.");

				                        return rtcode;


					     }//timer if block end here


			      

		   }//for loop end


	   
	 	
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int BNA_Activate()
{

	    
            g_OperationCompleteflag=false;

            T_BnrXfsResult  result=0; 
    
            T_XfsCdrStatus  bnrStatus={0};       
	    
            unsigned char log[100];

            memset(log,'\0',100);

            writeFileLog("[BNA_Activate()] Going to open BNA6F Prototype .");    
	    
            ClearGlobalEventState();

            //Open MEI Note Acceptor
            
            result = bnr_Open(operationCompleteOccured, statusOccured, intermediateOccured);

	    if ( BXR_NO_ERROR == result ) 
	    {
		      
                      writeFileLog("[BNA_Activate()] Open BNA6F Prototype Request accepted.");   

		      //Reset if needed
                      writeFileLog("[BNA_Activate()] Now Going to call bnr_GetStatus() api.");

		      if ( BXR_NO_ERROR == bnr_GetStatus(&bnrStatus) ) 
		      {
				 
                                memset(log,'\0',100);

                                sprintf(log,"[BNA_Activate()] bnr_GetStatus device status code %d .",bnrStatus.deviceStatus);

                                writeFileLog(log);   

                                if ( (  XFS_S_CDR_DS_ON_LINE    != bnrStatus.deviceStatus) ||
                                     (  XFS_S_CDR_DS_USER_ERROR != bnrStatus.deviceStatus)
                                   )
				 {

                                          writeFileLog("[BNA_Activate()] BNA6F Prototype Device is not online state! So going to reset it.");              

                                          ClearGlobalEventState(); 

                                          result=-1; 

                                          //Reset MEI Note Acceptor  
                                          writeFileLog("[BNA_Activate()] Now Going to call bnr_Reset() api.");     

                                          result=bnr_Reset();

                                          memset(log,'\0',100);

                                          sprintf(log,"[BNA_Activate()] Device reset function return code %d ." ,result);

                                          writeFileLog(log); 

                                          if(result>0)
                                          {
                       
                                                   memset(log,'\0',100);

                                                   sprintf(log,"[BNA_Activate()] Now Wait for BNA6F Prototype Reset Opertion Expected time = %d Seconds." ,BNR_RESET_OPERATION_TIME_OUT_IN_SECONDS);

                                                   writeFileLog(log); 

		                                  //Wait for bnr_Reset() operation complete
		                                  WaitforMeiOperationComplete( BNR_RESET_OPERATION_TIME_OUT_IN_SECONDS );

		                                  if( XFS_RC_SUCCESSFUL == g_operationCompleteResult)
						  {
							 
		                                        writeFileLog("[BNA_Activate()] BNA6F Prototype Device reset success.");      
                                                        return 1;
						   
						  }
						  else
						  { 
						        
                                                        writeFileLog("[BNA_Activate()] BNA6F Prototype Device reset failed!!.");     

							return 0;
							    
						  }
					  

                                          }
                                          else if(XFS_RC_SUCCESSFUL != result)
                                          {

                                                        writeFileLog("[BNA_Activate()] BNA6F Prototype Unable to proceed to reset request!!.");     

							return 0;
                              
                                           }


				 }
                                 else if(XFS_S_CDR_DS_ON_LINE   == bnrStatus.deviceStatus)
                                 {
                                          
                                          writeFileLog("[BNA_Activate()] BNA6F Prototype Device is online already state no need to reset it ."); 

                                          return 0;

                                 }

                                 
                      }
                      else
                      {
                                 

                                 writeFileLog("[BNA_Activate()] BNA6F Prototype Get Current Status Failed!!."); 

                                 return 0;

                      }

               }
               else
               {

		        
		           memset(log,'\0',100);

		           sprintf(log,"[BNA_Activate()] BNA6F Prototype Open Failed!! return code = %d .",result );

		           writeFileLog(log); 

		           return 0;
		  
               }


     
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int BNA_DeActivate()
{



     T_BnrXfsResult resultcode=-1;

     unsigned char log[100];

     memset(log,'\0',100);

     resultcode = bnr_Close();

     memset(log,'\0',100);

     sprintf(log,"[BNA_DeActivate()] bnr_Close() return code = %d .",resultcode );

     writeFileLog(log); 

     if( BXR_NO_ERROR == resultcode )
     {

          writeFileLog("[BNA_DeActivate()] Successfully close bnr port."); 
          return 1;

     }
     else
     {
          writeFileLog("[BNA_DeActivate()] Failed to  close bnr port."); 
          return 0;
  
     }

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int BNA_Reset()
{



                                          unsigned char log[100];

		                          memset(log,'\0',100);

                                          ClearGlobalEventState(); 

                                          int result=-1; 

                                          //Reset MEI Note Acceptor  
                                          writeFileLog("[BNA_Reset()]  Now Going to call bnr_Reset() api.");     

                                          result=bnr_Reset();

                                          memset(log,'\0',100);

                                          sprintf(log,"[BNA_Reset()] Device reset function return code %d ." ,result);

                                          writeFileLog(log); 

                                          if(result>0)
                                          {
                       
                                                   memset(log,'\0',100);

                                                   sprintf(log,"[BNA_Reset()] Now Wait for BNA Reset Opertion Expected time = %d Seconds." ,BNR_RESET_OPERATION_TIME_OUT_IN_SECONDS);

                                                   writeFileLog(log); 

		                                  //Wait for bnr_Reset() operation complete
		                                  WaitforMeiOperationComplete( BNR_RESET_OPERATION_TIME_OUT_IN_SECONDS );

                                                  pthread_mutex_lock(&operationCompleteOccured_Event_mutex);

		                                  if( XFS_RC_SUCCESSFUL == g_operationCompleteResult)
						  {
							 
                                                        pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);

                                                        writeFileLog("[BNA_Reset()] BNA6F Prototype Device reset success.");     

		                                        return 1;
						   
						  }
						  else
						  { 
						      
                                                        pthread_mutex_unlock(&operationCompleteOccured_Event_mutex);

		                                        writeFileLog("[BNA_Reset()] BNA6F Prototype Device reset failed!!.");     

							return 0;
							    
						  }
  
                                     }
                                     else
                                     {
                                            writeFileLog("[BNA_Reset()] bnr_Reset() api called failed.");     

		                            return 1;

                                     }



}


/*

#define XFS_S_CDR_OFFSET                          6152

#define XFS_S_CDR_CASHUNIT_CHANGED                6153                            One or more cash unit content has been changed.

#define XFS_S_CDR_CASHUNIT_CONFIGURATION_CHANGED  6154                            The cashunit configuration was changed. A physical cashunit has been inserted or removed from the BNR. 

#define XFS_S_CDR_CASHUNIT_THRESHOLD              6155                            The Threshold status has changed.  

#define XFS_S_CDR_DEVICE_STATUS_CHANGED           6162                            The device status has been changed. 

#define XFS_S_CDR_TRANSPORT_CHANGED               6167                            @deprecated No more used. 

#define XFS_S_CDR_DS_HARDWARE_ERROR               6174                            The device is not operational due to a hardware error. 

#define XFS_S_CDR_DS_USER_ERROR                   6175                            A person is preventing proper device operation. 

#define XFS_S_CDR_DS_OFF_LINE                     6179                            The device is not operational. A reset command is needed to try to make it operational. 

#define XFS_S_CDR_DS_ON_LINE                      6180                            The device is present and operational. 

#define XFS_S_CDR_DIS_OK                          6181                            All dispenser logical cash units are ok. 

#define XFS_S_CDR_DIS_CU_STATE                    6182                            One of the dispenser logical cash units present is in an abnormal state. 

#define XFS_S_CDR_DIS_CU_STOP                     6183                            Due to a cash unit failure dispensing is impossible. 

#define XFS_S_CDR_DIS_CU_UNKNOWN                  6184                            Due to a hardware error or other condition, the state of the cash units cannot be determined. 

#define XFS_S_CDR_IS_EMPTY                        6185                            The intermediate stacker is empty. 

#define XFS_S_CDR_IS_NOT_EMPTY                    6186                            The intermediate stacker is not empty. 

#define XFS_S_CDR_IS_UNKNOWN                      6187                            @deprecated No more used.

#define XFS_S_CDR_CASH_TAKEN                      6192                            The cash was taken by the user. 

#define XFS_S_CDR_SD_OPEN                         6194                            The cash module lock (safe door) is open. 

#define XFS_S_CDR_SD_LOCKED                       6196                            The cash module lock (safe door) is closed and locked. 

#define XFS_S_CDR_SD_UNKNOWN                      6197                  

*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int BNA_GetStatus()
{



		T_XfsCdrStatus bnrStatus={0};

		T_BnrXfsResult resultcode=-1;

		unsigned char log[100];

		memset(log,'\0',100);

		resultcode=bnr_GetStatus(&bnrStatus);

		memset(log,'\0',100);

		sprintf(log,"[BNA_GetStatus()] bnr_GetStatus() return code=%ld.",resultcode);

		writeFileLog(log);
  

		
		if ( BXR_NO_ERROR == resultcode ) 
		{

                         
                         writeFileLog("[BNA_GetStatus()] bnr_GetStatus() Successfully executed.");

		         memset(log,'\0',100);

			 sprintf(log,"[BNA_GetStatus()] bnr_GetStatus() deviceStatus=%ld.",bnrStatus.deviceStatus);

			 writeFileLog(log);


		         memset(log,'\0',100);

			 sprintf(log,"[BNA_GetStatus()] bnr_GetStatus() dispenserStatus=%ld.",bnrStatus.dispenserStatus);
 
			 writeFileLog(log);

		         memset(log,'\0',100);

			 sprintf(log,"[BNA_GetStatus()] bnr_GetStatus() intermediateStackerStatus=%ld.",bnrStatus.intermediateStackerStatus);

			 writeFileLog(log);

		         memset(log,'\0',100);

			 sprintf(log,"[BNA_GetStatus()] bnr_GetStatus() safeDoorStatus=%ld.",bnrStatus.safeDoorStatus);

			 writeFileLog(log);

		         memset(log,'\0',100);

			 sprintf(log,"[BNA_GetStatus()] bnr_GetStatus() shutterStatus=%ld.",bnrStatus.shutterStatus);

			 writeFileLog(log);

		         memset(log,'\0',100);

			 sprintf(log,"[BNA_GetStatus()] bnr_GetStatus() transportStatus=%ld.",bnrStatus.transportStatus);

			 writeFileLog(log);
                        
                         /////////////////////////////////////////////////////////////////////////////////////////////////////////

                         switch(bnrStatus.deviceStatus)
                         {

		                //6152
		                case XFS_S_CDR_OFFSET:        
                                                          break;                  

		                //6153  One or more cash unit content has been changed
		                case XFS_S_CDR_CASHUNIT_CHANGED:  writeFileLog("[BNA_GetStatus()] One or more cash unit content has been changed.");          

                                                                  break;


		                //6154 The cashunit configuration was changed. A physical cashunit has been inserted or removed from the BNR. 
                                case XFS_S_CDR_CASHUNIT_CONFIGURATION_CHANGED :writeFileLog("[BNA_GetStatus()] The cashunit configuration was changed. A physical cashunit has been inserted or removed from the BNA6f Prototype.");           
                                                                               return 0;
                                                                               break;

				//6155   The Threshold status has changed.  
				case XFS_S_CDR_CASHUNIT_THRESHOLD: writeFileLog("[BNA_GetStatus()] The Threshold status has changed.");    
                                                                   return 0;
          
                                                                   break;


				//6162  The device status has been changed. 
				case XFS_S_CDR_DEVICE_STATUS_CHANGED:  writeFileLog("[BNA_GetStatus()] The device status has been changed.");    
                                                                       return 0;
                                                                       break;

				//6167   @deprecated No more used.
				case XFS_S_CDR_TRANSPORT_CHANGED:  
                                                                   return 0;
                                                                   break;
       
				//6174  The device is not operational due to a hardware error. 
				case XFS_S_CDR_DS_HARDWARE_ERROR: writeFileLog("[BNA_GetStatus()] The device is not operational due to a hardware error .");    
                                                                  BNA_Reset();
                                                                  return 0;
                                                                  break; 

        
				//6175   A person is preventing proper device operation. 
				case XFS_S_CDR_DS_USER_ERROR: writeFileLog("[BNA_GetStatus()] A person is preventing proper device operation .");    
                                                              return 0;
                                                              break; 

        
				//6179  The device is not operational. A reset command is needed to try to make it operational. 
				case XFS_S_CDR_DS_OFF_LINE: writeFileLog("[BNA_GetStatus()] The device is not operational. A reset command is needed to try to make it operational .");    
                                                            BNA_Reset();            
                                                            break;


				//6180    The device is present and operational. 
				case XFS_S_CDR_DS_ON_LINE: writeFileLog("[BNA_GetStatus()] The device is present and operational .");    
                                                           return 1;               
                                                           break;


				//6181  All dispenser logical cash units are ok. 
				case XFS_S_CDR_DIS_OK: writeFileLog("[BNA_GetStatus()] All dispenser logical cash units are ok .");    
                                                       break;   

              
				//6182  One of the dispenser logical cash units present is in an abnormal state. 
				case XFS_S_CDR_DIS_CU_STATE:  writeFileLog("[BNA_GetStatus()] One of the dispenser logical cash units present is in an abnormal state .");    
                                                              return 0;   
                                                              break;   

           
				//6183   Due to a cash unit failure dispensing is impossible. 
				case XFS_S_CDR_DIS_CU_STOP:   writeFileLog("[BNA_GetStatus()] Due to a cash unit failure dispensing is impossible.  .");    
                                                              return 0; 
                                                              break;   

              
				//6184 Due to a hardware error or other condition, the state of the cash units cannot be determined. 
				case XFS_S_CDR_DIS_CU_UNKNOWN : writeFileLog("[BNA_GetStatus()] Due to a hardware error or other condition, the state of the cash units cannot be determined  .");    
                                                                return 0; 
                                                                break; 

                                          
				//6185  The intermediate stacker is empty.
				case XFS_S_CDR_IS_EMPTY    :   writeFileLog("[BNA_GetStatus()] The intermediate stacker is empty  ."); 
                                                               return 0;                  
                                                               break;


				//6186  The intermediate stacker is not empty.
				case XFS_S_CDR_IS_NOT_EMPTY:writeFileLog("[BNA_GetStatus()] The intermediate stacker is not empty."); 
                                                            return 0;
                                                            break;   

                 
				//6187 @deprecated No more used.
				case XFS_S_CDR_IS_UNKNOWN : 
                                                            return 0;   
                                                            break;   

           
				//6192 The cash was taken by the user. 
				case XFS_S_CDR_CASH_TAKEN  : writeFileLog("[BNA_GetStatus()] The cash was taken by the user."); 
                                                             return 0; 
                                                             break;  

               
				//6194 The cash module lock (safe door) is open. 
				case XFS_S_CDR_SD_OPEN :    writeFileLog("[BNA_GetStatus()] The cash module lock (safe door) is open ."); 
                                                            return 0;
                                                            break;    
               
				//6196 The cash module lock (safe door) is closed and locked. 
				case XFS_S_CDR_SD_LOCKED :   writeFileLog("[BNA_GetStatus()] The cash module lock (safe door) is closed and locked ."); 
                                                             return 0; 
                                                             break; 
                 
				//6197 
				case XFS_S_CDR_SD_UNKNOWN :   writeFileLog("[BNA_GetStatus()] XFS_S_CDR_SD_UNKNOWN  ."); 
                                                              return 0;
                                                              break;                                      


                         };

                         /////////////////////////////////////////////////////////////////////////////////////////////////////////

			 


		}
		else
		{
                        writeFileLog("[BNA_GetStatus()] bnr_GetStatus() failed to execute.");
		        return (-1);

		}



 
}//BNA_GetStatus() end






int EnableDenom(NOTE UserdefEnablebyte)
{
                

                MeiDenom MeiDenomvar={0,0,0,0,0,0,0};

                if( 1 == UserdefEnablebyte.INR_5 )
                {
                     MeiDenomvar.INR5=1; 
                     #ifdef BNA_DEBUG
                     writeFileLog("[ EnableDenom() ] Rs.5  is enable.");
                     #endif
                
                }

                if( 1 == UserdefEnablebyte.INR_10 )
	        {

		     MeiDenomvar.INR10=1; 
                     #ifdef BNA_DEBUG
                     writeFileLog("[ EnableDenom() ] Rs.10  is enable.");
                     #endif
                }

                if( 1 == UserdefEnablebyte.INR_20 )                {

		     MeiDenomvar.INR20=1; 
                     #ifdef BNA_DEBUG
                     writeFileLog("[ EnableDenom() ] Rs.20  is enable.");
                     #endif

	        }

                if( 1 == UserdefEnablebyte.INR_50 )
	        {
                     MeiDenomvar.INR50=1; 
                     #ifdef BNA_DEBUG
                     writeFileLog("[ EnableDenom() ] Rs.50  is enable.");
                     #endif
	        }

                if( 1 == UserdefEnablebyte.INR_100 )
                {

		    MeiDenomvar.INR100=1; 
                    #ifdef BNA_DEBUG
                    writeFileLog("[ EnableDenom() ] Rs.100  is enable."); 
                    #endif

	        }

                if( 1 == UserdefEnablebyte.INR_500 )
	        {

		    MeiDenomvar.INR500=1; 
                    #ifdef BNA_DEBUG
                    writeFileLog("[ EnableDenom() ] Rs.500  is enable.");
                    #endif

	        }

                if( 1 == UserdefEnablebyte.INR_1000 )
                {

		    MeiDenomvar.INR1000=1; 
                    #ifdef BNA_DEBUG
                    writeFileLog("[ EnableDenom() ] Rs.1000  is enable.");
                    #endif

                }

                #ifdef BNA_DEBUG
                writeFileLog("[ EnableDenom() ] Going To update denomation list in bnr.");
                #endif

                return(BNA_EnableDenom(MeiDenomvar));
             



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int BNA_CopyGlobalDenomTable()
{
               
               

	       T_BnrXfsResult resultcode =0;

	       g_denominationList.maxSize=20;

	       unsigned char log[100];

	       memset(log,'\0',100);

	       resultcode = bnr_QueryDenominations(&g_denominationList);

	       if ( BXR_NO_ERROR == resultcode ) 
	       {

                    writeFileLog("[BNA_CopyGlobalDenomTable()] BNA6F Prototype  Get Current denomation status get success.");

                    return 1;
		    
	       }
	       else
	       {
		    
                    writeFileLog("[BNA_CopyGlobalDenomTable()] BNA6F Prototype Get Current denomation status get failed.");
		    
		    return 0;

	       }
	       
	      



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int BNA_EnableDenom( MeiDenom MeiDenomvar )
{

	  
		    
	       //by default all enable are true
	       T_DenominationList denominationList={0};
	       T_BnrXfsResult resultcode =0;
	       denominationList.maxSize=20;
	       unsigned char log[100];
	       memset(log,'\0',100);

	       memset(log,'\0',100);

	       sprintf(log,"[BNA_EnableDenom()] INR5 : %d INR10 :%d INR20 :%d INR50 :%d INR100 :%d INR500:%d INR1000:%d ",MeiDenomvar.INR5,
	       MeiDenomvar.INR10,
	       MeiDenomvar.INR20,
	       MeiDenomvar.INR50,
	       MeiDenomvar.INR100,
	       MeiDenomvar.INR500,
	       MeiDenomvar.INR1000);

	       writeFileLog(log);

	       resultcode = bnr_QueryDenominations(&denominationList);

	       if ( BXR_NO_ERROR == resultcode ) 
	       {

		    #ifdef BNA_DEBUG
                    writeFileLog("[BNA_EnableDenom()] BNA6F Prototype  Get Current denomation status get success");
                    #endif
		    
	       }
	       else
	       {
		    

		    writeFileLog("[BNA_EnableDenom()] BNA6F Prototype Get Current denomation status get failed");
		    
		    return (-1);

	       }
	       
	      
               #ifdef BNA_DEBUG

	       memset(log,'\0',100);

	       sprintf(log,"[BNA_EnableDenom()] denominationList array size = %d",denominationList.size);

	       writeFileLog(log);

               #endif

	       int counter=0;

	       for(counter=0;counter<denominationList.size;counter++)
	       {
			    
                              #ifdef BNA_DEBUG

		              memset(log,'\0',100);
		              sprintf(log,"[BNA_EnableDenom()] denominationList.items[%d].cashType.currencyCode = %s",counter,denominationList.items[counter].cashType.currencyCode);
		              writeFileLog(log);

		              memset(log,'\0',100);
		              sprintf(log,"[BNA_EnableDenom()] denominationList.items[%d].cashType.value = %d",counter,(denominationList.items[counter].cashType.value/100) );
		              writeFileLog(log);

			   
			     memset(log,'\0',100);
		             sprintf(log,"[BNA_EnableDenom()] denominationList.items[%d].cashType.variant = %d",counter,denominationList.items[counter].cashType.variant );
		             writeFileLog(log);

			     memset(log,'\0',100);
		             sprintf(log,"[BNA_EnableDenom()] denominationList.items[%d].enableDenomination = %d",counter,denominationList.items[counter].enableDenomination );
		             writeFileLog(log);

			     memset(log,'\0',100);
		             sprintf(log,"[BNA_EnableDenom()] denominationList.items[%d].securityLevel = %d",denominationList.items[counter].securityLevel);
		             writeFileLog(log);

                             #endif

		             switch( denominationList.items[counter].cashType.value/100 )
		             {

					     case 5:
					     
						   if( 1 == MeiDenomvar.INR5)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.5 Enable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=true;

						   }
						   else if( 0 == MeiDenomvar.INR5)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.5 Disable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=false;

						   }

						   break;

					     case 10:
					     
						   if( 1 == MeiDenomvar.INR10)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.10 Enable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=true;

						   }
						   else  if( 0 == MeiDenomvar.INR10)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.10 Disable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=false;

						   }

						   break;

					     case 20:
					     
						   if( 1 == MeiDenomvar.INR20)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.20 Enable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=true;

						   }
						   else  if( 0 == MeiDenomvar.INR20)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.20 Disable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=false;

						   }

						   break;

					     case 50:
					     
						   if( 1 == MeiDenomvar.INR50)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.50 Enable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=true;

						   }
						   else if( 0 == MeiDenomvar.INR50)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.50 Disable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=false;

						   }

						   break;

					     case 100:
					     
						   if( 1 == MeiDenomvar.INR100)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.100 Enable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=true;

						   }
						   else if( 0 == MeiDenomvar.INR100)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.100 Disable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=false;

						   }

						   break;

					     case 500:
					     
						   if( 1 == MeiDenomvar.INR500)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.500 Enable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=true;

						   }
						   else if( 0 == MeiDenomvar.INR500)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.500 Disable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=false;

						   }

						   break;

					    case 1000:
					     
						   if( 1 == MeiDenomvar.INR1000)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.1000 Enable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=true;

						   }
						   else if( 0 == MeiDenomvar.INR1000)
						   {
                                                      //#ifdef BNA_DEBUG
						      writeFileLog("[BNA_EnableDenom()] Rs.1000 Disable.");
                                                      //#endif
						      denominationList.items[counter].enableDenomination=false;

						   }

						   break;

				          default: writeFileLog("[BNA_EnableDenom()] No value match.");
				                   break;

		              };

	       }
	       
	       
               T_BnrXfsResult returncode=-1;

               //issue denomonation update command
               returncode=bnr_UpdateDenominations(&denominationList);
               #ifdef BNA_DEBUG
	       memset(log,'\0',100);
	       sprintf(log,"[BNA_EnableDenom()] Denomination update return code: %d " ,returncode);
	       writeFileLog(log);
               #endif

               switch(returncode)
               {

                     case BXR_NO_ERROR :  writeFileLog("[BNA_EnableDenom()] Successfully executed enable command.");
                                          break;

                     case XFS_E_ILLEGAL : writeFileLog("[BNA_EnableDenom()] A dispense command is already active on the BNR.");
                                          break;

                     case XFS_E_NOT_SUPPORTED : writeFileLog("[BNA_EnableDenom()] operation not supported by the BNR firmware version.");
                                                break;

                     case XFS_E_PARAMETER_INVALID: writeFileLog("[BNA_EnableDenom()] Invalid array size. The array size is bigger than expected");
                                                   break;
 
                     case XFS_E_CDR_CASHIN_ACTIVE: writeFileLog("[BNA_EnableDenom()] A cashIn command has been issued and is already active");
                                                    break;

                     case XFS_E_FAILURE: writeFileLog("[BNA_EnableDenom()] a command is already running on the BNR or an internal error occured");
                                         break;

               
               };

               if(BXR_NO_ERROR==returncode)
               {
	             return 1;

               }
               else
               {
                    return 0;

               }



}//BNA_EnableDenom end



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BNA_ListCashUnit()
{


	       
	       //by default all enable are true
	       T_XfsCashUnit  cashUnit={0};
	       T_BnrXfsResult resultcode =0;
	       unsigned char log[100];
	       memset(log,'\0',100);
	       cashUnit.physicalCashUnitList.maxSize=20;
	       cashUnit.logicalCashUnitList.maxSize=20;
	       resultcode = bnr_QueryCashUnit(&cashUnit);
	       if ( BXR_NO_ERROR == resultcode ) 
	       {

		      writeFileLog("[BNA_ListCashUnit()] BNA6F Prototype  Get QueryCashUnit status get success.");

	       }
	       else
	       {

		       writeFileLog("[BNA_ListCashUnit()] BNA6F Prototype Get QueryCashUnit status get failed.");

		       return (-1);

	       }
	       
	       #ifdef BNA_DEBUG
	       memset(log,'\0',100);
	       sprintf(log,"[BNA_ListCashUnit()] Number of bills in the transport system = %d",cashUnit.transportCount);
	       writeFileLog(log);

		
	       memset(log,'\0',100);
	       sprintf(log,"[BNA_ListCashUnit()] Maximum number of T_XfsPhysicalCashUnit items that can be stored in the array = %d",cashUnit.physicalCashUnitList.maxSize);
	       writeFileLog(log);


	       memset(log,'\0',100);
	       sprintf(log,"[BNA_ListCashUnit()] Maximum number of T_LogicalCashUnitList items that can be stored in the array = %d",cashUnit.logicalCashUnitList.maxSize);
	       writeFileLog(log);

	       memset(log,'\0',100);
	       sprintf(log,"[BNA_ListCashUnit()] Maximum number of T_XfsPhysicalCashUnit items  = %d",cashUnit.physicalCashUnitList.size);
	       writeFileLog(log);

	       memset(log,'\0',100);
	       sprintf(log,"[BNA_ListCashUnit()] Maximum number of T_LogicalCashUnitList items  = %d",cashUnit.logicalCashUnitList.size);
	       writeFileLog(log);

               #endif

	       int counter=0;

               #ifdef BNA_DEBUG

               //Physical unit
	       for(counter=0;counter<cashUnit.physicalCashUnitList.size;counter++)
	       {
			   
                                     
				     memset(log,'\0',100);

				     sprintf(log,"[BNA_ListCashUnit()] Physical Unit Name = %s.",cashUnit.physicalCashUnitList.items[counter].name);
				     writeFileLog(log);

				     memset(log,'\0',100);

				     sprintf(log,"[BNA_ListCashUnit()] Unit id = %s.",cashUnit.physicalCashUnitList.items[counter].unitId);
				     writeFileLog(log);


				     memset(log,'\0',100);

				     sprintf(log,"[BNA_ListCashUnit()] Numbers of Bills = %d.",cashUnit.physicalCashUnitList.items[counter].count);
				     writeFileLog(log);

				     memset(log,'\0',100);

				     sprintf(log,"[BNA_ListCashUnit()] Bill Threshold Value full= %d",cashUnit.physicalCashUnitList.items[counter].threshold.full);

				     writeFileLog(log);


				     memset(log,'\0',100);

				     sprintf(log,"[BNA_ListCashUnit()] Bill Threshold Value high= %d.",cashUnit.physicalCashUnitList.items[counter].threshold.high);

				     writeFileLog(log);

				     memset(log,'\0',100);
				     sprintf(log,"[BNA_ListCashUnit()] Cashunit Status= %d.",cashUnit.physicalCashUnitList.items[counter].status);
				     writeFileLog(log);
	

				     memset(log,'\0',100);

				     sprintf(log,"[BNA_ListCashUnit()] Thresholdstatus of the Cash unit= %d.",cashUnit.physicalCashUnitList.items[counter].thresholdStatus);

				     writeFileLog(log);
			     
	 
		            

		  }

                  //Logical Unit
                  for(counter=0;counter<cashUnit.logicalCashUnitList.size;counter++)
	          {
			   
                                     writeFileLog("\n\n");
				     memset(log,'\0',100);
				     sprintf(log,"[BNA_ListCashUnit()] Logical Unit Cashtype = %d.",cashUnit.logicalCashUnitList.items[counter].cashType);
				     writeFileLog(log);

				     memset(log,'\0',100);
				     sprintf(log,"[BNA_ListCashUnit()] Logical Unit number = %d.",cashUnit.logicalCashUnitList.items[counter].number);
				     writeFileLog(log);


				     memset(log,'\0',100);
				     sprintf(log,"[BNA_ListCashUnit()] Logical Unit Type = %d.",cashUnit.logicalCashUnitList.items[counter].cuKind);
				     writeFileLog(log);

				     memset(log,'\0',100);
				     sprintf(log,"[BNA_ListCashUnit()] Logical Unit initial count= %d.",cashUnit.logicalCashUnitList.items[counter].initialCount);
				     writeFileLog(log);


				     memset(log,'\0',100);
				     sprintf(log,"[BNA_ListCashUnit()] Logical Unit count =%d.",cashUnit.logicalCashUnitList.items[counter].count);
				     writeFileLog(log);

				     memset(log,'\0',100);

				     sprintf(log,"[BNA_ListCashUnit()] Logical Unit  Status= %d.",cashUnit.logicalCashUnitList.items[counter].status);
				     writeFileLog(log);


		   }
	       
                  #endif

                  return cashUnit.physicalCashUnitList.items[0].count;
		 




}//BNA_ListCashUnit() end here


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef BNA_MAIN

int main()
{

    
    unsigned char log[100];

    memset(log,'\0',100);
    
    BNA_Activate();


    BNA_GetStatus();

    MeiDenom MeiDenomvar={1,1,1,1,1,1,1};

    //MeiDenom MeiDenomvar={0,0,1,0,0,0,0};

    BNA_EnableDenom(MeiDenomvar);

    //BNA_ListCashUnit();

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    g_OperationCompleteflag =false;

    writeFileLog("[main()]Going To Enable cashin operation.");

    T_BnrXfsResult result=-1;

    result=bnr_CashInStart();

    if(result>0)
    {

               writeFileLog("[main()]CashIn operation started successfully.");

    }
    else
    {

	       writeFileLog("[main()]CashIn operation failed.");
	       memset(log,'\0',100);
	       sprintf(log,"[main()] CashIn operation return code = %ld",result);
	       writeFileLog(log);
				     

    }

    int amount=100;
    
    g_OperationCompleteflag =false;

    bnr_CashIn(NULL,"INR");

    while(1)
    {

          bnr_CashIn(NULL,"INR");

          if(true == g_OperationCompleteflag)
          {
              
              //writeFileLog("[main()] CashIn operation complete.");
               
          }



    }

    

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    writeFileLog("[main()]Going To disable cashin operation.");
    bnr_Cancel();
    bnr_CashInEnd();
    bnr_Close();
    
    return 0;

}

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*

int GetNoteDetailInEscrow()
{
          

          unsigned int Current_state=0x00;

          unsigned int Current_Accepted_Denom=0x00;

   
          //make a log about to be accept fare
          char log[200];
          memset(log,'\0',200);

          #ifdef BNA_DEBUG
          sprintf(log,"[GetNoteDetailInEscrow()] Fare to be accept Rs. : %d ", g_faretobeaccept);
	  writeFileLog(log);
          #endif
          
         
          

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
                #ifdef BNA_DEBUG
                writeFileLog("[GetNoteDetailInEscrow()] Single Note Event Found.");  
                #endif
         }
         else
         {
                
                g_SingleNoteAcceptEvent=false;
                #ifdef BNA_DEBUG
                writeFileLog("[GetNoteDetailInEscrow()] Single Note Event Not Found."); 
                #endif

         }
       
         

         //Get State
         //Current_state=GetCurrent_BNA_State();
         


         /*
                     
		                         10   = 5,10
		                         20   = 5,10,20
		                         50   = 5,10,20,50
		                         100  = 5,10,20,50,100
		                         500  = 5,10,20,50,100,500
		                         1000 = 5,10,20,50,100,500,1000

                     
         
                        

                       //if note already in processing disable coin acceptor (no need to accept coin as single note event)
                       if( ( BNA_ACCEPTING == Current_state)       && 
                           ( false == g_AlreadyCoinDisable)        && 
                           ( true  == g_SingleNoteAcceptEvent)     && 
                           ( 0     == g_CoinDisableflag )
                        )
                       {
		                    
                                    #ifdef BNA_DEBUG
                                    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] Accepting State.");
                                    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] Start Coin Inhibit block.");
                                    #endif

                                    #ifdef COIN_ACCEPTOR
		                    //Lock coin acceptor                            
		                    //SetAllCoinInhibitStatus();
		                    //g_AlreadyCoinDisable=true;
                                    #endif

                                    #ifdef BNA_DEBUG

				    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] End Coin Inhibit block.");                          
                                    #endif

                                    return 0;

                  
                       } 


                     //if any below state found Again Enable Coin Acceptor
                     else if( (BNA_GENERIC_FAIL               == Current_state)  ||
                              (BNA_JAMMING                    == Current_state)  ||
	                      (BNA_DISABLED_WITHOUT_ESCROW    == Current_state)  ||
                              (BNA_REJECTING                  == Current_state)  
                       )
                       {
		               

		              if(  (true == g_AlreadyCoinDisable   )  && 
                                   (true == g_SingleNoteAcceptEvent)  && 
                                   ( 0   == g_CoinDisableflag      )  
                                )
		              {

		                     #ifdef BNA_DEBUG
                                     writeFileLog("[GetNoteDetailInEscrow() while loop] Start Release Coin Acceptor block.");
                                     #endif

                                     #ifdef COIN_ACCEPTOR
		                     //NaCoinEnableSet(g_faretobeaccept);
		                     //g_AlreadyCoinDisable=false;
                                     #endif

                                     #ifdef BNA_DEBUG
		                     writeFileLog("[GetNoteDetailInEscrow() while loop] End Release Coin Acceptor block.");
                                     #endif

		              }    
                            
                              return 0;
                          
                    }

                    

          


          //check note stacked or not stacked
	  if( 0x0f == Current_state ) 
          { 
                     
                     #ifdef BNA_DEBUG
                     writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Note Stacked.");  
                     #endif

                     //get current accepted denom        
                     Current_Accepted_Denom=GetCurrent_BNA_Accepted_Denom();

                     //make a log about accepted money
                     #ifdef BNA_DEBUG
                     memset(log,'\0',200);
                     sprintf(log,"[GetNoteDetailInEscrow() if block] Accepted Note Rs. : %d ", Current_Accepted_Denom);
	             writeFileLog(log);
                     #endif
                   
                     

                     //if accept money is remaining balance then no need to enable coin so return with success
                     if(  (Current_Accepted_Denom == g_faretobeaccept )        && 
                          (true == g_SingleNoteAcceptEvent)                    && 
                          (true == g_AlreadyCoinDisable)                       && 
                          ( 0   == g_CoinDisableflag )
                      )
                     {
		               
                               
                                #ifdef BNA_DEBUG

                                writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Start Accepted Note Amount is equal to fare to be accepted block.");
                                #endif

                                #ifdef COIN_ACCEPTOR
		                //g_AlreadyCoinDisable=false;
                                #endif

                                #ifdef BNA_DEBUG

                                writeFileLog("[GetNoteDetailInEscrow() Stacked if block] End Accepted Note Amount is equal to fare to be accepted block.."); 
                                #endif

                                return Current_Accepted_Denom ; //success

                                     
                     }
                     
                     //release coin acceptor if accepted note amount less than fare to be accept
                     else if( (g_faretobeaccept > Current_Accepted_Denom)           && 
                              (true == g_SingleNoteAcceptEvent)                     && 
                              (true == g_AlreadyCoinDisable)                        && 
                              ( 0   == g_CoinDisableflag )
                            )
                     {
                          
                           
                           #ifdef BNA_DEBUG
                           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Start Free Coin Poll flag block.");
                           #endif
                              
                           #ifdef COIN_ACCEPTOR
                           //NaCoinEnableSet(g_faretobeaccept- Current_Accepted_Denom); 
                           //g_AlreadyCoinDisable=false; 
                           #endif

                           #ifdef BNA_DEBUG
                           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] End Free Coin Poll flag block.");
                           #endif

                           return Current_Accepted_Denom ; //success


                     }
                     else 
                     {
                           
                           #ifdef BNA_DEBUG
                           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Normal path Now time to return from here.");
                           #endif
	                   return Current_Accepted_Denom ; //suceess

                     }
            
                                       
                     


	  }    
          else //by default return zero
          {

                   return 0;

          }
          


}

*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif

