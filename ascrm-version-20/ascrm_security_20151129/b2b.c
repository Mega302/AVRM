/*
   Note Acceptor Code (b2b)
*/


#include "b2b.h"


#ifdef    B2B_NOTE_ACCEPTOR

 
 //Global Variables List

 int    g_poll_flag; 
 char  *g_str_status;
 int    g_acptrPortHandler;
 int    g_amnt; 
 int    g_rqstFrmUperLevelSftr; 
 static volatile int g_state;
 int g_portNmbr;
 int g_amnt;  
 int g_amnt_TEST;
 unsigned char g_rcvPkt[MAX_LEN_OF_PKT];
 int  g_rcvPktLen;
 int g_noteAcptr_pktTrnsmisn_sqnce = COMPLETED;
 int g_inhibit_flag;
 int g_isAcptrFault;
 int g_noReplyCounter; 
 int g_comunicasn_err_ocrd; 
 int g_already_log_flag = NO;
 //Malay Add 2 feb 2013
 static unsigned int g_SpecificNoteInhibitFlag;
 static unsigned int g_NACurrentAtvmFare;
 static unsigned int g_NoteLowLevelPollFlagStatus=0;
 static unsigned int g_CloseNotePollThreadFlag=0;
 //malay add 12 april 2013
 static unsigned int g_UnloadQuanity=0;
 static unsigned int g_DispenseQuanity=0;
 static int g_faretobeaccept=0;
 static bool g_SingleNoteAcceptEvent=false;
 static bool g_AlreadyCoinDisable=false;
 static bool g_AlreadyNotePollFlag=false;
 static pthread_mutex_t g_NASpecificInhibitmutex = PTHREAD_MUTEX_INITIALIZER;
 static pthread_mutex_t g_NASetSpecificFaremutex = PTHREAD_MUTEX_INITIALIZER;
 static pthread_mutex_t g_NASetPollFlag = PTHREAD_MUTEX_INITIALIZER;
 static volatile unsigned int g_CoinDisableflag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static volatile bool g_stopThread;

pthread_mutex_t g_NAEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  g_NAEnableThreadCond = PTHREAD_COND_INITIALIZER;

static  pthread_mutex_t g_NoteLowLevelPollFlagStatusmutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_stopThreadmutex=PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_CloseNotePollThreadFlagmutex=PTHREAD_MUTEX_INITIALIZER;

static bool g_NADisableAcceptanceFlag;

static pthread_mutex_t g_NADisableAcceptanceFlagMutex= PTHREAD_MUTEX_INITIALIZER;

static bool g_NA_NoteAcceptanceFlag;

static pthread_mutex_t g_NA_NoteAcceptanceFlagMutex= PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Call this function to set thread close status
inline void  CloseNoteAcceptorPollThread()
{

   pthread_mutex_lock(&g_CloseNotePollThreadFlagmutex);

   g_CloseNotePollThreadFlag=1;

   pthread_mutex_unlock(&g_CloseNotePollThreadFlagmutex);



}

//Created By Malay 3 March 2013
/*
 2= already closed
 3= failed to disable b2b
 1= successfully closed
 0= failed to closed note acptr 
*/
unsigned int DeactivateB2BUnit()
{


	   //check port already closed or not
           if(g_acptrPortHandler<=0)
           {
                  writeFileLog("[DeactivateB2BUnit()] Already B2B com port closed.");
                  return 2;

           }

           //Disable all bill acceptance before close operation note acceptor
	   int state=-1;

	   state=IssuedisableAllBillCmd();

	   if(state!=CASHCODE_RES_DISABLED)
           {

                 writeFileLog("[DeactivateB2BUnit()] B2B Disabled failed.");

	   }
           else
           {

                 writeFileLog("[DeactivateB2BUnit()] B2B Disabled successfully.");

           }

	   //Wait for thread exit
	   delay_mSec(200); 

	   //Close Note Acceptor Serial port
	   int rtcode=-1;
	   rtcode=ClosePort(g_acptrPortHandler);  
	   if(SUCCESS==rtcode)
	   {

		      writeFileLog("[DeactivateB2BUnit()]  B2B com port closed successfully.");
		      g_acptrPortHandler=-1;
		      return 1;

	   }
	   else
           {

		      writeFileLog("[DeactivateB2BUnit()]  B2B com port closed failed.");
		      return 0;

           }


}




//wait for note acceptor low level poll thread closed status
unsigned int WaitforNoteAcptrLowlevelflagClosed(double waittime)
{


	 
	 time_t start,end,diff;
	 int rtcode=-1;
         unsigned char log[100];
         memset(log,'\0',100); 
	 
         #ifdef B2B_LOGLEVEL_ONE 
         writeFileLog("[WaitforNoteAcptrLowlevelflagClosed()]  Get Low level flag status starting..");
         #endif
	 
         pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

	 //if polling flag is on condition (on=1)
	 if(1==g_NoteLowLevelPollFlagStatus)
	 {

                   pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
	  
		   time(&start);  
		 
		   //Wait here untill polling flag is off (off=0)
		   for(;;)
		   {

			     time(&end); 
			     diff=end-start;
			     
			     //if timer is running
			     if(diff<waittime)
			     {

                                        pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

					if(0==g_NoteLowLevelPollFlagStatus) 
					{
                                             pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

					     #ifdef B2B_LOGLEVEL_ONE
                                             memset(log,'\0',100); 
                                             sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Already Low level flag status is closed in %d seconds.",diff);
					     writeFileLog(log);
                                             #endif

                                             return 1;
					}
					else
                                        {
                                             pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
                                             continue;
                                        }

                                        
			     }
			     //if timer time is end or timer is running
			     if(diff>=waittime)
			     {

                                        pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

					if( 0==g_NoteLowLevelPollFlagStatus)
					{
					  #ifdef B2B_LOGLEVEL_ONE
                                          memset(log,'\0',100);
                                          sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Low level flag status is closed %d seconds.",diff);
                                          writeFileLog(log);
                                          #endif	
				  
                                          rtcode=1;
					}
					else
					{ 
					   #ifdef B2B_LOGLEVEL_ONE
                                           memset(log,'\0',100);
                                           sprintf(log,"[WaitforNoteAcptrLowlevelflagClosed()] Low level flag status is not closed during time delay wait. %d .",diff);
                                           writeFileLog(log);
                                           #endif
					   rtcode=0;

					}

                                        pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

					return rtcode;

			     }//timer if block end here
			      

		   }//for loop end
	   
	 }//if thread status start end here
	 
	 else if(0==g_NoteLowLevelPollFlagStatus)
	 {
              pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
	      #ifdef B2B_LOGLEVEL_ONE
              writeFileLog(" [WaitforNoteAcptrLowlevelflagClosed()]  Already Low level flag status is closed.");
              #endif
	      return 1;

	 }

}



int enableSpecificBillsAndMakeIdle(  int fare,                  //API fare (in)
                                     int maxNoteQtyAllowed      //API Max no of cash allow number (in)
                                  )
{
	 
	 unsigned char rcvPkt[30]; 
         memset(rcvPkt,'\0',30);
	 unsigned int  rcvPktLen=0,retry=1;
	 int state=-1;


	 //Disable all notes by default
	 noteType NOTE={0,0,0,0,0,0,0,0};
	 
         //Analysis if exact fare is given
         #ifdef B2B_LOGLEVEL_ONE
         writeFileLog("[enableSpecificBillsAndMakeIdle()] Before Exact fare analysis block.");
         #endif

         //#ifdef B2B_LOGLEVEL_ONE

         //Make a log about fare and max no of cash
         char log[200];
         memset(log,'\0',200);
         sprintf(log,"[enableSpecificBillsAndMakeIdle()] Max No of Cash = %d And fare = %d",maxNoteQtyAllowed,fare);       
         writeFileLog(log);

         //#endif

	 if(1 == maxNoteQtyAllowed)
	 {
		   #ifdef B2B_LOGLEVEL_ONE
                   writeFileLog("[enableSpecificBillsAndMakeIdle()] Max no of cash is one given(Exact fare).");
                   #endif

                   switch(fare)
		   {
			     case 5:
			          NOTE.rs_5    = ENABLE;
                                  #ifdef B2B_LOGLEVEL_ONE
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 5 only enable.");
                                  #endif
			          break;

			     case 10:
                                  
			          NOTE.rs_10   = ENABLE;
                                  #ifdef B2B_LOGLEVEL_ONE
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 10 only enable.");
                                  #endif
			          break;

			     case 20:
			          NOTE.rs_20   = ENABLE;
                                  #ifdef B2B_LOGLEVEL_ONE
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 20 only enable.");
                                  #endif
			          break;

			     case 50:
			          NOTE.rs_50   = ENABLE;
                                  #ifdef B2B_LOGLEVEL_ONE
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 50 only enable.");
                                  #endif
			          break;

			     case 100:
			          NOTE.rs_100  = ENABLE;
                                  #ifdef B2B_LOGLEVEL_ONE
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 100 only enable.");
                                  #endif 
			          break;

			     case 500:
			          NOTE.rs_500  = ENABLE;
                                  #ifdef B2B_LOGLEVEL_ONE
                                  writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 only enable.");
                                  #endif
			          break;

			     case 1000:
			         NOTE.rs_1000 = ENABLE;
                                 #ifdef B2B_LOGLEVEL_ONE
                                 writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 1000 only enable.");
                                 #endif
			         break;

                             default: writeFileLog("[enableSpecificBillsAndMakeIdle()] No fare given.");
                                      break;

	      } 
	   }
           else if( maxNoteQtyAllowed > 1 ) //Exact fare not given
           {
                  #ifdef B2B_LOGLEVEL_ONE
                  writeFileLog("[enableSpecificBillsAndMakeIdle()] No Exact fare given.");
                  #endif

           
                  if(fare >= 5)
                  {
                        NOTE.rs_5      = ENABLE;
                        #ifdef B2B_LOGLEVEL_ONE
                        writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 5 Enable.");
                        #endif
                  }
                  if(fare >= 10)
                  {
                        NOTE.rs_10     = ENABLE;
                        #ifdef B2B_LOGLEVEL_ONE
                        writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 10 Enable.");
                        #endif
                  }
                  if(fare >= 20)
                  {
                       NOTE.rs_20     = ENABLE;
                       #ifdef B2B_LOGLEVEL_ONE
                       writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 20 Enable.");
                       #endif
                  }
                  if(fare >= 50)
                  {
                      NOTE.rs_50     = ENABLE;
                      #ifdef B2B_LOGLEVEL_ONE
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 50 Enable.");
                      #endif
                  }
                  if(fare >= 100)
                  {
                      NOTE.rs_100    = ENABLE;
                      #ifdef B2B_LOGLEVEL_ONE
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 100 Enable.");
                      #endif
                  }
                  if(fare >= 500)
                  {
                      NOTE.rs_500    = ENABLE;
                      #ifdef B2B_LOGLEVEL_ONE
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif
                  }
                  if(fare >= 1000)
                  {
                      NOTE.rs_1000   = ENABLE;
                      #ifdef B2B_LOGLEVEL_ONE
                      writeFileLog("[enableSpecificBillsAndMakeIdle()] Rs. 500 Enable.");
                      #endif
                  }
           }

           #ifdef B2B_LOGLEVEL_ONE
	   writeFileLog("[enableSpecificBillsAndMakeIdle()] After Exact fare analysis block.");
           #endif
        
           //Now Disable Notes
           #ifdef B2B_LOGLEVEL_ONE
           writeFileLog("[enableSpecificBillsAndMakeIdle()] Now going to enable note type.");
           #endif
	   for(retry=1; retry<=1;retry++)
	   {  


			       CASHCODE_NoteAcptr_enableBillTypes(NOTE); 
			       state = -1; 
			       CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);      
			       CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);  
                               //b2b not ack       
			       if( CASHCODE_RES_ACK != state )
			       {
			            //#ifdef B2B_LOGLEVEL_ONE
                                    writeFileLog("[enableSpecificBillsAndMakeIdle()] Bill enable command not ack by b2b.");
                                    //#endif
                                    return FAIL;
                                   
			       }
			       else //if b2b ack bill enable command
			       {
				    //#ifdef B2B_LOGLEVEL_ONE
                                    writeFileLog("[enableSpecificBillsAndMakeIdle()] Bill enable successfully ack by b2b.");
                                    //#endif
                                    return SUCCESS;
			       }


               }  

	       
  
}



#ifdef B2B_ACPTR

int AcceptFromEscrow(char cstNmbr, char qty)
{
        int ret = FAIL;
	cstNmbr = 1;  
        ret = CASHCODE_NoteAcptr_unloadBills(cstNmbr, qty);   
	return(ret);
}

int RejectFromEscrow(char billType,char qty)
{
         int ret = FAIL;
	 billType = 24;    
	 ret = CASHCODE_NoteAcptr_dispenseBills(billType,qty);
	 return(ret);
}

#else
int AcceptFromEscrow()
{
   CASHCODE_NoteAcptr_stack();
}

int RejectFromEscrow()
{
	CASHCODE_NoteAcptr_return();
	delay_mSec(500);   // wait for lst pkt to complete the recv/trnsmt

}
#endif

/////////////////////////////////////////Malay//////////////////////////////////////////////////////////////////////////



//Malay 8 April 2013
static int NoteAcptrStatusLog(int b2bstate)
{
    switch(b2bstate)
    {

          case CASHCODE_RES_POWERUP:
               writeFileLog("[NoteAcptrStatusLog()] B2B POWERUP STATE.");
               break;        
          case CASHCODE_RES_INITIALIZE:
               writeFileLog("[NoteAcptrStatusLog()] B2B INITIALIZE STATE."); 
               break;   	 
	  case CASHCODE_RES_IDLING:
               writeFileLog("[NoteAcptrStatusLog()] B2B IDLING STATE.");         	
	       break;
          case CASHCODE_RES_ACCEPTING:
               writeFileLog("[NoteAcptrStatusLog()] B2B ACCEPTING STATE.");
               break;      	
	  case CASHCODE_RES_STACKING:
               writeFileLog("[NoteAcptrStatusLog()] B2B STACKING STATE.");
               break;    	
	  case CASHCODE_RES_RETURNING:
               writeFileLog("[NoteAcptrStatusLog()] B2B RETURING STATE.");
               break;      
	  case CASHCODE_RES_DISABLED:
               writeFileLog("[NoteAcptrStatusLog()] B2B DISABLE STATE.");
               break;        	 
	  case CASHCODE_RES_HOLDING:
               writeFileLog("[NoteAcptrStatusLog()] B2B HOLDING STATE.");
               break;        	    
	  case CASHCODE_RES_BUSY:
               writeFileLog("[NoteAcptrStatusLog()] B2B BUSY STATE.");
               break;            
	  case CASHCODE_RES_REJECTING:
               writeFileLog("[NoteAcptrStatusLog()] B2B REJECTING STATE.");
               break;     	 
	  case CASHCODE_RES_CSTFULL:
               writeFileLog("[NoteAcptrStatusLog()] B2B CASSETTE FULL STATE.");
               break;           
	  case CASHCODE_RES_CSTREMOVED:
               writeFileLog("[NoteAcptrStatusLog()] B2B CASSETTE REMOVED STATE.");
               break;       
	  case CASHCODE_RES_ACPTR_JAM:
               writeFileLog("[NoteAcptrStatusLog()] B2B ACCEPTOR JAM STATE.");
               break;         
	  case CASHCODE_RES_STKR_JAM:
               writeFileLog("[NoteAcptrStatusLog()] B2B STACKER JAM STATE.");
               break;         
	  case CASHCODE_RES_CHEATED:
               writeFileLog("[NoteAcptrStatusLog()] B2B CHEATED BY USER  STATE.");
               break;          
	  case CASHCODE_RES_ESCROW:
               writeFileLog("[NoteAcptrStatusLog()] B2B ESCROW STATE.");
               break;          
	  case CASHCODE_RES_INVALID_CMD:
               writeFileLog("[NoteAcptrStatusLog()] B2B INVALID COMMAND STATE.");
               break;  	
	  case CASHCODE_RES_STACKED:
               writeFileLog("[NoteAcptrStatusLog()] B2B STACKED STATE.");
               break;           
	  case CASHCODE_RES_RETURNED:
               writeFileLog("[NoteAcptrStatusLog()] B2B RETURNED STATE.");
               break;         
	  case CASHCODE_RES_GENERIC_FAIL:
               writeFileLog("[NoteAcptrStatusLog()] B2B GENERIC FAIL STATE.");
               break;	 
          case CASHCODE_RES_UNLOADING:
               writeFileLog("[NoteAcptrStatusLog()] B2B UNLOADING STATE.");
               break;      	 
	  case CASHCODE_RES_UNLOADED:
               writeFileLog("[NoteAcptrStatusLog()] B2B UNLOADED STATE.");
               break;          
	  case CASHCODE_RES_DISPENSING:
               writeFileLog("[NoteAcptrStatusLog()] B2B DISPENSING STATE.");
               break;     
	  case CASHCODE_RES_DISPESED:
               writeFileLog("[NoteAcptrStatusLog()] B2B DISPENSED STATE.");
               break; 
          
          case 0x28:
               writeFileLog("[NoteAcptrStatusLog()] B2B INVALID BILL NUMBER STATE.");
               break; 

          //ACK
          case 0x00:
               writeFileLog("[NoteAcptrStatusLog()] B2B ACK STATE.");
               break;

          //NAK
          case 0xFF:
               writeFileLog("[NoteAcptrStatusLog()] B2B NAK STATE.");
               break;

          /*
          case 0x30:
               writeFileLog("[NoteAcptrStatusLog()] B2B ILLEGAL COMMAND STATE.");
               break;
          */

           
          case 0x21:
               writeFileLog("[NoteAcptrStatusLog()] B2B SETTING TYPE CASSETTE STATE.");
               break;
          
          
          case 0x29:
               writeFileLog("[NoteAcptrStatusLog()] B2B SET CASSETTE TYPE STATE.");
               break;
          

          default:
                writeFileLog("[NoteAcptrStatusLog()] NO VALID B2B STATE FOUND.");
                unsigned char log[100];
                memset(log,'\0',100);
                sprintf(log,"[NoteAcptrStatusLog()] B2B State : %x",b2bstate);
                writeFileLog(log);
                break; 
    };    	
}



