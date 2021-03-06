#include "atvmapi.h"
   

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Share by note and coin acceptior source 

static volatile int g_totalNoteAmountAccepted;

static volatile int g_totalCoinAmountAccepted;

static volatile int g_ttlNmbrOfNote;

static volatile int g_ttlNmbrOfCoin;

static volatile int g_totalAmountAccepted;

static int g_Fare;

static int g_maxNoOfCash;

static int g_maxNoOfCoin;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  volatile int g_noteAcptrStartFlag;

static  volatile int g_coinAcptrStartFlag;

static  volatile unsigned int g_RuntimeInhibitFlag;

static  volatile unsigned int g_ExternalKillAcptFareThread;

static  volatile unsigned int g_AcptFareThreadStatus;

static  volatile unsigned int g_CoinFullDisable;

static  volatile unsigned int g_NoteFullDisable;

static  int g_acpted_CurrencyDetail[MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN][2];

static  int  g_denomIndex;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static  pthread_mutex_t CloseAcceptFareThreadFlagmutex = PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_AcceptFareEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;

static  pthread_cond_t  g_AcceptFareEnableThreadCond  = PTHREAD_COND_INITIALIZER;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  pthread_mutex_t g_NAULEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;

static  pthread_cond_t  g_NAULEnableThreadCond  = PTHREAD_COND_INITIALIZER;

static  pthread_mutex_t g_CAULEnableThreadmutex = PTHREAD_MUTEX_INITIALIZER;

static  pthread_cond_t  g_CAULEnableThreadCond  = PTHREAD_COND_INITIALIZER;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  CoinsCounter g_CoinsCounter;

static  NotesCounter g_NotesCounter;
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  CoinsCounter g_AllCoinsCounter;

static  NotesCounter g_AllNotesCounter;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static  bool g_NACommitStatus=false;

static  bool g_NACancelStatus=false;

static  bool g_CACommitStatus=false;

static  bool g_CACancelStatus=false;

static  pthread_mutex_t g_NACommitStatusmutex = PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_CACommitStatusmutex = PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_NACancelStatusmutex = PTHREAD_MUTEX_INITIALIZER;

static  pthread_mutex_t g_CACancelStatusmutex = PTHREAD_MUTEX_INITIALIZER;

//static  pthread_mutex_t g_NACommitThreadmutex = PTHREAD_MUTEX_INITIALIZER;

//static  pthread_mutex_t g_CACommitThreadmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_NACompleteThreadmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t  g_NACompleteThreadCond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t g_CACompleteThreadmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t  g_CACompleteThreadCond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t g_NAComplteOperationmutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t g_CAComplteOperationmutex = PTHREAD_MUTEX_INITIALIZER;

static  bool g_NAComplteOperation=false;

static  bool g_CAComplteOperation=false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int activateDevice( int NoteAcceprtorComPort,int EscrowClearanceMode )
{

         
           //To activate all  cash/coin devices to be ready for accepting cash and coin. It will return values may be 0 ??? not ready, 1- ready.

	   int day=0,mon=0,yr=0,hr=0,min=0,sec=0;
	   char *destStr=NULL,*file=NULL;
	   int ret1=-1,ret2=-1, ret=-1,ret3=-1;
	   int noteQtyInRcyclCst = 0;
	   int noteAcptrPortNmbr=-1, CCTALKPortNumber=-1;
 
           int TransTimeout=0;

           #ifdef B2B_TRAP_GATE
	   //1jan 2013 Malay Add
	   int DSCUPortNo=-1;
           #endif

           //-1 return must if ini file settings wrong or read error from ini file

           //Note Acceptor Port Number
           #if defined(NOTE_ACCEPTOR) && (defined(B2B_NOTE_ACCEPTOR) || defined(JCM_NOTE_ACCEPTOR))
	   //noteAcptrPortNmbr = PortId; 
           #endif


           //Coin Acceptor Port Number
           #ifdef CCTALK_DEVICE
	   CCTALKPortNumber = GetCCTALKPortNo();
           #endif

          
	   //special device   
	   #ifdef B2B_TRAP_GATE

	   //1jan 2013 Malay Add
	   DSCUPortNo=GetDSCUPortNo();

	   //1jan 2013 Malay Add Get DSCU Threshold value
	   g_DropNotesNumber=GetDSCUThreshold();

           #endif

           
           //Print Console trans time out
           //printf("\n[Mega Designs Pvt. Ltd.] Atvm Trans Time =%d Seconds\n",TransTimeout);

           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       
          day=0,mon=0,yr=0,hr=0,min=0,sec=0;
          getDateTime(&day,&mon,&yr,&hr,&min,&sec);
          char *logdestStr = (char*)malloc(8096);
          sprintf(logdestStr ,"/home1/megalog/Mega_ConfigError_Dt_%d%d%d_Tm_%d_%d.log",yr,mon,day,hr,min,sec);

          #if defined(NOTE_ACCEPTOR) && (defined(B2B_NOTE_ACCEPTOR) || defined(JCM_NOTE_ACCEPTOR))
          
          /*
          if( READ_INI_FAIL == noteAcptrPortNmbr )
          {
                 
                 DefaultwriteFileLog(logdestStr ,"[activateDevice() Note Acceptor Port Number] read atvm.ini failed."); 
                 free(logdestStr);
                 return (-1); 

          }
          */
          
          #endif

          #ifdef CCTALK_DEVICE
          if( READ_INI_FAIL == CCTALKPortNumber  )
          {
                
             
                DefaultwriteFileLog(logdestStr,"[activateDevice() CCTALK Port Number] read atvm.ini failed."); 
                free(logdestStr);
                return (-1); 

          }
          #endif


          #ifdef B2B_TRAP_GATE

          if( READ_INI_FAIL == DSCUPortNo  )
          {
                
                DefaultwriteFileLog(logdestStr,"[activateDevice() Trap Gate Port Number ] read atvm.ini failed."); 
                free(logdestStr);
                return (-1); 

          }

          #endif
         
          ////////////////////////////////////////////////////////////////////////////////////

          /*

          //test equal port number check
        
          #if defined(NOTE_ACCEPTOR) && defined(B2B_NOTE_ACCEPTOR) &&  defined(B2B_TRAP_GATE) && !defined(CCTALK_DEVICE)

          if ( noteAcptrPortNmbr == DSCUPortNo )
          {

	        DefaultwriteFileLog(logdestStr,"[activateDevice() B2B with Trap Gate Block ] B2B and Trap Gate Port Number same input."); 
                free(logdestStr);
                return (-1); 

          }

          #endif

          #if defined(NOTE_ACCEPTOR) && defined(CCTALK_DEVICE) &&  defined(B2B_NOTE_ACCEPTOR) && defined(B2B_TRAP_GATE)

          if ( ( noteAcptrPortNmbr == DSCUPortNo ) || ( CCTALKPortNumber == DSCUPortNo ) || 
               ( CCTALKPortNumber == noteAcptrPortNmbr ) )
          {

	        DefaultwriteFileLog(logdestStr,"[activateDevice()] B2B and Trap Gate and Coin acceptor Port Number same input.");
                free(logdestStr); 
                return (-1); 

          }

          #endif

          #if defined(NOTE_ACCEPTOR) &&  defined(CCTALK_DEVICE) && defined(JCM_NOTE_ACCEPTOR)

          if ( noteAcptrPortNmbr == CCTALKPortNumber )
          {
	        DefaultwriteFileLog(logdestStr,"[activateDevice() JCM Block] Note acceptor and Coin acceptor Port Number same input."); 
                free(logdestStr);
                return (-1); 
          }
          else
          {

                //DefaultwriteFileLog(logdestStr,"[activateDevice() JCM Block] Note acceptor and Coin acceptor Port Number input ok."); 
                free(logdestStr); 
                

          }

          #endif

          */

          #if defined(NOTE_ACCEPTOR) && defined(B2B_NOTE_ACCEPTOR) && defined(CCTALK_DEVICE)

          if ( NoteAcceprtorComPort == CCTALKPortNumber )
          {

	        DefaultwriteFileLog(logdestStr,"[activateDevice() B2B with CCTALK] B2B and CCTALK Port Number same input.");
                printf("\n[activateDevice() B2B with CCTALK] B2B and CCTALK Port Number same input.");
                free(logdestStr);
                return (-1); 

          }

          #endif

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 
          //Set ASCRM General LOG File path
         
          unsigned char LogFilePath[8192];
      
          memset(LogFilePath,'\0',8192);
           
          day=0,mon=0,yr=0,hr=0,min=0,sec=0;

	  getDateTime(&day,&mon,&yr,&hr,&min,&sec);

	  destStr = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

          file = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

	  sprintf(destStr,"/Mega_Dt_%d%d%d_Tm_%d_%d.log",yr,mon,day,hr,min,sec);

	  if( -1 == GetLogFilePath(file) )
	  {

	          printf("\n/[Mega Designs Pvt. Ltd.] Error Create atvm log file.");

	  } 
          else
          {

		 //Copy trans logfile name
                 strcat(file,destStr);
		 
                 strcat(LogFilePath,file);
                 
                 SetGeneralFileLogPath( LogFilePath );

                 printf("\n[Mega Designs Pvt. Ltd.] successfully created general log file.");
                 
         }
        
         if(NULL!= destStr)
         {
           free(destStr);
         }
        
         if(NULL!=file)
         {
	   free(file);
         }


         //Set ASCRM Device fault LOG File path
	 memset(LogFilePath,'\0',8192);
 
         day=0,mon=0,yr=0,hr=0,min=0,sec=0;
 
	 getDateTime(&day,&mon,&yr,&hr,&min,&sec);

	 destStr = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

	 file = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

	 sprintf(destStr,"/Mega_Fault_Dt_%d%d%d.log",yr,mon,day); // 17-Nov-12

	 if( -1 ==GetLogFilePath(file))
	 {

	         printf("\n[Mega Designs Pvt. Ltd.] Error Create fault log file.");

	 } 
         else
         {
		 
                 //Copy Device fault logfile name
                 strcat(file,destStr);

		 strcat(LogFilePath,file); 

                 SetMachineFaultFileLogPath( LogFilePath );

		 printf("\n[Mega Designs Pvt. Ltd.] successfully created fault log file.");
         }
            
         if(NULL!= destStr)
         {
           free(destStr);
         }
        
         if(NULL!=file)
         {
	   free(file);
         }

         //set api time out test logfile path if timeout macro defined
         #ifdef CRISAPI_TIMEOUT_TEST

         memset(LogFilePath,'\0',8192);
           
         day=0,mon=0,yr=0,hr=0,min=0,sec=0;

	 getDateTime(&day,&mon,&yr,&hr,&min,&sec);

	 destStr = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

         file = (char*)malloc(MAX_STR_SIZE_OF_TRNSLOG);

	 sprintf(destStr,"/MegaApiTimeoutTest_Dt_%d%d%d_Tm_%d_%d.log",yr,mon,day,hr,min,sec);

         if( -1 ==GetLogFilePath(file))
	 {

	         printf("\n[Mega Designs Pvt. Ltd.] Error Create API Timeout Test log file.");

	 } 
         else
         {
		 
                 //Copy Device fault logfile name
                 strcat(file,destStr);

		 strcat(LogFilePath,file); 

                 SetAPITimeoutFileLogPath( LogFilePath );

		 printf("\n[Mega Designs Pvt. Ltd.] successfully created api timeout test log file.");

         }
            
         if(NULL!= destStr)
         {
           free(destStr);
         }
        
         if(NULL!=file)
         {
	   free(file);
         }

         #endif

         //Now fully activate device entry
         writeAPITimeoutLog("[activateDevice()] Entry.");

         writeFileLog("[activateDevice()] Entry.");
         

         //Get coin and note acceptor port read and write operation from linux

         #if defined(NOTE_ACCEPTOR) && (defined(B2B_NOTE_ACCEPTOR) || defined(JCM_NOTE_ACCEPTOR))

         ret1 = OpenCashAcptr(NoteAcceprtorComPort);  //Initialize Note Acceptor

         #endif

         #if defined(NOTE_ACCEPTOR) && defined(BNA_NOTE_ACCEPTOR)

         ret1 = OpenCashAcptr();  //Initialize Note Acceptor

         #endif

         ////////////////////////////////////////////////////////////////////////////////////
         
         #if defined(CCTALK_DEVICE)
         
         //port open must be done here
         int cctalkrtcode=-1;

         cctalkrtcode=IsCCTALKPortOpen();

         //CCTALK Port not opened
         if(0==cctalkrtcode) 
         {

                cctalkrtcode=-1;

                //Open CCTALK Port 

                cctalkrtcode=ActivateCCTALKPort(CCTALKPortNumber);

                if( 0 == cctalkrtcode )
		{

                          ret2 = FAIL;	

		          writeFileLog("[activateDevice()] CCTALK Open Port failed.");
		          

		}
		else if( 1 == cctalkrtcode )
		{  

		          ret2 = SUCCESS ;	
   
		          writeFileLog("[activateDevice()] CCTALK Open Port opened successfully.");

		}
                

         }
         else if(1 == cctalkrtcode) //CCTALK Port opened already
         {

                      
                      writeFileLog("[activateDevice()] CCTALK Com Port already opened successfully.");

                      ret2 = SUCCESS;  //CCTALK port opened successfully


         }
 
         #endif
        
         //////////////////////////////////////////////////////////////////////////////////////


         #if !defined(CCTALK_DEVICE)

         //by default CCTALK port open return code set to 1(success) if CCTALK not defined in atvmdevice headers
         ret2 = SUCCESS;  
         
         #endif


         ///////////////////////////////////////////////////////////////////////////////////////

         #if defined(CCTALK_DEVICE) && defined(COIN_ACCEPTOR)

         if( 1 == GetCoinAcptrStatus() )
         {


                                  writeFileLog("[activateDevice()] Coin Acceptor Simple Poll Success.");

                                  delay_mSec(200);
 
				  //make disable acceptance in coin acceptor
				  int rtcode=-1;

				  rtcode=DisableAllCoins();

				  if( SUCCESS == rtcode )
				  {

				          writeFileLog("[activateDevice()] Successfully Disable Coin Acceptor.");


				  }
				  else if( FAIL == rtcode )
				  {

				          writeFileLog("[activateDevice()] Failed to Disable Coin Acceptor.");
                                      

				  }
      
                                  ret2 = SUCCESS;

          }
          else 
          {

                 writeFileLog("[activateDevice()] Coin Acceptor Simple Poll failed.");
                 ret2 = FAIL;  

          }     
        
          #endif
          
         
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         #ifdef B2B_TRAP_GATE

         //malay 1 jan 2013 add
	 if(0==IsDropandSealUnitPortOpen())
	 {

               ret3 = OpenDropandSealUnit(DSCUPortNo);   //Initialize Bill to Bill Trap Gate

               if(1==ret3)
               {
                    writeFileLog("[activateDevice()] Trap gate open successfully.");
               }
               else
               {
                    writeFileLog("[activateDevice()] Trap gate already open failed."); 
                    unsigned char log[100];
                    memset(log,'\0',100);
                    sprintf(log,"[activateDevice()] Trap Gate open return code: %d .",ret3);
                    writeFileLog(log);

               }


         } 
         else
         {
            
                  writeFileLog("[activateDevice()] Trap gate already open successfully.");

         }

         #endif

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         unsigned char log[100];
         noteQtyInRcyclCst=0;
 
         //Get Note Acceptor current recycle quantity

         #ifdef NOTE_ACCEPTOR

         #ifdef B2B_NOTE_ACCEPTOR
         if( CASHCODE_RES_DISABLED == ret1  )
         #endif
         #ifdef JCM_NOTE_ACCEPTOR
         if( 1 == ret1 )
         #endif
         #ifdef BNA_NOTE_ACCEPTOR
         if( 1 == ret1 )
         #endif
	 {

		  if( 1 == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) )
		  {
		     memset(log,'\0',100);
 
                     sprintf(log,"[activateDevice()] Note present in recycle cassette: %d.",noteQtyInRcyclCst);

                     writeFileLog(log);

                     
		  }//if( 1 == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) )
                  else
                  {
                     memset(log,'\0',100); 
                     sprintf(log,"[activateDevice()] Unable to find any note in note acceptor");
                     writeFileLog(log);

                  }

         }

         #endif
       
         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //Note Acceptor And Coin Acceptor Active [11]

         #if defined(NOTE_ACCEPTOR) && defined(CCTALK_DEVICE)

         #if defined(B2B_NOTE_ACCEPTOR) &&  defined(B2B_TRAP_GATE)
	    if( (( CASHCODE_RES_DISABLED == ret1) && ( DSCU_SUCCESS == ret3 )) && (SUCCESS ==ret2) && (0 == noteQtyInRcyclCst) )
         #endif
         #if defined(B2B_NOTE_ACCEPTOR) &&  !defined(B2B_TRAP_GATE)
	    if( ( CASHCODE_RES_DISABLED == ret1 ) && (SUCCESS == ret2) && (0 == noteQtyInRcyclCst) )
         #endif
         #if defined(JCM_NOTE_ACCEPTOR)
	    if( (( 1 == ret1 )) && ( SUCCESS == ret2 ) && ( 0 == noteQtyInRcyclCst ) )
         #endif
         #if defined(BNA_NOTE_ACCEPTOR)
	    if( (( 1 == ret1 )) && ( SUCCESS == ret2 ) && ( 0 == noteQtyInRcyclCst ) )
         #endif
	 {
	      writeFileLog("[activateDevice() Exit] Device Activated Successfully");
              return 0; //Device Connected Successfully [return code: 0 ]
	 }

         #if defined(B2B_NOTE_ACCEPTOR) &&  defined(B2B_TRAP_GATE)
	   else if( ((ret1 == CASHCODE_RES_DISABLED) && (DSCU_SUCCESS == ret3 )) && (SUCCESS == ret2) && (noteQtyInRcyclCst > 0) )
         #endif
         #if defined(B2B_NOTE_ACCEPTOR) &&  !defined(B2B_TRAP_GATE)
	    else if( (CASHCODE_RES_DISABLED == ret1) && ( SUCCESS ==ret2 ) && (noteQtyInRcyclCst>0) )
         #endif
         #if defined(JCM_NOTE_ACCEPTOR)
	    else if( (( 1 == ret1 )) && ( SUCCESS == ret2 ) && (noteQtyInRcyclCst > 0) )
         #endif
         #if defined(BNA_NOTE_ACCEPTOR)
	    else if( (( 1 == ret1 )) && ( SUCCESS == ret2 ) && (noteQtyInRcyclCst > 0) )
         #endif
	 {
	      writeFileLog("[activateDevice() Exit] Device Activated Successfully with note present in note escrow.");
               
              printf("[activateDevice()] Escrow Clearance Mode: %d.",EscrowClearanceMode);

              if( 1 == EscrowClearanceMode )
              {
                        writeFileLog("[activateDevice()] Escrow Clearance Mode is defined.");

                        //Send money to collection bin
                        if( true == defaultCommit() )
                        {
                            writeFileLog("[activateDevice()] Escrow Clearance successfully done.");
                            noteQtyInRcyclCst = 0;
                            return 0; //Device Connected Successfully without escrow notes [return code: 0 ]
                        }
                        else
                        {
                            writeFileLog("[activateDevice()] Escrow Clearance failed to do.");
                            return 7; //Device Connected Successfully with escrow clearance failed [return code: 0 ]
                        }

              }//if( 1 == EscrowClearanceMode )
              else
              {
                   writeFileLog("[activateDevice()] Escrow Clearance Mode Not defined.");
                   return 1; //Device Connected Successfully with few note escrowed [return code: 1]
              }

             
	 }
	 else
         {

	      writeFileLog("[activateDevice() Exit] Device Activate failed!!.");

              ///////////////////////////////////////////////////////////////////////////////////////

              #if defined(NOTE_ACCEPTOR) 

              //close note and coin acceptor device port if opened successfully
              #if  defined(B2B_NOTE_ACCEPTOR) 

              if( CASHCODE_RES_DISABLED == ret1 )
              {
                  DeactivateB2BUnit();
              }
              #endif

              #if  defined(B2B_TRAP_GATE) 
              if( DSCU_SUCCESS == ret3 )
              {
                  DeactivateTrapGate();
              }
              #endif

              #if  defined(BNA_NOTE_ACCEPTOR)
              if( 1 == ret1 )
              {


              }
              #endif

              #if  defined(JCM_NOTE_ACCEPTOR)
              if( 1 == ret1 )
              {
                   Deactivate_JCM_NoteAcptr();
              }
              #endif

              #endif
 
              #if defined(CCTALK_DEVICE)
              if(SUCCESS == ret2)
              {
                   DeActivateCCTALKPort();
              }
              #endif

              ////////////////////////////////////////////////////////////////////////////////////////

              return 3; //Device activated failed communication failure[return code: 3 ]

         }
         
         #endif

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //Note Acceptor InActive And Coin Acceptor Active [01]

         #if !defined(NOTE_ACCEPTOR) && defined(CCTALK_DEVICE)

         if(  SUCCESS == ret2  )
         {
	      writeFileLog("[activateDevice() Exit] Coin Acceptor Activated Successfully");
              return 1;
	 }
         else if(  SUCCESS != ret2  )
         {

	      writeFileLog("[activateDevice() Exit] Coin Acceptor Activate failed!!.");
              return 0; 

         }
         
         #endif

         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //Note Acceptor Active And Coin Acceptor InActive [10]

         #if defined(NOTE_ACCEPTOR) && !defined(CCTALK_DEVICE)

         #if defined(B2B_NOTE_ACCEPTOR) &&  defined(B2B_TRAP_GATE)
	    if( (( CASHCODE_RES_DISABLED == ret1) && ( DSCU_SUCCESS == ret3 ))  && (0 == noteQtyInRcyclCst) )
         #endif
         #if defined(B2B_NOTE_ACCEPTOR) &&  !defined(B2B_TRAP_GATE)
	    if( ( CASHCODE_RES_DISABLED == ret1 )  && (0 == noteQtyInRcyclCst) )
         #endif
         #if defined(JCM_NOTE_ACCEPTOR)
	    if( (( 1 == ret1 ))  && ( 0 == noteQtyInRcyclCst ) )
         #endif
         #if defined(BNA_NOTE_ACCEPTOR)
	    if( (( 1 == ret1 ))  && ( 0 == noteQtyInRcyclCst ) )
         #endif
	 {
	      writeFileLog("[activateDevice() Exit] Note Acceptor Activated Successfully");
              return 0;
	 }

         #if defined(B2B_NOTE_ACCEPTOR) &&  defined(B2B_TRAP_GATE)
	   else if( ((CASHCODE_RES_DISABLED == ret1) && ( DSCU_SUCCESS == ret3))  && (noteQtyInRcyclCst > 0) )
         #endif
         #if defined(B2B_NOTE_ACCEPTOR) &&  !defined(B2B_TRAP_GATE)
	   else if( (CASHCODE_RES_DISABLED == ret1)  && (noteQtyInRcyclCst > 0) )
         #endif
         #if defined(JCM_NOTE_ACCEPTOR)
	   else if( ( 1 == ret1 ) && (noteQtyInRcyclCst > 0) )
         #endif
         #if defined(BNA_NOTE_ACCEPTOR)
	   else if( ( 1 == ret1 ) && (noteQtyInRcyclCst > 0) )
         #endif
	 {
	      writeFileLog("[activateDevice() Exit] Note Acceptor Activated Successfully with note present in note escrow.");
              
              printf("[activateDevice()] Escrow Clearance Mode: %d.",EscrowClearanceMode);

              if( 1 == EscrowClearanceMode )
              {
                        writeFileLog("[activateDevice()] Escrow Clearance Mode is defined.");

                        //Send money to collection bin
                        if( true == defaultCommit() )
                        {
                            writeFileLog("[activateDevice()] Escrow Clearance successfully done.");
                            noteQtyInRcyclCst = 0;
                            return 0; //Device Connected Successfully without escrow notes [return code: 0 ]
                        }
                        else
                        {
                            writeFileLog("[activateDevice()] Escrow Clearance failed to do.");
                            return 7; //Device Connected Successfully with escrow clearance failed [return code: 0 ]
                        }

              }//if( 1 == EscrowClearanceMode )
              else
              {
                   writeFileLog("[activateDevice()] Escrow Clearance Mode Not defined.");
                   return 1; //Device Connected Successfully with few note escrowed [return code: 1]
              }

              
	 }
	 else
         {

	      writeFileLog("[activateDevice() Exit] Note Acceptor Activate failed!!.");

              return 3 ; 

         }
         
         #endif

         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
         //Note Acceptor Active And Coin Acceptor InActive [00]

         #if !defined(NOTE_ACCEPTOR) && !defined(CCTALK_DEVICE) 

         writeFileLog("[activateDevice() Exit]  Error No note or coin Acceptor Activate present in this system!!.");

         return 0;
	
         #endif

         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////




}//activateDevice() end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int deactivateDevice()
{



                         writeFileLog("[deactivateDevice()] Entry.");

			 unsigned int rtcode=-1; 
			 
			 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			 #if defined(NOTE_ACCEPTOR) && !defined(CCTALK_DEVICE) 

			 //Deactivate Note Acceptor
			 rtcode=DeactivateNoteAcptr();

			 if(1!=rtcode)
			 {
			       writeFileLog("[deactivateDevice() Exit] Note acptr deactivate failed!!.");
			       return 0;
			 }
			 else
			 {
			       writeFileLog("[deactivateDevice() Exit] Note acptr deactivate successfully executed.");

			 }

			 #endif

			 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			 #if defined(CCTALK_DEVICE) &&  !defined(NOTE_ACCEPTOR)

                         rtcode=-1;

			 rtcode=DeActivateCCTALKPort();

			 if(1!=rtcode)
			 {
			       writeFileLog("[deactivateDevice() Exit] CCTALK port close operation failed.!!");
			       return 0;
			 }
			 else
			 {
			    
			      writeFileLog("[deactivateDevice() Exit] CCTALK port close operation successfully executed.");

			 }

			 #endif
		
			 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			 #ifdef B2B_TRAP_GATE

			 //Deactivate Trap Gate 
			 rtcode=DeactivateTrapGate();

			 if(1!=rtcode)
			 { 
			       writeFileLog("[deactivateDevice()] Note Acptr Trap Gate deactivate failed!!.");
			       return 0;
			 }
			 else
			 {
			       writeFileLog("[deactivateDevice()] Note Acptr Trap Gate deactivate successfully executed.");

			 }

			 #endif

			 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			 return 1;




}//deactivateDevice() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Return values are 
// 0001-  decimal 1,  Ready
// 0010 - decimal 2 , Cash acceptor device not ready
// 0100 - decimal 4 , Coin acceptor device not ready
// 1000 ??? decimal 8 , both device fault

