#ifndef DIR_INCLUDED
#define DIR_INCLUDED

#include "string.hh"

#include <dirent.h> 

class Dir
{
public:

	Dir()
		:  dir (NULL) 
		{
		}
	Dir(String directory_name) 
	/* Open the directory DIRECTORY_NAME.  May fail.  Check Is().  
	 */
	/* The directory is closed on destruction. 
	 */
		:  dir (NULL)
		{
			Open(directory_name); 
		}
	bool Is() const 
		{
			return dir != NULL; 
		} 
	operator bool () const
		{
			return Is(); 
		}
	void Close()	
	/* The directory must be open. 
	 */
		{
			assert(Is());  
			closedir(dir);  
			dir= NULL; 
		} 
	~Dir()
		{
			if (Is())
				Close(); 
		}

	bool Open(String directory_name);

	bool Read(String &);
	/* The directory must be open 
	 */
	/* Read the next file name from the directory into FILE_NAME.
	 * Return TRUE if the next file name was read, and FALSE if
	 * there is no next file.  
	 */

private:

	DIR *dir;
	/* NULL when closed, else open. 
	 */
};

#endif /* ! DIR_INCLUDED */
