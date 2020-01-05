#include "file_name.hh"

//#include "resolve_path.hh"
#include "strdupa.hh"

#include <libgen.h>

String Base_Name(String file_name)
{
	char *path= strdupa(file_name());
	char *base= basename(path);
	return String(base); 

}

String operator / (String directory_name, 
		   String file_name)
{
	/* If FILE_NAME is absolute, return it. 
	 */
	if (file_name[0] == '/')
		return file_name;

	/* If DIRECTORY_NAME is empty, return FILE_NAME. 
	 */
	if (*directory_name == 0)
		return file_name;

	/* If DIRECTORY_NAME does not end in '/', add it. 
	 */
	if (directory_name[-1] != '/')
		directory_name += '/';

	return directory_name + file_name;
}
