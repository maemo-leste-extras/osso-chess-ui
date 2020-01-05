#ifndef SERVER_IMAGE_INCLUDED
#define SERVER_IMAGE_INCLUDED

/* As server image is stored on the display server.  It can be quickly
   transfered to the screen. 
 */

#include "canvas.hh"
#include "widget.hh"

#include <gdk/gdk.h>

class Server_Image
{
public:

	Server_Image()
		:  pixmap(NULL)
		{
		}
	bool Is() const
		{
			return pixmap != NULL; 
		}
	~Server_Image()
		{
			if (Is())
				Free(); 
		}


	Canvas &Get_Canvas() const
	/* Has_Canvas() must have returned TRUE. 
	 */
		{
			assert (canvas.Is());
			return ((Server_Image *)this)->canvas; 
		}
	Canvas &operator () () const
	/* Alias for Get_Canvas(). 
	 */
		{
			return Get_Canvas(); 
		}
	Canvas *operator->() const
		{
			return &Get_Canvas(); 
		}

	Point Get_Size() const
	/* Image must have been created. */
		{
			assert (Is()); 
			return size; 
		}

	void Free();
	/* Do nothing if image is not created. 
	 */

	bool Create(Point new_size);
	/* Neither X nor Y of SIZE must be 0.  Return TRUE on success.
	 * Must not be allocated. 
	 */

	bool Has_Canvas() const
	/* Whether a canvas could be created.
	 */
		{
			assert (Is()); 
			if (canvas.Is())
				return true;
			return ((Server_Image *)this)->Open_GC();
		}

private:

	GdkPixmap *pixmap;
	GdkGC     *gc;
	Canvas     canvas;
	Point      size;

	bool Open_GC();
};

#endif /* ! SERVER_IMAGE_INCLUDED */ 
