/**
* Package
*/
package Cris;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.UnsupportedCommOperationException;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import java.awt.Color;
import java.awt.Graphics2D;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Arrays;
import javax.imageio.ImageReader;
import javax.imageio.stream.ImageInputStream;
import Cris.Currency;
//import java.nio.charset.StandardCharsets;
//import TestPrinter.Printer_Log;

/**
*	Class
*/
public class Printer
{
	private SerialPort serialPortObj;
	private InputStream inputStreamObj;
	private OutputStream outputStreamObj;
	private CommPortIdentifier portIdentifier;
	private CommPort port;
        private boolean ready = false;
        private boolean writejob = false;
        private boolean connectflag = false;
        private volatile boolean timeoutFlag = false;
	private String s1;
        private int rtvalue = 10;
        private boolean start_end_Print_FLAG = false;       /*false=not printing, true = printing*/
        
        private static final String DeviceId = "PRN";
		private static final int TRACE = 41;
        private static final int DEBUG = 42;
        private static final int INFO = 43;
        private static final int WARN = 44;
        private static final int ERROR = 45;
        private static final int FATAL = 46;
        private static final int ALL = 47;
        private static final int OFF = 40;
        private static final int slp_time = 200;
        
	public Printer()
	{
		portIdentifier=null;
		serialPortObj=null;
		inputStreamObj=null;
		outputStreamObj=null;
		port=null;
		s1=null;
	}//constructer Printer() end
	
        
        private void printlog(int loglvl, String msg)
        {
            Currency.Ascrm_WriteLog(DeviceId, msg, loglvl);
            //System.out.println(msg);
            //Currency.
        }
        
        
        private void setFlagValue(boolean value)
        {
            timeoutFlag = value;
            printlog(DEBUG,"[Printer] [setFlagValue()] falg value is "+timeoutFlag);
        }// private void setFlagValue(boolean value) end
		
        private boolean getFlagValue()
        {
            //TokenReader_Log("[TokenDispenser] [getFlagValue()] falg value is "+timeoutFlag);
            return timeoutFlag;
        }// private boolean getFlagValue() end
        
        public String GetPrinterFWVersion()
        {
            return "00.00.00";
        }
        
        public String GetNativeLibVersion()
        {
            /*
             * Status in StartPrint only.
             * New Method softReset()
             * softReset after every end print.
             * Date 13-07-2020          --v "01.01.00"
             */
			 
			 /*
			 * New Method private int PrintLogo(byte [] Logo, int Alignment)
			 * Any .png Image to monochrome 
			 * Date 16-09-2020          --v "02.00.00"
			 */
			 
			 /*
			 * New Method private int PrintLogo_mono(byte [] Logo, int Alignment)
			 * Only prints .png monochrome logo size fixed to 64x64
			 * Date 04-12-2020          --v "02.01.00"
			 */
			 
			 /*
			 * New Method public int PrintLogo_mono(int Timeout)
			 * Only prints .png monochrome logo size fixed to 64x64
			 * Date 31-12-2020          --v "02.02.00"
			 */
            return "02.02.00";
        }
        
        //public int GetPrinterStatus(int timeout)
        public int GetPrinterStatus(int timeout)
        {
            if(false == connectflag)
            {
                printlog(ERROR,"[printerOnlineStatus()] Printer not connected yet.");
                return 20; // Not Connected
            }
            else if(true == start_end_Print_FLAG)
            {
                printlog(ERROR,"[printerOnlineStatus()] Printer not ready. Printing");
                return 2;
            }
            int rtValue = 10;
            rtValue = printerOnlineStatus();
            if(4 == rtValue){
				//CutterError
				printlog(DEBUG, "[Printer] [printerOnlineStatus()] cutterError rtvalue change");
				rtValue = 31;
			}
            return rtValue;
        }
        
		private synchronized int printerOnlineStatus()
		{
            synchronized(this)
            {
                boolean rs = false;
                //int returnValue = 10;
                byte[] recv_byte = new byte[100];
                /*
                try{
				outputStreamObj.flush();
			}
			catch(IOException ex){
				printlog(ERROR, "[connectPort()] "+ex.getCause().toString());
			}
			*/
                //inputStreamObj = null;
                //inputStreamObj = serialPortObj.getInputStream();
                try
				{
					/*
					if(false == ready)
                        {
                            
                            return 20;
                        }
                        */
                        outputStreamObj.write(0x1D);
						outputStreamObj.write(0x72);
						outputStreamObj.write(0x01);
                        
                        rs = Read_Reply(recv_byte, 7);
                        //printlog("2nd "+recv_byte[3]);
                        //printlog("Paper jam: "+recv_byte[2]);
                        if(true == rs)
                        {
                            //success
                            switch(recv_byte[2])
                            {
                                case 1: //Paper Not in
									printlog(ERROR,"[printerOnlineStatus()] Paper Not in. recv_byte[2]="+recv_byte[2]);
                                    return 31;
                                    
								case 2: //Head open
									printlog(ERROR,"[printerOnlineStatus()] Head open. recv_byte[2]="+recv_byte[2]);
                                    return 31;
                                    
								case 4: //Cutter error
									printlog(ERROR,"[printerOnlineStatus()] Cutter Error. recv_byte[2]="+recv_byte[2]);
                                    return 4;
                                    //return 32;
                                
                                case 16: //Paper Jam
									printlog(ERROR,"[printerOnlineStatus()] Paper Jam. recv_byte[2]="+recv_byte[2]);
                                    return 2;
                                    
                                case 8: //Paper Roll nearly empty
									printlog(ERROR,"[printerOnlineStatus()] Paper Roll nearly empty. recv_byte[2]="+recv_byte[2]);
                                    return 3;
    
                                case 0:
                                    /*
                                    if(2 == recv_byte[3])
                                    {
                                        printlog(ERROR,"[printerOnlineStatus()] Other Error Occured. Timeout Less than 0");
                                        return 31;
                                    }
                                    */
                                    if(8 == recv_byte[3])
                                    {
                                        printlog(ERROR,"[printerOnlineStatus()] Other Error Occured. BM Find Error. recv_byte[3]="+recv_byte[3]);
                                        return 31;
                                    }
                                    /*
                                    else if(4 == recv_byte[3])
                                    {
                                        return 31;
                                    }
                                    */
                                    else
                                    {
                                        /*
										if(true == writejob)
                                        {
                                            //printer not ready
											printlog(ERROR,"[printerOnlineStatus()] printer not ready");
                                            return 1;
                                        }
										*/
                                        //printer ready
                                        printlog(ERROR,"[printerOnlineStatus()] recv_byte[2]="+recv_byte[2]);
                                        printlog(ERROR,"[printerOnlineStatus()] recv_byte[3]="+recv_byte[3]);
										printlog(INFO,"[printerOnlineStatus()] printer ready");
                                        return 0;                                   
                                    }
                                    
                                default:
									/*
                                    if(recv_byte[2]>16)
                                    {
                                        //Paper Jam
										printlog(ERROR,"[printerOnlineStatus()] recv_byte[2]>16, recv_byte[2]="+recv_byte[2]);
                                        return 2;
                                    }
                                    else if(recv_byte[2]>8 && recv_byte[2]<16)
                                    {
                                        //Paper roll nearly empty
										printlog(ERROR,"[printerOnlineStatus()] recv_byte[2]>8, recv_byte[2]="+recv_byte[2]);
                                        return 3;
                                    }
                                    else
                                    {
                                        //Other error
										printlog(ERROR,"[printerOnlineStatus()] other error, recv_byte[2]="+recv_byte[2]);
                                        return 31;
                                    }
                                    //break;
                                    */
                                    printlog(DEBUG,"[printerOnlineStatus()] is, recv_byte[2]="+recv_byte[2]);
                                    return 0;
                            }
                            //return 1;
                        }
                        else
                        {
                            //failure
							printlog(ERROR,"[printerOnlineStatus()] other error readReply=false");
                            return 31;
                        }
                        
		}
		catch(IOException ex)
		{
                    printlog(ERROR, ex.getCause().toString());
                    return 31;
		}
            }    
	}//printer_Online_Status() end
		