/*
  1=success
  0=fail
  2=port handle is not valid

*/
static int ClearB2BSerialPortbuffer()
{

          int rtcode1=-1,rtcode2=-1;
          //clear serial port transmit and receive buffer
          if(-1!=g_acptrPortHandler)
          {
              rtcode1=ClearTransmitBuffer(g_acptrPortHandler);
              rtcode2=ClearReceiveBuffer (g_acptrPortHandler);
              if( (B2B_SUCCESS == rtcode1) && (B2B_SUCCESS == rtcode2))
                return 1;
              else
                return 0;
          }
          else
              return 2;

}



static int IssueB2BResetCommand()
{

                   //Send reset to b2b
                   unsigned char rcvPkt[30]; 
                   memset(rcvPkt,'\0',30);
                   unsigned int  rcvPktLength=0;
                   CASHCODE_NoteAcptr_reste();
                   //read ack status from b2b unit
                   if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
	           {
			 //Ack
                         if(0x00==rcvPkt[3])
                         {
                            writeFileLog("[IssueB2BResetCommand()] Step 2:Reset command ack by b2b.");
                            return 1;
                         }
                         //nak
                         else if(0xff==rcvPkt[3])
                         {
                             writeFileLog("[IssueB2BResetCommand()] Step 2:Reset command nak by b2b.");
                             return FAIL;
                         }
                         //illegal command
                         else if(0x30==rcvPkt[3])
                         {
                             writeFileLog("[IssueB2BResetCommand()] Step 2:Reset command illegal say by b2b.");
                             return FAIL;
                         }

		   }
                   else
                   {
                          writeFileLog("[IssueB2BResetCommand()] Step 2:Reset command ack status read failed.");
                          return FAIL;
                   }
                   
}




//malay 12 april 2013
int OpenCashAcptr(int portNmbr)
{
	           
                   writeFileLog("[OpenCashAcptr()] Entry.");
		   int i;
		   unsigned char rcvPkt[30]; 
                   memset(rcvPkt,'\0',30); 
		   unsigned int  rcvPktLength=0;
		   int state=-1;
		   int acptrPortRet;
		   g_portNmbr = portNmbr;

                   unsigned int CurrentState=0x00;
                   
                   //malay add for time checking 6 april 2013
                   time_t start,end,diff;
                   //open port for b2b operation 
		   acptrPortRet = OpenPort(g_portNmbr,&g_acptrPortHandler);
                   if(SUCCESS==acptrPortRet)
                   {
                        writeFileLog("[OpenCashAcptr()] openport success.");
                   }
                   else
                   {
                        writeFileLog("[OpenCashAcptr()] Open port failed so reset canbe done now.");
                        return FAIL;
                   }

                   //////////Step 1:Issue poll for check present or not present////////////////////////                   
                   //clear buffer                   
                   ClearB2BSerialPortbuffer();
                   //Send B2B poll command
                   writeFileLog("[OpenCashAcptr()] Step 1 : Send b2b poll command.");
                   CASHCODE_NoteAcptr_sendPollCmd();
                   if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
	           {
			  //send ack to b2b
		          CASHCODE_NoteAcptr_sendACK();
                          writeFileLog("[OpenCashAcptr()] Step 1:Poll command ack by b2b");
		   }
                   else
                   {
                          writeFileLog("[OpenCashAcptr()] Step 1:Poll command not ack by b2b");
                          return FAIL;
                   }


                   ////////////////////////Step 2:Issue Reset Command///////////////////////////////////////////// 

                   //Send reset to b2b
                   delay_mSec(200);
                   memset(rcvPkt,'\0',30);
                   rcvPktLength=0;
                   //issue reset command
                   CASHCODE_NoteAcptr_reste();
                   //read ack status from b2b unit
                   if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
	           {
			 //Ack
                         if(0x00==rcvPkt[3])
                         writeFileLog("[OpenCashAcptr()] Step 2:Reset command ack by b2b.");
                         //nak
                         else if(0xff==rcvPkt[3])
                         {
                             writeFileLog("[OpenCashAcptr()] Step 2:Reset command nak by b2b.");
                             //close port
                             ClosePort(g_acptrPortHandler);
                             return FAIL;
                         }
                         //iillegal command
                         else if(0x30==rcvPkt[3])
                         {
                             writeFileLog("[OpenCashAcptr()] Step 2:Reset command illegal say by b2b.");
                             //close port
                             ClosePort(g_acptrPortHandler);
                             return FAIL;
                         }

		   }
                   else
                   {
                          writeFileLog("[OpenCashAcptr()] Step 2:Reset command ack status read failed.");
                          //close port
                          ClosePort(g_acptrPortHandler);
                          return FAIL;
                   }
                   

                   ////////////////////////Step 3/////////////////////////////////////////////

                   //delay 2 seconds (as b2b didnot reply after accept reset command )
                   Wait(2);
                   //get start time
                   time(&start);  
	           while(1)
		   {      
		                 
                                 delay_mSec(200);
		                 memset(rcvPkt,'\0',30);
                                 rcvPktLength=0;
		                 CASHCODE_NoteAcptr_sendPollCmd();
		                 if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
			         {
				      //send ack to b2b
				      CASHCODE_NoteAcptr_sendACK();
                                      
                                       if( rcvPkt[3] != CurrentState)
                                      {
		                          CurrentState = rcvPkt[3];
                                          writeFileLog("[OpenCashAcptr()] Step 3:Poll command ack by b2b");
                                          NoteAcptrStatusLog(rcvPkt[3]);
                                      }

                                      if(CASHCODE_RES_POWERUP==rcvPkt[3])
                                      {
		                             
                                             int rtcode=0;
                                             delay_mSec(200);
                                             rtcode=IssueB2BResetCommand();
                                             if(1==rtcode)
                                               continue;
                                             else
                                             {
                                                 writeFileLog("[OpenCashAcptr()] Step 3:Reset command not ack by b2b when it in powerup state.");
		                                 //close port
                                                 ClosePort(g_acptrPortHandler);
                                                 return FAIL;
                                             }
                                      }
		                      if(CASHCODE_RES_INITIALIZE==rcvPkt[3])
                                      {
		                             state=rcvPkt[3];
                                             continue;
                                      }
		                      if(CASHCODE_RES_DISABLED==rcvPkt[3])
		                      {
		                           //get end time
				           time(&end);
				           //calculate differnnce
				           diff=end-start;
                                           unsigned char log[100];
                                           memset(log,'\0',100);
                                           sprintf(log,"[OpenCashAcptr()] Step 3:disable state found %d second so reset done.",diff);
	                                   writeFileLog(log);
                                           if( CASHCODE_RES_DISABLED  == rcvPkt[3]) //if disable state found set recycle cassette
                                           {
                                                 writeFileLog("[OpenCashAcptr()] Going to check and set recycle cassette.");
                                                 if( 1 == SetRecycleCassette() )
                                                 {
                                                    writeFileLog("[OpenCashAcptr()] Check and Set recycle cassette success.");
                                                 }
                                                 else
                                                    writeFileLog("[OpenCashAcptr()] Check and Set recycle cassette failed.");
                                           }
                                           return rcvPkt[3];
		                      }
				 }
		                 else
		                 {
		                      writeFileLog("[OpenCashAcptr()] Step 3:Poll command not ack by b2b");
		                      state=-1;
                                      //close port
                                      ClosePort(g_acptrPortHandler);
                                      return FAIL;
		                 }

		                  //get end time
				  time(&end);
				  //calculate differnnce
				  diff=end-start;
                                  //if timeout return with state
				  if(diff>=WAIT_TIME_FOR_RESET) 
				  {
				      writeFileLog("[OpenCashAcptr()] Step 3:Timeout.");  
                                      //close port
                                      ClosePort(g_acptrPortHandler);
				      return state;
				  }
	
		
		   }    
                   
		  
}//int OpenCashAcptr(int portNmbr) end here



static unsigned int SetRecycleCassetteCommandStatus(const unsigned int RecycleCassetteNumber,
                                                   const unsigned int RecycleCassetteState)
{



    unsigned int rtcode=0;
    unsigned char rcvPkt[30];
    memset(rcvPkt,'\0',30);
    unsigned int  rcvPktLen=0;
    CASHCODE_NoteAcptr_SetRecycleCassette( RecycleCassetteNumber,RecycleCassetteState);    
    rtcode=CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
    if(SUCCESS == rtcode)
    {
                 if( 0x06 == rcvPkt[2] ) //reply hole byte length
		 {
			    // rcvPkt[3] reply Data field
                            if(0xFF == rcvPkt[3] ) 
		            {
				   writeFileLog("[SetRecycleCassetteCommandStatus()] NAK Command by b2b.");
		                   return 2; //nak command message
		            }
			    if(0x30 == rcvPkt[3] )
		            {
				   writeFileLog("[SetRecycleCassetteCommandStatus()] Illegal Command by b2b.");
		                   return 3; //illegal command message
		            }
		            if(0x00 == rcvPkt[3] )
                            {
			           writeFileLog("[SetRecycleCassetteCommandStatus()] ACK by b2b.");
                                   return 1; //ack command message
			    }
		 } 
    }
    else
    {
          printf("\n [SetRecycleCassetteCommandStatus()] b2b status read failed.");
          return 0;
    } 

}


static unsigned int SetRecycleCassette()
{
   //Step 1 :  read current recycle cassette status
   //Step 2 :  if any recycle cassette status read error make error about it and return
   //Step 3 :  if status get successfully then check recycle cassette 1,2,3 physically present ,if not return error
   //Step 4 :  if any recycle cassette 1,2,3 status not set according to atvm setting then set it according to api 
   //Step 5 :  if set recycle cassette reuturn error

   unsigned char rcvPkt[30];
   memset(rcvPkt,'\0',30);
   int rtcode=0;  
   unsigned int  rcvPktLen=0;
   bool rtcode1=false,rtcode2=false,rtcode3=false;
   
   if( 1 == WaitforState(CASHCODE_RES_DISABLED,2))
   {    
                                                   
         writeFileLog("[SetRecycleCassette()] Disable state found.");
   }
   else
   {
         writeFileLog("[SetRecycleCassette()] Disable state found failed.");
         return 0; 

   }
   //issue command
   delay_mSec(200); 
   CASHCODE_NoteAcptr_getRecycleCstStatus();
   //read status
   memset(rcvPkt,'\0',30);
   rtcode=CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
   if(SUCCESS == rtcode)
   {
		 if( 0x06 == rcvPkt[2] ) // reply hole byte length
		 {
		    
		            if(0xFF == rcvPkt[3] ) //data byte 
			       return 3; //nak command message

			    if(0x30 == rcvPkt[3] ) //data byte 
			       return 4; //illegal command message
		 } 

		 else if( 11 == rcvPkt[2] ) //original reply get successfully
		 {
		      if( 1== (rcvPkt[3]>>7)) //cassette 1 present 
		      {
		            writeFileLog("[SetRecycleCassette()] cassette 1 present ");

                            if ( 0x18 == (rcvPkt[3] & 0b00011111)) //check it is escrow or not
		            {
		                 //no need to do 
		                 rtcode1=true;
                                 writeFileLog("[SetRecycleCassette()] cassette 1 type is escrow ");
		            }
		            else //if not escrow then set as its escrow
		            {
		                delay_mSec(200); //must
		                rtcode=SetRecycleCassetteCommandStatus( 1,CASHCODE_ESCROW_CASSETTE_TYPE);
                                if(1==rtcode)
                                {
                                      if( 1 == WaitforState(CASHCODE_RES_DISABLED,WAIT_TIME_SET_RECYCLE))
                                      {    
                                           rtcode1=true;
                                           writeFileLog("[SetRecycleCassette()] cassette 1 type escrow execute succssfully.");
                                      }

                                }
                                else
                                {
                                    rtcode1=false;
                                    writeFileLog("[SetRecycleCassette()] cassette 1 type escrow execute failed.");

                                }
		            }
		            
		      }
		      if( 1== (rcvPkt[5]>>7)) //cassette 2 present 
		      {
		            writeFileLog("[SetRecycleCassette()] cassette 2 present ");
                            if ( 0x1f == (rcvPkt[5] & 0b00011111)) //check it is not assigned or not
		            {
		                 //no need to do 
		                 rtcode2=true;
                                 writeFileLog("[SetRecycleCassette()] cassette 2 type is 'not assigned'. ");
		            }
		            else //if not assigned type then set as not assigned type
		            {
		                delay_mSec(200);  //must
		                rtcode=SetRecycleCassetteCommandStatus( 2,CASHCODE_NOT_ASSIGNED_CASSETTE_TYPE);
                                if(1==rtcode)
                                {
                                        if( 1 == WaitforState(CASHCODE_RES_DISABLED,WAIT_TIME_SET_RECYCLE))
                                        {    
                                            rtcode2=true;
                                            writeFileLog("[SetRecycleCassette()] cassette 2 not assigned type execute succssfully.");
                                        }
                                                                                                                                           
                                }
                                else
                                {
                                     rtcode2=false;
                                     writeFileLog("[SetRecycleCassette()] cassette 2 'not assigned' failed.");
                                }
		            }
		      }
		      if( 1== (rcvPkt[7]>>7)) //cassette 3 present 
		      {
		            writeFileLog("[SetRecycleCassette()] cassette 3 present ");
                            if ( 0x1f == (rcvPkt[7] & 0b00011111)) //check it is not assigned or not
		            {
		                //no need to do 
		                rtcode3=true;
                                writeFileLog("[SetRecycleCassette()] cassette 3 type is 'not assigned' .");
		            }
		            else //if not assigned type then set as not assigned type
		            {
		                delay_mSec(200); //must
		                rtcode=SetRecycleCassetteCommandStatus( 3,CASHCODE_NOT_ASSIGNED_CASSETTE_TYPE);
                                if(1==rtcode)
                                {
                                      if( 1 == WaitforState(CASHCODE_RES_DISABLED,WAIT_TIME_SET_RECYCLE))
                                      {    
                                           rtcode3=true;
                                           writeFileLog("[SetRecycleCassette()] cassette 3 not assigned type execute succssfully.");
                                      }
                                }
                                else
                                {
                                   rtcode3=false;
                                   writeFileLog("[SetRecycleCassette()] cassette 3 'not assigned' failed. ");
                                }
		            }
		      }

		      if( (true == rtcode1) &&  (true == rtcode2) &&  (true == rtcode3) )
		         return 1;
		      else 
		         return 0;
		         
		 }
   } 
   else
      return 2;

  
}




/*

   MakeDisableState() return code:
   1 = make disable success
   
   0 = make disable failed
  
   2 = poll commnad failed


*/
static int MakeDisableState()
{
      
      writeFileLog("[MakeDisableState()] Entry.");
      int rtcode =0;
      time_t start,end,diff;
      unsigned char  PollReplyPacket[30];
      memset(PollReplyPacket,'\0',30);
      unsigned int   PollBufferLength=30;
      unsigned int   PollReplyPacketlength=0;
      int State=0;
      //get start time
      time(&start);       
      while(1)
      {
		    rtcode=GetCurrentB2BState(PollReplyPacket,PollBufferLength,&PollReplyPacketlength);
		    //if poll success
		    if(1==rtcode)
		    {
		             if( (CASHCODE_RES_DISPESED   ==   PollReplyPacket[3]) || 
		                 (CASHCODE_RES_UNLOADED   ==   PollReplyPacket[3]) || 
		                 (CASHCODE_RES_UNLOADING  ==   PollReplyPacket[3]) || 
		                 (CASHCODE_RES_DISPENSING ==   PollReplyPacket[3]) 
                               )
		            {
		                 writeFileLog("[MakeDisableState()] Still No Disable State found.");
                                 //Make log about current state
		                 NoteAcptrStatusLog(PollReplyPacket[3]);
		                 State=PollReplyPacket[3];
                                 memset(PollReplyPacket,'\0',30);   
		                 PollReplyPacketlength=0;             
		                
                             }
                           
                           else if(CASHCODE_RES_DISABLED == PollReplyPacket[3])
                           {
                                 //get end time
				 time(&end);
				 //calculate differnnce
				 diff=end-start;
                                 unsigned char log[100];
                                 memset(log,'\0',100);
                                 sprintf(log,"[MakeDisableState()] Disable state found %d second.",diff);
                                 return 1;
                           }
                          
		   }//if block end here
		   else
		   {
			 writeFileLog("[MakeDisableState()] Poll command failed.");
			 return 2;
		   }
		   //get end time
		   time(&end);
                   //calculate differnnce
	           diff=end-start;
		   if(diff>=WAIT_TIME_FOR_MAKE_DISABLE) 
	           {
		        writeFileLog("[MakeDisableState()] Timeout for make disable state for B2B."); 
                        if(CASHCODE_RES_DISABLED == State)
			  return 1;
                        else
                          return 0;
	           }

      }//while loop end

}


