#!/bin/bash
javac -classpath ".:ASCRMApi.jar"  TestTime.java
java -cp ".:ASCRMApi.jar"  TestTime
