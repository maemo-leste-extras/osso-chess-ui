#include "notation_reset.hh"

//#include "rc.hh"
#include "san.hh"
//#include "notation_stream.hh"
//#include "get_dir.hh"

#include <libintl.h>

void Reset(Notation &notation)
{
	String text= Notation_Reset_Get_Default_Name(); 
	/*Dbg_Str(text());
	if (! Read_From_File(Get_Dir(id_install)/"share/notation"/text, notation))
	*/
		notation= san;
}

String Notation_Reset_Get_Default_Name()
{
	/* JP 13.4.2005 Commented out. We do not want notation_English string id end up in .po files, */
	/* as this function is not used. (P12655-T-450) */

	return '\0';

/*	return */
	  /* This must be translated as "notation_LANGUAGE", where	   */
	  /* LANGUAGE is the english name of the local language.  A	   */
	  /* file in share/notation_settings must exist named	   */
	  /* "LANGUAGE".  
	   */
/*	  gettext("notation_English") + 9; */
}
