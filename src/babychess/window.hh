#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED

#include "gtk_object.hh"
#include "pointer.hh"
#include "string.hh"
//#include "dbg.hh"

#include <gtk/gtk.h>

class CH_Window
	:  public Gtk_Object 
{
public:

	CH_Window();

	bool Open(String caption,
		  Point size/*= Point(-1)*/,
		  const char *icon_name);

	void Set(Gtk_Object &new_gtk_object)
		{
			assert (Is());
			assert (! gtk_object.Is());
			gtk_container_add(GTK_CONTAINER(Get()),
					  new_gtk_object.Get());
			gtk_object= new_gtk_object;
		}
	
	void Set_Caption(String text)
		{
			gtk_window_set_title(GTK_WINDOW(Get()),
					     text()); 
		}

	String Get_Caption() const
		{
			String result= gtk_window_get_title(GTK_WINDOW(Get()));
			result.Isolate();
			return result; 
		}

	static void Loop();

	static void Inc() 
		{
			++ window_count;
		}
	static void Dec();

private:

	Pointer <Gtk_Object> gtk_object;
	/* Content 
	 */

	static int window_count;
	/* Number of open windows.  
	 */
	static bool in_loop;

	void Wdg_Gtk_Delete(); 
	bool Wdg_Close_Query()
		{
			//Dbg_Function();
			return gtk_object().Wdg_Close_Query(); 
		}
};

#endif /* ! WINDOW_INCLUDED */
