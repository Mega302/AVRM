package Cris;
import java.util.Properties;
import java.io.File;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.lang.SecurityException;
import java.lang.NullPointerException;
import java.net.URL;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.util.jar.Manifest;
import java.util.jar.Attributes;
import java.io.PrintWriter;

class AscrmLog
{


    private  static boolean g_AlreadyCreatedLogFile;

    private  static String g_LogFileFullPathAndName;

    private  static final String g_INIFILENAME="ascrm.ini";

    private  static Object Objectlock;

    private  static Object LogFileObjectlock;
 
    private  static Object LogdllAbsolutePathlock;

    private  static String MEGAJNISOFILENAME;

    private  static String LogdllAbsolutePath;


    static
    {
        
		g_AlreadyCreatedLogFile=false;

		g_LogFileFullPathAndName = null;

                MEGAJNISOFILENAME ="libMegaAscrmLogAPI.so";
                
                LogdllAbsolutePath=null;

                Objectlock = new Object();

                LogFileObjectlock = new Object();

                LogdllAbsolutePathlock = new Object();

	        //Load log dll
                loadSOFromJar();

                

    }//static block end


    public synchronized static String GetDateAndTimeStamp()
    {

         SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
	 Date date = new Date();
	 return ("["+dateFormat.format(date)+"]"); //2013/10/15 16:16:39 


    }//public synchronized static  int GetDateAndTimeStamp() end


    public synchronized static String GetLogDllPath()
    {

              String dllpath=null;

              synchronized(LogdllAbsolutePathlock)
              {
                 dllpath = LogdllAbsolutePath;

              }

              return dllpath;


    }//public synchronized static String GetLogDllPath() end

    private static String GetJarVersion(){

                try{
		        Class clazz = AscrmLog.class;
			String className = clazz.getSimpleName() + ".class";
			String classPath = clazz.getResource(className).toString();
			if (!classPath.startsWith("jar")) {
			     //Class not from JAR
		             System.out.println("[GetJarVersion()] Unable to get Specification Version");
			     return null;
			}
			String manifestPath = classPath.substring(0, classPath.lastIndexOf("!") + 1) + "/META-INF/MANIFEST.MF";
			Manifest manifest = new Manifest(new URL(manifestPath).openStream());
			Attributes attr = manifest.getMainAttributes();
			String SpecificationTitle = attr.getValue("Specification-Title");
		        String SpecificationVendor = attr.getValue("Specification-Vendor");
		        String SpecificationVersion = attr.getValue("Specification-Version");
		        return "JarInformation: "+SpecificationTitle+" "+SpecificationVendor+" "+SpecificationVersion;

               }catch(java.net.MalformedURLException ex){
                      System.out.println("[GetJarVersion()] MalformedURLException");
                      return null;

               }catch(java.io.IOException ex){
                       System.out.println("[GetJarVersion()] IOException");
                       return null;
                   
               }
   

    }//private int GetJarVersion() ends

    private static int WriteJarVersionInLog(File FileHandle,String Text){

           try{
		   PrintWriter pw = new PrintWriter(FileHandle);
		   pw.println(Text);
		   pw.close();
                   return 1;

           }catch(FileNotFoundException ex){
                 System.out.println("[WriteJarVersionInLog()] FileNotFoundException");
                 return 0;
                
           }
        
 

    }

    public synchronized static int CreateLogFile()
    {

              boolean logstatus;

              File file=null;

	      synchronized(Objectlock)
              {
                 logstatus = g_AlreadyCreatedLogFile;

              }

              if( false == logstatus )
              {

                      ///////////////////////////////////////////////////////////////////////////
                       
                      String LogFilePath=null;

		      try
                      {
			      Properties p = new Properties();
			      p.load(new FileInputStream(g_INIFILENAME));
			      //System.out.println(AscrmLog.GetDateAndTimeStamp()+" LogfilePath = " + p.getProperty("LOGPATH"));
                              LogFilePath = p.getProperty("LOGPATH") ; 
			     
			    
		      }
		      catch(FileNotFoundException ex)
                      {

                            System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] Got an FileNotFoundException");
 
                            return (-1);


                      }//catch(FileNotFoundException ex)
                      catch (IOException ex) 
                      {

		         System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] Got an ioexception");

                         return (-1);

                      }//catch (IOException ex) 
                      

                      ////////////////////////////////////////////////////////////////////////////
                      
                      //Mega_ascrm_Dt_yrmonday_Tm_hrminsec.log 

                      SimpleDateFormat currentdateformat = new SimpleDateFormat("yyyyMMdd");
                      
                      String currentdate = currentdateformat.format(new Date()); 

                      SimpleDateFormat currenttimeformat = new SimpleDateFormat("hhmmss");
                      
                      String currenttime= currenttimeformat.format(new Date()); 


                      //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] Date: "+currentdate);

                      //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] Time: "+currenttime);


                      String LogFileName=String.format("Mega_ascrm_Dt_%s_Tm_%s.log", currentdate,currenttime);