	/**
	*	To connect the printer device this method is used.
	*/
        
        public boolean isPrinterConnected()
        {
            return connectflag;
        }// public boolean isPrinterConnected() end
        
        private void setConnectFlag(boolean flagValue)
        {
            connectflag = flagValue;
        }// private void setConnectFlag(boolean flagValue) end
        
        public int ConnectDevice(int portName, int Timeout)
        {
            printlog(DEBUG, "[ConnectDevice(String portName, int Timeout)] Entry.");
            
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR,"[ConnectDevice()] Other Error Occured. Timeout Less than 0");
                return 31;
            }
            else if(true == isPrinterConnected())
            {
                //Already Connected
				printlog(ERROR,"[ConnectDevice()] Already Connected");
                return 20;
            }
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        rtvalue = connectPort("/dev/ttyS"+portName);
                        if(0 == rtvalue){
							setConnectFlag(true);
						}
						
                        setFlagValue(true);
                    }}); 
				
                //timeout_THREAD.setDaemon(true);
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        printlog(DEBUG, "[Printer] [ConnectDevice()] Return Value "+rtvalue);
                        //timeout_THREAD.interrupt();
                        setFlagValue(false);
                        return rtvalue;
                        //break;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    printlog(ERROR, "[Printer] [ConnectDevice()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    //printlog(ERROR, "[Printer] [ConnectDevice()] Operation Timeout Occured ");
					return rtvalue;
                }
	    		
            }
        }//public int ConnectDevice(String portName, int Timeout) end
	
        private synchronized int connectPort(String portName)
		{
			/*
			if(true == connectflag)
			{
				printlog(ERROR, "[Printer] [connectPort()] Already Connected ");
				return 20; //Already Connected
			}
			*/
			s1=portName;
			try
			{
				portIdentifier=CommPortIdentifier.getPortIdentifier(s1);
				//Printer_Log.writePrinterLog(" [ "+s1+" ] "+"Exist");
			
			}//try end
			catch(NoSuchPortException no_such_port_exc)
			{
				//Printer_Log.writePrinterLog(" [ "+s1+" ] "+"Dose not exist");
				printlog(ERROR, "[connectPort()] [ "+s1+" ] "+"Dose not exist");
				return 25; //Port does not exist
			
			}//catch(NoSuchPortException no_such_port_exc) end
		
			if(portIdentifier.isCurrentlyOwned())
			{
				printlog(ERROR, "[connectPort()] "+s1+"Port ss CURRENTLY IN USE");
				//Printer_Log.writePrinterLog(" [ "+s1+" ] "+"Port ss CURRENTLY IN USE");
				return 28; //Communication Failure
			
			}//if end
			else
			{
				int timeout=2000;
				try
				{
					port=portIdentifier.open(this.getClass().getName(),timeout);
				
				}//try end
			
			catch(PortInUseException port_in_use_exc)
			{
				printlog(ERROR, "[connectPort()] PortInUseException");
				return 28; //Communication Failure
				//Printer_Log.writePrinterLog("PortInUseException");
			}//catch(PortInUseException port_in_use_exc) end
			
			if(port instanceof SerialPort)
			{
				serialPortObj = ( SerialPort )port;
				try
				{
					serialPortObj.setSerialPortParams(115200,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
					
				}//try end
				
				catch(UnsupportedCommOperationException unsupported_comm_operation_exc)
				{
					printlog(ERROR, "[connectPort()] Other Error UnsupportedCommOperationException");
					return 31;
					//Printer_Log.writePrinterLog("UnsupportedCommOperationException");
				}//catch(UnsupportedCommOperationException unsupported_comm_operation_exc) end
				
				try
				{
					inputStreamObj = serialPortObj.getInputStream();
				}//try end
				catch(IOException get_input_stream)
				{
					//Printer_Log.writePrinterLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					printlog(ERROR, "[connectPort()] Other error IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					return 31; // Other Error
				}//catch(IOException get_input_stream) end
				
				try
				{
					outputStreamObj = serialPortObj.getOutputStream();
					
				}//try end
				catch(IOException get_output_stream)
				{
					//Printer_Log.writePrinterLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					printlog(ERROR, "[connectPort()] Other Error IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					return 31; // Othe Error
				}//catch(IOException get_output_stream) end
				
				//Printer_Log.writePrinterLog("Connection with PRINTER is SUCCESSFULL");
				
				//Initialize printer
				OutputStream out;
				InputStream stream =null;
				out=outputStreamObj;
				try
                {
					out.write(27);
					out.write(64);
				}
				catch(IOException ex){
					printlog(ERROR, "[connectPort()] printer initialization failed");
					printlog(ERROR, "[connectPort()] "+ex.getCause().toString());
                    return 31;
				}
				
				printlog(INFO, "[connectPort()] Connection with PRINTER is SUCCESSFULL");
				ready = true;
				//setConnectFlag(true);
				return 0; //Success
				
			}//if end
			else
			{
				//Printer_Log.writePrinterLog("Connection with PRINTER is FAILED "+"Not SerialPort");
				printlog(ERROR, "[connectPort()] Other Error Connection with PRINTER is FAILED "+"Not SerialPort");
				return 31; //Other Error
			}//else end
			
		}//else end
		
	}// ConnectDevice() end
	
	
	
	private int softReset(){
			
			if(null!=inputStreamObj)
				{
					try
					{
						inputStreamObj.close();
					}//try end
					catch(IOException io_in_exc)
					{
						//WBSEDCL_Log.putLog("FAILED");
						printlog(ERROR, "[softReset()] other error inputStream close FAILED");
						//Printer_Log.writePrinterLog("inputStream close FAILED");
						return 31; //Other Error
					}//catch(IOException io_in_exc) end
				}//if end
	
				if(null!=outputStreamObj)	
				{
					try
					{
						outputStreamObj.close();
					}//try end
					catch(IOException io_out_exc)
					{
						printlog(ERROR, "[softReset()] other error outputStream close FAILED");
						//Printer_Log.writePrinterLog("outputStream close FAILED");
						return 31; //Other Error
					}//catch(IOException io_out_exc) end
					
				}//if end
		
				//serialPortObj.close();
				serialPortObj=null;
				if(port != null){
					port.close();
				}
				//port.close();
				port=null;
				printlog(INFO, "[softReset()] Printer reset Start");
			
			
			try
				{
					port=portIdentifier.open(this.getClass().getName(),2000);
				
				}//try end
			
			catch(PortInUseException port_in_use_exc)
			{
				printlog(ERROR, "[connectPort()] PortInUseException");
				return 28; //Communication Failure
				//Printer_Log.writePrinterLog("PortInUseException");
			}//catch(PortInUseException port_in_use_exc) end
			if(port instanceof SerialPort){
				
				serialPortObj = ( SerialPort )port;
				try
				{
					serialPortObj.setSerialPortParams(115200,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
					
				}//try end
				
				catch(UnsupportedCommOperationException unsupported_comm_operation_exc)
				{
					printlog(ERROR, "[softReset()] Other Error UnsupportedCommOperationException");
					return 31;
					//Printer_Log.writePrinterLog("UnsupportedCommOperationException");
				}//catch(UnsupportedCommOperationException unsupported_comm_operation_exc) end
				
				try
				{
					inputStreamObj = serialPortObj.getInputStream();
				}//try end
				catch(IOException get_input_stream)
				{
					//Printer_Log.writePrinterLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					printlog(ERROR, "[softReset()] Other error IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					return 31; // Other Error
				}//catch(IOException get_input_stream) end
				
				try
				{
					outputStreamObj = serialPortObj.getOutputStream();
					
				}//try end
				catch(IOException get_output_stream)
				{
					//Printer_Log.writePrinterLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					printlog(ERROR, "[softReset()] Other Error IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					return 31; // Othe Error
				}//catch(IOException get_output_stream) end
				
				//Printer_Log.writePrinterLog("Connection with PRINTER is SUCCESSFULL");
				
				//Initialize printer
				OutputStream out;
				InputStream stream =null;
				out=outputStreamObj;
				try
                {
					out.write(27);
					out.write(64);
				}
				catch(IOException ex){
					printlog(ERROR, "[softReset()] printer initialization failed");
					printlog(ERROR, "[softReset()] "+ex.getCause().toString());
                    return 31;
				}
				
				printlog(INFO, "[softReset()] Connection with PRINTER is SUCCESSFULL");
				ready = true;
				//setConnectFlag(true);
				return 0; //Success
				
			}//if end
			else
			{
				//Printer_Log.writePrinterLog("Connection with PRINTER is FAILED "+"Not SerialPort");
				printlog(ERROR, "[softReset()] Other Error Connection with PRINTER is FAILED "+"Not SerialPort");
				return 31; //Other Error
			}//else end
			
			
	}
	
	/**
	*	Method for disconnecting printer device. 
	*/
        public int DisConnectDevice(int Timeout)
        {
            printlog(DEBUG, "[DisConnectDevice(int Timeout)] Entry.");
            
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR, "[DisConnectDevice()] Timeout is less ");
                return 31;
            }
            else if(false == isPrinterConnected())
            {
                //Not Connected
				printlog(ERROR, "[DisConnectDevice()] Printer Not Connected ");
                return 20;
            }
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        rtvalue = disconnectPort();
                        if(0 == rtvalue){
							setConnectFlag(false);
						}
						
                        setFlagValue(true);
                    }}); 
				
                //timeout_THREAD.setDaemon(true);
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        //printlog("[Printer] [disconnectPort()] Return Value "+rtvalue);
                        disconnectPort();
                        //timeout_THREAD.interrupt();
                        setFlagValue(false);
                        return rtvalue;
                        //break;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    printlog(ERROR, "[Printer] [DisConnectDevice()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    return rtvalue;
                }
	    		
            }
        }//public int DisConnectDevice(int Timeout) end
	
        private synchronized int disconnectPort()
		{
			
                if(null!=inputStreamObj)
				{
					try
					{
						inputStreamObj.close();
					}//try end
					catch(IOException io_in_exc)
					{
						//WBSEDCL_Log.putLog("FAILED");
						printlog(ERROR, "[disconnectPort()] other error inputStream close FAILED");
						//Printer_Log.writePrinterLog("inputStream close FAILED");
						return 31; //Other Error
					}//catch(IOException io_in_exc) end
				}//if end
	
				if(null!=outputStreamObj)	
				{
					try
					{
						outputStreamObj.close();
					}//try end
					catch(IOException io_out_exc)
					{
						printlog(ERROR, "[disconnectPort()] other error outputStream close FAILED");
						//Printer_Log.writePrinterLog("outputStream close FAILED");
						return 31; //Other Error
					}//catch(IOException io_out_exc) end
					
				}//if end
		
				//serialPortObj.close();
				if(port != null){
					port.close();
				}
				//port.close();
				port=null;
				serialPortObj=null;
				printlog(INFO, "[disconnectPort()] Printer Disconnected");
				//Printer_Log.writePrinterLog("Disconnected");
                //setConnectFlag(false);
				return 0; //Disconnected 
		
	}// DisConnectDevice() end
	
	/**
	*	Method for reading 
	*/
	
	public void readPort()
	{
		//return 0;
            InputStream inputStreamObj;
            inputStreamObj = this.inputStreamObj;
            byte[] buffer = new byte[1024];
            int len = -1;
            
            try
            {
                while ( ( len = this.inputStreamObj.read(buffer)) > -1 )
                {
                    System.out.print(new String(buffer,0,len));
                }
            }
            catch ( IOException e )
            {
                printlog(ERROR, "[readPort()] "+e.getMessage());//e.printStackTrace();
            }
	}// readPort() end
	
	/**
	*	Method for write in PRINTER
	*/
        //public int writePort(String msg)
	private synchronized int writePort(String msg, int alingnmnt)
	{
		//Scanner sc = new Scanner(System.in);
            synchronized(this){
                
                OutputStream out;
				InputStream stream =null;
				out=outputStreamObj;
                try
                {
                    switch(alingnmnt)
                    {
                        case 0: out.write(27);
                                out.write(97);
                                out.write(49);
                                break;
                        
                        case 1: out.write(27);
                                out.write(97);
                                out.write(48);
                                break;
                                
                        case 2: out.write(27);
                                out.write(97);
                                out.write(50);
                                break;
                    }
                }
                catch(IOException ex)
                {
                    printlog(ERROR, "[writePort()] "+ex.getCause().toString());
                    return 31;
                }
				String s="!";
				String pr=msg;
				pr=pr+"\n"+s;
				try
				{
					stream = new ByteArrayInputStream(pr.getBytes("UTF-8"));		
				}// try end
				catch(UnsupportedEncodingException uex)
				{
					printlog(ERROR, "[ writePort() other error UnsupportedEncodingException caught ]");
					return 31;
				}// catch(UnsupportedEncodingException uex) end
				byte[] recv_byte= new byte[100];
				int totalbytetorecv=0;
				try
				{
					int c=0;						
					while((c=stream.read())!=33)
					{									
						out.write(c);
						//printlog(c);
					}//while end

				}//try end
				catch(IOException ioexc)
				{
					printlog(ERROR, "[writePort()] "+ioexc.getCause().toString());
					return 31;
				}//catch end
				
				out=null;
				stream=null;
                printlog(INFO, "[Printer] [writePort()] Print msg Successfull.");
                
				return 0;
            }
	}// writePort() end
	
	public boolean Read_Reply( byte[] recv_byte, int totalbytetorecv)
	{
		int receive=0,counter=0;
		long endTime=0;
		long startTime=System.currentTimeMillis();
		
		//while(true)
		for(;;)
		{
				receive=0;
				try
				{
					receive = inputStreamObj.read();
					//inputStreamObj.read(b);
				}// try end
				catch(IOException ex)
				{
					printlog(ERROR, "[Printer] [Read_Reply()]"+ex.getMessage());
					return false;
				}// catch(IOException ex) end
		
				if(receive>=0 && receive<=255)
				{
					recv_byte[counter]= (byte)receive;
					counter++;
					if(counter==totalbytetorecv)
					{
						//stream = null;
						return true;
					}
				}
		
				endTime=System.currentTimeMillis();
				if(endTime-startTime>=1000)
				{
					if(counter==totalbytetorecv)
					{
						//stream = null;
						return true;
					}
					else
					{
						printlog(ERROR, "[Printer] [Read_Reply()] return false.");
						//stream = null;
						return false;
					}
				
				}
		}//while(true)
		
	}//public boolean Read_Reply( byte[] recv_byte,int totalbytetorecv)
        
        public int StartPrint(int Timeout)
        {
            printlog(DEBUG, "[Printer] [StartPrint(int Timeout)] Entry.");
            
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR, "[Printer] [StartPrint()] wrong Timeout value.");
                return 31;
            }
            else if(false == isPrinterConnected())
            {
                //Not Connected
                printlog(ERROR, "[Printer] [StartPrint()] Printer not connected.");
                return 20;
            }
            else if(true == start_end_Print_FLAG)
            {
                //Printer not ready to print
				printlog(ERROR, "[Printer] [StartPrint()] Printer not ready, print ongoing");
                return 1;
            }
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        rtvalue = printerOnlineStatus();
                        if(0 == rtvalue)
                        {
                            start_end_Print_FLAG = true;
                        }
						else{
							printlog(ERROR, "[EndPrint()] printerOnlineStatus return="+rtvalue);
							
							rtvalue = printerOnlineStatus();
							printlog(DEBUG, "[EndPrint()] printerOnlineStatus return="+rtvalue);
							if(0 == rtvalue)
							{
								printlog(DEBUG, "[EndPrint()] printerOnlineStatus return="+rtvalue);
								start_end_Print_FLAG = true;
							}
							
						}
                        setFlagValue(true);
                    }}); 
				
                //timeout_THREAD.setDaemon(true);
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        //printlog(DEBUG, "[Printer] [StartPrint()] Return Value "+rtvalue);
                        //timeout_THREAD.interrupt();
                        setFlagValue(false);
                        return rtvalue;
                        //break;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    printlog(ERROR, "[Printer] [StartPrint()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    printlog(INFO, "[Printer] [StartPrint()] returncode "+rtvalue);
                    if(4 == rtvalue ){
						//CutterError
						printlog(DEBUG, "[Printer] [StartPrint()] cutterError rtvalue change");
						rtvalue = 31;
					}
                    return rtvalue;
                }
	    		
            }

        }//public int StartPrint(int Timeout) end
	
        public int PrintLogo(byte[] Logo, int Alignment, int Timeout)
        {
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR, "[Printer] [PrintLogo()] Wrong Timeout ");
                return 31;
            }
            else if(false == isPrinterConnected())
            {
                //Not Connected
				printlog(ERROR, "[Printer] [PrintLogo()] Printer Not Connected");
                return 20;
            }
            else if(false == start_end_Print_FLAG)
            {
                //Printer not ready to print
				printlog(ERROR, "[Printer] [PrintLogo()] Printer Not Ready");
                return 1;
            }
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        rtvalue = 0;//printerOnlineStatus();
                        if(0 == rtvalue){
							//rtvalue = PrintLogo(Logo, Alignment);
							rtvalue = PrintLogo_mono(Logo, Alignment);
							//setFlagValue(true);
						}
                        else{
							printlog(ERROR, "[PrintLogo()] printerOnlineStatus return="+rtvalue);
							rtvalue = printerOnlineStatus();
							if(0 == rtvalue){
								printlog(DEBUG, "[PrintLogo()] printerOnlineStatus return="+rtvalue);
								//rtvalue = PrintLogo(Logo, Alignment);
								rtvalue = PrintLogo_mono(Logo, Alignment);
							//setFlagValue(true);
							}
							//setFlagValue(true);
						}
                        setFlagValue(true);
                    }}); 
				
                //timeout_THREAD.setDaemon(true);
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        printlog(DEBUG, "[Printer] [PrintLogo()] Return Value "+rtvalue);
                        //timeout_THREAD.interrupt();
                        setFlagValue(false);
                        return rtvalue;
                        //break;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    printlog(ERROR, "[Printer] [PrintLogo()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    printlog(INFO, "[Printer] [PrintLogo()] returncode "+rtvalue);
                    if(4 == rtvalue){
						//CutterError
						printlog(DEBUG, "[Printer] [PrintLogo()] cutterError rtvalue change");
						rtvalue = 31;
                    }
                    return rtvalue;
                }
	    		
            }
        }
        
		private int PrintLogo(byte [] Logo, int Alignment) {
			InputStream rawinput = null;
			InputStream grayinput = null;
			ImageInputStream iis_obj=null;
			BufferedImage togray = null;
			BufferedImage grayscale = null;
			BufferedImage tomono = null;
			BufferedImage monochrome = null;
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			//byte[] imagebyte = null;
			ArrayList<Byte> image = new ArrayList<Byte>();
			
				if(null != Logo){
					rawinput = new ByteArrayInputStream(Logo);
					
					try{
						/*
						iis_obj = ImageIO.createImageInputStream(new ByteArrayInputStream(Logo));
						Iterator<ImageReader> iter = ImageIO.getImageReaders(iis_obj);
						if (!iter.hasNext()) {
							printlog(ERROR, "[Printer] [PrintLogo()] No image readers found");
							return 31;
						}
						String format = iter.next().getFormatName();
						System.out.println("Format : "+format);
						if(null != format){
							printlog(ERROR, "[Printer] [PrintLogo()] image format null");
							return 31;
						}
						else if(format.equals("")){
							printlog(ERROR, "[Printer] [PrintLogo()] No image format found!");
							return 31;
						}
						*/
						//Convert to grayscale
						togray = ImageIO.read(rawinput);
						grayscale = new BufferedImage(togray.getWidth(), togray.getHeight(), BufferedImage.TYPE_BYTE_GRAY);
						
						Graphics2D graphic = grayscale.createGraphics();
						graphic.drawImage(togray, 0, 0, Color.WHITE, null);
						graphic.dispose();
						
						ImageIO.write(grayscale, "png", baos);
						
						//Grayscale to monochrome
						grayinput = new ByteArrayInputStream(baos.toByteArray());
						tomono = ImageIO.read(grayinput);
						monochrome = new BufferedImage(tomono.getWidth(), tomono.getHeight(), BufferedImage.TYPE_BYTE_BINARY);
						
						Graphics2D graphic_mono = monochrome.createGraphics();
						graphic_mono.drawImage(tomono, 0, 0, Color.WHITE, null);
						graphic_mono.dispose();
						
						File output = new File("smaple_mono.png");
						ImageIO.write(monochrome, "png", output);
						//constructing the imagedata
						
						int addOnPixels_no = 8-(monochrome.getWidth()%8);
						//Integer ad = 0;
							//for(int i=0; i<monochrome.getHeight(); i++) { 
							for(int i=monochrome.getHeight()-1; i>=0; i--) { 
							
							//for(int j=monochrome.getWidth()-1; j>=0; j--) {
							for(int j=0; j<monochrome.getWidth(); j++) { 
								
								if(j>monochrome.getWidth()-1){
									image.add((byte)1);
								}
								else{
									Color c = new Color(monochrome.getRGB(j,i));  //getting pixels row-column wise, not column-row wise
									Integer k = new Integer(c.getRed());
								//System.out.print(k.byteValue());
								/*
								if(j==(monochrome.getWidth()-1)){
									if(8!=addOnPixels_no){
										for(int aop=0; aop<addOnPixels_no; aop++){
											image.add((byte)0);
										}
									}
								}
								*/
									image.add(k.byteValue());
								}
							}
							/*
							if(8!=addOnPixels_no){
								for(int aop=0; aop<6; aop++){
									image.add((byte)0);
								}
							}
							*/
						}
						
						int[] imagebyte = new int[image.size()];
						for(int i=0; i<image.size(); i++){
							if(0 != image.get(i))
								imagebyte[i] = 1;
							else
								imagebyte[i] = 0;
						}
						
						String bytest = Arrays.toString(imagebyte).replace(", ", "");
						String arrst = bytest.replace("[", "").replace("]", "");		//pixelbits in string
						//System.out.println(" byte st : "+ arrst.charAt(0));
						byte[] bittobytearr = new byte[imagebyte.length/8];				//to store pixelbits in byte array
						int start = 0;
						int end = 8;
						for(int i = 0; i<bittobytearr.length; i++){
							
							if(0 != i){
								start = end;
								end = end+8;
							}
							
							String s = arrst.substring(start, end);
							byte b = (byte)Integer.parseInt(s, 2);					
							bittobytearr[i] = b;										//storeing pixelbits in byte array
						}
						//height and width 
						return PrintLogo_v1(bittobytearr, Alignment, monochrome.getHeight(), monochrome.getWidth());
						//sending print command
						//return PrintLogo_v1(bittobytearr, Alignment);
					}
					catch(IOException e){
						printlog(ERROR, "[Printer] [PrintLogo()] IOException "+e.getCause());
						return 31;
					}
					
				}// if end
				else{
					//print from rom
					printlog(ERROR, "[Printer] [PrintLogo()] input byte null");
					//PrintLogo_v2(Alignment);
					return 31;
				}//else end
		}//private int PrintLogo(byte [] Logo, int Alignment) end
		
		private int PrintLogo_mono(byte [] Logo, int Alignment) {
			InputStream rawinput = null;
			InputStream grayinput = null;
			ImageInputStream iis_obj=null;
			BufferedImage togray = null;
			BufferedImage grayscale = null;
			BufferedImage tomono = null;
			BufferedImage monochrome = null;
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			//byte[] imagebyte = null;
			ArrayList<Byte> image = new ArrayList<Byte>();
			
				if(null != Logo){
					rawinput = new ByteArrayInputStream(Logo);
					
					try{
						
						monochrome = ImageIO.read(rawinput);
						//constructing the imagedata
						
						int addOnPixels_no = 8-(monochrome.getWidth()%8);
						//Integer ad = 0;
							//for(int i=0; i<monochrome.getHeight(); i++) { 
							for(int i=monochrome.getHeight()-1; i>=0; i--) { 
							
							//for(int j=monochrome.getWidth()-1; j>=0; j--) {
							for(int j=0; j<monochrome.getWidth(); j++) { 
								Color c = new Color(monochrome.getRGB(j,i));  //getting pixels row-column wise, not column-row wise
								Integer k = new Integer(c.getRed());
								//System.out.print(k.byteValue());
								/*
								if(i==(monochrome.getWidth()-1)){
									if(8!=addOnPixels_no){
										for(int aop=0; aop<addOnPixels_no; aop++){
											image.add(k.byteValue());
										}
									}
								}
								*/
								image.add(k.byteValue());
								
							}
							/*
							if(8!=addOnPixels_no){
								for(int aop=0; aop<addOnPixels_no; aop++){
									image.add((byte)0);
								}
							}
							*/
						}
						
						int[] imagebyte = new int[image.size()];
						for(int i=0; i<image.size(); i++){
							if(0 != image.get(i))
								imagebyte[i] = 1;
							else
								imagebyte[i] = 0;
						}
						
						String bytest = Arrays.toString(imagebyte).replace(", ", "");
						String arrst = bytest.replace("[", "").replace("]", "");		//pixelbits in string
						//System.out.println(" byte st : "+ arrst.charAt(0));
						byte[] bittobytearr = new byte[imagebyte.length/8];				//to store pixelbits in byte array
						int start = 0;
						int end = 8;
						for(int i = 0; i<bittobytearr.length; i++){
							
							if(0 != i){
								start = end;
								end = end+8;
							}
							
							String s = arrst.substring(start, end);
							byte b = (byte)Integer.parseInt(s, 2);					
							bittobytearr[i] = b;										//storeing pixelbits in byte array
						}
						//height and width 
						return PrintLogo_v1(bittobytearr, Alignment, monochrome.getHeight(), monochrome.getWidth());
						//sending print command
						//return PrintLogo_v1(bittobytearr, Alignment);
					}
					catch(IOException e){
						printlog(ERROR, "[Printer] [PrintLogo()] IOException "+e.getCause());
						return 31;
					}
					
				}// if end
				else{
					//print from rom
					printlog(ERROR, "[Printer] [PrintLogo()] input byte null");
					//PrintLogo_v2(Alignment);
					return 31;
				}//else end
		}//private int PrintLogo_mono(byte [] Logo, int Alignment) end
		
		
		private int PrintLogo_v1(byte [] Logo, int Alignment, int Height, int Width ) {
        //private int PrintLogo_v1(byte [] Logo, int Alignment) {
			//png Only
			
			int to_print=0;
		
			OutputStream out = outputStreamObj;
			byte[] logobyte = Logo;//new byte[Logo.length - 62];
			System.out.println(" logobyte : "+logobyte.length);
			/*
			for(int i=0; i<logobyte.length; i++)
			{
				logobyte[i] = Logo[i+62];
			}
			*/
			int xL=0, xH=0, yL=0, yH=0;
			//Calculate L,H
			
			if(xL%8 != 0){
				xL = xL/8+1;
			}
			else{
				xL= Width/8;        //n/8
			}
			
			//xL= Width/8; 
			//xL= (int) ((Math.sqrt(logobyte.length*8))/8);        //n/8
			//xL=8;
			xH= 0;
			yL= Height;        //n
			//yL=64;
			//yL= (int) Math.sqrt(logobyte.length*8);        //n
			yH= 0;
			//alingment
                
			try{
				System.out.println("yl = " +yL);
				switch(Alignment){
					
					case 0: out.write(29);
							out.write(76);
							out.write((216-(yL/4))/2);
							out.write(0);
							break;

					case 1: out.write(29);
							out.write(76);
							out.write(0);
							out.write(0);
							break;

					case 2: out.write(29);
							out.write(76);
							out.write(216-(yL/4));
							out.write(0);
							break;
				}
			}
			
			catch(IOException ex){
				printlog(ERROR, "[Printer] [PrintLogo()] "+ex.getMessage());
				out = null;
				//return 31;
			}
                
			printlog(INFO, "[PrintLogo()] Alignment done, going to print logo");
                
			try {
				out.write(29);      //GS
				out.write(118);     //V
				out.write(48);      //0
				out.write(51);      //m
				out.write(xL);      //xL
				out.write(0);       //xH
				out.write(yL);      //yL
				out.write(0);       //yH
                    
				byte[] rearrangeddata = new byte[logobyte.length];
                    
				for(int counter1=0; counter1<yL; counter1++){
					for(int counter2=0; counter2<xL; counter2++){
						rearrangeddata[(counter1*xL)+counter2] = logobyte[(xL*yL)-(((counter1+1)*xL)-counter2)];
					}
				}
                    
				//rotate
				byte[] rotatedata = new byte[logobyte.length];
                    
				//rotatedata[]
                    
				/*for(int row=0; row<8; row++)
				{
					for(int col=0; col<64; col++){
						rotatedata[(row*8)+col] = rearrangeddata[(col*8)+row];
					}
				}*/
                    
				for (int i_count=0; i_count < rearrangeddata.length; i_count++){
					if(logobyte[i_count] < 0)
						to_print = rearrangeddata[i_count] + 256;
					else
						to_print = rearrangeddata[i_count];

					printlog(DEBUG, "[PrintLogo()] Actual value: "+to_print);
					printlog(DEBUG, "[PrintLogo()] Changed value: "+(255-to_print));
					out.write(255-to_print);
							//out.write(to_print);
							//Send the integer to serial port
				}
                        
                    //printlog(DEBUG, "Logo printed");
                    //out.write(0);
                    
				try {
					Thread.sleep(10);
				} catch (InterruptedException ex) {
					printlog(ERROR, "[PrintLogo()] "+ex.getMessage());
				}
				printlog(DEBUG, "[PrintLogo()] going to reset GS L");
                    //GS L Reset
                    out.write(29);
                    out.write(76);
                    out.write(0);
                    out.write(0);
                    
				out = null;
				printlog(INFO,  "[Printer] [PrintLogo()] image printed successfully.");
				return 0;
			} 
			catch (IOException ex) {
				System.out.println( "[Printer] [PrintLogo()] other error "+ex.getMessage());
				out = null;
				return 31;
			}
                
		}// private int PrintLogo_v1(byte [] Logo, int Alignment) end
     
		public int PrintLogo_NVM(int Alignment) {
			//Decimal 28 112 n m
			OutputStream out;
			InputStream stream =null;
			out=outputStreamObj;
			/*
			//alingment
			try{
				switch(Alignment){
					case 0: out.write(29);
							out.write(76);
							out.write((216-(yL/4))/2);
							out.write(0);
							break;

					case 1: out.write(29);
							out.write(76);
							out.write(0);
							out.write(0);
							break;

					case 2: out.write(29);
							out.write(76);
							out.write(216-(yL/4));
							out.write(0);
							break;
                }
			}
			catch(IOException ex){
				printlog(ERROR, "[Printer] [PrintLogo_NVM()] "+ex.getMessage());
				out = null;
				//return 31;
			}
			printlog(INFO, "[PrintLogo_NVM()_v2] Alignment done, going to print logo");
			*/
			printlog(DEBUG, "[PrintLogo_NVM()] going to print logo from printer NVM");
			try {
				out.write(28);
				out.write(112);
				out.write(1);           // n = 1
				//out.write(0);           //m = 0
				out.write(48);        //m = 48
			} catch (IOException ex) {
					printlog(ERROR, "[Printer] [PrintLogo_NVM()] "+ex.getMessage());;
					return 31;
			}
			return 0;
		}//private int PrintLogo_v2(int Alignment) end
        
		public int store_PrintLogo(int Timeout) {
			//Decimal 28 113 n
			ArrayList<Byte> image = new ArrayList<Byte>();
			OutputStream out;
			InputStream stream =null;
			out=outputStreamObj;
			byte[] imgbyte = null;
			BufferedImage imag;
			try {
				imag = ImageIO.read(new File("sample1.png"));
            //BufferedImage imag = ImageIO.read(new File("ir.png"));
            //BufferedImage imag = ImageIO.read(new File("24dpth1.png"));
				ByteArrayOutputStream bos = new ByteArrayOutputStream();
				ImageIO.write(imag, "png", bos);
				imgbyte = bos.toByteArray();
            
				int height = imag.getHeight();
				int width = imag.getWidth();
            
            //png
				for(int i=imag.getHeight()-1; i>=0; i--) { 
            //for(int i=0; i<imag.getHeight(); i++) {
            //for(int j=monochrome.getWidth()-1; j>=0; j--) {
					for(int j=0; j<imag.getWidth(); j++) { 
						Color c = new Color(imag.getRGB(j,i));  //getting pixels row-column wise, not column-row wise
                    //Color c = new Color(imag.getRGB(i,j));  //getting pixels row-column wise, not column-row wise
						Integer k = new Integer(c.getRed());
                    
						image.add(k.byteValue());

					}
				}
				int[] imagebyte = new int[image.size()];
				for(int i=0; i<image.size(); i++){
                    if(0 != image.get(i)){
                            //imagebyte[i] = 1;
                            imagebyte[i] = 0;
                    }
                    else{
                            //imagebyte[i] = 0;
                            imagebyte[i] = 1;
                    }
				}

				String bytest = Arrays.toString(imagebyte).replace(", ", "");
				String arrst = bytest.replace("[", "").replace("]", "");		//pixelbits in string
				//System.out.println(" byte st : "+ arrst.charAt(0));
				byte[] bittobytearr = new byte[imagebyte.length/8];				//to store pixelbits in byte array
				int start = 0;
				int end = 8;
				for(int i = 0; i<bittobytearr.length; i++){

                    if(0 != i){
                            start = end;
                            end = end+8;
                    }

                    String s = arrst.substring(start, end);
                    byte b = (byte)Integer.parseInt(s, 2);					
                    bittobytearr[i] = b;										//storeing pixelbits in byte array
				}
            
				int to_print=0;
            
				out.write(28);
				out.write(113);
				out.write(1);           // n = 1
				out.write(width/8);
				out.write(0);
				out.write(height/8);
				out.write(0);
				out.write(bittobytearr);
            
			} catch (IOException ex) {
				printlog(ERROR, "[Printer] [store_PrintLogo()] "+ex.getMessage());
				return 31;
			}
			return 0;
		}// public int store_PrintLogo(byte [] Logo, int Alignment, int Timeout) end
		/*
		private int store_PrintLogo(byte [] Logo, int Alignment) {
			//Decimal 28 113 n
			
			OutputStream out;
			InputStream stream =null;
			out=outputStreamObj;
			byte[] imgbyte = null;
			BufferedImage imag;
			try {
				imag = ImageIO.read(new File("sample1.png"));
				//BufferedImage imag = ImageIO.read(new File("ir.png"));
				//BufferedImage imag = ImageIO.read(new File("24dpth1.png"));
				ByteArrayOutputStream bos = new ByteArrayOutputStream();
				ImageIO.write(imag, "png", bos);
				imgbyte = bos.toByteArray();
				
				int height = imag.getHeight();
				int width = imag.getWidth();
				
				out.write(28);
				out.write(113);
				out.write(1);           // n = 1
				out.write(width/8);
				out.write(0);
				out.write(height);
				out.write(0);
				out.write(imgbyte);
			} catch (IOException ex) {
				Logger.getLogger(Store_Print_logo.class.getName()).log(Level.SEVERE, null, ex);
				return 1;
			}
			
			return 0;
		}
		*/
		public int PrintTextLine(String Text, int Alignment, int Timeout)
        {
            printlog(DEBUG, "[Printer] [PrintTextLine(int Timeout)] Entry.");
            
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR, "[Printer] [PrintTextLine()] wrong Timeout value");
                return 31;
            }
            else if(false == isPrinterConnected())
            {
                //Not Connected
                printlog(ERROR, "[Printer] [PrintTextLine()] Not Connected");
                return 20;
            }
            
            else if(false == start_end_Print_FLAG)
            {
                //Printer not ready to print
                printlog(ERROR, "[Printer] [PrintTextLine()] Printer not ready to print");
                return 1;
            }
            
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        /*
                        try{
							Thread.sleep(slp_time);
						}
						catch(InterruptedException ex){
							printlog(ERROR, "[Printer] [PrintTextLine()] Exception in Thread");
						}
						*/
                        //rtvalue = printerOnlineStatus();
                        rtvalue = 0;
						if(0 == rtvalue)
						{
							/*
							try{
								Thread.sleep(slp_time);
							}
							catch(InterruptedException ex){
								printlog(ERROR, "[Printer] [PrintTextLine()] Exception in Thread");
							}
							*/
							rtvalue = writePort(Text, Alignment);
							//setFlagValue(true);
						}
						else
						{
							printlog(ERROR, "[PrintTextLine()] printerOnlineStatus return="+rtvalue);
							
							rtvalue = printerOnlineStatus();
							printlog(DEBUG, "[PrintTextLine()] printerOnlineStatus return="+rtvalue);
							if(0 == rtvalue)
							{
								printlog(DEBUG, "[PrintTextLine()] printerOnlineStatus return="+rtvalue);
								rtvalue = writePort(Text, Alignment);
							}
							//setFlagValue(true);
						}
                        setFlagValue(true);
                    }}); 
				
                //timeout_THREAD.setDaemon(true);
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        printlog(ERROR, "[Printer] [PrintTextLine()] Return Value "+rtvalue);
                        //timeout_THREAD.interrupt();
                        setFlagValue(false);
                        return rtvalue;
                        //break;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    printlog(ERROR, "[Printer] [PrintTextLine()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    printlog(DEBUG, "[Printer] [PrintTextLine()] returncode "+rtvalue);
                    return rtvalue;
                }
	    		
            }
            
        }//public int PrintTextLine(String Text, int Alignment, int Timeout) end
        
        public int PrintBlankLine(int Timeout)
        {
            printlog(DEBUG, "[Printer] [PrintBlankLine(int Timeout)] Entry.");
            
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR, "[Printer] [PrintBlankLine()] Wrong Timeout value.");
                return 31;
            }
            else if(false == isPrinterConnected())
            {
                //Not Connected
                printlog(ERROR, "[Printer] [PrintBlankLine()] Not Connected");
                return 20;
            }
            
            else if(false == start_end_Print_FLAG)
            {
                //Printer not ready to print
                printlog(ERROR, "[Printer] [PrintBlankLine()] Printer not ready to print");
                return 1;
            }
            
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        //rtvalue = printerOnlineStatus();
                        if(0 == rtvalue)
                        {
							rtvalue = PrintBlankLine();
							//setFlagValue(true);
						}
						else
						{
							printlog(ERROR, "[PrintBlankLine()] printerOnlineStatus return="+rtvalue);
							
							rtvalue = printerOnlineStatus();
							printlog(DEBUG, "[PrintBlankLine()] printerOnlineStatus return="+rtvalue);
							if(0 == rtvalue)
							{
								printlog(DEBUG, "[PrintBlankLine()] printerOnlineStatus return="+rtvalue);
								rtvalue = PrintBlankLine();
							}
							//setFlagValue(true);
						}
                        setFlagValue(true);
                        //start_end_Print_FLAG
                    }}); 
				
                //timeout_THREAD.setDaemon(true);
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        printlog(DEBUG, "[Printer] [PrintBlankLine()] Return Value "+rtvalue);
                        //timeout_THREAD.interrupt();
                        setFlagValue(false);
                        return rtvalue;
                        //break;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    printlog(ERROR, "[Printer] [PrintBlankLine()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    printlog(DEBUG, "[Printer] [PrintBlankLine()] returncode "+rtvalue);
                    return rtvalue;
                }
	    		
            }
        }// public int PrintBlankLine(int Timeout) end
        
        private synchronized int PrintBlankLine()
        {
            //int rtValue = printerOnlineStatus();
            
            /*
            if(0 != rtValue)
            {
                printlog(ERROR, "[Printer] [PrintBlankLine()] Printer Error : "+rtValue);
                return rtValue;
            }
            */
            OutputStream out = outputStreamObj;
            try {
                out.write(27);
                out.write(100);
                out.write(1);
            } catch (IOException ex) {
                    
                printlog(ERROR, "[Printer] [PrintBlankLine()] IOexception "+ex.getMessage());
                out = null;
                return 31;
            }
            printlog(INFO, "[Printer] [PrintBlankLine()] successful.");
            return 0;//Success
        }// public int PrintBlankLine() end
        
        public int EndPrint(int PaperCuttingMethod, int Timeout)
        {
            printlog(DEBUG, "[Printer] [EndPrint()] Entry.");
            
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR,"[Printer] [EndPrint()] Wrong Timeout value.");
                return 31;
            }
            else if(false == isPrinterConnected())
            {
                //Not Connected
                printlog(ERROR, "[Printer] [EndPrint()] Not Connected");
                return 20;
            }
            
            else if(false == start_end_Print_FLAG)
            {
                //Printing not yet started
                printlog(ERROR, "[Printer] [EndPrint()] Printer not ready to print");
                return 1;
            }
            
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        /*
                        try{
							Thread.sleep(slp_time);
						}
						catch(InterruptedException ex){
							printlog(ERROR, "[Printer] [PrintTextLine()] Exception in Thread");
						}
                        */
                        rtvalue = printerOnlineStatus();
                        try{
							Thread.sleep(100);
						}
						catch(InterruptedException ex){
							printlog(ERROR, "[Printer] [PrintTextLine()] Exception in Thread");
						}
                        if(0 == rtvalue){
							rtvalue = EndPrint(PaperCuttingMethod);
							//setFlagValue(true);
							//start_end_Print_FLAG = true;
						}
						else{
							printlog(ERROR, "[EndPrint()] printerOnlineStatus return="+rtvalue);
							try{
								Thread.sleep(slp_time);
							}
							catch(InterruptedException ex){
								printlog(ERROR, "[Printer] [PrintTextLine()] Exception in Thread");
							}
							rtvalue = printerOnlineStatus();
							printlog(DEBUG, "[EndPrint()] printerOnlineStatus return="+rtvalue);
							if(0 == rtvalue)
							{
								printlog(DEBUG, "[EndPrint()] printerOnlineStatus return="+rtvalue);
								rtvalue = EndPrint(PaperCuttingMethod);
							}
							//setFlagValue(true);
							//start_end_Print_FLAG = true;
						}
                        //rtvalue = EndPrint(PaperCuttingMethod);
                        setFlagValue(true);
                        start_end_Print_FLAG = false;
                    }}); 
				
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        printlog(DEBUG, "[Printer] [EndPrint()] Return Value "+rtvalue);
                        setFlagValue(false);
                        return rtvalue;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    printlog(ERROR, "[Printer] [EndPrint()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    printlog(DEBUG, "[Printer] [EndPrint()] returncode "+rtvalue);
                    if(4 == rtvalue){
						//Cutter Error
						printlog(DEBUG, "[Printer] [EndPrint()] cutterError rtvalue change");
						rtvalue = 3;
					}
                    return rtvalue;
                }
	    		
            }
        }//public int EndPrint(int PaperCuttingMethod, int Timeout) end
        
        private synchronized int EndPrint(int PaperCuttingMethod)
        {
            
            int rtValue = 0;//printerOnlineStatus();
            /*
            if(0 != rtValue)
            {
                printlog(ERROR, "[Printer] [EndPrint()] Printer Error : "+rtValue);
                return rtValue;
            }
            */
            OutputStream out = outputStreamObj;
            
            if(1 == PaperCuttingMethod)
            {
                try {
                    out.write(27);
                    out.write(105);
                } catch (IOException ex) {
                    
                    printlog(ERROR, ex.getMessage());
                    out = null;
                    return 31;
                }
            }
            else if(2 == PaperCuttingMethod)
            {
                try {
                    out.write(27);
                    out.write(109);
                }
                catch (IOException ex) {
                    printlog(ERROR, ex.getMessage());
                    out = null;
                    return 31;
                }
                
            }
            
            try{
					Thread.sleep(1000);
				}
				catch(Exception e){
					printlog(ERROR, "[writePort()] "+e.getCause().toString());
				}
				/*
				try
                {
					out.write(27);
					out.write(114);
				}
				catch(IOException ex){
					printlog(ERROR, "[connectPort()] printer initialization failed");
					printlog(ERROR, "[connectPort()] "+ex.getCause().toString());
                    return 31;
				}
				*/
			try{
				out.flush();
				//outputStreamObj.flush();
			}
			catch(IOException ex){
				printlog(ERROR, "[connectPort()] "+ex.getCause().toString());
			}
            out = null;
            printlog(INFO, "[Printer] [EndPrint()] successful.");
            
            if(0 == softReset()){
				printlog(INFO, "[Printer] [EndPrint()] soft reset successful.");
            }
            else{
				printlog(INFO, "[Printer] [EndPrint()] soft reset failed.");
				return 31;
			}
			
            return 0;//Success
        }// public int EndPrint(int PaperCuttingMethod, int Timeout) end
        
        public int XChangeCommande(String Command, int Timeout)
        {
            printlog(DEBUG, "[Printer] [XChangeCommande()] Entry.");
            
            if(Timeout<=0)
            {
                //Other Error
                printlog(ERROR,"[Printer] [EndPrint()] Wrong Timeout value.");
                return 31;
            }
            else if(false == isPrinterConnected())
            {
                //Not Connected
                printlog(ERROR, "[Printer] [EndPrint()] Not Connected");
                return 20;
            }
            /*
            else if(true == start_end_Print_FLAG)
            {
                //Printing not yet started
                return 1;
            }
            */
            else 
            {
                Thread timeout_THREAD;	
                timeout_THREAD = new Thread(new Runnable(){	
                    public void run()
                    {
                        rtvalue = XChangeCommande(Command);
                        setFlagValue(true);
                        //start_end_Print_FLAG = true;
                    }}); 
				
                //timeout_THREAD.setDaemon(true);
                timeout_THREAD.start();
		
                long endtime = 0;
                endtime = System.currentTimeMillis()+Timeout;
                //return rtvalue;
			
                while(endtime>System.currentTimeMillis())
                {
                    if(true == getFlagValue())
                    {
                        //printlog("[Printer] [EndPrint()] Return Value "+rtvalue);
                        setFlagValue(false);
                        return rtvalue;
                    }
                }
				
                if(false == timeoutFlag)
                {
                    //Timeout
                    //printlog("[Printer] [EndPrint()] Operation Timeout Occured ");
                    return 18;
                }
                else
                {
                    return rtvalue;
                }
	    		
            }
        }// public int XChangeCommande(String Command, int Timeout)
        
        private int XChangeCommande(String Command)
        {
            byte[] commandByte;
            OutputStream out = outputStreamObj;
            if((null != Command) || (Command.equals("")))
            {
                 commandByte = Command.getBytes();
            }
            else
            {
                return 1;
            }
            
            try {
                
                for(int i=0; i<commandByte.length; i++)
                {
                    //out.write((byte)0x1B);
                    //out.write((byte)0x4A);
                    //out.write(2);
                    //printlog("Command: "+commandByte[i]);
                   // out.write(27);
                    //out.write(51);
                    //out.write(0);
                    //out.write(0);
                    out.write(29);
                    out.write(76);
                    out.write(85);
                    out.write(0);
                    
                }
                
                out=null;
                printlog(INFO, "[Printer] [XChangeCommande()] successful.");
                return 0;//Success
            } 
            catch (IOException ex) {
                printlog(ERROR, "[Printer] [XChangeCommande()]"+ex.getMessage());
                out = null;
                return 31;
            }
            
            
        }//private int XChangeCommande(String Command) end
}// public class PrinterProject end