//malay 24 july 2013
int InitCashAcptr()
{
	           
                   writeFileLog("[InitCashAcptr()] Entry.");
		   int i;
		   unsigned char rcvPkt[30]; 
                   memset(rcvPkt,'\0',30); 
		   unsigned int  rcvPktLength=0;
		   int state=-1;
		   

                   unsigned int CurrentState=0x00;
                   
                   //malay add for time checking 6 april 2013
                   time_t start,end,diff;
                  
                   //////////Step 1:Issue poll for check present or not present////////////////////////           
        
                   //clear buffer                   
                   ClearB2BSerialPortbuffer();

                   //Send B2B poll command
                   writeFileLog("[InitCashAcptr()] Step 1 : Send b2b poll command.");
                   CASHCODE_NoteAcptr_sendPollCmd();
                   if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
	           {
			//send ack to b2b
		        CASHCODE_NoteAcptr_sendACK();
                        writeFileLog("[InitCashAcptr()] Step 1:Poll command ack by b2b");
		   }
                   else
                   {
                          writeFileLog("[InitCashAcptr()] Step 1:Poll command not ack by b2b");
                          return FAIL;
                   }

                   ////////////////////////Step 2:Issue Reset Command///////////////////////////////////////////// 

                   //Send reset to b2b
                   delay_mSec(200);
                   memset(rcvPkt,'\0',30);
                   rcvPktLength=0;
                   //issue reset command
                   CASHCODE_NoteAcptr_reste();
                   //read ack status from b2b unit
                   if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
	           {
			 //Ack
                         if(0x00==rcvPkt[3])
                         writeFileLog("[InitCashAcptr()] Step 2:Reset command ack by b2b.");
                         //nak
                         else if(0xff==rcvPkt[3])
                         {
                             writeFileLog("[InitCashAcptr()] Step 2:Reset command nak by b2b.");
                             return FAIL;
                         }
                         //iillegal command
                         else if(0x30==rcvPkt[3])
                         {
                             writeFileLog("[InitCashAcptr()] Step 2:Reset command illegal say by b2b.");
                             return FAIL;
                         }

		   }
                   else
                   {
                          writeFileLog("[InitCashAcptr()] Step 2:Reset command ack status read failed.");
                          return FAIL;
                   }
                   

                   ////////////////////////Step 3/////////////////////////////////////////////
                   //delay 2 seconds (as b2b didnot reply after accept reset command )
                   Wait(2);
                   //get start time
                   time(&start);  
	           while(1)
		   {      
		                 
                                 delay_mSec(200);
		                 memset(rcvPkt,'\0',30);
                                 rcvPktLength=0;
		                 CASHCODE_NoteAcptr_sendPollCmd();
		                 if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
			         {
				      //send ack to b2b
				      CASHCODE_NoteAcptr_sendACK();
                                      
                                       if( rcvPkt[3] != CurrentState)
                                      {
		                          CurrentState = rcvPkt[3];
                                          writeFileLog("[InitCashAcptr()] Step 3:Poll command ack by b2b");
                                          NoteAcptrStatusLog(rcvPkt[3]);
                                      }

                                      if(CASHCODE_RES_POWERUP==rcvPkt[3])
                                      {
		                             
                                             int rtcode=0;
                                             delay_mSec(200);
                                             rtcode=IssueB2BResetCommand();
                                             if(1==rtcode)
                                               continue;
                                             else
                                             {
                                                 writeFileLog("[InitCashAcptr()] Step 3:Reset command not ack by b2b when it in powerup state.");
		                                
                                                 return FAIL;
                                             }
                                      }
		                      if(CASHCODE_RES_INITIALIZE==rcvPkt[3])
                                      {
		                             state=rcvPkt[3];
                                             continue;
                                      }
		                      if(CASHCODE_RES_DISABLED==rcvPkt[3])
		                      {
		                           //get end time
				           time(&end);
				           //calculate differnnce
				           diff=end-start;
                                           unsigned char log[100];
                                           memset(log,'\0',100);
                                           sprintf(log,"[InitCashAcptr()] Step 3:disable state found %d second so reset done.",diff);
	                                   writeFileLog(log);
                                           if( CASHCODE_RES_DISABLED  == rcvPkt[3]) //if disable state found set recycle cassette
                                           {
                                                 writeFileLog("[InitCashAcptr()] Going to check and set recycle cassette.");
                                                 if( 1 == SetRecycleCassette() )
                                                 {
                                                    writeFileLog("[InitCashAcptr()] Check and Set recycle cassette success.");
                                                 }
                                                 else
                                                    writeFileLog("[InitCashAcptr()] Check and Set recycle cassette failed.");
                                           }
                                           return SUCCESS;
		                      }
				 }
		                 else
		                 {
		                      writeFileLog("[InitCashAcptr()] Step 3:Poll command not ack by b2b");
		                      state=-1;
                                      return FAIL;
		                 }

		                  //get end time
				  time(&end);
				  //calculate differnnce
				  diff=end-start;
                                  //if timeout return with state
				  if(diff>=WAIT_TIME_FOR_RESET) 
				  {
				      writeFileLog("[InitCashAcptr()] Step 3:Timeout.");  
                                      return FAIL;
				  }
	
		
		   }    
                   
		  
}//int InitCashAcptr() end here




//Malay 8 April 2013
int GetNoteAcptrStatus()
{

          writeFileLog("[GetNoteAcptrStatus()] Entry.");
          unsigned char rcvPkt[30];
          unsigned int rcvPktLength=0; 
          memset(rcvPkt,'\0',30); 
          ClearB2BSerialPortbuffer();
          //delay for smooth operation
          delay_mSec(200); //ccnet recommended 200ms delay for between two command
          //Send B2B poll command
          writeFileLog("[GetNoteAcptrStatus()] Send b2b poll command.");
          CASHCODE_NoteAcptr_sendPollCmd();
          if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
	  {
	        //send ack to b2b
                CASHCODE_NoteAcptr_sendACK();
                 
                //if powerup state found then init b2b
                if(CASHCODE_RES_POWERUP==rcvPkt[3])
                {
		             int rtcode=0;
		             rtcode=InitCashAcptr();

                             //if init fail return zero
		             if(FAIL == rtcode)
		               return 0;

                            memset(rcvPkt,'\0',30); 
                            rcvPktLength=0; 
		            CASHCODE_NoteAcptr_sendPollCmd();
                            if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
                               return (rcvPkt[3]);
                            else
                               return 0;
                }


                //if previously dispensed or unloaded state not ack by b2b unit make disable b2b form this state
                if( (CASHCODE_RES_DISPESED   == rcvPkt[3]) || 
                    (CASHCODE_RES_UNLOADED   == rcvPkt[3]) || 
                    (CASHCODE_RES_UNLOADING  == rcvPkt[3]) || 
                    (CASHCODE_RES_DISPENSING == rcvPkt[3]) )
                {
                      if(CASHCODE_RES_DISPESED == rcvPkt[3])
                        writeFileLog("[GetNoteAcptrStatus()] Previous Dispensed state found.");
                      if(CASHCODE_RES_UNLOADED == rcvPkt[3])
                        writeFileLog("[GetNoteAcptrStatus()] Previous Unloaded state found.");
                      if(CASHCODE_RES_UNLOADING == rcvPkt[3])
                        writeFileLog("[GetNoteAcptrStatus()] Previous Unloading state found.");
                      if(CASHCODE_RES_DISPENSING == rcvPkt[3])
                        writeFileLog("[GetNoteAcptrStatus()] Previous Dispensing state found.");
                      int rtcode=-1;
                      //make disable b2b
                      rtcode=MakeDisableState();
                      if(1==rtcode)
                        return CASHCODE_RES_DISABLED;
                      else
                        return 0;
                       
                }    
                else
                { 
		        
                        //malay 10 april 2013 add 
		        NoteAcptrStatusLog(rcvPkt[3]);
		        writeFileLog("[GetNoteAcptrStatus()] b2b poll conmmand success now time return statusbyte.");
		        return (rcvPkt[3]);
                }
          }
          else
          {
                writeFileLog("[GetNoteAcptrStatus()] b2b poll conmmand status read failed now time return fail status.");
                return 0;
          }
}



inline int SetCurrentfare(int Remainbalance)
{

        g_faretobeaccept=Remainbalance;


}


inline int SetAlreadyCoinInhibit()
{

          g_AlreadyCoinDisable=false;


}



//Get current b2b state
static unsigned int GetCurrentB2BState( unsigned char* const  PollReplyPacket,
                                        unsigned int          PollBufferLength,
                                        unsigned int*  const  PollReplyPacketlength
                                      )
{

                               
                                //poll buffer length must be greater or equal to 20 
                                if( PollBufferLength < 20)
                                {  
                                     writeFileLog("[GetCurrentB2BState()] PollBufferLength must be greater than 20.");
                                     return 2;
                                }
                                *PollReplyPacketlength=0;
                                 int i=0;
                                 delay_mSec(200);
                                 unsigned char rcvPkt[30]; 
                                 memset(rcvPkt,'\0',30);
                                 unsigned int  rcvPktLength=0;
		                 CASHCODE_NoteAcptr_sendPollCmd();
		                 if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
			         {
				      //send ack to b2b
				      CASHCODE_NoteAcptr_sendACK();
		                      writeFileLog("[GetCurrentB2BState()] Poll command ack by B2B.");

                                      //write current status of log
                                      NoteAcptrStatusLog(rcvPkt[3]);

                                      //copy reply buffer array
                                      for(;i<rcvPktLength;i++)
                                      PollReplyPacket[i]=rcvPkt[i];
                                      *PollReplyPacketlength=rcvPktLength;
                                      return 1;
		                     
				 }
		                 else
		                 {
		                      writeFileLog("[GetCurrentB2BState()] Poll command not ack by B2B.");
		                      return 0;
		                 }

}




/*
   1 = Successfully B2B State found
   0 = Failed to get B2B State
   2 = Poll command not ack B2B 
*/
static int WaitforState(const unsigned int State,const unsigned int WaitTime)
{
                   unsigned char rcvPkt[30]; 
                   memset(rcvPkt,'\0',30);
                   unsigned char log[100];
                   memset(log,'\0',100);
                   unsigned int  rcvPktLength=0;

                   unsigned int CurrentState = 0x00;

                   //malay add for time checking 6 april 2013
                   time_t start,end,diff;

                   //get start time
                   time(&start);  

	           while(1)
		   {      
		                 delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND);
		                 memset(rcvPkt,'\0',30);
                                 rcvPktLength=0;
		                 CASHCODE_NoteAcptr_sendPollCmd();
		                 if( SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLength) )
			         {
				      //send ack to b2b
				      CASHCODE_NoteAcptr_sendACK();
                                      
                                      if( rcvPkt[3] != CurrentState )
                                      {
				              CurrentState = rcvPkt[3];
                                              writeFileLog("[WaitforState()] Poll command ack by B2B.");
		                              NoteAcptrStatusLog(rcvPkt[3]);
                                      }

                                      //if given state found
                                      if(State==rcvPkt[3])
                                      {
		                             //get end time
				             time(&end);
				             //calculate differnnce
				             diff=end-start;
                                             sprintf(log,"[WaitforState()] Given B2B State found withinn %d seconds.",diff);
	                                     writeFileLog(log);
                                             return 1;
                                      }
		                     
				 }
		                 else
		                 {
		                      writeFileLog("[WaitforState()] Poll command not ack by B2B.");
		                      return 2;
		                 }

		                  //get end time
				  time(&end);
				  //calculate differnnce
				  diff=end-start;
                                  //if timeout return with state
				  if(diff>=WaitTime) 
				  {
				          writeFileLog("[WaitforState()] Timeout.");
                                          //if given state found
                                          if(State==rcvPkt[3])
                                          {
		                                sprintf(log,"[WaitforState()] Timeout Given B2B State found withinn %d seconds.",diff);
                                                return 1;
                                           }
		                           else
                                           {
                                                 writeFileLog("[WaitforState()] Timeout Given B2B State not found.");
		                                 return 0;
                                           }
                                   }
		                     
                                 
                       }//while loop end

}




//Malay 8 April 2013
int CASHCODE_NoteAcptr_unloadBills(char frmRcyclCstNmbr, char qty)
{

		  unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
                  memset(rcvPkt,'\0',MAX_LEN_OF_PKT); 
		  int  rcvPktLen=0;
		  unsigned char sbytes[20];
                  memset(rcvPkt,'\0',20);
		  unsigned char crc1=0x00, crc2=0x00;
		  unsigned int state=0;
                  unsigned int i=0;
                  char log[100];
                  memset(log,'\0',100);

		  if(qty <= 0)
		  {
		      writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] Quantity not given thats why return from here.");
                      return SUCCESS;
		  }  
                  
                  
                  //Step 1:
                  delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100
                  if(FAIL == IssuedisableAllBillCmd())  
	          { 
		       writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B disable failed.");
                       return FAIL;
	          }
                  else
                  {
                      //store for further check
                      g_UnloadQuanity=qty;
                      writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B disable success.");
                  }
                     
                     //Step 2:
                     //check b2b stateB2BGetExtendedRecycleStatus
                     delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100
                     unsigned char  PollReplyPacket[30];
                     memset(PollReplyPacket,'\0',30); 
                     unsigned int   PollBufferLength=30;
                     unsigned int   PollReplyPacketlength=0;
                     int rtcode=0 ;
                     rtcode=GetCurrentB2BState(PollReplyPacket,PollBufferLength,&PollReplyPacketlength);
                     if(1==rtcode)
                     {
                          NoteAcptrStatusLog(PollReplyPacket[3]);
                          
                          if(CASHCODE_RES_DISABLED != PollReplyPacket[3])
                          {
                                  rtcode=WaitforState(CASHCODE_RES_DISABLED,WAIT_TIME_FOR_GET_DISABLE);

		                  if(1!=rtcode)
		                  {
		                       writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B disable state not found so unload operation can not be done now.");
		                       return FAIL;
		                   }
		                   else
		                     writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B disable state found so unload operation can be done now.");

                          }
                          else
		             writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B disable state found.");

                     }       
                     else 
                     {
                        writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] read b2b current state failed.");
                        return FAIL;
                     }


                  //Step 3:
                  delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100

                  memset(log,'\0',100); 
		  sprintf(log,"[CASHCODE_NoteAcptr_unloadBills()] Unload Qauntity : %d",qty);
		  writeFileLog(log);

		  sbytes[0] = CASHCODE_SYNC;  
		  sbytes[1] = DEVICE_ADDRESS; 
		  sbytes[2] = 0x08;                
		  sbytes[3] = CASHCODE_CMD_UNLOAD; 
		  sbytes[4]  = frmRcyclCstNmbr;
		  sbytes[5]  = qty; 
		  GetCRC16(sbytes,6,&crc1,&crc2);
		  sbytes[6]  = crc1;
		  sbytes[7]  = crc2;
                  
                  
                  //Send unload command
		  for(i=0;i< 8; i++)
		  SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
                  //get unload command reply 
                  rtcode=0;
		  rtcode=CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
                  if(FAIL == rtcode)
                  {
                         writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B unload command rply status byte read failed.");
                         return FAIL;
                  }

		  //CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);	
                  //if(state != CASHCODE_RES_ACK)

                  if(0x00 != rcvPkt[3]) //ack failed to get
		  {
		                   writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B unload command ack failed.");
                                   NoteAcptrStatusLog(rcvPkt[3]);
                                   int i=0;
		                   for(;i<rcvPktLen;i++)
		                   {
		                      memset(log,'\0',100); 
		                      sprintf(log,"[CASHCODE_NoteAcptr_unloadBills()] rcvPkt[%d] : %x",i,rcvPkt[i]);
		                      writeFileLog(log);
		                   }
		                   
				   return FAIL;
		  }
		  else
		  {       
		               NoteAcptrStatusLog(rcvPkt[3]);

                               //Step 4:
                               writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B send ack for unload command.");
                               int rtcode=-1;

                               //wait for unloading state
                               rtcode=WaitforState(CASHCODE_RES_UNLOADING,WAIT_TIME_FOR_UNLOADING);

                               //unloading state found
                               if(1 == rtcode)
                               {
                                    writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B Unloading State found.");
                                    return SUCCESS;
                               }

                               //unloading state not yet started
                               else if(0 == rtcode)
                               {
                                    writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B Unloading State not found.");
                                    return FAIL;
                               }

                               //b2b communication fail during wait for state
                               else if(2 == rtcode)
                               {
                                    writeFileLog("[CASHCODE_NoteAcptr_unloadBills()] B2B Communication failed.");
                                    return FAIL;
                               }

                            
                   }//else block end
                       

}




//Malay 8 April 2013
int GetAcceptedNoteDetail(int *Quantity, int *b2bStatus)
{

	                        writeFileLog("[GetAcceptedNoteDetail()] Entry.");
                               *Quantity   = 0; 
                               *b2bStatus =  0x00;  
	                        g_state    = -1;
                                unsigned char log[100];
                                memset(log,'\0',100);
		               int rtcode=-1;
                               //wait for unloading state
                               rtcode=WaitforState(CASHCODE_RES_UNLOADED,WAIT_TIME_FOR_UNLOADED);
                               if(1 == rtcode)
                               {
                                    writeFileLog("[GetAcceptedNoteDetail()] B2B Unloaded State found.");
                                    int notes=-1;
                                    GetNoteDetailsInRecycleCst(&notes);
                                    //B2BGetExtendedRecycleStatus(&notes,0);
                                    memset(log,'\0',100);
                                    sprintf(log,"[GetAcceptedNoteDetail()] Notes in recycle quntity= %d",notes);
	                            writeFileLog(log); 
                                    //Reset unload quanity
                                    g_UnloadQuanity=0; 
                                    //if(0==notes)   
                                      return SUCCESS;
                                    //else
                                      //return FAIL;
                               }
                               else if(0 == rtcode)
                               {
                                    writeFileLog("[GetAcceptedNoteDetail()] B2B Unloaded State not found.");
                                    return FAIL;
                               }
                               else if(2 == rtcode)
                               {
                                    writeFileLog("[GetAcceptedNoteDetail()] B2B Communication failed.");
                                    return FAIL;
                               }
}




