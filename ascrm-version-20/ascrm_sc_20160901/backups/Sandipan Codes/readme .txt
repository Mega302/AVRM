Date :17 March,2014 Sandipan

A Demo of JNI(Java Native Interface).

Things to Remember
1. Java path set or not. If Not then go to /etc/bash.bashrc
   And type 
   export PATH=/usr/jdk1.6.0_43/bin:$PATH
   At the End of the File.
2. As we are showing this demo in /root/Desktop/jni in this Path U may get couple of error.To Avoid this error 
   Just go to /usr/jdk1.6.0_43/include 
   and copy jawt.java, jawt_md.h, jni_md.h, jni.h, jvmti.h these file to /root/Desktop/jni/

  
------------------------------------------------ JNI with C---------------------------------------------

1. Create HelloJNI.java and write the code given below.
   /*
   public class HelloJNI {
             
 		 static {
                           try
	                      {
      				System.load("/root/Desktop/jni/HelloJNI.so"); // Load native library at runtime
                                // hello.dll (Windows) or libhello.so (Unixes)
   			      }
 	                
	                   catch (UnsatisfiedLinkError e) 
	                      {
      		                System.err.println("Native code library failed to load.\n" + e);
      	 	                System.exit(1);
   	                      }
			 } 		

   // Declare a native method sayHello() that receives nothing and returns void
   private native void sayHello();
 
   // Test Driver
   public static void main(String[] args) 
   {
      try
	{
      		new HelloJNI().sayHello();  // invoke the native method
	}
	catch (UnsatisfiedLinkError e) 
	{
      	         System.err.println("Failed to Invoke the Native Method.\nJava Error is : "+ e);
      	         System.exit(1);
   	}      
        System.out.println("cvdwv");   
   }
}


 
   */
   
   Save the java file compile
   javac HelloJNI.java

2. Now Create a HelloJNI.h file.
   
   > javah HelloJNI
   It will Create HelloJNI.h file.

3. Edit the HelloJNI.h File
   #include <jni.h>
   After Edit
   #include "jni.h"
   Save the File.

4. After that Create a HelloJNI.c file and type the code given below.
   #include "jni.h"
   #include <stdio.h>
   #include "HelloJNI.h"
 
   // Implementation of native method sayHello() of HelloJNI class
   JNIEXPORT void JNICALL Java_HelloJNI_sayHello(JNIEnv *env, jobject thisObj) 
   {
   printf("Hello World!\n");
   return;
   }
  
    Save the File.

5. We have to create Object file of HelloJNI.c
   gcc -c hello.c 
   It will create hello.o object file(Assembly Code).

6. After that we have to Create .so file.(Shared File)
   gcc hello.o -shared -o libhello.so
 
7. Now Run the Java file which is HelloJNI.java
   java HelloJNI

This will give u your desire output.






 
