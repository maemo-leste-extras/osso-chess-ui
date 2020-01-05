#include "packer.hh"

//#include "dbg.hh"

void Packer::Wdg_Get_Size_Request(int &/*width*/,
				  int &/*height*/,
				  bool &resizeable)
{
	resizeable= false;
}

bool Packer::Open(Pointer <Widget> parent,
		  Open_Info open_info)
{
	GtkWidget *gtkwidget= Packer_Open(open_info.spacing, open_info.homogeneous);
			
	if (gtkwidget == NULL)
		return false;

	gtk_widget_set_usize(gtkwidget, -1, -1);

	if (! Widget::Open(parent, gtkwidget)) 
		return false;

	use_next= false;
	start= open_info.start;
			
	return true;
}

void Packer::Wdg_Append_Child(Widget &widget)
{
	if (! use_next)
	{
		next_resizeable= false;
		int w= -1, h= -1;
		widget.Wdg_Get_Size_Request(w, h,
					    next_resizeable); 
		next_padding= 0;
	}
	else
		use_next= false;

	gboolean arg= next_resizeable ? TRUE : FALSE;

	if (start)
		gtk_box_pack_start(GTK_BOX(Get()), widget.Get(), arg, arg, next_padding);
	else 
		gtk_box_pack_end  (GTK_BOX(Get()), widget.Get(), arg, arg, next_padding);
}
