#include "SmartCardApi.h"
#include "smartcardreturn.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int g_ComHandle;
static pthread_mutex_t g_GetSmartCardLastErrormutex = PTHREAD_MUTEX_INITIALIZER;
static int g_GetSmartCardLastError;
static unsigned char g_getStatus[15] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
static unsigned char g_DisableCardAcceptance[9]={0x00,MTK_DEVICE_ADDRESS,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
static unsigned char g_EnableCardAcceptance[9]={0x00,MTK_DEVICE_ADDRESS,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static unsigned char g_moveToErrorBin[9]={0x00,MTK_DEVICE_ADDRESS,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static unsigned char g_resetData_Retain_Channel[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x30,0x33,0x03,0x00}; 
static unsigned char g_resetData_Capture_Card_Bin[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x30,0x31,0x03,0x00}; 
static unsigned char g_resetData_CardHolding_Position[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x30,0x30,0x03,0x00}; 
static char    g_SCRDDeviceid[MAX_STR_SIZE_OF_LOG];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UNKNOWN_ERROR 99
#define MISCELLANEOUS_ERROR 101
#define CARD_JAM_ERROR 102
#define SENSOR_ERROR 103
#define CARD_SIZE_ERROR 104
#define FORCED_INSERTION_ERROR 105
#define MOTOR_ERROR 106
#define EMPTY_STACKER 107
#define FULL_STACKER 108

int g_MaxDelay = 200;

static void SetLastError( unsigned int e1, unsigned int e2 ) {
	int errorcode=0;
    char log[LOG_ARRAY_SIZE];
	
	switch(e1)
	{
			case 0x30:
			case 0x34:
			case 0x36:
			case 0x42:
				errorcode = MISCELLANEOUS_ERROR;  //Miscellaneous error 
				break;
				
			case 0x31:
				if(e2 == 0x30)
					errorcode = CARD_JAM_ERROR;  //Card Jam
				else if(e2 == 0x32)
					errorcode = SENSOR_ERROR;  //Sensor error
				else if((e2 == 0x33) || (e2 == 0x34))
					errorcode = CARD_SIZE_ERROR;  //Card size mismatch error
				else if(e2 == 0x36)
					errorcode = FORCED_INSERTION_ERROR; //Forced insertion
				else
					errorcode = UNKNOWN_ERROR;
				break;
			
			case 0x35:
			    if(e2 == 0x30)
					errorcode = MISCELLANEOUS_ERROR; //Miscellaneous error
				else if(e2 == 0x31)
					errorcode = MOTOR_ERROR; //Motor error
				break;
			
			case 0x41:
			    if(e2 == 0x30)
					errorcode = EMPTY_STACKER; //Empty stacker
				else
					errorcode = FULL_STACKER;  //Full stacker
				break;
			
			default:
				errorcode = 99;  //Unknown
				break;
	}
/*	
		
		
		if(e1 == 0x30)
		{//Command error
				
		
		}
	
        char log[LOG_ARRAY_SIZE];

        int e1_code=0,
            e2_code=0,
            errorcode=0;

        if( ( ( e1 >=0x30 ) || ( e1 <=0x39 ) )  &&
            ( ( e2 >=0x30 ) || ( e2 <=0x39 ) )
          )
        {

			switch(e1)
			{

			      case 0x30:e1_code=0;
				        break;
			      case 0x31:e1_code=1;
				        break;
			      case 0x32:e1_code=2;
				        break;
			      case 0x33:e1_code=3;
				        break;
			      case 0x34:e1_code=4;
				        break;
			      case 0x35:e1_code=5;
				        break;
			      case 0x36:e1_code=6;
				        break;
			      case 0x37:e1_code=7;
				        break;
			      case 0x38:e1_code=8;
				        break;
			      case 0x39:e1_code=9;
				        break;

			};

			switch(e2)
			{

			      case 0x30:e2_code=0;
				        break;
			      case 0x31:e2_code=1;
				        break;
			      case 0x32:e2_code=2;
				        break;
			      case 0x33:e2_code=3;
				        break;
			      case 0x34:e2_code=4;
				        break;
			      case 0x35:e2_code=5;
				        break;
			      case 0x36:e2_code=6;
				        break;
			      case 0x37:e2_code=7;
				        break;
			      case 0x38:e2_code=8;
				        break;
			      case 0x39:e2_code=9;
				        break;

			};

			errorcode = (e1_code*10)+e2_code;

			pthread_mutex_lock(&g_GetSmartCardLastErrormutex);

			g_GetSmartCardLastError= errorcode;

			pthread_mutex_unlock(&g_GetSmartCardLastErrormutex); 

		        memset(log,'\0',LOG_ARRAY_SIZE);

		        sprintf(log,"[SetLastError() Exit] Error Code : %d.",errorcode);

		        //writeFileLog(log);

		        return;


        }//if( ( ( e1 >=0x30 ) || ( e1 <=0x39 ) ) &&
         //    ( ( e2 >=0x30 ) || ( e2 <=0x39 ) )
         // )
        else if( ( 0x12 == e1 ) && 
                 ( e2 >=0x30 ) || ( e2 <=0x39 ) 
               ) // e1:A e2: 0-9
        {

             
		      if( ( 0x12 == e1 ) && ( 0x30 == e2 ) ) 
		      {
		           errorcode = 0xA0;
	 
		      }  
		      else if( ( 0x12 == e1 ) && ( 0x31 == e2 ) ) 
		      {
		           errorcode = 0xA1;

		      }  
		      else if( ( 0x12 == e1 ) && ( 0x32 == e2 ) ) 
		      {
		           errorcode = 0xA2;

		      }  
		      else if( ( 0x12 == e1 ) && ( 0x33 == e2 ) ) 
		      {
		          errorcode = 0xA3;

		      }      
		      else if( ( 0x12 == e1 ) && ( 0x34 == e2 ) ) 
		      {
		         errorcode =0xA4;

		      }  
		      else if( ( 0x12 == e1 ) && ( 0x35 == e2 ) ) 
		      {
		         errorcode = 0xA5;

		      } 
		      else if( ( 0x12 == e1 ) && ( 0x36 == e2 ) ) 
		      {
		         errorcode = 0xA6;

		      }  
		      else if( ( 0x12 == e1 ) && ( 0x37 == e2 ) ) 
		      {
		         errorcode =0xA7;

		      }  
		      else if( ( 0x12 == e1 ) && ( 0x38 == e2 ) ) 
		      {
		         errorcode = 0xA8;

		      }   
		      else if( ( 0x12 == e1 ) && ( 0x39 == e2 ) ) 
		      {
		         errorcode =0xA9;

		      } 
 

                      pthread_mutex_lock(&g_GetSmartCardLastErrormutex);

		      g_GetSmartCardLastError= errorcode;

		      pthread_mutex_unlock(&g_GetSmartCardLastErrormutex); 

		      memset(log,'\0',LOG_ARRAY_SIZE);

		      sprintf(log,"[SetLastError() Exit] Error Code : %c.",errorcode);

		      //writeFileLog(log);

                      return;

        }
        else if( ( 0x13 == e1 ) && 
                 ( e2 >=0x30 ) || ( e2 <=0x39 ) 
               ) // e1:B e2: 0-9
        {

               
		      if( ( 0x13 == e1 ) && ( 0x30 == e2 ) ) 
		      {
		           errorcode =0xB0;
	 
		      }  
		      else if( ( 0x13 == e1 ) && ( 0x31 == e2 ) ) 
		      {
		           errorcode =0xB1;

		      }  
		      else if( ( 0x13 == e1 ) && ( 0x32 == e2 ) ) 
		      {
		           errorcode =0xB2;

		      }  
		      else if( ( 0x13 == e1 ) && ( 0x33 == e2 ) ) 
		      {
		          errorcode =0xB3;

		      }      
		      else if( ( 0x13 == e1 ) && ( 0x34 == e2 ) ) 
		      {
		         errorcode =0xB4;

		      }  
		      else if( ( 0x13 == e1 ) && ( 0x35 == e2 ) ) 
		      {
		         errorcode =0xB5;

		      } 
		      else if( ( 0x13 == e1 ) && ( 0x36 == e2 ) ) 
		      {
		         errorcode =0xB6;

		      }  
		      else if( ( 0x13 == e1 ) && ( 0x37 == e2 ) ) 
		      {
		         errorcode =0xB7;

		      }  
		      else if( ( 0x13 == e1 ) && ( 0x38 == e2 ) ) 
		      {
		         errorcode =0xB8;

		      }   
		      else if( ( 0x13 == e1 ) && ( 0x39 == e2 ) ) 
		      {
		         errorcode =0xB9;

		      } 
 

                      pthread_mutex_lock(&g_GetSmartCardLastErrormutex);

		      g_GetSmartCardLastError= errorcode;

		      pthread_mutex_unlock(&g_GetSmartCardLastErrormutex); 

		      memset(log,'\0',LOG_ARRAY_SIZE);

		      sprintf(log,"[SetLastError() Exit] Error Code : %c.",errorcode);

		      //writeFileLog(log);

                      return;



        }
 */       
		  pthread_mutex_lock(&g_GetSmartCardLastErrormutex);

		  g_GetSmartCardLastError= errorcode;

		  pthread_mutex_unlock(&g_GetSmartCardLastErrormutex); 

		  memset(log,'\0',LOG_ARRAY_SIZE);

		  sprintf(log,"[SetLastError() Exit] Error Code : %d.",errorcode);

		  AVRM_SCRD_writeFileLog(log,"DEBUG");
       
}//void SetLastError(unsigned int e1, unsigned int e2) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetSmartCardLastError( char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        int rtcode=0;

        char log[LOG_ARRAY_SIZE];

        pthread_mutex_lock(&g_GetSmartCardLastErrormutex);

        rtcode = g_GetSmartCardLastError;
 //       g_GetSmartCardLastError=-1;

        pthread_mutex_unlock(&g_GetSmartCardLastErrormutex);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[GetSmartCardLastError() Exit] Error Code : %d.",rtcode);

        AVRM_SCRD_writeFileLog(log,"DEBUG");

        return rtcode;
	

}//int GetSmartCardLastError() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ResetSmartCardLastError() {

        
        pthread_mutex_lock(&g_GetSmartCardLastErrormutex);

        g_GetSmartCardLastError=-1;

        pthread_mutex_unlock(&g_GetSmartCardLastErrormutex);


}//void ResetSmartCardLastError() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void SmartCardSetupLog(char *LogdllPathstr,char *LogFileName,char *deviceid,int fnLogfileMode ) {


               //++printf("\n[SmartCardSetupLog()] Entry");

               if( NULL == LogdllPathstr)
               {
                   printf("\n[SmartCardSetupLog()] Error NULL Log dll path");
                   return;

               }

               if( NULL == LogFileName)
               {
                   printf("\n[SmartCardSetupLog()] Error NULL Log file name");
                   return;

               }

               if( NULL == deviceid)
               {
                   printf("\n[SmartCardSetupLog()] Error NULL device id name");
                   return;

               }

               if(strlen (LogdllPathstr) <=0 )
               {

                  printf("\n[SmartCardSetupLog()] Error Empty Log dll path");
                  return;

               }

               if(strlen (LogFileName) <=0 )
               {
                  printf("\n[SmartCardSetupLog()] Error Empty Log file name");
                  return; 
                

               }

               if(strlen (deviceid) <=0 )
               {
                   printf("\n[SmartCardSetupLog()] Error Empty device name");
                   return; 


               }

               
               //g_Ascrm_writeFileLog=NULL;

	           //g_lib_handle=NULL;

	           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	       
	           //Step 1:Log dll Path
               //printf("\n[SmartCardSetupLog()] Log dll Path : %s", LogdllPathstr);
               SetSCRDSoFilePath((unsigned char*) LogdllPathstr);

	           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
               //Step 2:Log File Name
               //printf("\n[SmartCardSetupLog()] Log File Name : %s", LogFileName);
               SetSCRDGeneralFileLogPath( (unsigned char*) LogFileName );
               
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               
               //Step 3: Set LogLevel Mode
               SetSCRDSoLogFileMode((int)fnLogfileMode );
               //printf("\n[SmartCardSetupLog()] Log File Mode : %d", (int)fnLogfileMode);
               
               //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

               //Step 4: Get Device id
               //printf("\n[SmartCardSetupLog()] Log dll Path : %s", deviceid);
               SetSCRDDeviceID(deviceid);
               memset(g_SCRDDeviceid,'\0',MAX_STR_SIZE_OF_LOG);
               strcpy( g_SCRDDeviceid ,deviceid  );

               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
               ////writeFileLog("[SmartCardSetupLog()] I am Write by log");

               //printf("\n[SmartCardSetupLog()] Exit");

               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////



}//void SmartCardSetupLog() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CardBlockStatusRequest ( char *LogdllPathstr,char *LogFileName,char *deviceid,int fnLogfileMode) {


        
        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
   
        char log[LOG_ARRAY_SIZE];

        char RecvPackt[100];

        char command[100];

        memset(log,'\0',LOG_ARRAY_SIZE);
 
        memset(command,'\0',100);

        //Device status request command
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMT
        command[5]  = 0x31; //CM
        command[6]  = 0x30; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        //Reply Byte:
        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //Data   = 12 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 24 Byte
        
        int totalByteToRecv=12; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[CardBlockStatusRequest()] Command Transmit success");
                    AVRM_writeFileLog("[CardBlockStatusRequest()] Command Transmit success",DEBUG);

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         ////writeFileLog("[CardBlockStatusRequest()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[CardBlockStatusRequest()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[CardBlockStatusRequest()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[CardBlockStatusRequest()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] )  &&  //CMH
		                    ( 0x31 == RecvPackt[5] )  &&  //CM
		                    ( 0x30 == RecvPackt[6] )      //PM
		                  )
		                {
                                       ////writeFileLog("[CardBlockStatusRequest() Exit] Command Success ");
                                       AVRM_writeFileLog("[CardBlockStatusRequest() Exit] Command Success ",DEBUG);

				       //Check st0 byte
                                       if(RecvPackt[7] == 0x30)  //No card in channel
						return 5; //No card in channel
                                       else if(RecvPackt[7] == 0x31)  //One card at gate
						return 6; //One card at gate
				       else if(RecvPackt[7] == 0x32)  //One card at RF/IC
						return 7; //One card at RF/IC
                                  
				}// if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                //      ( 0x31 == RecvPackt[5] ) &&  //CM
		                //      ( 0x31 == RecvPackt[6] )     //PM
		                //    ) end
								//Soomit-29082016: You are possibly missing the other error conditions here [Solved]
                                else if( 0x4e == RecvPackt[4] ) 
                                 
                                {
                                      //Log e1 and e2 error code
                                      memset(log,'\0',LOG_ARRAY_SIZE);

                                      sprintf(log,"[CardBlockStatusRequest() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                      ////writeFileLog(log);
                                      
                                      AVRM_writeFileLog(log,DEBUG);

                                      SetLastError(RecvPackt[7],RecvPackt[8]);
                                      if(( 0x31 == RecvPackt[7] ) && 
                                         ( 0x36 == RecvPackt[8] ) )
                                      {
                                         ////writeFileLog("[CardBlockStatusRequest() Exit] Forced Insert Found");
                                         AVRM_writeFileLog("[CardBlockStatusRequest() Exit] Forced Insert Found",INFO);
		                         return 1; //Forced insert 
                                      }  
                                      else if(( 0x31 == RecvPackt[7] ) && 
                                              ( 0x30 == RecvPackt[8] ) )
                                      {
                                         ////writeFileLog("[CardBlockStatusRequest() Exit] Card Jam Found");
                                         AVRM_writeFileLog("[CardBlockStatusRequest() Exit] Card Jam Found",INFO);
		                         return 2; //card jammed
                                      } 
                                      else
                                      {
                                         ////writeFileLog("[CardBlockStatusRequest() Exit] Other Error Found");
                                         AVRM_writeFileLog("[CardBlockStatusRequest() Exit] Other Error Found",INFO);
                                         return 3; //other error 
                                        
                                      }   
                                      
                                }

		         }
		         else //failure case
		         {
                            ////writeFileLog("[CardBlockStatusRequest() Exit] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[CardBlockStatusRequest() Exit] Reply Bytes receieve from MUTEK is failed ",INFO);
                            return 4; //Communication error
/*

		            if( totalByteToRecv > 0 )
		            {

		                 if(  ( 0x4e == RecvPackt[4] ) && 
                                      ( 0x31 == RecvPackt[7] ) && 
                                      ( 0x36 == RecvPackt[8] ) )
		                 {
                                     //writeFileLog("[CardBlockStatusRequest() Exit] Card Block Status Found");
                                     return 1;
		                 
		                 }//if(( 0x4e == RecvPackt[4] ) && 
                                 //    ( 0x4e == RecvPackt[4] )   && 
                                 //    ( 0x4e == RecvPackt[4] ) )
                                 else
                                 {
                                      //writeFileLog("[CardBlockStatusRequest() Exit] No Card Block Status Found");
                                      return 0;

                                 }
		                     
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                //writeFileLog("[CardBlockStatusRequest() Exit] reply byte receieve from mutek");
                                return 0;
		                
		            }
*/
		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[CardBlockStatusRequest() Exit] fail receieve ack byte from mutek");
                         AVRM_writeFileLog("[CardBlockStatusRequest() Exit] fail receieve ack byte from mutek",INFO);
                         return 4; //Communication error
                         //return 0; 

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[CardBlockStatusRequest() Exit] Command Transmit failed");
            AVRM_writeFileLog("[CardBlockStatusRequest() Exit] Command Transmit failed",INFO);
            return 4; //Communication error
            //return 0; 
        }



}//int CardBlockStatusRequest() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetCardBlockStatus( char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
   
        char log[LOG_ARRAY_SIZE];

        char RecvPackt[100];

        char command[100];

        memset(log,'\0',LOG_ARRAY_SIZE);
 
        memset(command,'\0',100);

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x31; //CM
        command[6]  = 0x31; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        //Reply Byte:

        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //Data   = 12 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 24 Byte
        
        int totalByteToRecv=24; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[GetCardBlockStatus()] Command Transmit success");
                    AVRM_writeFileLog("[GetCardBlockStatus()] Command Transmit success",INFO);

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         ////writeFileLog("[GetCardBlockStatus()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[GetCardBlockStatus()] ACK receieve from MUTEK",INFO);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[GetCardBlockStatus()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[GetCardBlockStatus()] Reply Bytes receieve from MUTEK is success",INFO);


		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x31 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //byte 10 is Status blocked card status
                                     if( ( 0x30 == RecvPackt[10]) &&
                                         ( 0x30 == RecvPackt[11]) &&
                                         ( 0x30 == RecvPackt[12]) &&
                                         ( 0x30 == RecvPackt[13]) 
                                       )
		                     {
				          //no Block card found
                                          ////writeFileLog("[GetCardBlockStatus() Exit] No Block Card Found ");
                                          AVRM_writeFileLog("[GetCardBlockStatus() Exit] No Block Card Found ",INFO);
                                          return 0;
				     }
                                     //byte 10 is Status blocked card status
                                     else if( ( 0x31 == RecvPackt[10]) &&
                                              ( 0x31 == RecvPackt[11]) &&
                                              ( 0x31 == RecvPackt[12]) &&
                                              ( 0x31 == RecvPackt[13]) 
                                       )
		                     {
				          //Block card found
                                          ////writeFileLog("[GetCardBlockStatus() Exit] Block Card Found ");
                                          AVRM_writeFileLog("[GetCardBlockStatus() Exit] Block Card Found ",INFO);
                                          return 1;
				     }
                                     else if( ( 0x30 == RecvPackt[10]) &&
                                              ( 0x31 == RecvPackt[11]) &&
                                              ( 0x31 == RecvPackt[12]) &&
                                              ( 0x31 == RecvPackt[13]) 
                                       )
		                     {
				          //retain card found
                                          AscrmMutekInitWithCardRetain();
                                          ////writeFileLog("[GetCardBlockStatus() Exit] init with card retain found ");
                                          AVRM_writeFileLog("[GetCardBlockStatus() Exit] init with card retain found ",INFO);
                                          return 0;
				     }
                                     else if( ( 0x31 == RecvPackt[10]) &&
                                              ( 0x30 == RecvPackt[11]) &&
                                              ( 0x30 == RecvPackt[12]) &&
                                              ( 0x30 == RecvPackt[13]) 
                                       )
		                     {
				          //init with card infront found      
                                          
                                          ////writeFileLog("[GetCardBlockStatus() Exit] init with card in front found ");
                                          AVRM_writeFileLog("[GetCardBlockStatus() Exit] init with card in front found ",INFO);
                                          return 0;
				     }
                                     else if( ( 0x31 == RecvPackt[10]) &&
                                              ( 0x31 == RecvPackt[11]) &&
                                              ( 0x31 == RecvPackt[12]) &&
                                              ( 0x30 == RecvPackt[13]) 
                                       )
		                     {
                                          // DATA_EXCHANGE_ALL SCA1 Rx[24]:0xf2 0x0 0x0 0x12 0x50 0x31 0x31 0x31 0x30 0x30 0x31 0x31 0x31 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x31 0x31 0x3 0x83 
				          //init with card infront found
                                          ////writeFileLog("[GetCardBlockStatus() Exit] half insert card in front found ");
                                          AVRM_writeFileLog("[GetCardBlockStatus() Exit] half insert card in front found ",INFO);
                                          return 1;
				     }
                                     
                                     else if( ( 0x30 == RecvPackt[10]) &&
                                              ( 0x30 == RecvPackt[11]) &&
                                              ( 0x31 == RecvPackt[12]) &&
                                              ( 0x31 == RecvPackt[13]) 
                                       )
		                     {
                                          //DATA_EXCHANGE_ALL SCA1 Rx[24]:0xf2 0x0 0x0 0x12 0x50 0x31 0x31 0x31 0x30 0x30 0x30 0x30 0x31 0x31 0x31 0x30 0x30 0x30 0x30 0x30 0x31 0x31 0x3 0x83 
				          //init with card infront found
                                          ////writeFileLog("[GetCardBlockStatus() Exit] Full Forcefully insert card in the chanel found ");
                                          AVRM_writeFileLog("[GetCardBlockStatus() Exit] Full Forcefully insert card in the chanel found ",INFO);
                                          return 1;

				     }

                                     
                                    
				    
                               }// if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                //     ( 0x31 == RecvPackt[5] ) &&  //CM
		                //     ( 0x31 == RecvPackt[6] )     //PM
		                //   ) end

		         }
		         else //failure case
		         {

                            ////writeFileLog("[GetCardBlockStatus() Exit] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[GetCardBlockStatus() Exit] Reply Bytes receieve from MUTEK is failed ",INFO);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     ////writeFileLog("[GetCardBlockStatus() Exit] negative ack from mutek");
                                     AVRM_writeFileLog("[GetCardBlockStatus() Exit] negative ack from mutek",INFO);
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                ////writeFileLog("[GetCardBlockStatus() Exit] reply byte receieve from mutek");
                                AVRM_writeFileLog("[GetCardBlockStatus() Exit] reply byte receieve from mutek",INFO);
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[GetCardBlockStatus() Exit] fail receieve ack byte from mutek");
                         AVRM_writeFileLog("[GetCardBlockStatus() Exit] fail receieve ack byte from mutek",INFO);
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[GetCardBlockStatus() Exit] Command Transmit failed");
            AVRM_writeFileLog("[GetCardBlockStatus() Exit] Command Transmit failed",INFO);
            return 0;
        }


}//int GetCardBlockStatus() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int AscrmMutekInitWithCardRetain() {

        char log[LOG_ARRAY_SIZE];

        char RecvPackt[100];

        char command[100];

        memset(log,'\0',LOG_ARRAY_SIZE);
 
        memset(command,'\0',100);

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x30; //CM
        command[6]  = 0x33; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //Data   = 13 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 25 Byte
        
        int totalByteToRecv=25; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[MutekInitWithCardRetain()] Command Transmit success");
                    AVRM_writeFileLog("[MutekInitWithCardRetain()] Command Transmit success",DEBUG);

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         ////writeFileLog("[MutekInitWithCardRetain()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[MutekInitWithCardRetain()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {
                         
                                ////writeFileLog("[MutekInitWithCardRetain()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[MutekInitWithCardRetain()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x33 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     ////writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside success");
		                             AVRM_writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside success",INFO);
                                     return 1;
                                    
				    
                                }
                                else
                                {
                                     ////writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside failed");
                                     AVRM_writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside failed",INFO);
                                     return 0;

                                }

		         }
		         else //failure case
		         {

                            ////writeFileLog("[MutekInitWithCardRetain() Exit] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] Reply Bytes receieve from MUTEK is failed ",INFO);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     ////writeFileLog("[MutekInitWithCardRetain() Exit] negative ack from mutek");
                                     AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] negative ack from mutek",DEBUG);
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                ////writeFileLog("[MutekInitWithCardRetain() Exit] reply byte receieve from mutek");
                                AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] reply byte receieve from mutek",DEBUG);
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek");
                         AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek",DEBUG);
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[MutekInitWithCardRetain() Exit] Command Transmit failed");
            AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] Command Transmit failed",DEBUG);
            return 0;
        }


}//int GetCardBlockStatus() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MutekInitWithCardRetain( char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        char log[LOG_ARRAY_SIZE];

        char RecvPackt[100];

        char command[100];

        memset(log,'\0',LOG_ARRAY_SIZE);
 
        memset(command,'\0',100);

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x30; //CM
        command[6]  = 0x33; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //Data   = 13 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 25 Byte
        
        int totalByteToRecv=25; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[MutekInitWithCardRetain()] Command Transmit success");
                    AVRM_writeFileLog("[MutekInitWithCardRetain()] Command Transmit success",DEBUG);

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         ////writeFileLog("[MutekInitWithCardRetain()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[MutekInitWithCardRetain()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[MutekInitWithCardRetain()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[MutekInitWithCardRetain()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x33 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     ////writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside success");
		                              AVRM_writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside success",INFO);
                                     return 1;
                                    
				    
                                }
                                else  if( 0x4e == RecvPackt[4] )
                                {
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardRetain() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     ////writeFileLog(log); 
                                     
                                     AVRM_writeFileLog(log,INFO);

                                     ////writeFileLog("[MutekInitWithCardRetain() Exit] negative reply from mutek");
                                     AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] negative reply from mutek",INFO);
                                     return 0;
                                }
                                else
                                {
                                     ////writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside failed");
                                     AVRM_writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside failed",INFO);
                                     return 0;

                                }

		         }
		         else //failure case
		         {

                            ////writeFileLog("[MutekInitWithCardRetain() Exit] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] Reply Bytes receieve from MUTEK is failed ",INFO);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {

                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardRetain() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     ////writeFileLog(log); 
                                     
                                     AVRM_writeFileLog(log,INFO);

                                     ////writeFileLog("[MutekInitWithCardRetain() Exit] negative reply from mutek");

                                     ////writeFileLog("[MutekInitWithCardRetain() Exit] negative ack from mutek");
                                     
                                     AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] negative reply from mutek",DEBUG);
                                     
                                     AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] negative ack from mutek",DEBUG);
                                     
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                ////writeFileLog("[MutekInitWithCardRetain() Exit] reply byte receieve from mutek");
                                AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] reply byte receieve from mutek",INFO);
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek");
                         AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek",INFO);
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[MutekInitWithCardRetain() Exit] Command Transmit failed");
            AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] Command Transmit failed",INFO);
            return 0;
        }//else end


}//int GetCardBlockStatus() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MutekInitWithCardMoveInFront(char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
        
        char log[LOG_ARRAY_SIZE];

        char RecvPackt[100];

        char command[100];

        memset(log,'\0',LOG_ARRAY_SIZE);
 
        memset(command,'\0',100);

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x30; //CM
        command[6]  = 0x30; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //Data   = 13 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 25 Byte
        
        int totalByteToRecv=25; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[MutekInitWithCardMoveInFront()] Command Transmit success");
                    AVRM_writeFileLog("[MutekInitWithCardMoveInFront()] Command Transmit success",DEBUG);

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         ////writeFileLog("[MutekInitWithCardMoveInFront()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[MutekInitWithCardMoveInFront()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[MutekInitWithCardMoveInFront()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[MutekInitWithCardMoveInFront()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     ////writeFileLog("[MutekInitWithCardMoveInFront()] Init with card move to front success");
		                             AVRM_writeFileLog("[MutekInitWithCardMoveInFront()] Init with card move to front success",INFO);
                                     return 1;
                                    
				    
                                }
                                else if(0x4e == RecvPackt[4] ) 
                                {
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardMoveInFront() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     ////writeFileLog(log); 
                                     
                                     AVRM_writeFileLog(log,DEBUG);

                                     ////writeFileLog("[MutekInitWithCardMoveInFront() Exit] negative ack from mutek");
                                     
                                     AVRM_writeFileLog("[MutekInitWithCardMoveInFront() Exit] negative ack from mutek",DEBUG);
                                     
                                     return 0;

                                }
                                else
                                {
                                     ////writeFileLog("[MutekInitWithCardMoveInFront()] Init with card move to front failed");
                                     AVRM_writeFileLog("[MutekInitWithCardMoveInFront()] Init with card move to front failed",INFO);
                                     return 0;

                                }//else end

		         }
		         else //failure case
		         {

                            ////writeFileLog("[MutekInitWithCardMoveInFront() Exit] Reply Bytes receieve from MUTEK");
                            AVRM_writeFileLog("[MutekInitWithCardMoveInFront() Exit] Reply Bytes receieve from MUTEK",DEBUG);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {

                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardMoveInFront() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     ////writeFileLog(log);
                                     
                                     AVRM_writeFileLog(log,DEBUG);

                                     ////writeFileLog("[MutekInitWithCardMoveInFront() Exit] negative reply from mutek");
                                     
                                     AVRM_writeFileLog("[MutekInitWithCardMoveInFront() Exit] negative reply from mutek",DEBUG);
                                     
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                ////writeFileLog("[MutekInitWithCardMoveInFront() Exit] reply byte receieve from mutek");
                                AVRM_writeFileLog("[MutekInitWithCardMoveInFront() Exit] reply byte receieve from mutek",DEBUG);
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek");
                         AVRM_writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek",DEBUG);
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[MutekInitWithCardMoveInFront() Exit] Command Transmit failed");
            AVRM_writeFileLog("[MutekInitWithCardMoveInFront() Exit] Command Transmit failed",DEBUG);
            return 0;
        }


}//int MutekInitWithCardMoveInFront() end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MutekInitWithErrorCardBin( char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {

      
        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
  
        char log[LOG_ARRAY_SIZE];

        char RecvPackt[100];

        char command[100];

        memset(log,'\0',LOG_ARRAY_SIZE);
 
        memset(command,'\0',100);

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x30; //CM
        command[6]  = 0x31; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //Data   = 13 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 25 Byte
        
        int totalByteToRecv=25; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[MutekInitWithErrorCardBin()] Command Transmit success");
                    AVRM_writeFileLog("[MutekInitWithErrorCardBin()] Command Transmit success",DEBUG);

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[MutekInitWithErrorCardBin()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[MutekInitWithErrorCardBin()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[MutekInitWithErrorCardBin()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[MutekInitWithErrorCardBin()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                      ////writeFileLog("[MutekInitWithErrorCardBin()] Init with card move to front success");
		                              AVRM_writeFileLog("[MutekInitWithErrorCardBin()] Init with card move to front success",INFO);
                                      return 1;
                                    
				    
                                }
                                else if(0x4e == RecvPackt[4] ) 
                                {
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithErrorCardBin() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     ////writeFileLog(log); 
                                     
                                     AVRM_writeFileLog(log,INFO);

                                     ////writeFileLog("[MutekInitWithErrorCardBin() Exit] negative reply from mutek");
                                     
                                     AVRM_writeFileLog("[MutekInitWithErrorCardBin() Exit] negative reply from mutek",DEBUG);

                                     return 0;

                                }
                                else
                                {
                                     ////writeFileLog("[MutekInitWithErrorCardBin()] Init with card move to front failed");
                                     AVRM_writeFileLog("[MutekInitWithErrorCardBin()] Init with card move to front failed",INFO);
                                     return 0;

                                }

		         }
		         else //failure case
		         {

                            ////writeFileLog("[MutekInitWithErrorCardBin() Exit] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[MutekInitWithErrorCardBin() Exit] Reply Bytes receieve from MUTEK is failed ",DEBUG);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     ////writeFileLog("[MutekInitWithErrorCardBin() Exit] negative reply from mutek");
                                     
                                     AVRM_writeFileLog("[MutekInitWithErrorCardBin() Exit] negative reply from mutek",DEBUG);
                                     
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithErrorCardBin() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     ////writeFileLog(log); 
                                     
                                     AVRM_writeFileLog(log,DEBUG);

                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                ////writeFileLog("[MutekInitWithErrorCardBin() Exit] reply byte receieve from mutek");
                                AVRM_writeFileLog("[MutekInitWithErrorCardBin() Exit] reply byte receieve from mutek",DEBUG);
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[MutekInitWithErrorCardBin() Exit] fail receieve ack byte from mutek");
                         AVRM_writeFileLog("[MutekInitWithErrorCardBin() Exit] fail receieve ack byte from mutek",DEBUG);
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[MutekInitWithErrorCardBin() Exit] Command Transmit failed");
            AVRM_writeFileLog("[MutekInitWithErrorCardBin() Exit] Command Transmit failed",DEBUG);
            return 0;

        }//else end


}//int MutekInitWithErrorCardBin() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function Name : ConnectDevice_c 
// Return Type   : int 
                  //0 = Device Connected Successfully
                  //1 = Port Does Not Exits 
                  //2 = Communication Failure
                  //3 = Channel Clearance failed due to rejection bin full   [ Not Tested] 
                  //4 = Channel Clearance failed due to return mouth blocked [ Not implemented]
                  //5 = Channel Clearance failed due to unknown reason
                  //6 = Operation timeout occurred
                  //7 = Other Error 
// Parameters    : Name                           Type          Description 

//                  PortId                         int           Serial Port number to which the Device is Connected.

//                  ChannelClearanceMode           int           If there is a card in the device Channel either it will be sent to 		                                                       rejection bin or return from the mouth of the device or kept in the 								       position. 
//
//                                                               0 = Retain in the Channel.
//                                                               1 = Send to Rejection bin.
//                                                               2 = Return from the mouth of the Device.
                    
//                  Timeout                         int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 


int ConnectDevice_c( int  PortId, int  ChannelClearanceMode, int  Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        //printf("\n[ConnectDevice_c()] Log File Mode : %d", (int)fnLogfileMode);

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        ////writeFileLog("[ConnectDevice_c()] Entry ");
        
        AVRM_writeFileLog("[ConnectDevice_c()] Entry ",TRACE);

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char bcc=0x00;

        unsigned char initialized_Data[9],rcvPkt[25];

        int i=0,openPortStatus = -1,
                send_rvalue = 0,
                totalByteToRecv=0,
                rtcode = -1,
                cardBinFullFlag = 0;

        unsigned int rcvPktLen =0x00;
         
        //Check COM port Here
        ////writeFileLog("[ConnectDevice_c()] Going to Open Port");
        AVRM_writeFileLog("[ConnectDevice_c()] Going to Open Port",TRACE);

        if(PortId < 0)
        {
           ////writeFileLog("[ConnectDevice_c()] Fail to Open Port ");
           AVRM_writeFileLog("[ConnectDevice_c()] Fail to Open Port ",ERROR);
           // 1 = Port Does Not Exits 
           ClearReceiveBuffer (g_ComHandle);
           ClearTransmitBuffer(g_ComHandle);
           return COMMUNICATION_FAILURE;
        } 
        
        //++Now Call Open Port Function  
        //++get baudrate from ini file
        int baudrate=0;

        baudrate = GetComPortBaudRate();

        if( baudrate <=0 )
        {
            baudrate=38400;
        }
        
        //++baudrate = 9600;
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        
        sprintf(log,"[ConnectDevice_c()] Baudrate: %d . ",baudrate);
        
        AVRM_writeFileLog(log,INFO);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        g_ComHandle =-1;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
        openPortStatus = OpenPort(PortId,baudrate,&g_ComHandle);
        
        if(1 == openPortStatus)
        {
		   //printf("\n[ConnectDevice_c()] Open Port Successfully");
           ////writeFileLog("[ConnectDevice_c()] Open Port Successfully");
           AVRM_writeFileLog("[ConnectDevice_c()] Open Port Successfully",INFO);
           memset(log,'\0',LOG_ARRAY_SIZE);
           sprintf(log,"[ConnectDevice_c()] ComPortHandle: %d . ",g_ComHandle);
           ////writeFileLog(log);
           AVRM_writeFileLog(log,INFO);
        }
        else
        {
           ////writeFileLog("[ConnectDevice_c()] Fail to Open Port ");
           AVRM_writeFileLog("[ConnectDevice_c()] Fail to Open Port ",FATAL);
           //Port Does Not Exits 
           ClearReceiveBuffer (g_ComHandle);
           ClearTransmitBuffer(g_ComHandle);   
           return COMMUNICATION_FAILURE; 
        }

         
         //ChannelClearanceMode Check Here
         if(ChannelClearanceMode == 0)
         {
		    //Retain in the Channel 
		    //Now going to get bcc 
		    ////writeFileLog("[ConnectDevice_c()] Initialise with Retain in the Channel");
		    AVRM_writeFileLog("[ConnectDevice_c()] Initialise with Retain in the Channel",INFO);
		    g_resetData_Retain_Channel[8] =0x00;
		    bcc = getBcc(9,g_resetData_Retain_Channel);
		    g_resetData_Retain_Channel[8] = bcc;
		    memset(log,'\0',LOG_ARRAY_SIZE);
		    sprintf(log,"[ConnectDevice_c()] bcc value is 0x%xh",bcc);
		    ////writeFileLog(log); 
		    AVRM_writeFileLog(log,INFO);
		    memset(initialized_Data,'\0',9);
		    for(i=0;i<9;i++)
		    {
		          initialized_Data[i]=g_resetData_Retain_Channel[i];
		    }

         }
         else if(ChannelClearanceMode == 1)
         {
		    // Send to Rejection bin 
		    // Now going to get bcc 
		    ////writeFileLog("[ConnectDevice_c()] Initialise with Send to Rejection bin");
		    AVRM_writeFileLog("[ConnectDevice_c()] Initialise with Send to Rejection bin",INFO);
		    g_resetData_Capture_Card_Bin[8] =0x00;
		    bcc = getBcc(9,g_resetData_Capture_Card_Bin);
		    g_resetData_Capture_Card_Bin[8] = bcc;
		    memset(log,'\0',LOG_ARRAY_SIZE);
		    sprintf(log,"[ConnectDevice_c()] bcc value is 0x%xh",bcc);
		    ////writeFileLog(log); 
		    AVRM_writeFileLog(log,INFO);
		    memset(initialized_Data,'\0',9);
		    for(i=0;i<9;i++)
		    {
		          initialized_Data[i] = g_resetData_Capture_Card_Bin[i];
		    }  //for end
	  
         }
         else if(ChannelClearanceMode == 2)
         {
		    //++Return from the mouth of the Device 
		    //++Now going to get bcc 
		    ////writeFileLog("[ConnectDevice_c()] Initialise with Return from the mouth of the Device");
		    AVRM_writeFileLog("[ConnectDevice_c()] Initialise with Return from the mouth of the Device",INFO);
		    g_resetData_CardHolding_Position[8] =0x00;
		    bcc = getBcc(9,g_resetData_CardHolding_Position);
		    g_resetData_CardHolding_Position[8] = bcc;
		    memset(log,'\0',LOG_ARRAY_SIZE);
		    sprintf(log,"[ConnectDevice_c()] bcc value is 0x%xh",bcc);
		    ////writeFileLog(log); 
		    AVRM_writeFileLog(log,INFO);
		    memset(initialized_Data,'\0',9);
		    for(i=0;i<9;i++)
		    {
		         initialized_Data[i] = g_resetData_CardHolding_Position[i];
		    } //for end
 
         }
         else
         {  
		    //ChannelClearanceMode does not match so return from this function 
		    ////writeFileLog("[ConnectDevice_c()] Initialise Failed");
		    ////writeFileLog("[ConnectDevice_c()] Invalid ChannelClearanceMode found from Parameters");
		    ////writeFileLog("[ConnectDevice_c()] Now going to Return");
		    AVRM_writeFileLog("[ConnectDevice_c()] Initialise Failed",INFO);
		    AVRM_writeFileLog("[ConnectDevice_c()] Invalid ChannelClearanceMode found from Parameters",INFO);
		    AVRM_writeFileLog("[ConnectDevice_c()] Now going to Return",INFO);
		    //Channel Clearance failed due to unknown reason 
		    ClearReceiveBuffer (g_ComHandle);
		    ClearTransmitBuffer(g_ComHandle);
		    return CHANEL_CLEARANCE_FAILED_DUE_TO_UNKNOWN_REASON;

         }//else block
         
         ////writeFileLog("[ConnectDevice_c()] ChannelClearanceMode ok Now going to Send Initialize Data");
         AVRM_writeFileLog("[ConnectDevice_c()] ChannelClearanceMode ok Now going to Send Initialize Data",INFO);
         //Before Send Command clear all serial buffer 

         ClearReceiveBuffer (g_ComHandle);
         ClearTransmitBuffer(g_ComHandle);
         
         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         /*
         unsigned char error_Data[9]= {0xAA,0X55,0xAA,0X55,0xAA,0X55,0xAA,0X55,0xAA}; 
         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                               g_SCRDDeviceid,
                               "Tx",
                               error_Data,
                               9
                            );
         for(i=0;i<9;i++) {
			send_rvalue = SendSingleByteToSerialPort(g_ComHandle,error_Data[i]);
         }//for end
         memset(rcvPkt,'\0',24); 
         totalByteToRecv = 1;
         rcvPktLen  = 20;
         rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
         */
         
         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		 //int counter=1;
		 //1for(counter=1;counter<=1;counter++){
			 
			 SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
								   g_SCRDDeviceid,
								   "Tx",
								   initialized_Data,
								   9
								);

			 for(i=0;i<9;i++) {
					 
					 //memset(log,'\0',LOG_ARRAY_SIZE);

					 //sprintf(log,"[ConnectDevice_c()] Initialize Command[%d] = 0x%xh",i,initialized_Data[i]);

					 ////writeFileLog(log); 

					 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,initialized_Data[i]);

					 if(0 == send_rvalue) {
						////writeFileLog("[ConnectDevice_c()] Failed to send total Initialize Command ");
						AVRM_writeFileLog("[ConnectDevice_c()] Failed to send total Initialize Command ",ERROR);
						return COMMUNICATION_FAILURE;
					 }


			 }//for(i=0;i<9;i++)

	         //++}//for end
	     
			 AVRM_writeFileLog("[ConnectDevice_c()] Initialize Data send Successfully",INFO);
			 AVRM_writeFileLog("[ConnectDevice_c()] Now Going to read Acknowledgement",INFO);
			 
			 //++Now going to Check Acknowledgement  

			 totalByteToRecv = 1;

			 memset(rcvPkt,'\0',24); 
			 
			 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

			 memset(log,'\0',LOG_ARRAY_SIZE);

			 sprintf(log,"[ConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

			 AVRM_writeFileLog(log,INFO);
			 
			 //if(0 == rtcode && 1 == counter ){
			 //	continue;
			 //}
			 
			 if(0 == rtcode) {
					 
					 //++28 = Communication Failure 
					 ClearReceiveBuffer (g_ComHandle);
					 ClearTransmitBuffer(g_ComHandle);
					 ClosePort(g_ComHandle);
					 g_ComHandle = -1;
					 ////writeFileLog("[ConnectDevice_c()] Communication Failure when Going to read Acknowledgement");
					 AVRM_writeFileLog("[ConnectDevice_c()] Communication Failure when Going to read Acknowledgement",ERROR);
					 return COMMUNICATION_FAILURE;
			 }
			 else
			 {
			 
					memset(log,'\0',LOG_ARRAY_SIZE);

					sprintf(log,"[ConnectDevice_c()] Acknowledgement against Initialize Command [0] = 0x%xh.\n",rcvPkt[0]);
					
					AVRM_writeFileLog(log,DEBUG);
					
					
			}
			 
		 //}//for end
		  
         //If Return Data is 06h then Going to Read 25byte Data 

         if(0x06 == rcvPkt[0])
         {

                      ////writeFileLog("[ConnectDevice_c()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[ConnectDevice_c()] Acknowledgement Received",DEBUG);

                      totalByteToRecv = 25;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[ConnectDevice_c()] statusRead() return code = %d and receieve packet Length = %d",rtcode,rcvPktLen);

                      ////writeFileLog(log); 
                      
                      AVRM_writeFileLog(log,DEBUG);

                      if(0 == rtcode)
		              {
								 // 28 = Communication Failure 
								 ClearReceiveBuffer (g_ComHandle);
								 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[ConnectDevice_c()] Communication Failure when Going to read Initialize reply bytes");
                                 AVRM_writeFileLog("[ConnectDevice_c()] Communication Failure when Going to read Initialize reply bytes",DEBUG);
				                 return COMMUNICATION_FAILURE;
		      
                      }//if(0 == rtcode)    
                   

                      //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 31 30 31[e1] 36[e2] 3 bc

                      //Sample Data
                      //STX  (1 byte) : f2
	                  //ADDR (1 byte) : 0
					  //LENH (1 byte) : 0
					  //LENL (1 byte) : 5
					  //EMT  (1 byte) : 4e
					  //CM   (1 byte) : 31
					  //PM   (1 byte) : 30
							  //e1   (1 byte) : 31
					  //e0   (1 byte) : 36
							  //DATA (N bytes): No Byte
							  //ETX  (1 byte) : 3
					  //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[ConnectDevice_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,DEBUG);

                          ////writeFileLog("[ConnectDevice_c() Exit] Got 0x4e error: others error ");
                          
                          AVRM_writeFileLog("[ConnectDevice_c() Exit] Got 0x4e error: others error ",DEBUG);
                           
						  SetLastError(rcvPkt[7], rcvPkt[8]);
 
                          return OTHER_ERROR ; //Other Error


                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end

                      //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	                  {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[ConnectDevice_c()] Initialize Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                                ////writeFileLog(log); 

                                // Card Status Code st0 
                                if(7 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] No Card in MTK-571");
											   AVRM_writeFileLog("[ConnectDevice_c()] No Card in MTK-571",INFO);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] One Card in Gate");
											   AVRM_writeFileLog("[ConnectDevice_c()] One Card in Gate",INFO);
												
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] One Card on RF/IC Card Position");
											   AVRM_writeFileLog("[ConnectDevice_c()] One Card on RF/IC Card Position",INFO);
									} 
                                }

                                // Card Status Code st1 
                                if(8 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] No Card in Stacker");
											   AVRM_writeFileLog("[ConnectDevice_c()] No Card in Stacker",INFO);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] Few Card in Stacker");
											   AVRM_writeFileLog("[ConnectDevice_c()] Few Card in Stacker",INFO);
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] Enough Card in the Box");
											   AVRM_writeFileLog("[ConnectDevice_c()] Enough Card in the Box",INFO);
									}   
                                } 

                                // Card Status Code st2 
                                if(9 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] Error Card bin Not Full");
											   AVRM_writeFileLog("[ConnectDevice_c()] Error Card bin Not Full",INFO);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[ConnectDevice_c()] Error Card Bin Full");
											   AVRM_writeFileLog("[ConnectDevice_c()] Error Card Bin Full",INFO);
											   // Channel Clearance failed due to rejection bin full 
											   cardBinFullFlag = 1;
									}
                                        

                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++)

                      ////////////////////////////////////////////////////////////////////////////

                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      
                      //Log ACK byte upon receieve complete data from mutek
                      char ack_Data[1]={0x06};
                      
                      if(1 == send_rvalue) {
                      
						  SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
							   g_SCRDDeviceid,
							   "Tx",
							   ack_Data,
							   1
										 );
						  
                      }
                      //////////////////////////////////////////////////////////////
                      
                      
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
					  {
							////writeFileLog("[ConnectDevice_c() Exit] Failed to send total Acknowledgement Command ");
							AVRM_writeFileLog("[ConnectDevice_c() Exit] Failed to send total Acknowledgement Command ",ERROR);
							// 28 = Communication Failure 
						   return COMMUNICATION_FAILURE;
					  }
                       
                      if(1 == cardBinFullFlag)
                      {
                        // 3 = Channel Clearance failed due to rejection bin full 
                        ////writeFileLog("[ConnectDevice_c() Exit] Channel Clearance failed due to rejection bin full ");
                        AVRM_writeFileLog("[ConnectDevice_c() Exit] Channel Clearance failed due to rejection bin full ",INFO);
                        return CHANEL_CLEARANCE_FAILED_DUE_TO_REJECTION_BIN_FULL;  
                      }
                      // 0 = Device Connected Successfully 
                      ////writeFileLog("[ConnectDevice_c() Exit] Device Connected Successfully ");
                      AVRM_writeFileLog("[ConnectDevice_c() Exit] Device Connected Successfully ",INFO);
                      return SUCCESS;
                       
         }
         //If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             ////writeFileLog("[ConnectDevice_c() Exit] Nak Reply Received");
             AVRM_writeFileLog("[ConnectDevice_c() Exit] Nak Reply Received",INFO);
             //now close port
             ClosePort(g_ComHandle);
             g_ComHandle =-1;
             //28 = Communication Failure
             return COMMUNICATION_FAILURE;  
         }
         //Malay:If Return Data is 04h then No need to read Data 
         else if (0x04 == rcvPkt[0])
         {
             ////writeFileLog("[ConnectDevice_c() Exit] EOT Reply Received");
             AVRM_writeFileLog("[ConnectDevice_c() Exit] EOT Reply Received",INFO);
             //now close port
             ClosePort(g_ComHandle);
             g_ComHandle =-1;
             //28 = Communication Failure
             return COMMUNICATION_FAILURE;  
         }
         

}//ConnectDevice_c(int PortId, int ChannelClearanceMode, int Timeout) end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int CheckCardPresent() {


        unsigned char rcvPkt[25];

        int i=0, send_rvalue = 0,
                 totalByteToRecv=0,
                 rtcode = -1,
                 returnMouthFlag = 0,
                 channelBlockFlag = 0;

        char bcc=0x00;

        unsigned int rcvPktLen =0x00; 

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        g_getStatus[0] = 0xF2;
        g_getStatus[1] = MTK_DEVICE_ADDRESS;
        g_getStatus[2] = 0x00;
        g_getStatus[3] = 0x03;
        g_getStatus[4] = 0x43;
        g_getStatus[5] = 0x31;
        g_getStatus[6] = 0x30; 
        g_getStatus[7] = 0x03;
        g_getStatus[8] = 0x00;

        bcc = getBcc(9,g_getStatus);
        
        g_getStatus[8] = bcc;

        ////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

        ////////////////////////////////////////////////////////////////////////////////////////////

        for(i=0;i<9;i++)
	{
		 
                 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[CheckCardPresent()] Failed to Send Report status Command ");
                    AVRM_writeFileLog("[CheckCardPresent()] Failed to Send Report status Command ",DEBUG);
                    //++1 = Communication Failure 
                    return 1;
                 }


	}//for loop end

        ////////////////////////////////////////////////////////////////////////////////////////////////////

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        if(0 == rtcode)
        {
                 //1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 AVRM_writeFileLog("[CheckCardPresent()] Communication Failure ",ERROR);
                 return 1;
        }
        else
        {

             memset(log,'\0',LOG_ARRAY_SIZE);

             sprintf(log,"[CheckCardPresent()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
             ////writeFileLog(log);

             AVRM_writeFileLog(log,DEBUG);

        }

        //If Return Data is 06h then Going to Read 25byte Data 
        if(0x06 == rcvPkt[0])
        {
                      ////writeFileLog("[CheckCardPresent()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[CheckCardPresent()] Acknowledgement Received",DEBUG);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                     
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[CheckCardPresent()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log); 
                      
                      AVRM_writeFileLog(log,DEBUG);

                      if(0 == rtcode)
		      {
				 //1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[CheckCardPresent() Exit] Communication Failure ");
                 AVRM_writeFileLog("[CheckCardPresent() Exit] Communication Failure ",DEBUG);
				 return 1;
		      }

                      //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 33
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[CheckCardPresent()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,DEBUG);

                          ////writeFileLog("[CheckCardPresent() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[CheckCardPresent() Exit] Got 0x4e error",DEBUG);
						  
  						  SetLastError(rcvPkt[7], rcvPkt[8]);
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                ////writeFileLog("[CheckCardPresent() Exit] Insertion/return mouth blocked");
                                AVRM_writeFileLog("[CheckCardPresent() Exit] Insertion/return mouth blocked",INFO);
                          }
                          else
                          {
                                ////writeFileLog("[CheckCardPresent() Exit] Other error");
                                AVRM_writeFileLog("[CheckCardPresent() Exit] Other error",INFO);
                                

                          }
                          
                          return 5 ; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[CheckCardPresent()] Enable Card Acceptance Success");
                             AVRM_writeFileLog("[CheckCardPresent()] Enable Card Acceptance Success",INFO);

                      }
                      else
                      {

                             ////writeFileLog("[CheckCardPresent() Exit] Enable Card Acceptance Failed");
                             AVRM_writeFileLog("[CheckCardPresent() Exit] Enable Card Acceptance Failed",ERROR);
                             return 5;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[CheckCardPresent()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

				
                                //Card Status Code st0 
                                if(7 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[CheckCardPresent()] No Card in MTK-571");
											   AVRM_writeFileLog("[CheckCardPresent()] No Card in MTK-571",INFO);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[CheckCardPresent()] One Card in Gate");
											   AVRM_writeFileLog("[CheckCardPresent()] One Card in Gate",INFO);
											   returnMouthFlag = 1; 
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[CheckCardPresent()] One Card on RF/IC Card Position");
											   AVRM_writeFileLog("[CheckCardPresent()] One Card on RF/IC Card Position",INFO);
											   channelBlockFlag = 1;
									  
									} 
                                }

                                /*
                                //Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           //writeFileLog("[CheckCardPresent()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           //writeFileLog("[CheckCardPresent()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           //writeFileLog("[CheckCardPresent()] Enough Card in the Box");
		                        }   
                                } 
                                //Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           //writeFileLog("[CheckCardPresent()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           //writeFileLog("[CheckCardPresent()] Error Card Bin Full");
                                           
		                        }
                                }
                                */

				
	              }
                      //Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ////////////////////////////////////////////////////////////
                      
                      //Log ACK byte upon receieve complete data from mutek
                      char ack_Data[1]={0x06};
                      
                      if(1 == send_rvalue) {
                      
		              SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
					       g_SCRDDeviceid,
					       "Tx",
					       ack_Data,
					       1
		                             );
                      
                      }
                      //////////////////////////////////////////////////////////////
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[CheckCardPresent()] Failed to send total Acknowledgement Command ");
                            //1 = Communication Failure 
                            AVRM_writeFileLog("[CheckCardPresent()] Failed to send total Acknowledgement Command ",ERROR);
                            return 1;
		      }
                      else if(1 == returnMouthFlag)
                      { 
                      	    //3 = One Card infront od Gate
                      	    AVRM_writeFileLog("[CheckCardPresent()] One Card infront od Gate ",INFO);
                            return 3; 
                      }
                      else if(1 == channelBlockFlag)
                      {    
                            //2 = One Card on RF/IC Card Position 
                            AVRM_writeFileLog("[CheckCardPresent()] One Card on RF/IC Card Position ",INFO);
                            return 2;
                      }
                      else
                      {
                            //0 = No Card in chanel
                            AVRM_writeFileLog("[CheckCardPresent()] No Card in chanel ",INFO);
                            return 0;
                      }
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             ////writeFileLog("[CheckCardPresent()] Nak Reply Received");
             AVRM_writeFileLog("[CheckCardPresent()] Nak Reply Received",DEBUG);
             //5 = Other Error 
             return 5;  
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x04 == rcvPkt[0])
       {
             ////writeFileLog("[CheckCardPresent()] EOT Reply Received");
             AVRM_writeFileLog("[CheckCardPresent()] EOT Reply Received",DEBUG);
             //5 = Other Error 
             return 5;  
       }//else end



}//int CheckCardPresent() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int DisableCardAcceptance( char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode) {

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        unsigned char rcvPkt[25]="\0";

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        int i=0, send_rvalue = 0,
                 totalByteToRecv=0,
                 rtcode = -1,
                 returnMouthFlag = 0,
                 channelBlockFlag = 0;

        unsigned int rcvPktLen =0x00; 

        char bcc=0x00;

        struct timespec begints={0}, endts={0},diffts={0};

        //Start Time
        clock_gettime(CLOCK_MONOTONIC, &begints);

        g_DisableCardAcceptance[0]= 0xF2 ;
        g_DisableCardAcceptance[1]= MTK_DEVICE_ADDRESS ;
        g_DisableCardAcceptance[2]= 0x00 ;
        g_DisableCardAcceptance[3]= 0x03 ; 
        g_DisableCardAcceptance[4]= 0x43 ;
        g_DisableCardAcceptance[5]= 0x33 ;
        g_DisableCardAcceptance[6]= 0x31 ;
        g_DisableCardAcceptance[7]= 0x03 ;
        g_DisableCardAcceptance[8]= 0x00 ;

        ////writeFileLog("[DisableCardAcceptance()] Going to Send Disable Acceptance Command");
        
        AVRM_writeFileLog("[DisableCardAcceptance()] Going to Send Disable Acceptance Command",DEBUG);

        bcc = getBcc(9,g_DisableCardAcceptance);

        g_DisableCardAcceptance[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisableCardAcceptance()] bcc value is 0x%xh",bcc);

        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);
        
        delay_mSec(200);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_DisableCardAcceptance,
                          9
                        );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for(i=0;i<9;i++)
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance()] Disable Acceptance Command[%d] = 0x%xh",i,g_DisableCardAcceptance[i]);

                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_DisableCardAcceptance[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[DisableCardAcceptance()] Failed to Send Report status Command ");
                    AVRM_writeFileLog("[DisableCardAcceptance()] Failed to Send Report status Command ",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;


                 }

	}

        //////////////////////////////////////////////////////////////////////////////////////////


        //AVRM_writeFileLog("[DisableCardAcceptance()] Disable Acceptance Command send Successfully",INFO);
        //AVRM_writeFileLog("[DisableCardAcceptance()] Now Going to read Acknowledgement",INFO);


        // Now going to Check Acknowledgement  
  
        totalByteToRecv = 1;

        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisableCardAcceptance()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

        AVRM_writeFileLog(log,DEBUG);

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 AVRM_writeFileLog("[DisableCardAcceptance()] Communication Failure",ERROR);
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance()] Acknowledgement against Disable Acceptance Command[0] = 0x%xh.\n",rcvPkt[0]);

                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {
		 
                 //Soomit-29082016: Error code 0x4E needs to be handled here [Solved]
                 ////writeFileLog("[DisableCardAcceptance()] Acknowledgement Received");
                 
                 AVRM_writeFileLog("[DisableCardAcceptance()] Acknowledgement Received",DEBUG);

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',11);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);

                 
                 /*

                 for(i=0;i<rcvPktLen;i++)
	         {
		         memset(log,'\0',LOG_ARRAY_SIZE);

                         sprintf(log,"[DisableCardAcceptance()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                         //writeFileLog(log);

                 }
                 */

                 /////////////////////////////////////////////////////////////////////////////////////////////////
                 
                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 

				 ClearReceiveBuffer (g_ComHandle);

				 ClearTransmitBuffer(g_ComHandle);

                                 ////writeFileLog("[DisableCardAcceptance()] Failed to read status command reply bytes.");
                                 AVRM_writeFileLog("[DisableCardAcceptance()] Failed to read status command reply bytes.",DEBUG);
				 return COMMUNICATION_FAILURE;
		      }

                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 31
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {

                          //Log e1 and e2 error codes
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[DisableCardAcceptance()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,ERROR);

                          ////writeFileLog("[DisableCardAcceptance() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[DisableCardAcceptance() Exit] Got 0x4e error",ERROR);
						  
  						  SetLastError(rcvPkt[7], rcvPkt[8]);
						  
						  return OTHER_ERROR; //Other error
/*						                            
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                //writeFileLog("[DisableCardAcceptance() Exit] Insertion/return mouth blocked.");
                                return 3; //Insertion mouth blocked

                          }
                          else if( (0x31==rcvPkt[7]) && ( 0x30 == rcvPkt[8] ) ) 
                          {
                                //writeFileLog("[DisableCardAcceptance() Exit] Card Jammed.");
                                return 3; //Insertion mouth blocked

                          }
                          else
                          {
                                //writeFileLog("[DisableCardAcceptance() Exit] Other error");

                                return 5; //other error
                                

                          }
*/                          
                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x33 == rcvPkt[5] ) && ( 0x31 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[DisableCardAcceptance()] Disable Command issue success.");
                             
                             AVRM_writeFileLog("[DisableCardAcceptance()] Disable Command issue success.",INFO);
                             
                             clock_gettime(CLOCK_MONOTONIC, &endts);

                             diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                            
                             // 0 = Operation Successful 
                             return SUCCESS;
                             
                      }
                      else
                      {

                             ////writeFileLog("[DisableCardAcceptance() Exit] Disable Command issue failed.");
                             AVRM_writeFileLog("[DisableCardAcceptance() Exit] Disable Command issue failed.",DEBUG);
                             return OTHER_ERROR;//Other Error

                      }

                     

                 /////////////////////////////////////////////////////////////////////////////////////////////////

                 
        }
        else if (0x15 == rcvPkt[0])
        {
             ////writeFileLog("[DisableCardAcceptance()] Nak Reply Received");
             AVRM_writeFileLog("[DisableCardAcceptance()] Nak Reply Received",DEBUG);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }
        else if (0x04 == rcvPkt[0])
        {
             ////writeFileLog("[DisableCardAcceptance()] EOT Reply Received");
             AVRM_writeFileLog("[DisableCardAcceptance()] EOT Reply Received",DEBUG);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }

}//int DisableCardAcceptance() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// To get Current Report status of st0, st1, st2
// To Enable Card Acceptance 
// static unsigned char g_EnableCardAcceptance[9]={0xF2,0x00,0x00,0x03,0x43,0x33,0x30,0x03,0x00};
// To Disable Card Acceptance 
// static unsigned char g_DisableCardAcceptance[9]={0xF2,0x00,0x00,0x03,0x43,0x33,0x31,0x03,0x00};



