
package Cris;

//Other Package
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



public class SmartCard
{


         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 private static final String MEGAGATEWAYJNISOFILENAME ="libMegaAscrmSmartCardAPIGateWay.so";   

         private static final String MEGAJNISOFILENAME ="libMegaAscrmSmartCardAPI.so";
   
         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //private static HashMap<String, String> deviceid;

         //private static int DEVICE_NUMBER=10;

         private String dllpath;

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         
         /* 

         public native int ConnectDevice(int PortId,int ChannelClearanceMode, int Timeout);
         public native byte[] GetDeviceStatus(int ComponentId,int Timeout);
	 public native int AcceptCard(int Timeout);
	 public native int CollectCard(int Timeout);
	 public native int DisableCardAcceptance(int Timeout);
	 public native int DisConnectDevice(int Timeout);
	 public native int DispenseCard(int Timeout);
	 public native int EnableCardAcceptance(int Timeout);
	 public native int IsCardInChannel(int Timeout);
	 public native int IsCardRemoved(int Timeout);
	 public native int RejectCard(int Timeout);
	 public native int ReturnCard(int DispenseMode,int Timeout);         
         public native int SAMSlotPowerOnOff(int SAMSlotId,int PowerOnOffState);
	 public native byte[] ResetSAM(int SAMSlotId,int ResetType,int Timeout);
	 public native byte[] ActivateCard(int CardTechType,int SAMSlotId,int Timeout);
	 public native int DeactivateCard(int CardTechType,int SAMSlotId,int Timeout);
         public native byte[] XChangeAPDU(int CardTechType,byte[] CommandAPDU,int SAMSlotId,int Timeout);
         */

         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 
         //GateWay API List

         private  native  boolean JniSetupSmartCardLogFile( String dllpath,
                                                            String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath
                                                          );


         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         

         private synchronized native  int JniGetLastError(    String dllpath,
                                                              String Logfile,
                                                              int LogLevel,
                                                              String DeviceId,
                                                              String LogdllPath);

         private synchronized native  int GetCardBlockStatus( String dllpath,
                                                              String Logfile,
                                                              int LogLevel,
                                                              String DeviceId,
                                                              String LogdllPath);

         private synchronized native  int CardBlockStatusRequest( String dllpath,
                                                                  String Logfile,
                                                                  int LogLevel,
                                                                  String DeviceId,
                                                                  String LogdllPath);

         private synchronized native  int MutekInitWithCardRetain( String dllpath,
                                                                   String Logfile,
                                                                   int LogLevel,
                                                                   String DeviceId,
                                                                   String LogdllPath);

         private synchronized native  int MutekInitWithCardMoveInFront( String dllpath,
                                                                        String Logfile,
                                                                        int LogLevel,
                                                                        String DeviceId,
                                                                        String LogdllPath);

         private synchronized native  int MutekInitWithErrorCardBin( String dllpath,
                                                                     String Logfile,
                                                                     int LogLevel,
                                                                     String DeviceId,
                                                                     String LogdllPath);

         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         private  native int JniConnectDevice( String dllpath,
                                               String Logfile,
                                               int LogLevel,
                                               String DeviceId,
                                               String LogdllPath,
                                               int PortId,
                                               int ChannelClearanceMode, 
                                               int Timeout
                                             );

         private  native int JniDisConnectDevice( String dllpath, String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);
  
         private  native byte[] JniGetDeviceStatus(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int ComponentId,int Timeout);

	 private  native int JniAcceptCard(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);

	 private  native int JniCollectCard(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);

	 private  native int JniDisableCardAcceptance(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);
	 
	 private  native int JniDispenseCard(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);

	 private  native int JniEnableCardAcceptance(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);

	 private  native int JniIsCardInChannel(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);

	 private  native int JniIsCardRemoved(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);

	 private  native int JniRejectCard(String dllpath,String Logfile,
                                                            int LogLevel,
                                                            String DeviceId,
                                                            String LogdllPath,int Timeout);

