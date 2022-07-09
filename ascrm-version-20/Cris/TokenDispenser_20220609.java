package Cris;

import java.util.Arrays;
import java.util.List;
import java.lang.InterruptedException;
//import javax.smartcardio.*;
import javax.smartcardio.Card;
import javax.smartcardio.CardChannel;
import javax.smartcardio.CardException;
import javax.smartcardio.CardNotPresentException;
import javax.smartcardio.CardTerminal;
import javax.smartcardio.CommandAPDU;
import javax.smartcardio.ResponseAPDU;
import javax.smartcardio.TerminalFactory;

import Cris.Currency;
//import java.time.LocalDateTime;
//import java.time.format.DateTimeFormatter;

/*
                public static synchronized native   int ActivateCCTalkPort(int portNmbr); 1: Connect Successfully 0: Failed

                public static synchronized native   int DeActivateCCTalkPort(); 1: disconnect Successfully 0: Failed

                public static synchronized native   int GetReplyFromCCTalkPort(byte[] Command,int recvlength,byte[] Reply);

                public static synchronized native   int IsCCTALKPortOpen(); 0: Not Connected 1 :Connected
                
                public static synchronized native   int WriteLog(String fnDeviceId,String fnMessage);

                public static synchronized native   int WriteDataExchangeLog(String fnDeviceId,String fnMessageType,String fnMessage);

*/

public final class TokenDispenser{


       //private String DeviceId=null;
       private volatile boolean portbusy = false;
       //private volatile boolean flag_tokenUnpaid = false;
       public final static char[] hexArray = "0123456789ABCDEF".toCharArray();
       private byte HopperAddress = 0;
       private byte tokenCounter = 0;
       private byte tokenRemain = 0;
       private byte tokenPaid = 0;
       private byte tokenUnpaid = 0;
       private final int timeout = 5;//10;//20;
       private volatile int rtvalue = 10;
       private volatile byte[] returnarray = {(byte)10, (byte)10, (byte)10, (byte)10, (byte)10, (byte)10, (byte)10};
       private volatile byte[] returnReaderArray;
       //private volatile byte[] returnarray = null;
       private volatile boolean timeoutFlag = false;
       private volatile byte[] rtByte = {(byte)10, (byte)10, (byte)10, (byte)10, (byte)10, (byte)10, (byte)10, (byte)10, (byte)10};
       private int counter = 0;
	   private boolean conct_flag = false;
	   private boolean readercnctFlag = false;
	   private TerminalFactory terminalFactory_obj;
	   private List<CardTerminal> cardTerminals;
		private CardTerminal contactless_cardTerminal_obj;
		private CardTerminal SAM1_cardTerminal_obj;
		private CardTerminal SAM2_cardTerminal_obj;
		private Card card_obj;
		private CardChannel cardChannel_obj;
	   private static final String DeviceId = "TKN";
		private static final int TRACE = 41;
        private static final int DEBUG = 42;
        private static final int INFO = 43;
        private static final int WARN = 44;
        private static final int ERROR = 45;
        private static final int FATAL = 46;
        private static final int ALL = 47;
        private static final int OFF = 40;
        byte[] readULBLOCKreturnarray = new byte[17];
        final String AV2 = "MIFARE Plus SAM".toLowerCase();
		final String AV1 = "DESFire8 SAM-X".toLowerCase();
		private static byte[] copyOfDeviceStatus = {(byte)0,(byte)0,(byte)0,(byte)1,(byte)1,(byte)0,(byte)0,(byte)0}; 
		
       public TokenDispenser() {
              
              byte hopperAdd=0x03;
              if( hopperAdd < 1 )
              {
              		throw new IllegalArgumentException("Device id must be a positive integer");
              }
              else
              {
              		//++DeviceId = String.format("TKD%d",fnDeviceId);
              		synchronized(this)
              		{
              			HopperAddress = hopperAdd;
              		}
              		
              }
              
              Currency.fnSetupCurrencyLogFile();
              
       }//public TokenDispenser() end
	   /*
       public int TokenReader_Log(String fnMessage ){
              Currency.WriteLog(this.DeviceId,fnMessage);
              //++System.out.println(fnMessage);
              return 0;
       }
       */
       /** 
        * Main logging method
        * */
       public int TokenReader_Log(int lvl, String fnMessage ){
              Currency.Ascrm_WriteLog(this.DeviceId,fnMessage, lvl);
              //System.out.println(fnMessage);
              return 0;
       }

       public int TokenReader_Log(String fnMessageType,String fnMessage ){
              //CcTalk.WriteDataExchangeLog(this.DeviceId,fnMessageType,fnMessage);
              return 0;
       }

       public int TokenReader_Connect(int fnPortId ){
                
                //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
                if( 0 == Currency.IsCCTALKPortOpen() ){  
					
                       if( 0 == Currency.ActivateCCTalkPort( 1,fnPortId ) ){
                            return 0;//Connected Successfully
                       }else{
                            return 2;//Communication Failure
                       }
            
                }else{
                       //return 1;
                       return 0;//Connected Successfully   
                }
     
       } 

       public int TokenReader_Disconnect(){
                
               if( 1 == Currency.IsCCTALKPortOpen() ){

                       //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
                       if( 0 == Currency.DeActivateCCTalkPort(1) ){
                            return 0;//disconnected Successfully
                       }else{
                            return 2;//Communication Failure
                       }
            
                }else{
                       return 0;//disconnected Successfully   
                }

       } 

       public synchronized int TokenReader_CommunicationCycle(byte[] Command,int fnrecvlength,byte[] fnReply){
                 
                 //TokenReader_Log("Before Entry Synch Block");
                 synchronized(this)
                 {
					 //TokenReader_Log("Entry Synch Block");
					 int cctalkrplytimeout = 6;
                 	 portbusy = true;
                 	 //int rtCode = Currency.GetReplyFromCCTalkPort( Command,fnrecvlength,fnReply, 5);
                 	 int rtCode = Currency.GetReplyFromCCTalkPort( Command, fnrecvlength, fnReply, cctalkrplytimeout);
                 	 
					 if(0 == rtCode)																			//Success
					 {
							int command_length = 0, fnReply_length = 0;
							byte chksum = 0;
							byte[] ReplyArr = null;
							command_length = Command.length;
							fnReply_length = (fnReply.length - command_length);
						 
							if(fnrecvlength == (command_length+fnReply_length))
							{
								try
								{
									ReplyArr = Arrays.copyOfRange(fnReply, command_length, (fnReply.length-1));
								}
								catch(ArrayIndexOutOfBoundsException ex)
								{
									//TokenReader_Log("[TokenReader_CommunicationCycle()] ArrayIndexOutOfBoundsException caught." );
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] ArrayIndexOutOfBoundsException caught." );
									portbusy = false;
									return 31;
								}
								catch(IllegalArgumentException ex)
								{
									//TokenReader_Log("[TokenReader_CommunicationCycle()] IllegalArgumentException caught.");
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] IllegalArgumentException caught.");
									portbusy = false;
									return 31;
								}
								catch(NullPointerException ex)
								{
									//TokenReader_Log("[TokenReader_CommunicationCycle()] NullPointerException caught");
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] NullPointerException caught");
									portbusy = false;
									return 28;
								}
							
							
								//Validate Reply Array
								int lengthofData = 0;
								if(ReplyArr.length > 1)
								{
									lengthofData = ReplyArr[1];
								
									if(ReplyArr.length == (lengthofData+4))
									{
										chksum = GetCheckSum(ReplyArr);
										//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum is: "+byteToHex(chksum));
										TokenReader_Log(DEBUG, "[TokenReader_CommunicationCycle()] Checksum is: "+byteToHex(chksum));
							
										//CheckSum Calcutation
										byte checksum_from_replybyte = fnReply[fnReply.length - 1];
										if(checksum_from_replybyte == chksum)
										{
											//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum matched ");
											TokenReader_Log(DEBUG, "[TokenReader_CommunicationCycle()] Checksum matched ");
											portbusy = false;
											//return rtcode;
											return 0;
										}
										else
										{
											//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum mismatched, Checksum from ReplyByte is: "+byteToHex(checksum_from_replybyte));
											TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Checksum mismatched, Checksum from ReplyByte is: "+byteToHex(checksum_from_replybyte));
											//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum mismatched ");
											portbusy = false;
											return 28;  // Checksum mismatched
								
										}
									}
									else
									{
										//TokenReader_Log("[TokenReader_CommunicationCycle()] Reply data length mismatched ");
										TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Reply data length mismatched ");
										portbusy = false;
										return 28; // length mismatched
									}
								}
								else
								{
									//lengthofData = ReplyArr[1];
									//TokenReader_Log("[TokenReader_CommunicationCycle()] Reply data incomplete ");
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Reply data incomplete ");
									return 28;
								}
																						
							
							}// if(fnrecvlength == (command_length+fnReply_length)) end
							else
							{
								portbusy = false;
								TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Reply receive incomplete ");
								return 28; // length mismatched
							}
					}
					else
					{
							//Failure
							//TokenReader_Log("TokenReader_CommunicationCycle() rtCode = "+rtCode);
							//TokenReader_Log("TokenReader_CommunicationCycle() commandbyte : "+bytesToHex(Command));
							//TokenReader_Log("TokenReader_CommunicationCycle() replybyte : "+bytesToHex(fnReply));
							TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] rtCode = "+rtCode);
							String cm="[TX]: ";
							for(int i=0; i<Command.length; i++)
								cm = cm+"0x"+byteToHex(Command[i])+" ";
							TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] commandbyte : "+cm);
							String rp="[RX]: ";
							for(int i=0; i<fnReply.length; i++)
								rp = rp+"0x"+byteToHex(fnReply[i])+" ";
							TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] replybyte : "+rp);
							cm = null;
							rp = null;
							return 28;
					}
					
					//return 1;
                 }
       }// public synchronized int TokenReader_CommunicationCycle(byte[] Command,int fnrecvlength,byte[] fnReply) end 
       
       /**
        * Only used in issue token and reject token. For solenoid operation
        **/
       public synchronized int TokenReader_CommunicationCycle(byte[] Command,int fnrecvlength,byte[] fnReply, int timeout){
                 
                 //TokenReader_Log(DEBUG, "Before Entry Synch Block");
                 synchronized(this)
                 {
					 int cctalkrplytimeout = 6;
					 //TokenReader_Log("Entry Synch Block");
                 	 portbusy = true;
                 	 int rtCode = Currency.GetReplyFromCCTalkPort( Command,fnrecvlength,fnReply, timeout, cctalkrplytimeout);
                 	 
					 if(0 == rtCode)																			//Success
					 {
							int command_length = 0, fnReply_length = 0;
							byte chksum = 0;
							byte[] ReplyArr = null;
							command_length = Command.length;
							fnReply_length = (fnReply.length - command_length);
						 
							if(fnrecvlength == (command_length+fnReply_length))
							{
							
								try
								{
									ReplyArr = Arrays.copyOfRange(fnReply, command_length, (fnReply.length-1));
									
								}
								catch(ArrayIndexOutOfBoundsException ex)
								{
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] ArrayIndexOutOfBoundsException caught "+ex.getMessage());
									portbusy = false;
									return 28;
								}
								catch(IllegalArgumentException ex)
								{
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] IllegalArgumentException caught "+ex.getMessage());
									portbusy = false;
									return 28;
								}
								catch(NullPointerException ex)
								{
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] NullPointerException caught "+ex.getMessage());
									portbusy = false;
									return 28;
								}
							
							
								//Validate Reply Array
								int lengthofData = 0;
								if(ReplyArr.length > 1)
								{
									lengthofData = ReplyArr[1];
								
									if(ReplyArr.length == (lengthofData+4))
									{
										chksum = GetCheckSum(ReplyArr);
										//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum is: "+byteToHex(chksum));
										TokenReader_Log(DEBUG, "[TokenReader_CommunicationCycle()] Checksum is: "+byteToHex(chksum));
							
										//CheckSum Calcutation
										byte checksum_from_replybyte = fnReply[fnReply.length - 1];
										if(checksum_from_replybyte == chksum)
										{
											//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum matched ");
											TokenReader_Log(DEBUG, "[TokenReader_CommunicationCycle()] Checksum matched ");
											portbusy = false;
											//return rtcode;
											return 0;
										}
										else
										{
											//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum mismatched, Checksum from ReplyByte is: "+byteToHex(checksum_from_replybyte));
											TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Checksum mismatched, Checksum from ReplyByte is: "+byteToHex(checksum_from_replybyte));
											//TokenReader_Log("[TokenReader_CommunicationCycle()] Checksum mismatched ");
											portbusy = false;
											return 28;  // Checksum mismatched
								
										}
									}
									else
									{
										//TokenReader_Log("[TokenReader_CommunicationCycle()] Reply data length mismatched ");
										TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Reply data length mismatched ");
										portbusy = false;
										return 28;	// length mismatched
									}
								}
								else
								{
									//lengthofData = ReplyArr[1];
									//TokenReader_Log("[TokenReader_CommunicationCycle()] Reply data incomplete ");
									TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Reply data incomplete ");
									return 28;
								}
																						
							
							}// if(fnrecvlength == (command_length+fnReply_length)) end
							else
							{
								portbusy = false;
								TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] Reply Receive incomplete ");
								return 28; // length mismatched
							}
					}
					else
					{
							//Communication Failure
							//TokenReader_Log("TokenReader_CommunicationCycle() rtCode = "+rtCode);
							//TokenReader_Log("TokenReader_CommunicationCycle() commandbyte : "+bytesToHex(Command));
							//TokenReader_Log("TokenReader_CommunicationCycle() replybyte : "+bytesToHex(fnReply));
							TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] rtCode = "+rtCode);
							String cm="[TX]: ";
							for(int i=0; i<Command.length; i++)
								cm = cm+"0x"+byteToHex(Command[i])+" ";
							TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] commandbyte : "+cm);
							String rp="[RX]: ";
							for(int i=0; i<fnReply.length; i++)
								rp = rp+"0x"+byteToHex(fnReply[i])+" ";
							TokenReader_Log(ERROR, "[TokenReader_CommunicationCycle()] replybyte : "+rp);
							cm = null;
							rp = null;
							return 28;
					}
					
					//return 1;
                 }
       }// public synchronized int TokenReader_CommunicationCycle(byte[] Command,int fnrecvlength,byte[] fnReply, int timeout) end

       public static String byteToHex(byte b) {
	    int i = b & 0xFF;
	    return Integer.toHexString(i);
       }

       static byte GetCheckSum(byte[] bufData )
       { 
                  int CHKSUM=0x00;
		  int i=0;
		  CHKSUM = 0;
		  for(i=0; i < bufData.length; i++)
		  {	 
		    CHKSUM += bufData[i];   
		  } 
		  CHKSUM = ~CHKSUM;    // Complement the byte.
		  CHKSUM = CHKSUM + 1; // Final Byte.
		  return (byte)CHKSUM;
        }

        public static String bytesToHex(byte[] bytes) 
        {
			char[] hexChars = new char[bytes.length * 2];
			for ( int j = 0; j < bytes.length; j++ ) 
			{
				int v = bytes[j] & 0xFF;
				hexChars[j * 2] = hexArray[v >>> 4];
				hexChars[j * 2 + 1] = hexArray[v & 0x0F];
			}
			return new String(hexChars);
        }// public static String bytesToHex(byte[] bytes) end

		private void setFlagValue(boolean value)
		{
			synchronized(this){
				timeoutFlag = value;
			//TokenReader_Log("[TokenDispenser] [setFlagValue()] falg value is "+timeoutFlag);
			}
		}// private void setFlagValue(boolean value) end
		
		private boolean getFlagValue()
		{
			synchronized(this){
				//TokenReader_Log("[TokenDispenser] [getFlagValue()] falg value is "+timeoutFlag);
				return timeoutFlag;
			}
		}// private boolean getFlagValue() end
	   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		public final String GetNativeLibVersion()
		{
			/*	Issue/Reject is determined if issue/reject bit on. 
			 *  Status of other bit is not considered.
			 *  Date 05-02-2020
			 */
			 /*
			  *  ClearPath before every ph2 operation.
			  *  Date 18-02-2020
			  */
			  /*
			  *  New Method Reset Device
			  *  Date 28-02-2020   
			  */
			  /*
			   * Retry issue and reject
			   * Date 10-07-2020   --v 02.01.00
			   */
			   /*
			   * GetDeviceStatus Return byte sensor bits rearranged asper Cris
			   * Date 12-07-2020   --v 03.00.00
			   */
			   /*
			   * GetDeviceStatus Return copy of device status If the port is busy
				* Date 23-05-2022   --v 03.01.00
			   */
			   /*
			   * GetDeviceStatus Return copy of device status If the port is busy
				* Date 23-05-2022   --v 03.01.01
			   */
			return "03.01.01";
		}// public final String GetNativeLibVersion() end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  Method Name:ConnectDevice
  Return Type:int
  Channel Clearance Mode: 0-Retain in the channel, 1-Send to rejection bin, 2-Send to dispensing outlet of the device.
  Return Value: 0-Device connected successfully, 1-Port doesn't exist', 28-Communication failure, 3-Channel clearance failed due to rejection bin is full, 
  4-Channel clearance failed due to channel is blocked, 5- Channel clearance failed due to unknown reason
