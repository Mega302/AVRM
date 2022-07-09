#include "DSCU.h"


#ifdef B2B_TRAP_GATE


/*
  Pc Side Command format:
  -------------------------------------------------------------
  sync   :0x02  fixed for ccnet
  adr    :0x05  peripheral adrs(DSCU unit)
  length :total number of bytes including sync and crc
  cmd    :device command
  data   :omitted if not required by cmd 
  crclsb
  crcmsb
----------------------------------------------------------------
   Response from DSCU Unit: 
   -----------------------------
   sync    :0x02 fixed for ccnet
   adr     :0x05 peripheral adrs(DSCU unit)
   length  :total number of bytes including sync and crc
   data    :Response Data
   crclsb
   crcmsb
----------------------------------------------------------------
*/

//Global Variables for Drop and Seal Unit

int g_DSCUCommportHandle=-1;

int g_ErrorCode=0;


//DSCU Unit Commands Table
static DSCUCommands g_DSCUCommands[6]=  { 0x02,0x05,0x06,0x33,0x00,0x00,   //0 Poll
                                          0x02,0x05,0x06,0x10,0x00,0x00,   //1 Reset 
                                          0x02,0x05,0x06,0x00,0x00,0x00,   //2 Ack Resoponse
                                          0x02,0x05,0x06,0x30,0x00,0x00,   //3 Identification
                                          0x02,0x05,0x06,0x32,0x00,0x00,   //4 Get Option
		                          0x02,0x05,0x06,0x34,0x00,0x00    //5 Power Recovery
                                        };
//DSCU Unit Special Commands Table
static DSCUSpclCommands g_DSCUSpclCommands[3]={ 0x02,0x05,0x08,0xF0,0x20,0x00,0x00,0x00,   //0 Close Drop Casstte
                                                0x02,0x05,0x08,0xF0,0x20,0x01,0x00,0x00,   //1 Open  Drop Casstte 
                                                0x02,0x05,0x06,0x31,0x00,0x00,0x00,0x00    //2 Set option
                                              };
//DSCU Unit State Table
static DSCUState  g_DSCUState[8]={  0x10,Powerup,      
                                    0x11,Initializing,         
                                    0x12,Idling,         
                                    0x13,Failure,
                                    0x19,Busy,      
                                    0x20,Dropping,
                                    0x21,DropOk,
                                    0x22,DropFailed
                                 };

//DSCU Unit Error State Table
static DSCUErrorState  g_DSCUErrorState[6]={  0x50,DropCassettesPlateMotorFailure,   
                                              0x51,DropCassettesPlateSensorsFailure,         
				              0x54,DropCassettesGateMotorFailure,         
				              0x55,DropCassettesGatesSensorFailure,
				              0x60,MainPowerFailure,      
				              0x62,PowerSwitchFailure
				           };

//**************************Start:Drop and Seal Unit Lower Level************************************************//

static unsigned int OpenDSCU( unsigned int PortNumber /*Pc Serial Port No ex: 0 means ttyS0 (in)*/ )
{
  //check send command to device
  if(PortNumber <0 || PortNumber >255 )
  return DSCU_FAIL; //Function Parameter Not ok error

  int CommportHandle=-1;
  
  #ifdef DEBUG
  printf("\n(OpenDSCU) >> Before Open Serial Port h_commport= %d", CommportHandle);
  #endif
  //Com Port Open Section
  GenricOpenPort(PortNumber, &CommportHandle);
  if(CommportHandle>0)
  {
      #ifdef DEBUG
      printf("\n(OpenDSCU) >> Open ComPort ttyS%d successfully with handle id %d",PortNumber, CommportHandle);
      #endif
  }
  else
  {
      #ifdef DEBUG
      printf("\n(OpenDSCU) >> Com Port Handle=%d",CommportHandle);
      printf("\n(OpenDSCU) >> Open ComPort ttyS%d failed!!!",PortNumber);
      #endif
      return DSCU_FAIL; //Error Code
  }
   
  
  //Clear Transmit and Receieve Buffer of Serial Port Section
  GenricClearReceiveBuffer (CommportHandle);
  GenricClearTransmitBuffer(CommportHandle);
  
  return CommportHandle;
} 

static unsigned int CloseDSCU( unsigned int CommportHandle /*Pc Serial Port Handle (in)*/)
{
  //Com Port Close Section
  int ret =0;
  ret=GenricClosePort(CommportHandle);
  if(DSCU_SUCCESS==ret)
  {
     #ifdef DEBUG
     printf("\n(CloseDSCU) >> Close ComPort successfully with handle id %d",CommportHandle);
     #endif
     g_DSCUCommportHandle=-1;
     return DSCU_SUCCESS;
  }
  else
  {
      #ifdef DEBUG
      printf("\n(CloseDSCU) >> Open ComPort Close failed with handle id %d!!!",CommportHandle);
      #endif
      return DSCU_FAIL;
  }

}


/*
 Functon : DSCUGetReply() API Level
 Object  : Send DSCU unit Command and Get DSCU unit reply bytes packet
           if ReplyPakt=NULL,ReplyPaktLength=0,HostWaitTime=0 then this function only send command ,no receieve packet
           read will be done this module    
*/

static unsigned int DSCUGetReply  (  unsigned int   CommportHandle,  //Pc Serial Port Handle (in)
                                     unsigned char *Command,         //Slave Device Command  (in)
                                     unsigned int   CommandLength,   //Slave Device command Length (in)
                                     unsigned char *ReplyPakt,       //Slave Dev Reply Packt buffer (out)
                                     unsigned int   ReplyPaktLength, //Slave Dev Reply Packt buffer Length (in)
                                     unsigned int   HostWaitTime     //PC wait time for slave device reply (Millisecond)(in)
                                  )
{
  bool GoForReadReply=false;
  //Safety Checking Parameters
  //check send command to device
  if(CommportHandle <0 || NULL ==Command || CommandLength <0 || g_DSCUCommportHandle <0)
  return DSCU_FAIL; //Function Parameter Not ok error

  //Check for device reply option 
  if(ReplyPaktLength>0 && NULL!=ReplyPakt && HostWaitTime >0)
  GoForReadReply=true;
  
  //Variable Declaration Section
  int ret=0,totalByteIn=0;
  int recvedBytelength=-1;
  int recvingbytelength=ReplyPaktLength;
  int HostDelay=HostWaitTime;
  unsigned char rbyte=0;
  int counter=0;
  int RecvbyteIndex=CommandLength,SendByteLength=0;
  bool SendByteAck[SendByteLength],CheckSendByteflag=1;
  memset(SendByteAck,0,SendByteLength);

  //Clear Transmit and Receieve Buffer of Serial Port Section
  GenricClearReceiveBuffer (CommportHandle);
  GenricClearTransmitBuffer(CommportHandle);

  #ifdef DEBUG
  printf("\n*************(GenDevGetReply)Start:Send Data*****************");
  for(counter=0;counter<CommandLength;counter++)
  printf("\n(DSCUGetReply) >>Sending Data[%d]=0x%X",counter,Command[counter]);
  printf("\n*************(GenDevGetReply)End:Send Data*****************\n");
  #endif
 
  SendByteLength=CommandLength-1;
  //Send Data to Serial Port
  for(counter=0;counter<CommandLength;counter++)
  SendByteAck[counter]=GenricSendSingleByteToSerialPort(CommportHandle,Command[counter]);
  
  //Check all bytes send successfully or error happen during sending bytes
  for(counter=0;counter<CommandLength;counter++)
  CheckSendByteflag  = CheckSendByteflag & SendByteAck[counter]; 
  if(1!=CheckSendByteflag)
  {
    #ifdef DEBUG
    printf("\n(DSCUGetReply) >> Sending Bytes to DSCU failed \n");
    #endif
    return DSCU_FAIL;
  }
  
  //Collect Reply data from Device
  if(true==GoForReadReply)
  recvedBytelength=DSCURecvData(CommportHandle,ReplyPakt,recvingbytelength,HostDelay);

  if((recvedBytelength<0 || recvedBytelength==0) && ReplyPaktLength>0)
  {
    #ifdef DEBUG
    printf("\n(DSCUGetReply) >> No Byte Recv From DSCU Unit \n");
    #endif
    return DSCU_FAIL; //Error Code
  }

  #ifdef DEBUG
  if(true==GoForReadReply)
  {
     printf("\n***********(GenDevGetReply)Start:Reply Packet******************");
     for(counter=0;counter<recvingbytelength;counter++)
     printf("\n(DSCUGetReply) >>Received Data[%d]=0x%X",counter,ReplyPakt[counter]);
     printf("\n***********(GenDevGetReply)End:Reply Packet*******************\n");
    
  }
  #endif
  return DSCU_SUCCESS;

}

