#ifndef _CURRENCYRETURN_H_
#define _CURRENCYRETURN_H_

#define     SUCCESS                               0

#define     OTHER_ERROR                           31
#define     OPERATION_TIMEOUT_OCCURRED            18

#define     ALREADY_NOT_CONNECTED_NOTEACCEPTOR    20
#define     ALREADY_NOT_CONNECTED_COINACCEPTOR    21
#define     ALREADY_NOT_CONNECTED_COINESCROW      22

#define     COMMUNICATION_FAILURE                               28
#define     COMMUNICATION_FAILURE_NOTE_ACCEPTOR                 28
#define     COMMUNICATION_FAILURE_COIN_ACCEPTOR                 29
#define     COMMUNICATION_FAILURE_NOTE_AND_COIN_ACCEPTOR        30

//++Connect Device
#define     CONNECT_SUCCESS                                     0
#define     CONNECT_SUCCESS_WITHFEW_NOTECOIN_IN_ESCROW          1
#define     SECURITY_DOOR_OPENED                                2
#define     ESCROW_CLEARANCE_FAILED_AS_COLLECTION_BIN_FULL      3
#define     ESCROW_CLEARANCE_FAILED_AS_BLOCKING                 4
#define     ESCROW_CLEARANCE_FAILED_AS_UNKNOWN                  5
#define     PORT_DOESNOT_EXIST_NOTEACCEPTOR                     25
#define     PORT_DOESNOT_EXIST_COIN_ACCEPTOR                    26
#define     PORT_DOESNOT_EXIST_ALL_ACCEPTOR                     27

//++Disconnect Device
#define     DISCONNECT_SUCCESS_WITHFEW_NOTECOIN_IN_ESCROW       1
#define     NOTE_ACCEPTOR_NOT_YET_CONNECTED                     20
#define     COIN_ACCEPTOR_NOT_YET_CONNECTED                     21
#define     COIN_ESCROW_NOT_YET_CONNECTED                       22

//++Get Accepted Amount
#define   STATE_ACCEPTING                                      0  
#define   STATE_EXACT_AMOUNT_ACCEPTED                          1 
#define   STATE_EXCESS_AMOUNT_ACCEPTED                         2
#define   STATE_NOTE_ESCROW_FULL                               3
#define   STATE_COIN_ESCROW_FULL                               4
#define   STATE_NOTE_COIN_ESCROW_FULL                          5  

#define   STACKED_NOTE_TRANSPORT_CHANEL_BLOCKED                4
#define   STACKED_COIN_TRANSPORT_CHANEL_BLOCKED                5

#define   STACKED_NOTE_COIN                                    0
#define   STACKED_NOTE_CASH_BOX_FULL                           1
#define   STACKED_COIN_CASH_BOX_FULL                           2
#define   STACKED_NOTE_COIN_CASH_BOX_FULL                      3
#define   STACKED_NOTE_AND_COIN_TRANSPORT_CHANEL_BLOCKED       6

#define   RETURNED_NOTE_COIN                                   0
#define   RETURNED_NOTE_MOUTH_BLOCKED                          1
#define   RETURNED_COIN_MOUTH_BLOCKED                          2
#define   RETURNED_NOTE_AND_COIN_MOUTH_BLOCKED                 3

//Single Fare
#define NOTE_REJECTED_DUE_TO_INVALID_NOTE       10
#define COIN_REJECTED_DUE_TO_INVALID_COIN       10

#define NOTE_REJECTED_DUE_TO_INHIBITED_NOTE     20
#define COIN_REJECTED_DUE_TO_INHIBITED_COIN     20



#endif