	 private  native int JniReturnCard( String dllpath,
                                            String Logfile,
                                            int LogLevel,
                                            String DeviceId,
                                            String LogdllPath,
                                            int DispenseMode,int Timeout); 
        
         private  native int JniSAMSlotPowerOnOff( String dllpath,
                                                   String Logfile,
                                                   int LogLevel,
                                                   String DeviceId,
                                                   String LogdllPath,
                                                   int SAMSlotId,
                                                   int PowerOnOffState
                                                 );

	 private  native byte[] JniResetSAM( String dllpath,
                                             String Logfile,
                                             int LogLevel,
                                             String DeviceId,
                                             String LogdllPath,
                                             int SAMSlotId,
                                             int ResetType,
                                             int Timeout
                                           );

	 private  native byte[] JniActivateCard( String dllpath,
                                                 String Logfile,
                                                 int LogLevel,
                                                 String DeviceId,
                                                 String LogdllPath,
                                                 int CardTechType,
                                                 int SAMSlotId,
                                                 int Timeout
                                               );

	 private  native int JniDeactivateCard( String dllpath,
                                                String Logfile,
                                                int LogLevel,
                                                String DeviceId,
                                                String LogdllPath,
                                                int CardTechType,
                                                int SAMSlotId,
                                                int Timeout
                                              );

         private  native byte[] JniXChangeAPDU( String dllpath,
                                                String Logfile,
                                                int LogLevel,
                                                String DeviceId,
                                                String LogdllPath,
                                                int CardTechType,
                                                byte[] CommandAPDU,
                                                int SAMSlotId
                                               ,int Timeout
                                             );
                                             
                                             
          private  native  int JniisSAMEnable(   String dllpath,
                                                 String Logfile,
                                                 int LogLevel,
                                                 String DeviceId,
                                                 String LogdllPath
                                               );

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         
         private void ReturnBlockCard()
         {

                   if( 1 == CardBlockStatusRequest( this.dllpath,
                                                    this.LogFilePath,
                                                    this.LogFileMode,
                                                    this.DeviceId,
                                                    this.Logdllpath
                                                  ))
                   {
                               MutekInitWithCardMoveInFront( this.dllpath,
                                                             this.LogFilePath,
                                                             this.LogFileMode,
                                                             this.DeviceId,
                                                             this.Logdllpath);

                   }

                   return;

         }//private void ReturnBlockCard() end      

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         public synchronized int GetSmartCardLastError()
         {
                  
                 return( JniGetLastError(     this.dllpath,
		                              this.LogFilePath,
		                              this.LogFileMode,
		                              this.DeviceId,
		                              this.Logdllpath) );                



         }//public  synchronized int GetLastError() end

        
         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //Smartcard API List