int deviceStatus(int TransTime)
{

	
         writeFileLog("=========================================================================");  
         writeFileLog("[deviceStatus()] Entry.");

	 int noteAcptrRet=0;
	 int coinAcptrRet=0;
	 int DSCURet=0;

         char Byte0=0b00000101,
              Byte1=0x00,
              Byte2=0x00,
              Byte3=0x00;
         int  rtcode=0x00;


	 //get note acceptor status
         #if defined(NOTE_ACCEPTOR)

         noteAcptrRet = GetNoteAcptrStatus();

         #endif
         
         //get coin acceptor status
         #if defined(COIN_ACCEPTOR)

	 coinAcptrRet = GetCoinAcptrStatus();
         
         #endif

         #if !defined(COIN_ACCEPTOR)
	 coinAcptrRet = SUCCESS;
         #endif

    
         #ifdef B2B_TRAP_GATE

	 //Malay 1 Jan 2013 add 
	 DSCURet=AtvmGetDSCUStatus();
         if(1==DSCURet)
         {
               writeFileLog("[deviceStatus()] Trap Gate is ready.");
         }
         else if(2==DSCURet)
         {
              writeFileLog("[deviceStatus()] Trap Gate replied but not in idle (No operation can be done).");
         }
         else if(3==DSCURet)
         {
              writeFileLog("[deviceStatus()] Trap Gate not replied (No operation can be done).");
         }
	
         #endif


         
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
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

	        writeFileLog("[deviceStatus() Exit] Note acptr and coin acptr is ready.");
                writeFileLog("=========================================================================");  
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
		   
                 writeFileLog("[deviceStatus() Exit] Note acceptor fault.");
                 writeFileLog("=========================================================================");  
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
		 writeFileLog("[deviceStatus() Exit] Coin acceptor fault.");
                 writeFileLog("=========================================================================");  
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
		 writeFileLog("[deviceStatus() Exit] Note acceptor and Coin acceptor fault.");
                 writeFileLog("=========================================================================");  
		 return(8); // ret code as per CRIS

	 }

         #endif
          
         */

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         //Note Acceptor Active Coin Acceptor InActive

         #if defined(NOTE_ACCEPTOR) && !defined(COIN_ACCEPTOR)

         #if defined(B2B_NOTE_ACCEPTOR)
	 if( ( CASHCODE_RES_DISABLED == noteAcptrRet ) )
         #endif
         #if defined(JCM_NOTE_ACCEPTOR) 
	 if( 1 == noteAcptrRet )
         #endif
         #if defined(BNA_NOTE_ACCEPTOR) 
	 if(  1 == noteAcptrRet )
         #endif
	 {
	        writeFileLog("[deviceStatus() Exit] Note Acceptor is  ready.");
                writeFileLog("=========================================================================");  
                
                ///////////////////////
                Byte0=0b00000111;
                rtcode=Byte0;
                return(rtcode); 
                ///////////////////////

                //return 1; 

         }

         

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
		   
                 writeFileLog("[deviceStatus() Exit] Note acceptor fault.");
                 writeFileLog("=========================================================================");  
		 ///////////////////////
                 Byte0=0b00000101;
                 rtcode=Byte0;
                 return(rtcode); 
                 ////////////////////////
                 //return 0;
	 }
         
         #endif

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         //Note Acceptor InActive Coin Acceptor InActive

         #if !defined(NOTE_ACCEPTOR) &&  defined(COIN_ACCEPTOR)

         if( SUCCESS == coinAcptrRet )
         {
	        writeFileLog("[deviceStatus() Exit] Coin acceptor is ready.");
                writeFileLog("=========================================================================");  
                return(1); 

	 }

         else if(  FAIL == coinAcptrRet )
         {
		 writeFileLog("[deviceStatus() Exit]  Coin acceptor fault.");
                 writeFileLog("=========================================================================");  
		 return(4); 

	 }


         #endif



}//deviceStatus() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool defaultCommit()
{


	 writeFileLog("[defaultCommit()] Entry.");

         int value=0, noteStatus=0;
	 int retry=1,try=1;
	 int ret=0, ret1=0, ret2=0,ret3=0; 
	 int noteQtyInRcyclCst=0;
         ret1 = FAIL; 
	 ret2 = FAIL; 
	 ret  = FAIL;
         unsigned char log[100];
         memset(log,'\0',100); 
        
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////
         //Disable all acceptance for money accept machine

         #ifdef COIN_ACCEPTOR

         //Now Disable all note and coin acceptor
         delay_mSec(200);

         writeFileLog("[defaultCommit()] Before Disable note and coin section.");

         if( SUCCESS == DisableAllCoins() )
         {
              writeFileLog("[defaultCommit()] Disable coin acceptor success.");
         }
         else
         {
              writeFileLog("[defaultCommit()] Disable coin acceptor failed.");
         }

         #endif

         #ifdef NOTE_ACCEPTOR

		 #ifdef B2B_NOTE_ACCEPTOR
		 if(SUCCESS == IssuedisableAllBillCmd())
		 {
		      writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] Disable note acceptor success.");
		 }
		 else
		 {
		      writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] Disable note acceptor failed.");
		 }
		 #endif

                

         #endif

         writeFileLog("[defaultCommit()] After Disable note and coin section.");
 
        
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //Coin Escrow Accept Operation
         #ifdef COIN_ESCROW

         //Accept Coin
	 writeFileLog("[defaultCommit()] Going to accept coin...");

         for(retry = 1; retry<=2;retry++) 
	 { 

			 ret1 = AcceptFromCoinEscrow();

			 if(1==ret1)
		         {
				    writeFileLog("[defaultCommit()] Accept coin successfully.");
				    break;
		         }
			 else
		         {
				    writeFileLog("[defaultCommit()] Accept coin failed.");
				    memset(log,'\0',100); 
				    sprintf(log,"[defaultCommit()] Accept Coin Retry : %d.",retry);
				    writeFileLog(log);
				    continue;
		         }


         }

         #endif

         #if !defined(COIN_ESCROW)
 
         ret1=SUCCESS;

         #endif

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //Note Acceptor Unload Operation
         #ifdef  NOTE_ACCEPTOR
                  
                  ///////////////////////////////////////////////////////////////////////////////////////
                  //Step 1: Get current notes number from escrow
                  noteQtyInRcyclCst = 0;

		  if( SUCCESS == GetNoteDetailsInRecycleCst(&noteQtyInRcyclCst) )
		  {

				 memset(log,'\0',100); 
				 sprintf(log,"[defaultCommit()] Note present in recycle cassette: %d.",noteQtyInRcyclCst);
				 writeFileLog(log);

		  }
		  else
		  {

				 writeFileLog("[defaultCommit()] Read recycling cassette status read failed.");
				 return(FAIL);

		  }  
                  
                  writeFileLog("[defaultCommit()] Going to accept notes from note acceptor device.");

                  //////////////////////////////////////////////////////////////////////////////////////////
                  //Step 2: Accept current notes from escrow to vault
                  #ifdef JCM_NOTE_ACCEPTOR
		 
                  writeFileLog("[defaultCommit() JCM_NOTE_ACCEPTOR] Before Unload Operation Calling.");

                  int jcmrtcode=-1;

                  for(retry = 1; retry<=2;retry++) 
                  {

                         jcmrtcode=JCM_DefaultCommit();

                         if(1==jcmrtcode)
		         {

                                writeFileLog("[defaultCommit() JCM_NOTE_ACCEPTOR] Unload operation successfully executed.");
                                ret3 = SUCCESS;

                                break;

                         }   
                         else
                         {

                                writeFileLog("[defaultCommit() JCM_NOTE_ACCEPTOR] Accept note failed.");

				memset(log,'\0',100); 

				sprintf(log,"[defaultCommit() JCM_NOTE_ACCEPTOR]  Accept note Retry : %d.",retry);
				writeFileLog(log);

                                ret3 = FAIL;

                                delay_mSec(100);

				continue;


                         }


                  }

                  writeFileLog("[defaultCommit() JCM_NOTE_ACCEPTOR] After Unload Operation Completed.");

                  #endif

                  //////////////////////////////////////////////////////////////////////////////////////////////

                  #ifdef B2B_NOTE_ACCEPTOR

		  if(noteQtyInRcyclCst > 20) //max 20 note can b acprd at a time.
		  { 
		      noteQtyInRcyclCst = 20;
		  }
                  
		  //Accept Notes
                  writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] Before AcceptFromEscrow().");

		  for(retry = 1; retry<=2;retry++) 
		  {  
		

				ret2 = AcceptFromEscrow(1,noteQtyInRcyclCst); 

				if(SUCCESS==ret2)
				{   

					       writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] unload command issue success.");

					       writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] Before GetAcceptedNoteDetail().");
                                                  
		                              
					       ret3 = GetAcceptedNoteDetail(&value,&noteStatus); 

					       if(ret3 == SUCCESS)
                                               {
						    writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] Accept notes successfully.");

                                               }
					       else
					       {	 
                                                    writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] Accept notes failed.");
		                               }

					       writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] After GetAcceptedNoteDetail().");
					       break;
				}
				else 
				{

					     writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] unload command failed to execute.");
					     memset(log,'\0',100); 
					     sprintf(log,"[defaultCommit() B2B_NOTE_ACCEPTOR] Unload Retry : %d.",retry);
					     writeFileLog(log);
					     continue;

				}

			       
					  
		} // End for: retry
		                
		writeFileLog("[defaultCommit() B2B_NOTE_ACCEPTOR] After AcceptFromEscrow().");

		#endif

                writeFileLog("[defaultCommit()] after accept notes.");
                //Now again read recycle quantity
                int CurrentCastQuanity=0;
                GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
                memset(log,'\0',100);
                sprintf(log,"[defaultCommit()] After Unload Recycle Quantity = %d.",CurrentCastQuanity);
                writeFileLog(log);

          #endif

          #if !defined(NOTE_ACCEPTOR)
          writeFileLog("[defaultCommit()] No Note Acceptor device is present in ATVM system.");
          ret3 = SUCCESS ;
          #endif

          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  if(( SUCCESS == ret1  ) && ( SUCCESS == ret3  ))
          {
	     writeFileLog("[defaultCommit() Exit] Accept notes and coin finished with success.");
             return true;
          }
	  else
          {
	     writeFileLog("[defaultCommit() Exit] Accept notes and coin finished with failed.");
             return false;
	  }


	   
}//defaultCommit() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool defaultCancel()
{

                  
	         writeFileLog("[defaultCancel()] Entry.");

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

		 writeFileLog("[defaultCancel()] Before Disable note and coin section.");

		 if(SUCCESS == DisableAllCoins() )
		 {
		      writeFileLog("[defaultCancel()] Disable coin acceptor success.");
		 }
		 else
		 {
		      writeFileLog("[defaultCancel()] Disable coin acceptor failed.");
		 }

		 #endif
 
                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		 #ifdef NOTE_ACCEPTOR

			 #ifdef B2B_NOTE_ACCEPTOR

			 if(SUCCESS == IssuedisableAllBillCmd())
			 {
			      writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Disable note acceptor success.");
			 }
			 else
			 {
			      writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Disable note acceptor failed.");
			 }

			 #endif

                        
		 #endif

		 writeFileLog("[defaultCancel()] After Disable note and coin section.");
	 
		 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		 #ifdef COIN_ESCROW

		 //Reject Coin
		 writeFileLog("[defaultCancel()] Going to reject coin.");

		 writeFileLog("[defaultCancel()] Before RejectFromCoinEscrow().");

		 for(retry = 1; retry<=2;retry++) 
		 {

			 ret1 = RejectFromCoinEscrow(); 

			 if(SUCCESS==ret1)
		         {
			       writeFileLog("[defaultCancel()] Successfully reject coin.");
		               break;
		         }
			 else
		         {
			       writeFileLog("[defaultCancel()] failed to reject coin.");
		               memset(log,'\0',100); 
		               sprintf(log,"[defaultCancel()] Reject Coin retry: %d.",retry);
		               writeFileLog(log);
		               continue;
		         }

		 }

		 writeFileLog("[defaultCancel()] After RejectFromCoinEscrow().");

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
				   writeFileLog(log);

			 }
			 else
			 {
				  writeFileLog("[defaultCancel()] note acceptor recycling cassette status read failed so return from here.");
				  return(FAIL);
			 }  

                         writeFileLog("[defaultCancel()] Going to Dispense notes.");
                 

                         ////////////////////////////////////////////////////////////////////////////////////////////


                          #ifdef JCM_NOTE_ACCEPTOR
		 
		          writeFileLog("[defaultCancel() JCM_NOTE_ACCEPTOR] Before Dispense Operation Calling.");

		          int jcmrtcode=-1;

		          for(retry = 1; retry<=2;retry++) 
		          {

		                 jcmrtcode= JCM_DefaultCancel();

		                 if(1==jcmrtcode)
				 {

		                      writeFileLog("[defaultCancel() JCM_NOTE_ACCEPTOR] Dispense operation successfully executed.");
		                      ret3 = SUCCESS;

		                      break;

		                 }   
		                 else
		                 {

		                        writeFileLog("[defaultCancel() JCM_NOTE_ACCEPTOR] Reject note failed.");
					memset(log,'\0',100); 

					sprintf(log,"[defaultCancel() JCM_NOTE_ACCEPTOR]  Reject note Retry : %d.",retry);
					writeFileLog(log);

		                        ret3 = FAIL;

		                        delay_mSec(100);

					continue;


		                 }


		          }

                          writeFileLog("[defaultCancel() JCM_NOTE_ACCEPTOR] After Dispense Operation Completed.");

                          #endif


                         ////////////////////////////////////////////////////////////////////////////////////


                         #ifdef B2B_NOTE_ACCEPTOR

			 if(noteQtyInRcyclCst > 20) //max 20 note can b dspnsed at a time.
			 { 
			      noteQtyInRcyclCst = 20;
			 }
		         
                         writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Before RejectFromEscrow().");

			 for(retry = 1; retry<=2;retry++) 
			 {

				   //issue dispense command
				   
				   ret2 = RejectFromEscrow(24,noteQtyInRcyclCst);

				   if(SUCCESS == ret2)
				   {   
					   writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Dispence command issue success.");
				           writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Before GetReturnDetail().");
				           ret3 = GetReturnDetail(&value,&noteStatus);  

					   if(SUCCESS == ret3)
						writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Reject notes success.");
					   else     
						writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Reject notes failed.");

				           writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] After GetReturnDetail().");
				           break;
					    
				   } 
				   else
				   {
				         writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] Dispence command issue failed.");
				         memset(log,'\0',100); 
				         sprintf(log,"[defaultCancel() B2B_NOTE_ACCEPTOR] Note Dispense Retry: %d.",retry);
				         writeFileLog(log);
				         continue;
				   } 
				   
				
			  } 

			  writeFileLog("[defaultCancel() B2B_NOTE_ACCEPTOR] After RejectFromEscrow().");

			  #endif

                         ////////////////////////////////////////////////////////////////////////////////////////


                          writeFileLog("[defaultCancel()] after Dispense notes.");
                           
                          int CurrentCastQuanity=0;
                          GetNoteDetailsInRecycleCst( &CurrentCastQuanity );
                          memset(log,'\0',100);
                          sprintf(log,"[defaultCancel()] After Dispense Recycle Quantity = %d.",CurrentCastQuanity);
                          writeFileLog(log);
 

		 #endif

		 #if !defined(NOTE_ACCEPTOR)   
	   
		 ret3 =SUCCESS;

		 #endif

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		  if(( SUCCESS == ret1 ) && ( SUCCESS == ret3  ))
		  {
			   writeFileLog("[defaultCancel() Exit] Reject notes and coin finished with success.");
		           return true;
		  }
		  else
		  {

			  writeFileLog("[defaultCancel() Exit] Reject notes and coin finished with failed.");

		          return false;

		  }

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

		  
	

}//defaultCancel() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOTE_ACCEPTOR 

