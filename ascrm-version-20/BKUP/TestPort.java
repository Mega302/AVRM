import Cris.Currency;
class TestPort{

       public final static char[] hexArray = "0123456789ABCDEF".toCharArray();

       public static void main(String args[]){

                /*
                public static synchronized native   int ActivateCCTalkPort(int portNmbr);

                public static synchronized native   int DeActivateCCTalkPort();

                public static synchronized native   int GetReplyFromCCTalkPort(byte[] Command,int recvlength,byte[] Reply);

                public static synchronized native   int IsCCTALKPortOpen();
                */

                //Currency CurrencyObj = new Currency(1);

                if( 0 == Currency.IsCCTALKPortOpen() ){
                       Currency.ActivateCCTalkPort(0);
                }

                byte[] Command = new byte[5];
                byte[] Reply  = new byte[10];
                Command[0]=(byte)0x02;
                Command[1]=(byte)0x00;
                Command[2]=(byte)0x01;
                Command[3]=(byte)0xFE;
                Command[4]=(byte)0xFF;
                int recvlength =10;
                Currency.GetReplyFromCCTalkPort( Command,recvlength,Reply);
                String replybyte="";
                for(int counter=0;counter< Reply.length;counter++ ){
           
                    //System.out.println("ReplyByte: "+System.out.format("%02X",Reply[counter]) );
                    //System.out.println("ReplyByte: "+byteToHex(Reply[counter]) );
                    replybyte = replybyte+"0x"+byteToHex(Reply[counter])+" " ;
                }
                System.out.println("[java main()]ReplyByte: "+replybyte );
                //System.out.println("[java main()]ReplyByte: "+bytesToHex(Reply) );

                Currency.DeActivateCCTalkPort();

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


}//class TestPort end
