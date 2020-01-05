/**
    @file chess_enginecommunication.hh

    Prototypes for EngineCommunication

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

#ifndef CHESS_ENGINECOMMUNICATION_HH
#define CHESS_ENGINECOMMUNICATION_HH

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <signal.h>
#include <string.h>

#include "chess_log.h"
#include "chess_timing.hh"
#include "chess_communication.hh"

#define ENGINE_EXECUTABLE_PATH "/usr/games/gnuchess"

/* Define to use popen(). Otherwise pipe pair will be used.
   The popen way is simplier and better but does not provide feedback.
*/
//#define USE_POPEN
#ifndef USE_POPEN
#define READ_TIMEOUT 100
#endif

#define ENGINE_CMD_NEW     "new"
#define ENGINE_CMD_EASY    "easy"
#define ENGINE_CMD_WHITE   "white"
#define ENGINE_CMD_BLACK   "black"
#define ENGINE_CMD_NAME    "name"
#define ENGINE_CMD_LIMIT   "st"
#define ENGINE_CMD_LIMIT2  "depth"
#define ENGINE_CMD_MOVENOW "?"
#define ENGINE_CMD_DRAW    "draw"
#define ENGINE_CMD_HINT    "hint"
#define ENGINE_CMD_UNDO    "undo"
#define ENGINE_CMD_REMOVE  "remove"
#define ENGINE_CMD_GO      "go"
#define ENGINE_CMD_SAVE    "pgnsave"
#define ENGINE_CMD_LOAD    "pgnload"
#define ENGINE_CMD_QUIT    "quit"
#define ENGINE_CMD_FORCE   "force"

#define ENGINE_REPLY_ILLEGAL_MOVE "Illegal move"
#define ENGINE_REPLY_RESULT       "RESULT"
#define ENGINE_REPLY_MOVE         "My move is:"
#define ENGINE_REPLY_BLACK_WINS   "0-1"
#define ENGINE_REPLY_WHITE_WINS   "1-0"
#define ENGINE_REPLY_DRAW         "1/2-1/2"

typedef enum
{
  dl_easy=1,
  dl_medium=2,
  dl_hard=3,
  dl_hardest=4
} DifficultyLevel;

typedef enum
{
  cr_none=0,
  cr_white,
  cr_black,
  cr_draw
} ChessResult;

#define DIFFICULTY_TIME_EASY    0.1
#define DIFFICULTY_TIME_MEDIUM  1
#define DIFFICULTY_TIME_HARD    5
#define DIFFICULTY_TIME_HARDEST 10

#define DIFFICULTY_LEVEL_EASY    "0.1"
#define DIFFICULTY_LEVEL_MEDIUM  "1"
#define DIFFICULTY_LEVEL_HARD    "5"
#define DIFFICULTY_LEVEL_HARDEST "10"

#define DIFFICULTY_LEVEL2_EASY   "0.1"
#define DIFFICULTY_LEVEL2_MEDIUM "1"

/* Size of read buffer */
#define BUFFER_SIZE 128

/* Engine class */
class Engine 
{
private:
  #ifdef USE_POPEN
  FILE *engine_handle;
  #else
  int fd_to, fd_from;
  pid_t engine_pid;
  #endif

  gboolean engine_ok;
  gboolean thinking_stopped;
  gboolean human;
  guint go_cnt;
  gboolean force;
  gboolean force_go;

  ChessResult result;

  gboolean write(const gchar *cmd, const gchar *param);

  ChessTimer thinking_time;
  DifficultyLevel difficulty_level;

  GPtrArray *input_buffer;
  gchar *get_buffer();
public:
  Engine();
  ~Engine();
  gchar *read();
  inline gboolean ok() { return engine_ok; }
  void new_game();
  //gboolean players(gboolean white, gchar *name);
  gboolean players(gboolean white);
  gboolean difficulty(DifficultyLevel level);
  gboolean move(const gchar *command);
  gboolean force_move();
  gboolean offer_draw();
  gchar *hint();
  gboolean undo();
  gboolean go();
  gboolean remove();
  gboolean save(gchar *file);
  gboolean load(gchar *file);
  gboolean enough_time();
  void set_stopped(gboolean state) { thinking_stopped=state; }
  gboolean get_stopped() { return thinking_stopped; }
  void set_human() { human = TRUE; }
  gboolean is_human() { return human; }
  gboolean is_force() { return force; }
  void set_force() { force_go=TRUE; }
  void unset_force() { force_go=FALSE; }
  gboolean game_end() {
    if (result==cr_none) return FALSE;
    else return TRUE;
  }
  ChessResult get_result() { return result; }
};

Engine *get_chess_engine();
gboolean deinit_engine();
void set_engine_settings(Engine *chess_engine);

#endif