//Start Note Acceptor Transaction Complete Thread if success return 1 or fail 0
unsigned int  atvm_NACreateTransCompleteThread( unsigned int Transtype )
{


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



}

#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef COIN_ESCROW

//Start Coin Acceptor Transaction Complete Thread if success return 1 or fail 0
unsigned int  atvm_CACreateTransCompleteThread( unsigned int Transtype )
{

		  
                  
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef NOTE_ACCEPTOR 

static void* NACompleteTransactionThread(void *argptr)
{



                        writeFileLog("[NACompleteTransactionThread()] Entry .");

                        int try=1,ret3=FAIL,ret2=FAIL;
                        unsigned char log[100];   
                        memset(log,'\0',100); 
                        unsigned int value=0,noteStatus=0;

                        unsigned int *fnarg=(int *) argptr;
                        sprintf(log,"[NACompleteTransactionThread()] fnarg = %d.",*fnarg);
                        writeFileLog(log);
                        
                        pthread_mutex_lock(&g_NAComplteOperationmutex );
                        g_NAComplteOperation=false;
                        pthread_mutex_unlock(&g_NAComplteOperationmutex );
                        
                        // COMMIT_TRANS start here
                        if( COMMIT_TRANS ==  *fnarg )
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

                                                     ////////////////////////////////////////////////////////////////////////

                                                     /*

				                     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );

				                     memset(log,'\0',100);

				                     sprintf(log,"[NACompleteTransactionThread() Commit] JCM Before Unload Recycle Quantity = %d.",CurrentCastQuanity);
				                     writeFileLog(log);

                                                     */

                                                     //////////////////////////////////////////////////////////////////////

				                     int rtcode=-1;

				                     writeFileLog("[NACompleteTransactionThread() Commit] Before Unload Operation Calling.");

				                     for(try=1;try<=2;try++)
				                     {
						            
						                   //unload all notes from jcm escrow
						                   
						                   rtcode=UnloadCashAcptr();
						                  
						                   if(1 == rtcode)
						                   {

						                           writeFileLog("[NACompleteTransactionThread() Commit] Unload successfully executed.");
						                           break;

						                   }
						                   else
						                   {
				                                           writeFileLog("[NACompleteTransactionThread() Commit]  Unload Operation failed.");
						                           memset(log,'\0',100); 
									   sprintf(log,"[NACompleteTransactionThread() Commit]  retry: %d.",try);
									   writeFileLog(log);
				                                           delay_mSec(200);
				                                           rtcode=-1;
						                           continue;

						                   }


				                     }

				                     writeFileLog("[NACompleteTransactionThread() Commit] After Unload Operation Calling.");

				                      
				                     writeFileLog("[NACompleteTransactionThread() Commit] After Unload Operation now going to read jcm escrow notes number.");

                                                     /////////////////////////////////////////////////////////////////////////
                                                     /*

				                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );


				                     memset(log,'\0',100);

				                     sprintf(log,"[NACompleteTransactionThread() Commit] JCM After Unload Recycle Quantity = %d.",AfterCastQuanity );

				                     writeFileLog(log);
                                                  
                                                     */

                                                     ///////////////////////////////////////////////////////////////////////////

				                     if(1 == rtcode)
                                                     {
				                           ret3=SUCCESS;
                                                     }
				                     else
				                     {    
                                                           ret3=FAIL;
                                                     }

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
				                     sprintf(log,"[NACompleteTransactionThread() Commit] BNA Before Unload Recycle Quantity = %d.",CurrentCastQuanity);
				                     writeFileLog(log);
				                     int rtcode=-1;

				                     writeFileLog("[NACompleteTransactionThread() Commit] Before Unload Operation Calling.");

				                     for(try=1;try<=2;try++)
				                     {
						            
						                   //unload all notes from jcm escrow
						                   
						                   rtcode=UnloadCashAcptr();
						                  
						                   if(1 == rtcode)
						                   {

						                           writeFileLog("[NACompleteTransactionThread() Commit] Unload successfully executed.");
						                           break;

						                   }
						                   else
						                   {

				                                           writeFileLog("[NACompleteTransactionThread() Commit]  Unload Operation failed.");
						                           memset(log,'\0',100); 

									   sprintf(log,"[NACompleteTransactionThread() Commit]  retry: %d.",try);
									   writeFileLog(log);
				                                           delay_mSec(200);
				                                           rtcode=-1;
						                           continue;

						                   }


				                     }

				                     writeFileLog("[NACompleteTransactionThread() Commit] After Unload Operation Calling.");

                                                     writeFileLog("[NACompleteTransactionThread() Commit] After Unload Operation now going to read bna escrow notes number.");

				                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );


				                     memset(log,'\0',100);

				                     sprintf(log,"[NACompleteTransactionThread() Commit] BNA After Unload Recycle Quantity = %d.",AfterCastQuanity );

				                     writeFileLog(log);

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
				                   
                                                        writeFileLog("[NACompleteTransactionThread() Commit] Before AcceptFromEscrow().");

				                        for(try=1;try<=2;try++)
				                        {  
				                             
				                             //start b2b unloading state
				                             ret2 = AcceptFromEscrow(1,g_ttlNmbrOfNote); 

				                             if( SUCCESS == ret2 )  
				                             {

				                                  writeFileLog("[NACompleteTransactionThread() Commit] AcceptFromEscrow() return with success.");
				                                  writeFileLog("[NACompleteTransactionThread() Commit] Now no retry needed.");
				                                  writeFileLog("[NACompleteTransactionThread() Commit] Before GetAcceptedNoteDetail().");
				                                  for(try=1;try<=2;try++)
				                                  {
				                                     
				                                     //wait for b2b unloaded state and return when b2b finish with disabled state
				                                     ret3 = GetAcceptedNoteDetail(&value,&noteStatus); 

				                                     if(SUCCESS == ret3)
				                                     {
				                                          writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with success."); 
				                                          writeFileLog("[NACompleteTransactionThread() Commit] No retry needed."); 

				                                          break;

				                                      }
				                                      else
				                                      {
				                                           writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with fail."); 
				                                           memset(log,'\0',100);

				                                           sprintf(log,"[NACompleteTransactionThread() Commit] Retry due to fail return %d.",try);
				                                           writeFileLog(log);
				                                           continue;
				                                      }

				                                    }//internal foor loop end
				                                    if(SUCCESS == ret3)
				                                        writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with success.");
				                                    else
				                                        writeFileLog("[NACompleteTransactionThread() Commit] GetAcceptedNoteDetail() return with fail.");

				                                    writeFileLog("[NACompleteTransactionThread() Commit] After GetAcceptedNoteDetail().");
				                                    break;

				                                 }
				                                 else
				                                 {

				                                     memset(log,'\0',100);

				                                     sprintf(log,"[NACompleteTransactionThread() Commit] AcceptFromEscrow() return with fail %d .",try);
				                                     writeFileLog(log);  

				                                     continue;

				                                 }

				                              }//external foor loop end

				                              writeFileLog("[NACompleteTransactionThread() Commit] After AcceptFromEscrow().");

				                 }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
				                 else
				                 {

				                    writeFileLog("[NACompleteTransactionThread() Commit] Note Acceptor low level thread is not starting so no accept escrow can be done now!!.");

				                    ret3=FAIL;

				                 }
				                 #endif

                                                 //set commit transaction status
                                                 pthread_mutex_lock(&g_NACommitStatusmutex);

                                                 if(SUCCESS == ret3)
                                                 {
		                                      g_NACommitStatus=SUCCESS;
                                                 }
                                                 else
                                                 {
                                                      g_NACommitStatus=FAIL;
                                                 }

                                                 pthread_mutex_unlock(&g_NACommitStatusmutex);

 

                         }// COMMIT_TRANS end here

		         
                         // CANCEL_TRANS  start here
                         if( CANCEL_TRANS ==  *fnarg )
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

                                                     /////////////////////////////////////////////////////////////////////
                                                     /*

				                     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );

				                     memset(log,'\0',100);

				                     sprintf(log,"[NACompleteTransactionThread() Cancel] JCM Before Dispense Recycle Quantity = %d.",CurrentCastQuanity);
				                     writeFileLog(log);
                                                    
                                                     */
                                                     /////////////////////////////////////////////////////////////////////

				                     int rtcode=-1,try=1;

				                     writeFileLog("[NACompleteTransactionThread() Cancel] Before Dispense Operation Calling.");
				                 
				                     for(try=1;try<=2;try++)
				                     {
						            
				                           //dispense all notes
				                           
				                           rtcode=DispenseCashAcptr();

				                           if( (1 == rtcode) || (4 == rtcode) )
				                           { 

				                                 writeFileLog("[NACompleteTransactionThread() Cancel] Dispense Operation successfully executed.");
				                                 break;

				                           }
				                           else
				                           {

				                                  writeFileLog("[NACompleteTransactionThread() Cancel]  Dispense notes failed.");
						                  memset(log,'\0',100); 
						                  sprintf(log,"[NACompleteTransactionThread() Cancel]  Retry: %d.",try);
						                  writeFileLog(log);
				                                  delay_mSec(200);
				                                  rtcode=-1;
				                                  continue;

				                            }



				                     }

				                     writeFileLog("[NACompleteTransactionThread() Cancel] After Dispense Operation Calling.");

                                                     ///////////////////////////////////////////////////////////////////////
                                                     /*

				                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );

				                     memset(log,'\0',100);

				                     sprintf(log,"[NACompleteTransactionThread() Cancel] JCM After Dispensed Recycle Quantity = %d.",AfterCastQuanity );

				                     writeFileLog(log);

                                                     */
                                                     ////////////////////////////////////////////////////////////////////////

				                     if( (1 == rtcode)|| (4 == rtcode) )
                                                     {
				                          ret3=SUCCESS;
                                                     }
				                     else
                                                     {
				                          ret3=FAIL;
                                                     }


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

				                     writeFileLog("[NACompleteTransactionThread() Cancel] Before Wait for any reject state completion.");

				                     rtcode=WaitforNoteTakenBackByUser(3600); //1hr

				                     writeFileLog("[NACompleteTransactionThread() Cancel] After Wait for any reject state completion.");

				                     if(1 == rtcode )
				                     {
				                            
				                   
								     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );

								     memset(log,'\0',100);

								     sprintf(log,"[NACompleteTransactionThread() Cancel] BNA Before Dispense Recycle Quantity = %d.",CurrentCastQuanity);
								     writeFileLog(log);

								     int rtcode=-1,try=1;

								     writeFileLog("[NACompleteTransactionThread() Cancel] Before Dispense Operation Calling.");
								 
								     for(try=1;try<=2;try++)
								     {
									    
								           //dispense all notes
								           
								           rtcode=DispenseCashAcptr();

								           if( (1 == rtcode) )
								           { 

								                 writeFileLog("[NACompleteTransactionThread() Cancel] Dispense Operation successfully executed.");
								                 break;

								           }
								           else
								           {

								                  writeFileLog("[NACompleteTransactionThread() Cancel]  Dispense notes failed.");
										  memset(log,'\0',100); 
										  sprintf(log,"[NACompleteTransactionThread() Cancel]  Retry: %d.",try);
										  writeFileLog(log);
								                  delay_mSec(200);
								                  rtcode=-1;
								                  continue;

								            }



								     }

								     writeFileLog("[NACompleteTransactionThread() Cancel] After Dispense Operation Calling.");

				                                     delay_mSec(200);

								     GetNoteDetailsInRecycleCst( &AfterCastQuanity );

								     memset(log,'\0',100);

								     sprintf(log,"[NACompleteTransactionThread() Cancel] BNA After Dispensed Recycle Quantity = %d.",AfterCastQuanity );

								     writeFileLog(log);

								     if( 1 == rtcode )
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

				               unsigned int totalnotes=0,retry = 1;
				               totalnotes=CalculateCurrentBillQuantity();

				               for(try = 1; try<=2;try++)  
					       {

							 writeFileLog("[NACompleteTransactionThread() Cancel] before reject from escrow.");

				                         ret2 = RejectFromEscrow(24,g_ttlNmbrOfNote);

				                         if( SUCCESS == ret2 )
				                         {

				                                writeFileLog("[NACompleteTransactionThread() Cancel] RejectFromEscrow() return with success.");
				                                writeFileLog("[NACompleteTransactionThread() Cancel] Now no need to retry here.");
				                                writeFileLog("[NACompleteTransactionThread() Cancel] before GetReturnDetail().");   
				                                for(retry = 1; retry<=2;retry++) 
				                                {
									ret3 = GetReturnDetail(&value,&noteStatus);
						                        if( SUCCESS == ret3 )  
				                                        {
						                           writeFileLog("[NACompleteTransactionThread() Cancel] GetReturnDetail() return with success status.");
				                                           writeFileLog("[NACompleteTransactionThread() Cancel] Now no retry here.");
				                                           break;
				                                        }
						                        else
				                                        {
						                           writeFileLog("[NACompleteTransactionThread() Cancel] GetReturnDetail() return with fail status.");
				                                           memset(log,'\0',100); 
				                                           sprintf(log,"[NACompleteTransactionThread() Cancel] GetReturnDetail() Retry: %d.",retry);
				                                           writeFileLog(log);
				                                           continue; 
				                                        }

				                                 }//internal for loop end

				                                 writeFileLog("[NACompleteTransactionThread() Cancel] after GetReturnDetail().");
				                                 break;

				                         }
				                         else
				                         {
				                               writeFileLog("[NACompleteTransactionThread() Cancel] RejectFromEscrow() return with fail.");
				                               memset(log,'\0',100); 

				                               sprintf(log,"[NACompleteTransactionThread() Cancel]  RejectFromEscrow() Retry: %d.",retry);
				                               writeFileLog(log);
				                               continue;

				                         }


				                   }//external for loop end

				                   
				                   
				                   writeFileLog("[NACompleteTransactionThread() Cancel] after reject from escrow.");

				            

				         }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
				         else
				         {
				                writeFileLog("[NACompleteTransactionThread() Cancel] Note Acceptor low level thread is not closed so no reject escrow can be done now!!.");
				                ret3=FAIL;
				         }

				         #endif

                                         writeFileLog("[NACompleteTransactionThread()] Before Set Note acceptor cancel status .");

                                         //set cancel transaction status

                                         pthread_mutex_lock(&g_NACancelStatusmutex);

                                         if( SUCCESS == ret3 )
                                         {
		                              g_NACancelStatus=SUCCESS;

                                         }
                                         else
                                         {

                                              g_NACancelStatus=FAIL;

                                         }

                                         pthread_mutex_unlock(&g_NACancelStatusmutex);

                                         writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel status .");


                         }// CANCEL_TRANS  end here

                         //free thread arguments pointer
                         if(NULL != fnarg)
                         {
                                free(fnarg);

                         }

                         writeFileLog("[NACompleteTransactionThread()] Before Set Note acceptor cancel work complete signal .");

                         //Send Signal That note acceptor work [accept or reject ] is complete

                         //pthread_mutex_lock( &g_NACompleteThreadmutex );

		         //pthread_cond_signal( &g_NACompleteThreadCond );

		         //pthread_mutex_unlock( &g_NACompleteThreadmutex );

                         pthread_mutex_lock(&g_NAComplteOperationmutex );
                         g_NAComplteOperation=true;
                         pthread_mutex_unlock(&g_NAComplteOperationmutex );
                        
                         writeFileLog("[NACompleteTransactionThread()] After Set Note acceptor cancel work complete signal .");

                         writeFileLog("[NACompleteTransactionThread()] Exit .");

                         pthread_exit(0);


}//[NACompleteTransactionThread()] end

#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef COIN_ESCROW

static void* CACompleteTransactionThread(void *argptr)
{


                      writeFileLog("[CACompleteTransactionThread()] Entry .");
                      unsigned char log[100];
                      memset(log,'\0',100); 
                      int try=1,ret1=FAIL;
             
             
                      unsigned int *fnarg=(int *) argptr;
                      sprintf(log,"[CACompleteTransactionThread() Commit] myptr = %d.",*fnarg);
                      writeFileLog(log);

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
								   writeFileLog("[CACompleteTransactionThread() Commit] Accept Coin Successfully.");
								   memset(log,'\0',100); 

								   sprintf(log,"[CACompleteTransactionThread() Commit]  Retry: %d.",try);
								   writeFileLog(log);

								   break;
						       }
						       else
						       {
								   writeFileLog("[CACompleteTransactionThread() Commit] Accept Coin failed.");
								   memset(log,'\0',100); 

								   sprintf(log,"[CACompleteTransactionThread() Commit]  Retry: %d.",try);
								   writeFileLog(log);

								   continue;

						       }

				               }

				      }
				      else
				      {
				               writeFileLog("[CACompleteTransactionThread() Commit] Coin Acceptor Low Level flag is not closed so no accept escrow can be done now!!. ");
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

								   writeFileLog("[CACompleteTransactionThread() Cancel] Reject Coin Successfully.");
						                   memset(log,'\0',100); 

						                   sprintf(log,"[CACompleteTransactionThread() Cancel]  Retry: %d.",try);
						                   writeFileLog(log);

						                   break;
				                       }
						       else
				                       {
								  writeFileLog("[CACompleteTransactionThread() Cancel]  Reject Coin failed.");
						                  memset(log,'\0',100); 

						                  sprintf(log,"[CACompleteTransactionThread() Cancel]  Retry: %d.",try);
						                  writeFileLog(log);

						                  continue;

				                       }

				                }

					 }
				         else
				         {
				               writeFileLog("[CACompleteTransactionThread() Cancel] Coin Acceptor Low Level flag is not closed so no reject escrow can be done now!!.");
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

                           writeFileLog("[CACompleteTransactionThread()] Exit .");

                           pthread_exit(0);


}

