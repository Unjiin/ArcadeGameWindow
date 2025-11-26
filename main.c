#include <gtk/gtk.h>
#include <time.h>
#include <stdlib.h>
// #include "menu.h"
#include "app.h"
// gtk_widget_queue_draw(widget/button/window)
// Callback при нажатии на кнопки


int main(int argc, char *argv[]) {
    srand(time(0));
    gtk_init(&argc, &argv);
    int difficult = 0;
    AppData *app_data = g_malloc0(sizeof(AppData));
    // Создаём главное окно
    app_data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app_data->difficult = &difficult;
    gtk_window_set_title(GTK_WINDOW(app_data->window), "My Games");
    gtk_window_set_default_size(GTK_WINDOW(app_data->window), 1200, 1000);
    gtk_container_set_border_width(GTK_CONTAINER(app_data->window), 20);

    g_signal_connect(app_data->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    app_data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

    GtkWidget *main_menu = createMainMenu(app_data);

    gtk_stack_add_named(GTK_STACK(app_data->stack), main_menu, "menu");

    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "menu");

    gtk_window_set_child(GTK_WINDOW(app_data->window), app_data->stack);

    gtk_widget_show(app_data->window);

    gtk_main();

    g_free(app_data);

    // GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    // gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // openMenu(window, vbox, &difficult);
    // gtk_widget_show_all(window);
    // gtk_main();

    return 0;
}