#include "alpha_client_image.hh"

void
Alpha_Client_Image::Draw(Server_Image &si,
			 Point p)
{
	assert (Is());
	assert (si.Is());
	
	if (! si.Has_Canvas())
		return;

	Server_Image s1;

	if (! s1.Create(Get_Size()))
		return;

	if (! s1.Has_Canvas())
		return;
	
	image.Send_Rect(s1(),
			 Rect(Get_Size()),
			Point(0));

	Server_Image s2;

	if (! s2.Create(Get_Size()))
		return;

	if (! s2.Has_Canvas())
		return;

	alpha.Send_Rect(s2(),
			 Rect(Get_Size()),
			 Point(0));
	


	si().Set_Draw_Mode(Canvas::dm_andnot);
	si().Draw_Canvas(s2(),
			 Rect(p, Get_Size()),
			 Point(0));

	si().Set_Draw_Mode(Canvas::dm_or);
	si().Draw_Canvas(s1(),
			 Rect(p, Get_Size()),
			 Point(0));
}

