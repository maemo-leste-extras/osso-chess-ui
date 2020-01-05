#include "scaled_board_image.hh"

#include "board_image.hh"
#include "format.hh"
#include "file_name.hh"
#include "stream_file.hh"
#include "server_image_stream.hh"
#include "client_image.hh"
#include "file_test.hh"
#include "dir.hh"

#if defined (CLIENT_IMAGE_HAVE_MAP) && defined (CLIENT_IMAGE_HAVE_DRAW_STRETCH_CANVAS)
#    define SCALED_BOARD_IMAGE_HAVE_MAP
#endif

Vector <Dynamic <Scaled_Board_Image::Image_Info> > 
Scaled_Board_Image::images, Scaled_Board_Image::aa_images; 

bool Scaled_Board_Image::
Load(int new_width,
     bool new_anti_alias)
{
	//Dbg_Function();
	//Dbg_Int(new_anti_alias);

	assert (new_width > 0);

	/* Already loaded. */ 
	if (new_width == width && new_anti_alias == anti_alias)
		return true;

	/* Close */ 
	if (width != -1)
		Remove();

	anti_alias= new_anti_alias;

	/* Already open in list. */ 
	for (int i= 0; i < Get_Images().Get_Size(); ++i)
	{
		if (Get_Images()[i]().width == new_width)
		{
			++ Get_Images()[i]().ref_count;
			width= new_width;
			index= i;
			return true;
		}
	}

	/* Add new image to list */ 
	if (! Get_Images().Set(*Get_Images() + 1))
		return false;

	if (! Get_Images()[-1].Alloc())
	{
		Get_Images()= -1;
		return false;
	}

	index= *Get_Images() - 1;

	Get_Images()[-1]().ref_count= 1;
	Get_Images()[-1]().width= width= new_width;

	Point dimension(6 * new_width, 2 * new_width);
	Pointer <Alpha_Server_Image> image= Get_Images()[-1]().image; 

	//XXX
	//String dir= String(g_get_home_dir()) / "tmp"; 
	//if (! File_Is_Dir(dir))
	//	/*Dir::Make*/mkdir(dir(), 0777);

	/* Now load scaled images saved in temp directory, either as a
	 * map or as a file. 
	 */
#ifdef SCALED_BOARD_IMAGE_HAVE_MAP

	/* Load map */

/*	String map_type= Client_Image::Get_Map_Type(); 
	String map_name= dir /
		Format("babychess.scaled_board_image.%s%d.%s.map", 
		       anti_alias ? "aa." : "",
		       width,
		       map_type());
	String alpha_map_name= dir /
		Format("babychess.scaled_board_image.alpha.%s%d.%s.map", 
		       anti_alias ? "aa." : "",
		       width,
		       map_type()); 
		       */
	/*Dbg("\tReading maps...");
	Dbg("\t\t%s", map_name());
	Dbg("\t\t%s", alpha_map_name()); 
	*/

/*	Client_Image client_image;
	Client_Image client_alpha_image;
	if (client_image.Open_Map(map_name))
	{
		if (client_alpha_image.Open_Map(alpha_map_name))
		{
			//Dbg("\tOpened both maps.");
			if (client_image.Get_Size() == dimension        &&
			    client_alpha_image.Get_Size() == dimension  &&
			    image().Get_Image().Create(dimension)       && 
			    image().Get_Image().Has_Canvas()            &&
			    image().Get_Alpha_Image().Create(dimension) &&
			    image().Get_Alpha_Image().Has_Canvas()      &&
			    client_image.Send_Rect(image().Get_Image()(),
						   dimension,
						   0)                   &&
			    client_alpha_image.Send_Rect(image().Get_Alpha_Image()(),
							 dimension,
							 0))
			{
				return true; 
			}


			//Dbg("\tMaps have wrong size."); 
			client_alpha_image.Free(); 
		}
		client_image.Free(); 
	}
	*/
	//Dbg("\tOpening maps failed."); 

#else /* ! SCALED_BOARD_IMAGE_HAVE_MAP */ 

	/* Open files */ 

	/* XXX 
	String image_file_name= dir /
		Format("babychess.scaled_board_image.%s%d" SERVER_IMAGE_STREAM_SUFFIX, 
		       anti_alias ? "aa." : "",
		       width);
	String alpha_image_file_name= dir /
		Format("babychess.scaled_board_image.alpha.%s%d" SERVER_IMAGE_STREAM_SUFFIX, 
		       anti_alias ? "aa." : "",
		       width);
		 XXX       */

	/*Dbg("\tLoading from file...");
	Dbg("\t\t%s", image_file_name());
	Dbg("\t\t%s", alpha_image_file_name());
	*/

/*	if (Read_From_File(image_file_name, image().Get_Image())   &&
	    Read_From_File(alpha_image_file_name, image().Get_Alpha_Image()))
	{
	*/
		/*Dbg("\tLoaded from file.");
		Dbg("\timage      = (%d x %d)", 
		    image().Get_Image().Get_Size().x,
		    image().Get_Image().Get_Size().y);
		Dbg("\talpha_image= (%d x %d)", 
		    image().Get_Alpha_Image().Get_Size().x,
		    image().Get_Alpha_Image().Get_Size().y);
		    */
	/*	if (image().Get_Image().Get_Size() != dimension     ||
		    image().Get_Alpha_Image().Get_Size() != dimension)
		    */
			/* Images have wrong size */
//		{
			//Dbg("\tImages have wrong size."); 
/*			width= -1;
			return false;
		}
		return true;
	}
	*/

	//Dbg("\tLoading from file failed.");

	if (image().Get_Image().Is())
		image().Get_Image().Free();

#endif /* ! SCALED_BOARD_IMAGE_HAVE_MAP */ 

	/* Generate from source 
	 */ 
	//Dbg("\tGenerating stretched image."); 

	if (! (image().Get_Image().Create(dimension)       && 
	       image().Get_Image().Has_Canvas()            &&
	       image().Get_Alpha_Image().Create(dimension) &&
	       image().Get_Alpha_Image().Has_Canvas()))
	{
		image= NULL; 
		Get_Images()= -1;
		width= -1;
		return false;
	}		

	Pointer <Alpha_Server_Image> board= Board_Image::Get();

	if (! (board().Is()                        &&
	       board().Get_Image().Has_Canvas()    &&
	       board().Get_Alpha_Image().Has_Canvas()))
	{
		image= NULL; 
		Get_Images()= -1;
		width= -1; 
		return false;
	}

	/* Create stretched image and save to map or file. 
	 */
#ifdef SCALED_BOARD_IMAGE_HAVE_MAP

/*	if (client_image.Draw_Stretch_Canvas
	    (image().Get_Image()(), 
	     board().Get_Image()(),
	     dimension,
	     board().Get_Image().Get_Size())       &&
	    client_alpha_image.Draw_Stretch_Canvas
	    (image().Get_Alpha_Image()(),
	     board().Get_Alpha_Image()(),
	     dimension,
	     board().Get_Alpha_Image().Get_Size()),
	    anti_alias)
	{
		client_image.Write_Map(map_name);
		client_alpha_image.Write_Map(alpha_map_name);
		chmod(map_name(), 0644);
		chmod(alpha_map_name(), 0644);
	}
	*/

#else /* ! SCALED_BOARD_IMAGE_HAVE_MAP */ 

	image().Get_Image()().Draw_Stretch_Canvas
		(board().Get_Image()(),
		 dimension,
		 board().Get_Image().Get_Size(),
		 anti_alias);

	image().Get_Alpha_Image()().Draw_Stretch_Canvas2
		(board().Get_Alpha_Image()(),
		 dimension,
		 board().Get_Alpha_Image().Get_Size(),
		 anti_alias); 

	/* XXX  
	Write_To_File(image_file_name, image().Get_Image());
	Write_To_File(alpha_image_file_name, image().Get_Alpha_Image());
	chmod(image_file_name(), 0644);
	chmod(alpha_image_file_name(), 0644);
	 XXX */

#endif /* ! SCALED_BOARD_IMAGE_HAVE_MAP */ 

	return true;
}

void
Scaled_Board_Image::Remove()
{
	assert (width > 0);

	assert (Get_Images()[index]().width == width);
	assert (Get_Images()[index]().ref_count >= 1);

	-- Get_Images()[index]().ref_count;
	/* Keep unused sized. 
	 */
	width= -1;
}

void Scaled_Board_Image::Draw(Canvas &canvas,
			      Point origin,
			      int pt,
			      int cc)
{
	assert (pt >= 0 && pt < pt_count);
	assert (cc == cc_w || cc == cc_b);
	
	if (! (Get_Image().Has_Canvas()   &&
	       Get_Alpha_Image().Has_Canvas()))
		return;

	canvas.Set_Draw_Mode(Canvas::dm_andnot);
	canvas.Draw_Canvas(Get_Alpha_Canvas(),
		      Rect(origin,
			   Point(width)),
			   Point(width * pt,
				 width * cc));

	canvas.Set_Draw_Mode(Canvas::dm_or);
	canvas.Draw_Canvas(Get_Canvas(),
		      Rect(origin,
			   Point(width)),
		      Point(width * pt,
			    width * cc));
}
