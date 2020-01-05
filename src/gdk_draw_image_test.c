/**
    @file gdk_graw_image_test.c

    Gdk image test.

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

#include <gtk/gtk.h>
#include <gdk/gdk.h>

gboolean drawme(GtkWidget *draw)
{
	GdkImage *image;
	int i;

	image = gdk_image_new(
		GDK_IMAGE_FASTEST,
		//gdk_rgb_get_visual(),
		gdk_visual_get_system(),
		200,
		200
		);
	
	for (i=0; i<10; i++) {
		gdk_image_put_pixel(image, 10+i, 10+i, 0xFFFF);
		gdk_image_put_pixel(image, 20, 10+i, 0x00FF00);
	}

	gdk_draw_image( draw->window,
		draw->style->fg_gc[GTK_STATE_NORMAL],
		image,
		0,0,
		0,0,
		200,200
		);

	//return FALSE;
	return TRUE;
}

int main(int argc, char **argv) {
	GtkWidget *win;
	GtkWidget *draw;

	gtk_init(&argc, &argv);

	win = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_widget_set_usize( win, 800, 400);

	draw = gtk_drawing_area_new();
	gtk_widget_set_size_request( draw, 800, 400);
	gtk_container_add(GTK_CONTAINER (win), draw);

	g_timeout_add(100,(GtkFunction)drawme,draw);

	gtk_widget_show_all(win);

	gtk_main();

	return 0;
}
