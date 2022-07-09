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
import java.util.Arrays;

public final class Security {
	
         private static Security single_instance = null; 
         private static final String MEGAJNISOFILENAME ="libMegaAscrmSecurityAPI.so";   
         private String DeviceId;
         private String LogFilePath;
         private int LogFileMode;
         private String Logdllpath;
         private static boolean SecurityPortStatus=false;
         
         private static String g_securityDeviceidStr   = null;
         private static int    g_MainDoorStatus        = -1;
         private static int    g_CashDoorStatus        = -1;
         
         public Security() {
                
                int SecurityDeviceId=1;
                //Setup Logfilepath,Logfilemode,device id
                if(SecurityDeviceId < 0){
                    throw new IllegalArgumentException("Must be postive security device id");
                }//if end
                int counter=1;
                this.DeviceId=null;
                this.LogFilePath=null;
                this.LogFileMode=-1;
                this.Logdllpath=null;
                
                //Check and Store Currency So File
                if( false == Currency.g_loadCurrency ){
					if( 1 == Currency.loadSOFromJar() ){
						Currency.g_loadCurrency = true;
					}//if end
				}//if end
			
                //Setup Logfilepath,Logfilemode,device id
                //++1.Setup Deviceid
                String DeviceidStr=String.format("KSD%d",SecurityDeviceId );
                g_securityDeviceidStr = null;
                g_securityDeviceidStr = DeviceidStr;
                //++2.Setup Logfilepath
                String LogfileNameWithPath = AscrmLog.GetLogFileNameWithPath();
                //++3.Setup Logfilemode
                int LogLevelMode = -1;
                int mode=Common.GetLoggingLevelV2();
                if( mode>=0 ){
                   LogLevelMode= mode;
                }else{
                    LogLevelMode = 0;
                }//else end
                String LogdllPath =AscrmLog.GetLogDllPath();;
                this.DeviceId=DeviceidStr;
                this.LogFilePath=LogfileNameWithPath;
                this.LogFileMode=LogLevelMode;
                this.Logdllpath=LogdllPath;
                
                //System.out.println("[Security() Constructor] LogFileName: "+LogfileNameWithPath);
                //System.out.println("[Security() Constructor] Loglevel Mode: "+LogLevelMode);
                //System.out.println("[Security() Constructor] Device Id: "+DeviceidStr);
                //System.out.println("[Security() Constructor] Log dll Path: "+LogdllPath);  
                
                //++Set Log Mode and Device ID String
                Currency.SetupCurrencyLogFile( LogfileNameWithPath,Common.GetLoggingLevelV2(),LogdllPath);
                Currency.SetupKioskSecurityID(DeviceidStr);
                
         }//public Security()
         
         //++default6 destructor
         protected void finalize() throws Throwable {
                this.DeviceId=null;
                this.LogFilePath=null;
                this.LogFileMode=-1;
                this.Logdllpath=null;
         }//protected void finalize() throws Throwable end

         public  final synchronized    int ConnectDevice(int portid,int Timeout){
			  
			  synchronized(this){
				  int rtcode =-1;
				  //if( true != Currency.g_PortUseFlag ) {
					  
			           //++Currency.g_PortUseFlag = true;
			           //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
			           if( false == SecurityPortStatus ){
						   rtcode = Currency.ActivateCCTalkPort(2,portid);
			           }else{
						   //System.out.println("[Security][ConnectDevice()] Security Device Port Already Connected");
						   rtcode=Common.DEVICE_ALREADY_CONNECTED;//Already Connected
					   }//else end
			           if( 0 == rtcode ){
						 //System.out.println("[Security][ConnectDevice()] Security Device Port Connect Success");
						 Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[Security][ConnectDevice()] Security Device Port Connect Success",Common.GetLoggingLevelV2() );
						 SecurityPortStatus = true;   
					   }//if end
					   //++Currency.g_PortUseFlag = false;
					   
					   //++Init Door Status
					   this.GetDoorStatus(0);
					   
					   //++Init Vault Door Status
					   this.GetDoorStatus(1);
					   
			           return rtcode;
			           
			      /*}else {
					   //System.out.println("[Security][ConnectDevice()] Security Device Port Connect Failed");
					   return Common.COMMUNICATION_FAILURE; //Communication failure
				  }//else end
				  */
			  }//++ synchronized(this) end
			  
         }//++ public  synchronized    int ActivateSecurityPort(int portid) end
         
