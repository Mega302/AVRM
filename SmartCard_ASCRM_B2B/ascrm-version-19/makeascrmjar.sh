#!/bin/bash
####################################################################################################################################################################
javac  "Cris/SmartCard.java" "Cris/Security.java" "Cris/Currency.java"
sleep 1
###################################################################################################################################################################
rm -f /SmartCard_ASCRM_B2B/ascrm-version-19/ASCRMApi.jar
jar -cvfm ASCRMApi.jar Cris/Manifest.txt Cris/*.class "Cris/libMegaAscrmSmartCardAPI.so" "Cris/libMegaAscrmSecurityAPI.so"  "Cris/libMegaAscrmNoteAcceptorAPI.so" "Cris/libMegaAscrmLogAPI.so" "Cris/libMegaAscrmSmartCardAPIGateWay.so"
sleep 1
#################################################################################################################################################################
#rm -f /SmartCard_ASCRM_B2B/ascrm-version-19/ascrmgui/ASCRMApi.jar
#cp -f /SmartCard_ASCRM_B2B/ascrm-version-19/ASCRMApi.jar /SmartCard_ASCRM_B2B/ascrm-version-19/ascrmgui/ASCRMApi.jar
#chmod 777 /SmartCard_ASCRM_B2B/ascrm-version-19/ascrmgui/ASCRMApi.jar
#rm -f /ascrmgui/ASCRMApi.jar
#cp -f /SmartCard_ASCRM_B2B/ascrm-version-19/ASCRMApi.jar /ascrmgui/ASCRMApi.jar
#chmod 777 /ascrmgui/ASCRMApi.jar
##################################################################################################################################################################
#cd /home1/megalog
#rm -f *.log
###################################################################################################################################################################
cd /SmartCard_ASCRM_B2B/ascrm-version-19/Cris
./crisclean.sh
exit
###################################################################################################################################################################
