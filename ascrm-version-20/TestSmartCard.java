        	
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
	import java.util.concurrent.TimeUnit;
	import java.lang.InterruptedException;

    public class TestSmartCard{

       public static void main(String args[]){
             TestSmartCard();
       }//public static void main(String args[]) end

       public static void TestSmartCard(){

             int PortId=0,ChannelClearanceMode=0,Timeout=100;
             byte[] ActivateCard = null;
             SmartCard SmartCardObj = null;
             SmartCard SmartCardObjOther = null; 
             int rtcode = 0;
             int ComponentId=0;
             byte[] DeviceStatus = null;
                
             /*            
             try{
				SmartCardObj = new SmartCard(0);
             }catch(IOException ex){
				System.out.println("[TestSmartCard()] Smartcard object creation thrown IOException");
				return;
			 }//++ catch(IOException ex) end
			 
             int rtcode = SmartCardObj.ConnectDevice(PortId,ChannelClearanceMode,Timeout);
             System.out.println("[TestSmartCard()] Smartcard Connect Return Code: "+rtcode );
             
             //++SmartCardObj.EnableCardAcceptance(Timeout);
             //try{
			 //	TimeUnit.MINUTES.sleep(10);
		     //}catch(InterruptedException ex){
			 //	 
			 //}
			 
             int CardTechType = 0;
             byte[] CommandAPDU = new byte[4];
             CommandAPDU[0] = 0x5A;
             CommandAPDU[1] = 0x00;
             CommandAPDU[2] = 0x00;
             CommandAPDU[3] = 0x00;
             int SAMSlotId = 1;
             byte[] RtAPDU = null;
             
             
             System.out.println("\n");
             byte[] RtActivateArray = null;
             RtActivateArray = SmartCardObj.ActivateCard( CardTechType, SAMSlotId, Timeout);
             if(null == RtActivateArray ){
				 System.out.println("[TestSmartCard()] Smartcard activatecard Return NULL Array: " );
			 }else{
				 System.out.println("[TestSmartCard()] Smartcard activatecard Not Null array " );
				 System.out.println("[TestSmartCard()] Smartcard activatecard Array Length: "+RtActivateArray.length );
				 for(int counter=0;counter<RtActivateArray.length;counter++){
						System.out.println("[TestSmartCard()] RtActivateArray["+counter+"]: "+RtActivateArray[counter] );
				 }
			 }//else end
             
             //++XChangeAPDu
             System.out.println("\n");
             RtAPDU = SmartCardObj.XChangeAPDU( CardTechType, CommandAPDU, SAMSlotId, Timeout);
             if(null == RtAPDU ){
				 System.out.println("[TestSmartCard()] Smartcard XChangeAPDU Return NULL Array: " );
			 }else{
				 System.out.println("[TestSmartCard()] Smartcard XChangeAPDU Not Null array " );
				 System.out.println("[TestSmartCard()] Smartcard XChangeAPDU Array Length: "+RtAPDU.length );
				 for(int counter=0;counter<RtAPDU.length;counter++){
						System.out.println("[TestSmartCard()] RtAPDU["+counter+"]: "+RtAPDU[counter] );
				 }
			 }//else end
			 
             System.out.println("\n");
             System.out.println("[TestSmartCard()] Smartcard DisConnectDevice Return Code: "+SmartCardObj.DisConnectDevice (1000) );
			 */
			 
             
             //++Common.Test();
			 try{
				SmartCardObj      = new SmartCard(1);
				SmartCardObjOther = new SmartCard(2);
             }catch(IOException ex){
				System.out.println("[TestSmartCard()] Smartcard object creation thrown IOException");
				return;
			 }//++ catch(IOException ex) end
			 
			 System.out.println("[TestSmartCard()] Smartcard object creation success");
			 
			 rtcode = 0;
			 PortId = 3;
			 rtcode = SmartCardObj.ConnectDevice(PortId,ChannelClearanceMode,Timeout);
			 System.out.println("[TestSmartCard()] Smartcard Connect Return Code: "+rtcode );
				 
				 
			  System.out.println("[TestSmartCard()] Smartcard Other object creation success");
			  rtcode = 0;
			  PortId = 5;
			  rtcode = SmartCardObjOther.ConnectDevice(PortId,ChannelClearanceMode,Timeout);
			  System.out.println("[TestSmartCard()] Smartcard Other object Connect Return Code: "+rtcode );
			 
			 while(true){
			 
				 ComponentId = 0; //++ALL
				 DeviceStatus = null;
				 DeviceStatus = SmartCardObj.GetDeviceStatus( ComponentId, 100 );
				 System.out.println("[TestSmartCard()] SmartCardObj GetDeviceStatus Return Code: "+new String(DeviceStatus) );
				 
				 try {
					Thread.sleep(6000);
				 } catch (InterruptedException e) {
				 }//catch (InterruptedException e) end
				 
				 ComponentId = 0; //++ALL
				 DeviceStatus = null;
				 DeviceStatus = SmartCardObjOther.GetDeviceStatus( ComponentId, 100 );
				 System.out.println("[TestSmartCard()] SmartCardObjOther GetDeviceStatus Return Code: "+new String(DeviceStatus) );
				 
				 try {
					Thread.sleep(2000);
				 } catch (InterruptedException e) {
					
				 }//catch (InterruptedException e) end
				 
			 }//while end
			 
			 //System.out.println("[TestSmartCard()] Smartcard DisConnectDevice Return Code: "+SmartCardObj.DisConnectDevice (1000) );
			 //System.out.println("[TestSmartCard()] Smartcard Other DisConnectDevice Return Code: "+SmartCardObjOther.DisConnectDevice (1000) );
				 
             
             /*
             System.out.println("[TestSmartCard()] Smartcard EnableCardAcceptance Return Code: "+SmartCardObj.EnableCardAcceptance(100) );
             try {
				Thread.sleep(2000);
			 } catch (InterruptedException e) {
				
			 }//catch (InterruptedException e) end
			 
			 System.out.println("[TestSmartCard()] Smartcard Other Object EnableCardAcceptance Return Code: "+SmartCardObjOther.EnableCardAcceptance(100) );
             try {
				Thread.sleep(2000);
			 } catch (InterruptedException e) {
				
			 }//catch (InterruptedException e) end
             
             System.out.println("\n\n");
             //System.out.println("\n\n");
             //System.out.println("\n\n");
             
             //++Sam Card Activation
             ActivateCard = null;
             ActivateCard = SmartCardObj.ActivateCard(1,1,1000);
             System.out.println("[TestSmartCard()] Smartcard ActivateCard Return Code: "+ ActivateCard[0]);
             System.out.println("[TestSmartCard()] Smartcard ActivateCard Card Type: "+ ActivateCard[1]);
             
             System.out.println("\n\n");
             //System.out.println("\n\n");
             
             //++ContactLess card Activation 
             //ActivateCard = null;
             //ActivateCard = SmartCardObjOther.ActivateCard(0,1,1000);
             //System.out.println("[TestSmartCard()] Smartcard ActivateCard Return Code: "+ ActivateCard[0]);
             //System.out.println("[TestSmartCard()] Smartcard ActivateCard Card Type: "+ ActivateCard[1]);
             
             System.out.println("\n\n");
             //System.out.println("\n\n");
             //System.out.println("\n\n");

              //++Sam Card Activation
             ActivateCard = null;
             ActivateCard = SmartCardObjOther.ActivateCard(1,1,1000);
             System.out.println("[TestSmartCard()] Smartcard Other Object ActivateCard Return Code: "+ ActivateCard[0]);
             System.out.println("[TestSmartCard()] Smartcard Other Object ActivateCard Card Type: "+ ActivateCard[1]);
             
             System.out.println("\n\n");
             //System.out.println("\n\n");
             
             //++ContactLess card Activation 
             //ActivateCard = null;
             //ActivateCard = SmartCardObjOther.ActivateCard(0,1,1000);
             //System.out.println("[TestSmartCard()] Smartcard Other Object ActivateCard Return Code: "+ ActivateCard[0]);
             //System.out.println("[TestSmartCard()] Smartcard Other Object Card Type: "+ ActivateCard[1]);
             
             int CardTechType=0; //++Contact Less
             byte[] CommandAPDU  = new byte[4];
             CommandAPDU[0] = 0x5A;
             CommandAPDU[1] = 0x00;
             CommandAPDU[2] = 0x00;
             CommandAPDU[3] = 0x00;
             int SAMSlotId=1;
             Timeout=1000;
             byte[] ReplyByte = null;
             
             System.out.println("\n\n");
             //++ReplyByte = null;
             //++ReplyByte = SmartCardObj.XChangeAPDU(CardTechType, CommandAPDU,SAMSlotId,Timeout) ;
             
             ReplyByte = null;
             //ReplyByte = SmartCardObjOther.XChangeAPDU(CardTechType, CommandAPDU,SAMSlotId,Timeout) ;
             
             System.out.println("\n\n");
             if( ReplyByte!= null ) {
				 for(int counter=0;counter<ReplyByte.length;counter++){
					  System.out.println("[TestSmartCard()] Smartcard XChangeAPDU ReplyByte["+counter+"]:  "+ReplyByte[counter] );
				 }//++for end
             }//if end
             
             byte[] CommandAPDU2  = new byte[2];
             CommandAPDU2[0] = 0x0A;
             CommandAPDU2[1] = 0x00;
             
             ReplyByte = null;
             //++ReplyByte = SmartCardObj.XChangeAPDU(CardTechType, CommandAPDU2,SAMSlotId,Timeout) ;
             //ReplyByte = SmartCardObjOther.XChangeAPDU(CardTechType, CommandAPDU2,SAMSlotId,Timeout) ;
             System.out.println("\n\n");
             if( ReplyByte!= null ) {
				 for(int counter=0;counter<ReplyByte.length;counter++){
					  System.out.println("[TestSmartCard()] Smartcard XChangeAPDU ReplyByte["+counter+"]:  "+ReplyByte[counter] );
				 }//++for end
             }//if end
             
             //++System.out.println("[TestSmartCard()] Smartcard AcceptCard Return Code: "+SmartCardObj.AcceptCard (5000) );
             //++System.out.println("[TestSmartCard()] Smartcard DisableCardAcceptance Return Code: "+SmartCardObj.DisableCardAcceptance (5000) );
             //++System.out.println("[TestSmartCard()] Smartcard IsCardInChannel Return Code: "+SmartCardObj.IsCardInChannel (5000) );
             //++System.out.println("[TestSmartCard()] Smartcard DispenseCard Return Code: "+SmartCardObj.DispenseCard (5000) );
             //++System.out.println("[TestSmartCard()] Smartcard CollectCard Return Code: "+SmartCardObj.CollectCard (1000) );
             //++System.out.println("[TestSmartCard()] Smartcard RejectCard Return Code: "+SmartCardObj.RejectCard (1000) );
             
             System.out.println("[TestSmartCard()] Smartcard DisConnectDevice Return Code: "+SmartCardObj.DisConnectDevice (1000) );
             System.out.println("[TestSmartCard()] Smartcard Other DisConnectDevice Return Code: "+SmartCardObjOther.DisConnectDevice (1000) );
			 */
			 
			 	
       }//public static void TestSmartCard() end

	}//++public class TestSmartCard end
