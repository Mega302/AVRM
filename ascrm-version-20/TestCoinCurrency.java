import Cris.*;
import java.util.Scanner;

public class TestCoinCurrency{

            public static void main(String[] args){
				
		         TestCoinCurrency.TestNewAscrm();
		         
		    }//public static void main(String[] args) end
		    
		    public static int GetDenomValue(int notecode) {


                                       int denomvalue=0;

                                       switch(notecode)
                                       {

											  case 1: 
													 denomvalue= 5;
													 break;
							  
											  case 2:
													 denomvalue= 10;
													 break;

											  case 3:
													 denomvalue= 20;
													 break;

											  case 4:
													 denomvalue= 50;
													 break;

											  case 5:
													 denomvalue= 100;
													 break;

											  case 6:
													 denomvalue= 500;
													 break;
                              
                                             case 7:
													 denomvalue= 200;
													 break;
										     
										     case 8:
													 denomvalue= 2000;
													 break;
													 	 
											 default:
													 denomvalue= 0;
													 break;

                                       };

                                      return denomvalue;


            }//public static int GetDenomValue(int notecode) end
            
			public static void TestNewAscrm() {


                              String UserinputString = null;
					          Currency CurrencyObj = new Currency();
					          
						      //++Log Test
						      
						      int devStatus1=0,
						          devStatus2=0;
						      
						      String deviceid="JCM";
						      //++strcpy(g_Deviceid,deviceid);
						      //++g_LogModeLevel = (int)ALL_LOG;
						      
						      int  MAX_NMBR_OF_NOTE =20,
						           MAX_NMBR_OF_COIN =20;
						      int  counter=0,ret=0,rtcode=0,
						           UserAmountInput = 0,
						           prevfare=0,row=0 ,
						           col=0,
						           fare1=0,transtime=60,maxcash=0,maxcoin=0;
						           
						       byte[][] arry= null;
						       arry = new byte[MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN][2] ;
						       
						       for( row=0;row< (MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN) ;row++) {
						            for( col=0;col<2;col++){
						                  arry[row][col]=0;
						            }//for end

						       }//for end
				
						       int PortId1=1,PortId2=0,PortId3=0;
                               int EscrowMode=1; //++ 0: Retain Note or Coin 1: Clear Note and Coin
						       
						       //ret =  CurrencyObj.ConnectDevice(0,1,0,1,EscrowMode,5000);
							   //System.out.println( "[TestAscrm()] ACTIVATE Note Acceptor STATUS: "+ret ); 
							   
							   //if( 0 != ret ){
						           //System.out.println("[TestAscrm()] ACTIVATE Note Acceptor Failed" );
						           //return; 
						       //}//else end
						       
						       ret =  0;
						       ret =  CurrencyObj.ConnectDevice(0,1,0,2,EscrowMode,5000);
							   System.out.println("[TestAscrm()] ACTIVATE Coin Acceptor STATUS: "+ret ); 
							   
							   if( 0 != ret ){
						           System.out.println("[TestAscrm()] ACTIVATE Coin Acceptor Failed" );
						           return; 
						       }//else end
						       
						       
						       /*
						       for( ;; ) {
			
                                                  
												  System.out.println("Press \"ENTER\" to continue...");
												  
												  try {
														
														int read = System.in.read(new byte[2]);
														
														
												  } catch (java.io.IOException e) {
													    System.out.println("Programme will exit");
														break;
												  }
												  
												  prevfare=0;
									              byte[] devstatus=new byte[12];
									              byte[] coindevStatus=new byte[12];
									              
												  //++devstatus =CurrencyObj.DeviceStatus( 1 ,0);
												  
												  coindevStatus= CurrencyObj.DeviceStatus( 2 ,0);
												 
												  //++System.out.println("[TestAscrm()] Note Acceptor Status Code : "+devstatus[1]); 
												  
												  //++devStatus2 = (devStatus2&0x02)>>1;
												  
												  System.out.println("[TestAscrm()] Coin Acceptor Status Code : "+coindevStatus[2]); 
												
                                                  fare1=0;
									   
												  System.out.println("[TestAscrm()] ENTER FARE: ");
												  
												  Scanner scanner = new Scanner(System.in);
                                                  
                                                  UserinputString = null;
                                                  
                                                  UserinputString = scanner.next();
									   
												  fare1 = Integer.parseInt(UserinputString);

                                                  UserinputString = null;
                                                  
												  System.out.println("[TestAscrm()] ENTER FARE TIME: ");
									   
												  UserinputString = scanner.next();
												  
												  transtime = Integer.parseInt(UserinputString);
												  
												  System.out.println("[TestAscrm()] ENTER MAX CASH: ");
									   
									              UserinputString = null;
									              
												  UserinputString = scanner.next();
												  
												  maxcash = Integer.parseInt(UserinputString);
												  
												  System.out.println("[TestAscrm()] ENTER MAX COINS: ");
									   
									              UserinputString = null;
									              
												  UserinputString = scanner.next();
												  
												  maxcoin = Integer.parseInt(UserinputString);
												 
												  //++Clear Array
												  for( row=0;row<(MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN);row++){
													  
														for( col=0;col<2;col++){
														    arry[row][col]=0;
														}//for end

												  }//for end
					
												  //transtime = GetTransTimeout();
												  
												  System.out.println("[TestAscrm()] TransTime In Second: "+ transtime );

												  int DenomMask = 0x00;

												  System.out.println("[TestAscrm()] ENTER Denom Mask Value: ");
									              
									              UserinputString = null;
                                                  
                                                  UserinputString = scanner.next();
                                                  
												  DenomMask = Integer.parseInt(UserinputString);

												  System.out.println("[TestAscrm()] Denom Mask Value: "+DenomMask  ); 
												  
												  //if( maxcash >= 1 ) {
													//CurrencyObj.EnableTheseDenominations( 1, DenomMask,0);
                                                  //}
                                                  
                                                  if( maxcoin >= 1 ) {
													CurrencyObj.EnableTheseDenominations( 2, DenomMask,0);
						                          }
						                  
						                          System.out.println("[TestAscrm()] Max Cash: "+maxcash  ); 
						                          System.out.println("[TestAscrm()] Max Coin: "+maxcoin  ); 
						                          
												  //++boolean AcceptCurrencies(int CurrencyType,int Amount,int Timeout)
												  //++CurrencyType: 0 Both 1 : Note 2: Coin
												  boolean acptfarertcode = false;
												  
												  acptfarertcode = CurrencyObj.AcceptCurrencies(0,fare1,transtime*1000);
												  
												  
												  //if( ( maxcash >= 1 ) && ( maxcoin >= 1 ) ) {
												  //	System.out.println("[TestAscrm()]) acceptFare() Both  maxcash and maxcoin set");
												  //	acptfarertcode = CurrencyObj.AcceptCurrencies(0,fare1,transtime*1000);
												  //} else if( ( maxcash >= 1 ) && ( maxcoin <= 0 ) ){
													//System.out.println("[TestAscrm()]) acceptFare() Only  maxcash  set");
												    //acptfarertcode = CurrencyObj.AcceptCurrencies(1,fare1,transtime*1000);
											      //}else if( ( maxcash <=0 ) && ( maxcoin >= 1 ) ){
													//System.out.println("[TestAscrm()]) acceptFare() Only  maxcoin  set");
												    //acptfarertcode = CurrencyObj.AcceptCurrencies(2,fare1,transtime*1000);
											      //}
											      
											      
                                                  if( false == acptfarertcode){
													  System.out.println("[TestAscrm()]) acceptFare() return false again try!! ");
													  continue;
                                                  }else if( true == acptfarertcode){
													  System.out.println("[TestAscrm()]) acceptFare() return true.");
												  } //else if end
									  
												  rtcode = 0;
									  
												  UserAmountInput = 0;
									  
												  row=0;
												  
												  col=0;
										  
												  prevfare = 0;
                                                  
                                                  long start =0,end=0;
                                                  
                                                  double diff=0;
                                                  
                                                  start = System.nanoTime();
                                                  
                                                  //++System.out.println("[TestAscrm()] >>  start: "+start);  
                                                 
												  while(true) {
													  
																		
																		end = System.nanoTime();

																		diff= (double)( (end-start)/1000000000 ) ;
                                                                        
                                                                        //System.out.println("[TestAscrm()] >>  End: "+end);  
                                                                        //System.out.println("[TestAscrm()] >>  Diff: "+diff);  
                                                                        //System.out.println("[TestAscrm()] >>  Transtime: "+transtime);  
                                                                          
                                                                        ///////////////////////////////////////////////////////////////////////////////////////////
                                                                        
																		if( diff>=transtime ) {
																								   
                                                                      
																			   System.out.println("[TestAscrm()] >>  Transtimeout by system");  
																			   rtcode = CurrencyObj.GetAcceptedAmount(arry);
                                                                               System.out.println("[TestAscrm()]getDenomination Return code="+rtcode); 
																			   UserAmountInput=0; 
																			   //for( row=0;row<MAX_NMBR_OF_NOTE;row++)
																			   //{
																					  //if(arry[row][1]>0) {
																					  //UserAmountInput+=GetDenomValue(arry[row][1]); 
																					  //}//if end

																			   //}//for end

																			   
																			   System.out.println("[TestAscrm()] >> User Given Currency: "+UserAmountInput); 
																			   
																			   if(fare1==UserAmountInput) {
																								   
																					System.out.println("[TestAscrm()] >>  System now commit your transaction");
																					rtcode = CurrencyObj.StackAcceptedCurrencies(50000);
																					System.out.println("[TestAscrm()] >>  rtcode: "+ rtcode);

																			   }else {
																					
																					System.out.println("[TestAscrm()] >>  System now cancel your session");
																					CurrencyObj.ReturnAcceptedCurrencies(0);
																					System.out.println("[TestAscrm()] >>  Before check customer take note from outlet");
																					if(UserAmountInput > 0 ){
																					   //++CurrencyObj.IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
																					}
																					System.out.println("[TestAscrm()] >>  After check customer take note from outlet");
																			   }
																								   
																			   break;


																		}//if(diff>=transtime)

																		rtcode =-1;

																		rtcode = CurrencyObj.GetAcceptedAmount(arry);

                                                                        ///////////////////////////////////////////////////////////////////////////////////////////
                                                                        
																		//++Accepting State return code 0
																		if( 0 ==rtcode) {
																								  
																			  UserAmountInput=0;
																			  
																			  for( row=0;row<(MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN);row++){
																					
																					UserAmountInput += GetDenomValue( arry[row][0] ); 
																					UserAmountInput += GetDenomValue( arry[row][1] ); 
																					
																					
																			  }//for end
																								  
																			  if(UserAmountInput>prevfare){
																					   
																				   System.out.println("[TestNewAscrm()] >> User Given Fare = "+UserAmountInput); 
																				   prevfare=UserAmountInput;

																			   }//if end

																								   
																		}//if(1==rtcode) end

																		///////////////////////////////////////////////////////////////////////////////////////////

																		//Exact fare accepted return code 1
																		if(1==rtcode) {
																				   
																		   System.out.println("\n =========================================================================");            
																		   System.out.println("[TestNewAscrm()] GetAcceptedAmount return code: "+rtcode); 
																		   System.out.println("\n =========================================================================");      
																		   System.out.println("[TestAscrm()] >> ACCEPTED FARE GIVEN");  
															               rtcode = CurrencyObj.StackAcceptedCurrencies(50000);
															               System.out.println("[TestAscrm()] >> rtcode : "+ rtcode );  
																		   break;

																		}//if(2==rtcode)

																		
																		///////////////////////////////////////////////////////////////////////////////////////////

																		//++Transactions Timeout. return code 18
																		if (18==rtcode){
																			 
																				   System.out.println("\n =========================================================================");      
																				   System.out.println("[TestAscrm()] GetAcceptedAmount return code: "+rtcode); 
																				   System.out.println("\n =========================================================================");      
                                                                                   System.out.println("[TestAscrm()] >>  Transtimeout by user");  
                                                                                   UserAmountInput=0;
															                       //for( row=0;row<MAX_NMBR_OF_NOTE;row++){
																						//if(arry[row][1]>0) {
																							//UserAmountInput+=GetDenomValue(arry[row][1]); 
																						//}
																				   //}
																				   CurrencyObj.ReturnAcceptedCurrencies(50000);
																				   if(UserAmountInput > 0 ){
																					   //++IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
																				   }
                                                                                   
                                                                                   break;

																		}//if (0==rtcode)

																		
																		////////////////////////////////////////////////////////////////////////////////////////// 
																		
																		// More cash qty return code 2
																		if( 2==rtcode ){
																			 

																				   System.out.println("\n =========================================================================");      
																				   System.out.println("[TestAscrm()] GetAcceptedAmount return code = "+rtcode); 
																				   System.out.println("\n =========================================================================");      
																				   System.out.println("[TestAscrm()] >> More Note inserted its time for cancel by system");            
                                                                                   UserAmountInput=0;
															                       //for( row=0;row<MAX_NMBR_OF_NOTE;row++)
																				   //{
																						//if(arry[row][1]>0) {
																							//UserAmountInput+=GetDenomValue(arry[row][1]); 
																						//}

																				   //}
                                                                                   CurrencyObj.ReturnAcceptedCurrencies(50000);
                                                                                   if(UserAmountInput > 0 ){
																					   //CurrencyObj.IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
																				   }
																				   
																				   break;

																		}//if( 2==rtcode )
													   
												                        ///////////////////////////////////////////////////////////////////////////////////////////

											} // End: while loop

													                    	
								}//End:  for loop
								  
                                */
                                
                                //byte[] coindevStatus2=new byte[12];
									              
								//coindevStatus2= CurrencyObj.DeviceStatus( 2 ,0);
												 
								//System.out.println("[TestAscrm()] Coin Acceptor Status Code : "+coindevStatus2[2]); 
												
												
						        ret = 0;
						        ret = CurrencyObj.DisConnectDevice(2,5000);
							    System.out.println("[TestAscrm()] DEACTIVATE Coin Acceptor STATUS: "+ret ); 
							    


			}//TestNewAscrm() end

