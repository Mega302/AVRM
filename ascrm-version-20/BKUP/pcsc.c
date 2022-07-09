//gcc pcsc.c -o pcsc.bin -lrt -ldl -lpcsclite
#ifdef WIN32
#undef UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else

#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>
#include <unistd.h>
#endif

#ifdef WIN32
static char *pcsc_stringify_error(LONG rv)
{
 static char out[20];
 sprintf_s(out, sizeof(out), "0x%08X", rv);

 return out;
}
#endif

#define CHECK(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  printf(f ": %s\n", pcsc_stringify_error(rv)); \
  /*return -1;*/ \
 }

int main(void)
{
			 LONG rv;

			 SCARDCONTEXT hContext;
			 LPTSTR mszReaders;
			 SCARDHANDLE hCard;
			 DWORD dwReaders, dwActiveProtocol, dwRecvLength;

			 SCARD_IO_REQUEST pioSendPci;
			 BYTE pbRecvBuffer[258];
			 BYTE cmd1[] = { 0x00, 0xA4, 0x04, 0x00, 0x0A, 0xA0,0x00, 0x00, 0x00, 0x62, 0x03, 0x01, 0x0C, 0x06, 0x01 };
			 BYTE cmd2[] = { 0x00, 0x00, 0x00, 0x00 };

			 unsigned int i;
			 
			 while(1)
			 {

					 rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
					 if( SCARD_S_SUCCESS != rv )
					 {
						 printf("SCardEstablishContext Error: %s\n", pcsc_stringify_error(rv));
						 sleep(2);
						 continue;
					 }
					 CHECK("SCardEstablishContext", rv)

					 #ifdef SCARD_AUTOALLOCATE
						 printf("\n\nSCARD_AUTOALLOCATE\n");
						 dwReaders = SCARD_AUTOALLOCATE;
						 rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
						 //++CHECK("SCardListReaders", rv)
						 if( SCARD_S_SUCCESS != rv )
						 {
							printf("SCardListReaders Error: %s\n", pcsc_stringify_error(rv));
							sleep(1);
							continue;
						 }
					 #else
						 rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
						 CHECK("SCardListReaders", rv)
						 mszReaders = calloc(dwReaders, sizeof(char));
						 rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
						 if( SCARD_S_SUCCESS != rv )
						 {
							printf("SCardListReaders Error: %s\n", pcsc_stringify_error(rv));
							continue;
						 }
						 //++CHECK("SCardListReaders", rv)
					 #endif
					 printf("reader name: %s\n", mszReaders);

					 rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
					 if( SCARD_S_SUCCESS != rv )
					 {
						//++hContext = (long int)NULL; 
						rv = SCardFreeMemory(hContext, mszReaders);
						rv = SCardReleaseContext(hContext);
						printf("SCardConnect Error: %s\n", pcsc_stringify_error(rv));
						printf("\n\n");
						sleep(2);
						continue;
					 }
					 //++CHECK("SCardConnect", rv)

					 switch(dwActiveProtocol)
					 {
					  case SCARD_PROTOCOL_T0:
					   pioSendPci = *SCARD_PCI_T0;
					   break;

					  case SCARD_PROTOCOL_T1:
					   pioSendPci = *SCARD_PCI_T1;
					   break;
					 }
					 dwRecvLength = sizeof(pbRecvBuffer);
					 rv = SCardTransmit(hCard, &pioSendPci, cmd1, sizeof(cmd1),NULL, pbRecvBuffer, &dwRecvLength);
					 CHECK("SCardTransmit", rv)

					 printf("response: ");
					 for(i=0; i<dwRecvLength; i++)
					  printf("%02X ", pbRecvBuffer[i]);
					 printf("\n");

					 dwRecvLength = sizeof(pbRecvBuffer);
					 rv = SCardTransmit(hCard, &pioSendPci, cmd2, sizeof(cmd2),NULL, pbRecvBuffer, &dwRecvLength);
					 CHECK("SCardTransmit", rv)

					 printf("response: ");
					 for(i=0; i<dwRecvLength; i++)
					  printf("%02X ", pbRecvBuffer[i]);
					 printf("\n");

					 rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
					 if( SCARD_S_SUCCESS != rv )
					 {
						printf("SCardDisconnect Error: %s\n", pcsc_stringify_error(rv));
						continue;
					 }
					 CHECK("SCardDisconnect", rv)

					#ifdef SCARD_AUTOALLOCATE
					 rv = SCardFreeMemory(hContext, mszReaders);
					 if( SCARD_S_SUCCESS != rv )
					 {
						printf("SCardFreeMemory Error: %s\n", pcsc_stringify_error(rv));
						//continue;
					 }
					 CHECK("SCardFreeMemory", rv)

					#else
					 free(mszReaders);
					#endif

					 rv = SCardReleaseContext(hContext);
					 if( SCARD_S_SUCCESS != rv )
					 {
						printf("SCardReleaseContext Error: %s\n", pcsc_stringify_error(rv));
						//++continue;
					 }
					 //++CHECK("SCardReleaseContext", rv)
					 
					 //sleep(2);
					 
			 }		 

			 return 0;
}
