#include "widget.hh"

#include <gdk/gdk.h>

int Widget::depth= -1; 

bool Widget::Open(Pointer <Widget> parent) 
{
	GtkWidget *gtkwidget= gtk_drawing_area_new(); 

	if (gtkwidget == NULL)
		return false;

	Connect_Mouse_Signals(gtkwidget);

	if (!  Open(parent, gtkwidget, true, true))
		return false;

	int w= -1, h= -1;
	bool resizeable= false; 
	Wdg_Get_Size_Request(w, h, resizeable); 
	assert (w < 40 * unit);
	assert (h < 40 * unit);
  
	if (w >= 0 && h >= 0)
	{
		if (! resizeable)
		{
			gtk_drawing_area_size(GTK_DRAWING_AREA(gtkwidget),
					      w, h);
		}
		else
		{
		}
	}
  
	return true;
}

bool
Widget::Open(Pointer <Widget> new_parent,
	     GtkWidget *new_gtkwidget,
	     bool open_window,
	     bool expose_events)
{
	assert (! Is()); 

	if (new_gtkwidget == NULL)
		return false;

	if (depth == -1)
	{
		GdkVisual *visual= gtk_widget_get_visual(new_gtkwidget);
		if (visual != NULL)
			depth= visual->depth; 
	}

	Gtk_Object::Attach(new_gtkwidget, expose_events);
	if (new_parent.Is())
	{
		parent= new_parent(); 

		if (! parent().Append(this))
		{
			Gtk_Object::Close();
			return false;
		}
		gtk_widget_show(new_gtkwidget); 

		parent().Wdg_Append_Child(*this); 
	}
	else
	{
		if (open_window)
		{
			if (! gtk_window.Alloc())
			{
				Gtk_Object::Close();
				return false;
			}

			Point window_size= Wdg_Get_Window_Size();
			if (! gtk_window().Open(Wdg_Init_Caption(),
						window_size,
						Wdg_Icon_Name()))	
			{
				Gtk_Object::Close();
				return false;
			}
			gtk_window().Set(*this);
		}
		gtk_widget_show(new_gtkwidget);
	}

	return true;
}

bool
Widget::Append(Pointer <Widget> child)
{
	if (! children.Set(*children + 1))
		return false;
	children[-1]= child;
	return true; 
}

void Widget::Wdg_Do_Close()
{
	if (gtk_window)
	{
		gtk_window().Close(true); 
		assert (! gtk_window().Is());
		gtk_window.Free();

		assert (!gtk_window);
	}
	else 
		Gtk_Object::Wdg_Do_Close(); 

	assert (!parent);
	assert (*children == 0); 
}

void
Widget::Wdg_Close()
{
	/* Children are all closed by GTK automatically.  Forget
	 * them.
	 */
	children.Free();

	/* Remove self from parent child list. 
	 */
	if (parent.Is())
	{
		for (int i= 0;  i < *parent().children;  ++i)
		{
			if (parent().children[i] == this)
			{
				if (i + 1 < *parent().children)
					parent().children[i]= parent().children[-1];
				parent().children= -1; 
			}
		}
		parent= NULL; 
	}
	Message_Receiver::Remove_Pending_Messages();

	Gtk_Object::Wdg_Close(); 
}

Widget::Widget()
{
}

void Widget::Set_Mouse_Cursor(Mouse_Cursor /* mc */ )
{
}

void Widget::MR_Receive(String message)
{
	if (Get_Parent().Is())
	{
		Get_Parent()().MR_Receive(message); 
		return;
	}

	if (message == "cancel")
		Close();

	else if (message == "ok")
		Close();
}

void Widget::Wdg_Mouse(Mouse_Action, Mouse_Button, Point,
		       bool)
{
}

void Widget::Set_Transient(const Widget &widget)
{
	assert (Is());
	assert (widget.Is()); 

	Set_Transient(GTK_WINDOW(Get_Gtk_Window()),
		      GTK_WINDOW(widget.Get_Gtk_Window()));
}

Widget::~Widget()
{
}

void Widget::Wdg_Append_Child(Widget &widget)
{
	gtk_container_add(GTK_CONTAINER(Get()),
			  widget.Get()); 
}

void Widget::Set_Visible(bool visible)
{
	if (visible)
	{
		gtk_widget_show(Get());
	}
	else
	{
		gtk_widget_hide(Get());
	}
}

void Widget::Activate_Window()
{
	gtk_window_present(GTK_WINDOW(Get_Window().Get()));
}