//GenDevGetReply end

/*

 Functon : DSCURecvData()
 Object  : Get Generic Device Reply Bytes Packet

*/

static int DSCURecvData   (   unsigned int   h_commport,        //Com Port Handle  (in)
                              unsigned char *recvbyte,          //Recv Byte Array  (out)
                              unsigned int   recvingbytelength, //Recv Byte Length (in)
                              unsigned int   HostDelay          //Host Delay For Slave Reply (in) (Millisecond)
                          )
{
  /* To get 1 sec follow this step
     get value return from clock() and divide that value with CLOCKS_PER_SEC,if it is 1
     then time elapsed is 1 sec
     cpu total timetick is 1000000 in 1 Sec means 1000 ms gives 1000000 ticks
     so 1ms gives ticks=1000000/1000=1000ticks
     formaula to get desired delay in second:
     if you require x second delay then multiply with CLOCKS_PER_SEC to get total cpu timer
     ticks in x second
 */
  int rtcode=-1;  
  int TicksPerMs=1000; //Cpu gives 1000 timer ticks in 1ms
  //Safety Checking Parameters
  if( h_commport < 0 || NULL==recvbyte || recvingbytelength < 0 )
  return 0; //Function Parameter Not ok error

  int totalByteIn=0,RecvCounter=0,recvedbytelength=0;
  unsigned char rbyte='\0';
  //clock_t  st, ed;
  unsigned long long int st, ed;
  //Start Time
  st = clock();

  #ifdef DEBUG
  //printf("\n(GenDevRecvData) >> Start Time: %llu",st);
  #endif
  
  //HostDelay in MilliSecond
  ed = st +(HostDelay * 1000); //Calculation given hostdelay in Millisecond
  #ifdef DEBUG
  //printf("\n(GenDevRecvData) >> End Time: %llu",ed);
  #endif

  while(1)
  {
   rtcode=GenricReceiveSingleByteFromSerialPort(h_commport,&rbyte,&totalByteIn);
   if( totalByteIn == 1)
   {
      #ifdef DEBUG
      printf("\n(GenDevRecvData) >> receive byte=0x%X totalByteIn=%d",rbyte,totalByteIn);
      #endif
      if(RecvCounter < recvingbytelength)
      {
         recvbyte[RecvCounter]=rbyte;
         RecvCounter++;
         recvedbytelength++;
      }
   }
   //if all bytes recv completed then exit
   if(recvedbytelength==recvingbytelength)
   break;
   //if time completed then exit
   if(clock() >=  ed)
   break;
   //if time not completed then continue
   if (clock() <  ed)
   {
      continue;
   }
  }

  return recvedbytelength;
}


//CRC-CCITT [ P(x)=x^16+x^12+x^5+1 ] Calculation with give CRCLSB and CRCMSB

unsigned int DSCUGetCRC16( unsigned char *bufData,  //Bytes which crc value will be calculte (in) 
                           unsigned int   sizeData, //Bytes array length which crc value will be calculte (in)
                           unsigned char *CRCLSB,   //return CRC LSB (out)
                           unsigned char *CRCMSB    //return CRC MSB (out)
                         )
{

  unsigned int CRC=0, i;
  unsigned char j;
  unsigned int constant = 256;
  for(i=0; i < sizeData; i++)
  {	 
    CRC ^= bufData[i];
    for(j=0; j < 8; j++)
    {
         if(CRC & 0x0001) 
	 {
	    CRC >>= 1;
	    CRC ^= DSCU_POLYNOMIAL;
	 }
         else 
            CRC >>= 1;
	} 
     } 
    
   //Separate LSB and MSB
   *CRCMSB = CRC/constant;
   *CRCLSB = CRC-(*CRCLSB  * 256);
    return CRC;
}


//Send Command from PC to Drop and Seal Unit
static unsigned int DSCUSendCommand(unsigned char *Command,         //DSCU Command (in)
                                    unsigned int   CommandLength,   //DSCU Command Length (in)
                                    unsigned char *ReplyPakt,       //DSCU Reply packet return (out)
                                    unsigned int   ReplyPaktLength, //DSCU Reply packet length (in)
                                    unsigned int   HostWaitTime     //Pc Wait time for DSCU Reply in millisecond (in)
                                  )
{
  bool ReplyByteCRCChekflag=false;
  int PortNo=-1;
  unsigned int rtcode=0;
  if(NULL== Command || 0 >= CommandLength || g_DSCUCommportHandle < 0)
  {
     #ifdef DEBUG
     printf("\n(DSCUSendCommand) >> Function parameter not ok\n");
     #endif
     return DSCU_FAIL; //Function parameter not ok
  }

  if(NULL!=ReplyPakt && 0 < ReplyPaktLength && 0 < HostWaitTime)
  ReplyByteCRCChekflag=true;
  

  //Get CRC LSB and MSB of command
  unsigned char CRCLSB='\0';
  unsigned char CRCMSB='\0';
  unsigned int CRC=DSCUGetCRC16( Command,                   //Command (in)
                                 CommandLength-2,           //Cmnd Packt Length without crc 2bytes (in)
                                 &CRCLSB,                   //CRC LSB BYTE return  (out)  
                                 &CRCMSB                    //CRC MSB BYTE return  (out)
                               );
  
  Command[CommandLength-2]= CRCLSB; 
  Command[CommandLength-1]= CRCMSB;
  
  //Send  Command
  rtcode=DSCUGetReply  (g_DSCUCommportHandle,             //Serial Port Handle (in)
                        Command,                      //Comamnd (in)
                        Command[2],                   //Command length (in) 
                        ReplyPakt,                    //Reply Packt (out)
                        ReplyPaktLength,              //Slave Retuen Expected Byte (in)
                        HostWaitTime                  //Host Wait Time for DSCU Unit(in Millisecond) (in)
                       );
 
 //Check Command Reply Bytes CRC        
 if(true==ReplyByteCRCChekflag && DSCU_SUCCESS == rtcode)
 {   
    //Check CRC of reply bytes
    rtcode=DSCUReplyCRCCheck(ReplyPakt,ReplyPakt[(unsigned int)ReplyPaktLengthIndex]);
    if(rtcode==DSCU_SUCCESS)
    {
      #ifdef DEBUG
      printf("\n(DSCUSendCommand) >> Reply Pakt crc ok\n");
      #endif
      return DSCU_SUCCESS;
    }
    else
    {
      #ifdef DEBUG
      printf("\n(DSCUSendCommand) >> Reply Pakt crc not ok\n");
      #endif 
      return DSCU_FAIL;
    }
 }
 
 else if(DSCU_SUCCESS == rtcode)
 return DSCU_SUCCESS;
 
 else
 return DSCU_FAIL;

}