*/
       
		
		public final int ConnectDevice(int PortId, int ChannelClearanceMode, int Timeout)
		{
			//TokenReader_Log("[ConnectDevice()] Enter.");
			//int rtvalue = 10;
			if(Timeout<=0)
			{
				//TokenReader_Log("[ConnectDevice()] Wrong Timeout value.");
				TokenReader_Log(ERROR, "[ConnectDevice()] Wrong Timeout value ");
				//return 7;
				return 31;
			}
			else if(true == isdeviceConnected())
			{
				//Device Connected
				//TokenReader_Log("[ConnectDevice()] Device already Connected.");
				TokenReader_Log(ERROR, "[ConnectDevice()] Device already Connected ");
				return 20;
			}
			else 
			{
				Thread timeout_THREAD = new Thread(new Runnable(){
				
							public void run()
							{
								rtvalue = ConnectDevice_V2(PortId, ChannelClearanceMode);
								setFlagValue(true);
							}
				
				}); 
				
				//timeout_THREAD.setDaemon(true);
				timeout_THREAD.start();
			
				long endtime = 0;
				endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
				while(endtime>System.currentTimeMillis())
				{
					if(true == getFlagValue())
					{
						
						//TokenReader_Log("[TokenDispenser] [ConnectDevice()] Return Value "+rtvalue);
						TokenReader_Log(DEBUG, "[TokenDispenser] [ConnectDevice()] Return Value "+rtvalue);
						//timeout_THREAD.interrupt();
						setFlagValue(false);
						return rtvalue;
						//break;
					}
				}
				
				if(true == timeoutFlag)
				{
					//TokenReader_Log("[TokenDispenser] [ConnectDevice()] Operation Timeout Occured ");
					TokenReader_Log(ERROR, "[TokenDispenser] [ConnectDevice()] Operation Timeout Occured ");
					//return 6;
					setFlagValue(false);
					return 18;
				}
				else
				{
					setFlagValue(false);
					TokenReader_Log(INFO, "[TokenDispenser] [ConnectDevice()] rtvalue "+rtvalue);
					return rtvalue;
				}
				
			}
		}//public synchronized final int ConnectDevice(int PortId, int ChannelClearanceMode, int Timeout)
		
		private synchronized final int ConnectDevice_V2(int PortId, int ChannelClearanceMode)
		//public synchronized final int ConnectDevice(int PortId, int ChannelClearanceMode, int Timeout)
		{		
				synchronized(this)	
				{
					int rtCode = 10;
					TokenReader_Log(DEBUG, "Port Id: "+PortId);
					rtCode = TokenReader_Connect(PortId);
					
					if(0 == rtCode)
					{
						// Hopper Start
						//PortOpened
						rtCode = SimplePoll();																//Simple Poll
						try
						{
							Thread.sleep(timeout);
						}
						catch(InterruptedException ex)
						{
							//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
							TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
						}
						if(0 == rtCode)
						{
							rtCode = ResetHopper();															//Reset Hopper
							
							try
							{
								Thread.sleep(200);
							}
							catch(InterruptedException ex)
							{
								//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
								TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
							}
							
							if(0 == rtCode)
							{
								rtCode = TestHopper();														//Test Hopper
								if(0 == rtCode)
								{
									//rtCode = getSerialNumber();											//Request Serial Number
									//if(0 == rtCode)
									//{
										try
										{
											Thread.sleep(timeout);
										}
										catch(InterruptedException ex)
										{
											//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
											TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
										}
										
										rtCode = EnableHopper();											//Enable Hopper
										if(0 == rtCode)
										{
											try
											{
												Thread.sleep(timeout);
											}
											catch(InterruptedException ex)
											{
												//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex);
												TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
											}
											
											fn_ClearpathStatus();									//Reset Path Status
										}
										else
										{
											// Failure
											//TokenReader_Log("[ ConnectDevice_V2() ] Enable Hopper rt = "+rtCode);
											TokenReader_Log(ERROR, "[ ConnectDevice_V2() ] Enable Hopper rt = "+rtCode);
											return rtCode; //28 or 31
										}
								}
								else
								{
									// Failure
									//TokenReader_Log("[ ConnectDevice_V2() ] TestHopper rt = "+rtCode);
									TokenReader_Log(ERROR, "[ ConnectDevice_V2() ] TestHopper rt = "+rtCode);
									return rtCode; //28 or 31
								}
							}
							else
							{
								// Failure
								//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Reset Hopper = "+rtCode);
								TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] Reset Hopper = "+rtCode);
								return rtCode; //28 or 31
							}
						
						}
						else
						{
							//Failed
							//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] simple poll = "+rtCode);
							TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] simple poll = "+rtCode);
							return rtCode; //28 or 31
						}
					
						//////// Hopper End
						
						
						//////// Channel Start
						
						if(0 == rtCode)
						{
							rtCode = 10;
							// Doing Channel Clearance Functions
							if( 0 == ChannelClearanceMode)
							{
								//Success
								rtCode = fn_ClearpathStatus();
								if(0 == rtCode)
								{
									//Success
									setConctFlag(true);
									return 0;
								}
								else
								{
									String pathstat = getPathStatus_string();
									if('0'==pathstat.charAt(0) && '0'==pathstat.charAt(6) && '0'==pathstat.charAt(6))
									{
										//Successfull
										setConctFlag(true);
										return 0;
									}
									else
									{
										if('2'==pathstat.charAt(0))
										{
											//Communication Failure
											return 28;
										}
										//Other Error
										return 31;
									}
								}
								//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Device Connected ChannelClearanceMode is "+ChannelClearanceMode);
							}
							
							//Send to Rejection Bin
							else if( 1 == ChannelClearanceMode)
							{
								fn_ClearpathStatus();
								String pathstat = getPathStatus_string();
								if('0'==pathstat.charAt(7))
								{
									try
									{
										Thread.sleep(50);
									}
									catch(InterruptedException ex)
									{
										//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
										TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
									}
									pathstat = getPathStatus_string();
									if('0'==pathstat.charAt(7))
									{
										fn_ClearpathStatus();
										setConctFlag(true);
										//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Device Connected. No Token found in channel");
										TokenReader_Log(INFO, "[TokenDispenser] [ ConnectDevice_V2()] Device Connected. No Token found in channel");
										return 0;
									}

								}
								//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] ChannelClearanceMode is "+ChannelClearanceMode);
								//TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] ChannelClearanceMode is "+ChannelClearanceMode);
								if('1'==pathstat.charAt(4))
								{
									//Rejection Bin Full
									setConctFlag(true);
									//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Device Connected. cannot reject token rejection bin full");
									TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] Device Connected. cannot reject token rejection bin full");
									return 1;
								}
								
								rtCode = 10;
								rtCode = fn_ClearpathStatus();									//Reset Path Status
								if(8 == rtCode)
								{
									try
									{
										Thread.sleep(timeout);
									}
									catch(InterruptedException ex)
									{
										//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
										TokenReader_Log(DEBUG, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
									}
								
									rtCode = rejectToken();														//Token Reject					
									String pathStat = getPathStatus_string();
									if('1' == pathStat.charAt(5) && '0' == pathStat.charAt(0))
									{
										//Success
										try
										{
											Thread.sleep(timeout);
										}
										catch(InterruptedException ex)
										{
											//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
											TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
										}
										fn_ClearpathStatus();									//Reset Path Status
										setConctFlag(true);
										//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Device Connected, Token to Rejection bin "+rtCode);
										TokenReader_Log(INFO, "[TokenDispenser] [ ConnectDevice_V2()] Device Connected, Token to Rejection bin "+rtCode);
										return 0;
									}
									else
									{
										if('0' == pathStat.charAt(0))
										{
											//Communication Failure
											TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] Reject Token failed "+rtCode);
											//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Reject Token failed "+rtCode);
											return 28;
										}
										//Other error
										//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Reject Token failed "+rtCode);
										TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] Reject Token failed "+rtCode);
										return 31;
									}
								}
								else
								{
									//Other Error
									//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Path Clear failed "+rtCode);
									TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] Path Clear failed "+rtCode);
									return 31;
								}
								
							}
							//Send to Issue
							else if( 2 == ChannelClearanceMode)
							{
								fn_ClearpathStatus();
								String pathstat = getPathStatus_string();
								if('0'==pathstat.charAt(7))
								{
									try
									{
										Thread.sleep(50);
									}
									catch(InterruptedException ex)
									{
										TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
									}
									pathstat = getPathStatus_string();
									if('0'==pathstat.charAt(7))
									{
										fn_ClearpathStatus();
										setConctFlag(true);
										//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Device Connected. No Token found in channel");
										TokenReader_Log(INFO, "[TokenDispenser] [ ConnectDevice_V2()] Device Connected. No Token found in channel");
										return 0;
									}

								}
								//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] ChannelClearanceMode is "+ChannelClearanceMode);
								
								//rtCode = 10;
								try
								{
									Thread.sleep(timeout);
								}
								catch(InterruptedException ex)
								{
									//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex);
									TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
								}
								//rtCode = 
								issueToken();
								String pathStat = getPathStatus_string();
								if('1' == pathStat.charAt(6) && '0' == pathStat.charAt(0))
								{
									//Success
									try
									{
										Thread.sleep(timeout);
									}
									catch(InterruptedException ex)
									{
										//TokenReader_Log(" [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex);
										TokenReader_Log(ERROR, " [ ConnectDevice_V2() ] Exception in Thread sleep : "+ex.getMessage());
									}
									fn_ClearpathStatus();									//Reset Path Status
									setConctFlag(true);
									//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Device Connected Token to dispense outlet ");
									TokenReader_Log(INFO, "[TokenDispenser] [ ConnectDevice_V2()] Device Connected Token to dispense outlet ");
									return 0;
								}
								else
								{
									if('0' == pathStat.charAt(0))
									{
										//Communication Failure
										//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Token to dispense outlet failed "+rtCode);
										TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] Token to dispense outlet failed "+rtCode);
										return 28;
									}
									//Other Error
									//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] Token to dispense outlet failed "+rtCode);
									TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] Token to dispense outlet failed "+rtCode);
									return 31;
								}
							}
							else
							{
								//Other error
								//TokenReader_Log("[TokenDispenser] [ ConnectDevice_V2()] ChannelClearanceMode Not matched "+ChannelClearanceMode);
								TokenReader_Log(ERROR, "[TokenDispenser] [ ConnectDevice_V2()] ChannelClearanceMode Not matched "+ChannelClearanceMode);
								return 31;
							}
						}
						else
						{
							//Failure
							//TokenReader_Log("[ConnectDevice_V2()] Connection Failure Return value (Channel Start) = "+rtCode);
							TokenReader_Log(ERROR, "[ConnectDevice_V2()] Connection Failure Return value (Channel Start) = "+rtCode);
							return rtCode; //28 or 31
						}
						
					//////// Channel End
				}
				else
				{
					//Communication Failure
					//TokenReader_Log("Connection Failure Port = "+PortId);
					//TokenReader_Log("Connection Failure Return value (Hopper Start) = "+rtCode);
					TokenReader_Log(ERROR, " [ConnectDevice_V2()] Connection Failure Port = "+PortId);
					TokenReader_Log(ERROR, " [ConnectDevice_V2()] Connection Failure Return value (Hopper Start) = "+rtCode);
					return 28;
				}
			}
       			//return 0;
       }// public synchronized int ConnectDevice(int PortId, int ChannelClearanceMode, int Timeout) end 

		public synchronized boolean isdeviceConnected()
		{
			synchronized(this){
				return conct_flag;}
		}

		private synchronized void setConctFlag(boolean flag)
		{
			synchronized(this){
				conct_flag = flag;}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  Method Name:DisConnectDevice
  Return Type:int
  Return Value: 0-Device disconnected successfully, 28-Communication failure
*/

	public final int DisConnectDevice(int Timeout)
	{
		TokenReader_Log(DEBUG, "[DisConnectDevice()] Entry.");
		//int rtvalue = 10;
		if(Timeout<=0)
		{
			//Other Error
			TokenReader_Log(ERROR, "[DisConnectDevice()] Wrong Timeout value");
			return 31;
		}
		else if(false == isdeviceConnected())
		{
			//Device Not Connected
			TokenReader_Log(ERROR, "[DisConnectDevice()] Device not Connected.");
			return 20;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = DisConnectDevice();
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					//TokenReader_Log("[TokenDispenser] [DisConnectDevice()] Return Value "+rtvalue);
					TokenReader_Log(DEBUG, "[TokenDispenser] [DisConnectDevice()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(true == timeoutFlag)
			{
				//Timeout
				//TokenReader_Log("[TokenDispenser] [DisConnectDevice()] Operation Timeout Occured ");
				TokenReader_Log(ERROR, "[TokenDispenser] [DisConnectDevice()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				//TokenReader_Log("[TokenDispenser] [DisConnectDevice()] rtvalue "+rtvalue);
				TokenReader_Log(DEBUG, "[TokenDispenser] [DisConnectDevice()] rtvalue "+rtvalue);
				return rtvalue;
			}
				
		}
	}// public synchronized final int DisConnectDevice() end
	
	private synchronized final int DisConnectDevice()
	//public synchronized final int DisConnectDevice()
	{
			synchronized(this){
			
					int rtCode = 10;
					rtCode = DisableHopper();
					if(0 == rtCode)
					{
						rtCode = TestHopper();
						String pathstat = getPathStatus_string();
						if('1' == pathstat.charAt(7))
						{
							//Disconnected successfully but a token is in the channel
							TokenReader_Disconnect();
							//TokenReader_Log("[TokenReader_Disconnect()] Disconnected successfully but a token is in the channel");
							TokenReader_Log(INFO, "[DisConnectDevice()] Disconnected successfully but a token is in the channel");
							setConctFlag(false);
							return 1;
						}
						TokenReader_Disconnect();
						//TokenReader_Log("[TokenReader_Disconnect()] Disconnected successfully.");
						TokenReader_Log(INFO, "[DisConnectDevice()] Disconnected successfully.");
						setConctFlag(false);
						return 0;
					}
					else
					{
						//Failure
						//TokenReader_Log("[TokenReader_Disconnect()] Disconnection failed.");
						TokenReader_Log(ERROR, "[DisConnectDevice()] Disconnection failed.");
						return rtCode; //28 or 31
					}
			}
			
	}// public synchronized int DisConnectDevice() end
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  Method Name:GetDeviceStatus
  Return Type:int
  Return Value: 0-Operation successfull, 28-Communication failure, 2-Channel blocked, 3-No token found in channel
  		4-Operation timeout occured  
*/
	
	public final byte[] GetDeviceStatus(int ComponentId, int Timeout)
	{
			
			if(Timeout<=0)
			{
				//TokenReader_Log("[GetDeviceStatus()] Wrong Timeout value.");
				TokenReader_Log(ERROR, "[GetDeviceStatus()] Wrong Timeout value.");
				returnarray[0] = (byte)31;
				return returnarray;
			}
			else if(false == isdeviceConnected())
			{
				returnarray[0] = (byte)20;
				//TokenReader_Log("[GetDeviceStatus()] Device Not Connected.");
				TokenReader_Log(ERROR, "[GetDeviceStatus()] Device Not Connected.");
				return returnarray;
			}
			else 
			{
				Thread timeout_THREAD = new Thread(new Runnable(){
				
					public void run()
					{
						//TokenReader_Log("[GetDeviceStatus()] before status read : "+Arrays.toString(TokenDispenser.this.returnarray));
						returnarray = GetDeviceStatus(ComponentId);
						//TokenReader_Log("[GetDeviceStatus()] after status read : "+Arrays.toString(TokenDispenser.this.returnarray));
						setFlagValue(true);
					}
				}); 
					
				//timeout_THREAD.setDaemon(true);
				timeout_THREAD.start();
				
				long endtime = 0;
				endtime = System.currentTimeMillis()+Timeout;
				/*
				try
				{
					timeout_THREAD.join(Timeout);
				}
				catch(InterruptedException ex)
				{
					//TokenReader_Log("[GetDeviceStatus()]InterruptedException Exception "+ex.getMessage());
					TokenReader_Log(ERROR, "[GetDeviceStatus()]InterruptedException Exception "+ex.getMessage());
				}
				*/
				//return rtvalue;
					
				while(endtime>System.currentTimeMillis())
				{
					if(true == getFlagValue())
					{
						TokenReader_Log(INFO, "[TokenDispenser] [DisConnectDevice()] Return Value "+rtvalue);
						TokenReader_Log(INFO, "[ConnectDevice()] stat: "+Arrays.toString(this.returnarray));
						//timeout_THREAD.interrupt();
						setFlagValue(false);
						return returnarray;
						//break;
					}
				}
					
				if(true == getFlagValue())
				{
					//Timeout
					//TokenReader_Log("[TokenDispenser] [GetDeviceStatus()] Operation Timeout Occured ");
					TokenReader_Log(ERROR, "[TokenDispenser] [GetDeviceStatus()] Operation Timeout Occured ");
					TokenReader_Log(ERROR, "[TokenDispenser] [GetDeviceStatus()] join method omitted ");
					//TokenReader_Log("[ConnectDevice()] stat: "+Arrays.toString(this.returnarray));
					returnarray[0] = (byte)18;
					setFlagValue(false);
					return returnarray;
				}
				else
				{
					//TokenReader_Log("[GetDeviceStatus()] stat: "+Arrays.toString(this.returnarray));
					TokenReader_Log(INFO, "[GetDeviceStatus()] stat: "+Arrays.toString(this.returnarray));
					setFlagValue(false);
					return returnarray;
				}
			}
		//return new byte[7];
	}// public synchronized final byte[] GetDeviceStatus(int ComponentId, int Timeout) end

	private synchronized final byte[] GetDeviceStatus(int ComponentId)
	//public synchronized final byte[] GetDeviceStatus(int ComponentId, int Timeout)
	{
		synchronized(this)
		{
			
			byte[] rtByteArray = new byte[8];
			//checking if the port is busy
			/*
			int isPortBusy = Currency.IsCCTALKPortBusy();
			
			if(isPortBusy==0){
			
				return copyOfDeviceStatus;
				
			}
			*/
			
			
			//rtByteArray[6]={};
			
			byte[] rtChannelByte = new byte[8];
			int rtCode = 10;
			
			if(0 == ComponentId)
			{
				rtByteArray[1] = (byte) RFID_ReaderStatus();
				rtByteArray[2] = (byte) SAM_ReaderStatus();
				
				//Channel
				String rtString= getPathStatus_string();
				
				if('0'==rtString.charAt(0))
				{
					if(('0'==rtString.charAt(5)) &&
						('0'==rtString.charAt(6)) &&
						('0'==rtString.charAt(7)))
					{
						rtByteArray[5]=(byte)0;
					}
					else
					{
						rtByteArray[5]=(byte)1;
					}
					
				rtByteArray[3] = (byte)Character.getNumericValue(rtString.charAt(3));
				//TokenReader_Log("byte[3] "+rtByteArray[3]+" actual byte : "+(byte)Character.getNumericValue(rtString.charAt(3)));
				rtByteArray[4] = (byte)Character.getNumericValue(rtString.charAt(4));
				//TokenReader_Log("byte[4] "+rtByteArray[4]+" actual byte : "+(byte)Character.getNumericValue(rtString.charAt(4)));
				rtByteArray[0] = (byte)Character.getNumericValue(rtString.charAt(0));
				}	
				else
				{
					//TokenReader_Log("[GetDeviceStatus] PathStat Error returned. ");
					TokenReader_Log(ERROR, "[GetDeviceStatus()] PathStat Error returned. ");
					rtByteArray[0]=(byte)1;
					if('2' == rtString.charAt(0))
					{
						rtByteArray[0]=(byte)28;
					}
					rtByteArray[1]=(byte)0;
					rtByteArray[2]=(byte)0;
					rtByteArray[3]=(byte)0;
					rtByteArray[4]=(byte)0;
					rtByteArray[5]=(byte)0;
					//rtByteArray[5]=(byte)0;
				}
				
				byte byte6 = 0b00000000;
				
				//0 bit
				if('1' == rtString.charAt(0))
				{
					//TokenReader_Log("[GetDeviceStatus()] 0th bit before or :"+byte6);
					byte6 = (byte)( byte6 | (byte)0b00000001 );
					//TokenReader_Log("[GetDeviceStatus()] 0th bit after or :"+byte6);
				}
				
				//1 bit
				//TokenBin1
				if('0' == rtString.charAt(1))
				{
					//TokenReader_Log("[GetDeviceStatus()] 1st bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00000010 );
					//TokenReader_Log("[GetDeviceStatus()] 1st bit after or :"+byte6);
				}
				
				//2 bit
				//TokenBin2
				//RFU and always set to 0

				//3 bit
				//Rejection Bin Present or absent
				if('0' == rtString.charAt(2))
				{
					//TokenReader_Log("[GetDeviceStatus()] 3rd bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00001000 );
					//TokenReader_Log("[GetDeviceStatus()] 3rd bit after or :"+byte6);
				}
				
				//4 bit
				//RW Area Sensor status
				if('1' == rtString.charAt(7))
				{
					//TokenReader_Log("[GetDeviceStatus()] token in staging area");
					//TokenReader_Log("[GetDeviceStatus()] channel status staging area before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00010000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status staging area after or :"+byte6);
				}
				
				//5 bit
				//Collection Channel Sensor status
				
				//6 bit
				//Rejection Channel Sensor status
				
				//7 bit
				//RFU
				
				rtByteArray[6] = byte6;
				//TokenReader_Log("[GetDeviceStatus()] channel status byte"+rtByteArray[6]+" byte "+byte6);
				TokenReader_Log(INFO, "[GetDeviceStatus()] channel status byte"+rtByteArray[6]+" byte "+byte6);
				
				byte byte7 = 0b00000000;
				rtByteArray[7] = byte7;
				
				//0 bit
				//Bin1 Stock Sensor
				if('1' == rtString.charAt(3))
				{
					//TokenReader_Log("[GetDeviceStatus()] 0th bit before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b00000001 );
					//TokenReader_Log("[GetDeviceStatus()] 0th bit after or :"+byte7);
				}
				
				//1 bit
				//Bin2 Stock Sensor
				//always set 0
				
				//2 bit
				//Rejection Bin Stock Sensor
				if('1' == rtString.charAt(4))
				{
					//TokenReader_Log("[GetDeviceStatus()] 2nd bit before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b00000100 );
					//TokenReader_Log("[GetDeviceStatus()] 2nd bit after or :"+byte7);
				}
				
				//3 bit
				//RFU
				
				//4 bit
				//RFU
				
				//5 bit
				//Last Ejection Status
				if('1' == rtString.charAt(6))
				{
					//TokenReader_Log("[GetDeviceStatus()] token passed through issue channel");
					//TokenReader_Log("[GetDeviceStatus()] channel status issue before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b00100000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status issue after or :"+byte7);
				}
				
				//6 bit
				//RFU
				
				//7 bit
				//Last Rejection Status
				if('1' == rtString.charAt(5))
				{
					//TokenReader_Log("[GetDeviceStatus()] token passed through reject channel");
					//TokenReader_Log("[GetDeviceStatus()] channel status reject before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b10000000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status reject after or :"+byte7);
				}
				
				
				/*
				//rtByteArray[6] = 0b00000000&String.valueOf(new StringBuffer(getPathStatus_string())).getBytes();
				byte byte6 = 0b00000000;
				if('1' == rtString.charAt(0))
				{
					//TokenReader_Log("[GetDeviceStatus()] 1st bit before or :"+byte6);
					byte6 = (byte)( byte6 | (byte)0b00000001 );
					//TokenReader_Log("[GetDeviceStatus()] 1st bit after or :"+byte6);
				}
				if('1' == rtString.charAt(1))
				{
					//TokenReader_Log("[GetDeviceStatus()] 2nd bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00000010 );
					//TokenReader_Log("[GetDeviceStatus()] 2nd bit after or :"+byte6);
				}
				if('1' == rtString.charAt(2))
				{
					//TokenReader_Log("[GetDeviceStatus()] 3rd bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00000100 );
					//TokenReader_Log("[GetDeviceStatus()] 3rd bit after or :"+byte6);
				}
				if('1' == rtString.charAt(3))
				{
					//TokenReader_Log("[GetDeviceStatus()] 4th bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00001000 );
					//TokenReader_Log("[GetDeviceStatus()] 4th bit after or :"+byte6);
				}
				if('1' == rtString.charAt(4))
				{
					//TokenReader_Log("[GetDeviceStatus()] 5th bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00010000 );
					//TokenReader_Log("[GetDeviceStatus()] 5th bit after or :"+byte6);
				}
				if('1' == rtString.charAt(5))
				{
					//TokenReader_Log("[GetDeviceStatus()] token passed through reject channel");
					//TokenReader_Log("[GetDeviceStatus()] channel status reject before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00100000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status reject after or :"+byte6);
				}
				if('1' == rtString.charAt(6))
				{
					//TokenReader_Log("[GetDeviceStatus()] token passed through issue channel");
					//TokenReader_Log("[GetDeviceStatus()] channel status issue before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b01000000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status issue after or :"+byte6);
				}
				if('1' == rtString.charAt(7))
				{
					//TokenReader_Log("[GetDeviceStatus()] token in staging area");
					//TokenReader_Log("[GetDeviceStatus()] channel status staging area before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b10000000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status staging area after or :"+byte6);
				}
				rtByteArray[6] = byte6;
				//TokenReader_Log("[GetDeviceStatus()] channel status byte"+rtByteArray[6]+" byte "+byte6);
				TokenReader_Log(INFO, "[GetDeviceStatus()] channel status byte"+rtByteArray[6]+" byte "+byte6);
				rtByteArray[7] = 0b00000000;
				*/
			}
			else if(1 == ComponentId)
			{
				rtByteArray[0] = (byte)0;
				rtByteArray[1] = (byte)RFID_ReaderStatus();
				rtByteArray[2] = (byte)SAM_ReaderStatus();
				rtByteArray[3] = (byte)0;
				rtByteArray[4] = (byte)0;
				rtByteArray[5] = (byte)0;
				rtByteArray[6] = 0b00000000;
				rtByteArray[7] = 0b00000000;
			}
			else if(2 == ComponentId)
			{
				//Token Container
				String rtString= getPathStatus_string();
				
				rtByteArray[3] = (byte)rtString.charAt(3);
				if('0' == rtString.charAt(0))
				{
					rtByteArray[0] = (byte)rtString.charAt(0);
				}
				else if('2' == rtString.charAt(0))
				{
					rtByteArray[0]=(byte)28;
				}
				else
				{
					rtByteArray[0]=(byte)31;
				}
				rtByteArray[1] = (byte)0;
				rtByteArray[2] = (byte)0;
				rtByteArray[4] = (byte)0;
				rtByteArray[6] = 0b00000000;
				rtByteArray[7] = 0b00000000;
			}
			else if(3 == ComponentId)
			{
				//Rejection Bin
				String rtString= getPathStatus_string();
				if('0' == rtString.charAt(0))
				{
					rtByteArray[0] = (byte)rtString.charAt(0);
				}
				else if('2' == rtString.charAt(0))
				{
					rtByteArray[0]=(byte)28;
				}
				else
				{
					rtByteArray[0]=(byte)31;
				}
				rtByteArray[4] = (byte)rtString.charAt(0);
				rtByteArray[1] = (byte)0;
				rtByteArray[2] = (byte)0;
				rtByteArray[3] = (byte)0;
				rtByteArray[5] = (byte)0;
				rtByteArray[6] = 0b00000000;
				rtByteArray[7] = 0b00000000;
			}
			else if(4 == ComponentId)
			{
				//Channel
				String rtString = getPathStatus_string();//Integer.toBinaryString(getPathStatus());
				
				if('0'==rtString.charAt(0))
				{
					if(('0'==rtString.charAt(5)) &&
						('0'==rtString.charAt(6)) &&
						('0'==rtString.charAt(7)))
					{
						rtByteArray[0]=(byte)0;
						rtByteArray[1]=(byte)0;
						rtByteArray[2]=(byte)0;
						rtByteArray[3]=(byte)0;
						rtByteArray[4]=(byte)0;
						rtByteArray[5]=(byte)0;
					}
					else
					{
						rtByteArray[0]=(byte)0;
						rtByteArray[1]=(byte)0;
						rtByteArray[2]=(byte)0;
						rtByteArray[3]=(byte)0;
						rtByteArray[4]=(byte)0;
						rtByteArray[5]=(byte)1;
					}
				}	
				else
				{
					rtByteArray[0]=(byte)1;
					if('2' == rtString.charAt(0))
					{
						rtByteArray[0]=(byte)28;
					}
					rtByteArray[1]=(byte)0;
					rtByteArray[2]=(byte)0;
					rtByteArray[3]=(byte)0;
					rtByteArray[4]=(byte)0;
					rtByteArray[5]=(byte)0;
				}
				
				byte byte6 = 0b00000000;
				
				//0 bit
				if('1' == rtString.charAt(0))
				{
					//TokenReader_Log("[GetDeviceStatus()] 0th bit before or :"+byte6);
					byte6 = (byte)( byte6 | (byte)0b00000001 );
					//TokenReader_Log("[GetDeviceStatus()] 0th bit after or :"+byte6);
				}
				
				//1 bit
				//TokenBin1
				if('0' == rtString.charAt(1))
				{
					//TokenReader_Log("[GetDeviceStatus()] 1st bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00000010 );
					//TokenReader_Log("[GetDeviceStatus()] 1st bit after or :"+byte6);
				}
				
				//2 bit
				//TokenBin2
				//RFU and always set to 0

				//3 bit
				//Rejection Bin Present or absent
				if('0' == rtString.charAt(2))
				{
					//TokenReader_Log("[GetDeviceStatus()] 3rd bit before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00001000 );
					//TokenReader_Log("[GetDeviceStatus()] 3rd bit after or :"+byte6);
				}
				
				//4 bit
				//RW Area Sensor status
				if('1' == rtString.charAt(7))
				{
					//TokenReader_Log("[GetDeviceStatus()] token in staging area");
					//TokenReader_Log("[GetDeviceStatus()] channel status staging area before or :"+byte6);
					byte6 = (byte) ( byte6 | (byte)0b00010000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status staging area after or :"+byte6);
				}
				
				//5 bit
				//Collection Channel Sensor status
				
				//6 bit
				//Rejection Channel Sensor status
				
				//7 bit
				//RFU
				
				rtByteArray[6] = byte6;
				//TokenReader_Log("[GetDeviceStatus()] channel status byte"+rtByteArray[6]+" byte "+byte6);
				TokenReader_Log(INFO, "[GetDeviceStatus()] channel status byte"+rtByteArray[6]+" byte "+byte6);
				
				byte byte7 = 0b00000000;
				rtByteArray[7] = byte7;
				
				//0 bit
				//Bin1 Stock Sensor
				if('1' == rtString.charAt(3))
				{
					//TokenReader_Log("[GetDeviceStatus()] 0th bit before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b00000001 );
					//TokenReader_Log("[GetDeviceStatus()] 0th bit after or :"+byte7);
				}
				
				//1 bit
				//Bin2 Stock Sensor
				//always set 0
				
				//2 bit
				//Rejection Bin Stock Sensor
				if('1' == rtString.charAt(4))
				{
					//TokenReader_Log("[GetDeviceStatus()] 2nd bit before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b00000100 );
					//TokenReader_Log("[GetDeviceStatus()] 2nd bit after or :"+byte7);
				}
				
				//3 bit
				//RFU
				
				//4 bit
				//RFU
				
				//5 bit
				//Last Ejection Status
				if('1' == rtString.charAt(6))
				{
					//TokenReader_Log("[GetDeviceStatus()] token passed through issue channel");
					//TokenReader_Log("[GetDeviceStatus()] channel status issue before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b00100000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status issue after or :"+byte7);
				}
				
				//6 bit
				//RFU
				
				//7 bit
				//Last Rejection Status
				if('1' == rtString.charAt(5))
				{
					//TokenReader_Log("[GetDeviceStatus()] token passed through reject channel");
					//TokenReader_Log("[GetDeviceStatus()] channel status reject before or :"+byte7);
					byte7 = (byte) ( byte7 | (byte)0b10000000 );
					//TokenReader_Log("[GetDeviceStatus()] channel status reject after or :"+byte7);
				}
			}
			else
			{
				rtByteArray[0] = (byte)31;
				rtByteArray[1] = (byte)0;
				rtByteArray[2] = (byte)0;
				rtByteArray[3] = (byte)0;
				rtByteArray[4] = (byte)0;
				rtByteArray[5] = (byte)0;
				rtByteArray[6] = 0b00000000;
				rtByteArray[7] = 0b00000000;
			}
			
			//copyOfDeviceStatus = Arrays.copyOfRange(rtByteArray, 0, rtByteArray.length);
			//TokenReader_Log(INFO, " [GetDeviceStatus()] Copying the array: "+Arrays.toString(copyOfDeviceStatus));
			//TokenReader_Log(" [GetDeviceStatus()] Return array: "+Arrays.toString(rtByteArray));
			TokenReader_Log(INFO, " [GetDeviceStatus()] Return array: "+Arrays.toString(rtByteArray));
			return rtByteArray;
		}
		
	}// public synchronized byte[] GetDeviceStatus(int ComponentId, int Timeout) end
	
	public synchronized final byte[] GetStatus()
	{
		byte[] abc = new byte[8];
		return abc;
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  Method Name:DispanseTokenPhase1
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-Specified box empty
  		4-Operation timeout occured, 5-Other error
*/
	
	public final int DispenseTokenPhase1(int BoxNo, int Timeout)
	{
		
		//TokenReader_Log("********************** [DispanseTokenPhase1()] Method Call Start********************** ");
		TokenReader_Log(DEBUG, "********************** [DispanseTokenPhase1()] Method Call Start********************** ");
		
		if(Timeout<=0)
		{
			//TokenReader_Log("[DispanseTokenPhase1()] Wrong Timeout value.");
			TokenReader_Log(ERROR, "[DispanseTokenPhase1()] Wrong Timeout value.");
			return 31;
		}
		else if(false == isdeviceConnected())
		{
			//Device Not Connected
			//TokenReader_Log("[DispanseTokenPhase1()] Device Not Connected.");
			TokenReader_Log(ERROR, "[DispanseTokenPhase1()] Device Not Connected.");
			return 20;
		}
		else 
		{
			if(17000 > Timeout)
			{
				Timeout = 17000;
			}
			
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							long timeout_dispansePh1_start = 0;
							long timeout_dispansePh1_end = 0;
				
							timeout_dispansePh1_start = System.currentTimeMillis();
							rtvalue = DispanseTokenPhase1_V2(BoxNo, 16500);
							timeout_dispansePh1_end = System.currentTimeMillis();
							//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase1()] Time taken ph1 = "+(timeout_dispansePh1_end-timeout_dispansePh1_start)+" ms");
							TokenReader_Log(DEBUG, "[TokenDispenser] [DispanseTokenPhase1()] Time taken ph1 = "+(timeout_dispansePh1_end-timeout_dispansePh1_start)+" ms");
							//rtvalue = DispanseTokenPhase1_V3(BoxNo);
							setFlagValue(true);
						}
				
			}); 
			
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase1()] Return Value "+rtvalue);
					setFlagValue(false);
					//TokenReader_Log("********************** [DispanseTokenPhase1()] Method Call End********************** ");
					TokenReader_Log(DEBUG, "********************** [DispanseTokenPhase1()] Method Call End********************** ");
					return rtvalue;
				}
			}
				
			if(true == timeoutFlag)
			{
				//Timeout 
				//TokenReader_Log("********************** [DispanseTokenPhase1()] Method Call End********************** ");
				//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase1()] Operation Timeout Occured ");
				TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase1()] Operation Timeout Occured ");
				TokenReader_Log(ERROR, "********************** [DispanseTokenPhase1()] Method Call End********************** ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				//TokenReader_Log("********************** [DispanseTokenPhase1()] Method Call End********************** ");
				TokenReader_Log(INFO, "********************** [DispanseTokenPhase1()] Method Call End********************** ");
				setFlagValue(false);
				return rtvalue;
			}
		}
	}//public synchronized final int DispanseTokenPhase1(int BoxNo, int Timeout) end
	
	private synchronized final int DispanseTokenPhase1_V2(int BoxNo, long Timeout)
	//public synchronized final int DispanseTokenPhase1(int BoxNo, int Timeout)
	{
			synchronized(this){
				
				//TokenReader_Log("[DispanseTokenPhase1_V2()] Method Call ");
				
				//++Currency CurrencyObj = new Currency(1, 1);
				Currency CurrencyObj = new Currency();
				long[] start_time_arr = CurrencyObj.getTime();
				long DispanseTokenPhase1_V2_start = System.currentTimeMillis();

				int rtCode = 10;
				byte beforeDispense_tokenCount = 0;
				byte afterDispense_tokenCount = 0;
				
				String pathstat = "";
				pathstat = getPathStatus_string();
				/*
				if('1' == pathstat.charAt(4))
				{
					TokenReader_Log("Reject Bin Full. ChannelStat = "+pathstat);
					return 6;
				}
				*/
				rtCode = fn_ClearpathStatus();
				/* Clear Path Status Repeatedly*/
				if(0 != rtCode)
				{
					//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] clearfn error : "+rtCode);
					TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] ClearpathStatus error : "+rtCode);
					long trytimeout = 2000;
					long start_checktime = System.currentTimeMillis();
					while(System.currentTimeMillis() > (start_checktime+trytimeout))
					{
						
						try
						{
							Thread.sleep(50);
						}
						catch(InterruptedException ex)
						{
							//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
							TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
						}
						
						rtCode = fn_ClearpathStatus();
						
						if(0 == rtCode)
						{
							//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] clear done after: "+(System.currentTimeMillis()-start_checktime));
							TokenReader_Log(DEBUG, " [ DispanseTokenPhase1_V2() ] clear done after: "+(System.currentTimeMillis()-start_checktime));
							break;
						}
					}
				}
				
				try
				{
					Thread.sleep(timeout);
				}
				catch(InterruptedException ex)
				{
					//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
					TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
				}
				
				//rtCode = getPathStatus();
				//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] time1 : "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
				
				/* Check Path Status Repeatedly/
				if(0 != rtCode)
				{
					TokenReader_Log(DEBUG, " [ DispanseTokenPhase1_V2() ] pathstat error : "+rtCode);
					long trytimeout = 2000;
					long start_checktime = System.currentTimeMillis();
					while(System.currentTimeMillis() > (start_checktime+trytimeout))
					{
						
						try
						{
							Thread.sleep(50);
						}
						catch(InterruptedException ex)
						{
							//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex);
							TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
						}
						
						rtCode = getPathStatus();
						
						if(0 == rtCode)
						{
							//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] clear done after: "+(System.currentTimeMillis()-start_checktime));
							TokenReader_Log(DEBUG, " [ DispanseTokenPhase1_V2() ] clear done after: "+(System.currentTimeMillis()-start_checktime));
							break;
						}
					}
				}
				*/
				
				if(0 == rtCode)
				{
					try
					{
						Thread.sleep(timeout);
					}
					catch(InterruptedException ex)
					{
						//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex);
						TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex);
					}
					
					rtCode = HopperStatus();
					TokenReader_Log(DEBUG, " [ DispanseTokenPhase1_V2() ] time2 : "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
					if(0 == rtCode)
					{
						TokenReader_Log(DEBUG, " [TokenDispenser] [ DispanseTokenPhase1_V2() ] Token Counter = "+tokenCounter);
						beforeDispense_tokenCount = tokenCounter;
						if((byte)2 == (tokenUnpaid)*0) //&& 
						   //((byte)1 <= tokenCounter))
						{
							//Box Empty
							TokenReader_Log(ERROR, " [TokenDispenser] [ DispanseTokenPhase1_V2() ] Token Container Empty. ");
							return 3;
						}
						else
						{
							try
							{
								Thread.sleep(timeout);
							}
							catch(InterruptedException ex)
							{
								//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex);
								TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
							}
							
							byte[] rtByte = getCipherKey();
							//TokenReader_Log(" [ DispanseTokenPhase1_V2() ] time3 : "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
							if(2 == rtByte[0])
							//if(1 == 0)
							{
								TokenReader_Log(ERROR, " [TokenDispenser] [ DispanseTokenPhase1_V2() ] Other Error. ");
								//TokenReader_Log(" [TokenDispenser] [ DispanseTokenPhase1() ] Cypher Key: "+Arrays.toString(rtByte));
								return 31;
							}
							else if(1 == rtByte[0])
							{
								//Communication Failure
								TokenReader_Log(ERROR, " [TokenDispenser] [ DispanseTokenPhase1_V2() ] Communication Failure. ");
								return 28;
							}
							else
							{	
								try
								{
									Thread.sleep(timeout);
								}
								catch(InterruptedException ex)
								{
									TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
								}
								
								rtCode = DispenseToken(rtByte);
								TokenReader_Log(DEBUG, " [ DispanseTokenPhase1_V2() ] time4: "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
								if(0 == rtCode)
								{
									try
									{
										Thread.sleep(300);
									}
									catch(InterruptedException ex)
									{
										TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
									}
									
									rtCode = getPathStatus();
									long currentTime = System.currentTimeMillis();
									//TokenReader_Log("Timeout for token dispense : "+Timeout);
									Timeout = Timeout-(currentTime - DispanseTokenPhase1_V2_start);
									//TokenReader_Log("Timeout for appearing in staging area : "+Timeout);
									while(0 == rtCode)
									{
										if(Timeout > (System.currentTimeMillis()-currentTime))
										{
											try
											{
												Thread.sleep(timeout);
											}
											catch(InterruptedException ex)
											{
												TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex.getMessage());
											}
											rtCode = getPathStatus();
										}
										else
										{
											//Timeout for Dispense
											//TokenReader_Log("else block");
											//TokenReader_Log("Timeout for appearing in staging area : "+Timeout);
											TokenReader_Log(DEBUG, " [ DispanseTokenPhase1_V2() ] time5: "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
											TokenReader_Log(DEBUG, "[ DispanseTokenPhase1_V2() ] No Token in Staging area, Going to check hopper status.");
											HopperStatus();
											return 3;
										}
									}
									
									if(8 == rtCode)
									{
										TokenReader_Log(INFO, " [ DispanseTokenPhase1_V2() ] time5: "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
										TokenReader_Log(INFO, "[ DispanseTokenPhase1_V2() ] Token found Staging area, Going to check hopper status."+rtCode);
										HopperStatus();
										return 0;
									}
									else
									{
										TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] time5: "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
										TokenReader_Log(ERROR, "[ DispanseTokenPhase1_V2() ] Failure DispenseToken, Going to check hopper status."+rtCode);
										HopperStatus();
										while(0 == rtCode)
										{
										
											if(Timeout > (System.currentTimeMillis()-currentTime))
											{
												try
												{
													Thread.sleep(timeout);
												}
												catch(InterruptedException ex)
												{
													TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] Exception in Thread sleep : "+ex);
												}
												
												rtCode = getPathStatus();
												
											}
											else
											{
												//Timeout for Dispense
												//TokenReader_Log("else block");
												//TokenReader_Log("Timeout for appearing in staging area : "+Timeout);
												TokenReader_Log(ERROR, " [ DispanseTokenPhase1_V2() ] time5: "+(System.currentTimeMillis()-DispanseTokenPhase1_V2_start));
												TokenReader_Log(ERROR, "[ DispanseTokenPhase1_V2() ] No Token in Staging area, Going to check hopper status.");
												HopperStatus();
												
												if(rtCode == 0)
												{
													//String path = getPathStatus_string();
													if('1' == getPathStatus_string().charAt(3))
													{
														//Token Box Empty
														TokenReader_Log(ERROR, "Box Empty");
														return 3;
													}
													else
													{
														//Operation Failed
														//TokenReader_Log(ERROR, "Box Empty");
														return 1;
													}
												}
												else
												{
													TokenReader_Log(ERROR, "Box Empty");
													return rtCode;
												}
											}
										}
										return rtCode;
									}
								}
								else
								{
									TokenReader_Log(ERROR, "[ DispanseTokenPhase1_V2() ] Error in Dispense Token.");
									return rtCode; //28 or 31
								}						
							}
						}
					}
					else
					{
						TokenReader_Log(ERROR, "[ DispanseTokenPhase1_V2() ] HopperStatus Error.");
						return rtCode; //28 or 31
					}
					
				}
				else
				{
					String pathStat = getPathStatus_string();
					if('0' == pathStat.charAt(0))
					{
						if('1' == pathStat.charAt(7))
						{
							//Channel Blocked
							TokenReader_Log(ERROR, "[ DispanseTokenPhase1_V2() ]Channel Blocked.");
							return 2;
						}
						//Other Error
						//TokenReader_Log(ERROR, "[ DispanseTokenPhase1_V2() ] Error Reading Channel Path Stat");
						return 31;
					}
					else
					{
						if('2' == pathStat.charAt(0))
						{
							//Communication Failure
							TokenReader_Log(ERROR, "[ DispanseTokenPhase1_V2() ] Error Reading Channel Path Stat");
							return 28;
						}
						//Other Error
						return 31;
					}
				}
			}//synchronized
			
	}// public int DispanseTokenPhase1(int BoxNo, int Timeout) end
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  Method Name:DispanseTokenPhase2
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel
  		4-Operation timeout occured  , 5-Other error
