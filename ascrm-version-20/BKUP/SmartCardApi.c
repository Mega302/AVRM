#include "SmartCardApi.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int g_ComHandle;

static unsigned char g_getStatus[15] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 

static unsigned char g_DisableCardAcceptance[9]={0x00,MTK_DEVICE_ADDRESS,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 

static unsigned char g_EnableCardAcceptance[9]={0x00,MTK_DEVICE_ADDRESS,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static unsigned char g_moveToErrorBin[9]={0x00,MTK_DEVICE_ADDRESS,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static unsigned char g_resetData_Retain_Channel[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x30,0x33,0x03,0x00}; 

static unsigned char g_resetData_Capture_Card_Bin[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x30,0x31,0x03,0x00}; 

static unsigned char g_resetData_CardHolding_Position[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x30,0x30,0x03,0x00}; 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void SmartCardSetupLog(char *LogdllPathstr,char *LogFileName,char *deviceid,int fnLogfileMode )
{


               //printf("\n[SmartCardSetupLog()] Entry");

               
               ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

 
               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

               g_Ascrm_writeFileLog=NULL;

	       g_lib_handle=NULL;

	       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	       
	       //Step 1:Log dll Path
               //printf("\n[SmartCardSetupLog()] Log dll Path : %s", LogdllPathstr);
               SetSoFilePath((unsigned char*) LogdllPathstr);

	       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               
               //Step 2:Log File Name
               //printf("\n[SmartCardSetupLog()] Log File Name : %s", LogFileName);
               SetGeneralFileLogPath( (unsigned char*) LogFileName );
               
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               
               //Step 3: Set LogLevel Mode
               SetSoLogFileMode((int)fnLogfileMode );
               //printf("\n[SmartCardSetupLog()] Log File Mode : %d", (int)fnLogfileMode);
               
               //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

               //Step 4: Get Device id
               //printf("\n[SmartCardSetupLog()] Log dll Path : %s", deviceid);
               
               SetDeviceID(deviceid);

               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
               //writeFileLog("[SmartCardSetupLog()] I am Write by log");

               //printf("\n[SmartCardSetupLog()] Exit");

               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////



}//void SmartCardSetupLog() end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





int CardBlockStatusRequest ( char *LogdllPathstr,
                             char *LogFileName,
                             char *deviceid,
                             int fnLogfileMode
                           )
{


        
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

                    writeFileLog("[CardBlockStatusRequest()] Command Transmit success");

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[CardBlockStatusRequest()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[CardBlockStatusRequest()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] )  &&  //CMH
		                    ( 0x31 == RecvPackt[5] )  &&  //CM
		                    ( 0x30 == RecvPackt[6] )      //PM
		                  )
		                {
                                       writeFileLog("[CardBlockStatusRequest() Exit] Card not blocked ");
		                       return 0; //card not blocked                  
                                    
				}// if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                //      ( 0x31 == RecvPackt[5] ) &&  //CM
		                //      ( 0x31 == RecvPackt[6] )     //PM
		                //    ) end
								//Soomit-29082016: You are possibly missing the other error conditions here [Solved]
                                else if( 0x4e == RecvPackt[4] ) 
                                 
                                {
                                      //Log e1 and e2 error code
                                      memset(log,'\0',LOG_ARRAY_SIZE);

                                      sprintf(log,"[CardBlockStatusRequest() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                      writeFileLog(log);

                                      if(( 0x31 == RecvPackt[7] ) && 
                                         ( 0x36 == RecvPackt[8] ) )
                                      {
                                         writeFileLog("[CardBlockStatusRequest() Exit] Card blocked Found");
		                         return 1; //card blocked  
                                      }  
                                      else if(( 0x31 == RecvPackt[7] ) && 
                                              ( 0x30 == RecvPackt[8] ) )
                                      {
                                         writeFileLog("[CardBlockStatusRequest() Exit] Card Jam Found");
		                         return 2; //card jammed
                                      } 
                                      else
                                      {
                                         writeFileLog("[CardBlockStatusRequest() Exit] Other Error Found");
                                         return (-1); //other error 
                                        
                                      }   
                                      
                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[CardBlockStatusRequest() Exit] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {

		                 if(  ( 0x4e == RecvPackt[4] ) && 
                                      ( 0x31 == RecvPackt[7] ) && 
                                      ( 0x36 == RecvPackt[8] ) )
		                 {
                                     writeFileLog("[CardBlockStatusRequest() Exit] Card Block Status Found");
                                     return 1;
		                 
		                 }//if(( 0x4e == RecvPackt[4] ) && 
                                 //    ( 0x4e == RecvPackt[4] )   && 
                                 //    ( 0x4e == RecvPackt[4] ) )
                                 else
                                 {
                                      writeFileLog("[CardBlockStatusRequest() Exit] No Card Block Status Found");
                                      return 0;

                                 }
		                     
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[CardBlockStatusRequest() Exit] reply byte receieve from mutek");
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[CardBlockStatusRequest() Exit] fail receieve ack byte from mutek");
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[CardBlockStatusRequest() Exit] Command Transmit failed");
            return 0;
        }



}//int CardBlockStatusRequest() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int GetCardBlockStatus( char *LogdllPathstr,
                        char *LogFileName,
                        char *deviceid,
                        int  fnLogfileMode)
{


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

                    writeFileLog("[GetCardBlockStatus()] Command Transmit success");

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[GetCardBlockStatus()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[GetCardBlockStatus()] Reply Bytes receieve from MUTEK is success");

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
                                          writeFileLog("[GetCardBlockStatus() Exit] No Block Card Found ");
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
                                          writeFileLog("[GetCardBlockStatus() Exit] Block Card Found ");
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
                                          writeFileLog("[GetCardBlockStatus() Exit] init with card retain found ");
                                          return 0;
				     }
                                     else if( ( 0x31 == RecvPackt[10]) &&
                                              ( 0x30 == RecvPackt[11]) &&
                                              ( 0x30 == RecvPackt[12]) &&
                                              ( 0x30 == RecvPackt[13]) 
                                       )
		                     {
				          //init with card infront found      
                                          
                                          writeFileLog("[GetCardBlockStatus() Exit] init with card in front found ");
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
                                          writeFileLog("[GetCardBlockStatus() Exit] half insert card in front found ");
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
                                          writeFileLog("[GetCardBlockStatus() Exit] Full Forcefully insert card in the chanel found ");
                                          return 1;

				     }

                                     
                                    
				    
                               }// if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                //     ( 0x31 == RecvPackt[5] ) &&  //CM
		                //     ( 0x31 == RecvPackt[6] )     //PM
		                //   ) end

		         }
		         else //failure case
		         {

                            writeFileLog("[GetCardBlockStatus() Exit] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     writeFileLog("[GetCardBlockStatus() Exit] negative ack from mutek");
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[GetCardBlockStatus() Exit] reply byte receieve from mutek");
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[GetCardBlockStatus() Exit] fail receieve ack byte from mutek");
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[GetCardBlockStatus() Exit] Command Transmit failed");
            return 0;
        }


}//int GetCardBlockStatus() end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int AscrmMutekInitWithCardRetain()
{

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

                    writeFileLog("[MutekInitWithCardRetain()] Command Transmit success");

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[MutekInitWithCardRetain()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[MutekInitWithCardRetain()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x33 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside success");
                                     return 1;
                                    
				    
                                }
                                else
                                {
                                     writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside failed");
                                     return 0;

                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[MutekInitWithCardRetain() Exit] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     writeFileLog("[MutekInitWithCardRetain() Exit] negative ack from mutek");
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[MutekInitWithCardRetain() Exit] reply byte receieve from mutek");
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek");
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[MutekInitWithCardRetain() Exit] Command Transmit failed");
            return 0;
        }


}//int GetCardBlockStatus() end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int MutekInitWithCardRetain( char *LogdllPathstr,
                             char *LogFileName,
                             char *deviceid,
                             int  fnLogfileMode)
{


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

                    writeFileLog("[MutekInitWithCardRetain()] Command Transmit success");

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[MutekInitWithCardRetain()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[MutekInitWithCardRetain()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x33 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside success");
                                     return 1;
                                    
				    
                                }
                                else  if( 0x4e == RecvPackt[4] )
                                {
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardRetain() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     writeFileLog(log); 

                                     writeFileLog("[MutekInitWithCardRetain() Exit] negative reply from mutek");
                                     return 0;
                                }
                                else
                                {
                                     writeFileLog("[MutekInitWithCardRetain()] Init with card retain inside failed");
                                     return 0;

                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[MutekInitWithCardRetain() Exit] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {

                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardRetain() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     writeFileLog(log); 

                                     writeFileLog("[MutekInitWithCardRetain() Exit] negative reply from mutek");

                                     writeFileLog("[MutekInitWithCardRetain() Exit] negative ack from mutek");
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[MutekInitWithCardRetain() Exit] reply byte receieve from mutek");
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek");
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[MutekInitWithCardRetain() Exit] Command Transmit failed");
            return 0;
        }


}//int GetCardBlockStatus() end



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int MutekInitWithCardMoveInFront(char *LogdllPathstr,
                                 char *LogFileName,
                                 char *deviceid,
                                 int  fnLogfileMode)
{

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

                    writeFileLog("[MutekInitWithCardMoveInFront()] Command Transmit success");

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[MutekInitWithCardMoveInFront()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[MutekInitWithCardMoveInFront()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     writeFileLog("[MutekInitWithCardMoveInFront()] Init with card move to front success");
                                     return 1;
                                    
				    
                                }
                                else if(0x4e == RecvPackt[4] ) 
                                {
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardMoveInFront() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     writeFileLog(log); 

                                     writeFileLog("[MutekInitWithCardMoveInFront() Exit] negative ack from mutek");
                                     return 0;

                                }
                                else
                                {
                                     writeFileLog("[MutekInitWithCardMoveInFront()] Init with card move to front failed");
                                     return 0;

                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[MutekInitWithCardMoveInFront() Exit] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {

                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithCardMoveInFront() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     writeFileLog(log);

                                     writeFileLog("[MutekInitWithCardMoveInFront() Exit] negative reply from mutek");
                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[MutekInitWithCardMoveInFront() Exit] reply byte receieve from mutek");
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[MutekInitWithCardRetain() Exit] fail receieve ack byte from mutek");
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[MutekInitWithCardMoveInFront() Exit] Command Transmit failed");
            return 0;
        }


}//int MutekInitWithCardMoveInFront() end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int MutekInitWithErrorCardBin( char *LogdllPathstr,
                               char *LogFileName,
                               char *deviceid,
                               int  fnLogfileMode)
{

      
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

                    writeFileLog("[MutekInitWithErrorCardBin()] Command Transmit success");

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[MutekInitWithErrorCardBin()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[MutekInitWithErrorCardBin()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x30 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                      writeFileLog("[MutekInitWithErrorCardBin()] Init with card move to front success");
                                      return 1;
                                    
				    
                                }
                                else if(0x4e == RecvPackt[4] ) 
                                {
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithErrorCardBin() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     writeFileLog(log); 

                                     writeFileLog("[MutekInitWithErrorCardBin() Exit] negative reply from mutek");
                                     return 0;

                                }
                                else
                                {
                                     writeFileLog("[MutekInitWithErrorCardBin()] Init with card move to front failed");
                                     return 0;

                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[MutekInitWithErrorCardBin() Exit] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     writeFileLog("[MutekInitWithErrorCardBin() Exit] negative reply from mutek");
                                     memset(log,'\0',LOG_ARRAY_SIZE);

                                     sprintf(log,"[MutekInitWithErrorCardBin() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

                                     writeFileLog(log); 

                                     return 0;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[MutekInitWithErrorCardBin() Exit] reply byte receieve from mutek");
                                return 0;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[MutekInitWithErrorCardBin() Exit] fail receieve ack byte from mutek");
                         return 0;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[MutekInitWithErrorCardBin() Exit] Command Transmit failed");
            return 0;

        }


}//int MutekInitWithErrorCardBin() end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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




int ConnectDevice_c( int  PortId, 
                     int  ChannelClearanceMode, 
                     int  Timeout,
                     char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode
                   )
{


        //printf("\n[ConnectDevice_c()] Log File Mode : %d", (int)fnLogfileMode);

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        writeFileLog("[ConnectDevice_c()] Entry ");

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
        writeFileLog("[ConnectDevice_c()] Going to Open Port");

        if(PortId < 0)
        {
           writeFileLog("[ConnectDevice_c()] Fail to Open Port ");
           // 1 = Port Does Not Exits 
           ClearReceiveBuffer (g_ComHandle);
           ClearTransmitBuffer(g_ComHandle);
           return 1;
        } 
        
        //Now Call Open Port Function  
        //get baudrate from ini file
        int baudrate=0;

        baudrate = GetComPortBaudRate();

        if( baudrate <=0 )
        {
            baudrate=38400;
        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        
        sprintf(log,"[ConnectDevice_c()] Baudrate: %d . ",baudrate);
            
        writeFileLog(log); 

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        g_ComHandle =-1;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        openPortStatus = OpenPort(PortId,baudrate,&g_ComHandle);
        
        if(1 == openPortStatus)
        {
           writeFileLog("[ConnectDevice_c()] Open Port Successfully");
        }
        else
        {
           writeFileLog("[ConnectDevice_c()] Fail to Open Port ");
           //1 = Port Does Not Exits 
           ClearReceiveBuffer (g_ComHandle);
           ClearTransmitBuffer(g_ComHandle);   
           return 1; 
        }

         
        //ChannelClearanceMode Check Here
        if(ChannelClearanceMode == 0)
        {
		    //Retain in the Channel 
		    //Now going to get bcc 
		    writeFileLog("[ConnectDevice_c()] Initialise with Retain in the Channel");
		    bcc = getBcc(9,g_resetData_Retain_Channel);
		    g_resetData_Retain_Channel[8] = bcc;
		    memset(log,'\0',LOG_ARRAY_SIZE);
		    sprintf(log,"[ConnectDevice_c()] bcc value is 0x%xh",bcc);
		    writeFileLog(log); 
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
		    writeFileLog("[ConnectDevice_c()] Initialise with Send to Rejection bin");
		    bcc = getBcc(9,g_resetData_Capture_Card_Bin);
		    g_resetData_Capture_Card_Bin[8] = bcc;
		    memset(log,'\0',LOG_ARRAY_SIZE);
		    sprintf(log,"[ConnectDevice_c()] bcc value is 0x%xh",bcc);
		    writeFileLog(log); 
		    memset(initialized_Data,'\0',9);
		    for(i=0;i<9;i++)
		    {
		          initialized_Data[i] = g_resetData_Capture_Card_Bin[i];
		    }  
	  
         }
         else if(ChannelClearanceMode == 2)
         {
		    // Return from the mouth of the Device 
		    // Now going to get bcc 
		    writeFileLog("[ConnectDevice_c()] Initialise with Return from the mouth of the Device");
		    bcc = getBcc(9,g_resetData_CardHolding_Position);
		    g_resetData_CardHolding_Position[8] = bcc;
		    memset(log,'\0',LOG_ARRAY_SIZE);
		    sprintf(log,"[ConnectDevice_c()] bcc value is 0x%xh",bcc);
		    writeFileLog(log); 
		    memset(initialized_Data,'\0',9);
		    for(i=0;i<9;i++)
		    {
		         initialized_Data[i] = g_resetData_CardHolding_Position[i];
		    } 
 
         }
         else
         {  
		    //ChannelClearanceMode does not match so return from this function 
		    writeFileLog("[ConnectDevice_c()] Initialise Failed");
		    writeFileLog("[ConnectDevice_c()] Invalid ChannelClearanceMode found from Parameters");
		    writeFileLog("[ConnectDevice_c()] Now going to Return");
		    //5 = Channel Clearance failed due to unknown reason 
		    ClearReceiveBuffer (g_ComHandle);
		    ClearTransmitBuffer(g_ComHandle);
		    return 5;

         }//else block
         
 
         writeFileLog("[ConnectDevice_c()] ChannelClearanceMode ok Now going to Send Initialize Data");
         //Before Send Command clear all serial buffer 

         ClearReceiveBuffer (g_ComHandle);

         ClearTransmitBuffer(g_ComHandle);


         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          initialized_Data,
                          9
                        );

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         for(i=0;i<9;i++)
	 {
                 
		         //memset(log,'\0',LOG_ARRAY_SIZE);

		         //sprintf(log,"[ConnectDevice_c()] Initialize Command[%d] = 0x%xh",i,initialized_Data[i]);

		         //writeFileLog(log); 

			 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,initialized_Data[i]);

		         if(0 == send_rvalue) 
		         {
		            writeFileLog("[ConnectDevice_c()] Failed to send total Initialize Command ");
		            return 2;
                         }


	 }//for(i=0;i<9;i++)



         writeFileLog("[ConnectDevice_c()] Initialize Data send Successfully");

         writeFileLog("[ConnectDevice_c()] Now Going to read Acknowledgement");
         
         //Now going to Check Acknowledgement  

         totalByteToRecv = 1;

         memset(rcvPkt,'\0',24); 
         
         rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

         memset(log,'\0',LOG_ARRAY_SIZE);

         sprintf(log,"[ConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

         writeFileLog(log); 

         if(0 == rtcode)
         {
                 // 2 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[ConnectDevice_c()] Communication Failure when Going to read Acknowledgement");
                 return 2;
         }
         else
	 {
		 
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[ConnectDevice_c()] Acknowledgement against Initialize Command [0] = 0x%xh.\n",rcvPkt[0]);

                writeFileLog(log); 
				
	 }
         
         //If Return Data is 06h then Going to Read 25byte Data 

         if(0x06 == rcvPkt[0])
         {

                      writeFileLog("[ConnectDevice_c()] Acknowledgement Received");

                      totalByteToRecv = 25;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[ConnectDevice_c()] statusRead() return code = %d and receieve packet Length = %d",rtcode,rcvPktLen);

                      writeFileLog(log); 
                     
                      if(0 == rtcode)
		      {
				 // 2 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[ConnectDevice_c()] Communication Failure when Going to read Initialize reply bytes");
				 return 2;
		      
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

                          sprintf(log,"[ConnectDevice_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[ConnectDevice_c() Exit] Got 0x4e error: others error ");
 
                          return 7 ; //Other Error


                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end

                            

                      //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[ConnectDevice_c()] Initialize Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                                //writeFileLog(log); 

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] No Card in MTK-571");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] One Card in Gate");
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] One Card on RF/IC Card Position");
		                        } 
                                }

                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] Enough Card in the Box");
		                        }   
                                } 

                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[ConnectDevice_c()] Error Card Bin Full");
                                           
                                           // 3 = Channel Clearance failed due to rejection bin full 
                                           cardBinFullFlag = 1;
		                        }
                                        

                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++)

 
                      ////////////////////////////////////////////////////////////////////////////

                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            writeFileLog("[ConnectDevice_c() Exit] Failed to send total Acknowledgement Command ");
                            // 2 = Communication Failure 
                            return 2;
		      }
                       
                      if(1 == cardBinFullFlag)
                      {
                        // 3 = Channel Clearance failed due to rejection bin full 
                        writeFileLog("[ConnectDevice_c() Exit] Channel Clearance failed due to rejection bin full ");
                        return 3;  
                      }
                      // 0 = Device Connected Successfully 
                      writeFileLog("[ConnectDevice_c() Exit] Device Connected Successfully ");
                      return 0;
                       
         }
         //If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             writeFileLog("[ConnectDevice_c() Exit] Nak Reply Received");
             //now close port
             ClosePort(g_ComHandle);
             g_ComHandle =-1;
             //7 = Other Error 
             return 7;  
         }
         //Malay:If Return Data is 04h then No need to read Data 
         else if (0x04 == rcvPkt[0])
         {
             writeFileLog("[ConnectDevice_c() Exit] EOT Reply Received");
             //now close port
             ClosePort(g_ComHandle);
             g_ComHandle =-1;
             //7 = Other Error 
             return 7;  
         }
         

}//ConnectDevice_c(int PortId, int ChannelClearanceMode, int Timeout) end


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int CheckCardPresent()
{


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

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
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
                    writeFileLog("[CheckCardPresent()] Failed to Send Report status Command ");
                    //1 = Communication Failure 
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
                 return 1;
        }
        else
        {

             memset(log,'\0',LOG_ARRAY_SIZE);

             sprintf(log,"[CheckCardPresent()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
             writeFileLog(log);


        }

        //If Return Data is 06h then Going to Read 25byte Data 
        if(0x06 == rcvPkt[0])
        {
                      writeFileLog("[CheckCardPresent()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                     
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[CheckCardPresent()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log); 

                      if(0 == rtcode)
		      {
				 //1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[CheckCardPresent() Exit] Communication Failure ");
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

                          sprintf(log,"[CheckCardPresent()] Got 0x4e Error with E1:= 0x%xh and E1:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[CheckCardPresent() Exit] Got 0x4e error");
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[CheckCardPresent() Exit] Insertion/return mouth blocked");
                          }
                          else
                          {
                                writeFileLog("[CheckCardPresent() Exit] Other error");
                                

                          }
                          
                          return 5 ; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[CheckCardPresent()] Enable Card Acceptance Success");

                      }
                      else
                      {

                             writeFileLog("[CheckCardPresent() Exit] Enable Card Acceptance Failed");
                             return 5;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[CheckCardPresent()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

				
                                //Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] No Card in MTK-571");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] One Card in Gate");
                                            
                                           returnMouthFlag = 1; 
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] One Card on RF/IC Card Position");
                                           
                                           channelBlockFlag = 1;
                                  
		                        } 
                                }

                                /*
                                //Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] Enough Card in the Box");
		                        }   
                                } 
                                //Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[CheckCardPresent()] Error Card Bin Full");
                                           
		                        }
                                }
                                */

				
	              }
                      //Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            writeFileLog("[CheckCardPresent()] Failed to send total Acknowledgement Command ");
                            //1 = Communication Failure 
                            return 1;
		      }
                      else if(1 == returnMouthFlag)
                      { 
                      	    //3 = One Card infront od Gate
                            return 3; 
                      }
                      else if(1 == channelBlockFlag)
                      {    
                            //2 = One Card on RF/IC Card Position 
                            return 2;
                      }
                      else
                      {
                            //0 = No Card in chanel
                            return 0;
                      }
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             writeFileLog("[CheckCardPresent()] Nak Reply Received");
             //5 = Other Error 
             return 5;  
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x04 == rcvPkt[0])
       {
             writeFileLog("[CheckCardPresent()] EOT Reply Received");
             //5 = Other Error 
             return 5;  
       }



}//int CheckCardPresent() end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int DisableCardAcceptance( char *LogdllPathstr,
                           char *LogFileName,
                           char *deviceid,
                           int   fnLogfileMode
                        )
{




        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

        writeFileLog("[DisableCardAcceptance()] Going to Send Disable Acceptance Command");

        bcc = getBcc(9,g_DisableCardAcceptance);

        g_DisableCardAcceptance[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisableCardAcceptance()] bcc value is 0x%xh",bcc);

        writeFileLog(log);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_DisableCardAcceptance,
                          9
                        );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for(i=0;i<9;i++)
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance()] Disable Acceptance Command[%d] = 0x%xh",i,g_DisableCardAcceptance[i]);

                 writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_DisableCardAcceptance[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[DisableCardAcceptance()] Failed to Send Report status Command ");
                    // 1 = Communication Failure 
                    return 1;


                 }

	}

        //////////////////////////////////////////////////////////////////////////////////////////


        writeFileLog("[DisableCardAcceptance()] Disable Acceptance Command send Successfully");

        writeFileLog("[DisableCardAcceptance()] Now Going to read Acknowledgement");


        // Now going to Check Acknowledgement  
  
        totalByteToRecv = 1;

        memset(rcvPkt,'\0',11); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisableCardAcceptance()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

        writeFileLog(log);

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

                 sprintf(log,"[DisableCardAcceptance()] Acknowledgement against Disable Acceptance Command[0] = 0x%xh.\n",rcvPkt[0]);

                 writeFileLog(log);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {
		 
                 //Soomit-29082016: Error code 0x4E needs to be handled here [Solved]
                 writeFileLog("[DisableCardAcceptance()] Acknowledgement Received");

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',11);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

                 writeFileLog(log);

                 
                 /*

                 for(i=0;i<rcvPktLen;i++)
	         {
		         memset(log,'\0',LOG_ARRAY_SIZE);

                         sprintf(log,"[DisableCardAcceptance()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                         writeFileLog(log);

                 }
                 */

                 /////////////////////////////////////////////////////////////////////////////////////////////////
                 
                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 

				 ClearReceiveBuffer (g_ComHandle);

				 ClearTransmitBuffer(g_ComHandle);

                                 writeFileLog("[DisableCardAcceptance()] Failed to read status command reply bytes.");
				 return 1;
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

                          sprintf(log,"[DisableCardAcceptance()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[DisableCardAcceptance() Exit] Got 0x4e error");
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[DisableCardAcceptance() Exit] Insertion/return mouth blocked.");
                                return 3; //Insertion mouth blocked

                          }
                          else if( (0x31==rcvPkt[7]) && ( 0x30 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[DisableCardAcceptance() Exit] Card Jammed.");
                                return 3; //Insertion mouth blocked

                          }
                          else
                          {
                                writeFileLog("[DisableCardAcceptance() Exit] Other error");

                                return 5; //other error
                                

                          }
                          
                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x33 == rcvPkt[5] ) && ( 0x31 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[DisableCardAcceptance()] Disable Command issue success.");
                             clock_gettime(CLOCK_MONOTONIC, &endts);

                             diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                            
                             // 0 = Operation Successful 
                             return 0;
                             
                      }
                      else
                      {

                             writeFileLog("[DisableCardAcceptance() Exit] Disable Command issue failed.");
                             return 4;//Other Error

                      }

                     

                 /////////////////////////////////////////////////////////////////////////////////////////////////

                 
        }
        else if (0x15 == rcvPkt[0])
        {
             writeFileLog("[DisableCardAcceptance()] Nak Reply Received");
             // 5 = Other Error 
             return 5;  
        }
        else if (0x04 == rcvPkt[0])
        {
             writeFileLog("[DisableCardAcceptance()] EOT Reply Received");
             // 5 = Other Error 
             return 5;  
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




int AcceptCard_c( int   Timeout,
                  char  *LogdllPathstr,
                  char  *LogFileName,
                  char  *deviceid,
                  int   fnLogfileMode
               )
{


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

        writeFileLog("[AcceptCard_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[AcceptCard_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log); 

        //Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[AcceptCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[AcceptCard_c()] Failed to Send Report status Command ");
                    //1 = Communication Failure 
                    return 1;
                 }


	}//for(i=0;i<9;i++) block end

        //////////////////////////////////////////////////////////////////////////////////////////

        writeFileLog("[AcceptCard_c()] Status Command send Successfully");

        writeFileLog("[AcceptCard_c()] Now Going to read Acknowledgement");

        //Now going to Check Acknowledgement   

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d.",rtcode,rcvPktLen);
        
        writeFileLog(log); 

        if(0 == rtcode)
        {
                 //1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[AcceptCard_c()] Communication Failure");
                 return 1;
        }
        else
        {

             memset(log,'\0',LOG_ARRAY_SIZE);

             sprintf(log,"[AcceptCard_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
             writeFileLog(log);


        }

        //If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      writeFileLog("[AcceptCard_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                     
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log); 

                      if(0 == rtcode)
		      {
				 //1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[AcceptCard_c()] Communication Failure");
				 return 1;
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

                          sprintf(log,"[AcceptCard_c()] Got 0x4e Error with E1:= 0x%xh and E1:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[AcceptCard_c() Exit] Got 0x4e error");

                          /*
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[AcceptCard_c() Exit] Insertion/return mouth blocked");
                                return 3 ; //Insertion/return mouth blocked

                          }
                          else
                          {
                                writeFileLog("[AcceptCard_c() Exit] Other error");
                                return 5 ; //Other error

                          }
                          
                          */

                           writeFileLog("[AcceptCard_c() Exit] Other error found when issue device status command.");
                           return 5 ; //Other error



                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end

                            

                      //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[AcceptCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

				//Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] No Card in MTK-571");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] One Card in Gate");
                                           //3 = Insert/return mouth block 
                                           returnMouthFlag = 1; 
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] One Card on RF/IC Card Position");
                                           //2 = Channel blocked 
                                           channelBlockFlag = 1;
                                  
		                        } 
                                }
                                //Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] Enough Card in the Box");
		                        }   
                                } 
                                //Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[AcceptCard_c()] Error Card Bin Full");
                                           
		                        }
                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++)

                    
                      //////////////////////////////////////////////////////////////////////////////////////////////////////////

                      //Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            writeFileLog("[AcceptCard_c()] Failed to send total Acknowledgement Command ");
                            //1 = Communication Failure 
                            return 1;
		      }
                      if(1 == returnMouthFlag)
                      { 
                      	    //3 = Insert/return mouth block
                            writeFileLog("[AcceptCard_c()] Insert/return mouth block"); 
                            return 3; 
                      }
                      if(1 == channelBlockFlag)
                      {    
                            //2 = Channel blocked 
                            writeFileLog("[AcceptCard_c()] Channel blocked"); 
                            return 2;
                      }
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             writeFileLog("[AcceptCard_c()] Nak Reply Received");
             //5 = Other Error 
             return 5;  
       }
       // If Return Data is 0x04h then No need to read Data 
       else if (0x04 == rcvPkt[0])
       {
             writeFileLog("[AcceptCard_c()] EOT Reply Received");
             //5 = Other Error 
             return 5;  
       }
      

       ///////////////////////////////////////////////////////////////////////////////////////////////////
 
       writeFileLog("[AcceptCard_c()] All Status OK Now Going to Accept ");
       writeFileLog("[AcceptCard_c()] Going to send Accept Card Command ");
       bcc = getBcc(9,g_EnableCardAcceptance);
       g_EnableCardAcceptance[8] = bcc;

       memset(log,'\0',LOG_ARRAY_SIZE);
       sprintf(log,"[AcceptCard_c()] bcc value is 0x%xh",bcc);
       writeFileLog(log);

       // Before Send Command clear all serial buffer 
       ClearReceiveBuffer (g_ComHandle);
       ClearTransmitBuffer(g_ComHandle);

       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_EnableCardAcceptance,
                          9
                        );

       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       for(i=0;i<9;i++)
       {
		
                 //memset(log,'\0',LOG_ARRAY_SIZE);
                 //sprintf(log,"[AcceptCard_c()] Accept Card Command[%d] = 0x%xh",i,g_EnableCardAcceptance[i]);
                 //writeFileLog(log);
		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_EnableCardAcceptance[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[AcceptCard_c()] Failed to Send Accept Card Command ");
                    return 1;
                 }

	}

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        // Now going to Check Acknowledgement 

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);

        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle); 
                 writeFileLog("[AcceptCard_c()] Communication Failure");
                 return 1;
        }
        else
        {
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[AcceptCard_c()] Acknowledgement against Accept Card Command[0] = 0x%xh.\n",rcvPkt[0]);

                writeFileLog(log);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {

                 //Enable Card Entry: 
                 //Positive Reply: “P” 33H Pm st0 st1 st2
                 //Negative Reply: “N” 33H Pm e1 e0
                 writeFileLog("[AcceptCard_c()] Acknowledgement Received");

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',24);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                 memset(log,'\0',LOG_ARRAY_SIZE);
                 
                 sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
            
                 writeFileLog(log);


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

                          sprintf(log,"[AcceptCard_c()] Got 0x4e Error with E1:= 0x%xh and E1:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[AcceptCard_c() Exit] Got 0x4e error");

                          /*
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[AcceptCard_c() Exit] Insertion/return mouth blocked");
                                return 3 ; //Insertion/return mouth blocked

                          }
                          else
                          {
                                writeFileLog("[AcceptCard_c() Exit] Other error");

                                return 5 ; //Other error

                          }
                          
                          */

                           writeFileLog("[AcceptCard_c() Exit] Other error found when issue accept card command.");

                           return 5 ; //Other error


                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x33 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[AcceptCard_c()] Enable Card Acceptance Success");

                      }
                      else
                      {

                             writeFileLog("[AcceptCard_c() Exit] Enable Card Acceptance Failed");
                             return 5;//Other Error

                      }

                  //////////////////////////////////////////////////////////////////////////////////////////////////
    

  
                 /*
                 for(i=0;i<rcvPktLen;i++)
	         {
			
                      memset(log,'\0',LOG_ARRAY_SIZE);
                 
                      sprintf(log,"[AcceptCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
            
                      writeFileLog(log);

                 }
                 */
                  
                 
        }
        else if (0x15 == rcvPkt[0])
        {
             writeFileLog("[AcceptCard_c()] Nak Reply Received");
             // 5 = Other Error 
             return 5;  
        }
        else if (0x04 == rcvPkt[0])
        {
             writeFileLog("[AcceptCard_c()] EOT Reply Received");
             // 5 = Other Error 
             return 5;  
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        // Now its Time to wait for Some times to accept Card 
        // Time in Miliseconds 
        // Wait for 1 M 
        // Have to Start a thread for that and check for the card 
        // writeFileLog("[AcceptCard_c()] Waitng for accepting a Card");
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
                             (5==presentcard)||
                             (2==presentcard)) // communication failure+
                                               // NAK receieved+one card in reader position
                         {
                              //break loop 
                              writeFileLog("[AcceptCard_c()] Card Found");
                              return presentcard;

                         }
                         else if(3==presentcard) //One Card in infront of Gate
                         {
                              //break loop
                              writeFileLog("[AcceptCard_c()] Found One Card in infront of Gate.");
                              return 0;

                         }

                         //Check Current timer status
                         clock_gettime(CLOCK_MONOTONIC, &endts);

                         diffts.tv_sec = endts.tv_sec - begints.tv_sec;

                         //Time out and No data received. 
			 if( diffts.tv_sec >= (Timeout/1000)  )
                         {  
    
                                 memset(log,'\0',LOG_ARRAY_SIZE);

                                 sprintf(log,"[AcceptCard_c()] Time out in %d seconds.",diffts.tv_sec);

                                 writeFileLog(log);

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

        writeFileLog("[AcceptCard_c()] Going to Send Disable Acceptance Command");
        bcc = getBcc(9,g_DisableCardAcceptance);
        g_DisableCardAcceptance[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[AcceptCard_c()] bcc value is 0x%xh",bcc);
        writeFileLog(log);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);
                 sprintf(log,"[AcceptCard_c()] Disable Acceptance Command[%d] = 0x%xh",i,g_DisableCardAcceptance[i]);
                 writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_DisableCardAcceptance[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[AcceptCard_c()] Failed to Send Report status Command ");
                    // 1 = Communication Failure 
                    return 1;
                 }
	}

        writeFileLog("[AcceptCard_c()] Disable Acceptance Command send Successfully");
        writeFileLog("[AcceptCard_c()] Now Going to read Acknowledgement");


        // Now going to Check Acknowledgement  
  
        totalByteToRecv = 1;

        memset(rcvPkt,'\0',11); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        writeFileLog(log);

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
                 writeFileLog(log);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {
                 writeFileLog("[AcceptCard_c()] Acknowledgement Received");
                 totalByteToRecv = 12;
                 memset(rcvPkt,'\0',11);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);
                 sprintf(log,"[AcceptCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
                 writeFileLog(log);

                 for(i=0;i<rcvPktLen;i++)
	         {
		         memset(log,'\0',LOG_ARRAY_SIZE);

                         sprintf(log,"[AcceptCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
                         writeFileLog(log);

                 }
                 // 0 = Operation Successful 
                 return 0;
        }
        else if (0x15 == rcvPkt[0])
        {
             writeFileLog("[AcceptCard_c()] Nak Reply Received");
             // 5 = Other Error 
             return 5;  
        }

        */

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}//AcceptCard_c(int Timeout) end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//get rfid reader status
int GetRFIDReaderStatus()
{
       
        writeFileLog("[GetRFIDReaderStatus()] Entry");

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

                    writeFileLog("[GetRFIDReaderStatus()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[GetRFIDReaderStatus()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[GetRFIDReaderStatus()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x60 == RecvPackt[5] ) &&  //CM
		                    ( 0x32 == RecvPackt[6] )     //PM
		                  )
		                {

				             writeFileLog("[GetRFIDReaderStatus() Exit] Successfully get deactivate status from mutek.");                           
				             
		                             if( (0x30 == RecvPackt[10]) && (0x30 == RecvPackt[11]) )
				             {
						  return 1; //not ready
		                               
					     }
					     else if( (0x00 != RecvPackt[10]) && (0x00 != RecvPackt[11]) )
					     {
                                                  return 0; //ready state
                                             }
					     
                              }
                              else if( 0x4e == RecvPackt[4] )
		              {
		                     
                                     writeFileLog("[GetRFIDReaderStatus() Exit] Negative ACK receieved.");                    
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetRFIDReaderStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);      
       
       
                                     return (-1);
		                 
		              }//else if( 0x4e == RecvPackt[4] )
                              else
                              {
                                  writeFileLog("[GetRFIDReaderStatus() Exit] Reply bytes not matched .");
                                  return (-1);

                              }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     writeFileLog("[GetRFIDReaderStatus() Exit] Negative ACK receieved.");                       
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetRFIDReaderStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);      
    
                                     return (-1);
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                writeFileLog("[GetRFIDReaderStatus() Exit] Timeout occuered."); 
                          
		                return (-1);

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          writeFileLog("[GetRFIDReaderStatus()] Fail receieve ack byte from mutek.");
                          return (-1);
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             writeFileLog("[GetRFIDReaderStatus()] Fail transmit command to mutek.");
             return (-1);
        }




}//GetRFIDReaderStatus() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Get Sam reader status
int GetSAMeaderStatus()
{
       
        writeFileLog("[GetSAMeaderStatus()] Entry");

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

                    writeFileLog("[GetSAMeaderStatus()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[GetSAMeaderStatus()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[GetSAMeaderStatus()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x32 == RecvPackt[6] )     //PM
		                  )
		                {

				             writeFileLog("[GetSAMeaderStatus() Exit] Successfully get deactivate status from mutek.");                           
				             
		                             if( 0x30 == RecvPackt[10] )
				             {
						  return 1; //not ready
		                               
					     }
					     else if( (0x32 == RecvPackt[10] ) || 
                                                      (0x31 == RecvPackt[10] ) )
					     {

						  return 0; //ready state

					     }
					     
                              }
                              else if( 0x4e == RecvPackt[4] )
		              {
		                     
                                     writeFileLog("[GetSAMeaderStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetSAMeaderStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);      
       
                                     return (-1);
		                 
		              }//if( 0x4e == RecvPackt[4] )  
                              else
                              {
                                  writeFileLog("[GetSAMeaderStatus() Exit] Reply bytes not matched .");
                                  return (-1);

                              }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     writeFileLog("[GetSAMeaderStatus() Exit] Negative ACK receieved.");                    
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetSAMeaderStatus()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);      
       
       
                                     return (-1);
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                writeFileLog("[GetSAMeaderStatus() Exit] Timeout occuered."); 
                          
		                return (-1);

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          writeFileLog("[GetSAMeaderStatus()] Fail receieve ack byte from mutek.");
                          return (-1);
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             writeFileLog("[GetSAMeaderStatus()] Fail transmit command to mutek.");
             return (-1);
        }




}//GetSAMeaderStatus() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Get Stacker/Rejection/Collection Status and card count
int GetMTKeaderStatus(char *MTKStatus)
{
       
        writeFileLog("[GetMTKeaderStatus()] Entry");

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

                    writeFileLog("[GetMTKeaderStatus()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[GetMTKeaderStatus()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[GetMTKeaderStatus()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x31 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
		                  )
		               {

				     writeFileLog("[GetMTKeaderStatus() Exit] Successfully get deactivate status from mutek.");                           

                                     //////////////////////////////////////////////////////////////////////

                                     //stacker card status
                                     if( 0x30 == RecvPackt[8] ) // no card in mtk 571
                                     {
                                         MTKStatus[0] = 0;//empty
                                     }  
                                     else if( 0x31 == RecvPackt[8] )//one card in chanel
                                     {
                                         MTKStatus[0] = 1;//nearly empty
                                     }
                                     else if( 0x32 == RecvPackt[8] ) //one card in reader position
                                     {
                                         MTKStatus[0] = 3;//full

                                     }

                                     MTKStatus[1] = 0;//card number in stacker

                                     ///////////////////////////////////////////////////////////////////////

                                     //Rejection bin
                                     if( 0x30 == RecvPackt[9] ) // no card in mtk 571
                                     {
                                         MTKStatus[2] = 0;//empty
                                     }  
                                     else if( 0x31 == RecvPackt[9] )//one card in chanel
                                     {
                                         MTKStatus[2] = 3;//full
                                     }
                                     
				     MTKStatus[3] = 0;//card number in rejection bin

                                     ////////////////////////////////////////////////////////////////////////
                                     //Collection bin status
                                     MTKStatus[4] = 0;//empty

                                     MTKStatus[5] = 0;//card count in collection bin

                                     //////////////////////////////////////////////////////////////////////////
             
		                     return true; 
					     
                              }
                              else if( 0x4e == RecvPackt[4] )
		              {
		                     
                                     writeFileLog("[GetMTKeaderStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKeaderStatus()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);      
       
                                     return false;
		                 
		              }//if( 0x4e == RecvPackt[4] )  
                              else
                              {
                                  writeFileLog("[GetMTKeaderStatus() Exit] Reply bytes not matched .");
                                  return false;

                              }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     writeFileLog("[GetMTKeaderStatus() Exit] Negative reply receieved.");                           
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKeaderStatus()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);    

                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                writeFileLog("[GetMTKeaderStatus() Exit] Timeout occuered."); 
                          
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          writeFileLog("[GetMTKeaderStatus()] Fail receieve ack byte from mutek.");
                          return false;
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             writeFileLog("[GetMTKeaderStatus()] Fail transmit command to mutek.");
             return false;
        }


}//GetMTKeaderStatus() end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Get Stacker/Rejection/Collection Status and card count
int GetMTKSensorStatus(char *ChanelStatus,char *MTKSensorStatus)
{

       
        writeFileLog("[GetMTKSensorStatus()] Entry");

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

                    writeFileLog("[GetMTKSensorStatus()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[GetMTKSensorStatus()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[GetMTKSensorStatus()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x31 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		               {

				     writeFileLog("[GetMTKSensorStatus() Exit] Successfully get deactivate status from mutek.");                           

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
		                     
                                     writeFileLog("[GetMTKSensorStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKSensorStatus() Exit] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);      
       
                                     return false;
		                 
		              }//if( 0x4e == RecvPackt[4] )     
                              else
                              {
                                  writeFileLog("[GetMTKSensorStatus() Exit] Reply bytes not matched .");
                                  return false;

                              }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {

		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     
                                     writeFileLog("[GetMTKSensorStatus() Exit] Negative reply receieved.");               
                                     //Log e1 and e2 error codes
				     memset(log,'\0',LOG_ARRAY_SIZE);

				     sprintf(log,"[GetMTKSensorStatus()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",RecvPackt[7],RecvPackt[8]);

				     writeFileLog(log);      
       
                                     return false;

		                 
		                 }// if( 0x4e == RecvPackt[4] ) block

		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                writeFileLog("[GetMTKSensorStatus() Exit] Timeout occuered."); 
                          
		                return false;

		            }//else block

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          writeFileLog("[GetMTKSensorStatus()] Fail receieve ack byte from mutek.");
                          return false;

		    }//else block

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
             //Fail Transmit command to MUTEK
             writeFileLog("[GetMTKSensorStatus()] Fail transmit command to mutek.");

             return false;

        }//else block


}//GetMTKSensorStatus() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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

