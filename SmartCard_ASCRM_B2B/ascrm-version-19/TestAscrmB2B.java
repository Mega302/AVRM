import Cris.*;
import java.util.Scanner;
import java.util.Arrays;

class TestAscrmB2B{

	public static int MAX_NMBR_OF_NOTE = 15; //Max Cash

	public static int WAIT_TIME_FOR_DISPENSED=6000;

	public static void main(String args[]){
		TestAscrm();
		return;
	} //End main


	public static int GetDenomValue(int notecode){

					       //System.out.println("[GetDenomValue()] Note Code : "+notecode); 

		                               int denomvalue=0;

		                         	switch(notecode)
		                               {

		                                  case 0: 
							 //System.out.println("[GetDenomValue()] Rs 5 Accepted "); 
		                                         denomvalue= 5;
		                                         break;
						      
		                                  case 1:
							 //System.out.println("[GetDenomValue()] Rs 10 Accepted "); 
		                                         denomvalue= 10;
		                                         break;

		                                  case 2:
							 //System.out.println("[GetDenomValue()] Rs 20 Accepted "); 
		                                         denomvalue= 20;
		                                         break;

		                                  case 3:
						         //System.out.println("[GetDenomValue()] Rs 50 Accepted "); 
		                                         denomvalue= 50;
							 break;
		                                        
		                                  case 4:
						         //System.out.println("[GetDenomValue()] Rs 100 Accepted "); 
		                                         denomvalue= 100;
		                                         break;

		                                  case 5:
							 //System.out.println("[GetDenomValue()] Rs 500 Accepted "); 
		                                         denomvalue= 500;
		                                         break;

						   case 6:
							 //System.out.println("[GetDenomValue()] Rs 200 Accepted "); 
		                                         denomvalue= 200;
		                                         break;


						   default:
		                                         denomvalue= 0;
		                                         break;

		                               };

		                              return denomvalue;


	}//int GetDenomValue(int notecode) end

