#include "window.hh"

#include "get_dir.hh"
#include "file_name.hh"

int CH_Window::window_count= 0;
bool CH_Window::in_loop= false; 

void CH_Window::Wdg_Gtk_Delete()
{
	//Dbg("Wdg_Gtk_Delete()"); 

	gtk_object= NULL; 
	Dec();
}

bool CH_Window::Open(String caption,
		  Point size,
		  const char *icon_name)
{
	GtkWidget *gtkwidget=
		gtk_window_new(GTK_WINDOW_TOPLEVEL);
	if (gtkwidget == NULL)
		return false;
	gtk_window_set_title(GTK_WINDOW(gtkwidget),
			     caption()); 

	static GdkPixbuf *icon= NULL;

	if (icon == NULL)
	{
		String file_name= Get_Dir(id_install) / "desktop" / icon_name;
		
		icon= gdk_pixbuf_new_from_file(file_name(),
					       NULL);
	}

	if (icon != NULL)
	{
		gtk_window_set_icon(GTK_WINDOW(gtkwidget),
				    icon);
	}

	if (size.x != -1)
		gtk_window_set_default_size(GTK_WINDOW(gtkwidget),
					    size.x, size.y);
	else
		gtk_window_set_policy(GTK_WINDOW(gtkwidget), FALSE, FALSE, TRUE); 
	gtk_widget_show(gtkwidget);
	Attach(gtkwidget, false);
	++ window_count;
	return true;
}

CH_Window::CH_Window()
{
}

void CH_Window::Loop()
{
	assert (window_count >= 0); 

	if (window_count == 0)
		return; 
	in_loop= true;
	gtk_main();
	in_loop= false; 
}

void CH_Window::Dec()
{
	assert (window_count >= 1);
	-- window_count; 
	if (window_count == 0)
	{
		if (in_loop)
			gtk_main_quit();
	}
}