#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Accept Transaction Version 2
bool commitTran_Ver2()
{

 
         bool CAThreadStatus=false,NAThreadStatus=false,ComitStatus=false;	
         int totalnotes=0,rtcode=0;
         unsigned char log[100];   
         memset(log,'\0',100); 
         writeFileLog("[commitTran_Ver2()] Entry:  Commit Transaction issue by system......");
         
         bool already_unlock=false;
    
         pthread_mutex_lock(&AcptFareThreadStatusmutex);

         //if all acpt fare thread not stop then stop them before doing commit trans
	 if(THREAD_START==g_AcptFareThreadStatus)
	 {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);

	      //Malay Add 19 Jan 2013
	      writeFileLog("[commitTran_Ver2()] Commit Transaction issue by system before close all acceptfare threads.");

	      rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);

              already_unlock=true;                                              

	      writeFileLog("[commitTran_Ver2()] after return KillAllRunningThread().");


	 }
         
         if( false == already_unlock )
         {

               pthread_mutex_unlock(&AcptFareThreadStatusmutex);

         }

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	 if(THREAD_CLOSE==g_AcptFareThreadStatus)
         {

                  
                  pthread_mutex_unlock(&AcptFareThreadStatusmutex);
                  memset(log,'\0',100); 
                  sprintf(log,"[commitTran_Ver2()] Total Coins : %d .",g_ttlNmbrOfCoin);
                  writeFileLog(log);
                  memset(log,'\0',100);
                  sprintf(log,"[commitTran_Ver2()] Total Notes : %d .",g_ttlNmbrOfNote);
                  writeFileLog(log);  

                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #ifdef COIN_ESCROW
                  

                 

                  //writeFileLog("[commitTran_Ver2()] Before Accept Coin.");
                  
		  if(g_ttlNmbrOfCoin>0)
		  {
                           
                           pthread_mutex_lock(&g_CAComplteOperationmutex );
                           g_CAComplteOperation=false;
                           pthread_mutex_unlock(&g_CAComplteOperationmutex );

                           rtcode=0;
                           rtcode=atvm_CACreateTransCompleteThread( COMMIT_TRANS );
                           //Thread create successfully
                           if(1 == rtcode)
                           {
                                writeFileLog("[commitTran_Ver2()] Successfully started atvm_CACreateTransCompleteThread with commit mode.");
                                CAThreadStatus=true;
                           }
                           else //thread create failed
                           {

                               writeFileLog("[commitTran_Ver2()] failed to  start atvm_CACreateTransCompleteThread with commit mode.");
                           }

                  } 
		  else
                  {
		        
                           pthread_mutex_lock( &g_CACommitStatusmutex );
                           g_CACommitStatus=SUCCESS;
                           pthread_mutex_unlock( &g_CACommitStatusmutex );

                  }

                  //writeFileLog("[commitTran_Ver2()] After Accept Coin.");
                  
                  #endif
               
                  #ifndef COIN_ESCROW
                  pthread_mutex_lock( &g_CACommitStatusmutex );
                  g_CACommitStatus=SUCCESS;
                  pthread_mutex_unlock( &g_CACommitStatusmutex );
                  #endif

                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #ifdef NOTE_ACCEPTOR  
                               
                  if(g_ttlNmbrOfNote>0)
		  {  
                         
                           pthread_mutex_lock(&g_NAComplteOperationmutex );
                           g_NAComplteOperation=false;
                           pthread_mutex_unlock(&g_NAComplteOperationmutex );

                           //store global note number
                           totalnotes=g_ttlNmbrOfNote;
                           rtcode=0;
                           rtcode=atvm_NACreateTransCompleteThread( COMMIT_TRANS );
                           //Thread create successfully
                           if(1 == rtcode)
                           {
                                writeFileLog("[commitTran_Ver2()] Successfully started atvm_NACreateTransCompleteThread with commit mode.");
                                NAThreadStatus=true;

                           }
                           else //thread create failed
                           {

                                writeFileLog("[commitTran_Ver2()] failed to  start atvm_NACreateTransCompleteThread with commit mode.");
                           }
                            
  
                  }//if(g_ttlNmbrOfNote>0) block
                  else
                  {

		            pthread_mutex_lock( &g_NACommitStatusmutex );
		            g_NACommitStatus=SUCCESS;
		            pthread_mutex_unlock( &g_NACommitStatusmutex );

                  }
                  
                 #endif

                 #ifndef NOTE_ACCEPTOR
                 pthread_mutex_lock( &g_NACommitStatusmutex );
		 g_NACommitStatus=SUCCESS;
		 pthread_mutex_unlock( &g_NACommitStatusmutex );
                 #endif

                ///////////////////////////////////////////////////////////////////////////////////////////////////////////

                //wait for coin acceptor commit status signal
                struct timespec begints, endts,diffts;
                  
                  
                #ifdef COIN_ACCEPTOR  
                if(true == CAThreadStatus)
                {

                       //pthread_mutex_lock( &g_CACompleteThreadmutex );

                       //pthread_cond_wait( &g_CACompleteThreadCond , &g_CACompleteThreadmutex );

                       //pthread_mutex_unlock( &g_CACompleteThreadmutex );
                       clock_gettime(CLOCK_MONOTONIC, &begints);
                       while(1)
                       {
		                   pthread_mutex_lock(&g_CAComplteOperationmutex );
		                   if ( true == g_CAComplteOperation )
                                   {
                                           
                                           clock_gettime(CLOCK_MONOTONIC, &endts);
 
                                           diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                                           memset(log,'\0',100);

		                           sprintf(log,"[commitTran_Ver2()] coin acceptor operation complete in Seconds =%d .",diffts.tv_sec ); 

		                           writeFileLog(log);

		                           pthread_mutex_unlock(&g_CAComplteOperationmutex );

                                           break;

                                   }

                                   pthread_mutex_unlock(&g_CAComplteOperationmutex );

                       }

                }
                #endif


                //wait for note acceptor commit status signal

                #ifdef NOTE_ACCEPTOR  
                if(true == NAThreadStatus)
                {

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

		                           writeFileLog(log);

		                           pthread_mutex_unlock(&g_NAComplteOperationmutex );

                                           break;

                                   }

                                   pthread_mutex_unlock(&g_NAComplteOperationmutex );


                       }

                }   

                #endif

                ///////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #if defined(B2B_TRAP_GATE)

                   
		   //Start:Malay 1 Jan 2013 add
		   //Store Note Counts in global variable and drop it in big cashbox if threshold value cross
		   
                   writeFileLog("[commitTran_Ver2()] Before Drop notes.");
		                    
                   
		   //Store Notes accepted in each transactions
		   
                   g_NoteCounter=g_NoteCounter+totalnotes;
                   
                   memset(log,'\0',100);
                   sprintf(log,"[commitTran_Ver2()] Still Accepted Notes = %d .",g_NoteCounter);
                   writeFileLog(log);
                   memset(log,'\0',100);
                   sprintf(log,"[commitTran_Ver2()] Threshold value of Notes = %d .",g_DropNotesNumber);
                   writeFileLog(log);

		   //Check if it greater than or equal to notes drop threshold value if greater found drop it
		   if(g_NoteCounter >= g_DropNotesNumber)
		   {

			    //Drop Notes
                            int rtcode=-1;
			    rtcode=DropNotes();
			    if(1==rtcode || 4==rtcode) //Successfully drop notes
		            {
			       writeFileLog("[commitTran_Ver2()] Drop notes successfully.");
		               g_NoteCounter=0;
		            }
		            else
                            {
		               writeFileLog("[commitTran_Ver2()] Drop notes failed.");
                            }
                            
                            
		   }
                   else
                   {
                       writeFileLog("[commitTran_Ver2()] Threshold value not match so no drop note operation canbe done now.");

                   }

                   writeFileLog("[commitTran_Ver2()] After Drop notes.");

		   #endif

                  
                  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

                  #ifdef COIN_ESCROW
                  pthread_mutex_lock( &g_CACommitStatusmutex );
                  #endif
                  #ifdef NOTE_ACCEPTOR  
                  pthread_mutex_lock( &g_NACommitStatusmutex );
                  #endif

                  if( ( SUCCESS == g_CACommitStatus ) && ( SUCCESS == g_NACommitStatus ) )
                  {

		              writeFileLog("[commitTran_Ver2() Exit]  Return With Success Status.");
                              memset(log,'\0',100);
		              sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d committed With Success Status.",g_total_transaction);
		              writeFileLog(log);
		              ComitStatus= true;

                  }
		  else
                  {
                               
                             
			      writeFileLog("[commitTran_Ver2() Exit] Return With Fail Status.");
                              memset(log,'\0',100);
		              sprintf(log,"[commitTran_Ver2() Exit] Transaction Number %d not committed With Fail Status.",g_total_transaction);
		              writeFileLog(log);
		              ComitStatus= false;

                  }
                  #ifdef COIN_ESCROW
                  pthread_mutex_unlock( &g_CACommitStatusmutex );
                  #endif
                  #ifdef NOTE_ACCEPTOR  
                  pthread_mutex_unlock( &g_NACommitStatusmutex );
                  #endif
                  return ComitStatus;
                  
		  
      }//if(THREAD_CLOSE==g_AcptFareThreadStatus) block 

      else 
      {
              pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	      writeFileLog("[commitTran_Ver2() Exit] No acceptfare thread stated so committrans() return fail status.");
	      return false;
      }


 

}//commitTran_Ver2() end here




