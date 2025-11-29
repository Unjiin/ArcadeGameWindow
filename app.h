#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>

#define ROWS 11
#define COLS 25

// Объявление структуры CellData
typedef struct CellData CellData;

typedef struct {
    GtkWidget *window;
    GtkWidget *stack;
    int difficult;

    // Настройки
    GtkWidget *difficulty_label;
    double music_volume;
    double effects_volume;

    // Для сапера
    GtkWidget *minesweeper_grid;
    CellData **minesweeper_cells;
    GtkWidget *game_over_label;
    GtkWidget *flags_label;
    int bombs_count;
    int label_flags_count;
    int is_bombs_set;
    int game_time;
    guint timer_id;
    GtkWidget *time_label;
} AppData;

struct CellData {
    GtkWidget *button;
    int row;
    int col;
    int is_bomb;
    int is_open;
    int is_flagged;
    int neighbor_bombs;
    guint timeout_id;
};


// Прототипы функций
// Создание окон
GtkWidget* createMainMenu(AppData *data);
GtkWidget* createMinesweeperScreen(AppData *data);
GtkWidget* createSettingsScreen(AppData *data);

// Очистка
void cleanup_minesweeper_game(AppData *app_data);

// Музыка
void play_sound(const char *relative_path, int type, gboolean loop, AppData *data);
void init_sound_pool();
void add_hover_sound(GtkWidget *button, const char *sound_file, int type, gboolean loop, AppData *data);
void set_effects_volume(double vol, AppData *app_data);
void set_music_volume(double vol, AppData *app_data);
void stop_loop_sound(const char *relative_path);

// Изображение и GIF
GtkWidget *load_gif_animation_ms(const char *filename, int duration_ms);
GtkWidget* load_image(const char *filename);

// Виджеты
void showResults(AppData *app_data, int res);

// Утилиты
void back_to_menu(GtkWidget *widget, AppData *app_data);
void reopen_window(AppData *app_data, const char *str);
gchar* get_exe_directory();
#endif