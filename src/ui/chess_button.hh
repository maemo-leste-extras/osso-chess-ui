/**
    @file chess_button.hh

    Prototypes for ChessButton

    Copyright (c) 2004, 2005 Nokia Corporation.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#ifndef CHESS_BUTTON_HH
#define CHESS_BUTTON_HH

#include "babychess/widget.hh"
#include "babychess/stream_file.hh"
#include "babychess/server_image.hh"
#include "babychess/server_image_stream.hh"

/* Button styles, Undo or Redo */
enum Chess_Button_Style { cb_undo=0, cb_redo };
enum Chess_Button_Mode { cbm_normal=0, cbm_pressed, cbm_dimmed};

class Chess_Button :
	public Widget
{
public:
	bool Load(Chess_Button_Style st);
	void Wdg_Draw(Canvas &canvas);
	void Wdg_Mouse(Mouse_Action, Mouse_Button, Point, bool);

	/**
	 Set button (un)dimmed.

	 @param dim TRUE to dim, FALSE to undim
	 */
	void Dimmed(gboolean dim) {
		is_dimmed = dim;
		if (is_dimmed==FALSE) {
			mode = cbm_normal;
		}

		Update();
	}

	/**
	 Check whether button is dimmed.

	 @returns TRUE if dimmed, FALSE otherwise
	 */
	gboolean Is_Dimmed() {
		return is_dimmed;
	}

private:
	/**
	 Update widget
	 */
	void Update() {
		Point size = Get_Size();
		gtk_widget_queue_draw_area(Get(), 0, 0, size.x, size.y);
	}

	Server_Image buttons, dimmed, pressed;
	Chess_Button_Style style;
	Chess_Button_Mode mode;
	bool sized;
	gboolean is_dimmed;
	gboolean guard; /* main_iteration is called while waiting for the engine
					 * to reply to make the ui responsive; when user clicks
					 * redo too fast, Go_Forward could be called one more,
					 * before the previous run ended, which will result in the
					 * ui and engine out of sycn, so this is to guard these
					 * calls */
};

#endif
