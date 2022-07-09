#!/bin/bash
javac -classpath ".:AVRMApi.jar" TestSecurity.java
java -classpath ".:AVRMApi.jar" TestSecurity
