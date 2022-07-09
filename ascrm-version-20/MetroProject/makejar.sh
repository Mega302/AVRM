#!/bin/bash

rm -f mega_ascrm/*.class
rm -f ascrm.jar

javac -classpath ".:ASCRMApi.jar" mega_ascrm/Ascrm.java mega_ascrm/TestPrinter/METRO_Printer.java
jar cvfm ascrm.jar Manifest.txt mega_ascrm/*.class mega_ascrm/TestPrinter/*.class

rm -f /root/Desktop/ascrmgui/ascrm.jar
cp -f /root/Desktop/SmartCard_ASCRM/ascrm-version-18/MetroProject/ascrm.jar /root/Desktop/ascrmgui/ascrm.jar
chmod 777 /root/Desktop/SmartCard_ASCRM/ascrm-version-18/MetroProject/ascrm.jar