int DeviceStatus_C(  int    ComponentId , 
                     int    Timeout,
                     char  *ReplyDeviceStatus,
                     char  *LogdllPathstr,
                     char  *LogFileName,
                     char  *deviceid,
                     int    fnLogfileMode
                  )
{


              SmartCardSetupLog( LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

              writeFileLog("[DeviceStatus_C()] Entry");

              /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

              //init array
              for(counter=0;counter<11;counter++)
              {
                     ReplyDeviceStatus[counter] = 0x00; 

              }

              /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

              //Force Card detection
              
              //Card Block Status Found
              rtcode= CardBlockStatusRequest ( LogdllPathstr,LogFileName,deviceid,fnLogfileMode);

              if( (1 ==  rtcode) || ( -1 == rtcode ) || ( 2 == rtcode ))
              {

                       ReplyDeviceStatus[0] = 0; //Force Card detection error

                       GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);

                       //Byte 7: chanel status
                       if(1 ==  rtcode ) //Force Card Entry
                       {
                              ReplyDeviceStatus[7] = 0x02; //Block Card
                       }
                       else if(2 ==  rtcode ) //Jammed Card
                       {
                              ReplyDeviceStatus[7] = 0x03; //Jammed Card
                       }

                       //Byte 8: chanel sensor status
		       ReplyDeviceStatus[8] = MTKSensorStatus;

                       memset(log,'\0',LOG_ARRAY_SIZE);

                       sprintf(log,"[DeviceStatus_C()] ChanelStatus = 0x%xh. Sensor Status=0x%xh.",ChanelStatus,MTKSensorStatus);

                       writeFileLog(log);

                       strcpy(SingleLineLog,"[DeviceStatus_C()] ReplyByte:");

		       for(counter=0;counter<11;counter++)
		       {

		              sprintf(CommandHexLog,"%x ",ReplyDeviceStatus[counter]);

		              strcat(SingleLineLog,CommandHexLog);

		                     
		       }//for loop

		       writeFileLog(SingleLineLog);

                       writeFileLog("[DeviceStatus_C()] Exit.");

                       return 1; 



              }//if( (1 ==  rtcode) || ( -1 == rtcode ) ) end
              

              /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      //Byte 0: Execution status of api
	      ReplyDeviceStatus[0] = 0; //operation successfull

	      //Byte 1: RFID Reader status
              if( (0 == ComponentId) || (1 == ComponentId) )
              {
	            rtcode =-1;
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

              ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      //Byte 2: SAM Reader status
              if( (0 == ComponentId) || (1 == ComponentId) )
              {
	            rtcode =-1;
                    rtcode = GetSAMeaderStatus();
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
                   GetMTKeaderStatus(MTKStatus);
              }


              //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      //Byte 3: Stacker status Byte 4: Stacker card count
              if( (0 == ComponentId) || (2 == ComponentId) ) //2 Stacker
              {
	          ReplyDeviceStatus[3] =MTKStatus[0];

                  ReplyDeviceStatus[4] =MTKStatus[1];
              }
              else
              {
	          ReplyDeviceStatus[3] = 0 ;

                  ReplyDeviceStatus[4] = 0 ;
              }

              ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      //Byte 5: Rejection bin status Byte 6: Card count in rejection bin
              if( (0 == ComponentId) || (3 == ComponentId) ) // 3 Rejection Bin
	      {
	          ReplyDeviceStatus[5] =MTKStatus[2];

                  ReplyDeviceStatus[6] =MTKStatus[3];
              }
              else
              {
                  ReplyDeviceStatus[5] = 0 ;

                  ReplyDeviceStatus[6] = 0 ;
              }

              ////////////////////////////////////////////////////////////////////////////////////////////////////
             
              if(0 == ComponentId)
              {

		      GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);

		      memset(log,'\0',LOG_ARRAY_SIZE);

		      sprintf(log,"[DeviceStatus_C()] ChanelStatus = 0x%xh. Sensor Status=0x%xh.",ChanelStatus,MTKSensorStatus);
		
		      writeFileLog(log);

		      //Byte 7: chanel status
		      ReplyDeviceStatus[7] = ChanelStatus;

		      //Byte 8: chanel sensor status
		      ReplyDeviceStatus[8] = MTKSensorStatus;
               }
               else
               {
                      //Byte 7: chanel status
		      ReplyDeviceStatus[7] = 0x00;

		      //Byte 8: chanel sensor status
		      ReplyDeviceStatus[8] = 0x00;

               }

              //////////////////////////////////////////////////////////////////////////////////////////////////

	      //Byte 9:   collection bin status  Byte 10 : card count in collection bin
              if( (0 == ComponentId) || (5 == ComponentId) ) // 5 Collection Bin
              {
	           ReplyDeviceStatus[9] =MTKStatus[4];

                   ReplyDeviceStatus[10] =MTKStatus[5];

	      }
              else
              {
                   ReplyDeviceStatus[9]  = 0;

                   ReplyDeviceStatus[10] = 0;
              }

              /////////////////////////////////////////////////////////////////////////////////////////////////
	      
              strcpy(SingleLineLog,"[DeviceStatus_C()] ReplyByte:");

              for(counter=0;counter<11;counter++)
	      {

                            sprintf(CommandHexLog,"%x ",ReplyDeviceStatus[counter]);

                            strcat(SingleLineLog,CommandHexLog);

                             
              }//for loop

              writeFileLog(SingleLineLog);

              //////////////////////////////////////////////////////////////////////////////////////////////

              writeFileLog("[DeviceStatus_C()] Exit");
              
              return 1;

             ///////////////////////////////////////////////////////////////////////////////////////////////////


}//int DeviceStatus_C(int ComponentId , int Timeout,char *ReplyDeviceStatus)



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

