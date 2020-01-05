#include "promotion_dialog.hh"
#include "board_image.hh"
#include "settings.h"
#include "dbus.h"
#include "chessui_i18n.hh"
#include <cstdio>
#include <gdk/gdkkeysyms.h>
#include <X11/Xlib.h>
#include <gdk/gdkx.h>

Promotion_Dialog::Promotion_Dialog(GtkWindow *parent)
{
    GError *error = NULL;
    const guint indices[4] = {4, 3, 2, 1};
    GdkPixbuf *piece = NULL;

    active = 0;
    dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
                _("game_ti_chess_promote_pawn_title"),
                parent,
                GTK_DIALOG_MODAL,
		dgettext ("hildon-libs", "wdgt_bd_done" ),
                GTK_RESPONSE_OK,
                NULL));

    /*hildon_help_dialog_help_enable(
            GTK_DIALOG(dialog),
            CHESS_HELP_PROMOTEPAWN,
            get_app_data()->app_osso_data->osso);*/

    pieces = gdk_pixbuf_new_from_file(PIXMAPSDIR "/" BOARD_IMAGE_NAME,
            &error);

    button_hbox = GTK_HBOX(gtk_hbox_new(TRUE, 10));

    for (int i = 0; i < 4; i++)
    {
        piece = gdk_pixbuf_new_subpixbuf(pieces,
                indices[i] * 56, 0, 56, 56);
        piece_images[i] = GTK_IMAGE(gtk_image_new_from_pixbuf(piece));
        g_object_unref(piece);
        piece = NULL;
        piece_buttons[i] = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());

        g_signal_connect(G_OBJECT(piece_buttons[i]), "clicked",
                G_CALLBACK(Promotion_Dialog::Handle_Click),
                this);

        g_signal_connect(G_OBJECT(piece_buttons[i]), "key_release_event",
                G_CALLBACK(Promotion_Dialog::Piece_Key_Press),
                this);

        gtk_container_add(GTK_CONTAINER(piece_buttons[i]),
                GTK_WIDGET(piece_images[i]));

        gtk_box_pack_start_defaults(GTK_BOX(button_hbox),
                GTK_WIDGET(piece_buttons[i]));

        gtk_widget_show(GTK_WIDGET(piece_images[i]));
        gtk_widget_show(GTK_WIDGET(piece_buttons[i]));
    }

    g_object_unref(pieces);

    g_signal_connect(G_OBJECT(dialog), "response",
            G_CALLBACK(Promotion_Dialog::Handle_Response), this);

    g_signal_connect(G_OBJECT(dialog), "key-press-event",
            G_CALLBACK(Promotion_Dialog::Dialog_Key_Press), this);
    //dialog_focus_out_id=g_signal_connect(dialog,"focus-out-event",G_CALLBACK(Promotion_Dialog::Handle_Focus_Out),this);			 	
    active = piece_buttons[0];
    gtk_toggle_button_set_active(active, TRUE);

    label = GTK_LABEL(gtk_label_new(_("game_fi_chess_promote_pawn_description")));

    gtk_widget_show(GTK_WIDGET(label));

    gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox),
            GTK_WIDGET(label));
    gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox),
            GTK_WIDGET(button_hbox));
    gtk_widget_show(GTK_WIDGET(button_hbox));

    gtk_widget_show(GTK_WIDGET(dialog));
    delete_called=FALSE;

    mutex = 0;

}

gboolean Promotion_Dialog::Handle_Focus_Out(GtkWidget *,
                                            GdkEventFocus *,
                                            gpointer user_data)
{
    Promotion_Dialog *_this = (Promotion_Dialog *)user_data;
    if (!focus_is_own())
    {
        /* focus went out of game, close dialog and pause game*/
        gtk_dialog_response(GTK_DIALOG(_this->dialog),
                GTK_RESPONSE_DELETE_EVENT);
        _this->delete_called=TRUE;
        chess_pause(NULL, NULL);
        return TRUE;
    }

    return  FALSE;
}

Promotion_Dialog::~Promotion_Dialog()
{
}

