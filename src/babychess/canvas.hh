#ifndef CANVAS_INCLUDED
#define CANVAS_INCLUDED

#include "rect.hh"
#include "rgb.hh"
#include "string.hh"
#include "display_type.hh"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

class Canvas
{
public:

	class Color
		:  public GdkColor 
	{
	public:

		enum Type {t_white, t_black};

		Color()
			{
			}
		Color(const GdkColor &gdkcolor)
			:  GdkColor(gdkcolor)
			{
			}
		Color(RGB rgb);

		Color(Type type)
			{
				if (type == t_white)
					gdk_color_white(gdk_colormap_get_system(),
							this);
				else
					gdk_color_black(gdk_colormap_get_system(),
							this);
			}
		Color(gulong new_pixel)
			{
				pixel= new_pixel;
			}
	
		bool operator == (Color color)
			{
				return pixel == color.pixel;
			}
	};

	enum Line_Join_Style {ljs_miter= GDK_JOIN_MITER,
			      ljs_round= GDK_JOIN_ROUND,
			      ljs_bevel= GDK_JOIN_BEVEL,
			
			      ljs_default= ljs_miter};
	enum Line_End_Style {les_not_last= GDK_CAP_NOT_LAST,
			     les_butt= GDK_CAP_BUTT,
			     les_round= GDK_CAP_ROUND,
			     les_projecting= GDK_CAP_PROJECTING,
		       
			     les_default= les_not_last};


	typedef int Draw_Mode; 
	enum
	{ dm_copy= GDK_COPY,            
	  dm_and=  GDK_AND,
    dm_set=  GDK_SET,
	  dm_or=   GDK_OR,
	  dm_xor=  GDK_XOR,
	  dm_andnot=  GDK_AND_INVERT,
	  dm_not=  GDK_INVERT,
	  dm_andrev=  GDK_AND_REVERSE,
	  dm_ornot=   GDK_OR_INVERT,
	  dm_copynot=   GDK_COPY_INVERT,
	  dm_nand=   GDK_NAND,
	  dm_nor=   GDK_NOR,
	 };


	Canvas()
		:  drawable (NULL),
		   gc(NULL) 
		{
		}

	Canvas(GdkDrawable *new_drawable,
	       GdkGC       *new_gc)
		:  drawable(new_drawable),
		   gc(new_gc)
		{
		}
	bool Is() const
		{
			assert ((drawable == NULL) != (gc != NULL)); 
			return drawable != NULL; 
		}
	void Open(GdkDrawable *new_drawable,
		  GdkGC       *new_gc)
		{
			assert (drawable == NULL);
			assert (gc == NULL);
			drawable= new_drawable;
			gc= new_gc;
		}
	void Close()
		{
			assert (Is()); 
			drawable= NULL;
			gc= NULL; 
		}

	/* Everything is drawn in foreground color.
	 */
	void Draw_Rect(Rect rect)
		{
			gdk_draw_rectangle(drawable,
					   gc,
					   TRUE,
					   rect.origin.x,
					   rect.origin.y,
					   rect.size.x,
					   rect.size.y); 
		}
	void Set_Color(Color color, 
		       bool foreground= true)
		{
			assert (Is());

			if (foreground)
				gdk_gc_set_foreground(gc,
						      & color);
			else
				gdk_gc_set_background(gc,
						      & color);
		}

	void Set_Line_Mode(int w,
			   Line_Join_Style j= ljs_default,
			   Line_End_Style e= les_default)
		{
			gdk_gc_set_line_attributes(gc,w,GDK_LINE_SOLID,
						   (GdkCapStyle)e, (GdkJoinStyle)j); 
					     
		}
	void Draw_Line(Point,
		       Point);
	/* WIDTH must be greater than zero. 
	 */

	void Draw_Stretch_Canvas(const Canvas &canvas,
				 Rect dst,
				 Rect src,
				 bool anti_alias= true);
	void Draw_Stretch_Canvas2(const Canvas &canvas,
				 Rect dst,
				 Rect src,
				 bool anti_alias= true);
	void Set_Draw_Mode(Draw_Mode draw_mode);

	void Draw_Canvas(const Canvas & /* canvas */ ,
			 Rect /* dst */ ,
			 Point /* src */ );

	Rect Get_Region() const
		{
			return Rect(0, 0, 1000, 1000); 
		}
	void Draw_Ellipse(Rect); 
	void Draw_Rectangle(Rect); 
	void Draw_Label(GtkWidget *w, gint size, Point point, String text);
	void Draw_Label(Point point, 
			String text/*, 
				     Display_Type*/);

	static Color Get_Color(RGB rgb)
		{
			return Color(rgb); 
		}

	static Color Get_White()
		{
			return Color(Color::t_white);
		}
	static Color Get_Black()
		{
			return Color(Color::t_black);
		}
	/* Colors have have all planes filled with 1 or 0.  
	 */
	static Color Get_Color_0()
		{
			return Color(Color::t_black);
		}
	static Color Get_Color_1()
		{
			return Color(Color::t_white);
		}

	void Draw_Arc(Rect , 
		      int ,
		      int )
		{
		}

	void Draw_Polygon(Point *,
			  int)
		{
		}
	void Draw_Polygon_Line(Point *,
			       int,
			       bool= true)
		{
		}

	GdkDrawable *Get_Drawable() const
		{
			return drawable; 
		}
	GdkGC *Get_GC() const
		{
			return gc; 
		}

	static int Get_Ascent()
		{
			if (! Load_Font())
				return 1;
			return font->ascent;
		}
	static int Get_Descent()
		{
			if (! Load_Font())
				return 1;
			return font->descent; 
		}

	static int Get_Label_Width(String text);
	
private:

	GdkDrawable *drawable;
	GdkGC       *gc; 

	static GdkFont *font;

	static bool Load_Font();
};

#endif /* ! CANVAS_INCLUDED */