int DisableCardAcceptance_c( int   Timeout, 
                             char  *LogdllPathstr,
                             char  *LogFileName,
                             char  *deviceid,
                             int   fnLogfileMode
                          )
{


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

        writeFileLog("[DisableCardAcceptance_c()] Going to Send Disable Acceptance Command");

        bcc = getBcc(9,g_DisableCardAcceptance);

        g_DisableCardAcceptance[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisableCardAcceptance_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);


        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
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
        
                 writeFileLog(log);
                 */
		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_DisableCardAcceptance[i]);
                 
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[DisableCardAcceptance_c()] Failed to Send Report status Command .");
                    // 1 = Communication Failure 
                    return 1;
                 }//if(0 == send_rvalue) 

	}//for(i=0;i<9;i++)

        ////////////////////////////////////////////////////////////////////////////////////////////////

        writeFileLog("[DisableCardAcceptance_c()] Disable Acceptance Command send Successfully.");

        writeFileLog("[DisableCardAcceptance_c()] Now Going to read Acknowledgement.");


        // Now going to Check Acknowledgement
  
        totalByteToRecv = 1;

        memset(rcvPkt,'\0',11); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

       
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);

                 ClearTransmitBuffer(g_ComHandle);

                 writeFileLog("[DisableCardAcceptance_c()] Communication Failure when read ack bytes.");
                 return 1;

        }
        else
        {
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance_c()] Acknowledgement against Disable Acceptance Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                 writeFileLog(log);
        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {

                 writeFileLog("[DisableCardAcceptance_c()] Acknowledgement Received");

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',11);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DisableCardAcceptance_c()] Receive packet status = %d and Length = %d.",rtcode,rcvPktLen);
        
                 writeFileLog(log);

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

                          sprintf(log,"[DisableCardAcceptance_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[DisableCardAcceptance_c() Exit] Got 0x4e error");

                          /*
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[DisableCardAcceptance_c() Exit] Insertion/return mouth blocked.");
                          }
                          else
                          {
                                writeFileLog("[DisableCardAcceptance_c() Exit] Other error");
                                

                          }

                          */
                          
                          writeFileLog("[DisableCardAcceptance_c() Exit] Other error found when issue device status command.");
                          return 3 ; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x33 == rcvPkt[5] ) && ( 0x31 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[DisableCardAcceptance_c()] Disable Card Acceptance Success.");
                             
                             // 0 = Operation Successful 
                             return 0;

                      }
                      else
                      {

                             writeFileLog("[DisableCardAcceptance_c() Exit] Disable Card Acceptance Failed.");
                             return 3;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
	    
		         /*
		         for(i=0;i<rcvPktLen;i++)
			 {
				
		               memset(log,'\0',LOG_ARRAY_SIZE);

		               sprintf(log,"[DisableCardAcceptance_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
		
		               writeFileLog(log);

		         }
		         */
                
        }
        else if (0x15 == rcvPkt[0])
        {
             writeFileLog("[DisableCardAcceptance_c()] Nak Reply Received");
             // 3 = Other Error 
             return 3;  
        }
        else if (0x04 == rcvPkt[0])
        {
             writeFileLog("[DisableCardAcceptance_c()] EOT Received");
             // 3 = Other Error 
             return 3;  
        }

}//int DisableCardAcceptance_c(int Timeout) end



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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




int DisConnectDevice_c( int   Timeout,
                        char  *LogdllPathstr,
                        char  *LogFileName,
                        char  *deviceid,
                        int   fnLogfileMode
                     )
{

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        writeFileLog("[DisConnectDevice_c()] Entry");

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        
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

        writeFileLog("[DisConnectDevice_c()] Going to send Current Report status of st0, st1, st2");

        bcc = getBcc(9,g_getStatus);

        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisConnectDevice_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[DisConnectDevice_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[DisConnectDevice_c()] Failed to Send Report status Command ");
                    // 2 = Communication Failure 
                    return 2;
                 
                 }// if(0 == send_rvalue) 


	}//for(i=0;i<9;i++)


        ///////////////////////////////////////////////////////////////////////////////////////////

        writeFileLog("[DisConnectDevice_c()] Status Command send Successfully");

        writeFileLog("[DisConnectDevice_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

       
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DisConnectDevice_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 2 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[DisConnectDevice_c()] Communication Failure when read ack bytes.");
                 return 2;
        }
        else
        {
                 
              memset(log,'\0',LOG_ARRAY_SIZE);

              sprintf(log,"[DisConnectDevice_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
              writeFileLog(log);

        }

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      writeFileLog("[DisConnectDevice_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[DisConnectDevice_c()] statusRead() return code = %d and receieve packet Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      if(0 == rtcode)
		      {
				 // 2 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[DisConnectDevice_c()] Communication Failure when read getstatus reply bytes.");
				 return 2;

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

                          sprintf(log,"[DisConnectDevice_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[DisConnectDevice_c() Exit] Got 0x4e error");
                          
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[DisConnectDevice_c() Exit] Insertion/return mouth blocked");
                          }
                          else
                          {
                                writeFileLog("[DisConnectDevice_c() Exit] Other error");
                                

                          }
                          
                          return 4 ; //Other error

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {
  
                             // Success: Rx[12]:f2 0 0 6 50 31 30 32 31 30 3 95
                             writeFileLog("[DisConnectDevice_c()] Get Status Command issue success.");
                             
                      }
                      else
                      {

                             writeFileLog("[DisConnectDevice_c() Exit] Get Status Command issue failed.");
                             return 4;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[DisConnectDevice_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] No Card in MTK-571");
                                             
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] One Card in Gate");
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] One Card on RF/IC Card Position");
                                           // 1 = Card found in the Channel 
                                           oneCradInChannel = 1;  
                                  
		                        } 


                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] Enough Card in the Box");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[DisConnectDevice_c()] Error Card Bin Full");
                                           
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
		            writeFileLog("[DisConnectDevice_c()] Failed to send total Acknowledgement Command ");
                            // 2 = Communication Failure 
                            return 2;
		      }
                       
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             writeFileLog("[DisConnectDevice_c() Exit] Nak Reply Received");
             // 4 = Other Error 
             return 4;  
       }
       else if (0x04 == rcvPkt[0])
       {
             writeFileLog("[DisConnectDevice_c() Exit] EOT Reply Received");
             // 4 = Other Error 
             return 4;  
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
                      writeFileLog("[DisConnectDevice_c() Exit] DisConnected Successfully but a Card in the Channel.");
                      return 1;
               }
               else if(0 == oneCradInChannel)
               {
                      // No Card in Channel 
                      // 0 = DisConnected Successfully 
                      writeFileLog("[DisConnectDevice_c() Exit] DisConnected Successfully and no Card in the Channel.");
                      return 0;
               }
       }
       else if(0 == closePortStatus)
       {
           // Port Close UnSuccessfull 
           // 2 = Communication Failure 
           writeFileLog("[DisConnectDevice_c() Exit] DisConnected failed with Communication Failure .");
           return 2;

       }
       else 
       {
           // 4 = Other Error 
           writeFileLog("[DisConnectDevice_c() Exit] DisConnected failed with other error .");
           return 4;
       }
 

}//int DisConnectDevice_c(int Timeout) end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Function Name : EnableCardAcceptance_c 
// Return Type   : int 
//                  0 = Operation Successful
//                  1 = Communication Failure
//                  2 = Channel blocked 
//                  3 = Insert/return mouth block
//                  4 = Operation timeout occurred
//                  5 = Other Error 

