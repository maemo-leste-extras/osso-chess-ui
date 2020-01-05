/**
    @file chess_status.cc

    Implements ChessStatus

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

#include "chess_status.hh"

#define STATUS_ACTIVE      "Black_White_active.jpg"
#define STATUS_PASSIVE     "Black_White_passive.jpg"
#define STATUS_LABEL_LEFT  60
#define STATUS_LABEL_TOP   -8
#define STATUS_FONT_SIZE   24000

/**
 Load specified type of status.

 @param st Status style
 @return True on success, false otherwise
 */
bool Chess_Status::Load(Chess_Status_Style st)
{
  if (active.Is()) active.Free();
  if (passive.Is()) passive.Free();

  style = st;

  Read_From_File( PIXMAPSDIR "/" STATUS_ACTIVE, active);
  Read_From_File( PIXMAPSDIR "/" STATUS_PASSIVE, passive);

  sized = false;
  return true;
}

/**
 Draw the widget

 @param canvas The canvas widget is drawn to
 */
void Chess_Status::Wdg_Draw(Canvas &canvas)
{
  Server_Image *image = NULL;

  if (status == true) 
    image = &active;
  else
    image = &passive;

  if ( ! (image->Is() && image->Has_Canvas()) )
    return;
  
  (*image)().Set_Draw_Mode(Canvas::dm_copy);

  Point size = image->Get_Size();
  size.x /= 2;

  canvas.Draw_Canvas((*image)(),
    Point(size.x,size.y),
    Point(size.x*style,0));
  
  if (text!=NULL && strlen(text)>0) {
    canvas.Draw_Label(Get(),
      STATUS_FONT_SIZE,
      Point(STATUS_LABEL_LEFT,
      size.y/3+STATUS_LABEL_TOP), text);
  }

  if (!sized) {
    Set_Size(size.x,size.y);
    sized=true;
  }
}