//Check Reply Packet CRC
static unsigned int DSCUReplyCRCCheck(unsigned char *ReplyPackt,     //bytearray which crc value will be calculate (in)
                                      unsigned int   ReplyByteLength //bytearray legth which crc value will be calculate (in)
                                    )
{
  if(NULL==ReplyPackt || 0 >= ReplyByteLength)
  return DSCU_FAIL;
  
  unsigned char  CRCLSB='\0';
  unsigned char  CRCMSB='\0';
  unsigned int sizeData=ReplyByteLength-2;
  DSCUGetCRC16(ReplyPackt, 
               sizeData,
               &CRCLSB,
               &CRCMSB
              );

  #ifdef DEBUG
  printf("\n (DSCUReplyCRCCheck)>> 0x%x 0x%x",CRCLSB,CRCMSB);
  printf("\n (DSCUReplyCRCCheck)>> 0x%x 0x%x",ReplyPackt[ReplyByteLength-2],ReplyPackt[ReplyByteLength-1]);
  #endif

  if( (ReplyPackt[ReplyByteLength-1]==CRCMSB) && (ReplyPackt[ReplyByteLength-2]==CRCLSB) )
  return DSCU_SUCCESS;
  else
  return DSCU_FAIL;

}


//Analysis DSCU Poll Reply Bytes
/*
    DSCU Poll Sensor Status:
    ------------------------------------------------ 
    Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0
     x     x    x      x      x    x     x     x
    ------------------------------------------------
    Bit 0 =Bag Clamp opened
    Bit 1 =Sealer in working position
    Bit 2 =Sealer in home position
    Bit 3 =Bag frane opened
    Bit 4 =Bag frane closed
    Bit 5 =Drop cassette gate opened
    Bit 6 =Drop cassette gate closed
    Bit 7 =Service mode
*/
static DSCUPollReplyState  DSCUPollReplyPktAnalysis(unsigned char *ReplyPackt,          //DSCU Reply packet (in)
                                                    unsigned int   ReplyStateByteIndex  //DSCU Reply State bytes index(in)
                                                   )
{
    DSCUPollReplyState DSCUPollReplyStateVar;
    
    DSCUPollReplyStateVar.CurrentStatus=-1;
    DSCUPollReplyStateVar.CurrentErrorStatus=-1;
    
    DSCUPollReplyStateVar.ServiceMode=0;
    DSCUPollReplyStateVar.DropCastGateClose=0;
    DSCUPollReplyStateVar.DropCastGateOpen=0;
    DSCUPollReplyStateVar.BagClampOpen=0;
    DSCUPollReplyStateVar.DropCassettesPlateMotorFailure=0;
    DSCUPollReplyStateVar.DropCassettesPlateSensorsFailure=0;
    DSCUPollReplyStateVar.DropCassettesGateMotorFailure=0;
    DSCUPollReplyStateVar.DropCassettesGatesSensorFailure=0;
    DSCUPollReplyStateVar.MainPowerFailure=0;
    DSCUPollReplyStateVar.PowerSwitchFailure=0;

    if(NULL==ReplyPackt || 0 >= ReplyStateByteIndex )
    return DSCUPollReplyStateVar;

    int Counter=0;
    
    for(;Counter< 8;Counter++)
    {
       #ifdef DEBUG
       printf("\n\n(DSCUPollReplyPktAnalysis) >> ReplyPackt State=0x%x Enum State=0x%x",ReplyPackt[ReplyStateByteIndex],g_DSCUState[Counter].StateHexValue);
       #endif 
       if(ReplyPackt[ReplyStateByteIndex]==g_DSCUState[Counter].StateHexValue)
       {
          DSCUPollReplyStateVar.CurrentStatus=g_DSCUState[Counter].StateValue;
          if(Failure!=DSCUPollReplyStateVar.CurrentStatus)
          {
            //Mask=0b10000000
            DSCUPollReplyStateVar.ServiceMode=((0x80 & ReplyPackt[ReplyStateByteIndex+1])>>7);
            //Mask=0b01000000
            DSCUPollReplyStateVar.DropCastGateClose=((0x40 & ReplyPackt[ReplyStateByteIndex+1])>>6);
            //Mask=0b00100000
            DSCUPollReplyStateVar.DropCastGateOpen=((0x20 & ReplyPackt[ReplyStateByteIndex+1])>>5);
            //Mask=0b00000001
            DSCUPollReplyStateVar.BagClampOpen=(0x01 & ReplyPackt[ReplyStateByteIndex+1]);
          }
          break;
          
       }
       
     }//First For End

     //Determine current error status details 
     if(DSCUPollReplyStateVar.CurrentStatus==Failure) //Error State 0x13
     {
         for(Counter=0;Counter< 8;Counter++)
         {
              #ifdef DEBUG
              printf("\n\n(DSCUPollReplyPktAnalysis) >> ReplyPackt State=0x%x Enum State=0x%x",ReplyPackt[ReplyStateByteIndex+1],g_DSCUErrorState[Counter].ErrorStateHexValue);
              #endif
              if(ReplyPackt[ReplyStateByteIndex+1]==g_DSCUErrorState[Counter].ErrorStateHexValue)
              {
                 DSCUPollReplyStateVar.CurrentErrorStatus=g_DSCUErrorState[Counter].ErrorStateValue;
                 switch(g_DSCUErrorState[Counter].ErrorStateHexValue)
                 {
                    case 0x50:DSCUPollReplyStateVar.DropCassettesPlateMotorFailure=1;
                              break;
                    case 0x51:DSCUPollReplyStateVar.DropCassettesPlateSensorsFailure=1;
                              break;
                    case 0x54:DSCUPollReplyStateVar.DropCassettesGateMotorFailure=1;
                              break;
                    case 0x55:DSCUPollReplyStateVar.DropCassettesGatesSensorFailure=1;
                              break;
                    case 0x60:DSCUPollReplyStateVar.MainPowerFailure=1;
                              break;
                    case 0x62:DSCUPollReplyStateVar.PowerSwitchFailure=1;
                              break;
                 };

                 break;
              }
       }
              
     }
     
    #ifdef DEBUG
    printf("\n\n(DSCUPollReplyPktAnalysis)>> CurrentState=%d",DSCUPollReplyStateVar.CurrentStatus);
    printf("\n\n(DSCUPollReplyPktAnalysis)>> Current Error State=%d\n",DSCUPollReplyStateVar.CurrentErrorStatus);
    #endif
    
    return DSCUPollReplyStateVar;
}


static unsigned int DSCUAckReplyPktAnalysis(unsigned char *ReplyAcKPackt/*ack bytearray analysis (in)*/)
{
   int Counter=0;
   for(;Counter<4;Counter++)
   {
     if(g_DSCUCommands[2].Command[Counter]==ReplyAcKPackt[Counter])
     continue;
     else
     return DSCU_FAIL;
   }
   return DSCU_SUCCESS;

}