// Parameters    : Name                           Type          Description 

                            
//                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 


// To get Current Report status of st0, st1, st2


//static unsigned char g_getStatus[9]={0xF2,0x00,0x00,0x03,0x43,0x31,0x30,0x03,0x00};


// To Enable Card Acceptance 

//static unsigned char g_EnableCardAcceptance[9]={0xF2,0x00,0x00,0x03,0x43,0x33,0x30,0x03,0x00};




int EnableCardAcceptance_c( int   Timeout, 
                            char *LogdllPathstr,
                            char *LogFileName,
                            char *deviceid,
                            int   fnLogfileMode
                          )
{


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);


        ///////////////////////////////////////////////////////////////////////////////////

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

        writeFileLog("[EnableCardAcceptance_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);

        g_getStatus[8] = bcc;
       

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[EnableCardAcceptance_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
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

                 //sprintf(log,"[EnableCardAcceptance_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[EnableCardAcceptance_c()] Failed to Send Report status Command .");
                    return 1;

                 }//if(0 == send_rvalue) 


	}//for(i=0;i<9;i++)

        ////////////////////////////////////////////////////////////////////////////////////////

        writeFileLog("[EnableCardAcceptance_c()] Status Command send Successfully");

        writeFileLog("[EnableCardAcceptance_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement  
 
        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer(g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[EnableCardAcceptance_c()] Failed to read ack reply byte ");
                 return 1;
        }
        else
        {
                
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[EnableCardAcceptance_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                writeFileLog(log);

        }

        //If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {
                      writeFileLog("[EnableCardAcceptance_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[EnableCardAcceptance_c()] Failed to read status command reply bytes.");
				 return 1;
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

                          sprintf(log,"[EnableCardAcceptance_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[EnableCardAcceptance_c() Exit] Got 0x4e error");
                          
                          /*
                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[EnableCardAcceptance_c() Exit] Insertion/return mouth blocked");
                                return 3; //Insertion mouth blocked

                          }
                          else
                          {
                                writeFileLog("[EnableCardAcceptance_c() Exit] Other error");

                                return 5; //other error
                                

                          }
                          */

                          writeFileLog("[EnableCardAcceptance_c() Exit] Other error found when issue device status command.");

                          return 5; //other error
                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[EnableCardAcceptance_c()] Get Status Command issue success.");
                             
                      }
                      else
                      {

                             writeFileLog("[EnableCardAcceptance_c() Exit] Get Status Command issue failed.");
                             return 4;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
                     
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[EnableCardAcceptance_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);


                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] No Card in MTK-571");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] One Card in Gate");
                                           // 3 = Insert/return mouth block 
                                           returnMouthFlag = 1; 
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] One Card on RF/IC Card Position");
                                           // 2 = Channel blocked 
                                           channelBlockFlag = 1;
                                  
		                        } 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] Enough Card in the Box");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[EnableCardAcceptance_c()] Error Card Bin Full");
                                           
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
		            writeFileLog("[EnableCardAcceptance_c()] Failed to send total Acknowledgement Command ");
                            // 1 = Communication Failure 
                            return 1;
		      }
                      if(1 == returnMouthFlag)
                      { 
                      	    // 3 = Insert/return mouth block 
                            writeFileLog("[EnableCardAcceptance_c()] Insert/return mouth block ");
                            return 3; 
                      }
                      if(1 == channelBlockFlag)
                      {    
                            //  2 = Channel blocked 
                            writeFileLog("[EnableCardAcceptance_c()] Channel blocked  ");
                            return 2;
                      }
       }
       //If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             writeFileLog("[EnableCardAcceptance_c()] Nak Reply Received");
             // 5 = Other Error 
             return 5;  
       }
       //If Return Data is 15h then No need to read Data 
       else if (0x04 == rcvPkt[0])
       {
             writeFileLog("[EnableCardAcceptance_c()] EOT Reply Received");
             // 5 = Other Error 
             return 5;  
       }

       ///////////////////////////////////////////////////////////////////////////////////////////////

 
       writeFileLog("[EnableCardAcceptance_c()] All Status OK Now Going to Accept ");

       writeFileLog("[EnableCardAcceptance_c()] Going to send Accept Card Command ");

       bcc = getBcc(9,g_EnableCardAcceptance);

       g_EnableCardAcceptance[8] = bcc;
      
       memset(log,'\0',LOG_ARRAY_SIZE);

       sprintf(log,"[EnableCardAcceptance_c()] bcc value is 0x%xh",bcc);
        
       writeFileLog(log);

       // Before Send Command clear all serial buffer 
       ClearReceiveBuffer (g_ComHandle);

       ClearTransmitBuffer(g_ComHandle);

       for(i=0;i<9;i++)
       {
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[EnableCardAcceptance_c()] Accept Card Command[%d] = 0x%xh",i,g_EnableCardAcceptance[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_EnableCardAcceptance[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[EnableCardAcceptance_c()] Failed to Send Accept Card Command ");
                    // 1 = Communication Failure 
                    return 1;
                 }
	}

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // Now going to Check Acknowledgement 
        totalByteToRecv = 1;
        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[EnableCardAcceptance_c()] Unable to read enable command reply bytes");
                 return 1;
        }
        else
        {
                 
		memset(log,'\0',LOG_ARRAY_SIZE);

		sprintf(log,"[EnableCardAcceptance_c()] Acknowledgement against Accept Card Command[0] = 0x%xh.\n",rcvPkt[0]);
		
		writeFileLog(log);

        }

        // If Return Data is 06h then Going to Read 12byte Data 

        if(0x06 == rcvPkt[0])
        {
                 writeFileLog("[EnableCardAcceptance_c()] Acknowledgement Received");
                 totalByteToRecv = 12;
                 memset(rcvPkt,'\0',24);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

		 sprintf(log,"[EnableCardAcceptance_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
		
		 writeFileLog(log);

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 /*

                 for(i=0;i<rcvPktLen;i++)
	         {
			
                        memset(log,'\0',LOG_ARRAY_SIZE);

		        sprintf(log,"[EnableCardAcceptance_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
		
		        writeFileLog(log);

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

                          sprintf(log,"[EnableCardAcceptance_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[EnableCardAcceptance_c() Exit] Got 0x4e error");
                          
                          /*

                          if( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) 
                          {
                                writeFileLog("[EnableCardAcceptance_c() Exit] Insertion/return mouth blocked");
                                return 3; //Insertion mouth blocked
                          }
                          else
                          {
                                writeFileLog("[EnableCardAcceptance_c() Exit] Other error");
                                return 5; //other error
                                

                          }
                          */

                          writeFileLog("[EnableCardAcceptance_c() Exit] Other error found when issue enable card acceptance command.");
                          return 5; //other error
                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                               ( 0x33 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[EnableCardAcceptance_c()] Enable Success");
                             // 0 = Operation Successful 
                             return 0;
                             
                      }
                      else
                      {

                             writeFileLog("[EnableCardAcceptance_c() Exit] Enable command  failed.");
                             return 5;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                
        }
        else if (0x15 == rcvPkt[0])
        {
             writeFileLog("[EnableCardAcceptance_c()] Nak Reply Received");
             // 5 = Other Error 
             return 5;  
        }
        else if (0x04 == rcvPkt[0])
        {
             writeFileLog("[EnableCardAcceptance_c()] EOT Reply Received");
             // 5 = Other Error 
             return 5;  
        }
      
      

}//int EnableCardAcceptance_c(int Timeout) end





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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





int IsCardInChannel_c( int   Timeout,
                       char *LogdllPathstr,
                       char *LogFileName,
                       char *deviceid,
                       int   fnLogfileMode
                     )
{

        //printf("\n[IsCardInChannel_c()] Entry .");

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,oneCradInChannel = 0,noCardInChannel = 0;
        unsigned int rcvPktLen =0x00; 

        //If there was any card block or not 

        writeFileLog("[IsCardInChannel_c()] Going to send Current Report status of st0, st1, st2");
        
        bcc = getBcc(9,g_getStatus);
        
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[IsCardInChannel_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);

        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        ///////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
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
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[IsCardInChannel_c()] Failed to Send Report status Command ");
                    // 2 = Communication Failure 
                    return 2;

                 }
	

        }

        ////////////////////////////////////////////////////////////////////////////////////////////

        writeFileLog("[IsCardInChannel_c()] Status Command send Successfully");

        writeFileLog("[IsCardInChannel_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement   

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[IsCardInChannel_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 2 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[IsCardInChannel_c()] Failed to read ack reply");
                 return 2;
        }
        else
        {
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[IsCardInChannel_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                 writeFileLog(log);


        }

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      writeFileLog("[IsCardInChannel_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[IsCardInChannel_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      
                      if(0 == rtcode)
		      {
				 // 2 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[IsCardInChannel_c()] Failed to read status command reply bytes.");
				 return 2;
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

                          sprintf(log,"[IsCardInChannel_c()] Got 0x4e Error with E1:= 0x%xh and E1:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[IsCardInChannel_c() Exit] Got 0x4e error");
                          
                          if( ( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) ) || 
                              ( (0x31==rcvPkt[7]) && ( 0x30 == rcvPkt[8] ) )
                            )
                          {
                                writeFileLog("[IsCardInChannel_c() Exit] Force Card Entry Found.");
                                return 1; //Card Found in chanel
                          }
                          else
                          {
                                writeFileLog("[IsCardInChannel_c() Exit] Other error");
                                return 4; //other error
                                

                          }
                          
                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[IsCardInChannel_c()] Get Status Command issue success");
                             
                      }
                      else
                      {

                             writeFileLog("[IsCardInChannel_c() Exit] Get Status Command issue failed");
                             return 4;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {
				

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[IsCardInChannel_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {

		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] No Card in MTK-571");
                                           // 0 = No Card in the Channel 
                                           noCardInChannel = 1; 
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] One Card in Gate");
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] One Card on RF/IC Card Position");
                                           // 1 = Card found in the Channel 
                                           oneCradInChannel = 1; 
                                  
		                        } 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] Enough Card in the Box");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardInChannel_c()] Error Card Bin Full");
                                           
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
		            writeFileLog("[IsCardInChannel_c()] Failed to send total Acknowledgement Command ");
                            // 2 = Communication Failure 
                            return 2;
		      }
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
                       
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             writeFileLog("[IsCardInChannel_c()] Nak Reply Received");
             // 4 = Other Error
             return 4;  
       }
       else if (0x04 == rcvPkt[0])
       {
             writeFileLog("[IsCardInChannel_c()] EOT Reply Received");
             // 4 = Other Error
             return 4;  
       }
 

}//int IsCardInChannel_c(int Timeout) end





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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




