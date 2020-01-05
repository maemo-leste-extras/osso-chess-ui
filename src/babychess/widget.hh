#ifndef WIDGET_INCLUDED
#define WIDGET_INCLUDED

/* Commands:  cancel
 */

#include "window.hh"
#include "pointer.hh"
#include "vector.hh"
#include "message_receiver.hh"
#include "stretchness.hh"
#include "canvas.hh"
#include "point.hh"
#include "rect.hh"
#include "mouse_button.hh"
#include "mouse_action.hh"
#include "dynamic.hh"

enum Mouse_Cursor {ms_arrow= 0, ms_move, ms_horizontal, ms_vertical,
		   ms_swne, ms_nwse, ms_no, ms_hand, ms_layer,
		   ms_count};


class Widget
	:  public Gtk_Object, 
	   virtual public Message_Receiver
{
public:

	enum {unit= 20}; 

	Widget();

	virtual ~Widget();

	bool Open(Pointer <Widget> parent); 

	bool Open(Pointer <Widget> parent,
		  GtkWidget *new_gtkwidget,
		  bool open_window= true,  
		  bool expose_events= false); 
	/* NEW_GTKWIDGET as returned by a GTK function.
	 * NEW_GTKWIDGET may be NULL. 
	 * If PARENT is NULL and OPEN_WINDOW is TRUE, then a window is
	 * opened and the widget is put into that window. 
	 */

	bool Append(Pointer <Widget> child); 

	void MR_Receive(String/*, int*/);

	int Get_Children_Count() const
		{
			return *children; 
		}

	void Invalidate()
		{

			gtk_widget_queue_draw(Get());
		}

	void Invalidate(const Rect &rect)
		{
			gtk_widget_queue_draw_area
				(Get(),
				 rect.origin.x,
				 rect.origin.y,
				 rect.size.x,
				 rect.size.y); 
		}
	CH_Window &Get_Window()
		{
			if (gtk_window.Is())
			{
				assert (! parent.Is());
				return gtk_window();
			}
			else
			{
				assert (parent.Is()); 
				return parent().Get_Window(); 
			}
		}

	const CH_Window &Get_Window() const 
		{
			if (gtk_window.Is())
				return gtk_window();
			else
				return parent().Get_Window(); 
		}

	void Activate_Window();

	void Set_Transient(const Widget &widget);
	/* Set THIS window to be transient for WIDGET. 
	 */

	virtual GtkWindow *Get_Gtk_Window() const
		{
			return GTK_WINDOW(Get_Window().Get());
		}
	Widget &Get_Child(int n) const
		{
			return children[n](); 
		}
	void Invalidate_Size()
		{
			gtk_widget_queue_resize(Get());
		}
	void Set_Visible(bool visible= true);

	bool Get_Visible() const
		{
			return GTK_WIDGET_VISIBLE(Get()); 
		}
	void Set_Bounds(Rect) const
		{
		}

	void Wdg_Close(); 

	void Parent_Mouse(Mouse_Action a, 
			  Mouse_Button b, 
			  Point p,
			  bool ac)
		{
			Widget::Wdg_Mouse(a, b, p, ac); 
		}
	virtual void Wdg_Mouse(Mouse_Action, Mouse_Button, Point,
			       bool);

	Pointer <Widget> Get_Parent()
		{
			return parent; 
		}

	void Parent_Send_Message(String message /* , int msg */ )
		{
			if (Get_Parent().Is())
				Get_Parent()().MR_Send(message /* ,
								  msg
		*/ );
		}

	void Parent_Receive_Message(String message/*, int msg*/)
		{
			Get_Parent()().MR_Receive(message/*, msg*/);
		}
	
	void Set_Mouse_Cursor(Mouse_Cursor mc);

	virtual void Wdg_Append_Child(Widget &);

	void Set_Index(int index)
		{
			gtk_box_reorder_child(GTK_BOX(Get_Parent()().Get()),
					      Get(),
					      index);
		}

	void Set_Enabled(bool enabled= true)
		{
			gtk_widget_set_sensitive
				(Get(),
				 enabled ? TRUE : FALSE);
		}

	static void Loop()
		{
			CH_Window::Loop();
		}
	static int Get_Depth()
		{
			assert (depth > 0);
			return depth; 
		}
	static void Set_Transient(GtkWindow *window,
				  GtkWindow *for_window)
		{
			gtk_window_set_transient_for(window,
						     for_window); 
		}


private:

	Dynamic <CH_Window> gtk_window;   
	Vector <Pointer <Widget> > children;
	Pointer <Widget> parent;

	static int depth;

	virtual void Wdg_Draw(Canvas &)
		{
		}

	virtual void Wdg_Leave_Notify()
		{
		}

	void Wdg_Do_Close(); 
	virtual String Wdg_Init_Caption() const
		{
			return "Window"; 
		}
	virtual const char *Wdg_Icon_Name() const
		{
			return "chess.png";
		}
	virtual Point Wdg_Get_Window_Size() const
		/* Return x=-1 for no resizing. 
		 */
		{
			return Point(600, 400); 
		}
};
#endif /* ! WIDGET_INCLUDED */
