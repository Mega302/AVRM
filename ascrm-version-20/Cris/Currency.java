package Cris;

//++Other Package
import java.net.URL;
import java.io.InputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.lang.NullPointerException;
import java.io.IOException;
import java.lang.IllegalArgumentException;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.Date;
import java.text.SimpleDateFormat;
import java.util.jar.Manifest;
import java.util.jar.Attributes;
import java.io.BufferedReader;
import java.lang.Runtime;
import java.lang.Process;

public final class Currency {

         private final Object lock = new Object();
         private String  DeviceId;
         private String  LogFilePath;
         private int     LogFileMode;
         private String  Logdllpath;
         private boolean GetValidCurrencyStopFlag = false;
         
         private static final int COMMUNICATION_FAILURE_NOTE=28;
         private static final int COMMUNICATION_FAILURE_COIN=29;
         
         private static final int YET_NOT_CONNECTED_NOTE=20;
         private static final int YET_NOT_CONNECTED_COIN=21;
         
         private static boolean g_NoteAccetorActivateFlag=false;
         private static boolean g_CoinAccetorActivateFlag=false;
         private static boolean g_TokenDispenserActivateFlag=false;
         private static boolean g_SecurityDeviceActivateFlag=false;
         
         public    static boolean g_PortUseFlag=false;
         private   static Currency single_instance = null; 
         private   static final String MEGAJNISOFILENAME ="libMegaAscrmNoteAcceptorAPI.so";
         
         
         private static String g_NADeviceidStr   = null;
         private static String g_CADeviceidStr   = null;
         private static String g_CEDeviceidStr   = null;
         private static byte[] g_DeviceStatus    = new byte[12];
         private static byte[] g_NaDeviceStatus  = new byte[12];
         private static byte[] g_CaDeviceStatus  = new byte[12];
         public static boolean g_loadCurrency    = false;
         
         
         //++SmartCard Function
         public static synchronized native   int JniConnectPCSCService();
         public static synchronized native   int JniDisConnectPCSCService();
         public static synchronized native   String[] JniListPCSCSmartCardReader();
         public static synchronized native   int JniConnectPCSCSmartCardReader();
         public static synchronized native   int JniDisConnectPCSCSmartCardReader();
         public static synchronized native   byte[] JniTransmitToPCSCSmartCardReader(byte[] Command);
         
         //++Setup Log Settings
         public static synchronized native  boolean SetupCurrencyLogFile( String  Logfile,int  LogLevel,String  LogdllPath);
         public static synchronized native  boolean SetupNoteAcceptorID( String  DeviceId);
         public static synchronized native  boolean SetupCoinAcceptorID( String  DeviceId);
         public static synchronized native  boolean SetupCoinEscrowID( String  DeviceId);
         public static synchronized native  boolean SetupKioskSecurityID( String  DeviceId);
         public static synchronized native  boolean SetupUPSID( String DeviceId);
         public static synchronized native  boolean SetupNativeLogFile( String  Logfile,String  LogdllPath);
         public static synchronized native  boolean SetuDevMode( int fnDeviceId ,int LogLevel);
         
         private static synchronized native boolean EnableSingleCurrencyFlag();
         private static synchronized native boolean DisableSingleCurrencyFlag();

         //++CCTALK Device Port Open and Close Functions
         //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
         public static synchronized native   int JniActivateCCTalkPort(int deviceid,int portNmbr);
         public static synchronized native   int JniDeActivateCCTalkPort(int deviceid);
         
