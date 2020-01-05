#include "label.hh"

#include "format.hh"

bool Label::
Open(Pointer <Widget> parent,
     const Open_Info &open_info)
{
	/*Dbg_Function();
	Dbg_Int(open_info.alignment);

	assert (open_info.alignment >= 0 && 
		open_info.alignment < 3);
		*/

	if (! Widget::Open(parent, gtk_label_new("")))
		return false;
	float xalign[3]= {0.0, 0.5, 1.0};
	
	gtk_misc_set_alignment (GTK_MISC (Get()), 
				//xalign[open_info.alignment],
				xalign[0],
				0.5);

	span_attributes= open_info.span_attributes;

	Set_Text(open_info.text);

	return true;
}

String Label::Get_Text() const
{
	gchar *text;
	gtk_label_get(GTK_LABEL(Get()), & text);
	String result(text);
	result.Isolate();
	return result; 
}

void Label::Set_Text(String text)
{
	if (! span_attributes.Is())
	{
		gtk_label_set_text(GTK_LABEL(Get()), text()); 
	}
	else
	{
		char *m= g_markup_escape_text(text(),
					      *text);
		String marked_text= m;
		marked_text.Isolate(); 
		g_free(m); 
		
		String markup= Format("<span %s>%s</span>",
				      span_attributes(),
				      text());
		gtk_label_set_markup(GTK_LABEL(Get()), markup()); 
	}
}