            public static void TestSingleNewAscrm() {

                              /*
                              String UserinputString = null;
					          Currency CurrencyObj = new Currency();
					          
						      //Log Test
						      
						      int devStatus1=0,
						          devStatus2=0;
						      
						      String deviceid="JCM";
						      //++strcpy(g_Deviceid,deviceid);
						      //++g_LogModeLevel = (int)ALL_LOG;
						      
						      int  MAX_NMBR_OF_NOTE =20,
						           MAX_NMBR_OF_COIN =20;
						      int  counter=0,ret=0,rtcode=0,
						           UserAmountInput = 0,
						           prevfare=0,row=0 ,
						           col=0,
						           fare1=0,transtime=60,maxcash=0,maxcoin=0;
						           
						       byte[][] arry= null;
						       arry = new byte[MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN][2] ;
						       
						       for( row=0;row< (MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN) ;row++) {
						            for( col=0;col<2;col++){
						                  arry[row][col]=0;
						            }//for end

						       }//for end
				
						       int PortId1=1,PortId2=0,PortId3=0;
                               int EscrowMode=1; //++ 0: Retain Note or Coin 1: Clear Note and Coin
						       
						       ret =  CurrencyObj.ConnectDevice(0,1,0,1,EscrowMode,5000);
							   System.out.println( "[TestAscrm()] ACTIVATE Note Acceptor STATUS: "+ret ); 
							   
							   if( 0 != ret ){
						           System.out.println("[TestAscrm()] ACTIVATE Note Acceptor Failed" );
						           //return; 
						       }//else end
						       
						       ret =  0;
						       ret =  CurrencyObj.ConnectDevice(0,1,0,2,EscrowMode,5000);
							   System.out.println("[TestAscrm()] ACTIVATE Coin Acceptor STATUS: "+ret ); 
							   
							   if( 0 != ret ){
						           System.out.println("[TestAscrm()] ACTIVATE Coin Acceptor Failed" );
						           //return; 
						       }//else end
						       
						       for( ;; ) {
			
                                                  
												  System.out.println("Press \"ENTER\" to continue...");
												  try {
														
														int read = System.in.read(new byte[2]);
														
														
												  } catch (java.io.IOException e) {
													    System.out.println("Programme will exit");
														break;
												  }
												  
												  prevfare=0;
									   
												  devStatus1 =CurrencyObj.DeviceStatus( 1 ,0);
												  
												  devStatus2= CurrencyObj.DeviceStatus( 2 ,0);
												 
												  devStatus1 = (devStatus1&0x02)>>1;
												  
												  System.out.println("[TestAscrm()] Note Acceptor Status Code : "+devStatus1); 
												  
												  devStatus2 = (devStatus2&0x02)>>1;
												  
												  System.out.println("[TestAscrm()] Coin Acceptor Status Code : "+devStatus2); 
												
                                                  fare1=0;
									   
												  System.out.println("[TestAscrm()] ENTER FARE: ");
												  
												  Scanner scanner = new Scanner(System.in);
                                                  
                                                  UserinputString = null;
                                                  
                                                  UserinputString = scanner.next();
									   
												  fare1 = Integer.parseInt(UserinputString);

                                                  UserinputString = null;
                                                  
												  System.out.println("[TestAscrm()] ENTER FARE TIME: ");
									   
												  UserinputString = scanner.next();
												  
												  transtime = Integer.parseInt(UserinputString);
												  
												  System.out.println("[TestAscrm()] ENTER MAX CASH: ");
									   
									              UserinputString = null;
									              
												  UserinputString = scanner.next();
												  
												  maxcash = Integer.parseInt(UserinputString);
												  
												  System.out.println("[TestAscrm()] ENTER MAX COINS: ");
									   
									              UserinputString = null;
									              
												  UserinputString = scanner.next();
												  
												  maxcoin = Integer.parseInt(UserinputString);
												 
												  //++Clear Array
												  for( row=0;row<(MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN);row++){
													  
														for( col=0;col<2;col++){
														    arry[row][col]=0;
														}//for end

												  }//for end
					
												  //transtime = GetTransTimeout();
												  
												  System.out.println("[TestAscrm()] TransTime In Second: "+ transtime );

												  int DenomMask = 0x00;

												  System.out.println("[TestAscrm()] ENTER Denom Mask Value: ");
									              
									              UserinputString = null;
                                                  
                                                  UserinputString = scanner.next();
                                                  
												  DenomMask = Integer.parseInt(UserinputString);

												  System.out.println("[TestAscrm()] Denom Mask Value: "+DenomMask  );
												  
												  if( maxcash >= 1 ) { 
												      maxcash = 1;
											      }
											      
											      if( maxcoin >= 1 ) {
											          maxcoin=1;
											      }
											   
												  if( maxcash >= 1 ) {
													CurrencyObj.EnableTheseDenominations( 1, DenomMask,0);
                                                  }
                                                  
                                                  if( maxcoin >= 1 ) {
													CurrencyObj.EnableTheseDenominations( 2, DenomMask,0);
						                          }
						                          						                          
						                          System.out.println("[TestAscrm()] Max Cash: "+maxcash  ); 
						                          System.out.println("[TestAscrm()] Max Coin: "+maxcoin  ); 
						                          
												  //++boolean AcceptCurrencies(int CurrencyType,int Amount,int Timeout)
												  //++CurrencyType:  1 : Note 2: Coin
												  int acptfarertcode = 0;
												  
												  if( ( maxcash >= 1 ) && ( maxcoin <=0 ) ) {
													
													 System.out.println("[TestAscrm()]) acceptFare() Both  maxcash and maxcoin set");
													 acptfarertcode = CurrencyObj.GetValidCurrency(1,fare1,transtime*1000);
													
												  } else if( ( maxcash <=0 ) && ( maxcoin >=1  ) ){
													
													System.out.println("[TestAscrm()]) acceptFare() Only  maxcash  set");
												    acptfarertcode = CurrencyObj.GetValidCurrency(2,fare1,transtime*1000);
												    
											      }
											      
                                                  if( 0 == acptfarertcode){
													  System.out.println("[TestAscrm()]) acceptFare() return false again try!! ");
													  continue;
                                                  }else if( 1 == acptfarertcode){
													  System.out.println("[TestAscrm()]) acceptFare() return true.");
												  }  
									  
												  rtcode = 0;
									  
												  UserAmountInput = 0;
									  
												  row=0;col=0;
										  
												  prevfare = 0;
                                                  
                                                  long start =0,end=0;
                                                  
                                                  double diff=0;
                                                  
                                                  start = System.nanoTime();
                                                  
                                                  //System.out.println("[TestAscrm()] >>  start: "+start);  
                                                 
												  while(true) {
													  
																		
																		end = System.nanoTime();

																		diff= (double)( (end-start)/1000000000 ) ;
                                                                        
                                                                        //System.out.println("[TestAscrm()] >>  End: "+end);  
                                                                        
                                                                        //System.out.println("[TestAscrm()] >>  Diff: "+diff);  
                                                                        
                                                                        //System.out.println("[TestAscrm()] >>  Transtime: "+transtime);  
                                                                          
                                                                        ///////////////////////////////////////////////////////////////////////////////////////////
                                                                        
																		if( diff>=transtime ) {
																								   
                                                                      
																			   System.out.println("[TestAscrm()] >>  Transtimeout by system");  
																			   rtcode = CurrencyObj.GetAcceptedAmount(arry);
                                                                               System.out.println("[TestAscrm()]getDenomination Return code="+rtcode); 
																			   UserAmountInput=0; 
																			   //for( row=0;row<MAX_NMBR_OF_NOTE;row++)
																			   //{
																					  //if(arry[row][1]>0) {
																					  //UserAmountInput+=GetDenomValue(arry[row][1]); 
																					  //}//if end

																			   //}//for end

																			   
																			   System.out.println("[TestAscrm()] >> User Given Currency: "+UserAmountInput); 
																			   
																			   if(fare1==UserAmountInput) {
																								   
																					System.out.println("[TestAscrm()] >>  System now commit your transaction");
																					CurrencyObj.AcceptCurrentCurrency(0,0,0);

																			   }else {
																					
																					System.out.println("[TestAscrm()] >>  System now cancel your session");
																					CurrencyObj.ReturnCurrentCurrency(0,0);
																					System.out.println("[TestAscrm()] >>  Before check customer take note from outlet");
																					if(UserAmountInput > 0 ){
																					   //++CurrencyObj.IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
																					}
																					System.out.println("[TestAscrm()] >>  After check customer take note from outlet");
																			   }
																								   
																			   break;


																		}//if(diff>=transtime)

																		rtcode =-1;

																		rtcode = CurrencyObj.GetAcceptedAmount(arry);

                                                                        ///////////////////////////////////////////////////////////////////////////////////////////
                                                                        
																		//++Accepting State return code 0
																		if( 0 ==rtcode) {
																								  
																			  UserAmountInput=0;
																			  
																			  for( row=0;row<(MAX_NMBR_OF_NOTE+MAX_NMBR_OF_COIN);row++){
																					
																					if(arry[row][1]>0) {
																					  UserAmountInput += GetDenomValue( arry[row][1] ); 
																					}//if end
																					
																			  }//for end
																								  
																			  if(UserAmountInput>prevfare){
																					   
																				   System.out.println("[TestNewAscrm()] >> User Given Fare = "+UserAmountInput); 
																				   prevfare=UserAmountInput;

																			   }//if end

																			 
																								   
																		}//if(1==rtcode) end

																		///////////////////////////////////////////////////////////////////////////////////////////

																		//Exact fare accepted return code 1
																		if(1==rtcode) {
																				   
																		   System.out.println("\n =========================================================================");            
																		   System.out.println("[TestNewAscrm()] GetAcceptedAmount return code: "+rtcode); 
																		   System.out.println("\n =========================================================================");      
																		   System.out.println("[TestAscrm()] >> ACCEPTED FARE GIVEN");  
															               CurrencyObj.AcceptCurrentCurrency(0,0,0);
																		   break;

																		}//if(2==rtcode)

																		
																		///////////////////////////////////////////////////////////////////////////////////////////

																		//++Transactions Timeout. return code 18
																		if (18==rtcode){
																			 
																				   System.out.println("\n =========================================================================");      
																				   System.out.println("[TestAscrm()] GetAcceptedAmount return code: "+rtcode); 
																				   System.out.println("\n =========================================================================");      
                                                                                   System.out.println("[TestAscrm()] >>  Transtimeout by user");  
                                                                                   UserAmountInput=0;
															                       //for( row=0;row<MAX_NMBR_OF_NOTE;row++){
																						//if(arry[row][1]>0) {
																							//UserAmountInput+=GetDenomValue(arry[row][1]); 
																						//}
																				   //}
																				   CurrencyObj.ReturnCurrentCurrency(0,0);
																				   if(UserAmountInput > 0 ){
																					   //++IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
																				   }
                                                                                   
                                                                                   break;

																		}//if (0==rtcode)

																		
																		////////////////////////////////////////////////////////////////////////////////////////// 
																		
																		// More cash qty return code 2
																		if( 2==rtcode ){
																			 

																				   System.out.println("\n =========================================================================");      
																				   System.out.println("[TestAscrm()] GetAcceptedAmount return code = "+rtcode); 
																				   System.out.println("\n =========================================================================");      
																				   System.out.println("[TestAscrm()] >> More Note inserted its time for cancel by system");            
                                                                                   UserAmountInput=0;
															                       //for( row=0;row<MAX_NMBR_OF_NOTE;row++)
																				   //{
																						//if(arry[row][1]>0) {
																							//UserAmountInput+=GetDenomValue(arry[row][1]); 
																						//}

																				   //}
                                                                                   CurrencyObj.ReturnCurrentCurrency(0,0);
                                                                                   if(UserAmountInput > 0 ){
																					   //CurrencyObj.IsNoteRemoved(WAIT_TIME_FOR_DISPENSED);
																				   }
																				   
																				   break;

																		}//if( 2==rtcode )
													   
												                        ///////////////////////////////////////////////////////////////////////////////////////////

											} // End: while loop

													                    	
								}//End:  for loop
								  
                                
                                ret = 0;
                                ret =  CurrencyObj.DisConnectDevice(1,5000);
							    System.out.println( "[TestAscrm()] DEACTIVATE Note Acceptor STATUS: "+ret ); 
						       
						        ret = 0;
						        ret = CurrencyObj.DisConnectDevice(2,5000);
							    System.out.println("[TestAscrm()] DEACTIVATE Coin Acceptor STATUS: "+ret ); 
							    */
							   
			}//TestNewAscrm() end

}//Class TestCurrency End