int IsCardRemoved_c( int   Timeout,
                     char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int   fnLogfileMode)
{


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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
        int i=0,send_rvalue = 0,totalByteToRecv=0,rtcode = -1,oneCradInGate = 0;
        unsigned int rcvPktLen =0x00; 

        // If there was any card block or not 

        writeFileLog("[IsCardRemoved_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[IsCardRemoved_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);

        ///////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////


        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);
        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[IsCardRemoved_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[IsCardRemoved_c()] Failed to Send Report status Command ");
                    // 2 = Communication Failure 
                    return 2;
                 }
	}

        ////////////////////////////////////////////////////////////////////////////////////////

        writeFileLog("[IsCardRemoved_c()] Status Command send Successfully");

        writeFileLog("[IsCardRemoved_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[IsCardRemoved_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 2 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[IsCardRemoved_c()] Failed to ack bytes.");
                 return 2;
        }
        else
        {
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[IsCardRemoved_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                writeFileLog(log);

        }

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {
                      writeFileLog("[IsCardRemoved_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[IsCardRemoved_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      if(0 == rtcode)
		      {
				 // 2 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[IsCardRemoved_c()] Failed to read status command reply bytes.");
				 return 2;
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

                          sprintf(log,"[IsCardRemoved_c()] Got 0x4e Error with E1:= 0x%xh and E1:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                          writeFileLog(log); 

                          writeFileLog("[IsCardInChannel_c() Exit] Got 0x4e error");
                          
                          if( ( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) )  ||
                              ( (0x31==rcvPkt[7]) && ( 0x36 == rcvPkt[8] ) )  
                            )
                          {
                                writeFileLog("[IsCardRemoved_c() Exit] Force Card Entry Found.");
                                return 0; //Not Removed
                          }
                          else
                          {
                                writeFileLog("[IsCardRemoved_c() Exit] Other error");
                                return 4; //other error
                                

                          }
                          
                          

                      }//if( (rcvPktLen> 0) && ( 0x4e == RecvPackt[4] ) ) end
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                             ( 0x31 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] )
                        )
                            
                      {

                             writeFileLog("[IsCardRemoved_c()] Get Status Command issue success");
                             
                      }
                      else
                      {

                             writeFileLog("[IsCardRemoved_c() Exit] Get Status Command issue failed");
                             return 4;//Other Error

                      }

                     //////////////////////////////////////////////////////////////////////////////////////////////////
    
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[IsCardRemoved_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] No Card in MTK-571");
                                            
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] One Card in Gate");
                                           // 0 = Not Removed 
                                           oneCradInGate = 1;
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] One Card on RF/IC Card Position");
                                            
                                  
		                        } 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] Enough Card in the Box");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[IsCardRemoved_c()] Error Card Bin Full");
                                           
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
		            writeFileLog("[IsCardRemoved_c()] Failed to send total Acknowledgement Command ");
                            // 2 = Communication Failure 
                            return 2;
		      }
                      if(1 == oneCradInGate)
                      { 
                      	    // 0 = Not Removed 
                            return 0; 
                      }
                      else if(0 == oneCradInGate) 
                      {
                           // 1 = Removed 
                           return 1;
                      }
                       
       }
       // If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             writeFileLog("[IsCardRemoved_c()] Nak Reply Received");
             // 4 = Other Error 
             return 4;  
       }
       else if (0x04 == rcvPkt[0])
       {
             writeFileLog("[IsCardRemoved_c()] EOT Reply Received");
             // 4 = Other Error 
             return 4;  
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

int CollectCard_c(   int   Timeout,
                     char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int   fnLogfileMode
                 )
{


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

        writeFileLog("[CollectCard_c()] Going to send Current Report status of st0, st1, st2");
        
        bcc = getBcc(9,g_getStatus);
        
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);
        
        sprintf(log,"[CollectCard_c()] bcc value is 0x%xh.",bcc);
        
        writeFileLog(log); 


        ///////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////


        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[CollectCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[CollectCard_c()] Failed to Send Report status Command ");
                    // 1 = Communication Failure 
                    return 1;
                 }


	}

        //////////////////////////////////////////////////////////////////////////////////////////

        writeFileLog("[CollectCard_c()] Status Command send Successfully");

        writeFileLog("[CollectCard_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement 

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[CollectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[CollectCard_c()] Failed to read ack bytes");
                 return 1;
        }
        else
	{
		 
                memset(log,'\0',LOG_ARRAY_SIZE);

                sprintf(log,"[CollectCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                writeFileLog(log);
				
	}

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      writeFileLog("[CollectCard_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                       
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[CollectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);

				 ClearTransmitBuffer(g_ComHandle);

                                 writeFileLog("[CollectCard_c()] Failed to read status command reply bytes");

				 return 1;

		      }      
                      
                      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      //Check Negative Reply bytes
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[CollectCard_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              writeFileLog(log); 

		              //Force Card Detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
                                  ( 0x36 == rcvPkt[8] ) )
			      {
		                       writeFileLog("[CollectCard_c()] Negative response receieved for issue device status command");  
	   
		                        
		                       if( 1 == MutekInitWithCardMoveInFront( LogdllPathstr,
		                       LogFileName,deviceid,fnLogfileMode) )
		                       {
		                                
                                               writeFileLog("[CollectCard_c()] Operation successfull Block card collect successfull.");
		                               return 0; //Operation successfull card return successfull

		                       }
		                       else
		                       {
                                               writeFileLog("[CollectCard_c()] Operation failed to Negative response found.");
		                               return 5; //Other Error

		                       }

					 
		              }

    
                      }// if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) ) block

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[CollectCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[CollectCard_c()] No Card in MTK-571");
                                           // 3 = No Card in the Channel 
                                           noCardInRF_IC = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                        
                                           writeFileLog("[CollectCard_c()] One Card in Gate");
                                                                              
                  			 
                                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[CollectCard_c()] One Card on RF/IC Card Position");
                                            
                                           
		                        } 
                                }
                                //Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[CollectCard_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[CollectCard_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[CollectCard_c()] Enough Card in the Box");
		                        }   
                                } 
                                //Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[CollectCard_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[CollectCard_c()] Error Card Bin Full");
                                           // 2 = Rejection Bin Full 
                                           rejectionBinFull = 1;
		                        }
                                        

                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++)

                      //////////////////////////////////////////////////////////////////////////////////////////

                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            writeFileLog("[CollectCard_c()] Failed to send total Acknowledgement Command ");
                            //1 = Communication Failure 
                            return 1;
		      }
                      if(1 == noCardInRF_IC)
                      {
                           // 3 = No Card in the Channel 
                           writeFileLog("[CollectCard_c()] No Card in chanel. "); 
                           return 3;
                      }
                      if(1 == rejectionBinFull)
                      {
                           // 2 = Rejection Bin Full 
                           writeFileLog("[CollectCard_c()] Rejection Bin Full. ");  
                           return 2;

                      }
                      
                      
                       
         }//
         // If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             writeFileLog("[CollectCard_c()] Nak Reply Received");
             // 5 = Other Error 
             return 5;  
         }
         else if (0x04 == rcvPkt[0])
         {
             writeFileLog("[CollectCard_c()] EOT Reply Received");
             // 5 = Other Error 
             return 5;  
         }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

          
        
        //Now Check sensor status
        if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
        {

            if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
            {
                writeFileLog("[CollectCard_c()] Sensor 1 Status Found Blocked As Card in Mouth Other Error Found:5");
                // 5 = Other Error 
                return 5;  


            }//if(  1 == (MTKSensorStatus & 0x01) ) block end
              
          


        }//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end

        


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // All status ok and Going to issue Reject Card 
        writeFileLog("[CollectCard_c()] Going to send Reject Card Comamnd");

        //Reinit bcc value
        g_moveToErrorBin[8] =0x00;

        bcc = getBcc(9,g_moveToErrorBin);

        g_moveToErrorBin[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[CollectCard_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);

        ///////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
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
        
                 //writeFileLog(log);

                 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_moveToErrorBin[i]);
                 
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[CollectCard_c()] Failed to Send Reject Card Command ");
                    // 1 = Communication Failure 
                    return 1;
                 }

	}

        writeFileLog("[CollectCard_c()] Reject Card Command send Successfully");

        writeFileLog("[CollectCard_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement   

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[CollectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[CollectCard_c()] Failed to read reject command ack bytes.");
                 return 1;
        }
        else  if(0x06 == rcvPkt[0]) //ack byte receieved
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[CollectCard_c()] Acknowledgement against Reject Card Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 writeFileLog(log);

                 totalByteToRecv = 12;

                 rcvPktLen = 0;

                 memset(rcvPkt,'\0',24); 
         
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                 /////////////////////////////////////////////////////////////////////////////////

                 if(0 == rtcode)
		 {
		      // 1 = Communication Failure 
		      ClearReceiveBuffer (g_ComHandle);
		      ClearTransmitBuffer(g_ComHandle);
                      writeFileLog("[CollectCard_c()] Communication Failure when read reject command reply bytes.");
		      return 1;
		 }  
               
                 //Now send ack byte to mtk
                 int send_rvalue = 0;
                 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 if(0 == send_rvalue) 
		 {
		       writeFileLog("[CollectCard_c()] Failed to send Acknowledgement byte for reject command .");
                          
		 }

                 //////////////////////////////////////////////////////////////////////////////////
           
                 //Check Negative Reply bytes
                 if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                 {

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[CollectCard_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                      writeFileLog(log); 

                      writeFileLog("[CollectCard_c()] Collect Failed to do.");

		      return 5;//other error

                 }
                 else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                          ( 0x32 == rcvPkt[5] ) && ( 0x33 == rcvPkt[6] ) )
                 {
                
		         writeFileLog("[CollectCard_c()] Reject Successfully done.");

		         return 0;
                 }
                 else
                 {

                         writeFileLog("[CollectCard_c()] Reject Failed to do.");

		         return 5; //Other Error

                 }
                
				
	}
        // This function does not end here because Rejection and collection bin are not same 
        else  if(0x15 == rcvPkt[0]) //nak byte receieved
	{
              writeFileLog("[CollectCard_c()] Reject Failed to do a nak receieved.");

              return 5; //Other Error

        }
        else  if(0x04 == rcvPkt[0]) //ack byte receieved
	{
              writeFileLog("[CollectCard_c()] Reject Failed to do a eot receieved.");

              return 5; //Other Error

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
static unsigned char g_dispenseCard[9]={0xF2,MTK_DEVICE_ADDRESS,0x00,0x03,0x43,0x32,0x32,0x03,0x00};

int DispenseCard_c(  int  Timeout,
                     char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int  fnLogfileMode)
{


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );
        
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

        writeFileLog("[DispenseCard_c()] Going to send Current Report status of st0, st1, st2");
        bcc = getBcc(9,g_getStatus);
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DispenseCard_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);

        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[DispenseCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[DispenseCard_c()] Failed to Send Report status Command return command:1");
                    return 1;
                 }
	}

        writeFileLog("[DispenseCard_c()] Status Command send Successfully");

        writeFileLog("[DispenseCard_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement 
 
        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);


        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);

                 ClearTransmitBuffer(g_ComHandle);

                 writeFileLog("[DispenseCard_c()] Communication Failure for Check Acknowledgement .");
                 return 1;

        }
        else
        {
                 
               memset(log,'\0',LOG_ARRAY_SIZE);

               sprintf(log,"[DispenseCard_c()] Acknowledgement against Status Command[0] = 0x%xh.\n",rcvPkt[0]);
        
               writeFileLog(log);
 
        }

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      writeFileLog("[DispenseCard_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                     
                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[DispenseCard_c()] Communication Failure for read device status reply command");
				 return 1;
		      }
              
                     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {

                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[DispenseCard_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              writeFileLog(log); 

                              

		              //Force Card Detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
                                  ( 0x36 == rcvPkt[8] ) )
			      {
		                         writeFileLog("[DispenseCard_c()] Negative response receieved for issue device status command");  
	   
		                         if( 1 == MutekInitWithCardMoveInFront(LogdllPathstr,
	                                 LogFileName,deviceid,fnLogfileMode) )
                                         {
                                             writeFileLog("[DispenseCard_c()] Block Card Removed Successfully done ."); 
                                             return 0; //operation success

                                         }
                                         else
                                         {
                                            writeFileLog("[DispenseCard_c()] Unable to do dispense operation."); 
                                            return 6; //other error

                                         }

					 
		              }
                              else if( ( 0x4e == rcvPkt[4] ) && 
                                       ( 0x31 != rcvPkt[7] ) && 
                                       ( 0x36 != rcvPkt[8] ) )
                              {
                                   writeFileLog("[DispenseCard_c()]  Other Error found."); 
                                   return 6; //other error

                              }


                              



                      }//if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      for(i=0;i<rcvPktLen;i++)
	              {
				

                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[DispenseCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                             writeFileLog("[DispenseCard_c()] No Card in MTK-571.");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                             writeFileLog("[DispenseCard_c()] One Card in Gate.");
                                             // 3 = Insert/return mouth block 
                                             returnMouthFlag = 1; 
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                             writeFileLog("[DispenseCard_c()] One Card on RF/IC Card Position.");
                                             // 2 = Channel blocked 
                                             channelBlockFlag = 1;
                                        } 

                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[DispenseCard_c()] No Card in Stacker");
                                           // 4 = Stacker Empty 
                                           stackerEmptyFlag = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[DispenseCard_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[DispenseCard_c()] Enough Card in the Box");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[DispenseCard_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[DispenseCard_c()] Error Card Bin Full");
                                        }

                                }
                      }

                      ////////////////////////////////////////////////////////////////////////////////////////////////////

 
                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            writeFileLog("[DispenseCard_c()] Failed to send total Acknowledgement Command ");
                            // 1 = Communication Failure 
                            return 1;
		      }
                      if(1 == returnMouthFlag)
                      { 
                      	    // 3 = Insert/return mouth block 
                            writeFileLog("[DispenseCard_c()] Insert/return mouth block return code: 3 ");
                            return 3; 
                      }
                      if(1 == channelBlockFlag)
                      {    
                            // 2 = Channel blocked 
                            writeFileLog("[DispenseCard_c()] Channel blocked  return code: 2 ");
                            return 2;
                      }
                      if(1 == stackerEmptyFlag)
                      {
                            // 4 = Stacker Empty 
                            writeFileLog("[DispenseCard_c()] Stacker Empty return code: 4 ");
                            return 4;  
                      }


       }
       //If Return Data is 15h then No need to read Data 
       else if (0x15 == rcvPkt[0])
       {
             writeFileLog("[DispenseCard_c()] Nak Reply Received for device status command return code: 6");
             // 6 = Other Error 
             return 6;  
       }
       else if (0x04 == rcvPkt[0])
       {
             writeFileLog("[DispenseCard_c()] EOT Reply Received for device status command return code: 6");
             // 6 = Other Error 
             return 6;  
       }


       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       writeFileLog("[DispenseCard_c()] All Status OK Now Going to Dispense Card");

       writeFileLog("[DispenseCard_c()] Going to send Dispense Card Command ");

       //reinit bcc value
       g_dispenseCard[8] = 0x00;

       bcc = getBcc(9,g_dispenseCard);

       g_dispenseCard[8] = bcc;
       
       memset(log,'\0',LOG_ARRAY_SIZE);

       sprintf(log,"[DispenseCard_c()] Dispence Command bcc value is 0x%xh.",bcc);
        
       writeFileLog(log);

       //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_dispenseCard,
                          9
                        );

 
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


       // Before Send Command clear all serial buffer 
       ClearReceiveBuffer (g_ComHandle);

       ClearTransmitBuffer(g_ComHandle);

       for(i=0;i<9;i++)
       {
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[DispenseCard_c()] Dispense Card Command[%d] = 0x%xh",i,g_dispenseCard[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_dispenseCard[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[DispenseCard_c()] Failed to Send Dispense Card Command Communication Failure return code: 1");
                    // 1 = Communication Failure 
                    return 1;
                 }
	}// for(i=0;i<9;i++)
        
        // Now going to Check Acknowledgement 
        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);


        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);

                 ClearTransmitBuffer(g_ComHandle);

                 writeFileLog("[DispenseCard_c()] check acknowledgement for dispense command Communication Failure return code:1");
                 
                 return 1;
        }
        else
        {
                
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DispenseCard_c()] Acknowledgement against Dispense Card Command[0] = 0x%xh.\n",rcvPkt[0]);
        
                 writeFileLog(log);


        }

        // If Return Data is 06h then Going to Read 12byte Data 
        if(0x06 == rcvPkt[0])
        {

                 writeFileLog("[DispenseCard_c()] Acknowledgement Received for dispense command");

                 totalByteToRecv = 12;

                 memset(rcvPkt,'\0',24);
                      
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[DispenseCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                 writeFileLog(log);

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 
                 /*
                 for(i=0;i<rcvPktLen;i++)
	         {
				
                        memset(log,'\0',LOG_ARRAY_SIZE);

                        sprintf(log,"[DispenseCard_c()] Receive packet Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                        writeFileLog(log);

                 }
                 */
                 

                 ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
                 if( 0 == rtcode ) //Timeout and no data receieved
                 {
                       
                       writeFileLog("[DispenseCard_c()] dispense Operation timeout for reply data receieved Communication Failure ");
                       
                       return 1;

                 }
                 else if( 1 == rtcode ) //data receieved
                 {

                            if( ( 0x50 == rcvPkt[4] ) &&  //CMH
		                ( 0x32 == rcvPkt[5] ) &&  //CM
		                ( 0x32 == rcvPkt[6] )     //PM
		              )
		            {
                                        if(0x30 == rcvPkt[7])
		                        {
                                             writeFileLog("[DispenseCard_c()] No Card in MTK-571 chanel");   

                                             return 6; //Other erro
		                        } 
                                        else if(0x31 == rcvPkt[7])
		                        {
                                            
                                             writeFileLog("[DispenseCard_c()] Channel blocked  return code: 2 ");

                                             return 3;
                                             
		                        }
                                        else if(0x32 == rcvPkt[7])
		                        {
                                             writeFileLog("[DispenseCard_c()] One Card on RF/IC Card Position Operation successfully done");
                                             return 0;
                                        } 
                                        else if(0x30 == rcvPkt[8])
		                        {
                                             writeFileLog("[DispenseCard_c()] Stacker Empty");

                                             return 4;
                                        } 
                             }  //if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                //( 0x32 == RecvPackt[5] ) &&  //CM
		                //( 0x32 == RecvPackt[6] )     //PM
		                //)  
                            else if( ( 0x4e == rcvPkt[4] ) &&  //CMH
		                     ( 0x32 == rcvPkt[5] ) &&  //CM
		                     ( 0x32 == rcvPkt[6] )     //PM
		                  )
                            {

                                      writeFileLog("[DispenseCard_c()] Other error found when issue dispense card command.");

                                      return 6; //Other Error
    
                            }      


                 }//else if( 1 == rtcode ) //data receieved

              

        }
        else if (0x15 == rcvPkt[0])
        {
             writeFileLog("[DispenseCard_c()] Nak Reply Received for dispense command return code:6");
             //6 = Other Error 
             return 6;  
        }
        else if (0x04 == rcvPkt[0])
        {
             writeFileLog("[DispenseCard_c()] EOT Reply Received for dispense command return code:6");
             //6 = Other Error 
             return 6;  
        }

}//int DispenseCard_c(int Timeout) end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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


