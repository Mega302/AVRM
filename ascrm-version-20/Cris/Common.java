package Cris;

import java.io.FileOutputStream;
import java.io.OutputStream;
import java.util.Properties;
import java.io.File;
import java.io.InputStream;
import java.io.FileInputStream;
import java.lang.SecurityException;
import java.lang.NullPointerException;
import java.io.FileNotFoundException;
import java.util.jar.Manifest;
import java.util.jar.Attributes;
import java.net.URL;

public final class Common{

       public static int MEGADESIGNSPVTLTD=1;
       private static int CurrentLogLevel=0;
       
       //++All Log Modes
       public static final int OFF  =40;
	   public static final int TRACE=41;
	   public static final int DEBUG=42;
	   public static final int INFO =43;
	   public static final int WARN =44;
	   public static final int ERROR=45;
	   public static final int FATAL=46;
	   public static final int ALL  =47;
	   
	   //++All Common Return Codes
       public static final int OPERATION_TIMEOUT_OCCURED=18;
	   public static final int DEVICE_NOT_YET_CONNECTED=20;
	   public static final int DEVICE_ALREADY_CONNECTED=20;
	   public static final int COMMUNICATION_FAILURE=28;
	   public static final int OTHER_ERROR=31;

       
       //LogLevel
       //ALL   = 47
       //TRACE = 41
       //DEBUG = 42
       //INFO  = 43
       //WARN  = 44
       //ERROR = 45
       //FATAL = 46
       //OFF   = 40     
       
       public static int LogLevel=47; //Default Log Level
             
       public final static String GetJarVersion() {

               try{
						Class clazz = AscrmLog.class;
						String className = clazz.getSimpleName() + ".class";
						String classPath = clazz.getResource(className).toString();
						if (!classPath.startsWith("jar")) {
							 //Class not from JAR
							 System.out.println("[GetJarVersion()] Unable to get Specification Version");
							 return null;
						}//if end
						String manifestPath = classPath.substring(0, classPath.lastIndexOf("!") + 1) + "/META-INF/MANIFEST.MF";
						Manifest manifest = new Manifest(new URL(manifestPath).openStream());
						Attributes attr = manifest.getMainAttributes();
						String SpecificationTitle   = attr.getValue("Specification-Title");
						String SpecificationVendor  = attr.getValue("Specification-Vendor");
						String SpecificationVersion = attr.getValue("Specification-Version");
						return "JarInformation: "+SpecificationTitle+" "+SpecificationVendor+" "+SpecificationVersion;

               }catch(java.net.MalformedURLException ex){
                      System.out.println("[GetJarVersion()] MalformedURLException");
                      return null;

               }catch(java.io.IOException ex){
                       System.out.println("[GetJarVersion()] IOException");
                       return null;
               }
   

    }//public int GetJarVersion() ends

       public final synchronized int GetVendorId(){
		   return MEGADESIGNSPVTLTD;
	   }//public int GetVendorId() 
	   
	   public final synchronized String GetAVRMApiVersion(){
		   return Common.GetJarVersion();
	   }//public int GetAVRMApiVersion() 
	   
	   public final synchronized int SetLoggingLevel(int LogLevel){
		      
		      Common.LogLevel = LogLevel;
		      AscrmLog.SetLogLevel(LogLevel);
		      return 0; //++Success
		      
	   }//public int SetLoggingLevel() 
	   
	   public final static synchronized int GetLoggingLevelV2(){
		     return Common.LogLevel;
	   } //public int GetLoggingLevel() end
	   
	   public final synchronized int GetLoggingLevel(){
		     return Common.LogLevel;
	   } //public int GetLoggingLevel() end
	   
}//++public class Common end
