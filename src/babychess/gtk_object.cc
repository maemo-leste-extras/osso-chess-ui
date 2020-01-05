#include "gtk_object.hh"

//#include "dbg.hh"
#include "vector.hh"
#include "input_reader.hh"
//#include "gtk_timer.hh"

gint Gtk_Object::Signal_Delete(GtkWidget *this_gtkwidget,
			  GdkEvent  * /* event */ ,
			  gpointer   data)
{
	//Dbg_Function();

	/* Convert DATA to GTK_OBJECT and check
	 * consistency.  
	 */

	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);

//	Dbg_Int(gtk_object->force_close);

	if (/*(! gtk_object->force_close) && */
	    (! gtk_object->Wdg_Close_Query()))
		return TRUE;

	return FALSE;
}

void Gtk_Object::Signal_Destroy(GtkWidget *,
			   gpointer data)
{
//	Dbg_Function_();

	Gtk_Object &gtk_object= *((Gtk_Object *)data); 
	gtk_object.Wdg_Gtk_Delete(); 
	gtk_object.Wdg_Close(); 
	gtk_object.gtkwidget= NULL;
//	gtk_object.Wdg_Destroy();
}

void
Gtk_Object::Signal_Expose(GtkWidget *this_gtkwidget,
			  GdkEventExpose * /* event */ ,
			  gpointer data)
{
	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);

	Canvas canvas(this_gtkwidget->window,
		      this_gtkwidget->style->fg_gc[GTK_WIDGET_STATE(this_gtkwidget)]); 
	gtk_object->Wdg_Draw(canvas); 
	canvas.Set_Draw_Mode(Canvas::dm_copy);
	canvas.Set_Color(Canvas::Get_Black());
}

gint
Gtk_Object::Signal_Motion_Notify_Event(GtkWidget *this_gtkwidget,
				       GdkEventMotion *event,
				       gpointer data)
{
//	Dbg_Function_();

	assert (event != NULL);

	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);

	
	int x, y;
	GdkModifierType state;

	if (event->is_hint)
		gdk_window_get_pointer (event->window, &x, &y, &state);
	else
	{
	        x = (int)(event->x);
		y = (int)(event->y);
	}

	gtk_object->Wdg_Mouse(ma_move,
			      mb_left,
			      Point(x, y),
			      true);
	gtk_object->Wdg_Mouse_Cursor();

	return TRUE; 
}

gint
Gtk_Object::Signal_Leave_Notify_Event(GtkWidget *this_gtkwidget,
				       GdkEventMotion *event,
				       gpointer data)
{
	assert (event != NULL);

	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);

	gtk_object->Wdg_Leave_Notify();

	return TRUE; 
}

gint
Gtk_Object::Signal_Button_Press_Event(GtkWidget *this_gtkwidget,
				      GdkEventButton *event,
				      gpointer data)
{
//	Dbg_Function_();

	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);
	
	Mouse_Button buttons[4]= {mb_left, mb_left, mb_middle, mb_right};

	if (event->button < 1  ||
	    event->button > 3)
		return TRUE;
	gtk_object->Wdg_Mouse(ma_down, 
			      buttons[event->button],
			      Point(/*lrint*/(int)(event->x),
				    /*lrint*/(int)(event->y)),
			      true);
	return TRUE;
}

gint
Gtk_Object::Signal_Button_Release_Event(GtkWidget *this_gtkwidget,
					GdkEventButton *event,
					gpointer data)
{
//	Dbg_Function_();

	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);
	
	Mouse_Button buttons[4]= {mb_left, mb_left, mb_middle, mb_right};

	if (event->button < 1  ||
	    event->button > 3)
		return TRUE;
	gtk_object->Wdg_Mouse(ma_up, 
			      buttons[event->button],
			      Point((int)/*lrint*/(event->x),
				    (int)/*lrint*/(event->y)),
			      true);
	return TRUE; 
}

void Gtk_Object::Attach(GtkWidget *new_gtkwidget,
		   bool expose_events)
{
	assert (new_gtkwidget != NULL);
	assert (gtkwidget == NULL);
	gtkwidget= new_gtkwidget;

	gtk_signal_connect(GTK_OBJECT(gtkwidget), "delete_event",
			   GTK_SIGNAL_FUNC(Signal_Delete), this);
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "destroy",
			   GTK_SIGNAL_FUNC(Signal_Destroy), this);
	if (expose_events)
		gtk_signal_connect(GTK_OBJECT(gtkwidget), "expose_event",
				   GTK_SIGNAL_FUNC(Signal_Expose), this); 
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "size-allocate",
			   GTK_SIGNAL_FUNC(Signal_Size_Allocate), this); 
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "size-request",
			   GTK_SIGNAL_FUNC(Signal_Size_Request), this); 


