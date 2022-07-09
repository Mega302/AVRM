#!/bin/bash
rm -f *.class
javac -classpath ".:ASCRMApi.jar" TestPort.java
java -classpath ".:ASCRMApi.jar" TestPort
exit

