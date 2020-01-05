#include "client_image.hh"

#include <libosso.h>
#include <glib.h>
#include "chess_log.h"

bool Client_Image::Send_Rect(Canvas &canvas, 
			Rect dst,
			Point src)
{
	assert (Is());

	g_assert (canvas.Get_Drawable());
	g_assert (canvas.Get_GC());

	osso_log(LOG_DEBUG,"Client image: Send Rect");

	/*gdk_draw_image(canvas.Get_Drawable(),
		       canvas.Get_GC(),
		       image,
		       src.x, src.y,
		       dst.origin.x, dst.origin.y,
		       dst.size.x, dst.size.y);*/
  gdk_draw_pixbuf(canvas.Get_Drawable(),
		       canvas.Get_GC(),
		       image,
		       src.x, src.y,
		       dst.origin.x, dst.origin.y,
		       dst.size.x, dst.size.y,
            GDK_RGB_DITHER_NONE, 0,0);
	return true; 
}

bool
Client_Image::Receive_Rect(const Canvas &canvas,
			   Rect rect)
{
	assert (! Is());

	/*image= gdk_image_get(canvas.Get_Drawable(),
			     rect.origin.x,
			     rect.origin.y,
			     rect.size.x,
			     rect.size.y);*/
  image= gdk_pixbuf_get_from_drawable(NULL,canvas.Get_Drawable(),NULL,
			     rect.origin.x,
			     rect.origin.y,
            0,
            0,
			     rect.size.x,
			     rect.size.y);

	return image != NULL; 
}

bool
Client_Image::
Draw_Stretch_Canvas(Canvas &dst,
		    const Canvas &src,
		    Rect &dst_rect,
		    Rect &src_rect,
		    bool anti_alias)
{
        (void)anti_alias;
	//Dbg_Function();
	//Dbg_Int(anti_alias);

	Client_Image src_image;

	if (! src_image.Receive_Rect(src, src_rect))
		return false;

	Client_Image dst_image;
  
  GdkPixbuf *scaled_image=NULL;
  scaled_image=gdk_pixbuf_scale_simple(src_image.Get_Pixbufimage(),
  dst_rect.size.x,dst_rect.size.y,GDK_INTERP_NEAREST);
  dst_image.Set_Pixbuf(scaled_image);

	if (! dst_image.Send_Rect(dst, dst_rect, Point(0)))
		return false;

	return true; 
}

bool
Client_Image::
Draw_Stretch_Canvas2(Canvas &dst,
		    const Canvas &src,
		    Rect &dst_rect,
		    Rect &src_rect,
		    bool anti_alias)
{
	Client_Image src_image;
        (void)anti_alias;

	if (! src_image.Receive_Rect(src, src_rect))
		return false;

	Client_Image dst_image;
  
  GdkPixbuf *scaled_image=NULL;
  scaled_image=gdk_pixbuf_scale_simple(src_image.Get_Pixbufimage(),
  dst_rect.size.x,dst_rect.size.y,GDK_INTERP_NEAREST);
  dst_image.Set_Pixbuf(scaled_image);
  /*
	if (! dst_image.Create(dst_rect.size))
		return false;

	for (int x= 0; x < dst_rect.size.x; ++ x) 
	{
		for (int y= 0; y < dst_rect.size.y; ++ y)
		{
			int r= 0, g= 0, b= 0;
			src_image.Get_Pixel(src_rect.size.x * x / dst_rect.size.x,
					    src_rect.size.y * y / dst_rect.size.y,
					    r, g, b);

			if (r<128) r=0; else r=255;
			if (g<128) g=0; else g=255;
			if (b<128) b=0; else b=255;

			dst_image.Draw_Pixel(x, y, r, g, b); 
		}
	}*/


	if (! dst_image.Send_Rect(dst, dst_rect, Point(0)))
		return false;

	return true; 
}
