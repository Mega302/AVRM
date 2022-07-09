#!/bin/bash

####################################################################################

javac -classpath ".:/SmartCard_ASCRM_20191111/usblib/usb4java-1.2.0.jar" ListDevices.java

java -cp ".:/SmartCard_ASCRM_20191111/usblib/usb4java-1.2.0.jar:/SmartCard_ASCRM_20191111/usblib/libusb4java-1.2.0-linux-x86.jar:/SmartCard_ASCRM_20191111/usblib/commons-lang3-3.2.1.jar" ListDevices

####################################################################################
exit
####################################################################################
