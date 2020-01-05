#include "text_button.hh"

//#include "dbg.hh"

void Text_Button::Signal_Clicked(GtkButton *gtkbutton,
				 gpointer *data)
{
	//Dbg_Function();

	Text_Button *text_button= (Text_Button *)data;
	assert (text_button != NULL);
	assert (text_button->Get() == GTK_WIDGET(gtkbutton));

	text_button->MR_Send(text_button->msg_clicked);
}

bool Text_Button::Open(Pointer <Widget> parent,
		       Open_Info open_info)
{
	msg_clicked= open_info.msg_clicked;
	if (! Widget::Open
	    (parent,
	     open_info.stock 
	     ? gtk_button_new_from_stock(open_info.stock)
	     : gtk_button_new_with_label(open_info.text())))
		return false;
	Set_Enabled(open_info.enabled);
	gtk_signal_connect(GTK_OBJECT(Get()), 
			   "clicked",
			   GTK_SIGNAL_FUNC(Signal_Clicked), 
			   this);
	gtk_container_set_border_width(GTK_CONTAINER(Get()),
			     unit / 5);
			
	return true; 
}
