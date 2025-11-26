#include <gtk/gtk.h>
#include "utils.h"
const char *namesOfDifficulty[] = { "Easy", "Medium", "Hard" };
const int difficultyValues[] = { 10, 20, 50 };
gboolean on_button_enter(GtkWidget *widget, GtkEventCrossing *event, gpointer data) {
    GdkCursor *cursor = (GdkCursor*)data;
    GdkWindow *window = gtk_widget_get_window(widget);
    if (window)
        gdk_window_set_cursor(window, cursor);
    return FALSE;
}

gboolean on_button_leave(GtkWidget *widget, GdkEventCrossing *event, gpointer data) {
    GdkWindow *window = gtk_widget_get_window(widget);
    if (window)
        gdk_window_set_cursor(window, NULL); // сброс курсора
    return FALSE;
}