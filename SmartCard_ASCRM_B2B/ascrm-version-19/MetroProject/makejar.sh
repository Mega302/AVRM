#!/bin/bash

rm -f mega_ascrm/*.class
rm -f ascrm.jar

javac -classpath ".:ASCRMApi.jar" mega_ascrm/Ascrm.java mega_ascrm/TestPrinter/METRO_Printer.java
jar cvfm ascrm.jar Manifest.txt mega_ascrm/*.class mega_ascrm/TestPrinter/*.class

rm -f /SmartCard_ASCRM_B2B/ascrm-version-19/ascrmgui/ascrm.jar

cp -f /SmartCard_ASCRM_B2B/ascrm-version-19/MetroProject/ascrm.jar /SmartCard_ASCRM_B2B/ascrm-version-19/ascrmgui/ascrm.jar

chmod 777 /SmartCard_ASCRM_B2B/ascrm-version-19/ascrmgui/ascrm.jar


