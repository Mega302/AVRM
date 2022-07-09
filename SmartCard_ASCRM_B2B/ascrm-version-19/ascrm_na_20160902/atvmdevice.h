#ifndef _ATVMDEVICE_H_

#define _ATVMDEVICE_H_


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//must be defined if any note acceptor device is present in ascrm system otherwise port operation throw error
#define NOTE_ACCEPTOR

//must be defined if any cctalk device is present in ascrm system otherwise port operation throw error
//#define CCTALK_DEVICE

//must be defined if any trap gate device is present in atvm system otherwise port operation throw error
//#define TRAP_GATE_DEVICE  



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(CCTALK_DEVICE)

//CCTALK Devices List

//#define COIN_ACCEPTOR

//#define COIN_ESCROW

//#define ATVM_ALARM_VAULT

#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NOTE_ACCEPTOR)

//Note Acceptor Devices List

#define                                           B2B_NOTE_ACCEPTOR

//#define                                         JCM_NOTE_ACCEPTOR

//#define                                         BNA_NOTE_ACCEPTOR

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Trap Gate Devices List
//#define      B2B_TRAP_GATE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //must any note acceptor device activate in atvmdevice header file
  #ifdef NOTE_ACCEPTOR

  #if !defined(B2B_NOTE_ACCEPTOR) && !defined(JCM_NOTE_ACCEPTOR) && !defined(BNA_NOTE_ACCEPTOR) 
  #error No note acceptor device is defined in atvmdevice header file
  #endif

  #if ( defined(B2B_NOTE_ACCEPTOR) && defined(JCM_NOTE_ACCEPTOR) ) || ( defined(B2B_NOTE_ACCEPTOR) && defined(BNA_NOTE_ACCEPTOR) ) ||( defined(BNA_NOTE_ACCEPTOR) && defined(JCM_NOTE_ACCEPTOR) ) 

  #error More than two note acceptor device is defined in atvmdevice header file

  #endif

  #endif

  #if !defined(NOTE_ACCEPTOR)

  #if defined(B2B_NOTE_ACCEPTOR) || defined(JCM_NOTE_ACCEPTOR) || defined(BNA_NOTE_ACCEPTOR) 
  #error NOTE_ACCEPTOR MACRO NOT DEFINED
  #endif

  #endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 //must any cctalk device activate in atvmdevice header file

 #ifdef CCTALK_DEVICE

 #if !defined(COIN_ACCEPTOR) && !defined(COIN_ESCROW) && !defined(ATVM_ALARM_VAULT)
 #error No cctalk device is defined in atvmdevice header file
 #endif

 #endif

 #if  !defined(CCTALK_DEVICE)

 #if defined(COIN_ACCEPTOR) || defined(COIN_ESCROW) || defined(ATVM_ALARM_VAULT)
 #error CCTALK MACRO NOT DEFINED
 #endif

 #endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//must any cctalk device activate in atvmdevice header file

 #ifdef TRAP_GATE_DEVICE 

 #if !defined(B2B_TRAP_GATE) 
 #error No trap gate device is defined in atvmdevice header file
 #endif

 #endif
 
 #if !defined(TRAP_GATE_DEVICE) 

 #if defined(B2B_TRAP_GATE) 
 #error TRAP_GATE_DEVICE MACRO NOT DEFINED
 #endif

 #endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

