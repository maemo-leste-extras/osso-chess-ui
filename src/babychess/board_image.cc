#include "board_image.hh"

#include "stream_file.hh"
#include "alpha_server_image_stream.hh"
#include "file_name.hh"

Alpha_Server_Image Board_Image::image;

void
Board_Image::Load()
{
	assert (! image.Is()); 
  
	if (! Read_From_File(PIXMAPSDIR "/" BOARD_IMAGE_NAME, image))
	{
		//Dbg("\tFailed");
	}
	else
	{
		//Dbg("\tSuccess."); 
	}
}