// Function Name : AcceptCard_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Channel blocked 
//                  3 = Insert/return mouth block
//                  4 = Operation timeout occurred
//                  5 = Other Error 
//
//  Parameters    : Name                           Type          Description 

                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

int AcceptCard_c( int   Timeout,char  *LogdllPathstr,char  *LogFileName,char  *deviceid,int   fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        int presentcard=0;

        struct timespec begints={0}, endts={0},diffts={0};

        memset(log,'\0',LOG_ARRAY_SIZE);

        ////////////////////////////////////////////////////////////////////////////////////////
        
        g_getStatus[0] = 0xF2;
        g_getStatus[1] = MTK_DEVICE_ADDRESS;
        g_getStatus[2] = 0x00;
        g_getStatus[3] = 0x03;
        g_getStatus[4] = 0x43;
        g_getStatus[5] = 0x31;
        g_getStatus[6] = 0x30; 
        g_getStatus[7] = 0x03;
        g_getStatus[8] = 0x00;

        ////////////////////////////////////////////////////////////////////////////////////////

        g_DisableCardAcceptance[0]= 0xF2 ;
        g_DisableCardAcceptance[1]= MTK_DEVICE_ADDRESS ;
        g_DisableCardAcceptance[2]= 0x00 ;
        g_DisableCardAcceptance[3]= 0x03 ; 
        g_DisableCardAcceptance[4]= 0x43 ;
        g_DisableCardAcceptance[5]= 0x33 ;
        g_DisableCardAcceptance[6]= 0x31 ;
        g_DisableCardAcceptance[7]= 0x03 ;
        g_DisableCardAcceptance[8]= 0x00 ;

        ////////////////////////////////////////////////////////////////////////////////////////

        g_EnableCardAcceptance[0]=  0xF2 ;
        g_EnableCardAcceptance[1]=  MTK_DEVICE_ADDRESS;
        g_EnableCardAcceptance[2]=  0x00 ;
        g_EnableCardAcceptance[3]=  0x03 ;
        g_EnableCardAcceptance[4]=  0x43 ;
        g_EnableCardAcceptance[5]=  0x33 ;
        g_EnableCardAcceptance[6]=  0x30 ;
        g_EnableCardAcceptance[7]=  0x03 ;
        g_EnableCardAcceptance[8]=  0x00 ;

        ////////////////////////////////////////////////////////////////////////////////////////

        char bcc=0x00;

        unsigned char rcvPkt[25];

        int i=0, send_rvalue = 0,
                 totalByteToRecv=0,
                 rtcode = -1,
                 returnMouthFlag = 0,
                 channelBlockFlag = 0;

        unsigned int rcvPktLen =0x00; 

        //If there was any card block or not 

        AVRM_writeFileLog("[AcceptCard_c()] Going to send Current Report status of st0, st1, st2",INFO);
        
        bcc = getBcc(9,g_getStatus);
        
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[AcceptCard_c()] bcc value is 0x%xh",bcc);
        
        ////writeFileLog(log); 
        
        AVRM_writeFileLog(log,INFO);
        
        delay_mSec(g_MaxDelay);

        //Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[AcceptCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[AcceptCard_c()] Failed to Send Report status Command ");
                    AVRM_writeFileLog("[AcceptCard_c()] Failed to Send Report status Command ",INFO);
                    //28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }


	}//for(i=0;i<9;i++) block end

        //////////////////////////////////////////////////////////////////////////////////////////

        //AVRM_writeFileLog("[AcceptCard_c()] Status Command send Successfully",INFO);
        //AVRM_writeFileLog("[AcceptCard_c()] Now Going to read Acknowledgement",INFO);

        //Now going to Check Acknowledgement   

        totalByteToRecv = 1;

        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d.",rtcode,rcvPktLen);
        
        ////writeFileLog(log); 
        
        AVRM_writeFileLog(log,INFO);

        if(0 == rtcode)
        {
                 //28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[AcceptCard_c()] Communication Failure");
                 AVRM_writeFileLog("[AcceptCard_c()] Communication Failure",INFO);
                 return COMMUNICATION_FAILURE;
        }
        else
        {

             memset(log,'\0',LOG_ARRAY_SIZE);

             sprintf(log,"[AcceptCard_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
             ////writeFileLog(log);
             
             AVRM_writeFileLog(log,DEBUG);


        }

        //If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      //writeFileLog("[AcceptCard_c()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[AcceptCard_c()] Acknowledgement Received",INFO);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                     
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log); 
                      
                      AVRM_writeFileLog(log,DEBUG);

                      if(0 == rtcode)
		      {
								 //1 = Communication Failure 
								 ClearReceiveBuffer (g_ComHandle);
								 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[AcceptCard_c()] Communication Failure");
                                 AVRM_writeFileLog("[AcceptCard_c()] Communication Failure",ERROR);
				                 return COMMUNICATION_FAILURE;
		      }

                      //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 31 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 31
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[AcceptCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,ERROR);
 
                          ////writeFileLog("[AcceptCard_c() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[AcceptCard_c() Exit] Got 0x4e error",ERROR);

                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                ////writeFileLog("[AcceptCard_c() Exit] Forced insertion of card detected");
                                AVRM_writeFileLog("[AcceptCard_c() Exit] Forced insertion of card detected",INFO);
                          }

                          /*
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                //writeFileLog("[AcceptCard_c() Exit] Insertion/return mouth blocked");
                                return 3 ; //Insertion/return mouth blocked

                          }
                          else
                          {
                                //writeFileLog("[AcceptCard_c() Exit] Other error");
                                return 5 ; //Other error

                          }
                          
                          */

                          ////writeFileLog("[AcceptCard_c() Exit] Other error found when issue device status command.");
                          AVRM_writeFileLog("[AcceptCard_c() Exit] Other error found when issue device status command.",INFO);
   						  SetLastError(rcvPkt[7], rcvPkt[8]);
						   
                           return OTHER_ERROR ; //Other error



                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end

                            

                      //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[AcceptCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

				//Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] No Card in MTK-571");
                                           AVRM_writeFileLog("[AcceptCard_c()] No Card in MTK-571",INFO);
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] One Card in Gate");
                                           AVRM_writeFileLog("[AcceptCard_c()] One Card in Gate",INFO);
                                           //2 = Insert/return mouth block 
                                           returnMouthFlag = INSERT_RETURN_MOUTH_BLOCKED; 
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] One Card on RF/IC Card Position");
                                           AVRM_writeFileLog("[AcceptCard_c()] One Card on RF/IC Card Position",INFO);
                                           //1= Channel blocked 
                                           channelBlockFlag = CHANNEL_BLOCKED;
                                  
		                        } 
                                }
                                //Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] No Card in Stacker");
                                           AVRM_writeFileLog("[AcceptCard_c()] No Card in Stacker",INFO);
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] Few Card in Stacker");
                                           AVRM_writeFileLog("[AcceptCard_c()] Few Card in Stacker",INFO);
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] Enough Card in the Box");
                                           AVRM_writeFileLog("[AcceptCard_c()] Enough Card in the Box",INFO);
		                        }   
                                } 
                                //Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] Error Card bin Not Full");
                                           AVRM_writeFileLog("[AcceptCard_c()] Error Card bin Not Full",INFO);
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[AcceptCard_c()] Error Card Bin Full");
                                           AVRM_writeFileLog("[AcceptCard_c()] Error Card Bin Full",INFO);
                                           
		                        }
                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++)

                    
                      //////////////////////////////////////////////////////////////////////////////////////////////////////////

                      //Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ////////////////////////////////////////////////////////////
                      //Edited By: Abhishek Kole, 23-10-2017
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[AcceptCard_c()] Failed to send total Acknowledgement Command ");
		                    AVRM_writeFileLog("[AcceptCard_c()] Failed to send total Acknowledgement Command ",ERROR);
                            //28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                      if(1 == returnMouthFlag)
                      { 
                      	    //2 = Insert/return mouth block
                            ////writeFileLog("[AcceptCard_c()] Insert/return mouth block"); 
                            AVRM_writeFileLog("[AcceptCard_c()] Insert/return mouth block",INFO);
                            return INSERT_RETURN_MOUTH_BLOCKED; 
                      }
                      if(1 == channelBlockFlag)
                      {    
                            //1 = Channel blocked 
                            ////writeFileLog("[AcceptCard_c()] Channel blocked"); 
                            AVRM_writeFileLog("[AcceptCard_c()] Channel blocked",INFO);
                            return CHANNEL_BLOCKED;
                      }
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             ////writeFileLog("[AcceptCard_c()] Nak Reply Received");
             AVRM_writeFileLog("[AcceptCard_c()] Nak Reply Received",ERROR);
             //31 = Other Error 
             return OTHER_ERROR;  
       }
       // If Return Data is 0x04h then No need to read Data 
       else if (0x04 == rcvPkt[0])
       {
             ////writeFileLog("[AcceptCard_c()] EOT Reply Received");
             AVRM_writeFileLog("[AcceptCard_c()] EOT Reply Received",ERROR);
             //31 = Other Error 
             return OTHER_ERROR;  
       }
      

       ///////////////////////////////////////////////////////////////////////////////////////////////////
 
       ////writeFileLog("[AcceptCard_c()] All Status OK Now Going to Accept ");
       ////writeFileLog("[AcceptCard_c()] Going to send Accept Card Command ");
       AVRM_writeFileLog("[AcceptCard_c()] All Status OK Now Going to Accept ",INFO);
       AVRM_writeFileLog("[AcceptCard_c()] Going to send Accept Card Command ",INFO);
       bcc = getBcc(9,g_EnableCardAcceptance);
       g_EnableCardAcceptance[8] = bcc;

       memset(log,'\0',LOG_ARRAY_SIZE);
       sprintf(log,"[AcceptCard_c()] bcc value is 0x%xh",bcc);
       ////writeFileLog(log);
       AVRM_writeFileLog(log,INFO);

       // Before Send Command clear all serial buffer 
       ClearReceiveBuffer (g_ComHandle);
       ClearTransmitBuffer(g_ComHandle);

       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_EnableCardAcceptance,
                          9
                        );

       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       for(i=0;i<9;i++)
       {
		
                 //memset(log,'\0',LOG_ARRAY_SIZE);
                 //sprintf(log,"[AcceptCard_c()] Accept Card Command[%d] = 0x%xh",i,g_EnableCardAcceptance[i]);
                 ////writeFileLog(log);
		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_EnableCardAcceptance[i]);
                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[AcceptCard_c()] Failed to Send Accept Card Command ");
                    AVRM_writeFileLog("[AcceptCard_c()] Failed to Send Accept Card Command ",INFO);
                    return COMMUNICATION_FAILURE;
                 }

	}

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        // Now going to Check Acknowledgement 

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,INFO);

        if(0 == rtcode)
        {
                 // 28= Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle); 
                 ////writeFileLog("[AcceptCard_c()] Communication Failure");
                 AVRM_writeFileLog("[AcceptCard_c()] Communication Failure",ERROR);
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[AcceptCard_c()] Acknowledgement against Accept Card Command[0] = 0x%xh.\n",rcvPkt[0]);

                ////writeFileLog(log);
                
                AVRM_writeFileLog(log,DEBUG);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {

                 //Enable Card Entry: 
                 //Positive Reply: “P” 33H Pm st0 st1 st2
                 //Negative Reply: “N” 33H Pm e1 e0
                 ////writeFileLog("[AcceptCard_c()] Acknowledgement Received");
                 AVRM_writeFileLog("[AcceptCard_c()] Acknowledgement Received",INFO);

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',24);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                 memset(log,'\0',LOG_ARRAY_SIZE);
                 
                 sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
            
                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,INFO);

		// Now time to send Acknowledgement 
	        // Edited By: Abhishek Kole, 23-10-2017 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////

                 //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 33
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[AcceptCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,INFO);

                          ////writeFileLog("[AcceptCard_c() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[AcceptCard_c() Exit] Got 0x4e error",ERROR);

                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                ////writeFileLog("[AcceptCard_c() Exit] Forced insertion of card detected");
                                AVRM_writeFileLog("[AcceptCard_c() Exit] Forced insertion of card detected",INFO);
                          }
                          /*
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                //writeFileLog("[AcceptCard_c() Exit] Insertion/return mouth blocked");
                                return 3 ; //Insertion/return mouth blocked

                          }
                          else
                          {
                                //writeFileLog("[AcceptCard_c() Exit] Other error");

                                return 5 ; //Other error

                          }
                          
                          */
               			  SetLastError(rcvPkt[7], rcvPkt[8]);

                           ////writeFileLog("[AcceptCard_c() Exit] Other error found when issue accept card command.");
                           
                           AVRM_writeFileLog("[AcceptCard_c() Exit] Other error found when issue accept card command.",ERROR);

                           return OTHER_ERROR ; //Other error


                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x33 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[AcceptCard_c()] Enable Card Acceptance Success");
                             AVRM_writeFileLog("[AcceptCard_c()] Enable Card Acceptance Success",INFO);

                      }
                      else
                      {

                             ////writeFileLog("[AcceptCard_c() Exit] Enable Card Acceptance Failed");
                             AVRM_writeFileLog("[AcceptCard_c() Exit] Enable Card Acceptance Failed",ERROR);
                             return OTHER_ERROR;//Other Error

                      }

                  //////////////////////////////////////////////////////////////////////////////////////////////////
    

  
                 /*
                 for(i=0;i<rcvPktLen;i++)
	         {
			
                      memset(log,'\0',LOG_ARRAY_SIZE);
                 
                      sprintf(log,"[AcceptCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
            
                      //writeFileLog(log);

                 }
                 */
                  
                 
        }
        else if (0x15 == rcvPkt[0])
        {
             ////writeFileLog("[AcceptCard_c()] Nak Reply Received");
             AVRM_writeFileLog("[AcceptCard_c()] Nak Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }
        else if (0x04 == rcvPkt[0])
        {
             ////writeFileLog("[AcceptCard_c()] EOT Reply Received");
             AVRM_writeFileLog("[AcceptCard_c()] EOT Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        // Now its Time to wait for Some times to accept Card 
        // Time in Miliseconds 
        // Wait for 1 M 
        // Have to Start a thread for that and check for the card 
        // //writeFileLog("[AcceptCard_c()] Waitng for accepting a Card");
        // sleep(30); 
        //sleep(Timeout);
        
        //Start Time
        clock_gettime(CLOCK_MONOTONIC, &begints);
              
        for(;;)
	{        

                        
                         

                         presentcard=0;

                         presentcard = CheckCardPresent();

                         //////////////////////////////////////////////////////////////////////////////////////////////////////////
                        
                         /*

                         switch(presentcard)
                         {

                            case 0://no card
                                   break;

                            case 1://communication failure
                                   break;

                            case 5://NAK receieved
                                   break;

                            case 2://One Card on RF/IC Card Position
                                   break;

                            case 3://One Card in Gate
                                   break;


                         }


                         */

                         /////////////////////////////////////////////////////////////////////////////////////////////////////////

                         clock_gettime(CLOCK_MONOTONIC, &endts);

                         diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                         //printf("\n[AcceptCard_c() for Loop] Return Code: %d Time:%d Seconds",presentcard,diffts.tv_sec);

                         if( (1==presentcard)|| 
                             (5==presentcard) ) // communication failure+
                                               // NAK receieved+one card in reader position
                         {
                              //break loop 
                              ////writeFileLog("[AcceptCard_c()] Communication Failure");
                              AVRM_writeFileLog("[AcceptCard_c()] Communication Failure",ERROR);
                              return COMMUNICATION_FAILURE;

                         }
                         else if(2==presentcard) //One Card on RF/IC Card Position
                         {
                              //break loop
                              ////writeFileLog("[AcceptCard_c()] One Card on RF/IC Card Position.");
                              AVRM_writeFileLog("[AcceptCard_c()] One Card on RF/IC Card Position.",INFO);
                              return SUCCESS;

                         }
                         else if(3==presentcard) //One Card in infront of Gate
                         {
                              //break loop
                              ////writeFileLog("[AcceptCard_c()] Found One Card in infront of Gate.");
                              AVRM_writeFileLog("[AcceptCard_c()] Found One Card in infront of Gate.",INFO);
                              return INSERT_RETURN_MOUTH_BLOCKED;

                         }

                         //Check Current timer status
                         clock_gettime(CLOCK_MONOTONIC, &endts);

                         diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                         //Time out and No data received. 
			 if( diffts.tv_sec >= (Timeout/1000)  )
                         {  
    
                                 memset(log,'\0',LOG_ARRAY_SIZE);

                                 sprintf(log,"[AcceptCard_c()] Time out in %d seconds.",diffts.tv_sec);

                                 ////writeFileLog(log);
                                 
                                 AVRM_writeFileLog(log,INFO);

                                 return (DisableCardAcceptance( LogdllPathstr,
                                                                LogFileName,
                                                                deviceid,
                                                                fnLogfileMode)
                                        );

                                 break;


                         }//if( diffts.tv_sec >= (Timeout/1000)  )
                           
 
      	}//End for(;;)


        

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /*

        // Now its time To Disable Card Acceptance 

        //writeFileLog("[AcceptCard_c()] Going to Send Disable Acceptance Command");
        bcc = getBcc(9,g_DisableCardAcceptance);
        g_DisableCardAcceptance[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[AcceptCard_c()] bcc value is 0x%xh",bcc);
        //writeFileLog(log);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);
                 sprintf(log,"[AcceptCard_c()] Disable Acceptance Command[%d] = 0x%xh",i,g_DisableCardAcceptance[i]);
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_DisableCardAcceptance[i]);
                 if(0 == send_rvalue) 
                 {
                    //writeFileLog("[AcceptCard_c()] Failed to Send Report status Command ");
                    // 1 = Communication Failure 
                    return 1;
                 }
	}

        //writeFileLog("[AcceptCard_c()] Disable Acceptance Command send Successfully");
        //writeFileLog("[AcceptCard_c()] Now Going to read Acknowledgement");


        // Now going to Check Acknowledgement  
  
        totalByteToRecv = 1;

        memset(rcvPkt,'\0',11); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        //writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 return 1;
        }
        else
        {
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);
                 sprintf(log,"[AcceptCard_c()] Acknowledgement against Disable Acceptance Command[0] = 0x%xh.\n",rcvPkt[0]);
                 //writeFileLog(log);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {
                 //writeFileLog("[AcceptCard_c()] Acknowledgement Received");
                 totalByteToRecv = 12;
                 memset(rcvPkt,'\0',11);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);
                 sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
                 //writeFileLog(log);

                 for(i=0;i<rcvPktLen;i++)
	         {
		         memset(log,'\0',LOG_ARRAY_SIZE);

                         sprintf(log,"[AcceptCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                         //writeFileLog(log);

                 }
                 // 0 = Operation Successful 
                 return 0;
        }
        else if (0x15 == rcvPkt[0])
        {
             //writeFileLog("[AcceptCard_c()] Nak Reply Received");
             // 5 = Other Error 
             return 5;  
        }

        */

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}//AcceptCard_c(int Timeout) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Get rfid reader status
static int GetRFIDReaderStatus() {
       
        
        /*
        AVRM_writeFileLog("[GetRFIDReaderStatus()] Entry",TRACE);

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[20];

        memset(command,'\0',20 );

        //Construct Contact Less Card DeActivate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x60; //CM
        command[6]  = 0x32; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //STI    = 1 byte
        //STJ    = 1 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 14 Byte
        
        int totalByteToRecv=14; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[GetRFIDReaderStatus()] Command Transmit success");
                    
                    AVRM_writeFileLog("[GetRFIDReaderStatus()] Command Transmit success",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[GetRFIDReaderStatus()] ACK receieve from MUTEK");
                         
                         AVRM_writeFileLog("[GetRFIDReaderStatus()] Command Transmit success",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[GetRFIDReaderStatus()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[GetRFIDReaderStatus()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x60 == RecvPackt[5] ) &&  //CM
		                    ( 0x32 == RecvPackt[6] )     //PM
		                  )
		                {

				             ////writeFileLog("[GetRFIDReaderStatus() Exit] Successfully get deactivate status from mutek."); 
				             
				                     AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] Successfully get deactivate status from mutek.",DEBUG);                    
										 
														 if( (0x30 == RecvPackt[10]) && (0x30 == RecvPackt[11]) )
												 {
														 AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] RFID_READER_NOT_READY.",INFO);   
											  return RFID_READER_NOT_READY; //++not ready
														   
											 }
											 else if( (0x00 != RecvPackt[10]) && (0x00 != RecvPackt[11]) )
											 {
																	AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] RFID_READER_READY.",INFO);
																	  return RFID_READER_READY; //++ready state
																 }
																 
											 
								 
									  }
								
                        }
                        else if( 0x4e == RecvPackt[4] )
		              {
		                     
		                             AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] Negative ACK receieved.",DEBUG);
                                     ////writeFileLog("[GetRFIDReaderStatus() Exit] Negative ACK receieved.");                    
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetRFIDReaderStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log);      
       
                                     AVRM_writeFileLog(log,DEBUG);
                                      
                                     return (-1);
		                 
		              }//else if( 0x4e == RecvPackt[4] )
                              else
                              {
                                  ////writeFileLog("[GetRFIDReaderStatus() Exit] Reply bytes not matched .");
                                  AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] Reply bytes not matched .",DEBUG);
                                  return (-1);

                              }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     ////writeFileLog("[GetRFIDReaderStatus() Exit] Negative ACK receieved."); 
                                     AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] Negative ACK receieved.",DEBUG);                      
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetRFIDReaderStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log);     
				      
				                     AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] Negative ACK receieved.",DEBUG);      
    
                                     return (-1);
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                ////writeFileLog("[GetRFIDReaderStatus() Exit] Timeout occuered."); 
		                
		                AVRM_writeFileLog("[GetRFIDReaderStatus() Exit] Timeout occuered.",INFO);     
                          
		                return (-1);

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          ////writeFileLog("[GetRFIDReaderStatus()] Fail receieve ack byte from mutek.");
                          AVRM_writeFileLog("[GetRFIDReaderStatus()] Fail receieve ack byte from mutek.",DEBUG);     
                          return (-1);
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             ////writeFileLog("[GetRFIDReaderStatus()] Fail transmit command to mutek.");
             AVRM_writeFileLog("[GetRFIDReaderStatus()] Fail transmit command to mutek.",DEBUG); 
             return (-1);
        }//else end

		*/
		
		return RFID_READER_READY;


}//GetRFIDReaderStatus() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Get Sam reader status
static int GetSAMReaderStatus() {
       
        //++//writeFileLog("[GetSAMReaderStatus()] Entry");

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[20];

        memset(command,'\0',20 );

        //Construct Contact Less Card DeActivate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x52; //CM
        command[6]  = 0x32; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //STI    = 1 byte
        //STJ    = 1 byte [optional 1 byte for second sam slot]
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 13 Byte
        
        int totalByteToRecv=13; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[GetSAMReaderStatus()] Command Transmit success");
                    AVRM_writeFileLog("[GetSAMReaderStatus()] Command Transmit success",DEBUG); 

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[GetSAMReaderStatus()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[GetSAMReaderStatus()] ACK receieve from MUTEK",DEBUG); 

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[GetSAMReaderStatus()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[GetSAMReaderStatus()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x32 == RecvPackt[6] )     //PM
		                  )
		                {

				             
				             AVRM_writeFileLog("[GetSAMReaderStatus() Exit] Successfully get SAM status from mutek.",DEBUG);                         
				             
		                             if( 0x30 == RecvPackt[10] )
				             {
								     AVRM_writeFileLog("[GetSAMReaderStatus() Exit] SAM_READER_NOT_READY",INFO);
						  return SAM_READER_NOT_READY; //++not ready
		                               
					     }
					     else if( (0x32 == RecvPackt[10] ) || 
                                                      (0x31 == RecvPackt[10] ) )
					     {
                                   AVRM_writeFileLog("[GetSAMReaderStatus() Exit] SAM_READER_READY",INFO);
						  return SAM_READER_READY; //++ready state

					     }
					     
                              }
                              else if( 0x4e == RecvPackt[4] )
		              {
		                             AVRM_writeFileLog("[GetSAMReaderStatus() Exit] Negative reply receieved",DEBUG);
                                     ////writeFileLog("[GetSAMReaderStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetSAMReaderStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log);      
				                      
				                      AVRM_writeFileLog(log,DEBUG);
       
                                     return (-1);
		                 
		              }//if( 0x4e == RecvPackt[4] )  
                              else
                              {
                                  ////writeFileLog("[GetSAMReaderStatus() Exit] Reply bytes not matched .");
                                  AVRM_writeFileLog("[GetSAMReaderStatus() Exit] Reply bytes not matched .",DEBUG);
                                  return (-1);

                              }
                              

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     ////writeFileLog("[GetSAMReaderStatus() Exit] Negative ACK receieved."); 
                                     AVRM_writeFileLog("[GetSAMReaderStatus() Exit] Negative ACK receieved.",DEBUG);                   
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetSAMReaderStatus()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log);      
       
                                     AVRM_writeFileLog(log,DEBUG);     
                                     return (-1);
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                ////writeFileLog("[GetSAMReaderStatus() Exit] Timeout occuered."); 
                        AVRM_writeFileLog("[GetSAMReaderStatus() Exit] Timeout occuered.",WARN);       
		                return (-1);

		            }

		         }//else block
		         

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          ////writeFileLog("[GetSAMReaderStatus()] Fail receieve ack byte from mutek.");
                          AVRM_writeFileLog("[GetSAMReaderStatus()] Fail receieve ack byte from mutek.",DEBUG);
                          return (-1);
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             ////writeFileLog("[GetSAMReaderStatus()] Fail transmit command to mutek.");
             AVRM_writeFileLog("[GetSAMReaderStatus()] Fail receieve ack byte from mutek.",ERROR);
             return (-1);
        }




}//GetSAMReaderStatus() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Edited By: Abhishek Kole, 25-10-2017

