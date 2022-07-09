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

OUTPUTFLAG= -o ascrmsecurity.bin -lpthread -lrt


#library choice 
#-----------------------------------------------------------------------------------------------------------------------------

#Object File Name Variable

CCTALKOBJECTFILES= readini.o atvmlog.o serialc.o cctalkdevs.o upsstatus.o atvmparport.o atvmapi.o ascrmsecurityjni.o

#------------------------Start of Makefile------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------------

output:cctalkexe

cctalkexe:$(CCTALKOBJECTFILES)
	$(CC) $(CCTALKOBJECTFILES) $(OUTPUTFLAG)


#-----------------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------------


readini.o: readini.c
	$(CC)  $(CFLAGS)  readini.c	

atvmlog.o: atvmlog.c 
	$(CC)  $(CFLAGS)  atvmlog.c

serial.o: serial.c         
	$(CC)  $(CFLAGS)  serial.c

serialc.o: serialc.c         
	$(CC)  $(CFLAGS)  serialc.c

serialgen.o: serialgen.c         
	$(CC)  $(CFLAGS)  serialgen.c

b2b.o:b2b.c 
	$(CC)  $(CFLAGS)  b2b.c

jcm.o:jcm.c 
	$(CC)  $(CFLAGS)  jcm.c

bna.o:bna.c 
	$(CC)  $(CFLAGS)  bna.c

cctalkdevs.o:cctalkdevs.c 
	$(CC)  $(CFLAGS)  cctalkdevs.c

DSCU.o:DSCU.c
	$(CC)  $(CFLAGS)  DSCU.c

upsstatus.o: upsstatus.c 
	$(CC)  $(CFLAGS)  upsstatus.c

atvmparport.o: atvmparport.c 
	$(CC)  $(CFLAGS)  atvmparport.c

atvmapi.o: atvmapi.c 
	$(CC)  $(CFLAGS)  atvmapi.c

ascrmsecurityjni.o: ascrmsecurityjni.c 
	$(CC)  $(CFLAGS)  ascrmsecurityjni.c


#-----------------------------------------------------------------------------------------------------------------------------
#-------------------------------------End of makefile-------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------------