//Malay 8 April 2013
int CASHCODE_NoteAcptr_dispenseBills(char billType, char qty)
{

		     unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
		     memset(rcvPkt,'\0',MAX_LEN_OF_PKT); 
		     int  rcvPktLen=0;
		     unsigned char sbytes[20];
		     memset(sbytes,'\0',20); 
		     unsigned char crc1=0x00, crc2=0x00;
		     int state=0;
                     int i=0;
                     char log[100];
                     memset(log,'\0',100);

		     if(qty <= 0)
		     {
		         writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] Quantity not given thats why return from here");
                         return(SUCCESS);
		     }  
           

                     //Step 1:make disable b2b
                     delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100
                     if(FAIL == IssuedisableAllBillCmd())  
	             { 
		         writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B disable failed");
                         return FAIL;
	             }
                     else
                     {
                          writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B disable success");
                          //set dispense quanity
                          g_DispenseQuanity=qty;
                     }


                     //Step 2:check b2b state
                     delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100
                     unsigned char  PollReplyPacket[30];
                     memset(PollReplyPacket,'\0',30); 
                     unsigned int   PollBufferLength=30;
                     unsigned int   PollReplyPacketlength=0;
                     int rtcode=0 ;
                     rtcode=GetCurrentB2BState(PollReplyPacket,PollBufferLength,&PollReplyPacketlength);
                     if(1==rtcode)
                     {
                          NoteAcptrStatusLog(PollReplyPacket[3]);
                          if(CASHCODE_RES_DISABLED != PollReplyPacket[3])
                          {

		                  rtcode=WaitforState(CASHCODE_RES_DISABLED,WAIT_TIME_FOR_GET_DISABLE);
		                  if(1!=rtcode)
		                  {
		                       writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B disable state not found so dispense operation can be done now.");
		                       return FAIL;
		                  }
		                  else
		                   writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B disable state found so dispense operation can be done now."); 

                        }
                        else
		          writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B disable state found."); 

                     }       
                     else 
                     {
                        writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] read b2b current state failed.");
                        return FAIL;
                     }


                       //Step 3: issue dispense command
                       delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100

                       memset(log,'\0',100); 
		       sprintf(log,"[CASHCODE_NoteAcptr_dispenseBills()] Dispense Qauntity : %d",qty);
		       writeFileLog(log);

                       sbytes[0] = CASHCODE_SYNC;  
		       sbytes[1] = DEVICE_ADDRESS; 
		       sbytes[2] = 0x08;                  // Len
		       sbytes[3] = CASHCODE_CMD_DISPENSE; // Cmd 
		       sbytes[4]  = billType;             // for CRIS, It will be 0x18 or 24 dcml.( all bill types)
		       sbytes[5]  = qty;                 
		       GetCRC16(sbytes,6,&crc1,&crc2);
		       sbytes[6]  = crc1;
		       sbytes[7]  = crc2;
                       
                    
                       //send dispense command
                       for(i=0;i< 8; i++)
		       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);

		       //get dispense command reply
                       rtcode=0;
                       rtcode=CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
                       if(FAIL == rtcode)
                       {
                            writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B dispense command rply status byte read failed.");
                            return FAIL;
                       }

		       //CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);
                        	
                       //if(state != CASHCODE_RES_ACK)
                       if(0x00 !=rcvPkt[3]) //ack get failed
		       {
		                   writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B dispense command ack failed.");
		                   NoteAcptrStatusLog(rcvPkt[3]);
		                   int i=0;
		                   for(;i<rcvPktLen;i++)
		                   {
		                      memset(log,'\0',100); 
		                      sprintf(log,"[CASHCODE_NoteAcptr_dispenseBills()] rcvPkt[%d] : %x",i,rcvPkt[i]);
		                      writeFileLog(log);
		                   }
		                   
				   return (FAIL);
		       }
		       else
		       {
                               NoteAcptrStatusLog(rcvPkt[3]);

                               //Step 4: Check Dispensing state
                               writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B send ack for dispense command.");
                               int rtcode=-1;

                               //wait for dispensing state
                               rtcode=WaitforState(CASHCODE_RES_DISPENSING,WAIT_TIME_FOR_DISPENSING);

                               //dispensing state found
                               if(1 == rtcode)
                               {
                                    writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B Dispensing State found.");
                                    return SUCCESS;
                               }

                               //dispensing state not yet started
                               else if(0 == rtcode)
                               {
                                    writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B Dispensing State not found.");
                                    return FAIL;
                               }

                               //b2b communication fail during wait for state
                               else if(2 == rtcode)
                               {
                                    writeFileLog("[CASHCODE_NoteAcptr_dispenseBills()] B2B Communication failed.");
                                    return FAIL;
                               }

				
                       }//else block end here
     
}




//Malay 8 April 2013
int GetReturnDetail(int *Quantity,int *b2bStatus)
{
	 
                                writeFileLog("[GetReturnDetail()] Entry.");
                               *Quantity   = 0; 
                               *b2bStatus =  0x00;  
	                        g_state    = -1;
                                unsigned char log[100];
                                memset(log,'\0',100);
		                int rtcode=-1;
                               //wait for dispensed state
                               rtcode=WaitforState(CASHCODE_RES_DISPESED,WAIT_TIME_FOR_DISPENSED);
                               if(1 == rtcode)
                               {
                                    writeFileLog("[GetReturnDetail()] B2B Dispensed State found.");
                                    int notes=-1;
                                    GetNoteDetailsInRecycleCst(&notes);
                                    //B2BGetExtendedRecycleStatus(&notes,0);
                                    memset(log,'\0',100);
                                    sprintf(log,"[GetReturnDetail()] Notes in recycle quntity= %d .",notes);
	                            writeFileLog(log); 
                                    //Reset unload quanity
                                    g_DispenseQuanity=0; 
                                   //if(0==notes)   
                                      return SUCCESS;
                                    //else
                                    //  return FAIL;
                               }
                               else if(0 == rtcode)
                               {
                                    writeFileLog("[GetReturnDetail()] B2B Dispensed State not found.");
                                    int rtcode=0;
                                    unsigned char  PollReplyPacket[30];
                                    memset(PollReplyPacket,'\0',30);
                                    unsigned int   PollBufferLength=30;
                                    unsigned int   PollReplyPacketlength=0;
                                    rtcode=GetCurrentB2BState( PollReplyPacket,PollBufferLength,&PollReplyPacketlength);
                                    if(1==rtcode)
                                    {
                                          if( (CASHCODE_RES_DISPENSING == PollReplyPacket[3]) && ( 0x01 == PollReplyPacket[4]) )
                                          {
                                              writeFileLog("[GetReturnDetail()] Dispensing state is found,customer did not take money yet.");
                                              return SUCCESS;
                                          }
                                    }
                                    else 
                                       return FAIL;
                               }
                               else if(2 == rtcode)
                               {
                                    writeFileLog("[GetReturnDetail()] B2B Communication failed.");
                                    return FAIL;
                               }


        
}





//malay 12 april 2013
int GetNoteDetailsInRecycleCst(int *qty)
{
	
	            int state = FAIL; 
                    *qty=0;  
	            unsigned char rcvPkt[MAX_LEN_OF_PKT];
                    memset(rcvPkt,'\0',MAX_LEN_OF_PKT); 
                    unsigned char log[100];
                    memset(log,'\0',100); 
                    int rcvPktLen=0;           
	            if( 1 == WaitforState(CASHCODE_RES_DISABLED,2)) //wait for disable state
                    {
                            writeFileLog("[GetNoteDetailsInRecycleCst()] Disable state found.");
		            //delay properly execute b2b command
			    delay_mSec(200);

		            //now send command
		            CASHCODE_NoteAcptr_getRecycleCstStatus();
		            //if command reply bytes receieved analysis that packet
			    if(CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen)== SUCCESS )
			    {
				      CASHCODE_NoteAcptr_sendACK();
		                      //check cassette msb byte for varios flag settings
				      if( 1== (rcvPkt[3]>>7) )
				      {
						  //get first recycle cassette note status
		                                  *qty =   rcvPkt[4];    
		                                   sprintf(log,"[GetNoteDetailsInRecycleCst()] Note present in recycle cassette: %d .",rcvPkt[4]);
		                                   writeFileLog(log);    
						   return 1;				 
							       
			              }
				      else
				      {   
					   writeFileLog("[GetNoteDetailsInRecycleCst()] no bill in b2b recycle cassette.");
		                           *qty=0;
					   return 0;
				      }
			      }    
			      else
			      {   
				   writeFileLog("[GetNoteDetailsInRecycleCst()] b2b reply packet error.");
		                   *qty=0;
				   return 0;
			       }

                 }//if( 1 == WaitforState(CASHCODE_RES_DISABLED,2)) //wait for disable state
                 else
                 {
                      writeFileLog("[GetNoteDetailsInRecycleCst()] No Disable state found so recycle cassette read canbe done now.");
                      *qty=0;
                      return 0;
                 }
        
}//GetNoteDetailsInRecycleCst() end




//Milliseconds delay [value must be 0-900]
static void delay_mSec(const int milisec) 
{
        

        unsigned char log[100];

        memset(log,'\0',100);

        struct timespec req = {0},rim={0};

	req.tv_sec = 0;

	req.tv_nsec = milisec * 1000000L;

        int rtcode =0;

        rtcode = nanosleep(&req, &rim);
        
        if( rtcode < 0 )   
	{
	      
              memset(log,'\0',100);
              sprintf(log,"[delay_mSec()] nanosleep() system call failed with return code  %d .",rtcode); 
              writeFileLog(log);

              return;
	      

	}
        else
        {
              //memset(log,'\0',100);

              //sprintf(log,"[delay_mSec()] nanosleep() system call success with return code  %d .",rtcode); 
              //writeFileLog(log);

              return;

        }

      
            
}//delay_mSec() end




unsigned int  NoteAcptrStartCreditPollThread()
{
		  
                  

                  char *message1 = "Thread 1";
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
		  iret1     = pthread_create( &thread1, &attr,ThreadProc1_CASHCODE_NoteAcptrPoll,NULL); // 'thread1' Thread Handler
		  if(0!= iret1)
		  return 0;
		  returnVal = pthread_attr_destroy(&attr);
		  return 1;

                  


}//NoteAcptrStartCreditPollThread() end


int getNoteAcptrPollFlagStatus()
{

   return(g_poll_flag);

}//getNoteAcptrPollFlagStatus() end




//Created By Malay
static inline void IntelligentNoteAcceptorInhibit()
{
	     int NACurrentAtvmFare=0;

             if(g_inhibit_flag == ON)
	     {
	             g_inhibit_flag = OFF;        
		     IssuedisableAllBillCmd();
                     return;
	     }

             else if(1==g_SpecificNoteInhibitFlag)  
             {
		   pthread_mutex_lock(&g_NASpecificInhibitmutex);
                   
                   g_SpecificNoteInhibitFlag=0;

		   NACurrentAtvmFare = g_NACurrentAtvmFare;
                   
                   pthread_mutex_unlock(&g_NASpecificInhibitmutex); 

                   DisableSpecificNotes(NACurrentAtvmFare);
                   
                   return;
             }
             
}//IntelligentNoteAcceptorInhibit() end




void intelligent_delay_mSec(int milisec)
{

           struct timespec req = {0};
	   req.tv_sec = 0;
           //The value of the nanoseconds field must be in the range 0 to 999999999. 
	   req.tv_nsec = 1* 1000000L; //1ms delay

           int NACurrentAtvmFare=0;

           for(;milisec>=1; milisec--)
           {
		     if( ON == g_inhibit_flag )
		     {
			     g_inhibit_flag = OFF;        
			     IssuedisableAllBillCmd();
                             return;

                     }
                     else if( 1 == g_SpecificNoteInhibitFlag )  
                     {
                           pthread_mutex_lock(&g_NASpecificInhibitmutex);

                           g_SpecificNoteInhibitFlag=0;
                           NACurrentAtvmFare = g_NACurrentAtvmFare;
                         
                           pthread_mutex_unlock(&g_NASpecificInhibitmutex);

                           DisableSpecificNotes(NACurrentAtvmFare);
                     }
                     else 
                     {   
                            nanosleep(&req, (struct timespec *)NULL);
                            
                     }
 
            } 
             
}//intelligent_delay_mSec() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline void ClearGlobalPollVars()
{

          if(CASHCODE_RES_STACKED != g_state)
          {
		  //clear global variables
		  int i=0;
		  g_state  = 0x00;
		  g_rcvPktLen=0;
		  for(i = 0; i<MAX_LEN_OF_PKT; i++)
		  {
		        g_rcvPkt[i]='\0';
		  }
          }

	 
}//ClearGlobalPollVars() end


inline void SetCoinFullDisableFlag(int maxNoOfCash , int maxNoOfCoin)
{
     
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



}//SetCoinFullDisableFlag() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void InitNATransModel()
{
	   
           g_faretobeaccept=0;  

           pthread_mutex_lock(&g_NASetSpecificFaremutex);

	   g_NACurrentAtvmFare=0;

           pthread_mutex_unlock(&g_NASetSpecificFaremutex);

           pthread_mutex_lock(&g_NASetSpecificFaremutex);

	   g_SpecificNoteInhibitFlag=0;

           pthread_mutex_unlock(&g_NASetSpecificFaremutex);

	   g_inhibit_flag = OFF;

	   g_SingleNoteAcceptEvent=false;

	   g_AlreadyCoinDisable=false;

           pthread_mutex_lock( &g_stopThreadmutex );

           g_stopThread=false;

           pthread_mutex_unlock( &g_stopThreadmutex );

           int i=0;

           g_state  = 0x00;

           g_rcvPktLen=0;

	   for(i = 0; i<MAX_LEN_OF_PKT; i++)
	   {
		g_rcvPkt[i]='\0';
	   }


}//InitNATransModel() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Malay 7 April 2013
int GetNoteDetailInEscrow(int *value, int *noteStatus)
{
          
          #ifdef B2B_LOGLEVEL_ONE
          writeFileLog("[GetNoteDetailInEscrow()] Entry.");
          #endif
          
          //malay add for time checking 6 april 2013
          time_t start,end,diff;
          unsigned int amnt     = 0; 
	 *value        = amnt;


          //Init clear global denom packet
          ClearGlobalPollVars();

         
	 *noteStatus  = g_state; 

          //make a log about to be accept fare
          char log[100];
          memset(log,'\0',100);

          #ifdef B2B_LOGLEVEL_ONE
          sprintf(log,"[GetNoteDetailInEscrow()] Fare to be accept Rs. : %d ", g_faretobeaccept);
	  writeFileLog(log);
          #endif
          
          if(  (5    == g_faretobeaccept)  ||
	       (10   == g_faretobeaccept)  ||
	       (20   == g_faretobeaccept)  ||
	       (50   == g_faretobeaccept)  ||
	       (100  == g_faretobeaccept)  ||
	       (500  == g_faretobeaccept)  ||
	       (1000 == g_faretobeaccept)   )
         {
                
                g_SingleNoteAcceptEvent=true;
                #ifdef B2B_LOGLEVEL_ONE
                writeFileLog("[GetNoteDetailInEscrow()] Single Note Event Found.");  
                #endif
         }
         else
         {
                
                g_SingleNoteAcceptEvent=false;
                #ifdef B2B_LOGLEVEL_ONE
                writeFileLog("[GetNoteDetailInEscrow()] Single Note Event Not Found."); 
                #endif
         }

          pthread_mutex_lock(&g_NASetPollFlag);

          //start polling b2b
          g_poll_flag = ON;

          pthread_mutex_unlock(&g_NASetPollFlag);

          //get start time
          time(&start);      

          /*
		                         10   = 5,10
		                         20   = 5,10,20
		                         50   = 5,10,20,50
		                         100  = 5,10,20,50,100
		                         500  = 5,10,20,50,100,500
		                         1000 = 5,10,20,50,100,500,1000
         */

          while(1) 
	  { 

                       //if note already in processing disable coin acceptor (no need to accept coin as single note event)
                       if( (CASHCODE_RES_ACCEPTING == g_state) && (false == g_AlreadyCoinDisable) && (true == g_SingleNoteAcceptEvent) && ( 0 == g_CoinDisableflag ))
                       {
		                    #ifdef B2B_LOGLEVEL_ONE
                                    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] Accepting State.");
                                    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] Start Coin Inhibit block.");
                                    #endif

                                    #ifdef COIN_ACCEPTOR

		                    //Lock coin acceptor                            
		                    SetAllCoinInhibitStatus();
		                    g_AlreadyCoinDisable=true;

                                    #endif

                                    #ifdef B2B_LOGLEVEL_ONE
				    writeFileLog("[GetNoteDetailInEscrow() while loop Accepting] End Coin Inhibit block.");                          
                                    #endif
                  
                       } 
 
              
                     //if any below state found Again Enable Coin Acceptor
                     if( (CASHCODE_RES_ACPTR_JAM    == g_state)  ||
	                 (CASHCODE_RES_CSTFULL      == g_state)  ||
	                 (CASHCODE_RES_CSTREMOVED   == g_state)  ||
	                 (CASHCODE_RES_GENERIC_FAIL == g_state)  
                       )
                     {

		               NoteAcptrStatusLog(g_state);

		               if( (true == g_AlreadyCoinDisable) && (true == g_SingleNoteAcceptEvent) && ( 0 == g_CoinDisableflag )  )
		               {
		                     #ifdef B2B_LOGLEVEL_ONE
                                     writeFileLog("[GetNoteDetailInEscrow() while loop] Start Release Coin Acceptor block.");
                                     #endif

                                     #ifdef COIN_ACCEPTOR
		                     NaCoinEnableSet(g_faretobeaccept);
		                     g_AlreadyCoinDisable=false;
                                     #endif
                                    

                                     #ifdef B2B_LOGLEVEL_ONE
		                     writeFileLog("[GetNoteDetailInEscrow() while loop] End Release Coin Acceptor block.");
                                     #endif
		               }    

                               #ifdef B2B_LOGLEVEL_ONE
		               writeFileLog("[GetNoteDetailInEscrow() while loop] Note acceptor error found so return from here.");
		               #endif

                               //return FAIL;
                    }

                     //if note reject or returned by note acceptor then release coin acceptor (Again Enable Coin Acceptor)
                     if( (CASHCODE_RES_REJECTING == g_state ) || (CASHCODE_RES_RETURNED == g_state) )
                     {
		               //NoteAcptrStatusLog(g_state);

                               if( (true == g_AlreadyCoinDisable) && (true == g_SingleNoteAcceptEvent) && ( 0 == g_CoinDisableflag ) )
		               {
		                     #ifdef B2B_LOGLEVEL_ONE
                                     writeFileLog("[GetNoteDetailInEscrow() while loop Rejecting] Start Release Coin Acceptor block.");
                                     #endif

                                     #ifdef COIN_ACCEPTOR
		                     NaCoinEnableSet(g_faretobeaccept);
		                     g_AlreadyCoinDisable=false;
                                     #endif

                                     #ifdef B2B_LOGLEVEL_ONE
		                     writeFileLog("[GetNoteDetailInEscrow() while loop Rejecting] End Release Coin Acceptor block.");
                                     #endif

		               }    
                    }

                    

                     //if note stacked exit from loop and check accepted note value
                     if( CASHCODE_RES_STACKED == g_state ) 
		     {
		            #ifdef B2B_LOGLEVEL_ONE
                            writeFileLog("[GetNoteDetailInEscrow() while loop] Note Stacked."); 
                            #endif
                            break;
		     }

                     //if communication fails exit from loop and free coin acceptor
                     if( YES == g_comunicasn_err_ocrd  ) 
		     {  
		            
                            writeFileLog("[GetNoteDetailInEscrow() while loop] Note Acceptor Communication fails.");
                            if( (true == g_SingleNoteAcceptEvent) && (true == g_AlreadyCoinDisable) && ( 0 == g_CoinDisableflag ) )
                            {
                                     #ifdef B2B_LOGLEVEL_ONE
                                     writeFileLog("[GetNoteDetailInEscrow() while loop] Release Coin Acceptor.");
                                     #endif
                                     #ifdef COIN_ACCEPTOR
                                     NaCoinEnableSet(g_faretobeaccept);
                                     g_AlreadyCoinDisable=false;
                                     #endif
                                     
                            }

                            
		     }

                     //get end time
		     time(&end);
		     //calculate differnnce
		     diff=end-start;
                     //if timeout exit from loop
		     if(diff>=WAIT_FORRECV_NOTE) 
		     {
		            #ifdef B2B_LOGLEVEL_ONE
                            writeFileLog("[GetNoteDetailInEscrow() while loop] Timeout."); 
                            #endif
                            break;
		     }
                       
	  }    

	                    
          pthread_mutex_lock(&g_NASetPollFlag);

          g_poll_flag = OFF;        

          pthread_mutex_unlock(&g_NASetPollFlag);
 

          //return note poll status
          *noteStatus =   g_state; 

          //check note stacked or not stacked
	  if( CASHCODE_RES_STACKED == g_state ) 
          { 
	             
                     //CASHCODE_NoteAcptr_sendACK(); 

                     #ifdef B2B_LOGLEVEL_ONE 
                     writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Note Stacked.");  
                     #endif
                      
                     CASHCODE_NoteAcptr_noteAmountReceived(g_rcvPkt,&amnt);       
		     *value = amnt;
                        
                     //clear stacked information
                     writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Before clear all stacked information.");

                     int i=0;
                     g_state  = 0x00;
                     g_rcvPktLen=0;
		     for(i = 0; i<MAX_LEN_OF_PKT; i++)
		     {
		        g_rcvPkt[i]=0x00;
		     }

                     writeFileLog("[GetNoteDetailInEscrow() Stacked if block] After clear all stacked information.");

                     //make a log about accepted money
                     #ifdef B2B_LOGLEVEL_ONE
                     memset(log,'\0',100);
                     sprintf(log,"[GetNoteDetailInEscrow() if block] Accepted Note Rs. : %d ", amnt);
	             writeFileLog(log);
                     #endif
                   
                     /*
                     //Write Current B2B State
                     unsigned char  PollReplyPacket[30];
                     memset(PollReplyPacket,'\0',30); 
                     unsigned int   PollBufferLength=30;
                     unsigned int   PollReplyPacketlength=0;
                     int rtcode=0 ;
                     rtcode=GetCurrentB2BState(PollReplyPacket,PollBufferLength,&PollReplyPacketlength);
                     */
                     
                     //if accept money is remaining balance then no need to enable coin so return with success
                     if( (g_faretobeaccept == amnt ) && (true == g_SingleNoteAcceptEvent) && (true == g_AlreadyCoinDisable) && ( 0 == g_CoinDisableflag ))
                     {
		                #ifdef B2B_LOGLEVEL_ONE 
                                writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Start Accepted Note Amount is equal to fare to be accepted block.");
                                #endif

                                #ifdef COIN_ACCEPTOR
		                g_AlreadyCoinDisable=false;
                                #endif

                                #ifdef B2B_LOGLEVEL_ONE 
                                writeFileLog("[GetNoteDetailInEscrow() Stacked if block] End Accepted Note Amount is equal to fare to be accepted block.."); 
                                #endif

                                return SUCCESS;
                                     
                     }
                     
                     //release coin acceptor if accepted note amount less than fare to be accept
                     else if( (g_faretobeaccept > amnt) && (true == g_SingleNoteAcceptEvent) && (true == g_AlreadyCoinDisable) && ( 0 == g_CoinDisableflag ))
                     {
                           #ifdef B2B_LOGLEVEL_ONE
                           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Start Free Coin Poll flag block.");
                           #endif
                              
                           #ifdef COIN_ACCEPTOR
                           NaCoinEnableSet(g_faretobeaccept- amnt); 
                           g_AlreadyCoinDisable=false;
                           #endif

                           #ifdef B2B_LOGLEVEL_ONE
                           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] End Free Coin Poll flag block.");
                           #endif

                           return SUCCESS;
                     }
                     else 
                     {
                           #ifdef B2B_LOGLEVEL_ONE
                           writeFileLog("[GetNoteDetailInEscrow() Stacked if block] Normal path Now time to return from here.");
                           #endif
	                   return SUCCESS;  
                     }

                              
                     
	  }    
          //if not note stacked 
          else if( CASHCODE_RES_STACKED != g_state )   
          {
              #ifdef B2B_LOGLEVEL_ONE
              writeFileLog("[GetNoteDetailInEscrow() Not Stacked if block] No Notes Stacked Exit now.");
              #endif
	      return FAIL;   
          }

	 
}//GetNoteDetailInEscrow() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



