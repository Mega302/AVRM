
#include "readini.h"

#ifdef READINI_MAIN


int main(void) 
{

	  //printf("\n[Main()] Note Acceptor Port Number =%d\n.",GetNoteAcceptorPortNo());
	  ////printf("\n[Main()] CCTALK Port number =%d\n.",GetCCTALKPortNo());
	  //char logfilepath[FILENAME_MAX];
	  //memset( logfilepath,'\0',FILENAME_MAX);
	  //GetLogFilePath(logfilepath);
	  ////printf("\n[Main()] logfilepath =%s\n.",logfilepath);
	  
	  return 0;  


}

#endif

//////////////////////////////////Start:Upper Level API Interface///////////////////////////////////////////////////////////


int GetLogFileMode(char *logfilepath)
{


		     if(NULL==logfilepath)
		     return READ_INI_FAIL;
		     int rtcode=-1;
		     int intvalue=-1;
		     char IniAbsoluteFilePath[FILENAME_MAX];
		     
		     rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
		     if(READ_INI_FAIL==rtcode)
		     return READ_INI_FAIL;

		     char KeyValue[FILENAME_MAX];
		     memset(KeyValue,'\0',FILENAME_MAX);
		     
		     rtcode=GetStringKeyValue(IniAbsoluteFilePath,"ASCRM_SMARTCARD_LOGMODE","ENABLE",KeyValue);
		     
		     if(0==rtcode)
		     {
			       #ifdef READINI_DEBUG
			       //printf("\n[GetLogFileMod()] No Value Return.");
			       #endif
			       return READ_INI_FAIL;
		     }
		     
		     else
		     {
			       //Copy Logfilepath
			       int i=0;

			       for(;'\0'!=KeyValue[i];i++)
                               {
			          logfilepath[i]=KeyValue[i];
                               }

			       logfilepath[i]='\0';

		               ////printf("\n[GetLogFileMod()] Logfilepath: %s .",logfilepath);

			       return READ_INI_SUCCESS;


		     }



}//GetLogFileMode() end




int GetDSCUPortNo(void)
{

   
	   int rtcode=-1;
	   char IniAbsoluteFilePath[FILENAME_MAX];
	   rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
	   if(READ_INI_SUCCESS==rtcode)
	   {
		return(GetIntKeyValue(IniAbsoluteFilePath,"ASCRM_PORTS","TrapGatePort"));
	   } 
	   else
	   {
		return READ_INI_FAIL;
	   }


}




int GetTransTimeout()
{


	   int rtcode=-1;
	   char IniAbsoluteFilePath[FILENAME_MAX];
	   rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
	   if(READ_INI_SUCCESS==rtcode)
	   return(GetIntKeyValue(IniAbsoluteFilePath,"ASCRM_CONFIG","TransactionTimeout")); 
	   else
	   return READ_INI_FAIL;


}


int GetDSCUThreshold(void)
{

	   int rtcode=-1;
	   char IniAbsoluteFilePath[FILENAME_MAX];
	   rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
	   if(READ_INI_SUCCESS==rtcode)
	   return(GetIntKeyValue(IniAbsoluteFilePath,"ATVM_DSCU","DISPENSE_THRSHLD")); 
	   else
	   return READ_INI_FAIL;


}