//++Get Sam enable status
int isSAMEnable(  char  *LogdllPathstr,char  *LogFileName,char  *deviceid,int   fnLogfileMode) {
       
        
        SmartCardSetupLog( LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        //++//writeFileLog("[isSAMEnable()] Entry");

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[20];

        memset(command,'\0',20 );

        //Construct Contact Card Status Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x52; //CM
        command[6]  = 0x32; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //STI    = 1 byte
        //STJ    = 1 byte [optional 1 byte for second sam slot]
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 13 Byte
        
        int totalByteToRecv=13; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[isSAMEnable()] Command Transmit success");
                    AVRM_writeFileLog("[isSAMEnable()] Command Transmit success",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[isSAMEnable()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[isSAMEnable()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[isSAMEnable()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[isSAMEnable()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x32 == RecvPackt[6] )     //PM
		                  )
		                {

				             ////writeFileLog("[isSAMEnable() Exit] Successfully get SAM status from mutek."); 
				             AVRM_writeFileLog("[isSAMEnable() Exit] Successfully get SAM status from mutek.",INFO);                          
				             
		                             if( 0x30 == RecvPackt[10] )
				             {
								     AVRM_writeFileLog("[isSAMEnable() Exit] Not Ready",INFO);
						  return 0; //not ready
		                               
					     }
					     else if( (0x32 == RecvPackt[10] ) || 
                                                      (0x31 == RecvPackt[10] ) )
					     {
                           AVRM_writeFileLog("[isSAMEnable() Exit] Ready State",INFO);
						  return 1; //ready state

					     }
					     
                              }
                              else if( 0x4e == RecvPackt[4] )
		              {
		                     
                                     ////writeFileLog("[isSAMEnable() Exit] Negative reply receieved.");    
                                     AVRM_writeFileLog("[isSAMEnable() Exit] Negative reply receieved.",DEBUG);           
                                     //Log e1 and e2 error codes
				     /*
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[isSAMEnable() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     //writeFileLog(log);      
       				     */
       				     
       				     SetLastError(RecvPackt[7],RecvPackt[8]);       				     
                                     return 5;
		                 
		              }//if( 0x4e == RecvPackt[4] )  
                              else
                              {
                                  ////writeFileLog("[isSAMEnable() Exit] Reply bytes not matched .");
                                  AVRM_writeFileLog("[isSAMEnable() Exit] Reply bytes not matched .",DEBUG); 
                                  return 4;

                              }                              

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     ////writeFileLog("[isSAMEnable() Exit] Negative ACK receieved.");   
                                     AVRM_writeFileLog("[isSAMEnable() Exit] Negative ACK receieved.",DEBUG);                  
                                     //Log e1 and e2 error codes
				     /*
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[isSAMEnable()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     //writeFileLog(log);      
       
       					*/
       				     
       				     SetLastError(RecvPackt[7],RecvPackt[8]);
                                     return 5;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                ////writeFileLog("[isSAMEnable() Exit] Timeout occuered."); 
                        AVRM_writeFileLog("[isSAMEnable() Exit] Timeout occuered.",INFO);   
		                return 3;

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          ////writeFileLog("[isSAMEnable()] Fail receieve ack byte from mutek.");
                          AVRM_writeFileLog("[isSAMEnable()] Fail receieve ack byte from mutek.",DEBUG);  
                          return 2;
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             ////writeFileLog("[isSAMEnable()] Fail transmit command to mutek.");
             AVRM_writeFileLog("[isSAMEnable()] Fail transmit command to mutek.",DEBUG);
             return 2;
        }




}//isSAMEnable() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Get Stacker/Rejection/Collection Status and card count
static int GetMTKeaderStatus(char *MTKStatus) {
       
        //++//writeFileLog("[GetMTKeaderStatus()] Entry");

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[20];

        memset(command,'\0',20 );

        //Construct Contact Less Card DeActivate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x31; //CM
        command[6]  = 0x30; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //STI    = 1 byte
        //STJ    = 1 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 12 Byte
        
        int totalByteToRecv=12; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[GetMTKeaderStatus()] Command Transmit success");
                    AVRM_writeFileLog("[GetMTKeaderStatus()] Command Transmit success",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[GetMTKeaderStatus()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[GetMTKeaderStatus()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[GetMTKeaderStatus()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x31 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
		                  )
		               {

				     ////writeFileLog("[GetMTKeaderStatus() Exit] Successfully get deactivate status from mutek.");   
				                    AVRM_writeFileLog("[GetMTKeaderStatus() Exit] Successfully get deactivate status from mutek.",INFO);                        

                                     //////////////////////////////////////////////////////////////////////

                                     //stacker card status
                                     if( 0x30 == RecvPackt[8] ) // no card in mtk 571
                                     {
                                         MTKStatus[0] = STACKER_EMPTY;//++empty
                                     }  
                                     else if( 0x31 == RecvPackt[8] )//one card in chanel
                                     {
                                         MTKStatus[0] = STACKER_NEARLY_EMPTY;//++nearly empty
                                     }
                                     else if( 0x32 == RecvPackt[8] ) //one card in reader position
                                     {
                                         MTKStatus[0] = STACKER_FULL;//++full

                                     }

                                     MTKStatus[1] = 0;//card number in stacker

                                     ///////////////////////////////////////////////////////////////////////

                                     //Rejection bin
                                     if( 0x30 == RecvPackt[9] ) // no card in mtk 571
                                     {
                                         MTKStatus[2] = REJECTION_BIN_EMPTY;//empty
                                     }  
                                     else if( 0x31 == RecvPackt[9] )//one card in chanel
                                     {
                                         MTKStatus[2] = REJECTION_BIN_FULL;//full
                                     }
                                     
				     MTKStatus[3] = 0;//card number in rejection bin

                                     ////////////////////////////////////////////////////////////////////////
                                     //Collection bin status
                                     MTKStatus[4] = COLLECTION_BIN_EMPTY;//empty

                                     MTKStatus[5] = 0;//card count in collection bin

                                     //////////////////////////////////////////////////////////////////////////
             
		                     return true; 
					     
                              }
                              else if( 0x4e == RecvPackt[4] )
		              {
		                     
                                     ////writeFileLog("[GetMTKeaderStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKeaderStatus()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log);      
       
                                     AVRM_writeFileLog(log,INFO);  
                                     return false;
		                 
		              }//if( 0x4e == RecvPackt[4] )  
                              else
                              {
                                  ////writeFileLog("[GetMTKeaderStatus() Exit] Reply bytes not matched .");
                                  AVRM_writeFileLog("[GetMTKeaderStatus() Exit] Reply bytes not matched .",DEBUG);  
                                  return false;

                              }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     ////writeFileLog("[GetMTKeaderStatus() Exit] Negative reply receieved.");                           
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKeaderStatus()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log);    
                                     AVRM_writeFileLog(log,DEBUG); 
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                ////writeFileLog("[GetMTKeaderStatus() Exit] Timeout occuered."); 
                        AVRM_writeFileLog("[GetMTKeaderStatus() Exit] Timeout occuered.",INFO); 
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          ////writeFileLog("[GetMTKeaderStatus()] Fail receieve ack byte from mutek.");
                          AVRM_writeFileLog("[GetMTKeaderStatus()] Fail receieve ack byte from mutek.",DEBUG); 
                          return false;
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             ////writeFileLog("[GetMTKeaderStatus()] Fail transmit command to mutek.");
             AVRM_writeFileLog("[GetMTKeaderStatus()] Fail transmit command to mutek.",DEBUG); 
             return false;
        }


}//GetMTKeaderStatus() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Get Stacker/Rejection/Collection Status and card count
static int GetMTKSensorStatus(char *ChanelStatus,char *MTKSensorStatus) {

       
        //++//writeFileLog("[GetMTKSensorStatus()] Entry");

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[20];

        memset(command,'\0',20 );

        //Construct Contact Less Card DeActivate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x31; //CM
        command[6]  = 0x31; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //SENSOR = 10 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 24 Byte
        
        int totalByteToRecv=24; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[GetMTKSensorStatus()] Command Transmit success");
                    AVRM_writeFileLog("[GetMTKSensorStatus()] Command Transmit success",DEBUG); 
		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[GetMTKSensorStatus()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[GetMTKSensorStatus()] ACK receieve from MUTEK",DEBUG); 

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[GetMTKSensorStatus()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[GetMTKSensorStatus()] Reply Bytes receieve from MUTEK is success",DEBUG);
		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x31 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		               {

									 ////writeFileLog("[GetMTKSensorStatus() Exit] Successfully get deactivate status from mutek.");                           
                                     AVRM_writeFileLog("[GetMTKSensorStatus() Exit] Successfully get deactivate status from mutek.",DEBUG);
                                     ///////////////////////////////////////////////////////////////////////////////////////

                                     //stacker card status
                                     *ChanelStatus=0;

                                     //Soomit-29082016: Are you trying to judge forced insertion of card by sensor status? If yes, this may create problem [I ignore this sensor status]
									 
                                     //force inserted block card found
                                     if( ( 0x31 == RecvPackt[10])  &&                
                                         ( 0x31 == RecvPackt[11])  &&
                                         ( 0x31 == RecvPackt[12])  &&
                                         ( 0x31 == RecvPackt[13]) )
                                     {
                                          *ChanelStatus = 2;//Force inserted card found

                                     }
                                     else if( 0x30 == RecvPackt[7] ) // no card in mtk 571
                                     {
                                          *ChanelStatus = 0;//Clear
                                     }  
                                     else if( ( 0x31 == RecvPackt[7] )||
                                              ( 0x32 == RecvPackt[7] ) )//one card in chanel
                                     {
                                          //0x31 = One Card in Gate and 0x32= One Card on RF Card position
                                         *ChanelStatus = 1;//Blocked
                                     }
                                    
                                     

                                     ///////////////////////////////////////////////////////////////////////////////////////

                                     /*   
                                                 Binary      Hex        
                                                    1 = 1
                                                   10 = 2
                                                  100 = 4 
                                                 1000 = 8
                                                10000 = 10
                                               100000 = 20
                                              1000000 = 40
                                             10000000 = 80


                                     */

                                     *MTKSensorStatus =0;

                                     if( 0x31 == RecvPackt[10] ) // Sensor 1
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x01;
                                     }  
                                     if( 0x31 == RecvPackt[11] ) // Sensor 2
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x02;
                                     }
                                     if( 0x31 == RecvPackt[12] ) // Sensor 3
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x04;
                                     }
                                     if( 0x31 == RecvPackt[13] ) // Sensor 4
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x08;
                                     }
                                     //Sensor 5 is reserved
                                     if( 0x31 == RecvPackt[14] ) // Sensor 6
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x10;
                                     }
                                     if( 0x31 == RecvPackt[15] ) // Sensor 7
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x20;
                                     }
                                     if( 0x31 == RecvPackt[16] ) // Sensor 8
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x40;
                                     }
                                     if( 0x31 == RecvPackt[17] ) // Sensor 9
                                     {
                                         *MTKSensorStatus = *MTKSensorStatus | 0x80;
                                     }


                                     //////////////////////////////////////////////////////////////////////////
             
		                     return true; 
					     
                              }
                              else if( 0x4e == RecvPackt[4] )
		              {
		                     
                                     ////writeFileLog("[GetMTKSensorStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKSensorStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log); 
				     
				                     AVRM_writeFileLog(log,ERROR);
       
                                     return false;
		                 
		              }//if( 0x4e == RecvPackt[4] )     
                              else
                              {
                                  ////writeFileLog("[GetMTKSensorStatus() Exit] Reply bytes not matched .");
                                  AVRM_writeFileLog("[GetMTKSensorStatus() Exit] Reply bytes not matched .",ERROR);
                                  return false;

                              }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {

		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     ////writeFileLog("[GetMTKSensorStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKSensorStatus()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     ////writeFileLog(log);      
       
                                     AVRM_writeFileLog(log,ERROR);
                                     
                                     return false;

		                 
		                 }// if( 0x4e == RecvPackt[4] ) block

		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                ////writeFileLog("[GetMTKSensorStatus() Exit] Timeout occuered."); 
                        AVRM_writeFileLog("[GetMTKSensorStatus() Exit] Timeout occuered.",WARN);
		                return false;

		            }//else block

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          ////writeFileLog("[GetMTKSensorStatus()] Fail receieve ack byte from mutek.");
                          AVRM_writeFileLog("[GetMTKSensorStatus()] Fail receieve ack byte from mutek.",ERROR);
                          return false;

		    }//else block

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             ////writeFileLog("[GetMTKSensorStatus()] Fail transmit command to mutek.");
             AVRM_writeFileLog("[GetMTKSensorStatus()] Fail transmit command to mutek.",ERROR);
             return false;

        }//else block


}//GetMTKSensorStatus() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

      Byte 0: Execution status of api

      Byte 1: RFID Reader status
      Byte 2: SAM Reader status

      Byte 3: Stacker status
      Byte 4: Stacker card count

      Byte 5: Rejection bin status
      Byte 6: Card count in rejection bin

      Byte 7: chanel status
      Byte 8: chanel sensor status

      Byte 9:   collection bin status
      Byte 10 : card count in collection bin

*/

// 0: All Component 1: Reader 2: Stacker 3:Rejection Bin 4: Chanel 5:Collection Bin

static pthread_mutex_t DeviceStatusProcMutex = PTHREAD_MUTEX_INITIALIZER;

int DeviceStatus_C(  int    ComponentId , int    Timeout,char  *ReplyDeviceStatus,char  *LogdllPathstr,char  *LogFileName,char  *deviceid,int    fnLogfileMode) {

			  pthread_mutex_lock(&DeviceStatusProcMutex);	
			  		  
              SmartCardSetupLog( LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
              ResetSmartCardLastError();            
              char SingleLineLog[8096];
              memset(SingleLineLog,'\0',8096);
              char CommandHexLog[LOG_ARRAY_SIZE];
              memset(CommandHexLog,'\0',LOG_ARRAY_SIZE);
              char log[LOG_ARRAY_SIZE];
              char MTKStatus[6];
              char ChanelStatus =0x00;
              char MTKSensorStatus=0x00;	     
	          memset(MTKStatus,0,6);
              memset(log,'\0',LOG_ARRAY_SIZE);
              int rtcode=-1,counter=0;	   
              int delay = 200;   
		      AVRM_writeFileLog("[DeviceStatus_C()] Entry",TRACE);
              //++init array
              for(counter=0;counter<11;counter++)
              {
                     ReplyDeviceStatus[counter] = 0x00; 
              }//++for end
              
              //Stacker Status
              ReplyDeviceStatus[3] = 0x03; 
              //++Stacker Card Count
              ReplyDeviceStatus[4] = 0x85; 
              //++Rejection Bin Status
              //++ReplyDeviceStatus[5] = 0x00; 
              //++ReplyDeviceStatus[6] = 0x00; 
              
              if( g_ComHandle <= 0 )
              {
                    ReplyDeviceStatus[0] = COMMUNICATION_FAILURE;
					AVRM_writeFileLog("[DeviceStatus_C() Exit] Communication Failure.",ERROR);
					return COMMUNICATION_FAILURE;
              }//++communication failure     
              
			  memset(log,'\0',LOG_ARRAY_SIZE);
			  sprintf(log,"[DeviceStatus_C()] Device Handle = %d ",g_ComHandle);
			  AVRM_writeFileLog(log,DEBUG) ;   
			  
			  memset(log,'\0',LOG_ARRAY_SIZE);
			  sprintf(log,"[DeviceStatus_C()] Device ID = %d ",deviceid);
			  AVRM_writeFileLog(log,DEBUG) ;     
               
               /*
               
              //++byte 0
              ReplyDeviceStatus[0] = SUCCESS; //SUCCESS 
			  
              //++Force Card detection
              //++Card Block Status Found
              delay_mSec(delay);
              rtcode= CardBlockStatusRequest ( LogdllPathstr,LogFileName,deviceid,fnLogfileMode);
			 
			  //memset(log,'\0',LOG_ARRAY_SIZE);
			  //sprintf(log,"[DeviceStatus_C()] CardBlockStatusRequest rtcode = %d",rtcode);
			  //writeFileLog(log);
			 
              //++delay_mSec(delay);
              //++GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);
              //sprintf(buff,"[DeviceStatus_C() MTKSensorStatus = 0x%xh",MTKSensorStatus);
	          //writeFileLog(buff);
	          
              switch(rtcode)
              {
					case 1:  //++Forced insertion
						ReplyDeviceStatus[7] = 0x03; //Forced insert			
						break;

				  case 2:  //++Card jam
					    ReplyDeviceStatus[7] = 0x04; //Card jam
                        break;
 
                case 3:  //++Other error
						ReplyDeviceStatus[0] = OTHER_ERROR; //Other error
                        break;

                case 4:  //++Communication failure
						ReplyDeviceStatus[0] = COMMUNICATION_FAILURE; //Communication failure
                        return ;

                case 5:  //++No card in channel
						ReplyDeviceStatus[7] = 0x00; //Channel clear
                        break;

                case 6:  //++One card at gate
						ReplyDeviceStatus[7] = 0x02; //Card at gate
                        break;

                case 7:  //++One card at RF/IC
						ReplyDeviceStatus[7] = 0x01; //Card at RF/IC
                        break;

					default:  
								break;
              }
              
              //++byte 8 Chanel Sensor status
              ReplyDeviceStatus[8] = MTKSensorStatus;
              * /

             /*
              if( (1 ==  rtcode) || ( -1 == rtcode ) || ( 2 == rtcode ))
              {

                       ReplyDeviceStatus[0] = 0; //Force Card detection error
					  
					  GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);

					   if(  1 == (MTKSensorStatus & 0x01) )
					   {
                              ReplyDeviceStatus[7] = 0x02; //Card at gate
					   }
					   else if((  1 == (MTKSensorStatus & 0x04) ) && (  1 == (MTKSensorStatus & 0x08)) && (  1 == (MTKSensorStatus & 0x10)))  //If sensors 3,4,5 blocked then card must be in IC or RF position
					   {
                              ReplyDeviceStatus[7] = 0x01; //Card at RF position
					   }
					   else if((  1 == (MTKSensorStatus & 0x02) ) && (  1 == (MTKSensorStatus & 0x04)) && (  1 == (MTKSensorStatus & 0x08)))  //If sensors 2,3,4 blocked then card must be in IC or RF position
					   {
                              ReplyDeviceStatus[7] = 0x01; //Card at RF position
					   }
					   
                       //Byte 7: chanel status
                       if(1 ==  rtcode ) //Force Card Entry
                       {
                              ReplyDeviceStatus[7] = 0x03; //Forced insert
                              SetLastError(0x31,0x36);
                       }
                       else if(2 ==  rtcode ) //Jammed Card
                       {
                              ReplyDeviceStatus[7] = 0x04; //Card jam
                              SetLastError(0x31,0x30);
                       }


                       //Byte 8: chanel sensor status
					  ReplyDeviceStatus[8] = MTKSensorStatus;

                       memset(log,'\0',LOG_ARRAY_SIZE);

                       sprintf(log,"[DeviceStatus_C()] ChanelStatus = 0x%xh. Sensor Status=0x%xh.",ChanelStatus,MTKSensorStatus);

                       //writeFileLog(log);

                       strcpy(SingleLineLog,"[DeviceStatus_C()] ReplyByte:");

					   for(counter=0;counter<11;counter++)
					   {

							  sprintf(CommandHexLog,"%x ",ReplyDeviceStatus[counter]);

							  strcat(SingleLineLog,CommandHexLog);

									 
					   }//for loop

					  //writeFileLog(SingleLineLog);

                       //writeFileLog("[DeviceStatus_C()] Exit.");

                       return 1; 
                       
              }//if( (1 ==  rtcode) || ( -1 == rtcode ) ) end
              else
              {
              		GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);

              		//Byte 8: chanel sensor status
              		ReplyDeviceStatus[8] = MTKSensorStatus;
              }

	          //Byte 0: Execution status of api
	         ReplyDeviceStatus[0] = 0; //operation successfull
             */
             
			  //++Byte 1: RFID Reader status
              if( (0 == ComponentId) || (1 == ComponentId) )
              {
					rtcode =-1;
					delay_mSec(delay);
                    rtcode = GetRFIDReaderStatus();
                    if( (0 != rtcode) || (1 !=rtcode) )
                    {
                       ReplyDeviceStatus[1] = 0;
                    }
			  }
              else
              {
                  ReplyDeviceStatus[1] = 0;
              }
              
			  //++Byte 2: SAM Reader status
              if( (0 == ComponentId) || (1 == ComponentId) )
              {
					rtcode =-1;
					delay_mSec(delay);
					rtcode = GetSAMReaderStatus();
                    if( (0 != rtcode) || (1 != rtcode) )
                    {
                       ReplyDeviceStatus[2]  = 0;
                    }
              }
              else
              {
					ReplyDeviceStatus[2] = 0;
              }

              if( (0 == ComponentId) || (2 == ComponentId) || (3 == ComponentId) || (5 == ComponentId) )
              {
				   delay_mSec(delay);
                   GetMTKeaderStatus(MTKStatus);
              }//++if end
              
	          //++Byte 3: Stacker status 
	          //++Byte 4: Stacker card count
              if( (0 == ComponentId) || (2 == ComponentId) ) //2 Stacker
              {
	              ReplyDeviceStatus[3] =MTKStatus[0];
                  ReplyDeviceStatus[4] =MTKStatus[1];
              }
              else
              {
	              ReplyDeviceStatus[3] = 0 ;
                  ReplyDeviceStatus[4] = 0 ;
              }//++else end
              
	         //++Byte 5: Rejection bin status 
	         //++Byte 6: Rejection Bin card count
              if( (0 == ComponentId) || (3 == ComponentId) ) // 3 Rejection Bin
	         {
	              ReplyDeviceStatus[5] =MTKStatus[2];
                  ReplyDeviceStatus[6] =MTKStatus[3];
              }
              else
              {
                  ReplyDeviceStatus[5] = 0 ;
                  ReplyDeviceStatus[6] = 0 ;
              }//++else end
              
              //++Byte 7: Chanel status 
	          //++Byte 8: Chanel Sensor Status
              if(0 == ComponentId) 
              {
				  delay_mSec(delay);
				  GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);
				  memset(log,'\0',LOG_ARRAY_SIZE);
				  sprintf(log,"[DeviceStatus_C()] ChanelStatus = 0x%xh. Sensor Status=0x%xh.",ChanelStatus,MTKSensorStatus);
				  AVRM_writeFileLog(log,DEBUG);
				  //++Byte 7: chanel status
				  ReplyDeviceStatus[7] = ChanelStatus;
				  //++Byte 8: chanel sensor status
				  ReplyDeviceStatus[8] = MTKSensorStatus;
              } 
              else 
              {
                      //Byte 7: chanel status
					  ReplyDeviceStatus[7] = 0x00;
					  //Byte 8: chanel sensor status
					  ReplyDeviceStatus[8] = 0x00;
               }//else end

	          //++Byte 9:   Collection bin status  
			  //++Byte 10 : Collection Bin card count
              if( (0 == ComponentId) || (5 == ComponentId) ) // 5 Collection Bin
              {
	               ReplyDeviceStatus[9] =MTKStatus[4];
                   ReplyDeviceStatus[10] =MTKStatus[5];
			 }
             else
             {
                   ReplyDeviceStatus[9]  = 0;
                   ReplyDeviceStatus[10] = 0;
             }//++else end
             
             //++Write Device Status Log
             strcpy(SingleLineLog,"[DeviceStatus_C()] ReplyByte:");
             for(counter=0;counter<11;counter++)
	         {	
				 sprintf(CommandHexLog,"%x ",ReplyDeviceStatus[counter]);
				 strcat(SingleLineLog,CommandHexLog);            
             }//++for loop
			 AVRM_writeFileLog(SingleLineLog,INFO);
			 
			 AVRM_writeFileLog("[DeviceStatus_C()] Exit",TRACE);
			 pthread_mutex_unlock(&DeviceStatusProcMutex);
             return 1;
             
}//++int DeviceStatus_C(int ComponentId , int Timeout,char *ReplyDeviceStatus)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function Name : DisableCardAcceptance_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Operation timeout occurred
//                  3 = Other Error 

// Parameters    : Name                           Type          Description 

                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

// To Disable Card Acceptance 