                      //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] FileName: "+LogFileName);

                      synchronized(LogFileObjectlock)
                      {
                      g_LogFileFullPathAndName=String.format("%s/%s",LogFilePath,LogFileName );

                      //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] LogfilePath: "+g_LogFileFullPathAndName);
                      }
                      
                      try 
                      {

                              synchronized(LogFileObjectlock)
                              {
                                 file = new File(g_LogFileFullPathAndName);
                              }

                              if ( false == file.exists() ) 
		              {

					  if( true ==  file.createNewFile() )
				          {

		                              //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] File Created Successfully");
                                              String JarVersion = GetJarVersion();
                                              if( null == JarVersion ){
                                                                                       
                                                    JarVersion="UnKnown_Jar_Version";
                                              }

                                              WriteJarVersionInLog(file,JarVersion);

                                              synchronized(Objectlock)
                                              {
		                                      //Runtime.getRuntime().exec("host -t a " + domain);
		                                      g_AlreadyCreatedLogFile=true;
                                              }

                                              return 1;

				          }
				          else
				          {
		                              System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] File Created failed");
                                              return 0;

				          }

			      }
			      else
		              {
		                  //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] Already file exist");

                                  return 2;

		              }
     
 
                      }
                      catch(NullPointerException ex)
                      {
                          System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] File name is empty");
                          return (-1);

                      }
		      catch(IOException ex)
                      {

                         System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] File create get ioexception");
                         return (-1);

                      }
                      catch(SecurityException ex)
                      {
                           System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] File create get security exception");
                           return (-1);

                      }
                      

                     /////////////////////////////////////////////////////////////////////////////////

                      

          }
          else
          {
              //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[CreateLogFile()] Already file created");
              return 0;

          }
          
		    

    }//public static int CreateLogFile() end


    /////////////////////////////////////////////////////////////////////////////////////////////

    public synchronized static boolean IsAlreadyCreatedLogFile()
    {
         
          boolean logstatus;

          synchronized(Objectlock)
          {

                 logstatus = g_AlreadyCreatedLogFile;


          }

          return  logstatus;
		
		
    }//public static boolean IsAlreadyCreatedLogFile() end

    /////////////////////////////////////////////////////////////////////////////////////////////

    public synchronized static String GetLogFileNameWithPath()
    {

           String LogFileNameWithPath=null;

           synchronized(LogFileObjectlock)
           {
               LogFileNameWithPath = g_LogFileFullPathAndName;

           }


           return LogFileNameWithPath;
		
		
    }//public static boolean IsAlreadyCreatedLogFile() end

    /////////////////////////////////////////////////////////////////////////////////////////////

    public synchronized static String GetIniStringValue(String Key)
    {
           
           String KeyValue=null;

           Properties p = new Properties();

           try
           {
		   p.load(new FileInputStream(g_INIFILENAME));
		   
		   KeyValue = p.getProperty(Key) ; 

                   //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[GetIniStringValue()] Key = " + Key+" KeyValue = " + KeyValue );

                   return KeyValue;

           }
           catch(FileNotFoundException ex)
           {

                    System.out.println(AscrmLog.GetDateAndTimeStamp()+"[GetIniStringValue()] Got an FileNotFoundException");
 
                    return null;


            }//catch(FileNotFoundException ex)
            catch (IOException ex) 
            {

		     System.out.println(AscrmLog.GetDateAndTimeStamp()+"[GetIniStringValue()] Got an ioexception");

                     return null;

            }//catch (IOException ex) 

           


    }//public synchronized static int GetIniStringValue(String Key) end


    ///////////////////////////////////////////////////////////////////////////////////////////////////////


    public synchronized static int GetIniIntValue(String Key)
    {
           

           String KeyValue=null;

           Properties p = new Properties();

           try
           {

		   p.load(new FileInputStream(g_INIFILENAME));
		   
		   KeyValue = p.getProperty(Key) ; 

		   //System.out.println(AscrmLog.GetDateAndTimeStamp()+"[GetIniIntValue()] Key = " + Key+" KeyValue = " + KeyValue );

                   
                   if(null != KeyValue)
                   {
                         return ( Integer.parseInt(KeyValue) );
                   }
                   else
                   {
                         return (-1);

                   }


           }
           catch(FileNotFoundException ex)
           {

                    System.out.println(AscrmLog.GetDateAndTimeStamp()+"[GetIniStringValue()] Got an FileNotFoundException");
 
                     return (-1);


           }//catch(FileNotFoundException ex)
           catch (IOException ex) 
           {

		     System.out.println(AscrmLog.GetDateAndTimeStamp()+"[GetIniStringValue()] Got an ioexception");

                     return (-1);

           }//catch (IOException ex) 


           

    }//public synchronized static int GetIniIntValue(String Key) end

    /////////////////////////////////////////////////////////////////////////////////////////////

    public static void main(String args[])
    {
        
         AscrmLog.CreateLogFile();

    }//public static void main(String args[]) end

    /////////////////////////////////////////////////////////////////////////////////////////////

    //Load libMegaAscrmLogAPI.so from jar
    private synchronized static int loadSOFromJar() 
    {

		 try
                 {

                                InputStream fis=null;
                                URL res=null;
                                File dll=null;
                                FileOutputStream fos = null;

                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Going to delete previous libMegaAscrmLogAPI.so from system temp directory.");

				//Step 1: Delete old dll file from /tmp folder
				Multipliefiledelete("/tmp","libMegaAscrmLogAPI");

                                //System.out.println("[Mega Designs Pvt. Ltd.] After delete previous libMegaAscrmLogAPI.so from system temp directory.");
			    
				/* Get DLL from JAR file */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Load libMegaAscrmLogAPI.so from AscrmApi jar.");

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

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Load libMegaAscrmLogAPI.so from AscrmApi jar.");

				int SoFileLength=0;

				//Get SO File Size
                                SoFileLength=fis.available();
			        //System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmLogAPI.so file size = "+SoFileLength+" bytes");

				/*Define the destination file*/
				/*createTempFile(String prefix, String suffix)(for temp file name and its extension)*/
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before Create temp libMegaAscrmLogAPI.so ");

                                try
                                {
				        
                                        dll = File.createTempFile("libMegaAscrmLogAPI",
".so");

                                }
                                catch (IllegalArgumentException e)
                                {
                                        System.out.println("[Mega Designs Pvt. Ltd.] Get IllegalArgumentException to create temp libMegaAscrmLogAPI.so ");
                                        return 0;
                                       
                                }
                                catch (IOException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd.] Get IOException to create temp libMegaAscrmLogAPI.so ");
                                       return 0;
                                       

                                }
                                catch (SecurityException e)
                                {

                                       System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException to create temp libMegaAscrmLogAPI.so ");
                                       return 0;

                                }
                                

                                //System.out.println("[Mega Designs Pvt. Ltd.] After Create temp libMegaAscrmLogAPI.so ");

				/* Open the destination file */
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create stream from original libMegaAscrmLogAPI.so ");
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

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create stream from original libMegaAscrmLogAPI.so ");

				/* Copy the DLL from the JAR to the filesystem */
				byte[] array = new byte[SoFileLength];

			       /*Reads some number of bytes from the input stream and 
				 stores them into the buffer array.This method blocks 
				 until input data is available, end of file is detected
				 Returns:the total number of bytes read into the buffer, or -1 
				 is there is no more data because the end of the stream has been reached.
			       */

                                //System.out.println("[Mega Designs Pvt. Ltd.] Before create copy original libMegaAscrmLogAPI.so to temp libMegaAscrmLogAPI.so .");
				for(int i=fis.read(array);i!=-1;i=fis.read(array)) 
				{
		                            try
		                            {
						  fos.write(array,0,i);
		                            }
		                            catch(IOException e)
		                            {
		                                  //System.out.println("[Mega Designs Pvt. Ltd.] Get IOException when write stream libMegaAscrmLogAPI.so to temp libMegaAscrmLogAPI.so .");
		                                  return 0;

		                            }

				}

                                //System.out.println("[Mega Designs Pvt. Ltd.] After create copy original libMegaAscrmLogAPI.so to temp libMegaAscrmLogAPI.so .");
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
                                //System.out.println("[Mega Designs Pvt. Ltd.] Before going to load temp libMegaAscrmLogAPI.so from system temp directory.");
                                try
                                {
				       System.load( dll.getAbsolutePath() );
                                }
                                catch(SecurityException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd.] Get SecurityException load temp libMegaAscrmLogAPI.so from system temp directory.");
                                      return 0;
                                }
                                catch(UnsatisfiedLinkError e) 
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd.] Get UnsatisfiedLinkError load temp libMegaAscrmLogAPI.so from system temp directory.");
                                      return 0;
    
                                }
                                catch(NullPointerException e)
                                {
                                      System.out.println("[Mega Designs Pvt. Ltd.] Get NullPointerException load temp libMegaAscrmLogAPI.so from system temp directory.");
                                      return 0;
    

                                }

                                //System.out.println("[Mega Designs Pvt. Ltd.] After load temp libMegaAscrmLogAPI.so from system temp directory.");
                                
                                //System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmLogAPI.so load success .");

                                synchronized(LogdllAbsolutePathlock)
                                {
                                    LogdllAbsolutePath=dll.getAbsolutePath();
                                }

                                return 1;

		    }
		    catch(Throwable e)
		    {
                        
                        System.out.println("[Mega Designs Pvt. Ltd.] libMegaAscrmLogAPI.so load failed! .");
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

	 
	 public synchronized static boolean delete(String fileName)
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


	 public synchronized static String[] Search(String Directory,final String FileNameFilter)
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


       /////////////////////////////////////////////////////////////////////////////////////////////


}//class AscrmLog end