         public  final synchronized    int DisConnectDevice(int Timeout) {
			 
			  synchronized(this){
				  
				  int rtcode =-1;
				  //if( true != Currency.g_PortUseFlag ) {
					  
			           //Currency.g_PortUseFlag = true;
			           //++Device Flag: 0: Coin Acceptor 1: Token Dispenser 2: Security 
			           if( true == SecurityPortStatus ){
							rtcode = Currency.DeActivateCCTalkPort(2);
							if( 0 == rtcode ){
								//System.out.println("[Security][DisConnectDevice()] Security Device Port DisConnectDevice Success");
								Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[Security][ConnectDevice()] Security Device Port DisConnectDevice Success",Common.GetLoggingLevelV2() );
							    SecurityPortStatus = false;	
							}//if end
					   } else {
						    //System.out.println("[Security][DisConnectDevice()] Security Device Port DisConnectDevice failed due to no activate prior");
						    Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[Security][ConnectDevice()] Security Device Port DisConnectDevice failed due to no activate prior",Common.GetLoggingLevelV2() );
						    rtcode = Common.DEVICE_NOT_YET_CONNECTED;  //++Security device not yet connected
					   }//else end
			           //Currency.g_PortUseFlag = false;
			           return rtcode;
			      /*}else {
					   //System.out.println("[Security][DisConnectDevice()] Security Device Port DisConnect Failed");
					   return Common.COMMUNICATION_FAILURE; //Communication failure
				  }//else end
				  */
			  }//++ synchronized(this) end
			  
	     }//++ public  synchronized    int DeActivateSecurityPort() end
	     
	     //++ASCRM Version
	     public  final synchronized    int DisableAlarm(int DoorType,int Time) {
			 synchronized(this){
				  int rtcode =-1;
				  //if( true != Currency.g_PortUseFlag ) {
					  
			           //Currency.g_PortUseFlag = true;
			           
			           if( false == SecurityPortStatus ){
			                Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[Security][DisConnectDevice()] No Security Device Port Activated",Common.GetLoggingLevelV2() );
			                rtcode=Common.COMMUNICATION_FAILURE;//Communication Failure
				       }else {
						   byte alarmlogicbits=0x00;
						   if( 0 == DoorType ){ //++ Both Door
							   alarmlogicbits=(byte)0b11100001;
							   rtcode = Currency.JniDisableAlarm( 1,Time,2,Time,alarmlogicbits);
						   } else if( 1 == DoorType ){ //++ Top Door
							   alarmlogicbits=(byte)0b01100001;
							   rtcode = Currency.JniDisableAlarm( 1,Time,0,0,alarmlogicbits);
						   } else if( 2 == DoorType ){ //++ Vault Door
							   alarmlogicbits=(byte)0b10100001;
							   rtcode = Currency.JniDisableAlarm( 0,0,2,Time,alarmlogicbits);
						   }//else if end
					   }//else end
					   
			           //Currency.g_PortUseFlag = false;
			           return rtcode;
			           
			      /*}else {
					   //System.out.println("[Security][DisConnectDevice()] Communication failure");
					   return Common.COMMUNICATION_FAILURE; //++Communication Failure
				  }*/
				  
			  }//++ synchronized(this) end
			  
	     }//public  synchronized    int DisableAlarm(int DoorType,int Time) end
                 
	     public  final synchronized    int DisableAlarm( int DoorType1,int time1,int DoorType2,int time2,byte alarmlogicbits) {
             
             synchronized(this){
				  int rtcode =-1;
				  if( true != Currency.g_PortUseFlag ) {
					  
			           Currency.g_PortUseFlag = true;
			           rtcode = Currency.JniDisableAlarm( DoorType1,time1,DoorType2,time2,alarmlogicbits);
			           Currency.g_PortUseFlag = false;
			           return rtcode;
			           
			      }else {
					   //System.out.println("[Security][DisableAlarm()] Communication failure");
					   return Common.COMMUNICATION_FAILURE; //SUccess
				  }
				  
			  }//++ synchronized(this) end
			  
	     }//++ public  synchronized    int DisableAlarm( int DoorType1,int time1,int DoorType2,int time2) end
	     
