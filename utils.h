#include <gtk/gtk.h>
extern const char *namesOfDifficulty[];
extern const int difficultyValues[];
gboolean on_button_enter(GtkWidget *widget, GdkEventCrossing *event, gpointer data);
gboolean on_button_leave(GtkWidget *widget, GdkEventCrossing *event, gpointer data);