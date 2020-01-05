#include "canvas.hh"

#include "client_image.hh"
//#include "dbg.hh"

#include <pango/pango.h>

#include <limits.h> /* ULONG_MAX */

GdkFont *Canvas::font= NULL;

Canvas::Color::Color(RGB rgb)
{
	red=   rgb.r * 0x100;
	green= rgb.g * 0x100;
	blue=  rgb.b * 0x100;

	if (FALSE == gdk_colormap_alloc_color
	    (gdk_colormap_get_system(),
	     this,
	     FALSE,
	     TRUE))
	{
		gdk_color_white(gdk_colormap_get_system(),
				this);
	}
	else
	  {
	  }
}

void Canvas::Draw_Ellipse(Rect rect)
{
	assert (Is()); 

	gdk_draw_arc(drawable,
		     gc,
		     TRUE,
		     rect.origin.x,
		     rect.origin.y,
		     rect.size.x,
		     rect.size.y,
		     0,
		     64 * 360);
}

void Canvas::Draw_Rectangle(Rect rect)
{
	assert (Is()); 

	gdk_draw_rectangle(drawable,
		     gc,
		     FALSE,
		     rect.origin.x,
		     rect.origin.y,
		     rect.size.x,
		     rect.size.y);
}

void Canvas::Draw_Canvas(const Canvas &canvas,
		    Rect dst,
		    Point src)
{
	assert (Is() && canvas.Is());

	//gdk_draw_pixmap(drawable,
	gdk_draw_drawable(drawable,
			gc,
			canvas.drawable,
			src.x, src.y,
			dst.origin.x, dst.origin.y,
			dst.size.x, dst.size.y);
}

void Canvas::
Draw_Stretch_Canvas(const Canvas &canvas,
		    Rect dst,
		    Rect src,
		    bool anti_alias)
{
	Client_Image::Draw_Stretch_Canvas(*this, canvas, dst, src, anti_alias); 
}

void Canvas::
Draw_Stretch_Canvas2(const Canvas &canvas,
		    Rect dst,
		    Rect src,
		    bool anti_alias)
{
	Client_Image::Draw_Stretch_Canvas2(*this, canvas, dst, src, anti_alias); 
}

void Canvas::Draw_Label(GtkWidget *w, gint size, Point point,
			String text)
{
	assert (Is());

	PangoLayout *pl;
	
	PangoAttribute *pa;
	PangoAttrList *pal;

	pa = pango_attr_size_new(size);
	pa->start_index=0;
	pa->end_index=G_MAXINT;
	pal = pango_attr_list_new();
	pango_attr_list_insert(pal,pa);

	pl = gtk_widget_create_pango_layout(w,text());
	pango_layout_set_attributes(pl,pal);

	gdk_draw_layout(drawable, gc, point.x, point.y, pl);
}

void Canvas::Draw_Label(Point point,
			String text)
{

	assert (Is());

	if (! Load_Font())
		return;

	gdk_draw_text(drawable,
		      font,
		      gc,
		      point.x, point.y,
		      text(),
		      *text);
}

void Canvas::Draw_Line(Point from,
			   Point to)
{
  assert (Is());

  gdk_draw_line(drawable, gc,
		from.x, from.y,
		to.x, to.y);
}


void Canvas::Set_Draw_Mode(Draw_Mode draw_mode)
		{
			assert (Is());

			gdk_gc_set_function(gc,
					    (GdkFunction)draw_mode);
		}

bool Canvas::Load_Font()
{
	if (font != NULL)
		return true; 

	const char *font_name=
		"-*-*-bold-*-normal--*-200-*-*-*-*-iso8859-1"; 

	font= gdk_fontset_load(font_name);

	if (font == NULL)
	{
		return false;
	}

	return true; 
}

int Canvas::Get_Label_Width(String text)
{
	assert (text.Is());

	if (! Load_Font())
		return 1;

	gint lbearing, rbearing, width, ascent, descent;

	gdk_text_extents(font,
			 text(),
			 *text,
			 & lbearing,
			 & rbearing,
			 & width,
			 & ascent,
			 & descent);

	return width; 
}

