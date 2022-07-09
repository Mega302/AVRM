/**
* Package
*/
package mega_ascrm.TestPrinter;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.UnsupportedCommOperationException;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.UnsupportedCommOperationException;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.lang.InterruptedException;
import java.lang.IllegalArgumentException;
import java.io.ByteArrayInputStream;
import java.nio.charset.*;
import java.io.UnsupportedEncodingException;
//import java.nio.charset.StandardCharsets;
//import TestPrinter.Printer_Log;

/**
*	Class
*/
public class METRO_Printer
{
	private SerialPort serialPortObj;
	private InputStream inputStreamObj;
	private OutputStream outputStreamObj;
	CommPortIdentifier portIdentifier;
	CommPort port;
	String s1;
	
	public METRO_Printer()
	{
		portIdentifier=null;
		serialPortObj=null;
		inputStreamObj=null;
		outputStreamObj=null;
		port=null;
		s1=null;
	}//constructer METRO_Printer() end
	
	public void printer_Online_Status()
	{
		try
		{
			outputStreamObj.write(0x1D);
			outputStreamObj.write(0x72);
			outputStreamObj.write(0x01);
		}
		catch(IOException ex)
		{
			
		}
	}//printer_Online_Status() end
		
	/**
	*	To connect the printer device this method is used.
	*/
	public int connectPort(String portName)
	{
		s1=portName;
		try
		{
			portIdentifier=CommPortIdentifier.getPortIdentifier(s1);
			//Printer_Log.writePrinterLog(" [ "+s1+" ] "+"Exist");
			
		}//try end
		catch(NoSuchPortException no_such_port_exc)
		{
			//WBSEDCL_Log.putLog(" [ "+portName+" ] "+"Dose not exist");
			//Printer_Log.writePrinterLog(" [ "+s1+" ] "+"Dose not exist");
			System.out.println(" [ "+s1+" ] "+"Dose not exist");
			
		}//catch(NoSuchPortException no_such_port_exc) end
		
		if(portIdentifier.isCurrentlyOwned())
		{
			//WBSEDCL_Log.putLog(portName+"Port ss CURRENTLY IN USE");
			System.out.println(s1+"Port ss CURRENTLY IN USE");
			//Printer_Log.writePrinterLog(" [ "+s1+" ] "+"Port ss CURRENTLY IN USE");
			return 0;
			
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
				System.out.println("PortInUseException");
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
					System.out.println("UnsupportedCommOperationException");
					//Printer_Log.writePrinterLog("UnsupportedCommOperationException");
				}//catch(UnsupportedCommOperationException unsupported_comm_operation_exc) end
				
				try
				{
					inputStreamObj = serialPortObj.getInputStream();
				}//try end
				catch(IOException get_input_stream)
				{
					//WBSEDCL_Log.putLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					//Printer_Log.writePrinterLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					System.out.println("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					return 0;
				}//catch(IOException get_input_stream) end
				
				try
				{
					outputStreamObj = serialPortObj.getOutputStream();
					
				}//try end
				catch(IOException get_output_stream)
				{
					//WBSEDCL_Log.putLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					//Printer_Log.writePrinterLog("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					System.out.println("IO EXCEPTION in"+" [ "+"serialPortObj.getOutputStream()"+" ] ");
					return 0;
				}//catch(IOException get_output_stream) end
				
				//WBSEDCL_Log.putLog("Connection with PRINTER is SUCCESSFULL");
				//Printer_Log.writePrinterLog("Connection with PRINTER is SUCCESSFULL");
				System.out.println("Connection with PRINTER is SUCCESSFULL");
				return 1;
				
			}//if end
			else
			{
				//WBSEDCL_Log.putLog("Connection with PRINTER is FAILED "+"Not SerialPort");
				//Printer_Log.writePrinterLog("Connection with PRINTER is FAILED "+"Not SerialPort");
				System.out.println("Connection with PRINTER is FAILED "+"Not SerialPort");
				return 0;
			}//else end
			
		}//else end
		
	}// connectPort() end
	
	/**
	*	Method for disconnecting printer device. 
	*/
	public int disconnectPort()
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
				System.out.println("inputStream close FAILED");
				//Printer_Log.writePrinterLog("inputStream close FAILED");
				return 0;
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
				//WBSEDCL_Log.putLog("FAILED");
				System.out.println("outputStream close FAILED");
				//Printer_Log.writePrinterLog("outputStream close FAILED");
				return 0;
			}//catch(IOException io_out_exc) end
			
		}//if end
		
		//serialPortObj.close();
		port.close();
		port=null;
		serialPortObj=null;
		System.out.println("Disconnected");
		//Printer_Log.writePrinterLog("Disconnected");
		return 1;
		
	}// disconnectPort() end
	
	/**
	*	Method for reading 
	*/
	
	public int readPort()
	{
		return 0;
	}// readPort() end
	
	/**
	*	Method for write in PRINTER
	*/

	public int writePort(String msg)
	{
		//Scanner sc = new Scanner(System.in);
		OutputStream out;
		InputStream stream =null;
		out=outputStreamObj;
		String s="\n\n\n\n\n\n\n !";
		String pr=msg;
		pr="\n\n\n"+pr+"\n"+s;
		try
		{
			stream = new ByteArrayInputStream(pr.getBytes("UTF-8"));		
		}// try end
		catch(UnsupportedEncodingException uex)
		{
			System.out.println(" [ writePort() UnsupportedEncodingException caught ] ");
			return 0;
		}// catch(UnsupportedEncodingException uex) end
		byte[] recv_byte= new byte[100];
		int totalbytetorecv=0;
		try
		{
			int c=0;						
			while((c=stream.read())!=33)
			{									
				out.write(c);
				//System.out.println(c);
			}//while end
			
			out.write(27);
			//out.write(105);
			out.write(109);
			//printer_Online_Status();
			
			//Read_Reply(recv_byte,1);
                        
			//System.out.println("[writePort()] Recv Byte: "+recv_byte[0]);
			
			
		}//try end
		catch(IOException ioexc)
		{
			System.out.println("ioexc");
			//Printer_Log.writePrinterLog("IOException in [ writePort() ] ");
			return 0;
			
		}//catch end
		
		out=null;
		stream=null;
		return 1;
		
	}// writePort() end
	
	public boolean Read_Reply( byte[] recv_byte,int totalbytetorecv)
	{
		int receive=0,counter=0;
		long endTime=0;
		long startTime=System.currentTimeMillis();
		
		while(true)
		{
                            receive=0;
                            try
                            {
                                    receive = inputStreamObj.read();
                            }// try end
                            catch(IOException ex)
                            {
                                    System.out.println(" [Read_Reply()] IOException ex [ (receive = inputStreamObj.read()) ]");
                                    return false;
                            }// catch(IOException ex) end
					
                            if(receive>=0 && receive<=255)
                            {
                                        recv_byte[counter]= (byte)receive;
                                        counter++;
                                        if(counter==totalbytetorecv)
                                        {
                                                return true;
                                        }
                            }
					
                            endTime=System.currentTimeMillis();
                            if(endTime-startTime>=1000)
                            {
                                        if(counter==totalbytetorecv)
                                        {
                                                return true;
                                        }
                                        else
                                        {
                                                return false;
                                        }
							
                            }
		}//while(true)
		
	}//public boolean Read_Reply( byte[] recv_byte,int totalbytetorecv)
	
	
}// public class PrinterProject end
