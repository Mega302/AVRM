#!/bin/bash
javac -classpath ".:AVRMApi.jar" TestUPS.java
java -classpath ".:AVRMApi.jar" TestUPS
