/**
    @file chess_enginecommunication.cc

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

#include "chess_enginecommunication.hh"

gboolean stop_reading = FALSE;


Engine *chess_engine = NULL;

/**
 Engine class constructor. Starts chess engine, does initializations.

 */
Engine::Engine()
{
    engine_ok = FALSE;
    human = FALSE;
    go_cnt = 0;
    force = FALSE;
    force_go = FALSE;
    result = cr_none;

#ifdef USE_POPEN
    /* Generate command */
    GString *cmd = g_string_new(ENGINE_EXECUTABLE_PATH);
    cmd = g_string_append(cmd, " xboard >/dev/null");

    /* Execute and open pipe */
    engine_handle = popen(cmd->str, "w");

    g_string_free(cmd);

#else
    char *args[3];
    int to_engine[2], from_engine[2];
    gint stat = 0;
    gint *status = &stat;
    gint res = 0;
    pid_t pid;

    /* Generate command */
    args[0] = ENGINE_EXECUTABLE_PATH;
    args[1] = "xboard";
    args[2] = NULL;

    /* Ignore pipe signals */
    ::signal(SIGPIPE, SIG_IGN);

    /* Create pipes */
    ::pipe(to_engine);
    ::pipe(from_engine);

    engine_ok = TRUE;

    /* Forking, parent continues, child starts the engine */
    osso_log(LOG_DEBUG, "Forking...");
    pid =::fork();
    if (pid == 0)
    {
        /* Engine */
        osso_log(LOG_DEBUG, "Engine thread");

        /* Assigning pipes */
        osso_log(LOG_DEBUG, "Assigning pipes");

        ::dup2(to_engine[0], 0);
        ::dup2(from_engine[1], 1);
        ::close(to_engine[0]);
        ::close(to_engine[1]);
        ::close(from_engine[0]);
        ::close(from_engine[1]);
        ::dup2(1, fileno(stderr));

        /* Running engine */
        osso_log(LOG_DEBUG, "Running engine with command: %s %s", args[0],
                 args[1]);
        ::execvp(args[0], args);

        osso_log(LOG_DEBUG, "Engine exited");
        engine_ok = FALSE;

        /* Failed */
        _exit(42);
    }

    osso_log(LOG_DEBUG, "User interface thread");

    /* Parent/UI */
    ::close(to_engine[0]);
    ::close(from_engine[1]);

    /* Checking if engine is running */
    engine_pid = pid;
    osso_log(LOG_DEBUG, "Calling waitpid for pid %d", engine_pid);
    res = waitpid(engine_pid, status, WNOHANG);
    osso_log(LOG_DEBUG, "Waitpid returned with code %d", res);

    if (status == NULL)
    {
        osso_log(LOG_DEBUG, "Status was NULL");
    }
    else if (res > 0)
    {
        if (res == engine_pid)
        {
            engine_ok = FALSE;
            osso_log(LOG_DEBUG, "Engine pid %d exited", res);
        }

        if (WIFEXITED(*status))
        {
            engine_ok = FALSE;
            osso_log(LOG_DEBUG, "Engine exited, code %d",
                     WEXITSTATUS(*status));
        }
    }

    /* Set file descriptors */
    fd_from = from_engine[0];
    fd_to = to_engine[1];

    if (engine_ok)
    {
        osso_log(LOG_DEBUG, "Engine start ok");
    }
    else
    {
        osso_log(LOG_DEBUG, "Engine start failed");
    }
    osso_log(LOG_DEBUG, "Chess engine pid: %d\n", engine_pid);
#endif

    difficulty_level = dl_easy;
    thinking_stopped = FALSE;

    input_buffer = g_ptr_array_new();
    g_assert(input_buffer);
}

/**
 Engine class destructor.

 */
Engine::~Engine()
{
    /* Quit the engine */
    write(ENGINE_CMD_QUIT, NULL);

#ifdef USE_POPEN
    /* Close pipe */
    if (engine_handle != NULL)
        pclose(engine_handle);

#else
    int status;

    /* Close pipe */
    ::close(fd_from);
    ::close(fd_to);

    /* Check if engine still running */
    // waitpid(engine_pid,&status,WNOHANG);
    waitpid(engine_pid, &status, 0);

    /* Force termination */
    if (!WIFEXITED(status))
    {
        osso_log(LOG_DEBUG, "Killing engine: %d\n", engine_pid);
        g_print("Killing engine: %d\n", engine_pid);
        kill(engine_pid, SIGKILL);
    }
#endif

    if (input_buffer != NULL)
        g_ptr_array_free(input_buffer, TRUE);
}