int DisableCardAcceptance_c( int   Timeout, char  *LogdllPathstr,char  *LogFileName,char  *deviceid,int   fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);


        ////////////////////////////////////////////////////////////////////////////////////

        g_DisableCardAcceptance[0]= 0xF2 ;
        g_DisableCardAcceptance[1]= MTK_DEVICE_ADDRESS ;
        g_DisableCardAcceptance[2]= 0x00 ;
        g_DisableCardAcceptance[3]= 0x03 ;
        g_DisableCardAcceptance[4]= 0x43 ;
        g_DisableCardAcceptance[5]= 0x33 ;
        g_DisableCardAcceptance[6]= 0x31 ;
        g_DisableCardAcceptance[7]= 0x03 ;
        g_DisableCardAcceptance[8]= 0x00 ;

        ////////////////////////////////////////////////////////////////////////////////////

        char bcc=0x00;
        unsigned char rcvPkt[11];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,
        returnMouthFlag = 0,channelBlockFlag = 0;
        unsigned int rcvPktLen =0x00; 
        AVRM_writeFileLog("[DisableCardAcceptance_c()] Going to Send Disable Acceptance Command",INFO);        
	    // bcc 
        bcc = getBcc(9,g_DisableCardAcceptance);
        g_DisableCardAcceptance[8] = bcc;     
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[DisableCardAcceptance_c()] bcc value is 0x%xh",bcc);
        
        AVRM_writeFileLog(log,DEBUG);

		delay_mSec(g_MaxDelay);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_DisableCardAcceptance,
                          9
                        );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for(i=0;i<9;i++)
	{
		 /*
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance_c()] Disable Acceptance Command[%d] = 0x%xh",i,g_DisableCardAcceptance[i]);
        
                 //writeFileLog(log);
                 */
		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_DisableCardAcceptance[i]);
                 
                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[DisableCardAcceptance_c()] Failed to Send Report status Command .");
                    AVRM_writeFileLog("[DisableCardAcceptance_c()] Failed to Send Report status Command .",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }//if(0 == send_rvalue) 

	}//for(i=0;i<9;i++)

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ////writeFileLog("[DisableCardAcceptance_c()] Disable Acceptance Command send Successfully.");
        ////writeFileLog("[DisableCardAcceptance_c()] Now Going to read Acknowledgement.");
        
        //AVRM_writeFileLog("[DisableCardAcceptance_c()] Disable Acceptance Command send Successfully.",INFO);
        //AVRM_writeFileLog("[DisableCardAcceptance_c()] Now Going to read Acknowledgement.",INFO);


        // Now going to Check Acknowledgement
  
        totalByteToRecv = 1;
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[DisableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);

                 ClearTransmitBuffer(g_ComHandle);

                 ////writeFileLog("[DisableCardAcceptance_c()] Communication Failure when read ack bytes.");
                 
                 AVRM_writeFileLog("[DisableCardAcceptance_c()] Communication Failure when read ack bytes.",ERROR);
                 
                 return COMMUNICATION_FAILURE;

        }
        else
        {
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance_c()] Acknowledgement against Disable Acceptance Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);
        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {

                 ////writeFileLog("[DisableCardAcceptance_c()] Acknowledgement Received");
                 
                 AVRM_writeFileLog("[DisableCardAcceptance_c()] Acknowledgement Received",DEBUG);

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',11);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance_c()] Receive packet status = %d and Length = %d.",rtcode,rcvPktLen);
        
                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);
                 
                 // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      // Edited By: Abhishek Kole, 18-10-2017
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////

                  //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 33
		      //PM   (1 byte) : 31
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[DisableCardAcceptance_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,ERROR);

                          ////writeFileLog("[DisableCardAcceptance_c() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[DisableCardAcceptance_c() Exit] Got 0x4e error",ERROR);

                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                ////writeFileLog("[DisableCardAcceptance_c() Exit] Forced insertion of card detected");
                                AVRM_writeFileLog("[DisableCardAcceptance_c() Exit] Forced insertion of card detected",INFO);
                          }

                          /*
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                //writeFileLog("[DisableCardAcceptance_c() Exit] Insertion/return mouth blocked.");
                          }
                          else
                          {
                                //writeFileLog("[DisableCardAcceptance_c() Exit] Other error");
                                

                          }

                          */
                          
  						  SetLastError(rcvPkt[7], rcvPkt[8]);

                          ////writeFileLog("[DisableCardAcceptance_c() Exit] Other error found when issue device status command.");
                          AVRM_writeFileLog("[DisableCardAcceptance_c() Exit] Other error found when issue device status command.",ERROR);
                          return OTHER_ERROR ; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x33 == rcvPkt[5] ) && ( 0x31 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[DisableCardAcceptance_c()] Disable Card Acceptance Success.");
                             AVRM_writeFileLog("[DisableCardAcceptance_c()] Disable Card Acceptance Success.",INFO);
                             // 0 = Operation Successful 
                             return SUCCESS;

                      }
                      else
                      {
                             AVRM_writeFileLog("[DisableCardAcceptance_c() Exit] Disable Card Acceptance Failed.",ERROR);
                             ////writeFileLog("[DisableCardAcceptance_c() Exit] Disable Card Acceptance Failed.");
                             return OTHER_ERROR;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
	    
		         /*
		         for(i=0;i<rcvPktLen;i++)
			 {
				
		               memset(log,'\0',LOG_ARRAY_SIZE);

		               sprintf(log,"[DisableCardAcceptance_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
		
		               //writeFileLog(log);

		         }
		         */
                
        }
        else if (0x15 == rcvPkt[0])
        {
             ////writeFileLog("[DisableCardAcceptance_c()] Nak Reply Received");
             AVRM_writeFileLog("[DisableCardAcceptance_c()] Nak Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }
        else if (0x04 == rcvPkt[0])
        {
             ////writeFileLog("[DisableCardAcceptance_c()] EOT Received");
             // 31 = Other Error 
             return OTHER_ERROR;  
        }

}//int DisableCardAcceptance_c(int Timeout) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function Name : DisConnectDevice_c 
// Return Type   : int 
//                  0 = DisConnected Successfully
//                  1 = DisConnected Successfully but a Card in the Channel
//                  2 = Communication Failure
//                  3 = Operation timeout occurred
//                  4 = Other Error 

// Parameters    : Name                           Type          Description 

                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

// To get Current Report status of st0, st1, st2


//static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};

int DisConnectDevice_c( int   Timeout,char  *LogdllPathstr,char  *LogFileName,char  *deviceid,int   fnLogfileMode) {

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        ////writeFileLog("[DisConnectDevice_c()] Entry");
        AVRM_writeFileLog("[DisConnectDevice_c()] Entry",TRACE);

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);
        
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[DisConnectDevice_c()] ComPortHandle: %d . ",g_ComHandle);
        ////writeFileLog(log);
        AVRM_writeFileLog(log,INFO);
        
        g_getStatus[0] = 0xF2;
		g_getStatus[1] = MTK_DEVICE_ADDRESS;
		g_getStatus[2]=  0x00;
		g_getStatus[3]=  0x03;
		g_getStatus[4]=  0x43;
		g_getStatus[5]=  0x31;
		g_getStatus[6]=  0x30;
		g_getStatus[7]=  0x03;
		g_getStatus[8]=  0x00;
        char bcc=0x00;

        unsigned char rcvPkt[25];

        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,oneCradInChannel = 0,closePortStatus = 0;

        unsigned int rcvPktLen =0x00; 

        // If there was any card block or not 

        AVRM_writeFileLog("[DisConnectDevice_c()] Going to send Current Report status of st0, st1, st2",INFO);
        
        g_getStatus[8] = 0x00;
        
        bcc = getBcc(9,g_getStatus);

        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisConnectDevice_c()] bcc value is 0x%xh",bcc);
        
        AVRM_writeFileLog(log,INFO);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		delay_mSec(g_MaxDelay);
		
        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[DisConnectDevice_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[DisConnectDevice_c()] Failed to Send Report status Command ");
                    AVRM_writeFileLog("[DisConnectDevice_c()] Failed to Send Report status Command ",INFO);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 
                 }// if(0 == send_rvalue) 


	}//for(i=0;i<9;i++)


        ///////////////////////////////////////////////////////////////////////////////////////////

       
        //AVRM_writeFileLog("[DisConnectDevice_c()] Status Command send Successfully",INFO); 
        //AVRM_writeFileLog("[DisConnectDevice_c()] Now Going to read Acknowledgement",INFO);

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;

        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,INFO);

        if(0 == rtcode)
        {
                 //++28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[DisConnectDevice_c()] Communication Failure when read ack bytes.");
                 AVRM_writeFileLog("[DisConnectDevice_c()] Communication Failure when read ack bytes.",INFO);
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                 
              memset(log,'\0',LOG_ARRAY_SIZE);

              sprintf(log,"[DisConnectDevice_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
              ////writeFileLog(log);
              
              AVRM_writeFileLog(log,INFO);

        }

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      ////writeFileLog("[DisConnectDevice_c()] Acknowledgement Received");
                      AVRM_writeFileLog("[DisConnectDevice_c()] Acknowledgement Received",INFO);
                      
                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[DisConnectDevice_c()] statusRead() return code = %d and receieve packet Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);
                      
                      AVRM_writeFileLog(log,INFO);
                      
                      ///////////////////////////////////////////////////////////////
                      
                      //Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      //Log ACK byte upon receieve complete data from mutek
                      // Edited By: Abhishek Kole, 18-10-2017                      
                      char ack_Data[1]={0x06};
                      
                      if(1 == send_rvalue) {
                      
		              SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
					       g_SCRDDeviceid,
					       "Tx",
					       ack_Data,
					       1
		                             );
                      
                      }
                      
                      ///////////////////////////////////////////////////////////////////
                      

                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[DisConnectDevice_c()] Communication Failure when read getstatus reply bytes.");
                                 AVRM_writeFileLog("[DisConnectDevice_c()] Communication Failure when read getstatus reply bytes.",INFO);
				 return COMMUNICATION_FAILURE;

		      }//if(0 == rtcode)

                      //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 31
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[DisConnectDevice_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,INFO);

                          ////writeFileLog("[DisConnectDevice_c() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[DisConnectDevice_c() Exit] Got 0x4e error",INFO);
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                ////writeFileLog("[DisConnectDevice_c() Exit] Insertion/return mouth blocked");
                                AVRM_writeFileLog("[DisConnectDevice_c() Exit] Insertion/return mouth blocked",INFO);
                          }
                          else
                          {
                                ////writeFileLog("[DisConnectDevice_c() Exit] Other error");
                                AVRM_writeFileLog("[DisConnectDevice_c() Exit] Other error",ERROR);
                                

                          }
   						  SetLastError(rcvPkt[7], rcvPkt[8]);
   						  
   						  //Patch for force disconnect  on 23 oct 2020
   						  AVRM_writeFileLog("[DisConnectDevice_c() Exit] Other error 4e go to Patch close posrt",INFO);
   						  if(1 == ClosePort(g_ComHandle) ){
							  AVRM_writeFileLog("[DisConnectDevice_c() Exit] Port Close Success",INFO);
					      }else{
							  AVRM_writeFileLog("[DisConnectDevice_c() Exit] Port Close Failed",INFO);
					      }//else end	  		
                          g_ComHandle = -1;
                          return OTHER_ERROR ; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {
  
                             // Success: Rx[12]:f2 0 0 6 50 31 30 32 31 30 3 95
                             ////writeFileLog("[DisConnectDevice_c()] Get Status Command issue success.");
                             AVRM_writeFileLog("[DisConnectDevice_c()] Get Status Command issue success.",ERROR);
                             
                      }
                      else
                      {

                             ////writeFileLog("[DisConnectDevice_c() Exit] Get Status Command issue failed.");
                             AVRM_writeFileLog("[DisConnectDevice_c() Exit] Get Status Command issue failed.",ERROR);
                             return OTHER_ERROR;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[DisConnectDevice_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
										if(0x30 == rcvPkt[i])
										{
												   ////writeFileLog("[DisConnectDevice_c()] No Card in MTK-571");
												   AVRM_writeFileLog("[DisConnectDevice_c()] No Card in MTK-571",INFO);
													 
										}
												else if(0x31 == rcvPkt[i])
										{
												   ////writeFileLog("[DisConnectDevice_c()] One Card in Gate");
												   AVRM_writeFileLog("[DisConnectDevice_c()] One Card in Gate",INFO);
													
										}
												else if(0x32 == rcvPkt[i])
										{
												   ////writeFileLog("[DisConnectDevice_c()] One Card on RF/IC Card Position");
												   AVRM_writeFileLog("[DisConnectDevice_c()] One Card on RF/IC Card Position",INFO);
												   // 1 = Card found in the Channel 
												   oneCradInChannel = 1;  
										  
										} 


                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[DisConnectDevice_c()] No Card in Stacker");
											   AVRM_writeFileLog("[DisConnectDevice_c()] No Card in Stacker",INFO);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[DisConnectDevice_c()] Few Card in Stacker");
											   AVRM_writeFileLog("[DisConnectDevice_c()] Few Card in Stacker",INFO);
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[DisConnectDevice_c()] Enough Card in the Box");
											   AVRM_writeFileLog("[DisConnectDevice_c()] Enough Card in the Box",INFO);
									}   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[DisConnectDevice_c()] Error Card bin Not Full");
											   AVRM_writeFileLog("[DisConnectDevice_c()] Error Card bin Not Full",ERROR);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[DisConnectDevice_c()] Error Card Bin Full");
											   AVRM_writeFileLog("[DisConnectDevice_c()] Error Card Bin Full",ERROR);
											   
									}
                                }
                                
				
	              }
	              
                      
                      //////////////////////////////////////////////////////////////
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[DisConnectDevice_c()] Failed to send total Acknowledgement Command ");
		                     AVRM_writeFileLog("[DisConnectDevice_c()] Failed to send total Acknowledgement Command",ERROR);
                            // 28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                       
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             ////writeFileLog("[DisConnectDevice_c() Exit] Nak Reply Received");
             AVRM_writeFileLog("[DisConnectDevice_c() Exit] Nak Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
       }
       else if (0x04 == rcvPkt[0])
       {
             ////writeFileLog("[DisConnectDevice_c() Exit] EOT Reply Received");
             AVRM_writeFileLog("[DisConnectDevice_c() Exit] EOT Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
       }

       // Status Read Successfull Now Going to Close the Serial Port 
       
       closePortStatus = ClosePort(g_ComHandle);
       
       // Port close and going to return 
       if(1 == closePortStatus)    
       {

               ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

               g_ComHandle =-1;

               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

               // Port Close Successfull 
               if(1 == oneCradInChannel)
               {
                      // One Card in Channel 
                      // 1 = DisConnected Successfully but a Card in the Channel 
                      ////writeFileLog("[DisConnectDevice_c() Exit] DisConnected Successfully but a Card in the Channel.");
                      AVRM_writeFileLog("[DisConnectDevice_c() Exit] DisConnected Successfully but a Card in the Channel.",INFO);
                      return DISCONNECT_SUCCESS_BUT_ONE_CARD_IN_CHANNEL;
               }
               else if(0 == oneCradInChannel)
               {
                      // No Card in Channel 
                      // 0 = DisConnected Successfully 
                      ////writeFileLog("[DisConnectDevice_c() Exit] DisConnected Successfully and no Card in the Channel.");
                      AVRM_writeFileLog("[DisConnectDevice_c() Exit] DisConnected Successfully and no Card in the Channel.",INFO);
                      return SUCCESS;
               }
       }
       else if(0 == closePortStatus)
       {
           // Port Close UnSuccessfull 
           // 28 = Communication Failure 
           ////writeFileLog("[DisConnectDevice_c() Exit] DisConnected failed with Communication Failure .");
           AVRM_writeFileLog("[DisConnectDevice_c() Exit] DisConnected failed with Communication Failure .",INFO);
           return COMMUNICATION_FAILURE;

       }
       else 
       {
           // 31 = Other Error 
           ////writeFileLog("[DisConnectDevice_c() Exit] DisConnected failed with other error .");
           AVRM_writeFileLog("[DisConnectDevice_c() Exit] DisConnected failed with other error .",INFO);
           return OTHER_ERROR;
       }//else
 

}//int DisConnectDevice_c(int Timeout) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function Name : EnableCardAcceptance_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Channel blocked 
//                  3 = Insert/return mouth block
//                  4 = Operation timeout occurred
//                  5 = Other Error 
// Parameters    : Name                           Type          Description 
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 	operation otherwise return timeout status. 
// To get Current Report status of st0, st1, st2
//static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};
// To Enable Card Acceptance 
//static unsigned char g_EnableCardAcceptance[9]={0xF2,0x00,0x00,0x03,0x43,0x33,0x30,0x03,0x00};

int EnableCardAcceptance_c( int   Timeout, char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        g_getStatus[0] = 0xF2;
        g_getStatus[1] = MTK_DEVICE_ADDRESS;
        g_getStatus[2] = 0x00;
        g_getStatus[3] = 0x03;
        g_getStatus[4] = 0x43;
        g_getStatus[5] = 0x31;
        g_getStatus[6] = 0x30;
        g_getStatus[7] = 0x03;
        g_getStatus[8] = 0x00;
        ///////////////////////////////////////////////////////////////////////////////////
        g_EnableCardAcceptance[0]= 0xF2;
        g_EnableCardAcceptance[1]= MTK_DEVICE_ADDRESS;
        g_EnableCardAcceptance[2]= 0x00;
        g_EnableCardAcceptance[3]= 0x03;
        g_EnableCardAcceptance[4]= 0x43;
        g_EnableCardAcceptance[5]= 0x33;
        g_EnableCardAcceptance[6]= 0x30;
        g_EnableCardAcceptance[7]= 0x03;
        g_EnableCardAcceptance[8]= 0x00;

        ///////////////////////////////////////////////////////////////////////////////////


        char bcc=0x00;

        unsigned char rcvPkt[25];

        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,
                    returnMouthFlag = 0,channelBlockFlag = 0;
        unsigned int rcvPktLen =0x00; 

        ////writeFileLog("[EnableCardAcceptance_c()] Going to send Current Report status of st0, st1, st2");
        
        AVRM_writeFileLog("[EnableCardAcceptance_c()] Going to send Current Report status of st0, st1, st2",INFO);
        
        g_getStatus[8] =0x00;
        
        bcc = getBcc(9,g_getStatus);

        g_getStatus[8] = bcc;
       
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[EnableCardAcceptance_c()] bcc value is 0x%xh",bcc);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,INFO);
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[EnableCardAcceptance_c()] ComPortHandle= %d",g_ComHandle);
        ////writeFileLog(log);
        AVRM_writeFileLog(log,INFO);
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		
		delay_mSec(g_MaxDelay);
		
        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[EnableCardAcceptance_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 ////writeFileLog(log);

				send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[EnableCardAcceptance_c()] Failed to Send Report status Command .");
                    AVRM_writeFileLog("[EnableCardAcceptance_c()] Failed to Send Report status Command .",INFO);
                    return COMMUNICATION_FAILURE;

                 }//if(0 == send_rvalue) 


	}//for(i=0;i<9;i++)

        ////////////////////////////////////////////////////////////////////////////////////////

        
        //++AVRM_writeFileLog("[EnableCardAcceptance_c()] Status Command send Successfully .",INFO);
        //++AVRM_writeFileLog("[EnableCardAcceptance_c()] Now Going to read Acknowledgement .",INFO);
   
        // Now going to Check Acknowledgement  
        totalByteToRecv = 1;        
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);      
        AVRM_writeFileLog(log,INFO);

        if(0 == rtcode)
        {
                 //28 = Communication Failure 
                 ClearReceiveBuffer(g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[EnableCardAcceptance_c()] Failed to read ack reply byte ");
                 AVRM_writeFileLog("[EnableCardAcceptance_c()] Failed to read ack reply byte ",ERROR);
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[EnableCardAcceptance_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                ////writeFileLog(log);
                
                AVRM_writeFileLog(log,INFO);

        }

        //If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {
                      ////writeFileLog("[EnableCardAcceptance_c()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[EnableCardAcceptance_c()] Acknowledgement Received",INFO);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);
                      
                      AVRM_writeFileLog(log,INFO);

                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[EnableCardAcceptance_c()] Failed to read status command reply bytes.");
                                 AVRM_writeFileLog(log,INFO);
				 return COMMUNICATION_FAILURE;
		      }

                      //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 31
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[EnableCardAcceptance_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,ERROR);

                          ////writeFileLog("[EnableCardAcceptance_c() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[EnableCardAcceptance_c() Exit] Got 0x4e error",INFO);
                          
   			  SetLastError(rcvPkt[7], rcvPkt[8]);
			  
                          return OTHER_ERROR; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[EnableCardAcceptance_c()] Get Status Command issue success.");
                             AVRM_writeFileLog("[EnableCardAcceptance_c()] Get Status Command issue success.",INFO);
                             
                      }
                      else
                      {

                             ////writeFileLog("[EnableCardAcceptance_c() Exit] Get Status Command issue failed.");
                             AVRM_writeFileLog("[EnableCardAcceptance_c() Exit] Get Status Command issue failed.",ERROR);
                             return OTHER_ERROR;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
                     
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[EnableCardAcceptance_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);


                                // Card Status Code st0 
                                if(7 == i)
                                {
										if(0x30 == rcvPkt[i])
										{
												   ////writeFileLog("[EnableCardAcceptance_c()] No Card in MTK-571");
												   AVRM_writeFileLog("[EnableCardAcceptance_c()] No Card in MTK-571",INFO);
										}
												else if(0x31 == rcvPkt[i])
										{
												   ////writeFileLog("[EnableCardAcceptance_c()] One Card in Gate");
												   AVRM_writeFileLog("[EnableCardAcceptance_c()] One Card in Gate",INFO);
												   // 3 = Insert/return mouth block 
												   returnMouthFlag = 1; 
										}
												else if(0x32 == rcvPkt[i])
										{
												   ////writeFileLog("[EnableCardAcceptance_c()] One Card on RF/IC Card Position");
												   AVRM_writeFileLog("[EnableCardAcceptance_c()] One Card on RF/IC Card Position",INFO);
												   // 2 = Channel blocked 
												   channelBlockFlag = 1;
										  
										} 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[EnableCardAcceptance_c()] No Card in Stacker");
											   AVRM_writeFileLog("[EnableCardAcceptance_c()] One Card on RF/IC Card Position",INFO);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[EnableCardAcceptance_c()] Few Card in Stacker");
											   AVRM_writeFileLog("[EnableCardAcceptance_c()] Few Card in Stacker",INFO);
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[EnableCardAcceptance_c()] Enough Card in the Box");
											   AVRM_writeFileLog("[EnableCardAcceptance_c()] Enough Card in the Box",INFO);
									}   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[EnableCardAcceptance_c()] Error Card bin Not Full");
											   AVRM_writeFileLog("[EnableCardAcceptance_c()] Error Card bin Not Full",ERROR);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[EnableCardAcceptance_c()] Error Card Bin Full");
											   AVRM_writeFileLog("[EnableCardAcceptance_c()] Error Card Bin Full",ERROR);
											   
									}
                                }
                                
				
	              }
                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      // Edited By: Abhishek Kole, 18-10-2017                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                             
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[EnableCardAcceptance_c()] Failed to send total Acknowledgement Command ");
		                     AVRM_writeFileLog("[EnableCardAcceptance_c()] Failed to send total Acknowledgement Command ",ERROR);
                            // 28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                      if(1 == returnMouthFlag)
                      { 
                      	    // 2= Insert/return mouth block 
                            ////writeFileLog("[EnableCardAcceptance_c()] Insert/return mouth block ");
                            AVRM_writeFileLog("[EnableCardAcceptance_c()] Insert/return mouth block ",INFO);
                            return INSERT_RETURN_MOUTH_BLOCKED; 
                      }
                      if(1 == channelBlockFlag)
                      {    
                            //  1 = Channel blocked 
                            ////writeFileLog("[EnableCardAcceptance_c()] Channel blocked  ");
                            AVRM_writeFileLog("[EnableCardAcceptance_c()] Channel blocked  ",INFO);
                            return CHANNEL_BLOCKED;
                      }
       }
       //If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             ////writeFileLog("[EnableCardAcceptance_c()] Nak Reply Received");
             AVRM_writeFileLog("[EnableCardAcceptance_c()] Nak Reply Received  ",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
       }
       //If Return Data is 15h then No need to read Data 
       else if (0x04 == rcvPkt[0])
       {
             ////writeFileLog("[EnableCardAcceptance_c()] EOT Reply Received");
             AVRM_writeFileLog("[EnableCardAcceptance_c()] EOT Reply Received ",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
       }

       ///////////////////////////////////////////////////////////////////////////////////////////////

       AVRM_writeFileLog("[EnableCardAcceptance_c()] All Status OK Now Going to Accept ",INFO);
       AVRM_writeFileLog("[EnableCardAcceptance_c()] All Status OK Now Going to Accept ",DEBUG);

       g_EnableCardAcceptance[8] =0x00;
       
       bcc = getBcc(9,g_EnableCardAcceptance);
     
       g_EnableCardAcceptance[8] = bcc;
      
       memset(log,'\0',LOG_ARRAY_SIZE);

       sprintf(log,"[EnableCardAcceptance_c()] bcc value is 0x%xh",bcc);
        
       ////writeFileLog(log);
      
       AVRM_writeFileLog(log,DEBUG);
       
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_EnableCardAcceptance,
                          9
                        );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


       // Before Send Command clear all serial buffer 
       ClearReceiveBuffer (g_ComHandle);

       ClearTransmitBuffer(g_ComHandle);

       for(i=0;i<9;i++)
       {
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[EnableCardAcceptance_c()] Accept Card Command[%d] = 0x%xh",i,g_EnableCardAcceptance[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_EnableCardAcceptance[i]);
                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[EnableCardAcceptance_c()] Failed to Send Accept Card Command ");
                    AVRM_writeFileLog("[EnableCardAcceptance_c()] Failed to Send Accept Card Command ",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }
	}

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // Now going to Check Acknowledgement 
        totalByteToRecv = 1;
        
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[EnableCardAcceptance_c()] Unable to read enable command reply bytes");
                 AVRM_writeFileLog("[EnableCardAcceptance_c()] Unable to read enable command reply bytes",DEBUG);
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                 
				memset(log,'\0',LOG_ARRAY_SIZE);

				sprintf(log,"[EnableCardAcceptance_c()] Acknowledgement against Accept Card Command[0] = 0x%xh.\n",rcvPkt[0]);
				
				////writeFileLog(log);
				
				AVRM_writeFileLog(log,DEBUG);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {
                 ////writeFileLog("[EnableCardAcceptance_c()] Acknowledgement Received");
                 AVRM_writeFileLog("[EnableCardAcceptance_c()] Acknowledgement Received",DEBUG);
                 totalByteToRecv = 12;
                 memset(rcvPkt,'\0',24);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

		 sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
		
		 ////writeFileLog(log);
		 
		              AVRM_writeFileLog(log,DEBUG);
		 
		      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      // Edited By: Abhishek Kole, 18-10-2017                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 /*

                 for(i=0;i<rcvPktLen;i++)
	         {
			
                        memset(log,'\0',LOG_ARRAY_SIZE);

		        sprintf(log,"[EnableCardAcceptance_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
		
		        //writeFileLog(log);

                 }

                 */    

                 //////////////////////////////////////////////////////////////////////////////////////////////////
                      

                      //EnableAcceptance Command Success Data:f2 0 0 6 50 33 30 30 31 30 3 95

                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 33
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[EnableCardAcceptance_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,ERROR);

                          ////writeFileLog("[EnableCardAcceptance_c() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[EnableCardAcceptance_c() Exit] Got 0x4e error",ERROR);

/*
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                //writeFileLog("[EnableCardAcceptance_c() Exit] Forced insertion of card detected");
                                return 2; //Channel blocked due to forced insertion of card

                          }
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                //writeFileLog("[EnableCardAcceptance_c() Exit] Insertion/return mouth blocked");
                                return 3; //Insertion mouth blocked
                          }
                          else
                          {
                                //writeFileLog("[EnableCardAcceptance_c() Exit] Other error");
                                return 5; //other error
                                

                          }
                          */

                          ////writeFileLog("[EnableCardAcceptance_c() Exit] Other error found when issue enable card acceptance command.");
                          AVRM_writeFileLog("[EnableCardAcceptance_c() Exit] Other error found when issue enable card acceptance command.",ERROR);
  						  SetLastError(rcvPkt[7], rcvPkt[8]);

                          return OTHER_ERROR; //other error
                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                               ( 0x33 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[EnableCardAcceptance_c()] Enable Success");
                             AVRM_writeFileLog("[EnableCardAcceptance_c()] Enable Success",INFO);
                             // 0 = Operation Successful 
                             return SUCCESS;
                             
                      }
                      else
                      {

                             ////writeFileLog("[EnableCardAcceptance_c() Exit] Enable command  failed.");
                             AVRM_writeFileLog("[EnableCardAcceptance_c() Exit] Enable command  failed.",ERROR);
                             return OTHER_ERROR;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                
        }
        else if (0x15 == rcvPkt[0])
        {
             ////writeFileLog("[EnableCardAcceptance_c()] Nak Reply Received");
             AVRM_writeFileLog("[EnableCardAcceptance_c()] Nak Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }
        else if (0x04 == rcvPkt[0])
        {
             ////writeFileLog("[EnableCardAcceptance_c()] EOT Reply Received");
             AVRM_writeFileLog("[EnableCardAcceptance_c()] EOT Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }
      
      

}//int EnableCardAcceptance_c(int Timeout) end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function Name : IsCardInChannel_c 
// Return Type   : int 
//                  0 = No Card in the Channel
//                  1 = Card found in the Channel
//                  2 = Communication Failure
//                  3 = Operation timeout occurred
//                  4 = Other Error 

// Parameters    : Name                           Type          Description 

                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 




// To get Current Report status of st0, st1, st2

//static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};

int IsCardInChannel_c( int   Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode) {

        //printf("\n[IsCardInChannel_c()] Entry .");

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        g_getStatus[0] = 0xF2;
        g_getStatus[1] = MTK_DEVICE_ADDRESS;
        g_getStatus[2] = 0x00;
        g_getStatus[3] = 0x03;
        g_getStatus[4] = 0x43;
        g_getStatus[5] = 0x31;
        g_getStatus[6] = 0x30;
        g_getStatus[7] = 0x03;
        g_getStatus[8] = 0x00;

        char bcc=0x00;

        unsigned char rcvPkt[25];

        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,oneCradInChannel = 0,noCardInChannel = 0, oneCardInGate=0;
        int cardInGate=0, cardInRFPosition=0;
        unsigned int rcvPktLen =0x00; 

        //If there was any card block or not 
        AVRM_writeFileLog("[IsCardInChannel_c()] Going to send Current Report status of st0, st1, st2",INFO);

        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = 0x00;        
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[IsCardInChannel_c()] bcc value is 0x%xh",bcc);
        
        AVRM_writeFileLog(log,DEBUG);
        
        delay_mSec(g_MaxDelay);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        ///////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////

        //printf("\n[IsCardInChannel_c()] Com Handle: %d .",g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[IsCardInChannel_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[IsCardInChannel_c()] Failed to Send Report status Command ");
                    AVRM_writeFileLog("[IsCardInChannel_c()] Failed to Send Report status Command ",ERROR);
                    // 2 = Communication Failure 
                    //return 2;  //Return value changed
		    return COMMUNICATION_FAILURE;
                 }
	

        }

        ////////////////////////////////////////////////////////////////////////////////////////////

        //AVRM_writeFileLog("[IsCardInChannel_c()] Status Command send Successfully ",INFO);       
        //AVRM_writeFileLog("[IsCardInChannel_c()] Now Going to read Acknowledgement ",INFO);

        // Now going to Check Acknowledgement   

        totalByteToRecv = 1;
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[IsCardInChannel_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,INFO);

        if(0 == rtcode)
        {
                 // 2 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[IsCardInChannel_c()] Failed to read ack reply");
                 AVRM_writeFileLog("[IsCardInChannel_c()] Failed to read ack reply",ERROR);
                 //return 2;  //Return value changed
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[IsCardInChannel_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);


        }

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      ////writeFileLog("[IsCardInChannel_c()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[IsCardInChannel_c()] Acknowledgement Received",INFO);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[IsCardInChannel_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);
                      
                      AVRM_writeFileLog(log,DEBUG);
                      
                      // Now time to send Acknowledgement
                      // Edited By: Abhishek Kole, 18-10-2017                       
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                      
                      if(0 == rtcode)
		      {
				 // 2 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[IsCardInChannel_c()] Failed to read status command reply bytes.");
                                 AVRM_writeFileLog("[IsCardInChannel_c()] Failed to read status command reply bytes.",ERROR);
                                //return 2;  //Return value changed
		                return COMMUNICATION_FAILURE;
		      }

                      //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 31
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[IsCardInChannel_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,ERROR);
                          
  						  SetLastError(rcvPkt[7], rcvPkt[8]);

			              return OTHER_ERROR;  //Other error
/*
                          //writeFileLog("[IsCardInChannel_c() Exit] Got 0x4e error");
                          
                          if( ( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) || 
                              ( (0x31==rcvPkt[7]) && ( 0x30 == rcvPkt[8] ) )
                            )
                          {
                                //writeFileLog("[IsCardInChannel_c() Exit] Force Card Entry Found.");
                                return 1; //Card Found in chanel
                          }
                          else
                          {
                                //writeFileLog("[IsCardInChannel_c() Exit] Other error");
                                return 4; //other error
                                

                          }
                          
*/                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[IsCardInChannel_c()] Get Status Command issue success");
                             AVRM_writeFileLog("[IsCardInChannel_c()] Get Status Command issue success",INFO);
                             
                      }
                      else
                      {
                             ////writeFileLog("[IsCardInChannel_c() Exit] Get Status Command issue failed");
                             AVRM_writeFileLog("[IsCardInChannel_c() Exit] Get Status Command issue failed",ERROR);
                             return COMMUNICATION_FAILURE;//Communication failure
                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {
				

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[IsCardInChannel_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {

										if(0x30 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardInChannel_c()] No Card in MTK-571");
												   AVRM_writeFileLog("[IsCardInChannel_c()] No Card in MTK-571",INFO);
												   // 0 = No Card in the Channel 
		//                                           noCardInChannel = 1; 
		//                                           oneCradInChannel = 0; 
										}
												else if(0x31 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardInChannel_c()] One Card in Gate");
												   AVRM_writeFileLog("[IsCardInChannel_c()] One Card in Gate",INFO);
												   cardInGate=1;
		//                                         oneCardInGate=1;
		//                                         noCardInChannel = 1; 
										}
												else if(0x32 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardInChannel_c()] One Card on RF/IC Card Position");
												   AVRM_writeFileLog("[IsCardInChannel_c()] One Card on RF/IC Card Position",INFO);
												   cardInRFPosition=1;
												   // 1 = Card found in the Channel 
		//                                           oneCradInChannel = 1; 
		//					   noCardInChannel = 0;
										} 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
										if(0x30 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardInChannel_c()] No Card in Stacker");
												   AVRM_writeFileLog("[IsCardInChannel_c()] One Card on RF/IC Card Position",INFO);
										}
												else if(0x31 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardInChannel_c()] Few Card in Stacker");
												   AVRM_writeFileLog("[IsCardInChannel_c()] Few Card in Stacker",INFO);
										}
												else if(0x32 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardInChannel_c()] Enough Card in the Box");
												   AVRM_writeFileLog("[IsCardInChannel_c()] Enough Card in the Box",INFO);
										}   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[IsCardInChannel_c()] Error Card bin Not Full");
											   AVRM_writeFileLog("[IsCardInChannel_c()] Error Card bin Not Full",ERROR);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[IsCardInChannel_c()] Error Card Bin Full");
											   AVRM_writeFileLog("[IsCardInChannel_c()] Error Card Bin Full",ERROR);
											   
									}
                                }
                                
				
	              }
                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);

		        char ChanelStatus =0x00,MTKSensorStatus =0x00;

                        //Now Check sensor status
			if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
			{

			    if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
			    {
			      ////writeFileLog("[IsCardInChannel_c()] Sensor 1 Status Found Blocked As Card in Mouth ");
			       AVRM_writeFileLog("[IsCardInChannel_c()] Sensor 1 Status Found Blocked As Card in Mouth",INFO);
			      
			      cardInGate = 1;  
			    }//if(  1 == (MTKSensorStatus & 0x01) ) block end
			}//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end


                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[IsCardInChannel_c()] Failed to send total Acknowledgement Command ");
		            AVRM_writeFileLog("[IsCardInChannel_c()] Failed to send total Acknowledgement Command ",ERROR);
                            // Communication Failure 
                            
	         	    return COMMUNICATION_FAILURE;
		      }

		      if(cardInGate && cardInRFPosition) //++Card both at RF position and Gate
		      {
                                ////writeFileLog("[IsCardInChannel_c()] Card both at RF position and Gate");
                                AVRM_writeFileLog("[IsCardInChannel_c()] Card both at RF position and Gate ",INFO);
				return CARD_FOUND_IN_CHANNEL;
                      }

		      if(cardInGate) //++No card in RF but in Gate
                      {
                                ////writeFileLog("[IsCardInChannel_c()] No card in RF but in Gate");
                                AVRM_writeFileLog("[IsCardInChannel_c()] No card in RF but in Gate ",INFO);
				return 2;
                      }

		      if(cardInRFPosition) //No card in Gate but in RF position
                      {
                                ////writeFileLog("[IsCardInChannel_c()] No card in Gate but in RF position");
                                AVRM_writeFileLog("[IsCardInChannel_c()] No card in Gate but in RF position",INFO);
				return CARD_FOUND_IN_CHANNEL;
                      }

		      return NO_CARD_FOUND_IN_CHANNEL;  //No card in channel		      

/*
                      if(1 == oneCradInChannel)
                      { 
                      	    // 1 = Card found in the Channel  
                            return 1; 
                      }
                      if(1 == noCardInChannel) 
                      {
                           // 0 = No Card in the Channel 
                           return 0;
                      }

                      if(1 == oneCradInChannel)
                      { 
			    if(1 == oneCardInGate)
			    {
                                //writeFileLog("[IsCardInChannel_c()] One card in RF/IC position and one card in gate");
				return 6;  //One card in RF/IC position and one card in gate
			    }
                            else
                      	    // 1 = Card found in the Channel  
	                        return 1; 
                      }
		      if(1 == oneCardInGate)
                      {
                         if(1 == noCardInChannel)
                         {
                                //writeFileLog("[IsCardInChannel_c()] No card in RF/IC position and one card in gate");
				return 5;  //No card in RF/IC position and one card in gate
                         }
                         else
                         {
                               //writeFileLog("[IsCardInChannel_c()] One card in RF/IC position and one card in gate");
				return 6;  //One card in RF/IC position and one card in gate
                         }
		      }
                      if(1 == noCardInChannel) 
                      {
                           // 0 = No Card in the Channel 
                           return 0;
                      }
*/
                       
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             ////writeFileLog("[IsCardInChannel_c()] Nak Reply Received");
             AVRM_writeFileLog("[IsCardInChannel_c()] Nak Reply Received",ERROR);
             // 31 = Other Error
             return OTHER_ERROR;  
       }
       else if (0x04 == rcvPkt[0])
       {
             ////writeFileLog("[IsCardInChannel_c()] EOT Reply Received");
             AVRM_writeFileLog("[IsCardInChannel_c()] EOT Reply Received",ERROR);
             //  31 = Other Error
             return OTHER_ERROR;  
       }
 

}//int IsCardInChannel_c(int Timeout) end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function Name : IsCardRemoved_c 
// Return Type   : int 
//                  0 = Not Removed
//                  1 = Removed
//                  2 = Communication Failure
//                  3 = Operation timeout occurred
//                  4 = Other Error 

// Parameters    : Name                           Type          Description 

                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 




// To get Current Report status of st0, st1, st2

//static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};

int IsCardRemoved_c( int   Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        char ChanelStatus =0x00,MTKSensorStatus =0x00;

        memset(log,'\0',LOG_ARRAY_SIZE);

        //Now Check sensor status
        if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
        {

            if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
            {
              ////writeFileLog("[IsCardRemoved_c()] Sensor 1 Status Found Blocked As Card in Mouth ");
              AVRM_writeFileLog("[IsCardRemoved_c()] Sensor 1 Status Found Blocked As Card in Mouth",INFO);
              return 0;  
            }//if(  1 == (MTKSensorStatus & 0x01) ) block end
        }//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end


        g_getStatus[0] = 0xF2;
        g_getStatus[1] = MTK_DEVICE_ADDRESS;
        g_getStatus[2] = 0x00;
        g_getStatus[3] = 0x03;
        g_getStatus[4] = 0x43;
        g_getStatus[5] = 0x31;
        g_getStatus[6] = 0x30;
        g_getStatus[7] = 0x03;
        g_getStatus[8] = 0x00;

        char bcc=0x00;
        unsigned char rcvPkt[25];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,oneCradInGate = 0;
        unsigned int rcvPktLen =0x00; 

        // If there was any card block or not 

        ////writeFileLog("[IsCardRemoved_c()] Going to send Current Report status of st0, st1, st2");
        AVRM_writeFileLog("[IsCardRemoved_c()] Going to send Current Report status of st0, st1, st2",INFO);
        
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = 0x00;
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[IsCardRemoved_c()] bcc value is 0x%xh",bcc);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,INFO);

        ///////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////

		delay_mSec(g_MaxDelay);
		
        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[IsCardRemoved_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[IsCardRemoved_c()] Failed to Send Report status Command ");
                    AVRM_writeFileLog("[IsCardRemoved_c()] Failed to Send Report status Command ",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }
	}

        ////////////////////////////////////////////////////////////////////////////////////////

       
        //AVRM_writeFileLog("[IsCardRemoved_c()] Status Command send Successfully",DEBUG);
        
        //AVRM_writeFileLog("[IsCardRemoved_c()] Now Going to read Acknowledgement",DEBUG);

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;
 
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[IsCardRemoved_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[IsCardRemoved_c()] Failed to ack bytes.");
                 AVRM_writeFileLog("[IsCardRemoved_c()] Failed to ack bytes.",DEBUG);
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[IsCardRemoved_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                ////writeFileLog(log);
                
                AVRM_writeFileLog(log,DEBUG);

        }//else 

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {
                      
                      ////writeFileLog("[IsCardRemoved_c()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[IsCardRemoved_c()] Acknowledgement Received",INFO);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[IsCardRemoved_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);
                      
                      AVRM_writeFileLog(log,INFO);
                      
                      // Now time to send Acknowledgement 
                      // Edited By: Abhishek Kole, 18-10-2017
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////

                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[IsCardRemoved_c()] Failed to read status command reply bytes.");
                                 AVRM_writeFileLog("[IsCardRemoved_c()] Failed to read status command reply bytes.",DEBUG);
				 return COMMUNICATION_FAILURE;
		      }
                      //////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Malay: Sample Check 0x4e Data: f2 0 0 5 4e 33 30 31[e1] 36[e2] 3 bc

                      //STX  (1 byte) : f2
	              //ADDR (1 byte) : 0
		      //LENH (1 byte) : 0
		      //LENL (1 byte) : 5
		      //EMT  (1 byte) : 4e
		      //CM   (1 byte) : 31
		      //PM   (1 byte) : 30
                      //e1   (1 byte) : 31
		      //e0   (1 byte) : 36
                      //DATA (N bytes): No Byte
                      //ETX  (1 byte) : 3
		      //BCC  (1 byte) : bc

                      // E1: RecvPackt[7] E2: RecvPackt[8]
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {
                          memset(log,'\0',LOG_ARRAY_SIZE);

                          sprintf(log,"[IsCardRemoved_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          ////writeFileLog(log); 
                          
                          AVRM_writeFileLog(log,DEBUG);

                          ////writeFileLog("[IsCardRemoved() Exit] Got 0x4e error");
                          
                          AVRM_writeFileLog("[IsCardRemoved() Exit] Got 0x4e error",DEBUG);
/*                          
                          if( ( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) )  ||
                              ( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) )  
                            )
                          {
                                //writeFileLog("[IsCardRemoved_c() Exit] Force Card Entry Found.");
                                return 0; //Not Removed
                          }
                          else
                          {
                                //writeFileLog("[IsCardRemoved_c() Exit] Other error");
                                return 4; //other error
                                

                          }
  */                        
   						  SetLastError(rcvPkt[7], rcvPkt[8]);
						  return OTHER_ERROR; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             ////writeFileLog("[IsCardRemoved_c()] Get Status Command issue success");
                             AVRM_writeFileLog("[IsCardRemoved_c()] Get Status Command issue success",INFO);
                             
                      }
                      else
                      {

                             ////writeFileLog("[IsCardRemoved_c() Exit] Get Status Command issue failed");
                             AVRM_writeFileLog("[IsCardRemoved_c() Exit] Get Status Command issue failed",INFO);
                             return OTHER_ERROR;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[IsCardRemoved_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
										if(0x30 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardRemoved_c()] No Card in MTK-571");
												   AVRM_writeFileLog("[IsCardRemoved_c()] No Card in MTK-571",INFO);
													
										}
												else if(0x31 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardRemoved_c()] One Card in Gate");
												   AVRM_writeFileLog("[IsCardRemoved_c()] One Card in Gate",INFO);
												   // 0 = Not Removed 
												   oneCradInGate = 1;
													
										}
												else if(0x32 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardRemoved_c()] One Card on RF/IC Card Position");
												   AVRM_writeFileLog("[IsCardRemoved_c()] One Card on RF/IC Card Position",INFO);
													
										  
										} 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
										if(0x30 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardRemoved_c()] No Card in Stacker");
												   AVRM_writeFileLog("[IsCardRemoved_c()] No Card in Stacker",INFO);
										}
												else if(0x31 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardRemoved_c()] Few Card in Stacker");
												   AVRM_writeFileLog("[IsCardRemoved_c()] Few Card in Stacker",INFO);
										}
												else if(0x32 == rcvPkt[i])
										{
												   ////writeFileLog("[IsCardRemoved_c()] Enough Card in the Box");
												   AVRM_writeFileLog("[IsCardRemoved_c()] Few Card in Stacker",INFO);
										}   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[IsCardRemoved_c()] Error Card bin Not Full");
											   AVRM_writeFileLog("[IsCardRemoved_c()] Error Card bin Not Full",ERROR);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[IsCardRemoved_c()] Error Card Bin Full");
											   AVRM_writeFileLog("[IsCardRemoved_c()] Error Card Bin Full",ERROR);
											   
									}
                                }
                                
				
	              }
                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[IsCardRemoved_c()] Failed to send total Acknowledgement Command ");
		                    AVRM_writeFileLog("[IsCardRemoved_c()] Failed to send total Acknowledgement Command",ERROR);
                            // 28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                      if(1 == oneCradInGate)
                      { 
                      	    // 0 = Not Removed 
                      	    AVRM_writeFileLog("[IsCardRemoved_c()] CARD_NOT_REMOVED",INFO);
                            return CARD_NOT_REMOVED; 
                      }
                      else if(0 == oneCradInGate) 
                      {
                           // 1 = Removed 
                           AVRM_writeFileLog("[IsCardRemoved_c()] CARD_REMOVED",INFO);
                           return CARD_REMOVED;
                      }
                       
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             ////writeFileLog("[IsCardRemoved_c()] Nak Reply Received");
             AVRM_writeFileLog("[IsCardRemoved_c()] Nak Reply Received",INFO);
             // 31 = Other Error 
             return OTHER_ERROR;  
       }
       else if (0x04 == rcvPkt[0])
       {
             ////writeFileLog("[IsCardRemoved_c()] EOT Reply Received");
             AVRM_writeFileLog("[IsCardRemoved_c()] EOT Reply Received",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
       }





}//int IsCardRemoved_c(int Timeout) end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function Name : CollectCard_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Rejection Bin Full
//                  3 = No Card in the Channel
//                  4 = Operation timeout Occurred 
//                  5 = Other Error
                