*/
	
	public final int DispenseTokenPhase2(int BoxNo, int TokenDest, int Timeout)
	{	
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, " [DispanseTokenPhase2()] Wrong Timeout Value.");
			return 31;
		}
		else if(false == isdeviceConnected())
		{
			//Device Not Connected
			TokenReader_Log(ERROR, " [DispanseTokenPhase2()] Device Not Connected yet.");
			return 20;
		}
		/*
		else if('1' == getPathStatus_string.charAt(4))
		{
			TokenReader_Log("Reject Bin Full. ChannelStat = "+getPathStatus_string.charAt(4));
			return 31;
		}
		*/
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							long timeout_dispansePh1_start = 0;
							long timeout_dispansePh1_end = 0;
				
							timeout_dispansePh1_start = System.currentTimeMillis();
							rtvalue = DispanseTokenPhase2_V2(BoxNo, TokenDest);
							timeout_dispansePh1_end = System.currentTimeMillis();
							TokenReader_Log(DEBUG, "[TokenDispenser] [DispanseTokenPhase1()] Time taken ph2 = "+(timeout_dispansePh1_end-timeout_dispansePh1_start)+" ms");
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase2()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				return rtvalue;
			}
		}
	}// public synchronized final int DispanseTokenPhase2(int BoxNo, int TokenDest, int Timeout) end
	
	private synchronized final int DispanseTokenPhase2_V2(int BoxNo, int TokenDest)
	//public synchronized final int DispanseTokenPhase2(int BoxNo, int TokenDest, int Timeout)
	{
		synchronized(this){
			//int counter = 0;
			fn_ClearpathStatus();
			String rtString = "";
			
			long currentTime = System.currentTimeMillis();
			while(1000 > (System.currentTimeMillis()-currentTime))
			{
				try
				{
					Thread.sleep(timeout);
				}
				catch(InterruptedException ex)
				{
					TokenReader_Log(ERROR, " [ ConnectDevice() ] Exception in Thread sleep : "+ex.getMessage());
				}
				rtString = getPathStatus_string();
				
				if('0' == rtString.charAt(0))
				{
					break;
				}
			}
			
			//Check status 
			if('0' == rtString.charAt(0))
			{
				TokenReader_Log(DEBUG, "[TokenDispenser] [DispanseTokenPhase2()] Read Channel Status succesful. ");
				//Token Presence
				if('1' == rtString.charAt(7))
				{
					counter = 0;
					TokenReader_Log(DEBUG, "[TokenDispenser] [DispanseTokenPhase2()] Token found in statging area. ");
					//Check Channel
					if('0' == rtString.charAt(5) && ('0' == rtString.charAt(6)))
					{	
						String chk_after_action = null;
						try
						{
							Thread.sleep(timeout);
						}
						catch(InterruptedException ex)
						{
							TokenReader_Log(ERROR, " [ ConnectDevice() ] Exception in Thread sleep : "+ex.getMessage());
						}
						//TokenDest
						switch(TokenDest)
						{
							//Issue
							case 1:	issueToken();
									
								try
								{
									Thread.sleep(timeout);
								}
								catch(InterruptedException ex)
								{
									TokenReader_Log(ERROR, " [ DispanseTokenPhase2() ] Exception in Thread sleep : "+ex.getMessage());
								}
								chk_after_action = getPathStatus_string();
								if('1' == chk_after_action.charAt(6))//&& '0' == chk_after_action.charAt(5))
								{
									//Success
									TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Token Issue Successful. ");
									TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Time For Issue: "+(currentTime-System.currentTimeMillis()));
									//int rt = fn_ClearpathStatus();
									//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase2()] Issue PathClear rtCode = "+rt);
									return 0;
								}
								else
								{
									//Operation Failed
									//Retry
									try
									{
										Thread.sleep(500);
									}
									catch(InterruptedException ex)
									{
										TokenReader_Log(ERROR, " [ DispanseTokenPhase2() ] Exception in Thread sleep : "+ex.getMessage());
									}
									issueToken();
									chk_after_action = getPathStatus_string();
									if('1' == chk_after_action.charAt(6))//&& '0' == chk_after_action.charAt(5))
									{
										//Success
										TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Token Issue Successful. ");
										TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Time For Issue: "+(currentTime-System.currentTimeMillis()));
										//int rt = fn_ClearpathStatus();
										//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase2()] Issue PathClear rtCode = "+rt);
										return 0;
									}
									else{
										TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Token Issue Fail. ");
										return 1;
									}
								}
								
							//Reject
							case 2:	
								if('1' == rtString.charAt(4))
								{
									//other error
									TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Rejection bin full ");
									return 31;
								}
								else
								{
									rejectToken();
									
									try
									{
										Thread.sleep(timeout);
									}
									catch(InterruptedException ex)
									{
										TokenReader_Log(ERROR, " [ DispanseTokenPhase2() ] Exception in Thread sleep : "+ex.getMessage());
									}
									chk_after_action = getPathStatus_string();
									if('1' == chk_after_action.charAt(5)) //&& '0' == chk_after_action.charAt(6))
									{
										//Success
										TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Token Reject Successful. ");
										TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Time For Reject: "+(currentTime-System.currentTimeMillis()));
										//int rt = fn_ClearpathStatus();
										//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase2()] Reject PathClear rtCode = "+rt);
										return 0;
									}
									else
									{
										//Operation Failed
										//Retry
										try
										{
											Thread.sleep(500);
										}
										catch(InterruptedException ex)
										{
											TokenReader_Log(ERROR, " [ DispanseTokenPhase2() ] Exception in Thread sleep : "+ex.getMessage());
										}
										rejectToken();
										if('1' == chk_after_action.charAt(5)) //&& '0' == chk_after_action.charAt(6))
										{
											//Success
											TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Token Reject Successful. ");
											TokenReader_Log(INFO, "[TokenDispenser] [DispanseTokenPhase2()] Time For Reject: "+(currentTime-System.currentTimeMillis()));
											//int rt = fn_ClearpathStatus();
											//TokenReader_Log("[TokenDispenser] [DispanseTokenPhase2()] Reject PathClear rtCode = "+rt);
											return 0;
										}
										else{
											TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Token Reject Fail. ");
											return 1;
										}
									}
								}
							//Other Error
							default: TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Wrong Token Dest. ");
							return 31;
						}
					}
					else
					{
						//Other Error
						TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Other Error Issue/Reject ");
						return 31;
					}
				}
				else
				{
					//Other Error
					TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] No Token in statging area. ");
					return 3;
				}
			}
			else
			{
				if('2' == rtString.charAt(0))
				{
					//Communication Failure
					TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Read Channel Status Comm Faiure. ");
					return 28;
				}
				//Other Error
				TokenReader_Log(ERROR, "[TokenDispenser] [DispanseTokenPhase2()] Read Channel Status failed. ");
				return 31;
			}
		}
			//return 0;
	}// public int DispanseTokenPhase2(int BoxNo, int TokenDest, int Timeout) end
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  Method Name:EmptyTokenBox
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel
  		4-Operation timeout occured  
