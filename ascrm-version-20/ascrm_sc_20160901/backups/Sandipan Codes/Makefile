# This is Makefile for Metro Rail Smart Card Machine Project
# Designed By Mega Designs. Pvt. Ltd Saltlake Kolkata
#-----------------------------------------------------------------------------------------

#compiler name variable
CC= gcc

#-----------------------------------------------------------------------------------------

#Compiler flags Variable

#for an unix executable use this flags
CFLAGS= -c 


#-----------------------------------------------------------------------------------------

OUTPUTFLAG= -shared -o autoCardDespenser.so -lrt

#library choice 
#-----------------------------------------------------------------------------------------

#Object File Name Variable
SmartCardApiObjectFile= delaytime.o serial.o ConnectDevice.o AcceptCard.o CollectCard.o DisableCardAcceptance.o DisConnectDevice.o DispenseCard.o EnableCardAcceptance.o IsCardInChannel.o IsCardRemoved.o RejectCard.o ReturnCard.o



#----------------------------------Start of Makefile--------------------------------------
#-----------------------------------------------------------------------------------------

output:SmartCardApi

SmartCardApi:$(SmartCardApiObjectFile)
	$(CC) $(SmartCardApiObjectFile) $(OUTPUTFLAG)


#-----------------------------------------------------------------------------------------



delaytime.o: delaytime.c
	$(CC)  $(CFLAGS)  delaytime.c
serial.o: serial.c
	$(CC)  $(CFLAGS)  serial.c
ConnectDevice.o: ConnectDevice.c       
	$(CC)  $(CFLAGS)  ConnectDevice.c
AcceptCard.o: AcceptCard.c
	$(CC)  $(CFLAGS)  AcceptCard.c
CollectCard.o: CollectCard.c
	$(CC)  $(CFLAGS)  CollectCard.c
DisableCardAcceptance.o:  DisableCardAcceptance.c
	$(CC)  $(CFLAGS)  DisableCardAcceptance.c
DisConnectDevice.o:  DisConnectDevice.c
	$(CC)  $(CFLAGS)  DisConnectDevice.c
DispenseCard.o:  DispenseCard.c
	$(CC)  $(CFLAGS)  DispenseCard.c
EnableCardAcceptance.o:   EnableCardAcceptance.c
	$(CC)  $(CFLAGS)  EnableCardAcceptance.c
IsCardInChannel.o:   IsCardInChannel.c
	$(CC)  $(CFLAGS)  IsCardInChannel.c
IsCardRemoved.o:   IsCardRemoved.c
	$(CC)  $(CFLAGS)  IsCardRemoved.c
RejectCard.o:   RejectCard.c
	$(CC)  $(CFLAGS)  RejectCard.c 
ReturnCard.o:   ReturnCard.c
	$(CC)  $(CFLAGS)  ReturnCard.c    


#-------------------------------------------------------------------------------------
#-------------------------------------End of------------------------------------------
#-------------------------------------------------------------------------------------


