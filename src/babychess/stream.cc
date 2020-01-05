#include "stream.hh"

/* configuration macro:
 *
 * STREAM_HAVE_UNLOCKED:  Use the `*_unlocked' versions of some
 *                        functions. 
 */

//#include "error.hh" 

void (*stream_hook)(int fd)= NULL;
/* Called for each FD created.  Set in Set_FD_Creation_Hook().  The
 * PROCESS module registers a hook that sets each FD opened to
 * close-on-exec.  
 */

void Stream_Call_Hook(int fd)
{
	if (stream_hook != NULL)
		stream_hook(fd);
}

bool Stream::Open(int fd,
		  const char *opentype)
{
	assert (! Is());
	assert (fd >= 0);

	FILE *fdo= fdopen(fd, opentype);
	if (fdo == NULL)
	{
		return false;
	}
	file= fdo;

	Stream_Call_Hook(fileno(file));

	return true;
}

bool Stream::Do_Open(String file_name,  const char *opentype)
{
  Set_Filename(file_name);
	file= fopen(file_name(), /*ms*/ opentype);
	if (file == NULL)
	{
		//Error::Set_File_Name(file_name); 
		return false;
	}

	Stream_Call_Hook(fileno(file));

	return true;
}

bool Stream::Open(String file_name, const char *opentype)
{
	assert(! Is());

	return Do_Open(file_name, /*mo, ty*/opentype);
}

bool Stream::Dup(int f, const char *opentype)
{
	assert (! Is());
  
	int d= dup(f);

	if (d == -1)
		return false;

	FILE *fdo= fdopen(d, opentype);
	if (fdo == NULL)
	{
		close(d);
		return false;
	}
	file= fdo;

	Stream_Call_Hook(fileno(file));

	return true;
}

Stream::~Stream()
{
	if (type == -1           &&
	    file != NULL)
		fclose(file);
	/* Don't close stdin & co.
	 */
}

void
Stream::Set_Block(bool block)
{
	assert (Is());

	int flags= fcntl(Get_FD(), F_GETFL);

	if (block)
		flags &= (~ O_NONBLOCK);
	else
		flags |= O_NONBLOCK;

	fcntl(Get_FD(), F_SETFL, flags);
}

Stream::Stream(int fd)
{
	type= 1 + fd;
	file= NULL;
}

void Stream::Open_FD(int fd)
{
	assert (! Is());

	type= 1 + fd;
}

void Stream::Set_FD_Creation_Hook(void (*f)(int fd))
{
	assert (stream_hook == NULL);
	stream_hook= f;
}
