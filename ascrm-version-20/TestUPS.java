        	
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

    public class TestUPS{

       public static void main(String args[]){
             TestUPSDevice();
       }//public static void main(String args[]) end

       public static void TestUPSDevice(){

             UPS UPSObj = null;
             TokenDispenser TokenDispenser_obj = new TokenDispenser((byte)0x03);
			 Currency Currency_obj = new Currency(1, 1);
			 try{
			 	SmartCard SmartCard_obj1 = new SmartCard(1);
			 	SmartCard SmartCard_obj2 = new SmartCard(2);
			 }catch(Exception e){
			 	
			 }//catch end
			 Security Security_obj = new Security(1);
             UPSObj = new UPS();
             System.out.println("[TestSecurityDevice()] Security GetUPSStatus Return Code: "+UPSObj.GetUPSStatus() );
             System.out.println("[TestSecurityDevice()] Security GetBatteryStatus Return Code: "+UPSObj.GetBatteryStatus() );
             return;
             
       }//public static void TestSecurityDevice() end

	}//public class TestUPS end
