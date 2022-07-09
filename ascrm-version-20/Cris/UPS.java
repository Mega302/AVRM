package Cris;

public final class UPS{
	
	    private static int DEVICE_ALREADY_CONNECTED=20;
        private static int DEVICE_NOT_YET_CONNECTED=20;
        private static int OTHER_ERROR=31;
        private static int COMMUNICATION_FAILURE=28;
        
        private String DeviceId;
        private String LogFilePath;
        private int LogFileMode;
        private String Logdllpath;
         
        public  UPS(){
		        
		        int UPSDeviceId=1;
                //Setup Logfilepath,Logfilemode,device id
                if(UPSDeviceId < 0){
                    throw new IllegalArgumentException("Must be postive UPS device id");
                }//if end
                
                //++Check and Store Currency So File
                if( false == Currency.g_loadCurrency ){
					if( 1 == Currency.loadSOFromJar() ){
						Currency.g_loadCurrency = true;
					}//if end
				}//if end
			

                int counter=1;
                this.DeviceId=null;
                this.LogFilePath=null;
                this.LogFileMode=-1;
                this.Logdllpath=null;
                //Setup Logfilepath,Logfilemode,device id
                //++1.Setup Deviceid
                String DeviceidStr=String.format("UPS%d",UPSDeviceId );
                //++2.Setup Logfilepath
                String LogfileNameWithPath = AscrmLog.GetLogFileNameWithPath();
                //++3.Setup Logfilemode
                int LogLevelMode = Common.ALL;
                int mode=Common.GetLoggingLevelV2();
                if( mode>=0 ){
                   LogLevelMode= mode;
                }else{
                    LogLevelMode = Common.ALL;
                }//else end
                String LogdllPath =AscrmLog.GetLogDllPath();;
                this.DeviceId=DeviceidStr;
                this.LogFilePath=LogfileNameWithPath;
                this.LogFileMode=LogLevelMode;
                this.Logdllpath=LogdllPath;
                //++Set Log Mode and Device ID String
                Currency.SetupCurrencyLogFile( LogfileNameWithPath,Common.GetLoggingLevelV2(),LogdllPath);
                Currency.SetupUPSID(DeviceidStr);
                //System.out.println("[UPS() Constructor] LogFileName: "+LogfileNameWithPath);
                //System.out.println("[UPS() Constructor] Loglevel Mode: "+LogLevelMode);
                //System.out.println("[UPS() Constructor] Device Id: "+DeviceidStr);
                //System.out.println("[UPS() Constructor] Log dll Path: "+LogdllPath);  
                
		}//public  UPS() end
		
		public  synchronized String GetNativeLibVersion(){
	         return "01.00.00";
	    }//public  synchronized String GetNativeLibVersion() end
	    
	    public  synchronized String GetUPSFWVersion(){
	         return "01.00.00";
	    }//public  synchronized String GetUPSFWVersion() end
        
        public  synchronized int ConnectDevice(int PortId,int Timeout){
			  
			  synchronized(this){
				  
				  int rtcode =-1;
			      rtcode = Currency.JniGetUPSStatus();
			      if( (0==rtcode) || (1==rtcode) ){
					  return 0; //Ups Connected Successfully
				  }else{
			          return COMMUNICATION_FAILURE; //Ups Not Connected
			      }//else end
			  
			  }//++ synchronized(this) end
			  
		}//public  synchronized int ConnectDevice(int PortId,int Timeout) end
		
		public  synchronized int DisconnectDevice(int Timeout){
			  
			  synchronized(this){
				 return 0; //Ups Disconnected Successfully
			  }//++ synchronized(this) end
			  
		}//public  synchronized int DisconnectDevice(int PortId,int Timeout) end
		
        public  synchronized int GetUPSStatus(){
			  
			  synchronized(this){
				  int rtcode =-1;
			      rtcode = Currency.JniGetUPSStatus();
			      return rtcode;
			  }//++ synchronized(this) end
			  
         }//++ public  synchronized    int GetUPSStatus(int portid) end
         
        public  synchronized int GetBatteryStatus(){
			  
			  synchronized(this){
				  int rtcode =-1;
			      rtcode = Currency.JniGetBatteryStatus();
			      rtcode = -(100-rtcode);
			      System.out.println( "[GetBatteryStatus()] Battery Status: "+ rtcode);
			      return rtcode;
			  }//++ synchronized(this) end
			  
		}//++ public  synchronized    int GetBatteryStatus end

}//public class UPS end
