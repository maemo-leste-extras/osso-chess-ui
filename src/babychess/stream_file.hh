#ifndef STREAM_FILE_INCLUDED
#define STREAM_FILE_INCLUDED

/* Read and write an object from/to a file.  Functions of the STREAM
 * module are used.  Return FALSE on error.  
 */

#include "stream.hh"
//#include "dbg.hh"

template <class Type> 
bool Read_From_File(String file_name,
		    Type &object)
{
	Stream stream;
	
	if (! stream.Open(file_name, "rb"))
		return false;

	if (! Read_Stream(stream, object))
		return false;
	if (! stream.Close())
		return false;
	return true;
}

template <class Type>
bool Write_To_File(String file_name,
		   const Type &object)
{
	Stream stream;

	if (! stream.Open(file_name, "wb"))
		return false;
	if (! Write_Stream(stream, object))
		return false;
	if (! stream.Close())
		return false;
	return true;
}

#endif /* ! STREAM_FILE_INCLUDED */
