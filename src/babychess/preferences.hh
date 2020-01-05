#ifndef PREFERENCES_INCLUDED
#define PREFERENCES_INCLUDED

/* TODO Make this use GConf */

#include "rc.hh"

template <class Type/*, class Type_Editor*/>
class Preferences
	:  public Message_Receiver
{
public:
	
	enum {menu_main}; 

	~Preferences()
		{
		}

	void Open(Widget &new_owner,
		  String new_msg_changed)
		{
			owner= new_owner;
			msg_changed= new_msg_changed;

			RC_Load(object);
		}

	void Close()
		{
			owner= NULL; 
		}

	const Type &Get_Value() const
		{
			return object; 
		}
	const Type &operator () () const
		{
			return Get_Value(); 
		}
	const Type *operator -> () const
		{
			return & Get_Value(); 
		}
	Type &Get_Change_Value() 
		/* The value can be changed, but Changed() must be
		 * called after the change. 
		 */
		{
			return object;
		}

	const Type &Get_Old_Value() const
		/* Only called inside MSG_CHANGED. 
		 */
		{
			return old_object();
		}
	void Changed()
		/* Must be called after changes were made to the
		 * object using Get_Change_Value().  Doesn't need to
		 * be called are not saved to file. 
		 */
		{
		}
	void Caption_Changed()
		{
		}

private:

	Type object;
	Dynamic <Type> old_object; 
	String msg_changed;
	Pointer <Widget> owner; 
	const char *name;
};

#endif /* ! PREFERENCES_INCLUDED */
