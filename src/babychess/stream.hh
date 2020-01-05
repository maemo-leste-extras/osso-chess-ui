#ifndef STREAM_INCLUDED
#define STREAM_INCLUDED

/* Open() opens a named file but a Stream can also be opened as
 * another type of file using functions of other modules such as
 * PIPE. 
 */

#include "string.hh"
//#include "date.hh"
#include "moveable.hh"

//#include "stream.config.hh"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef STREAM_HAVE_UNLOCKED
#    define STREAM_FUNCTION(name)  name ## _unlocked
#else
#    define STREAM_FUNCTION(name)  name
#endif

class Stream
	:  public Pointed
{
public:

	Stream()
		/* Stream is closed on creation */
		:  type(-1),  file(NULL) 
		{
		}
	Stream(String file_name, /*int open_mode, int type=
				  * binary*/const char *opentype)
		:  type(-1),  file (NULL)
		{
			Open(file_name, /*open_mode, type*/opentype); 
		}
	Stream(FILE *f)
		/* F is one of the std... variables.
		 */
		: type(0), file(f) 
		{
			setvbuf(f,
				NULL,
				_IOLBF,
				0x80);
		}
	Stream(int fd);
	~Stream();
	/* Stream is closed automatically. 
	 */

	bool Is() const 
		{
			return file != NULL; 
		}
    
    void Set_Filename(String filenm)
    {
      filename+=filenm;
      return;
    }
    
    String Get_Filename()
    {
      return filename;
    }
	bool Open(String file_name, 
		  const char *opentype);
	/* Opens the file with name FILE_NAME.  The Stream object must
	 * be closed. 
	 *
	 * Return value: Wether opening was successfull.
	 */
#if 0
	/*
	 * OPEN_MODE=
	 *     read:   Open an existing file for reading.  Fail if the
	 *             file does not exist. 
	 *     write:  Open a file for writing.  Remove an existing
	 *             file when necessary. 
	 *     append: Open a file for appending.  Create the file if
	 *             it does not exist. 
	 *     write_excl:  Open the file which must not exist.  Fail
	 *                  if the file exists. 
	 *
	 * TYPE=
	 *     text:   open as text file
	 *     binary: open as binary file
	 */
#endif /* 0 */ 


	/* In the following non-static functions, THIS must be open. 
	 */
	void Open_FD(int fd);
	bool Close()
	/* Return FALSE on error.  If an error occurs, the object will
	 * be closed, but data was not written or read completely. 
	 */
		{
			assert(type == -1 && file != NULL);

			int fc= fclose(file);
			file= NULL;
			return 0 == fc;
		}

	int Read(void *data, int length)
	/* Read LENGTH bytes and store them in DATA.  Return the
	 * actual number of byte read.  Thus the return value is
	 * LENGTH on success, and less on error or end-of-file. 
	 */
		{
			assert (Is());
			return STREAM_FUNCTION(fread)(data, length, 1, file);
		}

	bool Write(const void *data, int length)
	/* Write LENGTH byte from DATA.  Return FALSE on error. 
	 */
		{
			assert (Is());
			return STREAM_FUNCTION(fwrite)(data, length, 1, file);
		}
	bool Read_Line(char *data, int length)
		{
			assert (Is());
			return STREAM_FUNCTION(fgets)(data, length, file) != NULL;
		}

	bool Write_Line(const char *s)
	/* Write the string.  Return TRUE on success.  S must not be NULL. 
	 */
		{
			assert (Is());
			assert (s != NULL);
			return EOF != STREAM_FUNCTION(fputs)(s, file);
		}

	int Read_Char()
		{
			assert(Is());
			return STREAM_FUNCTION(getc)(file);
		}

	bool Write_Char(char c)
	/* return value:  success
	 */
		{
			assert(Is());
			return EOF != STREAM_FUNCTION(putc)(c, file);
		}

	void Flush()
		{
			assert(Is());
			fflush(file);
		}

	void Set_Block(bool);
	/* Set what corresponds to the negation of the O_NONBLOCK
	 * flag.  Default is TRUE.  If FALSE, a read returns an error
	 * instead of blocking.  
	 */

	void Set_FILE(FILE *f)
		{
			file= f;
		}


	FILE *operator () () const
		{
			assert (file != NULL);
			return file; 
		}

	int Get_FD() const
		/* Must be */
		{
			assert (Is() || type >= 1);
			return (type >= 1
				? type - 1
				: fileno(file));
		}

	bool Dup(int fd, const char *opentype);
	/* Stream must be closed.  Duplicate FD.  Return TRUE on
	 * success, FALSE on failure.  MOD is the mode passed to
	 * fdopen().  	 
	 */
	bool Dup(const Stream &stream, const char *opentype)
		{
			return Dup(stream.Get_FD(), opentype);
		}
	bool Open(int fd, const char *opentype);

	bool Has_FD() const
		/* Whether there is a file descriptor.
		 */
		{
			return type != -1 || file != NULL;
		}

	bool End_Of_File() const
		{
			assert (Is());
		
			return 0 != feof(file);
		}
	bool Write_String(String text)
		{
			return Write(text(), *text); 
		}
	
	static void Set_FD_Creation_Hook(void (*)(int fd));
	/* Function that will be called on each FD created. 
	 */

private:

	int type;
	/*  0:     standard input/output, and FILE contains STDIN...
	 *  -1:    owned file.
	 *  >= 1:  1+FD, where FD is not owned.
	 */
	FILE *file;
	/* NULL when closed or TYPE >= 1.
	 */
  String filename;

	bool Do_Open(String file_name,  const char *opentype);
	/* Open FILE_NAME.  If successfull, set FILE and return
	 * TRUE, else return FALSE.   
	 */

};


void Stream_Call_Hook(int); 
/* Other modules that open files call this functions.  With the FD
 * created as argument. 
 */

#endif /* ! STREAM_INCLUDED */