*/
	/*
	public final int EmptyTokenBox(int BoxNo, int TokenDest, int Timeout)
	{
		if(Timeout<=0)
		{
			TokenReader_Log("[EmptyTokenBox()] Other Error Occured.");
			return 5;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = DispanseTokenPhase2_V2(BoxNo, TokenDest);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log("[TokenDispenser] [DispanseTokenPhase2()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					return rtvalue;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				TokenReader_Log("[TokenDispenser] [DispanseTokenPhase2()] Operation Timeout Occured ");
				return 4;
			}
			else
			{
				return rtvalue;
			}
		}
	}// public final int EmptyTokenBox(int BoxNo, int TokenDest, int Timeout) end
	*/
	//private synchronized final int EmptyTokenBox(int BoxNo, int TokenDest)
	public synchronized final int EmptyTokenBox(int BoxNo, int TokenDest, int Timeout)
	{
			synchronized(this){
			
				int rtCode = 10;
				long startATOKENtime = 0;
				long finishATOKENtime = 0;
				
				if(false == isdeviceConnected())
				{
					//Device not Connected
					TokenReader_Log(ERROR, "[EmptyTokenBox()] Device not Connected yet.");
					return 20;
				}
				
				if(Timeout > 0)
				{
					long endTime = System.currentTimeMillis()+Timeout;
					
					while(true)
					{
						if(System.currentTimeMillis() <= endTime)
						{
							startATOKENtime = finishATOKENtime = 0;
							/*
							try
							{
								Thread.sleep(50);
							}
							catch(InterruptedException ex)
							{
								TokenReader_Log(" [ ConnectDevice() ] Exception in Thread sleep : "+ex);
							}
							*/
							startATOKENtime = System.currentTimeMillis();
							rtCode = DispanseTokenPhase1_V2(BoxNo, 16500);
							if(0 == rtCode)
							{
								/*
								try
								{
									Thread.sleep(10);
								}
								catch(InterruptedException ex)
								{
									TokenReader_Log(" [ ConnectDevice() ] Exception in Thread sleep : "+ex);
								}
								*/
								rtCode = 10;
								rtCode = DispanseTokenPhase2_V2(BoxNo, TokenDest);
								finishATOKENtime = System.currentTimeMillis();
								//TokenReader_Log("Time Taken for a token dispense : "+(float)((finishATOKENtime-startATOKENtime)/1000));
								TokenReader_Log(DEBUG, "[ EmptyTokenBox() ] Time Taken for a token dispense : "+(finishATOKENtime-startATOKENtime));
								if(0 != rtCode)
								{
									TokenReader_Log(DEBUG, " [ EmptyTokenBox() for loop] Return Code : "+rtCode);
									if(3 == rtCode)
									{
										
										long start_statusread = System.currentTimeMillis();
										long end_statusread = System.currentTimeMillis()+2100;
										
										while(end_statusread > System.currentTimeMillis())
										{
											try
											{
												Thread.sleep(50);
											}
											catch(InterruptedException ex)
											{
												TokenReader_Log(ERROR, " [ EmptyTokenBox() ] Exception in Thread sleep : "+ex.getMessage());
											}
											
											rtCode = DispanseTokenPhase2_V2(BoxNo, TokenDest);
											
											if(3 != rtCode)
											{
												if(0 == rtCode)
												{
													TokenReader_Log(INFO, "[ EmptyTokenBox() ] Token found after "+(System.currentTimeMillis() - start_statusread));
													break;
												}
												else
												{
													//Operation Failed
													TokenReader_Log(ERROR, "[ EmptyTokenBox() ] could not found Token "+(System.currentTimeMillis() - start_statusread));
													return 1;
												}
											}
										}
										
										/*
										try
										{
											Thread.sleep(50);
										}
										catch(InterruptedException ex)
										{
											TokenReader_Log(" [ ConnectDevice() ] Exception in Thread sleep : "+ex);
										}
										
										rtCode = DispanseTokenPhase2_V2(BoxNo, TokenDest);
										if(0 == rtCode)
										{
											continue;
										}
										else
										{
											if(3 == rtCode)
											{
												try
												{
													Thread.sleep(60);
												}
												catch(InterruptedException ex)
												{
													TokenReader_Log(" [ ConnectDevice() ] Exception in Thread sleep : "+ex);
												}
												rtCode = DispanseTokenPhase2_V2(BoxNo, TokenDest);
												if(0 == rtCode)
												{
													continue;
												}
												else
												{
													TokenReader_Log(" [ EmptyTokenBox() for loop] Return Code after retry : "+rtCode);
													return 2;
												}
											}
											return 2;
										}
										*/
									}
									
									if(0 != rtCode)
									{
										if(3 == rtCode)
										{
											//Operation Failed
											return 1;
										}
										TokenReader_Log(ERROR, " [ EmptyTokenBox() ] Failed Return Code : "+rtCode);
										return rtCode;
									}
								}
							}
							else if(3 == rtCode)
							{
								TokenReader_Log(INFO, " [ EmptyTokenBox() for loop] Successfull : "+rtCode);
								return 0;																	// Success
							}
							
							else
							{
								TokenReader_Log(DEBUG, " [ EmptyTokenBox() for loop] : "+rtCode);
								return rtCode;
							}
						}
						else
						{
							//Timeout
							TokenReader_Log(DEBUG, " [ EmptyTokenBox() ] RTCODE: "+rtCode);
							return 18;																		// Timeout occurred
						}
					}
				}
				else
				{
					//Other Error
					TokenReader_Log(DEBUG, " [ EmptyTokenBox() ] RTCODE: "+rtCode);
					return 31;																				//Other Error
				}
				
			}
			//return 0;
	}// public int EmptyTokenBox(int BoxNo, int TokenDest, int Timeout) end
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  Method Name:ClearJammedToken
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/
	public final int ClearJammedToken(int BoxNo, int TokenDest, int Timeout)
	{
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, "[DispanseTokenPhase2()] Wrong Timeout Value.");
			return 31;
		}
		if(false == isdeviceConnected())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[DispanseTokenPhase2()] Device not Connected yet.");
			return 20;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = ClearJammedToken(BoxNo, TokenDest);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(INFO, "[TokenDispenser] [ClearJammedToken()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[TokenDispenser] [ClearJammedToken()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				return rtvalue;
			}
		}
	}// public final int ClearJammedToken(int BoxNo, int TokenDest, int Timeout) end
	
	private synchronized final int ClearJammedToken(int BoxNo, int TokenDest)
	//public synchronized final int ClearJammedToken(int BoxNo, int TokenDest, int Timeout)
	{
			synchronized(this){
			
				int rtCode = 10;
				String rtString = "null";
				rtString = getPathStatus_string();
				if('0'== rtString.charAt(0))
				{
					if('0' == rtString.charAt(7))
					{
						//No Token Found
						return 3;
					}
					
					if('0'== rtString.charAt(5) && '0'== rtString.charAt(6))
					{
						rtCode = DispanseTokenPhase2_V2(BoxNo, TokenDest);
						if(rtCode == 0)
						{
							TokenReader_Log(INFO, "[ClearJammedToken()] Token cleared");
							return 0;																							// Success
							
						}
						else
						{
							//rtCode = getPathStatus_string();
							//isTokeninStagingArea();
							return rtCode;
						}
					}
					else
					{
						TokenReader_Log(ERROR, "[ClearJammedToken()] Channel Blocked");
						return 2;
					}
					
				}
				else
				{
					TokenReader_Log(ERROR, "[ClearJammedToken()] Channel Status Read Error");
					return rtCode;
				}
				
			}
			//return 0;
	}// public int ClearJammedToken(int BoxNo, int TokenDest, int Timeout) end
	

	public final String GetDeviceFWVersion()
	{
		String s = TokenDispenserReaderFWVersion();
		TokenReader_Log(INFO, "FW Version "+"00.00.00");
		return "00.00.00";
	}//public final String GetDeviceFWVersion()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	public final int ResetDevice(int timeout){
		int rt = 0;
		//Reset Hopper
		rt = ResetHopper();
		try{
			Thread.sleep(300);
		}
		catch(InterruptedException ex){
			TokenReader_Log(ERROR, " [ ResetDevice() ] Exception in Thread sleep : "+ex.getMessage());
		}
		//Enable Hopper
		if(rt == 0){
			rt = EnableHopper();
		}
		else{
			//Failure
			return 1;
		}
		//Clear Jammed Token(if any)
		if(rt == 0){
			String rt_st = getPathStatus_string();
			
			if('1' == rt_st.charAt(8)){
					rt = rejectToken();
					try{
						Thread.sleep(20);
					}
					catch(InterruptedException ex){
						TokenReader_Log(ERROR, " [ ResetDevice() ] Exception in Thread sleep : "+ex.getMessage());
					}
					//if(100 != rt){
						rt_st = getPathStatus_string();
						if('1' == rt_st.charAt(5)){
							//Reset Channel
							TokenReader_Log(INFO, "Token Rejected.");
							rt = fn_ClearpathStatus();
							if(0 == rt){
								//Success
								TokenReader_Log(INFO, "Path Cleared.");
								return 0;
							}
							else{
								//Failure
								TokenReader_Log(ERROR, "Path not Cleared.");
								return 1;
							}
						}
						else{
							//Failure
							TokenReader_Log(ERROR, "Wrong Dest.");
							return 1;
						}
					//}
					/*else{
						//Failure
						TokenReader_Log(ERROR, "Reject Failed.");
						return 1;
					}
					*/
				}
				else{
					rt = fn_ClearpathStatus();
					if(0 == rt){
						//Success
						TokenReader_Log(INFO, "Path Cleared.");
						return 0;
					}
					else{
						//Failure
						TokenReader_Log(ERROR, "Path not Cleared.");
						return 1;
					}
				}
		}
		else{
			return 1;//Failure
		}
	}//public final int ResetDevice() end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  Method Name:ConnectReaderDevice
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/
	
	public final int ConnectDeviceReader(int DeviceId, int PortId, int Timeout)
	{
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, "[ConnectDeviceReader()] Wrong Timeout Value.");
			return 31;
		}
		//for finding if token is in reader or not
		/*
		else if(false == isdeviceConnected())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectDeviceReader()] Main Device not Connected.");
			return 31;
		}
		* */
		else if(true == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectDeviceReader()] Device already Connected.");
			return 20;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = ConnectReaderDevice(DeviceId, PortId);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(INFO, " [ConnectDeviceReader()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[ConnectDeviceReader()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				return rtvalue;
			}
		}
	}
	
	private synchronized final int ConnectReaderDevice(int DeviceId, int PortId)
	{
		synchronized(this){
			terminalFactory_obj = TerminalFactory.getDefault();
			try 
			{
				cardTerminals = terminalFactory_obj.terminals().list();
				TokenReader_Log(DEBUG, "[ConnectReaderDevice()] CardTerminals "+cardTerminals);
			} 
			catch (CardException ex) 
			{
				// TODO Auto-generated catch block
				//e.printStackTrace();
				TokenReader_Log(ERROR, "[ConnectReaderDevice()] CardTerminal Exception "+ex.getMessage());
				return 28;
			}
			
			if(null != cardTerminals && false == cardTerminals.isEmpty())
			{
				contactless_cardTerminal_obj = cardTerminals.get(0);
				SAM1_cardTerminal_obj = cardTerminals.get(2);
				SAM2_cardTerminal_obj = cardTerminals.get(1);
				
				if(null != contactless_cardTerminal_obj)
				{
					TokenReader_Log(INFO, "[ConnectReaderDevice()] Card Terminal : "+contactless_cardTerminal_obj);
					//setReaderFlag(true);
					//return 0;														//Success
				}
				else
				{
					TokenReader_Log(ERROR, "[ConnectReaderDevice()] Card Terminal : "+contactless_cardTerminal_obj);
					return 31;														//Failure
				}
				
				if(null != SAM1_cardTerminal_obj)
				{
					TokenReader_Log(INFO, "[ConnectReaderDevice()] Card Terminal : "+SAM1_cardTerminal_obj);
					//setReaderFlag(true);
					//return 0;														//Success
				}
				else
				{
					TokenReader_Log(ERROR, "[ConnectReaderDevice()] Card Terminal : "+SAM1_cardTerminal_obj);
					return 31;														//Failure
				}
				
				if(null != SAM2_cardTerminal_obj)
				{
					TokenReader_Log(INFO, "[ConnectReaderDevice()] Card Terminal : "+SAM2_cardTerminal_obj);
					setReaderFlag(true);
					return 0;														//Success
				}
				else
				{
					TokenReader_Log(ERROR, "[ConnectReaderDevice()] Card Terminal : "+SAM2_cardTerminal_obj);
					return 31;														//Failure
				}
			}
			else
			{
				TokenReader_Log(ERROR, "[ConnectReaderDevice()] Failed to get readers.");
				return 31;															//Failure
			}
		}
		//return 31;	
	}
	
	private synchronized void setReaderFlag(boolean flag)
	{
		synchronized(this)
		{
			readercnctFlag = flag;
			//TokenReader_Log(DEBUG, "[setReaderFlag()] flag is "+readercnctFlag);
		}
	}// private synchronized void setReaderFlag(boolean flag) end
	
	private synchronized boolean getReaderFlag()
	{
		synchronized(this)
		{
			return readercnctFlag;
		}
	}// private synchronized boolean getReaderFlag() end

/**
  Method Name:ActivateCard
  Return Type:byte[]
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/
	
	public final byte [] ActivateCard(int DeviceId, int CardTechType,int SAMSlotId, int Timeout)
	{
		returnReaderArray = new byte[10];
		
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Wrong Timeout Value.");
			returnReaderArray[0]=(byte)31;
			return returnReaderArray;
		}
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Device already Connected.");
			returnReaderArray[0]=(byte)20;
			return returnReaderArray;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							returnReaderArray = ActivateCard(DeviceId, CardTechType, SAMSlotId);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(INFO, " [ConnectReaderDevice()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return returnReaderArray;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[ConnectReaderDevice()] Operation Timeout Occured ");
				setFlagValue(false);
				returnReaderArray[0]=(byte)18;
				return returnReaderArray;
			}
			else
			{
				setFlagValue(false);
				return returnReaderArray;
			}
		}
	}
	
	private synchronized byte [] ActivateCard(int DeviceId, int CardTechType,int SAMSlotId)
	{
		byte[] returnarray = new byte[10];
		CardTerminal terminal = null;
		if(0 == CardTechType)
		{
			terminal = contactless_cardTerminal_obj;
			TokenReader_Log(DEBUG, "[ActivateCard()] cardTerminal_obj is null.");
		}
		else
		{
			if(1 == SAMSlotId){
				terminal = SAM1_cardTerminal_obj;
				TokenReader_Log(DEBUG, "[ActivateCard()] SAM1 "+terminal);
			}
			else if(2 == SAMSlotId){
				terminal = SAM2_cardTerminal_obj;
				TokenReader_Log(DEBUG, "[ActivateCard()] SAM2 "+terminal);
			}
			else{
				TokenReader_Log(ERROR, "[ActivateCard()] Wrong SAMslot");
				returnarray[0]=(byte)31;
				return returnarray;
			}
		}
		
		if(null == terminal)
		{
			TokenReader_Log(ERROR, "[ActivateCard()] cardTerminal_obj is null.");
			returnarray[0]=(byte)31;
			return returnarray;
		}
		
		//Connect to card
		try 
		{
			card_obj = terminal.connect("*");
			TokenReader_Log(INFO, "[ActivateCard()] card connected.");
			returnarray[0]=(byte)0;
			byte[] atr = card_obj.getATR().getBytes();
			if(1 == CardTechType){
			//Contact Card type
			//SAM Type
			String samatr = new String(atr).toLowerCase();
				if(true == samatr.contains(AV2))
				{
					TokenReader_Log(INFO, "[ActivateCard()] MIFARE SAM AV2");
					returnarray[1] = (byte)2;
				}
				else if(true == samatr.contains(AV1))
				{
					TokenReader_Log(INFO, "[ActivateCard()] MIFARE SAM AV1");
					returnarray[1] = (byte)1;
				}
				else
				{
					TokenReader_Log(ERROR, "[ActivateCard()] Unknown SAM");
					returnarray[1] = (byte)0;
				}
			}
			else{
			//ContactLess Card type
				
				if(null != atr && 20==atr.length)
				{
					if(0==atr[13] && 1==atr[14])
					{
						TokenReader_Log(INFO, "[ActivateCard()] Contactless card type Classic 1K");
						returnarray[1]=(byte)01;
					}
					else if(0==atr[13] && 2==atr[14])
					{
						TokenReader_Log(INFO, "[ActivateCard()] Contactless card type MIFARE Classic 4K");
						returnarray[1]=(byte)02;
					}
					else if(0==atr[13] && 3==atr[14])
					{
						//Ultralight
						TokenReader_Log(INFO, "[ActivateCard()] Contactless card type MIFARE Ultralight");
						returnarray[1]=(byte)03;
					}
					else
					{
						TokenReader_Log(ERROR, "[ActivateCard()] Contactless card type unknown");
						returnarray[1]=(byte)0;
					}
					
				}
				else{
					TokenReader_Log(ERROR, "[ActivateCard()] Contactless card type unknown");
					returnarray[0]=(byte)2;
					return returnarray;
				}
			}
			//find uid
			CardChannel channel = card_obj.getBasicChannel();
			if(channel == null)
			{
				TokenReader_Log(ERROR, "[ActivateCard()] Contactless card type unknown");
				return returnarray;
			}
			byte[] UID = channel.transmit(new CommandAPDU(0xFF, 0xCA, 0x00, 0x00, 0x00)).getBytes();
			TokenReader_Log(DEBUG, "[ActivateCard()] UID: "+Arrays.toString(UID));
			if(null != UID && 0!=UID.length){
				if((byte)144 == UID[UID.length -2] && (byte)0 == UID[UID.length -1])
				{
					returnarray[2]=(byte)(UID.length-2);
					System.arraycopy(UID, 0, returnarray, 3, (UID.length-2));
					return returnarray;
				}				
				/*
				else
				{
					
				}
				*/
			}
			return returnarray;
			//find uid
			
		} 
		catch (NullPointerException e) 
		{
			// TODO Auto-generated catch block
			TokenReader_Log(ERROR, "[ActivateCard()] Card NullPointerException: "+e.getMessage());
			returnarray[0]=(byte)31;
			return returnarray;
		}
		catch (IllegalArgumentException e) 
		{
			// TODO Auto-generated catch block
			TokenReader_Log(ERROR, "[ActivateCard()] IllegalArgumentException: "+e.getMessage());
			returnarray[0]=(byte)31;
			return returnarray;
		}
		catch (CardNotPresentException e) 
		{
			// TODO Auto-generated catch block
			TokenReader_Log(ERROR, "[ActivateCard()] CardNotPresentException: Failed to connect the card, no card is present in this terminal");
			returnarray[0]=(byte)31;
			return returnarray;
		}
		catch (CardException e) 
		{
			
			TokenReader_Log(ERROR, "[ActivateCard()] CardException: "+ e.getMessage());
			if('1' == getPathStatus_string().charAt(7)){
				TokenReader_Log(ERROR, "[ActivateCard()] Card found but activation failed");
				returnarray[0]=(byte)1;
				return returnarray;
			}
			else{
				TokenReader_Log(ERROR, "[ActivateCard()] No card found");
				returnarray[0]=(byte)10;
				return returnarray;
			}
			
		}
		catch (SecurityException e) 
		{
			// TODO Auto-generated catch block
			TokenReader_Log(ERROR, "[ActivateCard()] SecurityException: Failed to connect the card");
			returnarray[0]=(byte)31;
			return returnarray;
		}
	}// public synchronized byte [] ActivateCard(int DeviceId, int CardTechType,int SAMSlotId, int Timeout) end
	
