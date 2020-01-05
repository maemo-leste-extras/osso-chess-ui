#ifndef PACKER_INCLUDED
#define PACKER_INCLUDED

#include "widget.hh"

class Packer
	:  virtual public Widget
{
public:

	class Open_Info
	{
	public:
		int spacing;
		bool homogeneous;  
		bool start;
		Open_Info(int new_spacing= 0,
			  bool new_homogeneous= false,
			  bool new_start= true)
			:  spacing(new_spacing),
			   homogeneous(new_homogeneous),
			   start(new_start)
			{
			}
	};

	bool Open(Pointer <Widget> parent,
		  Open_Info= Open_Info());

	void Set_Next_Resizeable(bool resizeable,
				 int padding= 0)
		{
			use_next= true;
			next_resizeable= resizeable;
			next_padding= padding;
		}


private:

	bool next_resizeable;
	int next_padding;
	bool use_next;
	bool start;

	void Wdg_Append_Child(Widget &widget);

	void Wdg_Get_Size_Request(int & /* width */ ,
				  int & /* height */ ,
				  bool &resizeable);
	virtual GtkWidget *Packer_Open(int /* spacing */,
				       bool /* homogenous */ ) 
		const
		{
			return NULL; 
		}
};

class V_Packer
	:  public Packer
{
private:

	GtkWidget *Packer_Open(int spacing, bool homogeneous) const
		{
			return gtk_vbox_new(homogeneous, spacing);
		}
};

class H_Packer
	:  public Packer
{
private:

	GtkWidget *Packer_Open(int spacing, bool homogeneous) const
		{
			//Dbg_Int(homogeneous); 
			return gtk_hbox_new(homogeneous ? TRUE : FALSE, spacing); 			
		}
};

#endif /* ! PACKER_INCLUDED */
