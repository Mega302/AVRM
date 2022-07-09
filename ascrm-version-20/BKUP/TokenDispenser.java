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

class TokenDispenser{


       private String DeviceId=null;

       public final static char[] hexArray = "0123456789ABCDEF".toCharArray();

       TokenDispenser(int fnDeviceId){
              DeviceId = String.format("TKD%d",fnDeviceId);
       }

       public int TokenReader_Log(String fnMessage ){
              Currency.WriteLog(this.DeviceId,fnMessage);
              return 0;
       }

       public int TokenReader_Log(String fnMessageType,String fnMessage ){
              Currency.WriteDataExchangeLog(this.DeviceId,fnMessageType,fnMessage);
              return 0;
       }

       public int TokenReader_Connect(int fnPortId ){
                
                if( 0 == Currency.IsCCTALKPortOpen() ){ // 

                       if( 1 == Currency.ActivateCCTalkPort(fnPortId) ){
                            return 0;//Connected Successfully
                       }else{
                            return 2;//Communication Failure
                       }
            
                }else{
                       return 0;//Connected Successfully   
                }
     
       } 

       public int TokenReader_Disconnect(){
                
               if( 1 == Currency.IsCCTALKPortOpen() ){

                       if( 1 == Currency.DeActivateCCTalkPort() ){
                            return 0;//disconnected Successfully
                       }else{
                            return 2;//Communication Failure
                       }
            
                }else{
                       return 0;//disconnected Successfully   
                }

       } 

       public int  TokenReader_CommunicationCycle(byte[] Command,int fnrecvlength,byte[]  fnReply){
                 return Currency.GetReplyFromCCTalkPort( Command,fnrecvlength,fnReply);
       } 

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

        public static String bytesToHex(byte[] bytes) {
		char[] hexChars = new char[bytes.length * 2];
		for ( int j = 0; j < bytes.length; j++ ) {
		    int v = bytes[j] & 0xFF;
		    hexChars[j * 2] = hexArray[v >>> 4];
		    hexChars[j * 2 + 1] = hexArray[v & 0x0F];
		}
		return new String(hexChars);
        }

       public static void main(String args[]){

                byte[] Command = new byte[5];
                byte[] Reply  = new byte[10];
                String replybyte=null;
                TokenDispenser TokenDispenserObj=null;

                Command[0]=(byte)0x02;
                Command[1]=(byte)0x00;
                Command[2]=(byte)0x01;
                Command[3]=(byte)0xFE;
                Command[4]=(byte)0xFF;
                int recvlength =10;
                
                TokenDispenserObj = new TokenDispenser(1);

                TokenDispenserObj.TokenReader_Connect( 0 );

                //Log Command Bytes
                replybyte = "";
                for(int counter=0;counter< Command.length;counter++ ){
                     replybyte = replybyte+"0x"+byteToHex(Command[counter])+" " ;
                }

                System.out.println("[java main()]CommandByte: "+replybyte );

                TokenDispenserObj.TokenReader_Log("[java main()]CommandByte: "+replybyte );

                TokenDispenserObj.TokenReader_CommunicationCycle( Command,recvlength,Reply );

                //Log Command reply Bytes
                replybyte=null;
                replybyte="";
                
                for(int counter=0;counter< Reply.length;counter++ ){
                     replybyte = replybyte+"0x"+byteToHex(Reply[counter])+" " ;
                }
                
                System.out.println("[java main()]ReplyByte: "+replybyte );
                TokenDispenserObj.TokenReader_Log("[java main()]ReplyByte: "+replybyte );
                
                TokenDispenserObj.TokenReader_Disconnect();

       }

       public static String GetDateAndTime(){
        
                        /*
                        DateTimeFormatter dtfdate = DateTimeFormatter.ofPattern("dd/MM/yyyy");
			LocalDateTime nowdate = LocalDateTime.now();

                        DateTimeFormatter dtftime = DateTimeFormatter.ofPattern("HH:mm:ss");
			LocalDateTime nowtime = LocalDateTime.now();
                        
			System.out.println( "["+dtfdate.format(nowdate)+"] " +"["+dtftime.format(nowtime)+"]" ); 

                        return ( "["+dtfdate.format(nowdate)+"] " +"["+dtftime.format(nowtime)+"]" ); 
                        */
                        return null;
       }

}//class TestPort end
