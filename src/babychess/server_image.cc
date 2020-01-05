#include "server_image.hh"
#include <libosso.h>
#include "chess_log.h"

void Server_Image::Free()
{
	if (! Is())
		return; 

	if (canvas.Is())
	{
		canvas.Close();
		gdk_gc_unref(gc); 
	}
	gdk_pixmap_unref(pixmap);
	pixmap= NULL; 
}


bool Server_Image::Open_GC()
{
	assert (gc == NULL);

	osso_log(LOG_DEBUG,"SImage: New GC");
	gc= gdk_gc_new(pixmap); 
	if (gc == NULL)
		return false;

	osso_log(LOG_DEBUG,"SImage: Open canvas");
	canvas.Open(pixmap, gc); 
	return true;
}

bool Server_Image::Create(Point new_size)
		{
			assert (new_size.x > 0 && new_size.y > 0); 
			assert (! Is()); 

			osso_log(LOG_DEBUG,"SImage: Create");
			osso_log(LOG_DEBUG,"SImage: depth %d",Widget::Get_Depth());
			pixmap= gdk_pixmap_new(NULL,
					       new_size.x, new_size.y,
					       Widget::Get_Depth());
               
      g_assert (pixmap);
			gc= NULL; 
			size= new_size; 
			return true; 
		}