/**
  Method Name:SAMSlotPowerOnOff
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/
	public final int SAMSlotPowerOnOff(int DeviceId, int SAMSlotId,int PowerOnOffState, int Timeout)
	{
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Wrong Timeout Value.");
			return 31;
		}
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Device already Connected.");
			return 20;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = SAMSlotPowerOnOff(DeviceId, SAMSlotId, PowerOnOffState);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(INFO, " [ConnectReaderDevice()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[ConnectReaderDevice()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				return rtvalue;
			}
		}
	}
	
	private synchronized final int SAMSlotPowerOnOff(int DeviceId, int SAMSlotId,int PowerOnOffState)
	{
		switch(PowerOnOffState){
			
			case 0:	TokenReader_Log(INFO," [SAMSlotPowerOnOff()] PowerOnState");
					TokenReader_Log(ERROR," [SAMSlotPowerOnOff()] PowerOnState");
				if(false == getReaderFlag())
					return 20;
				else
					return DeactivateCard(DeviceId, 1, SAMSlotId);//, Timeout);
				
			case 1: TokenReader_Log(INFO," [SAMSlotPowerOnOff()] PowerOnState");
					TokenReader_Log(ERROR," [SAMSlotPowerOnOff()] PowerOnState");
				if(false == getReaderFlag())
					return 20;
				else{
					byte[] rt = ActivateCard(DeviceId, 1, SAMSlotId);//, Timeout);
					return rt[0];
				}
				//break;
				
			default: TokenReader_Log(ERROR," [SAMSlotPowerOnOff()] Undefiend Power State");
				return 31;
			
		}
	}//public synchronized finalint SAMSlotPowerOnOff() end

/**
  Method Name:ResetSAM
  Return Type:byte[]
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/	
	public final byte [] ResetSAM(int DeviceId, int SAMSlotId, int ResetType, int Timeout)
	{
		returnReaderArray = new byte[10];
		
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Wrong Timeout Value.");
			returnReaderArray[0]=(byte)31;
			return returnReaderArray;
		}
		//for finding if token is in reader or not
		/*
		if(false == isdeviceConnected())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Main Device not Connected.");
			returnReaderArray[0]=(byte)20;
			return returnReaderArray;
		}
		*/
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Device already Connected.");
			returnReaderArray[0]=(byte)20;
			return returnReaderArray;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							returnReaderArray = ResetSAM(DeviceId, SAMSlotId, ResetType);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(INFO, " [ConnectReaderDevice()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return returnReaderArray;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[ConnectReaderDevice()] Operation Timeout Occured ");
				setFlagValue(false);
				returnReaderArray[0]=(byte)18;
				return returnReaderArray;
			}
			else
			{
				setFlagValue(false);
				return returnReaderArray;
			}
		}
	}
	
	private synchronized final byte [] ResetSAM(int DeviceId, int SAMSlotId, int ResetType)
	{
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ResetSAM()] Device not Connected.");
			byte[] returnarray = new byte[1];
			returnarray[0]=(byte)20;
			return returnarray;
		}
		CardTerminal terminal = null;
		if(1 == SAMSlotId){
			terminal = SAM1_cardTerminal_obj;
			TokenReader_Log(DEBUG, "[ResetSAM()] SAM1 "+terminal);
		}
		else if(2 == SAMSlotId){
			terminal = SAM2_cardTerminal_obj;
			TokenReader_Log(DEBUG, "[ResetSAM()] SAM2 "+terminal);
		}
		else{
			TokenReader_Log(ERROR, "[ResetSAM()] Wrong SAMslot");
			returnarray[0]=(byte)31;
			return returnarray;
		}
		
		if(null == terminal)
		{
			TokenReader_Log(ERROR, "[ResetSAM()] cardTerminal_obj is null.");
			returnarray[0]=(byte)31;
			return returnarray;
		}
		
		try 
		{
			card_obj = terminal.connect("*");
			TokenReader_Log(INFO, "[ResetSAM()] card connected.");
			returnarray[0]=(byte)0;
			
			//CL Card type
			
			card_obj.disconnect(true);
			card_obj = terminal.connect("*");
			
			byte[] atr = card_obj.getATR().getBytes();
			byte[] sucssarry = new byte[atr.length+1];
			sucssarry[0] = (byte)0;//returnarray
			System.arraycopy(atr, 0, sucssarry, 1, atr.length);
			return sucssarry;
			
		}
		catch(NullPointerException ex)
		{
			TokenReader_Log(ERROR, "[ResetSAM()] NullPointerException: "+ex.getMessage());
			returnarray[0] = (byte)31;
			return returnarray;
		}
		catch(IllegalArgumentException ex)
		{
			TokenReader_Log(ERROR, "[ResetSAM()] IllegalArgumentException: "+ex.getMessage());
			returnarray[0] = (byte)31;
			return returnarray;
		}
		catch(CardNotPresentException ex)
		{
			TokenReader_Log(ERROR, "[ResetSAM()] CardNotPresentException: "+ex.getMessage());
			returnarray[0] = (byte)31;
			return returnarray;
		}
		catch(CardException ex)
		{
			TokenReader_Log(ERROR, "[ResetSAM()] CardException: "+ex.getMessage());
			returnarray[0] = (byte)31;
			return returnarray; 
		}
		catch (SecurityException ex) 
		{
			TokenReader_Log(ERROR, "[ResetSAM()] SecurityException: "+ex.getMessage());
			returnarray[0] = (byte)31;
			return returnarray; 
		}
		
		/*
		TokenReader_Log(INFO, "[ResetSAM()]");
		TokenReader_Log(ERROR, "[ResetSAM()]");
		DisConnectReaderDevice(DeviceId, Timeout);
		int rt = ConnectReaderDevice(DeviceId, 51, Timeout);
		byte[] rtbyte = new byte[30];
		rtbyte[0] = (byte)rt;
		//SAM ATR Required
		return rtbyte;
		*/
	}//public synchronized final byte [] ResetSAM() end
	
/**
  Method Name:DeactivateCard
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/
	public final int DeactivateCard(int DeviceId, int CardTechType, int SAMSlotId, int Timeout)
	{
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Wrong Timeout Value.");
			return 31;
		}
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Device already Connected.");
			return 20;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = DeactivateCard(DeviceId, CardTechType, SAMSlotId);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(INFO, " [ConnectReaderDevice()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[ConnectReaderDevice()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				return rtvalue;
			}
		}
	}
	
	private synchronized final int DeactivateCard(int DeviceId, int CardTechType, int SAMSlotId)
	{
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[ConnectReaderDevice()] Device not Connected.");
			return 20;
		}
		CardTerminal terminal = null;
		if(0 == CardTechType)
		{
			terminal = contactless_cardTerminal_obj;
			TokenReader_Log(DEBUG, "[DeactivateCard()] cardTerminal_obj is null.");
		}
		else
		{
			if(1 == SAMSlotId){
				terminal = SAM1_cardTerminal_obj;
				TokenReader_Log(DEBUG, "[DeactivateCard()] SAM1 "+terminal);
			}
			else if(2 == SAMSlotId){
				terminal = SAM1_cardTerminal_obj;
				TokenReader_Log(DEBUG, "[DeactivateCard()] SAM2 "+terminal);
			}
			else{
				TokenReader_Log(ERROR, "[ActivateCard()] Wrong SAMslot");
				return 31;
			}
		}
		if(null == terminal)
		{
			TokenReader_Log(ERROR, "[DeactivateCard()] cardTerminal_obj is null.");
			return 31;
		}
		
		try {
			if(null != card_obj){
				card_obj.disconnect(false);
				TokenReader_Log(INFO, "[DeactivateCard()] Card found and Disconnected");
				return 0;
			}
			else
			{
				TokenReader_Log(ERROR, "[DeactivateCard()] Card obj is null");
				return 31;
			}
			
		} catch (CardException ex) {
			TokenReader_Log(ERROR, "[DeactivateCard()] "+ex.getMessage());
			if('1' == getPathStatus_string().charAt(7)){
					TokenReader_Log(ERROR, "[DeactivateCard()] Card found but deactivation failed");
					return 1;
			}
			else{
					TokenReader_Log(ERROR, "[DeactivateCard()] No Card found");
					return 10;
			}
		}
	}// public synchronized final int DeactivateCard(int DeviceId, int CardTechType,int SAMSlotId,int Timeout) end
	
/**
  Method Name:ReadUltralightBlock
  Return Type:byte[]
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/

	public final byte[] ReadUltralightBlock(int DeviceId, int Addr, int Timeout)
	{
		for(int in=0; in<readULBLOCKreturnarray.length; in++)
		{
			readULBLOCKreturnarray[in]=(byte)0;
		}
		TokenReader_Log(DEBUG, "[ReadUltralightBlock()] Entry.");
		//int rtvalue = 10;
		if(Timeout<=0)
		{
			//Other Error
			TokenReader_Log(ERROR, "[ReadUltralightBlock()] Wrong Timeout value");
			readULBLOCKreturnarray[0] = (byte)31;
			return readULBLOCKreturnarray;
		}
		else if(false == getReaderFlag())
		{
			//Device Not Connected
			TokenReader_Log(ERROR, "[ReadUltralightBlock()] Device not Connected.");
			readULBLOCKreturnarray[0] = (byte)20;
			return readULBLOCKreturnarray;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							readULBLOCKreturnarray = ReadUltralightBlock(DeviceId, Addr);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(DEBUG, "[ReadUltralightBlock()] Return Value "+rtvalue);
					setFlagValue(false);
					//readULBLOCKreturnarray[0] = (byte)31;
					return readULBLOCKreturnarray;
					//break;
				}
			}
				
			if(true == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[ReadUltralightBlock()] Operation Timeout Occured ");
				setFlagValue(false);
				readULBLOCKreturnarray[0] = (byte)18;
				return readULBLOCKreturnarray;
			}
			else
			{
				setFlagValue(false);
				TokenReader_Log(DEBUG, "[ReadUltralightBlock()] rtvalue "+rtvalue);
				return readULBLOCKreturnarray;
			}
				
		}
	}// public final int ReadUltralightBlock(int Addr, int Timeout) end
	
	private synchronized final byte[] ReadUltralightBlock(int DeviceId, int Addr)
	{
		
		synchronized(this){
			byte[] returnarray = new byte[17];
			
			byte [] SendBuff = new byte[5];
			SendBuff[0] = (byte)0xFF; //CLA
			SendBuff[1] = (byte)0xB0; //INS //B1
			SendBuff[2] = 0x00; //P1
			SendBuff[3] = (byte)Addr;     //P2
			SendBuff[4] = 0x10;             //Le ox00

			//byte [] data = new byte[16];
			if(null == card_obj)
			{
				TokenReader_Log(ERROR, "Card object is Null "+card_obj);
				returnarray[0]=(byte)31;
				return returnarray;
			}
			
			CardChannel channelobj = card_obj.getBasicChannel();
			
			try
			{
				byte[] RespBuffer =channelobj.transmit(new CommandAPDU(SendBuff)).getBytes();
				if(RespBuffer==null || RespBuffer.length==0) {
					TokenReader_Log(ERROR, "[ReadUltralightBlock()] Buffer Length null");
					returnarray[0]=(byte)31;
					return returnarray;
				}
				int RespLen = RespBuffer.length;
				if(RespBuffer[RespLen - 2]!=(byte)0x90 ||RespBuffer[RespLen - 1]!=0x00)
				{
					returnarray[0] = (byte)31;
					TokenReader_Log(ERROR, "[ReadUltralightBlock()] Other Error Length mismatch");
					return returnarray;
				}
				else
				{
					System.arraycopy(RespBuffer, 0, returnarray, 1, 16);
					//success
					returnarray[0] = (byte)0;
					TokenReader_Log(INFO, "[ReadUltralightBlock()] Successfully read data: "+Arrays.toString(returnarray));
					return returnarray;
				}
			}
			catch(CardException ce)
			{
				returnarray[0] = (byte)31;
				TokenReader_Log(ERROR, "[ReadUltralightBlock()] Exception "+ce.getMessage());
				return returnarray;
			}
			catch(IllegalStateException ise)
			{
				returnarray[0] = (byte)31;
				TokenReader_Log(ERROR, "[ReadUltralightBlock()] Exception "+ise.getMessage());
				return returnarray;
			}
		}
		
		//byte[] returnarray = new byte[17];
		//returnarray[0]=(byte)31;
		//return returnarray;
	}// public synchronized final byte[] ReadUltralightBlock(int Addr) end
	
	public boolean WriteUltralightBlock(int DeviceId, int Addr, byte [] Data, int Timeout)
    {
        /*
        if(Data == null || Data.length !=16) return false;
        byte [] SendBuff = new byte[21];
        SendBuff[0] = (byte)0xFF; //CLA
        SendBuff[1] = (byte)0xD6; //INS
        SendBuff[2] = 0x00; //P1
        SendBuff[3] = (byte)Addr;     //P2
        SendBuff[4] = 0x10;             //Lc
        System.arraycopy(Data, 0, SendBuff, 5, 16);

        try
        {
            byte[] RespBuffer =channel.transmit(new CommandAPDU(SendBuff)).getBytes();
            if(RespBuffer== null || RespBuffer.length==0) return false;
            int RespLen = RespBuffer.length;
            if(RespBuffer[RespLen - 2]!=(byte)0x90 ||RespBuffer[RespLen - 1]!=0x00)
            {
                return false;
            }
        }
        catch(CardException ce)
        {
            return false;
        }
        catch(IllegalStateException ise)
        {
            return false;
        }
        return true;
        */
        return false;
    }
	
	public byte [] ReadUltralightPage(int DeviceId, int Addr, int Timeout)
    {
        byte [] SendBuff = new byte[5];
        SendBuff[0] = (byte)0xFF; //CLA
        SendBuff[1] = (byte)0xB0; //INS
        SendBuff[2] = 0x00; //P1
        SendBuff[3] = (byte)Addr;       //P2
        SendBuff[4] = 0x04;             //Le ox00

        byte [] data = new byte[4];
        return null;
		/*
        try
        {
            byte[] RespBuffer =channel.transmit(new CommandAPDU(SendBuff)).getBytes();
            if(RespBuffer==null) return null;
            int RespLen = RespBuffer.length;
            if(RespBuffer[RespLen - 2]!=(byte)0x90 ||RespBuffer[RespLen - 1]!=0x00)
            {
                return null;
            }
            else
            {
                System.arraycopy(RespBuffer, 0, data, 0, 4);
                return data;
            }
        }
        catch(CardException ce)
        {
            return null;
        }
        catch(IllegalStateException ise)
        {
            return null;
        }
        * */
    }
    