// Parameters    : Name                           Type          Description 
                  
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

// To get Current Report status of st0, st1, st2
// static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};
// To Move to Error Bin
// static unsigned char g_moveToErrorBin[9]={0xF2,0x00,0x00,0x03,0x43,0x32,0x33,0x03,0x00};

int CollectCard_c(   int   Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        char ChanelStatus =0x00,MTKSensorStatus =0x00;

        memset(log,'\0',LOG_ARRAY_SIZE);


        g_getStatus[0]= 0xF2;
		g_getStatus[1]= MTK_DEVICE_ADDRESS;
		g_getStatus[2]= 0x00;
		g_getStatus[3]= 0x03;
		g_getStatus[4]= 0x43;
		g_getStatus[5]= 0x31;
		g_getStatus[6]= 0x30; 
		g_getStatus[7]= 0x03;
		g_getStatus[8]= 0x00;       

        g_moveToErrorBin[0]= 0xF2;
        g_moveToErrorBin[1]= MTK_DEVICE_ADDRESS;
        g_moveToErrorBin[2]= 0x00;
        g_moveToErrorBin[3]= 0x03;
        g_moveToErrorBin[4]= 0x43;
        g_moveToErrorBin[5]= 0x32;
        g_moveToErrorBin[6]= 0x33;
        g_moveToErrorBin[7]= 0x03;
        g_moveToErrorBin[8]= 0x00;

        char bcc=0x00;  
     
        unsigned char rcvPkt[25];

        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,noCardInRF_IC = 0,rejectionBinFull = 0;
        unsigned int rcvPktLen =0x00; 

        ////writeFileLog("[CollectCard_c()] Going to send Current Report status of st0, st1, st2");
        AVRM_writeFileLog("[CollectCard_c()] Going to send Current Report status of st0, st1, st2",INFO);
        
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8]= 0x00;
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        
        sprintf(log,"[CollectCard_c()] bcc value is 0x%xh.",bcc);
        
        ////writeFileLog(log); 
        
        AVRM_writeFileLog(log,INFO);


        ///////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////

		delay_mSec(g_MaxDelay);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[CollectCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[CollectCard_c()] Failed to Send Report status Command ");
                    AVRM_writeFileLog("[CollectCard_c()] Failed to Send Report status Command ",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }


	}

        //////////////////////////////////////////////////////////////////////////////////////////

    
        //AVRM_writeFileLog("[CollectCard_c()] Status Command send Successfully ",DEBUG);
        //AVRM_writeFileLog("[CollectCard_c()] Now Going to read Acknowledgement",DEBUG);

        // Now going to Check Acknowledgement 

        totalByteToRecv = 1;
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[CollectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[CollectCard_c()] Failed to read ack bytes");
                 AVRM_writeFileLog("[CollectCard_c()] Failed to read ack bytes",ERROR);
                 return COMMUNICATION_FAILURE;
        }
        else
	{
		 
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[CollectCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                ////writeFileLog(log);
                
                AVRM_writeFileLog(log,DEBUG);
				
	}

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      ////writeFileLog("[CollectCard_c()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[CollectCard_c()] Acknowledgement Received",DEBUG);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                       
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[CollectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);
                      
                      AVRM_writeFileLog(log,DEBUG);
                      
                      /*
                      // Now time to send Acknowledgement 
                      // Edited By: Abhishek Kole, 18-10-2017                      
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
		       */
                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);

				 ClearTransmitBuffer(g_ComHandle);

                                 ////writeFileLog("[CollectCard_c()] Failed to read status command reply bytes");
                                 AVRM_writeFileLog("[CollectCard_c()] Failed to read status command reply bytes",ERROR);

				 return COMMUNICATION_FAILURE;

		      }      
                      
                      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      //Check Negative Reply bytes
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[CollectCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              ////writeFileLog(log); 
                              
                              AVRM_writeFileLog(log,ERROR);
/*
		              //Force Card Detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
                                  ( 0x36 == rcvPkt[8] ) )
			      {
		                       //writeFileLog("[CollectCard_c()] Negative response receieved for issue device status command");  
	   
		                        
		                       if( 1 == MutekInitWithCardMoveInFront( LogdllPathstr,
		                       LogFileName,deviceid,fnLogfileMode) )
		                       {
		                                
                                               //writeFileLog("[CollectCard_c()] Operation successfull Block card collect successfull.");
		                               return 0; //Operation successfull card return successfull

		                       }
		                       else
		                       {
                                               //writeFileLog("[CollectCard_c()] Operation failed to Negative response found.");
		                               return 5; //Other Error

		                       }

					 
		              }
*/
						  SetLastError(rcvPkt[7], rcvPkt[8]);
        			      return OTHER_ERROR; //Other Error
    
                      }// if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) ) block

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[CollectCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[CollectCard_c()] No Card in MTK-571");
											   AVRM_writeFileLog("[CollectCard_c()] No Card in MTK-571",INFO);
											   // 3 = No Card in the Channel 
											   noCardInRF_IC = 1;
									}
											else if(0x31 == rcvPkt[i])
									{
											
											   ////writeFileLog("[CollectCard_c()] One Card in Gate");
											   AVRM_writeFileLog("[CollectCard_c()] One Card in Gate",INFO);
																				  
								 
											}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[CollectCard_c()] One Card on RF/IC Card Position");
											   AVRM_writeFileLog("[CollectCard_c()] One Card on RF/IC Card Position",INFO);
												
											   
									} 
                                }
                                //Card Status Code st1 
                                if(8 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[CollectCard_c()] No Card in Stacker");
											   AVRM_writeFileLog("[CollectCard_c()] No Card in Stacker",INFO);
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[CollectCard_c()] Few Card in Stacker");
											   AVRM_writeFileLog("[CollectCard_c()] Few Card in Stacker",INFO);
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[CollectCard_c()] Enough Card in the Box");
											   AVRM_writeFileLog("[CollectCard_c()] Enough Card in the Box",INFO);
									}   
                                } 
                                //Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[CollectCard_c()] Error Card bin Not Full");
                                            AVRM_writeFileLog("[CollectCard_c()] Error Card bin Not Full",INFO);
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[CollectCard_c()] Error Card Bin Full");
                                           AVRM_writeFileLog("[CollectCard_c()] Error Card Bin Full",INFO);
                                           // 2 = Rejection Bin Full 
                                           rejectionBinFull = 1;
		                        }
                                        

                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++)

                      //////////////////////////////////////////////////////////////////////////////////////////
                      // Now time to send Acknowledgement 
                      // Edited By: Abhishek Kole, 18-10-2017                      
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[CollectCard_c()] Failed to send total Acknowledgement Command ");
		                     AVRM_writeFileLog("[CollectCard_c()] Failed to send total Acknowledgement Command ",ERROR);
                            //28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                      if(1 == noCardInRF_IC)
                      {
                           // 3 = No Card in the Channel 
                           ////writeFileLog("[CollectCard_c()] No Card in chanel. "); 
                           AVRM_writeFileLog("[CollectCard_c()] No Card in chanel. ",INFO);
                           return NO_CARD_IN_CHANNEL;
                      }
                      if(1 == rejectionBinFull)
                      {
                           // 2 = Rejection Bin Full 
                           ////writeFileLog("[CollectCard_c()] Rejection Bin Full. "); 
                           AVRM_writeFileLog("[CollectCard_c()] Rejection Bin Full. ",INFO); 
                           return REJECTION_BIN_FULL;

                      }
                      
                      
                       
         }//
         // If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             ////writeFileLog("[CollectCard_c()] Nak Reply Received");
             AVRM_writeFileLog("[CollectCard_c()] Nak Reply Received",ERROR); 
             // 31 = Other Error 
             return OTHER_ERROR;  
         }
         else if (0x04 == rcvPkt[0])
         {
             ////writeFileLog("[CollectCard_c()] EOT Reply Received");
             AVRM_writeFileLog("[CollectCard_c()] EOT Reply Received",ERROR); 
             // 31 = Other Error 
             return OTHER_ERROR;  
         }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

          
        
        //Now Check sensor status
        if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
        {

            if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
            {
                ////writeFileLog("[CollectCard_c()] Sensor 1 Status Found Blocked As Card in Mouth Other Error Found:5");
                AVRM_writeFileLog("[CollectCard_c()] Sensor 1 Status Found Blocked As Card in Mouth Other Error Found:5",ERROR); 
                // 31 = Other Error 
                return OTHER_ERROR;  


            }//if(  1 == (MTKSensorStatus & 0x01) ) block end
              
          


        }//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end

        


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // All status ok and Going to issue Reject Card 
        ////writeFileLog("[CollectCard_c()] Going to send Reject Card Comamnd");
        
        AVRM_writeFileLog("[CollectCard_c()] Going to send Reject Card Comamnd",INFO); 

        //Reinit bcc value
        g_moveToErrorBin[8] =0x00;

        bcc = getBcc(9,g_moveToErrorBin);

        g_moveToErrorBin[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[CollectCard_c()] bcc value is 0x%xh",bcc);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG); 

        ///////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_moveToErrorBin,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////

        
        // Before Send Command clear all serial buffer 

        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[CollectCard_c()] Reject Card Command[%d] = 0x%xh",i,g_moveToErrorBin[i]);
        
                 ////writeFileLog(log);

                 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_moveToErrorBin[i]);
                 
                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[CollectCard_c()] Failed to Send Reject Card Command ");
                    AVRM_writeFileLog("[CollectCard_c()] Failed to Send Reject Card Command ",DEBUG); 
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }

	}

        ////writeFileLog("[CollectCard_c()] Reject Card Command send Successfully");
        AVRM_writeFileLog("[CollectCard_c()] Reject Card Command send Successfully",INFO);
        ////writeFileLog("[CollectCard_c()] Now Going to read Acknowledgement");
         AVRM_writeFileLog("[CollectCard_c()] Now Going to read Acknowledgement",DEBUG);

        // Now going to Check Acknowledgement   

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[CollectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);
        
                     
		              
                       //////////////////////////////////////////////////////////////

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[CollectCard_c()] Failed to read reject command ack bytes.");
                 AVRM_writeFileLog("[CollectCard_c()] Failed to read reject command ack bytes.",ERROR);
                 return COMMUNICATION_FAILURE;
        }
        else  if(0x06 == rcvPkt[0]) //ack byte receieved
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[CollectCard_c()] Acknowledgement against Reject Card Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);

                 totalByteToRecv = 12;

                 rcvPktLen = 0;

                 memset(rcvPkt,'\0',24); 
         
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                 /////////////////////////////////////////////////////////////////////////////////

                 if(0 == rtcode)
		 {
		      // 28 = Communication Failure 
		      ClearReceiveBuffer (g_ComHandle);
		      ClearTransmitBuffer(g_ComHandle);
                      ////writeFileLog("[CollectCard_c()] Communication Failure when read reject command reply bytes.");
                      AVRM_writeFileLog("[CollectCard_c()] Communication Failure when read reject command reply bytes.",ERROR);
		      return COMMUNICATION_FAILURE;
		 }  
               
                 //Now send ack byte to mtk
                 int send_rvalue = 0;
 		 // Now time to send Acknowledgement 
                      // Edited By: Abhishek Kole, 18-10-2017                      
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 if(0 == send_rvalue) 
		 {
		       ////writeFileLog("[CollectCard_c()] Failed to send Acknowledgement byte for reject command .");
		        AVRM_writeFileLog("[CollectCard_c()] Failed to send Acknowledgement byte for reject command .",ERROR);
                          
		 }

                 //////////////////////////////////////////////////////////////////////////////////
           
                 //Check Negative Reply bytes
                 if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                 {

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[CollectCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                      ////writeFileLog(log); 
                      
                      AVRM_writeFileLog(log,ERROR);

                      ////writeFileLog("[CollectCard_c()] Collect Failed to do.");
                      
                      AVRM_writeFileLog("[CollectCard_c()] Collect Failed to do.",ERROR);
                      
					  SetLastError(rcvPkt[7], rcvPkt[8]);

        		      return OTHER_ERROR;//other error
                 }
                 else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                          ( 0x32 == rcvPkt[5] ) && ( 0x33 == rcvPkt[6] ) )
                 {
                
		         ////writeFileLog("[CollectCard_c()] Collect Successfully done.");
		         AVRM_writeFileLog("[CollectCard_c()] Collect Successfully done.",INFO);

		         return SUCCESS;
                 }
                 else
                 {

                         ////writeFileLog("[CollectCard_c()] Collect Failed to do.");
                         AVRM_writeFileLog("[CollectCard_c()] Collect Failed to do.",ERROR);

		         return OTHER_ERROR; //Other Error

                 }
                
				
	}
        // This function does not end here because Rejection and collection bin are not same 
        else  if(0x15 == rcvPkt[0]) //nak byte receieved
	{
              ////writeFileLog("[CollectCard_c()] Reject Failed to do a nak receieved.");
              
              AVRM_writeFileLog("[CollectCard_c()] Reject Failed to do a nak receieved.",ERROR);

              return OTHER_ERROR; //Other Error

        }
        else  if(0x04 == rcvPkt[0]) //ack byte receieved
	{
              ////writeFileLog("[CollectCard_c()] Reject Failed to do a eot receieved.");
              
              AVRM_writeFileLog("[CollectCard_c()] Reject Failed to do a eot receieved.",ERROR);

              return OTHER_ERROR; //Other Error

        }
        
      


}//int CollectCard_c(int Timeout) end

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function Name : d_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Channel blocked 
//                  3 = Insert/return mouth block
//                  4 = Stacker Empty
//                  5 = Operational timeout occurred
//                  6 = Other Error 

// Parameters    : Name                           Type          Description 

                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 


 

// To get Current Report status of st0, st1, st2
// To Dispense Card   
//static unsigned char g_dispenseCard[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x32,0x32,0x03,0x00};
static unsigned char g_dispenseCard[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x32,0x31,0x03,0x00};  //Bring card to IC position to protect from forced insertion

int DispenseCard_c(  int  Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();
        
        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        g_getStatus[0] = 0xF2;
		g_getStatus[1] = MTK_DEVICE_ADDRESS;
		g_getStatus[2] = 0x00;
		g_getStatus[3] = 0x03;
		g_getStatus[4] = 0x43;
		g_getStatus[5] = 0x31;
		g_getStatus[6] = 0x30;
		g_getStatus[7] = 0x03;
		g_getStatus[8] = 0x00;
        char bcc=0x00;
        unsigned char rcvPkt[25];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,
        returnMouthFlag = 0,channelBlockFlag = 0,stackerEmptyFlag = 0;
        unsigned int rcvPktLen =0x00; 

        ////writeFileLog("[DispenseCard_c()] Going to send Current Report status of st0, st1, st2");
        AVRM_writeFileLog("[DispenseCard_c()] Going to send Current Report status of st0, st1, st2",DEBUG);
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = 0x00;
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DispenseCard_c()] bcc value is 0x%xh",bcc);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		delay_mSec(g_MaxDelay);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[DispenseCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[DispenseCard_c()] Failed to Send Report status Command return command:1");
                    AVRM_writeFileLog("[DispenseCard_c()] Failed to Send Report status Command return command:1",ERROR);
                    return COMMUNICATION_FAILURE;
                 }
	}


        //++AVRM_writeFileLog("[DispenseCard_c()] Status Command send Successfully",DEBUG);
        //++AVRM_writeFileLog("[DispenseCard_c()] Now Going to read Acknowledgement",DEBUG);

        // Now going to Check Acknowledgement 
 
        totalByteToRecv = 1;

        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);

        AVRM_writeFileLog(log,ERROR);

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);

                 ////writeFileLog("[DispenseCard_c()] Communication Failure for Check Acknowledgement .");
                 
                 AVRM_writeFileLog("[DispenseCard_c()] Communication Failure for Check Acknowledgement .",ERROR);
                 
                 return COMMUNICATION_FAILURE;

        }
        else
        {
                 
               memset(log,'\0',LOG_ARRAY_SIZE);

               sprintf(log,"[DispenseCard_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
               ////writeFileLog(log);
               
               AVRM_writeFileLog(log,DEBUG);
 
        }

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      ////writeFileLog("[DispenseCard_c()] Acknowledgement Received");
                      
                      AVRM_writeFileLog("[DispenseCard_c()] Acknowledgement Received",DEBUG);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);

                      AVRM_writeFileLog(log,DEBUG);

                     
                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[DispenseCard_c()] Communication Failure for read device status reply command");
                                 AVRM_writeFileLog("[DispenseCard_c()] Communication Failure for read device status reply command",ERROR);
				 return COMMUNICATION_FAILURE;
		      }
              
                     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {

                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[DispenseCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              ////writeFileLog(log); 
                              
                              AVRM_writeFileLog(log,ERROR);
							  
	  						  SetLastError(rcvPkt[7], rcvPkt[8]);

			                  return OTHER_ERROR; //Other error
/*
		              //Force Card Detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
                                  ( 0x36 == rcvPkt[8] ) )
			      {
		                         //writeFileLog("[DispenseCard_c()] Forced insertion of card detected");  
	   
		                         if( 1 == MutekInitWithCardMoveInFront(LogdllPathstr,
	                                 LogFileName,deviceid,fnLogfileMode) )
                                         {
                                             //writeFileLog("[DispenseCard_c()] Block Card Removed Successfully done ."); 
                                             // return 0; //operation success
                                         }
                                         else
                                         {
                                            //writeFileLog("[DispenseCard_c()] Unable to do dispense operation."); 
                                            return 6; //other error

                                         }

					 
		              }
			      else
			      {
                                   //writeFileLog("[DispenseCard_c()]  Other Error found."); 
                                   return 6; //other error
			      }

                              else if( ( 0x4e == rcvPkt[4] ) && 
                                       ( 0x31 != rcvPkt[7] ) && 
                                       ( 0x36 != rcvPkt[8] ) )
                              {
                                   //writeFileLog("[DispenseCard_c()]  Other Error found."); 
                                   return 6; //other error

                              }
*/

                      }//if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      for(i=0;i<rcvPktLen;i++)
	              {
				

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[DispenseCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                             ////writeFileLog("[DispenseCard_c()] No Card in MTK-571.");
                                             AVRM_writeFileLog("[DispenseCard_c()] No Card in MTK-571.",INFO);
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                             ////writeFileLog("[DispenseCard_c()] One Card in Gate.");
                                              AVRM_writeFileLog("[DispenseCard_c()] One Card in Gate.",INFO);
                                             // 2 = Insert/return mouth block 
                                             returnMouthFlag = INSERT_RETURN_MOUTH_BLOCKED; 
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                             ////writeFileLog("[DispenseCard_c()] One Card on RF/IC Card Position.");
                                             AVRM_writeFileLog("[DispenseCard_c()] One Card on RF/IC Card Position.",INFO);
                                             // 1 = Channel blocked 
                                             channelBlockFlag = CHANNEL_BLOCKED;
                                        } 

                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											  ////writeFileLog("[DispenseCard_c()] No Card in Stacker");
											   AVRM_writeFileLog("[DispenseCard_c()] No Card in Stacker",INFO);
											   // 4 = Stacker Empty 
											   stackerEmptyFlag = 1;
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[DispenseCard_c()] Few Card in Stacker");
											   AVRM_writeFileLog("[DispenseCard_c()] Few Card in Stacker",INFO);
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[DispenseCard_c()] Enough Card in the Box");
											   AVRM_writeFileLog("[DispenseCard_c()] Enough Card in the Box",INFO);
									}   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[DispenseCard_c()] Error Card bin Not Full");
                                           AVRM_writeFileLog("[DispenseCard_c()] Error Card bin Not Full",INFO);
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[DispenseCard_c()] Error Card Bin Full");
                                           AVRM_writeFileLog("[DispenseCard_c()] Error Card Bin Full",INFO);
                                        }

                                }
                      }

                      ////////////////////////////////////////////////////////////////////////////////////////////////////
		      //Edited By: Abhishek Kole, 23-10-2017
 
                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                
                      ////////////////////////////////////////////////////////////
                      
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                       
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[DispenseCard_c()] Failed to send total Acknowledgement Command ");
		                    AVRM_writeFileLog("[DispenseCard_c()] Failed to send total Acknowledgement Command ",ERROR);
                            // 28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                      if(1 == returnMouthFlag)
                      { 
                      	    // 2 = Insert/return mouth block 
                            ////writeFileLog("[DispenseCard_c()] Insert/return mouth block return code: 3 ");
                            AVRM_writeFileLog("[DispenseCard_c()] Insert/return mouth block return code: 3 ",INFO);
                            return INSERT_RETURN_MOUTH_BLOCKED; 
                      }
                      if(1 == channelBlockFlag)
                      {    
                            // 1 = Channel blocked 
                            ////writeFileLog("[DispenseCard_c()] Channel blocked  return code: 2 ");
                             AVRM_writeFileLog("[DispenseCard_c()] Channel blocked  return code: 2 ",INFO);
                            return CHANNEL_BLOCKED;
                      }
                      if(1 == stackerEmptyFlag)
                      {
                            // 3 = Stacker Empty 
                            ////writeFileLog("[DispenseCard_c()] Stacker Empty return code: 4 ");
                            AVRM_writeFileLog("[DispenseCard_c()] Stacker Empty return code: 4 ",INFO);
                            return STACKER_EMPTY;  
                      }


       }
       //++If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             ////writeFileLog("[DispenseCard_c()] Nak Reply Received for device status command return code: 6");
             AVRM_writeFileLog("[DispenseCard_c()] Nak Reply Received for device status command return code: 6",ERROR);
			 SetLastError(99,0);
			 
             // 31 = Other Error 
             return OTHER_ERROR;  
       }
       else if (0x04 == rcvPkt[0])
       {
             ////writeFileLog("[DispenseCard_c()] EOT Reply Received for device status command return code: 6");
             AVRM_writeFileLog("[DispenseCard_c()] EOT Reply Received for device status command return code: 6",ERROR);
             // 31 = Other Error 
			 SetLastError(99,0);

             return OTHER_ERROR;  
       }


       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       ////writeFileLog("[DispenseCard_c()] All Status OK Now Going to Dispense Card");
       AVRM_writeFileLog("[DispenseCard_c()] All Status OK Now Going to Dispense Card",DEBUG);
       ////writeFileLog("[DispenseCard_c()] Going to send Dispense Card Command ");
       AVRM_writeFileLog("[DispenseCard_c()] Going to send Dispense Card Command ",DEBUG);
       
       memset(rcvPkt,'\0',24); 

       //reinit bcc value
       g_dispenseCard[8] = 0x00;

       bcc = getBcc(9,g_dispenseCard);

       g_dispenseCard[8] = bcc;
       
       memset(log,'\0',LOG_ARRAY_SIZE);

       sprintf(log,"[DispenseCard_c()] Dispence Command bcc value is 0x%xh.",bcc);
     
        AVRM_writeFileLog(log,DEBUG);

       //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_dispenseCard,
                          9
                        );

 
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		delay_mSec(g_MaxDelay);
		
       // Before Send Command clear all serial buffer 
       ClearReceiveBuffer (g_ComHandle);
       ClearTransmitBuffer(g_ComHandle);

       for(i=0;i<9;i++)
       {
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[DispenseCard_c()] Dispense Card Command[%d] = 0x%xh",i,g_dispenseCard[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_dispenseCard[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[DispenseCard_c()] Failed to Send Dispense Card Command Communication Failure return code: 1");
                    AVRM_writeFileLog("[DispenseCard_c()] Failed to Send Dispense Card Command Communication Failure return code: 1",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }
	}// for(i=0;i<9;i++)
        
        // Now going to Check Acknowledgement 
        totalByteToRecv = 1;
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        ////writeFileLog(log);


        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);

                 ClearTransmitBuffer(g_ComHandle);

                 ////writeFileLog("[DispenseCard_c()] check acknowledgement for dispense command Communication Failure return code:1");
                 
                 AVRM_writeFileLog("[DispenseCard_c()] check acknowledgement for dispense command Communication Failure return code:1",ERROR);
                 
                 return COMMUNICATION_FAILURE;
        }
        else
        {
                
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DispenseCard_c()] Acknowledgement against Dispense Card Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                 ////writeFileLog(log);

                 AVRM_writeFileLog(log,DEBUG);

        }

        // If Return Data is 06h then Going to Read 12byte Data 
        if(0x06 == rcvPkt[0])
        {

                 ////writeFileLog("[DispenseCard_c()] Acknowledgement Received for dispense command");
                 AVRM_writeFileLog("[DispenseCard_c()] Acknowledgement Received for dispense command",DEBUG);

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',24);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);
                 
                 // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      //Edited By: Abhishek Kole, 23-10-2017
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                       

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 
                 /*
                 for(i=0;i<rcvPktLen;i++)
	         {
				
                        memset(log,'\0',LOG_ARRAY_SIZE);

                        sprintf(log,"[DispenseCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                        //writeFileLog(log);

                 }
                 */
                 

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
                 if( 0 == rtcode ) //Timeout and no data receieved
                 {
                       
                       ////writeFileLog("[DispenseCard_c()] dispense Operation timeout for reply data receieved Communication Failure ");
                       AVRM_writeFileLog("[DispenseCard_c()] dispense Operation timeout for reply data receieved Communication Failure ",ERROR);
                       return COMMUNICATION_FAILURE;

                 }
                 else if( 1 == rtcode ) //data receieved
                 {

/*
                            if( ( 0x50 == rcvPkt[4] ) &&  //CMH
		                ( 0x32 == rcvPkt[5] ) &&  //CM
		                ( 0x32 == rcvPkt[6] )     //PM
		              )
*/			    if( 0x50 == rcvPkt[4] )
		            {
                                        if(0x30 == rcvPkt[7])
		                        {
                                             ////writeFileLog("[DispenseCard_c()] No Card in MTK-571 channel");   
                                             AVRM_writeFileLog("[DispenseCard_c()] No Card in MTK-571 channel",INFO);
                                             return OTHER_ERROR; //Other error
		                        } 
                                        else if(0x31 == rcvPkt[7])
		                        {
                                            
                                             ////writeFileLog("[DispenseCard_c()] Channel blocked  ");
											 AVRM_writeFileLog("[DispenseCard_c()] Channel blocked  ",INFO);
                                             return CHANNEL_BLOCKED;
                                             
		                        }
                                        else if(0x32 == rcvPkt[7])
		                        {
                                             ////writeFileLog("[DispenseCard_c()] One Card on RF/IC Card Position Operation successfully done");
                                             AVRM_writeFileLog("[DispenseCard_c()] One Card on RF/IC Card Position Operation successfully done",INFO);
                                             return SUCCESS;
                                        } 
/*
                                        else if(0x30 == rcvPkt[8])
		                        {
                                             //writeFileLog("[DispenseCard_c()] Stacker Empty");

                                             //return 4;
                                        } 
*/

                             }  //if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                //( 0x32 == RecvPackt[5] ) &&  //CM
		                //( 0x32 == RecvPackt[6] )     //PM
		                //)  
                            else if( ( 0x4e == rcvPkt[4] ) &&  //CMH
		                     ( 0x32 == rcvPkt[5] ) &&  //CM
		                     ( 0x32 == rcvPkt[6] )     //PM
		                  )
                            {

                                      ////writeFileLog("[DispenseCard_c()] Other error found when issue dispense card command.");
                                      AVRM_writeFileLog("[DispenseCard_c()] Other error found when issue dispense card command.",ERROR);
			  						  SetLastError(rcvPkt[7], rcvPkt[8]);

                                      return OTHER_ERROR; //Other Error
    
                            }      


                 }//else if( 1 == rtcode ) //data receieved

              

        }
        else if (0x15 == rcvPkt[0])
        {
             ////writeFileLog("[DispenseCard_c()] Nak Reply Received for dispense command return code:6");
             AVRM_writeFileLog("[DispenseCard_c()] Nak Reply Received for dispense command return code:6",ERROR);
 			 SetLastError(99,0);

             //31 = Other Error 
             return OTHER_ERROR;  
        }
        else if (0x04 == rcvPkt[0])
        {
             ////writeFileLog("[DispenseCard_c()] EOT Reply Received for dispense command return code:6");
             AVRM_writeFileLog("[DispenseCard_c()] EOT Reply Received for dispense command return code:6",ERROR);
			 SetLastError(99,0);

             //31 = Other Error 
             return OTHER_ERROR;  
        }

}//int DispenseCard_c(int Timeout) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function Name : RejectCard_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Rejection Bin Full
//                  3 = No Card in the Channel
//                  4 = Operation timeout Occurred 
//                  5 = Other Error
//                

// Parameters    : Name                           Type          Description 
                  
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								         operation otherwise return timeout status. 


int RejectCard_c( int   Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        g_getStatus[0] = 0xF2;
        g_getStatus[1] = MTK_DEVICE_ADDRESS;
        g_getStatus[2] = 0x00;
        g_getStatus[3] = 0x03;
        g_getStatus[4] = 0x43;
        g_getStatus[5] = 0x31;
        g_getStatus[6] = 0x30;
        g_getStatus[7] = 0x03;
        g_getStatus[8] = 0x00;

        g_moveToErrorBin[0]= 0xF2;
        g_moveToErrorBin[1]= MTK_DEVICE_ADDRESS;
        g_moveToErrorBin[2]= 0x00;
        g_moveToErrorBin[3]= 0x03;
        g_moveToErrorBin[4]= 0x43;
        g_moveToErrorBin[5]= 0x32;
        g_moveToErrorBin[6]= 0x33;
        g_moveToErrorBin[7]= 0x03;
        g_moveToErrorBin[8]= 0x00;

  
        char ChanelStatus =0x00;
        char MTKSensorStatus =0x00;

        char bcc=0x00;

        unsigned char rcvPkt[25];

        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,noCardInRF_IC = 0,rejectionBinFull = 0;

        unsigned int rcvPktLen =0x00; 

        ////writeFileLog("[RejectCard_c()] Going to send Current Report status of st0, st1, st2");
        
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = 0x00;
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[RejectCard_c()] bcc value is 0x%xh",bcc);
        
        AVRM_SCRD_writeFileLog(log,"DEBUG");


        ///////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////

		delay_mSec(g_MaxDelay);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);
                 //sprintf(log,"[RejectCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);        
                 //AVRM_SCRD_writeFileLog(log,"INFO");

				 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    AVRM_SCRD_writeFileLog("[RejectCard_c()] Failed to Send Report status Command Communication Failure return code: 1","ERROR");
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }
	}

        //AVRM_SCRD_writeFileLog("[RejectCard_c()] Status Command send Successfully","INFO");

        //AVRM_SCRD_writeFileLog("[RejectCard_c()] Now Going to read Acknowledgement","INFO");

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;
          
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        AVRM_SCRD_writeFileLog(log,"DEBUG");

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 AVRM_SCRD_writeFileLog("[RejectCard_c()]  Failed to recieved Acknowledgement Byte Communication Failure return code: 1","ERROR");
                 return COMMUNICATION_FAILURE;
        }
        else
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[RejectCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 AVRM_SCRD_writeFileLog(log,"DEBUG");
				
	}

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      AVRM_SCRD_writeFileLog("[RejectCard_c()] Acknowledgement Received","DEBUG");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                       
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      AVRM_SCRD_writeFileLog(log,"INFO");

                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);

				 ClearTransmitBuffer(g_ComHandle);   
         
                 AVRM_SCRD_writeFileLog("[RejectCard_c()]  Failed to recieved reply Byte report status command Communication Failure return code: 1","ERROR");

				 return COMMUNICATION_FAILURE;

		      }    

                      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      //Check Negative Reply bytes
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[RejectCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              AVRM_SCRD_writeFileLog(log,"ERROR");
                               
	  						  SetLastError(rcvPkt[7], rcvPkt[8]);

/*
		              //Force Card Detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
                                  ( 0x36 == rcvPkt[8] ) )
			      {
		                       //writeFileLog("[RejectCard_c()] Negative response receieved for issue device status command");  
	   
		                       if( 1 == MutekInitWithErrorCardBin( LogdllPathstr,LogFileName,deviceid,fnLogfileMode) )
		                       {
		                            
                                                //writeFileLog("[RejectCard_c()] Successfully removed block card so operation successfully done"); 
   
		                                return 0; //Operation successfull card return to collection bin successfully

		                       }
		                       else
		                       {
                                              //writeFileLog("[RejectCard_c()] Failed to do operation for block card status other error"); 
   
		                               return 5; //Operation not successfull so return operation timeout happened

		                       }


					 
		              }
*/
			      return OTHER_ERROR;  

                      }//if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[RejectCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           AVRM_SCRD_writeFileLog("[RejectCard_c()] No Card in MTK-571","INFO");
                                           // 3 = No Card in the Channel 
                                           noCardInRF_IC = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                        
                                          AVRM_SCRD_writeFileLog("[RejectCard_c()] One Card in Gate","INFO");
                                                                              
                  			}
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           AVRM_SCRD_writeFileLog("[RejectCard_c()] One Card on RF/IC Card Position","INFO");
                                            
                                           
		                        } 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           AVRM_SCRD_writeFileLog("[RejectCard_c()] No Card in Stacker","INFO");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           AVRM_SCRD_writeFileLog("[RejectCard_c()] Few Card in Stacker","INFO");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           AVRM_SCRD_writeFileLog("[RejectCard_c()] Enough Card in the Box","INFO");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                          AVRM_SCRD_writeFileLog("[RejectCard_c()] Error Card bin Not Full","ERROR");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           AVRM_SCRD_writeFileLog("[RejectCard_c()] Error Card Bin Full","ERROR");
                                           // 2 = Rejection Bin Full  
                                           rejectionBinFull = 1;
		                        }
                                        

                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++) block

                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
//                      Edited By: Abhishek Kole, 23-10-2017
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                      
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            AVRM_SCRD_writeFileLog("[RejectCard_c()] Failed to send Acknowledgement byte command Communication Failure return code:1","ERROR");
                            // 28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                      if(1 == noCardInRF_IC)
                      {
                           // 2 = No Card in the Channel 
                          AVRM_SCRD_writeFileLog("[RejectCard_c()] No Card in Chanel return code:3","INFO");
                           return NO_CARD_IN_CHANNEL;
                      }
                      if(1 == rejectionBinFull)
                      {
                           // 1 = Rejection Bin Full  
                           AVRM_SCRD_writeFileLog("[RejectCard_c()] Rejection Bin Full return code:2","INFO");
                           return REJECTION_BIN_FULL;

                      }
                      
                      
                       
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             AVRM_SCRD_writeFileLog("[RejectCard_c()] Device status Command Nak Reply Received return code:5","ERROR");
 			 SetLastError(99,0);

             // 31 = Other Error 
             return OTHER_ERROR;  
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x04 == rcvPkt[0])
         {
             AVRM_SCRD_writeFileLog("[RejectCard_c()] Device status Command EOT Reply Received return code:5","ERROR");
			 SetLastError(99,0);

             // 31 = Other Error 
             return OTHER_ERROR;  
         }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //++Now Check sensor status
        if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
        {

            if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
            {
                AVRM_SCRD_writeFileLog("[RejectCard_c()] Sensor 1 Status Found Blocked As Card in Mouth:6 returned","ERROR");
                //  Card at Gate 
                return OTHER_ERROR;  
            }//if(  1 == (MTKSensorStatus & 0x01) ) block end

        }//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end

 


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         // All status ok and Going to issue Reject Card 

         ////writeFileLog("[RejectCard_c()] Going to send Reject Card Comamnd");

         //Reinit bcc value
         g_moveToErrorBin[8] =0x00;

         bcc = getBcc(9,g_moveToErrorBin);

         g_moveToErrorBin[8] = bcc;
         
         memset(log,'\0',LOG_ARRAY_SIZE);

         sprintf(log,"[RejectCard_c()] bcc value is 0x%xh",bcc);
        
         AVRM_SCRD_writeFileLog(log,"INFO");
        

        ///////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_moveToErrorBin,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////



        // Before Send Command clear all serial buffer 

        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[RejectCard_c()] Reject Card Command[%d] = 0x%xh",i,g_moveToErrorBin[i]);
        
                 

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_moveToErrorBin[i]);
                 if(0 == send_rvalue) 
                 {
                    AVRM_SCRD_writeFileLog("[RejectCard_c()] Failed to Send Reject Card Command return code:1","INFO");
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }
	}

        AVRM_SCRD_writeFileLog("[RejectCard_c()] Reject Card Command send Successfully","INFO");

        AVRM_SCRD_writeFileLog("[RejectCard_c()] Now Going to read Acknowledgement","INFO");

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        AVRM_SCRD_writeFileLog(log,"DEBUG");

        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);

                 ClearTransmitBuffer(g_ComHandle);

                 AVRM_SCRD_writeFileLog("[RejectCard_c()] Failed to check Acknowledgement byte for reject command return code:1","ERROR");

                 return COMMUNICATION_FAILURE;

        }
        else  if(0x06 == rcvPkt[0]) //ack byte receieved
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[RejectCard_c()] Acknowledgement against Reject Card Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 AVRM_SCRD_writeFileLog(log,"DEBUG");

                 totalByteToRecv = 12;

                 rcvPktLen = 0;

                 memset(rcvPkt,'\0',24); 
         
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                 /////////////////////////////////////////////////////////////////////////////////

                 if(0 == rtcode)
		 {
		      // 28 = Communication Failure 
		      ClearReceiveBuffer (g_ComHandle);
		      ClearTransmitBuffer(g_ComHandle);
                      AVRM_SCRD_writeFileLog("[RejectCard_c()] Communication Failure when read reject command reply bytes.","ERROR");
		      return COMMUNICATION_FAILURE;
		 }  
               
                 // Now time to send Acknowledgement 
                      int send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      //Edited By: Abhishek Kole, 23-10-2017
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 if(0 == send_rvalue) 
		 {
		       AVRM_SCRD_writeFileLog("[RejectCard_c()] Failed to send Acknowledgement byte for reject command .","ERROR");
                          
		 }

                 //////////////////////////////////////////////////////////////////////////////////
           
                 //Check Negative Reply bytes
                 if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                 {

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[RejectCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                      AVRM_SCRD_writeFileLog(log,"ERROR"); 

                      AVRM_SCRD_writeFileLog("[RejectCard_c()] Reject Failed to do.","ERROR");
					  SetLastError(rcvPkt[7], rcvPkt[8]);

		      return OTHER_ERROR;//other error

                 }
                 else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                          ( 0x32 == rcvPkt[5] ) && ( 0x33 == rcvPkt[6] ) )
                 {
                
		         AVRM_SCRD_writeFileLog("[RejectCard_c()] Reject Successfully done.","INFO");

		         return SUCCESS;
                 }
                 else
                 {
                         AVRM_SCRD_writeFileLog("[RejectCard_c()] Reject Failed to do.","ERROR");
			 			 SetLastError(99,0);

						 return OTHER_ERROR; //Other Error
                 }
                
				
	}
        // This function does not end here because Rejection and collection bin are not same 
        else  if(0x15 == rcvPkt[0]) //nak byte receieved
	{
              AVRM_SCRD_writeFileLog("[RejectCard_c()] Reject Failed to do a nak receieved.","ERROR");
              SetLastError(99,0);

              return OTHER_ERROR; //Other Error

        }
        else  if(0x04 == rcvPkt[0]) //ack byte receieved
	{
              AVRM_SCRD_writeFileLog("[RejectCard_c()] Reject Failed to do a eot receieved.","ERROR");
              SetLastError(99,0);

              return OTHER_ERROR; //Other Error

        }
      


}//int RejectCard_c(int Timeout) end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function Name : ReturnCard_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Return mouth block
//                  3 = No Card in the Channel
//                  4 = Operation timeout Occurred 
//                  5 = Other Error
// Parameters    : Name                           Type          Description 
                  
