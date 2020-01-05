#include "board_defaults.hh"
#include "rgb.hh"

bool Read_Defaults(Stream &stream, 
		 Board_Settings &board)
{
	(void)stream;

	//board.flags = bs_frame | bs_smooth | bs_tutor;
	board.flags = bs_frame | bs_tutor | bs_smooth;
	board.colors[0]=RGB(255,255,255);
	board.colors[1]=RGB(226,226,226);
	board.colors[2]=RGB(252,254,115);
	board.colors[3]=RGB(170,170,170);
	board.colors[4]=RGB(153,153,153);
	board.colors[5]=RGB(255,0,0);
	int i;
	for (i=6; i<c_count; i++)
		board.colors[i]=RGB(0,0,0);

	return true;
}
