#ifndef _SMARTCARDRETURN_H_
#define _SMARTCARDRETURN_H_


#define     SUCCESS                                           0
#define     OPERATION_FAILED                                  1
#define     OTHER_ERROR                                       31
#define     OPERATION_TIMEOUT_OCCURRED                        18
#define     COMMUNICATION_FAILURE                             28
#define     ALREADY_NOT_CONNECTED                             20
#define     DEVICE_NOT_YET_CONNECTED                          20

#define CHANEL_CLEARANCE_FAILED_DUE_TO_REJECTION_BIN_FULL     1
#define CHANEL_CLEARANCE_FAILED_DUE_TO_RETURN_MOUTH_BLOCKED   2
#define CHANEL_CLEARANCE_FAILED_DUE_TO_UNKNOWN_REASON         3

#define INSERT_RETURN_MOUTH_BLOCKED                          2
#define CHANNEL_BLOCKED                                      1
#define STACKER_EMPTY                                        3
#define NO_CARD_IN_CHANNEL                                   2
#define REJECTION_BIN_FULL                                   1
#define CARD_FOUND_IN_CHANNEL                                1
#define NO_CARD_FOUND_IN_CHANNEL                             0

#define CARD_NOT_REMOVED                                     0
#define CARD_REMOVED                                         1

#define DISCONNECT_SUCCESS_BUT_ONE_CARD_IN_CHANNEL           1

#define CARD_FOUND_AND_DEACTIVATED                           0
#define CARD_FOUND_AND_DEACTIVATED_FAILED                    1
#define NO_CARD_FOUND                                        10

#define CARD_FOUND_AND_ACTIVATED                             0
#define CARD_FOUND_AND_ACTIVATED_FAILED                      1
#define CARD_FOUND_AND_BUT_UNSUPPORTED                       2

#define NO_SAM_CARD_FOUND                                    2

#define RFID_READER_READY                                    0
#define RFID_READER_NOT_READY                                1

#define SAM_READER_READY                                     0
#define SAM_READER_NOT_READY                                 1


#define STACKER_EMPTY                                     0
#define STACKER_NEARLY_EMPTY                              1
#define STACKER_NEARLY_FULL                               2
#define STACKER_FULL                                      3

#define REJECTION_BIN_EMPTY                               0
#define REJECTION_BIN_FULL                                3

#define COLLECTION_BIN_EMPTY                              0
#define COLLECTION_BIN_NEARLY_EMPTY                       1
#define COLLECTION_BIN_NEARLY_FULL                        2
#define COLLECTION_BIN_FULL                               3



#endif
