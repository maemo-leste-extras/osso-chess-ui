#ifndef SERVER_IMAGE_BINFILE_INCLUDED
#define SERVER_IMAGE_BINFILE_INCLUDED

/* Write/read a SERVER_IMAGE to/from a file by using a temporary
   CLIENT_IMAGE.
 */

#include "server_image.hh"
#include "client_image_stream.hh"

#define SERVER_IMAGE_STREAM_SUFFIX CLIENT_IMAGE_STREAM_SUFFIX 

bool
Write_Stream(Stream &, 
	   const Server_Image &);

bool
Read_Stream(Stream &,
	    Server_Image &);

#endif /* ! SERVER_IMAGE_BINFILE_INCLUDED */