//////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Reject Transaction Version 2
bool cancelTran_Ver2()
{

	
         int rtcode=0;
         unsigned char log[100];
         memset(log,'\0',100);
         bool already_unlock=false;
         bool cancelrtcode=false;
	 bool CAThreadStatus=false,NAThreadStatus=false;
         writeFileLog(" \n\n\n\n ");
         writeFileLog("[cancelTran_Ver2() Entry] ");
         writeFileLog("[cancelTran_Ver2() Entry]  Cancel Transaction issue by system...");

         //if all acpt fare thread not stop then stop them before doing cancel trans

         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	 if( THREAD_START== g_AcptFareThreadStatus )
	 {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);
     
              already_unlock=true;

	      //Malay Add 19 Jan 2013
	      //writeFileLog("[cancelTran_Ver2] Cancel Transaction issue by system before close all acceptfare threads.");

	      rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);

	      //writeFileLog("[cancelTran_Ver2] after return KillAllRunningThread().");


	 }

         if( false == already_unlock )
         {

               pthread_mutex_unlock(&AcptFareThreadStatusmutex);

         }


         pthread_mutex_lock(&AcptFareThreadStatusmutex);

	 if(THREAD_CLOSE==g_AcptFareThreadStatus)
         {
                 
                  pthread_mutex_unlock(&AcptFareThreadStatusmutex);
                  memset(log,'\0',100);
                  sprintf(log,"[cancelTran_Ver2] Total Coins : %d .",g_ttlNmbrOfCoin);
                  writeFileLog(log);
                  memset(log,'\0',100);
                  sprintf(log,"[cancelTran_Ver2()] Total Notes : %d .",g_ttlNmbrOfNote);
                  writeFileLog(log);

		  //Malay Add 19 Jan 2013
                  #ifdef COIN_ESCROW

		  //writeFileLog("[cancelTran_Ver2()] before start reject coin thread.");

		  if(g_ttlNmbrOfCoin>0)
		  {  
                              
                           pthread_mutex_lock(&g_CAComplteOperationmutex );
                           g_CAComplteOperation=false;
                           pthread_mutex_unlock(&g_CAComplteOperationmutex );
                        
                           rtcode=0;
                           rtcode=atvm_CACreateTransCompleteThread( CANCEL_TRANS );
                           //Thread create successfully
                           if(1 == rtcode)
                           {
                                writeFileLog("[cancelTran_Ver2()] Successfully started atvm_CACreateTransCompleteThread with commit mode.");
                                CAThreadStatus=true;

                           }
                           else //thread create failed
                           {

                                writeFileLog("[cancelTran_Ver2()] failed to  start atvm_CACreateTransCompleteThread with commit mode.");
                           }
                            

                  }
		  else
                  {
		        
                        pthread_mutex_lock( &g_CACancelStatusmutex );
                        g_CACancelStatus = SUCCESS ; 
                        pthread_mutex_unlock( &g_CACancelStatusmutex );

                  }
                  
		  //writeFileLog("[cancelTran_Ver2()] after finish reject coin thread.");

                  #endif

                  #ifndef COIN_ESCROW
                  pthread_mutex_lock( &g_CACancelStatusmutex );
                  g_CACancelStatus = SUCCESS ; 
                  pthread_mutex_unlock( &g_CACancelStatusmutex );
                  #endif
                 
		  #ifdef NOTE_ACCEPTOR

		  //writeFileLog("[cancelTran_Ver2()].");

		  if(g_ttlNmbrOfNote>0)
		  {  
                           
                           
                           pthread_mutex_lock(&g_NAComplteOperationmutex );
                           g_NAComplteOperation=false;
                           pthread_mutex_unlock(&g_NAComplteOperationmutex );
                           rtcode=0;
                           rtcode=atvm_NACreateTransCompleteThread( CANCEL_TRANS );
                           //Thread create successfully
                           if(1 == rtcode)
                           {
                                writeFileLog("[cancelTran_Ver2()] Successfully started atvm_NACreateTransCompleteThread with cancel mode.");
                                NAThreadStatus=true;

                           }
                           else //thread create failed
                           {

                                writeFileLog("[cancelTran_Ver2()] failed to  start atvm_NACreateTransCompleteThread with cancel mode.");
                           
                           }
                            
  

                  
                  } //if(g_ttlNmbrOfNote>0) block
		  else
                  {

		       writeFileLog("[cancelTran_Ver2()] Set Note acceptor status.");
                       pthread_mutex_lock( &g_NACancelStatusmutex );
                       g_NACancelStatus = SUCCESS ; 
                       pthread_mutex_unlock( &g_NACancelStatusmutex );

                       #ifdef BNA_NOTE_ACCEPTOR

                       if(1==WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
		       {

                                     rtcode=0;

                                     writeFileLog("[cancelTran_Ver2()] Before Wait for any reject state completion.");

                                     rtcode=WaitforNoteTakenBackByUser(1);

                                     if( 1 != rtcode )
                                     {

                                            writeFileLog("[cancelTran_Ver2()] Before Going to close reject state operation.");

                                            StopRejectProcess();

                                            writeFileLog("[cancelTran_Ver2()] After Going to close reject state operation.");
  

                                     }

                                     writeFileLog("[cancelTran_Ver2()] After Wait for any reject state completion.");

                                     writeFileLog("[cancelTran_Ver2()] Before  close all BNA CashIn Process.");      
    
                                     CompleteCashInProcess(); 

                                     writeFileLog("[cancelTran_Ver2()] After  close all BNA CashIn Process."); 

                                     


                      }
                      #endif

                      
                 }

                 //writeFileLog("[cancelTran_Ver2()] after reject notes thread .");

                 #endif

                 #ifndef NOTE_ACCEPTOR
                 pthread_mutex_lock( &g_NACancelStatusmutex );
                 g_NACancelStatus = SUCCESS ; 
                 pthread_mutex_unlock( &g_NACancelStatusmutex );
                 #endif

                ///////////////////////////////////////////////////////////////////////////////////////////////////////////

                //wait for coin acceptor cancel status signal

                #ifdef COIN_ACCEPTOR  
                writeFileLog("[cancelTran_Ver2()] Before wait for coin acceptor cancel status signal."); 
                
                if(true == CAThreadStatus)
                {

                       //pthread_mutex_lock( &g_CACompleteThreadmutex );

                       //pthread_cond_wait( &g_CACompleteThreadCond , &g_CACompleteThreadmutex );

                       //pthread_mutex_unlock( &g_CACompleteThreadmutex );
                       while(1)
                       {
		                   pthread_mutex_lock(&g_CAComplteOperationmutex );

		                   if ( true == g_CAComplteOperation )
                                   {
		                           pthread_mutex_unlock(&g_CAComplteOperationmutex );

                                           break;
                                   }

                                   pthread_mutex_unlock(&g_CAComplteOperationmutex );

                       }

                } 
                

                writeFileLog("[cancelTran_Ver2()] After get  coin acceptor cancel status signal."); 


                #endif


                //wait for note acceptor cancel status signal

                #ifdef NOTE_ACCEPTOR  

                writeFileLog("[cancelTran_Ver2()] Before wait for note acceptor cancel status signal."); 
                
                if(true == NAThreadStatus)
                {

                       //pthread_mutex_lock( &g_NACompleteThreadmutex );

                       //pthread_cond_wait( &g_NACompleteThreadCond , &g_NACompleteThreadmutex );

                       //pthread_mutex_unlock( &g_NACompleteThreadmutex );

                       while(1)
                       {
		                   pthread_mutex_lock(&g_NAComplteOperationmutex );

		                   if ( true == g_NAComplteOperation )
                                   {
		                           pthread_mutex_unlock(&g_NAComplteOperationmutex );

                                           break;

                                   }

                                   pthread_mutex_unlock(&g_NAComplteOperationmutex );

                       }

                }   

                
                writeFileLog("[cancelTran_Ver2()] After get note acceptor cancel status signal."); 

                #endif

                 ///////////////////////////////////////////////////////////////////////////////////////////////////////////


                 #ifdef COIN_ESCROW
                 pthread_mutex_lock( &g_CACancelStatusmutex );
                 #endif
                 #ifdef NOTE_ACCEPTOR
                 pthread_mutex_lock( &g_NACancelStatusmutex );
                 #endif
		 if(( SUCCESS == g_CACancelStatus ) && ( SUCCESS == g_NACancelStatus ))
                 {


		        writeFileLog("[cancelTran_Ver2() Exit] finished with success.");
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with success status.",g_total_transaction);
                        writeFileLog(log);
                        cancelrtcode= true;

                 }
		 else
                 {

                        writeFileLog("[cancelTran_Ver2() Exit] finished with failed.");
                        memset(log,'\0',100); 
                        sprintf(log,"[cancelTran_Ver2() Exit] Transaction Number %d canceled with failed status.",g_total_transaction);
                        writeFileLog(log);
                        cancelrtcode = false;

		  
		 }
		  
                #ifdef COIN_ESCROW
	        pthread_mutex_unlock( &g_CACancelStatusmutex );
                #endif

                #ifdef NOTE_ACCEPTOR
                pthread_mutex_unlock( &g_NACancelStatusmutex );
                #endif

                return cancelrtcode;


      
      }//if(THREAD_CLOSE==g_AcptFareThreadStatus) block
      else 
      {
           pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	   writeFileLog("[cancelTran_Ver2()] No acceptfare thread stated so canceltrans() return fail status.");
	   return false;
      }
	 




}//cancelTran_Ver2() end




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Accept Transaction Version 1
bool commitTran()
{
	
 
         
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
         writeFileLog("[commitTran()] Commit Transaction issue by system......");
         
         bool already_unlock=false;
    
         pthread_mutex_lock(&AcptFareThreadStatusmutex);

         //if all acpt fare thread not stop then stop them before doing commit trans
	 if(THREAD_START==g_AcptFareThreadStatus)
	 {

              pthread_mutex_unlock(&AcptFareThreadStatusmutex);

	      //Malay Add 19 Jan 2013
	      writeFileLog("[commitTran()] Attempt to kill all running threads.");

	      rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);

              already_unlock=true;                                              

	      writeFileLog("[commitTran()] after return KillAllRunningThread().");


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
                  writeFileLog(log);
                  memset(log,'\0',100);
                  sprintf(log,"[commitTran()] Total Notes : %d",g_ttlNmbrOfNote);
                  writeFileLog(log);  

                  

                  #ifdef COIN_ESCROW
                  
                  writeFileLog("[commitTran()] Before Accept Coin.");
                  
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
				                   writeFileLog("[commitTran()] Accept Coin Successfully.");
				                   memset(log,'\0',100); 
				                   sprintf(log,"[commitTran()]  try: %d.",try);
				                   writeFileLog(log);
				                   break;
		                       }
		                       else
		                       {
				                   writeFileLog("[commitTran()] Accept Coin failed.");
				                   memset(log,'\0',100); 
				                   sprintf(log,"[commitTran()]  try: %d.",try);
				                   writeFileLog(log);
				                   continue;
		                       }

                               }
                      }
                      else
                      {
                               writeFileLog("[commitTran()] Coin Acceptor Low Level flag is not closed so no accept escrow can be done now!!. ");
                               ret1=FAIL;
                       }

                       

		  } 
		  else
                  {
		      ret1=SUCCESS;

                  }

                  writeFileLog("[commitTran()] After Accept Coin.");
                  
                  #endif
               
                  #ifndef COIN_ESCROW
                  ret1=SUCCESS;
                  #endif

                  #ifdef NOTE_ACCEPTOR  
                               
                  writeFileLog("[commitTran()] Before Accept Notes.");

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
                                     writeFileLog(log);
                                     int rtcode=-1;

                                     writeFileLog("[commitTran()] Before Unload Operation Calling.");

                                     for(try=1;try<=2;try++)
                                     {
		                            
		                                   //unload all notes from jcm escrow
		                                   
		                                   rtcode=UnloadCashAcptr();
		                                  
		                                   if(1 == rtcode)
		                                   {

		                                           writeFileLog("[commitTran()] Unload successfully executed.");
		                                           break;

		                                   }
		                                   else
		                                   {
                                                           writeFileLog("[commitTran()]  Unload Operation failed.");
		                                           memset(log,'\0',100); 
						           sprintf(log,"[commitTran()]  retry: %d.",try);
						           writeFileLog(log);
                                                           delay_mSec(200);
                                                           rtcode=-1;
		                                           continue;

		                                   }


                                     }

                                     writeFileLog("[commitTran()] After Unload Operation Calling.");

                                      
                                     writeFileLog("[commitTran()] After Unload Operation now going to read jcm escrow notes number.");

                                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );


                                     memset(log,'\0',100);

                                     sprintf(log,"[commitTran()] JCM After Unload Recycle Quantity = %d.",AfterCastQuanity );

                                     writeFileLog(log);

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
                                     writeFileLog(log);
                                     int rtcode=-1;

                                     writeFileLog("[commitTran()] Before Unload Operation Calling.");

                                     for(try=1;try<=2;try++)
                                     {
		                            
		                                   //unload all notes from jcm escrow
		                                   
		                                   rtcode=UnloadCashAcptr();
		                                  
		                                   if(1 == rtcode)
		                                   {

		                                           writeFileLog("[commitTran()] Unload successfully executed.");
		                                           break;

		                                   }
		                                   else
		                                   {
                                                           writeFileLog("[commitTran()]  Unload Operation failed.");
		                                           memset(log,'\0',100); 
						           sprintf(log,"[commitTran()]  retry: %d.",try);
						           writeFileLog(log);
                                                           delay_mSec(200);
                                                           rtcode=-1;
		                                           continue;

		                                   }


                                     }

                                     writeFileLog("[commitTran()] After Unload Operation Calling.");

                                      
                                     writeFileLog("[commitTran()] After Unload Operation now going to read bna escrow notes number.");

                                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );


                                     memset(log,'\0',100);

                                     sprintf(log,"[commitTran()] BNA After Unload Recycle Quantity = %d.",AfterCastQuanity );

                                     writeFileLog(log);

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

                                        writeFileLog("[commitTran()] Before AcceptFromEscrow().");

                                        for(try=1;try<=4;try++)
                                        {  
                                             
                                             
                                             //start b2b unloading state
                                             ret2 = AcceptFromEscrow(1,g_ttlNmbrOfNote); 

                                             if( SUCCESS == ret2 )  
                                             {

                                                  writeFileLog("[commitTran()] AcceptFromEscrow() return with success.");
                                                  writeFileLog("[commitTran()] Now no retry needed.");
                                                  writeFileLog("[commitTran()] Before GetAcceptedNoteDetail().");
                                                  for(try=1;try<=2;try++)
                                                  {
                                                     
                                                     //wait for b2b unloaded state and return when b2b finish with disabled state
                                                     ret3 = GetAcceptedNoteDetail(&value,&noteStatus); 

                                                     if(SUCCESS == ret3)
                                                     {
                                                          writeFileLog("[commitTran()] GetAcceptedNoteDetail() return with success."); 
                                                          writeFileLog("[commitTran()] No retry needed."); 

                                                          break;

                                                      }
                                                      else
                                                      {
                                                           writeFileLog("[commitTran()] GetAcceptedNoteDetail() return with fail."); 
                                                           memset(log,'\0',100);
                                                           sprintf(log,"[commitTran()] Retry due to fail return %d.",try);
                                                           writeFileLog(log);
                                                           continue;
                                                      }

                                                    }//internal foor loop end
                                                    if(SUCCESS == ret3)
                                                        writeFileLog("[commitTran()] GetAcceptedNoteDetail() return with success.");
                                                    else
                                                        writeFileLog("[commitTran()] GetAcceptedNoteDetail() return with fail.");

                                                    writeFileLog("[commitTran()] After GetAcceptedNoteDetail().");
                                                    break;
                                                 }
                                                 else
                                                 {
                                                     memset(log,'\0',100);
                                                     sprintf(log,"[commitTran()] AcceptFromEscrow() return with fail %d .",try);
                                                     writeFileLog(log);  
                                                     continue;
                                                 }

                                              }//external foor loop end

                                              writeFileLog("[commitTran()] After AcceptFromEscrow().");

                                              //clear global note number
                                              totalnotes=g_ttlNmbrOfNote;
                                              g_ttlNmbrOfNote=0;

						
                                 }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
                                 else
                                 {
                                    writeFileLog("[commitTran()] Note Acceptor low level thread is not starting so no accept escrow can be done now!!.");
                                    ret3=FAIL;
                                 }
                                 #endif

                                 

		  }//if(g_ttlNmbrOfNote>0) block

		  else
                  {

		        ret3=SUCCESS;

                  }
                  
                  writeFileLog("[commitTran()] After Accept Notes.");
                  

                  #endif

                 #ifndef NOTE_ACCEPTOR
                  ret3=SUCCESS;
                 #endif
                   
                   #if defined(B2B_TRAP_GATE)

                   
		    //Start:Malay 1 Jan 2013 add
		    //Store Note Counts in global variable and drop it in big cashbox if threshold value cross
		   
                   writeFileLog("[commitTran()] Before Drop notes.");
		                    
                   
		   //Store Notes accepted in each transactions
		   
                   g_NoteCounter=g_NoteCounter+totalnotes;
                   
                   memset(log,'\0',100);
                   sprintf(log,"[commitTran()] Still Accepted Notes = %d .",g_NoteCounter);
                   writeFileLog(log);
                   memset(log,'\0',100);
                   sprintf(log,"[commitTran()] Threshold value of Notes = %d .",g_DropNotesNumber);
                   writeFileLog(log);

		   //Check if it greater than or equal to notes drop threshold value if greater found drop it
		   if(g_NoteCounter >= g_DropNotesNumber)
		   {
			    //Drop Notes
                            int rtcode=-1;
			    rtcode=DropNotes();
			    if(1==rtcode || 4==rtcode) //Successfully drop notes
		            {
			       writeFileLog("[commitTran()] Drop notes successfully.");
		               g_NoteCounter=0;
		            }
		            else
                            {
		               writeFileLog("[commitTran()] Drop notes failed.");
                            }
                            
                            
		   }
                   else
                      writeFileLog("[commitTran()] Threshold value not match so no drop note operation canbe done now.");

                   writeFileLog("[commitTran()] After Drop notes.");

		  #endif

                  
                  if( ( SUCCESS == ret1 ) && ( SUCCESS == ret3 ) )
                  {
		              writeFileLog("[commitTran()] Return With Success Status.");
                              memset(log,'\0',100);
		              sprintf(log,"[commitTran()] Transaction Number %d committed With Success Status.",g_total_transaction);
		              writeFileLog(log);
		              return true;
                  }
		  else
                  {

			      writeFileLog("[commitTran()] Return With Fail Status.");
                              memset(log,'\0',100);
		              sprintf(log,"[commitTran()] Transaction Number %d not committed With Fail Status.",g_total_transaction);
		              writeFileLog(log);
		              return false;

                  }
                  
                  
		  
      }//if(THREAD_CLOSE==g_AcptFareThreadStatus) block 

      else 
      {
              pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	      writeFileLog("[commitTran()] No acceptfare thread stated so committrans() return fail status.");
	      return false;
      }


 
}//commitTran() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Reject Transaction Version 1
bool cancelTran()
{
	

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
	      writeFileLog("[cancelTran()] Cancel Transaction issue by system...");

	      rtcode=KillAllRunningThread(WAIT_TIME_FINISH_ALL_LOWLEVEL_THREAD);

	      writeFileLog("[cancelTran()] after return KillAllRunningThread().");


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
                  writeFileLog(log);
                  memset(log,'\0',100);
                  sprintf(log,"[cancelTran()] Total Notes : %d",g_ttlNmbrOfNote);
                  writeFileLog(log);

		  //Malay Add 19 Jan 2013
                  #ifdef COIN_ESCROW

		  writeFileLog("[cancelTran()] before reject coin.");

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

				                   writeFileLog("[cancelTran()] Reject Coin Successfully.");
		                                   memset(log,'\0',100); 
		                                   sprintf(log,"[cancelTran()]  Retry: %d.",retry);
		                                   writeFileLog(log);
		                                   break;
                                       }
		                       else
                                       {
				                  writeFileLog("[cancelTran()]  Reject Coin failed.");
		                                  memset(log,'\0',100); 
		                                  sprintf(log,"[cancelTran()]  Retry: %d.",retry);
		                                  writeFileLog(log);
		                                  continue;

                                       }

                                }

			 }
                         else
                         {
                               writeFileLog("[cancelTran()] Coin Acceptor Low Level flag is not closed so no reject escrow can be done now!!.");
                               ret1=FAIL;
                         }
                           
                         
                         

		  }
		  else
		   ret1=SUCCESS;
                  
		  writeFileLog("[cancelTran()] after reject coin.");
                  #endif

                  #ifndef COIN_ESCROW
                  ret1=SUCCESS;
                  #endif
                 
		  #ifdef NOTE_ACCEPTOR

		  writeFileLog("[cancelTran()] before reject notes.");

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
                                     writeFileLog(log);
                                     int rtcode=-1,try=1;

                                     writeFileLog("[cancelTran()] Before Dispense Operation Calling.");
                                 
                                     for(try=1;try<=2;try++)
                                     {
		                            
                                           //dispense all notes
                                           
                                           rtcode=DispenseCashAcptr();

                                           if( (1 == rtcode) || (4 == rtcode) )
                                           { 

                                                 writeFileLog("[cancelTran()] Dispense Operation successfully executed.");
                                                 break;

                                           }
                                           else
                                           {

                                                  writeFileLog("[cancelTran()]  Dispense notes failed.");
		                                  memset(log,'\0',100); 
		                                  sprintf(log,"[cancelTran()]  Retry: %d.",try);
		                                  writeFileLog(log);
                                                  delay_mSec(200);
                                                  rtcode=-1;
                                                  continue;

                                            }



                                     }

                                     writeFileLog("[cancelTran()] After Dispense Operation Calling.");

                                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );

                                     memset(log,'\0',100);

                                     sprintf(log,"[cancelTran()] JCM After Dispensed Recycle Quantity = %d.",AfterCastQuanity );

                                     writeFileLog(log);

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

                                     writeFileLog("[cancelTran()] Before Wait for any reject state completion.");

                                     rtcode=WaitforNoteTakenBackByUser(3600); //1hr

                                     writeFileLog("[cancelTran()] After Wait for any reject state completion.");

                                     if(1 == rtcode )
                                     {
                                            
                                   
				                     GetNoteDetailsInRecycleCst( &CurrentCastQuanity );

				                     memset(log,'\0',100);

				                     sprintf(log,"[cancelTran()] BNA Before Dispense Recycle Quantity = %d.",CurrentCastQuanity);
				                     writeFileLog(log);

				                     int rtcode=-1,try=1;

				                     writeFileLog("[cancelTran()] Before Dispense Operation Calling.");
				                 
				                     for(try=1;try<=2;try++)
				                     {
						            
				                           //dispense all notes
				                           
				                           rtcode=DispenseCashAcptr();

				                           if( (1 == rtcode) )
				                           { 

				                                 writeFileLog("[cancelTran()] Dispense Operation successfully executed.");
				                                 break;

				                           }
				                           else
				                           {

				                                  writeFileLog("[cancelTran()]  Dispense notes failed.");
						                  memset(log,'\0',100); 
						                  sprintf(log,"[cancelTran()]  Retry: %d.",try);
						                  writeFileLog(log);
				                                  delay_mSec(200);
				                                  rtcode=-1;
				                                  continue;

				                            }



				                     }

				                     writeFileLog("[cancelTran()] After Dispense Operation Calling.");

                                                     delay_mSec(200);

				                     GetNoteDetailsInRecycleCst( &AfterCastQuanity );

				                     memset(log,'\0',100);

				                     sprintf(log,"[cancelTran()] BNA After Dispensed Recycle Quantity = %d.",AfterCastQuanity );

				                     writeFileLog(log);

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

					 writeFileLog("[cancelTran()] before reject from escrow.");

                                         ret2 = RejectFromEscrow(24,g_ttlNmbrOfNote);

                                         //ret2 = RejectFromEscrow(24,totalnotes);

                                         if( SUCCESS == ret2 )
                                         {
                                                writeFileLog("[cancelTran()] RejectFromEscrow() return with success.");
                                                writeFileLog("[cancelTran()] Now no need to retry here.");
                                                writeFileLog("[cancelTran()] before GetReturnDetail().");
                                                for(retry = 1; retry<=2;retry++) 
                                                {
							ret3 = GetReturnDetail(&value,&noteStatus);
		                                        if( SUCCESS == ret3 )  
                                                        {
		                                           writeFileLog("[cancelTran()] GetReturnDetail() return with success status.");
                                                           writeFileLog("[cancelTran()] Now no retry here.");
                                                           break;
                                                        }
		                                        else
                                                        {
		                                           writeFileLog("[cancelTran()] GetReturnDetail() return with fail status.");
                                                           memset(log,'\0',100); 
                                                           sprintf(log,"[cancelTran()] GetReturnDetail() Retry: %d.",retry);
                                                           writeFileLog(log);
                                                           continue; 
                                                        }
                                                 }//internal for loop end
                                                 writeFileLog("[cancelTran()] after GetReturnDetail().");
                                                 break;
                                         }
                                         else
                                         {
                                               writeFileLog("[cancelTran()] RejectFromEscrow() return with fail.");
                                               memset(log,'\0',100); 
                                               sprintf(log,"[cancelTran()] RejectFromEscrow() Retry: %d.",retry);
                                               writeFileLog(log);
                                               continue;

                                         }


                                   }//external for loop end

                                   
                                   
                                   writeFileLog("[cancelTran()] after reject from escrow.");

                                   //clear global note number
                                   g_ttlNmbrOfNote=0;

                            
                         }//if(1==WaitforNoteAcptrLowlevelflagClosed() block
                         else
                         {
                                writeFileLog("[cancelTran()] Note Acceptor low level thread is not closed so no reject escrow can be done now!!.");
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

                                     writeFileLog("[cancelTran()] Before Wait for any reject state completion.");

                                     rtcode=WaitforNoteTakenBackByUser(1);

                                     if( 1 != rtcode )
                                     {

                                            writeFileLog("[cancelTran()] Before Going to close reject state operation.");

                                            StopRejectProcess();

                                            writeFileLog("[cancelTran()] After Going to close reject state operation.");
  

                                     }

                                     writeFileLog("[cancelTran()] After Wait for any reject state completion.");

                                     writeFileLog("[cancelTran()] Before  close all BNA CashIn Process.");      
    
                                     CompleteCashInProcess(); //++

                                     writeFileLog("[cancelTran()] After  close all BNA CashIn Process."); 

                                     


                      }

                      

                      #endif
                      

                  }
                  writeFileLog("[cancelTran()] after reject notes.");

                  #endif

                 #ifndef NOTE_ACCEPTOR
                 ret3=SUCCESS;
                 #endif

		  if(( SUCCESS == ret1 ) && ( SUCCESS == ret3 ))
                  {

		        writeFileLog("[canceltrans()] finished with success.");
                        memset(log,'\0',100); 
                        sprintf(log,"[canceltrans()] Transaction Number %d canceled with success status.",g_total_transaction);
                        writeFileLog(log);
                        return true;

                  }
		  else
                  {
		        
                        writeFileLog("[canceltrans()] finished with failed.");
                        memset(log,'\0',100); 
                        sprintf(log,"[canceltrans()] Transaction Number %d canceled with failed status.",g_total_transaction);
                        writeFileLog(log);
                        return false;

		  
		  }
		  
                  
		  
      
      }//if(THREAD_CLOSE==g_AcptFareThreadStatus) block
      else 
      {
           pthread_mutex_unlock(&AcptFareThreadStatusmutex);
	   writeFileLog("[cancelTran()] No acceptfare thread stated so canceltrans() return fail status.");
	   return false;
      }
	 


}//cancelTran() end here



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool acceptFare(int Fare,int maxNoOfCash,int maxNoOfCoin,int TransTime)
{
	  
          
			  writeFileLog("[acceptFare()] Entry.");
		          
                          char log[200];
		          
                          memset(log,'\0',200);

			  /////////////////////////////////////////////////////////////////////////////////////////
			  
                          if( Fare <  0)
			  {

                                memset(log,'\0',200);

			        sprintf(log,"[acceptFare()] Negative fare not accepeted. Given fare : %d ",Fare);
                                writeFileLog(log);
  
                                return false;


			  }

                           //Get Trans timeout from ini file
			   
			   if( TransTime <= 0 )
			   {

				memset(log,'\0',200);

			        sprintf(log,"[acceptFare()] Trans time is zero or less than zero given : %d ",TransTime);

			        writeFileLog(log);

                                return (-3);
  

			   }
			   else if( TransTime > 0 )
			   {

                                memset(log,'\0',200);

			        sprintf(log,"[acceptFare()] Trans time : %d ",TransTime);

			        writeFileLog(log);

				g_TransTimeOut=TransTime;

			   }


			  #if defined(NOTE_ACCEPTOR)

                            if(  maxNoOfCash < 0 ) 
                            {

                                memset(log,'\0',200);

			        sprintf(log,"[acceptFare() Exit] negative number of maximum number of note not accepeted. Given maxNoOfCash : %d ",maxNoOfCash);

			        writeFileLog(log);
  
                                return false;


			    }
                            else if(  maxNoOfCash > 20 ) 
                            {

                                memset(log,'\0',200);

			        sprintf(log,"[acceptFare() Exit] maximum number of note cris cross threshold value . Given maxNoOfCash : %d .",maxNoOfCash);

			        writeFileLog(log);
  
                                return false;


			    }

                          #endif

                          #if defined(COIN_ACCEPTOR)

                            if(  maxNoOfCoin < 0 ) 
                            {

                                memset(log,'\0',200);

			        sprintf(log,"[acceptFare() Exit] negative number of maximum number of coin accepeted. Given maxNoOfcoin : %d .",maxNoOfCoin);

			        writeFileLog(log);
  
                                return false;


			    }
                            else if(  maxNoOfCoin > 20 ) 
                            {

                                memset(log,'\0',200);

			        sprintf(log,"[acceptFare() Exit] maximum number of cris coin threshold value cross. Given maxNoOfcoin : %d .",maxNoOfCoin);

			        writeFileLog(log);
  
                                return false;


			    }

                          #endif

			  /////////////////////////////////////////////////////////////////////////////////////////
			  
                          pthread_t threadAcptFare;

			  int  iretAcptFare=-1;

			  int i=0,j=0;
			  
			  int day=0,mon=0,yr=0,hr=0,min=0,sec=0;

			  //////////////////////////////////////////////////////////////////////////
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

			  pthread_mutex_unlock(&mut);

			  /////////////////////////////////////////////////////////////////////////////

			  //Add By Malay on 31 Jan 2013
			  pthread_attr_t attr;

			  int returnVal=-1;

                          //////////////////////////////////////////////////////////////////////////////////////////////

                           pthread_mutex_lock(&mut);

                           g_Fare        = Fare;

                           g_maxNoOfCash = maxNoOfCash;

                           g_maxNoOfCoin = maxNoOfCoin;

                           //clear denomation array
			   for( i=0; i<  (MAX_NMBR_OF_NOTE + MAX_NMBR_OF_COIN); i++)  
			   {

				       for(j=0; j<2; j++) 
		                       {

					    g_acpted_CurrencyDetail[i][j] = -1; 

		                       }

			   } 

			   //Set previous state of this flag(Malay add)
			   g_ExternalKillAcptFareThread=THREAD_NO_KILL;

			   g_transactiontimeout=false; 

                           g_AllAlreadyDisableFlag=false;

                           pthread_mutex_unlock(&mut);

                           //////////////////////////////////////////////////////////////////////////////////////////////

                           #if defined(COIN_ACCEPTOR)
                           ClearCoinsCounter();
                           #endif

                           #if defined(NOTE_ACCEPTOR)
                           ClearNotesCounter();
                           #endif

                           //writeFileLog("\n\n");

                           #if defined(NOTE_ACCEPTOR)
			   SetAlreadyCoinInhibit();
			   #endif

			   
			   #if defined(COIN_ACCEPTOR)
			   InitCATransModel();
			   #endif
			   
			   #if defined(NOTE_ACCEPTOR)
			   InitNATransModel();
			   #endif


			   #ifdef NOTE_ACCEPTOR
			   //30 May 2013 add by malay Read recycle status before transaction
			   GetRecycleStatusBeforeStartAcceptfare();
			   #endif

                          ///////////////////////////////////////////////////////////////////////////////

                           bool StartNoteAcceptorRtcode=false,
                           StartCoinAcceptorRtcode=false,
                           StartManagerRtcode=false,
                           ActivateAllRtcode=false;
                           pthread_mutex_lock( &CloseAcceptFareThreadFlagmutex );

                           g_CloseAcceptFareThreadFlag =false;

                           pthread_mutex_unlock( &CloseAcceptFareThreadFlagmutex );

                           InitAcceptFaresModel();

                           StartNoteAcceptorRtcode = StartNoteAcceptorAllThreads(); //Level 1

                           if( true ==  StartNoteAcceptorRtcode)
                           {
                                 
                                   
                                   writeFileLog("[acceptFare()] Start NoteAcceptor All Threads started successfully.");

                                   StartCoinAcceptorRtcode=StartCoinAcceptorAllThreads(); //Level 2

                                   if( true ==  StartCoinAcceptorRtcode )
                                   {
                                              
                                               writeFileLog("[acceptFare()] Start CoinAcceptor All Threads started successfully.");

		                               StartManagerRtcode = StartManagerThread(); //Level 3

		                               if( true ==  StartManagerRtcode)
		                               {

		                                            ActivateAllRtcode=ActivateAllAcceptors(); //Level 4

		                                            if( true ==  ActivateAllRtcode)
		                                            {


                                                                  writeFileLog("[acceptFare()] Start Acceptfare Manager Thread started successfully.");
                                                                  writeFileLog("[acceptFare()] Before Start All Credit Polling Threads.");
                                                                  StartAllAcceptProcessThreads();

                                                                  writeFileLog("[acceptFare()] After Start All Credit Polling Threads.");


                                                            }
                                                            else if( false ==  ActivateAllRtcode)
		                                            {


                                                                      writeFileLog("[acceptFare()] Start Acceptfare Manager thread failed to start.");
                                                                      StopAllAcceptProcessThreads(LEVEL4);

                                                                      return false;


		                                            }

 
		                               }
                                               else if( false ==  StartManagerRtcode)
                                               {

                                                      writeFileLog("[acceptFare()] Start Acceptfare Manager thread failed to start.");

                                                      StopAllAcceptProcessThreads(LEVEL3);

                                                      return false;
 
 
                                               }         
                                              
                                   }
                                   else if( false ==  StartCoinAcceptorRtcode )
                                   {
                                          
                                           writeFileLog("[acceptFare()] Start CoinAcceptor All Threads failed to start.");

                                           StopAllAcceptProcessThreads(LEVEL2);

                                           return false;


                                   }

                           }
                           else if( false ==  StartNoteAcceptorRtcode)
                           {

                                writeFileLog("[acceptFare()] Start NoteAcceptor All Threads failed to start.");

                                StopAllAcceptProcessThreads(LEVEL1);

                                return false;

                           }       
                    
                          ///////////////////////////////////////////////////////////////////////////////

                           //make a log about current fare

			   g_total_transaction++; 

                           memset(log,'\0',200);

			   sprintf(log,"[acceptFare()] Transaction Number : %d.",g_total_transaction);

			   writeFileLog(log);

                           //Make a log about fare
			   getDateTime(&day,&mon,&yr,&hr,&min,&sec);

                           memset(log,'\0',200);

			   sprintf(log,"[acceptFare()] Transaction Date(dd/mm/yyyy)= %d/%d/%d Start Time(hh:mm:ss)= %d:%d:%d.",day,mon,yr,hr,min,sec);
			   writeFileLog(log);     

                           
                           pthread_mutex_lock(&mut);

			   memset(log,'\0',200);

			   sprintf(log,"[acceptFare()] Transaction Fare= %d",g_Fare);

			   writeFileLog(log);     


                           memset(log,'\0',200);

			   sprintf(log,"[acceptFare() Exit] Max Numbers of Cash= %d  .",g_maxNoOfCash);

			   writeFileLog(log);   

                           pthread_mutex_unlock(&mut);
 
                           return true;




}//acceptFare() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int  GetFare()
{
         
          int fareamount=0;

          pthread_mutex_lock(&mut);

          fareamount = g_Fare ;
        
          pthread_mutex_unlock(&mut);

          return fareamount;


}

