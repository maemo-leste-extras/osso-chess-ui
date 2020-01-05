#include "server_image_stream.hh"
#include <libosso.h>
#include "chess_log.h"

bool
Write_Stream(Stream &, 
	     const Server_Image &server_image)
{
	assert (server_image.Is());

	if (! server_image.Has_Canvas())
		return false;

	Client_Image client_image;
	if (! client_image.Receive_Rect(server_image(), 
			     Rect(Point(0),
				  server_image.Get_Size())))
		return false;

    return true;
}

bool
Read_Stream(Stream &stream,
	    Server_Image &server_image)
{
	Client_Image client_image;

	osso_log(LOG_DEBUG,"Read SI: Read stream");
  
  /*Loading the client image with the pixbuf lib*/
  GdkPixbuf *image=NULL;
  GError *error=NULL;
  image=gdk_pixbuf_new_from_file(stream.Get_Filename()(),&error);

  g_assert(image);
  client_image.Set_Pixbuf(image);
  
	/*if (! Read_Stream(stream,
			client_image))
		return false;*/

	osso_log(LOG_DEBUG,"Read SI: Create");
	if (! server_image.Create(client_image.Get_Size()))
		return false;

	osso_log(LOG_DEBUG,"Read SI: Has Canvas");
	if (! server_image.Has_Canvas())
		return false;

	osso_log(LOG_DEBUG,"Read SI: Send rect");
	client_image.Send_Rect(server_image.Get_Canvas(),
			       Rect(client_image.Get_Size()),
			       Point(0));

	return true;
}