inline void  NoteAcptrStartSignalCreditPollThread()
{


                /////////////////////////////////////////////////////////////////////////////////////////////

                pthread_mutex_lock( &g_stopThreadmutex );

                g_stopThread =false;
		
                pthread_mutex_unlock( &g_stopThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////

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

                pthread_mutex_lock( &g_NAEnableThreadmutex );

                pthread_cond_signal( &g_NAEnableThreadCond );

                pthread_mutex_unlock( &g_NAEnableThreadmutex );

                /////////////////////////////////////////////////////////////////////////////////////////////
                
                return;


}//NoteAcptrStopSignalCreditPollThread() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////




bool GetNADisableAcceptanceSignal()
{
       
       //writeFileLog("[GetNADisableAcceptanceSignal() b2b] Entry."); 

       bool rtcode=false;

       pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );

       rtcode = g_NADisableAcceptanceFlag ;

       pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );

       //writeFileLog("[GetNADisableAcceptanceSignal() b2b] Exit.");

       return rtcode;
       

}//GetNADisableAcceptanceSignal() end


bool GetNANoteAcceptanceStatus()
{

       //writeFileLog("[GetNANoteAcceptanceStatus() b2b] Entry.");

       bool rtcode=false;

       pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

       rtcode = g_NA_NoteAcceptanceFlag ;
 
       pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );

       //writeFileLog("[GetNANoteAcceptanceStatus() b2b] Exit.");

       return rtcode;
       

}//GetNANoteAcceptanceStatus() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void* ThreadProc1_CASHCODE_NoteAcptrPoll(void *ptr)
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

		writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll() b2b] B2B Poll Thread now exit."); 

                pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

		g_NoteLowLevelPollFlagStatus=0;

                pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
		                    
	        pthread_exit(0);

 
       }
       else
       {

                pthread_mutex_unlock(&g_stopThreadmutex);
                writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll() b2b] B2B Poll Thread Running."); 

       }

               
   /////////////////////////////////////////////////////////////////////////////////////////////////////
   
    pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

    g_NA_NoteAcceptanceFlag =false;
 
    pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );
 
    pthread_mutex_lock( &g_NADisableAcceptanceFlagMutex );

    g_NADisableAcceptanceFlag =false;

    pthread_mutex_unlock( &g_NADisableAcceptanceFlagMutex );


   /////////////////////////////////////////////////////////////////////////////////////////////////////
   unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
   memset(rcvPkt,'\0',MAX_LEN_OF_PKT);
   int  rcvPktLen=0;
   int  state=0x00;
   int  rjct=0;
   rcvPktLen = 0;
   g_noReplyCounter      = 0; 
   g_comunicasn_err_ocrd = NO; 
   g_AlreadyNotePollFlag=false;
   bool alreadydisableflag=false;
   
  
   for(;;)
   {

           /////////////////////////////////////////////////////////////////////////////////////////////////////////////

            pthread_mutex_lock(&g_CloseNotePollThreadFlagmutex);

	    //close note acptr thread (3 March 2013) 
            if(1==g_CloseNotePollThreadFlag)
            {

                g_CloseNotePollThreadFlag=0;

                pthread_mutex_unlock(&g_CloseNotePollThreadFlagmutex);

                pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

                g_NoteLowLevelPollFlagStatus=0; 

                pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);

                writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Thread Exit.");

                pthread_exit(0);

            }

            pthread_mutex_unlock(&g_CloseNotePollThreadFlagmutex);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////

            IntelligentNoteAcceptorInhibit();
                       
            pthread_mutex_lock(&g_NASetPollFlag);

            //if poll flag is on
            if ( ( ON == g_poll_flag  ) &&  ( CASHCODE_RES_STACKED != g_state ) )
	    { 

                         ///////////////////////////////////////////////////////////////
                         
                          pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

                          g_NA_NoteAcceptanceFlag =false;
 
                          pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );
                       
                         ///////////////////////////////////////////////////////////////
		         
                         pthread_mutex_unlock(&g_NASetPollFlag);

                         g_AlreadyNotePollFlag=false;
                        
                         pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

                         //Set Low level flag status is running condition
                         g_NoteLowLevelPollFlagStatus=1; 

                         pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
                         
                         //Delay between two poll and check any inhibit denom required
                         
			 intelligent_delay_mSec(40); //40ms

                         //clear array
			 memset(rcvPkt,'\0',MAX_LEN_OF_PKT);
                         rcvPktLen=0;

                         CASHCODE_NoteAcptr_sendPollCmd();
		     
		
		         if(  SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen)  )
			 {

				    //if successfullt get poll reply bytes then inform b2b with ack status  
                                    CASHCODE_NoteAcptr_sendACK();

				    g_noReplyCounter      = 0; 

				    g_comunicasn_err_ocrd = NO; 
				  
				    //Copy into Global var
				    CopyReceivedPacketGlobally(rcvPkt,rcvPktLen);
				  
				 		
				    CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);	
	  
				    g_state = state;
				   
				    g_isAcptrFault = NO; // added on 01-Nov-12

                                    
						
				  switch(state)
				  {
						 
                                                 case CASHCODE_RES_ACK:
						      break;
				                 
						 case CASHCODE_RES_POWERUP:
                                                      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Powerup State.");			 
						      break;
                                                 
						 case CASHCODE_RES_INITIALIZE:      
						      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Initialize state.");
						      break;

						 case CASHCODE_RES_DISABLED: 
                                                      //#ifdef B2B_LOGLEVEL_ONE        
						      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Disable state.");         
                                                      //#endif
                                                      if( false == alreadydisableflag ) 
                                                      {
		                                              pthread_mutex_lock( 
		                                              &g_NADisableAcceptanceFlagMutex );
		                                              
		                                              g_NADisableAcceptanceFlag =true;

		                                              pthread_mutex_unlock( 
		                                              &g_NADisableAcceptanceFlagMutex ); 

                                                              alreadydisableflag =true;
                                                      }
     
				  	              break;


						 case CASHCODE_RES_IDLING:
                                                      #ifdef B2B_LOGLEVEL_ONE          
						      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Idling state.");
						      #endif 
						      g_already_log_flag = NO;                 
						      break;

						 case CASHCODE_RES_ACCEPTING:    		
						      #ifdef B2B_LOGLEVEL_ONE
						      //writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Accepting state.");
						      #endif 
						      break;

						 case CASHCODE_RES_ESCROW:
						      #ifdef B2B_LOGLEVEL_ONE
						      //writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Escrow state.");
						      #endif 
						      break;


						 case CASHCODE_RES_RETURNING:       
						      #ifdef B2B_LOGLEVEL_ONE
						      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Returning state.");          
                                                      #endif  
						      break;

						      
						 case CASHCODE_RES_INVALID_BILL_NMBR:
                                                      #ifdef B2B_LOGLEVEL_ONE
						      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Invalid bill number state.");          
                                                      #endif    
				  	              
						      break;
						      

						 case CASHCODE_RES_HOLDING:                 
						      #ifdef B2B_LOGLEVEL_ONE
						      //writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Holding state.");
						      #endif  
						      break;

						 case CASHCODE_RES_REJECTING:  
     
						      CASHCODE_NoteAcptr_getCauseOfReject(rcvPkt,&rjct);	
						      break;


						 case CASHCODE_RES_STACKING:   
						      #ifdef B2B_LOGLEVEL_ONE
						      //writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Stacking state.");
						      #endif
						      break;

						 case CASHCODE_RES_STACKED: 
                                                      pthread_mutex_lock(&g_NASetPollFlag);
                                                      g_poll_flag = OFF;        
                                                      pthread_mutex_unlock(&g_NASetPollFlag);
                                                      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Stacked state.");
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
						      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Invalid command state.");     
                                                      #endif
						      break;

						 
						 case CASHCODE_RES_RETURNED:
                                                      #ifdef B2B_LOGLEVEL_ONE
                                                      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Returned state.");
                                                      #endif
						      
						      break;

						 case CASHCODE_RES_ACPTR_JAM:      
						     //#ifdef B2B_LOGLEVEL_ONE
                                                     writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Accepter Jam State.");
                                                     //#endif
						     g_isAcptrFault = YES;
						     break; 

						 case CASHCODE_RES_STKR_JAM: 
						     //#ifdef B2B_LOGLEVEL_ONE
                                                     writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Stacker Jam State");
                                                     //#endif
						     g_isAcptrFault = YES;         
						     break;        

						 case CASHCODE_RES_CSTFULL:
						      //#ifdef B2B_LOGLEVEL_ONE
                                                      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Drop Cassette full state."); 
                                                      //#endif
						      g_isAcptrFault = YES;                
						      break; 

						 case  CASHCODE_RES_CSTREMOVED:  
						     //#ifdef B2B_LOGLEVEL_ONE
                                                     writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Drop Cassette removed state.");
                                                     //#endif

						     g_isAcptrFault = YES;                 
						     break; 		 

						 case CASHCODE_RES_GENERIC_FAIL:
						      //#ifdef B2B_LOGLEVEL_ONE
                                                      writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Note Acceptor Generic Fail");
                                                      //#endif
	                            		      g_isAcptrFault = YES;                    
						      break; 

						  case CASHCODE_RES_CHEATED:
						       #ifdef B2B_LOGLEVEL_ONE
                                                       writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Cheated State");
                                                       #endif
						       break;	
	  
						 
						 case  CASHCODE_RES_RCYCL_CST_STAT:  
                                                       #ifdef B2B_LOGLEVEL_ONE
						       writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Rcycl Cst Stat");
                                                       #endif
                                                       break; 

                                                 default:		 
					                 writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] No Valid b2b state found.");
						         break; 
			 }  // End switch(state)         

		
		 }// polling status read success status if block

		 else  //Polling status read failed else block
		 {        
                          //send nak to b2b
                          //CASHCODE_NoteAcptr_sendNAK();

                          writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Note Acptr Read Port Error.");
                          g_noReplyCounter+=1;
			  if(g_noReplyCounter >= MAX_NO_RPLY_LIM_COUNT)
			  {
			       g_comunicasn_err_ocrd = YES; 
			    
                               writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Note Acptr Communication Failing due maximum read try.");
			    
			  }     
			  
		 }//else block end here

	     } //if(g_poll_flag == ON) 

             else //Polling flag off else block
             {         
                      
                               
         
                               pthread_mutex_unlock(&g_NASetPollFlag);
                           
                               pthread_mutex_lock(&g_NoteLowLevelPollFlagStatusmutex);

			       //Set Low level flag is closed status
			       g_NoteLowLevelPollFlagStatus=0; 

                               if( ( 0 == g_NoteLowLevelPollFlagStatus ) && 
                                   ( false == g_AlreadyNotePollFlag)  
                                 )
                               {
                                    

			            //#ifdef B2B_LOGLEVEL_ONE
                                    writeFileLog("[ThreadProc1_CASHCODE_NoteAcptrPoll()] Poll Off.");
                                    //#endif
                                    g_AlreadyNotePollFlag=true;
                                    
                                    ///////////////////////////////////////////////////////////////
                         
                                    pthread_mutex_lock( &g_NA_NoteAcceptanceFlagMutex );

                                    g_NA_NoteAcceptanceFlag =true;
 
                                    pthread_mutex_unlock( &g_NA_NoteAcceptanceFlagMutex );
                       
                               
                                    ///////////////////////////////////////////////////////////////
                                    

                               }

                               pthread_mutex_unlock(&g_NoteLowLevelPollFlagStatusmutex);
			       
            }       
   
   
    } // End for(;;)



}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int B2BFinishRejectingState()
{
                            
		            unsigned char rcvPkt[30];
                            memset(rcvPkt,'\0',30);
                            unsigned char log[100];
                            memset(log,'\0',100);
	                    int rcvPktLen=0;
                            int rtcode=0;

                            delay_mSec(200); 

		            CASHCODE_NoteAcptr_sendPollCmd();
                            
		            if(SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen) )
                            {
		                         CASHCODE_NoteAcptr_sendACK();

                                         //if it is rejecting state wait
		                         if( 0x1C == rcvPkt[3] )
				         {
				                      writeFileLog("[B2BFinishRejectingState()] B2B Rejecting State Found.");
                                                      //wait for 5min to get disable state
                                                      rtcode = WaitforState(CASHCODE_RES_DISABLED,900); //15min =15*60=900 Seconds

				                      //disable state found
				                      if(1 == rtcode)
				                      {
				                           writeFileLog("[B2BFinishRejectingState()] B2B Rejecting to Disabled State found.");
				                           return SUCCESS;
				                      }
				                      //disable state not yet found
				                      else if(0 == rtcode)
				                      {
				                           writeFileLog("[B2BFinishRejectingState()] B2B Disabled State not found.");
				                           return FAIL;
				                      }
				                      //b2b communication fail during wait for state
				                      else if(2 == rtcode)
				                      {
				                            writeFileLog("[B2BFinishRejectingState()] B2B Communication failed.");
				                            return FAIL;
				                      }
		                         }

                                         //if it is returning state wait
		                         if( 0x18 == rcvPkt[3] )
				         {
				                      writeFileLog("[B2BFinishRejectingState()] B2B Returning State Found.");
                                                      //wait for 5min to get disable state
                                                      rtcode = WaitforState(CASHCODE_RES_DISABLED,1800); //30min =30*60=1800Second

				                      //disable state found
				                      if(1 == rtcode)
				                      {
				                           writeFileLog("[B2BFinishRejectingState()] B2B Returning to Disabled State found.");
				                           return SUCCESS;
				                      }
				                      //disable state not yet found
				                      else if(0 == rtcode)
				                      {
				                           writeFileLog("[B2BFinishRejectingState()] B2B Disabled State not found.");
				                           return FAIL;
				                      }
				                      //b2b communication fail during wait for state
				                      else if(2 == rtcode)
				                      {
				                            writeFileLog("[B2BFinishRejectingState()] B2B Communication failed.");
				                            return FAIL;
				                      }
		                         }
                                         
                                         else if( 0x19 == rcvPkt[3] ) //disable
                                         {
                                                writeFileLog("[B2BFinishRejectingState()] B2B Disable State Found .");
				                return SUCCESS;
		                         }  
                                         else 
                                         {
                                                sprintf(log,"[B2BFinishRejectingState()] B2B Disable or Rejecting or Returning State Not Found rcvPkt[3] =%x.",rcvPkt[3]);
                                                writeFileLog(log);
				                return SUCCESS;
		                         }   
 

                            }
                            else
                            {
                                  writeFileLog("[B2BFinishRejectingState()] B2B Poll status read failed.");
				  return FAIL;
		            }          
                     
		                 
			  

}