int RejectCard_c( int   Timeout,
                  char *LogdllPathstr,
                  char *LogFileName,
                  char *deviceid,
                  int  fnLogfileMode)
{


        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

        writeFileLog("[RejectCard_c()] Going to send Current Report status of st0, st1, st2");
        
        bcc = getBcc(9,g_getStatus);
        
        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[RejectCard_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);


        ///////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ////////////////////////////////////////////////////////////////////////////////////////


        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		 
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[RejectCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[RejectCard_c()] Failed to Send Report status Command Communication Failure return code: 1");
                    // 1 = Communication Failure 
                    return 1;
                 }
	}

        writeFileLog("[RejectCard_c()] Status Command send Successfully");

        writeFileLog("[RejectCard_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[RejectCard_c()]  Failed to recieved Acknowledgement Byte Communication Failure return code: 1");
                 return 1;
        }
        else
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[RejectCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 writeFileLog(log);
				
	}

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      writeFileLog("[RejectCard_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                       
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);

				 ClearTransmitBuffer(g_ComHandle);   
         
                                 writeFileLog("[RejectCard_c()]  Failed to recieved reply Byte report status command Communication Failure return code: 1");

				 return 1;

		      }    

                      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      //Check Negative Reply bytes
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[RejectCard_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              writeFileLog(log); 

		              //Force Card Detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
                                  ( 0x36 == rcvPkt[8] ) )
			      {
		                       writeFileLog("[RejectCard_c()] Negative response receieved for issue device status command");  
	   
		                       if( 1 == MutekInitWithErrorCardBin( LogdllPathstr,LogFileName,deviceid,fnLogfileMode) )
		                       {
		                            
                                                writeFileLog("[RejectCard_c()] Successfully removed block card so operation successfully done"); 
   
		                                return 0; //Operation successfull card return to collection bin successfully

		                       }
		                       else
		                       {
                                              writeFileLog("[RejectCard_c()] Failed to do operation for block card status other error"); 
   
		                               return 5; //Operation not successfull so return operation timeout happened

		                       }


					 
		              }


                      }//if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[RejectCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[RejectCard_c()] No Card in MTK-571");
                                           // 3 = No Card in the Channel 
                                           noCardInRF_IC = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                        
                                           writeFileLog("[RejectCard_c()] One Card in Gate");
                                                                              
                  			}
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[RejectCard_c()] One Card on RF/IC Card Position");
                                            
                                           
		                        } 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[RejectCard_c()] No Card in Stacker");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[RejectCard_c()] Few Card in Stacker");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[RejectCard_c()] Enough Card in the Box");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[RejectCard_c()] Error Card bin Not Full");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[RejectCard_c()] Error Card Bin Full");
                                           // 2 = Rejection Bin Full  
                                           rejectionBinFull = 1;
		                        }
                                        

                                }
                                
				
	              }//for(i=0;i<rcvPktLen;i++) block

                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            writeFileLog("[RejectCard_c()] Failed to send Acknowledgement byte command Communication Failure return code:1");
                            // 1 = Communication Failure 
                            return 1;
		      }
                      if(1 == noCardInRF_IC)
                      {
                           // 3 = No Card in the Channel 
                           writeFileLog("[RejectCard_c()] No Card in Chanel return code:3");
                           return 3;
                      }
                      if(1 == rejectionBinFull)
                      {
                           // 2 = Rejection Bin Full  
                           writeFileLog("[RejectCard_c()] Rejection Bin Full return code:2");
                           return 2;

                      }
                      
                      
                       
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             writeFileLog("[RejectCard_c()] Device status Command Nak Reply Received return code:5");
             // 5 = Other Error 
             return 5;  
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x04 == rcvPkt[0])
         {
             writeFileLog("[RejectCard_c()] Device status Command EOT Reply Received return code:5");
             // 5 = Other Error 
             return 5;  
         }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Now Check sensor status
        if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
        {

            if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
            {
                writeFileLog("[RejectCard_c()] Sensor 1 Status Found Blocked As Card in Mouth Other Error Found:5");
                // 5 = Other Error 
                return 5;  


            }//if(  1 == (MTKSensorStatus & 0x01) ) block end
              
          


        }//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end

 


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         // All status ok and Going to issue Reject Card 

         writeFileLog("[RejectCard_c()] Going to send Reject Card Comamnd");

         //Reinit bcc value
         g_moveToErrorBin[8] =0x00;

         bcc = getBcc(9,g_moveToErrorBin);

         g_moveToErrorBin[8] = bcc;
         
         memset(log,'\0',LOG_ARRAY_SIZE);

         sprintf(log,"[RejectCard_c()] bcc value is 0x%xh",bcc);
        
         writeFileLog(log);
        

        ///////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
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
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_moveToErrorBin[i]);
                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[RejectCard_c()] Failed to Send Reject Card Command return code:1");
                    // 1 = Communication Failure 
                    return 1;
                 }
	}

        writeFileLog("[RejectCard_c()] Reject Card Command send Successfully");

        writeFileLog("[RejectCard_c()] Now Going to read Acknowledgement");

        // Now going to Check Acknowledgement  

        totalByteToRecv = 1;

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[RejectCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);

                 ClearTransmitBuffer(g_ComHandle);

                 writeFileLog("[RejectCard_c()] Failed to check Acknowledgement byte for reject command return code:1");

                 return 1;

        }
        else  if(0x06 == rcvPkt[0]) //ack byte receieved
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[RejectCard_c()] Acknowledgement against Reject Card Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 writeFileLog(log);

                 totalByteToRecv = 12;

                 rcvPktLen = 0;

                 memset(rcvPkt,'\0',24); 
         
                 rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

                 /////////////////////////////////////////////////////////////////////////////////

                 if(0 == rtcode)
		 {
		      // 1 = Communication Failure 
		      ClearReceiveBuffer (g_ComHandle);
		      ClearTransmitBuffer(g_ComHandle);
                      writeFileLog("[RejectCard_c()] Communication Failure when read reject command reply bytes.");
		      return 1;
		 }  
               
                 //Now send ack byte to mtk
                 int send_rvalue = 0;
                 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 if(0 == send_rvalue) 
		 {
		       writeFileLog("[RejectCard_c()] Failed to send Acknowledgement byte for reject command .");
                          
		 }

                 //////////////////////////////////////////////////////////////////////////////////
           
                 //Check Negative Reply bytes
                 if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                 {

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[RejectCard_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                      writeFileLog(log); 

                      writeFileLog("[RejectCard_c()] Reject Failed to do.");

		      return 5;//other error

                 }
                 else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] ) &&
                          ( 0x32 == rcvPkt[5] ) && ( 0x33 == rcvPkt[6] ) )
                 {
                
		         writeFileLog("[RejectCard_c()] Reject Successfully done.");

		         return 0;
                 }
                 else
                 {

                         writeFileLog("[RejectCard_c()] Reject Failed to do.");

		         return 5; //Other Error

                 }
                
				
	}
        // This function does not end here because Rejection and collection bin are not same 
        else  if(0x15 == rcvPkt[0]) //nak byte receieved
	{
              writeFileLog("[RejectCard_c()] Reject Failed to do a nak receieved.");

              return 5; //Other Error

        }
        else  if(0x04 == rcvPkt[0]) //ack byte receieved
	{
              writeFileLog("[RejectCard_c()] Reject Failed to do a eot receieved.");

              return 5; //Other Error

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


int ReturnCard_c(    int   DispenseMode,
                     int   Timeout,
                     char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int   fnLogfileMode)
{

        SmartCardSetupLog(LogdllPathstr,LogFileName,deviceid,fnLogfileMode );

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

        writeFileLog("[ReturnCard_c()] Going to send Current Report status of st0, st1, st2");

        bcc = getBcc(9,g_getStatus);

        g_getStatus[8] = bcc;
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[ReturnCard_c()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
                          "Tx",
                          g_getStatus,
                          9
                        );

 
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////



        // Before Send Command clear all serial buffer 
        ClearReceiveBuffer (g_ComHandle);

        ClearTransmitBuffer(g_ComHandle);
       
        for(i=0;i<9;i++)
	{
		
                 //memset(log,'\0',LOG_ARRAY_SIZE);

                 //sprintf(log,"[ReturnCard_c()] Report Status Command[%d] = 0x%xh",i,g_getStatus[i]);
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,g_getStatus[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[ReturnCard_c()] Failed to Send Report status Command Communication Failure rtcode:1");
                    // 1 = Communication Failure 
                    return 1;
                 }


	}//for(i=0;i<9;i++) end

        ///////////////////////////////////////////////////////////////////////////////////////////

         writeFileLog("[ReturnCard_c()] Status Command send Successfully");

         writeFileLog("[ReturnCard_c()] Now Going to read Acknowledgement");

         //Now going to Check Acknowledgement  

         totalByteToRecv = 1;

         memset(rcvPkt,'\0',24); 
         
         rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

         memset(log,'\0',LOG_ARRAY_SIZE);

         sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
         writeFileLog(log);

         if(0 == rtcode)
         {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[ReturnCard_c()] Acknowledgement Failed to Received for Report status Command.");
                 return 1;
         }
         else
	 {
		
               memset(log,'\0',LOG_ARRAY_SIZE);

               sprintf(log,"[ReturnCard_c()] Acknowledgement against Report status Command [0] = 0x%xh.\n",rcvPkt[0]);
        
               writeFileLog(log);

				
	 }

         // If Return Data is 06h then Going to Read 25byte Data 

         if(0x06 == rcvPkt[0])
         {

                      writeFileLog("[ReturnCard_c()] Acknowledgement Received for Report status Command.");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                      
                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[ReturnCard_c()] Communication Failure when read status command reply bytes.");
				 return 1;
		      }  
           
                      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                      
                      //Check Negative Reply bytes
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[ReturnCard_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              writeFileLog(log); 

		              //FOrce card detection
		              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
		                  ( 0x36 == rcvPkt[8] ) )
			      {
		                         writeFileLog("[ReturnCard_c()] Negative response receieved for issue device status command");  
	   
		                         if( 1 == MutekInitWithCardMoveInFront( LogdllPathstr,
                                             LogFileName,deviceid,fnLogfileMode) )
		                         {
                                                writeFileLog("[ReturnCard_c()] operation successfully and return blocked card.");
		                                
		                                return 0; //Operation successfull card return successfull

		                         }
		                         else
		                         { 
                                               writeFileLog("[ReturnCard_c()] retrun mouth blocked.");

		                               return 2; //return mouth blocked

		                         }

					 
		              }
                              else
                              {
                                  writeFileLog("[ReturnCard_c()] Other Error found when issue  device status command.");
                                  return 5;//other error
 
                              }

                      }//if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )

                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                //memset(log,'\0',LOG_ARRAY_SIZE);

                                //sprintf(log,"[ReturnCard_c()] Report status Command Reply Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                //writeFileLog(log);

                                // Card Status Code st0 
                                if(7 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] No Card in MTK-571.");
                                           // 4 = No Card in the Channel  
                                           noCardInChannelFlag = 1;
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] One Card in Gate.");
                                           // 2 = Return mouth block 
                                           returnMouthFlag = 1;
                  			   
                                            
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] One Card on RF/IC Card Position.");
                                           oneCardInRF_IC = 1;
                                           
		                        } 
                                }
                                // Card Status Code st1 
                                if(8 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] No Card in Stacker.");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] Few Card in Stacker.");
		                        }
                                        else if(0x32 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] Enough Card in the Box.");
		                        }   
                                } 
                                // Card Status Code st2 
                                if(9 == i)
                                {
		                        if(0x30 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] Error Card bin Not Full.");
		                        }
                                        else if(0x31 == rcvPkt[i])
		                        {
                                           writeFileLog("[ReturnCard_c()] Error Card Bin Full.");
                                           ErrorBinFull = 1; 
                                           
		                        }
                                        

                                }
                                
				
	              }

                      /////////////////////////////////////////////////////////////////////////////////////////////////

                      // Now time to send Acknowledgement 
                      send_rvalue = 0;
                      send_rvalue = SendSingleByteToSerialPort(g_ComHandle,0x06);
                      ClearReceiveBuffer (g_ComHandle);
                      ClearTransmitBuffer(g_ComHandle);
                      if(0 == send_rvalue) 
		      {
		            writeFileLog("[ReturnCard_c()] Failed to send Acknowledgement byte for report status command return code: 1.");
                            // 1 = Communication Failure 
                            return 1;
		      }
                      else if(1 == noCardInChannelFlag)
                      {
                           // 3 = No Card in the Channel 
                           writeFileLog("[ReturnCard_c()] No Card in Channel return code:3.");
                           return 3; 
                        
                      }
                      else if(1 == returnMouthFlag)
                      {
                           // 2 = Return mouth block 
                           writeFileLog("[ReturnCard_c()] Return Mouth Block return code:5.");
                           return 5; 
                           
                      }
                      else if( 1 == ErrorBinFull )
                      {
                           // 2 = Rejection Bin full
                           writeFileLog("[ReturnCard_c()] Rejection bin full:2.");
                           return 2; 

                      }
                      
                       
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x15 == rcvPkt[0])
         {
             writeFileLog("[ReturnCard_c()] Nak Reply Received for report status command return code:5.");
             // 5 = Other Error 
             return 5;  
         }
         // If Return Data is 15h then No need to read Data 
         else if (0x04 == rcvPkt[0])
         {
             writeFileLog("[ReturnCard_c()] EOT Reply Received for report status command return code:5.");
             // 5 = Other Error 
             return 5;  
         }


         ///////////////////////////////////////////////////////////////////////////////////////////////

         //Issue sensor status command
        
         //Now Check sensor status
         if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) )
         {

            if(  1 == (MTKSensorStatus & 0x01) ) //Sensor 1 status
            {
                writeFileLog("[ReturnCard_c()] Sensor 1 Status Found Blocked As Card in Mouth Found:2");
                // 2 = Card in Mouth
                return 2;  


            }//if(  1 == (MTKSensorStatus & 0x01) ) block end
              
          


         }//if( true == GetMTKSensorStatus( &ChanelStatus, &MTKSensorStatus ) ) block end

 


         ///////////////////////////////////////////////////////////////////////////////////////////////


         // Return Mouth not blocked and No card in Channel So going to check one card in One Card on RF/IC Card Position or Not 
         if(0 == DispenseMode) //Hold at the mouth untill it is taken by out by customer
         {
            
		    writeFileLog("[ReturnCard_c()] Hold at the Mouth untill it is taken by the customer.");
                    if( 0 == oneCardInRF_IC )
		    {
		       // 5 = Other Error 
		       writeFileLog("[ReturnCard_c()] Unable to Dispense Error No Card in RF/IC ");    
		       return 5;
		      
		    }
		    else if(1 == oneCardInRF_IC)
		    {
		            // Found one Card in RF/IC Reader 
		            writeFileLog("[ReturnCard_c()] Got One Card in RF/IC Reader and Going to return from Mouth.");
		            // Going to issue command to send the card to Mouth */ 
		        
		            // Now going to get bcc 
			    writeFileLog("[ReturnCard_c()] Going to get bcc for Hold at Mouth.");

		            //Reinit bcc value
		            g_holdAtMouth[8] =0x00;

			    bcc = getBcc(9,g_holdAtMouth);

			    g_holdAtMouth[8] = bcc;
			    
		            memset(log,'\0',LOG_ARRAY_SIZE);

		            sprintf(log,"[ReturnCard_c()] bcc value is 0x%xh.",bcc);
		
		            writeFileLog(log);

			    memset(DispenseModeData,'\0',9);

			    for(i=0;i<9;i++)
			    {
				     DispenseModeData[i]=g_holdAtMouth[i];

			    }

		    }//else if(1 == oneCardInRF_IC) end

         } 
         else if(1 == DispenseMode) //Dispense it immediately
         {
		    writeFileLog("[ReturnCard_c()] Dispense it immediately.");
		    
		    if( 0 == oneCardInRF_IC )
		    {
		       // 5 = Other Error 
		       writeFileLog("[ReturnCard_c()] Unable to Dispense Error No Card in RF/IC .");    
		       return 5;
		      
		    }
                    else if( 1 == oneCardInRF_IC ) //Found one card in RF/IC Reader
		    {
		           
		            // Found one Card in RF/IC Reader 
		            writeFileLog("[ReturnCard_c()] Got One Card in RF/IC Reader and Going to Send to Error bin.");
		            // Going to issue command to send the card to error bin */
		       
		            // Now going to get bcc 
			    writeFileLog("[ReturnCard_c()] Going to get bcc for Send to Error bin .");

			    bcc = getBcc(9,MovetoGateCommand);

			    MovetoGateCommand[8] = bcc;
			    
		            memset(log,'\0',LOG_ARRAY_SIZE);

		            sprintf(log,"[ReturnCard_c()] bcc value is 0x%xh",bcc);
		
		            writeFileLog(log);

			    memset(DispenseModeData,'\0',9);

			    for(i=0;i<9;i++)
			    {
				DispenseModeData[i]=MovetoGateCommand[i];
			    }

		    }//else if( 1 == oneCardInRF_IC ) //Found one card in RF/IC Reader

         }
         else
         {
            writeFileLog("[ReturnCard_c()] Function parameter DispenseMode not Ok.");
            // 5 = Other Error 
            return 5;
         }

        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         LogDataExchnage( DATA_EXCHANGE_ALL,
                          g_Deviceid,
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
        
                 //writeFileLog(log);

		 send_rvalue = SendSingleByteToSerialPort(g_ComHandle,DispenseModeData[i]);

                 if(0 == send_rvalue) 
                 {
                    writeFileLog("[ReturnCard_c()] Failed to Send Dispense Mode Command Communication Failure.");
                    // 1 = Communication Failure 
                    return 1;
                 }


	}//for block

        writeFileLog("[ReturnCard_c()] Dispense Mode Command send Successfully.");

        writeFileLog("[ReturnCard_c()] Now Going to read Acknowledgement.");
        
        // Now going to Check Acknowledgement  

        totalByteToRecv = 1; 

        memset(rcvPkt,'\0',24); 
         
        rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);

        
        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d.",rtcode,rcvPktLen);
        
        writeFileLog(log);

        if(0 == rtcode)
        {
                 // 1 = Communication Failure 
                 ClearReceiveBuffer (g_ComHandle);
                 ClearTransmitBuffer(g_ComHandle);
                 writeFileLog("[ReturnCard_c()] Ack bytes for return command read failed.");
                 return 1;
        }
        else
	{
		 
                 memset(log,'\0',LOG_ARRAY_SIZE);

                 sprintf(log,"[ReturnCard_c()] Acknowledgement against Dispense Mode Command [0] = 0x%xh.\n",rcvPkt[0]);
        
                 writeFileLog(log);
				
	} 

        // If Return Data is 06h then Going to Read 25byte Data 

        if(0x06 == rcvPkt[0])
        {

                      writeFileLog("[ReturnCard_c()] Acknowledgement Received");

                      totalByteToRecv = 12;

                      memset(rcvPkt,'\0',24);
                      
                      rtcode = statusRead(g_ComHandle,rcvPkt,&rcvPktLen,totalByteToRecv);
                       
                      //memset(log,'\0',LOG_ARRAY_SIZE);

                      //sprintf(log,"[ReturnCard_c()] Receive packet status = %d and Length = %d",rtcode,rcvPktLen);
        
                      //writeFileLog(log);

                      if(0 == rtcode)
		      {
				 // 1 = Communication Failure 
				 ClearReceiveBuffer (g_ComHandle);
				 ClearTransmitBuffer(g_ComHandle);
                                 writeFileLog("[ReturnCard_c()] Reply bytes for return command read failed.");
				 return 1;
		      }    

                      /*
         
                      for(i=0;i<rcvPktLen;i++)
	              {
				
                                memset(log,'\0',LOG_ARRAY_SIZE);

                                sprintf(log,"[ReturnCard_c()] Initialize Command Dispense Mode Data[%d] = 0x%xh",i,rcvPkt[i]);
        
                                writeFileLog(log);

                      }

                      */
                      if( (rcvPktLen> 0) && ( 0x4e == rcvPkt[4] ) )
                      {


                              memset(log,'\0',LOG_ARRAY_SIZE);

                              sprintf(log,"[ReturnCard_c()] Got 0x4e Error with E1:= 0x%xh and E2:= 0x%xh.",rcvPkt[7],rcvPkt[8]);

                              writeFileLog(log); 
     
                              if( ( 0x4e == rcvPkt[4] ) && 
                                  ( 0x31 == rcvPkt[7] ) && 
		                  ( 0x36 == rcvPkt[8] ) )
                              {
                                  writeFileLog("[ReturnCard_c()] Return mouth blcoked.");
                                  return 2;

                              }
                              else
                              {
                                  writeFileLog("[ReturnCard_c()] Negative reply receieved.");
                                  return 5; //Other error

                              }

                      }
                      //Hold at mouth : 0x50,0x32,0x30
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] )  && 
                               ( 0x32 == rcvPkt[5] ) && ( 0x30 == rcvPkt[6] ) )
                      {
                          // 0 = Operation Successful 
                          writeFileLog("[ReturnCard_c()] Hold at Mouth Return Operation Successful done");
                          return 0;


                      }

                      //Throw card :0x43 0x32 0x39;
                      else if( (rcvPktLen> 0) && ( 0x50 == rcvPkt[4] )  && 
                               ( 0x32 == rcvPkt[5] ) && ( 0x39 == rcvPkt[6] ) )
                      {
                          // 0 = Operation Successful 
                          writeFileLog("[ReturnCard_c()] Throw Card Return Operation Successful done");
                          return 0;


                      }
                      else 
                      {
                          writeFileLog("[ReturnCard_c()] Return failed other reason");
                          return 5; //Other Error

                      }

                      
        }
        // If Return Data is 15h then No need to read Data 
        else if (0x15 == rcvPkt[0])
        {
             writeFileLog("[ReturnCard_c()] Nak Reply Received for return command");
             // 5 = Other Error 
             return 5;  
        }
        else if (0x04 == rcvPkt[0])
        {
             writeFileLog("[ReturnCard_c()] EOT Reply Received for return command");
             // 5 = Other Error 
             return 5;  
        }





}//int ReturnCard_c(int DispenseMode,int Timeout) end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int IsAnyCardPresentInReaderPosition()
{

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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int CheckCardType()
{

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// RecvPackt[0] = 0xfffffff2. //STX
// RecvPackt[1] = 0x0.        //ADDR
// RecvPackt[2] = 0x0.  //Length High
// RecvPackt[3] = 0x18. //Length Low
// RecvPackt[4] = 0x50. //CMH
// RecvPackt[5] = 0x60. //Command
// RecvPackt[6] = 0x30. //Command Parameter

// RecvPackt[7] = 0x32. //ST0
// RecvPackt[8] = 0x30. //ST1
// RecvPackt[9] = 0x30. //ST2

//Card Type
// RecvPackt[10] = 0x41. //Rtype

//Card Name
// RecvPackt[11] = 0x3.  //ATQ First Byte  
// RecvPackt[12] = 0x44. //ATQ Second Byte

// RecvPackt[13] = 0x7.  //UID Length
// RecvPackt[14] = 0x4.  //UID_Data 
// RecvPackt[15] = 0xe.  //UID_Data 
// RecvPackt[16] = 0x23. //UID_Data 
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


int ActivateContactLessCard( char *APIReply,
                             int  *ReplyAPDULength,
                             char *LogdllPathstr,
                             char *LogFileName,
                             char *deviceid,
                             int  fnLogfileMode)
{

        writeFileLog("[ActivateContactLessCard()] Entry");

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
        
        
        int CommandLength = 11;

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

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(command,CommandLength) )
        {

                    writeFileLog("[ActivateContactLessCard()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[ActivateContactLessCard()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[ActivateContactLessCard()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x60 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //Byte 0 Status of Card Activation
                                     if( 0x30 == RecvPackt[7]  )
		                     {
				              APIReply[0]= 0; //no card found
				     }
				     else if( 0x31 == RecvPackt[7]  )
				     {

				              APIReply[0]= 2; //card found and activated failed
				     }
				     else if( 0x32 == RecvPackt[7] )
				     {

				               APIReply[0]= 1; //card found and activated
				     }
                                     

		                     //Byte 1 Type of Card Found 
                                     if( ( 0x00 == RecvPackt[11] ) && ( 0x44 == RecvPackt[12] ) ) //UltraLight
                                     {
                                         APIReply[1]= 3;
                                     }
                                     if( ( 0x03 == RecvPackt[11] ) && ( 0x44 == RecvPackt[12] ) ) //Desfire EV1
                                     {
                                         APIReply[1]= 2;
                                     }

		                     //Byte 2 Size of UID
                                     APIReply[2]= RecvPackt[13] ; //UID Data Length

                                     /*

		                     //Byte 3-9 UID Byte
                                     APIReply[3]= RecvPackt[14] ; //UID_Data 
                                     APIReply[4]= RecvPackt[15] ; //UID_Data 
                                     APIReply[5]= RecvPackt[16] ; //UID_Data 
                                     APIReply[6]= RecvPackt[17] ; //UID_Data 
                                     APIReply[7]= RecvPackt[18] ; //UID_Data 
                                     APIReply[8]= RecvPackt[19] ; //UID_Data 
                                     APIReply[9]= RecvPackt[20] ; //UID_Data 

                                     */

                                     //Byte 3-N UID Length
                                     int UIDData_Length =RecvPackt[13],counter=0,
                                     UIDDataStartIndex=14,ReplyArrayCounter=3;

                                     for(counter=0;counter<UIDData_Length;counter++)
                                     {
                                           APIReply[ReplyArrayCounter] =  RecvPackt[UIDDataStartIndex];
                                           UIDDataStartIndex++;
                                           ReplyArrayCounter++;
                                     }
                                     
                                     //Now Set ReplyAPDULength
                                     *ReplyAPDULength = RecvPackt[13]+3; //Ststus Byte+CardTypeByte+UID LengthByte

                                     writeFileLog("[ActivateContactLessCard() Exit] Successfully found activate contact less card.");
                                     return true;
		                
                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[ActivateContactLessCard() Exit] Reply Bytes receieve from MUTEK is failed ");
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                    writeFileLog("[ActivateContactLessCard() Exit] Negative Response from mutek  ");
		                    APIReply[0] = 6 ; //other error
                                    *ReplyAPDULength = 1;
                                    return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                 writeFileLog("[ActivateContactLessCard() Exit] Operation timeout ");
		                APIReply[0] = 5 ; //operation time out
		                *ReplyAPDULength = 1;
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[ActivateContactLessCard() Exit] Ack receieved failed");
                         APIReply[0] = 6 ; //other error
		         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[ActivateContactLessCard() Exit] Command Transmit failed");
            APIReply[0] = 4 ; //communication failure
            *ReplyAPDULength = 1;
             return false;
        }


}//ActivateContactLessCard() end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int DeActivateContactLessCard( int *Reply,
                               char *LogdllPathstr,
                               char *LogFileName,
                               char *deviceid,
                               int  fnLogfileMode)
{


        writeFileLog("[DeActivateContactLessCard()] Entry");

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

                    writeFileLog("[DeActivateContactLessCard()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[DeActivateContactLessCard()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[DeActivateContactLessCard()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x60 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //Byte 0 Status of Card Activation
                                     if( 0x30 == RecvPackt[7]  )
		                     {
				              *Reply = 0; //no card found

				     }
				     else if( 0x31 == RecvPackt[7]  )
				     {

				              *Reply = 2; //card found and deactivated failed

				     }
				     else if( 0x32 == RecvPackt[7] )
				     {

				              *Reply = 1; //card found and deactivated successfully

				     }
                                     
                                     writeFileLog("[DeActivateContactLessCard() Exit] Successfully get deactivate status from mutek.");                           
		                     return true;

		                }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     *Reply = 5 ; //other error

                                     writeFileLog("[DeActivateContactLessCard() Exit] Negative ack receieved.");                           
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                *Reply = 5 ; //operation time out

		                writeFileLog("[DeActivateContactLessCard() Exit] Timeout occuered.");                           
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         *Reply = 5 ; //other error
                          writeFileLog("[DeActivateContactLessCard()] Fail receieve ack byte from mutek.");
                          return false;
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            *Reply = 5 ; //other error
            writeFileLog("[DeActivateContactLessCard()] Fail transmit command to mutek.");
            return false;
        }


}//int DeActivateContactLessCard(char *APIReply) end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ContactLessCardXChangeAPDU(  char *CommandAPDU, 
                                 int   CommandAPDULength,
                                 char *ReplyAPDU,
                                 int  *ReplyAPDULength,
                                 char *LogdllPathstr,
                                 char *LogFileName,
                                 char *deviceid,
                                 int   fnLogfileMode 
                              )
{



        writeFileLog("[ContactLessCardXChangeAPDU()] Entry");

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        char XChangeAPDUBuffer[LOG_ARRAY_SIZE],tempXChangeAPDUBuffer[LOG_ARRAY_SIZE];

        memset(XChangeAPDUBuffer,'\0',LOG_ARRAY_SIZE);     

        memset(tempXChangeAPDUBuffer,'\0',LOG_ARRAY_SIZE);

        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ContactLessCardXChangeAPDU()] Command APDU Length = %d .",CommandAPDULength);
        writeFileLog(log);

        int TotalCommandLength=0;
        //3 Byte: CMT+PM+CM        
        TotalCommandLength = CommandAPDULength+3;

        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ContactLessCardXChangeAPDU()] Total Comamnd Length = %d .",TotalCommandLength);
        writeFileLog(log);

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

        memset(RecvPackt,'\0',300);

        //Step 1 : Send Command to MUTEK
        if( true == MUTEK_CommandTransmit(APDUCommand,CommandLength) )
        {

                    writeFileLog("[ContactLessCardXChangeAPDU()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                                writeFileLog("[ContactLessCardXChangeAPDU()] ACK receieve from MUTEK");

		                //Step 3: After Receieve ACK Go for read reply bytes
		        
                                MUTEK_CommandReceieve_V2(RecvPackt);
		         
                                writeFileLog("[ContactLessCardXChangeAPDU()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x60 == RecvPackt[5] ) &&  //CM
		                    ( 0x34 == RecvPackt[6] )     //PM
		                  )
		                {
				             int ReplyAPDUCounter=1;
				             ReplyAPDU[0] = 1;//Success
		                             //Get Return R-APDU Length 
		                             int RceieveTextLength = 0;
		                             RceieveTextLength = (RecvPackt[2]<<8)+RecvPackt[3];

		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactLessCardXChangeAPDU()] Receieve Text length = %d .",RceieveTextLength);
		                             writeFileLog(log);

		                             int RAPDUTextLength =0,StartRAPDUIndex=10,CountDownLength=0;
		                             
		                             RAPDUTextLength = RceieveTextLength-6 ; //6 Byte: PMT+CM+PM+ST0+ST1+ST2
		                             
		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactLessCardXChangeAPDU()] APDU Text length = %d .",RAPDUTextLength);

		                             writeFileLog(log);

                                             strcat(XChangeAPDUBuffer,"[ContactLessCardXChangeAPDU()] ReplyAPDU [0x]: ");

		                             for(;CountDownLength<RAPDUTextLength;CountDownLength++)
		                             {
		                                   ReplyAPDU[ReplyAPDUCounter] =  RecvPackt[StartRAPDUIndex];

		                                   //memset(log,'\0',LOG_ARRAY_SIZE);

		                                   //sprintf(log,"[ContactLessCardXChangeAPDU()] ReplyAPDU[%d] = 0x%xh. RecvPackt[%d]=0x%xh.",ReplyAPDUCounter,ReplyAPDU[ReplyAPDUCounter],StartRAPDUIndex,RecvPackt[StartRAPDUIndex]);

		                                   //writeFileLog(log);

                                                   sprintf(tempXChangeAPDUBuffer,"%x ",ReplyAPDU[ReplyAPDUCounter]);

                                                   strcat(XChangeAPDUBuffer,tempXChangeAPDUBuffer);
                             
		                                   ReplyAPDUCounter++;
		                                   
		                                   StartRAPDUIndex++;
		                                  
		                             }

                                             writeFileLog(XChangeAPDUBuffer);

		                             //Now Set ReplyAPDULength
		                             *ReplyAPDULength = RAPDUTextLength+1;

		                              memset(log,'\0',LOG_ARRAY_SIZE);

		                              sprintf(log,"[ContactLessCardXChangeAPDU() Exit] Successfully get APDU and return array length = %d .",*ReplyAPDULength);

		                              writeFileLog(log);
				              
		                              return 1;

				        }
		                        else
					{
						     
				                   ReplyAPDU[0] =  4 ;//Other Error
		                                   *ReplyAPDULength = 1;
		                                   writeFileLog("[ContactLessCardXChangeAPDU() Exit] Other Error Block1.");
				                   return 4;
					}

		         }
		         else //failure case
		         {

		                    writeFileLog("[ContactLessCardXChangeAPDU()] Reply Bytes receieve from MUTEK is failed .");

				    if( 0x4e == RecvPackt[4] ) //Negative PMT header
				    {
				             ReplyAPDU[0] =  4 ; //other error
		                             *ReplyAPDULength = 1;
		                             writeFileLog("[ContactLessCardXChangeAPDU() Exit] Negative ACK receieved.");
		                             return 2;
				         
				    }//if( 0x4e == RecvPackt[4] )
				    else
				    {
				             
		                           ReplyAPDU[0] =  3 ; //operation time out
		                           *ReplyAPDULength = 1;
		                           writeFileLog("[ContactLessCardXChangeAPDU() Exit] Operation timeout.");
		                           return 3;
				    }
              
                      
				

		      }
		    

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[ContactLessCardXChangeAPDU() Exit] Command Transmit failed");
            ReplyAPDU[0] = 4 ;//Other Error
            *ReplyAPDULength = 1;
            return 4;
        }




}//int ContactLessCardXChangeAPDU(char* CommandAPDU,char* ReplyAPDU) end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int SelectSAM(int SAMSlotId)
{


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

                    writeFileLog("[SelectSAM()] Command Transmit success");

                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[SelectSAM()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[SelectSAM()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x40 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //byte 7 is Status of sam Card Activation
                                     if( 0x30 == RecvPackt[7]  )
		                     {
				          //no card found
                                          writeFileLog("[SelectSAM() Exit] no sam card found ");
                                          return false;
				     }
				     else if( 0x31 == RecvPackt[7]  )
				     {
				          //card found and activated failed
                                          writeFileLog("[SelectSAM() Exit] sam card found but activation failed ");
                                          return false;
				     }
				     else if( 0x32 == RecvPackt[7] )
				     {
                                          //card found and activated
                                          writeFileLog("[SelectSAM() Exit] sam card found and activated successfully ");
                                          return true;
     
				     }
                                     

		                     
		                }

		         }
		         else //failure case
		         {

                            writeFileLog("[SelectSAM() Exit] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     writeFileLog("[SelectSAM() Exit] negative ack from mutek");
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[SelectSAM() Exit] reply byte receieve from mutek");
                                return false;
		                
		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[SelectSAM() Exit] fail receieve ack byte from mutek");
                         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(command,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[SelectSAM() Exit] Command Transmit failed");
            return false;
        }


}//int SelectSAM(int SAMSlotId) end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//SAM Card warm reset
int WarmResetSAM( int   SAMSlotId,
                  char *ReplyByte,
                  int  *ReplyByteLength,
                  char *LogdllPathstr,
                  char *LogFileName,
                  char *deviceid,
                  int   fnLogfileMode 
                )
{


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

                    writeFileLog("[WarmResetSAM()] Command Transmit success");
                    
                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[WarmResetSAM()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[WarmResetSAM()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x38 == RecvPackt[6] )     //PM
		                  )
		                {
				              writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match success");

		                             //Byte 0 Status of Card Activation reply byte index 10 hold card type
		                             if( ( 0x30 == RecvPackt[10] ) || ( 0x31 == RecvPackt[10]  ) )
				             {
						  writeFileLog("[WarmResetSAM()] Operation success.");

		                                  ReplyByte[0]=1; //operation success

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
		                                   writeFileLog("[WarmResetSAM()] No SAM Card Found.");
		                                   ReplyByte[0]=0; //no sam card found
		                                  *ReplyByteLength=1;
		                                   return false;

					     }
                                     

                                     
		                }
                                else
                                {
                                       writeFileLog("[WarmResetSAM()] Reply Bytes from MUTEK match failed");
                                       ReplyByte[0]=2; //Operation failed
                                       *ReplyByteLength=1;
                                       return false;
                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[WarmResetSAM()] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                    
                                     writeFileLog("[WarmResetSAM()] Negative ACK receieve from mutek");
		                     ReplyByte[0]=5; //other error
                                    *ReplyByteLength=1;
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                 writeFileLog("[WarmResetSAM()] Timeout happened");
		                 ReplyByte[0]=4; //Timeout happens
                                *ReplyByteLength=1;
                                 return false;

		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[WarmResetSAM()] Fail receieve ACK Byte from MUTEK");
                         ReplyByte[0]=5; //other error
                        *ReplyByteLength=1;
                         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[WarmResetSAM()] Command Transmit failed");
            ReplyByte[0]=3; //communiation failure
            *ReplyByteLength=1;
            return false;
        }


}//int WarmResetSAM(int SAMSlotId,char *ReplyByte,int *ReplyByteLength)




//SAM Card warm reset
int ColdResetSAM( int   SAMSlotId,
                  char *ReplyByte,
                  int  *ReplyByteLength,
                  char *LogdllPathstr,
                  char *LogFileName,
                  char *deviceid,
                  int  fnLogfileMode
                 )
{


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

                    writeFileLog("[ColdResetSAM()] Command Transmit success");
                    
                    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[ColdResetSAM()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[ColdResetSAM()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
		                  )
		                {
		                     writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match success");

                                     //Byte 0 Status of Card Activation reply byte index 10 hold card type
                                     if( ( 0x30 == RecvPackt[10] ) || ( 0x31 == RecvPackt[10]  ) )
		                     {
				          writeFileLog("[ColdResetSAM()] Operation success.");
                                          ReplyByte[0]=1; //operation success
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
                                           writeFileLog("[ColdResetSAM()] No SAM Card Found.");
                                           ReplyByte[0]=0; //No SAM Card Found
                                           *ReplyByteLength=1;
                                           return false;

				     }
				   
                                     
		                }
                                else
                                {
                                     writeFileLog("[ColdResetSAM()] Reply Bytes from MUTEK match failed");
                                     ReplyByte[0]=2; //Operation failed
                                    *ReplyByteLength=1;
                                     return false;

                                }

		         }
		         else //failure case
		         {

                            writeFileLog("[ColdResetSAM()] Reply Bytes receieve from MUTEK is failed ");

		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
                                     writeFileLog("[ColdResetSAM()] Negative ack receieved ");
		                     ReplyByte[0]=5; //Other error
                                    *ReplyByteLength=1;
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
                                writeFileLog("[ColdResetSAM()] Operation timeout. ");
		                ReplyByte[0] = 4; //operation time out
                               *ReplyByteLength=1;
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[ColdResetSAM()] Fail receieve ACK Byte from MUTEK. ");
		         ReplyByte[0] = 5; //other error
                        *ReplyByteLength=1;
		         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[ColdResetSAM()] Command Transmit failed");
            ReplyByte[0] = 3; //communication failure
           *ReplyByteLength=1;
            return false;
        }


}//int ColdResetSAM(int SAMSlotId,char *ReplyByte,int *ReplyByteLength)



/////////////////////////////////////////////////////////////////////////////////////////////////


//Activate SAM Card
int ActivateContactCard(char *APIReply)
{

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

                    writeFileLog("[ActivateContactCard()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                         writeFileLog("[ActivateContactCard()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve_V2(RecvPackt))
		         {

                                writeFileLog("[ActivateContactCard()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x30 == RecvPackt[6] )     //PM
                                    
		                  )
		                {
                                       
		                       writeFileLog("[ActivateContactCard()] Sam Card activate is success");    
				       return true;

		                }// if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                 //   ( 0x52 == RecvPackt[5] ) &&  //CM
		                 //   ( 0x30 == RecvPackt[6] )     //PM
		                 // ) 
                               else  if(  0x4e == RecvPackt[4] )
		               {
                                     APIReply[0] =  6; //other error
		                     writeFileLog("[ActivateContactCard()] 0x4e Byte  recv from MUTEK");
                                     return false;
                               }

		         }
		         else //failure case
		         {

                                 writeFileLog("[ActivateContactCard()] Reply Bytes receieve from MUTEK is failed ");

		                 if( 0x4e == RecvPackt[4] )
		                 {        
                                     APIReply[0] =  6; //other error
		                     writeFileLog("[ActivateContactCard()] Negative ACK Byte from MUTEK");
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		                 else
		                 {
                                     writeFileLog("[ActivateContactCard()] No Recv Byte recv from MUTEK");
		                     APIReply[0] = 0 ; //operation time out
		                     return false;
                                 }

		         }//else block

		    }
		    else
		    {
		         //Fail receieve ACK Byte from MUTEK
                         writeFileLog("[ActivateContactCard()] Fail receieve ACK Byte from MUTEK");
                         return false;

		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[ActivateContactCard()] Command Transmit failed");
            APIReply[0] =  6; //other error
            return false;
        }


}//ActivateContactCard() end



///////////////////////////////////////////////////////////////////////////////////////////////////


//Deactivate Contact card
int DeActivateContactCard( int  *Reply,
                           char *LogdllPathstr,
                           char *LogFileName,
                           char *deviceid,
                           int  fnLogfileMode
                         )
{

        writeFileLog("[DeActivateContactCard()] Entry");

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

                    writeFileLog("[DeActivateContactCard()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {

                         writeFileLog("[DeActivateContactCard()] ACK receieve from MUTEK");

		         //Step 3: After Receieve ACK Go for read reply bytes
		         if ( true == MUTEK_CommandReceieve(RecvPackt,totalByteToRecv))
		         {

                                writeFileLog("[DeActivateContactCard()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x31 == RecvPackt[6] )     //PM
		                  )
		                {
		                   
		                     //Byte 0 Status of Card Activation
                                     if( 0x30 == RecvPackt[7]  )
		                     {
				              *Reply = 0; //no card found

				     }
				     else if( 0x31 == RecvPackt[7]  )
				     {

				              *Reply = 2; //card found and deactivated failed

				     }
				     else if( 0x32 == RecvPackt[7] )
				     {

				              *Reply = 1; //card found and deactivated successfully

				     }
                                     
                                     writeFileLog("[DeActivateContactCard() Exit] Successfully get deactivate status from mutek.");                           
		                     return true;

		                }

		         }
		         else //failure case
		         {

                           
		            if( totalByteToRecv > 0 )
		            {
		                 if( 0x4e == RecvPackt[4] )
		                 {
		                     *Reply = 5 ; //other error

                                     writeFileLog("[DeActivateContactCard() Exit] Negative ACK receieved.");                           
                                     return false;
		                 
		                 }//if( 0x4e == RecvPackt[4] )
		             
		            }//if( totalByteToRecv > 0 ) block
		            else
		            {
		                *Reply = 4 ; //operation time out

		                writeFileLog("[DeActivateContactCard() Exit] Timeout occuered."); 
                          
		                return false;

		            }

		         }//else block

		    }
		    else
		    {
		          //Fail receieve ACK Byte from MUTEK
                          *Reply = 5 ; //other error
                          writeFileLog("[DeActivateContactCard()] Fail receieve ack byte from mutek.");
                          return false;
		    }

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            *Reply = 5 ; //other error
             writeFileLog("[DeActivateContactCard()] Fail transmit command to mutek.");
             return false;
        }





}//int DeActivateContactCard(int *APIReply) end



//////////////////////////////////////////////////////////////////////////////////////////////////

//SAM CARD XCHNAGE APDU
int ContactCardXChangeAPDU(  char *CommandAPDU,
                             int   CommandAPDULength,
                             char *ReplyAPDU,
                             int  *ReplyAPDULength,
                             char *LogdllPathstr,
                             char *LogFileName,
                             char *deviceid,
                             int  fnLogfileMode 
                          )
{



        writeFileLog("[ContactCardXChangeAPDU()] Entry");

        char log[LOG_ARRAY_SIZE];

        memset(log,'\0',LOG_ARRAY_SIZE);

        
        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ContactCardXChangeAPDU()] Command APDU Length = %d .",CommandAPDULength);
        writeFileLog(log);

        int TotalCommandLength=0;
        //3 Byte: CMT+PM+CM        
        TotalCommandLength = CommandAPDULength+3;

        memset(log,'\0',LOG_ARRAY_SIZE);
        sprintf(log,"[ContactCardXChangeAPDU()] Total Comamnd Length = %d .",TotalCommandLength);
        writeFileLog(log);

        char APDUCommand[300];
        memset(APDUCommand,'\0',300);
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

                    writeFileLog("[ContactCardXChangeAPDU()] Command Transmit success");

		    //Step 2: Wait for ACK Byte
		    if( true ==  IsAckReceieve() )
		    {
                                writeFileLog("[ContactCardXChangeAPDU()] ACK receieve from MUTEK");

		                //Step 3: After Receieve ACK Go for read reply bytes
		        
                                MUTEK_CommandReceieve_V2(RecvPackt);
		         
                                writeFileLog("[ContactCardXChangeAPDU()] Reply Bytes receieve from MUTEK is success");

		                if( ( 0x50 == RecvPackt[4] ) &&  //CMH
		                    ( 0x52 == RecvPackt[5] ) &&  //CM
		                    ( 0x34 == RecvPackt[6] )     //PM
		                  )
		                {
				             int ReplyAPDUCounter=1;
				             ReplyAPDU[0] = 1;//Success
		                             //Get Return R-APDU Length 
		                             int RceieveTextLength = 0;
		                             RceieveTextLength = (RecvPackt[2]<<8)+RecvPackt[3];

		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactCardXChangeAPDU()] Receieve Text length = %d .",RceieveTextLength);
		                             
                                             writeFileLog(log);

		                             int RAPDUTextLength =0,StartRAPDUIndex=10,CountDownLength=0;
		                             
		                             RAPDUTextLength = RceieveTextLength-6 ; //6 Byte: PMT+CM+PM+ST0+ST1+ST2
		                             
		                             memset(log,'\0',LOG_ARRAY_SIZE);

		                             sprintf(log,"[ContactCardXChangeAPDU()] APDU Text length = %d .",RAPDUTextLength);

		                             writeFileLog(log);

		                             for(;CountDownLength<RAPDUTextLength;CountDownLength++)
		                             {
		                                   ReplyAPDU[ReplyAPDUCounter] = RecvPackt[StartRAPDUIndex];

		                                   memset(log,'\0',LOG_ARRAY_SIZE);

		                                   sprintf(log,"[ContactCardXChangeAPDU()] ReplyAPDU[%d] = 0x%xh. RecvPackt[%d]=0x%xh.",ReplyAPDUCounter,(unsigned char)ReplyAPDU[ReplyAPDUCounter],StartRAPDUIndex,(unsigned char)RecvPackt[StartRAPDUIndex]);

		                                   writeFileLog(log);
		                                   
		                                   ReplyAPDUCounter++;
		                                   
		                                   StartRAPDUIndex++;
		                                  
		                             }

		                             //Now Set ReplyAPDULength
		                             *ReplyAPDULength = RAPDUTextLength+1;

		                              memset(log,'\0',LOG_ARRAY_SIZE);

		                              sprintf(log,"[ContactCardXChangeAPDU() Exit] Successfully get APDU and return array length = %d .",*ReplyAPDULength);

		                              writeFileLog(log);
				              
		                              return 1;

				        }
		                        else
					{
						     
				                   ReplyAPDU[0] =  4 ;//Other Error
		                                   *ReplyAPDULength = 1;
		                                   writeFileLog("[ContactCardXChangeAPDU() Exit] Other Error Block1");
				                   return 4;
					}

		         }
		         else //failure case
		         {

		                    writeFileLog("[ContactCardXChangeAPDU()] Reply Bytes receieve from MUTEK is failed ");

				    if( 0x4e == RecvPackt[4] ) //Negative PMT header
				    {
				             ReplyAPDU[0] =  4 ; //other error
		                             *ReplyAPDULength = 1;
		                             writeFileLog("[ContactCardXChangeAPDU() Exit] Negative ACK receieved");
		                             return 2;
				         
				    }//if( 0x4e == RecvPackt[4] )
				    else
				    {
				             
		                           ReplyAPDU[0] =  3 ; //operation time out
		                           *ReplyAPDULength = 1;
		                           writeFileLog("[ContactCardXChangeAPDU() Exit] Operation timeout");
		                           return 3;
				    }
              
                      
				

		      }
		    

        }//if( true == MUTEK_CommandTransmit(g_getStatus,CommandLength) )
        else
        {
            //Fail Transmit command to MUTEK
            writeFileLog("[ContactCardXChangeAPDU() Exit] Command Transmit failed");
            ReplyAPDU[0] = 4 ;//Other Error
            *ReplyAPDULength = 1;
            return 4;
        }




}//int ContactCardXChangeAPDU(char* CommandAPDU,char* ReplyAPDU) end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ActivateSAMCard( int   SAMSlotId,
                     int   Timeout,
                     char *SAMAPIReply,
                     char *LogdllPathstr,
                     char *LogFileName,
                     char *deviceid,
                     int   fnLogfileMode
                  )
{

        
              char SAMReplyAPDU[100];
			     
              memset( SAMReplyAPDU,'\0',100);

              char log[LOG_ARRAY_SIZE];

              memset(log,'\0',LOG_ARRAY_SIZE);

              int counter=0;

              if( 1 == ActivateContactCard(SAMAPIReply) )
              {
                              
                              //Byte 0 Status of Card Activation
                              /*

                                1: Card found and activated
				2: Card found but activation failed
				3: Card found but it is unsupported
				4: Communication failure
				5: Operation timeout occurred
				6: Other error

                              */

		              SAMAPIReply[0]= 1; //Card found and activated
					   
                              /*

			      for( counter =0 ; counter < 2; counter++)
			      {
		
				    memset(log,'\0',LOG_ARRAY_SIZE);

				    sprintf(log,"[ActivateSAMCard()] SAMAPIReply[%d] = 0x%xh.",counter,SAMAPIReply[counter]);
		
				    writeFileLog(log);

			      }	

                              */

                              writeFileLog("[ActivateSAMCard()] Successfully activated sam card now going for get uid from it using exchange apdu api");
		 
			      //Now get uid data from sam slot
			      char SAMCommandAPDU[5] ={0x80,0x60,0x00,0x00,0x00};
			      
			      int SAMCommandAPDULength = 5,SAMReplyAPDULength=0 ;
			      
			      if(1 == ContactCardXChangeAPDU( SAMCommandAPDU,
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
                                                  writeFileLog(log);

					   }

                                           */
                                           writeFileLog("[ActivateSAMCard()] Successfully sam card activate with uid data get also sucess");
                                           return true;

			      }
                              else
                              {
                                   writeFileLog("[ActivateSAMCard()] Successfully sam card activate without uid data ");

                                   return false;
 
                              }			     
             

                }
                else
                {
                       writeFileLog("[ActivateSAMCard()] Unable to activate sam card ");

                       return false;
               
                }


}//int ActivateSAMCard(int SAMSlotId,int Timeout,char *SAMAPIReply) end


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool IsAckReceieve()
{
       
          
		//Now Going TO Check ACK from MUTEK   
                char log[LOG_ARRAY_SIZE];
                
                memset(log,'\0',LOG_ARRAY_SIZE);

		int totalByteIn = -1;
		
		char rxByte =0x00;
		
                delay_mSec(100);

                totalByteIn = read(g_ComHandle,&rxByte,1);

                //memset(log,'\0',LOG_ARRAY_SIZE);

                //sprintf(log,"[IsAckReceieve()] Receieve ACK Byte: 0x%x",rxByte);

		//writeFileLog(log);
		
		if( (totalByteIn >0) && (0x06 == rxByte) )
		{
		      return true; 
		       
		}
		else
		{
		      return false; 

		}




}//IsAckReceieve() end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool SendAckToMutek()
{
                      char log[LOG_ARRAY_SIZE];

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      char data=0x06;

                      int rtcode =-1;

                      rtcode = write(g_ComHandle,&data,1);

                      if(rtcode < 0 )
		      {

			   //Unable to write any data to com port
                           return false;



		      }//if(rtcode<0)
                      else 
                      {
                           //write data to com port success
                           return true;


                      }//else block





}//SendAckToMutek() end





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



