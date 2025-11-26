#include <gtk/gtk.h>
#include <time.h>
#include <stdlib.h>
#include "app.h"

static void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;

    // Попробуем загрузить из файла
    if (g_file_test("style.css", G_FILE_TEST_EXISTS)) {
        gtk_css_provider_load_from_path(provider, "style.css");
    }

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(provider);
}

static void activate(GtkApplication *app, gpointer user_data) {
    int difficult = 0;
    AppData *app_data = g_malloc0(sizeof(AppData));

    load_css();
    // ИНИЦИАЛИЗИРУЕМ AppData правильно!
    app_data->window = gtk_application_window_new(app);  // Сохраняем в структуру
    app_data->stack = gtk_stack_new();  // Создаем stack и сохраняем в структуру
    app_data->difficult = &difficult;

    gtk_window_set_title(GTK_WINDOW(app_data->window), "My Games");
    gtk_window_set_default_size(GTK_WINDOW(app_data->window), 1200, 1000);

    // Настраиваем stack
    gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

    // Создаем меню
    GtkWidget *main_menu = createMainMenu(app_data);
    GtkWidget *minesweeper_screen = createMinesweeperScreen(app_data);

    gtk_stack_add_named(GTK_STACK(app_data->stack), main_menu, "menu");
    gtk_stack_add_named(GTK_STACK(app_data->stack), minesweeper_screen, "minesweeper");

    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "menu");

    // Устанавливаем stack как дочерний виджет окна
    gtk_window_set_child(GTK_WINDOW(app_data->window), app_data->stack);
    gtk_widget_show(app_data->window);

    // Освобождаем память при закрытии окна
    g_signal_connect_swapped(app_data->window, "destroy", G_CALLBACK(g_free), app_data);
}

int main(int argc, char *argv[]) {
    srand(time(0));

    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.my-games", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}