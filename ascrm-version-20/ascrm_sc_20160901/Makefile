#This is Makefile for Indian Rail Automatic Ticket vending Machine Project
# Designed By Mega Designs. Pvt. Ltd Saltlake Kolkata
#-----------------------------------------------------------------------------------------------------------------------------

#compiler name variable
CC =gcc

#-----------------------------------------------------------------------------------------------------------------------------

#Compiler flags Variable

#for an unix executable use this flags
CFLAGS= -c 


#-----------------------------------------------------------------------------------------------------------------------------

OUTPUTFLAG= -o smartcard.bin -lpthread -lrt


#library choice 
#-----------------------------------------------------------------------------------------------------------------------------

#Object File Name Variable

SMARTCARDOBJECTFILES= readini.o atvmlog.o delaytime.o serial.o SmartCardApi.o smartcardjni.o


#------------------------Start of Makefile------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------------

output:smartcardexe

smartcardexe:$(SMARTCARDOBJECTFILES)
	$(CC) $(SMARTCARDOBJECTFILES) $(OUTPUTFLAG)



#-----------------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------------

readini.o: readini.c
	$(CC)  $(CFLAGS)  readini.c	

atvmlog.o: atvmlog.c 
	$(CC)  $(CFLAGS)  atvmlog.c

delaytime.o: delaytime.c
	$(CC)  $(CFLAGS)  delaytime.c

serial.o: serial.c
	$(CC)  $(CFLAGS)  serial.c

SmartCardApi.o: SmartCardApi.c       
	$(CC)  $(CFLAGS)  SmartCardApi.c

smartcardjni.o: smartcardjni.c       
	$(CC)  $(CFLAGS)  smartcardjni.c


#-----------------------------------------------------------------------------------------------------------------------------
#-------------------------------------End of makefile-------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------------


