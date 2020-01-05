#ifndef ALPHA_CLIENT_IMAGE_INCLUDED
#define ALPHA_CLIENT_IMAGE_INCLUDED

#include "client_image.hh"
#include "server_image.hh"

class Alpha_Client_Image
{
public:

	bool Is() const
		{
			return image.Is();
		}

	void Clear()
		{
		  image.Free();
			alpha.Free();
		}

	Point Get_Size() const
		{
			assert (Is());
			return image.Get_Size();
		}

	void Draw(Server_Image &,
		  Point);

	Client_Image &Get_Image() 
		{
			return image;
		}

	Client_Image &Get_Alpha_Image()
		{
			return alpha;
		}

private:

	Client_Image image;
	Client_Image alpha;
	/* transparent is black, opaque is white. 
	 */
};

#endif /* ! ALPHA_CLIENT_IMAGE_INCLUDED */
