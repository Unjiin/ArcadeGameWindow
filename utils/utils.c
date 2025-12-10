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
    } else if (strcmp(str, "tictactoe") == 0) {
        new_screen = createTicTacToeScreen(app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    } else if (strcmp(str, "luckyGame") == 0) {
        new_screen = createLuckyGameScreen(app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    }

    if (new_screen) {
        gtk_stack_add_named(GTK_STACK(app_data->stack), new_screen, str);
        gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), str);
    }
}


void showResults(AppData *app_data, int res, GtkWidget *widget, int type, int grid_cols, int grid_rows) {
    // Останавливаем таймер только для сапёра
    if (type == 3) {
        if (app_data->minesweeper->timer_id != 0) {
            g_source_remove(app_data->minesweeper->timer_id);
            app_data->minesweeper->timer_id = 0;
        }
    }

    // Проигрываем звуки
    if (type == 3) {
        if (res == 0) {
            openAllGrid(app_data);
            play_sound("sounds/explosion.flac", 0, 0, app_data);
        } else {
            play_sound("sounds/bomb-has-been-defused.wav", 0, 0, app_data);
        }
    }


    // Главный оверлей — вертикальный бокс по центру
    GtkWidget *result_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
    gtk_widget_set_halign(result_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(result_box, GTK_ALIGN_CENTER);

    // Большая центральная картинка
    if (res < 2) {
        GtkWidget *big_emoji = load_image(res == 1 ? "win.svg" : "lose.svg");
        gtk_widget_set_size_request(big_emoji, 200, 200); // Ограничиваем размер
        gtk_box_append(GTK_BOX(result_box), big_emoji);
    }
    char *text;
    switch (res) {
        case 0:
            text = "Game Over";
            break;
        case 1:
            text = "You Win";
            break;
        case 2:
            text = "Draw";
            break;
        default:
            text = "Game Over";
    }
    // Текст под картинкой — используем CSS для размера
    GtkWidget *text_label = gtk_label_new(text);
    gtk_widget_add_css_class(text_label, "game-result-text");
    gtk_widget_add_css_class(text_label, res == 1 ? "win-message" : "lose-message");
    gtk_box_append(GTK_BOX(result_box), text_label);

    // Кладём весь бокс поверх поля
    gtk_grid_attach(GTK_GRID(widget), result_box, 0, 0, grid_cols, grid_rows);
    gtk_widget_set_visible(result_box, TRUE);

    // Блокируем клетки
    if (type == 3) { // Сапёр
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                gtk_widget_set_sensitive(app_data->minesweeper->cells[row][col].button, FALSE);
            }
        }
    } else if (type == 4) { // Крестики-нолики
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                gtk_widget_set_sensitive(app_data->tic_tac_toe->cells[row][col].button, FALSE);
            }
        }
    }
}