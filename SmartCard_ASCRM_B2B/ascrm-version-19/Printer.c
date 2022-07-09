//22.03.2012 Inesoft
//gcc -O2 -o hd_SPP_02_111111_0140 hd_SPP_02_111111_0140.c

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/io.h>

#include <fcntl.h> /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define BAUDRATE B115200

#define PRINTER_NORMAL	0x00

#define PAPER_EMPTY	0x01
#define HEAD_OPEN	0x02
#define CUTTER_ERROR	0x04
#define PAPER_NEAR_END	0x08

#define Presenter_in_jam	0x02
#define Presenter_center_jam	0x04
#define BM_find				0x08

unsigned char send_bytes[] = { 0x1D, 0x72, 0x01};

int main(int argc, char *argv[]) { 
	if (argc != 2) { 
        fprintf(stderr, "Usage: %s port\nFor example: %s /dev/ttyS0\n", argv[0], argv[0]); 
        exit(0); 
    } 

	int fd = open (argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		printf ("Port do not opened\n");
        exit (1);
	}

	// set new parameters to the serial device
	struct termios newtio;
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag |= (IXON | IXOFF | IXANY);

	// set to 8N1
	newtio.c_cflag &= ~PARENB;
	newtio.c_cflag &= ~CSTOPB;
	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8;

	newtio.c_iflag = IGNPAR;

	// output mode to
	//newtio.c_oflag = 0;
	newtio.c_oflag |= OPOST;

	// set input mode (non-canonical, no echo,...) 
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME] = 10; // inter-character timer 1 sec 
	newtio.c_cc[VMIN] = 0; // blocking read disabled  

	tcflush(fd, TCIFLUSH);
	if (tcsetattr(fd, TCSANOW, &newtio)) {
    	printf("Could not set the serial settings!\n");
	    return -99;
	}

	fcntl(fd, F_SETFL, 0);

    while(1) {
   		int n = write(fd, send_bytes, 3);  //Send data
	    if (n < 0)
      		printf("write() failed! %d\n", n);

/* read characters into our string buffer until we get a CR or NL */
		char buffer[255];  /* Input buffer */
		char *bufptr;      /* Current char in buffer */
		int  nbytes;       /* Number of bytes read */
		int  _bytes = 0;       /* Number of bytes read */
		bufptr = buffer;
		while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0) {
	  		bufptr += nbytes;
			_bytes += nbytes; 
//      		printf("%x\n", *bufptr);
	  		if (_bytes >= 7)
            	break;
		}

		if ((buffer[0] == 0x13) && (buffer[1] == 0x72)) {
			printf ("********** STATUS **********\n");
			if (buffer[2] & PAPER_EMPTY) 
				printf ("  PAPER NOT IN\n");
			if (buffer[2] & HEAD_OPEN) 
				printf ("  HEAD OPEN\n");
			if (buffer[2] & CUTTER_ERROR) 
				printf ("  CUTTER ERROR\n");
			if (buffer[2] & PAPER_NEAR_END) 
				printf ("  PAPER NEAR END\n");
			if (buffer[3] & Presenter_in_jam) 
				printf ("  Presenter in jam : Error\n");
			if (buffer[3] & Presenter_center_jam) 
				printf ("  Presenter center jam : Error\n");
			if (buffer[3] & BM_find) 
				printf ("  BM find : Error\n");

			if (buffer[2] == PRINTER_NORMAL) 
				printf ("  PRINTER NORMAL STATUS\n");
			printf ("****************************\n");
		}

		printf("wait to exit....\n");
		usleep(1000000);
	}//while end.

} //main end.