//read b2b reply bytes
static int B2BGetExtendedRecycleStatusRead(unsigned char* const rcvPkt,int* const rcvPktLen)
{
	    unsigned char rxByte=0x00;
	    int       rcvStat=0;
            int       byteRcvdCounter=0;
	    int       totalByteToRecv=0; 
	    int       totalByteIn=0;
                              
	   
            time_t start,end,diff;

            //Start Timer
            time(&start);
	    for(;;)
            {        
			 rcvStat = ReceiveSingleByteFromSerialPort(g_acptrPortHandler,&rxByte,&totalByteIn);

                         //if 1 byte data received.
			 if( 1 == totalByteIn) 
			 {
                              rcvPkt[byteRcvdCounter] = rxByte; 
                              byteRcvdCounter +=1;         
                                                           
			  
                         } //End if totalByteIn is 1 byte
                       
                         //Check timer status
                         time(&end); 
                         diff=end-start;
                         if( diff>= 1 )
                         {
                              break;
                         }


           }  // End for(;;)
            
           *rcvPktLen  = byteRcvdCounter + 1 ;

           if(0==byteRcvdCounter)
           { 
                writeFileLog("[B2BGetExtendedRecycleStatusRead()] No byte received from b2b.");
                return FAIL;                                     
	   }
	   else
	        return SUCCESS;

}//B2BGetExtendedRecycleStatusRead(unsigned char* const rcvPkt,int* const rcvPktLen) end



int B2BGetExtendedRecycleStatus( int* const NoteQuanity,const unsigned int Seconds)
{
                                   
                                   

                                   unsigned char rcvPkt[30];
	                           int rcvPktLen=0,i=0;
                                   memset(rcvPkt,'\0',30);

                                   *NoteQuanity=0;

                                   unsigned char log[100];
                                   memset(log,'\0',100);

                                   if(SUCCESS == IssuedisableAllBillCmd())
                                   {
                                        writeFileLog("[B2BGetExtendedRecycleStatus()] Disable Command Success.");
                                   }
                                   else
                                   {
                                        writeFileLog("[B2BGetExtendedRecycleStatus()] Disable Command failed. ");
                                        return FAIL;
                                   }

                                   if( 1 == WaitforState(CASHCODE_RES_DISABLED,5)) 
                                   {
					   
		                                   CASHCODE_NoteAcptr_ExtendedRecycleCstStatus();

                                                   Wait(Seconds);

						   //CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
                                                   
                                                   B2BGetExtendedRecycleStatusRead(rcvPkt,&rcvPktLen);


						   CASHCODE_NoteAcptr_sendACK();

                                                   sprintf(log,"[B2BGetExtendedRecycleStatus()] Recv Packet Length = %d ",rcvPktLen);
		                                   writeFileLog(log);
		                                   memset(log,'\0',100);
                                                   for(i=0;i<rcvPktLen;i++)
						   {
							
		                                        sprintf(log,"[B2BGetExtendedRecycleStatus()] rcvPkt[%d] = %x ",i,rcvPkt[i]);
		                                        writeFileLog(log);
		                                        memset(log,'\0',100);
				                   }

                                                   //check reply bytes checksum
                                                   unsigned char crc1=0x00, crc2=0x00;

                                                   
                                                   //unsigned char LastIndex= rcvPktLen -1 ;

                                                   rcvPktLen= rcvPkt[2] ;

                                                   GetCRC16(rcvPkt, rcvPktLen-2 ,&crc1,&crc2);

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[B2BGetExtendedRecycleStatus()] Reply Packet CRC LSB= %x ",rcvPkt[rcvPktLen -2]);
		                                   writeFileLog(log);

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[B2BGetExtendedRecycleStatus()] Reply Packet CRC MSB= %x ",rcvPkt[rcvPktLen -1]);
		                                   writeFileLog(log);

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[B2BGetExtendedRecycleStatus()] Calculted CRC LSB= %x ",crc1);
		                                   writeFileLog(log);

                                                   memset(log,'\0',100);
                                                   sprintf(log,"[B2BGetExtendedRecycleStatus()] Calculted CRC MSB= %x ",crc2);
		                                   writeFileLog(log);
		                                   
                                                  
                                                   if( ( crc1 == rcvPkt[rcvPktLen -2] ) &&  // reply packet lsb
                                                       ( crc2 == rcvPkt[rcvPktLen -1] )     // reply packet  msb
                                                     )   
                                                   {
                                                      writeFileLog("[B2BGetExtendedRecycleStatus()] Reply packet checksum ok.");
                                                   }
                                                   else
                                                   {
                                                      writeFileLog("[B2BGetExtendedRecycleStatus()] Reply packet checksum not ok.");
                                                   }

           		                           unsigned int counter=0; 
                                                   if(rcvPktLen > 0)
                                                   {
                                                          counter= rcvPktLen - 6;
                                                   }
                                                   
                                                   memset(log,'\0',100);
                                                   sprintf(log,"[B2BGetExtendedRecycleStatus()] Total Notes = %d .",counter);
		                                   writeFileLog(log);
                                                   *NoteQuanity = counter;
                                                   return SUCCESS;
                                                     
                                   }
                                   else
                                   {
                                         writeFileLog("[B2BGetExtendedRecycleStatus()] Disable state not found. "); 
                                         return FAIL;
                                   }


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Add By Malay 25 Jan 2013
inline unsigned int InhibitNoteAcptr()
{
   g_inhibit_flag = ON; // Inhibit Cmd issues from inside intelligentDelay func.
   return(SUCCESS);  
}



//Malay add 2 Feb 2013

inline void SetSpecificNoteInhibitStatus(int fare)
{
	   

           pthread_mutex_lock(&g_NASetSpecificFaremutex);

	   g_SpecificNoteInhibitFlag=1;
	   g_NACurrentAtvmFare=fare;

           /*

	   char log[100];
	   memset(log,'\0',100);
	   sprintf(log,"[SetSpecificNoteInhibitStatus()] fare: Rs. %d .",g_NACurrentAtvmFare); 
	   writeFileLog(log);
	   
          */

	   pthread_mutex_unlock(&g_NASetSpecificFaremutex);
	      

}


inline int DisableSpecificNotes(int fare)
{
	

			 unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
			 memset(rcvPkt,'\0',MAX_LEN_OF_PKT);
			 int  rcvPktLen=0;
			 int state=-1,retry=1;
			 noteType NOTE={0,0,0,0,0,0,0,0};
			 if(fare>=5)
			 NOTE.rs_5    = 1;
			 if(fare>=10)
			 NOTE.rs_10   = 1;
			 if(fare>=20)
			 NOTE.rs_20   = 1;
			 if(fare>=50)
			 NOTE.rs_50   = 1;
			 if(fare>=100)
			 NOTE.rs_100  = 1;
			 if(fare>=500)
			 NOTE.rs_500  = 1;
			 if(fare>=1000)
			 NOTE.rs_1000 = 1;

			 #ifdef B2B_ACPTR  
			 NOTE.escrow_on_off = 0;
			 #else
			 NOTE.escrow_on_off = 1;
			 #endif  

                        #ifdef B2B_LOGLEVEL_ONE
                        char log[100];
                        memset(log,'\0',100);
                        sprintf(log,"[DisableSpecificNotes()] Current fare : Rs. %d.", fare);       
		        writeFileLog(log);
                        #endif

                       //equal fare
	               if( 0 == fare)
		       {
			    //writeFileLog("[DisableSpecificNotes()] Equal fare state found."); 
			    //writeFileLog("[DisableSpecificNotes()] Now Going to disable notes.");
                            writeFileLog("[DisableSpecificNotes()] Zero fare receieved.");
                            return;
		       }
                       
			    
                       CASHCODE_NoteAcptr_enableBillTypes(NOTE); 
		       state = -1; 
		       CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);      
		       CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);        
		       if( CASHCODE_RES_ACK == state)
		       {
		            #ifdef B2B_LOGLEVEL_ONE
                            writeFileLog("[DisableSpecificNotes()] Specific Notes Disable success."); 
                            #endif
                            return SUCCESS;
		       }
                       else if( CASHCODE_RES_ACK != state)
		       {
			    #ifdef B2B_LOGLEVEL_ONE
                            writeFileLog("[DisableSpecificNotes()] Specific Notes Disable failed."); 
                            #endif
                            return FAIL;
		       }
       
	       
}


int IssuedisableAllBillCmd()
{
	

	    int retry;
	    unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
	    memset(rcvPkt,'\0',MAX_LEN_OF_PKT);
	    int  rcvPktLen=0;
	    int state = FAIL;

	    noteType NOTE;
	    NOTE.rs_5    = 0;
	    NOTE.rs_10   = 0;
	    NOTE.rs_20   = 0;
	    NOTE.rs_50   = 0;
	    NOTE.rs_100  = 0;
	    NOTE.rs_500  = 0;
	    NOTE.rs_1000 = 0; 
	    #ifdef B2B_ACPTR  
	    NOTE.escrow_on_off = 0;
	    #else
	    NOTE.escrow_on_off = 1;
	    #endif  


	    
		       CASHCODE_NoteAcptr_enableBillTypes(NOTE); 
		      
		       //......... added on 28-Apr-12 ...........
		       state = -1;   
		       CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
		       CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);	
		       if(state == CASHCODE_RES_ACK)
		       {
				#ifdef B2B_LOGLEVEL_ONE   
				writeFileLog("[IssuedisableAllBillCmd()] Disable all bills Cmd issued success."); 
                                #endif
				state = SUCCESS; 
				
		       }
		       else
		       {
				#ifdef B2B_LOGLEVEL_ONE   
				writeFileLog("[IssuedisableAllBillCmd()] Disable all bills Cmd issued failed."); 
                                #endif
				state = FAIL;
				
		       }   
	       
	     

	      return(state);
}


//malay 12 april 2013
/*bit position: 8   7  6  5  4  3  2  1   : Note value
           ---------------------------------------------------------------
                0   0  0  0  0  0  0  1 b = 5   
	        0   0  0  0  0  0  1  0 b = 10
	        0   0  0  0  0  1  0  0 b = 20
	        0   0  0  0  1  0  0  0 b = 50
	        0   0  0  1  0  0  0  0 b = 100
	        0   0  1  0  0  0  0  0 b = 500
	        0   1  0  0  0  0  0  0 b = 1000 

*/



void CASHCODE_NoteAcptr_enableBillTypes(noteType NOTE)
{

	    unsigned int i=0;
	    unsigned char crc1=0x00, crc2=0x00;
            char noteEnableByte = 0x00;       
	    unsigned char escrow = 0;
	    unsigned char sbytes[20]; 
            memset(sbytes,'\0',20);
	    int ret = SUCCESS;
	    int retry;  
	    if( ENABLE == NOTE.rs_5 )
            {
                 noteEnableByte = noteEnableByte | 0x01; //0b00000001;
                 #ifdef B2B_LOGLEVEL_TWO
                 writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Rs. 5 Enable. "); 
                 #endif
            }
            if( ENABLE == NOTE.rs_10 )
	    {
		  noteEnableByte = noteEnableByte | 0x02;//0b00000010;
                  #ifdef B2B_LOGLEVEL_TWO
                  writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Rs. 10 Enable. "); 
                  #endif
            }
            if( ENABLE == NOTE.rs_20 )
            {
		  noteEnableByte = noteEnableByte | 0x04;//0b00000100;
                  #ifdef B2B_LOGLEVEL_TWO
                  writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Rs. 20 Enable. ");  
                  #endif
	    }
            if( ENABLE == NOTE.rs_50 )
	    {
                  noteEnableByte = noteEnableByte | 0x08;//0b00001000;
                  #ifdef B2B_LOGLEVEL_TWO
                  writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Rs. 50 Enable. "); 
                  #endif
	    }
            if( ENABLE == NOTE.rs_100 )
            {
		  noteEnableByte = noteEnableByte | 0x10;//0b00010000;
                  #ifdef B2B_LOGLEVEL_TWO
                  writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Rs. 100 Enable. "); 
                  #endif
	    }
            if( ENABLE == NOTE.rs_500 )
	    {
		  noteEnableByte = noteEnableByte | 0x20;//0b00100000;
                  #ifdef B2B_LOGLEVEL_TWO
                  writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Rs. 500 Enable. "); 
                  #endif
	    }
            if( ENABLE == NOTE.rs_1000 )
            {
		  noteEnableByte = noteEnableByte | 0x40;//0b01000000;
                  #ifdef B2B_LOGLEVEL_TWO
                  writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Rs. 1000 Enable. ");  
                  #endif
            }
            #ifdef B2B_ACPTR  
                 NOTE.escrow_on_off = 0;        
            #else
                 NOTE.escrow_on_off = 1;         
            #endif  
           sbytes[0] = CASHCODE_SYNC;  
	   sbytes[1] = DEVICE_ADDRESS; 
	   sbytes[2] = 0x0C;              //hole command Length
	   sbytes[3] = CASHCODE_CMD_ENABLE_BILL_TYPE; 
	   sbytes[4] = 0x00; 
	   sbytes[5] = 0x00; 
	   sbytes[6] = noteEnableByte;    //Y1-Y3: bill types enable 
	   sbytes[7] = 0x00; 
	   sbytes[8] = 0x00; 
	   sbytes[9] = escrow;            //Y4-Y6: bill types with Escrow
           GetCRC16(sbytes,10,&crc1,&crc2);
           sbytes[10]  = crc1;
	   sbytes[11]  = crc2;
           for(i=0;i< 12; i++)
	   ret = SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
           if(SUCCESS==ret)
           {
                #ifdef B2B_LOGLEVEL_TWO
                writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Enable bill type Cmd sent with success status."); 
                #endif
           }
           else
           {
                #ifdef B2B_LOGLEVEL_TWO
                writeFileLog("[CASHCODE_NoteAcptr_enableBillTypes()] Enable bill type Cmd sent with fail status");
                #endif 
           }


}//CASHCODE_NoteAcptr_enableBillTypes() end


//read b2b reply bytes
static int CASHCODE_NoteAcptr_statusRead(unsigned char* const rcvPkt,int* const rcvPktLen)
{
	    unsigned char rxByte;
	    int       rcvStat=0;
            int       byteRcvdCounter=0;
	    int       totalByteToRecv=0; 
	    int       totalByteIn=0;
            long int i=0;                     
	    totalByteIn     = 0;
	    byteRcvdCounter = 0;
	    totalByteToRecv = 0;
            time_t start,end,diff;
            //Start Timer
            time(&start);
	    for(;;)
            {        
			 rcvStat = ReceiveSingleByteFromSerialPort(g_acptrPortHandler,&rxByte,&totalByteIn);

                         //if 1 byte data received.
			 if( 1 == totalByteIn) 
			 {
                              rcvPkt[byteRcvdCounter] = rxByte; 
                              byteRcvdCounter +=1;         
                              //3rd Byte Of Reply Pkt Contains Total No. of bytes to Read, against the issued Command.      
		              if( 3 == byteRcvdCounter )                 
			      {
                                     totalByteToRecv = rxByte;
				     *rcvPktLen      = totalByteToRecv; 
			      }
                              if(byteRcvdCounter == totalByteToRecv)  
		 	      { 
                                     //writeFileLog("[CASHCODE_NoteAcptr_statusRead()] Expected bytes received from b2b.");
                                     return SUCCESS;
			      }          
			  
                       } //End if totalByteIn is 1 byte
                       
                       //Check timer status
                       time(&end); 
                       diff=end-start;
                       if(diff>= WAIT_TIME_FOR_B2BREPLY )
                       break;


           }  // End for(;;)
            
           if(0==byteRcvdCounter)
           { 
                //writeFaultLog("[CASHCODE_NoteAcptr_statusRead()] No byte received from b2b.");
                writeFileLog("[CASHCODE_NoteAcptr_statusRead()] No byte received from b2b.");
                return FAIL;                                     
	   }
	   if(byteRcvdCounter < totalByteToRecv)
	   {
                //writeFaultLog("[CASHCODE_NoteAcptr_statusRead()] Some byte received from b2b.");
                writeFileLog("[CASHCODE_NoteAcptr_statusRead()] Some byte received from b2b.");
	        return FAIL;                         
           }  
	    
           return SUCCESS;

}//CASHCODE_NoteAcptr_statusRead() end



static  void  displayReceivedPacket(const unsigned char* const rcvPkt,const int  rcvPktLen)
{
	  int i;  
	  for(i = 0; i<rcvPktLen; i++)
	  {
	     //printf(" [%d]=0x%x  ",i,rcvPkt[i]);
	  } 

}

/*

static void CopyReceivedPacketGlobally(const unsigned char* const rcvPkt,const int  rcvPktLen)   
{
	  int i;  
	  g_rcvPktLen = rcvPktLen;
	  for(i = 0; i<rcvPktLen; i++)
	  {
	     g_rcvPkt[i] = rcvPkt[i];
	  } 
}

*/

static void CopyReceivedPacketGlobally(const unsigned char* const rcvPkt,const int  rcvPktLen)   
{
	  int i=0;  

          //clear global variables
          g_rcvPktLen=0;
          for(i = 0; i<MAX_LEN_OF_PKT; i++)
          {
                g_rcvPkt[i]='\0';
          }

          //Now stores values
	  g_rcvPktLen = rcvPktLen;
	  for(i = 0; i<rcvPktLen; i++)
	  {
	     g_rcvPkt[i] = rcvPkt[i];
	  } 
}

static unsigned int GetCRC16(const unsigned char* bufData, const unsigned int sizeData,char* const x1,char* const x2)
{

	  unsigned int CRC, i;
	  unsigned char j;
	  unsigned constant = 256;
          CRC = 0;
          for(i=0; i < sizeData; i++)
	  {	 
		    CRC ^= bufData[i];
		    for(j=0; j < 8; j++)
	            {
		         if(CRC & 0x0001) 
			 {
			    CRC >>= 1;
			    CRC ^= POLYNOMIAL;
			 }
		         else 
			    CRC >>= 1;
		     } // End: inner for
	  
          } // End: outer for
          
          *x2 = CRC/constant;
	  *x1 = CRC-(*x2  * 256);
	  return CRC;
}



static int CASHCODE_NoteAcptr_analysisRcvdPkt(const unsigned char* rcvPkt,int* const state)
{ 
   
   
     *state = rcvPkt[3];              //The 4th field contains the status  
      return(SUCCESS);


}