	     /*
		 * 0 : Main Door
		 * 1 : CashBox Door
	     */
	     public  final synchronized    int GetDoorStatus(int DoorType){
	     	 
	     	 synchronized(this){
				 
				  int rtcode =-1;
				  int CCTalkBusyflag = Currency.IsCCTALKPortBusy();
				  if( 1 == CCTalkBusyflag ) { //++ Not Busy
			           if( false == SecurityPortStatus )
			           {
			                Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[Security][GetDoorStatus()] No Security Device Port Activated",Common.GetLoggingLevelV2() );
			                rtcode=28;//Communication failure			                
				       }
				       else{ 					   
						   // [Java][ 0 = Main Door and 1 : Cash vault ]
						   rtcode = Currency.JniGetDoorStatus( DoorType);
						   if( 0 == DoorType )
						   {
								g_MainDoorStatus = rtcode;
						   }
						   else if( 1 == DoorType )
						   {
								g_CashDoorStatus = rtcode;
						   }//else if end
					   }//else end
			           return rtcode;
			      }else { //++Busy
					   Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[GetDoorStatus()] Device doing door status busy operation ", Common.GetLoggingLevelV2() );
					   if( 0 == DoorType ){
							rtcode = g_MainDoorStatus;
							if( 0 == rtcode )
							{
								Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[GetDoorStatus()] Main Door Open ", Common.GetLoggingLevelV2() );
							}
							else{
								Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[GetDoorStatus()] Main Door Closed ", Common.GetLoggingLevelV2() );
							}	
					   }else if( 1 == DoorType ){
							rtcode = g_CashDoorStatus;
							if( 0 == rtcode )
							{
								Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[GetDoorStatus()] CashBox Door Open ", Common.GetLoggingLevelV2() );
							}
							else{
								Currency.Ascrm_WriteLog(g_securityDeviceidStr,"[GetDoorStatus()] CashBox Door Closed ", Common.GetLoggingLevelV2() );
							}	
					   }//else if end
					   return rtcode;
				  }//++else
				  
			  }//++ synchronized(this) end
	     	 
         }//++ public  synchronized    int GetDoorStatus(int DoorType) end
         
         private final synchronized    int GetSecurityVersion( byte[] VersionString ) {
		       
		       synchronized(this){
				  int rtcode =-1;
				  //if( true != Currency.g_PortUseFlag ) {
			           //Currency.g_PortUseFlag = true;
			           rtcode = Currency.JniGetSecurityVersion( VersionString );
			           //Currency.g_PortUseFlag = false;
			           return rtcode;
			      /*}else {
					   return 0;
				  }*/
				  
			  }//++ synchronized(this) end
		 
	     }// GetSecurityVersion end
	 
	     public  final synchronized    String GetNativeLibVersion() {
			 return "01.00.00"; 
		 }//public  synchronized    String GetNativeLibVersion() end
		 
		 public final synchronized    String GetSecurityDevFWVersion() {
			 
			 byte[] SecurityVersion=new byte[5];
			 int rtcode = Currency.JniGetSecurityVersion(SecurityVersion);
			 if( 1 == rtcode ){
			    return Arrays.toString(SecurityVersion);
		     }else {
				return null;
			 }//else 
			  
		 }//public  synchronized    String GetSecurityDevFWVersion()  end
		          
         public final synchronized String GetDeviceId() {
              return this.DeviceId;

         }//public synchronized String GetDeviceId() end
          
         public final synchronized String GetLogFilePath() {
              return this.LogFilePath;

         }//public synchronized String GetLogFilePath() end
          
         public final synchronized int GetLogFileMode() {
              return this.LogFileMode;

         }// public synchronized int GetLogFileMode()  end
          
         public final synchronized String GetLogdllpath() {
              return this.Logdllpath;

         }//public synchronized String GetLogdllpath() end
          
         public final synchronized static String GetDateAndTimeStamp() {
              SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
			  Date date = new Date();
			  return ("["+dateFormat.format(date)+"]"); //2013/10/15 16:16:39 
         }//public synchronized static  int GetDateAndTimeStamp() end


}//public class Security end
