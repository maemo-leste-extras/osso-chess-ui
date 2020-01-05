#include "input_reader.hh"

Vector <Pointer <Input_Reader> > Input_Reader::list;

void Input_Reader::Close()
{
	if (index == -1)
		return;

	Unregister_Input(*this); 
	list[index]= NULL; 
	index= -1;
	stream= NULL;
	int new_size= *list;
	while (new_size != 0 && !list[new_size - 1])
		-- new_size;
	list= new_size; 
}

bool Input_Reader::Open(const Stream &new_stream)
{
	assert (! Is()); 
	for (int i= 0;  i < *list && index == -1;  ++i)
	{
		if (! list[i])
		{
			index= i;
		}
	}
	if (index == -1)
	{
		if (! list.Set(*list + 1))
			return false;
		index= *list - 1;
	}
	list[index]= this;

	stream= new_stream;

	id= Register_Input(*this); 
  
	return true;
}
