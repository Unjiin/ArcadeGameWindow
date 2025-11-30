#include <gtk/gtk.h>
#include <glib.h>
#include "../app.h"
#include <windows.h>
gchar* get_exe_directory() {
    wchar_t wpath[MAX_PATH];
    if (GetModuleFileNameW(NULL, wpath, MAX_PATH) == 0) {
        return NULL;
    }
    char path[MAX_PATH];
    wcstombs(path, wpath, MAX_PATH);

    char *dir = g_path_get_dirname(path);
    return dir;
}

void reopen_window(AppData *app_data, const char *str) {

    GtkWidget *old_screen = gtk_stack_get_child_by_name(GTK_STACK(app_data->stack), str);

    if (old_screen) {
        gtk_stack_remove(GTK_STACK(app_data->stack), old_screen);

        if (strcmp(str, "minesweeper") == 0) {
            cleanup_minesweeper_game(app_data);
        }
    }

    // Создаём свежий экран
    GtkWidget *new_screen = NULL;

    if (strcmp(str, "settings") == 0) {
        new_screen = createSettingsScreen(app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    } else if (strcmp(str, "minesweeper") == 0) {
        new_screen = createMinesweeperScreen(app_data);
        stop_loop_sound("sounds/mainMenuTheme.ogg");
        play_sound("sounds/mainMineSweeperTheme.mp3", 1, 1, app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    } else if (strcmp(str, "menu") == 0) {
        new_screen = createMainMenu(app_data);
        stop_loop_sound("sounds/mainMineSweeperTheme.mp3");
        play_sound("sounds/mainMenuTheme.ogg", 1, 1, app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_RIGHT);
    }

    if (new_screen) {
        gtk_stack_add_named(GTK_STACK(app_data->stack), new_screen, str);
        gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), str);
    }
}
