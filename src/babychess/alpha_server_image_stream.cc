#include "alpha_server_image_stream.hh"

#include "alpha_client_image_stream.hh"

bool
Read_Stream(Stream &stream,
	    Alpha_Server_Image &alpha_server_image)
{
	Alpha_Client_Image alpha_client_image;

  
  GdkPixbuf *image=NULL;
  GError *error=NULL;
  image=gdk_pixbuf_new_from_file(stream.Get_Filename()(),&error);
  g_assert(image);
  //alpha_client_image.Get_Alpha_Image().Set_Pixbuf_for_alpha(image);
  alpha_client_image.Get_Image().Set_Pixbuf(alpha_client_image.Get_Alpha_Image().Set_Pixbuf_for_alpha(image));
  
	/*if (! Read_Stream(stream, alpha_client_image))
	{
		return false;
	}*/

	if (! (alpha_server_image.Get_Image()
	       .Create(alpha_client_image.Get_Image().Get_Size())   &&
	       alpha_server_image.Get_Alpha_Image()
	       .Create(alpha_client_image.Get_Alpha_Image().Get_Size())))
	{
		return false;
	}

	if (! (alpha_server_image.Get_Image().Has_Canvas()   &&
	       alpha_server_image.Get_Alpha_Image().Has_Canvas()))
	{
		return false;
	}

	alpha_client_image.Get_Image().Send_Rect(alpha_server_image.Get_Image()(),
						 alpha_client_image.Get_Image().Get_Size(),
						 Point(0));
	alpha_client_image.Get_Alpha_Image().Send_Rect(alpha_server_image.Get_Alpha_Image()(),
						       alpha_client_image.Get_Alpha_Image().Get_Size(),
						       Point(0));

	return true;
}
