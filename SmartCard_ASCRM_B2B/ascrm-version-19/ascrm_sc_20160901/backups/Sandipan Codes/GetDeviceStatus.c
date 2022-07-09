
#include "GetDeviceStatus.h"

/*
* Function Name : GetDeviceStatus_c 
* Return Type   : char[10] byte Array where 
                   
                  char[0] = Execution Status of the API

                                0 = Operation Successful
                                1 = Communication Failure
                                2 = Operation timeout occurred
                                3 = Other Error

                  char[1] = RFID Reader Status

                                0 = Ready
                                1 = Not Ready
                                                  
                  char[2] = SAM Reader Status 

                                0 = Ready
                                1 = Not Ready
                                 
                  char[3] = Stacker Status 
                     
                                0 = Empty 
                                1 = Nearly Empty 
                                2 = Nearly Full 
                                3 = Full

                  char[4] = Apporx. Card count in stacker 
 
                  char[5] = Rejection Bin Status 
                         
                                0 = Empty 
                                1 = Nearly Empty
                                2 = Nearly Full 
                                3 = Full 
   
                  char[6] = Card Count in Rejection Bin
                              
                  char[7] = Channel Status

                                0 = Clear
                                1 = Blocked 
               
                  char[8] = Channel sensor status in 8 bit where each bit will indicate whether a sensor is blocked or not

                                0 = Clear
                                1 = Blocked 
                                Note : If no sensors is less than 8 higher significant bits will be filled with 0's.
                  
                  char[9] = Collection Bin Status
      
                                0 = Empty
                                1 = Nearly Empty
                                2 = Nearly Full 
                                3 = Full

                   char[10] = Card count in Collection Bin

                     
                                
* Parameters    : Name                           Type          Description 

                  ComponentId                    int           0 = All Component 
                                                               1 = Reader 
                                                               2 = Stacker 
                                                               3 = Rejection Bin
                                                               4 = Channel 
                                                               5 = Collection Bin

                                     
                  Timeout                        int           Time in Milliseconds the API will try to inform its intended 								       operation otherwise return timeout status. 

Note : When ComponentId is other than 0 API will fill revelent values for the request component leaving other component status 0; However execution status of the API is mandatory for any values of ComponentId.

*/


`
void GetDeviceStatus_c(int ComponentId,int timeout)
{

}