         public synchronized int ConnectDevice(int PortId,int ChannelClearanceMode, int Timeout)
         {

                //System.out.println("\n[Mega Designs Pvt. Ltd. ConnectDevice()] Current instance dll path: "+this.dllpath);

                int rtcode =0;

                rtcode = JniConnectDevice( this.dllpath,
                                           this.LogFilePath,
                                           this.LogFileMode,
                                           this.DeviceId,
                                           this.Logdllpath,
                                           PortId,
                                           ChannelClearanceMode, 
                                           Timeout);
                       

               if( 2 == rtcode ) //Nak or EOT
               {
 
                   return( JniConnectDevice( this.dllpath,
                                             this.LogFilePath,
                                             this.LogFileMode,
                                             this.DeviceId,
                                             this.Logdllpath,
                                             PortId,
                                             ChannelClearanceMode, 
                                             Timeout) );


               }
               else
               {

                    return rtcode;

               }



                

         }//public int ConnectDevice(int PortId,int ChannelClearanceMode, int Timeout) end

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         public synchronized byte[] GetDeviceStatus(int ComponentId,int Timeout)
         {

                   
                    return(JniGetDeviceStatus(this.dllpath,
                                              this.LogFilePath,
                                              this.LogFileMode,
                                              this.DeviceId,
                                              this.Logdllpath,
                                              ComponentId,      
                                              Timeout));

                    

        }//public synchronized byte[] GetDeviceStatus(int ComponentId,int Timeout) end

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 public synchronized int AcceptCard(int Timeout)
         {

             return(JniAcceptCard( this.dllpath,
                                   this.LogFilePath,
                                   this.LogFileMode,
                                   this.DeviceId,
                                   this.Logdllpath,Timeout));

             
        }//public synchronized int AcceptCard(int Timeout) end

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         public synchronized int DisableCardAcceptance(int Timeout)
         {

             return(JniDisableCardAcceptance( this.dllpath,
                                              this.LogFilePath,
                                              this.LogFileMode,
                                              this.DeviceId,
                                              this.Logdllpath,Timeout));

         }//public synchronized int DisableCardAcceptance(int Timeout)end

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 public synchronized int DisConnectDevice(int Timeout)
         {

               return(JniDisConnectDevice( this.dllpath,
                                           this.LogFilePath,
                                           this.LogFileMode,
                                           this.DeviceId,
                                           this.Logdllpath,Timeout));

         }//public synchronized int DisConnectDevice(int Timeout) end

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         public synchronized int EnableCardAcceptance(int Timeout)
         {

               return(JniEnableCardAcceptance( this.dllpath,
                                               this.LogFilePath,
                                               this.LogFileMode,
                                               this.DeviceId,
                                               this.Logdllpath,Timeout));

         }//public synchronized int EnableCardAcceptance(int Timeout) end

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
	 public synchronized int IsCardInChannel(int Timeout)
         {

              return(JniIsCardInChannel(   this.dllpath,
                                           this.LogFilePath,
                                           this.LogFileMode,
                                           this.DeviceId,
                                           this.Logdllpath,Timeout));

         }//public synchronized int IsCardInChannel(int Timeout) end

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 public synchronized int IsCardRemoved(int Timeout)
         {

              return(JniIsCardRemoved(     this.dllpath,
                                           this.LogFilePath,
                                           this.LogFileMode,
                                           this.DeviceId,
                                           this.Logdllpath,Timeout));

         }//public synchronized int IsCardRemoved(int Timeout) end

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