//                  DispenseMode                   int           0 = Hold at the mouth of the device until it taken by the Customer 
//                                                               1 = Dispense it immediately  
                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

// To Hold at Mouth
static unsigned char g_holdAtMouth[9]={0xF2,0x00,0x00,0x03,0x43,0x32,0x30,0x03,0x00};

int ReturnCard_c( int   DispenseMode,int   Timeout,char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode) {

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        char ChanelStatus =0x00,MTKSensorStatus =0x00;

        memset(log,'\0',LOG_ARRAY_SIZE);

        g_getStatus[0]= 0xF2 ;
        g_getStatus[1]= MTK_DEVICE_ADDRESS ;
        g_getStatus[2]= 0x00 ;
        g_getStatus[3]= 0x03 ;
        g_getStatus[4]= 0x43 ; 
        g_getStatus[5]= 0x31 ;
        g_getStatus[6]= 0x30 ;
        g_getStatus[7]= 0x03 ;
        g_getStatus[8]= 0x00 ;

        /*
        g_moveToErrorBin[0]= 0xF2;
        g_moveToErrorBin[1]= MTK_DEVICE_ADDRESS;
        g_moveToErrorBin[2]= 0x00;
        g_moveToErrorBin[3]= 0x03;
        g_moveToErrorBin[4]= 0x43;
        g_moveToErrorBin[5]= 0x32;
        g_moveToErrorBin[6]= 0x33;
        g_moveToErrorBin[7]= 0x03;
        g_moveToErrorBin[8]= 0x00;
        */

        char MovetoGateCommand[9];

        memset(MovetoGateCommand,'\0',9);

        MovetoGateCommand[0]= 0xF2;
        MovetoGateCommand[1]= MTK_DEVICE_ADDRESS;
        MovetoGateCommand[2]= 0x00;
        MovetoGateCommand[3]= 0x03;
        MovetoGateCommand[4]= 0x43;
        MovetoGateCommand[5]= 0x32;
        MovetoGateCommand[6]= 0x39;
        MovetoGateCommand[7]= 0x03;
        MovetoGateCommand[8]= 0x00;

        char bcc=0x00;
        unsigned char rcvPkt[25],DispenseModeData[9];
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,returnMouthFlag = 0,noCardInChannelFlag = 0;
        int oneCardInRF_IC = 0,ErrorBinFull = 0;
        unsigned int rcvPktLen =0x00; 

        ////writeFileLog("[ReturnCard_c()] Going to send Current Report status of st0, st1, st2");
        AVRM_writeFileLog("[ReturnCard_c()] Going to send Current Report status of st0, st1, st2",INFO);

        bcc = getBcc(9,g_getStatus);
	    g_getStatus[8]= 0x00 ;
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[ReturnCard_c()] bcc value is 0x%xh",bcc);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,INFO);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		delay_mSec(g_MaxDelay);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[ReturnCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[ReturnCard_c()] Failed to Send Report status Command Communication Failure rtcode:1");
                    AVRM_writeFileLog("[ReturnCard_c()] Failed to Send Report status Command Communication Failure rtcode:1",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }


	}//for(i=0;i<9;i++) end

        ///////////////////////////////////////////////////////////////////////////////////////////

       
		 //AVRM_writeFileLog("[ReturnCard_c()] Status Command send Successfully",DEBUG);
         //AVRM_writeFileLog("[ReturnCard_c()] Now Going to read Acknowledgement",DEBUG);

         //Now going to Check Acknowledgement  

         totalByteToRecv = 1;

         rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

         memset(log,'\0',LOG_ARRAY_SIZE);

         sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
         ////writeFileLog(log);
         
         AVRM_writeFileLog(log,DEBUG);

         if(0 == rtcode)
         {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[ReturnCard_c()] Acknowledgement Failed to Received for Report status Command.");
                 AVRM_writeFileLog("[ReturnCard_c()] Acknowledgement Failed to Received for Report status Command.",ERROR);
                 return COMMUNICATION_FAILURE;
         }
         else
	 {
		
               memset(log,'\0',LOG_ARRAY_SIZE);

               sprintf(log,"[ReturnCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
        
               ////writeFileLog(log);

			   AVRM_writeFileLog(log,DEBUG);
	 }

         // If Return Data is 06h then Going to Read 25byte Data 

         if(0x06 == rcvPkt[0])
         {

                      //////writeFileLog("[ReturnCard_c()] Acknowledgement Received for Report status Command.");
                      AVRM_writeFileLog("[ReturnCard_c()] Acknowledgement Received for Report status Command.",DEBUG);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);
                      
                      AVRM_writeFileLog(log,DEBUG);

                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[ReturnCard_c()] Communication Failure when read status command reply bytes.");
                                 AVRM_writeFileLog("[ReturnCard_c()] Communication Failure when read status command reply bytes.",ERROR);
				 return COMMUNICATION_FAILURE;
		      }  
           
                      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Check Negative Reply bytes
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[ReturnCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              ////writeFileLog(log); 
                              
                              AVRM_writeFileLog(log,ERROR);
                              
	  						  SetLastError(rcvPkt[7], rcvPkt[8]);


		              //FOrce card detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
		                  ( 0x36 == rcvPkt[8] ) )
			      {
		                         ////writeFileLog("[ReturnCard_c()] Negative response receieved for issue device status command"); 
		                         AVRM_writeFileLog("[ReturnCard_c()] Negative response receieved for issue device status command",ERROR); 
	   
		                         if( 1 == MutekInitWithCardMoveInFront( LogdllPathstr,
                                             LogFileName,deviceid,fnLogfileMode) )
		                         {
                                                ////writeFileLog("[ReturnCard_c()] operation successfully and return blocked card.");
		                                        AVRM_writeFileLog("[ReturnCard_c()] operation successfully and return blocked card.",INFO); 
		                                return SUCCESS; //Operation successfull card return successfull

		                         }
		                         else
		                         { 
                                               ////writeFileLog("[ReturnCard_c()] retrun mouth blocked.");
                                               AVRM_writeFileLog("[ReturnCard_c()] retrun mouth blocked.",INFO); 
		                               return INSERT_RETURN_MOUTH_BLOCKED; //return mouth blocked

		                         }

					 
		              }
                              else
                              {
                                  ////writeFileLog("[ReturnCard_c()] Other Error found when issue  device status command.");
                                  AVRM_writeFileLog("[ReturnCard_c()] Other Error found when issue  device status command.",ERROR); 
    				              SetLastError(99,0);

                                  return OTHER_ERROR;//other error
 
                              }
//			   return 5; //Other error
                      }//if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[ReturnCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                ////writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[ReturnCard_c()] No Card in MTK-571.");
                                           AVRM_writeFileLog("[ReturnCard_c()] No Card in MTK-571.",INFO); 
                                           // 4 = No Card in the Channel  
                                           noCardInChannelFlag = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[ReturnCard_c()] One Card in Gate.");
                                            AVRM_writeFileLog("[ReturnCard_c()] One Card in Gate.",INFO); 
                                           // 2 = Return mouth block 
                                           returnMouthFlag = 1;
                  			   
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           ////writeFileLog("[ReturnCard_c()] One Card on RF/IC Card Position.");
                                           AVRM_writeFileLog("[ReturnCard_c()] One Card on RF/IC Card Position.",INFO); 
                                           oneCardInRF_IC = 1;
                                           
		                        } 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[ReturnCard_c()] No Card in Stacker.");
											   AVRM_writeFileLog("[ReturnCard_c()] No Card in Stacker.",INFO); 
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[ReturnCard_c()] Few Card in Stacker.");
											   AVRM_writeFileLog("[ReturnCard_c()] Few Card in Stacker.",INFO); 
									}
											else if(0x32 == rcvPkt[i])
									{
											   ////writeFileLog("[ReturnCard_c()] Enough Card in the Box.");
											   AVRM_writeFileLog("[ReturnCard_c()] Few Card in Stacker.",INFO); 
									}   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
									if(0x30 == rcvPkt[i])
									{
											   ////writeFileLog("[ReturnCard_c()] Error Card bin Not Full.");
											    AVRM_writeFileLog("[ReturnCard_c()] Error Card bin Not Full..",ERROR); 
									}
											else if(0x31 == rcvPkt[i])
									{
											   ////writeFileLog("[ReturnCard_c()] Error Card Bin Full.");
											   AVRM_writeFileLog("[ReturnCard_c()] Error Card Bin Full.",ERROR); 
											   ErrorBinFull = 1; 
											   
									}
                                        

                                }
                                
				
	              }

                      /////////////////////////////////////////////////////////////////////////////////////////////////

                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      //Edited By: Abhishek Kole, 23-10-2017
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////
                       
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            ////writeFileLog("[ReturnCard_c()] Failed to send Acknowledgement byte for report status command return code: 1.");
		                    AVRM_writeFileLog("[ReturnCard_c()] Failed to send Acknowledgement byte for report status command return code: 1.",ERROR); 
                            // 28 = Communication Failure 
                            return COMMUNICATION_FAILURE;
		      }
                      else if(1 == noCardInChannelFlag)
                      {
                           // 2 = No Card in the Channel 
                           ////writeFileLog("[ReturnCard_c()] No Card in Channel ");
                            AVRM_writeFileLog("[ReturnCard_c()] No Card in Channel ",INFO); 
			   if(0 == DispenseMode)
                           	return NO_CARD_IN_CHANNEL; 
                        
                      }
                      else if(1 == returnMouthFlag)
                      {
                           // 1 = Return mouth block 
                           ////writeFileLog("[ReturnCard_c()] Return Mouth Block return code:2.");
                           AVRM_writeFileLog("[ReturnCard_c()] Return Mouth Block return code:2.",INFO); 
			   if(0 == DispenseMode)
                           	return INSERT_RETURN_MOUTH_BLOCKED; 
                      }
/*
                      else if( 1 == ErrorBinFull )
                      {
                           // 2 = Rejection Bin full
                           //writeFileLog("[ReturnCard_c()] Rejection bin full:2.");
                           return 2; 

                      }
*/                      
                       
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             ////writeFileLog("[ReturnCard_c()] Nak Reply Received for report status command return code:5.");
             AVRM_writeFileLog("[ReturnCard_c()] Nak Reply Received for report status command return code:5.",ERROR); 
             SetLastError(99,0);

             // 31 = Other Error 
             return OTHER_ERROR;  
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x04 == rcvPkt[0])
         {
             ////writeFileLog("[ReturnCard_c()] EOT Reply Received for report status command return code:5.");
             AVRM_writeFileLog("[ReturnCard_c()] EOT Reply Received for report status command return code:5.",ERROR); 
             SetLastError(99,0);
             // 31 = Other Error 
             return OTHER_ERROR;  
         }


         ///////////////////////////////////////////////////////////////////////////////////////////////

         //Issue sensor status command
        
         //Now Check sensor status

         if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
         {

            if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
            {
                ////writeFileLog("[ReturnCard_c()] Sensor 1 Status Found Blocked As Card in Mouth Found:2");
                 AVRM_writeFileLog("[ReturnCard_c()] Sensor 1 Status Found Blocked As Card in Mouth Found:2",ERROR); 
                // 2 = Card in Mouth
               //return 2;  //This is blocked as per CRIS request dated 13-09-2016. Only log will reflect the status


            }//if(  1 == (MTKSensorStatus & 0x01) ) block end
 
        }//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end

         ///////////////////////////////////////////////////////////////////////////////////////////////


         // Return Mouth not blocked and No card in Channel So going to check one card in One Card on RF/IC Card Position or Not 
         if(0 == DispenseMode) //Hold at the mouth untill it is taken by out by customer
         {
            
		    ////writeFileLog("[ReturnCard_c()] Hold at the Mouth untill it is taken by the customer.");
		    AVRM_writeFileLog("[ReturnCard_c()] Hold at the Mouth untill it is taken by the customer.",INFO); 
                    if( 0 == oneCardInRF_IC )
		    {
		       // 31 = Other Error 
		       ////writeFileLog("[ReturnCard_c()] Unable to Dispense Error No Card in RF/IC ");  
		       AVRM_writeFileLog("[ReturnCard_c()] Unable to Dispense Error No Card in RF/IC ",ERROR); 
		       return OTHER_ERROR;
		      
		    }
		    else if(1 == oneCardInRF_IC)
		    {
		            // Found one Card in RF/IC Reader 
		            ////writeFileLog("[ReturnCard_c()] Got One Card in RF/IC Reader and Going to return from Mouth.");
		            AVRM_writeFileLog("[ReturnCard_c()] Got One Card in RF/IC Reader and Going to return from Mouth",INFO);
		            // Going to issue command to send the card to Mouth */ 
		        
		            // Now going to get bcc 
			    ////writeFileLog("[ReturnCard_c()] Going to get bcc for Hold at Mouth.");

		            //Reinit bcc value
		            g_holdAtMouth[8] =0x00;

			    bcc = getBcc(9,g_holdAtMouth);
			    
			    g_holdAtMouth[8] = bcc;
			    
		            memset(log,'\0',LOG_ARRAY_SIZE);

		            sprintf(log,"[ReturnCard_c()] bcc value is 0x%xh.",bcc);
		
		            ////writeFileLog(log);
		            
		            AVRM_writeFileLog(log,DEBUG);

			    memset(DispenseModeData,'\0',9);

			    for(i=0;i<9;i++)
			    {
				     DispenseModeData[i]=g_holdAtMouth[i];

			    }

		    }//else if(1 == oneCardInRF_IC) end

         } 
         else if(1 == DispenseMode) //Dispense it immediately
         {
		    ////writeFileLog("[ReturnCard_c()] Dispense it immediately.");
		    AVRM_writeFileLog("[ReturnCard_c()] Dispense it immediately.",INFO);
		    
		    if( 0 == oneCardInRF_IC )
		    {
		       // 3 = No card at RF position 
		       ////writeFileLog("[ReturnCard_c()] No Card in RF/IC .");   
		       AVRM_writeFileLog("[ReturnCard_c()] No Card in RF/IC .",INFO); 
//		       return 3;
		      
		    }
/*
                    char buff[64]={0};
                    sprintf(buff,"[ReturnCard_c()] %d, %d ",oneCardInRF_IC,returnMouthFlag);
		    //writeFileLog(buff);
*/
                    if(( 1 == oneCardInRF_IC ) || ( 1 == returnMouthFlag))//Found one card in RF/IC Reader
		    {
		           
		            // Found one Card in RF/IC Reader 
		            ////writeFileLog("[ReturnCard_c()] Got One Card in RF/IC Reader and Going to Send to Error bin.");
		            AVRM_writeFileLog("[ReturnCard_c()] Got One Card in RF/IC Reader and Going to Send to Error bin.",INFO); 
		            // Going to issue command to send the card to error bin */
		       
		            // Now going to get bcc 
			    ////writeFileLog("[ReturnCard_c()] Going to get bcc for Send to Error bin .");
			    AVRM_writeFileLog("[ReturnCard_c()] Going to get bcc for Send to Error bin .",INFO); 

			    bcc = getBcc(9,MovetoGateCommand);

			    MovetoGateCommand[8] = bcc;
			    
		            memset(log,'\0',LOG_ARRAY_SIZE);

		            sprintf(log,"[ReturnCard_c()] bcc value is 0x%xh",bcc);
		
		            ////writeFileLog(log);
		            
		            AVRM_writeFileLog(log,DEBUG);

			    memset(DispenseModeData,'\0',9);

			    for(i=0;i<9;i++)
			    {
				DispenseModeData[i]=MovetoGateCommand[i];
			    }

		    }//if( 1 == oneCardInRF_IC ) || ( 1 == returnMouthFlag) //Found one card in RF/IC Reader or mouth

         }
         else
         {
            ////writeFileLog("[ReturnCard_c()] Function parameter DispenseMode not Ok.");
            AVRM_writeFileLog("[ReturnCard_c()] Function parameter DispenseMode not Ok.",INFO);
            // 31 = Other Error 
            return OTHER_ERROR;
         }

        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_SCRDDeviceid,
                          "Tx",
                          DispenseModeData,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        // All status Ok Now going to issue Dispense Mode Command 
        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[ReturnCard_c()] Dispense Mode Command[%d] = 0x%xh",i,DispenseModeData[i]);
        
                 ////writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,DispenseModeData[i]);

                 if(0 == send_rvalue) 
                 {
                    ////writeFileLog("[ReturnCard_c()] Failed to Send Dispense Mode Command Communication Failure.");
                    AVRM_writeFileLog("[ReturnCard_c()] Failed to Send Dispense Mode Command Communication Failure.",ERROR);
                    // 28 = Communication Failure 
                    return COMMUNICATION_FAILURE;
                 }


	}//for block

        ////writeFileLog("[ReturnCard_c()] Dispense Mode Command send Successfully.");
        AVRM_writeFileLog("[ReturnCard_c()] Dispense Mode Command send Successfully.",DEBUG);
        ////writeFileLog("[ReturnCard_c()] Now Going to read Acknowledgement.");
        AVRM_writeFileLog("[ReturnCard_c()] Now Going to read Acknowledgement.",DEBUG);
        
        // Now going to Check Acknowledgement  

        totalByteToRecv = 1; 

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d.",rtcode,rcvPktLen);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);
	
	
        if(0 == rtcode)
        {
                 // 28 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 ////writeFileLog("[ReturnCard_c()] Ack bytes for return command read failed.");
                 AVRM_writeFileLog("[ReturnCard_c()] Ack bytes for return command read failed.",ERROR);
                 return COMMUNICATION_FAILURE;
        }
        else
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[ReturnCard_c()] Acknowledgement against Dispense Mode Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 ////writeFileLog(log);
                 
                 AVRM_writeFileLog(log,DEBUG);
				
	} 

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      ////writeFileLog("[ReturnCard_c()] Acknowledgement Received");
                      AVRM_writeFileLog("[ReturnCard_c()] Acknowledgement Received",DEBUG);

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                       
                      //memset(log,'\0',LOG_ARRAY_SIZE);

                      //sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      ////writeFileLog(log);
                      
                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      
                      ////////////////////////////////////////////////////////////
                      //Edited By: Abhishek Kole, 23-10-2017
		              //Log ACK byte upon receieve complete data from mutek
		              
		              char ack_Data[1]={0x06};
		              if(1 == send_rvalue) {
		              
				      SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
						       g_SCRDDeviceid,
						       "Tx",
						       ack_Data,
						       1
				                     );
		              
		              }
		              
                       //////////////////////////////////////////////////////////////

                      if(0 == rtcode)
		      {
				 // 28 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 ////writeFileLog("[ReturnCard_c()] Reply bytes for return command read failed.");
                                 AVRM_writeFileLog("[ReturnCard_c()] Reply bytes for return command read failed.",ERROR);
				 return COMMUNICATION_FAILURE;
		      }    

                      /*
         
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                memset(log,'\0',LOG_ARRAY_SIZE);

                                sprintf(log,"[ReturnCard_c()] Initialize Command Dispense Mode Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                      }

                      */
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[ReturnCard_c()] Got 0x4e Error with E1:= 0x%xh and E0:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              ////writeFileLog(log); 
                              
                              AVRM_writeFileLog(log,ERROR);
                              
	  						  SetLastError(rcvPkt[7], rcvPkt[8]);
     
/*
                              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
		                  ( 0x36 == rcvPkt[8] ) )
                              {
                                  //writeFileLog("[ReturnCard_c()] Return mouth blcoked.");
                                  return 2;

                              }
                              else
                              {
                                  //writeFileLog("[ReturnCard_c()] Other error occured.");
                                  return 5; //Other error

                              }
*/
			      return OTHER_ERROR; //Other error
                      }
                      //Hold at mouth : 0x50,0x32,0x30
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] )  && 
                               ( 0x32 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] ) )
                      {
                          // 0 = Operation Successful 
                          ////writeFileLog("[ReturnCard_c()] Hold at Mouth Return Operation Successful done");
                          AVRM_writeFileLog("[ReturnCard_c()] Hold at Mouth Return Operation Successful done",INFO);
                          return SUCCESS;


                      }

                      //Throw card :0x43 0x32 0x39;
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] )  && 
                               ( 0x32 == rcvPkt[5] ) && ( 0x39 == rcvPkt[6] ) )
                      {
                          // 0 = Operation Successful 
                          ////writeFileLog("[ReturnCard_c()] Throw Card Return Operation Successful done");
                          AVRM_writeFileLog("[ReturnCard_c()] Throw Card Return Operation Successful done",INFO);
                          return SUCCESS;


                      }
/*
                      else 
                      {
                          //writeFileLog("[ReturnCard_c()] Return failed other reason");
                          return 5; //Other Error

                      }
*/
                      
        }
        // If Return Data is 15h then No need to read Data 
        else if (0x15 == rcvPkt[0])
        {
             ////writeFileLog("[ReturnCard_c()] Nak Reply Received for return command");
             AVRM_writeFileLog("[ReturnCard_c()] Nak Reply Received for return command",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }
        else if (0x04 == rcvPkt[0])
        {
             ////writeFileLog("[ReturnCard_c()] EOT Reply Received for return command");
             AVRM_writeFileLog("[ReturnCard_c()] EOT Reply Received for return command",ERROR);
             // 31 = Other Error 
             return OTHER_ERROR;  
        }





}//int ReturnCard_c(int DispenseMode,int Timeout) end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int IsAnyCardPresentInReaderPosition() {

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);
        
        //Construct Contact Less Card Activate Commnad
        g_getStatus[0]  = 0xF2; //STX
        g_getStatus[1]  = MTK_DEVICE_ADDRESS; //ADDR
        g_getStatus[2]  = 0x00; //LENH
        g_getStatus[3]  = 0x03; //LENL
        g_getStatus[4]  = 0x43; //CMH
        g_getStatus[5]  = 0x31; //CM
        g_getStatus[6]  = 0x30; //PM
        g_getStatus[7]  = 0x03; //ETX
        g_getStatus[8]  = 0x00; //BCC
        
        /////////////////////////////////////////////////////////////////////////////////

        
              //Reply Byte:

              //STX    = 1byte
              //ADDR   = 1 byte
              //LENGTH = 2 byte
              //CMH    = 1 byte
              //CM     = 1 byte
              //PM     = 1 byte
              //ST0    = 1 byte
              //ST1    = 1 byte
              //ST2    = 1 byte
              //ETX    = 1 byte
              //BCC    = 1 byte
    
              //Total Reply Byte = 30 Byte
        


        /////////////////////////////////////////////////////////////////////////////////

        int CommandLength = 9;

        char RecvPackt[100];

        int totalByteToRecv=12; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        {

            //Step 2: Wait for ACK Byte
            if( true ==  IsAckReceieve() )
            {
                 //Step 3: After Receieve ACK Go for read reply bytes
                 if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
                 {
                        if( 0x50 == RecvPackt[4] )
                        {
		                   if( 0x32 == RecvPackt[7] ) //ST0 Index
		                   {
		                      return 1; //Card in RF/IC Read Position
		                   }
		                   else if( 0x31 == RecvPackt[7] )
		                   {

		                      return 2; //Card in Gate
		                   }
		                   else if( 0x30 == RecvPackt[7] )
		                   {

		                      return 3; //NO Card Present
		                   }
		                   

                        }
                        else if( 0x4E == RecvPackt[4] )
                        {
                                  return 4; //communication failure

                        }
                         
                 }
                 else //failure case
                 {
                    return 4; //communication failure

                 }//else block


           }//if( true ==  IsAckReceieve() )
           else
           {
               return 4; //communication failure
           }

       }
       else
       {

              return 4; //communication failure

       }


}//bool IsAnyCardPresentInReaderPosition() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int CheckCardType() {

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        //Construct Contact Less Card Activate Commnad
        g_getStatus[0]  = 0xF2; //STX
        g_getStatus[1]  = MTK_DEVICE_ADDRESS; //ADDR
        g_getStatus[2]  = 0x00; //LENH
        g_getStatus[3]  = 0x03; //LENL
        g_getStatus[4]  = 0x43; //CMH
        g_getStatus[5]  = 0x50; //CM
        g_getStatus[6]  = 0x31; //PM
        g_getStatus[7]  = 0x03; //ETX
        g_getStatus[8]  = 0x00; //BCC
        
        /////////////////////////////////////////////////////////////////////////////////

        
              //Reply Byte:

              //STX    = 1byte
              //ADDR   = 1 byte
              //LENGTH = 2 byte
              //CMH    = 1 byte
              //CM     = 1 byte
              //PM     = 1 byte
              //ST0    = 1 byte
              //ST1    = 1 byte
              //ST2    = 1 byte
              //CARD_TYPE = 2 byte
              //ETX    = 1 byte
              //BCC    = 1 byte
    
              //Total Reply Byte = 14 Byte
        


        /////////////////////////////////////////////////////////////////////////////////

        int CommandLength = 9;

        char RecvPackt[100];

        int totalByteToRecv=14; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        {
           

            //Step 2: Wait for ACK Byte
            if( true ==  IsAckReceieve() )
            {
                 //Step 3: After Receieve ACK Go for read reply bytes
                 if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
                 {
                        if( 0x50 == RecvPackt[4] )
                        {
		                   if( (0x32 == RecvPackt[10]) && (0x30 == RecvPackt[11]) )
		                   {
		                      return TYPA_A_CPU_CARD; //Mifare Type A Card
		                   }
		                   else if( (0x33 == RecvPackt[10]) && (0x30 == RecvPackt[11]) )
		                   {

		                      return TYPA_B_CPU_CARD; //Mifare Type B Card
		                   }
		                   else if( (0x30 == RecvPackt[10]) && (0x30 == RecvPackt[11]) )
		                   {

		                      return UNKNOWN_CARD; //Unknown card
		                   }
                                   else if( (0x31 == RecvPackt[10]) && (0x30 == RecvPackt[11]) )
		                   {

		                      return MIFARE_S50_CARD; //Mifare S50 Card
		                   }
		                   else if( (0x31 == RecvPackt[10]) && (0x31 == RecvPackt[11]) )
		                   {

		                      return MIFARE_S70_CARD; //Mifare S70 Card
		                   }
		                   else if( (0x31 == RecvPackt[10]) && (0x32 == RecvPackt[11]) )
		                   {

		                      return MIFARE_UL_CARD; //Mifare UL Card
		                   }
                                   else
                                   {
                                      return UNKNOWN_CARD;

                                   }

                        }
                        else if( 0x4E == RecvPackt[4] )
                        {
                                  return 4; //communication failure

                        }
                         
                 }
                 else //failure case
                 {
                    return 4; //communication failure

                 }//else block


           }//if( true ==  IsAckReceieve() )
           else
           {
               return 4; //communication failure
           }

       }
       else
       {

              return 4; //communication failure

       }




}//CheckCardType() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// RecvPackt[0] = 0xfffffff2. //STX
// RecvPackt[1] = 0x0.        //ADDR
// RecvPackt[2] = 0x0.  	  //Length High
// RecvPackt[3] = 0x18. 	  //Length Low
// RecvPackt[4] = 0x50. 	  //CMH
// RecvPackt[5] = 0x60. 	  //Command
// RecvPackt[6] = 0x30. 	  //Command Parameter

// RecvPackt[7] = 0x32. //ST0
// RecvPackt[8] = 0x30. //ST1
// RecvPackt[9] = 0x30. //ST2

//Card Type
// RecvPackt[10] = 0x41. //Rtype

// Card Name
// RecvPackt[11] = 0x3.  //ATQ First Byte  
// RecvPackt[12] = 0x44. //ATQ Second Byte

// RecvPackt[13] = 0x7.  	   //UID Length
// RecvPackt[14] = 0x4.  	   //UID_Data 
// RecvPackt[15] = 0xe.  	   //UID_Data 
// RecvPackt[16] = 0x23. 	   //UID_Data 
// RecvPackt[17] = 0xffffffa9. //UID_Data 
// RecvPackt[18] = 0xffffffff. //UID_Data 
// RecvPackt[19] = 0x1c.       //UID_Data 
// RecvPackt[20] = 0xffffff80. //UID_Data 

// RecvPackt[21] = 0x20.       //SAK byte
// RecvPackt[22] = 0x6.        //Sak BYte

// RecvPackt[23] = 0x75.       //ATS Byte
// RecvPackt[24] = 0x77.       //ATS Byte
// RecvPackt[25] = 0xffffff81. //ATS Byte
// RecvPackt[26] = 0x2.        //ATS Byte
// RecvPackt[27] = 0xffffff80. //ATS Byte

// RecvPackt[28] = 0x3.        //ETX
// RecvPackt[29] = 0x1e.       //BCC

//New Card Firmware Reply bytes
//f2 0 0 1a 50 60 30 32 30 30 41 3 44 7 88 4 1a 43 52 34 65 80 20 6 75 77 81 2 80 d1 3 78

//New Firmware Reply Bytes
//STX                 : 0xf2 
//ADDR                : 0x0 
//Length High 		  : 0x0 
//Length Low 		  : 0x1a 
//CMH 				  : 0x50 
//Command 			  : 0x60 
//Command Parameter   : 0x30 
//ST0 				  : 0x32 
//ST1 				  : 0x30 
//ST2 				  : 0x30 
//Card Type Rtype     : 0x41 
//ATQ First Byte      : 0x3  
//ATQ Second Byte     : 0x44 
//UID Length and Data : 0x7 0x88 0x4 0x1a 0x43 0x52 0x34 0x65 0x80
//SAK                 : 0x20 0x6 
//ATS                 : 0x75 0x77 0x81 0x2 0x80 0xd1
//ETX+BCC             : 0x3 0x78

/*
int ActivateContactLessCard( char *APIReply,int  *ReplyAPDULength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ////writeFileLog("[ActivateContactLessCard()] Entry");
        AVRM_writeFileLog("[ActivateContactLessCard()] Entry",INFO);

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[12];

        memset(command,'\0',12);

        char bcc=0x00;

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x05; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x60; //CM
        command[6]  = 0x30; //PM
        command[7]  = 0x41; //DATA
        command[8]  = 0x30; //DATA
        command[9]  = 0x03; //ETX
        command[10] = 0x00; //BCC
        
        //++g_ComHandle
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ActivateContactLessCard()] DeviceID= %s",deviceid);
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,INFO);
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ActivateContactLessCard()] ComPortHandle= %d",g_ComHandle);
        
        AVRM_writeFileLog(log,INFO);
        
        int CommandLength = 11;

        char RecvPackt[100];

        //++Reply Byte:
        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //DATA:
        //Rtype    = 1 byte
        //ATQ      = 2 byte
        //UID_Len  = 1 byte
        //UID_DATA = 7 byte
        //SAK      = 2 byte
        //ATS      = 5 byte
        //ETX      = 1 byte
        //BCC      = 1 byte
    
        //Total Reply Byte = 30 Byte
        
        int totalByteToRecv=30; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //++Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) ) {

             ////writeFileLog("[ActivateContactLessCard()] Command Transmit success");
             AVRM_writeFileLog("[ActivateContactLessCard()] Command Transmit success",DEBUG);

		    //++Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() ) {
				
                 AVRM_writeFileLog("[ActivateContactLessCard()] ACK receieve from MUTEK",DEBUG);

		         //++Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv)) {

							AVRM_writeFileLog("[ActivateContactLessCard()] Reply Bytes receieve from MUTEK is success",DEBUG);

							if( ( 0x50 == RecvPackt[4] ) &&  //CMH
								( 0x60 == RecvPackt[5] ) &&  //CM
								( 0x30 == RecvPackt[6] )     //PM
							  )
							{
										 //++Byte 0 Status of Card Activation
										 if( 0x30 == RecvPackt[7]  )
										 {
												  APIReply[0]= NO_CARD_FOUND; //no card found
										 }
										 else if( 0x31 == RecvPackt[7]  )
										 {

												  APIReply[0]= CARD_FOUND_AND_ACTIVATED_FAILED; //card found and activated failed
										 }
										 else if( 0x32 == RecvPackt[7] )
										 {

												  APIReply[0]= CARD_FOUND_AND_ACTIVATED; //card found and activated
										 }else{
										          APIReply[0]= 0x31; //++Other Error
										 
									     }//else end
									      
										 //++Byte 1 Type of Card Found 
										 if( ( 0x00 == RecvPackt[11] ) && ( 0x44 == RecvPackt[12] ) ) //UltraLight
										 {
											 APIReply[1]= 3;
										 }
										 if( ( 0x03 == RecvPackt[11] ) && ( 0x44 == RecvPackt[12] ) ) //Desfire EV1
										 {
											 APIReply[1]= 2;
										 }

										 //Byte 2 Size of UID
										 APIReply[2]= RecvPackt[13] ; //++UID Data Length

										 
										 //Byte 3-9 UID Byte
										 //APIReply[3]= RecvPackt[14] ; //UID_Data 
										 //APIReply[4]= RecvPackt[15] ; //UID_Data 
										 //APIReply[5]= RecvPackt[16] ; //UID_Data 
										 //APIReply[6]= RecvPackt[17] ; //UID_Data 
										 //APIReply[7]= RecvPackt[18] ; //UID_Data 
										 //APIReply[8]= RecvPackt[19] ; //UID_Data 
										 //APIReply[9]= RecvPackt[20] ; //UID_Data 
                                         

										 //Byte 3-N UID Length
										 int UIDData_Length =RecvPackt[13],counter=0,
										 UIDDataStartIndex=14,ReplyArrayCounter=3;

										 for(counter=0;counter<UIDData_Length;counter++){
											   APIReply[ReplyArrayCounter] =  RecvPackt[UIDDataStartIndex];
											   UIDDataStartIndex++;
											   ReplyArrayCounter++;
										 }//for end
										 
										 //Now Set ReplyAPDULength
										 *ReplyAPDULength = RecvPackt[13]+3; //Ststus Byte+CardTypeByte+UID LengthByte

										 ////writeFileLog("[ActivateContactLessCard() Exit] Successfully found activate contact less card.");
										 AVRM_writeFileLog("[ActivateContactLessCard() Exit] Successfully found activate contact less card.",INFO);
										 return true;
							
						 }else { //++Negative AcK Reply bytes receieved
							 
							   ////writeFileLog("[ActivateContactLessCard() Exit] Receieved Negative ACK From reader.");
							   AVRM_writeFileLog("[ActivateContactLessCard() Exit] Receieved Negative ACK From reader.",ERROR);
							   APIReply[0] = NO_CARD_FOUND ; //++NO_CARD_FOUND
                               *ReplyAPDULength = 1;
 				               return false;
							 
					     }//else end

				 }else { //++failure case reply bytes read error
					 
								////writeFileLog("[ActivateContactLessCard() Exit] Reply Bytes receieve from MUTEK is failed ");
								AVRM_writeFileLog("[ActivateContactLessCard() Exit] Reply Bytes receieve from MUTEK is failed.",ERROR);
								if( totalByteToRecv > 0 )
								{
									 if( 0x4e == RecvPackt[4] )
									 {
												////writeFileLog("[ActivateContactLessCard() Exit] Negative Response from mutek  ");
												AVRM_writeFileLog("[ActivateContactLessCard() Exit] Negative Response from mutek  ",ERROR);
												APIReply[0] = OTHER_ERROR; //other error
												*ReplyAPDULength = 1;
												return false;
									 
									 }//if( 0x4e == RecvPackt[4] )
								 
								}//if( totalByteToRecv > 0 ) block
								else
								{
									////writeFileLog("[ActivateContactLessCard() Exit] Operation timeout ");
									AVRM_writeFileLog("[ActivateContactLessCard() Exit] Operation timeout ",ERROR);
									APIReply[0] = OPERATION_TIMEOUT_OCCURRED ; //operation time out
									*ReplyAPDULength = 1;
									return false;

								}//else end

					 }//else block

		    } else { //++Fail receieve ACK Byte from MUTEK
		                 //++Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[ActivateContactLessCard() Exit] Ack receieved failed");
                         AVRM_writeFileLog("[ActivateContactLessCard() Exit] Ack receieved failed ",ERROR);
                         APIReply[0] = COMMUNICATION_FAILURE ; //COMMUNICATION_FAILURE
		                 return false;

		    }//else end

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             ////writeFileLog("[ActivateContactLessCard() Exit] Command Transmit failed");
             AVRM_writeFileLog("[ActivateContactLessCard() Exit] Command Transmit failed ",ERROR);
             APIReply[0] = COMMUNICATION_FAILURE ; //++communication failure
             *ReplyAPDULength = 1;
             return false;
        }//else end

}//ActivateContactLessCard() end
*/

