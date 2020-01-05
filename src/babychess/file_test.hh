#ifndef FILE_TEST_INCLUDED
#define FILE_TEST_INCLUDED

#include "inline.hh"
//#include "stat.hh"

#include <sys/stat.h>
#include <fcntl.h>

Inline bool File_Is_Dir(String file_name)
{
	struct stat stat;
	if(::stat(file_name(), &stat))
		return false;

	return S_ISDIR(stat.st_mode);
}

#endif /* ! FILE_TEST_INCLUDED */