         public synchronized int DispenseCard(int Timeout)
         {

                   
              /*

                      if( 1 == CardBlockStatusRequest( this.dllpath,
                                                    this.LogFilePath,
                                                    this.LogFileMode,
                                                    this.DeviceId,
                                                    this.Logdllpath
                                                  ))
                   {
                               
                               //Command : 0x30 0x30
                               if( 1 == MutekInitWithCardMoveInFront( this.dllpath,
		                                                      this.LogFilePath,
		                                                      this.LogFileMode,
		                                                      this.DeviceId,
		                                                      this.Logdllpath) )
                               {
                                   
                                       return 2; //Operation successfull card return successfull so return chanel blocked

                               }
                               else
                               {
                                       return 3; //Operation not successfull so Insertion/return mouth blocked

                               }

                   }
                   else
                   {
                      
                      return(JniDispenseCard( this.dllpath,
		                              this.LogFilePath,
		                              this.LogFileMode,
		                              this.DeviceId,
		                              this.Logdllpath,Timeout));
                  }       

                  */
                  ///////////////////////////////////////////////////////////////////////////////////////////////////

                  //C layer: DispenseCard_c 
                  return(JniDispenseCard(     this.dllpath,
		                              this.LogFilePath,
		                              this.LogFileMode,
		                              this.DeviceId,
		                              this.Logdllpath,Timeout));

            

         }//public synchronized int DispenseCard(int Timeout) end
 
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         public synchronized int CollectCard(int Timeout)
         {  
                   
                  /*
                
                   if( 1 == CardBlockStatusRequest( this.dllpath,
                                                    this.LogFilePath,
                                                    this.LogFileMode,
                                                    this.DeviceId,
                                                    this.Logdllpath
                                                  ))
                   {
                               

                               if( 1 == MutekInitWithCardMoveInFront( this.dllpath,
		                                                      this.LogFilePath,
		                                                      this.LogFileMode,
		                                                      this.DeviceId,
		                                                      this.Logdllpath) )
                               {
                                   
                                        return 0; //Operation successfull card return successfull

                               }
                               else
                               {
                                       return 2; //return mouth blocked

                               }

                   }
                   else
                   {
			     return(JniCollectCard( this.dllpath,
				                    this.LogFilePath,
				                    this.LogFileMode,
				                    this.DeviceId,
				                    this.Logdllpath,Timeout));

                    }

                    */

                    ////////////////////////////////////////////////////////////////////////////////////////////
     
                    //C Layer: CollectCard_c 

                     return(JniCollectCard( this.dllpath,
				                    this.LogFilePath,
				                    this.LogFileMode,
				                    this.DeviceId,
				                    this.Logdllpath,Timeout));

               
         }//public synchronized int CollectCard(int Timeout) end

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 public synchronized int RejectCard(int Timeout)
         {
                     /*

                     if( 1 == CardBlockStatusRequest( this.dllpath,
                                                      this.LogFilePath,
                                                      this.LogFileMode,
                                                      this.DeviceId,
                                                      this.Logdllpath
                                                    ))
                   {
                               
                               // init : 0x30 0x31
                               if( 1 == MutekInitWithErrorCardBin( this.dllpath,
		                                                   this.LogFilePath,
		                                                   this.LogFileMode,
		                                                   this.DeviceId,
		                                                   this.Logdllpath) )
                               {
                                   
                                       return 0; //Operation successfull card return to collection bin successfully

                               }
                               else
                               {
                                       return 5; //Operation not successfull so return operation timeout happened

                               }

                   }
                   else
                   {
			     return(JniRejectCard( this.dllpath,
				                   this.LogFilePath,
				                   this.LogFileMode,
				                   this.DeviceId,
				                   this.Logdllpath,Timeout));

                  }

                  */

                  //////////////////////////////////////////////////////////////////////////////////////////////////////
                   //C layer: RejectCard_c
                   return(JniRejectCard(  this.dllpath,
				          this.LogFilePath,
				          this.LogFileMode,
				          this.DeviceId,
				          this.Logdllpath,Timeout));

                 
         }//public synchronized int RejectCard(int Timeout) end

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

