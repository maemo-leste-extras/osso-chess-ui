#include "read_line.hh"

//#include "dbg.hh"

bool Read_Line(Stream &stream,
	  String &line)
{
#ifdef READ_LINE_GETLINE
	/* This implementation would be preferable but does not work
	 * in conjunction with IO_CHILD_PROCESS for unknown reasons.  
	 */

	//Dbg_Function(); 

	line.Free(); 

	size_t n= 0;
	char *lineptr= NULL;
	
	ssize_t g= getline(&lineptr, &n, stream.Get_FILE());

	//Dbg_Int(g); 

	if (g < 0)
	{
		/*Dbg("\terror %d %s",
		    errno, strerror(errno));
		    */
		return false;
	}

	assert (g > 0);

	//Dbg_Str(lineptr); 

	if (g > 1)
		Object_Reset(String, &line, lineptr, g-1);

	free(lineptr); 

	return true; 

#else /* ! READ_LINE_GETLINE */ 

	enum {buf_size= 0x60};
	line.Free();
	char buffer[buf_size];

	while (true)
	{
		if (! stream.Read_Line(buffer, buf_size))
		{
			return line.Is(); 
		}

		assert (buffer[0] != '\0');

		String text= buffer;
		if (text[-1] == '\n')
		{
			line += text.Sub(0, *text - 1); 
			return true;
		}
		else
			line += text; 
	}

#endif /* ! READ_LINE_GETLINE */ 
}