/*
static void Delay(unsigned int MilliSeconds/*User given time for delay (in))
{
   
   unsigned long long int  st, ed;
   
   //Start Time
   st = clock();
   #ifdef DEBUG
   //printf("\n(Delay) >> Starting CpuTimerTicks=%lld\n",st);
   #endif
   
   //HostDelay in MilliSecond
   ed = st +( (MilliSeconds * CLOCKS_PER_SEC) / 1000); //Calculation given hostdelay in Millisecond
   
   #ifdef DEBUG
   //printf("\n(Delay) >> Ending CpuTimerTicks=%lld\n",ed);
   #endif

   while(1)
   {
     #ifdef DEBUG
     //printf("\n(Delay) >> Current CpuTimerTicks=%ld\n",clock());
     #endif
     
     //if time completed then exit
     if(clock() >=  ed)
     break;
     
     //if time not completed then continue
     if (clock() <  ed)
     continue;
  }


}
*/


static void Delay(unsigned int MilliSeconds /*User given time for delay (in)*/)
{
        struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = MilliSeconds * 1000000L;
        nanosleep(&req, (struct timespec *)NULL);

}


//**************************End:Drop and Seal Unit Lower Level************************************************//



//**************************Start:Drop and Seal Unit Middle Level************************************************//

//Send Ack Command
static unsigned int  DSCUSendAck()
{
  int rtcode=-1;
  rtcode=  DSCUSendCommand( g_DSCUCommands[2].Command,    //Ack Command       (in)
                            g_DSCUCommands[2].Command[2], //Ack Command Length (in)
                            NULL,                         //No Reply Bytes Wants to recv (out)
                            0,                            //No reply bytes length (in)
                            0                             //Host doesnot wants to read recv bytes (in)
                          );
    if(rtcode==DSCU_SUCCESS)
    return DSCU_SUCCESS;
    else
    return DSCU_FAIL;

}


//Send Poll Command
static unsigned int  DSCUSendPoll(unsigned char *PollReplyPackt,unsigned int PollReplyPacktLength,int HostWaitTime)
{
  int rtcode=-1;
  rtcode=  DSCUSendCommand( g_DSCUCommands[0].Command,     //Poll Command (in)
                            g_DSCUCommands[0].Command[2],  //Poll Command Length (in)
                            PollReplyPackt,                //Reply Bytes Array  (out)
                            PollReplyPacktLength,          //Reply bytes length (in)
                            HostWaitTime                   //Host delaytime for poll command reply bytes (in)
                          );
    if(rtcode==DSCU_SUCCESS)
    {
       rtcode=DSCUSendAck();
       if(rtcode==DSCU_SUCCESS)
       {   
         #ifdef DEBUG
         printf("\n (DSCUSendPoll) Poll Successs with send ack status");
         #endif
         return DSCU_SUCCESS;
       }
       else 
       {
         #ifdef DEBUG
         printf("\n (DSCUSendPoll) Poll failed with failed send ack status"); 
         #endif
         return DSCU_FAIL;
       }
    }
    
    else
    {
         #ifdef DEBUG
         printf("\n (DSCUSendPoll) Poll failed No reply from DSCU Unit");
         #endif
         return DSCU_FAIL;
    }
}


//Send Reset Command
static unsigned int  DSCUSendReset(int HostWaitTime)
{
  int rtcode=-1;
  unsigned char ResetReplyPackt[20]={'\0'}; //Slave return Ack Status
  unsigned int  ResetReplyPacktLength=6;    
  rtcode=  DSCUSendCommand( g_DSCUCommands[1].Command,      //ReSet Command (in)
                            g_DSCUCommands[1].Command[2],   //ReSet Command Length (in)
                            ResetReplyPackt,                //Reply Bytes Array (out)
                            ResetReplyPacktLength,          //Reply bytes length (in)
                            HostWaitTime                    //Host delaytime for Reset command reply bytes (in)
                          );
    if(rtcode==DSCU_SUCCESS)
    {
     rtcode=DSCUAckReplyPktAnalysis(ResetReplyPackt);
     if(rtcode==DSCU_SUCCESS)
        return DSCU_SUCCESS;
     else
        return DSCU_FAIL;
    }
    else
    return DSCU_FAIL;

}

//Send Reset Command
static unsigned int  DSCUSendIdentification(unsigned char *IdentificationReplyPackt,//Identification command reply packet(out)
                                            unsigned int   IdentificationReplyPacktLength,//Identification command reply packet legth (in)
                                            int            HostWaitTime //Host delaytime for Identification command (in)
                                           )
{

  int rtcode=-1;
  rtcode=  DSCUSendCommand( g_DSCUCommands[3].Command,               //Identification Command (in)
                            g_DSCUCommands[3].Command[2],            //Identification Command Length (in)
                            IdentificationReplyPackt,                //Identification Reply Bytes Array (out)
                            IdentificationReplyPacktLength,          //Identification Reply bytes length (in)
                            HostWaitTime                             //Host delaytime for Reset command reply bytes (in)
                          );
    if(rtcode==DSCU_SUCCESS)
    {
     rtcode=DSCUSendAck();
     if(rtcode==DSCU_SUCCESS)
        return DSCU_SUCCESS;
     else
        return DSCU_FAIL;
    }
    else
    return DSCU_FAIL;

}

//Send Option (Not tested)
static unsigned int  DSCUSendSetoption(unsigned char *OptionByte, //Set option byte for setoption command(in)
                                       int HostWaitTime           //host delay for set option command (in)
                                      )
{
  
  int rtcode=-1;
  //Set System in Single or Dual Mode
  g_DSCUSpclCommands[2].Command[4]=*OptionByte;
  //reply bytes array
  unsigned char SetOptionReplyPackt[20]={'\0'};
  //reply bytes length
  unsigned int  SetOptionReplyPacktLength=6;
  
  rtcode=  DSCUSendCommand( g_DSCUSpclCommands[2].Command,     //Set Option Command (in)
                            g_DSCUSpclCommands[2].Command[2],  //Set Option Command Length (in)
                            SetOptionReplyPackt,               //Set Option Reply Bytes Array (out)
                            SetOptionReplyPacktLength,         //Set Option Reply bytes length (in)
                            HostWaitTime                       //Host delaytime for Set Option command reply bytes (in)
                          );
    if(rtcode==DSCU_SUCCESS)
    {
     rtcode=DSCUAckReplyPktAnalysis(SetOptionReplyPackt);
     if(rtcode==DSCU_SUCCESS)
        return DSCU_SUCCESS;
     else
        return DSCU_FAIL;
    }
    else
    return DSCU_FAIL;
}


//Send Drop Open Command
static unsigned int  DSCUSendDropOpen(int HostWaitTime)
{
  int rtcode=-1;
  unsigned char DropOpenReplyPackt[20]={'\0'}; //Slave return Ack Status
  unsigned int  DropOpenReplyPacktLength=6;
  rtcode=  DSCUSendCommand( g_DSCUSpclCommands[1].Command,         //Drop Open Command (in)
                            g_DSCUSpclCommands[1].Command[2],      //Drop Open Command Length (in)
                            DropOpenReplyPackt,                    //Reply Bytes Array (out)
                            DropOpenReplyPacktLength,              //Reply bytes length (in)
                            HostWaitTime                           //Host delaytime for Drop open command reply bytes (in)
                          );
   if(rtcode==DSCU_SUCCESS)
    {
     rtcode=DSCUAckReplyPktAnalysis(DropOpenReplyPackt);
     if(rtcode==DSCU_SUCCESS)
        return DSCU_SUCCESS;
     else
        return DSCU_FAIL;
    }
    else
    return DSCU_FAIL;

}