/**
 Write command to engine

 @param data The command to be written
 @returns TRUE on success, FALSE otherwise
 */
gboolean
Engine::write(const gchar * cmd, const gchar * param)
{
    GString *tmp = NULL;
    g_assert(cmd);
    if (!cmd)
        return FALSE;
    size_t cnt = 0;

    /* Check if engine ok */
#ifndef USE_POPEN
    g_assert(engine_ok);
    if (!engine_ok)
        return FALSE;
#else
    g_assert(engine_handle);
    if (engine_handle == NULL)
        return FALSE;
#endif

    /* Generate command */
    tmp = g_string_new(cmd);

    /* Apply parameters */
    if (param != NULL)
    {
        g_assert(param);

        tmp = g_string_append(tmp, " ");
        tmp = g_string_append(tmp, param);
    }
    tmp = g_string_append(tmp, "\n");

    g_assert(tmp);

    osso_log(LOG_DEBUG, "To engine: %s", tmp->str);

    /* Write command */
#ifdef USE_POPEN
    cnt = fwrite(tmp->str, tmp->len, 1, engine_handle);

    if (cnt == 0)
    {
        g_string_free(tmp, TRUE);
        return FALSE;
    }
#else
    cnt =::write(fd_to, tmp->str, tmp->len);

    if (cnt != tmp->len)
    {
        g_string_free(tmp, TRUE);
        return FALSE;
    }
#endif

    g_string_free(tmp, TRUE);
    return TRUE;
}

/**
 Get a line from input buffer

 @returns Pointer to string or NULL on error
 */
gchar *
Engine::get_buffer()
{
    GString *tmp = NULL;
    AppData *app_data = get_app_data();

    if (!input_buffer)
        return NULL;

    if (input_buffer->len > 0)
    {
        gchar *res = NULL;
        tmp = (GString *) g_ptr_array_remove_index(input_buffer, 0);
        if (tmp == NULL)
        {
            return NULL;
        }
        g_assert(tmp);

        osso_log(LOG_DEBUG, "From engine: %s", tmp->str);
        if (strstr(tmp->str, "#") != NULL)
        {
            app_data->app_ui_data->mate_pos = TRUE;
        }
        else
        {
            app_data->app_ui_data->mate_pos = FALSE;
        }
        if (strstr(tmp->str, ENGINE_REPLY_BLACK_WINS) != NULL)
        {
            result = cr_black;
            stop_reading = TRUE;
        }
        if (strstr(tmp->str, ENGINE_REPLY_WHITE_WINS) != NULL)
        {
            result = cr_white;
            stop_reading = TRUE;
        }
        if (strstr(tmp->str, ENGINE_REPLY_DRAW) != NULL)
        {
            result = cr_draw;
            stop_reading = TRUE;
        }

        /* Duplicating string from GString, must be freed by the caller */
        // res = g_strdup(tmp->str);
        res = tmp->str;
        // g_string_free(tmp,TRUE);
        g_string_free(tmp, FALSE);
        return res;
        // return tmp->str;
    }
    else
    {
        return NULL;
    }
}

/**
 Read data from engine

 @return Line of data
 */
gchar *
Engine::read()
{
#ifndef USE_POPEN
    gchar buffer[BUFFER_SIZE + 1];
    GString *tmp = NULL;
    char *pos = NULL, *endp = NULL;

    g_assert(engine_ok);
    if (!engine_ok)
        return NULL;

    /* Poll if data exists */
    struct pollfd pd;
    pd.fd = fd_from;
    pd.events = POLLIN | POLLPRI;
    pd.revents = 0;

    /* If no data, return */
    if (::poll(&pd, 1, READ_TIMEOUT) <= 0) {
        return get_buffer();
    }
    if (pd.revents == 0) {
        return get_buffer();
    }

/* Read data */
    gint cnt;
    g_assert(input_buffer);
    tmp = g_string_new("");
    while ((cnt =::read(fd_from, buffer, BUFFER_SIZE)) > 0)
    {
        while (gtk_events_pending()) gtk_main_iteration();
        if (cnt >= 0 && cnt < BUFFER_SIZE)
            buffer[cnt] = 0;
        else
            buffer[BUFFER_SIZE] = 0;

        pos = buffer;
        endp = strchr(pos, '\n');
        if (endp == NULL)
        {
            g_string_append(tmp, pos);
            g_assert(tmp);
            g_ptr_array_add(input_buffer, (gpointer) tmp);
            tmp = g_string_new("");
        }
        else
        {
            while (endp != NULL)
            {
                *endp = 0;

                /* Add string to input buffer */
                g_string_append(tmp, pos);
                g_assert(tmp);
                g_ptr_array_add(input_buffer, (gpointer) tmp);
                tmp = g_string_new("");

                pos = endp + 1;
                endp = strchr(pos, '\n');
            }
            if (strlen(pos) > 0)
            {
                g_string_append(tmp, pos);
                g_assert(tmp);
                g_ptr_array_add(input_buffer, (gpointer) tmp);
                tmp = g_string_new("");
            }
        }

        /* If no data, don't try to read it */
        if (::poll(&pd, 1, READ_TIMEOUT) <= 0)
            break;
        if (pd.revents == 0)
            break;
    }
    if (tmp != NULL && tmp->len > 0)
    {
        g_assert(tmp);
        g_ptr_array_add(input_buffer, (gpointer) tmp);
    }
    else if (tmp != NULL)
    {
        g_string_free(tmp, TRUE);
    }
    return get_buffer();

#else
    return NULL;

#endif

}

