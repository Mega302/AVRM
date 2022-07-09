/*
 * Simple libusb-1.0 test programm
 * It openes an USB device, expects two Bulk endpoints,
 *   EP1 should be IN
 *   EP2 should be OUT
 * It alternates between reading and writing a packet to the Device.
 * It uses Synchronous device I/O
 *
 * Compile:
 *   gcc -lusb-1.0 -o test test.c
 * Run:
 *   ./test
 * Thanks to BertOS for the example:
 *   http://www.bertos.org/use/tutorial-front-page/drivers-usb-device
 *
 * For Documentation on libusb see:
 *   http://libusb.sourceforge.net/api-1.0/modules.html
 */
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

//change if your libusb.h is located elswhere
#include <libusb-1.0/libusb.h>
//or uncomment this line:
//#include <libusb.h>
//and compile with:
//gcc -lusb-1.0 -o test.bin -I/path/to/libusb-1.0/ testlsusb.c

//int 	libusb_control_transfer (libusb_device_handle *dev_handle, uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout)
//int 	libusb_bulk_transfer (libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *transferred, unsigned int timeout)
//int 	libusb_interrupt_transfer (libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *transferred, unsigned int timeout)

#define USB_VENDOR_ID	    0x4143      /* USB vendor ID used by the device* 0x0483 is STMs ID*/
#define USB_PRODUCT_ID	    0x5407      /* USB product ID used by the device */
#define USB_ENDPOINT_IN	    0x82   		/* endpoint address */
#define USB_ENDPOINT_OUT	0x02   		/* endpoint address */
#define USB_TIMEOUT	        5000   		/* Connection timeout (in ms) */

static libusb_context *ctx = NULL;
static libusb_device_handle *handle;
static uint8_t receiveBuf[64];
uint8_t transferBuf[64];
uint16_t counter=0;

/*
 * Read a packet
 */
static int usb_read(void)
{
	int nread=0, ret=0;
	ret = libusb_bulk_transfer(handle, USB_ENDPOINT_IN, receiveBuf, sizeof(receiveBuf),&nread, USB_TIMEOUT);
	if (ret)
	{
		printf("[usb_read] ERROR in bulk read: %d\n", ret);
		return -1;
    }
	else
	{
		printf("[usb_read] %d receive %d bytes from device: %s\n", ++counter, nread, receiveBuf);
		printf("%s", receiveBuf);  //Use this for benchmarking purposes
		for(int counter=0;counter<nread;counter++)
		{
			printf("\n receiveBuf[%d] : %0xh", counter,receiveBuf[counter]);
		}
		return 0;
    }
}//static int usb_read(void) end


/*
 * write a few bytes to the device
 *
 */
uint16_t count=0;
static int usb_write(unsigned char *data,int cmdlength)
{
	int n=0, ret=0;
	int transferredLength =0;
    //count up
    //n = sprintf(transferBuf, "%d\0",count++);
    n = sprintf(data, "%d\0",count++);
    
    
    //write transfer
    //probably unsafe to use n twice...
    //int 	libusb_bulk_transfer (libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *transferred, unsigned int timeout)
	ret = libusb_bulk_transfer(handle, USB_ENDPOINT_OUT, data, cmdlength,&transferredLength, USB_TIMEOUT);
    //Error handling
    switch(ret)
    {
        case 0:
            printf("\n[usb_write] send %d bytes to device\n", transferredLength);
            return 0;
        case LIBUSB_ERROR_TIMEOUT:
            printf("\n[usb_write] ERROR in bulk write: %d Timeout\n", ret);
            break;
        case LIBUSB_ERROR_PIPE:
            printf("\n[usb_write] ERROR in bulk write: %d Pipe\n", ret);
            break;
        case LIBUSB_ERROR_OVERFLOW:
            printf("\n[usb_write] ERROR in bulk write: %d Overflow\n", ret);
            break;
        case LIBUSB_ERROR_NO_DEVICE:
            printf("\n[usb_write] ERROR in bulk write: %d No Device\n", ret);
            break;
        default:
            printf("\n[usb_write] ERROR in bulk write: %d\n", ret);
            break;

    }
    return -1;

}

/*
 * on SIGINT: close USB interface
 * This still leads to a segfault on my system...
 */
static void sighandler(int signum)
{
    printf( "\nInterrupt signal received\n" );
	if (handle){
        libusb_release_interface (handle, 0);
        printf( "\nInterrupt signal received1\n" );
        libusb_close(handle);
        printf( "\nInterrupt signal received2\n" );
	}
	printf( "\nInterrupt signal received3\n" );
	libusb_exit(NULL);
	printf( "\nInterrupt signal received4\n" );

	exit(0);
}

int main(int argc, char **argv)
{
	
    //Pass Interrupt Signal to our handler
	signal(SIGINT, sighandler);

	libusb_init(&ctx);
	libusb_set_option(ctx, 3);

    //++Open Device with VendorID and ProductID
	handle = libusb_open_device_with_vid_pid(ctx,USB_VENDOR_ID, USB_PRODUCT_ID);
	if (!handle) 
	{
		printf("\n[main] device not found");
		return 1;
	}
	printf("[main] device found");
	
	int Interface = 0,r=0;
	//++Claim Interface 0 from the device
    r = libusb_claim_interface(handle, Interface);
	if (r < 0) 
	{
		fprintf(stderr, "usb_claim_interface error %d\n", r);
		return 2;
	}
	printf("\n[main] Interface claimed");
	
	unsigned char cmd1[15] = { 0x00, 0xA4, 0x04, 0x00, 0x0A, 0xA0, 0x00, 0x00, 0x00, 0x62, 0x03, 0x01, 0x0C, 0x06, 0x01 };
    unsigned char cmd2[4]  = { 0x00, 0x00, 0x00, 0x00 };
    unsigned char cmd3[5] = { 0xFF,0xCA,0x00,0x00,0x00 };
    
    //GetVersionString
    unsigned char cmd4[5] = { 0xE0,0x00,0x00,0x18,0x00 };
    
    //Get Control Parameter
    unsigned char cmd5[5] = { 0xE0,0x00,0x00,0x21,0x00 };
    
    //Enable Buzzer
    unsigned char cmd6[6] = { 0xE0,0x00,0x00,0x28,0x01,0x01 };
    
    unsigned char cmd7[5] = { 0xE0, 0x0, 0x0, 0x24, 0x0};
    
    unsigned char cmd8[6]  = { 0xE0, 0x0, 0x0, 0x29, 0x1, 0x00 };
    
    
    unsigned char cmd9[5]  = { 0xFF, 0xCA, 0x01, 0x00, 0x00 };

    //usb_read();
	usb_write(cmd9,5);
	usb_read();
		
    //++never reached
	libusb_close(handle);
	printf("\n[main] Close  Device Handel");
	libusb_exit(NULL);
	printf("\n[main] Destroy libusb library");
	
	return 0;
	
}//int main(int argc, char **argv) end

/*
 //++Buzzer
 receiveBuf[0] : 81h
 receiveBuf[1] : 0h
 receiveBuf[2] : 0h
 receiveBuf[3] : 0h
 receiveBuf[4] : 0h
 receiveBuf[5] : 1h
 receiveBuf[6] : 30h
 receiveBuf[7] : 42h
 receiveBuf[8] : 0h
 receiveBuf[9] : 0h
 
 */
