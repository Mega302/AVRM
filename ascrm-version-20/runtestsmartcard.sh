#!/bin/bash
javac -classpath ".:AVRMApi.jar" TestSmartCard.java
java -classpath ".:AVRMApi.jar" TestSmartCard
