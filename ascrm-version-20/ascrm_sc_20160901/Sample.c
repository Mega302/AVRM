#include <dlfcn.h>
#include <stdio.h>

int main()
{
			int rtcode = 0;
			void *lib_handle;
	 
			int (*fnptr)(int PortId, 
		             int ChannelClearanceMode, 
		             int Timeout,
		             char *LogdllPathstr,
		             char *LogFileName,
		             char *deviceid,
		             int fnLogfileMode)=NULL;

		

		//printf("\n[Java_Cris_SmartCard_JniConnectDevice()] dll path:%s",ObjectdllPath);

			lib_handle = dlopen("//root//Desktop//ascrm//ascrm_sc_20151129//libMegaAscrmSmartCardAPI.so", RTLD_LAZY);
		
			if (NULL==lib_handle)
			{
			      printf("\n [DrawCardManagementDevices()] Get so memory pointer failed");
			      //ShowMessage("[DrawCardManagementDevices()] Get so memory pointer failed", 1);
			      return 7; //failed
			}
			else
			{
				//success
				printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory pointer successfully done");
				//ShowMessage("[DrawCardManagementDevices()] Get so memory pointer successfully done", 1);
				fnptr = dlsym(lib_handle, "ConnectDevice_c");

				if (NULL == fnptr )  
				{
				       printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer failed");				
				       //ShowMessage("[DrawCardManagementDevices()] Get so memory function pointer failed", 1);
				       return 7;
				}
				else
				{ 
					printf("\n[Java_Cris_SmartCard_JniConnectDevice()] Get so memory function pointer get success");
					
					//Now Write Log through so file
					rtcode  = fnptr( 0,
							 0,
							 0,
							 NULL,
							 NULL,
							 NULL,
							 0
						       );
				 
					//printf("\n[Java_Cris_SmartCard_JniConnectDevice()] function call return code: %d",rtcode);

				        

					if(NULL!= lib_handle)
					{
				             dlclose(lib_handle);
					     fnptr=NULL;
				        }
		
					//newtFormDestroy(CardManagementForm1);
					//DrawCardManagement();
					
				
				}//else block end
		
			}//else( NULL == lib_handle ) end
			
}