/**
 Start new game
 */
void
Engine::new_game()
{
    write(ENGINE_CMD_NEW, NULL);
    write(ENGINE_CMD_EASY, NULL);
}

/**
 Make a move

 @param cmd The move to make
 @returns TRUE on success, FALSE otherwise
 */
gboolean
Engine::move(const gchar * command)
{
#ifndef USE_POPEN
    gchar *res = NULL;
#endif

    if (!command)
        return FALSE;

    unsigned int i = 0;
    gchar *cmd = g_strdup(command);

    while (i < strlen(cmd) && cmd[i] != 0)
    {
        if (cmd[i] == '*')
        {
            cmd[i] = 0;
            force_go = TRUE;
            break;
        }
        i++;
    }

    /* Write move to engine */
    if (write(cmd, NULL) == FALSE)
    {
        g_free(cmd);
        return FALSE;
    }

    if (force && force_go)
        go();


    /* Start calculating thinking time */
    thinking_time.start();

#ifndef USE_POPEN
    /* Read reply */
    gboolean result = TRUE;
    gboolean gotmove = FALSE;
    do
    {
        if (res != NULL)
            g_free(res);

        if (gtk_events_pending()) 
            gtk_main_iteration(); 

        res = read();
        if (res == NULL)
            continue;
        g_assert(res);

        /* Got move from engine */
        if (strstr(res, ENGINE_REPLY_MOVE) != NULL)
        {
            GString *tmp = g_string_new(res);
            g_ptr_array_add(input_buffer, (gpointer) tmp);
            g_free(res);
            res = NULL;
            break;
        }

        /* Got illegal move reply from engine */
        if (strstr(res, ENGINE_REPLY_ILLEGAL_MOVE) != NULL)
        {
            result = FALSE;
        }
        /* End of the game */
        if (game_end())
        {
            chess_end(get_result());
            result = TRUE;
            g_free(res);
            res = NULL;
            break;
        }
        if (strstr(res, cmd) != NULL)
        {
            gotmove = TRUE;
        }
    } while (res != NULL || !gotmove || (res != NULL && gotmove));
    if (res != NULL)
        g_free(res);
#endif
    g_free(cmd);
    return result;
}

/**
 Set players information

 @param white Human player color, TRUE if white, FALSE if black
 @param name Human playr name
 @return TRUE on success, FALSE otherwise
 */
// gboolean Engine::players(gboolean white, gchar *name)
gboolean
Engine::players(gboolean white)
{
    gboolean res = white || TRUE;

    /* Set player color */
    /* if (white) { if (write(ENGINE_CMD_WHITE, NULL)==FALSE) res = FALSE; }
     * else { if (write(ENGINE_CMD_BLACK, NULL)==FALSE) res = FALSE; } */

    /* Set player name */
    // if (write(ENGINE_CMD_NAME, name)==FALSE) res = FALSE;

    return res;
}