//Send Drop Close Command
static unsigned int  DSCUSendDropClose(int HostWaitTime)
{
  int rtcode=-1;
  unsigned char DropCloseReplyPackt[20]={'\0'}; //Slave return Ack Status
  unsigned int  DropCloseReplyPacktLength=6;
  rtcode=  DSCUSendCommand( g_DSCUSpclCommands[0].Command,          //Drop Close Command (in)
                            g_DSCUSpclCommands[0].Command[2],       //Drop Close Command Length (in)
                            DropCloseReplyPackt,                    //Reply Bytes Array (out)
                            DropCloseReplyPacktLength,              //Reply bytes length (in)
                            HostWaitTime                            //Host delaytime for Drop Close command reply bytes(in)
                          );
    if(rtcode==DSCU_SUCCESS)
    {
     rtcode=DSCUAckReplyPktAnalysis(DropCloseReplyPackt);
     if(rtcode==DSCU_SUCCESS)
        return DSCU_SUCCESS;
     else
        return DSCU_FAIL;
    }
    else
    return DSCU_FAIL;

}


//**************************End:Drop and Seal Unit Middle Level************************************************//


//**************************Start:Drop and Seal Unit Upper Level************************************************//

/*
  -1= if Device is not send any state report
   x = Device current status (x=0-8)

*/

static int GetCurrentDSCUState(void)
{
  unsigned char PollReplyPackt[30]={'\0'};
  unsigned int  PollReplyPacktLength=7;
  int HostWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
  unsigned int rtcode=0; 
  rtcode=DSCUSendPoll(PollReplyPackt,PollReplyPacktLength,HostWaitTime);
  if(DSCU_SUCCESS==rtcode)
  {
    DSCUPollReplyState CurrentStatevar;
    CurrentStatevar=DSCUPollReplyPktAnalysis(PollReplyPackt,ReplyPaktStateIndex);
    if(-1!=CurrentStatevar.CurrentStatus)
    {  
       #ifdef DEBUG
       printf("\n (GetCurrentDSCUStatus) >> Current Status=%d",CurrentStatevar.CurrentStatus);
       #endif
       if(Failure==CurrentStatevar.CurrentStatus)
       {
         #ifdef DEBUG
         printf("\n (GetCurrentDSCUStatus) >> CurrentErrorStatus=%d",CurrentStatevar.CurrentErrorStatus);
         printf("\n (GetCurrentDSCUStatus) >> DropCassettesPlateMotorFailure=%d",CurrentStatevar.DropCassettesPlateMotorFailure);
         printf("\n (GetCurrentDSCUStatus) >> DropCassettesPlateSensorsFailure=%d",CurrentStatevar.DropCassettesPlateSensorsFailure);
         printf("\n (GetCurrentDSCUStatus) >> DropCassettesGateMotorFailure=%d",CurrentStatevar.DropCassettesGateMotorFailure);
         printf("\n (GetCurrentDSCUStatus) >> DropCassettesGatesSensorFailure=%d",CurrentStatevar.DropCassettesGatesSensorFailure);
         printf("\n (GetCurrentDSCUStatus) >> MainPowerFailure=%d",CurrentStatevar.MainPowerFailure);
         printf("\n (GetCurrentDSCUStatus) >> PowerSwitchFailure=%d",CurrentStatevar.PowerSwitchFailure);
         #endif
       }
       else
       {
       #ifdef DEBUG
       printf("\n (GetCurrentDSCUStatus) >> BagClampOpen=%d",CurrentStatevar.BagClampOpen);
       printf("\n (GetCurrentDSCUStatus) >> DropCastGateOpen=%d",CurrentStatevar.DropCastGateOpen);
       printf("\n (GetCurrentDSCUStatus) >> DropCastGateClose=%d",CurrentStatevar.DropCastGateClose);
       printf("\n (GetCurrentDSCUStatus) >> ServiceMode=%d",CurrentStatevar.ServiceMode);
       #endif
       }
       
       return CurrentStatevar.CurrentStatus;
       
    }
    else
    return -1;
  }
  else
  return -1;

}


/*
  1 = Device in idle state
  2 = Device in failure state
  3 = Device not replied against poll commands
*/
static int IsDSCUInIdle(void)
{
    int Status=-1;
    Status=GetCurrentDSCUState();
    if(Idling==Status)
    return 1;
    else if(Failure==Status)
    return 2;
    else if (-1 == Status)
    return 3;
}


/*
  1= Device is present in system (Device replied with ack status against poll command)
  0= Device is not present in system (Device not replied with ack status against poll command)
*/

static int GetDSCUStatus(void)
{
  unsigned char PollReplyPackt[30]={'\0'};
  unsigned int  PollReplyPacktLength=7;
  int HostWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
  unsigned int rtcode=0; 
  rtcode=DSCUSendPoll(PollReplyPackt,PollReplyPacktLength,HostWaitTime);
  if(DSCU_SUCCESS==rtcode)
  return 1;
  else
  return 0;

}


/*
  0=DropCastGateClose
  1=DropCastGateOpen
  2=Device in failure state
  3=Invalid command returned by DSCU Unit
*/
static int GetDSCUDropGateStatus(void)
{
  unsigned char PollReplyPackt[30]={'\0'};
  unsigned int  PollReplyPacktLength=7;
  int HostWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
  unsigned int rtcode=0; 
  rtcode=DSCUSendPoll(PollReplyPackt,PollReplyPacktLength,HostWaitTime);
  if(DSCU_SUCCESS==rtcode)
  {
    DSCUPollReplyState CurrentStatevar;
    CurrentStatevar=DSCUPollReplyPktAnalysis(PollReplyPackt,ReplyPaktStateIndex);
    if(-1!=CurrentStatevar.CurrentStatus)
    {
       if(1==CurrentStatevar.DropCastGateOpen)
       return 1;
       else if(1==CurrentStatevar.DropCastGateClose)
       return 0;
   }
   else if(Failure==CurrentStatevar.CurrentStatus)
   return 2;
  }
  else
  return 3;
}

