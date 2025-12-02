#include <gtk/gtk.h>
#include <glib.h>
#include "../app.h"

#ifdef _WIN32
    #include <windows.h> // Подключаем только на Windows
#else
    #include <limits.h>   // Для PATH_MAX
    #include <mach-o/dyld.h> // macOS-специфичный API для получения пути
    #include <stdlib.h>   // Для realpath()
#endif

gchar* get_exe_directory() {
    char path_buf[PATH_MAX]; // PATH_MAX определен в limits.h
    char actual_path[PATH_MAX];

#ifdef _WIN32
    // ===== РЕАЛИЗАЦИЯ ДЛЯ WINDOWS =====
    wchar_t wpath[MAX_PATH];
    if (GetModuleFileNameW(NULL, wpath, MAX_PATH) == 0) {
        return NULL;
    }
    // Преобразуем широкие символы в UTF-8 для согласованности
    char path[MAX_PATH];
    wcstombs(path, wpath, MAX_PATH);
    // Приводим путь к канонической форме (убираем символы ".." и ".")
    if (_fullpath(actual_path, path, PATH_MAX) == NULL) {
        return NULL;
    }
#else
    // ===== РЕАЛИЗАЦИЯ ДЛЯ MACOS (и других UNIX-систем) =====
    uint32_t size = sizeof(path_buf);
    if (_NSGetExecutablePath(path_buf, &size) != 0) {
        // Буфер слишком мал - выделяем память динамически
        char *dynamic_buf = (char *)g_malloc(size);
        if (_NSGetExecutablePath(dynamic_buf, &size) != 0) {
            g_free(dynamic_buf);
            return NULL;
        }
        // Получаем абсолютный канонический путь
        if (realpath(dynamic_buf, actual_path) == NULL) {
            g_free(dynamic_buf);
            return NULL;
        }
        g_free(dynamic_buf);
    } else {
        // Получаем абсолютный канонический путь
        if (realpath(path_buf, actual_path) == NULL) {
            return NULL;
        }
    }
#endif

    // ОБЩАЯ ЧАСТЬ: получаем директорию из полного пути
    char *dir = g_path_get_dirname(actual_path);
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