bool MUTEK_CommandReceieve(char *RecvPackt,int totalByteToRecv)
{


                      char log[LOG_ARRAY_SIZE];

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      int rcvPktLen=0,rtcode=-1,counter=0;

                      //delay_mSec(100);

                      rtcode = statusRead(g_ComHandle,RecvPackt,&rcvPktLen,totalByteToRecv);

                      /*

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[MUTEK_CommandReceieve()] statusRead() Return Code = %d and Receieve packet Length = %d .",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      

                      for(counter=0;counter<rcvPktLen;counter++)
		      { 
				    
				
                                memset(log,'\0',LOG_ARRAY_SIZE);

                                sprintf(log,"[MUTEK_CommandReceieve()] RecvPackt[%d] = 0x%x.",counter,RecvPackt[counter]);
        
                                writeFileLog(log);
				
	              }

                      */
             
                      if(0 == rtcode)
		      {

			       //Timeout and less data recieved
                               return false;

                      }//if(0 == rtcode)
                      else if(1 == rtcode)
                      {
                               //Successfully Return
                               return true;


                      }//else if(1 == rtcode)




}//MUTEK_CommandReceiev() end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



bool MUTEK_CommandReceieve_V2(char *RecvPackt)
{


                      char log[LOG_ARRAY_SIZE];

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      int rcvPktLen=0,rtcode=-1,counter=0;

                      rtcode = statusRead_V2(g_ComHandle,RecvPackt,&rcvPktLen);

                      /*

                      memset(log,'\0',LOG_ARRAY_SIZE);

                      sprintf(log,"[MUTEK_CommandReceieve_V2()] statusRead() Return Code = %d and Receieve packet Length = %d .",rtcode,rcvPktLen);
        
                      writeFileLog(log);

                      
                      for(counter=0;counter<rcvPktLen;counter++)
		      { 
				    
				
                                memset(log,'\0',LOG_ARRAY_SIZE);

                                sprintf(log,"[MUTEK_CommandReceieve_V2()] RecvPackt[%d] = 0x%x.",counter,RecvPackt[counter]);
        
                                writeFileLog(log);
				
	              }

                      */


                      if(0 == rtcode)
		      {

			       //Timeout and less data recieved          
                               writeFileLog("[MUTEK_CommandReceieve_V2()] Timeout and less data recieved");
                               return false;

                      }//if(0 == rtcode)
                      else if(1 == rtcode)
                      {  
                               //Successfully Return
                               writeFileLog("[MUTEK_CommandReceieve_V2()] Successfully data recieved");
                               return true;

                      }//else if(1 == rtcode)



}//MUTEK_CommandReceiev() end



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MUTEK_CommandTransmit(char *Command,int CommandLength)
{


        //////////////////////////////////////////////////////////////////////////

        char log[LOG_ARRAY_SIZE];
        
        memset(log,'\0',LOG_ARRAY_SIZE);

        char bcc=0x00;

        int  counter=0,rtcode=-1;

        //writeFileLog("[MUTEK_CommandTransmit()] Going for calculate bcc value");

        bcc = getBcc(CommandLength-1,Command);

        Command[CommandLength-1] = bcc;

        memset(log,'\0',LOG_ARRAY_SIZE);

        sprintf(log,"[MUTEK_CommandTransmit()] bcc value is 0x%xh",bcc);
        
        writeFileLog(log);

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
        
            writeFileLog(log);
                
	}
        

        */

        
        /////////////////////////////////////////////////////////////////////////////////////////

          LogDataExchnage( DATA_EXCHANGE_ALL,
                           g_Deviceid,
                           "Tx",
                           Command,
                           CommandLength
                         );

        ////////////////////////////////////////////////////////////////////////////////////////

        //Now Send Command to MUTEK
        for(counter=0;counter<CommandLength;counter++)
	{
		
                 rtcode = -1;
		 rtcode = SendSingleByteToSerialPort(g_ComHandle,Command[counter]);
                 if(FAIL  == rtcode ) 
                 {
                    writeFileLog("[MUTEK_CommandTransmit()] Failed to Send Command to MUTEK Device");
                    //Communication Failure
                    return false;
                 }


	}//for loop

        //writeFileLog("[MUTEK_CommandTransmit()] Successfully Transmit Command");

        return true; //Successfully send to MUTEK
        

}//MUTEK_CommandTransmit() end



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
             writeFileLog("\n\n");         

             while(1)
             {         
                     DeviceStatus_C( 0 , 0,ReplyDeviceStatus );
                     for( counter =0 ; counter < 10; counter++)
		     {
		
			    memset(log,'\0',LOG_ARRAY_SIZE);

			    sprintf(log,"[main()] ReplyDeviceStatus[%d] = 0x%xh.",counter,ReplyDeviceStatus[counter]);
		
			    writeFileLog(log);

		      }	
                      writeFileLog("\n\n");
                      break;
                      
             }

             ////////////////////////////////////////////////////////////////////////////////////////

             /*

             char ChanelStatus =0x00;

             char MTKSensorStatus=0x00;

             GetMTKSensorStatus(&ChanelStatus,&MTKSensorStatus);

             memset(log,'\0',LOG_ARRAY_SIZE);

             sprintf(log,"[main()] ChanelStatus = 0x%xh. Sensor Status=0x%xh.",ChanelStatus,MTKSensorStatus);
		
	     writeFileLog(log);
	   
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

                     writeFileLog("=====================================================================");
	     
		     writeFileLog("[main()] Now Going for activate card");
		     
		     int APIReplyLength=0;

		     ActivateContactLessCard(APIReply,&APIReplyLength);
		     
		     for( counter =0 ; counter < APIReplyLength; counter++)
		     {
		
			    memset(log,'\0',LOG_ARRAY_SIZE);

			    sprintf(log,"[main()] APIReply[%d] = 0x%xh.",counter,APIReply[counter]);
		
			    writeFileLog(log);

		      }	
	
		      writeFileLog("====================================================================");

                      return ;

                }


             }//while loop

             

             return ;

             

	     ///////////////////////////////////////////////////////////////////////////////////////////////////////////

	     
	     
	     writeFileLog("=================================================================");

	     /*

	     writeFileLog("[main()] Now Check any card present in card reader");

	     if ( 1 == IsAnyCardPresentInReaderPosition() ) 
	     {
		writeFileLog("[main()] Card is present ");

	     }

	     int cardtype =0;
	     cardtype = CheckCardType();
	     if( 4!= cardtype )
	     {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] Card Type: %d",cardtype);
		
		    writeFileLog(log);

	     }		 

	    */

             ///////////////////////////////////////////////////////////////////////////////////////////////////
           
             
             /*

             //ContactLess card activate and deactivate
	     writeFileLog("=====================================================================");
	     
	     writeFileLog("[main()] Now Going for activate card");
	     
             int APIReplyLength=0;

	     ActivateContactLessCard(APIReply,&APIReplyLength);
	     
	     for( counter =0 ; counter < APIReplyLength; counter++)
	     {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] APIReply[%d] = 0x%xh.",counter,APIReply[counter]);
		
		    writeFileLog(log);

	      }	
	
	      writeFileLog("====================================================================");
	  
              writeFileLog("[main()] Before Send APDU Command");

              //char CommandAPDU =0x60 ;
              char CommandAPDU[4] ={0x5A,0x00,0x01,0x00};
              int CommandAPDULength = 4,ReplyAPDULength=0 ;
              char ReplyAPDU[300];
              memset( ReplyAPDU,'\0',300);
              
              ContactLessCardXChangeAPDU(CommandAPDU,CommandAPDULength,ReplyAPDU,&ReplyAPDULength);

              memset(log,'\0',LOG_ARRAY_SIZE);

	      sprintf(log,"[main()] Reply APDU Array Length = %d ",ReplyAPDULength);
		
	      writeFileLog(log);

              for( counter =0 ; counter < ReplyAPDULength; counter++)
	      {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] ReplyAPDU[%d] = 0x%xh.",counter,ReplyAPDU[counter]);
		
		    writeFileLog(log);

	      }	

              writeFileLog("[main()] After Send APDU Command");

              writeFileLog("====================================================================");
              
              

	      int Reply=-1;
	      DeActivateContactLessCard(&Reply);
	      memset(log,'\0',LOG_ARRAY_SIZE);
	      sprintf(log,"[main()] Deactivate Return Code: %d",Reply);
	      writeFileLog(log);
	      writeFileLog(" =======================================================");

              */
             
             
             ///////////////////////////////////////////////////////////////////////////////////////////////////////             

             /*

             writeFileLog("=====================================================================");
             
             memset(APIReply,'\0',10);

             ActivateContactCard(APIReply);

             for( counter =0 ; counter < 10; counter++)
	     {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] APIReply[%d] = 0x%xh.",counter,APIReply[counter]);
		
		    writeFileLog(log);

	     }	
             
             writeFileLog("=====================================================================");

             WarmResetSAM(1);

             writeFileLog("=====================================================================");


             */
             ///////////////////////////////////////////////////////////////////////////////////////////////////////

              /*
              //writeFileLog("Going to Select SAM Slot1");

              //SelectSAM(1);

              writeFileLog("=====================================================================");
 
              ActivateContactCard(SAMAPIReply);
           
              for( counter =0 ; counter < 2; counter++)
	      {
		
		    memset(log,'\0',LOG_ARRAY_SIZE);

		    sprintf(log,"[main()] SAMAPIReply[%d] = 0x%xh.",counter,SAMAPIReply[counter]);
		
		    writeFileLog(log);

	      }	
 

              writeFileLog("=====================================================================");
              
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
		
		    writeFileLog(log);

	      }	

              writeFileLog("=====================================================================");

              int DeActivateSamAPIReply =-1;

              DeActivateContactCard(&DeActivateSamAPIReply)  ; 

              writeFileLog("=====================================================================");
         
             */

	     ///////////////////////////////////////////////////////////////////////////////////////////////////////
	     
	     //DisConnectDevice_c(0);
				  
	     //////////////////////////////////////////////////////////////////////////////////////////////////////




//}//main() end


#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////

