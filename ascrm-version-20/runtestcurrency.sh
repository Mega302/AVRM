#!/bin/bash
source /etc/profile
javac -classpath ".:AVRMApi.jar" TestCurrency.java
java -classpath ".:AVRMApi.jar" TestCurrency
