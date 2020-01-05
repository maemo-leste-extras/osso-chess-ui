#ifndef PROMOTION_DIALOG_INCLUDED
#define PROMOTION_DIALOG_INCLUDED

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

class Promotion_Dialog
{
public:
	Promotion_Dialog(GtkWindow *parent);
	~Promotion_Dialog();

	int Get_Selected();
	void Show();
	static void Handle_Click(GtkWidget *widget, gpointer data);
	static void Handle_Response(GtkDialog *widget, int arg, gpointer data);
	static gboolean Piece_Key_Press(GtkWidget *widget, GdkEventKey *event, gpointer data);
	static gboolean Dialog_Key_Press(GtkWidget *widget, GdkEventKey *event, gpointer data);
  static gboolean Handle_Focus_Out(GtkWidget  *widget, GdkEventFocus *event,gpointer user_data);
private:
	GtkDialog *dialog;
	GdkPixbuf *pieces;
	GtkImage *piece_images[4];
	GtkToggleButton *piece_buttons[4];
	GtkHBox *button_hbox;
	GtkLabel *label;
	GtkToggleButton *active;
	gboolean mutex;
  guint dialog_focus_out_id;
  gboolean delete_called;
};

#endif /* ! PROMOTIION_DIALOG_INCLUDED */
