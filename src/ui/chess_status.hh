/**
    @file chess_status.hh

    Prototypes for ChessStatus

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

#ifndef CHESS_STATUS_HH
#define CHESS_STATUS_HH

#include "babychess/widget.hh"
#include "babychess/stream_file.hh"
#include "babychess/server_image.hh"
#include "babychess/server_image_stream.hh"

/* Status style black or white player */
enum Chess_Status_Style { cs_white=0, cs_black };

class Chess_Status :
  public Widget
{
public:
  /**
   Constructor, initialize variables.
   */
  Chess_Status() : Widget() {
    text = NULL;
    sized = false;
    status = false;
  }

  ~Chess_Status() {
    if (text!=NULL) g_free(text);
  }

  bool Load(Chess_Status_Style);
  void Wdg_Draw(Canvas &);

  /**
   Set text to view (player name).

   @param label New text.
   */
  void Set_Label(const gchar *label) {
    if (text!=NULL) g_free(text);
    text = g_strdup(label);

    Update();
  }

  /**
   Set widget (un)active.

   @param st True to set active, false to not.
   */
  void Set_Active(bool st) {
    sized = false;
    status = st;

    Update();
  }

  gboolean Get_Active() {
    return status;
  }

  /**
   Update the view of widget
   */
  void Update() {
    if (! Is()) return;
    Point size = Get_Size();
    gtk_widget_queue_draw_area(Get(), 0, 0, size.x, size.y);
  }

private:
  Server_Image active, passive;
  Chess_Status_Style style;
  gchar *text;
  bool status;
  bool sized;
};

#endif