int CASHCODE_NoteAcptr_getCauseOfReject(const unsigned char* const rcvPkt,int* const rjct)
{    
	   unsigned char log[200];
           memset(log,'\0',200);

	   *rjct = rcvPkt[4];     
        
            switch(*rjct)
	    {
		      case CASHCODE_RES_RJCTNG_INSRT_ERR:  
                            memset(log,'\0',200);              
			    strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to Note insertion Err.");
                            break;

		      case CASHCODE_RES_RJCTNG_MAGNTC_ERR:
                            memset(log,'\0',200);  
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to Magnetic Err");
		            break;

		      case CASHCODE_RES_RJCTNG_BILL_IN_HEAD:
                             memset(log,'\0',200);   
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to  Note present in head"); 
		             break;

		      case CASHCODE_RES_RJCTNG_MULTIPLY_ERR: 
                            memset(log,'\0',200);  
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to Multiply Err.");  
		            break;

		      case CASHCODE_RES_RJCTNG_CONVEY_ERR: 
                            memset(log,'\0',200);    
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to Convey Err.,check brezzle & all settings."); 
                            break;

		      case CASHCODE_RES_RJCTNG_IDNTFY_ERR:  
                             memset(log,'\0',200);        
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to identification Err."); 
		             break;

		      case CASHCODE_RES_RJCTNG_VRFY_ERR: 
                             memset(log,'\0',200);             
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to verification Err.");  
		             break;

		      case CASHCODE_RES_RJCTNG_OPTIC_ERR: 
                             memset(log,'\0',200);            
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to optical Err."); 
		             break;

		      case  CASHCODE_RES_RJCTNG_INHIBIT_DENOM: 
                             memset(log,'\0',200);       
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to Inhibit denomination.");
		             break;

		      case  CASHCODE_RES_RJCTNG_CAPACITY_ERR:  
                             memset(log,'\0',200);        
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to capacity Err.");
		             break;

		      case CASHCODE_RES_RJCTNG_OPERATION_ERR: 
                             memset(log,'\0',200);        
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to operation Err.");  
		             break;
 
		      case CASHCODE_RES_RJCTNG_LNGTH_ERR: 
                            memset(log,'\0',200);             
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to length Err.");  
		            break;

		      case  CASHCODE_RES_RJCTNG_UV_PROPTY_MISMATCH:
                            memset(log,'\0',200);    
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to  UV property mismatch Err.");  
		            break;

		      case CASHCODE_RES_RJCTNG_TREATED_AS_BARCODE:  
                            memset(log,'\0',200);  
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to treated as Barcode.");
		            break;

		      case CASHCODE_RES_RJCTNG_WRONG_BARCODE_CHAR:   
                            memset(log,'\0',200);  
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to wrong Barcode char."); 
		            break;

		      case  CASHCODE_RES_RJCTNG_WRONG_BARCODE_START: 
                            memset(log,'\0',200);   
		            strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to wrong Barcode start Err.");  
		            break;

		      case  CASHCODE_RES_RJCTNG_WRONG_BARCODE_STOP: 
                             memset(log,'\0',200);    
		             strcpy (log,"[CASHCODE_NoteAcptr_getCauseOfReject()] Rejecting due to  wrong Barcode stop Err.");
		             break;
	     } 
            
           
	    if( g_already_log_flag == NO )
	    {    
		  g_already_log_flag  = YES;  
                  writeFileLog(log); 
	    }
	    
            return(SUCCESS);

}

  
static void CASHCODE_NoteAcptr_noteAmountReceived(const unsigned char* const rcvPkt,int* const amnt)    
{

	  *amnt        = 0;

	  if(rcvPkt[4] == 0)   
	      *amnt = 5;

	  if(rcvPkt[4] == 1)   
	      *amnt = 10;


	  if(rcvPkt[4] == 2)
	      *amnt = 20;


	  if(rcvPkt[4] == 3)
	      *amnt = 50;


	  if(rcvPkt[4] == 4)
	      *amnt = 100;



	  if(rcvPkt[4] == 5)
	      *amnt = 500;


	  if(rcvPkt[4] == 6)
	      *amnt = 1000;
}


static void CASHCODE_NoteAcptr_noteQtyReceived(const unsigned char* const rcvPkt,int* const amnt)
{
     //MODULE: This madule is aplicable only for B2B.
     *amnt        = rcvPkt[4];
}


///////////////////////////////START:CASHCODE LOWLEVEL COMMAND ////////////////////////////////////////////////////////


void CASHCODE_NoteAcptr_sendPollCmd()
{
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       sbytes[0] = CASHCODE_SYNC;     // SYNC 
       sbytes[1] = DEVICE_ADDRESS;    // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;              // Len
       sbytes[3] = CASHCODE_CMD_POLL; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i;
       for(i=0;i< 6; i++)
       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
            
}


void CASHCODE_NoteAcptr_reste()
{
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       sbytes[0] = CASHCODE_SYNC;     // SYNC 
       sbytes[1] = DEVICE_ADDRESS;    // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;              // Len sbytes[0] = CASHCODE_SYNC;     // SYNC 
       sbytes[3] = CASHCODE_CMD_RESET; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i;
       for(i=0;i< 6; i++)
       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
}

void CASHCODE_NoteAcptr_sendACK()
{
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1=0, crc2=0;
       sbytes[0] = CASHCODE_SYNC;     // SYNC 
       sbytes[1] = DEVICE_ADDRESS;    // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;              // Len
       sbytes[3] = CASHCODE_CMD_ACK;  // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i=0;
       for(i=0;i< 6; i++)
       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
}


static void CASHCODE_NoteAcptr_sendNAK()
{
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1=0, crc2=0;
       sbytes[0] = CASHCODE_SYNC;     
       sbytes[1] = DEVICE_ADDRESS;    
       sbytes[2] = 0x06;              
       sbytes[3] = 0XFF;  
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i=0;
       for(i=0;i< 6; i++)
       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
}


void CASHCODE_NoteAcptr_hold()
{

       //unsigned char sbytes[200];
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       sbytes[0] = CASHCODE_SYNC;     // SYNC 
       sbytes[1] = DEVICE_ADDRESS;    // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;              // Len
       sbytes[3] = CASHCODE_CMD_HOLD; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i=0;
       for(i=0;i< 6; i++)
       {
            SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
           
       } 

}


int CASHCODE_NoteAcptr_stack()
{
       
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
       int  rcvPktLen;
       int state;
       sbytes[0] = CASHCODE_SYNC;     // SYNC 
       sbytes[1] = DEVICE_ADDRESS;    // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;              // Len
       sbytes[3] = CASHCODE_CMD_STACK; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i;
       for(i=0;i< 6; i++)
       {
            SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
       } 
      //............ added on 3-Oct-12................
      CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
      CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);	
      if(state == CASHCODE_RES_ACK) 
      {
	    return(SUCCESS);
      }
      else
      {  
	    return(FAIL);
      }

}


int CASHCODE_NoteAcptr_return()
{

       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
       int  rcvPktLen;
       int state;

       sbytes[0] = CASHCODE_SYNC;       // SYNC 
       sbytes[1] = DEVICE_ADDRESS;      // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;                // Len
       sbytes[3] = CASHCODE_CMD_RETURN; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i;
      
       for(i=0;i< 6; i++)
       {
            SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
       } 

	 //............ added on 3-Oct-12................
	 CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
	 CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);	
	 if(state == CASHCODE_RES_ACK)
	 {
	     return(SUCCESS);
	 }
	 else
	 {  
	     return(FAIL);
	 }
}

void CASHCODE_NoteAcptr_status()
{

       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       sbytes[0] = CASHCODE_SYNC;           // SYNC 
       sbytes[1] = DEVICE_ADDRESS;          // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;                    // Len
       sbytes[3] = CASHCODE_CMD_GET_STATUS; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i;
       for(i=0;i< 6; i++)
       {
            SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
       } 
}


void CASHCODE_NoteAcptr_identification()
{
       //unsigned char sbytes[200];
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       sbytes[0] = CASHCODE_SYNC;               // SYNC 
       sbytes[1] = DEVICE_ADDRESS;              // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;                        // Len
       sbytes[3] = CASHCODE_CMD_IDENTIFICATION; // Cmd 
 
       GetCRC16(sbytes,4,&crc1,&crc2);
 
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;

       int i;
        
       for(i=0;i< 6; i++)
       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
          
}



void CASHCODE_NoteAcptr_setSecurity(const unsigned int EnableByte1,
                                    const unsigned int EnableByte2,
                                    const unsigned int EnableByte3
                                   )
{

       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       sbytes[0] = CASHCODE_SYNC;               // SYNC 
       sbytes[1] = DEVICE_ADDRESS;              // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x09;                        // Len
       sbytes[3] = CASHCODE_CMD_SET_SECURITY;   // Cmd 
       sbytes[4] = EnableByte1;
       sbytes[5] = EnableByte2;
       sbytes[6] = EnableByte3;

       GetCRC16(sbytes,7,&crc1,&crc2);
 
       sbytes[7]  = crc1;
       sbytes[8]  = crc2;

       int i;
        
       for(i=0;i< 9; i++)
       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);

}

void CASHCODE_NoteAcptr_getBillTable()
{
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1, crc2;
       sbytes[0] = CASHCODE_SYNC;               // SYNC 
       sbytes[1] = DEVICE_ADDRESS;              // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;                        // Len
       sbytes[3] = CASHCODE_CMD_GET_BILL_TABLE; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i;
       for(i=0;i< 6; i++)
       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
             
}

//Set Recycle Cassette 
static unsigned int CASHCODE_NoteAcptr_SetRecycleCassette( const unsigned int RecycleCassetteNumber,
                                                           const unsigned int RecycleCassetteState
                                                         )
{
       
       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1=0, crc2=0;
       sbytes[0] = CASHCODE_SYNC;               
       sbytes[1] = DEVICE_ADDRESS;              
       sbytes[2] = 0x08;                        
       sbytes[3] = CASHCODE_CMD_SET_RECYCLE_CASSETTE; 
       sbytes[4] = RecycleCassetteNumber;
       sbytes[5] = RecycleCassetteState; 
       GetCRC16(sbytes,6,&crc1,&crc2);
       sbytes[6]  = crc1;
       sbytes[7]  = crc2;
       int i;
       int rtcode=0;
       /*
       for(i=0;i< 8; i++)
       {
           printf("\n [CASHCODE_NoteAcptr_SetRecycleCassette] sbytes[%d] = %x ",i,sbytes[i]); 
       }
       */
       for(i=0;i< 8; i++)
       {
            rtcode=SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
       }

       return rtcode;   

}

//Get Current Recycle Cassette Status
void CASHCODE_NoteAcptr_getRecycleCstStatus()
{

       unsigned char sbytes[B2B_COMMAND_LEN_OF_PKT];
       char crc1=0, crc2=0;
       sbytes[0] = CASHCODE_SYNC;     // SYNC 
       sbytes[1] = DEVICE_ADDRESS;    // Dev Adrs( B2B adrs = 0x01, Cash Code adrs = 0x03)
       sbytes[2] = 0x06;              // Len sbytes[0] = CASHCODE_SYNC;     
       sbytes[3] = CASHCODE_CMD_RECYCL_CST_STATUS; // Cmd 
       GetCRC16(sbytes,4,&crc1,&crc2);
       sbytes[4]  = crc1;
       sbytes[5]  = crc2;
       int i;
       /*
       for(i=0;i< 6; i++)
       {
           printf("\n sbytes[%d] = %x ",i,sbytes[i]); 
       }
       */
       for(i=0;i< 6; i++)
       {
           SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
       }
           
}

void CASHCODE_NoteAcptr_ExtendedRecycleCstStatus()
{

       unsigned char sbytes[10]; 
       int i=0;
       for(i=0;i<10;i++)
       {
          sbytes[i]=0;
       }

       char crc1=0, crc2=0;
       sbytes[0] = CASHCODE_SYNC;     
       sbytes[1] = DEVICE_ADDRESS;   
       sbytes[2] = 0x07;                 
       sbytes[3] = 0x70; 
       sbytes[4] = 0x01;  
       GetCRC16(sbytes,5,&crc1,&crc2);
       sbytes[5]  = crc1;
       sbytes[6]  = crc2;
       /*
       for(i=0;i< 7; i++)
       {
           printf("\n sbytes[%d] = %x ",i,sbytes[i]); 
       }
       */

       for(i=0;i< 7; i++)
       {
           SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
       }
           
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef B2B_MAIN
void SetTempCassettetype()
{
                    unsigned char rcvPkt[30];
	            int rcvPktLen=0,i=0;
                         
                    //set recycle cassette 1
                    delay_mSec(200); //must

                    CASHCODE_NoteAcptr_SetRecycleCassette( 1,0b00000001);     //Escrow=0b00011000
                    memset(rcvPkt,'\0',30);
                    rcvPktLen=0;
		    CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
                    for(i=0;i<rcvPkt[2];i++)
	            {
		   
		       printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

	            }
                    //WaitforState(0x21,5);
                    //WaitforState(0x29,10);
                    //WaitforState(0x19,10);   
         
                    
                    while(0x19 != rcvPkt[3])
                    {
		            delay_mSec(200); //must
		            
		            CASHCODE_NoteAcptr_sendPollCmd();
		            memset(rcvPkt,'\0',30);
		            rcvPktLen=0;
			    CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
		            CASHCODE_NoteAcptr_sendACK();
		            for(i=0;i<rcvPkt[2];i++)
			    {
			   
			       printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

			    }
                    }
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    //set recycle cassette 2
                    delay_mSec(200); //must

                    CASHCODE_NoteAcptr_SetRecycleCassette( 2,0b00000010);  //1f=0b00011111  
                    memset(rcvPkt,'\0',30);
                    rcvPktLen=0;
		    CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
                    for(i=0;i<rcvPkt[2];i++)
	            {
		   
		       printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

	            }
                    //WaitforState(0x21,5);
                    //WaitforState(0x29,10);
                    //WaitforState(0x19,10); 
             
                    
                    while(0x19 != rcvPkt[3])
                    {
		            delay_mSec(200); //must
		            
		            CASHCODE_NoteAcptr_sendPollCmd();
		            memset(rcvPkt,'\0',30);
		            rcvPktLen=0;
			    CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
		            CASHCODE_NoteAcptr_sendACK();
		            for(i=0;i<rcvPkt[2];i++)
			    {
			   
			       printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

			    }
                    }
                    

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    //set recycle cassette 3

                    delay_mSec(200); //must

                    CASHCODE_NoteAcptr_SetRecycleCassette( 3,0b00000011);  //not assigned 0b00011111  
                    memset(rcvPkt,'\0',30);
                    rcvPktLen=0;
		    CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
                    for(i=0;i<rcvPkt[2];i++)
	            {
		   
		       printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

	            }
                    
                    //WaitforState(0x21,5);
                    //WaitforState(0x29,10);
                    //WaitforState(0x19,10); 
                    
                     while(0x19 != rcvPkt[3])
                    {
				    delay_mSec(200); //must
				    
				    CASHCODE_NoteAcptr_sendPollCmd();
				    memset(rcvPkt,'\0',30);
				    rcvPktLen=0;

				    CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);

				    CASHCODE_NoteAcptr_sendACK();

				    for(i=0;i<rcvPkt[2];i++)
				    {
			                printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);
                                    }
                    
                    }
                    

                   
}



void PrintRecycleStatus()
{

               
                   //get recycle status
                   unsigned char rcvPkt[30];
	           int rcvPktLen=0,i=0;
	           memset(rcvPkt,'\0',30);
	           CASHCODE_NoteAcptr_getRecycleCstStatus();
                   memset(rcvPkt,'\0',30);
	           CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen); 
                   CASHCODE_NoteAcptr_sendACK();
                   printf("\n------------------------------------- "); 
                   printf("\n rcvPktLen =%d ",rcvPktLen);
	           
	           for(;i<rcvPkt[2];i++)
	           {
		   
		    printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

	           }
 
                    printf("\n [main()] cassette 1 = %d ", rcvPkt[3]>>7 ); //cassette 1 present
                    printf("\n [main()] cassette 2 = %d ", rcvPkt[5]>>7 ); //cassette 2 present 
                    printf("\n [main()] cassette 3 = %d ", rcvPkt[7]>>7 ); //cassette 3 present 

                    printf("\n [main()] cassette 1 type= %x ", rcvPkt[3] & 0b00011111 ); //cassette 1 present
                    printf("\n [main()] cassette 2 type= %x ", rcvPkt[5] & 0b00011111 ); //cassette 2 present 
                    printf("\n [main()] cassette 3 type= %x ", rcvPkt[7] & 0b00011111 ); //cassette 3 present 
                    
                    
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Enable()
{
                            unsigned char rcvPkt[30];
                            int rcvPktLen=0,i=0;
                            memset(rcvPkt,'\0',30);
                            rcvPktLen=0;
                            noteType NOTE;

			    NOTE.rs_5    = 1;
			    NOTE.rs_10   = 1;
			    NOTE.rs_20   = 1;
			    NOTE.rs_50   = 1;
			    NOTE.rs_100  = 1;
			    NOTE.rs_500  = 1;
			    NOTE.rs_1000 = 1; 
			    NOTE.escrow_on_off = 0;

                            CASHCODE_NoteAcptr_enableBillTypes(NOTE);
                            CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);  
                            int state=0;    
			    CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);  
                            //b2b not ack       
			    if( CASHCODE_RES_ACK != state )
			    {
			            printf("\n[Enable()] Bill enable command not ack by b2b.");
                                    
                                   
			    }
			    else //if b2b ack bill enable command
			    {
				    printf("\n[Enable()] Bill enable successfully ack by b2b.");
                                    
			    }



}

void Disable()
{
                            unsigned char rcvPkt[30];
                            int rcvPktLen=0,i=0;
                            memset(rcvPkt,'\0',30);
                            rcvPktLen=0;
                            noteType NOTE;

			    NOTE.rs_5    = 0;
			    NOTE.rs_10   = 0;
			    NOTE.rs_20   = 0;
			    NOTE.rs_50   = 0;
			    NOTE.rs_100  = 0;
			    NOTE.rs_500  = 0;
			    NOTE.rs_1000 = 0; 
			    NOTE.escrow_on_off = 0;

                            CASHCODE_NoteAcptr_enableBillTypes(NOTE);
                            CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen); 
                            for(i=0;i<rcvPkt[2];i++)
			    {
			   
			       printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

			    } 

                            int state=0;    
			    CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);  
                            //b2b not ack       
			    if( CASHCODE_RES_ACK != state )
			    {
			            printf("\n[Disable()] Bill enable command not ack by b2b.");
                                    
                                   
			    }
			    else //if b2b ack bill enable command
			    {
				    printf("\n[Disable()] Bill enable successfully ack by b2b.");
                                    
			    }


}