void Promotion_Dialog::Handle_Click(GtkWidget *widget, gpointer data)
{
    Promotion_Dialog *_this = (Promotion_Dialog *)data;
    if (_this->mutex)
        return;
    _this->mutex = 1;
    _this->delete_called=FALSE;

    if (_this->active && _this->active != GTK_TOGGLE_BUTTON(widget))
        gtk_toggle_button_set_active(_this->active, FALSE);
    else if (_this->active == GTK_TOGGLE_BUTTON(widget))
        gtk_toggle_button_set_active(_this->active, TRUE);

    _this->active = GTK_TOGGLE_BUTTON(widget);
    _this->mutex = 0;
}

void Promotion_Dialog::Show()
{
    /*
       for (int i = 0; i < 4; i++)
       gtk_toggle_button_set_active(piece_buttons[i], FALSE);
       active = piece_buttons[0];
       gtk_toggle_button_set_active(active, TRUE);
       */
    delete_called=FALSE;
    dialog_focus_out_id=g_signal_connect(dialog,"focus-out-event",G_CALLBACK(Promotion_Dialog::Handle_Focus_Out),this);
    gtk_widget_show(GTK_WIDGET(dialog));
}

int Promotion_Dialog::Get_Selected()
{
    AppData *app_data = get_app_data();

    Show();
    delete_called=FALSE;

    app_data->app_ui_data->dialog = dialog;	// Rama

    gtk_dialog_run(dialog);
    g_signal_handler_block(dialog,dialog_focus_out_id);
    gtk_widget_hide(GTK_WIDGET(dialog));
    if (!active)
        return -1;
    if (delete_called)
    {
        return -1;
    }
    for (int i = 0; i < 4; i++)
        if (active == piece_buttons[i])
        {
            /* active = NULL; */
            return i;
        }
    /* active = NULL; */
    return -1;
}

void Promotion_Dialog::Handle_Response(GtkDialog *widget,
        int arg,
        gpointer data)
{
    (void)arg;
    (void)data;
    AppData *app_data = get_app_data();
    Promotion_Dialog *_this = (Promotion_Dialog *)data;
    g_signal_handler_block(_this->dialog,_this->dialog_focus_out_id);
    app_data->app_ui_data->dialog = NULL;
    gtk_widget_hide(GTK_WIDGET(widget));

    //	AppData *app_data = get_app_data();
    //	app_data->app_ui_data->game->Dragged();
}

gboolean Promotion_Dialog::Piece_Key_Press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    (void)widget;

    if (event->state & (GDK_CONTROL_MASK |
                GDK_SHIFT_MASK |
                GDK_MOD1_MASK |
                GDK_MOD3_MASK |
                GDK_MOD4_MASK |
                GDK_MOD5_MASK)) {
        return FALSE;
    }

    switch (event->keyval)
    {
        case GDK_Up:
        case GDK_Down:
        case GDK_Left:
        case GDK_Right:
            Promotion_Dialog::Handle_Click(widget, data);
            return TRUE;
            break;
    }

    return FALSE;
}

gboolean Promotion_Dialog::Dialog_Key_Press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    (void)widget;
    (void)data;
    AppData *app_data = get_app_data();

        if ((event->keyval == GDK_Escape))
        {
            Promotion_Dialog *_this = (Promotion_Dialog *)data;
            gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_DELETE_EVENT);
            _this->delete_called=TRUE;
            //app_data->app_ui_data->game->Go_Backward();
            //app_data->app_ui_data->decrease_undo=TRUE;
	    app_data->app_ui_data->dialog = NULL;
            return TRUE;
        }
    if ((event->keyval == GDK_Return))
    {
        Promotion_Dialog *_this = (Promotion_Dialog *)data;
        gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_OK);
        _this->delete_called=FALSE;
	app_data->app_ui_data->dialog = NULL;
        return TRUE;
    }else
        if ((event->keyval == GDK_F5))
        {
            Promotion_Dialog *_this = (Promotion_Dialog *)data;
            gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_DELETE_EVENT);
            _this->delete_called=TRUE;
            //app_data->app_ui_data->game->Go_Backward();
            //app_data->app_ui_data->decrease_undo=TRUE;
	    app_data->app_ui_data->dialog = NULL;
            return TRUE;
        }
    
    app_data->app_ui_data->dialog = NULL;
    return FALSE;
}
