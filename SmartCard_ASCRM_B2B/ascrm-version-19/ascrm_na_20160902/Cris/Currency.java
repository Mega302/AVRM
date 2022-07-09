
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



public class Currency
{

         
         //////////////////////////////////////////////////////////////////////////////////////////////////////
	 
         private  static   final String MEGAJNISOFILENAME ="libMegaAscrmNoteAcceptorAPI.so";

         /////////////////////////////////////////////////////////////////////////////////////////////////////

         private synchronized native  boolean SetupNoteAcceptorLogFile(  String  Logfile,
                                                                         int     LogLevel,
                                                                         String  DeviceId,
                                                                         String  LogdllPath
                                                                      );

         ///////////////////////////////////////////////////////////////////////////////////////////////////////

         /////////////////////////////////////////////////////////////////////////////////////////////////////

         public  synchronized native   int ConnectDevice( int PortId, 
                                                          int EscrowClearanceMode,
                                                          int Timeout
                                                        );

         public  synchronized native   int DisConnectDevice(int Timeout);

         public  synchronized native   int DeviceStatus(int Timeout);

         public  synchronized native   int defaultCommit(int Timeout);

         public  synchronized native   int defaultCancel(int Timeout);

         public  synchronized native   boolean AcceptNotes(int Amount,int Timeout);

         public  synchronized native   int  GetAcceptedAmount(byte[] AcptdAmt);

         public  synchronized native   int  StackAcceptedNotes(int Timeout);

         public  synchronized native   int  ReturnAcceptedNotes(int Timeout);

         public  synchronized native   int  IsNoteRemoved(int Timeout);

         public  synchronized native   int  ClearJammedNotes(int EscrowClearanceMode,int Timeout);

         public  synchronized native   int  GetValidNote(int Denom,int Timeout);

         public  synchronized native   int  AcceptCurrentNote(int Denom,int Timeout);

         public  synchronized native   int  ReturnCurrentNote(int Timeout);

         public  synchronized native   int  EnableTheseDenominations(int DenomMask,int Timeout);

         public  synchronized native   int  HoldCurrentNote(int Timeout);

         private synchronized native   void WriteNativeLog(String message);

         //////////////////////////////////////////////////////////////////////////////////////////////////////////

         private String DeviceId;

         private String LogFilePath;

         private int LogFileMode;

         private String Logdllpath;

         ///////////////////////////////////////////////////////////////////////////////////////////
         
         //Private Default constructor
         private Currency()
         {


         }//private Currency() end

         /////////////////////////////////////////////////////////////////////////////////////////////

         //Default constructor
         public Currency(int NoteAcceptordeviceid)
         {
                
                if(NoteAcceptordeviceid <= 0)
                {
                    throw new IllegalArgumentException("Must be postive note acceptor device id");
                }

                int counter=1;
 
                ///////////////////////////////////////////////////////////////////////////////////////////////////////

                this.DeviceId=null;

                this.LogFilePath=null;

                this.LogFileMode=-1;

                this.Logdllpath=null;

                //////////////////////////////////////////////////////////////////////////////////////////////////////
                
                //Setup Logfilepath,Logfilemode,device id

                //1.Setup Deviceid

                String DeviceidStr=String.format("BNA%d",NoteAcceptordeviceid);

                /////////////////////////////////////////////////////////////////////////////////////////////////////
               
               //2.Setup Logfilepath
               String LogfileNameWithPath = AscrmLog.GetLogFileNameWithPath();
                  
               /////////////////////////////////////////////////////////////////////////////////////////////////////
               
               //3.Setup Logfilemode
               int LogLevelMode = -1;
              
               String currencydevtype=String.format("Note_Acceptor_%d_Log_Level",NoteAcceptordeviceid );

	       int mode=AscrmLog.GetIniIntValue(currencydevtype);

               //System.out.println("[Mega Designs Pvt. Ltd.] Device Id: "+currencydevtype);

               //System.out.println("[Mega Designs Pvt. Ltd.] LogLevel Mode: "+mode);

               if( mode>=0 )
               {
                    LogLevelMode= mode;
                  
               }
               else
               {
                    LogLevelMode = 0;
               }

               //System.out.println("[Mega Designs Pvt. Ltd.] Class LogLevel Mode: "+LogLevelMode);
              
              //////////////////////////////////////////////////////////////////////////////////

              String LogdllPath =AscrmLog.GetLogDllPath();

              ///////////////////////////////////////////////////////////////////////////////////

               this.DeviceId=DeviceidStr;

               this.LogFilePath=LogfileNameWithPath;

               this.LogFileMode=LogLevelMode;

               this.Logdllpath=LogdllPath;

              
              ///////////////////////////////////////////////////////////////////////////////////

               
               SetupNoteAcceptorLogFile( LogfileNameWithPath,
                                         LogLevelMode,
                                         DeviceidStr,
                                         LogdllPath
                                       );
              
              //////////////////////////////////////////////////////////////////////////////////


            
         }//public Currency()

         
         ////////////////////////////////////////////////////////////////////////////////////////////////

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

