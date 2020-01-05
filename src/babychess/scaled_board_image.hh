#ifndef SCALED_BOARD_IMAGE_INCLUDED
#define SCALED_BOARD_IMAGE_INCLUDED

/* Scaled version of the board image as a Server_Image. 
 */

#include "alpha_server_image.hh"
//#include "main.hh"
#include "piece_type.hh"
#include "chess_color.hh"
#include "vector.hh"
#include "dynamic.hh"

class Scaled_Board_Image
{
public:

	Scaled_Board_Image()
		:  width(-1) 
		{
		}
	~Scaled_Board_Image() 
		{
			if (width != -1)
				Remove(); 
		}

	bool Load(int width, bool anti_alias= false);
	/* Load from main board image with size WIDTH.  WIDTH must not
	 * be zero.  Can be called several times.   
	 */

	Server_Image &Get_Image()
		/* Must have been loaded successfully.  Don't close it. 
		 */
		{
			assert (width != -1);
			return Get_Images()[index]().image.Get_Image();
		}

	Server_Image & Get_Alpha_Image()
		{
			assert (width != -1);
			return Get_Images()[index]().image.Get_Alpha_Image();
		}

	Alpha_Server_Image &Get_Full_Image()
		{
			assert (width != -1);
			return Get_Images()[index]().image; 
		}

	void Draw(Canvas &canvas,
		  Point origin,
		  int piece_type,
		  int chess_color);
	/* Draw piece onto canvas.  Do not draw background. 
	 */
	bool Is() const
		{
			return width != -1;
		}
	int Get_Width() const {return width; }

private:

	class Image_Info
	{
	public:

		Alpha_Server_Image image;

		int width;
		int ref_count;
	};

	int width;
	/* -1 if not loaded.
	 */
	int index;
	/* In IMAGES of THIS.  Defined if WIDTH != -1. 
	 */
	bool anti_alias; 
	static Vector <Dynamic <Image_Info> > images, aa_images;

	void Remove();
	/* Remove THIS from list.  Must be loaded. 
	 */

	Canvas &Get_Canvas() 
		{
			return Get_Image()();
		}

	Canvas &Get_Alpha_Canvas()
		{
			return Get_Alpha_Image()(); 
		}
	Vector <Dynamic <Image_Info> > &Get_Images() const
		{
			return anti_alias ? aa_images : images;
		}
};

#endif /* ! SCALED_BOARD_IMAGE_INCLUDED */