//	force_close= false;
	/* For widgets for which Signal_Destroy() is
	 * called automatically. 
	 */
}

void Gtk_Object::Close(bool force)
{
	/*Dbg_Function(); 
	Dbg_Int(force); 
	*/

	assert (gtkwidget != NULL);

	if (force || Wdg_Close_Query())
	{
		Wdg_Do_Close();
		assert (gtkwidget == NULL);
	}
}

void
Gtk_Object::Signal_Size_Allocate(GtkWidget *this_gtkwidget,
				 GtkAllocation *allocation,
				 gpointer data)
{
	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);

	gtk_object->Wdg_Size(Point(allocation->width,
				   allocation->height)); 
}

void
Gtk_Object::Signal_Size_Request(GtkWidget *this_gtkwidget,
				GtkRequisition *requisition,
				gpointer data)
{
//	Dbg_Function_();
	Gtk_Object *gtk_object= (Gtk_Object *)data;
	assert (gtk_object != NULL);
	assert (gtk_object->gtkwidget == this_gtkwidget);

	int w= requisition->width;
	int h= requisition->height;
	assert (w >= -1);
	assert (h >= -1);
	bool resizeable= false;
	gtk_object->Wdg_Get_Size_Request(w, h, resizeable);
	if (w <= 0) w= 1;
	if (h <= 0) h= 1;
	assert (w < 1000);
	assert (h < 1000);
//	Dbg_Int_(w);
//	Dbg_Int_(h); 
	requisition->width= w;
	requisition->height= h; 
}

class Gtk_Input_Info
{
public:

	Pointer <Stream> stream;
	guint id;
};

static Vector <Gtk_Input_Info> gtk_input_vector;

static
void Gtk_Object_Input_Function(gpointer data,
			       gint /* source */ ,
			       GdkInputCondition gic)
{
//	Dbg_Function_();

	if (! (gic & GDK_INPUT_READ))
		return; 

	Input_Reader &ir= *(Input_Reader *)data;
  
	ir.IR_Input();
}

int Register_Input(Input_Reader &ir)
{
	guint id= gtk_input_add_full
		(ir.Get_Stream().Get_FD(),
		 GDK_INPUT_READ,
		 & Gtk_Object_Input_Function,
		 NULL,
		 & ir,
		 NULL);

	return id; 
}

void Unregister_Input(Input_Reader &ir)
{
//	Dbg_Function_();
	gtk_input_remove(ir.Get_ID()); 
}

void Gtk_Object::Connect_Key_Signals(GtkWidget *widget)
{
//	Dbg_Function_();
	gtk_widget_add_events(widget, GDK_KEY_PRESS); 

	gtk_signal_connect(GTK_OBJECT(gtkwidget), "key-press-event",
			   GTK_SIGNAL_FUNC(Signal_Key_Press_Event), this);
}

void Gtk_Object::Connect_Mouse_Signals(GtkWidget *gtkwidget)
{
//	Dbg_Function_();

	GTK_WIDGET_SET_FLAGS (gtkwidget, GTK_CAN_FOCUS);

	gtk_widget_add_events(gtkwidget,
			      GDK_POINTER_MOTION_MASK      |
			      GDK_POINTER_MOTION_HINT_MASK |
			      GDK_BUTTON_MOTION_MASK       |
			      GDK_BUTTON_PRESS_MASK        |
			      GDK_LEAVE_NOTIFY_MASK        |
			      GDK_BUTTON_RELEASE_MASK);

	gtk_signal_connect(GTK_OBJECT(gtkwidget), "motion-notify-event",
			   GTK_SIGNAL_FUNC(Signal_Motion_Notify_Event), this); 
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "button-press-event",
			   GTK_SIGNAL_FUNC(Signal_Button_Press_Event), this); 
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "button-release-event",
			   GTK_SIGNAL_FUNC(Signal_Button_Release_Event), this); 

	gtk_signal_connect(GTK_OBJECT(gtkwidget), "leave-notify-event",
			   GTK_SIGNAL_FUNC(Signal_Leave_Notify_Event), this); 
}

gint Gtk_Object::Signal_Key_Press_Event(GtkWidget *,
					GdkEventKey *e,
					gpointer data)
{
//	Dbg_Function_();

	Gtk_Object &o= *(Gtk_Object *)data;

	return o.Wdg_Key_Press(e->keyval);
}

void Gtk_Object::Wdg_Do_Close()
{
//	Dbg_Function(); 

	gtk_widget_destroy(gtkwidget);
	/* GTKWIDGET is set to NULL by
	 * Signal_Destroy() which is called by GTK. 
	 */

	assert (gtkwidget == NULL); 
}