	 public synchronized int ReturnCard(int DispenseMode,
                                            int Timeout)
         {

                   /*
                   if( 1 == CardBlockStatusRequest( this.dllpath,
                                                    this.LogFilePath,
                                                    this.LogFileMode,
                                                    this.DeviceId,
                                                    this.Logdllpath
                                                  ))
                   {
                               
                               //Init : 0x30 0x30
                               if( 1 == MutekInitWithCardMoveInFront( this.dllpath,
		                                                      this.LogFilePath,
		                                                      this.LogFileMode,
		                                                      this.DeviceId,
		                                                      this.Logdllpath) )
                               {
                                   
                                        return 0; //Operation successfull card return successfull

                               }
                               else
                               {
                                       return 2; //return mouth blocked

                               }

                   }
                   else
                   {
		        
                         return(JniReturnCard( this.dllpath,
		                               this.LogFilePath,
		                               this.LogFileMode,
		                               this.DeviceId,
		                               this.Logdllpath,
                                               DispenseMode,
                                               Timeout));
                  }          
 
                  */
                  ///////////////////////////////////////////////////////////////////////////////////////////////////

                   //C layer: ReturnCard_c
 
                   return(JniReturnCard(       this.dllpath,
		                               this.LogFilePath,
		                               this.LogFileMode,
		                               this.DeviceId,
		                               this.Logdllpath,
                                               DispenseMode,
                                               Timeout));
 
            
         }//public synchronized int ReturnCard(int DispenseMode,int Timeout)end


         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         
         public synchronized int SAMSlotPowerOnOff(int SAMSlotId,int PowerOnOffState)
         {

             return(JniSAMSlotPowerOnOff( this.dllpath,
                                           this.LogFilePath,
                                           this.LogFileMode,
                                           this.DeviceId,
                                           this.Logdllpath,SAMSlotId,PowerOnOffState));

         }//public synchronized int SAMSlotPowerOnOff(int SAMSlotId,int PowerOnOffState) end

         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 public synchronized byte[] ResetSAM(int SAMSlotId,int ResetType,int Timeout)
         {

              return(JniResetSAM(  this.dllpath,
                                   this.LogFilePath,
                                   this.LogFileMode,
                                   this.DeviceId,
                                   this.Logdllpath,
                                   SAMSlotId,
                                   ResetType,
                                   Timeout));


            
         }// public synchronized byte[] ResetSAM(int SAMSlotId,int ResetType,int Timeout) end

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 public synchronized byte[] ActivateCard(int CardTechType,int SAMSlotId,int Timeout)
         {

              return(JniActivateCard( this.dllpath,
                                      this.LogFilePath,
                                      this.LogFileMode,
                                      this.DeviceId,
                                      this.Logdllpath,
                                      CardTechType,
                                      SAMSlotId,
                                      Timeout));

         }//public synchronized byte[] ActivateCard(int CardTechType,int SAMSlotId,int Timeout) end

         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 public synchronized int DeactivateCard(int CardTechType,int SAMSlotId,int Timeout)
         {

                  return(JniDeactivateCard(this.dllpath,
                                           this.LogFilePath,
                                           this.LogFileMode,
                                           this.DeviceId,
                                           this.Logdllpath,
                                           CardTechType,SAMSlotId,Timeout));

         }//public synchronized int DeactivateCard(int CardTechType,int SAMSlotId,int Timeout) end

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         public synchronized byte[] XChangeAPDU(int CardTechType, 
                                                byte[] CommandAPDU,
                                                int SAMSlotId,
                                                int Timeout 
                                               )
         {

                

                return(JniXChangeAPDU( this.dllpath,
                                       this.LogFilePath,
                                       this.LogFileMode,
                                       this.DeviceId,
                                       this.Logdllpath,
                                       CardTechType,CommandAPDU,SAMSlotId,Timeout));

 
         }//public synchronized byte[] XChangeAPDU end

         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         public synchronized int InitMutek(int Choice)
         {


          
       
                      switch(Choice)
                      {

		                  case 0:
		                         //Keep Card
		                         MutekInitWithCardRetain( this.dllpath,
						                  this.LogFilePath,
						                  this.LogFileMode,
						                  this.DeviceId,
						                  this.Logdllpath);

		                         return 0;

		                  case 1:  
		                           //Collect Card
		                           MutekInitWithErrorCardBin( this.dllpath,
		                                                      this.LogFilePath,
		                                                      this.LogFileMode,
		                                                      this.DeviceId,
		                                                      this.Logdllpath
		                                                    );
		                         
		                         return 1;
		                  case 2:
		                           //Return Card
		                           MutekInitWithCardMoveInFront( this.dllpath,
								         this.LogFilePath,
								         this.LogFileMode,
								         this.DeviceId,
								         this.Logdllpath);
		                         return 2;

		                  default:
		                          
		                          break;
                      }

                      return 0;

		      

         }//public synchronized int DispenseCard(int Timeout) end
         
         
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         
         public synchronized int isSAMEnable()
         {
                                           
                                           
                                           return (  JniisSAMEnable(  this.dllpath,
		                                                      this.LogFilePath,
		                                                      this.LogFileMode,
		                                                      this.DeviceId,
		                                                      this.Logdllpath
		                                                    ) );
		                         
		     
		      

         }//public synchronized int DispenseCard(int Timeout) end


         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //public  native byte[] ReadUltralightBlock(int Addr,int Timeout);

	 //public  native int WriteUltralightPage(int Addr,byte[] Data,int Timeout);

         private native void WriteNativeLog(String message);
 
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         private String DeviceId;

         private String LogFilePath;

         private int LogFileMode;

         private String Logdllpath;

