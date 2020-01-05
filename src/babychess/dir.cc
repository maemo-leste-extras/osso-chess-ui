#include "dir.hh"

//#include "deref.hh"
//#include "dbg.hh"

#include <sys/stat.h>

bool Dir::Open(String directory_name)
{
	assert (! Is()); 
	assert (directory_name.Is()); 

	dir= opendir(directory_name());
	return dir != NULL;
}

bool Dir::Read(String &file_name)
{
	assert(Is());

	struct dirent *dirent= readdir(dir);
	if (dirent == NULL)
		return false; 

	file_name= dirent->d_name;

	return true;
}

