#ifndef TEXT_BUTTON_INCLUDED
#define TEXT_BUTTON_INCLUDED

#include "widget.hh"

class Text_Button
	:  public Widget 
{
public:

	class Open_Info
	{
	public:

		Open_Info(String new_msg_clicked= "",
			  String new_text= "",
			  bool new_enabled= true,
			  const char *new_stock= NULL)
			:  msg_clicked(new_msg_clicked),
			   text(new_text),
			   enabled(new_enabled),
			   stock(new_stock)
			{
			}

		String msg_clicked;
		String text;
		bool enabled;
		const char *stock;
	};

	bool Open(Pointer <Widget> parent,
		  String msg_clicked,
		  String text= "",
		  bool enabled= true,
		  const char *stock= NULL)
		{
			return Open(parent, 
				    Open_Info(msg_clicked,
					      text, 
					      enabled,
					      stock)); 
		}
	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);

private:

	String msg_clicked;

	static void Signal_Clicked(GtkButton *gtkbutton,
				   gpointer *data);
};

#endif /* ! TEXT_BUTTON_INCLUDED */ 