static int getFirmwareVersion(char *firmwareversion) {


        char log[LOG_ARRAY_SIZE];
		memset(log,'\0',LOG_ARRAY_SIZE);
		char command[100];
		memset(command,'\0',100);
		//get firmware version Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0xA4; //CM
        command[6]  = 0x30; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
        int CommandLength = 9;
        unsigned char RecvPackt[100];
        //Reply Byte:
		//STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //VER    = N BYTE
        //ETX    = 1 byte
        //BCC    = 1 byte
		//Total Reply Byte = 30 Byte
        int totalByteToRecv=40; //++Reply Byte Length
        memset(RecvPackt,'\0',100);
        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) ){
			
					AVRM_writeFileLog("[getFirmwareVersion()] Command Transmit success",DEBUG);
                    //Step 2: Wait for ACK Byte
					if( true ==  IsAckReceieve() ){
						
							 //Step 3: After Receieve ACK Go for read reply bytes
							 if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv)){
								 
									//CMH+CM+PM
									if( ( 0x50 == RecvPackt[4] ) &&  ( 0xA4 == RecvPackt[5] ) &&  ( 0x30 == RecvPackt[6] ) ){
									   AVRM_writeFileLog("[getFirmwareVersion()] Reply Bytes receieve from MUTEK is success",DEBUG);
									   //++Reply byte: f2 0 0 17 50 a4 30 32 30 30 [43 35 37 31 5f 56 32 30 5f 41 5f 31 39 30 38 30 31] 3 2b 
									   //Start Index: 10
									   //End Index: 26
									   int counter2=0;
									   for(int counter=10;counter<=26;counter++){
											firmwareversion[counter2] = RecvPackt[counter];
											counter2++;
									   }//++for end
									   firmwareversion[counter2] = '\0';
									   memset(log,'\0',LOG_ARRAY_SIZE);
									   sprintf(log,"[getFirmwareVersion()] Firmware Version= %s",firmwareversion);
									   AVRM_writeFileLog(log,INFO);
									   return true;
									}else{
									   AVRM_writeFileLog("[getFirmwareVersion()] Reply Bytes receieve from MUTEK is failed",DEBUG);
									   return false;
									}//++else end
						
							 }else {
									AVRM_writeFileLog("[getFirmwareVersion() Exit] Reply Bytes receieve from MUTEK is failed ",INFO);
									if( (totalByteToRecv > 0) && ( 0x4e == RecvPackt[4] ) )
									{
										AVRM_writeFileLog("[getFirmwareVersion() Exit] negative ack from mutek ",ERROR);
										return false;
									}else{
										AVRM_writeFileLog("[getFirmwareVersion() Exit] no proper reply byte receieve from mutek",INFO);
										return false;
									}//++else end

							 }//++else block

					}else {
						 AVRM_writeFileLog("[getFirmwareVersion() Exit] fail receieve ack byte from mutek",INFO);
						 return false;
					}//++else end

        }else{
            AVRM_writeFileLog("[getFirmwareVersion() Exit] Command Transmit failed",ERROR);
            return false;
        }//++else end
        
}//int getFirmwareVersion() end

int ActivateContactLessCard( char *APIReply,int  *ReplyAPDULength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        AVRM_writeFileLog("[ActivateContactLessCard()] Entry",INFO);

        ResetSmartCardLastError();

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);
        
        unsigned char firmwareversion[18];
        memset(firmwareversion,'\0',18);

        char command[12];

        memset(command,'\0',12);

        char bcc=0x00;

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x05; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x60; //CM
        command[6]  = 0x30; //PM
        command[7]  = 0x41; //DATA
        command[8]  = 0x30; //DATA
        command[9]  = 0x03; //ETX
        command[10] = 0x00; //BCC
        
        //++g_ComHandle
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ActivateContactLessCard()] DeviceID= %s",deviceid);
        AVRM_writeFileLog(log,INFO);
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ActivateContactLessCard()] ComPortHandle= %d",g_ComHandle);
        AVRM_writeFileLog(log,INFO);
        
        int CommandLength = 11;
	    unsigned char RecvPackt[100];

        //++Reply Byte:
        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //DATA:
        //Rtype    = 1 byte
        //ATQ      = 2 byte
        //UID_Len  = 1 byte
        //UID_DATA = 7 byte
        //SAK      = 2 byte
        //ATS      = 5 byte
        //ETX      = 1 byte
        //BCC      = 1 byte
        //Total Reply Byte = 30 Byte
        int totalByteToRecv=30; //Reply Byte Length
		memset(RecvPackt,'\0',100);
		
		//++New Smartcard Dispenser Firmware Version: C571_V20_A_190801
		//++Old Smartcard Dispenser Firmware Version: C571_V10_A_121106
		if( true == getFirmwareVersion(firmwareversion) ){
			memset(log,'\0',LOG_ARRAY_SIZE);
			sprintf(log,"[ActivateContactLessCard()] Firmware Version= %s",firmwareversion);
			AVRM_writeFileLog(log,INFO);
			if( 0 == strcmp(firmwareversion,"C571_V20_A_190801") ){
				AVRM_writeFileLog("[ActivateContactLessCard()] New Card Firmware Version Found[C571_V20_A_190801].",DEBUG);
				totalByteToRecv =  totalByteToRecv+2;
			}//if end	
		}//++if end
		
        //++Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) ) {

             AVRM_writeFileLog("[ActivateContactLessCard()] Command Transmit success",DEBUG);

		    //++Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() ) {
				
                 AVRM_writeFileLog("[ActivateContactLessCard()] ACK receieve from MUTEK",DEBUG);

		         //++Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv)) {

							AVRM_writeFileLog("[ActivateContactLessCard()] Reply Bytes receieve from MUTEK is success",DEBUG);

							if( ( 0x50 == RecvPackt[4] ) &&  //CMH
								( 0x60 == RecvPackt[5] ) &&  //CM
								( 0x30 == RecvPackt[6] )     //PM
							  )
							{
										 //++Byte 0 Status of Card Activation
										 if( 0x30 == RecvPackt[7]  )
										 {
												  APIReply[0]= NO_CARD_FOUND; //no card found
										 }
										 else if( 0x31 == RecvPackt[7]  )
										 {

												  APIReply[0]= CARD_FOUND_AND_ACTIVATED_FAILED; //++card found and activated failed
										 }
										 else if( 0x32 == RecvPackt[7] )
										 {

												  APIReply[0]= CARD_FOUND_AND_ACTIVATED; //++card found and activated
										 }else{
										          APIReply[0]= 0x31; //++Other Error
										 
									     }//else end
									      
										 //++Byte 1 Type of Card Found 
										 if( ( 0x00 == RecvPackt[11] ) && ( 0x44 == RecvPackt[12] ) ) //UltraLight
										 {
											 APIReply[1]= 3;
										 }
										 if( ( 0x03 == RecvPackt[11] ) && ( 0x44 == RecvPackt[12] ) ) //Desfire EV1
										 {
											 APIReply[1]= 2;
										 }

										 //Byte 2 Size of UID
										 APIReply[2]= RecvPackt[13] ; //++UID Data Length

										 //Byte 3-9 UID Byte
										 //++APIReply[3]= RecvPackt[14] ; //UID_Data 
										 //++APIReply[4]= RecvPackt[15] ; //UID_Data 
										 //++APIReply[5]= RecvPackt[16] ; //UID_Data 
										 //++APIReply[6]= RecvPackt[17] ; //UID_Data 
										 //++APIReply[7]= RecvPackt[18] ; //UID_Data 
										 //++APIReply[8]= RecvPackt[19] ; //UID_Data 
										 //++APIReply[9]= RecvPackt[20] ; //UID_Data 
                                         
										 //Byte 3-N UID Length
										 int UIDData_Length =RecvPackt[13],counter=0,
										 UIDDataStartIndex=14,ReplyArrayCounter=3;
										 
										 memset(log,'\0',LOG_ARRAY_SIZE);
										 sprintf(log,"[ActivateContactLessCard()] UID StartIndex(14) = 0x%x .",RecvPackt[14]);
										 AVRM_writeFileLog(log,INFO);
											 
										 
										 if( 0x88 == RecvPackt[14] && 0x1a == RecvPackt[3] ){
											AVRM_writeFileLog("[ActivateContactLessCard() Exit] Found 0x88 Byte.",INFO);
											UIDDataStartIndex = UIDDataStartIndex+1;
										 }else{
											AVRM_writeFileLog("[ActivateContactLessCard() Exit] No 0x88 Byte.",INFO);
										 }//++else end
										 
										 for(counter=0;counter<UIDData_Length;counter++){
											   APIReply[ReplyArrayCounter] =  RecvPackt[UIDDataStartIndex];
											   UIDDataStartIndex++;
											   ReplyArrayCounter++;
										 }//++for end
										 
										 //++Now Set ReplyAPDULength
										 *ReplyAPDULength = RecvPackt[13]+3; //Status Byte+CardTypeByte+UID LengthByte

										 AVRM_writeFileLog("[ActivateContactLessCard() Exit] Successfully found activate contact less card.",INFO);
										 return true;
							
						 }else { //++Negative AcK Reply bytes receieved
						
							   AVRM_writeFileLog("[ActivateContactLessCard() Exit] Receieved Negative ACK From reader.",ERROR);
							   APIReply[0] = NO_CARD_FOUND ; //++NO_CARD_FOUND
                               *ReplyAPDULength = 1;
 				               return false;
							 
					     }//else end

				 }else { //++failure case reply bytes read error
					 
								AVRM_writeFileLog("[ActivateContactLessCard() Exit] Reply Bytes receieve from MUTEK is failed.",ERROR);
								if( totalByteToRecv > 0 )
								{
									 if( 0x4e == RecvPackt[4] )
									 {
												////writeFileLog("[ActivateContactLessCard() Exit] Negative Response from mutek  ");
												AVRM_writeFileLog("[ActivateContactLessCard() Exit] Negative Response from mutek  ",ERROR);
												APIReply[0] = OTHER_ERROR; //other error
												*ReplyAPDULength = 1;
												return false;
									 
									 }//if( 0x4e == RecvPackt[4] )
								 
								}//if( totalByteToRecv > 0 ) block
								else
								{
									////writeFileLog("[ActivateContactLessCard() Exit] Operation timeout ");
									AVRM_writeFileLog("[ActivateContactLessCard() Exit] Operation timeout ",ERROR);
									APIReply[0] = OPERATION_TIMEOUT_OCCURRED ; //operation time out
									*ReplyAPDULength = 1;
									return false;

								}//else end

					 }//else block

		    } else { //++Fail receieve ACK Byte from MUTEK
		                 //++Fail receieve ACK Byte from MUTEK
                         AVRM_writeFileLog("[ActivateContactLessCard() Exit] Ack receieved failed ",ERROR);
                         APIReply[0] = COMMUNICATION_FAILURE ; //COMMUNICATION_FAILURE
		                 return false;

		    }//else end

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             ////writeFileLog("[ActivateContactLessCard() Exit] Command Transmit failed");
             AVRM_writeFileLog("[ActivateContactLessCard() Exit] Command Transmit failed ",ERROR);
             APIReply[0] = COMMUNICATION_FAILURE ; //++communication failure
             *ReplyAPDULength = 1;
             return false;
        }//else end

}//ActivateContactLessCard() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int DeActivateContactLessCard( int *Reply,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        ////writeFileLog("[DeActivateContactLessCard()] Entry");
        AVRM_writeFileLog("[DeActivateContactLessCard()] Entry ",TRACE);

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);


        //Construct Contact Less Card DeActivate Commnad
        g_getStatus[0]  = 0xF2; //STX
        g_getStatus[1]  = MTK_DEVICE_ADDRESS; //ADDR
        g_getStatus[2]  = 0x00; //LENH
        g_getStatus[3]  = 0x03; //LENL
        g_getStatus[4]  = 0x43; //CMH
        g_getStatus[5]  = 0x60; //CM
        g_getStatus[6]  = 0x31; //PM
        g_getStatus[7]  = 0x03; //ETX
        g_getStatus[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 12 Byte
        
        int totalByteToRecv=12; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        {

                    ////writeFileLog("[DeActivateContactLessCard()] Command Transmit success");
                    AVRM_writeFileLog("[DeActivateContactLessCard()] Command Transmit success",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         ////writeFileLog("[DeActivateContactLessCard()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[DeActivateContactLessCard()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[DeActivateContactLessCard()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[DeActivateContactLessCard()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x60 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //Byte 0 Status of Card Activation
                                     if( 0x30 == RecvPackt[7]  )
		                     {
				              *Reply = NO_CARD_FOUND; //no card found
				              AVRM_writeFileLog("[DeActivateContactLessCard()] NO_CARD_FOUND",INFO);

				     }
				     else if( 0x31 == RecvPackt[7]  )
				     {

				              *Reply = CARD_FOUND_AND_DEACTIVATED_FAILED; //card found and deactivated failed
				              AVRM_writeFileLog("[DeActivateContactLessCard()] CARD_FOUND_AND_DEACTIVATED_FAILED",INFO);

				     }
				     else if( 0x32 == RecvPackt[7] )
				     {

				              *Reply = CARD_FOUND_AND_DEACTIVATED; //card found and deactivated successfully
				              AVRM_writeFileLog("[DeActivateContactLessCard()] CARD_FOUND_AND_DEACTIVATED",INFO);

				     }
                                     
                                     ////writeFileLog("[DeActivateContactLessCard() Exit] Successfully get deactivate status from mutek.");       
                                     AVRM_writeFileLog("[DeActivateContactLessCard() Exit] Successfully get deactivate status from mutek.",INFO);                    
		                     return true;

		                }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     *Reply = OTHER_ERROR ; //other error

                                     ////writeFileLog("[DeActivateContactLessCard() Exit] Negative ack receieved.");  
                                     AVRM_writeFileLog("[DeActivateContactLessCard() Exit] Negative ack receieved.",INFO);                         
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                *Reply = OPERATION_TIMEOUT_OCCURRED ; //operation time out

		                ////writeFileLog("[DeActivateContactLessCard() Exit] Timeout occuered.");   
		                AVRM_writeFileLog("[DeActivateContactLessCard() Exit] Timeout occuered.",INFO);                         
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         *Reply = OTHER_ERROR ; //other error
                          ////writeFileLog("[DeActivateContactLessCard()] Fail receieve ack byte from mutek.");
                          AVRM_writeFileLog("[DeActivateContactLessCard()] Fail receieve ack byte from mutek.",INFO); 
                          return false;
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            *Reply = OTHER_ERROR ; //other error
            ////writeFileLog("[DeActivateContactLessCard()] Fail transmit command to mutek.");
            AVRM_writeFileLog("[DeActivateContactLessCard()] Fail transmit command to mutek.",INFO); 
            return false;
        }


}//int DeActivateContactLessCard(char *APIReply) end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int SelectSAM(int SAMSlotId) {


        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);
 
        char command[100];

        memset(command,'\0',100);

        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x04; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x52; //CM
        command[6]  = 0x40; //PM
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

        //DATA SAM1=0x30 SAM2=0x31 SAM3=0x32 SMA4=0x34
        if( 1 == SAMSlotId )
        {
           command[7]  = 0x30; //SAM1
        }
        else if( 2 == SAMSlotId )
        {
           command[7]  = 0x31; //SAM2
        }
        else if( 3 == SAMSlotId )
        {
           command[7]  = 0x32; //SAM3
        }
        else if( 4 == SAMSlotId )
        {
           command[7]  = 0x33; //SAM4
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

        command[8]  = 0x03; //ETX

        command[9]  = 0x00; //BCC
      
        int CommandLength = 10;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 30 Byte
        
        int totalByteToRecv=12; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[SelectSAM()] Command Transmit success");
                     AVRM_writeFileLog("[SelectSAM()] Command Transmit success",DEBUG);

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         ////writeFileLog("[SelectSAM()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[SelectSAM()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[SelectSAM()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x40 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //byte 7 is Status of sam Card Activation
                                     if( 0x30 == RecvPackt[7]  )
		                     {
				          //no card found
                                          ////writeFileLog("[SelectSAM() Exit] no sam card found ");
                                          AVRM_writeFileLog("[SelectSAM()] Reply Bytes receieve from MUTEK is success",DEBUG);
                                          return false;
				     }
				     else if( 0x31 == RecvPackt[7]  )
				     {
				          //card found and activated failed
                                          ////writeFileLog("[SelectSAM() Exit] sam card found but activation failed ");
                                          AVRM_writeFileLog("[SelectSAM() Exit] sam card found but activation failed ",ERROR);
                                          return false;
				     }
				     else if( 0x32 == RecvPackt[7] )
				     {
                                          //card found and activated
                                          ////writeFileLog("[SelectSAM() Exit] sam card found and activated successfully ");
                                          AVRM_writeFileLog("[SelectSAM() Exit] sam card found and activated successfully ",INFO);
                                          return true;
     
				     }
                                     

		                     
		                }

		         }
		         else //failure case
		         {

                            ////writeFileLog("[SelectSAM() Exit] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[SelectSAM() Exit] Reply Bytes receieve from MUTEK is failed ",INFO);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     ////writeFileLog("[SelectSAM() Exit] negative ack from mutek");
                                     AVRM_writeFileLog("[SelectSAM() Exit] negative ack from mutek ",ERROR);
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                ////writeFileLog("[SelectSAM() Exit] reply byte receieve from mutek");
                                AVRM_writeFileLog("[SelectSAM() Exit] reply byte receieve from mutek",INFO);
                                return false;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[SelectSAM() Exit] fail receieve ack byte from mutek");
                         AVRM_writeFileLog("[SelectSAM() Exit] fail receieve ack byte from mutek",INFO);
                         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[SelectSAM() Exit] Command Transmit failed");
            AVRM_writeFileLog("[SelectSAM() Exit] Command Transmit failed",ERROR);
            return false;
        }


}//int SelectSAM(int SAMSlotId) end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++SAM Card warm reset
int WarmResetSAM( int   SAMSlotId,char *ReplyByte,int  *ReplyByteLength,char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode ) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[10];

        memset(command,'\0',10);
        
        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x03; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x52; //CM
        command[6]  = 0x38; //PM
        command[7]  = 0x03; //ETX
        command[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        memset(RecvPackt,'\0',100);
        
        //Reply Byte:

        //STX      = 1byte
        //ADDR     = 1 byte
        //LENGTH   = 2 byte
        //CMH      = 1 byte
        //CM       = 1 byte
        //PM       = 1 byte
        //ST0      = 1 byte
        //ST1      = 1 byte
        //ST2      = 1 byte
        
        //DATA:
        //Type     = 1 byte
        //ATR      = 29 byte
        //ETX      = 1 byte
        //BCC      = 1 byte
    
        //Total Reply Byte = 30 Byte
        
        int totalByteToRecv=30; //Reply Byte Length

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[WarmResetSAM()] Command Transmit success");
                    AVRM_writeFileLog("[WarmResetSAM()] Command Transmit success",TRACE);
                    
                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         //writeFileLog("[WarmResetSAM()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[WarmResetSAM()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[WarmResetSAM()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x38 == RecvPackt[6] )     //PM
		                  )
		                {
				              ////writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match success");
				              AVRM_writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match success",DEBUG);

		                             //Byte 0 Status of Card Activation reply byte index 10 hold card type
		                             if( ( 0x30 == RecvPackt[10] ) || ( 0x31 == RecvPackt[10]  ) )
				             {
						  ////writeFileLog("[WarmResetSAM()] Operation success.");
										  AVRM_writeFileLog("[WarmResetSAM()] Operation success.",INFO);

		                                  ReplyByte[0]=SUCCESS; //operation success

		                                  ///////////////////////////////////////////////////////////////////// 
		                                  //Now copy atr bytes

		                                  int StartIndex=11,FinishIndex=38,ReplyByteCounter=1;

		                                  for(;StartIndex<=FinishIndex;StartIndex++,ReplyByteCounter++)
		                                  {
		                                       ReplyByte[ReplyByteCounter] = RecvPackt[StartIndex] ;
		                                  }

		                                  *ReplyByteLength=29;//Return code(1)+ATRByte(28)

		                                  /////////////////////////////////////////////////////////////////////
		                                  return true;

					     }
					     else 
					     {
		                                   ////writeFileLog("[WarmResetSAM()] No SAM Card Found.");
		                                   AVRM_writeFileLog("[WarmResetSAM()] No SAM Card Found.",INFO);
		                                   ReplyByte[0]=NO_SAM_CARD_FOUND; //no sam card found
		                                  *ReplyByteLength=1;
		                                   return false;

					     }
                                     

                                     
		                }
                                else
                                {
                                       ////writeFileLog("[WarmResetSAM()] Reply Bytes from MUTEK match failed");
                                       AVRM_writeFileLog("[WarmResetSAM()] Reply Bytes from MUTEK match failed",DEBUG);
                                       ReplyByte[0]=OPERATION_FAILED; //Operation failed
                                       *ReplyByteLength=1;
                                       return false;
                                }

		         }
		         else //failure case
		         {

                            ////writeFileLog("[WarmResetSAM()] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[WarmResetSAM()] Reply Bytes receieve from MUTEK is failed",ERROR);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                    
                                     ////writeFileLog("[WarmResetSAM()] Negative ACK receieve from mutek");
                                     AVRM_writeFileLog("[WarmResetSAM()] Negative ACK receieve from mutek",ERROR);
		                     ReplyByte[0]=OTHER_ERROR; //other error
                                    *ReplyByteLength=1;
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                 ////writeFileLog("[WarmResetSAM()] Timeout happened");
		                 AVRM_writeFileLog("[WarmResetSAM()] Negative ACK receieve from mutek",ERROR);
		                 ReplyByte[0]=OPERATION_TIMEOUT_OCCURRED; //Timeout happens
                                *ReplyByteLength=1;
                                 return false;

		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[WarmResetSAM()] Fail receieve ACK Byte from MUTEK");
                         AVRM_writeFileLog("[WarmResetSAM()] Fail receieve ACK Byte from MUTEK",ERROR);
                         ReplyByte[0]=OTHER_ERROR; //other error
                        *ReplyByteLength=1;
                         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[WarmResetSAM()] Command Transmit failed");
            AVRM_writeFileLog("[WarmResetSAM()] Command Transmit failed",ERROR);
            ReplyByte[0]=COMMUNICATION_FAILURE; //communiation failure
            *ReplyByteLength=1;
            return false;
        }


}//int WarmResetSAM(int SAMSlotId,char *ReplyByte,int *ReplyByteLength)

//++SAM Card warm reset
int ColdResetSAM( int   SAMSlotId,char *ReplyByte,int  *ReplyByteLength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char command[10];

        memset(command,'\0',10);
        
        //Construct Contact Less Card Activate Commnad
        command[0]  = 0xF2; //STX
        command[1]  = MTK_DEVICE_ADDRESS; //ADDR
        command[2]  = 0x00; //LENH
        command[3]  = 0x04; //LENL
        command[4]  = 0x43; //CMH
        command[5]  = 0x52; //CM
        command[6]  = 0x30; //PM
        command[7]  = 0x33; //DATA
        command[8]  = 0x03; //ETX
        command[9]  = 0x00; //BCC
      
        int CommandLength = 10;

        char RecvPackt[100];

        memset(RecvPackt,'\0',100);

        //Reply Byte:

        //STX      = 1 byte
        //ADDR     = 1 byte
        //LENGTH   = 2 byte
        //CMH      = 1 byte
        //CM       = 1 byte
        //PM       = 1 byte
        //ST0      = 1 byte
        //ST1      = 1 byte
        //ST2      = 1 byte
        
        //DATA:
        //Type     = 1 byte
        //ATR      = 28 byte Index 11 to 37 hold atr byte
        //ETX      = 1 byte
        //BCC      = 1 byte
    
        //Total Reply Byte = 41 Byte
        int totalByteToRecv=41; //Reply Byte Length
        
        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    ////writeFileLog("[ColdResetSAM()] Command Transmit success");
                    AVRM_writeFileLog("[ColdResetSAM()] Command Transmit success",INFO);
                    
                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[ColdResetSAM()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[ColdResetSAM()] ACK receieve from MUTEK",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[ColdResetSAM()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[ColdResetSAM()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
		                  )
		                {
		                     ////writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match success");
		                             AVRM_writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match success",DEBUG);

                                     //Byte 0 Status of Card Activation reply byte index 10 hold card type
                                     if( ( 0x30 == RecvPackt[10] ) || ( 0x31 == RecvPackt[10]  ) )
		                     {
				          ////writeFileLog("[ColdResetSAM()] Operation success.");
				                          AVRM_writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match success",INFO);
                                          ReplyByte[0]=SUCCESS; //operation success
                                          ///////////////////////////////////////////////////////////////////// 
                                          //Now copy atr bytes

                                          int StartIndex=11,FinishIndex=38,ReplyByteCounter=1;

                                          for(;StartIndex<=FinishIndex;StartIndex++,ReplyByteCounter++)
                                          {
                                               ReplyByte[ReplyByteCounter] = RecvPackt[StartIndex] ;
                                          }

                                          *ReplyByteLength=29;//Return code(1)+ATRByte(28)

                                          /////////////////////////////////////////////////////////////////////

                                          return true;

				     }
				     else 
				     {
                                           ////writeFileLog("[ColdResetSAM()] No SAM Card Found.");
                                           AVRM_writeFileLog("[ColdResetSAM()] No SAM Card Found.",INFO);
                                           ReplyByte[0]=NO_SAM_CARD_FOUND; //No SAM Card Found
                                           *ReplyByteLength=1;
                                           return false;

				     }
				   
                                     
		                }
                                else
                                {
                                     ////writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match failed");
                                     AVRM_writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match failed.",ERROR);
                                     ReplyByte[0]=OPERATION_FAILED; //Operation failed
                                    *ReplyByteLength=1;
                                     return false;

                                }

		         }
		         else //failure case
		         {

                            ////writeFileLog("[ColdResetSAM()] Reply Bytes receieve from MUTEK is failed ");
                            AVRM_writeFileLog("[ColdResetSAM()] Reply Bytes receieve from MUTEK is failed ",ERROR);

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     ////writeFileLog("[ColdResetSAM()] Negative ack receieved ");
                                     AVRM_writeFileLog("[ColdResetSAM()] Negative ack receieved ",ERROR);
		                     ReplyByte[0]=OTHER_ERROR; //Other error
                                    *ReplyByteLength=1;
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                ////writeFileLog("[ColdResetSAM()] Operation timeout. ");
                                 AVRM_writeFileLog("[ColdResetSAM()] Operation timeout. ",ERROR);
		                ReplyByte[0] = OPERATION_TIMEOUT_OCCURRED; //operation time out
                               *ReplyByteLength=1;
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[ColdResetSAM()] Fail receieve ACK Byte from MUTEK. ");
                         AVRM_writeFileLog("[ColdResetSAM()] Fail receieve ACK Byte from MUTEK. ",ERROR);
		         ReplyByte[0] = OTHER_ERROR; //other error
                        *ReplyByteLength=1;
		         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[ColdResetSAM()] Command Transmit failed");
            AVRM_writeFileLog("[ColdResetSAM()] Command Transmit failed. ",ERROR);
            ReplyByte[0] = COMMUNICATION_FAILURE; //communication failure
           *ReplyByteLength=1;
            return false;
        }


}//int ColdResetSAM(int SAMSlotId,char *ReplyByte,int *ReplyByteLength)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Activate SAM Card
static int ActivateContactCard(char *APIReply) {

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        //Construct Contact Less Card Activate Commnad
        g_getStatus[0]  = 0xF2; //STX
        g_getStatus[1]  = MTK_DEVICE_ADDRESS; //ADDR
        g_getStatus[2]  = 0x00; //LENH
        g_getStatus[3]  = 0x04; //LENL
        g_getStatus[4]  = 0x43; //CMH
        g_getStatus[5]  = 0x52; //CM
        g_getStatus[6]  = 0x30; //PM
        g_getStatus[7]  = 0x33; //DATA
        g_getStatus[8]  = 0x03; //ETX
        g_getStatus[9]  = 0x00; //BCC
      
        int CommandLength = 10;

        char RecvPackt[100];

        unsigned int RecvPacktLength=0;

        
        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //DATA:
        //Type     = 1 byte
        //ATR      = 29 byte
        //ETX      = 1 byte
        //BCC      = 1 byte
    
        //Total Reply Byte = 30 Byte
        
        int totalByteToRecv=35; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        {

                    ////writeFileLog("[ActivateContactCard()] Command Transmit success");
                    AVRM_writeFileLog("[ActivateContactCard()] Command Transmit success ",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         //writeFileLog("[ActivateContactCard()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve_V2(RecvPackt,&RecvPacktLength))
		         {

                                ////writeFileLog("[ActivateContactCard()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[ActivateContactCard()] Reply Bytes receieve from MUTEK is success ",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
                                    
		                  )
		                {
                                       
		                       ////writeFileLog("[ActivateContactCard()] Sam Card activate is success");    
		                        AVRM_writeFileLog("[ActivateContactCard()] Sam Card activate is success ",INFO);
				       return true;

		                }// if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                 //   ( 0x52 == RecvPackt[5] ) &&  //CM
		                 //   ( 0x30 == RecvPackt[6] )     //PM
		                 // ) 
                               else  if(  0x4e == RecvPackt[4] )
		               {
                                     APIReply[0] =  OTHER_ERROR; //other error
		                     ////writeFileLog("[ActivateContactCard()] 0x4e Byte  recv from MUTEK");
		                     AVRM_writeFileLog("[ActivateContactCard()] 0x4e Byte  recv from MUTEK",ERROR);
                                     return false;
                               }

		         }
		         else //failure case
		         {

                                 ////writeFileLog("[ActivateContactCard()] Reply Bytes receieve from MUTEK is failed ");
                                 AVRM_writeFileLog("[ActivateContactCard()] Reply Bytes receieve from MUTEK is failed ",ERROR);

		                 if( 0x4e == RecvPackt[4] )
		                 {        
                                     APIReply[0] =  OTHER_ERROR; //other error
		                     ////writeFileLog("[ActivateContactCard()] Negative ACK Byte from MUTEK");
		                     AVRM_writeFileLog("[ActivateContactCard()] Negative ACK Byte from MUTEK",ERROR);
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		                 else
		                 {
                                     ////writeFileLog("[ActivateContactCard()] No Recv Byte recv from MUTEK");
                                     AVRM_writeFileLog("[ActivateContactCard()] No Recv Byte recv from MUTEK",ERROR);
		                     APIReply[0] = OPERATION_TIMEOUT_OCCURRED; //operation time out
		                     return false;
                                 }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         ////writeFileLog("[ActivateContactCard()] Fail receieve ACK Byte from MUTEK");
                         AVRM_writeFileLog("[ActivateContactCard()] Fail receieve ACK Byte from MUTEK",ERROR);
                         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[ActivateContactCard()] Command Transmit failed");
             AVRM_writeFileLog("[ActivateContactCard()] Command Transmit failed",ERROR);
            APIReply[0] =  OTHER_ERROR; //other error
            return false;
        }


}//ActivateContactCard() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//++Deactivate Contact card
int DeActivateContactCard( int  *Reply,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode) {

        
        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
        
        ////writeFileLog("[DeActivateContactCard()] Entry");
        AVRM_writeFileLog("[DeActivateContactCard()] Entry ",TRACE);
        
        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);
        
        //Construct Contact Less Card DeActivate Commnad
        g_getStatus[0]  = 0xF2; //STX
        g_getStatus[1]  = MTK_DEVICE_ADDRESS; //ADDR
        g_getStatus[2]  = 0x00; //LENH
        g_getStatus[3]  = 0x03; //LENL
        g_getStatus[4]  = 0x43; //CMH
        g_getStatus[5]  = 0x52; //CM
        g_getStatus[6]  = 0x31; //PM
        g_getStatus[7]  = 0x03; //ETX
        g_getStatus[8]  = 0x00; //BCC
      
        int CommandLength = 9;

        char RecvPackt[100];

        
        //Reply Byte:

        //STX    = 1 byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Reply Byte = 12 Byte
        
        int totalByteToRecv=12; //Reply Byte Length

        memset(RecvPackt,'\0',100);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        {

                    ////writeFileLog("[DeActivateContactCard()] Command Transmit success");
                    AVRM_writeFileLog("[DeActivateContactCard()] Command Transmit success ",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         ////writeFileLog("[DeActivateContactCard()] ACK receieve from MUTEK");
                         AVRM_writeFileLog("[DeActivateContactCard()] ACK receieve from MUTEK ",DEBUG);

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                ////writeFileLog("[DeActivateContactCard()] Reply Bytes receieve from MUTEK is success");
                                AVRM_writeFileLog("[DeActivateContactCard()] Reply Bytes receieve from MUTEK is success",DEBUG);

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //Byte 0 Status of Card Activation
                                     if( 0x30 == RecvPackt[7]  )
		                     {
				              *Reply = NO_CARD_FOUND; //no card found

				     }
				     else if( 0x31 == RecvPackt[7]  )
				     {

				              *Reply = CARD_FOUND_AND_DEACTIVATED_FAILED; //card found and deactivated failed

				     }
				     else if( 0x32 == RecvPackt[7] )
				     {

				              *Reply = CARD_FOUND_AND_DEACTIVATED; //card found and deactivated successfully

				     }
                                     
                                     ////writeFileLog("[DeActivateContactCard() Exit] Successfully get deactivate status from mutek.");   
                                     AVRM_writeFileLog("[DeActivateContactCard() Exit] Successfully get deactivate status from mutek.",INFO);                        
		                     return true;

		                }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     *Reply = OTHER_ERROR ; //other error

                                     ////writeFileLog("[DeActivateContactCard() Exit] Negative ACK receieved.");    
                                     AVRM_writeFileLog("[DeActivateContactCard() Exit] Negative ACK receieved..",ERROR);                         
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                *Reply = OPERATION_TIMEOUT_OCCURRED ; //operation time out

		                ////writeFileLog("[DeActivateContactCard() Exit] Timeout occuered."); 
		                AVRM_writeFileLog("[DeActivateContactCard() Exit] Timeout occuered..",ERROR);
                          
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          *Reply = OTHER_ERROR ; //other error
                          ////writeFileLog("[DeActivateContactCard()] Fail receieve ack byte from mutek.");
                          AVRM_writeFileLog("[DeActivateContactCard()] Fail receieve ack byte from mutek.",ERROR);
                          return false;
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            *Reply = OTHER_ERROR ; //other error
             ////writeFileLog("[DeActivateContactCard()] Fail transmit command to mutek.");
             AVRM_writeFileLog("[DeActivateContactCard()] Fail transmit command to mutek..",ERROR);
             return false;
        }





}//int DeActivateContactCard(int *APIReply) end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//SmartCard XChangeAPDU
int ContactLessCardXChangeAPDU(  char *CommandAPDU, int   CommandAPDULength,char *ReplyAPDU,int  *ReplyAPDULength,char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode ) {


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        ResetSmartCardLastError();

        ////writeFileLog("[ContactLessCardXChangeAPDU()] Entry");
        AVRM_writeFileLog("[ContactLessCardXChangeAPDU()] Entry",TRACE);

        char log[LOG_ARRAY_SIZE];

        unsigned int rcvPktLen=0;

        memset(log,'\0',LOG_ARRAY_SIZE);

        char XChangeAPDUBuffer[LOG_ARRAY_SIZE],tempXChangeAPDUBuffer[LOG_ARRAY_SIZE];

        memset(XChangeAPDUBuffer,'\0',LOG_ARRAY_SIZE);     

        memset(tempXChangeAPDUBuffer,'\0',LOG_ARRAY_SIZE);

        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[ContactLessCardXChangeAPDU()] Command APDU Length = %d .",CommandAPDULength);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,TRACE);

        int TotalCommandLength=0;
        //3 Byte: CMT+PM+CM        
        TotalCommandLength = CommandAPDULength+3;

        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ContactLessCardXChangeAPDU()] Total Comamnd Length = %d .",TotalCommandLength);
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);

        char APDUCommand[300];
        memset(APDUCommand,'\0',300);
        int APDUCommandcounter = 0,APDUByteCounter=0,APDUByteLengthCounter=1;

        //Construct Contact Less Card DeActivate Command
        APDUCommand[0]  = 0xF2; //STX
        APDUCommand[1]  = MTK_DEVICE_ADDRESS; //ADDR
        
        APDUCommand[2]  = 0x00; //LENH
        APDUCommand[3]  = TotalCommandLength; //LENL
        APDUCommand[4]  = 0x43; //CMH
        APDUCommand[5]  = 0x60; //CM
        APDUCommand[6]  = 0x34; //PM
        
        //Now fill APDU Command data byte
         
        APDUCommandcounter=7;
        APDUByteCounter=0;
        for(;APDUByteLengthCounter<=CommandAPDULength;){

              APDUCommand[APDUCommandcounter]  = CommandAPDU[APDUByteCounter]; 
              APDUCommandcounter++; //Increment Local Command Array Counter 
              APDUByteCounter++;    //Increment Function C APDU Array Counter
              APDUByteLengthCounter++; //Increment CAPDU Command Length Counter

        }//for end

        APDUCommand[APDUCommandcounter]  = 0x03; //ETX

        APDUCommand[APDUCommandcounter+1]  = 0x00; //BCC
      
        APDUCommandcounter = APDUCommandcounter+2;

        int CommandLength = APDUCommandcounter;

        char RecvPackt[MUTEK_REPLY_LENGTH_MAX];
      
        
        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //R-APDU = MAX 258 Byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Max Reply Byte = 270 Byte
        
        //int totalByteToRecv= 270; //Reply Byte Max Length 

        memset(RecvPackt,'\0',MUTEK_REPLY_LENGTH_MAX);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(APDUCommand,CommandLength) )
        {

            ////writeFileLog("[ContactLessCardXChangeAPDU()] Command Transmit success");
            AVRM_writeFileLog("[ContactLessCardXChangeAPDU()] Command Transmit success",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                                ////writeFileLog("[ContactLessCardXChangeAPDU()] ACK receieve from MUTEK");
                                AVRM_writeFileLog("[ContactLessCardXChangeAPDU()] ACK receieve from MUTEK",DEBUG);

		                        //Step 3: After Receieve ACK Go for read reply bytes
		                        //++if( false == MUTEK_CommandReceieve_V2(RecvPackt,&rcvPktLen) ){
								if( false == MUTEK_CommandReceieve_V2(RecvPackt,&rcvPktLen) ){	
                                        
				                      ReplyAPDU[0] =  OTHER_ERROR ;//Other Error
		                             *ReplyAPDULength = 1;
		                              AVRM_writeFileLog("[ContactLessCardXChangeAPDU() Exit] Other Error Command Read Section.",ERROR);
				                      return OTHER_ERROR;
                                } else{
                                   ////writeFileLog("[ContactLessCardXChangeAPDU()] Reply Bytes receieve from MUTEK is success");
                                }//else end
                                
								if( ( 0x50 == RecvPackt[4] ) &&  //CMH
									( 0x60 == RecvPackt[5] ) &&  //CM
									( 0x34 == RecvPackt[6] )     //PM
								  )
								{
									 int ReplyAPDUCounter=1;
									 ReplyAPDU[0] = SUCCESS;//Success
		                             //++Get Return R-APDU Length 
		                             int RceieveTextLength = 0;
		                             RceieveTextLength = (RecvPackt[2]<<8)+RecvPackt[3];

                                     if( rcvPktLen!= (RceieveTextLength+6) ){
                                                   
                                                  memset(log,'\0',LOG_ARRAY_SIZE);

		                                          sprintf(log,"[ContactCardXChangeAPDU()] Receieve Text length = %d  and Reply length Byte=%d not matched.",rcvPktLen,RceieveTextLength);
		                                          
		                                          AVRM_writeFileLog(log,DEBUG);

                                                  ReplyAPDU[0] =  OTHER_ERROR ; //other error

		                                          *ReplyAPDULength = 1;

                                                  return OTHER_ERROR;

                                     }//++if( rcvPktLen!= (RceieveTextLength+6) ) end

		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactLessCardXChangeAPDU()] Receieve Text length = %d .",RceieveTextLength);
		                             
		                             ////writeFileLog(log);
		                             
		                             AVRM_writeFileLog(log,DEBUG);

		                             int RAPDUTextLength =0,StartRAPDUIndex=10,CountDownLength=0;
		                             
		                             RAPDUTextLength = RceieveTextLength-6 ; //6 Byte: PMT+CM+PM+ST0+ST1+ST2
		                             
		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactLessCardXChangeAPDU()] APDU Text length = %d .",RAPDUTextLength);
		                             
		                             AVRM_writeFileLog(log,DEBUG);

                                     strcat(XChangeAPDUBuffer,"[ContactLessCardXChangeAPDU()] ReplyAPDU [0x]: ");

		                             for(;CountDownLength<RAPDUTextLength;CountDownLength++) {
										 
		                                   ReplyAPDU[ReplyAPDUCounter] =  RecvPackt[StartRAPDUIndex];

		                                   //++memset(log,'\0',LOG_ARRAY_SIZE);

		                                   //++sprintf(log,"[ContactLessCardXChangeAPDU()] ReplyAPDU[%d] = 0x%xh. RecvPackt[%d]=0x%xh.",ReplyAPDUCounter,(unsigned char)ReplyAPDU[ReplyAPDUCounter],StartRAPDUIndex,(unsigned char)RecvPackt[StartRAPDUIndex]);

		                                   //++writeFileLog(log);

                                           sprintf(tempXChangeAPDUBuffer,"%x ",(unsigned char)ReplyAPDU[ReplyAPDUCounter]);

                                           strcat(XChangeAPDUBuffer,tempXChangeAPDUBuffer);
                             
		                                   ReplyAPDUCounter++;
		                                   
		                                   StartRAPDUIndex++;
		                                  
		                             }//++for end

                                     
		                             //Now Set ReplyAPDULength
		                             *ReplyAPDULength = RAPDUTextLength+1;

		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactLessCardXChangeAPDU() Exit] Successfully get APDU and return array length = %d .",*ReplyAPDULength);

		                             AVRM_writeFileLog(log,DEBUG);
				              
		                             return SUCCESS;

				        }else {
						     
				                     ReplyAPDU[0] =  OTHER_ERROR ;//Other Error
		                             *ReplyAPDULength = 1;
		                             AVRM_writeFileLog("[ContactLessCardXChangeAPDU() Exit] Other Error Block1.",ERROR);
				                     return OTHER_ERROR;
				                           
					    }//++else end Other Error

		         }else { //++failure case
		         

		                    AVRM_writeFileLog("[ContactLessCardXChangeAPDU()] Reply Bytes receieve from MUTEK is failed .",ERROR);

							if( 0x4e == RecvPackt[4] ) //Negative PMT header
							{
									 ReplyAPDU[0] =  OTHER_ERROR ; //other error
								     *ReplyAPDULength = 1;
									 AVRM_writeFileLog("[ContactLessCardXChangeAPDU()] Reply Bytes receieve from MUTEK is failed .",ERROR);
									 return OTHER_ERROR;
								 
							}//if( 0x4e == RecvPackt[4] )
							else
							{
									 ReplyAPDU[0] =  OPERATION_TIMEOUT_OCCURRED ; //operation time out
								     *ReplyAPDULength = 1;									  
									  AVRM_writeFileLog("[ContactLessCardXChangeAPDU() Exit] Operation timeout.",ERROR);
									 return OPERATION_TIMEOUT_OCCURRED;
									 
							}//else end
              
		       }//else end failure case
		    

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            AVRM_writeFileLog("[ContactLessCardXChangeAPDU() Exit] Command Transmit failed",ERROR);
            ReplyAPDU[0] = OTHER_ERROR ;//Other Error
            *ReplyAPDULength = 1;
            return OTHER_ERROR;
            
        }//else end Fail Transmit command

}//int ContactLessCardXChangeAPDU(char* CommandAPDU,char* ReplyAPDU) end

