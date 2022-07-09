/* DO NOT EDIT THIS FILE - it is machine generated */
//#include <jni.h>
/* Header for class Cris_Currency */

#ifndef _Included_Cris_Currency
#define _Included_Cris_Currency
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     Cris_Currency
 * Method:    SetupNoteAcceptorLogFile
 * Signature: (Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_Cris_Currency_SetupNoteAcceptorLogFile
  (JNIEnv *, jobject, jstring, jint, jstring, jstring);

/*
 * Class:     Cris_Currency
 * Method:    ConnectDevice
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_ConnectDevice
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     Cris_Currency
 * Method:    DisConnectDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_DisConnectDevice
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    DeviceStatus
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_DeviceStatus
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    defaultCommit
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_defaultCommit
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    defaultCancel
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_defaultCancel
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    AcceptNotes
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_Cris_Currency_AcceptNotes
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     Cris_Currency
 * Method:    GetAcceptedAmount
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_GetAcceptedAmount
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     Cris_Currency
 * Method:    StackAcceptedNotes
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_StackAcceptedNotes
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    ReturnAcceptedNotes
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_ReturnAcceptedNotes
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    IsNoteRemoved
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_IsNoteRemoved
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    ClearJammedNotes
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_ClearJammedNotes
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     Cris_Currency
 * Method:    GetValidNote
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_GetValidNote
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     Cris_Currency
 * Method:    AcceptCurrentNote
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_AcceptCurrentNote
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     Cris_Currency
 * Method:    ReturnCurrentNote
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_ReturnCurrentNote
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    EnableTheseDenominations
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_EnableTheseDenominations
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     Cris_Currency
 * Method:    HoldCurrentNote
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Cris_Currency_HoldCurrentNote
  (JNIEnv *, jobject, jint);

/*
 * Class:     Cris_Currency
 * Method:    WriteNativeLog
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_Cris_Currency_WriteNativeLog
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