/**
  Method Name:WriteUltralightPage
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/

    public final int WriteUltralightPage(int DeviceId, int Addr, byte[] Data, int Timeout)
	{
		TokenReader_Log(DEBUG, "[WriteUltralightPage()] Entry.");
		//int rtvalue = 10;
		if(Timeout<=0)
		{
			//Other Error
			TokenReader_Log(ERROR, "[WriteUltralightPage()] Wrong Timeout value");
			return 31;
		}
		else if(false == getReaderFlag())
		{
			//Device Not Connected
			TokenReader_Log(ERROR, "[WriteUltralightPage()] Device not Connected.");
			return 20;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = WriteUltralightPage(DeviceId, Addr, Data);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(DEBUG, "[WriteUltralightPage()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(true == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[WriteUltralightPage()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				TokenReader_Log(DEBUG, "[WriteUltralightPage()] rtvalue "+rtvalue);
				return rtvalue;
			}
				
		}
	}//public synchronized final int WriteUltralightPage(int DeviceId, int Addr, byte[] Data, int Timeout) end
	
	private synchronized final int WriteUltralightPage(int DeviceId, int Addr, byte[] Data)
	{
		
		synchronized(this){
			if(Data == null || Data.length !=4) {
				TokenReader_Log(ERROR, "[WriteUltralightPage()] Failed insufficient Data ");
				return 31;
			}
			byte [] SendBuff = new byte[9];
			SendBuff[0] = (byte)0xFF; //CLA
			SendBuff[1] = (byte)0xD6; //INS
			SendBuff[2] = 0x00; //P1
			SendBuff[3] = (byte)Addr;     //P2
			SendBuff[4] = 0x04;             //Lc
			System.arraycopy(Data, 0, SendBuff, 5, 4);
			
			if(null == card_obj)
			{
				TokenReader_Log(ERROR, "Card object is Null "+card_obj);
				return 31;
			}
			
			CardChannel channelobj = card_obj.getBasicChannel();
			
			try
			{
				byte[] RespBuffer =channelobj.transmit(new CommandAPDU(SendBuff)).getBytes();
				if(RespBuffer==null || RespBuffer.length==0) {
					TokenReader_Log(ERROR, "[WriteUltralightPage()] Failed Buffer Null");
					return 1;
				}
				int RespLen = RespBuffer.length;
				if(RespBuffer[RespLen - 2]!=(byte)0x90 ||RespBuffer[RespLen - 1]!=0x00)
				{
					TokenReader_Log(ERROR, "[WriteUltralightPage()] Failed ");
					return 1;
				}
			}
			catch(CardException ce)
			{
				TokenReader_Log(ERROR, "[WriteUltralightPage()] "+ce.getMessage());
				return 31;
			}
			catch(IllegalStateException ise)
			{
				TokenReader_Log(ERROR, "[WriteUltralightPage()] "+ise.getMessage());
				return 31;
			}
			
			//Success
			TokenReader_Log(INFO, "[WriteUltralightPage()] Success");
			return 0;
		}
		//return 31;
	}//private synchronized final int WriteUltralightPage(int DeviceId, int Addr, byte[] Data) end
	
/**
  Method Name:DisConnectReaderDevice
  Return Type:int
  Return Value: 0-Operation successfull, 1-Communication failure, 2-Channel blocked, 3-No token found in channel,
  		4-Operation timeout occured
*/
	public final int DisConnectDeviceReader(int DeviceId, int Timeout)
	{
		if(Timeout<=0)
		{
			TokenReader_Log(ERROR, "[DisConnectDeviceReader()] Wrong Timeout Value.");
			return 31;
		}
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[DisConnectDeviceReader()] Device not Connected yet.");
			return 20;
		}
		if(false == getReaderFlag())
		{
			//Device not Connected
			TokenReader_Log(ERROR, "[DisConnectDeviceReader()] Device not Connected yet.");
			return 20;
		}
		else 
		{
			Thread timeout_THREAD = new Thread(new Runnable(){
			
						public void run()
						{
							rtvalue = DisConnectReaderDevice(DeviceId);
							setFlagValue(true);
						}
				
			}); 
				
			//timeout_THREAD.setDaemon(true);
			timeout_THREAD.start();
		
			long endtime = 0;
			endtime = System.currentTimeMillis()+Timeout;
			//return rtvalue;
				
			while(endtime>System.currentTimeMillis())
			{
				if(true == getFlagValue())
				{
					TokenReader_Log(INFO, " [DisConnectDeviceReader()] Return Value "+rtvalue);
					//timeout_THREAD.interrupt();
					setFlagValue(false);
					return rtvalue;
					//break;
				}
			}
				
			if(false == timeoutFlag)
			{
				//Timeout
				TokenReader_Log(ERROR, "[DisConnectDeviceReader()] Operation Timeout Occured ");
				setFlagValue(false);
				return 18;
			}
			else
			{
				setFlagValue(false);
				return rtvalue;
			}
		}
	}// public final int DisConnectDeviceReader(int DeviceId, int Timeout) end
	
	private synchronized final int DisConnectReaderDevice(int DeviceId)
	{
		
		synchronized(this){
			
			
			terminalFactory_obj=null;
			cardTerminals=null;
			contactless_cardTerminal_obj=null;
			contactless_cardTerminal_obj=null;
			SAM1_cardTerminal_obj=null;
			SAM2_cardTerminal_obj=null;
			card_obj=null;
			String pthaStatus_forreader = getPathStatus_string();
			if('0' == pthaStatus_forreader.charAt(7)){
				TokenReader_Log(INFO, "[DisConnectReaderDevice()] Device Disconnected.");
				setReaderFlag(false);
				return 0;
			}
			else{
				TokenReader_Log(INFO, "[DisConnectReaderDevice()] Device Disconnected, but a token is in the channel");
				setReaderFlag(false);
				return 1;
			}
		}
		//return 31;
	}//private synchronized final int DisConnectReaderDevice(int DeviceId) end
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	private synchronized final int SimplePoll()
	{
		TokenReader_Log(DEBUG, "---------------------	[SimplePoll()]  -------------------- ");
		
		synchronized(this)
		{
			int rtCode = 10;
		
			byte[] CommandSimplePoll = new byte[5];
			byte[] SimplePollReply = new byte[10];
			String SimplePollReplybyte = null;
		
			//Simple Poll
			//CommandSimplePoll[0] = (byte)0x03;
			CommandSimplePoll[0] = HopperAddress;
			CommandSimplePoll[1] = (byte)0x00;
			CommandSimplePoll[2] = (byte)0x01;
			CommandSimplePoll[3] = (byte)0xFE;
			//CommandSimplePoll[4] = (byte)0xFE;
			byte chksum = GetCheckSum(CommandSimplePoll);
			CommandSimplePoll[4] = chksum;
		
			int SimplePollrecvlength =10;
			SimplePollReplybyte = "";
		
			for(int counter=0; counter<CommandSimplePoll.length; counter++)
			{
				SimplePollReplybyte = SimplePollReplybyte+"0x"+byteToHex(CommandSimplePoll[counter])+" ";	
			}
		
			TokenReader_Log(DEBUG, "[java SimplePoll()] CommandByte: "+SimplePollReplybyte);
	
			rtCode = TokenReader_CommunicationCycle(CommandSimplePoll, SimplePollrecvlength, SimplePollReply);
			
			if(0 == rtCode)
			{
				//Success
				
				//Log Command reply Bytes
				SimplePollReplybyte="";
				for(int counter=0; counter<SimplePollReply.length; counter++)
				{
					SimplePollReplybyte = SimplePollReplybyte+"0x"+byteToHex(SimplePollReply[counter])+" ";
				}
		
				TokenReader_Log(DEBUG, "[java SimplePoll()] ReplyByte: "+SimplePollReplybyte);
		
				//Check ack
				if(00==SimplePollReply[CommandSimplePoll.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[SimplePoll()] ack returned");
					return 0;
				}
				else
				{
					//Other Error
					TokenReader_Log(DEBUG, "[SimplePoll()] ack not returned ");
					return 31;
				}
			}
			else
			{
				//Communication Failure 
				TokenReader_Log(DEBUG, "[SimplePoll()] Communication Failure "+rtCode);
				return rtCode; //28
			}
			
			//return 0;
		}
			
	}// public final int SimplePoll() end
	
	private synchronized final int HopperStatus()
	{
		TokenReader_Log(DEBUG, "---------------------	[HopperStatus()]  -------------------- ");
		
		synchronized(this)
		{
			int rtCode = 0;
			tokenCounter = 0;
			tokenRemain = 0;
			tokenPaid = 0;
			tokenUnpaid = 0;
			//
			byte[] Command_hopperStatus = new byte[5];
			byte[] hopperStatusReply  = new byte[14];
			String hopperStatusreplybyte=null;
			//hopper status
			//Command_hopperStatus[0] = (byte)0x03;
			Command_hopperStatus[0] = HopperAddress;
			Command_hopperStatus[1] = (byte)0x00;
			Command_hopperStatus[2] = (byte)0x01;
			Command_hopperStatus[3] = (byte)0xA6;
			//Command_hopperStatus[4] = (byte)0x56;
			byte chksum = GetCheckSum(Command_hopperStatus);
			Command_hopperStatus[4] = chksum;
			
			int hopperStatusrecvlength =14;
			
			hopperStatusreplybyte = "";
			for(int counter=0;counter< Command_hopperStatus.length;counter++ )
			{
				 hopperStatusreplybyte = hopperStatusreplybyte+"0x"+byteToHex(Command_hopperStatus[counter])+" " ;
			}

			//TokenReader_Log("[java HopperStatus()]CommandByte: "+hopperStatusreplybyte );

			TokenReader_Log(DEBUG, "[java HopperStatus()]CommandByte: "+hopperStatusreplybyte );

			rtCode = TokenReader_CommunicationCycle( Command_hopperStatus, hopperStatusrecvlength, hopperStatusReply );

			if(0 == rtCode)
			{
					//Log Command reply Bytes
					hopperStatusreplybyte=null;
					hopperStatusreplybyte="";
					
					for(int counter=0;counter< hopperStatusReply.length;counter++ ){
						 hopperStatusreplybyte = hopperStatusreplybyte+"0x"+byteToHex(hopperStatusReply[counter])+" " ;
					}
					
					TokenReader_Log(DEBUG, "[java HopperStatus()]ReplyByte: "+hopperStatusreplybyte );
					//TokenReader_Log("[java HopperStatus()]ReplyByte: "+hopperStatusreplybyte );
					
					//return hopperStatusReply;
					
					////////////	Check ack	//////////////
					if(00==hopperStatusReply[Command_hopperStatus.length+3])
					{
						TokenReader_Log(DEBUG, "[HopperStatus()] ack returned");
						
						tokenCounter = hopperStatusReply[Command_hopperStatus.length+4];
						tokenRemain = hopperStatusReply[Command_hopperStatus.length+5];
						tokenPaid = hopperStatusReply[Command_hopperStatus.length+6];
						tokenUnpaid = hopperStatusReply[Command_hopperStatus.length+7];
						
						TokenReader_Log(DEBUG, " Event couter: "+tokenCounter+" Coins Remain: "+tokenRemain+" Coin Paid last payout: "+tokenPaid+" Coin Unpaid: "+tokenUnpaid);
						
						return 0;
					}
					else
					{
						TokenReader_Log(DEBUG, "[HopperStatus()] nak returned ");
						return 1;
					}
			}
			else
			{
				//Failure
				hopperStatusreplybyte=null;
				hopperStatusreplybyte="";
					
					for(int counter=0;counter< hopperStatusReply.length;counter++ ){
						 hopperStatusreplybyte = hopperStatusreplybyte+"0x"+byteToHex(hopperStatusReply[counter])+" " ;
					}
					
					
				TokenReader_Log(DEBUG, "[HopperStatus()] Communication Failure ");
				TokenReader_Log(DEBUG, "[HopperStatus()] Failure Reply Byte : "+hopperStatusreplybyte);
				return 1;
			}
		}
			
	}//public final byte[] HopperStatus() end
	
	private synchronized final int TestHopper()
	{
			TokenReader_Log(DEBUG, "---------------------	[TestHopper()]  -------------------- ");
			//Test Hopper
			//Command: 03 00 01 A3 59
			//Response: 01 02 03 00 C0 00 BA	
			synchronized(this)
			{
				int rtCode = 10;
				
				byte[] TestCommand = new byte[5];
				byte[] TestCommandReply = new byte[12];
				String TestCommandreplybyte = null;
			
				//TestCommand[0] = (byte)0x03;
				TestCommand[0] = HopperAddress;
				TestCommand[1] = (byte)0x00;
				TestCommand[2] = (byte)0x01;
				TestCommand[3] = (byte)0xA3;
				//TestCommand[4] = (byte)0x59;
				byte chksum = GetCheckSum(TestCommand);
				TestCommand[4] = chksum;
				
				int TestCommandrecvlength =12;
			
				TestCommandreplybyte = "";
				for(int counter=0;counter< TestCommand.length;counter++ ){
				     TestCommandreplybyte = TestCommandreplybyte+"0x"+byteToHex(TestCommand[counter])+" " ;
				}

				TokenReader_Log(DEBUG, "[java TestHopper()]CommandByte: "+TestCommandreplybyte );

				//TokenReader_Log("[java TestHopper()]CommandByte: "+TestCommandreplybyte );

				rtCode = TokenReader_CommunicationCycle( TestCommand,TestCommandrecvlength,TestCommandReply );

				if(0 == rtCode)
				{
					//Log Command reply Bytes
					TestCommandreplybyte=null;
					TestCommandreplybyte="";
					
					for(int counter=0;counter< TestCommandReply.length;counter++ ){
						 TestCommandreplybyte = TestCommandreplybyte+"0x"+byteToHex(TestCommandReply[counter])+" " ;
					}
					
					TokenReader_Log(DEBUG, "[java TestHopper()]ReplyByte: "+TestCommandreplybyte );
					//TokenReader_Log("[java TestHopper()]ReplyByte: "+TestCommandreplybyte );
					
					
					////////////	Check ack	//////////////
					if(00==TestCommandReply[TestCommand.length+3])
					{
						TokenReader_Log(DEBUG, "[TestHopper()] ack returned");
						
						//check errors
						String data1=null, data2=null;
						data1 = data2 = "";
						data1 = Integer.toBinaryString((int)(TestCommandReply[9] & 0xFF));	
						data2 = Integer.toBinaryString((int)(TestCommandReply[10] & 0xFF));	
						
						//Data1 String
						if(data1.length() < 8)
						{
							int counter = data1.length();
							while(counter<8)
							{
								counter++;
								data1 = "0"+data1;
							}
						}

						//Data2 String
						if(data2.length() < 8)
						{
							int counter = data2.length();
							while(counter<8)
							{
								counter++;
								data2 = "0"+data2;
							}
						}
						
						TokenReader_Log(DEBUG, "[TestHopper()] Data1: "+data1+" || Data2: "+data2 );
						
						data1 = new StringBuffer(data1).reverse().toString();
						data2 = new StringBuffer(data2).reverse().toString();
						
						///////////////		Data 1
						TokenReader_Log(DEBUG, " [TestHopper()] DATA1 ");
						//data1[0]
						if('0'==data1.charAt(0))
							TokenReader_Log(DEBUG, "[TestHopper()] Maximum current OK.");
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Maximum current Exceeded. ");
							return 31;
						}
						//data1[1]	
						if('0'==data1.charAt(1))
							TokenReader_Log(DEBUG, "[TestHopper()] Payout OK.");
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Payout timeout Occured. ");
							return 31;
						}
						//data1[2]
						if('0'==data1.charAt(2))
							TokenReader_Log(DEBUG, "[TestHopper()] Motor OK.");
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Motor Reserved during last payout to clear jam. ");
							return 31;
						}
						//data1[3]	
						if('0'==data1.charAt(3))
							TokenReader_Log(DEBUG, "[TestHopper()] Opto fraud attempt, path OK.");
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Opto fraud attempt, path blocked during idle. ");
							return 31;
						}
						//data1[4]	
						if('0'==data1.charAt(4))
							TokenReader_Log(DEBUG, "[TestHopper()] Opto fraud attempt, circuit OK.");
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Opto fraud attempt, short-circuit during idle. ");
							return 31;
						}
						//data1[5]	
						if('0'==data1.charAt(5))
							TokenReader_Log(DEBUG, "[TestHopper()] Opto status OK.");
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Opto blocked permanently during payout. ");
							return 31;
						}
						//data1[6]	
						if('0'==data1.charAt(6))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Power-up not Detected..");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Power-up Detected. ");
						}
						//data1[7]	
						if('0'==data1.charAt(7))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Payout Enabled.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Payout Disabled. ");
						}
						
						///////////////		Data 2
						TokenReader_Log(DEBUG, " [TestHopper()] DATA2 ");
						//data2[0]
						if('0'==data2.charAt(0))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Opto fraud attempt, OK.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Opto fraud attempt, short-circuit during payout. ");
							return 31;
						}
						//data2[1]	
						if('0'==data2.charAt(1))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] payout mode.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Single coin payout mode. ");
							return 31;
						}
						//data2[2]
						if('0'==data2.charAt(2))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum A OK.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum A error. ");
							return 31;
						}
						//data2[3]	
						if('0'==data2.charAt(3))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum B OK.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum B error. ");
							return 31;
						}
						//data2[4]	
						if('0'==data2.charAt(4))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum C OK.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum C error. ");
							return 31;
						}
						//data2[5]	
						if('0'==data2.charAt(5))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum D OK.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Checksum D error. ");
							return 31;
						}
						//data2[6]	
						if('0'==data2.charAt(6))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Power OK.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Power fail during NV Memory write. ");
							return 31;
						}
						//data2[7]	
						if('0'==data2.charAt(7))
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Pin number mechanism disabled.");
						}
						else
						{
							TokenReader_Log(DEBUG, "[TestHopper()] Pin number mechanism enabled. ");
							//return 1;
						}
						
						return 0;
					}
					else
					{
						//Other Error
						TokenReader_Log(DEBUG, "[TestHopper()] ack not returned ");
						return 31;
					}
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[TestHopper()] Communication Failure "+rtCode);
					return rtCode; //28
				}
				
			}
			//return 0;
	}// public final int TestHopper() end
	
	private synchronized final int EnableHopper()
	{
			TokenReader_Log(DEBUG, "---------------------	[EnableHopper()]  -------------------- ");
			
			synchronized(this)
			{
				int rtCode = 10;
				//Enable Hopper
				//Command: 03 01 01 A4 A5 B2
				//Response: 01 00 03 00 FC
				
				byte[] EnableHopperCommand = new byte[6];
				byte[] EnableHopperCommandReply  = new byte[11];
				String EnableHopperCommandreplybyte=null;
				
				EnableHopperCommandreplybyte = null;
				EnableHopperCommandreplybyte="";
				
				//EnableHopperCommand[0] = (byte)0x03;
				EnableHopperCommand[0] = HopperAddress;
				EnableHopperCommand[1] = (byte)0x01;
				EnableHopperCommand[2] = (byte)0x01;
				EnableHopperCommand[3] = (byte)0xA4;
				EnableHopperCommand[4] = (byte)0xA5;
				//EnableHopperCommand[5] = (byte)0xB2;
				byte chksum = GetCheckSum(EnableHopperCommand);
				EnableHopperCommand[5] = chksum;
						
				int EnableHopperCommandrecvlength =11;
			
				EnableHopperCommandreplybyte = "";
				for(int counter=0;counter< EnableHopperCommand.length;counter++ ){
				     EnableHopperCommandreplybyte = EnableHopperCommandreplybyte+"0x"+byteToHex(EnableHopperCommand[counter])+" " ;
				}

				//TokenReader_Log("[java EnableHopper()]CommandByte: "+EnableHopperCommandreplybyte );

				TokenReader_Log(DEBUG, "[java EnableHopper()]CommandByte: "+EnableHopperCommandreplybyte );

				rtCode = TokenReader_CommunicationCycle( EnableHopperCommand, EnableHopperCommandrecvlength, EnableHopperCommandReply );

				
				if(0 == rtCode)
				{
					//Log Command reply Bytes
					EnableHopperCommandreplybyte=null;
					EnableHopperCommandreplybyte="";
					
					for(int counter=0;counter< EnableHopperCommandReply.length;counter++ ){
						 EnableHopperCommandreplybyte = EnableHopperCommandreplybyte+"0x"+byteToHex(EnableHopperCommandReply[counter])+" " ;
					}
					
					//TokenReader_Log("[java EnableHopper()]ReplyByte: "+EnableHopperCommandreplybyte );
					TokenReader_Log(DEBUG, "[java EnableHopper()]ReplyByte: "+EnableHopperCommandreplybyte );
					
					
					////////////	Check ack	//////////////
					if(00==EnableHopperCommandReply[EnableHopperCommand.length+3])
					{
						TokenReader_Log(DEBUG, "[EnableHopper()] ack returned");
						return 0;
					}
					else
					{
						TokenReader_Log(DEBUG, "[EnableHopper()] ack not returned ");
						return 31;
					}
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[EnableHopper()] Communication Failure ");
					return rtCode; //28
				}
			}
	}// public final int EnableHopper() end
	
	private synchronized final int DisableHopper()
	{
			TokenReader_Log(DEBUG, "---------------------	[DisableHopper()]  -------------------- ");
			
			synchronized(this)
			{
				int rtCode = 10;
				//Enable Hopper
				//Command: 03 01 01 A4 A6 B2
				//Response: 01 00 03 00 FC
			
				byte[] EnableHopperCommand = new byte[6];
				byte[] EnableHopperCommandReply  = new byte[11];
				String EnableHopperCommandreplybyte=null;
				
				EnableHopperCommandreplybyte = null;
				EnableHopperCommandreplybyte="";
				
				//EnableHopperCommand[0] = (byte)0x03;
				EnableHopperCommand[0] = HopperAddress;
				EnableHopperCommand[1] = (byte)0x01;
				EnableHopperCommand[2] = (byte)0x01;
				EnableHopperCommand[3] = (byte)0xA4;
				EnableHopperCommand[4] = (byte)0xA6;
				//EnableHopperCommand[5] = (byte)0xB2;
				byte chksum = GetCheckSum(EnableHopperCommand);
				EnableHopperCommand[5] = chksum;
						
				int EnableHopperCommandrecvlength =11;
			
				EnableHopperCommandreplybyte = "";
				for(int counter=0;counter< EnableHopperCommand.length;counter++ ){
				     EnableHopperCommandreplybyte = EnableHopperCommandreplybyte+"0x"+byteToHex(EnableHopperCommand[counter])+" " ;
				}

				TokenReader_Log(DEBUG, "[java DisableHopper()]CommandByte: "+EnableHopperCommandreplybyte );

				rtCode = TokenReader_CommunicationCycle( EnableHopperCommand, EnableHopperCommandrecvlength, EnableHopperCommandReply );
				
				if(0 == rtCode)
				{
					//Log Command reply Bytes
					EnableHopperCommandreplybyte=null;
					EnableHopperCommandreplybyte="";
					
					for(int counter=0;counter< EnableHopperCommandReply.length;counter++ ){
						 EnableHopperCommandreplybyte = EnableHopperCommandreplybyte+"0x"+byteToHex(EnableHopperCommandReply[counter])+" " ;
					}
					
					//TokenReader_Log("[java DisableHopper()]ReplyByte: "+EnableHopperCommandreplybyte );
					TokenReader_Log(DEBUG, "[java DisableHopper()]ReplyByte: "+EnableHopperCommandreplybyte );
					
					
					////////////	Check ack	//////////////
					if(00==EnableHopperCommandReply[EnableHopperCommand.length+3])
					{
						//Success
						TokenReader_Log(DEBUG, "[DisableHopper()] ack returned");
						return 0;
					}
					else
					{
						//Other Error
						TokenReader_Log(DEBUG, "[DisableHopper()] nak returned ");
						return 28;
					}
				}
				else
				{
					//Communication Failure
					return rtCode; //28
				}
			}
	}// public final int DisableHopper() end
	
	private synchronized final int EmergencyStop()
	{
		TokenReader_Log(DEBUG, "---------------------	[EmergencyStop()]  -------------------- ");
		
		synchronized(this)
		{
			//Emergency Stop
			//Command: 03 00 01 AC 50
			//Response: 01 01 03 00 00 FB	
			
			int rtCode = 10;
			
			byte[] EmergencyStopCommand = new byte[5];
			byte[] EmergencyStopCommandReply  = new byte[11];
			String EmergencyStopCommandreplybyte=null;
			
			//EmergencyStopCommand[0] = (byte)0x03;
			EmergencyStopCommand[0] = HopperAddress;
			EmergencyStopCommand[1] = (byte)0x00;
			EmergencyStopCommand[2] = (byte)0x01;
			EmergencyStopCommand[3] = (byte)0xAC;
			//EmergencyStopCommand[4] = (byte)0x50;
			byte chksum = GetCheckSum(EmergencyStopCommand);
			EmergencyStopCommand[4] = chksum; 
						
			int EmergencyStopCommandrecvlength =11;
			
			EmergencyStopCommandreplybyte = "";
			for(int counter=0;counter< EmergencyStopCommand.length;counter++ )
			{
				 EmergencyStopCommandreplybyte = EmergencyStopCommandreplybyte+"0x"+byteToHex(EmergencyStopCommand[counter])+" " ;
			}

			TokenReader_Log(DEBUG, "[java EmergencyStop()]CommandByte: "+EmergencyStopCommandreplybyte );

			//TokenReader_Log("[java EmergencyStop()]CommandByte: "+EmergencyStopCommandreplybyte );

			rtCode = TokenReader_CommunicationCycle( EmergencyStopCommand, EmergencyStopCommandrecvlength, EmergencyStopCommandReply );

			if(0 == rtCode)
			{
		        
		        //Log Command reply Bytes
		        EmergencyStopCommandreplybyte=null;
		        EmergencyStopCommandreplybyte="";
		        
		        for(int counter=0;counter< EmergencyStopCommandReply.length;counter++ ){
		             EmergencyStopCommandreplybyte = EmergencyStopCommandreplybyte+"0x"+byteToHex(EmergencyStopCommandReply[counter])+" " ;
		        }
		        
		        TokenReader_Log(DEBUG, "[java EmergencyStop()]ReplyByte: "+EmergencyStopCommandreplybyte );
		        //TokenReader_Log("[java EmergencyStop()]ReplyByte: "+EmergencyStopCommandreplybyte );
		        
		        
				////////////	Check ack	//////////////
				if(00==EmergencyStopCommandReply[EmergencyStopCommand.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[EmergencyStop()] ack returned");
					return 0;
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[EmergencyStop()] nak returned ");
					return 28;
				}
			}
			else
			{
				//Communication Failure
				return rtCode;
			}
		}
			
	}// public final int EmergencyStop() end
	
	private synchronized final int getProductCode()
	{
		
		TokenReader_Log(DEBUG, "\n---------------------	[getSerialNumber()]  -------------------- ");
		
		synchronized(this)
		{
			//Hopper Product Code
			//Command: 03 00 01 F4 08
			//Response: 01 0E 03 00 53 43 48 32 2D 55 53 45 5F 53 45 52 4E 52 DB
			
			int rtCode = 10;
			
			byte[] RequestProductCode = new byte[5];
			byte[] RequestProductCodeReply = new byte[19];
			String RequestProductCodeReplybyte = null;
			
			//RequestProductCode[0] = (byte)0x03;
			RequestProductCode[0] = HopperAddress;
			RequestProductCode[1] = (byte)0x00;
			RequestProductCode[2] = (byte)0x01;
			RequestProductCode[3] = (byte)0xF4;
			//RequestProductCode[4] = (byte)0x08;
			byte chksum = GetCheckSum(RequestProductCode);
			RequestProductCode[4] = chksum;
			
			int RequestProductCodeReplylength =19;
			
			RequestProductCodeReplybyte = "";
			for(int counter=0;counter< RequestProductCode.length;counter++ )
			{
				 RequestProductCodeReplybyte = RequestProductCodeReplybyte+"0x"+byteToHex(RequestProductCode[counter])+" " ;
			}

			//TokenReader_Log("[java EmergencyStop()]CommandByte: "+RequestProductCodeReplybyte );

			TokenReader_Log(DEBUG, "[java EmergencyStop()]CommandByte: "+RequestProductCodeReplybyte );

			rtCode = TokenReader_CommunicationCycle( RequestProductCode, RequestProductCodeReplylength, RequestProductCodeReply );

			if(0 == rtCode)
			{
				//Success
		        //Log Command reply Bytes
		        RequestProductCodeReplybyte=null;
		        RequestProductCodeReplybyte="";
		        
		        for(int counter=0;counter< RequestProductCodeReply.length;counter++ )
		        {
		             RequestProductCodeReplybyte = RequestProductCodeReplybyte+"0x"+byteToHex(RequestProductCodeReply[counter])+" " ;
		        }
		        
		        //TokenReader_Log("[java EmergencyStop()]ReplyByte: "+RequestProductCodeReplybyte );
		        TokenReader_Log(DEBUG, "[java EmergencyStop()]ReplyByte: "+RequestProductCodeReplybyte );
		
				////////////	Check ack	//////////////
				if(00==RequestProductCodeReply[RequestProductCode.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[getProductCode()] ack returned");
					return 0;
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[getProductCode()] nak returned ");
					return 28;
				}
			}
			else
			{
				//Communication Failure
				return rtCode;
			}
		}
		
		
	}// public synchronized final int getProductCode() end
	
	private synchronized final int getSerialNumber()
	{
		
		TokenReader_Log(DEBUG, "---------------------	[getSerialNumber()]  -------------------- ");
		
		synchronized(this)
		{
			//Hopper Serial Number
			//Command: 03 00 01 F2 0A
			//Response: 01 03 03 00 97 A4 01 BD
			
			int rtCode = 10;
			
			byte[] RequestSerialNr = new byte[5];
			byte[] RequestSerialNrReply = new byte[13];
			String RequestSerialNrReplybyte = null;
			
			RequestSerialNr[0] = (byte)0x03;
			RequestSerialNr[1] = (byte)0x00;
			RequestSerialNr[2] = (byte)0x01;
			RequestSerialNr[3] = (byte)0xF2;
			RequestSerialNr[4] = (byte)0x0A;
			
			int RequestSerialNrReplylength = 13;
			
			RequestSerialNrReplybyte = "";
			for(int counter=0;counter< RequestSerialNr.length;counter++ )
			{
		             RequestSerialNrReplybyte = RequestSerialNrReplybyte+"0x"+byteToHex(RequestSerialNr[counter])+" " ;
		    }

			//TokenReader_Log("[java EmergencyStop()]CommandByte: "+RequestSerialNrReplybyte );

			TokenReader_Log(DEBUG, "[java EmergencyStop()]CommandByte: "+RequestSerialNrReplybyte );

		    rtCode = TokenReader_CommunicationCycle( RequestSerialNr, RequestSerialNrReplylength, RequestSerialNrReply );

		    if(0 == rtCode)
		    {
		        //Success
		        //Log Command reply Bytes
		        RequestSerialNrReplybyte=null;
		        RequestSerialNrReplybyte="";
		        
		        for(int counter=0;counter< RequestSerialNrReply.length;counter++ )
		        {
		             RequestSerialNrReplybyte = RequestSerialNrReplybyte+"0x"+byteToHex(RequestSerialNrReply[counter])+" " ;
		        }
		        
		        //TokenReader_Log("[java EmergencyStop()]ReplyByte: "+RequestSerialNrReplybyte );
		        TokenReader_Log(DEBUG, "[java EmergencyStop()]ReplyByte: "+RequestSerialNrReplybyte );
		        
		        
		        ////////////	Check ack	//////////////
				if(00==RequestSerialNrReply[RequestSerialNr.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[getSerialNumber()] ack returned");
					return 0;
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[getSerialNumber()] nak returned ");
					return 28;
				}
			}
			else
			{
				//Communication Failure
				return rtCode;
			}
		}
		
	}// public synchronized final int getSerialNumber() end
	
	private synchronized final byte[] getCipherKey()
	{
		TokenReader_Log(DEBUG, "---------------------	[getCipherKey()]  -------------------- ");
		synchronized(this)
		{
			//Request Cypher Key
			//Command:  0x03 0x00 0x01 0xa0
			//Response: 0x01 0x08 0x03 0x00 ck0 ck1 ck2 ck3 ck4 ck5 ck6 ck7 cksum
			
			int rtCode = 10;
			byte[] cypherArray = new byte[9];
			cypherArray[0] = (byte)0x10;
			
			byte[] RequestCypher = new byte[5];
			byte[] RequestCypherReply = new byte[18];
			String RequestCypherReplybyte = null;
			
			RequestCypher[0] = HopperAddress;
			RequestCypher[1] = (byte)0x00;
			RequestCypher[2] = (byte)0x01;
			RequestCypher[3] = (byte)0xa0;
			byte chksum = GetCheckSum(RequestCypher);
			RequestCypher[4] = chksum;
			
			int RequestCypherReplybytelength = 18;
			
			RequestCypherReplybyte = "";
			for(int counter=0;counter< RequestCypher.length;counter++ )
			{
				 RequestCypherReplybyte = RequestCypherReplybyte+"0x"+byteToHex(RequestCypher[counter])+" " ;
			}
			
			//TokenReader_Log("[java getCipherKey()]CommandByte: "+RequestProductCodeReplybyte );
			TokenReader_Log(DEBUG, "[java getCipherKey()]CommandByte: "+RequestCypherReplybyte );

			rtCode = TokenReader_CommunicationCycle( RequestCypher, RequestCypherReplybytelength, RequestCypherReply );

			if(0 == rtCode)
			{
				//Success
		        //Log Command reply Bytes
		        RequestCypherReplybyte=null;
		        RequestCypherReplybyte="";
		        
		        for(int counter=0;counter< RequestCypherReply.length;counter++ )
		        {
		             RequestCypherReplybyte = RequestCypherReplybyte+"0x"+byteToHex(RequestCypherReply[counter])+" " ;
		        }
		        
		        //TokenReader_Log("[java EmergencyStop()]ReplyByte: "+RequestProductCodeReplybyte );
		        TokenReader_Log(DEBUG, "[java getCipherKey()]ReplyByte: "+RequestCypherReplybyte );
		        
		        for(int index=9; index<17; index++)
		        {
					cypherArray[index-8]=RequestCypherReply[index];
				}
		
				////////////	Check ack	//////////////
				if(00==RequestCypherReply[RequestCypher.length+3])
				{
					TokenReader_Log(DEBUG, "[getCipherKey()] ack returned");
					cypherArray[0] = (byte)0x00;
					return cypherArray;
				}
				else
				{
					TokenReader_Log(DEBUG, "[getCipherKey()] nak returned ");
					cypherArray[0] = (byte)0x01;
					return cypherArray;
				}
			}
			else if(-1 == rtCode || 28 ==rtCode)
			{
				//Communication Failure
				cypherArray[0] = (byte)0x01;
				return cypherArray;
			}
			else
			{
				//Other Error
				cypherArray[0] = (byte)0x02;
				return cypherArray;
			}
			//return 0;
		}
	}// private synchronized final int getCipherKey() end
	
	private synchronized final int DispenseToken()
	{
		TokenReader_Log(DEBUG, "---------------------	[DispenseToken()]  -------------------- ");
		
		synchronized(this)
		{
			//Dispense Token
			//Command: 03 04 01 a7 97 a4 01 01 14
			//Response: 01 01 03 00 02 F9
			
			int rtCode = 10;
			byte[] DispenseToken = new byte[9];
			byte[] DispenseTokenReply = new byte[15];
			String DispenseTokenReplybyte = null;
			
			//DispenseToken[0] = (byte)0x03;
			DispenseToken[0] = HopperAddress;
			DispenseToken[1] = (byte)0x04;
			DispenseToken[2] = (byte)0x01;
			DispenseToken[3] = (byte)0xA7;
			DispenseToken[4] = (byte)0x97;
			DispenseToken[5] = (byte)0xA4;
			DispenseToken[6] = (byte)0x01;
			DispenseToken[7] = (byte)0x01;
			//DispenseToken[8] = (byte)0x14;
			byte chksum = GetCheckSum(DispenseToken);
			DispenseToken[8] = chksum;
			
			int DispenseTokenReplylength = 15;
			
			
			DispenseTokenReplybyte = "";
			for(int counter=0;counter< DispenseToken.length;counter++ )
			{
		             DispenseTokenReplybyte = DispenseTokenReplybyte+"0x"+byteToHex(DispenseToken[counter])+" " ;
			}

			//TokenReader_Log("[java DispenseToken()]CommandByte: "+DispenseTokenReplybyte );

			TokenReader_Log(DEBUG, "[java DispenseToken()]CommandByte: "+DispenseTokenReplybyte );

			rtCode = TokenReader_CommunicationCycle( DispenseToken, DispenseTokenReplylength, DispenseTokenReply );

		    if(0 == rtCode)
		    {
				//Success
		        //Log Command reply Bytes
		        DispenseTokenReplybyte=null;
		        DispenseTokenReplybyte="";
		        
		        for(int counter=0;counter< DispenseTokenReply.length;counter++ )
		        {
		             DispenseTokenReplybyte = DispenseTokenReplybyte+"0x"+byteToHex(DispenseTokenReply[counter])+" " ;
		        }
		        
		        //TokenReader_Log("[java DispenseToken()]ReplyByte: "+DispenseTokenReplybyte );
		        TokenReader_Log(DEBUG, "[java DispenseToken()]ReplyByte: "+DispenseTokenReplybyte );
						
				////////////	Check ack	//////////////
				if(00==DispenseTokenReply[DispenseToken.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[DispenseToken()] ack returned");
					return 0;
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[DispenseToken()] nak returned ");
					return 1;
				}
			}
			else
			{
				//Communication Failure
				return rtCode;
			}
		}
		
	}// public synchronized final int DispenseToken() end
	
	private synchronized final int DispenseToken(byte cypher_byte[])
	{
		TokenReader_Log(DEBUG, "---------------------	[DispenseToken(rtByte)]  -------------------- ");
		
		synchronized(this)
		{
			//Dispense Token
			//Command: 03 09 01 a7 ck1 ck2 ck3 ck4 ck5 ck6 ck7 ck8 01 chksum
			//Response: 01 01 03 00 02 F9
			
			int rtCode = 10;
			byte[] DispenseToken = new byte[14];
			byte[] DispenseTokenReply = new byte[20];
			String DispenseTokenReplybyte = null;
			
			if((byte)0x00==cypher_byte[0])
			{
				//DispenseToken[0] = (byte)0x03;
				DispenseToken[0] = HopperAddress;
				DispenseToken[1] = (byte)0x09;
				DispenseToken[2] = (byte)0x01;
				DispenseToken[3] = (byte)0xA7;
				DispenseToken[4] = cypher_byte[1];
				DispenseToken[5] = cypher_byte[2];
				DispenseToken[6] = cypher_byte[3];
				DispenseToken[7] = cypher_byte[4];
				DispenseToken[8] = cypher_byte[5];
				DispenseToken[9] = cypher_byte[6];
				DispenseToken[10] = cypher_byte[7];
				DispenseToken[11] = cypher_byte[8];
				DispenseToken[12] = (byte)0x01;
				//DispenseToken[8] = (byte)0x14;
				byte chksum = GetCheckSum(DispenseToken);
				DispenseToken[13] = chksum;
				
				int DispenseTokenReplylength = 20;
				
				
				DispenseTokenReplybyte = "";
				for(int counter=0;counter< DispenseToken.length;counter++ )
				{
						 DispenseTokenReplybyte = DispenseTokenReplybyte+"0x"+byteToHex(DispenseToken[counter])+" " ;
				}

				//TokenReader_Log("[java DispenseToken()]CommandByte: "+DispenseTokenReplybyte );

				TokenReader_Log(DEBUG, "[java DispenseToken(byte cypher_byte[])]CommandByte: "+DispenseTokenReplybyte );

				rtCode = TokenReader_CommunicationCycle( DispenseToken, DispenseTokenReplylength, DispenseTokenReply );

				if(0 == rtCode)
				{
					//Success
					//Log Command reply Bytes
					DispenseTokenReplybyte=null;
					DispenseTokenReplybyte="";
					
					for(int counter=0;counter< DispenseTokenReply.length;counter++ )
					{
						 DispenseTokenReplybyte = DispenseTokenReplybyte+"0x"+byteToHex(DispenseTokenReply[counter])+" " ;
					}
					
					//TokenReader_Log("[java DispenseToken()]ReplyByte: "+DispenseTokenReplybyte );
					TokenReader_Log(DEBUG, "[java DispenseToken(byte cypher_byte[])]ReplyByte: "+DispenseTokenReplybyte );
							
					////////////	Check ack	//////////////
					if(00==DispenseTokenReply[DispenseToken.length+3])
					{
						//Success
						TokenReader_Log(DEBUG, "[DispenseToken(byte cypher_byte[])] ack returned");
						return 0;
					}
					else
					{
						//Communication Failure
						TokenReader_Log(DEBUG, "[DispenseToken(byte cypher_byte[])] nak returned ");
						return 28;
					}
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[DispenseToken(byte cypher_byte[])] rtcode: "+rtCode);
					TokenReader_Log(DEBUG, "[java DispenseToken(byte cypher_byte[])] ReplyByte: "+Arrays.toString(DispenseTokenReply) );
					return 28;
				}
			}
			else
			{
				//Communication Failure
				TokenReader_Log(DEBUG, "[DispenseToken(byte cypher_byte[])] wrong cypher key ");
				TokenReader_Log(DEBUG, "[java DispenseToken(byte cypher_byte[])] ReplyByte: "+Arrays.toString(DispenseTokenReply) );
				return 28;
			}
			
		}
		
	}// public synchronized final int DispenseToken(byte cypher_byte[]) end
	
	private synchronized final int DispenseToken_cypher()
	{
		TokenReader_Log(DEBUG, "---------------------	[DispenseToken(rtByte)]  -------------------- ");
		
		synchronized(this)
		{
			//Dispense Token
			//Command: 03 09 01 a7 ck1 ck2 ck3 ck4 ck5 ck6 ck7 ck8 01 chksum
			//Response: 01 01 03 00 02 F9
			
			int rtCode = 10;
			byte[] DispenseToken = new byte[14];
			byte[] DispenseTokenReply = new byte[20];
			String DispenseTokenReplybyte = null;
			
			//if((byte)0x00==cypher_byte[0])
			{
				//DispenseToken[0] = (byte)0x03;
				DispenseToken[0] = HopperAddress;
				DispenseToken[1] = (byte)0x09;
				DispenseToken[2] = (byte)0x01;
				DispenseToken[3] = (byte)0xA7;
				
				//DispenseToken[4] = cypher_byte[1];
				DispenseToken[4] = (byte)0x99;
				
				//DispenseToken[5] = cypher_byte[2];
				DispenseToken[5] = (byte)0x5e;
				
				//DispenseToken[6] = cypher_byte[3];
				DispenseToken[6] = (byte)0x3f;
				
				//DispenseToken[7] = cypher_byte[4];
				DispenseToken[7] = (byte)0xc;
				
				//DispenseToken[8] = cypher_byte[5];
				DispenseToken[8] = (byte)0x55;
				
				//DispenseToken[9] = cypher_byte[6];
				DispenseToken[9] = (byte)0x6a;
				
				//DispenseToken[10] = cypher_byte[7];
				DispenseToken[10] = (byte)0x5b;
				
				//DispenseToken[11] = cypher_byte[8];
				DispenseToken[11] = (byte)0xf8;
				
				DispenseToken[12] = (byte)0x01;
				//DispenseToken[8] = (byte)0x14;
				byte chksum = GetCheckSum(DispenseToken);
				DispenseToken[13] = chksum;
				
				int DispenseTokenReplylength = 20;
				
				
				DispenseTokenReplybyte = "";
				for(int counter=0;counter< DispenseToken.length;counter++ )
				{
						 DispenseTokenReplybyte = DispenseTokenReplybyte+"0x"+byteToHex(DispenseToken[counter])+" " ;
				}

				//TokenReader_Log("[java DispenseToken()]CommandByte: "+DispenseTokenReplybyte );

				TokenReader_Log(DEBUG, "[java DispenseToken(byte cypher_byte[])]CommandByte: "+DispenseTokenReplybyte );

				rtCode = TokenReader_CommunicationCycle( DispenseToken, DispenseTokenReplylength, DispenseTokenReply );

				if(0 == rtCode)
				{
					//Success
					//Log Command reply Bytes
					DispenseTokenReplybyte=null;
					DispenseTokenReplybyte="";
					
					for(int counter=0;counter< DispenseTokenReply.length;counter++ )
					{
						 DispenseTokenReplybyte = DispenseTokenReplybyte+"0x"+byteToHex(DispenseTokenReply[counter])+" " ;
					}
					
					//TokenReader_Log("[java DispenseToken()]ReplyByte: "+DispenseTokenReplybyte );
					TokenReader_Log(DEBUG, "[java DispenseToken(byte cypher_byte[])]ReplyByte: "+DispenseTokenReplybyte );
							
					////////////	Check ack	//////////////
					if(00==DispenseTokenReply[DispenseToken.length+3])
					{
						//Success
						TokenReader_Log(DEBUG, "[DispenseToken(byte cypher_byte[])] ack returned");
						return 0;
					}
					else
					{
						//Communication Failure
						TokenReader_Log(DEBUG, "[DispenseToken(byte cypher_byte[])] nak returned ");
						return 28;
					}
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[DispenseToken(byte cypher_byte[])] rtcode: "+rtCode);
					TokenReader_Log(DEBUG, "[java DispenseToken(byte cypher_byte[])] ReplyByte: "+Arrays.toString(DispenseTokenReply) );
					return rtCode;
				}
			}
			/*
			else
			{
				TokenReader_Log("[DispenseToken(byte cypher_byte[])] wrong cypher key ");
				TokenReader_Log("[java DispenseToken(byte cypher_byte[])] ReplyByte: "+Arrays.toString(DispenseTokenReply) );
				return 1;
			}
			*/
		}
		
	}// public synchronized final int DispenseToken(byte cypher_byte[]) end
	
	private synchronized final int GetOptoStatus()
	{
		
		TokenReader_Log(DEBUG, "---------------------	[GetOptoStatus()]  -------------------- ");
		
		synchronized(this)
		{
			//Read Opto Status
			//Command: 03 00 01 EC 10
			//Response: 01 01 03 00 81 7A
			
			int rtCode = 10;
			
			byte[] ReadOptoStatus = new byte[5];
			byte[] ReadOptoStatusReply = new byte[11];
			String ReadOptoStatusbyte = null;
			
			//ReadOptoStatus[0] = (byte)0x03;
			ReadOptoStatus[0] = HopperAddress;
			ReadOptoStatus[1] = (byte)0x00;
			ReadOptoStatus[2] = (byte)0x01;
			ReadOptoStatus[3] = (byte)0xEC;
			//ReadOptoStatus[4] = (byte)0x10;
			byte chksum = GetCheckSum(ReadOptoStatus);
			ReadOptoStatus[4] = chksum;
			
			int ReadOptoStatusReplylength = 11;
			
			ReadOptoStatusbyte = "";
			for(int counter=0;counter< ReadOptoStatus.length;counter++ )
			{
		             ReadOptoStatusbyte = ReadOptoStatusbyte+"0x"+byteToHex(ReadOptoStatus[counter])+" " ;
		    }

			//TokenReader_Log("[java GetOptoStatus()]CommandByte: "+ReadOptoStatusbyte );

			TokenReader_Log(DEBUG, "[java GetOptoStatus()]CommandByte: "+ReadOptoStatusbyte );

			rtCode = TokenReader_CommunicationCycle( ReadOptoStatus, ReadOptoStatusReplylength, ReadOptoStatusReply );

			if(0 == rtCode)
			{
				//Success
		        //Log Command reply Bytes
		        ReadOptoStatusbyte=null;
		        ReadOptoStatusbyte="";
		        
		        for(int counter=0;counter< ReadOptoStatusReply.length;counter++ ){
		             ReadOptoStatusbyte = ReadOptoStatusbyte+"0x"+byteToHex(ReadOptoStatusReply[counter])+" " ;
		        }
		        
		        //TokenReader_Log("[java GetOptoStatus()]ReplyByte: "+ReadOptoStatusbyte );
		        TokenReader_Log(DEBUG, "[java GetOptoStatus()]ReplyByte: "+ReadOptoStatusbyte );
			
			
				////////////	Check ack	//////////////
				if(00==ReadOptoStatusReply[ReadOptoStatus.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[GetOptoStatus()] ack returned");
					return 0;
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[GetOptoStatus()] nak returned ");
					return 28;
				}
			}
			else
			{
				//Communication Failure
				return rtCode;
			}
		}
		
	}// public synchronized final int GetOptoStatus() end
	
	private synchronized final int ResetHopper()
	{
		
		TokenReader_Log(DEBUG, "---------------------	[ResetHopper()]  -------------------- ");
		
		synchronized(this)
		{
			//Reset Hopper
			//Command: 03 00 01 01 F9
			//Response: 01 00 03 00 FC
			
			int rtCode = 10;
			
			byte[] ResetDevice = new byte[5];
			byte[] ResetDeviceReply = new byte[10];
			String ResetDeviceReplybyte = null;
			
			//ResetDevice[0] = (byte)0x03;
			ResetDevice[0] = HopperAddress;
			ResetDevice[1] = (byte)0x00;
			ResetDevice[2] = (byte)0x01;
			ResetDevice[3] = (byte)0x01;
			//ResetDevice[4] = (byte)0xF9;
			byte chksum = GetCheckSum(ResetDevice);
			ResetDevice[4] = chksum;
			
			int ResetDeviceReplylength = 10;
			
			ResetDeviceReplybyte = "";
			for(int counter=0;counter< ResetDevice.length;counter++ )
			{
		             ResetDeviceReplybyte = ResetDeviceReplybyte+"0x"+byteToHex(ResetDevice[counter])+" " ;
			}

			//TokenReader_Log("[java ResetHopper()]CommandByte: "+ResetDeviceReplybyte );

			TokenReader_Log(DEBUG, "[java ResetHopper()]CommandByte: "+ResetDeviceReplybyte );

			rtCode = TokenReader_CommunicationCycle( ResetDevice, ResetDeviceReplylength, ResetDeviceReply );

			if(0 == rtCode)
			{
		        //Success
		        //Log Command reply Bytes
		        ResetDeviceReplybyte=null;
		        ResetDeviceReplybyte="";
		        
		        for(int counter=0;counter< ResetDeviceReply.length;counter++ )
		        {
		             ResetDeviceReplybyte = ResetDeviceReplybyte+"0x"+byteToHex(ResetDeviceReply[counter])+" " ;
		        }
		        
		        //TokenReader_Log("[java ResetHopper()]ReplyByte: "+ResetDeviceReplybyte );
		        TokenReader_Log(DEBUG, "[java ResetHopper()]ReplyByte: "+ResetDeviceReplybyte );
			
			
				////////////	Check ack	//////////////
				if(00 == ResetDeviceReply[ResetDevice.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[ResetHopper()] ack returned");
					return 0;
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[ResetHopper()] ack not returned ");
					return 31;
				}
			}
			else
			{
				//Communication Failure
				TokenReader_Log(DEBUG, "[ResetHopper()] Communication Failed. "+rtCode);
				return rtCode; //28
			}
		}
		
	}// public synchronized final int ResetHopper() end
	
	private synchronized final int RequestEquipmentCatagory()
	{
		TokenReader_Log(DEBUG, "---------------------	[RequestEquipmentCatagory()]  -------------------- ");
		
		synchronized(this)
		{
			//Request Equipment Catagory
			//Command: 03 00 01 F5 07
			//Response: 01 06 03 00 50 61 79 6F 75 74 74
			
			int rtCode = 10;
			
			byte[] RequestEquipmentCatagory = new byte[5];
			byte[] RequestEquipmentCatagoryReply = new byte[15];
			String RequestEquipmentCatagoryReplybyte = null;
			
			//ResetDevice[0] = (byte)0x03;
			RequestEquipmentCatagory[0] = HopperAddress;
			RequestEquipmentCatagory[1] = (byte)0x00;
			RequestEquipmentCatagory[2] = (byte)0x01;
			RequestEquipmentCatagory[3] = (byte)0xF6;
			//ResetDevice[4] = (byte)0xF9;
			byte chksum = GetCheckSum(RequestEquipmentCatagory);
			RequestEquipmentCatagory[4] = chksum;
			
			int RequestEquipmentCatagoryReplylength = 15;
			
			RequestEquipmentCatagoryReplybyte = "";
			for(int counter=0;counter< RequestEquipmentCatagory.length;counter++ )
			{
		             RequestEquipmentCatagoryReplybyte = RequestEquipmentCatagoryReplybyte+"0x"+byteToHex(RequestEquipmentCatagory[counter])+" " ;
			}

			//TokenReader_Log("[java ResetHopper()]CommandByte: "+ResetDeviceReplybyte );

			TokenReader_Log(DEBUG, "[java RequestEquipmentCatagory()]CommandByte: "+RequestEquipmentCatagoryReplybyte );

			rtCode = TokenReader_CommunicationCycle( RequestEquipmentCatagory, RequestEquipmentCatagoryReplylength, RequestEquipmentCatagoryReply );

			if(0 == rtCode)
			{
		        //Success
		        //Log Command reply Bytes
		        RequestEquipmentCatagoryReplybyte=null;
		        RequestEquipmentCatagoryReplybyte="";
		        
		        for(int counter=0;counter< RequestEquipmentCatagoryReply.length;counter++ )
		        {
		             RequestEquipmentCatagoryReplybyte = RequestEquipmentCatagoryReplybyte+"0x"+byteToHex(RequestEquipmentCatagoryReply[counter])+" " ;
		        }
		        
		        //TokenReader_Log("[java ResetHopper()]ReplyByte: "+ResetDeviceReplybyte );
		        TokenReader_Log(DEBUG, "[java RequestEquipmentCatagory()]ReplyByte: "+RequestEquipmentCatagoryReplybyte );
			
			
				////////////	Check ack	//////////////
				if(00 == RequestEquipmentCatagoryReply[RequestEquipmentCatagory.length+3])
				{
					//Success
					TokenReader_Log(DEBUG, "[RequestEquipmentCatagory()] ack returned");
					return 0;
				}
				else
				{
					//Communication Failure
					TokenReader_Log(DEBUG, "[RequestEquipmentCatagory()] ack not returned ");
					return 31;
				}
			}
			else
			{
				//Communication Failure
				TokenReader_Log(DEBUG, "[RequestEquipmentCatagory()] Communication Failed. "+rtCode);
				return rtCode; //28
			}
		}
	}// private synchronized final int RequestEquipmentCatagory() end
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			Path Controller Command
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	private synchronized final int getPathStatus()
	{
		
		TokenReader_Log(DEBUG, "---------------------	[getPathStatus()]  -------------------- ");
		
		synchronized(this)
		{
			int rtCode = 10;
			
			byte[] PathStatus = new byte[5];
			byte[] PathStatusReply = new byte[10];
			String PathStatusReplybyte = null;
			
			PathStatus[0] = (byte)0x54;
			PathStatus[1] = (byte)0x00;
			PathStatus[2] = (byte)0x01;
			PathStatus[3] = (byte)0x10;
			//PathStatus[4] = (byte)0xyy;
			byte chksum = GetCheckSum(PathStatus);
			
			PathStatus[4] = chksum;
			
			int PathStatusReplylength = 10;
			
			PathStatusReplybyte = "";
		        for(int counter=0;counter< PathStatus.length;counter++ ){
		             PathStatusReplybyte = PathStatusReplybyte+"0x"+byteToHex(PathStatus[counter])+" " ;
		        }

		        //TokenReader_Log("[java GetOptoStatus()]CommandByte: "+PathStatusReplybyte );

		        TokenReader_Log(DEBUG, "[java GetOptoStatus()]CommandByte: "+PathStatusReplybyte );

		        rtCode = TokenReader_CommunicationCycle( PathStatus, PathStatusReplylength, PathStatusReply );

				//TokenReader_CommunicationCycle( PathStatus, PathStatusReplylength, PathStatusReply, 30);
		        if(0 == rtCode)
		        {
					//Log Command reply Bytes
					PathStatusReplybyte=null;
					PathStatusReplybyte="";
		        
					for(int counter=0;counter< PathStatusReply.length;counter++ ){
						PathStatusReplybyte = PathStatusReplybyte+"0x"+byteToHex(PathStatusReply[counter])+" " ;
					}
		        
					//TokenReader_Log("[java GetOptoStatus()]ReplyByte: "+PathStatusReplybyte );
					TokenReader_Log(DEBUG, "[java GetOptoStatus()]ReplyByte: "+PathStatusReplybyte );
					
					////////////	Check ack	//////////////
					rtCode = fn_getStatus(PathStatusReply);
					
					//Success
					return rtCode;
				}
				else if(-1 == rtCode || 28 == rtCode)
				{
					//Communication Failure
					return rtCode; //-1 or 28
				}
				else
				{
					//Other Error
					return rtCode; //31
				}
			
			/*
			String rawdata = "";
			rawdata = Integer.toBinaryString((int)(PathStatusReply[PathStatus.length+3] & 0xFF));
			
			if(rawdata.length() < 8)
			{
				int counter = rawdata.length();
				while(counter<8)
				{
					counter++;
					rawdata = "0"+rawdata;
				}
			}
			
			String arragedata = "";
			arragedata = new StringBuffer(rawdata).reverse().toString();
			
			if('0' == arragedata.charAt(0))
			{
				TokenReader_Log("[getPathStatus()] ack returned");
				return 0;
			}
			else
			{
				TokenReader_Log("[getPathStatus()] nak returned ");
				return 1;
			}
			
			if('1' == arragedata.charAt(5))
			{
				TokenReader_Log("[getPathStatus()] Token Rejected");
				return 0;
			}
			/*
			else
			{
				TokenReader_Log("[getPathStatus()] nak returned ");
				return 1;
			}
			/
			
			if('1' == arragedata.charAt(6))
			{
				TokenReader_Log("[getPathStatus()] Token Issued");
				return 0;
			}
			/*
			else
			{
				TokenReader_Log("[getPathStatus()] nak returned ");
				return 1;
			}
			/
			
			if('1' == arragedata.charAt(7))
			{
				TokenReader_Log("[getPathStatus()] Token in pocket.");
				return 0;
			}
			/*
			else
			{
				TokenReader_Log("[getPathStatus()] nak returned ");
				return 1;
			}
			*/
			//return rtCode;
		}
	}// private synchronized final int getPathStatus() end
	
	private synchronized final String getPathStatus_string()
	{
		
		TokenReader_Log(DEBUG, "---------------------	[getPathStatus()]  -------------------- ");
		
		synchronized(this)
		{
			int rtCode = 10;
			
			byte[] PathStatus = new byte[5];
			byte[] PathStatusReply = new byte[10];
			String PathStatusReplybyte = null;
			
			PathStatus[0] = (byte)0x54;
			PathStatus[1] = (byte)0x00;
			PathStatus[2] = (byte)0x01;
			PathStatus[3] = (byte)0x10;
			//PathStatus[4] = (byte)0xyy;
			byte chksum = GetCheckSum(PathStatus);
			
			PathStatus[4] = chksum;
			
			int PathStatusReplylength = 10;
			
			PathStatusReplybyte = "";
		        for(int counter=0;counter< PathStatus.length;counter++ ){
		             PathStatusReplybyte = PathStatusReplybyte+"0x"+byteToHex(PathStatus[counter])+" " ;
		        }

		        //TokenReader_Log("[java GetOptoStatus()]CommandByte: "+PathStatusReplybyte );

		        TokenReader_Log(DEBUG, "[java GetOptoStatus()]CommandByte: "+PathStatusReplybyte );

		        rtCode = TokenReader_CommunicationCycle( PathStatus, PathStatusReplylength, PathStatusReply );

				//TokenReader_CommunicationCycle( PathStatus, PathStatusReplylength, PathStatusReply, 30);
		        if(0 == rtCode)
		        {
					//Log Command reply Bytes
					PathStatusReplybyte=null;
					PathStatusReplybyte="";
		        
					for(int counter=0;counter< PathStatusReply.length;counter++ ){
						PathStatusReplybyte = PathStatusReplybyte+"0x"+byteToHex(PathStatusReply[counter])+" " ;
					}
		        
					//TokenReader_Log("[java GetOptoStatus()]ReplyByte: "+PathStatusReplybyte );
					TokenReader_Log(DEBUG, "[java GetOptoStatus()]ReplyByte: "+PathStatusReplybyte );
					
					////////////	Check ack	//////////////
					String rtSt = fn_getStatusinArr(PathStatusReply);
					
					//Success
					return rtSt;
				}
				else if(-1 == rtCode || 28 == rtCode )
				{
					//Communication Failure
					return "20000000";
				}
				else
				{
					// Failure
					return "10000000";
				}
			
		}
	}// private synchronized final int getPathStatus() end
	
	private synchronized final int rejectToken()
	{
		TokenReader_Log(DEBUG, "---------------------	[rejectToken()]  -------------------- ");
		synchronized(this)
		{
			int rtCode = 10;
			
			byte[] RejectToken = new byte[5];
			byte[] RejectTokenReply = new byte[10];
			String RejectTokenReplybyte = null;
			
			RejectToken[0] = (byte)0x54;
			RejectToken[1] = (byte)0x00;
			RejectToken[2] = (byte)0x01;
			RejectToken[3] = (byte)0x30;
			//IssueAToken[4] = (byte)0xyy;
			byte chksum = GetCheckSum(RejectToken);
			
			RejectToken[4] = chksum;
			
			int RejectTokenReplylength = 10;
			
			RejectTokenReplybyte = "";
		        for(int counter=0;counter< RejectToken.length;counter++ ){
		             RejectTokenReplybyte = RejectTokenReplybyte+"0x"+byteToHex(RejectToken[counter])+" " ;
		        }

		        //TokenReader_Log("[java GetOptoStatus()]CommandByte: "+RejectTokenReplybyte );

		        TokenReader_Log(DEBUG, "[java GetOptoStatus()]CommandByte: "+RejectTokenReplybyte );

				long starttime = System.currentTimeMillis();
		        rtCode = TokenReader_CommunicationCycle( RejectToken, RejectTokenReplylength, RejectTokenReply, 250);
				TokenReader_Log(DEBUG, " Time Taken for rejectCommand : "+(System.currentTimeMillis() - starttime));
				
		        if( 0 == rtCode )
		        {
		        
					//Log Command reply Bytes
					RejectTokenReplybyte=null;
					RejectTokenReplybyte="";
		        
					for(int counter=0;counter< RejectTokenReply.length;counter++ ){
						RejectTokenReplybyte = RejectTokenReplybyte+"0x"+byteToHex(RejectTokenReply[counter])+" " ;
					}
		        
					//TokenReader_Log("[java GetOptoStatus()]ReplyByte: "+IssueATokenReplybyte );
					TokenReader_Log(DEBUG, "[java GetOptoStatus()]ReplyByte: "+RejectTokenReplybyte );
			
			
					////////////	Check ack	//////////////
					rtCode = fn_getStatus(RejectTokenReply);
					
					return rtCode;
				}
				else if(-1 == rtCode || 28 == rtCode)
		        {
					//Communication Failure
					TokenReader_Log(DEBUG, "[java rejectToken()] Communication Failure");
					return rtCode;
				}
				else
				{
					//Other Error
					return rtCode; //31
				}
		}
	}// private synchronized final int rejectToken() end
	
	private synchronized final int issueToken()
	{
		
		TokenReader_Log(DEBUG, "---------------------	[issueToken()]  -------------------- ");
		
		synchronized(this)
		{
			int rtCode = 10;
			
			byte[] IssueAToken = new byte[5];
			byte[] IssueATokenReply = new byte[10];
			String IssueATokenReplybyte = null;
			
			IssueAToken[0] = (byte)0x54;
			IssueAToken[1] = (byte)0x00;
			IssueAToken[2] = (byte)0x01;
			IssueAToken[3] = (byte)0x20;
			//PathStatus[4] = (byte)0xyy;
			byte chksum = GetCheckSum(IssueAToken);
			
			IssueAToken[4] = chksum;
			
			int IssueATokenReplylength = 10;
			
			IssueATokenReplybyte = "";
		        for(int counter=0;counter< IssueAToken.length;counter++ ){
		             IssueATokenReplybyte = IssueATokenReplybyte+"0x"+byteToHex(IssueAToken[counter])+" " ;
		        }

		        //TokenReader_Log("[java GetOptoStatus()]CommandByte: "+IssueATokenReplybyte );

		        TokenReader_Log(DEBUG, "[java GetOptoStatus()]CommandByte: "+IssueATokenReplybyte );
				
				long starttime = System.currentTimeMillis();
		        rtCode = TokenReader_CommunicationCycle( IssueAToken, IssueATokenReplylength, IssueATokenReply, 250 );
				TokenReader_Log(DEBUG, " Time Taken for issueCommand : "+(System.currentTimeMillis() - starttime));
				
		        if( 0 == rtCode )
		        {
						//Log Command reply Bytes
						IssueATokenReplybyte=null;
						IssueATokenReplybyte="";
		        
						for(int counter=0;counter< IssueATokenReply.length;counter++ )
						{
							IssueATokenReplybyte = IssueATokenReplybyte+"0x"+byteToHex(IssueATokenReply[counter])+" " ;
						}
		        
						//TokenReader_Log("[java GetOptoStatus()]ReplyByte: "+IssueATokenReplybyte );
						TokenReader_Log(DEBUG, "[java GetOptoStatus()]ReplyByte: "+IssueATokenReplybyte );
			
			
						////////////	Check ack	//////////////
						rtCode = fn_getStatus(IssueATokenReply);
			
						return rtCode;
				}
		        else if(-1 == rtCode || 28 == rtCode)
		        {
					//Communication Failure
					TokenReader_Log(DEBUG, "[java issueToken()] Communication Failure");
					return rtCode; //-1 or 28
				}
				else
				{
					TokenReader_Log(DEBUG, "[java issueToken()] Other Error");
					return rtCode; //31
				}
		}
	}// private synchronized final int issueToken() end
	
	private synchronized final int fn_ClearpathStatus()
	{
		
		TokenReader_Log(DEBUG, "---------------------	[fn_ClearpathStatus()]  -------------------- ");
		
		synchronized(this)
		{
			int rtCode = 10;
			
			byte[] ClearStatus = new byte[5];
			byte[] ClearStatusReply = new byte[10];
			String ClearStatusReplybyte = null;
			
			ClearStatus[0] = (byte)0x54;
			ClearStatus[1] = (byte)0x00;
			ClearStatus[2] = (byte)0x01;
			ClearStatus[3] = (byte)0x40;
			
			byte chksum = GetCheckSum(ClearStatus);
			
			ClearStatus[4] = chksum;
			int ClearStatusReplylength = 10;
			
			ClearStatusReplybyte = "";
		        for(int counter=0;counter< ClearStatus.length;counter++ ){
		             ClearStatusReplybyte = ClearStatusReplybyte+"0x"+byteToHex(ClearStatus[counter])+" " ;
		        }

		        //TokenReader_Log("[java GetOptoStatus()]CommandByte: "+ClearStatusReplybyte );

		        TokenReader_Log(DEBUG, "[java GetOptoStatus()]CommandByte: "+ClearStatusReplybyte );

		        rtCode = TokenReader_CommunicationCycle( ClearStatus, ClearStatusReplylength, ClearStatusReply );
				if( 0 == rtCode )
				{
						//Log Command reply Bytes
						ClearStatusReplybyte=null;
						ClearStatusReplybyte="";
		        
						for(int counter=0;counter< ClearStatusReply.length;counter++ )
						{
							ClearStatusReplybyte = ClearStatusReplybyte+"0x"+byteToHex(ClearStatusReply[counter])+" " ;
						}
		        
						//TokenReader_Log("[java GetOptoStatus()]ReplyByte: "+ClearStatusReplybyte );
						TokenReader_Log(DEBUG, "[java GetOptoStatus()]ReplyByte: "+ClearStatusReplybyte );
			
						rtCode = fn_getStatus(ClearStatusReply);
						
						return rtCode;	// Success
				}
		        else if(-1 == rtCode || 28 ==rtCode)
		        {
					//Communication Failure
					return rtCode; //-1 or 28
				}
				else
				{
					return rtCode; //31
				}
			
		}
	}// private synchronized final int ClearStatus() end
	
	private final int fn_getStatus(byte[] Reply)
	{
		synchronized(this)
		{
			//Check ack
			int rtCode = 10;
			String rawdata = null;
			rawdata = Integer.toBinaryString((int)(Reply[8] & 0xFF));
			
			if(rawdata.length() < 8)
			{
				int counter = rawdata.length();
				while(counter<8)
				{
					counter++;
					rawdata = "0"+rawdata;
				}
			}
			
			String arragedata = "";
			arragedata = new StringBuffer(rawdata).reverse().toString();
			
			TokenReader_Log(DEBUG, "////////////////////////////");
			TokenReader_Log(DEBUG, "DataString - "+arragedata);
			TokenReader_Log(DEBUG, "////////////////////////////");
			
			if('0' == arragedata.charAt(0))
			{
				TokenReader_Log(DEBUG, "[getStatus()] ack returned");
				rtCode = 0;
			}
			else
			{
				TokenReader_Log(DEBUG, "[getStatus()] nak returned ");
				rtCode = 1;
			}
			
			if('1' == arragedata.charAt(5))
			{
				TokenReader_Log(DEBUG, "[getStatus()] Token Rejected");
				rtCode = rtCode+6;
			}
			/*
			else
			{
				TokenReader_Log("[getStatus()] nak returned ");
				//return 1;
			}
			*/
			
			if('1' == arragedata.charAt(6))
			{
				TokenReader_Log(DEBUG, "[getStatus()] Token Issued");
				rtCode = rtCode+7;
			}
			/*
			else
			{
				TokenReader_Log("[getStatus()] nak returned ");
			}
			*/
			
			if('1' == arragedata.charAt(7))
			{
				TokenReader_Log(DEBUG, "[getStatus()] Token in pocket.");
				rtCode = rtCode+8;
			}
			/*
			else
			{
				TokenReader_Log("[getStatus()] nak returned ");
			}
			*/
			return rtCode;
		}
	}// private final getStatus(byte byteValue) end
	
	private final String fn_getStatusinArr(byte[] Reply)
	{
		synchronized(this)
		{
			String rawdata = null;
			rawdata = Integer.toBinaryString((int)(Reply[8] & 0xFF));
			
			if(rawdata.length() < 8)
			{
				int counter = rawdata.length();
				while(counter<8)
				{
					counter++;
					rawdata = "0"+rawdata;
				}
			}
			
			String arragedata = "";
			arragedata = new StringBuffer(rawdata).reverse().toString();
			
			TokenReader_Log(DEBUG, "////////////////////////////");
			TokenReader_Log(DEBUG, "DataString - "+arragedata);
			TokenReader_Log(DEBUG, "////////////////////////////");
			
			return arragedata;
		}
	}// private final getStatus(byte byteValue) end
	
	private synchronized final String TokenDispenserReaderFWVersion()
	{
		int rtCode = 10;
		byte[] Commandfirmwareversion = new byte[9];
		byte[] firmwareversionReply = new byte[18];
		String firmwareversionReplybyte = null;
		
		Commandfirmwareversion[0] = (byte)0x55;
		Commandfirmwareversion[1] = (byte)0x04;
		Commandfirmwareversion[2] = (byte)0x01;
		Commandfirmwareversion[3] = (byte)0x80;
		Commandfirmwareversion[4] = (byte)0xFF;
		Commandfirmwareversion[5] = (byte)0x00;
		Commandfirmwareversion[6] = (byte)0x00;
		Commandfirmwareversion[7] = (byte)0x00;
		byte chksum = GetCheckSum(Commandfirmwareversion);
		Commandfirmwareversion[8] = chksum;
		
		int firmwareversionReplylength = 18;
			
		firmwareversionReplybyte = "";
		for(int counter=0;counter< Commandfirmwareversion.length;counter++ ){
			 firmwareversionReplybyte = firmwareversionReply+"0x"+byteToHex(Commandfirmwareversion[counter])+" " ;
		}

		        //TokenReader_Log("[java GetOptoStatus()]CommandByte: "+ClearStatusReplybyte );

		        TokenReader_Log(DEBUG, "[java TokenDispenserReaderFWVersion()] CommandByte: "+firmwareversionReplybyte );

		        rtCode = TokenReader_CommunicationCycle( Commandfirmwareversion, firmwareversionReplylength, firmwareversionReply );
				if( 0 == rtCode )
				{
						//Log Command reply Bytes
						firmwareversionReplybyte=null;
						firmwareversionReplybyte="";
		        
						for(int counter=0;counter< firmwareversionReply.length;counter++ )
						{
							firmwareversionReplybyte = firmwareversionReplybyte+"0x"+byteToHex(firmwareversionReply[counter])+" " ;
						}
		        
						//TokenReader_Log("[java GetOptoStatus()]ReplyByte: "+ClearStatusReplybyte );
						TokenReader_Log(DEBUG, "[java GetOptoStatus()]ReplyByte: "+firmwareversionReplybyte );
			
						//rtCode = fn_getStatus(firmwareversionReply);
						
						return firmwareversionReplybyte;	// Success
				}
		        else
		        {
						// Failure
						return "00.00.00";
				}
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	
	private final int isTokeninStagingArea()
	{
		//0 - No Token in Area, 1 - Token Found
		return 0;
	}
	
	private final int RFID_ReaderStatus()
	{
		return 0;
	}//private final int RFID_ReaderStatus()
	
	private final int SAM_ReaderStatus()
	{
		return 0;
	}//private final int SAM_ReaderStatus()	
	
}//class TestPort end