int  GetCurrentAcceptedAmount()
{
         
          int acceptedamount=0;

          pthread_mutex_lock(&mut);

          acceptedamount = g_totalAmountAccepted ;
        
          pthread_mutex_unlock(&mut);

          return acceptedamount;

}


int getDenomination(int arry[][2])
{



                                  //writeFileLog("[getDenomination()] Entry.");

				  int i = 0, j = 0;

				  int fare=0,totalAmountAccepted=0;

                                  char log[200];
 
                                  memset(log,'\0',200);

                                  //malay add 29 Jan 2014
				  pthread_mutex_lock(&mut);

                                  for(i=0; i<MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN; i++)
				  {

					 for(j = 0; j<2; j++)
				         {
					     
				             arry[i][j] = g_acpted_CurrencyDetail[i][j];
				         
				         }

					     
				   }

				  //Max no cash and coin accepted 
			          if( (g_ttlNmbrOfNote >= g_maxNoOfCash) && (g_maxNoOfCash >0) 
                                       && (g_Fare != g_totalAmountAccepted)  )
                                  {

                                             pthread_mutex_unlock(&mut);

                                             writeFileLog("[getDenomination() Exit] Note Escrow Full.");

                                             return (-2);   

                                  }
                                
                                  //API timeout happened (Internally and Externally) and accepting state closed by forcefully
				  else if(  true == g_transactiontimeout  )
				  { 

                                             pthread_mutex_unlock(&mut);

					     writeFileLog("[getDenomination() Exit] Transaction timeout from api.");

					     return 0;  

				  }

                                  //Accepting State stopped and accepted fare equal to fare
				  else if(  g_Fare == g_totalAmountAccepted  )
				  {
                                        
                                            pthread_mutex_unlock(&mut);

                                            writeFileLog("[getDenomination() Exit] Accepted total fare.");

			                    return 2;  //Exact Amount

					     
				  }
                                  //Accepting State stopped and accepted fare greater than fare
				  else if( g_Fare < g_totalAmountAccepted )
				  {

                                             pthread_mutex_unlock(&mut);

                                             writeFileLog("[getDenomination() Exit] Accepted fare greater than Selected fare.");

					     return 3;   //Excess Fare

				  }
                                  //Accepting State running
				  else if(  g_Fare > g_totalAmountAccepted  )
				  { 

                                            //writeFileLog("[getDenomination() Exit] Accepting State running.");

                                            pthread_mutex_unlock(&mut);

                                            return 1; //Accepting State
 
					      
				  }
			          else 
				  {

                                             pthread_mutex_unlock(&mut);

                                             writeFileLog("[getDenomination() Exit] No Accepting sequence matched!!!.");

					     return (-3);  //Exception State

				  }






}//getDenomination() end here


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void* ThreadProc_acceptFare(void *ptr)
{
	  

             writeFileLog("[ThreadProc_acceptFare()] Entry.");

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               
             
             //wait for signal
             pthread_mutex_lock( &g_AcceptFareEnableThreadmutex );

             pthread_cond_wait( &g_AcceptFareEnableThreadCond  , &g_AcceptFareEnableThreadmutex  );

             pthread_mutex_unlock( &g_AcceptFareEnableThreadmutex );


           //////////////////////////////////////////////////////////////////////////////////////////////////////////////

            pthread_mutex_lock( &CloseAcceptFareThreadFlagmutex );

            
            //if credit poll flag is set exit from thread
            if( true == g_CloseAcceptFareThreadFlag )
            {
                     
                    g_CloseAcceptFareThreadFlag=false;

                    pthread_mutex_unlock (&CloseAcceptFareThreadFlagmutex );

                    pthread_mutex_lock(&AcptFareThreadStatusmutex);

		    g_AcptFareThreadStatus=THREAD_CLOSE;

		    pthread_mutex_unlock(&AcptFareThreadStatusmutex);

                    writeFileLog("[ThreadProc_acceptFare() Exit] Thread Exit.");
				                   
                    pthread_exit(0);


	   }
           else
           {

                   pthread_mutex_unlock( &CloseAcceptFareThreadFlagmutex );

                   writeFileLog("[ThreadProc_acceptFare()] Thread Running.");

           }

          //////////////////////////////////////////////////////////////////////////////////////////

          //Intimate device to start accepting currency for the total ticket fare with max number of notes and coins to be accepted for    single one transaction . API will return true if all the devices are ready to accept fare. 
	  
          struct timespec begints={0},endts={0},diffts={0};

          pthread_t thread1,thread2;

	  int  iret1=-1,iret2=-1,CoinCreditThreadRtcode=-1,NotePollThreadRtcode=-1,returnVal=-1;

          unsigned char log[200];

          memset(log,'\0',200);




   writeFileLog("[ThreadProc_acceptFare()] Now Going to monitor accepting process.");

   //Start Transtime
   clock_gettime(CLOCK_MONOTONIC, &begints);
	
   while(1) // Set some Time Out.
   {


			    
                            //Get Current Time
                            clock_gettime(CLOCK_MONOTONIC, &endts);

                            //Differnce between start and end time
			    diffts.tv_sec = endts.tv_sec - begints.tv_sec;
                            
                            //API Transaction Timeout
                            if( diffts.tv_sec >= g_TransTimeOut )
			    {

				       writeFileLog("[ThreadProc_acceptFare()] Start Transaction Timeout Block.");

                                       pthread_mutex_lock(&mut);

                                       //Signal Low Level Note And Coin Credit Polling Thread to closed their operation
                                       g_transactiontimeout=true;

                                       pthread_mutex_unlock(&mut);

                                       StopAcceptingState();
        
                                       writeFileLog("[ThreadProc_acceptFare()] End Transaction Timeout Block.");
				       
				       break;
  
  
			    }	   

                            //////////////////////////////////////////////////////////////////////////////////

                            pthread_mutex_lock(&mut);

                            //External Anytime Accept process close 
			    //Malay Add 19 Jan 2013
			    if( THREAD_KILL == g_ExternalKillAcptFareThread )
			    {
				       
                                       writeFileLog("[ThreadProc_acceptFare()] Start External Kill Thread Block.");

                                       pthread_mutex_unlock(&mut);

                                       StopAcceptingState();

                                       writeFileLog("[ThreadProc_acceptFare()] End External Kill Thread Block.");

                                       break;

			    }
    

                            //Accepted money greater than fare selected
			    if( g_totalAmountAccepted > g_Fare  )
			    {

                                       pthread_mutex_unlock(&mut);

				       writeFileLog("[ThreadProc_acceptFare()] Start Greater than fare Block.");
                                      
                                       StopAcceptingState();

				       writeFileLog("[ThreadProc_acceptFare()] End Greater than fare Block.");
 
                                       break;
				       
			    }
 
                         

			   //Max no of cash and Max no Coin limit crossed [default cash and coin max is 20]
			   if( ( (g_ttlNmbrOfNote >= g_maxNoOfCash) && (g_maxNoOfCash >0) ) || 
                              (  (g_ttlNmbrOfCoin >= g_maxNoOfCoin) && (g_maxNoOfCoin >0) )
                             )
			   {
                                     
                                        pthread_mutex_unlock(&mut);
					
                                        writeFileLog("[ThreadProc_acceptFare()] Start Maximum Cash or Coin Equal Block.");
                                     
                                        StopAcceptingState();
				       
			                memset(log,'\0',200);

		                        sprintf(log,"[ThreadProc_acceptFare()]  Accepted Total Notes= %d Accepted Total Coins= %d.", g_ttlNmbrOfNote,g_ttlNmbrOfCoin);

                                        writeFileLog(log);

                                        writeFileLog("[ThreadProc_acceptFare()] End Maximum Cash or Coin Equal Block.");

                                        break; 

			
			    }
     
                            

		            //Accepted money equal to fare selected
			    if( g_Fare == g_totalAmountAccepted )
			    {

                                       pthread_mutex_unlock(&mut);

				       writeFileLog("[ThreadProc_acceptFare()] Start Equal fare Block.");
                                       
                                       StopAcceptingState();
                                       
				       writeFileLog("[ThreadProc_acceptFare()] End Equal fare accepted.");
 
                                       break;

					   
			    }

                          pthread_mutex_unlock(&mut);

                          //////////////////////////////////////////////////////////////////////////////////

	
    } // End while


    //Log Trans Data
    //LogThisTransData();

    //Now Close Thread
    writeFileLog("[ThreadProc_acceptFare()] Now set flag to close status.");

    pthread_mutex_lock(&AcptFareThreadStatusmutex);

    g_AcptFareThreadStatus=THREAD_CLOSE;

    pthread_mutex_unlock(&AcptFareThreadStatusmutex);

    writeFileLog("[ThreadProc_acceptFare() Exit] End ThreadProc_acceptFare Thread.");

    pthread_exit(0);
				       
   

} //End ThreadProc_acceptFare function


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline void InitAcceptFaresModel()
{

          
          writeFileLog("[InitAcceptFaresModel()] Entry.");
         
          #ifdef NOTE_ACCEPTOR

          //check range of cash and coin (valid range 0-20 and other any invalid range)
          if( (g_maxNoOfCash >= 0) && (g_maxNoOfCash <= MAX_NMBR_OF_NOTE))
          {
                writeFileLog("[InitAcceptFaresModel()] Valid range maxnoofcash.");
          }
	  else
          {
                //g_maxNoOfCash is negative number
                g_maxNoOfCash=0;
                
                /*////////////////////////////////////////////////////////////////////////

	        if( g_Fare > 0)
                {
                    g_maxNoOfCash = 20;
                }
                else
                {  
                    g_maxNoOfCash = 0;

                }

                *//////////////////////////////////////////////////////////////////////////

                writeFileLog("[InitAcceptFaresModel()] Invalid range maxnoofcash.");

	  }
    
          #endif

          #ifdef COIN_ACCEPTOR

	  if( (g_maxNoOfCoin >= 0) && (g_maxNoOfCoin <=  MAX_NMBR_OF_COIN))
          {
                writeFileLog("[InitAcceptFaresModel()] Valid range maxnoofcoin.");
          }
	  else
          {

                 //g_maxNoOfCoin is negative number
                 g_maxNoOfCoin=0;

                 /*///////////////////////////////////////////////////////////////////////////

	         if(g_Fare > 0)
                 {
                     g_maxNoOfCoin = 20;
                 }
                 else
                 { 
                     g_maxNoOfCoin = 0;

                 }

                 *///////////////////////////////////////////////////////////////////////////

                 writeFileLog("[InitAcceptFaresModel()] Invalid range maxnoofcoin.");

          }

          #endif

          ////////////////////////////////////////////////////////////////////////////////////////

          #ifdef COIN_ACCEPTOR

          if( 0 == g_maxNoOfCoin)
          {
               g_CoinFullDisable=1; //set (runtime control disabled)
               writeFileLog("[InitAcceptFaresModel()] Runtime Coin Control Disabled.");
          }
          else
          {
               g_CoinFullDisable=0;//default (runtime control enabled)
               writeFileLog("[InitAcceptFaresModel()] Runtime Coin Control Enabled.");
          }

          #endif

          #ifdef NOTE_ACCEPTOR

          if( 0 == g_maxNoOfCash)
          {
               g_NoteFullDisable=1; //set (runtime control disabled)
               writeFileLog("[InitAcceptFaresModel()] Runtime Note Control Disabled.");
               
          }
          else
          {
               g_NoteFullDisable=0; //default (user runtime control enabled)
               writeFileLog("[InitAcceptFaresModel()] Runtime Note Control Enabled.");
          }
           
          #endif
          
          #ifdef NOTE_ACCEPTOR
          SetCoinFullDisableFlag(g_maxNoOfCash,g_maxNoOfCoin);
	  #endif

          pthread_mutex_lock(&RuntimeInhibitFlagmutex);

          //malay add 3 feb 2013
          g_RuntimeInhibitFlag=1;

          pthread_mutex_unlock(&RuntimeInhibitFlagmutex);
	   
          writeFileLog("[InitAcceptFaresModel()] Exit.");
          
          return;



}//InitAcceptFaresModel() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline bool ActivateAllAcceptors()
{


           int rtcode=FAIL;
 
           rtcode = makeAccepters_at_ActiveteMode(g_Fare,g_maxNoOfCash,g_maxNoOfCoin);

           //Now time to activate note and coin acceptor	   
	   if( FAIL == rtcode ) 
	   {     

		      writeFileLog("[ActivateAllAcceptors()] makeAccepters_at_ActiveteMode() return fail.");
                      return false;  
  

	   }
           else if( SUCCESS == rtcode ) 
           {

                     writeFileLog("[ActivateAllAcceptors()] makeAccepters_at_ActiveteMode() return success.");
                     
                     return true;

           }




}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline bool StartManagerThread()
{

        
	                              
                           pthread_t threadAcptFare;
          
			   int iretAcptFare=-1,returnVal=-1;

			   pthread_attr_t attr;

                           //Create the detached thread using POSIX routines add by malay on 31 Jan 2013

			   returnVal = pthread_attr_init(&attr);

			   if(0!=returnVal)
			   {

			       writeFileLog("[StartManagerThread()] acceptFareThread attribute create failed so return here from.");

			       return false;

			   }

			   returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			   if(0!=returnVal)
			   {

			       writeFileLog("[StartManagerThread()] acceptFareThread thread attribute detached create failed so return here from.");
			       return false;

			   }

			   //Thread Create here
			   iretAcptFare= pthread_create(&threadAcptFare, &attr,&ThreadProc_acceptFare, NULL);

			   //Malay Add 19 Jan 2013
			   //Signal External Function that acpt fare thread start now
			   if(0==iretAcptFare)
			   {

			     g_AcptFareThreadStatus=THREAD_START;

			     writeFileLog("[StartManagerThread()] acceptFareThread Creation Success.");
                             
			   }
			   else
			   {

			      g_AcptFareThreadStatus=THREAD_CLOSE;

			      writeFileLog("[StartManagerThread()] acceptFareThread Creation Failed!.");

			      return false;

			   }

			   returnVal = pthread_attr_destroy(&attr);

			   if(0!=returnVal)
			   {

			     writeFileLog("[StartManagerThread()] acceptFareThread thread attribute destroy failed!.");

			   }
			   
                           return true;


}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Level 1
static inline bool StartNoteAcceptorAllThreads()
{
       

           #if defined(NOTE_ACCEPTOR)

           bool lowlevelthread=false; 

           pthread_t thread1;
          
           int returnVal=-1,iret1=-1;

           pthread_attr_t attr1;

           returnVal = pthread_attr_init(&attr1);

           if(0!=returnVal)
           {

               writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart Thread attribute create failed so return here from.");
               
           }

	   returnVal = pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);

           if(0!=returnVal)
           {

               writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart thread attribute detached create failed so return here from.");
              
 
           }
	   iret1     = pthread_create(&thread1, &attr1,&ThreadProc_NoteAcptrStart, NULL);

           if(0==iret1)
	   {

	        writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart Creation Success.");

                lowlevelthread=true;
	   }
	   else
	   {
              
	        writeFileLog("[acceptFare()] ThreadProc_NoteAcptrStart Creation Failed!.");
	      
	   }

	   returnVal = pthread_attr_destroy(&attr1);

           if(0!=returnVal)
           {
             writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc_NoteAcptrStart thread attribute destroy failed!.");
           }
           
           
           if(true == lowlevelthread)
           {

		   //Start Note Acceptor Low level thread

		   unsigned int NALowlevelThreadRtcode=0;

		   NALowlevelThreadRtcode=NoteAcptrStartCreditPollThread();

		   if( 1 != NALowlevelThreadRtcode )
		   {

		        writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc1_CASHCODE_NoteAcptrPoll thread create failed!!!.");

		        return false;
		   }
		   else
                   {

		        writeFileLog("[StartNoteAcceptorAllThreads()] ThreadProc1_CASHCODE_NoteAcptrPoll thread create successed.");  

                        return true;

                   }


         } 
         else if( false == lowlevelthread)
         {

                return false;

         }
        
         #endif

        #if !defined(NOTE_ACCEPTOR)
        return true;
        #endif
 


}//StartNoteAcceptorAllThreads() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Level 2
static inline bool StartCoinAcceptorAllThreads()
{

 
           #if defined(COIN_ACCEPTOR)

           int returnVal=-1,CoinCreditThreadRtcode=-1,iret2=-1;

           pthread_attr_t attr2;

           pthread_t thread2;

           bool lowlevelthread=true; 

           //Start Coin Acceptor Upper Level Thread
           returnVal = pthread_attr_init(&attr2);

           if(0!=returnVal)
           {
               writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc_CoinAcptrStart Thread attribute create failed so return here from.");
               
           }

	   returnVal = pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);

           if(0!=returnVal)
           {

               writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc_CoinAcptrStart thread attribute detached create failed so return here from.");
               
           }

	   iret2     = pthread_create(&thread2, &attr2,&ThreadProc_CoinAcptrStart, NULL);

           if(0==iret2)
	   {

              lowlevelthread=true;

	      writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart Creation Success.");

	   }
	   else
	   {

	      writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart Creation Failed!.");
	      
	   }

	   returnVal = pthread_attr_destroy(&attr2);

           if(0!=returnVal)
           {

             writeFileLog("[StartCoinAcceptorAllThreads()] ThreadProc2_CoinAcptrStart thread attribute destroy failed!.");

           }
           
           if( true == lowlevelthread )
           {

		   //Now Start Coin acceptor low level thread
		   CoinCreditThreadRtcode=MONEYCONTROL_CoinAcptrCreateCreditPoll();

		   if(1!=CoinCreditThreadRtcode)
		   {

		        writeFileLog("[StartCoinAcceptorAllThreads()] MONEYCONTROL_CoinAcptrCreateCreditPoll thread create failed!!!.");

                        return false;
		        
		   }
		   else
		   {

		        writeFileLog("[ThreadProc_acceptFare()] MONEYCONTROL_CoinAcptrCreateCreditPoll thread create successed.");

                        return true;

		   }
           }
           else if( false == lowlevelthread )
           {

                 return false;

           }
           #endif

           #if !defined(COIN_ACCEPTOR)
           return true;
           #endif




}//StartCoinAcceptorAllThreads() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void StartAllAcceptProcessThreads()
{

         
                             writeFileLog("[StartAllAcceptProcessThreads()] Entry.");

                             #if defined(NOTE_ACCEPTOR)

                             writeFileLog("[StartAllAcceptProcessThreads()] Entry Enable all Note Acceptor Threads.");

                             //Start Note Acceptort All Level Threads
                             pthread_mutex_lock(&noteAcptrStartFlagmutex);

                             g_noteAcptrStartFlag = ON ;
  
                             pthread_mutex_unlock(&noteAcptrStartFlagmutex);

                             pthread_mutex_lock( &g_NAULEnableThreadmutex );

                             pthread_cond_signal( &g_NAULEnableThreadCond );

                             pthread_mutex_unlock( &g_NAULEnableThreadmutex );

                             NoteAcptrStartSignalCreditPollThread();

                             writeFileLog("[StartAllAcceptProcessThreads()] Exit Enable all Note Acceptor Threads.");

                             #endif

                             #if defined(COIN_ACCEPTOR)

                             writeFileLog("[StartAllAcceptProcessThreads()] Entry Enable all Coin Acceptor Threads.");

                             //Start Coin Acceptort All Level Threads

                             writeFileLog("[StartAllAcceptProcessThreads()] Entry Coin Acceptor Upper Level Threads.");

                             pthread_mutex_lock(&coinAcptrStartFlagmutex);

			     g_coinAcptrStartFlag = ON;

                             pthread_mutex_unlock(&coinAcptrStartFlagmutex);

                             writeFileLog("[StartAllAcceptProcessThreads()] Exit Coin Acceptor Upper Level Threads.");

                             
                             //wait for signal

                             writeFileLog("[StartAllAcceptProcessThreads()] Entry Coin Acceptor Upper Level Threads Signal.");

			     pthread_mutex_lock(&g_CAULEnableThreadmutex);

			     pthread_cond_signal( &g_CAULEnableThreadCond  );

			     pthread_mutex_unlock(&g_CAULEnableThreadmutex);

                             writeFileLog("[StartAllAcceptProcessThreads()] Exit Coin Acceptor Upper Level Threads Signal.");

			     CoinAcptrStartSignalCreditPollThread();
                           
                             writeFileLog("[StartAllAcceptProcessThreads()] Exit Enable all Coin Acceptor Threads.");

                             #endif
                            


                             writeFileLog("[StartAllAcceptProcessThreads()] Entry Enable Acceptfare Manager Thread.");

                             //Start Acceptfare Thread
                             pthread_mutex_lock( &CloseAcceptFareThreadFlagmutex );

                             g_CloseAcceptFareThreadFlag =false;

                             pthread_mutex_unlock( &CloseAcceptFareThreadFlagmutex );
 
                             pthread_mutex_lock( &g_AcceptFareEnableThreadmutex );

                             pthread_cond_signal( &g_AcceptFareEnableThreadCond );

                             pthread_mutex_unlock( &g_AcceptFareEnableThreadmutex );

                             writeFileLog("[StartAllAcceptProcessThreads()] Exit Enable Acceptfare Manager Thread.");

                             writeFileLog("[StartAllAcceptProcessThreads()] Exit.");

                             return;



}//StartAllAcceptProcessThreads() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void StopAllAcceptProcessThreads(int Level)
{


               switch(Level)
               {

 
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

           };

           Wait( 2 );

           return;


}//StopAllAcceptProcessThreads() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline void DisableAllAcceptorV2()
{
   
          

          #if defined(COIN_ACCEPTOR)
          
	  DisableAllCoins();
	 
          #endif

          #if  defined (NOTE_ACCEPTOR)
     
          IssuedisableAllBillCmd();

          #endif

	  


  
}//DisableAllAcceptorV2() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline void DisableAllAcceptor()
{
   

          #if defined(COIN_ACCEPTOR)

          if( 1 == WaitforCoinAcptrLowlevelflagClosed(WAIT_TIME_COIN_ACPTR_FINISH_LOWLEVEL_THREAD) )
          {

               
	       DisableAllCoins();
	 
          }

          #endif

          #if  defined (NOTE_ACCEPTOR)
          
	  if( 1 == WaitforNoteAcptrLowlevelflagClosed(WAIT_TIME_NOTE_ACPTR_FINISH_LOWLEVEL_THREAD))
          {

	       IssuedisableAllBillCmd();

          }
          
          #endif

	  delay_mSec(200);


  
}//DisableAllAcceptor() end here

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static  int makeAccepters_at_ActiveteMode(int fare,int maxNoteQtyAllowed,int maxCoinQtyAllowed)
{
          
         

	  int stateNote = FAIL; 

	  int stateCoin = FAIL;
  
	  #if defined (COIN_ACCEPTOR)
          
          if( FAIL != enableSpecificCoinsAndMakeIdle(fare,maxCoinQtyAllowed) )
	  {
	      writeFileLog("[makeAccepters_at_ActiveteMode()] Coin acceptor activate successed.");
	      stateCoin = SUCCESS;
	  }
	  else
	  {
	     writeFileLog("[makeAccepters_at_ActiveteMode()] Coin acceptor activate failed.");
	     return FAIL;
	  }
     
          #endif

          #if !defined (COIN_ACCEPTOR)
          stateCoin = SUCCESS;
          #endif


          #if defined (NOTE_ACCEPTOR)

	  // Added on 29-Oct-12
	  if(  FAIL != enableSpecificBillsAndMakeIdle(fare,maxNoteQtyAllowed) )
	  {
	       writeFileLog("[makeAccepters_at_ActiveteMode()] Note acceptor activate successed.");
	       stateNote = SUCCESS;
	  }
	  else
	  {
	      writeFileLog("[makeAccepters_at_ActiveteMode()] Note acceptor activate failed.");
	      return FAIL;
	  }

          #endif

          #if !defined (NOTE_ACCEPTOR)
          stateNote = SUCCESS;
          #endif

	  
	 if( (SUCCESS == stateNote ) && ( SUCCESS == stateCoin ))
         {
	     return (SUCCESS);
         }
	 else
         {
	     return (FAIL);
         }


}//makeAccepters_at_ActiveteMode() end here


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NOTE_ACCEPTOR)
void IncrementNotesCounter(int NoteValue)
{

       switch(NoteValue)
       {

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(COIN_ACCEPTOR)

void ClearCoinsCounter()
{

        g_CoinsCounter.C1=0;
  
        g_CoinsCounter.C5=0;
   
        g_CoinsCounter.C10=0;
       
        return;

}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NOTE_ACCEPTOR)

void ClearNotesCounter()
{
       

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LogThisTransData()
{

            
          char log[200];

          memset(log,'\0',200);

          #if defined(NOTE_ACCEPTOR)

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.5 Notes: %d .Total Rs.5 Notes Value= %d .", g_NotesCounter.N5,(g_NotesCounter.N5*5) );
          writeFileLog(log);
 
          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.10 Notes: %d .Total Rs.10 Notes Value= %d .", g_NotesCounter.N10,(g_NotesCounter.N10*10));
          writeFileLog(log);

          
          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.20 Notes: %d .Total Rs.20 Notes Value= %d .", g_NotesCounter.N20,(g_NotesCounter.N20*20));
          writeFileLog(log);


          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.50 Notes: %d .Total Rs.50 Notes Value= %d .", g_NotesCounter.N50,(g_NotesCounter.N50*50));
          writeFileLog(log);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.100 Notes: %d .Total Rs.100 Notes Value= %d .", g_NotesCounter.N100,(g_NotesCounter.N100*100));
          writeFileLog(log);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.500 Notes: %d .Total Rs.500 Notes Value= %d .", g_NotesCounter.N500,(g_NotesCounter.N500*500));
          writeFileLog(log);
         
          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.1000 Notes: %d .Total Rs.1000 Notes Value= %d .", g_NotesCounter.N1000,(g_NotesCounter.N1000*1000));
          writeFileLog(log);

          #endif

          #if defined(COIN_ACCEPTOR)

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.1 Coins: %d .Total Rs.1 Coins Value: %d .", g_CoinsCounter.C1,(g_CoinsCounter.C1*1) );
          writeFileLog(log);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.5 Coins: %d .Total Rs.5 Coins Value: %d .", g_CoinsCounter.C5,(g_CoinsCounter.C5*5));
          writeFileLog(log);

          memset(log,'\0',200);
          sprintf(log,"[LogThisTransData()] Total Rs.10 Coins: %d .Total Rs.10 Coins Value: %d .", g_CoinsCounter.C10,(g_CoinsCounter.C10*10));
          writeFileLog(log);

          #endif

          #if defined(NOTE_ACCEPTOR)

          int TotalNoteValue=0;

          TotalNoteValue= ( (g_NotesCounter.N5*5)+(g_NotesCounter.N10*10)+(g_NotesCounter.N20*20)+(g_NotesCounter.N50*50)+
          (g_NotesCounter.N100*100)+(g_NotesCounter.N500*500)+(g_NotesCounter.N1000*1000) );

          memset(log,'\0',200);

          sprintf(log,"[LogThisTransData()] Total Notes Value= %d .", TotalNoteValue );

          writeFileLog(log);

          #endif

          #if defined(COIN_ACCEPTOR)

          int TotalCoinValue=0;

          TotalCoinValue = ( (g_CoinsCounter.C1*1)+(g_CoinsCounter.C5*5)+(g_CoinsCounter.C10*10) );

          memset(log,'\0',200);

          sprintf(log,"[LogThisTransData()] Total Coins Value= %d .", TotalCoinValue );

          writeFileLog(log);

          #endif


          return;

}//LogThisTransData() end here


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NOTE_ACCEPTOR)

//Note Acceptor Thread
void* ThreadProc_NoteAcptrStart(void *ptr)
{   
  

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //wait for signal
  pthread_mutex_lock( &g_NAULEnableThreadmutex );

  pthread_cond_wait( &g_NAULEnableThreadCond , &g_NAULEnableThreadmutex );

  pthread_mutex_unlock( &g_NAULEnableThreadmutex );

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  pthread_mutex_lock(&noteAcptrStartFlagmutex);

  if( OFF == g_noteAcptrStartFlag )
  {

       pthread_mutex_unlock(&noteAcptrStartFlagmutex);

       writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Exit."); 

       pthread_exit(0);

  }
  else
  {

         pthread_mutex_unlock(&noteAcptrStartFlagmutex);
         writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Running."); 

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		    if( ON == g_noteAcptrStartFlag  )
		    {

			    
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
		            #endif

		            #ifdef JCM_NOTE_ACCEPTOR 
		            
			    noteValue=GetNoteDetailInEscrow();

		            #endif

		            #ifdef BNA_NOTE_ACCEPTOR 
		            
			    noteValue=GetNoteDetailInEscrow();

		            #endif


		            #ifdef B2B_NOTE_ACCEPTOR 
		            if( ( SUCCESS == ret ) && 
		                ( CASHCODE_RES_STACKED == noteStatus ) && 
		                ( (noteValue >= 5)  &&  (noteValue <= 1000) ) 
		              )    
		            #endif
		            #ifdef JCM_NOTE_ACCEPTOR 
		            if( (noteValue >= 5)  &&  (noteValue <= 1000) ) 
		            #endif
		            #ifdef BNA_NOTE_ACCEPTOR 
		            if( (noteValue >= 5)  &&  (noteValue <= 1000) ) 
		            #endif
			    {

				       pthread_mutex_lock(&mut);   

				       g_totalNoteAmountAccepted += noteValue;
		                       
		                       g_totalAmountAccepted += noteValue;

				       g_ttlNmbrOfNote +=1; 
				      
				       g_acpted_CurrencyDetail[g_denomIndex][0] = NOTE; 
                                       
                                       
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
                                                 g_acpted_CurrencyDetail[g_denomIndex][1]= 0;
                                                 break;

                                       };
				       
                                       //g_acpted_CurrencyDetail[g_denomIndex][1] = noteValue;   
   
				       g_denomIndex+=1;    
		                       
                                       
                                       IncrementNotesCounter(noteValue);
                               
				       writeFileLog("\n"); 
		                       memset(log,'\0',200);
		                       sprintf(log,"[ThreadProc_NoteAcptrStart()] Accepted Note : Rs. %d  Current Accepted Ammount : Rs. %d .", noteValue,g_totalAmountAccepted);
				       writeFileLog(log);

		                       //clear array
		                       memset(log,'\0',200);
		                       sprintf(log,"[ThreadProc_NoteAcptrStart()] Total Notes Number : %d .", g_ttlNmbrOfNote);
				       writeFileLog(log);
				       
		                       //Store Event
				       PrevNoteEvent=NoteEvent;
				       NoteEvent=NoteEvent+1;

				       pthread_mutex_unlock(&mut); 

				      
			    }//End if : ( CASHCODE_RES_STACKED / CASHCODE_RES_ESCROW)
		    

                            pthread_mutex_lock(&RuntimeInhibitFlagmutex);

			    //Runtime Inhibit
			    if( (NoteEvent>PrevNoteEvent)  && (1==g_RuntimeInhibitFlag) )
			    {

                                       
                                       pthread_mutex_unlock(&RuntimeInhibitFlagmutex);

				       //Runtime Inhibit lock
				       pthread_mutex_lock(&Inhibitmutex); //Malay add 1 feb 2013

		                       #ifdef ATVM_DEBUG

		                       writeFileLog("[ThreadProc_NoteAcptrStart()] Going to disable specific coins and notes.");
		                       
		                       //clear log array
		                       
		                       memset(log,'\0',200);	       

		                       sprintf(log,"[ThreadProc_NoteAcptrStart()]  Fare : Rs. %d  Current Accepted Fare: Rs. %d .", g_Fare,g_totalAmountAccepted);
				       writeFileLog(log);

		                       #endif

                                       pthread_mutex_lock(&mut);
 
		                       //Calculate current fare to be accept
		                       NACurrentfare=g_Fare - g_totalAmountAccepted;

                                       pthread_mutex_unlock(&mut);

		                       //#ifdef ATVM_DEBUG

		                       memset(log,'\0',200);	       

		                       sprintf(log,"[ThreadProc_NoteAcptrStart()] Current Fare to be accept: Rs. %d .", NACurrentfare);

				       writeFileLog(log);

		                       //#endif
	 
                                       pthread_mutex_lock(&mut);

		                       //Runtime all coin and note inhibit
		                       if( ( ( g_maxNoOfCash > 0 ) && (g_ttlNmbrOfNote >= g_maxNoOfCash) )               || 
				           ( ( g_maxNoOfCoin > 0 ) && (g_ttlNmbrOfCoin >= g_maxNoOfCoin) )               ||
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
				                          writeFileLog("[ThreadProc_NoteAcptrStart()] Inhibit from Note Event.");
				                          //#endif

				                    }


		                       }
		                       else //normal
		                       {
                                              
                                               

                                               pthread_mutex_unlock(&mut);

					       #if defined(COIN_ACCEPTOR)       
		                               if( 0 ==  g_CoinFullDisable )
		                               {
		                                     
		                                     //Runtime coin inhibit
						     SetSpecificCoinInhibitStatus(NACurrentfare);
		                                     
		                               }
		                               #endif

		                               #if defined(NOTE_ACCEPTOR) 
		                               if( 0 ==  g_NoteFullDisable )
		                               {
						     
		                                     
		                                     //Runtime Note inhibit
						     SetSpecificNoteInhibitStatus(NACurrentfare);
		                                     

		                                     
		                              }
		                              #endif


		                       }

				       NoteEvent=0;
				       PrevNoteEvent=0;

		                       #ifdef ATVM_DEBUG
		                       writeFileLog("[ThreadProc_NoteAcptrStart()] Successfully disable specific coins and notes.");
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

				   //Runtime all coin and note inhibit
				   if(( ( g_maxNoOfCash > 0 ) && (g_ttlNmbrOfNote >= g_maxNoOfCash) )               || 
				     ( ( g_maxNoOfCoin > 0 ) && (g_ttlNmbrOfCoin >= g_maxNoOfCoin) )                ||
				     (  THREAD_KILL == g_ExternalKillAcptFareThread )                               ||
				     (  true == g_transactiontimeout )                                              ||
				     (  g_Fare == g_totalAmountAccepted )                                           ||
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
		                                            writeFileLog("[ThreadProc_NoteAcptrStart()] Inhibit from Note External.");
		                                            //#endif

		                                    }    


				     }
                                     else
                                     {
                                          pthread_mutex_unlock(&mut);

                                     }

		           pthread_mutex_unlock(&ExternalInhibitmutex);

	     
	   }//End if g_noteAcptrStartFlag == ON
	   
	   else if( OFF == g_noteAcptrStartFlag )
	   {
               pthread_mutex_unlock(&noteAcptrStartFlagmutex);
	       writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Exit."); 
	       pthread_exit(0);

	   }
     

  }//End While(1)    	


}//ThreadProc_NoteAcptrStart() end here

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(COIN_ACCEPTOR)

