#ifndef CLIENT_IMAGE_INCLUDED
#define CLIENT_IMAGE_INCLUDED

/* See etc/GDK-PIXBUF for why GdkPixbuf is not used. 
 */

#include "point.hh"
#include "pointer.hh"
#include "canvas.hh"

class Client_Image
	:  public Pointed
{
public:

	typedef guint32 Pixel; 

	Client_Image()
		:  image(NULL)
		{
		}
	bool Is() const
		{
			return image != NULL; 
		}
	void Free()
		{
			if (! Is())
				return;
      g_object_unref(image);
			image= NULL; 
		}
	~Client_Image()
		{
			if (Is())
				Free(); 
		}

	Point Get_Size() const
		{
			assert (Is());
      return Point(gdk_pixbuf_get_width(image),
				     gdk_pixbuf_get_height(image));
		}

	void Get_Pixel(int x, 
		       int y,
		       int &r,
		       int &g,
		       int &b) const
		{
      guchar *pixels, *p;
      int width, height, rowstride, n_channels;
      n_channels = gdk_pixbuf_get_n_channels (image);
      
      
      g_assert (gdk_pixbuf_get_colorspace (image) == GDK_COLORSPACE_RGB);
      g_assert (gdk_pixbuf_get_bits_per_sample (image) == 8);

      width = gdk_pixbuf_get_width (image);
      height = gdk_pixbuf_get_height (image);

      g_assert (x >= 0 && x < width);
      g_assert (y >= 0 && y < height);

      rowstride = gdk_pixbuf_get_rowstride (image);
      pixels = gdk_pixbuf_get_pixels (image);

      p = pixels + y * rowstride + x * n_channels;
      
      
      if (p[3]==0)
      {
      r=p[0];
      g=p[1];
      b=p[2];
      }
      else
      {
        r=0;
        b=0;
        g=0;
      }
		}

	void Get_Pixel(int x, 
		       int y,
		       guint32 &pixel) const
		{
      guchar *pixels, *p;
      int width, height, rowstride, n_channels;
      n_channels = gdk_pixbuf_get_n_channels (image);
      
      
      g_assert (gdk_pixbuf_get_colorspace (image) == GDK_COLORSPACE_RGB);
      g_assert (gdk_pixbuf_get_bits_per_sample (image) == 8);

      width = gdk_pixbuf_get_width (image);
      height = gdk_pixbuf_get_height (image);

      g_assert (x >= 0 && x < width);
      g_assert (y >= 0 && y < height);

      rowstride = gdk_pixbuf_get_rowstride (image);
      pixels = gdk_pixbuf_get_pixels (image);

      p = pixels + y * rowstride + x * n_channels;
      pixel=0;
      if (p[3]==0)
      {
      pixel |= (p[0]&0x00f8) >> 3;
			pixel |= (p[1]&0x00fc) << 3;
			pixel |= (p[2]&0x00f8) << 8;      
      }
      else
      {
        pixel=0;
      }
		}

	bool Create(Point size)
		{
			assert (size.x > 0 && size.y > 0); 
			assert (! Is()); 

      image=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,size.x,size.y);

			return image != NULL; 
		}

	void Draw_Pixel(int x,
			int y,
			int r,
			int g,
			int b)
		{
      
			assert (Is());

      guchar *pixels, *p;
      int width, height, rowstride, n_channels;
      n_channels = gdk_pixbuf_get_n_channels (image);
      
      
      g_assert (gdk_pixbuf_get_colorspace (image) == GDK_COLORSPACE_RGB);
      g_assert (gdk_pixbuf_get_bits_per_sample (image) == 8);

      width = gdk_pixbuf_get_width (image);
      height = gdk_pixbuf_get_height (image);

      g_assert (x >= 0 && x < width);
      g_assert (y >= 0 && y < height);

      rowstride = gdk_pixbuf_get_rowstride (image);
      pixels = gdk_pixbuf_get_pixels (image);

      p = pixels + y * rowstride + x * n_channels;
      p[0] = r;
      p[1] = g;
      p[2] = b;
      
		}

	void Draw_Pixel(int x,
			int y,
			guint32 pixel)
		{
      
			assert (Is());

      guchar *pixels, *p;
      guchar r=0,g=0,b=0;
      int width, height, rowstride, n_channels;
      r= (pixel << 3) & 0xf8;
			g= (pixel >> 3) & 0xfc;
			b= (pixel >> 8) & 0xf8;
      n_channels = gdk_pixbuf_get_n_channels (image);
      
      
      g_assert (gdk_pixbuf_get_colorspace (image) == GDK_COLORSPACE_RGB);
      g_assert (gdk_pixbuf_get_bits_per_sample (image) == 8);

      width = gdk_pixbuf_get_width (image);
      height = gdk_pixbuf_get_height (image);

      g_assert (x >= 0 && x < width);
      g_assert (y >= 0 && y < height);

      rowstride = gdk_pixbuf_get_rowstride (image);
      pixels = gdk_pixbuf_get_pixels (image);

      p = pixels + y * rowstride + x * n_channels;
      p[0] = r;
      p[1] = g;
      p[2] = b;
      p[3] = 255;
		}
  GdkPixbuf *Get_Pixbufimage(void)
    {
      return image;
    }
  void Set_Pixbuf(GdkPixbuf *theimage)
    {
      gdk_pixbuf_render_pixmap_and_mask(theimage,&pixmap, &tilemask,128);
      image=theimage;
    }
    
  
  GdkPixbuf* Set_Pixbuf_for_alpha(GdkPixbuf *theimage)
    {
      /*TODO: use only GDK functions, not this way, to create the mask*/
      int height=0,width=0,row=0,col=0, rowstride,rowstrider, n_channels;;      
      guchar *pixels, *p,*pixelsr, *pr;
      GdkPixbuf *returnimage=NULL;
      returnimage=gdk_pixbuf_copy(theimage);
      n_channels = gdk_pixbuf_get_n_channels (theimage);
      
      g_assert (gdk_pixbuf_get_colorspace (theimage) == GDK_COLORSPACE_RGB);
      g_assert (gdk_pixbuf_get_bits_per_sample (theimage) == 8);

      width = gdk_pixbuf_get_width (theimage);
      height = gdk_pixbuf_get_height (theimage);


      rowstride = gdk_pixbuf_get_rowstride (theimage);
      pixels = gdk_pixbuf_get_pixels (theimage);
      rowstrider = gdk_pixbuf_get_rowstride (returnimage);
      pixelsr = gdk_pixbuf_get_pixels (returnimage);

      for (row= 0; row < height; ++row)
		    for (col= 0; col < width; ++col)
		    {
			    guchar mask= 0xFF;
          p = pixels + row * rowstride + col * n_channels;
          pr = pixelsr + row * rowstrider + col * n_channels;
				  mask= p[3];
          pr[0]=p[0]&mask;
          pr[1]=p[1]&mask;
          pr[2]=p[2]&mask;
          pr[3]=255;
				  p[0]=mask;
          p[1]=mask;
          p[2]=mask;
          p[3]=255;
		     }
      image=theimage;
      return returnimage;
    }
	bool Send_Rect(Canvas &, 
		       Rect ,
		       Point ); 
	bool Receive_Rect(const Canvas &canvas,
			  Rect);
	/* Must be closed.  A new client image is created which in
	 * taken from CANVAS. 
	 */

	static bool Draw_Stretch_Canvas(Canvas &,
					const Canvas &,
					Rect &,
					Rect &,
					bool anti_alias= true);
	static bool Draw_Stretch_Canvas2(Canvas &,
					const Canvas &,
					Rect &,
					Rect &,
					bool anti_alias= true);

private:

  GdkBitmap *tilemask;
  GdkPixmap *pixmap;
  GdkPixbuf *image;
}; 

#endif /* ! CLIENT_IMAGE_INCLUDED */
