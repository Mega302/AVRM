#!/bin/bash
javac -classpath ".:AVRMApi.jar" TestCCtalk.java
java -classpath ".:AVRMApi.jar" TestCCtalk