         //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
         static
	 { 


                      int rtcode=0,counter=1;
                      
                      /*
		      try 
		      {

                                 System.out.println("\n[Mega Designs Pvt. Ltd.] Going to load libMegaAscrmSmartCardAPI.so from java libpath.");
 
				 System.loadLibrary("MegaAscrmSmartCardAPI");

                      }
		      catch(UnsatisfiedLinkError excp)
		      {
		      */

                                 //System.out.println("\n[Mega Designs Pvt. Ltd.] Unable to load libMegaAscrmSmartCardAPI.so from java libpath.");

                                 //System.out.println("\n[Mega Designs Pvt. Ltd.] So going to load libMegaAscrmSmartCardAPI.so from cashapi.jar file.");

                                 Multipliefiledelete("/tmp","libMegaAscrmSmartCardAPIGateWay");

                                 Multipliefiledelete("/tmp","libMegaAscrmSmartCardAPI");
				 
                                 rtcode =1;

                                 rtcode = loadSOFromJar();

                                 if( 1== rtcode)
                                 {

                                     System.out.println("[Mega Designs Pvt. Ltd.] So Load Successfull.");
                                     
                                     if( false == AscrmLog.IsAlreadyCreatedLogFile() )
                                     {
                                             //System.out.println("[Mega Designs Pvt. Ltd.] File is not created.");

		                             //System.out.println("[Mega Designs Pvt. Ltd.] Before Create Logfile.");

		                             rtcode = AscrmLog.CreateLogFile();

		                             if( 1 == rtcode )
		                             {
		                                //System.out.println("[Mega Designs Pvt. Ltd. SmartCard] Logfile created.");
		                             }
		                             else if( 2 == rtcode )
		                             {
		                                //System.out.println("[Mega Designs Pvt. Ltd. SmartCard] Logfile already created.");
		                             }
		                             else if( -1 == rtcode )
		                             {
		                                System.out.println("[Mega Designs Pvt. Ltd. SmartCard] Logfile already created got exception.");
		                             }

		                             //System.out.println("[Mega Designs Pvt. Ltd.] After Create Logfile.");

                                     }//if( true == AscrmLog.IsAlreadyCreatedLogFile() )
                                     else
                                     {
                                           //System.out.println("[Mega Designs Pvt. Ltd.] File is already created.");
                                     }
                                     
                                 }//if( 1==loadSOFromJar() )
                                 else
                                 {
                                     System.out.println("[Mega Designs Pvt. Ltd.] load so failed.");
                                 } 


		      //}//catch(UnsatisfiedLinkError excp)


	 }//static block end here
	 

         
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
         //default constructor
         private SmartCard()
         {


         }//private SmartCard() end


         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //constructor with args
         public SmartCard(int SmartCardDeviceid) throws IllegalArgumentException,IOException
         {


                //System.out.println("\n\n[Mega Designs Pvt. Ltd. SmartCard()] Constructor Start.");

                ////////////////////////////////////////////////////////////////////////////////
              
                //Setup Logfilepath,Logfilemode,device id
                if(SmartCardDeviceid < 0)
                {

                    throw new IllegalArgumentException("[Mega Designs Pvt. Ltd. SmartCard()] Must be postive smart card device id");

                }

                ////////////////////////////////////////////////////////////////////////////////

                this.dllpath =null;

                if( 1 != this.ObjectloadSOFromJar(SmartCardDeviceid))
                {
                   throw new IOException("[Mega Designs Pvt. Ltd. SmartCard()] libMegaAscrmSmartCardAPI.so loading failed");

                }
                else
                {
                    //System.out.println("[Mega Designs Pvt. Ltd. SmartCard()] libMegaAscrmSmartCardAPI.so loading success");

                    //System.out.println("[Mega Designs Pvt. Ltd. SmartCard()] dll path: "+this.dllpath);

                }

                ///////////////////////////////////////////////////////////////////////////////

                
                this.DeviceId=null;

                this.LogFilePath=null;

                this.LogFileMode=-1;

                this.Logdllpath=null;

                ///////////////////////////////////////////////////////////////////////////////
                            

               //Setup Logfilepath,Logfilemode,device id
                
               //1.Setup Deviceid

               String DeviceidStr=String.format("SCA%d",SmartCardDeviceid );
		
               ///////////////////////////////////////////////////////////////////////////////
               
               //2.Setup Logfilepath
               String LogfileNameWithPath = AscrmLog.GetLogFileNameWithPath();

               ///////////////////////////////////////////////////////////////////////////////
               
               //3.Setup Logfilemode
               int LogLevelMode=-1;
               
               String cardtype=String.format("Smart_Card_Reader_%d_log_Level",SmartCardDeviceid );

	       int mode = AscrmLog.GetIniIntValue(cardtype);
               
               if( mode>=0 )
               {
                    LogLevelMode= mode;
               }
               else
               {
                    LogLevelMode = 0;
               }

              ///////////////////////////////////////////////////////////////////////////////////
 
               String LogdllPath =AscrmLog.GetLogDllPath();
              
              ///////////////////////////////////////////////////////////////////////////////////

               this.DeviceId=DeviceidStr;

               this.LogFilePath=LogfileNameWithPath;

               this.LogFileMode=LogLevelMode;

               this.Logdllpath=LogdllPath;
 
        
              ////////////////////////////////////////////////////////////////////////////////////
             
               
               //Setup common logpath
               /*

               JniSetupSmartCardLogFile( this.dllpath,
                                         LogfileNameWithPath,
                                         LogLevelMode,
                                         DeviceidStr,
                                         LogdllPath
                                       );
                
              
              */
              //////////////////////////////////////////////////////////////////////////////////

              //System.out.println("\n\n[Mega Designs Pvt. Ltd. SmartCard()] Constructor Exit.\n\n");

         }//public SmartCard()