void MakePoll()
{
                            unsigned char rcvPkt[30];
                            int rcvPktLen=0,i=0;
                            memset(rcvPkt,'\0',30);
                            rcvPktLen=0;

		            CASHCODE_NoteAcptr_sendPollCmd();
		            CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
		            CASHCODE_NoteAcptr_sendACK();

		            for(i=0;i<rcvPkt[2];i++)
			    {
			          //printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);
                            }

                            
                            printf("\n rcvPkt[3] = %x ",rcvPkt[3]);
                            printf("\n rcvPkt[4] = %x ",rcvPkt[4]);
                            printf("\n rcvPkt[5] = %x ",rcvPkt[5]);  
                            printf("\n\n\n\n");
                         
 
                            if(rcvPkt[3]== 0x14) //idling
                            {
                                 //printf("\n [MakePoll()] Idling");
                            }
                            if(rcvPkt[3]== 0x15) //accepting
                            {
                                 //printf("\n [MakePoll()] Accepting");
                            }
                            if(rcvPkt[3]== 0x17) //stacking
                            {
                                 printf("\n [MakePoll()] Stacking");
                                 //CASHCODE_NoteAcptr_sendACK();
                            }
                            if(rcvPkt[3]== 0x18) //returning
                            {
                                  printf("\n [MakePoll()] returning");
                                  
                            }
                            if(rcvPkt[3]== 0x19) //disabled
                            {
                                //printf("\n [MakePoll()] Disabled");
                            }
                            if(rcvPkt[3]== 0x1C) //rejecting
                            {
                                printf("\n [MakePoll()] Rejecting");
                            }
                            if(rcvPkt[3]== 0x45) //cheated  *
                            {
                                printf("\n [MakePoll()] Cheated");
                            }    
                            if(rcvPkt[3]== 0x81) //stacked  *
                            {
                                 printf("\n [MakePoll()] Stacked");
                                 printf("\n [MakePoll()] Stacked Rs %d .",rcvPkt[4]);
                                 
                                 
                            }

                            if(rcvPkt[3]== 0x82) //returned *
                            {
                                  printf("\n [MakePoll()] returned");
                                  printf("\n [MakePoll()] Returned Rs %d .",rcvPkt[4]);
                                  
                            }
                            /*
                            if(1 == pollflag)
                            {
                                 pollflag=0;
                                 pthread_exit(0);
                            }
                            */


}


//api
int FinishRejectingState()
{
                            
		            unsigned char rcvPkt[30];
                            memset(rcvPkt,'\0',30);
	                    int rcvPktLen=0;
                            int rtcode=0;
                            delay_mSec(200); 
		            CASHCODE_NoteAcptr_sendPollCmd();
                            CASHCODE_NoteAcptr_sendACK();
		            if(SUCCESS == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen) )
                            {
		                         //if it is rejecting state wait
		                         if( 0x1C == rcvPkt[3] )
				         {
				                      printf("\n[FinishRejectingState()] B2B Rejecting State Found.");
                                                      //wait for 5min to get disable state
                                                      rtcode = WaitforState(CASHCODE_RES_DISABLED,300);

				                      //disable state found
				                      if(1 == rtcode)
				                      {
				                           printf("\n[FinishRejectingState()] B2B Rejecting to Disabled State found.");
				                           return SUCCESS;
				                      }
				                      //disable state not yet started
				                      else if(0 == rtcode)
				                      {
				                           printf("\n[FinishRejectingState()] B2B Disabled State not found.");
				                           return FAIL;
				                      }
				                      //b2b communication fail during wait for state
				                      else if(2 == rtcode)
				                      {
				                            printf("\n[FinishRejectingState()] B2B Communication failed.");
				                            return FAIL;
				                      }
		                         }
                                         else if( 0x19 == rcvPkt[3] ) //disable
                                         {
                                                printf("\n[FinishRejectingState()] B2B Disable State Found .");
				                return SUCCESS;
		                         }  
                                         else 
                                         {
                                                printf("\n[FinishRejectingState()] B2B Disable or Rejecting State Not Found rcvPkt[3] =%x.",rcvPkt[3]);
				                return SUCCESS;
		                         }   
 

                            }
                            else
                            {
                                  printf("\n[FinishRejectingState()] B2B Poll status read failed.");
				  return FAIL;
		            }          
                     
		                 
			  

}

//api

int GetExtendedRecycleStaus( int* const NoteQuanity)
{

                                   Wait(3);
                                   unsigned char rcvPkt[30];
	                           int rcvPktLen=0,i=0;
                                   memset(rcvPkt,'\0',30);
                                   *NoteQuanity=0;
                                   //if(SUCCESS == DIssuedisableAllBillCmd())
                                   if(SUCCESS == IssuedisableAllBillCmd())
                                   {
                                        printf("\n [GetExtendedRecycleStaus()] Disable Command Success.");
                                   }
                                   else
                                   {
                                        printf("\n [GetExtendedRecycleStaus()] Disable Command failed. ");
                                        return FAIL;
                                   }

                                   if( 1 == WaitforState(CASHCODE_RES_DISABLED,5)) 
                                   {
					   CASHCODE_NoteAcptr_ExtendedRecycleCstStatus();
					   CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
					   CASHCODE_NoteAcptr_sendACK();
					   for(i=0;i<rcvPkt[2];i++)
					   {
					      printf("\n [GetExtendedRecycleStaus()] rcvPkt[%d] = %x ",i,rcvPkt[i]);
		                           }
		                           *NoteQuanity = rcvPkt[3];
                                            return SUCCESS;
                                   }
                                   else
                                   {
                                         printf("\n [GetExtendedRecycleStaus()] Disable state not found. "); 
                                         return FAIL;
                                   }

}


//CASHCODE_NoteAcptrCreatePollThread()
//int value=0, int noteStatus=0;
//GetNoteDetailInEscrow(&value, &noteStatus)
//g_inhibit_flag =ON;
//g_faretobeaccept=0;
//g_CloseNotePollThreadFlag=1
//Wait(5)

void CreditPoll()
{
                   int value=0;
                   int noteStatus=0;
                   time_t start,end,diff;
  
                   for(;;)
                   {               
		            value=0;
                            noteStatus=0;
                            g_faretobeaccept=0;
                            printf("\n [CreditPoll()] Now going to B2B credit polling...");

                            Enable();
                            CASHCODE_NoteAcptrCreatePollThread();

		            //Start Timer
		            time(&start);
                            while(1)
                            {
					 value=0;
		                         noteStatus=0;
		                         GetNoteDetailInEscrow(&value, &noteStatus);
		                         if(CASHCODE_RES_STACKED == noteStatus)
		                         {
		                                printf("\n [CreditPoll()] Note value =%d.",value);
		                         }
		                         
	                                 
		                         //end Timer
				         time(&end);
		                         diff =end-start;
		                         if(diff >= 15)
		                         {
		                              printf("\n [CreditPoll()] Credit Polling going to stop...");
		                              g_inhibit_flag =ON;
		                              Wait(5);
		                              g_CloseNotePollThreadFlag=1;
                                              Wait(2);
		                              printf("\n [CreditPoll()] Now going to dispense notes...");
                                              
                                              FinishRejectingState();

		                              int totalnotes=0;
		                              GetExtendedRecycleStaus(&totalnotes);
                                              printf("\n [CreditPoll()] Recycle cassette notes : %d ",totalnotes);
		                              dispenseBills(0x18,totalnotes);
		                              break;
		                             

		                        }

                            }//while loop end
		         
                   }//for loop end

}// CreditPoll() end


int DIssuedisableAllBillCmd()
{
	

	    int retry;
	    unsigned char rcvPkt[10]; 
	    memset(rcvPkt,'\0',10);
	    int  rcvPktLen=0;
	    int state = FAIL;

	    noteType NOTE;
	    NOTE.rs_5    = 0;
	    NOTE.rs_10   = 0;
	    NOTE.rs_20   = 0;
	    NOTE.rs_50   = 0;
	    NOTE.rs_100  = 0;
	    NOTE.rs_500  = 0;
	    NOTE.rs_1000 = 0; 
	    #ifdef B2B_ACPTR  
	    NOTE.escrow_on_off = 0;
	    #else
	    NOTE.escrow_on_off = 1;
	    #endif  


	               
		       CASHCODE_NoteAcptr_enableBillTypes(NOTE); 
		       state = -1;   
		       CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
		       CASHCODE_NoteAcptr_analysisRcvdPkt(rcvPkt,&state);
                       int i=0;
                       for(i=0;i<rcvPkt[2];i++)
		       {
			   printf("\n [DIssuedisableAllBillCmd()] rcvPkt[%d] = %x ",i,rcvPkt[i]);
                       } 	
		       if(state == CASHCODE_RES_ACK)
		       {
				    
				printf("\n[IssuedisableAllBillCmd()] Disable all bills Cmd issued success."); 
				state = SUCCESS; 
				
		       }
		       else
		       {
				    
				printf("\n[IssuedisableAllBillCmd()] Disable all bills Cmd issued failed."); 
				state = FAIL;
				
		       }   
	       
	               return(state);

}


int  dispenseBills(int billType, int qty)
{

		     unsigned char rcvPkt[MAX_LEN_OF_PKT]; 
		     memset(rcvPkt,'\0',MAX_LEN_OF_PKT); 
		     int  rcvPktLen=0;
		     unsigned char sbytes[20];
		     memset(sbytes,'\0',20); 
		     unsigned char crc1=0x00, crc2=0x00;
		     int state=0;
                     int i=0;
                     char log[100];
                     memset(log,'\0',100);

		     if(qty <= 0)
		     {
		          printf("\n[dispenseBills()] Quantity not given thats why return from here");
                          return(SUCCESS);
		     }  
           
                     //Step 1:make disable b2b
                     delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100
                     
                     //if( FAIL == DIssuedisableAllBillCmd() )  
                     if( FAIL == IssuedisableAllBillCmd() ) 
	             { 
		         printf("\n[dispenseBills()] B2B disable failed");
                         return FAIL;
	             }
                     else
                     {
                          printf("\n[dispenseBills()] B2B disable success.");
                         
                     }
                     
                     //Step 2:check b2b state
                     delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100
                     unsigned char  PollReplyPacket[30];
                     memset(PollReplyPacket,'\0',30); 
                     unsigned int   PollBufferLength=30;
                     unsigned int   PollReplyPacketlength=0;
                     int rtcode=0 ;
                     rtcode=GetCurrentB2BState(PollReplyPacket,PollBufferLength,&PollReplyPacketlength);
                     if(1==rtcode)
                     {
                          NoteAcptrStatusLog(PollReplyPacket[3]);
                          if(CASHCODE_RES_DISABLED != PollReplyPacket[3])
                          {

		                  rtcode=WaitforState(CASHCODE_RES_DISABLED,WAIT_TIME_FOR_GET_DISABLE);
		                  if(1!=rtcode)
		                  {
		                       printf("\n[dispenseBills()] B2B disable state not found so dispense operation can be done now.");
		                       return FAIL;
		                  }
		                  else
		                   printf("\n[dispenseBills()] B2B disable state found so dispense operation can be done now."); 

                        }
                        else
		          printf("\n[dispenseBills()] B2B disable state found."); 

                     }       
                     else 
                     {
                        printf("\n[dispenseBills()] read b2b current state failed.");
                        return FAIL;
                     }
                       //Step 3: issue dispense command
                       delay_mSec(WAIT_TIME_ISSUE_NEXT_COMMAND); //default tested 100

                       
		       printf("\n[dispenseBills()] Dispense Qauntity : %d",qty);
		       

                       sbytes[0] = CASHCODE_SYNC;  
		       sbytes[1] = DEVICE_ADDRESS; 
		       sbytes[2] = 0x08;                  // Len
		       sbytes[3] = CASHCODE_CMD_DISPENSE; // Cmd 
		       sbytes[4]  = billType;             // for CRIS, It will be 0x18 or 24 dcml.( all bill types)
		       sbytes[5]  = qty;                 
		       GetCRC16(sbytes,6,&crc1,&crc2);
		       sbytes[6]  = crc1;
		       sbytes[7]  = crc2;
                       
                    
                       //send dispense command
                       for(i=0;i< 8; i++)
		       SendSingleByteToSerialPort(g_acptrPortHandler,sbytes[i]);
		       //get dispense command reply
                       rtcode=0;
                       rtcode=CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
                       if(FAIL == rtcode)
                       {
                            printf("\n[dispenseBills()] B2B unload command rply status byte read failed.");
                            return FAIL;
                       }

		       
                        	
                       //if(state != CASHCODE_RES_ACK)
                       if(0x00 !=rcvPkt[3]) //ack get failed
		       {
		                   printf("\n[dispenseBills()] B2B dispense command ack failed.");
		                   //NoteAcptrStatusLog(rcvPkt[3]);
		                   
		                   unsigned char log[100];
		                   int i=0;
		                   for(;i<rcvPkt[2];i++)
		                   {
		                       
		                      printf("\n[dispenseBills()] rcvPkt[%d] : %d",i,rcvPkt[i]);
		                      
		                   }
		                   
				   return (FAIL);
		       }
		       else
		       {
                               //NoteAcptrStatusLog(rcvPkt[3]);
                               //Step 4: Check Dispensing state
                               printf("\n[dispenseBills()] B2B send ack for dispense command.");
                               int rtcode=-1;
                               //wait for dispensing state
                               rtcode=WaitforState(CASHCODE_RES_DISPENSING,WAIT_TIME_FOR_DISPENSING);
                               //dispensing state found
                               if(1 == rtcode)
                               {
                                    printf("\n[dispenseBills()] B2B Dispensing State found.");
                                    //return SUCCESS;
                               }
                               //dispensing state not yet started
                               else if(0 == rtcode)
                               {
                                    printf("\n[dispenseBills()] B2B Dispensing State not found.");
                                    return FAIL;
                               }
                               //b2b communication fail during wait for state
                               else if(2 == rtcode)
                               {
                                    printf("\n[dispenseBills()] B2B Communication failed.");
                                    return FAIL;
                               }
                               
                               //wait for dispensed state
                               rtcode=WaitforState(CASHCODE_RES_DISPESED,WAIT_TIME_FOR_DISPENSED);
                               //dispensing state found
                               if(1 == rtcode)
                               {
                                    printf("\n[dispenseBills()] B2B Dispensed State found.");
                                    return SUCCESS;
                               }
                               //dispensing state not yet started
                               else if(0 == rtcode)
                               {
                                    printf("\n[dispenseBills()] B2B Dispensed State not found.");
                                    return FAIL;
                               }
                               //b2b communication fail during wait for state
                               else if(2 == rtcode)
                               {
                                    printf("\n[dispenseBills()] B2B Communication failed.");
                                    return FAIL;
                               }				


                       }//else block end here
     
}

int main()
{
 
                   OpenCashAcptr(3);
                   CreditPoll();

                   /*
                   unsigned char rcvPkt[30]; 
		   memset(rcvPkt,'\0',30); 
		   int  rcvPktLen=0;
                   int totalnotes=0;
                   int i=0;
		   totalnotes = GetExtendedRecycleStaus();
                   Disable();
                   return 0;
                   while(1)
                   {
                            delay_mSec(200); 
                            CASHCODE_NoteAcptr_sendPollCmd();
		            memset(rcvPkt,'\0',30); 
		            rcvPktLen=0;
			    CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen);
		            CASHCODE_NoteAcptr_sendACK();
                            printf("\n rcvPkt[3] = %x ",rcvPkt[3]);
                            /*
		            for(i=0;i<rcvPkt[2];i++)
			    {
			   
			       printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);

			    }
                            */
                   //}

                   //printf("\n [CreditPoll()] Recycle cassette notes : %d ",totalnotes);
		   //dispenseBills(0x18,7);
                   

                   /*
                   unsigned char rcvPkt[30];
	           int rcvPktLen=0,i=0;
                   int value=0;
                   int noteStatus=0;
                   time_t start,end,diff;
                   for(;;)
                   {               
		            value=0;
                            noteStatus=0;
                            printf("\n [main()] Now going to enable b2b.");
                            Enable();

	                    //CASHCODE_NoteAcptrCreatePollThread();

		            //Start Timer
		            time(&start);
                            while(1)
                            {
				 delay_mSec(200); //must
                                 printf("\n [main()] Going to call Poll.");
                                 MakePoll();
                                 
                                 //end Timer
		                 time(&end);
                                 diff =end-start;
                                 if(diff >= 60)
                                 {
                                      
                                      printf("\n [main()] Now going to disable b2b");
                                      Disable();
                                      int totalnotes=0;
                                      totalnotes = GetExtendedRecycleStaus();
                                      CASHCODE_NoteAcptr_dispenseBills(0x18,(char)totalnotes);
                                      break;
                                     

                                 }

                            }
		         
                   }

                   /*
                    
                   int i=1,j=5;
                   while(1)
                   {
                          
                         if(SUCCESS == DisableSpecificNotes(i*j))
                            printf("\n Disable Success.");
                         else
                            printf("\n Disable failed.");
                         if(200==i)
                           i=1;
                        else
                          i++;
                  }
                  
                   /*
                   unsigned int EnableByte1=0x00;
                   unsigned int EnableByte2=0x00
                   unsigned int EnableByte3=0b01111111; //(bill id 0 to bill id 7)
                   CASHCODE_NoteAcptr_setSecurity( EnableByte1,
                                                   EnableByte2,
                                                   EnableByte3
                                                 );
                   */

                   /*
                   CASHCODE_NoteAcptr_status();
                   unsigned char rcvPkt[30];
	           int rcvPktLen=0,i=0;
	           memset(rcvPkt,'\0',30);
	           CASHCODE_NoteAcptr_getRecycleCstStatus();
                   memset(rcvPkt,'\0',30);
	           if( 1 == CASHCODE_NoteAcptr_statusRead(rcvPkt,&rcvPktLen) )
                   {
                        CASHCODE_NoteAcptr_sendACK();
                        for(;i<rcvPkt[2];i++)
	                {
                            printf("\n rcvPkt[%d] = %x ",i,rcvPkt[i]);
                        }
                   }
                   //PrintRecycleStatus();
                   //SetTempCassettetype();
                   //printf("\n Return Code =%d ",SetRecycleCassette());                      
                   //PrintRecycleStatus();
                   */

}

#endif


#endif 