#ifndef FILE_NAME_INCLUDED
#define FILE_NAME_INCLUDED

/* Operations on file names.
 */

#include "inline.hh"
#include "string.hh"

String Base_Name(String file_name);
/* The name of the file whose absolute file name is FILE_NAME.  Return
 * the empty string if FILE_NAME is NULL. 
 */
String operator / (String d, String f);
/* Return the name of the file named FILE_NAME in directory
 * DIRECTORY_NAME.  If DIRECTORY_NAME is empty, return FILE_NAME.  If
 * FILE_NAME is absolute, return FILE_NAME. 
 */

#endif /* ! FILE_NAME_INCLUDED */
