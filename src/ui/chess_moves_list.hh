/**
    @file chess_moves_list.hh

    Prototypes for ChessMovesList

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

#ifndef CHESS_MOVES_LIST_HH
#define CHESS_MOVES_LIST_HH

#include <libosso.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "chess_log.h"
#include "babychess/widget.hh"

#define MOVES_LIST_NUMBER_SIZE 10
#define MOVES_LIST_TURNS_CNT 7
#define MOVES_LIST_FONT "Nokia Sans 16"
//#define MOVES_LIST_FONT "-*-helvetica-bold-r-normal--*-180-*-*-*-*-iso8859-1"
//#define MOVES_LIST_FONT "-*-helvetica-bold-r-normal--*-260-*-*-*-*-utf-8"

class Chess_Moves_List
{
public:
	/**
	 Constructor.

	 @param lab Label to write the list
	 */
	Chess_Moves_List(GtkLabel *lab) {
		g_assert( lab );

		moves = g_array_new(TRUE, TRUE, sizeof(gchar*));
		label = lab;
		gtk_label_set_use_markup(lab, true);
	}

	~Chess_Moves_List() {
		g_array_free(moves,TRUE);
	}

	/**
	 Add a move to the list

	 @param val String value
	 */
	void Add(const gchar *val) {
		g_assert( moves );
		g_assert( val );

		gchar *tmp = g_strdup(val);
		g_array_append_val(moves, tmp);

		Update();
	}

	/**
	 Update list item value at position.

	 @param pos Position of move to update
	 @param val New value
	 */
	void Update(guint pos, const gchar *val) {
		g_assert( moves );
		g_assert( val );

		if (pos>moves->len) Add(val);
		else {
			gchar *tmp = g_strdup(val);
			Remove(pos);
			g_array_insert_val(moves, pos, tmp);
		}

		Update();
	}

	/**
	 Resize list to defined amout of moves.

	 @param size New size of the list.
	 */
	void Resize(guint size) {
		g_assert( moves );

		if (size>moves->len) {
			for (unsigned int i=moves->len; i<size; i++) {
				Add("-");
			}
		} else
		if (moves->len>size) {
			while (moves->len>size) {
				Remove_Last();
			}
		}

	}

	/**
	 Remove move from specified index.

	 @param pos Index of move to be removed
	 */
	void Remove(guint pos) {
		g_assert( moves );

		if (moves->len==0 || pos>=moves->len) {
			osso_log(LOG_ERR,"Moves list is empty");
			return;
		}

		g_assert( pos<moves->len );

		gchar *tmp = g_array_index( moves, gchar*, pos);
		g_array_remove_index( moves, pos );
		g_free(tmp);

		Update();
	}

	/**
	 Removes the last move.
	 */
	void Remove_Last() {
		g_assert( moves );

		if (moves->len>0)
			Remove( moves->len-1 );
		else osso_log(LOG_ERR,"Moves list is empty");
	}

	/**
	 Remove last turn, two moves.
	 */
	void Remove_Turn() {
		g_assert( moves );

		if (moves->len>0) {
			Remove( moves->len-1 );

			if (moves->len>0 && moves->len%2==1) {
				Remove( moves->len-1 );
			}
		} else {
			osso_log(LOG_ERR,"Moves list is empty");
		}
	}

	/**
	 Clear all moves.
	 */
	void Clear() {
		g_assert( moves );

		if (moves->len==0) return;
		/*g_array_remove_range(moves, 0, moves->len);*/
		g_array_free(moves,TRUE);
		moves = g_array_new(TRUE, TRUE, sizeof(gchar*));
		Update();
	}

	guint Get_Length()
	{
		g_assert( moves );

		return moves->len;
	}

	gchar* Get(guint pos)
	{
		g_assert( moves );

		if (moves->len==0 || pos>=moves->len) {
			osso_log(LOG_ERR,"Moves list is empty");
			return NULL;
		}

		g_assert( pos<moves->len );

		gchar *tmp = g_array_index( moves, gchar*, pos);
		return tmp; /* Must be freed! */
	}

private:
	/**
	 Update the view
	 */
	void Update() {
		gint i=0, num=0, start=0;
		gchar number[MOVES_LIST_NUMBER_SIZE];
		GString *line = g_string_new("<span font_desc=\""
			MOVES_LIST_FONT "\">");

		num = moves->len;
		start = num;
		if (start%2==1) start++;
		start-=MOVES_LIST_TURNS_CNT*2;
		if (start<0) start=0;

		/* Build the list */
		for(i=start;i<num;i++) {
			if (i%2==0) {
				g_snprintf(number, MOVES_LIST_NUMBER_SIZE, "%d. ",(i/2)+1);
				number[MOVES_LIST_NUMBER_SIZE-1]=0;
				g_string_append(line, number);
			}
			gchar *tmp=g_array_index(moves,gchar*,i);
			if (tmp!=NULL) g_string_append(line, tmp);
			if (i%2==0) g_string_append(line, " ");
			if (i%2==1) g_string_append(line, "\n");
		}
		g_string_append(line, "</span>");

		gtk_label_set_markup( label, line->str );
		g_string_free(line, TRUE);
	}

	GArray *moves;
	GtkLabel *label;
};

#endif
