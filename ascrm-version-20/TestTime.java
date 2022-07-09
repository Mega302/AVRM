	
	
	import Cris.Currency;
	class TestTime{

          public static void main(String args[]){
                
                Currency CurrencyObj = new Currency(1);
                while(true){
					
					long[] StartTimeArray = CurrencyObj.getTime();
					System.out.println("[main()] StartTime in second: "+StartTimeArray[0]);
					System.out.println("[main()] StartTime in nanosecond: "+StartTimeArray[1]);
					try{
						 Thread.sleep(400);
					}catch(InterruptedException e){System.out.println("[main()] Sleep got Exception");}  
					
					
					long[] EndTimeArray = CurrencyObj.getTime();
					System.out.println("[main()] EndTime in second: "+EndTimeArray[0]);
					System.out.println("[main()] EndTime in nanosecond: "+EndTimeArray[1]);
					
					System.out.println("[main()] Difference in second: "+(EndTimeArray[0]-StartTimeArray[0]) );
					
					double milliSecond = 0;
					if( EndTimeArray[0] == StartTimeArray[0] ){
						milliSecond = (EndTimeArray[1]-StartTimeArray[1])/1000000;
					} else {
						long ns = (EndTimeArray[1]-StartTimeArray[1]) +1000000000;
						milliSecond = ns/1000000;
					}			
					
					//++System.out.println("[main()] Difference in nanosecond: "+(double)((EndTimeArray[1]-StartTimeArray[1])*1000000) );
					
					System.out.println("[main()] Difference in milisecond: "+milliSecond );
			   }

	      }//public static void main(String args[]) end
	  
	}//class TestTime end
