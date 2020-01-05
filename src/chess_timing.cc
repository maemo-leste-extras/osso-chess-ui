/**
    @file chess_timing.cc

    Implements ChessTimer module

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

#include "chess_timing.hh"

/**
 Start timing.
 */
void ChessTimer::start()
{
  if (ok()==TRUE) g_timer_start(t);
}

/**
 Stop timing.
 */
void ChessTimer::stop() {
  if (ok()==TRUE) g_timer_stop(t);
}

/**
 Continue stopped timing.
 */
void ChessTimer::cont() {
  stop();
  if (ok()==TRUE) g_timer_continue(t);
}

/**
 Restart timing.
 */
void ChessTimer::reset() {
  start();
}

/**
 Report elapsed time.

 @returns Elapsed time in seconds, including fractional part or -1 on error.
 */
gdouble ChessTimer::elapsed() {
  if (ok()==TRUE) return g_timer_elapsed(t,NULL);
  else return -1;
}
