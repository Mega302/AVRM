#!/bin/bash
source /etc/profile
#####################################################################################################################################
# Make NoteAcceptor SO File and Copy it in Cris Folder
cd /SmartCard_ASCRM_20191111/ascrm-version-20/ascrm_na_20160902
./clean.sh
echo -e ">> Going to Make libMegaAscrmNoteAcceptorAPI.so"
make -f Makeso
cp -f libMegaAscrmNoteAcceptorAPI.so /SmartCard_ASCRM_20191111/ascrm-version-20/Cris
./clean.sh
###################################################################################################################################
# Make SmartCard Gateway SO File and Copy it in Cris Folder
cd /SmartCard_ASCRM_20191111/ascrm-version-20/ascrmgateway_20160516
./clean.sh
echo -e "\n\n>> Going to Make libMegaAscrmSmartCardAPIGateWay.so"
make -f Makeso
cp -f libMegaAscrmSmartCardAPIGateWay.so /SmartCard_ASCRM_20191111/ascrm-version-20/Cris
./clean.sh
###################################################################################################################################
# Make SmartCard SO File and Copy it in Cris Folder
cd /SmartCard_ASCRM_20191111/ascrm-version-20/ascrm_sc_20160901
./clean.sh
echo -e "\n\n>> Going to Make libMegaAscrmSmartCardAPI.so"
make -f Makeso
cp -f libMegaAscrmSmartCardAPI.so /SmartCard_ASCRM_20191111/ascrm-version-20/Cris
./clean.sh
###################################################################################################################################

# Make Security SO File and Copy it in Cris Folder
#cd /SmartCard_ASCRM_20191111/ascrm-version-20/ascrm_security_20151129
#./clean.sh
#make -f Makeso
#cp -f libMegaAscrmSecurityAPI.so /SmartCard_ASCRM_20191111/ascrm-version-20/Cris
#./clean.sh

###################################################################################################################################

# Make Log SO File and Copy it in Cris Folder
cd /SmartCard_ASCRM_20191111/ascrm-version-20/ascrm_log_20160505
./clean.sh
echo -e "\n\n>> Going to Make libMegaAscrmLogAPI.so"
make -f Makeso
cp -f libMegaAscrmLogAPI.so /SmartCard_ASCRM_20191111/ascrm-version-20/Cris
./clean.sh

##################################################################################################################################
cd /SmartCard_ASCRM_20191111/ascrm-version-20/Cris
echo -e "\n\n>>ldd -d libMegaAscrmSmartCardAPI.so"
ldd -d libMegaAscrmSmartCardAPI.so
echo -e "\n\n>>ldd -d libMegaAscrmNoteAcceptorAPI.so"
ldd -d libMegaAscrmNoteAcceptorAPI.so
echo -e "\n\n>>ldd -d libMegaAscrmLogAPI.so"
ldd -d libMegaAscrmLogAPI.so
echo -e "\n\n>>ldd -d libMegaAscrmSmartCardAPIGateWay.so"
ldd -d libMegaAscrmSmartCardAPIGateWay.so
cd /SmartCard_ASCRM_20191111/ascrm-version-20/
##################################################################################################################################

echo -e "\n\n>> javac  -classpath .:RXTXcomm.jar Cris/AscrmLog.java Cris/SmartCard.java Cris/Security.java Cris/Currency.java Cris/UPS.java Cris/Printer.java Cris/Common.java Cris/TokenDispenser.java"

javac  -classpath ".:RXTXcomm.jar" "Cris/AscrmLog.java" "Cris/SmartCard.java" "Cris/Security.java" "Cris/Currency.java" "Cris/UPS.java" "Cris/Printer.java" "Cris/Common.java" "Cris/TokenDispenser.java"
sleep 1

#################################################################################################################################

rm -f /SmartCard_ASCRM_20191111/ascrm-version-20/AVRMApi.jar
echo -e "\n\n>>jar -cvfm AVRMApi.jar Cris/Manifest.txt Cris/*.class Cris/libMegaAscrmSmartCardAPI.so Cris/libMegaAscrmNoteAcceptorAPI.so Cris/libMegaAscrmLogAPI.so Cris/libMegaAscrmSmartCardAPIGateWay.so"
echo -e "\n\n"
jar -cvfm AVRMApi.jar Cris/Manifest.txt Cris/*.class "Cris/libMegaAscrmSmartCardAPI.so" "Cris/libMegaAscrmNoteAcceptorAPI.so" "Cris/libMegaAscrmLogAPI.so" "Cris/libMegaAscrmSmartCardAPIGateWay.so"
sleep 1


##############################################################################################################################

#rm -f /SmartCard_ASCRM_20191111/ascrm-version-20/ascrmgui/AVRMApi.jar
#cp -f /SmartCard_ASCRM_20191111/ascrm-version-20/AVRMApi.jar /SmartCard_ASCRM_20191111/ascrm-version-20/ascrmgui/AVRMApi.jar
#chmod 777 /SmartCard_ASCRM_20191111/ascrm-version-20/ascrmgui/AVRMApi.jar

#############################################################################################################################################################################################

#cd /home1/megalog
#rm -f *.log

#######################################################################################################################################

cd /SmartCard_ASCRM_20191111/ascrm-version-20/Cris
./crisclean.sh
exit
########################################################################################################################################
