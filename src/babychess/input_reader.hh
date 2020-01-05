#ifndef INPUT_READER_INCLUDED
#define INPUT_READER_INCLUDED

#include "pointer.hh"
#include "vector.hh"
#include "stream.hh"

class Input_Reader; 

extern int Register_Input(Input_Reader &);
extern void Unregister_Input(Input_Reader &); 

class Input_Reader
	:  virtual public Pointed
{
public:

	Input_Reader()
		:  index (-1)
		{
		}
	virtual ~Input_Reader()
		{
			if (index != -1)
				Close();
		}

	/* Called when FILE is ready.  May call Close(). 
	 */
	virtual void IR_Input()
		{
		}
	bool Open(const Stream &new_stream);

	void Close(); 

	const Stream &Get_Stream() const
		{
			assert (index != -1);
			return stream();
		}

	bool Is() const
		{
			assert ((index == -1) == (! stream.Is()));
			assert (index == -1 || stream == list[index]().Get_Stream()); 
			return index != -1; 
		}

	int Get_ID() const
		{
			return id; 
		}

	/* The list may contain NULL entries. 
	 */
	static const Vector <Pointer <Input_Reader> > &Get_List()
		{
			return list;
		}

private:

	int index; 
	/* in LIST, or -1 when closed. */
	Pointer <const Stream> stream;
	int id;

	static Vector <Pointer <Input_Reader> > list;
	/* some may be NULL. */
};

#endif /* ! INPUT_READER_INCLUDED */
