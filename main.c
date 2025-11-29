#include <gtk/gtk.h>
#include <time.h>
#include <stdlib.h>
#include "app.h"
#include <gst/gst.h>

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

    app_data->window = gtk_application_window_new(app);  // Сохраняем в структуру
    app_data->stack = gtk_stack_new();  // Создаем stack и сохраняем в структуру
    app_data->difficult = difficult;
    app_data->effects_volume = 1.0;
    app_data->music_volume = 0.2;

    gtk_window_set_title(GTK_WINDOW(app_data->window), "My Games");
    gtk_window_set_default_size(GTK_WINDOW(app_data->window), 1200, 1000);

    // Настраиваем stack
    gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

    // Создаем меню
    GtkWidget *main_menu = createMainMenu(app_data);
    // GtkWidget *minesweeper_screen = createMinesweeperScreen(app_data);

    gtk_stack_add_named(GTK_STACK(app_data->stack), main_menu, "menu");
    // gtk_stack_add_named(GTK_STACK(app_data->stack), minesweeper_screen, "minesweeper");
    play_sound("sounds/mainMenuTheme.ogg", 1, 1, app_data);
    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "menu");

    // Устанавливаем stack как дочерний виджет окна
    gtk_window_set_child(GTK_WINDOW(app_data->window), app_data->stack);
    gtk_widget_set_visible(app_data->window, TRUE);

    // Освобождаем память при закрытии окна
    g_signal_connect_swapped(app_data->window, "destroy", G_CALLBACK(g_free), app_data);
}

int main(int argc, char *argv[]) {
    // gchar *exe_dir = get_exe_directory();  // или используй get_exe_directory() из прошлого сообщения
    //
    // gchar *pixbuf_dir = g_build_filename(exe_dir, "lib", "gdk-pixbuf-2.0", "2.10.0", NULL);
    // gchar *cache_file = g_build_filename(pixbuf_dir, "loaders.cache", NULL);
    //
    // g_setenv("GDK_PIXBUF_MODULEDIR", pixbuf_dir, TRUE);
    // g_setenv("GDK_PIXBUF_MODULE_FILE", cache_file, TRUE);
    //
    // g_free(pixbuf_dir);
    // g_free(cache_file);
    gst_init(&argc, &argv);
    init_sound_pool();
    srand(time(0));

    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.my-games", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}