#!/bin/bash

###########################################################################

# Make NoteAcceptor SO File and Copy it in Cris Folder
cd /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/ascrm_na_20160902
./clean.sh
make -f Makeso
cp -f libMegaAscrmNoteAcceptorAPI.so /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/Cris
./clean.sh
###########################################################################


# Make SmartCard SO File and Copy it in Cris Folder
cd /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/ascrmgateway_20160516
./clean.sh
make -f Makeso
cp -f libMegaAscrmSmartCardAPIGateWay.so /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/Cris
./clean.sh
###########################################################################

# Make SmartCard SO File and Copy it in Cris Folder
cd /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/ascrm_sc_20160901
./clean.sh
make -f Makeso
cp -f libMegaAscrmSmartCardAPI.so /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/Cris
./clean.sh
###########################################################################

# Make Security SO File and Copy it in Cris Folder
cd /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/ascrm_security_20151129
./clean.sh
make -f Makeso
cp -f libMegaAscrmSecurityAPI.so /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/Cris
./clean.sh
###########################################################################

# Make Log SO File and Copy it in Cris Folder
cd /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/ascrm_log_20160505
./clean.sh
make -f Makeso
cp -f libMegaAscrmLogAPI.so /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/Cris
./clean.sh
###########################################################################


# Now calld desktop jar file make script
#cd /root/Desktop/SmartCard_ASCRM_20190814/ascrm-version-19/
#./makeascrmjar.sh

###########################################################################
exit
###########################################################################
