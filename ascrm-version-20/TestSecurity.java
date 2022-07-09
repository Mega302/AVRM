        	
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

    public class TestSecurity{

       public static void main(String args[]){
             //++TestSecurityDevice();
             Common CommonObj = new Common();
			 CommonObj.SetLoggingLevel(47);
             Security SecurityObj = null;
             try{
				SecurityObj = new Security();
             }catch(IllegalArgumentException ex){
				System.out.println("[TestSecurityDevice()] Security object creation thrown IllegalArgumentException");
				return;
			 }
             System.out.println("[TestSecurityDevice()] Main Door Return Code: "+SecurityObj.GetDoorStatus(0) );
             System.out.println("[TestSecurityDevice()] Cash Door Return Code: "+SecurityObj.GetDoorStatus(1) );
       }//public static void main(String args[]) end

       public static void TestSecurityDevice(){

             int PortId=1,ChannelClearanceMode=1,Timeout=100;
             
             Security SecurityObj = null;
             //++Common.Test();
			 try{
				SecurityObj = new Security();
             }catch(IllegalArgumentException ex){
				System.out.println("[TestSecurityDevice()] Security object creation thrown IllegalArgumentException");
				return;
			 }
			 
			 System.out.println("[TestSecurityDevice()] Security object creation success");
             int rtcode =0;
             rtcode = SecurityObj.ConnectDevice( PortId,0 );
             System.out.println("[TestSecurityDevice()] Security Connect Return Code: "+rtcode );
             
             //++2 Cash Vault
             //++System.out.println("[TestSecurityDevice()] Security DisableAlarm Return Code: "+SecurityObj.DisableAlarm(0,0,2,10,(byte)0b10100001) );
             //++System.out.println("[TestSecurityDevice()] Security DisableAlarm Return Code: "+SecurityObj.DisableAlarm(2,10) );
             
             //++1 Main Door 
             //++System.out.println("[TestSecurityDevice()] Security DisableAlarm Return Code: "+SecurityObj.DisableAlarm(1,10,0,0,(byte)0b01100001) );
             //++System.out.println("[TestSecurityDevice()] Security DisableAlarm Return Code: "+SecurityObj.DisableAlarm(1,10) );
             
             //++0 both Door 
             System.out.println("[TestSecurityDevice()] Security DisableAlarm Return Code: "+SecurityObj.DisableAlarm(0,10) );
             
             //System.out.println("[TestSecurityDevice()] Security GetDoorStatus  Top Door Return Code: "+SecurityObj.GetDoorStatus(0) );
             //System.out.println("[TestSecurityDevice()] Security GetDoorStatus  Top Door Return Code: "+SecurityObj.GetSecurityVersion(null) );
             //System.out.println("[TestSecurityDevice()] Security GetDoorStatus  Cash Vault Return Code: "+SecurityObj.GetDoorStatus(1) );
             //System.out.println("[TestSecurityDevice()] Security GetDoorStatus  Bottom Door Return Code: "+SecurityObj.GetDoorStatus(2) );
             //System.out.println("[TestSecurityDevice()] Security GetDoorStatus  Alarm Status Return Code: "+SecurityObj.GetDoorStatus(3) );
             System.out.println("[TestSecurityDevice()] Security DeActivateSecurityPort Return Code: "+SecurityObj.DisConnectDevice(0) );
             
       }//public static void TestSecurityDevice() end

	}//public class TestSecurity end
