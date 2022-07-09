#!/bin/bash
export PATH=$PATH:/usr/jdk1.6.0_25/bin
javac  "Cris/CashApi.java" "Cris/Security.java"
sleep 1
jar -cvf0 CashKiosk.jar Cris/*.class  "Cris/libMegaAtvmAPI.so"
sleep 1
rm -f /root/NetBeansProjects/AtvmProjectV1/lib/CashKiosk.jar
cp -f /root/Desktop/CashKiosk.jar /root/NetBeansProjects/AtvmProjectV1/lib
cd /root/Desktop/Cris
./crisclean.sh
exit