                  this.WriteNativeLog(logmessage);
                  return;

		
	  }//public synchronized void WriteLog() end

          
         ///////////////////////////////////////////////////////////////////////////////////////////
 
          public synchronized static String GetDateAndTimeStamp()
	  {

		 SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		 Date date = new Date();
		 return ("["+dateFormat.format(date)+"]"); //2013/10/15 16:16:39 


	  }//public synchronized static  int GetDateAndTimeStamp() end


         ///////////////////////////////////////////////////////////////////////////////////////////

         static
	 { 

                      int rtcode=0,counter=1;

		      try 
		      {

                                 //System.out.println("[Mega Designs Pvt. Ltd.] Going to load libMegaAscrmNoteAcceptorAPI.so from java libpath.");
 
				 System.loadLibrary("MegaAscrmNoteAcceptorAPI");

				 //System.load("/home/user/dist/lib/libMegaAscrmNoteAcceptorAPI.so");


		      }//try end
		      catch(UnsatisfiedLinkError excp)
		      {

                                 //System.out.println("[Mega Designs Pvt. Ltd.] Unable to load libMegaAscrmNoteAcceptorAPI.so from java libpath.");

                                 //System.out.println("[Mega Designs Pvt. Ltd.] So going to load libMegaAscrmNoteAcceptorAPI.so from AscrmApi.jar file.");

				 if( 1==loadSOFromJar() )
                                 {


                                     //System.out.println("[Mega Designs Pvt. Ltd.] Before Create Logfile.");

                                     if( false == AscrmLog.IsAlreadyCreatedLogFile() )
                                     {
                                      
		                             rtcode = AscrmLog.CreateLogFile();

		                             if( 1 == rtcode )
		                             {
		                                //System.out.println("[Mega Designs Pvt. Ltd. NoteAcceptor] Logfile created.");
		                             }
		                             else if( 2 == rtcode )
		                             {
		                                //System.out.println("[Mega Designs Pvt. Ltd. NoteAcceptor] Logfile already created.");
		                             }
		                             else if( -1 == rtcode )
		                             {
		                                System.out.println("[Mega Designs Pvt. Ltd. NoteAcceptor] Logfile already created got exception.");
		                             }
                                     }
                                     else
                                     {
                                           //System.out.println("[Mega Designs Pvt. Ltd.] File is already created.");


                                     }
                                     //System.out.println("[Mega Designs Pvt. Ltd.] After Create Logfile.");
                                 }
                                 else
                                 {
                                     System.out.println("[Mega Designs Pvt. Ltd.] load so failed.");
                                 } 

				 //excp.printStackTrace();



		      }//catch(UnsatisfiedLinkError excp) end    


	 }//static block end here
	 
 
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	 //Load libMegaAscrmNoteAcceptorAPI.so from jar
	 private static int loadSOFromJar() 
	 {

		 try
                 {

                                InputStream fis=null;
                                URL res=null;
                                File dll=null;
                                FileOutputStream fos = null;

                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Going to delete previous libMegaAscrmNoteAcceptorAPI.so from system temp directory.");

				//Step 1: Delete old dll file from /tmp folder
				Multipliefiledelete("/tmp","libMegaAscrmNoteAcceptorAPI");

                                //System.out.println("[Mega Designs Pvt. Ltd.] After delete previous libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
			    
				/* Get DLL from JAR file */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Load libMegaAscrmNoteAcceptorAPI.so from AscrmApi jar.");

                                try
                                {
				       
                                       res = Currency.class.getResource(MEGAJNISOFILENAME);

                                }
                                catch(NullPointerException e)
                                {
                                      //System.out.println("[Mega Designs Pvt. Ltd.] Get NullPointerException when create resource from AscrmApi jar.");
                                      return 0;

                                }
 
                                try
                                {
				        //InputStream fis = res.openStream();
                                        fis = res.openStream();

                                }
                                catch(IOException e)
                                {

                                     //System.out.println("[Mega Designs Pvt. Ltd.] Get IOException when create stream from AscrmApi jar.");
                                     return 0;

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Load libMegaAscrmNoteAcceptorAPI.so from AscrmApi jar.");

				int SoFileLength=0;

				//Get SO File Size
                                SoFileLength=fis.available();
			        //System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmNoteAcceptorAPI.so file size = "+SoFileLength+" bytes");

				/*Define the destination file*/
				/*createTempFile(String prefix, String suffix)(for temp file name and its extension)*/
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Create temp libMegaAscrmNoteAcceptorAPI.so ");

                                try
                                {
				        
                                        dll = File.createTempFile("libMegaAscrmNoteAcceptorAPI",
".so");

                                }
                                catch (IllegalArgumentException e)
                                {
                                        System.out.println("[Mega Designs Pvt. Ltd.] Get IllegalArgumentException to create temp libMegaAscrmNoteAcceptorAPI.so ");
                                        return 0;
                                       
                                }
                                catch (IOException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd.] Get IOException to create temp libMegaAscrmNoteAcceptorAPI.so ");
                                       return 0;
                                       

                                }
                                catch (SecurityException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException to create temp libMegaAscrmNoteAcceptorAPI.so ");
                                       return 0;

                                }
                                

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Create temp libMegaAscrmNoteAcceptorAPI.so ");

				/* Open the destination file */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create stream from original libMegaAscrmNoteAcceptorAPI.so ");
                                try
                                {
				       //FileOutputStream fos = new FileOutputStream(dll);
                                       fos = new FileOutputStream(dll);

                                }
                                catch(FileNotFoundException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd.] Get FileNotFoundException to create stream from AscrmApi.jar. ");
                                       return 0;

                                } 
                                catch( SecurityException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException to create stream from AscrmApi.jar. ");
                                       return 0;

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create stream from original libMegaAscrmNoteAcceptorAPI.so ");

				/* Copy the DLL from the JAR to the filesystem */
				byte[] array = new byte[SoFileLength];

			       /*Reads some number of bytes from the input stream and 
				 stores them into the buffer array.This method blocks 
				 until input data is available, end of file is detected
				 Returns:the total number of bytes read into the buffer, or -1 
				 is there is no more data because the end of the stream has been reached.
			       */

                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create copy original libMegaAscrmNoteAcceptorAPI.so to temp libMegaAscrmNoteAcceptorAPI.so .");
				for(int i=fis.read(array);i!=-1;i=fis.read(array)) 
				{
		                            try
		                            {
						  fos.write(array,0,i);
		                            }
		                            catch(IOException e)
		                            {
		                                  //System.out.println("[Mega Designs Pvt. Ltd.] Get IOException when write stream libMegaAscrmNoteAcceptorAPI.so to temp libMegaAscrmNoteAcceptorAPI.so .");
		                                  return 0;

		                            }

				}

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create copy original libMegaAscrmNoteAcceptorAPI.so to temp libMegaAscrmNoteAcceptorAPI.so .");
				/* Close all streams */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Going to close all input output stream .");
                                try
                                {
				      fos.close();
                                }
                                catch(IOException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd.] Get IOException close all output stream .");
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

				/* Load the DLL from the filesystem */
				//getAbsolutePath to find the location of temporary file.
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before going to load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
                                try
                                {
				       System.load(dll.getAbsolutePath());
                                }
                                catch(SecurityException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
                                      return 0;
                                }
                                catch(UnsatisfiedLinkError e) 
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd.] Get UnsatisfiedLinkError load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
                                      return 0;
    
                                }
                                catch(NullPointerException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd.] Get NullPointerException load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
                                      return 0;
    

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After load temp libMegaAscrmNoteAcceptorAPI.so from system temp directory.");
                                
                                //System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmNoteAcceptorAPI.so load success .");

                                return 1;

		    }
		    catch(Throwable e)
		    {
                        
                        System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmNoteAcceptorAPI.so load failed! .");
		        //e.printStackTrace();
                        return 0;
		    }
	       

	 }//loadSOFromJar() end here
	 
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