	static void TestAscrm() {

		   String UserinputString=null;

		   Currency CurrencyObj=new Currency(1);

		   int counter=0,ret=0,rtcode=0,
                       UserAmountInput = 0,
                       prevfare=0,row=0 ,
                       col=0,
                       fare1=0,transtime=60,maxcash=0,maxcoin=0;

		   boolean acptfarertcode=false;
                   
                   double diff=0;

		   byte[] arry = new byte[MAX_NMBR_OF_NOTE];
                  
                   int EscrowMode=0;
                  
                   for( row=0;row<MAX_NMBR_OF_NOTE;row++)
                   {
                       arry[row]=0;
                        for( row=0;row<MAX_NMBR_OF_NOTE;row++)
		          {
		                    arry[row]=-1;
		               
		          }//++for end
                   }//for end
        
                   int portno=0;
		  										  
		   Scanner scanner = new Scanner(System.in);
                                                  
                   UserinputString = null;
                   

		   System.out.println("[TestAscrm()] ENTER COM PORT NUMBER[0-255]: ");
		                             
                   UserinputString = null;
                                                  
                   UserinputString = scanner.next();
									   
	           portno = Integer.parseInt(UserinputString);


		   System.out.println("[TestAscrm()] ENTER ESCROW MODE[1: Accept Note 2: Return Note Any Other No No Do Nothing]: ");
												                               
                   UserinputString = null;
                                                  
                   UserinputString = scanner.next();
									   
	           EscrowMode = Integer.parseInt(UserinputString);

                   System.out.println("[TestAscrm()] NOW GOING TO ACTIVATE DEVICE" ); 

		   ret =  CurrencyObj.ConnectDevice(portno,EscrowMode,5000);

		   System.out.println("[TestAscrm()] ACTIVATE DEV STATUS: "+ret ); 
                  
		   if(-7==ret)
		   {
		       
                        System.out.println("[TestAscrm()] ACTIVATE DEV STATUS: %d thats why system cannot run now "+ret ); 

                        return;

		  }
                  else if( 2 == ret ) //activated successfully but few notes in escrow
                  {

                       System.out.println("\n[TestAscrm()] Press 1 for accept escrow notes ,\n 2 for return escrow notes and press any other key to go for system default choice which is accept escrow notes");

                       char ch=0;

		       //ch=getchar();

		       switch(ch) {
                              case '1':// defaultCommit(1000); 
                                        break;

                              case '2': //defaultCancel(1000);
                                        break;

                              default : 
                                        break;
			      
		       }//switch end

                       
                  }//else if end
		  
                  for( ;; ){
			

                         acptfarertcode=false;

		         prevfare=0;
			   
			 System.out.println("\n\n\n\n\n\n[TestAscrm()] Press any key to continue or Press 1 for exit : ");

			 System.out.println("Press \"ENTER\" to continue...");
			 try {
				int read = System.in.read(new byte[2]);
			 } catch (java.io.IOException e) {
				System.out.println("Programme will exit");
				break;
		         }//try end
                           
                          ret = CurrencyObj.DeviceStatus(0);
                          
                          System.out.println("[TestAscrm()] Before And Device Status Anding Code : "+ret); 
                           
                          ret = (ret&0x02)>>1;
                           
                          System.out.println("[TestAscrm()] After And Device Status Code : "+ret); 
                          
                          if( 0 ==  ret )
		          {
		                    System.out.println("[TestAscrm()] Press any key to continue: ");
				    try {
					int read = System.in.read(new byte[2]);
				    } catch (java.io.IOException e) {
					System.out.println("Programme will exit");
				        break;
			            }//catch end
                                    continue;
		          }//if end

                          System.out.println("[TestAscrm()] Both Device Status Code: "+ret); 
                          
                          fare1=0;

			  System.out.println("[TestAscrm()] ENTER FARE: ");
			                    
                          UserinputString = null;
                                                  
                          UserinputString = scanner.next();
									   
	                  fare1 = Integer.parseInt(UserinputString);


                          System.out.println("[TestAscrm()] ENTER FARE TIME: ");
									   
			  UserinputString = scanner.next();
												  
			  transtime = Integer.parseInt(UserinputString);


                          maxcash=100;
                           
                          maxcoin=0;
			   
                          //++Clear Array
                          for( row=0;row<MAX_NMBR_OF_NOTE;row++)
		          {
		                    arry[row]=-1;
		               
		          }//++for end
		
                          int DenomMask = 0x00;

			  System.out.println("[TestAscrm()] ENTER Denom Mask: ");
									   
			  UserinputString = scanner.next();

			  DenomMask = Integer.parseInt(UserinputString);
			   
                          System.out.println("[TestAscrm()] Denom Mask Value: "+DenomMask  ); 

			  CurrencyObj.EnableTheseDenominations( DenomMask,0);
                                                  
			  acptfarertcode = false;

	                  acptfarertcode=CurrencyObj.AcceptNotes(fare1,transtime*1000);

			  if( false == acptfarertcode)
			  {
			      System.out.println("[TestAscrm()]) acceptFare() return false again try!! ");
			      
                              continue;

			  }  
                          else if( false == acptfarertcode)
			  {
			      System.out.println("[TestAscrm()]) acceptFare() return true.");
			      
			  }  
			  
                          rtcode = 0;
			  
                          UserAmountInput = 0;
			  
                          row=0;col=0;
			      
			  prevfare = 0;

			  long start=0,end=0;

		          start = System.nanoTime();

			  while(true)
			  {   

			   	    end = System.nanoTime();

				    diff = (double) ( (end-start)/1000000000 );
				    
                                    if(diff>=transtime)
				    {
                                               

					       System.out.println("[TestAscrm()] >>  Transtimeout by system"); 

					        for( row=0;row<MAX_NMBR_OF_NOTE;row++)
					        {
						     arry[row]=-1;
					       
					        }//++for end 

					       rtcode = CurrencyObj.GetAcceptedAmount(arry);

                                               System.out.println("[TestAscrm()]getDenomination Return code: "+rtcode); 
                                               
                                               UserAmountInput=0; 

					       for( row=0;row<MAX_NMBR_OF_NOTE;row++)
					       {
						    UserAmountInput+=GetDenomValue(arry[row]); 
                                                 
					       }//for end

                                               
                                               System.out.println("[TestAscrm()] >> User Given Currency: "+UserAmountInput);

					       if(fare1==UserAmountInput)
                                               {
					            System.out.println("[TestAscrm()] >>  System now commit your transaction");
                                                    CurrencyObj.StackAcceptedNotes(6000);

					       }
					       else
                                               {
					           System.out.println("[TestAscrm()] >> System now cancel your session");

                                                   CurrencyObj.ReturnAcceptedNotes(6000);

                                                   System.out.println("[TestAscrm()] >>  Before check customer take note from outlet");

                                                   if(UserAmountInput > 0 )
                                                   {
					              CurrencyObj.IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                                   }

                                                    System.out.println("[TestAscrm()] >>  After check customer take note from outlet");
                                               }
                                               
					       break;


				    }//if(diff>=transtime)

                                    rtcode =-1;

				    for( row=0;row<MAX_NMBR_OF_NOTE;row++)
				    {
				            arry[row]=-1;
				       
				     }//++for end

				    rtcode = CurrencyObj.GetAcceptedAmount(arry);
			           
                                    //Accepting State return code 1
				    if(1==rtcode)
				    {
                                              
                                              UserAmountInput=0;
					      
                                              for( row=0;row<MAX_NMBR_OF_NOTE;row++)
					      {
						UserAmountInput+=GetDenomValue(arry[row]); 
					      }
                                              
					      
					      if(UserAmountInput>prevfare)
					      {
		                                   System.out.println("[TestAscrm()] >> User Given Fare = "+UserAmountInput); 
		                                      
                                                    prevfare=UserAmountInput;

                                              }

					     
                                               
                                    }//if(1==rtcode) end


				    //Exact fare accepted return code 2
				    if(2==rtcode)
				    {
                                               
                                               System.out.println("\n =========================================================================");            
                                               System.out.println("[TestAscrm()] getDenomination return code: "+rtcode); 
		                               System.out.println("\n =========================================================================");      
					       System.out.println("[TestAscrm()] >> ACCEPTED FARE GIVEN");  
					       
                                               CurrencyObj.StackAcceptedNotes(6000);
                                              
					       break;

                                    }//if(2==rtcode)



				    //Transactions Timeout. return code 0
				    if (0==rtcode)
				    { 
                                               System.out.println("\n =========================================================================");      
                                               System.out.println("[TestAscrm()] getDenomination return code: "+rtcode); 
		                               System.out.println("\n =========================================================================");      
					       System.out.println("[TestAscrm()] >>  Transtimeout by user");  

                                               UserAmountInput=0;
					      
		                               for( row=0;row<MAX_NMBR_OF_NOTE;row++)
					       {
					           
						        UserAmountInput+=GetDenomValue(arry[row]); 
                                                  
					       }//for end
                                              
                                               CurrencyObj.ReturnAcceptedNotes(6000);

                                               if(UserAmountInput > 0 )
                                               {
					            CurrencyObj.IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                               }

					       break;    
                                             
                                             
         
				    }//if (0==rtcode)

                                    
                                    // More qty return code 3
				    if( 3==rtcode )
				    { 

                                               System.out.println("\n =========================================================================");      
                                               System.out.println("[TestAscrm()] getDenomination return code ="+rtcode); 
					       System.out.println("\n =========================================================================");      

					       System.out.println("[TestAscrm()] >> More note and coin inserted its time for cancel by system");            

                                               UserAmountInput=0;
					      
		                               for( row=0;row<MAX_NMBR_OF_NOTE;row++)
					       {
						        UserAmountInput+=GetDenomValue(arry[row]); 
                                                 
					       }

                                               CurrencyObj.ReturnAcceptedNotes(6000);

                                               if(UserAmountInput > 0 )
                                               {
                                                   CurrencyObj.IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
                                               }
					       break;


                                              
                                    }//if( 3==rtcode )
				   
				 
			  } // End: while loop

                         
		   } // End:  for loop




	}//TestAscrm() end


}