int GetElectronicsLockTime(void)
{

	   int rtcode=-1;
	   char IniAbsoluteFilePath[FILENAME_MAX];
	   rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
	   if(READ_INI_SUCCESS==rtcode)
	   return(GetIntKeyValue(IniAbsoluteFilePath,"ASCRM_ELECTRONICS_LOCK_TIME","LOCKTIME")); 
	   else
	   return READ_INI_FAIL;


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

int GetB2BPortNo()
{
   int rtcode=-1;
   char IniAbsoluteFilePath[FILENAME_MAX];
   rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
   if(READ_INI_SUCCESS==rtcode)
   return(GetIntKeyValue(IniAbsoluteFilePath,"ASCRM_PORTS","B2BPort")); 
   else
   return READ_INI_FAIL;
}

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int GetNoteAcceptorPortNo()
{

	   int rtcode=-1;
	   char IniAbsoluteFilePath[FILENAME_MAX];
	   rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
	   if(READ_INI_SUCCESS==rtcode)
	   return(GetIntKeyValue(IniAbsoluteFilePath,"ASCRM_PORTS","NoteAcceptorPort")); 
	   else
	   return READ_INI_FAIL;


}//GetNoteAcceptorPortNo() end



int GetCCTALKPortNo()
{
   
	   int rtcode=-1;
	   char IniAbsoluteFilePath[FILENAME_MAX];
	   rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
	   if(READ_INI_SUCCESS==rtcode)
	   {
		return(GetIntKeyValue(IniAbsoluteFilePath,"ASCRM_PORTS","CCTALKPort")); 
		
	   }
	   else
	   {
		return READ_INI_FAIL;
	   }


}//GetCCTALKPortNo() end


int GetLogFilePath(char *logfilepath)
{

		     if(NULL==logfilepath)
		     return READ_INI_FAIL;
		     int rtcode=-1;
		     int intvalue=-1;
		     char IniAbsoluteFilePath[FILENAME_MAX];
		     
		     rtcode=GetIniAbsoluteFilePath("ascrm.ini",IniAbsoluteFilePath);
		     if(READ_INI_FAIL==rtcode)
		     return READ_INI_FAIL;

		     char KeyValue[FILENAME_MAX];
		     memset(KeyValue,'\0',FILENAME_MAX);
		     
		     rtcode=GetStringKeyValue(IniAbsoluteFilePath,"ASCRM_LOG_PATHS","LOGPATH",KeyValue);
		     
		     if(0==rtcode)
		     {
			       #ifdef READINI_DEBUG
			       //printf("\n[GetIntKeyValue()] No Value Return.");
			       #endif
			       return READ_INI_FAIL;
		     }
		     
		     else
		     {
			       //Copy Logfilepath
			       int i=0;
			       for(;'\0'!=KeyValue[i];i++)
			       logfilepath[i]=KeyValue[i];
			       logfilepath[i]='\0';
		               //printf("\n[GetIntKeyValue()] Logfilepath: %s .",logfilepath);
			       return READ_INI_SUCCESS;
		     }


}//GetLogFilePath() end

/////////////////////////////End:Upper Level API Interface////////////////////////////////////////////////////////


////////////////////////////////////////Start:Middle level/////////////////////////////////////////////////////////

static int GetIniAbsoluteFilePath(char *Inifilename,          //Inifilename (in)
                                  char *IniAbsoluteFilePath   //Inifile absolute path with current working directory (out)
                                 )
{

	   if( (0==strlen(Inifilename)) || (NULL==IniAbsoluteFilePath) )
	   return READ_INI_FAIL;
	 
	   char CurrentWorkingpath[FILENAME_MAX];
	   size_t size=FILENAME_MAX;

	   if(NULL==getcwd(CurrentWorkingpath, size))
	   {
		//printf("\n[GetIniAbsoluteFilePath()] Ini File current directory path get failed.!");
		return READ_INI_FAIL;

	   }
	   
	   char Inifilepath[FILENAME_MAX];
	   memset(IniAbsoluteFilePath,'\0',FILENAME_MAX);
	   
	   strcat(IniAbsoluteFilePath,CurrentWorkingpath);
	   strcat(IniAbsoluteFilePath,"/");
	   strcat(IniAbsoluteFilePath,Inifilename);
	   //printf("\n[GetIniAbsoluteFilePath()] Ini File path: %s .",IniAbsoluteFilePath);
	   return READ_INI_SUCCESS;


}

static int   GetStringKeyValue(char *IniFilePath,    //Ini file path (in)
                               char *SectionHeader,  //Ini file section name (in)
                               char *KeyName,        //Ini file section key name (in)
                               char *ReturnKeyStr    //return Ini file section keyvalue name (out)
                              ) 
{

		    if( (0==strlen(IniFilePath)) || (0==strlen(SectionHeader)) || (0==strlen(KeyName)) || (NULL==ReturnKeyStr) )
		    {
                             //printf("\n[GetIniAbsoluteFilePath()] function parameter not ok .");
			     return READ_INI_FAIL; //function parameter not ok
		    }

		    //Get Ini File Size
		    g_IniFileSize=IniFileSize(IniFilePath);
		    char IniBuffer[g_IniFileSize]; 
		    int rtcode=-1;
		    //open ini file
		    rtcode=IniOpenFile(IniFilePath,IniBuffer); 
		    if(READ_INI_SUCCESS==rtcode) 
		    {
			      #ifdef READINI_DEBUG
			      //printf("\n[GetStringKeyValue()] Ini File Buffer Created Successfully.");
			      #endif
		      
		    }
		    else
		    {
			      #ifdef READINI_DEBUG
			      //printf("\n[GetStringKeyValue()] Ini File Buffer created Failed.");
			      #endif  
			      return READ_INI_FAIL; 
		    } 
		    
		    //add '[' before and after of SectionHeader
		    g_SectionHeaderlength=strlen(SectionHeader);
		    char NewSectionHeader[g_SectionHeaderlength+3];
		    memset(NewSectionHeader,'\0',g_SectionHeaderlength+3);
		    strcat(NewSectionHeader,"[");
		    strcat(NewSectionHeader,SectionHeader);
		    strcat(NewSectionHeader,"]"); 
		  
		    //print Section headers
		    //printf("\n[GetStringKeyValue()] Input Section headers: %s .",NewSectionHeader);
		 
		    //Get Keyvalue 
		    rtcode=IniGetKeyValue(IniBuffer ,NewSectionHeader,KeyName,ReturnKeyStr);
		    if(READ_INI_SUCCESS==rtcode)
		    {
			  #ifdef READINI_DEBUG
			  //printf("\n[GetStringKeyValue()] Return String: %s .",ReturnKeyStr);
			  #endif 
			  return READ_INI_SUCCESS;
		    }
		    else
		    {
			 #ifdef READINI_DEBUG
			 //printf("\n[GetStringKeyValue()] No Value Return.");
			 #endif
			 return READ_INI_FAIL;

		     }



}//GetStringKeyValue() end


static int GetIntKeyValue( char *IniFilePath,    //Ini file path  (in)
                           char *SectionHeader,  //Ini file section name (in)
                           char *KeyName         //ini file keyname (in)
                         ) 
{

    
		     char KeyValue[FILENAME_MAX];
		     int rtcode=-1,intvalue=-1;;
		     rtcode=GetStringKeyValue(IniFilePath,SectionHeader,KeyName,KeyValue);
		     if(READ_INI_FAIL==rtcode)
		     {
			       #ifdef READINI_DEBUG
			       //printf("\n[GetIntKeyValue()] No Value Return.");
			       #endif
			       return READ_INI_FAIL;
		     }
		     
		     else
		     {
			       intvalue=atoi(KeyValue);
			       return intvalue;

		     }


}//GetIntKeyValue() end 


////////////////////////////////////////End:Middle level/////////////////////////////////////////////////////////


////////////////////////////////////////Start:Lower level/////////////////////////////////////////////////////////




static int IniOpenFile(char *IniFilePath,     //Ini file path (in)
		       char *ReturnIniBuffer  //Ini file buffer for store whole ini file (out)
		      )
{

		      if(0==strlen(IniFilePath) || (NULL== ReturnIniBuffer) )
		      {
			    return READ_INI_FAIL; //function parameter not ok
			     
		      }
		      else
		      {
			   #ifdef READINI_DEBUG 
			   //printf("\n[IniOpenFile()] File Path is not blank.");
			   #endif
		      }

                      FILE *fp=NULL;
		      char ch='\0';
                      int rtcode=-1,counter=0;
	              fp = fopen(IniFilePath,"r");
                      if( NULL == fp)
		      {
                            #ifdef READINI_DEBUG 
			    //printf("\n[IniOpenFile()] File open failed.");
			    #endif
			    return READ_INI_FAIL; 
			     
		      }
		      else
		      {
			   #ifdef READINI_DEBUG 
			   //printf("\n[IniOpenFile()] File open successed.");
			   #endif
		      }
		      do
		      {

			      ch = fgetc(fp);
                              if( ('\0'!= ch) || (EOF != ch) )
                              {
                                      ReturnIniBuffer[counter]=ch;
                                      counter++;

                              }
                              #ifdef READINI_DEBUG
                              if( ('\0'!= ch) || ( EOF != ch) )
                              {
                                  //printf("%c", ch);
                              }
                              #endif

			      if( 0 != feof(fp) )
			      { 
                                  //printf("\n[IniOpenFile()] End of file reached.");
				  break ;

			      }

			      
		      }while(ch != EOF);

                      //put null character in return string buffer
                      ReturnIniBuffer[counter]= '\0' ;
                      //printf("\n[IniOpenFile()] =======================================================");
                      //printf("\n[IniOpenFile()] %s .", ReturnIniBuffer);
                      //printf("\n[IniOpenFile()] =======================================================");


	              rtcode = fclose(fp);

                      if( 0 != rtcode)
		      {
                            #ifdef READINI_DEBUG 
			    //printf("\n[IniOpenFile()] File close failed.");
			    #endif
			    
	              }
		      else
		      {
			   #ifdef READINI_DEBUG 
			   //printf("\n[IniOpenFile()] File close successed.");
			   #endif
		      }
	      

                      return READ_INI_SUCCESS;



}//IniOpenFile() End here




static unsigned int IniFileSize(char *IniFilePath /*Ini file path (in)*/)
{


		   if(0==strlen(IniFilePath))
                   {
		            
                          return READ_INI_FAIL; //function parameter not ok

                   }
			       
		   int FileHwnd = open(IniFilePath,O_RDONLY);
		   if(-1==FileHwnd)
		   return 0;
		   else
		   {
		     #ifdef READINI_DEBUG 
		     //printf("\n[IniFileSize()] File Open Successfully");
		     #endif
		   }
		   struct stat FileInfo;
		   int Rtcode=fstat(FileHwnd, &FileInfo);  //On success, zero is returned
		   if(0!=Rtcode)
		   {
		       close(FileHwnd);  
		       return READ_INI_FAIL;
		   }
		   close(FileHwnd);  
		   return (FileInfo.st_size);


}//IniFileSize() end



//Return keyvalue under keyname under sectionheader in ini file
static int    IniGetKeyValue(char *IniBuffer ,        //ini file buffer which will be used for string parse (in)
                             char *Sectionname,       //ini file Section name (in)
                             char *SectionKeyname,    //ini file keyname (in)
                             char *ReturnKeyvalue     //return keyvalue of keyname under section (out)
                            )
{ 

         if( (0==strlen(IniBuffer)) || (0==strlen(Sectionname)) || (0==strlen(SectionKeyname)) || (NULL==ReturnKeyvalue) )
         {
             return READ_INI_FAIL; //function parameter not ok
         }
         
         char ch;
         #ifdef READINI_DEBUG
         //printf("\n[IniGetKeyValue()] Ini file Hole contents :%s",IniBuffer);
         //printf("\n[IniGetKeyValue()] Section name :%s .",Sectionname);
         //printf("\nIniGetKeyValue()] Section  key name:%s .",SectionKeyname);
         #endif
         
         char *FileArray=IniBuffer;
         int  SectionnameLength =strlen(Sectionname);
         int  SectionKeyLength  =strlen(SectionKeyname);
         int  FileArrayCounter=0,
              SectionNameCounter=0,
              SectionKeyCounter=0,
              KeyValueCounter=0,
              eualLength=0;
       

         //Get Section Header and stop after sectionheader
         for(;'\0'!=FileArray[FileArrayCounter];FileArrayCounter++)
         {

		   if(Sectionname[SectionNameCounter]==FileArray[FileArrayCounter])
		   { 
		      #ifdef READINI_DEBUG
		      ch=FileArray[FileArrayCounter];
		      //printf("\n[IniGetKeyValue()] Section Header Compared :%c",ch);
		      #endif
		 
		      eualLength++;
		      if(eualLength!=SectionnameLength)
		      {   
		        SectionNameCounter++;
		      }
		      else if(eualLength==SectionnameLength)
		      {
		           FileArrayCounter++;
		           break;
		      }

		   }

         } 
        
         if(eualLength!=SectionnameLength)
         {
             //printf("\n[IniGetKeyValue()] Section header not found.");
             return READ_INI_FAIL;

         }
         else
         {
           #ifdef READINI_DEBUG 
           //printf("\n[IniGetKeyValue()] Section header found.");
           #endif
         }

         //Check KeyName and stop before keyvalue
         eualLength=0;
         for(;'\0'!=FileArray[FileArrayCounter];FileArrayCounter++)
         {

		   if(SectionKeyname[SectionKeyCounter]==FileArray[FileArrayCounter])
		   { 
		      
			      eualLength++;
			      if(eualLength!=SectionKeyLength)
			      {   
				  SectionKeyCounter++;
			      }
			      else if(eualLength==SectionKeyLength)
			      {
				   //Stop Before Equal Sign
				   
				   #ifdef READINI_DEBUG
				   ch=FileArray[FileArrayCounter];
				   //printf("\n[IniGetKeyValue()] Character before equal sign :%c",ch);
				   #endif
			       
				   FileArrayCounter+=2;
				   
				   #ifdef READINI_DEBUG
				   ch=FileArray[FileArrayCounter];
				   //printf("\n[IniGetKeyValue()] charcter after equal sign :%c .",ch);
				   #endif
			      
				   break;
			      }
			      
			      #ifdef READINI_DEBUG      
			      ch=FileArray[FileArrayCounter];
			      //printf("\n[IniGetKeyValue()] Secttion KeyName Compared: %c",ch);
			      #endif                
			      
		   }

         }

         if(eualLength!=SectionKeyLength)
         {
                  //printf("\n[IniGetKeyValue()] Keyname found error!.");
                  return READ_INI_FAIL;

         }
         
         else
         {
		   #ifdef READINI_DEBUG 
		   //printf("\n[IniGetKeyValue()] Keyname found.");
		   #endif
          
         }

         //copy keyvalue into user assigned buffer(stop when space found)
         if(NULL==ReturnKeyvalue)
         {
               //printf("\n[IniGetKeyValue()] return buffer null found.");
               return 0;
         }

         for(;'\n'!=FileArray[FileArrayCounter];FileArrayCounter++,KeyValueCounter++)
         {
                 ReturnKeyvalue[KeyValueCounter]=FileArray[FileArrayCounter];
         }
         
         ReturnKeyvalue[KeyValueCounter]='\0'; 
         
         if(0!=KeyValueCounter)
         return READ_INI_SUCCESS; //success
         
         else
         return READ_INI_FAIL; //fail

      
}//IniGetKeyValue End here


/////////////////////////////////////////////End:Lower level/////////////////////////////////////////////////////////