//Coin Acceptor Thread
void* ThreadProc_CoinAcptrStart(void *ptr)
{ 

  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //wait for signal
  pthread_mutex_lock(&g_CAULEnableThreadmutex);

  pthread_cond_wait( &g_CAULEnableThreadCond , &g_CAULEnableThreadmutex );

  pthread_mutex_unlock(&g_CAULEnableThreadmutex);
 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  pthread_mutex_lock(&coinAcptrStartFlagmutex);

  if( OFF == g_coinAcptrStartFlag )
  {

       pthread_mutex_unlock(&coinAcptrStartFlagmutex);

       writeFileLog("[ThreadProc_CoinAcptrStart()] Thread Exit."); 

       pthread_exit(0);

  }
  else
  {

        pthread_mutex_unlock(&coinAcptrStartFlagmutex);

        writeFileLog("[ThreadProc_NoteAcptrStart()] Thread Running."); 

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
  int ret=0,i=0,coinValue[5]={0},coinStatus=0x00;

  g_totalCoinAmountAccepted = 0;

  //Malay Add 30 Jan 2013  
  int currentfare=0;
  int CoinEvent=0,PrevCoinEvent=0;
  
  //malay add 6 aprl 2013
  char log[200];

  memset(log,'\0',200);
  
  int CACurrentfare=0;
  

  while(1)
  {
   
	pthread_mutex_lock(&coinAcptrStartFlagmutex);

	if( ON == g_coinAcptrStartFlag  )
	{

		   pthread_mutex_unlock(&coinAcptrStartFlagmutex);

                   for(i = 0;i<5;i++)
                   {
		       coinValue[i] = 0;
                   }
		   
                   coinStatus = 0x00;

                   g_CoinEventFlag=false;

		   GetCoinDetailInEscrow(coinValue,&coinStatus);

                   if(  COINACPTR_RES_ESCROW == coinStatus  )
		   {
			 
                             pthread_mutex_lock(&mut);

			     for(i=0;i<5;i++)
			     { 

					 //store coin information
                                         if(coinValue[i] > 0)
					 {
					   
				                    g_totalCoinAmountAccepted += coinValue[i] ;

						    g_totalAmountAccepted     += coinValue[i] ;
				                    
						    g_acpted_CurrencyDetail[g_denomIndex][0] = COIN; 

						    g_acpted_CurrencyDetail[g_denomIndex][1] = coinValue[i];   
 
						    g_denomIndex+=1;
                                                 
                                                    g_ttlNmbrOfCoin +=1; 

                                                    IncrementCoinsCounter(coinValue[i]);
                                                                                                     
                                                    //#ifdef ATVM_DEBUG

                                                    memset(log,'\0',200);

                                                    sprintf(log,"[ThreadProc_CoinAcptrStart()] Current Coin Accepted: Rs. %d Current Accepted Ammount: Rs. %d .",coinValue[i],g_totalAmountAccepted); 
      
						    writeFileLog(log);

                                                    
                                                    //#endif

                                                    g_CoinEventFlag=true;

                                                   //clear array
                                                   memset(log,'\0',200);

                                                   sprintf(log,"[ThreadProc_CoinAcptrStart()] Total Coins Number : %d .", g_ttlNmbrOfCoin);
			                           writeFileLog(log);

                                                   //clear array
				                   memset(log,'\0',200);

			       
					  }

					  
			     }  

                             
		           
			    //Store Event
                            if( true == g_CoinEventFlag)
                            {
		                  PrevCoinEvent=CoinEvent;
		                  CoinEvent=CoinEvent+1;
		            }

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
                               writeFileLog("[ThreadProc_CoinAcptrStart()] Going to disable specific coins and notes.");
                               
                               //clear log array
                               memset(log,'\0',200);	       

                               sprintf(log,"[ThreadProc_CoinAcptrStart()]  Fare : Rs. %d  Current Accepted Fare: Rs. %d .", g_Fare,g_totalAmountAccepted);

			       writeFileLog(log);
                               #endif

                               pthread_mutex_lock(&mut);

                               //Calculate current fare to be collect
                               CACurrentfare= g_Fare - g_totalAmountAccepted;

                               pthread_mutex_unlock(&mut);

                               //#ifdef ATVM_DEBUG

                               memset(log,'\0',200);	       

                               sprintf(log,"[ThreadProc_CoinAcptrStart()] Fare to be accept: Rs. %d .",CACurrentfare );

			       writeFileLog(log);

                               //#endif

                               pthread_mutex_lock(&mut);

                               //Runtime all coin and note inhibit
			       if(  (( g_maxNoOfCash > 0 ) && (g_ttlNmbrOfNote >= g_maxNoOfCash) )               || 
		                 (   ( g_maxNoOfCoin > 0 ) && (g_ttlNmbrOfCoin >= g_maxNoOfCoin) )               ||
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
                                            writeFileLog("[ThreadProc_CoinAcptrStart()] Inhibit from Coin Event.");
                                            //#endif

                                    }

                               }
                               else
                               {

                                       pthread_mutex_unlock(&mut);

                                       #if defined(COIN_ACCEPTOR)
                                       if( 0 ==  g_CoinFullDisable )
                                       {
                                             //Runtime coin inhibit
                                             SetSpecificCoinInhibitStatus(CACurrentfare);
                                       }
                                       #endif

                                       #if defined(NOTE_ACCEPTOR)
                                       if( 0 ==  g_NoteFullDisable )
                                       {
				             //Runtime Note inhibit
				             SetSpecificNoteInhibitStatus(CACurrentfare);
                                       }
                                       #endif

			       }      
 
			       CoinEvent=0;

			       PrevCoinEvent=0;

                               #ifdef ATVM_DEBUG
			       writeFileLog("[ThreadProc_CoinAcptrStart()] Successfully disable specific coins and notes.");
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
                                writeFileLog("[ThreadProc_CoinAcptrStart()] Inhibit from External Coin Event.");     
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
              writeFileLog("[ThreadProc_CoinAcptrStart()] Thread exit.");
              pthread_exit(0);

          }


  } // End while(1)



}//end

#endif



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UTILITY FUNCTION


/*

void  StopAcceptingState()
{



                                       writeFileLog("[StopAcceptingState()] Start.");

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
                                       writeFileLog("[StopAcceptingState()] Before Signal Note Acceptor Thread Stop.");
                                       CloseNoteAcceptorPollThread();
                                       writeFileLog("[StopAcceptingState()] After Signal Note Acceptor Thread Stop.");
                                       delay_mSec(NOTE_ACPTR_END_DELAY); 
                             
                                       #endif

                                       //Safety
                                       DisableAllAcceptor(); 

                                       writeFileLog("[StopAcceptingState()] End.");
                
                                       return;





}//StopAcceptingState() end here


*/


/////////////////////////////////////////////////////////////////////////////////////////////////////


void StopNAUpperCreditPollThread()
{


       //off noteacptr credit polling flag
       
       pthread_mutex_lock(&noteAcptrStartFlagmutex);
       g_noteAcptrStartFlag = OFF; 
       pthread_mutex_unlock(&noteAcptrStartFlagmutex);
       





}//end


void StopCAUpperCreditPollThread()
{

      //off coinacptr credit polling flag
      #ifdef COIN_ACCEPTOR
      pthread_mutex_lock(&coinAcptrStartFlagmutex);
      g_coinAcptrStartFlag = OFF; 
      pthread_mutex_unlock(&coinAcptrStartFlagmutex);
      #endif

}//end


void StopCALowerCreditPollThread()
{

      #ifdef COIN_ACCEPTOR
      //close coin acceptor low level thread 
      CloseCoinCreditPollingThread(); 
      #endif
      
 
}//end


void StopNALowerCreditPollThread()
{

      #if defined(NOTE_ACCEPTOR) 
      //close note acptr low level thread
      CloseNoteAcceptorPollThread();
      delay_mSec(NOTE_ACPTR_THREAD_END_DELAY);
      #endif 
     
 
}//end

///////////////////////////////////////////////////////////////////////////////////////////////////////////



void  StopAcceptingState()
{



                                       writeFileLog("[StopAcceptingState()] Start.");

                                       int logsize=1000;
                             
                                       char log[logsize];

                                       memset(log,'\0',logsize);
                                  
                                       struct timespec begints, endts,diffts;

                                       #if defined(NOTE_ACCEPTOR) 

                                     
		                               

		                               clock_gettime(CLOCK_MONOTONIC, &begints);
	 
                                               //wait for note acceptor disable signal status
		                               while(1)
		                               {
		                                    

				                           
				                            if( true == GetNADisableAcceptanceSignal() )
				                            {
		                                                clock_gettime(CLOCK_MONOTONIC, &endts);

					                        diffts.tv_sec = endts.tv_sec - begints.tv_sec;

		                                                memset(log,'\0',100);

		                                                sprintf(log,"[StopAcceptingState() Note Disable Status] Get Disable acceptance signal get in  %d second.",(int)diffts.tv_sec);

		                                                writeFileLog(log);

		                                                break;
				                            }
				                            

				                           
		                                            //Timeout exit [abnormal exit]
					                    clock_gettime(CLOCK_MONOTONIC, &endts);
					                    diffts.tv_sec = endts.tv_sec - begints.tv_sec;
		                                            if(diffts.tv_sec >
                                                               DELAY_FOR_NOTE_DENOM_DISABLE_COMPLETE )
		                                            {
		                                                
		                                                memset(log,'\0',100);

		                                                sprintf(log,"[StopAcceptingState() Note Disable Status Timeout] unable to Get Disable acceptance signal get in  %d second.",(int)diffts.tv_sec);

		                                                writeFileLog(log);

		                                                break;
		                                              
		                                            }
					              
		                                            


		                               }//while loop
                                              
                                               //delay between two checking
                                               delay_mSec(NOTE_ACPTR_STATUS_CHECK_DELAY);
                                               
                                               //wait for note acceptor note acceptance complete signal
                                               while(1)
		                               {
		                                    

				                           
				                            if( false == GetNANoteAcceptanceStatus() )
				                            {
		                                                clock_gettime(CLOCK_MONOTONIC, &endts);

					                        diffts.tv_sec = endts.tv_sec - begints.tv_sec;

		                                                memset(log,'\0',logsize);

		                                                sprintf(log,"[StopAcceptingState() Note Acceptance Status] Get Note acceptance sequence finish signal get in  %d second.",(int)diffts.tv_sec);

		                                                writeFileLog(log);

		                                                break;
				                            }
				                            

				                           
		                                            //Timeout exit [abnormal exit]
					                    clock_gettime(CLOCK_MONOTONIC, &endts);
					                    diffts.tv_sec = endts.tv_sec - begints.tv_sec;
		                                            if(diffts.tv_sec > DELAY_FOR_NOTE_ACCEPTANCE_COMPLETE)
		                                            {
		                                                
		                                                memset(log,'\0',logsize);

		                                                sprintf(log,"[StopAcceptingState() Note Acceptance Status Timeout] unable to Get Note acceptance sequence finish signal get in  %d second.",(int)diffts.tv_sec);

		                                                writeFileLog(log);

		                                                break;
		                                              
		                                            }
					              
		                                            


		                               }//while loop

                                       
                                      
                                       #endif
                                       ////////////////////////////////////////////////////////////////////
                                        
                                        #ifdef COIN_ACCEPTOR

                                        clock_gettime(CLOCK_MONOTONIC, &begints);
	 
                                        //wait for note acceptor disable signal status
		                        while(1)
		                        {
		                                    

				                           
				                            if( true == GetCADisableAcceptanceSignal() )
				                            {
		                                                clock_gettime(CLOCK_MONOTONIC, &endts);

					                        diffts.tv_sec = endts.tv_sec - begints.tv_sec;

		                                                memset(log,'\0',logsize);

		                                                sprintf(log,"[StopAcceptingState() Coin Disable Status] Get Disable acceptance signal get in  %d second.",(int)diffts.tv_sec);

		                                                writeFileLog(log);

		                                                break;
				                            }
				                            

				                           
		                                            //Timeout exit [abnormal exit]
					                    clock_gettime(CLOCK_MONOTONIC, &endts);
					                    diffts.tv_sec = endts.tv_sec - begints.tv_sec;
		                                            if(diffts.tv_sec > 
                                                            DELAY_FOR_COIN_DENOM_DISABLE_COMPLETE )
		                                            {
		                                                
		                                                memset(log,'\0',logsize);

		                                                sprintf(log,"[StopAcceptingState() Coin Disable Status Timeout] unable to Get Disable acceptance signal get in  %d second.",(int)diffts.tv_sec);

		                                                writeFileLog(log);

		                                                break;
		                                              
		                                            }
					              
		                                            


		                       }//while loop

                                       #endif
                                       

                                       #if  defined(COIN_ACCEPTOR) &&  !defined(NOTE_ACCEPTOR)
                                       //Delay for coin acceptance sequene
                                       Wait(DELAY_FOR_COIN_ACCEPTANCE_COMPLETE);
                                       #endif       
                                       

                                       ////////////////////////////////////////////////////////////////////

                                       //Now Close all thread

                                       pthread_mutex_lock(&RuntimeInhibitFlagmutex);

                                       //Stop all accepting process
                                       g_RuntimeInhibitFlag=0;

                                       pthread_mutex_unlock(&RuntimeInhibitFlagmutex);
                                       
                                       #ifdef NOTE_ACCEPTOR
                                       StopNAUpperCreditPollThread();
                                       StopNALowerCreditPollThread();
                                       Wait(1);
                                       #endif
                                      
                                       #ifdef COIN_ACCEPTOR
                                       StopCAUpperCreditPollThread();
                                       StopCALowerCreditPollThread();
                                       #endif
 
                                       /////////////////////////////////////////////////////////////////

                                       //Safety purpose disable all accptor

                                       DisableAllAcceptor(); 


                                       /////////////////////////////////////////////////////////////////

                                       writeFileLog("[StopAcceptingState()] End.");
                
                                       return;





}//StopAcceptingState() end here






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Millisecond delay
static void delay_mSec(int milisec) 
{


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

		writeFileLog(log);


        }



}//end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline int Wait(unsigned int Second)
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
		           
		                  sprintf(log,"[Wait()] Seconds Elapsed=%d",diffts.tv_sec); 

		                  writeFileLog(log);

		                  #endif

		                  if(diffts.tv_sec>=Second)
		                  {

		                     return 1;

		                  }


                 }


}//Wait() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Deactivate Note Acceptor

