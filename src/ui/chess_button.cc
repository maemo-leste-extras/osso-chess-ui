/**
    @file chess_button.cc

    Implements ChessButton.

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

#include "chess_button.hh"
#include "dbus.h"

#define BUTTONS "Undo_Redo.jpg"
#define BUTTONS_DIMMED "Undo_Redo_dimmed.jpg"
#define BUTTONS_PRESSED "Undo_Redo_pressed.jpg"


/**
 Load button of specified style

 @param st Button style
 @return TRUE on success, FALSE otherwise
 */
bool Chess_Button::Load(Chess_Button_Style st)
{
	if ( buttons.Is() ) return false;

	guard = FALSE;
	style = st;
	mode = cbm_normal;

	Read_From_File( PIXMAPSDIR "/" BUTTONS, buttons);
	Read_From_File( PIXMAPSDIR "/" BUTTONS_DIMMED, dimmed);
	Read_From_File( PIXMAPSDIR "/" BUTTONS_PRESSED, pressed);

	sized=false;
	return true;
}

/**
 Draw the widget

 @param canvas The canvas widget is drawn to
 */
void Chess_Button::Wdg_Draw(Canvas &canvas)
{
	Server_Image *image;

	if (is_dimmed) mode = cbm_dimmed;

	switch (mode) {
		case cbm_pressed:
			image = &pressed;
			break;

		case cbm_dimmed:
			image = &dimmed;
			break;

		default:
		case cbm_normal:
			image = &buttons;
			break;
	}

	if ( ! (image->Is() && image->Has_Canvas()) ) 
		return;
	
	(*image)().Set_Draw_Mode(Canvas::dm_copy);

	Point size = image->Get_Size();
	size.x /= 2;

	canvas.Draw_Canvas((*image)(),
		Point(size.x,size.y),
		Point(size.x*style,0));

	if (!sized) {
		Set_Size(size.x,size.y);
		sized=true;
	}
}

/**
  Handle mouse events

  @param ma Mouse action status
  @param mb Mouse button status
  @param p Mouse position
  @param ac True if ok, false otherwise
  */
void Chess_Button::Wdg_Mouse(Mouse_Action ma,
	Mouse_Button mb,
	Point p,
	bool ac)
{
	(void)p;
	AppData *app_data = get_app_data();

    if (!app_data || !app_data->app_ui_data || !app_data->app_ui_data->game) {
        return;
    }

	Point size = buttons.Get_Size();

	if (mode == cbm_dimmed) return;

	if (guard) return;
	guard = TRUE;

	if (ma == ma_down && mb == mb_left && ac) {
		switch (style) {
			case cb_undo:
			app_data->app_ui_data->game->Go_Backward();
			break;

			case cb_redo:
			app_data->app_ui_data->game->Go_Forward();
			break;
		}
		if (mode != cbm_pressed) {
			mode = cbm_pressed;
			Update();
		}
	} else
	if (mode != cbm_normal) {
		mode = cbm_normal;
		Update();
	}

	guard = FALSE;
}