//++SAM CARD XCHNAGE APDU
int ContactCardXChangeAPDU(  char *CommandAPDU,int   CommandAPDULength,char *ReplyAPDU,int  *ReplyAPDULength,char *LogdllPathstr,char *LogFileName,char *deviceid,int  fnLogfileMode ) {

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        AVRM_writeFileLog("[ContactCardXChangeAPDU()] Entry ",TRACE);

        char log[LOG_ARRAY_SIZE];
        unsigned int rcvPktLen=0;

        memset(log,'\0',LOG_ARRAY_SIZE);

        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ContactCardXChangeAPDU()] Command APDU Length = %d .",CommandAPDULength);
        AVRM_writeFileLog(log,INFO);

        int TotalCommandLength=0;
        //3 Byte: CMT+PM+CM        
        TotalCommandLength = CommandAPDULength+3;

        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ContactCardXChangeAPDU()] Total Comamnd Length = %d .",TotalCommandLength);
        AVRM_writeFileLog(log,INFO);

        char APDUCommand[MUTEK_REPLY_LENGTH_MAX];
        memset(APDUCommand,'\0',MUTEK_REPLY_LENGTH_MAX);
        int APDUCommandcounter = 0,APDUByteCounter=0,APDUByteLengthCounter=1;

        //Construct Contact Less Card DeActivate Command
        APDUCommand[0]  = 0xF2; //STX
        APDUCommand[1]  = MTK_DEVICE_ADDRESS; //ADDR
        
        APDUCommand[2]  = 0x00; //LENH
        APDUCommand[3]  = TotalCommandLength; //LENL
        APDUCommand[4]  = 0x43; //CMH
        APDUCommand[5]  = 0x52 ; //CM
        APDUCommand[6]  = 0x34; //PM
        
        //Now fill APDU Command data byte
         
        APDUCommandcounter=7;
        APDUByteCounter=0;
        for(;APDUByteLengthCounter<=CommandAPDULength;)
	{

              APDUCommand[APDUCommandcounter]  = CommandAPDU[APDUByteCounter]; 
              APDUCommandcounter++; //Increment Local Command Array Counter 
              APDUByteCounter++;    //Increment Function C APDU Array Counter
              APDUByteLengthCounter++; //Increment CAPDU Command Length Counter

        }

        APDUCommand[APDUCommandcounter]  = 0x03; //ETX

        APDUCommand[APDUCommandcounter+1]  = 0x00; //BCC
      
        APDUCommandcounter = APDUCommandcounter+2;

        int CommandLength = APDUCommandcounter;

        char RecvPackt[300];

        memset(RecvPackt,'\0',300);
        
        //Reply Byte:

        //STX    = 1byte
        //ADDR   = 1 byte
        //LENGTH = 2 byte
        //CMH    = 1 byte
        //CM     = 1 byte
        //PM     = 1 byte
        //ST0    = 1 byte
        //ST1    = 1 byte
        //ST2    = 1 byte
        //R-APDU = MAX 258 Byte
        //ETX    = 1 byte
        //BCC    = 1 byte
    
        //Total Max Reply Byte = 270 Byte
        
        //int totalByteToRecv= 270; //Reply Byte Max Length 

        

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(APDUCommand,CommandLength) )
        {

                    ////writeFileLog("[ContactCardXChangeAPDU()] Command Transmit success");
                    AVRM_writeFileLog("[ContactCardXChangeAPDU()] Command Transmit success",DEBUG);

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                                ////writeFileLog("[ContactCardXChangeAPDU()] ACK receieve from MUTEK");
                                AVRM_writeFileLog("[ContactCardXChangeAPDU()] ACK receieve from MUTEK",DEBUG);

		                //Step 3: After Receieve ACK Go for read reply bytes
		        
                                if( false == MUTEK_CommandReceieve_V2(RecvPackt,&rcvPktLen))
                                {
                                        ReplyAPDU[0] =  OTHER_ERROR ;//Other Error
		                        *ReplyAPDULength = 1;
		                        ////writeFileLog("[ContactCardXChangeAPDU() Exit] Other Error Com read section");
		                        AVRM_writeFileLog("[ContactCardXChangeAPDU() Exit] Other Error Com read section",ERROR);
				        return OTHER_ERROR;

                                }else{
                                    ////writeFileLog("[ContactCardXChangeAPDU()] Reply Bytes receieve from MUTEK is success");
                                    AVRM_writeFileLog("[ContactCardXChangeAPDU()] Reply Bytes receieve from MUTEK is success",INFO);

                                }
		         
                                

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x34 == RecvPackt[6] )     //PM
		                  )
		                {
				             int ReplyAPDUCounter=1;
				             ReplyAPDU[0] = SUCCESS;//Success
		                             //Get Return R-APDU Length 
		                             int RceieveTextLength = 0;
		                             RceieveTextLength = (RecvPackt[2]<<8)+RecvPackt[3];

                                             if( rcvPktLen!= (RceieveTextLength+6) ){
                                                   
                                                  memset(log,'\0',LOG_ARRAY_SIZE);

		                                  sprintf(log,"[ContactCardXChangeAPDU()] Receieve Text length = %d  and Reply length Byte=%d not matched.",rcvPktLen,RceieveTextLength);
		                                  ////writeFileLog(log);
		                                  
		                                          AVRM_writeFileLog(log,INFO);

                                                  ReplyAPDU[0] =  OTHER_ERROR ; //other error

		                                  *ReplyAPDULength = 1;

                                                  return OTHER_ERROR;

                                             }

		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactCardXChangeAPDU()] Receieve Text length = %d .",RceieveTextLength);
		                             
                                            // //writeFileLog(log);
                                             
                                             AVRM_writeFileLog(log,INFO);

		                             int RAPDUTextLength =0,StartRAPDUIndex=10,CountDownLength=0;
		                             
		                             RAPDUTextLength = RceieveTextLength-6 ; //6 Byte: PMT+CM+PM+ST0+ST1+ST2
		                             
		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactCardXChangeAPDU()] APDU Text length = %d .",RAPDUTextLength);

		                             ////writeFileLog(log);
		                             
		                             AVRM_writeFileLog(log,INFO);

		                             for(;CountDownLength<RAPDUTextLength;CountDownLength++)
		                             {
		                                   ReplyAPDU[ReplyAPDUCounter] = RecvPackt[StartRAPDUIndex];

		                                   memset(log,'\0',LOG_ARRAY_SIZE);

		                                   sprintf(log,"[ContactCardXChangeAPDU()] ReplyAPDU[%d] = 0x%xh. RecvPackt[%d]=0x%xh.",ReplyAPDUCounter,(unsigned char)ReplyAPDU[ReplyAPDUCounter],StartRAPDUIndex,(unsigned char)RecvPackt[StartRAPDUIndex]);
		                                   
		                                   AVRM_writeFileLog(log,INFO);
		                                   
		                                   ReplyAPDUCounter++;
		                                   
		                                   StartRAPDUIndex++;
		                                  
		                             }

		                             //Now Set ReplyAPDULength
		                             *ReplyAPDULength = RAPDUTextLength+1;

		                              memset(log,'\0',LOG_ARRAY_SIZE);

		                              sprintf(log,"[ContactCardXChangeAPDU() Exit] Successfully get APDU and return array length = %d .",*ReplyAPDULength);
		                              
		                              AVRM_writeFileLog(log,INFO);
				              
		                              return SUCCESS;

				        }
		                        else
					{
						     
				                   ReplyAPDU[0] = OTHER_ERROR ;//Other Error
		                                   *ReplyAPDULength = 1;
		                                   ////writeFileLog("[ContactCardXChangeAPDU() Exit] Other Error Block1");
		                                   AVRM_writeFileLog("[ContactCardXChangeAPDU() Exit] Other Error Block1",ERROR);
				                   return 4;
					}

		         }
		         else //failure case
		         {

		                    ////writeFileLog("[ContactCardXChangeAPDU()] Reply Bytes receieve from MUTEK is failed ");
		                    AVRM_writeFileLog("[ContactCardXChangeAPDU()] Reply Bytes receieve from MUTEK is failed ",ERROR);

				    if( 0x4e == RecvPackt[4] ) //Negative PMT header
				    {
				             ReplyAPDU[0] =  OTHER_ERROR; //other error
		                             *ReplyAPDULength = 1;
		                             ////writeFileLog("[ContactCardXChangeAPDU() Exit] Negative ACK receieved");
		                             AVRM_writeFileLog("[ContactCardXChangeAPDU() Exit] Negative ACK receieved ",ERROR);
		                             return OTHER_ERROR;
				         
				    }//if( 0x4e == RecvPackt[4] )
				    else
				    {
				             
		                           ReplyAPDU[0] =  OPERATION_TIMEOUT_OCCURRED; //operation time out
		                           *ReplyAPDULength = 1;
		                           ////writeFileLog("[ContactCardXChangeAPDU() Exit] Operation timeout");
		                           AVRM_writeFileLog("[ContactCardXChangeAPDU() Exit] Operation timeout ",ERROR);
		                           return OPERATION_TIMEOUT_OCCURRED;
				    }
              
                      
				

		      }
		    

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            ////writeFileLog("[ContactCardXChangeAPDU() Exit] Command Transmit failed");
            AVRM_writeFileLog("[ContactCardXChangeAPDU() Exit] Command Transmit failed",ERROR);
            ReplyAPDU[0] = OTHER_ERROR ;//Other Error
            *ReplyAPDULength = 1;
            return OTHER_ERROR;
        }

}//int ContactCardXChangeAPDU(char* CommandAPDU,char* ReplyAPDU) end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ActivateSAMCard( int   SAMSlotId,int   Timeout,char *SAMAPIReply,char *LogdllPathstr,char *LogFileName,char *deviceid,int   fnLogfileMode) {


              SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
        
              char SAMReplyAPDU[100];
			     
              memset( SAMReplyAPDU,'\0',100);

              char log[LOG_ARRAY_SIZE];

              memset(log,'\0',LOG_ARRAY_SIZE);

              int counter=0;

              if( 1 == ActivateContactCard(SAMAPIReply) )
              {
                              
                              //Byte 0 Status of Card Activation
                              /*

                                0: Card found and activated
				1: Card found but activation failed
				2: Card found but it is unsupported
				28: Communication failure
				18: Operation timeout occurred
				31: Other error

                              */

		              SAMAPIReply[0]= CARD_FOUND_AND_ACTIVATED; //Card found and activated
					   
                              /*

			      for( counter =0 ; counter < 2; counter++)
			      {
		
				    memset(log,'\0',LOG_ARRAY_SIZE);

				    sprintf(log,"[ActivateSAMCard()] SAMAPIReply[%d] = 0x%xh.",counter,SAMAPIReply[counter]);
		
				    //writeFileLog(log);

			      }	

                              */

                              ////writeFileLog("[ActivateSAMCard()] Successfully activated sam card now going for get uid from it using exchange apdu api");
                              AVRM_writeFileLog("[ActivateSAMCard()] Successfully activated sam card now going for get uid from it using exchange apdu api",INFO);
		 
			      //Now get uid data from sam slot
			      char SAMCommandAPDU[5] ={0x80,0x60,0x00,0x00,0x00};
			      
			      int SAMCommandAPDULength = 5,SAMReplyAPDULength=0 ;
			      
			      if(SUCCESS == ContactCardXChangeAPDU( SAMCommandAPDU,
                                                              SAMCommandAPDULength,
                                                              SAMReplyAPDU,
                                                              &SAMReplyAPDULength,
                                                              LogdllPathstr,
                                                              LogFileName,
                                                              deviceid,
                                                              fnLogfileMode
                                                            ) 
                                )
			      {

                                         
					     
		                           //Byte 1 Type of Card Found 
                                           // SAMReplyAPDU[31] hold card type 
                                           // a1 : sam av1 card 
                                           // a2 : sam av2 card    
                                           unsigned char samcardtype = (unsigned char)SAMReplyAPDU[31]  ;      
                                           if( 0xa1 == samcardtype)
                                           {
                                               SAMAPIReply[1]= 1; //SAM AVI 1 
                                           }
                                           else if( 0xa2 == samcardtype)
                                           {
                                               SAMAPIReply[1]= 2; //SAM AVI 2 
                                           }
                           
		                          
                                           //Byte 2: uid length
                                           SAMAPIReply[2] =  7; //7 byte uid length

				           //Byte 3-9 : uid bytes 
                                           //Now copy uid data
				           //SAMReplyAPDU[15] to SAMReplyAPDU[21] hold uid data
				          
				           SAMAPIReply[3] = (unsigned char) SAMReplyAPDU[15]; //start with 0x04
				           SAMAPIReply[4] = (unsigned char) SAMReplyAPDU[16];
				           SAMAPIReply[5] = (unsigned char) SAMReplyAPDU[17];
				      
				           SAMAPIReply[6] = (unsigned char) SAMReplyAPDU[18];
				           SAMAPIReply[7] = (unsigned char) SAMReplyAPDU[19];
				           SAMAPIReply[8] = (unsigned char) SAMReplyAPDU[20];

				           SAMAPIReply[9] = (unsigned char) SAMReplyAPDU[21]; //end with 0x80

                                           /*

                                           for( counter =0 ; counter < 10; counter++)
					   {
		
						  memset(log,'\0',LOG_ARRAY_SIZE);

						  sprintf(log,"[ActivateSAMCard()] SAMAPIReply[%d] = 0x%xh.",counter,(unsigned char)SAMAPIReply[counter]);
                                                  //writeFileLog(log);

					   }

                                           */
                                           ////writeFileLog("[ActivateSAMCard()] Successfully sam card activate with uid data get also sucess");
                                           AVRM_writeFileLog("[ActivateSAMCard()] Successfully sam card activate with uid data get also sucess",INFO);
                                           return true;

			      }
                              else
                              {
                                   ////writeFileLog("[ActivateSAMCard()] Successfully sam card activate without uid data ");
								   AVRM_writeFileLog("[ActivateSAMCard()] Successfully sam card activate without uid data",INFO);
                                   return false;
 
                              }			     
             

                }
                else
                {
                       //++//writeFileLog("[ActivateSAMCard()] Unable to activate sam card ");
					   AVRM_writeFileLog("[ActivateSAMCard()] Unable to activate sam card",ERROR);
                       return false;
               
                }


}//int ActivateSAMCard(int SAMSlotId,int Timeout,char *SAMAPIReply) end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool IsAckReceieve() {
       
          
		        //Now Going TO Check ACK from MUTEK   
                char log[LOG_ARRAY_SIZE];
                
                memset(log,'\0',LOG_ARRAY_SIZE);

				int totalByteIn = -1;
				int send_rvalue = 0;
				char rxByte =0x00;
				int retry = 1;
				for(retry = 1;retry<=4;retry ++){
					delay_mSec(50);
					totalByteIn = read(g_ComHandle,&rxByte,1);
					if( (totalByteIn >0) && (0x06 == rxByte) ){
							break;
					}else{
							continue;
					}
				} //for end
				
				//++Disable by Malay Jun 10 2022
                //delay_mSec(50);
                //totalByteIn = read(g_ComHandle,&rxByte,1);

                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[IsAckReceieve()] Receieve ACK Byte: 0x%x Success.",rxByte);

				AVRM_writeFileLog(log,DEBUG);
				
				memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[IsAckReceieve()] ComPort Handle: %d ",g_ComHandle);

				AVRM_writeFileLog(log,INFO);
				
				if( (totalByteIn >0) && (0x06 == rxByte) )
				{
							  
					        ////////////////////////////////////////////////////////////
							
							  //Log ACK byte upon receieve complete data from mutek
							  char ack_Data[1]={0x06};
							  SCRD_LogDataExchnage(   DATA_EXCHANGE_ALL,
								 g_SCRDDeviceid,
								 "Rx",
								 ack_Data,
								 1
									  );
							 
							 //////////////////////////////////////////////////////////////
							 
							return true; 
					   
				}
				else
				{
					  AVRM_writeFileLog("[IsAckReceieve()] Receieve ACK Byte Failed.",ERROR);
					  return false; 

				}


}//IsAckReceieve() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool SendAckToMutek() {
	
                      char log[LOG_ARRAY_SIZE];

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      char data=0x06;

                      int rtcode =-1;
                      int send_rvalue = 0;

                      rtcode = write(g_ComHandle,&data,1);

                      if(rtcode < 0 )
		      {

			   //Unable to write any data to com port
                           ////writeFileLog("[SendAckToMutek()] Send ACK Byte Failed.");
                           AVRM_writeFileLog("[SendAckToMutek()] Send ACK Byte Failed.",ERROR);
                           return false;



		      }//if(rtcode<0)
                      else 
                      {
                           //write data to com port success
                           sprintf(log,"[SendAckToMutek()] Send ACK Byte: 0x%x Success.",data);
			   ////writeFileLog(log);
			               AVRM_writeFileLog(log,DEBUG);
                           ////////////////////////////////////////////////////////////
                           
                           //Log ACK byte upon receieve complete data from mutek
		           char ack_Data[1]={0x06};
		           SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
				            g_SCRDDeviceid,
				            "Tx",
				            ack_Data,
					    1
				          );
		              
		           //////////////////////////////////////////////////////////////
			   return true;


                      }//else block





}//SendAckToMutek() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool SendNakToMutek(){

                      char log[LOG_ARRAY_SIZE];

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      char data=0x15;

                      int rtcode =-1;
                      int send_rvalue = 0;

                      rtcode = write(g_ComHandle,&data,1);

                      if(rtcode < 0 )
		      {
                           //Unable to write any data to com port
                           ////writeFileLog("[SendNakToMutek()] Send NAK Byte failed.");
                            AVRM_writeFileLog("[SendNakToMutek()] Send NAK Byte failed.",ERROR);
                           return false;



		      }//if(rtcode<0)
                      else 
                      {
                           //write data to com port success
                           sprintf(log,"[SendNakToMutek()] Send NAK Byte: 0x%x Success",data);
			   ////writeFileLog(log);
			                AVRM_writeFileLog(log,INFO);
                           ////////////////////////////////////////////////////////////
                           
                           //Log ACK byte upon receieve complete data from mutek
		           char nak_Data[1]={0x15};
		           SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,
				            g_SCRDDeviceid,
				            "Tx",
				            nak_Data,
					    1
				          );
		              
		           //////////////////////////////////////////////////////////////

			   return true;


                      }//else block





}//SendNakToMutek() end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool MUTEK_CommandReceieve(char *RecvPackt,int totalByteToRecv) {


                      char log[LOG_ARRAY_SIZE];

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      int rcvPktLen=0,rtcode=-1,counter=0;

                      //delay_mSec(100);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[MUTEK_CommandReceieve()] Device Handle = %d ",g_ComHandle);
        
                      AVRM_writeFileLog(log,INFO);
					 
					 //++Add by Malay 10 jun 2022
					  delay_mSec(100); 
					  
                      rtcode = statusRead(g_ComHandle,RecvPackt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[MUTEK_CommandReceieve()] Recv Packet Length = %d ",rcvPktLen);
        
                      AVRM_writeFileLog(log,INFO);

                      /*

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[MUTEK_CommandReceieve()] statusRead() Return Code = %d and Receieve packet Length = %d .",rtcode,rcvPktLen);
        
                      //writeFileLog(log);

                      

                      for(counter=0;counter<rcvPktLen;counter++)
		              { 
				    
				
                                memset(log,'\0',LOG_ARRAY_SIZE);

                                sprintf(log,"[MUTEK_CommandReceieve()] RecvPackt[%d] = 0x%x.",counter,RecvPackt[counter]);
        
                                //writeFileLog(log);
				
	                  }

                      */
             
                      if(0 == rtcode)
		             { 

								//Timeout and less data recieved
                               SendNakToMutek();
                               return false;

                      }//if(0 == rtcode)
                      else if(1 == rtcode)
                      {
                               //Successfully Return
                               SendAckToMutek();
                               return true;


                      }//else if(1 == rtcode)

}//MUTEK_CommandReceiev() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool MUTEK_CommandReceieve_V2(char *RecvPackt,unsigned int *rcvPktLen) {


                      char log[LOG_ARRAY_SIZE];

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      int rtcode=-1,counter=0,CalCulatedrcvPktLen=0;
                      
                      unsigned char CalcultedBCC=0;

                      *rcvPktLen=0;

					   //++Add by Malay 10 jun 2022
					  delay_mSec(100); 
					  
                      rtcode = statusRead_V2(g_ComHandle,RecvPackt,rcvPktLen);

                      /*

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[MUTEK_CommandReceieve_V2()] statusRead() Return Code = %d and Receieve packet Length = %d .",rtcode,rcvPktLen);
        
                      //writeFileLog(log);

                      
                      for(counter=0;counter<rcvPktLen;counter++)
		      { 
				    
				
                                memset(log,'\0',LOG_ARRAY_SIZE);

                                sprintf(log,"[MUTEK_CommandReceieve_V2()] RecvPackt[%d] = 0x%x.",counter,RecvPackt[counter]);
        
                                //writeFileLog(log);
				
	              }

                      */


                      if(0 == rtcode)
					  {
							   AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout] Timeout or less data recieved",DEBUG);
                               //Timeout and less data recieved
                               if( (0xF2 == RecvPackt[0]) && (MTK_DEVICE_ADDRESS == RecvPackt[1]) && (*rcvPktLen>=4) )
                               {          
                                          CalCulatedrcvPktLen = (RecvPackt[2]<<8)+RecvPackt[3]+6;//6 Byte: STX+ADDR+LENH+LENL+ETX+BCC
                                          if( *rcvPktLen < CalCulatedrcvPktLen   ){
                                               ////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Only Less data recieved");
                                               AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Only Less data recieved",DEBUG);
                                               //++SendNakToMutek();
                                               return false;
                                          }
                                          else if(*rcvPktLen > CalCulatedrcvPktLen)  {
                                               ////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Length Byte error recieved");
                                               AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Length Byte error recieved",DEBUG);
                                               //++SendNakToMutek(); 
                                               return false;
                                          }
                                          else if(  *rcvPktLen > 1024  ){
                                               ////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] greater data recieved");
                                               AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] greater data recieved",DEBUG);
                                               return false;
                                               
                                          }
                                          else if(CalCulatedrcvPktLen == *rcvPktLen ){
                                               
                                               ////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Equal data receieved");
                                               AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Equal data receieved",DEBUG);
                                               CalcultedBCC = getBcc(*rcvPktLen-1 , RecvPackt );
                                               
                                               memset(log,'\0',LOG_ARRAY_SIZE);
                                               sprintf(log,"[MUTEK_CommandReceieve_V2()] Chksum = 0x%x.",CalcultedBCC );
                                               ////writeFileLog(log);
                                               AVRM_writeFileLog(log,DEBUG);

                                               memset(log,'\0',LOG_ARRAY_SIZE);
                                               sprintf(log,"[MUTEK_CommandReceieve_V2()] Recv Packet Chksum = 0x%x.",(unsigned char)RecvPackt[*rcvPktLen-1] );
                                               ////writeFileLog(log);
                                               AVRM_writeFileLog(log,DEBUG);

                                               if(CalcultedBCC != (unsigned char) RecvPackt[*rcvPktLen-1]){
                                                  ////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Chksum Failed");
                                                  AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout if] Chksum Failed",ERROR);
                                                  //++SendNakToMutek();
                                                  return false;
                                               }
                                               else
                                               {
                                                    //Successfully Return
													SendAckToMutek();          
													////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout] Successfully data recieved");
													AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout] Successfully data recieved",DEBUG);
													return true;
				                    
                                               }
                                          }
                                          else{
                                                 
                                                 ////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout] Grabage Data receieved.");
                                                 AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout] Grabage Data receieved",ERROR);
                                                 
				                 return false;

                                          }
                                       
                               }
                               else{
                                      ////writeFileLog("[MUTEK_CommandReceieve_V2() Timeout else] Grabage Data receieved.");
                                      AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() Timeout else] Grabage Data receieved..",ERROR);
									  return false;
                               }
                               
                               //++SendNakToMutek();
                               //++return false; 

                      }//if(0 == rtcode)
                      else if(1 == rtcode)
                      {  
                               
                               CalCulatedrcvPktLen = 0;
                               CalCulatedrcvPktLen = (RecvPackt[2]<<8)+RecvPackt[3]+6;//6 Byte: STX+ADDR+LENH+LENL+ETX+BCC
                               CalcultedBCC = 0;

                               memset(log,'\0',LOG_ARRAY_SIZE);
                               sprintf(log,"[MUTEK_CommandReceieve_V2() SuccessBlock] Recv Packt Length = %d.",*rcvPktLen );
                               AVRM_writeFileLog(log,DEBUG);

                               CalcultedBCC = getBcc(*rcvPktLen-1 , RecvPackt );

                               
                               memset(log,'\0',LOG_ARRAY_SIZE);
                               sprintf(log,"[MUTEK_CommandReceieve_V2() SuccessBlock] Chksum = 0x%x.",CalcultedBCC );
                               AVRM_writeFileLog(log,DEBUG);

                               memset(log,'\0',LOG_ARRAY_SIZE);
                               sprintf(log,"[MUTEK_CommandReceieve_V2() SuccessBlock] Recv Packet Chksum = 0x%x.",(unsigned char)RecvPackt[*rcvPktLen-1] );
                               AVRM_writeFileLog(log,DEBUG);
 
                               if( CalcultedBCC != (unsigned char)RecvPackt[*rcvPktLen-1] ){
                                        ////writeFileLog("[MUTEK_CommandReceieve_V2() SuccessBlock] Chksum Failed");
                                        AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() SuccessBlock] Chksum Failed",ERROR);
                                        SendNakToMutek();
                                        return false;
                               }
                               else{
                          
                                    //Successfully Return
                                    SendAckToMutek();          
                                    AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() SuccessBlock] Successfully data recieved",DEBUG);
                                    return true;

                               }
                               
                               //++SendAckToMutek();
                               //++AVRM_writeFileLog("[MUTEK_CommandReceieve_V2() SuccessBlock] Successfully data recieved",DEBUG);
                               //++return true;
                               

                      }//else if(1 == rtcode)
                      else{
                          AVRM_writeFileLog("[MUTEK_CommandReceieve_V2()] other error",ERROR);
                          return false;
                      }//else end


}//MUTEK_CommandReceiev() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool MUTEK_CommandTransmit(char *Command,int CommandLength){


        //////////////////////////////////////////////////////////////////////////
        
        char log[LOG_ARRAY_SIZE];
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        
        sprintf(log,"[MUTEK_CommandTransmit()] Device Handle = %d ",g_ComHandle);
        
        AVRM_writeFileLog(log,DEBUG);
        
        unsigned char bcc=0x00;

        int  counter=0,rtcode=-1;

        bcc = getBcc(CommandLength-1,Command);

        Command[CommandLength-1] = bcc;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[MUTEK_CommandTransmit()] bcc value is 0x%xh",bcc);
        
        AVRM_writeFileLog(log,DEBUG);

        //Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        /////////////////////////////////////////////////////////////////////////////////////////

        
        /*

        //Display Command
        for(counter=0;counter<CommandLength;counter++)
		{ 
				
				memset(log,'\0',LOG_ARRAY_SIZE);

				sprintf(log,"[MUTEK_CommandTransmit()] Command[%d] = 0x%x",counter,Command[counter]);
			
				//writeFileLog(log);
					
		}
        

        */

        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SCRD_LogDataExchnage( DATA_EXCHANGE_ALL,g_SCRDDeviceid,"Tx",Command,CommandLength);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[MUTEK_CommandTransmit()] Device Handle = %d ",g_ComHandle);
        
        ////writeFileLog(log);
        
        AVRM_writeFileLog(log,DEBUG);

        //Now Send Command to MUTEK
        for(counter=0;counter<CommandLength;counter++)
		{
			
					 rtcode = -1;
					 rtcode = SendSingleByteToSerialPort(g_ComHandle,Command[counter]);
					 if(FAIL  == rtcode ) 
					 {
						////writeFileLog("[MUTEK_CommandTransmit()] Failed to Send Command to MUTEK Device");
						AVRM_writeFileLog("[MUTEK_CommandTransmit()] Failed to Send Command to MUTEK Device",ERROR);
						//Communication Failure
						return false;
					 }//if end


		}//for loop

        ////writeFileLog("[MUTEK_CommandTransmit()] Successfully Transmit Command");

        return true; //++Successfully send to MUTEK
        

}//MUTEK_CommandTransmit() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SMARTCARDMAIN
/*
int main()
{

             char ReplyDeviceStatus[10];    
         
             memset( ReplyDeviceStatus,'\0',10 );
           
             SetupSmartCardLogFile();
             
             ///////////////////////////////////////////////////////////////////////////

	     char log[LOG_ARRAY_SIZE];

	     memset(log,'\0',LOG_ARRAY_SIZE);

             char APIReply[10]; 
	     
	     memset(APIReply,'\0',10);

             char SAMAPIReply[10]; 
	     
	     memset(SAMAPIReply,'\0',10);

             int counter = 0;

	     //////////////////////////////////////////////////////////////////////////////////////

	     SetupSmartCardLogFile();

	     ///////////////////////////////////////////////////////////////////////////////////////

	     ConnectDevice_c(0,0,0); 
             //writeFileLog("\n\n");         

             while(1)
             {         
                     DeviceStatus_C( 0 , 0,ReplyDeviceStatus );
                     for( counter =0 ; counter < 10; counter++)
		     {
		
			    memset(log,'\0',LOG_ARRAY_SIZE);

			    sprintf(log,"[main()] ReplyDeviceStatus[%d] = 0x%xh.",counter,ReplyDeviceStatus[counter]);
		
			    //writeFileLog(log);

		      }	
                      //writeFileLog("\n\n");
                      break;
                      
             }

             ////////////////////////////////////////////////////////////////////////////////////////

             /*

             char ChanelStatus =0x00;

             char MTKSensorStatus=0x00;

             GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);

             memset(log,'\0',LOG_ARRAY_SIZE);

             sprintf(log,"[main()] ChanelStatus = 0x%xh. Sensor Status=0x%xh.",ChanelStatus,MTKSensorStatus);
		
	     //writeFileLog(log);
	   
	     //EnableCardAcceptance_c(0);       

             */

	     ///////////////////////////////////////////////////////////////////////
             
             /*

             counter =1;
	     
             while(1)
             {

	        if( 1 == IsCardInChannel_c(0) )
                {
                     delay_mSec(300); 

                     //writeFileLog("=====================================================================");
	     
		     //writeFileLog("[main()] Now Going for activate card");
		     
		     int APIReplyLength=0;

		     ActivateContactLessCard(APIReply,&APIReplyLength);
		     
		     for( counter =0 ; counter < APIReplyLength; counter++)
		     {
		
			    memset(log,'\0',LOG_ARRAY_SIZE);

			    sprintf(log,"[main()] APIReply[%d] = 0x%xh.",counter,APIReply[counter]);
		
			    //writeFileLog(log);

		      }	
	
		      //writeFileLog("====================================================================");

                      return ;

                }


             }//while loop

             

             return ;

             

	     ///////////////////////////////////////////////////////////////////////////////////////////////////////////

	     
	     
	     //writeFileLog("=================================================================");

	     /*

	     //writeFileLog("[main()] Now Check any card present in card reader");

	     if ( 1 == IsAnyCardPresentInReaderPosition() ) 
	     {
		//writeFileLog("[main()] Card is present ");

	     }

	     int cardtype =0;
	     cardtype = CheckCardType();
	     if( 4!= cardtype )
	     {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] Card Type: %d",cardtype);
		
		    //writeFileLog(log);

	     }		 

	    */

             ///////////////////////////////////////////////////////////////////////////////////////////////////
           
             
             /*

             //ContactLess card activate and deactivate
	     //writeFileLog("=====================================================================");
	     
	     //writeFileLog("[main()] Now Going for activate card");
	     
             int APIReplyLength=0;

	     ActivateContactLessCard(APIReply,&APIReplyLength);
	     
	     for( counter =0 ; counter < APIReplyLength; counter++)
	     {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] APIReply[%d] = 0x%xh.",counter,APIReply[counter]);
		
		    //writeFileLog(log);

	      }	
	
	      //writeFileLog("====================================================================");
	  
              //writeFileLog("[main()] Before Send APDU Command");

              //char CommandAPDU =0x60 ;
              char CommandAPDU[4] ={0x5A,0x00,0x01,0x00};
              int CommandAPDULength = 4,ReplyAPDULength=0 ;
              char ReplyAPDU[300];
              memset( ReplyAPDU,'\0',300);
              
              ContactLessCardXChangeAPDU(CommandAPDU,CommandAPDULength,ReplyAPDU,&ReplyAPDULength);

              memset(log,'\0',LOG_ARRAY_SIZE);

	      sprintf(log,"[main()] Reply APDU Array Length = %d ",ReplyAPDULength);
		
	      //writeFileLog(log);

              for( counter =0 ; counter < ReplyAPDULength; counter++)
	      {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] ReplyAPDU[%d] = 0x%xh.",counter,ReplyAPDU[counter]);
		
		    //writeFileLog(log);

	      }	

              //writeFileLog("[main()] After Send APDU Command");

              //writeFileLog("====================================================================");
              
              

	      int Reply=-1;
	      DeActivateContactLessCard(&Reply);
	      memset(log,'\0',LOG_ARRAY_SIZE);
	      sprintf(log,"[main()] Deactivate Return Code: %d",Reply);
	      //writeFileLog(log);
	      //writeFileLog(" =======================================================");

              */
             
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////             

             /*

             //writeFileLog("=====================================================================");
             
             memset(APIReply,'\0',10);

             ActivateContactCard(APIReply);

             for( counter =0 ; counter < 10; counter++)
	     {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] APIReply[%d] = 0x%xh.",counter,APIReply[counter]);
		
		    //writeFileLog(log);

	     }	
             
             //writeFileLog("=====================================================================");

             WarmResetSAM(1);

             //writeFileLog("=====================================================================");


             */
             ///////////////////////////////////////////////////////////////////////////////////////////////////////

              /*
              ////writeFileLog("Going to Select SAM Slot1");

              //SelectSAM(1);

              //writeFileLog("=====================================================================");
 
              ActivateContactCard(SAMAPIReply);
           
              for( counter =0 ; counter < 2; counter++)
	      {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] SAMAPIReply[%d] = 0x%xh.",counter,SAMAPIReply[counter]);
		
		    //writeFileLog(log);

	      }	
 

              //writeFileLog("=====================================================================");
              
              char SAMCommandAPDU[5] ={0x80,0x60,0x00,0x00,0x00};
              
              int SAMCommandAPDULength = 5,SAMReplyAPDULength=0 ;
              
              char SAMReplyAPDU[300];
              
              memset( SAMReplyAPDU,'\0',300);

              if(1 == ContactCardXChangeAPDU(SAMCommandAPDU,SAMCommandAPDULength,SAMReplyAPDU,&SAMReplyAPDULength) )
              {

                           SAMAPIReply[2] =  6; //6 byte uid length

                           //Now copy uid data
                           //SAMReplyAPDU[15] to SAMReplyAPDU[21] hold uid data
                          
                           SAMAPIReply[3] = SAMReplyAPDU[15]; //start with 0x04
                           SAMAPIReply[4] = SAMReplyAPDU[16];
                           SAMAPIReply[5] = SAMReplyAPDU[17];
                      
                           SAMAPIReply[6] = SAMReplyAPDU[18];
                           SAMAPIReply[7] = SAMReplyAPDU[19];
                           SAMAPIReply[8] = SAMReplyAPDU[20];

                           SAMAPIReply[9] = SAMReplyAPDU[21]; //end with 0x80

              }


              for( counter =0 ; counter < 10; counter++)
	      {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] SAMAPIReply[%d] = 0x%xh.",counter,SAMAPIReply[counter]);
		
		    //writeFileLog(log);

	      }	

              //writeFileLog("=====================================================================");

              int DeActivateSamAPIReply =-1;

              DeActivateContactCard(&DeActivateSamAPIReply)  ; 

              //writeFileLog("=====================================================================");
         
             */

	     ///////////////////////////////////////////////////////////////////////////////////////////////////////
	     
	     //DisConnectDevice_c(0);
				  
	     //////////////////////////////////////////////////////////////////////////////////////////////////////




//}//main() end


#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

