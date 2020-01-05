#ifndef PANER_INCLUDED
#define PANER_INCLDUED

//#include "dbg.hh"

class H_Paner
	:  public Widget
{
public:

	bool Open(Pointer <Widget> parent)
		{
			next= 0;
			return Widget::Open(parent, gtk_hpaned_new());
		}
	void Wdg_Append_Child(Widget &widget)
		{
			/*Dbg_Function();
			Dbg_Int(next);
			*/
			if (next==0)
			{
				gtk_paned_add1(GTK_PANED(Get()), widget.Get());
				++next;
			}
			else if (next==1)
			{
				gtk_paned_add2(GTK_PANED(Get()), widget.Get());
				++next;
				int w= -1, h;
				bool r;
				widget.Wdg_Get_Size_Request(w, h, r);
				//Dbg_Int(w);
				gtk_paned_set_position(GTK_PANED(Get()), w);
			}
			else
				assert(0);
			//Dbg_Int(next);
		}

private:

	int next;
};

#endif /* ! PANER_INCLUDED */ 
