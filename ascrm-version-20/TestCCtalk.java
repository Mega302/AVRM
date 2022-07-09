        	
    import Cris.*;
    import java.util.Scanner;
    
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

    public class TestCCtalk{

       public static void main(String args[]){
             TestTestCCtalkDevice();
       }//public static void main(String args[]) end

       public static void TestTestCCtalkDevice(){

             int Timeout=0;
             int rtcode =0;
             int DeviceType = 2; //++Coin Acceptor
             int EscrowClearanceMode = 0; //++Retain Note and Coins
             int PortId2 = 1 ;//++Coin Acceptor Port
             
             Currency CurrencyObj = null;
             try{
				CurrencyObj = new Currency();
             }catch(IllegalArgumentException ex){
				System.out.println("[TestTestCCtalkDevice()] Currency object creation thrown IllegalArgumentException");
				return;
			 }//++catch end
			 
			 Security SecurityObj = null;
             try{
				SecurityObj = new Security();
             }catch(IllegalArgumentException ex){
				System.out.println("[TestTestCCtalkDevice()] Security object creation thrown IllegalArgumentException");
				return;
			 }//++catch end
			 
			 TokenDispenser TokenDispenserObj = null;
             TokenDispenserObj = new TokenDispenser();
             
             System.out.println("\n\n");
             
             //++Security Device
             rtcode =0;
             rtcode = SecurityObj.ConnectDevice( PortId2,0 );
             System.out.println("[TestTestCCtalkDevice()] Security Connect Return Code: "+rtcode );
             //++System.out.println("[TestSecurityDevice()] Security GetDoorStatus  Top Door Return Code: "+SecurityObj.GetDoorStatus(0) );
             
             //++Currency Device
             rtcode = CurrencyObj.ConnectDevice( 0,PortId2,50,DeviceType,EscrowClearanceMode, 0 );
             System.out.println("[TestTestCCtalkDevice()] Currency Connect Return Code: "+rtcode );
             
             System.out.println("[TestTestCCtalkDevice()] Security DeActivateSecurityPort Return Code: "+ SecurityObj.DisConnectDevice(0) );
             
             
             //++TokenDispenser Device
             rtcode =0;
             int PortId=0;
             //++Channel Clearance Mode: 0-Retain in the channel, 1-Send to rejection bin, 2-Send to dispensing outlet of the device.
             int ChannelClearanceMode=0;
             rtcode = TokenDispenserObj.ConnectDevice( PortId2, ChannelClearanceMode, 1000);
             System.out.println("[TestTestCCtalkDevice()] TokenDispenser Connect Return Code: "+rtcode );
             System.out.println("[TestTestCCtalkDevice()] TokenDispenser DeActivateSecurityPort Return Code: "+TokenDispenserObj.DisConnectDevice(1000) );
             
             
             //System.out.println("\n\n");
             System.out.println("[TestTestCCtalkDevice()] Currency DeActivatePort Return Code: "+ CurrencyObj.DisConnectDevice(DeviceType,0) );
             System.out.println("[TestTestCCtalkDevice()] SecurityObj DeActivateSecurityPort Return Code: "+ SecurityObj.DisConnectDevice(0) );
             
             
       }//public static void TestSecurityDevice() end

	}//public class TestCCtalk end
