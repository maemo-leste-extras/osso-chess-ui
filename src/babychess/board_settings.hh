#ifndef BOARD_SETTINGS_INCLUDED
#define BOARD_SETTINGS_INCLUDED

#include "rgb.hh"
#include "pointer.hh"

/* flags */ 
enum {bs_turned=          (1 << 0),
      bs_frame=           (1 << 1),
      bs_square_names=    (1 << 2),
      bs_smooth=          (1 << 3),
      bs_tutor=           (1 << 4),
      bs_highlight_last=  (1 << 5),
      bs_riffled=         (1 << 6),

      bs_count= 7};

/* color indices */ 
enum {c_light,         /* light squares          */
      c_dark,          /* dark_squares           */
      c_last_move,     /* last move circle       */
      c_frame,         /* frame                  */
      c_highlight,     /* current from square... */
      c_text,          /* all text               */
      c_count};


class Board_Settings
	:  virtual public Pointed
{
public:

	int flags;

	RGB colors[c_count];
};

#endif /* ! BOARD_SETTINGS_INCLUDED */
