#ifndef GTK_OBJECT_INCLUDED
#define GTK_OBJECT_INCLUDED

#include "pointer.hh"
#include "canvas.hh"
#include "stretchness.hh"
#include "mouse_action.hh"
#include "mouse_button.hh"

#include <gtk/gtk.h>

class Gtk_Object
	:  virtual public Pointed
{
public:

	Gtk_Object()
		:  gtkwidget(NULL)
		{
		}
	virtual ~Gtk_Object()
		{
			assert (gtkwidget == NULL);
		}

	virtual void Wdg_Close()
		{
		}
 	void Attach(GtkWidget *new_gtkwidget,
		    bool expose_events); 

	void Close(bool force= false); 

	bool Is() const
		{
			return gtkwidget != NULL;
		}

	GtkWidget *Get() const
		{
			return gtkwidget;
		}

	bool Set_Size(int a, int b)
	{
		if (gtkwidget == NULL)
			return false;
		gtk_widget_set_size_request(gtkwidget,a,b);
		//gtk_widget_set_usize(gtkwidget, a, b);
		return true;
	}

	Point Get_Size() const
		{
			assert (Is());

			Point result (gtkwidget->allocation.width,
				      gtkwidget->allocation.height);

			return result;
		}

	virtual void Wdg_Do_Close();

	virtual void Wdg_Get_Size_Request(int & /* width */ ,
					  int & /* height */ ,
					  bool & /* resizeable */ )
		/* Return preferred size, and whether widget can be
		 * resized.  The widget may check width and height.
		 * If one of them is not -1, then the returned value
		 * should not be bigger than that value. 
		 */
		{
		}

	virtual void Wdg_Mouse(Mouse_Action, 
			       Mouse_Button,
			       Point,
			       bool)
		{
		}
	virtual void Wdg_Mouse_Cursor()
		{
		}
	virtual void Wdg_Leave_Notify()
		{
		}
	virtual void Wdg_Draw(Canvas &)
		{
		}
	virtual bool Wdg_Key_Press(guint)
		/* Return value:  whether the key was processed. 
		 */
		{
			return false;
		}
	virtual bool Wdg_Close_Query()
		/* Whether the widget can be closed.  Called before Wdg_Close(). 
		 */
		{
			return true; 
		}

	void Connect_Mouse_Signals(GtkWidget *gtkwidget);
	void Connect_Key_Signals(GtkWidget *gtkwidget);
	/* To be called after opening when these signals are used. 
	 */

private:

	GtkWidget *gtkwidget;

	virtual void Wdg_Gtk_Delete()
		/* When Signal_Delete() is received.  Only called on
		 * windows. 
		 */
		{
		}
	virtual void Wdg_Size(Point)
		{
		}

	/* To do:  check return values of signals handlers. 
	 */
	static gint Signal_Delete(GtkWidget *this_gtkwidget,
				  GdkEvent  * /* event */ ,
				  gpointer   data); 
	/* Called before destruction of an object.  Only
	 * called for windows.   
	 */


	static void Signal_Destroy(GtkWidget *,
				   gpointer   data); 
	/* Called after destruction. 
	 */

	static void Signal_Expose(GtkWidget *this_gtkwidget,
				  GdkEventExpose *event,
				  gpointer data); 
	static void Signal_Size_Allocate(GtkWidget *,
					 GtkAllocation *,
					 gpointer);
	static void Signal_Size_Request(GtkWidget *,
					GtkRequisition *,
					gpointer);
	static gint Signal_Motion_Notify_Event(GtkWidget *,
					       GdkEventMotion *,
					       gpointer);
	static gint Signal_Leave_Notify_Event(GtkWidget *,
					       GdkEventMotion *,
					       gpointer);
	static gint Signal_Button_Press_Event(GtkWidget *,
					      GdkEventButton *,
					      gpointer);
	static gint Signal_Button_Release_Event(GtkWidget *,
						GdkEventButton *,
						gpointer);
	static gint Signal_Key_Press_Event(GtkWidget *,
					   GdkEventKey *,
					   gpointer);
};

#endif /* ! GTK_OBJECT_INCLUDED */