#if defined(NOTE_ACCEPTOR)
static inline int DeactivateNoteAcptr()
{


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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(CCTALK_DEVICE) && defined(COIN_ACCEPTOR)

//Deactivate Coin Acceptor 
static inline int DeactivateCoinAcptr()
{

       return ( CloseCoinAcptr() );

}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TRAP_GATE_DEVICE) && defined(B2B_TRAP_GATE) 

//Deactivate Trap Gate 
static inline int  DeactivateTrapGate()
{


   return ( CloseDropandSealUnit() );


}

#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void GetRecycleStatusBeforeStartAcceptfare()
{
                 
                   
                   unsigned char log[100];

                   memset(log,'\0',100);

                   writeFileLog("[GetRecycleStatusBeforeAcceptfare()] Before Going to read current recycle cassette status.");

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
                        writeFileLog("[GetRecycleStatusBeforeAcceptfare()] Read note acceptor recycle cassette status success.");
                        g_PrevCastFlag=true;
                   }
                   else
                   {
                        writeFileLog("[GetRecycleStatusBeforeAcceptfare()] Read note acceptor recycle cassette status failed.");
                        g_PrevCastFlag=false;
                   }

                   #endif

                   sprintf(log,"[CalculateCurrentBillQuantity()] Previous Recycle Quantity : %d",g_PrevCastQuanity);
                   writeFileLog(log); 

                   writeFileLog("[GetRecycleStatusBeforeAcceptfare()] After read current recycle cassette status.");
                   


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void GetRecycleStatusAfterFinishAcceptfare()
{
         
                  

                   unsigned char log[100];
                   memset(log,'\0',100);

                   writeFileLog("[GetRecycleStatusAfterFinishAcceptfare()] Before Going to read current recycle cassette status.");

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
                        writeFileLog("[GetRecycleStatusAfterFinishAcceptfare()] Read b2b recycle cassette status success.");
                        g_CurrentCastFlag=true;
                   }
                   else
                   {
                        writeFileLog("[GetRecycleStatusAfterFinishAcceptfare()] Read b2b recycle cassette status failed.");
                        g_CurrentCastFlag=false;
                   }
                   
                   #endif

                   sprintf(log,"[CalculateCurrentBillQuantity()] Current Recycle Quantity : %d",g_CurrentCastQuanity);
                   writeFileLog(log);    
       
                   writeFileLog("[GetRecycleStatusAfterFinishAcceptfare()] After read current recycle cassette status.");

                   



}//GetRecycleStatusAfterFinishAcceptfare() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline unsigned int CalculateCurrentBillQuantity()
{

           

           unsigned char log[100];
           memset(log,'\0',100);

           
	   GetRecycleStatusAfterFinishAcceptfare();

           memset(log,'\0',100);
           sprintf(log,"[CalculateCurrentBillQuantity()] Current Recycle Quantity : %d",g_CurrentCastQuanity);
           writeFileLog(log);    
       
           memset(log,'\0',100);
           sprintf(log,"[CalculateCurrentBillQuantity()] Previous Recycle Quantity : %d",g_PrevCastQuanity);
           writeFileLog(log); 

           memset(log,'\0',100);
           sprintf(log,"[CalculateCurrentBillQuantity()] Current Counted Note Number : %d",g_ttlNmbrOfNote);
           writeFileLog(log);   

       
           if( (true == g_CurrentCastFlag) &&  ( true == g_PrevCastFlag ) ) 
           {

		           if( g_CurrentCastQuanity!= (g_PrevCastQuanity+g_ttlNmbrOfNote) )
			   {
				   //writeFileLog("[CalculateCurrentBillQuantity() current read block ] Note count error happens.");
				   return ( g_CurrentCastQuanity - g_PrevCastQuanity );
			   }  
			   else
			   {
				   //writeFileLog("[CalculateCurrentBillQuantity() current read block ] No Note count error.");
				   return ( g_ttlNmbrOfNote );

			   }

         }

         else
         {
	        writeFileLog("[CalculateCurrentBillQuantity()] current read failed.");
		return (g_ttlNmbrOfNote);

         }

         

}//CalculateCurrentBillQuantity() end here


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 Return Code:
 1=Successfully all close running thread
 0=Unable to close all thread
 2=thread already closed

*/

static int KillAllRunningThread(double waittime)
{


		 
		 time_t start,end,diff;

		 int rtcode=-1;

                 unsigned char log[200];

                 memset(log,'\0',200);

		 writeFileLog("[KillAllRunningThread()] External Thread Termination in processing.....");
		 
		 //if Acpt Fare Thread in Started Condition
                 pthread_mutex_lock(&AcptFareThreadStatusmutex);

		 if(THREAD_START==g_AcptFareThreadStatus)
		 {

                           pthread_mutex_unlock(&AcptFareThreadStatusmutex);

                           writeFileLog("[KillAllRunningThread()] Before Signal Close AcceptFareThread.");

                           pthread_mutex_lock(&mut);

			   //Signal Acpt fare thread to closed or kill
			   g_ExternalKillAcptFareThread=THREAD_KILL;

                           pthread_mutex_unlock(&mut);

                           writeFileLog("[KillAllRunningThread()] After Signal Close AcceptFareThread.");

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

                                     writeFileLog(log);

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

                                                    //writeFileLog(log);

						    return 1;

						}
                                                else
                                                {
                                                    pthread_mutex_unlock(&AcptFareThreadStatusmutex);
                                       
                                                }
						

				     }

				     //if timer time is end or timer is running
				     if(diff>=waittime)
				     {

                                                pthread_mutex_lock(&AcptFareThreadStatusmutex);

						if( THREAD_CLOSE==g_AcptFareThreadStatus)
						{

                                                    pthread_mutex_unlock(&AcptFareThreadStatusmutex);

						    //memset(log,'\0',200);
                   
                                                    //sprintf(log,"[KillAllRunningThread() diff>=waittime block] All thread termination completed within %d Seconds.",diff); 

                                                    //writeFileLog(log);
                                                    
						    return 1;

						}
						else
						{ 
                                                    pthread_mutex_unlock(&AcptFareThreadStatusmutex);

						    //memset(log,'\0',200);
                   
                                                    //sprintf(log,"[KillAllRunningThread() diff>=waittime block] All thread termination failed: %d Seconds.",diff); 
                                                    //writeFileLog(log);

						    return 0;

						}

					        


				     }//timer if block end here
				      

			   }//for loop end
		   
		 }//if thread status start end here
		 else if(THREAD_CLOSE==g_AcptFareThreadStatus)
                 {

                      pthread_mutex_unlock(&AcptFareThreadStatusmutex);

		      writeFileLog("[KillAllRunningThread()] All thread already terminated.");

                      return 2;

                 }



}//KillAllRunningThread() end here


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#ifdef ASCRMAPIMAIN

/*

int main()
{
  
 
	  TestAscrm();
	  return 0;


} //End main



static void TestAscrm()
{


		   int ret=0,rtcode=0,UserAmountInput = 0,prevfare=0,row=0 ,col=0,arry[40][2]={0},acptfarertcode=0,fare1=0,transtime=60,maxcash=0,maxcoin=0;
                   double diff=0;
                   time_t start=0,end=0;
                   unsigned char log[100];
                   memset(log,'\0',100);

                   //writeFileLog("[TestAscrm()] Enter Transtime: ");
		   //scanf("%d",&transtime);

                   transtime = GetTransTimeout();

		   ret =  activateDevice(0,1);
		   memset(log,'\0',100);
                   sprintf(log,"[TestAscrm()] ACTIVATE DEV STATUS: %d",ret ); 
                   writeFileLog(log);

		   if(-1==ret)
		   {
		       
                        memset(log,'\0',100);

                        sprintf(log,"[TestAscrm()] ACTIVATE DEV STATUS: %d thats why system cannot run now ",ret ); 
                        writeFileLog(log);

                        return;

		  }
                  else if( 99 == ret )
                  {

                       printf("\n[TestAscrm()] Press 1 for accept escrow notes ,\n 2 for return escrow notes and press any other key to go for system default choice which is accept escrow notes");

                       char ch=0;

		       ch=getchar();

		       switch(ch)
		       {
                              case '1': defaultCommit(); 
                                        break;

                              case '2': defaultCancel();
                                        break;

                              default : defaultCommit(); 
                                        break;
			      
		       }

                       
                       

                  }
		  
                  for( ;; )
		  {
			

                           acptfarertcode=0;
		           prevfare=0;
			   
			   writeFileLog("\n\n\n\n\n\n[TestAscrm()] Press any key to continue or Press 1 for exit : ");
			   char ch=0;
			   ch=getchar();
			   if( '1' == ch)
			   {
			      writeFileLog("[TestAscrm()]) Program now exit by user. ");
			      return;
			   }
                           

                           
                           
                           

                           ret = deviceStatus(0);
                           
                           memset(log,'\0',100);

                           sprintf(log,"[TestAscrm()] Before And Device Status Anding Code : %d",ret); 
                           writeFileLog(log);

                           ret = ret&0x02;
                           
                           memset(log,'\0',100);
                           sprintf(log,"[TestAscrm()] After And Device Status Code : %d",ret); 
                           writeFileLog(log);

                           if( 0 ==  ret )
		           {
		                    writeFileLog("[TestAscrm()] Press any key to continue: ");
				    char ch=0;
				    ch=getchar();
                                    continue;
		           }

                           memset(log,'\0',100);
                           sprintf(log,"[TestAscrm()] Both Device Status Code: %d",ret); 
                           writeFileLog(log);

                           
			   fare1=0;
			   writeFileLog("[TestAscrm()] ENTER FARE: ");
			   scanf("%d",&fare1);


                           maxcash=0;
			   writeFileLog("[TestAscrm()] ENTER MAX CASH: ");
			   scanf("%d",&maxcash);

                           maxcoin=0;
			
	                  acptfarertcode=acceptFare(fare1,maxcash,maxcoin,GetTransTimeout());
			  if( false == acptfarertcode)
			  {
			      writeFileLog("[TestAscrm()]) acceptFare() return false again try!! ");
			      continue;
			  }  
                          else if( false == acptfarertcode)
			  {
			      writeFileLog("[TestAscrm()]) acceptFare() return true.");
			      
			  }  
			  rtcode = 0;
			  UserAmountInput = 0;
			  row=0;col=0;
			      
			 
			  time(&start);

			  while(1)
			  {   

			   
				    time(&end);

				    diff=end-start;

				    if(diff>=transtime)
				    {

					       writeFileLog("[TestAscrm()] >>  Transtimeout by system");  
					       rtcode = getDenomination(arry);
					       memset(log,'\0',100);
                                               sprintf(log,"[TestAscrm()]getDenomination Return code=%d",rtcode); 
                                               writeFileLog(log);
                                               UserAmountInput=0; 
					       for( row=0;row<40;row++)
					       {
						  if(arry[row][1]>0) 
						  UserAmountInput+=arry[row][1]; 
					       }
                                               memset(log,'\0',100);
                                               sprintf(log,"[TestAscrm()] >> User Given Currency:%d",UserAmountInput); 
                                               writeFileLog(log);
					       if(fare1==UserAmountInput)
                                               {
					            writeFileLog("[TestAscrm()] >>  System now commit your transaction");
                                                    
					            //commitTran();
                                                    commitTran_Ver2();
					       }
					       else
                                               {
					          writeFileLog("[TestAscrm()] >>  System now cancel your session");

					          //cancelTran();
                                                  cancelTran_Ver2();
					       }

					       break;

				    }

				    rtcode = getDenomination(arry);
				    
                                    //Accepting State return code 1
				    if(1==rtcode)
				    {
                                               
					      UserAmountInput=0;
					      for( row=0;row<40;row++)
					      {
						 if(arry[row][1]>0) 
						 UserAmountInput+=arry[row][1]; 
					      }
					      //system("clear");
					      if(UserAmountInput>prevfare)
					      {
		                                      memset(log,'\0',100);
		                                      sprintf(log,"[TestAscrm()] >> User Given Fare = %d",UserAmountInput); 
		                                      writeFileLog(log);
                                              }

					      prevfare=UserAmountInput;

				    }

				    //Exact fare accepted return code 2
				    if(2==rtcode)
				    {
                                               writeFileLog("\n =========================================================================");            
                                               memset(log,'\0',100);
		                               sprintf(log,"[TestAscrm()] getDenomination return code %d",rtcode); 
		                               writeFileLog(log);
                                               writeFileLog("\n =========================================================================");      
					       writeFileLog("[TestAscrm()] >> ACCEPTED FARE GIVEN");  
					       //commitTran();
                                               commitTran_Ver2();
					       break;
				    }

				    //Transactions Timeout. return code 0
				    if (0==rtcode)
				    { 
                                               writeFileLog("\n =========================================================================");      
                                               memset(log,'\0',100);
		                               sprintf(log,"[TestAscrm()] getDenomination return code %d",rtcode); 
		                               writeFileLog(log);
                                               writeFileLog("\n =========================================================================");      

					       writeFileLog("[TestAscrm()] >>  Transtimeout by user");  
					       //cancelTran();
                                               cancelTran_Ver2();
					       break;             
				    }
				    // More qty return code 3
				    if( 3==rtcode )
				    { 

                                               writeFileLog("\n =========================================================================");      
                                               memset(log,'\0',100);

		                               sprintf(log,"[TestAscrm()] getDenomination return code = %d",rtcode); 

		                               writeFileLog(log);

                                               writeFileLog("\n =========================================================================");      

					       writeFileLog("[TestAscrm()] >> More note and coin inserted its time for cancel by system");            
                                               //cancelTran();
                                                

                                               cancelTran_Ver2();

					       break;

				    } 
				   
				    //if(diff> prevdiff)
				    //{

					       //prevdiff=diff;
					       //system("clear");
					       //printf("\n[TestAscrm()] >> %ld Second Elapsed",(end-start)); 

				    //}
				       

				   
			  } // End: while loop


			 
		   } // End:  for loop




}//TestAscrm() end


*/


#endif

