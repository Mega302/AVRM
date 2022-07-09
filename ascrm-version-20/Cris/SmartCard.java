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

public final class SmartCard {
	

         private static int g_SmartCardDeviceid=0;
         private boolean SmartCardConnectFlag=false;
         private static final String MEGAGATEWAYJNISOFILENAME ="libMegaAscrmSmartCardAPIGateWay.so";   
         private static final String MEGAJNISOFILENAME ="libMegaAscrmSmartCardAPI.so";
         private String dllpath;
         
         //++AVRM API List
         private synchronized native  boolean JniSetupSmartCardLogFile( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath);
         private synchronized native  int JniGetLastError( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath );
         private synchronized native  int GetCardBlockStatus( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath);
         private synchronized native  int CardBlockStatusRequest( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath);
         private synchronized native  int MutekInitWithCardRetain( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath);
         private synchronized native  int MutekInitWithCardMoveInFront( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath);
         private synchronized native  int MutekInitWithErrorCardBin( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath);
         private synchronized native  int JniConnectDevice( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath ,int PortId,int ChannelClearanceMode, int Timeout );
         private synchronized native  int JniDisConnectDevice( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  byte[] JniGetDeviceStatus( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int ComponentId,int Timeout);
         private synchronized native  int JniAcceptCard(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  int JniCollectCard(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  int JniDisableCardAcceptance(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
		 private synchronized native  int JniDispenseCard(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  int JniEnableCardAcceptance(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  int JniIsCardInChannel(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  int JniIsCardRemoved(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  int JniRejectCard(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int Timeout);
         private synchronized native  int JniReturnCard( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int DispenseMode,int Timeout); 
         private synchronized native  int JniSAMSlotPowerOnOff( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int SAMSlotId,int PowerOnOffState );
         private synchronized native  byte[] JniResetSAM( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int SAMSlotId,int ResetType,int Timeout);
         private synchronized native  byte[] JniActivateCard( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int CardTechType,int SAMSlotId,int Timeout);
         private synchronized native  int JniDeactivateCard( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int CardTechType,int SAMSlotId,int Timeout);
         private synchronized native  byte[] JniXChangeAPDU( String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath,int CardTechType,byte[] CommandAPDU,int SAMSlotId,int Timeout);
         private synchronized native  int JniisSAMEnable(String dllpath,String Logfile,int LogLevel,String DeviceId,String LogdllPath);
         public  synchronized byte[] ReadUltralightBlock(int Addr,int Timeout){
	             return null;
	     }//public  synchronized byte[] ReadUltralightBlock(int Addr,int Timeout) end
         public  synchronized int WriteUltralightPage(int Addr,byte[] Data,int Timeout){
	             return 0;
	     }//public  synchronized int WriteUltralightPage(int Addr,byte[] Data,int Timeout) end	 

         private void ReturnBlockCard() {

                   if( 1 == CardBlockStatusRequest( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath)){
                       MutekInitWithCardMoveInFront( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath);
                   }//if end

                   return;

         }//private void ReturnBlockCard() end      

         public synchronized int GetSmartCardLastError(){
                  return( JniGetLastError(this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath) );                
         }//public  synchronized int GetLastError() end
      
         //++Smartcard API List        
         public synchronized String GetSCCardDevNativeLibVersion(){
			 return "01.00.00";
		 }
		 
		 public synchronized String GetSCCardDevFWVersion(){
			 return "01.00.00";
		 }
		 
		 public synchronized String GetSCCardReaderDevFWVersion(){
			 return "01.00.00";
		 }

         public synchronized int ConnectDevice(int PortId,int ChannelClearanceMode, int Timeout) {

                int rtcode =0;
                if( false == this.SmartCardConnectFlag ){
					
					rtcode = JniConnectDevice( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,PortId,ChannelClearanceMode, Timeout);
				    if( 2 == rtcode ) { //++Nak or EOT
					   rtcode =0;
					   rtcode = JniConnectDevice( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,PortId,ChannelClearanceMode, Timeout) ;
					   if( (0==rtcode) || (1==rtcode) || (2==rtcode) || (3==rtcode) || (20==rtcode) ){
							SmartCardConnectFlag = true; //Device now connected
					   }//++if end
					   return rtcode;
					   
					}else{
						
					   if( (0==rtcode) || (1==rtcode) || (2==rtcode) || (3==rtcode) || (20==rtcode) ){
							SmartCardConnectFlag = true; //Device now connected
					   }//++if end
					   return rtcode;
					   
					}//++else end
             }else {
				 return Common.DEVICE_ALREADY_CONNECTED;
			 }//else end

         }//++public int ConnectDevice(int PortId,int ChannelClearanceMode, int Timeout) end
         
         public synchronized int DisConnectDevice(int Timeout) {
               
               int rtcode =0;
               if( true == this.SmartCardConnectFlag ) {
				   rtcode = JniDisConnectDevice( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout);
				   //1 = DisConnected Successfully but a Card in the Channel 
				   //0 = DisConnected Successfully 
				   if( (0==rtcode) || (1==rtcode) || (Common.DEVICE_NOT_YET_CONNECTED==rtcode) || (Common.OTHER_ERROR==rtcode) ){ //0:Disconnect 20: Not yet connectd 
							SmartCardConnectFlag = false; //Device now disconnected
				   }//++if end
				   return rtcode;
              }else{
				  return Common.DEVICE_NOT_YET_CONNECTED;
			  }//else end
			  
         }//public synchronized int DisConnectDevice(int Timeout) end

         public synchronized byte[] GetDeviceStatus(int ComponentId,int Timeout) {
			 if( true == this.SmartCardConnectFlag ) {
				return(JniGetDeviceStatus(this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,ComponentId,Timeout));
			 }else {
		          byte[] DeviceStatus= new byte[10];
		          DeviceStatus[0] = (byte)Common.OTHER_ERROR; 
		          return DeviceStatus;
		     }//else end
         }//++public synchronized byte[] GetDeviceStatus(int ComponentId,int Timeout) end

	     public synchronized int AcceptCard(int Timeout) {
             if( true == this.SmartCardConnectFlag ) {
				return(JniAcceptCard( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout));
			 }else{
				return Common.DEVICE_NOT_YET_CONNECTED;
			 }
         }//public synchronized int AcceptCard(int Timeout) end

         public synchronized int DisableCardAcceptance(int Timeout) {
              if( true == this.SmartCardConnectFlag ) {
				return(JniDisableCardAcceptance( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout));
			  }else{
				return Common.DEVICE_NOT_YET_CONNECTED;
			 }
         }//public synchronized int DisableCardAcceptance(int Timeout)end

         public synchronized int EnableCardAcceptance(int Timeout) {
              if( true == this.SmartCardConnectFlag ) {
			      return(JniEnableCardAcceptance( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout));
			  }else {
				  return Common.DEVICE_NOT_YET_CONNECTED;
			  }
			  
         }//public synchronized int EnableCardAcceptance(int Timeout) end

	     public synchronized int IsCardInChannel(int Timeout) {
             if( true == this.SmartCardConnectFlag ) {
				return(JniIsCardInChannel(   this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout));
			 }else {
				  return Common.DEVICE_NOT_YET_CONNECTED;
			 }
         }//public synchronized int IsCardInChannel(int Timeout) end

	     public synchronized int IsCardRemoved(int Timeout) {
			 if( true == this.SmartCardConnectFlag ) {
				 return(JniIsCardRemoved(this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout));
             }else{
				  return Common.DEVICE_NOT_YET_CONNECTED;
			 }//else end
			  
         }//public synchronized int IsCardRemoved(int Timeout) end

         public synchronized int DispenseCard(int Timeout) {
              
              if( true == this.SmartCardConnectFlag ) {
				  return(JniDispenseCard( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout));
			  }else{
				  return Common.DEVICE_NOT_YET_CONNECTED;
			  }//else end
			 
         }//public synchronized int DispenseCard(int Timeout) end
 
         public synchronized int CollectCard(int Timeout) {  
			
			if( true == this.SmartCardConnectFlag ) {
				return( JniCollectCard( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout) );
			}else{
				return Common.DEVICE_NOT_YET_CONNECTED;
			}//else end
			
         }//public synchronized int CollectCard(int Timeout) end

	     public synchronized int RejectCard(int Timeout) {
             
             if( true == this.SmartCardConnectFlag ) {
				return(JniRejectCard(  this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,Timeout));
			 }else{
				return Common.DEVICE_NOT_YET_CONNECTED;
			}//else end
			
         }//public synchronized int RejectCard(int Timeout) end

	     public synchronized int ReturnCard(int DispenseMode,int Timeout) {
               
               if( true == this.SmartCardConnectFlag ) { 
				   return(JniReturnCard( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,DispenseMode,Timeout));
               }else{
				   return Common.DEVICE_NOT_YET_CONNECTED;
			   }//else end
			   
         }//public synchronized int ReturnCard(int DispenseMode,int Timeout)end

         public synchronized int SAMSlotPowerOnOff(int SAMSlotId,int PowerOnOffState,int Timeout) {
              
              if( true == this.SmartCardConnectFlag ) { 
				  return(JniSAMSlotPowerOnOff( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,SAMSlotId,PowerOnOffState));
			  }else{
				   return Common.DEVICE_NOT_YET_CONNECTED;
			  }//else end
			  
         }//public synchronized int SAMSlotPowerOnOff(int SAMSlotId,int PowerOnOffState) end

	     public synchronized byte[] ResetSAM(int SAMSlotId,int ResetType,int Timeout) {
              
              if( true == this.SmartCardConnectFlag ) { 
				   return(JniResetSAM(  this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,SAMSlotId,ResetType,Timeout));
              }else{
				   byte[] ResetSAM = new byte[1];
				   ResetSAM[0] = (byte)Common.DEVICE_NOT_YET_CONNECTED;
				   return ResetSAM;
			  }//else end

            
         }// public synchronized byte[] ResetSAM(int SAMSlotId,int ResetType,int Timeout) end

	     public synchronized byte[] ActivateCard(int CardTechType,int SAMSlotId,int Timeout) {

              if( true == this.SmartCardConnectFlag ) { 
				   return(JniActivateCard( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,CardTechType,SAMSlotId,Timeout));
              }else{
				   byte[] ActivateCard = new byte[10];
				   ActivateCard[0] = (byte)Common.DEVICE_NOT_YET_CONNECTED;
				   return ActivateCard;
			  }//else end
			  
         }//public synchronized byte[] ActivateCard(int CardTechType,int SAMSlotId,int Timeout) end

	     public synchronized int DeactivateCard(int CardTechType,int SAMSlotId,int Timeout) {

              if( true == this.SmartCardConnectFlag ) { 
                  return(JniDeactivateCard(this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,CardTechType,SAMSlotId,Timeout));
              }else{
				   return Common.DEVICE_NOT_YET_CONNECTED;
			  }//else end
			  
         }//public synchronized int DeactivateCard(int CardTechType,int SAMSlotId,int Timeout) end

         public synchronized byte[] XChangeAPDU(int CardTechType, byte[] CommandAPDU,int SAMSlotId,int Timeout ) {

                if( true == this.SmartCardConnectFlag ) { 
					return(JniXChangeAPDU( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath,CardTechType,CommandAPDU,SAMSlotId,Timeout));
                }else{
				    byte[] XChangeAPDU = new byte[1];
				    XChangeAPDU[0] = (byte)Common.DEVICE_NOT_YET_CONNECTED;
				    return XChangeAPDU;
			    }//else end
 
         }//public synchronized byte[] XChangeAPDU end

         public synchronized int InitMutek(int Choice) {

 
               if( true == this.SmartCardConnectFlag ) { 
       
                      switch(Choice)
                      {

		                  case 0:
		                         //Keep Card
		                         MutekInitWithCardRetain( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath);
		                         return 0;

		                  case 1:  
		                           //Collect Card
		                           MutekInitWithErrorCardBin( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath);
		                           return 1;
		                  case 2:
		                           //Return Card
		                           MutekInitWithCardMoveInFront( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath);
		                           return 2;

		                  default:
		                          
		                          break;
                      }

                      return 0;
                      
                 }else{
				   return Common.DEVICE_NOT_YET_CONNECTED;
			    }//else end
		      

         }//public synchronized int DispenseCard(int Timeout) end
         
         public synchronized int isSAMEnable() {
             
             if( true == this.SmartCardConnectFlag ) { 
				return (  JniisSAMEnable( this.dllpath,this.LogFilePath,this.LogFileMode,this.DeviceId,this.Logdllpath ) );
		     }else{
				return Common.DEVICE_NOT_YET_CONNECTED;
			 }//else end 
			                  
		 }//public synchronized int DispenseCard(int Timeout) end
         
          private native void WriteNativeLog(String message);
          private String DeviceId;
          private String LogFilePath;
          private int LogFileMode;
          private String Logdllpath;

          static { 
                                 
                                 int rtcode=0,counter=1;
                                 System.out.println("\n[Mega Designs Pvt. Ltd. SmartCard] Unable to load libMegaAscrmSmartCardAPI.so from java libpath.");
                                 System.out.println("\n[Mega Designs Pvt. Ltd. SmartCard] So going to load libMegaAscrmSmartCardAPI.so from AVRMApi.jar file.");
                                 Multipliefiledelete("/tmp","libMegaAscrmSmartCardAPIGateWay");
                                 Multipliefiledelete("/tmp","libMegaAscrmSmartCardAPI");
				                 rtcode =1;
				                 rtcode = loadSOFromJar();
				                 if( 1== rtcode){

                                     System.out.println("[Mega Designs Pvt. Ltd. SmartCard] So Load Successfull.");
                                     if( false == AscrmLog.IsAlreadyCreatedLogFile() ){
                                             //System.out.println("[Mega Designs Pvt. Ltd.] File is not created.")
                                             //System.out.println("[Mega Designs Pvt. Ltd.] Before Create Logfile.");
                                             rtcode = AscrmLog.CreateLogFile();
												 if( 1 == rtcode ){
													//System.out.println("[Mega Designs Pvt. Ltd. SmartCard] Logfile created.");
												 }else if( 2 == rtcode ){
													//System.out.println("[Mega Designs Pvt. Ltd. SmartCard] Logfile already created.");
												 }else if( -1 == rtcode ){
													System.out.println("[Mega Designs Pvt. Ltd. SmartCard] Logfile already created got exception.");
												 }
												 //System.out.println("[Mega Designs Pvt. Ltd. SmartCard ] After Create Logfile.");
											 }//if( true == AscrmLog.IsAlreadyCreatedLogFile() )
											 else
											 {
												   //System.out.println("[Mega Designs Pvt. Ltd. SmartCard] File is already created.");
											 }
                                     
                                 }//if( 1==loadSOFromJar() )
                                 else
                                 {
                                     System.out.println("[Mega Designs Pvt. Ltd. SmartCard] load so failed.");
                                 } //else end


		      //}//catch(UnsatisfiedLinkError excp)


	      }//static block end here
	    
          //constructor with args
          public SmartCard(int SmartCardDeviceType) throws IllegalArgumentException,IOException {
                  
                SmartCardConnectFlag=false;
                //System.out.println("\n\n[Mega Designs Pvt. Ltd. SmartCard()] Constructor Start.");
                g_SmartCardDeviceid = g_SmartCardDeviceid+1;
                int SmartCardDeviceid = g_SmartCardDeviceid;
                //System.out.println("\n\n[Mega Designs Pvt. Ltd. SmartCard()] Device ID: "+ SmartCardDeviceid);
                //Setup Logfilepath,Logfilemode,device id
                if(SmartCardDeviceid < 0){
                   throw new IllegalArgumentException("[Mega Designs Pvt. Ltd. SmartCard()] Must be postive smart card device id");
                }//if end
                this.dllpath =null;
                if( 1 != this.ObjectloadSOFromJar(SmartCardDeviceid)){
                   throw new IOException("[Mega Designs Pvt. Ltd. SmartCard()] libMegaAscrmSmartCardAPI.so loading failed");
                }else{
                    //System.out.println("[Mega Designs Pvt. Ltd. SmartCard()] libMegaAscrmSmartCardAPI.so loading success");
                    //System.out.println("[Mega Designs Pvt. Ltd. SmartCard()] dll path: "+this.dllpath);
                }//else end
                this.DeviceId=null;
                this.LogFilePath=null;
                this.LogFileMode=-1;
                this.Logdllpath=null;
                //Setup Logfilepath,Logfilemode,device id
                //1.Setup Deviceid
                String DeviceidStr=String.format("SCA%d",SmartCardDeviceid );
                //2.Setup Logfilepath
                String LogfileNameWithPath = AscrmLog.GetLogFileNameWithPath();
                //3.Setup Logfilemode
                int LogLevelMode=-1;
                int mode = Common.GetLoggingLevelV2();
                if( mode>=0 ){
                    LogLevelMode= mode;
                }else{
                    LogLevelMode = Common.ALL;
                }//else end
                String LogdllPath =AscrmLog.GetLogDllPath();
                this.DeviceId=DeviceidStr;
                this.LogFilePath=LogfileNameWithPath;
                this.LogFileMode=LogLevelMode;
                this.Logdllpath=LogdllPath;
                //System.out.println("[Mega Designs Pvt. Ltd. SmartCard()] DeviceID: "+ this.DeviceId );
                //Setup common logpath
                JniSetupSmartCardLogFile( this.dllpath,LogfileNameWithPath,LogLevelMode,DeviceidStr,LogdllPath);
                //++System.out.println("\n\n[Mega Designs Pvt. Ltd. SmartCard()] Constructor Exit.\n\n");

         }//public SmartCard()

          //default destructor
          protected void finalize() throws Throwable {
			    this.DeviceId=null;
                this.LogFilePath=null;
                this.LogFileMode=-1;
                this.Logdllpath=null;
         }//protected void finalize() throws Throwable end

          public synchronized String GetDeviceId(){
              return this.DeviceId;
          }

          public synchronized String GetLogFilePath(){
              return this.LogFilePath;
          }

          public synchronized int GetLogFileMode(){
              return this.LogFileMode;
          }

          public synchronized String GetLogdllpath(){
              return this.Logdllpath;
          }

          public synchronized void WriteLog(String logmessage) {
                  //this.WriteNativeLog(logmessage);
			      return;
          }//public synchronized void WriteLog() end

          public synchronized static String GetDateAndTimeStamp(){
             SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
			 Date date = new Date();
			 return ("["+dateFormat.format(date)+"]"); //2013/10/15 16:16:39 
          }//public synchronized static  int GetDateAndTimeStamp() end

          //++Load libMegaAscrmSmartCardAPI.so from jar
		  private int ObjectloadSOFromJar(int DeviceID)  {

		         try
                 {

                                InputStream fis=null;
                                URL res=null;
                                File dll=null;
                                FileOutputStream fos = null;

                                //System.out.println("\n[Mega Designs Pvt. Ltd.] Before Going to delete previous libMegaAscrmSmartCardAPI.so from system temp directory.");

				
                                //System.out.println("\n[Mega Designs Pvt. Ltd.] After delete previous libMegaAscrmSmartCardAPI.so from system temp directory.");
			    
				                /*Get DLL from JAR file */
                                //System.out.println("\n[Mega Designs Pvt. Ltd.] Before Load libMegaAscrmSmartCardAPI.so from cashapi jar.");

                                try
                                {
				       
                                       res = SmartCard.class.getResource(MEGAJNISOFILENAME);

                                }
                                catch(NullPointerException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get NullPointerException when create resource from cashapi jar.");
                                      return 0;

                                }
 
                                try
                                {
				                        //InputStream fis = res.openStream();
                                        fis = res.openStream();

                                }
                                catch(IOException e)
                                {

                                     System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get IOException when create stream from cashapi jar.");
                                     return 0;

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Load libMegaAscrmSmartCardAPI.so from cashapi jar.");

								int SoFileLength=0;

								//Get SO File Size
								SoFileLength=fis.available();
								
								//System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmSmartCardAPI.so file size = "+SoFileLength+" bytes");

								/*Define the destination file*/
								/*createTempFile(String prefix, String suffix)(for temp file name and its extension)*/
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Create temp libMegaAscrmSmartCardAPI.so ");

                                try
                                {
				        
                                        dll = File.createTempFile(String.format("libMegaAscrmSmartCardAPI_%d",DeviceID),".so");

                                }
                                catch (IllegalArgumentException e)
                                {
                                        System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get IllegalArgumentException to create temp libMegaAscrmSmartCardAPI.so ");
                                        return 0;
                                       
                                }
                                catch (IOException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get IOException to create temp libMegaAscrmSmartCardAPI.so ");
                                       return 0;
                                       

                                }
                                catch (SecurityException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get SecurityException to create temp libMegaAscrmSmartCardAPI.so ");
                                       return 0;

                                }
                                

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Create temp libMegaAscrmSmartCardAPI.so ");

				                 /* Open the destination file */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create stream from original libMegaAscrmSmartCardAPI.so ");
                                try
                                {
				                       //FileOutputStream fos = new FileOutputStream(dll);
                                       fos = new FileOutputStream(dll);

                                }
                                catch(FileNotFoundException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get FileNotFoundException to create stream from cashapi.jar. ");
                                       return 0;

                                } 
                                catch( SecurityException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException to create stream from cashapi.jar. ");
                                       return 0;

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create stream from original libMegaAscrmSmartCardAPI.so ");

				/* Copy the DLL from the JAR to the filesystem */
				byte[] array = new byte[SoFileLength];

			       /*Reads some number of bytes from the input stream and 
				 stores them into the buffer array.This method blocks 
				 until input data is available, end of file is detected
				 Returns:the total number of bytes read into the buffer, or -1 
				 is there is no more data because the end of the stream has been reached.
			       */

                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create copy original libMegaAscrmSmartCardAPI.so to temp libMegaAscrmSmartCardAPI.so .");
								for(int i=fis.read(array);i!=-1;i=fis.read(array)) 
								{
		                            try
		                            {
										fos.write(array,0,i);
		                            }
		                            catch(IOException e)
		                            {
		                                  System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get IOException when write stream libMegaAscrmSmartCardAPI.so to temp libMegaAscrmSmartCardAPI.so .");
		                                  return 0;

		                            }

								}

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create copy original libMegaAscrmSmartCardAPI.so to temp libMegaAscrmSmartCardAPI.so .");
				/* Close all streams */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Going to close all input output stream .");
                                try
                                {
				      fos.close();
                                }
                                catch(IOException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] Get IOException close all output stream .");
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
                                //System.out.println(String.format("[Mega Designs Pvt. Ltd. ObjectloadSOFromJar()] libMegaAscrmSmartCardAPI.so load success For Device ID: %d.",DeviceID));

                                this.dllpath = dll.getAbsolutePath();

                                return 1;

		    }
		    catch(Throwable e)
		    {
                        
                        System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmSmartCardAPI.so load failed! .");
		        //e.printStackTrace();
                        this.dllpath =null;
                        return 0; 
		    }
	       

	 }//loadSOFromJar() end here
         
          //++Load libMegaAscrmSmartCardGateWayAPI.so from jar
		  private static int loadSOFromJar()  {

		        try
                 {

                                InputStream fis=null;
                                URL res=null;
                                File dll=null;
                                FileOutputStream fos = null;

                                //System.out.println("\n[Mega Designs Pvt. Ltd.] Before Going to delete previous libMegaAscrmSmartCardAPI.so from system temp directory.");

								//Step 1: Delete old dll file from /tmp folder
								Multipliefiledelete("/tmp","libMegaAscrmSmartCardAPIGateWay");

                                //System.out.println("\n[Mega Designs Pvt. Ltd.] After delete previous libMegaAscrmSmartCardAPI.so from system temp directory.");
			    
				                /*Get DLL from JAR file */
                                //System.out.println("\n[Mega Designs Pvt. Ltd.] Before Load libMegaAscrmSmartCardAPI.so from cashapi jar.");

                                try
                                {
				       
                                       res = SmartCard.class.getResource(MEGAGATEWAYJNISOFILENAME);

                                }
                                catch(NullPointerException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar() ] Get NullPointerException when create resource from cashapi jar.");
                                      return 0;

                                }
 
                                try
                                {
				                        //InputStream fis = res.openStream();
                                        fis = res.openStream();

                                }
                                catch(IOException e)
                                {

                                     System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get IOException when create stream from cashapi jar.");
                                     return 0;

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Load libMegaAscrmSmartCardAPI.so from cashapi jar.");

								int SoFileLength=0;

								//Get SO File Size
                                SoFileLength=fis.available();
							   //System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmSmartCardAPI.so file size = "+SoFileLength+" bytes");

								/*Define the destination file*/
								/*createTempFile(String prefix, String suffix)(for temp file name and its extension)*/
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Create temp libMegaAscrmSmartCardAPI.so ");

                                try
                                {
				        
                                        dll = File.createTempFile("libMegaAscrmSmartCardAPIGateWay",".so");

                                }
                                catch (IllegalArgumentException e)
                                {
                                        System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get IllegalArgumentException to create temp libMegaAscrmSmartCardAPIGateWay.so ");
                                        return 0;
                                       
                                }
                                catch (IOException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get IOException to create temp libMegaAscrmSmartCardAPIGateWay.so ");
                                       return 0;
                                       

                                }
                                catch (SecurityException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get SecurityException to create temp libMegaAscrmSmartCardAPIGateWay.so ");
                                       return 0;

                                }
                                

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Create temp libMegaAscrmSmartCardAPI.so ");

				                /* Open the destination file */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create stream from original libMegaAscrmSmartCardAPI.so ");
                                try
                                {
				                       //FileOutputStream fos = new FileOutputStream(dll);
                                       fos = new FileOutputStream(dll);

                                }
                                catch(FileNotFoundException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get FileNotFoundException to create stream from cashapi.jar. ");
                                       return 0;

                                } 
                                catch( SecurityException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get SecurityException to create stream from cashapi.jar. ");
                                       return 0;

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create stream from original libMegaAscrmSmartCardAPI.so ");

								/* Copy the DLL from the JAR to the filesystem */
								byte[] array = new byte[SoFileLength];

								   /*Reads some number of bytes from the input stream and 
								 stores them into the buffer array.This method blocks 
								 until input data is available, end of file is detected
								 Returns:the total number of bytes read into the buffer, or -1 
								 is there is no more data because the end of the stream has been reached.
								   */

                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create copy original libMegaAscrmSmartCardAPI.so to temp libMegaAscrmSmartCardAPI.so .");
								for(int i=fis.read(array);i!=-1;i=fis.read(array)) 
								{
		                            try
		                            {
										fos.write(array,0,i);
		                            }
		                            catch(IOException e)
		                            {
		                                  System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get IOException when write stream libMegaAscrmSmartCardAPIGateWay.so to temp libMegaAscrmSmartCardAPIGateWay.so .");
		                                  return 0;

		                            }

				                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create copy original libMegaAscrmSmartCardAPI.so to temp libMegaAscrmSmartCardAPI.so .");
				                /* Close all streams */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Going to close all input output stream .");
                                try
                                {
				                   fos.close();
                                }
                                catch(IOException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get IOException close all output stream .");
                                      return 0;

                                }
                                try
                                {
				                    fis.close();

                                }
                                catch(IOException e)
                                {
                                      
                                      System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get IOException close all input stream .");
                                      return 0;


                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After close all input output stream .");

								/* Load the DLL from the filesystem */
								//getAbsolutePath to find the location of temporary file.
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before going to load temp libMegaAscrmSmartCardAPI.so from system temp directory.");
                                try
                                {
										System.load(dll.getAbsolutePath());
                                       
                                }
                                catch(SecurityException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get SecurityException load temp libMegaAscrmSmartCardAPIGateWay.so from system temp directory.");
                                      return 0;
                                }
                                catch(UnsatisfiedLinkError e) 
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get UnsatisfiedLinkError load temp libMegaAscrmSmartCardAPIGateWay.so from system temp directory.");
                                      e.printStackTrace();
                                      return 0;
    
                                }
                                catch(NullPointerException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] Get NullPointerException load temp libMegaAscrmSmartCardAPIGateWay.so from system temp directory.");
                                      return 0;
    

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After load temp libMegaAscrmSmartCardAPI.so from system temp directory.");
                                //System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] libMegaAscrmSmartCardAPIGateWay.so load success");

                                return 1;

				}
				catch(Throwable e)
				{
							
							System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] libMegaAscrmSmartCardAPIGateWay.so load failed! .");
							//e.printStackTrace();

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


}//public final class SmartCard end here
