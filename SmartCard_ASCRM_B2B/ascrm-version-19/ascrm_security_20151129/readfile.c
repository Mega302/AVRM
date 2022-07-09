/***********************************************************************
* Code listing from "Advanced Linux Programming," by CodeSourcery LLC  *
* Copyright (C) 2001 by New Riders Publishing                          *
* See COPYRIGHT for license information.                               *
***********************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Read the contents of FILENAME into a newly-allocated buffer.  The
   size of the buffer is stored in *LENGTH.  Returns the buffer, which
   the caller must free.  If FILENAME doesn't correspond to a regular
   file, returns NULL.  */

char* read_file (const char* filename, size_t* length)
{
	  int fd;
	  struct stat file_info;
	  //char* buffer;

	  /* Open the file.  */
	  fd = open (filename, O_RDONLY);

	  /* Get information about the file.  */
	  fstat (fd, &file_info);
	  *length = file_info.st_size;
	  /* Make sure the file is an ordinary file.  */
	  if (!S_ISREG (file_info.st_mode)) {
	    /* It's not, so give up.  */
	    close (fd);
	    return NULL;
	  }

	  /* Allocate a buffer large enough to hold the file's contents.  */
	  //buffer = (char*) malloc (*length);

          char buffer[4096];

	  /* Read the file into the buffer.  */
	  int size=0;
          size=read (fd, buffer, *length);
          printf("\n bytes read by linux read(): %d bytes",size);
          printf("\n File contents: \n %s",buffer);
          

	  /* Finish up.  */
	  close (fd);
	  //return buffer;
          return NULL;

}


int main()
{
         
 
           FILE *fp;
	   int c;
	  
	   fp = fopen("atvm.ini","r");
	   while(1)
	   {
	      c = fgetc(fp);
	      if( feof(fp) )
	      { 
		  break ;
	      }
	      printf("%c", c);
	   }
	   fclose(fp);
	   return(0);


}