/**
 Force engine to move

 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::force_move()
{
    thinking_time.stop();
    kill(engine_pid, SIGINT);
    return true;
    // return write(ENGINE_CMD_MOVENOW, NULL);
}

/**
 Set difficulty

 @param level Difficulty level
 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::difficulty(DifficultyLevel level)
{
    GString *tmp = NULL;
    gboolean res = TRUE;

    difficulty_level = level;
    switch (level)
    {
        case dl_easy:
            tmp = g_string_new(DIFFICULTY_LEVEL_EASY);
            res = write(ENGINE_CMD_LIMIT, DIFFICULTY_LEVEL2_EASY);
            break;
        case dl_medium:
            tmp = g_string_new(DIFFICULTY_LEVEL_MEDIUM);
            res = write(ENGINE_CMD_LIMIT, DIFFICULTY_LEVEL2_MEDIUM);
            break;
        case dl_hard:
            tmp = g_string_new(DIFFICULTY_LEVEL_HARD);
            break;
        case dl_hardest:
            tmp = g_string_new(DIFFICULTY_LEVEL_HARDEST);
            break;
        default:
            return FALSE;
    }

    g_assert(tmp);

    if (res)
        res = write(ENGINE_CMD_LIMIT, tmp->str);

    g_string_free(tmp, TRUE);

    return res;
}

/**
 Offer draw to engine

 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::offer_draw()
{
    return write(ENGINE_CMD_DRAW, NULL);
}

/**
 Ask hint from engine

 @return Hint or NULL on error
 */
gchar *
Engine::hint()
{
    if (write(ENGINE_CMD_HINT, NULL) == TRUE)
        return read();

    return NULL;
}

/**
 Undo last move

 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::undo()
{
    result = cr_none;
    return write(ENGINE_CMD_UNDO, NULL);
}

/**
 Force engine to start thinking

 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::go()
{
    go_cnt = 0;
    force = FALSE;
    return write(ENGINE_CMD_GO, NULL);
}

/**
 Retract move. Engine undoes two moves.

 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::remove()
{
    result = cr_none;
    gboolean res = write(ENGINE_CMD_REMOVE, NULL);
    if (res)
    {
        res = write(ENGINE_CMD_FORCE, NULL);
        go_cnt = 1;
        force_go = FALSE;
        force = TRUE;
    }
    return res;
}

/**
 Save the game to a file.

 @param file Target file name.
 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::save(gchar * file)
{
    return write(ENGINE_CMD_SAVE, file);
}

/**
 Load the game from a file.

 @param file File name to load from.
 @return TRUE on success, FALSE otherwise
 */
gboolean
Engine::load(gchar * file)
{
    return write(ENGINE_CMD_LOAD, file);
}

/**
 Reports if engine got enough thinking time

 @return TRUE if enough time, FALSE otherwise
 */
gboolean
Engine::enough_time()
{
    gdouble time = thinking_time.elapsed();

    switch (difficulty_level)
    {
        case dl_easy:
            if (time > DIFFICULTY_TIME_EASY / 2)
                return TRUE;
        case dl_medium:
            if (time > DIFFICULTY_TIME_MEDIUM / 2)
                return TRUE;
        case dl_hard:
            if (time > DIFFICULTY_TIME_HARD / 2)
                return TRUE;
        case dl_hardest:
            if (time > DIFFICULTY_TIME_HARDEST / 2)
                return TRUE;
    }

    return FALSE;
}

/**
 Get pointer to an instance of Engine class.

 @return Pointer to Engine class of currently running engine.
 */
Engine *
get_chess_engine()
{
    /* Chech if engine started */
    if (chess_engine == NULL)
    {

        /* Start engine */
        chess_engine = new Engine();

        g_assert(chess_engine);

        /* Can't start engine */
        if (!chess_engine->ok())
        {
            delete chess_engine;
            chess_engine = NULL;
        }
        else
            chess_engine->new_game();
    }

    return chess_engine;
}

/**
 Destroys engine

 @return TRUE on success, FALSE otherwise
 */
gboolean
deinit_engine()
{
    if (chess_engine)
    {
        delete chess_engine;
        chess_engine = NULL;
    }
    return TRUE;
}

/**
 Reads settings from GConf and sets corresponding settings to engine.
 */
void
set_engine_settings(Engine * chess_engine)
{
    if (!chess_engine)
        return;

    /* Player color selection */
    gboolean white = get_app_data()->app_ui_data->player_color_white;
    chess_engine->players(white);

    /* Two human players? */
    gboolean human = get_app_data()->app_ui_data->opponent_type_human;

    /* If not human, set computer difficulty */
    if (human == FALSE)
    {
        gint level = get_app_data()->app_ui_data->difficulty_level;

        chess_engine->difficulty((DifficultyLevel) (level + 1));
    }
    else
        chess_engine->set_human();
}
