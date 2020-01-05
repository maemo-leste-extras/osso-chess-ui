#ifndef BOARD_IMAGE_INCLUDED
#define BOARD_IMAGE_INCLUDED

/* Original-sized version of the board image. 
 */

#include "alpha_server_image.hh"

#define BOARD_IMAGE_NAME     "pieces.png"

class Board_Image
{
public:

	static Alpha_Server_Image &Get()
		/* The big image.  Load on demand.  On failure the
		   image may not be.  Don't change the image.  Only
		   use their canvas. 
		*/
		{
			if (! image.Is())
				Load();
			return image;
		}

private:
    
	static Alpha_Server_Image image;

	static void Load();
};

#endif /* ! BOARD_IMAGE_INCLUDED */
