/**
    @file chess_log.h

    Prototypes for main

    Copyright (c) 2009 Nokia Corporation.

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

#ifndef CHESS_LOG_H_USED
#define CHESS_LOG_H_USED

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Debugging in chess-ui incorrectly uses osso_log instead of ULOG_DEBUG */
#ifdef DEBUG
  #include <osso-log.h>
#else
  #define LOG_FUNCTIONS_H_

  #define ULOG_OPEN(arg)	((void)0)
  #define LOG_CLOSE()		((void)0)

  #ifdef osso_log
  #undef osso_log
  #endif
  #define osso_log(...)		((void)0)
#endif

#endif /* CHESS_LOG_H_USED */

