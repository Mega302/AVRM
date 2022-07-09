#!/bin/bash
rm -f *.class
javac -classpath ".:ASCRMApi.jar" TokenDispenser.java
java -classpath ".:ASCRMApi.jar" TokenDispenser
exit