static int DSCUPolling(void)
{
     unsigned char  PollReplyPackt[20]={'\0'};
     unsigned int   PollReplyPacktLength=7;
     unsigned int   HostWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
     int rtcode=-1;
     //Open port for DSCU operate
     //printf("\n (DSCUPolling)before OpenDropandSealUnit Handle id =%d\n",g_DSCUCommportHandle);
     //OpenDropandSealUnit(GetDSCUPortNo());
     //printf("\n (DSCUPolling)After OpenDropandSealUnit Handle id =%d\n", g_DSCUCommportHandle);
     if(-1==g_DSCUCommportHandle)
     return 0;
     while(1)
     {
         rtcode=DSCUSendPoll(PollReplyPackt, PollReplyPacktLength,DSCU_WAIT_TIME_MS);
         //printf("\nDSCUPolling status code=%d\n",rtcode);
         if(rtcode==1)
         printf("\nDSCU unit is successfully reply against poll command");
         else
         printf("\nDSCU unit is not reply against poll command");
         memset(PollReplyPackt,'\0',20);
         Delay(DSCU_POLL_INTERVAL_DELAY);
         rtcode=-1;
     }
    
     //CloseDropandSealUnit();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                            Start:Drop Gate Open/Close Function
//                     
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
  //Normal Return Code
  0=Device drop gate open/close fail
  1=Device drop gate open/close success
  //Error Return Code
  2=Device in failure state
  3=Device not respond against poll command
  4=Device not in idle state so no drop command can be issue
  5=Device send invalid command against drop command
  6=Drop Gate open
  7=Drop Gate closed
  8=Device not presents in system

*/

static int SetDSCUDropGateOperation(DropGate Operation)
{
  int DropOperationWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
  unsigned int rtcode=0;
  
  rtcode=CheckDSCUBeforeIssueDropCommand(Operation);
  if(DSCU_SUCCESS!=rtcode)
  {
    return rtcode; 
  }

  #ifdef POLL_DELAY 
  Delay(DSCU_POLL_INTERVAL_DELAY);
  #endif
  
  //State 4: 
  if(Operation==DropGateOpen)
  rtcode=DSCUSendDropOpen(DropOperationWaitTime);
  else if(Operation==DropGateClose)
  rtcode=DSCUSendDropClose(DropOperationWaitTime);

  #ifdef DEBUG
  printf("\n(SetDSCUDropOpen) DSCUSendDropOpen rtcode=%d",rtcode);
  #endif
  //State 5: drop open command successfully ack by DSCU controller,Check its Current Status by make poll command
  if(rtcode==DSCU_SUCCESS)
  {
     //Delay for issue next command 
     #ifdef POLL_DELAY 
     Delay(DSCU_POLL_INTERVAL_DELAY);
     #endif
     return (ContinuousPollandGetDropStatus(Operation));

  } //if(rtcode==DSCU_SUCCESS) end here
  
  //State 6: drop open command not ack by DSCU controller,return error 
  else if(rtcode== DSCU_FAIL)
  return 5;  //Device send invalid command against poll command
                
} //SetDSCUDropOpen end here



/*
  2=Device in failure state
  4=Device not in idle state so no drop command can be issue
  6=Drop Gate opened
  7=Drop Gate closed
  8=Device not presents in system
*/

//Check DSCU Staus before issue any drop open or close command
static int CheckDSCUBeforeIssueDropCommand(DropGate DropGateType)
{
  int rtcode=-1;
  //State 1 :Check DSCU present in system or not
  rtcode=GetDSCUStatus();
  //State 3: if DSCU nt present return error code
  if(0==rtcode)
  {
     #ifdef DEBUG
     printf("\n (CheckDSCUBeforeIssueDropCommand) >> Device is not present in system return code=%d\n",rtcode);
     #endif
     return 8; //Device not presents in system
  }
  
  //Delay for issue next command  
  #ifdef POLL_DELAY
  Delay(DSCU_POLL_INTERVAL_DELAY);
  #endif
 
  //State 2: Check DSCU in idle or not
  rtcode=IsDSCUInIdle();
  //State 4: if nt DSCU idle return error  
  if(0==rtcode)
  {
     #ifdef DEBUG
     printf("\n (CheckDSCUBeforeIssueDropCommand) >> Device is not idle IsDSCUInIdle() return code=%d\n",rtcode);
     #endif
     return 4; //Device not in idle state so no drop command can be issue
  }
  
  //Delay for issue next command 
  #ifdef POLL_DELAY 
  Delay(DSCU_POLL_INTERVAL_DELAY);
  #endif

  //State 3:Get Current Drop Gate Status
  rtcode=GetDSCUDropGateStatus();  //return code 0=DropCastGateClose 1=DropCastGateOpen
   
  //State 5: check drop gate status depending upon enum variable value
  if(DropGateOpen==DropGateType)
  {
     if(1==rtcode) //DropCastGateOpen already open
     {
       #ifdef DEBUG
       printf("\n (CheckDSCUBeforeIssueDropCommand) >> Device drop gate alreday open return code=%d\n",rtcode);
       #endif
       return 6; //Drop Gate in open condition
     }
  }
  if(DropGateClose==DropGateType)
   {
     if(0==rtcode) //DropCastGateClose alreday closed
     {
       #ifdef DEBUG
       printf("\n (CheckDSCUBeforeIssueDropCommand) >> Device drop gate alreday closed return code=%d\n",rtcode);
       #endif
       return 7; //Drop Gate in close condition
     }
  }
  
  return 1; //drop command can be issue
}


static int ContinuousPollandGetDropStatus(DropGate DropGateTypeVar)
{
       unsigned int DoorStatus=-1,PollStatusFlag=-1; 
       unsigned char PollReplyPackt[20];
       memset(PollReplyPackt,'\0',20);
       unsigned int PollReplyPacktLength=7;
       unsigned int PollHostWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
       bool PollResponseFlag=true;
       unsigned long long st, ed;

       //Start Time
       st = clock();
       #ifdef DEBUG
       //printf("\n(ContinuousPollandGetDropStatus) Poll Stop time :%lld", st);
       #endif
       //wait for drop ok ack from dscu (in MilliSecond)
       ed = st +( 1000 * DSCU_WAIT_TIME_MS ); //Calculation given hostdelay in MilliSecond
       #ifdef DEBUG
       //printf("\n(ContinuousPollandGetDropStatus) Poll Stop time :%lld", ed);
       #endif
       while(1)
       {
             //State 8:if time completed or no reponse from DSCU then exit
             if(clock() >= ed)
             {
               #ifdef DEBUG
               //printf("\n(ContinuousPollandGetDropStatus) Time completed");
               #endif
               if(false == PollResponseFlag)
               return 3; //Device not respond against poll command
               break;
             }
             
             //Do continuous Poll to check door gate status
             
             //Delay for issue next command 
             #ifdef POLL_DELAY  
             Delay(DSCU_POLL_INTERVAL_DELAY);
             #endif
              
             PollStatusFlag=ContinuousPollStatus(PollReplyPackt,PollReplyPacktLength,PollHostWaitTime);
             #ifdef DEBUG
             printf("\n(ContinuousPollandGetDropStatus) PollStatusFlag=%d\n",PollStatusFlag);
             #endif
             if(DSCU_SUCCESS==PollStatusFlag)
             {
                PollResponseFlag=true; //Device poll success 
                DoorStatus=GetDoorStatusDuringPoll(PollReplyPackt,(unsigned int)ReplyPaktStateIndex,DropGateTypeVar);
                #ifdef DEBUG
                printf("\n(ContinuousPollandGetDropStatus) DoorStatus=%d\n",DoorStatus);
                #endif
                if(4 == DoorStatus || 5 == DoorStatus) //DSCU internal operation processing
                continue;
                else if(-1==DoorStatus) //-1 means undefined door gate status
                continue;
                else 
                return DoorStatus;  //return drop gate status
             }
             else
             {
                PollResponseFlag=false; //Device poll failed
             }                        
             
      }//while end here

}

static int ContinuousPollStatus(unsigned char *PollReplyPackt,unsigned PollReplyPacktLength,unsigned char PollHostWaitTime)
{
   int rtcode=-1;
    
   //Clear PollReply Packet
   memset(PollReplyPackt,'\0',PollReplyPacktLength);
   
   //State 5: issue Poll command
   rtcode=DSCUSendPoll(PollReplyPackt,        //Poll reply packet (out)
                       PollReplyPacktLength,  //Poll Reply Packet expected bytes length (in)
                       PollHostWaitTime       //Pc wait time for DSCU reply (MilliSecon) (in)
                      );
   #ifdef DEBUG
   //printf("\n(ContinuousPollStatus) Poll rtcode=%d",rtcode);
   #endif     
   return rtcode;
}


static int GetDoorStatusDuringPoll(unsigned char *PollReplyPackt,unsigned int ReplyPaktStateIndex,DropGate DropGateTypeVar )
{
    DSCUPollReplyState CurrentStatevar;
   //State 6: if poll success analyse poll reply packet
   CurrentStatevar=DSCUPollReplyPktAnalysis(PollReplyPackt,      //PollReplypacket Bytes return from DSCU Unit(out)
                                            ReplyPaktStateIndex  //Pollreplypacket State bytes index no in arra(in)
                                           );
   //State 9: if poll successed then analyse current gate open status and return status
   if(-1!=CurrentStatevar.CurrentStatus)
   {  
       #ifdef DEBUG
       printf("\n (GetDoorStatusDuringPoll) >> CurrentStatus=%d\n",CurrentStatevar.CurrentStatus);
       printf("\n (GetDoorStatusDuringPoll) >> CurrentErrorStatus=%d\n",CurrentStatevar.CurrentErrorStatus);
       printf("\n (GetDoorStatusDuringPoll) >> DropCastGateOpen=%d\n",CurrentStatevar.DropCastGateOpen);
       #endif
       switch(CurrentStatevar.CurrentStatus)
       {
            //State 10 final state
            case Idling: if(DropGateOpen==DropGateTypeVar)  //idle condition
                         {
                               if(1==CurrentStatevar.DropCastGateOpen)  
                               return 1; //Drop Gate open success
                               else
                               return 0; //Drop Gate open failed
                         }
                         else if (DropGateClose==DropGateTypeVar)
                         {
                               if(1==CurrentStatevar.DropCastGateClose)  
                               return 1; //Drop Gate Close success
                               else
                               return 0; //Drop Gate Close failed
                         }
                         break;
           //State 9 to State 5 again poll 
           case Busy:return 4; //Doing internal operation (Busy Condition)
                     break;
           //State 9 to State 5 again poll  
           case Dropping:return 5; //Doing internal operation (Dropping Condition)
                         break;
           //State 12 failure 
           case Failure: return 2; //Device in failure state (failure Condition)
                         break;
           //State 11 Drop failed 
           case DropFailed: return 0; //Drop open failed (Drop failed Condition)
                            break;
            
           //State 10 final state
           case DropOk: if(DropGateOpen==DropGateTypeVar)
                         {
                               if(1==CurrentStatevar.DropCastGateOpen)  
                               return 1; //Drop Gate open success
                               else
                               return 0; //Drop Gate open failed
                         }
                         else if (DropGateClose==DropGateTypeVar)
                         {
                               if(1==CurrentStatevar.DropCastGateClose)  
                               return 1; //Drop Gate Close success
                               else
                               return 0; //Drop Gate Close failed
                         }
                        break;
           }; //Switch End
    
    }//if block end here
   
    else
    {
       printf("\n (GetDoorStatusDuringPoll) >> Could not obtain current status from dscu unit=%d",CurrentStatevar.CurrentStatus);
       return (-1);
    }
                        
}//GetDoorStatus() end here

//Get Last error code of gate open or close operation
int GetDSCULastErroCode(void)
{
  return g_ErrorCode;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                            End:Drop Gate Open/Close Function
//                     
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////Start:ATVM API Zone///////////////////////////////////////////////////////////////

/*
  IsDropandSealUnitPortOpen() Return Codes:
  0= Port still not open 
  1= Port Already Open
*/

int IsDropandSealUnitPortOpen()
{
  if(g_DSCUCommportHandle>0)
  return 1;
  else
  return 0;
}

/*
 OpenDropandSealUnit() Return Codes:
  0= Port Open error
  1= Device open successfully
  2= Device Not reponse against poll command
  3= Device Not in idle state
 -1= OpenDropandSealUnit() function parameters not ok 

*/

//Open Drop and Seal Unit for ATVM API 
int OpenDropandSealUnit( unsigned int PortNumber /*Pc Serial Port Number ex: 0 means ttyS0 */ )
{ 
  //check send command to device
  if(PortNumber <0 || PortNumber >255 )
  return -1; //Function Parameter Not ok error 
  int rtcode=-1;
  g_DSCUCommportHandle=0;
  g_DSCUCommportHandle=OpenDSCU(PortNumber);
  if(DSCU_FAIL!=g_DSCUCommportHandle)
  {
      //State :Check DSCU present in system or not
      rtcode=GetDSCUStatus();
      //State : if DSCU nt present return error code
      if(0==rtcode)
     {
       #ifdef DEBUG
       printf("\n (OpenDropandSealUnit) >> Device is not present in system return code=%d\n",rtcode);
       #endif
       //close port
       CloseDSCU(g_DSCUCommportHandle);
       return 2; //Device not presents in system
     }
     
     Delay(DSCU_POLL_INTERVAL_DELAY);

     //State : Check DSCU in idle or not
     rtcode=IsDSCUInIdle();
     //State : if nt DSCU idle return error  
     if(0==rtcode)
     {
       #ifdef DEBUG
       printf("\n (OpenDropandSealUnit) >> Device is not idle IsDSCUInIdle() return code=%d\n",rtcode);
       #endif
       CloseDSCU(g_DSCUCommportHandle);
       return 3; //Device not in idle state so no drop command can be issue
     }
     
     return 1; //Device successfully open
 }
  
  //Return Port Open Error
  else
  return 0;
 
}


/*
  CloseDropandSealUnit() Return Codes:
  0= Port close  error
  1= Device close successfully
  2= Port not opened so close operation cannot be done now 

*/

//Close Drop and Seal Unit for ATVM API 
int CloseDropandSealUnit(void)
{ 
  int rtcode=-1;
  if(-1!=g_DSCUCommportHandle)
  {
     rtcode=CloseDSCU(g_DSCUCommportHandle);
     if(DSCU_SUCCESS==rtcode)
     return 1;
     else
     return 0;
  }
  
  //Return Port Open Error
  else
  return 2;
 
}



/*
  AtvmGetDSCUStatus() Return Codes:
  1= Device is idle and normal drop open and close operation can be done
  2= Device replied but not in idle (No operation can be done)
  3= Device not replied (No operation can be done)
*/

int AtvmGetDSCUStatus(void)
{
  unsigned char PollReplyPackt[30]={'\0'};
  unsigned int  PollReplyPacktLength=7;
  int HostWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
  unsigned int rtcode=0; 
  rtcode=DSCUSendPoll(PollReplyPackt,PollReplyPacktLength,HostWaitTime);
  if(DSCU_SUCCESS==rtcode)
  {
      //Delay for issue next command 
      #ifdef POLL_DELAY 
      Delay(DSCU_POLL_INTERVAL_DELAY);
      #endif
      rtcode=IsDSCUInIdle();
      if(DSCU_SUCCESS==rtcode)
      return 1;
      else
      return 2;
  }
  else
  return 3;

}


/*

  DropNotes() Return Codes:
  1= Drop notes successfully
  2= Port not opened for this operation
  3= Drop gate failed to open
  4= Drop gate open successfully but drop gate could not close
  

  /*SetDSCUDropGateOperation() Return Codes
  //Normal Return Code
  0=Device drop gate open/close fail
  1=Device drop gate open/close success
  //Error Return Code
  2=Device in failure state
  3=Device not respond against poll command
  4=Device not in idle state so no drop command can be issue
  5=Device send invalid command against drop command
  6=Drop Gate open
  7=Drop Gate closed
  8=Device not presents in system

*/


int DropNotes(void)
{
 int rtcode=-1; 

 if(-1==g_DSCUCommportHandle)
 return 2;
 Delay(DSCU_POLL_INTERVAL_DELAY);
 //Open Drop Gate
 rtcode=SetDSCUDropGateOperation(DropGateOpen);
 if(1==rtcode)
 {
      Delay(DSCU_POLL_INTERVAL_DELAY);
      //Now Close Gate
      rtcode=SetDSCUDropGateOperation(DropGateClose);
      if(1==rtcode)
      return 1; //Drop notes successfully
      else if(0 == rtcode || rtcode >= 2 ||rtcode <=8 )
      return 4; //Drop gate open successfully but it failed to closed drop gate
    
 }
 
 else if(6==rtcode)
 {
      /*
      Delay(DSCU_POLL_INTERVAL_DELAY);
      //Now Close Gate
      rtcode=SetDSCUDropGateOperation(DropGateClose);
      if(1==rtcode)
      return 1; //Drop notes successfully
      else if(0 == rtcode || rtcode >= 2 ||rtcode <=8 )
      return 4; //Drop gate open successfully but it failed to closed drop gate
     */
     return 6;
 }
 
 
 else
 return 3; //Drop gate failed to open
}


/*
   1= Reset Success with dscu send ack status
   2= Device not present in system 
   3= Device present in system but not replied against reset command
*/

//Reset DSCU Device
static int ResetDSCU(void)
{
   int HostWaitTime=DSCU_WAIT_TIME_MS; //In MilliSecond
   int rtcode=GetDSCUStatus();
   if(DSCU_SUCCESS==rtcode)
   {
     Delay(DSCU_POLL_INTERVAL_DELAY);
     rtcode=DSCUSendReset(HostWaitTime);
     if(DSCU_SUCCESS==rtcode)
     return 1;
     else
     return 3;
   }
   else
   return 2;

}





/////////////////////////////////////End:ATVM API Zone///////////////////////////////////////////////////////////////


///////////////////////////////////////Start://Testing zone//////////////////////////////////////////////////////////

#ifdef DSCUMAIN

/*
int main()
{
    
     int choice =-1;
     int rtcode=-1;
     //Open port for DSCU operate
     printf("\n (Main)before OpenDropandSealUnit Handle id =%d\n",g_DSCUCommportHandle);
     OpenDropandSealUnit(GetDSCUPortNo());
     printf("\n (Main)After OpenDropandSealUnit Handle  id =%d\n",g_DSCUCommportHandle);
     Delay(DSCU_POLL_INTERVAL_DELAY);
     if(-1==g_DSCUCommportHandle)
     {
     printf("\n (Main)Open port failed Programm now exit\n");
     return 0;
     }

   
     while(1)
     {
        printf("\n Drop And Seal Unit Menu ");
        printf("\n 1.Drop Open");
        printf("\n 2.Drop Close");
        printf("\n 3.Reset Drop and Seal Unit");
        printf("\n 4.Drop and Seal Unit Status");
        printf("\n 5.ATVM API Gate Open and Close");
        printf("\n 6.Continuous Polling DSCU");
        printf("\n 7.Exit");
        printf("\n Enter your choice in number: ");
        scanf("%d",&choice);
        if(choice >7)
        {
          printf("\n Wrong Input Again Choice");
          continue;
        }

        if(7==choice)
        {
          printf("\n Program now exit");
          break;
        }

        switch(choice)
        {
           case 1: //DSCUSendDropOpen(5000); //Direct No State Diagram follow
                   rtcode=SetDSCUDropGateOperation(DropGateOpen);
                   //printf("\n(Main)SetDSCUDropGate Open Operation Return Code=%d ",rtcode);
                   switch(rtcode)
                   {
                      case 1:
                              printf("\n(Main) Gate Open successfully");
                              break;
                      case 2:
                              printf("\n(Main) DSCU unit in faliure state ");
                              break;
                      case 3:
                              printf("\n(Main) DSCU unit is not replied against drop open operation");
                              break;
                      case 4:
                              printf("\n(Main) DSCU unit is not in idle condition so no drop open operation can be done");
                              break;
                      case 5:
                              printf("\n(Main) DSCU unit send invalid command reply against drop open command ");
                              break;
                      case 6:
                              printf("\n(Main) Drop gate is already open so drop open cannot work now please close first and then choice drop open");
                              break;
                        case 8:
                              printf("\n(Main) DSCU unit is not presents in this system so no drop open operation can be done ");
                              break;
                    };
                   Delay(DSCU_POLL_INTERVAL_DELAY);
                   break;
           case 2: //DSCUSendDropClose(5000); //Direct No State Diagram follow
                   rtcode=SetDSCUDropGateOperation(DropGateClose);
                   //printf("\n(Main)SetDSCUDropGate Close Operation Return Code=%d ",rtcode);
                    switch(rtcode)
                   {
                      case 1:
                              printf("\n(Main) Gate Open successfully");
                              break;
                      case 2:
                              printf("\n(Main) DSCU unit in faliure state ");
                              break;
                      case 3:
                              printf("\n(Main) DSCU unit is not replied against drop close operation");
                              break;
                      case 4:
                              printf("\n(Main) DSCU unit is not in idle condition so no drop close operation can be done");
                              break;
                      case 5:
                              printf("\n(Main) DSCU unit send invalid command reply against drop close command ");
                              break;
                      case 7:
                              printf("\n(Main) Drop gate is already close so drop open cannot work now please open first and then choice drop close");
                              break;
                        case 8:
                              printf("\n(Main) DSCU unit is not presents in this system so no drop close operation can be done ");
                              break;
                    };
                   Delay(DSCU_POLL_INTERVAL_DELAY);
                   break;
           case 3: rtcode=ResetDSCU();
                   //printf("\n(Main)ResetDSCU() Return Code=%d ",rtcode);
                    switch(rtcode)
                   {
                      case 1:
                              printf("\n(Main) Reset DSCU unit is successfully done");
                              break;
                      case 2:
                              printf("\n(Main) DSCU unit is not present in system so reset operation cannot be done");
                              break;
                      case 3:
                              printf("\n(Main) DSCU unit is present in system but not replied against reset command");
                              break;
                   };
                   Delay(DSCU_POLL_INTERVAL_DELAY);
                   break;

           case 4: rtcode=AtvmGetDSCUStatus();
                   //printf("\n(Main)AtvmGetDSCUStatus() Return Code=%d ",rtcode);
                   switch(rtcode)
                   {
                      case 1:
                              printf("\n(Main) DSCU unit is idle and normal drop open and close operation can be done");
                              break;
                      case 2:
                              printf("\n(Main) DSCU unit is not idle so no operation can be done now");
                              break;
                      case 3:
                              printf("\n(Main) DSCU unit is not present in system ");
                              break;
                   };
                   Delay(DSCU_POLL_INTERVAL_DELAY);
                   break;

           case 5: rtcode=DropNotes();
                   //printf("\n(Main)DropNotes() Return Code=%d ",rtcode);
                   switch(rtcode)
                   {
                      case 1:
                              printf("\n(Main) Drop Note successfully done");
                              break;
                      case 2:
                              printf("\n(Main) Port not opened for drop notes operation ");
                              break;
                      case 3:
                              printf("\n(Main) DSCU unit Drop gate failed to open ");
                              break;
                      case 4:
                              printf("\n(Main) DSCU unit drop gate open successfully but drop gate failed to close");
                              break;
                       case 6:
                              printf("\n(Main) DSCU unit drop gate already open drop notes operation failed");
                              break;
                     };
                   Delay(DSCU_POLL_INTERVAL_DELAY);
                   break;

           case 6: DSCUPolling();
                   break;

           case 7: CloseDropandSealUnit();
                   exit(0);
                   break;
           
        }
        
     }
     
    CloseDropandSealUnit();  
    return 0;
}//main() end

*/
#endif


#endif

///////////////////////////////////////End://Testing zone//////////////////////////////////////////////////////////


