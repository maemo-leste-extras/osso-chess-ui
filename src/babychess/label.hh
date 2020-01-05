#ifndef LABEL_INCLUDED
#define LABEL_INCLUDED

#include "widget.hh"
//#include "alignment.hh"
//#include "padding.hh"
#include "stretchness.hh"
#include "string.hh"

class Label
	:  public Widget
{
public:

	class Open_Info
	{
	public:
      
		String text;
		//Alignment alignment;
		String span_attributes;

		Open_Info(String new_text= "",
			  //Alignment new_alignment= al_default,
			  String new_span_attributes= "")
			:  text(new_text), 
			   //alignment(new_alignment),
			   span_attributes(new_span_attributes)

			{
			}
	};

	bool Open(Pointer <Widget> parent,
		  String new_text= "",
		  //Alignment alignment= al_begin,
		  String new_span_attributes= "")
		{
			Open_Info oi(new_text, 
		//		     alignment,
				     new_span_attributes);
			return Open(parent, oi);
		}

	bool Open(Pointer <Widget> parent,
		  const Open_Info &open_info);

	String Get_Text() const;
  
	void Set_Text(String text);
	/* Use current span attributes. 
	 */

private:

	String span_attributes;
};

#endif /* ! LABEL_INCLUDED */