         //////////////////////////////////////////////////////////////////////////////////////////

         //default destructor
         protected void finalize() throws Throwable 
         {

                this.DeviceId=null;

                this.LogFilePath=null;

                this.LogFileMode=-1;

                this.Logdllpath=null;


         }//protected void finalize() throws Throwable end

         ////////////////////////////////////////////////////////////////////////////////////////////

          public synchronized String GetDeviceId()
          {
              return this.DeviceId;

          }

          public synchronized String GetLogFilePath()
          {
              return this.LogFilePath;

          }

          public synchronized int GetLogFileMode()
          {
              return this.LogFileMode;

          }

          public synchronized String GetLogdllpath()
          {
              return this.Logdllpath;

          }

          public synchronized void WriteLog(String logmessage)
	  {

                  //this.WriteNativeLog(logmessage);
                  return;

		
	  }//public synchronized void WriteLog() end


         ///////////////////////////////////////////////////////////////////////////////////////////
 
          public synchronized static String GetDateAndTimeStamp()
	  {

		 SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		 Date date = new Date();
		 return ("["+dateFormat.format(date)+"]"); //2013/10/15 16:16:39 


	  }//public synchronized static  int GetDateAndTimeStamp() end


         /////////////////////////////////////////////////////////////////////////////////////////////////

         
         //Load libMegaAscrmSmartCardAPI.so from jar
	 private int ObjectloadSOFromJar(int DeviceID) 
	 {

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


         ////////////////////////////////////////////////////////////////////////////////////////////


	 //Load libMegaAscrmSmartCardAPI.so from jar
	 private static int loadSOFromJar() 
	 {

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

                                System.out.println("[Mega Designs Pvt. Ltd.] After load temp libMegaAscrmSmartCardAPI.so from system temp directory.");
                                System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] libMegaAscrmSmartCardAPIGateWay.so load success");

                                return 1;

		    }
		    catch(Throwable e)
		    {
                        
                        System.out.println("[Mega Designs Pvt. Ltd. loadSOFromJar()] libMegaAscrmSmartCardAPIGateWay.so load failed! .");
		        //e.printStackTrace();

                        return 0;
		    }
	       

	 }//loadSOFromJar() end here


        
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	 
	 public static int Multipliefiledelete(String Directory,String StartingFileNameFilter)
	 {

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

	 
	 public static boolean delete(String fileName)
	 {


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


	 public static String[] Search(String Directory,final String FileNameFilter)
	 {
		   
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


}//class end here
