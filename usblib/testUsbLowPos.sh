#!/bin/bash

####################################################################################

javac -classpath ".:/root/Downloads/usblib/usb4java-1.2.0.jar" UsbLowPos.java

java -cp ".:/root/Downloads/usblib/usb4java-1.2.0.jar:/root/Downloads/usblib/libusb4java-1.2.0-linux-x86_64.jar:/root/Downloads/usblib/commons-lang3-3.2.1.jar" UsbLowPos

####################################################################################
exit
####################################################################################
