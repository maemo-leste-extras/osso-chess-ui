#ifndef RC_INCLUDED
#define RC_INCLUDED

/* Resource files in the home directory.
 */

#include "file_name.hh"
#include "inline.hh"
#include "dir.hh"
#include "stream_file.hh"
//#include "dbg.hh"
#include "file_test.hh"
#include "stream.hh"

#include <typeinfo>
#include <glib.h>

//#define RC_DIR_NAME   ".babychess"

// FIXME

Inline bool RC_Make_Dir()
/* Make the BabyChess directory in the home directory. */ 
{
	/*if (File_Is_Dir(String(g_get_home_dir()) / RC_DIR_NAME))
		return true;
	return 0 <= mkdir((String(g_get_home_dir()) / RC_DIR_NAME)(), 0777);
	*/
	return true;
}

Inline String RC_File_Name(String name)
{
	return name;
	//return String(g_get_home_dir()) / RC_DIR_NAME / name;
}

template <class Type>
Inline bool RC_Load(Type &info)
{
	Stream stream;

	return Read_Defaults(stream, info);
}

template <class Type>
Inline bool RC_Save(const Type &info, String name= "")
{
	//Dbg_Function();
	/*if (! RC_Make_Dir())
	{
		return false; 
	}
	if (!name)  name= typeid(Type).name();
	bool result= Write_To_File(RC_File_Name(name), info);
	//Dbg_Int(result);
	return result; 
	*/
	return false;
}

#endif /* ! RC_INCLUDED */