         public static synchronized  int ActivateCCTalkPort(int deviceid,int portNmbr){
			 
			      //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
			      int rtcode=-1;
			      if( 1 == deviceid ){ //++ 1: Token Dispenser 
					  
					  if( true == g_CoinAccetorActivateFlag ){
						  Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] TokenDispenser Already CCTALK Port Opened",Common.GetLoggingLevelV2() );
						  g_TokenDispenserActivateFlag = true;
						  return 0; //++Already Connected					  
					  }//if end
					  
					  if( (false == g_TokenDispenserActivateFlag) && (false == g_SecurityDeviceActivateFlag) ){
						 
						 //System.out.println("[Currency][ActivateCCTalkPort()] TokenDispenser is not connected so going to connect device");
						 Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] TokenDispenser is not connected so going to connect device",Common.GetLoggingLevelV2() );
						 rtcode = JniActivateCCTalkPort(deviceid,portNmbr);
						 
					  }else {
						  
						 //System.out.println("[Currency][ActivateCCTalkPort()] TokenDispenser is alreday connected");
						 Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] TokenDispenser is alreday connected",Common.GetLoggingLevelV2() );
						 return 0; //++Already Connected  
					  }//else done
					  
				      if( 0 == rtcode ){ //++operation success
						  //System.out.println("[Currency][ActivateCCTalkPort()] TokenDispenser is connection success");
						  Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] TokenDispenser is connection success",Common.GetLoggingLevelV2() );
						  g_TokenDispenserActivateFlag = true;
					  }//if end
					  
					  return rtcode;
					  
				  } else if( 2 == deviceid ){ //++ 2: Security
					  
					  if( true == g_CoinAccetorActivateFlag ){
						  
						  //System.out.println("[Currency][ActivateCCTalkPort()] SecurityDevice Already CCTALK Port Opened");
						  Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] SecurityDevice Already CCTALK Port Opened",Common.GetLoggingLevelV2() );
						  g_SecurityDeviceActivateFlag = true;
						  return 0; //++Already Connected
						  
					  }//if end
					  
					  if( (false == g_SecurityDeviceActivateFlag) && (false == g_TokenDispenserActivateFlag) ){
						
						//System.out.println("[Currency][ActivateCCTalkPort()] Security is not connected so going to connect device");
						Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] Security is not connected so going to connect device",Common.GetLoggingLevelV2() );
						rtcode = JniActivateCCTalkPort(deviceid,portNmbr);
						
					  }else {
						  
						//System.out.println("[Currency][ActivateCCTalkPort()] Security is already connected");
						Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] Security is already connected",Common.GetLoggingLevelV2() );
						return 0; //++Already Connected  
					  }
				      if( 0 == rtcode ){ //++operation success
						  
						  //System.out.println("[Currency][ActivateCCTalkPort()] Security is connection success");
						  Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] Security is connection success",Common.GetLoggingLevelV2() );
						  g_SecurityDeviceActivateFlag = true;
					  }
				      return rtcode;
				      
				  }else {
					  //System.out.println("[Currency][ActivateCCTalkPort()] No Device ID Matched return other error");
					  Currency.Ascrm_WriteLog("API","[Currency][ActivateCCTalkPort()] No Device ID Matched return other error",Common.GetLoggingLevelV2() );
					  return 31; //++Exception No Device id matched other error
				  }//else end
			 
			 
		 }//public static synchronized  int ActivateCCTalkPort(int deviceid,int portNmbr) end
		 
         public static synchronized  int DeActivateCCTalkPort(int deviceid){
			       
			      int rtcode=-1;
			      if( 1 == deviceid ){ //++ 1: Token Dispenser 					  
					  if( ( false == g_CoinAccetorActivateFlag ) && ( false == g_SecurityDeviceActivateFlag ) ){
							  if( false == g_TokenDispenserActivateFlag ){
								  //System.out.println("[Currency][DeActivateCCTalkPort()] Token Dispenser Already Port Closed So return not yet conneced");
								  Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Token Dispenser Already Port Closed So return not yet connected",Common.GetLoggingLevelV2() );
								  return 20; //++not yet connected
							  }//if end
							  							  
							  rtcode = JniDeActivateCCTalkPort(deviceid);
							  if( 0 == rtcode ){  //++operation success
								  
								 //System.out.println("[Currency][DeActivateCCTalkPort()] Token Dispenser Port Closed Success");
								 Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Token Dispenser Port Closed Success",Common.GetLoggingLevelV2() );
								 g_TokenDispenserActivateFlag=false;
								 
							  }else {
								 //System.out.println("[Currency][DeActivateCCTalkPort()] Token Dispenser Port Closed Failed");  
								 Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Token Dispenser Port Closed Failed",Common.GetLoggingLevelV2() );
							  }
							  return rtcode;
					      
					  }else {
						  //System.out.println("[Currency][DeActivateCCTalkPort()] Token Dispenser Port Closed Success Default Return");
						  Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Token Dispenser Port Closed Success Default Return",Common.GetLoggingLevelV2() );
						  g_TokenDispenserActivateFlag=false;
						  return 0; //++Default Success as already port is used by another application	  
					  }//else end
					  
				  }else if( 2 == deviceid ){ //++ 2: Security					  
					  if( ( false == g_CoinAccetorActivateFlag ) && ( false == g_TokenDispenserActivateFlag ) ){
						  
							  if( false == g_SecurityDeviceActivateFlag ){
								  
								  //System.out.println("[Currency][DeActivateCCTalkPort()] Security Already Port Closed So return not yet connected error");
								  Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Security Already Port Closed So return not yet connected erro",Common.GetLoggingLevelV2() );
								  return 20; //++Other error as port already closed
								  
							  }//if end
							  
							  rtcode = JniDeActivateCCTalkPort(deviceid);
							  if( 0 == rtcode ){ //++operation success
								  
								 //System.out.println("[Currency][DeActivateCCTalkPort()] Security Already Port Closed Success");
								 Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Security Already Port Closed Succes",Common.GetLoggingLevelV2() );
								 g_SecurityDeviceActivateFlag=false;
								 
							  }else{
								 
								 //System.out.println("[Currency][DeActivateCCTalkPort()] Security Already Port Closed Failed");  
								 Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Security Already Port Closed Faileds",Common.GetLoggingLevelV2() );
							  
							  }//else 
							  
							  return rtcode;
					      
					  }else {						  
						  //System.out.println("[Currency][DeActivateCCTalkPort()] Security Already Port Closed Success Default Return");
						  Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] Security Already Port Closed Success Default Return",Common.GetLoggingLevelV2() );
						  g_SecurityDeviceActivateFlag=false;
						  return 0; //++Default Success as already port is used by another application
					  }//else end
				  }else {
					  //System.out.println("[Currency][DeActivateCCTalkPort()] No Device ID Matched Other Error");
					  Currency.Ascrm_WriteLog("API","[Currency][DeActivateCCTalkPort()] No Device ID Matched Other Error",Common.GetLoggingLevelV2() );
					  return 31; //++Exception No Device id matched other error
				  }//else end
			 
		 }//public static synchronized  int DeActivateCCTalkPort() end
		
         //++1.Token Device API in C
         public static synchronized native   int      IsCCTALKPortBusy();
         public static synchronized native   int      GetReplyFromCCTalkPort(byte[] Command,int recvlength,byte[] Reply,int CcTalkReplyWaitTime);
         public static synchronized native   int      GetReplyFromCCTalkPort(byte[] Command,int recvlength,byte[] Reply,int WaitBeforeReadReply,int CcTalkReplyWaitTime);
         
         public static synchronized native   int      IsCCTALKPortOpen();
         public static synchronized native   void     SetCCTalkPollInterValTime(int fnCCTalkPollInterValTime);
         public static synchronized native   int      WriteLog(String fnDeviceId,String fnMessage,int fnLogLevel);
         public static synchronized native   int      WriteDataExchangeLog(String fnDeviceId,String fnMessageType,String fnMessage);
         public synchronized        native   long[]    getTime();
         public synchronized        native   void      WriteNativeLog(String message);
         
         //++2.Currency API in C [Note+Coin+CoinEscrow]
         private  synchronized native   int 	    JniConnectDevice( int PortId,int DeviceType,int EscrowClearanceMode,int Timeout);
         private  synchronized native   int 	    JniDisConnectDevice(int DeviceType,int Timeout);
         private  synchronized native   byte[] 	JniDeviceStatus(int DeviceType,int Timeout);
         private  synchronized native   String   getCashFirmwareVersion();
         private  synchronized native   String   getCoinFirmwareVersion();
         private  synchronized native   int      ClearJammedCurrenciesAccept(int Timeout,int DeviceType);
         private  synchronized native   int      ClearJammedCurrenciesReject(int Timeout,int DeviceType);
         
         //++Common Enable for Note&Coin
         private  synchronized native   int  	JniEnableTheseDenominations(int CurrencyType,int DenomMask,int Timeout);
         
         //++Multi Note/Coin Acceptance
         private  synchronized native   boolean JniAcceptCurrencies(int CurrencyType,int Amount,int Timeout);
         private  synchronized native   int  	JniGetAcceptedAmount(byte[][] AcptdAmt);
         private  synchronized native   int  	JniStackAcceptedCurrencies(int Timeout);
         private  synchronized native   int  	JniReturnAcceptedCurrencies(int Timeout);
         
         //++Single Note/Coin Acceptance
         private  synchronized native   int  	JniGetValidCurrency(int CurrencyType,int Denom,int Timeout);
         private  synchronized native   int  	JniAcceptCurrentCurrency(int CurrencyType,int Denom,int Timeout);
         private  synchronized native   int  	JniReturnCurrentCurrency(int CurrencyType,int Timeout);
         private  synchronized native   int     JniIsNoteRemoved(int Timeout);
         private  synchronized native   int     JniClearJammedNotes(int EscrowClearanceMode,int Timeout);
         private  synchronized native   int     JniGetCurrencyAcceptorLastError();
         
         //++3.Security Device API in C [Door+Alarm]
         protected  static synchronized native   int  	 JniGetSecurityVersion(byte[] VersionString);
         protected  static synchronized native   int     JniDisableAlarm( int DoorType1,int time1,int DoorType2,int time2,byte alarmlogicbits);
         protected  static synchronized native   int     JniGetDoorStatus(int DoorType);
         protected  static synchronized native   boolean SetupSecurityLogFile( String  Logfile,int LogLevel,String DeviceId,String  LogdllPath);
         
         //++4.UPS Device API in C [Door+Alarm]
         protected  static synchronized native   int 	 JniGetUPSStatus();
         protected  static synchronized native   int 	 JniGetBatteryStatus();                                                          
                                                                                                                  
         public synchronized static int Ascrm_WriteLog(String fnDeviceId,String fnMessage,int LogLevel) {
			    return Currency.WriteLog(fnDeviceId,fnMessage,LogLevel);
		 }//++public synchronized int WriteLog( String fnDeviceId,String fnMessage,int LogLevel) end
         
         //++Currency API Implemention in java [Note+Coin]
         
         public final synchronized String getAPIInfo(){
                    
                    try{
						Class clazz = Currency.class;
						String className = clazz.getSimpleName() + ".class";
						String classPath = clazz.getResource(className).toString();
						if (!classPath.startsWith("jar")) {
							 //Class not from JAR
								 //System.out.println("[getAPIInfo()] Unable to get Specification Version");
							 return null;
						}
						String manifestPath = classPath.substring(0, classPath.lastIndexOf("!") + 1) + "/META-INF/MANIFEST.MF";
						Manifest manifest = new Manifest(new URL(manifestPath).openStream());
						Attributes attr = manifest.getMainAttributes();
						//1.Bidder Name
                        String ImplementationVendor = attr.getValue("Implementation-Vendor");
                        //2.Implemention Year
                        String ImplementationVersion = attr.getValue("Implementation-Version");
						//3.Jar Version
                        String SpecificationVersion = attr.getValue("Specification-Version");
                        //System.out.println("[getCashAPIVersion()] CashAPIVersion: "+ImplementationVendor+","+ImplementationVersion+"."+SpecificationVersion);
                        return ImplementationVendor.replace(" ","")+","+ImplementationVersion+"."+SpecificationVersion;

		       }catch(java.net.MalformedURLException ex){
		              System.out.println("[getAPIInfo()] MalformedURLException");
		              return null;

		       }catch(java.io.IOException ex){
		               System.out.println("[getAPIInfo()] IOException");
		               return null;
		           
		       }
   	    
         }//public String getAPIInfo() end
         
         public final synchronized String GetNativeLibVersion(){
	         return "01.00.00";
	     }
	     
	     public final synchronized String GetCurrencyDevFWVersion(){
	         return "01.00.00";
	     }
	     
         public  final synchronized int ConnectDevice( int PortId1,int PortId2,int PortId3,int DeviceType,int EscrowClearanceMode,int Timeout) {
                 
                 synchronized(this){
				     int rtcode = -1;
				     //if( true != g_PortUseFlag ) {
						 //g_PortUseFlag = true;
						 if( 1 == DeviceType ) { //++Note Acceptor
							Currency.Ascrm_WriteLog("API","[Currency][ConnectDevice()] Device Type Note Acceptor",Common.GetLoggingLevelV2() ); 
							rtcode = JniConnectDevice( PortId1,DeviceType,EscrowClearanceMode,Timeout);
							if( (0==rtcode) || (20==rtcode) || (1==rtcode) || (3==rtcode) || (4==rtcode) || (5==rtcode) ){
								g_NoteAccetorActivateFlag = true;
						    }//if end
						 } else if( 2 == DeviceType ) { //++Coin Acceptor with Escrow
							Currency.Ascrm_WriteLog("API","[Currency][ConnectDevice()] Device Type Coin Acceptor",Common.GetLoggingLevelV2() );  
							rtcode = JniConnectDevice( PortId2,DeviceType,EscrowClearanceMode,Timeout); 
							if( (0==rtcode) || (21==rtcode) || (1==rtcode) || (3==rtcode) || (4==rtcode) || (5==rtcode) ){
								g_CoinAccetorActivateFlag = true;
						    }//if end
						 }//else if end
						 //g_PortUseFlag = false;
						 return rtcode;
					 /*} else {
						 return 0;
					 }//else end
					 */
			     }//++synchronized(this) end
                 
         }//++public  synchronized int ConnectDevice( int PortId,int DeviceType,int EscrowClearanceMode,int Timeout) end
         
         public  final synchronized int DisConnectDevice(int DeviceType,int Timeout) {
                 
                 synchronized(this){
					 
				     int rtcode = -1;
				     //if( true != g_PortUseFlag ) {
						 
						 //g_PortUseFlag = true;
						 
						 //System.out.println("[DisConnectDevice()] Device ID: "+ DeviceType);
						 
						 if( 1 == DeviceType ) { //++Note Acceptor
							 
							Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] DisConnectDevice Device Type Note Acceptor",Common.GetLoggingLevelV2() );
						    
						    if( false == g_NoteAccetorActivateFlag ){								
								//g_PortUseFlag = false;
								//System.out.println("[DisConnectDevice()] Note Acceptor Port Not yet connected");
								Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] Note Acceptor Port Not yet connected",Common.GetLoggingLevelV2() );
								return 20;//++Not Yet Connected port already closed
								
							}//if end
							
							//System.out.println("[DisConnectDevice()] Going to close Note Acceptor Port");
						    rtcode = JniDisConnectDevice(DeviceType,Timeout);						    
						    if( (0==rtcode) || (20==rtcode) || (1==rtcode) ){
								g_NoteAccetorActivateFlag = false;
						    }//if end
						 
						 
					     }else if( 2 == DeviceType ) { //++Coin Acceptor with Escrow
							 
							Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] DisConnectDevice Device Type Coin Acceptor",Common.GetLoggingLevelV2() );
							
							if( false == g_CoinAccetorActivateFlag ){								
								//System.out.println("[DisConnectDevice()] Coin Acceptor Not yet connected");
								Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] Coin Acceptor Not yet connected",Common.GetLoggingLevelV2() );
								//g_PortUseFlag = false;
								return 21;//++Not yet connected port already closed								
							}//if end
							
							//System.out.println("[DisConnectDevice()] Going to close Coin Acceptor Port");
							if( ( false == g_CoinAccetorActivateFlag ) && ( false == g_SecurityDeviceActivateFlag ) ){
								rtcode = JniDisConnectDevice(DeviceType,Timeout);
							}else{
								//System.out.println("[DisConnectDevice()] Coin Acceptor Port Closed Default Success");
								Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] Coin Acceptor Port Closed Default Success",Common.GetLoggingLevelV2() );
							    rtcode=0;
						    }//else 
							if( (0==rtcode) || (21==rtcode) || (1==rtcode) ){
								
								//System.out.println("[DisConnectDevice()] Coin Acceptor Port Closed Success");
								Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] Coin Acceptor Port Closed Success",Common.GetLoggingLevelV2() );
								g_CoinAccetorActivateFlag = false;
								
						    }else{								
								//System.out.println("[DisConnectDevice()] Coin Acceptor Port Closed Failed");
								Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] Coin Acceptor Port Closed Failed",Common.GetLoggingLevelV2() );								
						    }//else end
						 }else{
							 //++No Device id matched
							 //System.out.println("[DisConnectDevice()] DisConnectDevice Failed Due to No Device ID Matched");
							 //g_PortUseFlag = false;
							 Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] DisConnectDevice Failed Due to No Device ID Matched",Common.GetLoggingLevelV2() );
							 return 31; //++Other error
						 }//else end
						 
						 //g_PortUseFlag = false;
						 return rtcode;
						 
					 /*} else {
						 //System.out.println("[DisConnectDevice()] DisConnectDevice Failed Due to Communication failure");
						 Currency.Ascrm_WriteLog("API","[Currency][DisConnectDevice()] DisConnectDevice Failed Due to Communication failure",Common.GetLoggingLevelV2() );
						 return 28; //++Communication failure
					 }//else end
					 */
					 
			     }//++synchronized(this) end
			     
	     }//++ public  synchronized int DisConnectDevice(int Timeout) end
	     
	     public  final synchronized byte[] DeviceStatus(int DeviceType,int Timeout) {
	             
	             synchronized(this){
					 int counter=0;
				     byte[] rtcode1= null,rtcode2=null,rtcode3=null;
				     rtcode3 = new byte[12];
				     for(counter=0;counter<12;counter++){
						 rtcode3[counter]=0x00;
					 }//++ for end
					 
				     if( true != g_PortUseFlag ) {
						 
						 //++Ascrm_WriteLog("[Currency]","[DeviceStatus()] Device Type: "+DeviceType, Common.GetLoggingLevelV2() );
						 g_PortUseFlag = true;
						 
						 if( 0 == DeviceType ) { //++Both Device
							
							//++Byte1 Note Accepter status 
							if( true == g_NoteAccetorActivateFlag ){
								 
								rtcode1 = JniDeviceStatus(1,Timeout);
								rtcode3[1] = rtcode1[1]; //++Note Acceptor Status
								//System.out.println("[DeviceStatus() Both Device] Note Acceptor Status Byte: "+rtcode1[1] );
								
							}else{
								
								rtcode3[1] = 20;  //++Note Acceptor not yet connected	
															
							}//else end
							
							//++Byte2 Coin Accepter status
							if( true == g_CoinAccetorActivateFlag ){ 
								rtcode2    = JniDeviceStatus(2,Timeout);
								rtcode3[2] = rtcode2[2]; //++Coin Acceptor Status
								//System.out.println("[DeviceStatus() Both Device] Coin Acceptor Status Byte: "+rtcode2[2] );
							}else {
								
							   rtcode3[2] = 21;  //++Coin Acceptor not yet connected
								
							}//else end
							
							//++ Byte[0] Operation Status Byte
							if(  ( 0 == rtcode1[0] ) &&  ( 0 == rtcode2[0] ) ){
								
								rtcode3[0] = 0; //++Operation Success
								
							} else {
								
								rtcode3[0] = 1;  //++Operation Failed
								
							}//else end
							
							//++Byte3-7 : Escrowed Notes
							if( true == g_NoteAccetorActivateFlag ){ 
								//++Escrow Notes
								rtcode3[3] = rtcode1[3]; 
								rtcode3[4] = rtcode1[4]; 
								rtcode3[5] = rtcode1[5]; 
								rtcode3[6] = rtcode1[6]; 
								rtcode3[7] = rtcode1[7];
								//++RFU
							    rtcode3[8] = 0;
							}//++if end
							
							//++Byte9-11: Escrowed Coins
							//++Byte10:0-3 Indicates no of escrowed INR 5 Coins
                            //++Byte10:4-7 Indicates no of escrowed INR 10 Coins
							if( true == g_CoinAccetorActivateFlag ) {
								//++Escrow Coins
								rtcode3[9]  = rtcode2[9]; 
								rtcode3[10] = rtcode2[10]; 
								rtcode3[11] = rtcode2[11]; 
							}//if end
							
							//++Store old status
							for(counter=0;counter<12;counter++){
								g_DeviceStatus[counter] = 0x00;
							}//++ for end
							for(counter=0;counter<12;counter++){
								g_DeviceStatus[counter] = rtcode3[counter];
							}//++ for end
							
							g_PortUseFlag = false;
							return rtcode3;
							
						 } else {
							 
							 if( (1 == DeviceType) && ( false == g_NoteAccetorActivateFlag ) ) { 
								 
								 rtcode3[0] = 20;  //++Note Acceptor not yet connected
								 g_PortUseFlag = false;
								 return rtcode3;
								 
							 }else if( (2 == DeviceType) && ( false == g_CoinAccetorActivateFlag ) ) { 
								 
								 rtcode3[0] = 21;  //++Coin Acceptor not yet connected
								 g_PortUseFlag = false;
								 return rtcode3;
								
							 }else{
								rtcode1 = JniDeviceStatus(DeviceType,Timeout);
								if( 1 == DeviceType && null!= rtcode1 ){
									//++Store old status
									for(counter=0;counter<12;counter++){
										g_NaDeviceStatus[counter] = 0x00;
									}//++ for end
									for(counter=0;counter<12;counter++){
										g_NaDeviceStatus[counter] = rtcode1[counter];
									}//++ for end
								}else if( 2 == DeviceType && null!= rtcode1 ){
									//++Store old status
									for(counter=0;counter<12;counter++){
										g_CaDeviceStatus[counter] = 0x00;
									}//++ for end
									for(counter=0;counter<12;counter++){
										g_CaDeviceStatus[counter] = rtcode1[counter];
									}//++ for end
								}//++else if end
								g_PortUseFlag = false;
								return rtcode1;
								
							 }//else end
							 
						 }//++else end
					
					 } else {
						
						//++System.out.println("[DeviceStatus()] Device doing busy operation" );
						if( 1 == DeviceType  ){
							Ascrm_WriteLog(g_NADeviceidStr,"[DeviceStatus()] Device doing busy operation ", Common.GetLoggingLevelV2() );
							return g_NaDeviceStatus;
						}else if( 2 == DeviceType  ){
							Ascrm_WriteLog(g_CADeviceidStr,"[DeviceStatus()] Device doing busy operation ", Common.GetLoggingLevelV2() );
							return g_CaDeviceStatus;
						}else if( 0 == DeviceType  ){
							Ascrm_WriteLog("BOTH","[DeviceStatus()] Device doing busy operation ", Common.GetLoggingLevelV2() );
							return g_DeviceStatus;
						}//++else if end
						return null;
						
					 }//else end
				     
			     }//++synchronized(this) end
			     
	     }//++ public  synchronized byte[] DeviceStatus(int DeviceType,int Timeout)
	      
	     protected synchronized  final int defaultCommit(int Timeout) {
	            
	            synchronized(this){
					 
				     int rtcode = -1;
				     if( true != g_PortUseFlag ) {
						 g_PortUseFlag = true;
						 //++rtcode = JnidefaultCommit(Timeout);
						 g_PortUseFlag = false;
						 return rtcode;
					 } else {
						 return 0;
					 }
				     
			     }//++synchronized(this) end
			     
	     }//++ public  synchronized  int defaultCommit(int Timeout) end
	 
         protected synchronized  final int defaultCancel(int Timeout) {
	              
	              synchronized(this){
					 
				     int rtcode = -1;
				     if( true != g_PortUseFlag ) {
						 
						 g_PortUseFlag = true;
						 //++rtcode = JnidefaultCancel(Timeout);
						 g_PortUseFlag = false;
						 return rtcode;
						 
					 } else {
						 return 0;
					 }
				     
			     }//++synchronized(this) end
			     
	     }//++ public  synchronized  int defaultCancel(int Timeout) end
	     
         public  final synchronized  int GetAcceptedAmount(byte[][] AcptdAmt) {
	             
	             synchronized(this){
			
				     int rtcode = -1;
				     
				     //if( true != g_PortUseFlag ) {
						 
						 //g_PortUseFlag = true;
						 
						 if( ( false == g_NoteAccetorActivateFlag ) && (false == g_CoinAccetorActivateFlag) )
						 {
							 
							 if( false == g_NoteAccetorActivateFlag ){
								//g_PortUseFlag = false;
							    Ascrm_WriteLog(g_NADeviceidStr+"/"+g_CADeviceidStr,"[GetAcceptedAmount()] Note Acceptor Port Not yet connected",AscrmLog.INFO);
							    return 21;//++Not Yet Connected port already closed
							 }//if( false == g_NoteAccetorActivateFlag ) end
							 
							 if( false == g_CoinAccetorActivateFlag ){
								//g_PortUseFlag = false;
							    Ascrm_WriteLog(g_NADeviceidStr+"/"+g_CADeviceidStr,"[GetAcceptedAmount()] Coin Acceptor Port Not yet connected",AscrmLog.INFO);
							    return 21;//++Not Yet Connected port already closed
							 }//if( false == g_NoteAccetorActivateFlag ) end
							 
						 }//if end	 
						 
						 rtcode = -1;
						 rtcode = JniGetAcceptedAmount(AcptdAmt);
						 //g_PortUseFlag = false;
						 return rtcode;
					 
					 //} else {
						 //System.out.println("[GetAcceptedAmount()] Other Error");
						 //return 31; //++Other Error
					 //}//else end
					 
				     
			     }//++synchronized(this) end
			     
	     }//++ public  synchronized  int GetAcceptedAmount(byte[] AcptdAmt) end
	 
	     public  final synchronized  boolean AcceptCurrencies(int CurrencyType,int Amount,int Timeout) {
	               
	             synchronized(this){
					 
				     boolean rtcode = false;
				     //if( true != g_PortUseFlag ) {
						 
						  //g_PortUseFlag = true;
						 
						  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						  
						  if( 0 == CurrencyType ) 
						  { //++Both Device
							  
							  if( ( true == g_NoteAccetorActivateFlag ) && (true == g_CoinAccetorActivateFlag) ){
								   //System.out.println("[AcceptCurrencies()] Both device activation found");	
								   Currency.Ascrm_WriteLog("API","[Currency][AcceptCurrencies()] Both device activation found",Common.GetLoggingLevelV2() );
							       rtcode = JniAcceptCurrencies( CurrencyType,Amount,Timeout);
						      }else{
								  rtcode=false;
							  }//else end			
						 
						  }
						  else if( 1 == CurrencyType )
						  { //++Note Acceptor
							  
							  if(  true == g_NoteAccetorActivateFlag  ){	
								  //System.out.println("[AcceptCurrencies()] Only Cash device activation found");	
								  Currency.Ascrm_WriteLog("API","[Currency][AcceptCurrencies()] Only Cash device activation found",Common.GetLoggingLevelV2() );
							      rtcode = JniAcceptCurrencies( CurrencyType,Amount,Timeout);
						      }else{
								  rtcode=false;
							  }//else end							
	                     
	                      }
	                      else if( 2 == CurrencyType )
	                      { //++Coin Acceptor
							  
							  if(  true == g_CoinAccetorActivateFlag  ){
								  //System.out.println("[AcceptCurrencies()] Only Coin device activation found");		
								  Currency.Ascrm_WriteLog("API","[Currency][AcceptCurrencies()] Only Coin device activation found",Common.GetLoggingLevelV2() );
							      rtcode = JniAcceptCurrencies( CurrencyType,Amount,Timeout);
						      }else{
								  rtcode=false;
							  }//else end	
							  					
		                  }
		                  else
		                  {
						     //System.out.println("[AcceptCurrencies()] No Currency Device Found");	
						     Currency.Ascrm_WriteLog("API","[Currency][AcceptCurrencies()] No Currency Device Found",Common.GetLoggingLevelV2() );
						     rtcode=false;
					      }//else end
					     
						 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						 
						 //g_PortUseFlag = false;
						 return rtcode;
						 
					 /*} else {
						 //System.out.println("[AcceptCurrencies()] Other Device using api");
						 Currency.Ascrm_WriteLog(g_NADeviceidStr,"[Currency][AcceptCurrencies()] Other Device using api",Common.GetLoggingLevelV2() );
						 return false;
					 }//else end*/
					 
			     }//++synchronized(this) end  
	               
	     }//++ public  synchronized  boolean AcceptNotes(int Amount,int Timeout) end
	 
	     public  final synchronized  int   StackAcceptedCurrencies(int Timeout) {
		         
		         synchronized(this){
					
				     int rtcode = -1;
				     //if( true != g_PortUseFlag ) {
						 
						 //g_PortUseFlag = true;
						 
						 if( ( true == g_NoteAccetorActivateFlag ) || (true == g_CoinAccetorActivateFlag) )
						 {	
							 rtcode = JniStackAcceptedCurrencies( Timeout);
						 } 
						 else 
						 {
							 
							 if( false == g_NoteAccetorActivateFlag ) 
							 {
								//System.out.println("[StackAcceptedCurrencies()] Note Acceptor Yet not connected");
								Currency.Ascrm_WriteLog("API","[Currency][StackAcceptedCurrencies()] Note Acceptor Yet not connected",Common.GetLoggingLevelV2() );
								rtcode = YET_NOT_CONNECTED_NOTE;
							 } 
							 else if (false == g_CoinAccetorActivateFlag) 
							 {
								//System.out.println("[StackAcceptedCurrencies()] Coin Acceptor Yet not connected");
								Currency.Ascrm_WriteLog("API","[Currency][StackAcceptedCurrencies()] Coin Acceptor Yet not connected",Common.GetLoggingLevelV2() );
								rtcode = YET_NOT_CONNECTED_COIN;
							 }
						 }//else end
						 
						 //g_PortUseFlag = false;
						 return rtcode;
						 
					 //} else {
						 //System.out.println("[StackAcceptedCurrencies()] Other Error");
						 //return 31;
					 //}//else end
				     
				     
			     }//++synchronized(this) end  
			     
	     }//++ public  synchronized  int  StackAcceptedNotes(int Timeout) end
	 
	     public  final synchronized  int   ReturnAcceptedCurrencies(int Timeout) {
		         
		         synchronized(this){
					
				     int rtcode = -1;
				     //if( true != g_PortUseFlag ) {
						 
						 //g_PortUseFlag = true;
						 
						 if( ( true == g_NoteAccetorActivateFlag ) || (true == g_CoinAccetorActivateFlag) ){	
							 rtcode = JniReturnAcceptedCurrencies( Timeout);
						 } else {
							 
							 if( false == g_NoteAccetorActivateFlag ) {
								//System.out.println("[ReturnAcceptedCurrencies()] Note Acceptor Not Yet Connected");
								Currency.Ascrm_WriteLog("API","[Currency][ReturnAcceptedCurrencies()] Note Acceptor Not Yet Connected",Common.GetLoggingLevelV2() );
								rtcode = YET_NOT_CONNECTED_NOTE;
							 } else if (false == g_CoinAccetorActivateFlag) {
								//System.out.println("[ReturnAcceptedCurrencies()] Coin Acceptor Not Yet Connected");
								Currency.Ascrm_WriteLog("API","[Currency][ReturnAcceptedCurrencies()] Coin Acceptor Not Yet Connected",Common.GetLoggingLevelV2() );
								rtcode = YET_NOT_CONNECTED_COIN;
							 }
						 } //else end
						 
						 //g_PortUseFlag = false;
						 return rtcode;
						 
					 //} else {
						 //System.out.println("[ReturnAcceptedCurrencies()] Other Error");
						 //return 31;
					 //}//else end
				     
				     
			     }//++synchronized(this) end  
			     
	     }//++ public  synchronized  int  ReturnAcceptedNotes end
	 
	     public  final synchronized  int   GetValidCurrency(int CurrencyType,int Denom,int Timeout) {
		         
		         synchronized(this){
					 
				     int rtcode = -1;
				     boolean orgrtcode =false;
				     byte[][] AcptdAmt= new byte[40][2];
				     
				     if( true != g_PortUseFlag ) {
						 
						 g_PortUseFlag = true;
						 
						 if( (1 == CurrencyType) &&  (false == g_NoteAccetorActivateFlag) ){
							 g_PortUseFlag = false;
							 System.out.println("[GetValidCurrency()] Note Acceptor Port Not yet connected");
							 return 20;//++Not Yet Connected port already closed
								
					     }//if end
					     
					     if( (2 == CurrencyType) && (false == g_CoinAccetorActivateFlag) ){
							 g_PortUseFlag = false;
							 System.out.println("[GetValidCurrency()] Coin Acceptor Port Not yet connected");
							 return 21;//++Not Yet Connected port already closed
								
					     }//if end
					     					 
						 if( 1 == CurrencyType ){
							 
							 g_PortUseFlag = false;
							 
							 this.EnableSingleCurrencyFlag();
							 orgrtcode= this.AcceptCurrencies( 1, Denom, Timeout); //++NoteAcceptor
							 this.DisableSingleCurrencyFlag();
							 
							 g_PortUseFlag = true;
							 
						 }else if( 2 == CurrencyType ){
							 
							 g_PortUseFlag = false;
							 
							 this.EnableSingleCurrencyFlag();
							 orgrtcode = this.AcceptCurrencies( 2, Denom, Timeout); //++CoinAcceptor
							 this.DisableSingleCurrencyFlag();
							 
							 g_PortUseFlag = true;
							 
						 }else{
							 
							 System.out.println("[GetValidCurrency()] Unable to get any currency type.");
							 g_PortUseFlag = false;
							 return 31; //Other Error
							 
						 }//else end
						 
						 if( true == orgrtcode ){
							 
							 System.out.println("[GetValidCurrency()] AcceptCurrencies() success return.");
							 
							 long end   = 0 ,elapsedTime=0;
							 double elapsedTimeInSecond=0;
							 double fnTimeout = ( Timeout / 1000 );
							 long start = System.nanoTime();
							 rtcode = 0;
							 
							 //synchronized(lock){
							   //GetValidCurrencyStopFlag=false;
							 //}//synchronized(lock) end
							 
							 while(true){
							      
							      synchronized(lock){
									  
									  if(  true == GetValidCurrencyStopFlag  ){
										  rtcode = 18; //++Operation Timeout
										  System.out.println("[GetValidCurrency()] Operation Timeout.");
										  break;
									  }//if(  true == GetValidCurrencyStopFlag  ) end
									  
								  } //synchronized(lock) end
								  
								  //Set Port Status to false
								  g_PortUseFlag = false;
								  rtcode = -1;
							      rtcode = GetAcceptedAmount(AcptdAmt);
							      //++System.out.println("[GetValidCurrency()] GetAcceptedAmount return code: "+rtcode);
							      g_PortUseFlag = true;
							      
							      //System.out.println("[GetValidCurrency()] GetAcceptedAmount return code: "+rtcode);
							      
							      if( 1 == rtcode ){ //++Exact Amount Accepted [C: STATE_EXACT_AMOUNT_ACCEPTED]
									  
									  g_PortUseFlag = false;
									  System.out.println("[GetValidCurrency()]  Exact Amount Accepted.");
									  return 0; // Note/Coin of correct denomination validated
									  
							      }else if( 2 == rtcode ){ //++Excess Amount Accepted [C: STATE_EXCESS_AMOUNT_ACCEPTED]
									  
									  g_PortUseFlag = false;
									  System.out.println("[GetValidCurrency()]  Excess Amount Accepted.");
									  return 31; //Other Error
									  
								  }else if( 0 == rtcode ){ //++Accepting state continuing  [C: STATE_ACCEPTING]
									  
									  //System.out.println("[GetValidCurrency()]  Accepting state continuing.");
									  
								  }else if( 18 == rtcode ){ //++Operation Timeout
								      
								      g_PortUseFlag = false;
									  System.out.println("[GetValidCurrency()] Operation Timeout");
									  return 18; //Operation timeout occured
									  
							      }else if( 10 == rtcode ){ //++[C: NOTE/COIN REJECTED DUE TO INVALID NOTE/COIN ]
								      
								      System.out.println("[GetValidCurrency()] GetAcceptedAmount return code: "+rtcode);
								      g_PortUseFlag = false;
									  System.out.println("[GetValidCurrency()] NOTE/COIN REJECTED DUE TO INVALID NOTE/COIN");
									  return 1; 
									  
							      }else if( 20 == rtcode ){ //++[C: NOTE/COIN REJECTED DUE TO INHIBITED NOTE/COIN ]
								      
								      System.out.println("[GetValidCurrency()] GetAcceptedAmount return code: "+rtcode);
								      g_PortUseFlag = false;
									  System.out.println("[GetValidCurrency()] NOTE/COIN REJECTED DUE TO INHIBITED NOTE/COIN");
									  return 2; 
									  
							      }//++else if end
							      
							      end = System.nanoTime();
							      elapsedTimeInSecond=0;
							      elapsedTime = end-start;
							      elapsedTimeInSecond = (double) (elapsedTime/1000000000) ;
							      //if( elapsedTimeInSecond >= fnTimeout ){
									  //g_PortUseFlag = false;
									  //System.out.println("[GetValidCurrency()] Operation Timeout");
									  //return 18; //Operation timeout occured
								  //}//if end
							      
						     }//++while end
							 
						 }else{
							
							g_PortUseFlag = false;
							System.out.println("[GetValidCurrency()] AcceptCurrencies() failed return.");
						    rtcode = 3 ;//Required Currency Acceptor is not ready
						    
					     }//else end
					     
						 g_PortUseFlag = false;
						 return rtcode;
						 
					 } else {
						 System.out.println("[GetValidCurrency()] Other Api currently working..");
						 return 31; //Other Error
					 }//else end
								     
			     }//++synchronized(this) end 
			     
	     }//++public  synchronized  int  GetValidCurrency(int DeviceType,int Denom,int Timeout)
	 
	     public  final synchronized  int   AcceptCurrentCurrency(int CurrencyType,int Denom,int Timeout) {
		          
		        synchronized(this){
					 
				     int rtcode = -1;
				     if( true != g_PortUseFlag ) {
						 
						      g_PortUseFlag = true;
						  
							  if( (1 == CurrencyType ) && (false == g_NoteAccetorActivateFlag ) ){
								 
								 g_PortUseFlag = false;
								 System.out.println("[ReturnCurrentCurrency()] Note Acceptor Port Not yet connected");
								 return 20;//++Not Yet Connected port already closed
									
							  }//if end
							 
							  if( (2 == CurrencyType ) && (false == g_CoinAccetorActivateFlag ) ){
								 
								 g_PortUseFlag = false;
								 System.out.println("[ReturnCurrentCurrency()] Coin Acceptor Port Not yet connected");
								 return 21;//++Not Yet Connected port already closed
									
							  }//if end	 
				              
				              //synchronized(lock){
							  //	GetValidCurrencyStopFlag=true;
							  //}
							  
							  g_PortUseFlag = false;
							  rtcode = this.StackAcceptedCurrencies(Timeout);
							  g_PortUseFlag = false;
							  return rtcode;
					 
					 } else {
						 System.out.println("[AcceptCurrentCurrency()] Other Error");
						 return 31;//Other Error
					 }//else end
					 
			     }//++synchronized(this) end 
			     
	     }//++ public  synchronized  int  AcceptCurrentCurrency(int Denom,int Timeout)
	     
	     public  final synchronized  int   ReturnCurrentCurrency(int CurrencyType,int Timeout) {
	             
	             synchronized(this){
					 
					 int rtcode = -1;
				     if( true != g_PortUseFlag ) {
						 
						 g_PortUseFlag = true;
						 
						 if( (1 == CurrencyType ) && (false == g_NoteAccetorActivateFlag ) ){
							 
							 g_PortUseFlag = false;
							 System.out.println("[ReturnCurrentCurrency()] Note Acceptor Port Not yet connected");
							 return 20;//++Not Yet Connected port already closed
								
					     }//if end
					     
					     if( (2 == CurrencyType ) && (false == g_CoinAccetorActivateFlag ) ){
							 
							 g_PortUseFlag = false;
							 System.out.println("[ReturnCurrentCurrency()] Coin Acceptor Port Not yet connected");
							 return 21;//++Not Yet Connected port already closed
								
					     }//if end
					     
					     //synchronized(lock){
						 //	GetValidCurrencyStopFlag=true;
						 //}
						 
						 g_PortUseFlag = false;
						 rtcode = ReturnAcceptedCurrencies(Timeout);
						 g_PortUseFlag = false;
						 return rtcode;
						 
					 } else {
						 System.out.println("[ReturnCurrentCurrency()] Other Error");
						 return 31;//Other Error
					 }//else end
					 
				     
			     }//++synchronized(this) end 
			     
	     }//++ public  synchronized  int   ReturnCurrentCurrency(int Timeout)
	     
	     public  final synchronized  int   EnableTheseDenominations(int CurrencyType,int DenomMask,int Timeout) {
	             
	             synchronized(this){
				     int rtcode = -1;
				     //if( true != g_PortUseFlag ) {
						 
						 //System.out.println("[EnableTheseDenominations()] CurrencyType: "+CurrencyType );
						 //++g_PortUseFlag = true;
						 if( (1 == CurrencyType) && ( false == g_NoteAccetorActivateFlag ) ) { 
								 //++Note Acceptor not yet connected
								 //++g_PortUseFlag = false;
								 Currency.Ascrm_WriteLog("API","[Currency][EnableTheseDenominations()] Note Acceptor not yet connected",Common.GetLoggingLevelV2() );
								 return 20;
								 
						 }else if( (2 == CurrencyType) && ( false == g_CoinAccetorActivateFlag ) ) { 
								 
								 //++Coin Acceptor not yet connected
								 //+g_PortUseFlag = false;
								 Currency.Ascrm_WriteLog("API","[Currency][EnableTheseDenominations()] Coin Acceptor not yet connected",Common.GetLoggingLevelV2() );
								 return 21;
								
						 }else if( ( 0 == CurrencyType) && ( false == g_CoinAccetorActivateFlag ) && ( false == g_NoteAccetorActivateFlag ) ) { 
							    //++Other Error No Device yet connected
							    //++g_PortUseFlag = false;
							    Currency.Ascrm_WriteLog("API","[Currency][EnableTheseDenominations()] No Device yet connected",Common.GetLoggingLevelV2() );
								return 31;
						 }else{
							rtcode = JniEnableTheseDenominations( CurrencyType,DenomMask,Timeout );
						 }
					     //++g_PortUseFlag = false;
						 return rtcode;
							
					 /*} else {
						 return 0;
					 }//else end*/
					 
			     }//++synchronized(this) end 
			     
	     }//++ public  synchronized  int   EnableTheseDenominations(int DenomMask,int Timeout)
          
         public  final synchronized  int   IsNoteRemoved(int Timeout) {
                  
                  synchronized(this){
				     int rtcode = -1;
				     //if( true != g_PortUseFlag ) {
						 //g_PortUseFlag = true;
						 if( true == g_NoteAccetorActivateFlag ) {
							for(int retry=0;retry<5;retry++){
								rtcode = -1;
								rtcode = JniIsNoteRemoved( Timeout );
								if( 31 != rtcode ){
									Currency.Ascrm_WriteLog("API","[Currency][IsNoteRemoved()] IsNoteRemoved",Common.GetLoggingLevelV2() );
									return rtcode;
								}else if( 31 == rtcode ){
									Currency.Ascrm_WriteLog("API","[Currency][IsNoteRemoved()] IsNoteRemoved retry",Common.GetLoggingLevelV2() );
									continue;
								}//else end
							}//for end 
							Currency.Ascrm_WriteLog("API","[Currency][IsNoteRemoved()] IsNoteRemoved return",Common.GetLoggingLevelV2() );
							return rtcode;
						 } else {
							 Currency.Ascrm_WriteLog("API","[Currency][IsNoteRemoved()] Port is not yet connected",Common.GetLoggingLevelV2() );
							 rtcode = YET_NOT_CONNECTED_NOTE; //++Note Accepter not yet connected
						 }//else end
						 //g_PortUseFlag = false;
						 return rtcode;
					 /*} else {
						 return 0;
					 }//else end*/
					 
			     }//++synchronized(this) end 
	     }
         
         public  final synchronized  int   ClearJammedCurrencies(int CurrencyType,int EscrowClearnceMode ,int Timeout) {
               
               
                synchronized(this){
					 
				     int rtcode = -1;
				     if( true != g_PortUseFlag ) {
						 
						      g_PortUseFlag = true;
						  
							  if( (1 == CurrencyType ) && (false == g_NoteAccetorActivateFlag ) ){
								 
								 g_PortUseFlag = false;
								 System.out.println("[ClearJammedCurrencies()] Note Acceptor Port Not yet connected");
								 return 20;//++Not Yet Connected port already closed
									
							  }//if end
							 
							  if( (2 == CurrencyType ) && (false == g_CoinAccetorActivateFlag ) ){
								 
								 g_PortUseFlag = false;
								 System.out.println("[ClearJammedCurrencies()] Coin Acceptor Port Not yet connected");
								 return 21;//++Not Yet Connected port already closed
									
							  }//if end	 
				            
				              if( 0 == EscrowClearnceMode  ){ //Reject Mode
				                    rtcode = ClearJammedCurrenciesReject(Timeout, CurrencyType);
						      }else if( 1 == EscrowClearnceMode ){ //Accept Mode
								    rtcode = ClearJammedCurrenciesAccept(Timeout, CurrencyType);
							  }//else if( 1 == EscrowClearnceMode  ) end
							  
							  g_PortUseFlag = false;
							 
							  return rtcode;
					 
					 } else {
						 System.out.println("[ClearJammedCurrencies()] Other Error");
						 return 31;//Other Error
					 }//else end
					 
			     }//++synchronized(this) end 
			     
	      }//++ public  synchronized  int   ClearJammedCurrencies(int CurrencyType,int EscrowClearnceMode ,int Timeout) end
	     
          //++Default constructor
          public Currency(){
                
                int counter=1;
                int NoteAcceptordeviceid=1,CoinAcceptordeviceid=1,CoinEscrowdeviceid=1;
                if( (NoteAcceptordeviceid>0) && (CoinAcceptordeviceid>0) && (CoinEscrowdeviceid>0)){
                
						this.DeviceId=null;
                        this.LogFilePath=null;
                        this.LogFileMode=-1;
                        this.Logdllpath=null;
                        //Setup Logfilepath,Logfilemode,device id
                        
                        //++1.Setup Currency Deviceid
                        String NADeviceidStr=String.format("BNA%d",NoteAcceptordeviceid);
                        String CADeviceidStr=String.format("BCA%d",CoinAcceptordeviceid);
                        String CEDeviceidStr=String.format("BCE%d",CoinEscrowdeviceid);
                        
                                 
						g_NADeviceidStr   = NADeviceidStr;
						g_CADeviceidStr   = CADeviceidStr;
						g_CEDeviceidStr   = CEDeviceidStr;
                        
                        //++2.Setup Logfilepath
						String LogfileNameWithPath = AscrmLog.GetLogFileNameWithPath();
						
						//++3.Setup Logfilemode
                        this.LogFileMode=Common.GetLoggingLevelV2();
					    
					    //++4.Setup Log Dll Path
					    String LogdllPath =AscrmLog.GetLogDllPath();
                        this.DeviceId=NADeviceidStr;
                        this.LogFilePath=LogfileNameWithPath;
                        this.Logdllpath=LogdllPath;
                        
                        //System.out.println("[Currency() Constructor] LogFileName: "+LogfileNameWithPath);
						//System.out.println("[Currency() Constructor] Loglevel Mode: "+Common.GetLoggingLevelV2());
						//System.out.println("[Currency() Constructor] Log dll Path: "+LogdllPath);  
                        
                        //++4.Setup Log Settings in dll
                        Currency.SetupCurrencyLogFile( LogfileNameWithPath,Common.GetLoggingLevelV2(),LogdllPath);
                        
                        //++4.Setup All Device ID
                        Currency.SetupNoteAcceptorID(NADeviceidStr);
						Currency.SetupCoinAcceptorID(CADeviceidStr);
                        Currency.SetupCoinEscrowID(CEDeviceidStr);
                   
				  }//++ if( (0==NoteAcceptordeviceid) && (0==CoinAcceptordeviceid) ) end
              
                  return ;
              
          }//++public Currency()
          
          //++Set Currency Log File setup
          public static final void  fnSetupCurrencyLogFile(){
                  
                  String LogdllPath =AscrmLog.GetLogDllPath();
                  String LogfileNameWithPath = AscrmLog.GetLogFileNameWithPath();
                  Currency.SetupCurrencyLogFile( LogfileNameWithPath,Common.GetLoggingLevelV2(),LogdllPath);
                  return;
                  
	      }//fnSetupCurrencyLogFile() end
	      
          //default destructor
          protected void finalize() throws Throwable {
                this.DeviceId=null;
                this.LogFilePath=null;
                this.LogFileMode=-1;
                this.Logdllpath=null;
          }//protected void finalize() throws Throwable end

          public synchronized String GetDeviceId() {
              return this.DeviceId;
          }

          public synchronized String GetLogFilePath() {
              return this.LogFilePath;
          }
          
          public synchronized int GetLogFileMode() {
              return this.LogFileMode;
          }

          public synchronized String GetLogdllpath() {
              return this.Logdllpath;
          }

          public synchronized void WriteLog(String logmessage) {
			  this.WriteNativeLog(logmessage);
			  return;
          }//public synchronized void WriteLog() end

          public synchronized static String GetDateAndTimeStamp(){
			   SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
			   Date date = new Date();
			   return ("["+dateFormat.format(date)+"]"); //2013/10/15 16:16:39 
		   }//public synchronized static  int GetDateAndTimeStamp() end

		  static { 

						  int rtcode=0,counter=1;
						  
						  for(counter=0;counter<12;counter++){
								g_DeviceStatus[counter]=0x00;
						  }//++ for end
						  for(counter=0;counter<12;counter++){
								g_NaDeviceStatus[counter]=0x00;
						  }//++ for end
						  for(counter=0;counter<12;counter++){
								g_CaDeviceStatus[counter]=0x00;
						  }//++ for end
						  counter=1;
						  
						  try {

							 //System.out.println("[Mega Designs Pvt. Ltd.] Going to load libMegaAscrmNoteAcceptorAPI.so from java libpath.");
			                 System.loadLibrary("MegaAscrmNoteAcceptorAPI");
							 //System.load("/home/user/dist/lib/libMegaAscrmNoteAcceptorAPI.so");
							 
						  }//try end
						  catch(UnsatisfiedLinkError excp)
						  {

											 //System.out.println("[Mega Designs Pvt. Ltd.] Unable to load libMegaAscrmNoteAcceptorAPI.so from java libpath.");

											 //System.out.println("[Mega Designs Pvt. Ltd.] So going to load libMegaAscrmNoteAcceptorAPI.so from AscrmApi.jar file.");

											 if( 1==loadSOFromJar() )
											 {


												 //System.out.println("[Mega Designs Pvt. Ltd.] Before Create Logfile.");

												 if( false == AscrmLog.IsAlreadyCreatedLogFile() )
												 {
												  
												 rtcode = AscrmLog.CreateLogFile();

												 if( 1 == rtcode )
												 {
													System.out.println("[Mega Designs Pvt. Ltd. NoteAcceptor] Logfile created.");
												 }
												 else if( 2 == rtcode )
												 {
													System.out.println("[Mega Designs Pvt. Ltd. NoteAcceptor] Logfile already created.");
												 }
												 else if( -1 == rtcode )
												 {
													System.out.println("[Mega Designs Pvt. Ltd. NoteAcceptor] Logfile already created got exception.");
												 }
												 }
												 else
												 {
													   //System.out.println("[Mega Designs Pvt. Ltd.] File is already created.");


												 }
												 //Store currency load so status to true
												 g_loadCurrency     = true;
												 //System.out.println("[Mega Designs Pvt. Ltd.] After Create Logfile.");
											 }
											 else
											 {
												 System.out.println("[Mega Designs Pvt. Ltd.] load so failed.");
											 } 

							 //excp.printStackTrace();



						  }//catch(UnsatisfiedLinkError excp) end    


		 }//static block end here
		 
		   //++Load libMegaAscrmNoteAcceptorAPI.so from jar
		   
		   public static int loadSOFromJar() {

			         try
					 {

									InputStream fis=null;
									URL res=null;
									File dll=null;
									FileOutputStream fos = null;

									System.out.println("[Mega Designs Pvt. Ltd.] Before Going to delete previous libMegaAscrmNoteAcceptorAPI.so from system temp directory.");

									//Step 1: Delete old dll file from /tmp folder
									Multipliefiledelete("/tmp","libMegaAscrmNoteAcceptorAPI");

									System.out.println("[Mega Designs Pvt. Ltd.] After delete previous libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
					
					                // Get DLL from JAR file 
									System.out.println("[Mega Designs Pvt. Ltd.] Before Load libMegaAscrmNoteAcceptorAPI.so from AscrmApi jar.");

									try
									{
						   
										   res = Currency.class.getResource(MEGAJNISOFILENAME);

									}
									catch(NullPointerException e)
									{
										  System.out.println("[Mega Designs Pvt. Ltd.] Get NullPointerException when create resource from AscrmApi jar.");
										  return 0;

									}
	 
									try
									{
							                //InputStream fis = res.openStream();
											fis = res.openStream();

									}
									catch(IOException e)
									{

										 System.out.println("[Mega Designs Pvt. Ltd.] Get IOException when create stream from AscrmApi jar.");
										 return 0;

									}

									System.out.println("[Mega Designs Pvt. Ltd.] After Load libMegaAscrmNoteAcceptorAPI.so from AscrmApi jar.");

									int SoFileLength=0;

									//Get SO File Size
									SoFileLength=fis.available();
								    
								    System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmNoteAcceptorAPI.so file size = "+SoFileLength+" bytes");

									//Define the destination file
									//createTempFile(String prefix, String suffix)(for temp file name and its extension)
									System.out.println("[Mega Designs Pvt. Ltd.] Before Create temp libMegaAscrmNoteAcceptorAPI.so ");

									try
									{
							
											dll = File.createTempFile("libMegaAscrmNoteAcceptorAPI",".so");

									}
									catch (IllegalArgumentException e)
									{
											System.out.println("[Mega Designs Pvt. Ltd.] Get IllegalArgumentException to create temp libMegaAscrmNoteAcceptorAPI.so ");
											return 0;
										   
									}
									catch (IOException e)
									{

										   System.out.println("[Mega Designs Pvt. Ltd.] Get IOException to create temp libMegaAscrmNoteAcceptorAPI.so ");
										   return 0;
										   

									}
									catch (SecurityException e)
									{

										   System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException to create temp libMegaAscrmNoteAcceptorAPI.so ");
										   return 0;

									}
									

									System.out.println("[Mega Designs Pvt. Ltd.] After Create temp libMegaAscrmNoteAcceptorAPI.so ");

									// Open the destination file 
									System.out.println("[Mega Designs Pvt. Ltd.] Before create stream from original libMegaAscrmNoteAcceptorAPI.so ");
									try
									{
						                   //FileOutputStream fos = new FileOutputStream(dll);
										   fos = new FileOutputStream(dll);

									}
									catch(FileNotFoundException e)
									{

										   System.out.println("[Mega Designs Pvt. Ltd.] Get FileNotFoundException to create stream from AscrmApi.jar. ");
										   return 0;

									} 
									catch( SecurityException e)
									{

										   System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException to create stream from AscrmApi.jar. ");
										   return 0;

									}

									System.out.println("[Mega Designs Pvt. Ltd.] After create stream from original libMegaAscrmNoteAcceptorAPI.so ");

									// Copy the DLL from the JAR to the filesystem 
									byte[] array = new byte[SoFileLength];

									//Reads some number of bytes from the input stream and 
									//stores them into the buffer array.This method blocks 
									//until input data is available, end of file is detected
									//Returns:the total number of bytes read into the buffer, or -1 
									//is there is no more data because the end of the stream has been reached.
									 
									System.out.println("[Mega Designs Pvt. Ltd.] Before create copy original libMegaAscrmNoteAcceptorAPI.so to temp libMegaAscrmNoteAcceptorAPI.so .");
									for(int i=fis.read(array);i!=-1;i=fis.read(array)) 
									{
										try
										{
											fos.write(array,0,i);
										}
										catch(IOException e)
										{
											  System.out.println("[Mega Designs Pvt. Ltd.] Get IOException when write stream libMegaAscrmNoteAcceptorAPI.so to temp libMegaAscrmNoteAcceptorAPI.so .");
											  return 0;

										}

					                }//for end

									System.out.println("[Mega Designs Pvt. Ltd.] After create copy original libMegaAscrmNoteAcceptorAPI.so to temp libMegaAscrmNoteAcceptorAPI.so .");
					                // Close all streams 
									System.out.println("[Mega Designs Pvt. Ltd.] Before Going to close all input output stream .");
									try
									{
						                fos.close();
									}
									catch(IOException e)
									{
										  System.out.println("[Mega Designs Pvt. Ltd.] Get IOException close all output stream .");
										  return 0;

									}
									try
									{
						                fis.close();

									}
									catch(IOException e)
									{
										  
										  System.out.println("[Mega Designs Pvt. Ltd.] Get IOException close all input stream .");
										  return 0;


									}

									//System.out.println("[Mega Designs Pvt. Ltd.] After close all input output stream .");

									// Load the DLL from the filesystem 
									//getAbsolutePath to find the location of temporary file.
									System.out.println("[Mega Designs Pvt. Ltd.] Before going to load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
									try
									{
						                 System.load(dll.getAbsolutePath());
									}
									catch(SecurityException e)
									{
										  System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
										  return 0;
									}
									catch(UnsatisfiedLinkError e) 
									{
										  System.out.println("[Mega Designs Pvt. Ltd.] Get UnsatisfiedLinkError load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
										  return 0;
		
									}
									catch(NullPointerException e)
									{
										  System.out.println("[Mega Designs Pvt. Ltd.] Get NullPointerException load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
										  return 0;
		

									}

									System.out.println("[Mega Designs Pvt. Ltd.] After load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
									
									System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmNoteAcceptorAPI.so load success .");

									return 1;

				}
				catch(Throwable e)
				{
							
							System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmNoteAcceptorAPI.so load failed! .");
					        e.printStackTrace();
							return 0;
				}
			   

		   }//loadSOFromJar() end here
		   
		   public static int Multipliefiledelete(String Directory,String StartingFileNameFilter) {

					int i=0;
					String filenamewithpath;
					String[] s=Search(Directory,StartingFileNameFilter);
					if(s.length<=0)
					{
					   ////System.out.println("[Multipliefiledelete()] No file found in given directory.");
					   ////System.out.println("[Multipliefiledelete()] Program now exit.");
					   return 0;
					}

					//Create boolean variable
					boolean flag[]=new boolean[s.length];
					for (i=0; i< s.length; i++) 
					{
					 String filename = s[i];
					 ////System.out.println("[Multipliefiledelete()] "+filename +" .");
					 filenamewithpath=Directory+"/"+filename;
					 ////System.out.println("[Multipliefiledelete()] "+filenamewithpath +" .");
					 flag[i]=delete(filenamewithpath);
						
					}//for end here

					boolean result=true;
					for (i=0; i< s.length; i++) 
								{
						 result=(result && flag[i]);

								}
					if(false==result)
					{
					////System.out.println("[Multipliefiledelete()] deletion failed! .");
					return 0;
					}
					else
					{
					////System.out.println("[Multipliefiledelete()] deletion success .");
					return 1;
					}


		 }//filedelete end here

		   public static boolean delete(String fileName) {


						boolean success =false;

				File f = new File(fileName);

						try
						{

					if( false == f.exists() ) //SecurityException
					{
					//System.out.println("[Delete()] no such file or directory: "+fileName+" .");
					return false;
					}

						}
						catch(SecurityException e)
						{

								//System.out.println("[Delete()] Get SecurityException when call java file class exists() function.");
							return false;

						}
	 
						try
						{
					if( false == f.canWrite() ) //SecurityException
					{
					//System.out.println("[Delete()] write protected: "+fileName+" .");
					return false;
					}

						}
						catch( SecurityException e )
						{

							   //System.out.println("[Delete()] Get SecurityException when call java file class canWrite() function.");
						   return false;

						}
	 
						try
						{

					if( true == f.isDirectory() ) //SecurityException
					{

						  String[] files = f.list();

						  if(files.length > 0)
						  {

						   //System.out.println("[Delete()] directory not empty: "+fileName+" .");

						   return false;


						  }

					}

						}
						catch( SecurityException e )
						{

							   //System.out.println("[Delete()] Get SecurityException when call java file class isDirectory() function.");
						   return false;

						}


				try
						{
						success = f.delete(); //SecurityException

					if( false == success )
					{
					  ////System.out.println("[Delete()] deletion failed! .");
					  return false;
					}
					else if( true == success )
					{
					 ////System.out.println("[Delete()] deletion success .");
					 return true;
					}

						}
						catch(SecurityException e)
						{

							   //System.out.println("[Delete()] Get SecurityException when call java file class delete() function.");
						   return false;

						}

						return false;
		   
		}//delete() end here

		   public static String[] Search(String Directory,final String FileNameFilter) {
		   
		      File dir = new File(Directory);

		      FilenameFilter filter = new FilenameFilter() 
		      {

			 public boolean accept(File dir, String name) 
		         {
			    return name.startsWith(FileNameFilter);
			 }


		      };

		      String[] children = dir.list(filter); //SecurityException


                      ////////////////////////////////////////////////////////////////////////////////////////////////

		      /*

                      //Testing Display purpose

		      if (children == null) 
		      {
			  //System.out.println("[Search()] Either dir does not exist or is not a directory.");
			 
		      } 
		      else 
		      {
			 for (int i=0; i< children.length; i++) 
			 {
			    String filename = children[i];
			    //System.out.println("[Search()] "+filename+" .");
			 }
			 
		      } 

		      */
                    
                      ////////////////////////////////////////////////////////////////////////////////////////////////

		      return children;


	  }//search end here

           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}//++class Currency end here
