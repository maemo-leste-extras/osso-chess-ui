#ifndef ALPHA_SERVER_IMAGE_INCLUDED
#define ALPHA_SERVER_IMAGE_INCLUDED

#include "server_image.hh"

class Alpha_Server_Image
	:  virtual public Pointed
{
public:

	bool Is() const
		{
			return image.Is();
		}

	Server_Image &Get_Image()
		{
			return image;
		}

	Server_Image &Get_Alpha_Image()
		{
			return alpha_image;
		}

private:

	Server_Image image;
	Server_Image alpha_image;
};

#endif /* ! ALPHA_SERVER_IMAGE_INCLUDED */
