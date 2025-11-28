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
};


// Прототипы функций
GtkWidget* createMainMenu(AppData *data);
GtkWidget* createMinesweeperScreen(AppData *data);
GtkWidget* createSettingsScreen(AppData *data);
void back_to_menu(GtkWidget *widget, AppData *app_data);
void cleanup_minesweeper_game(AppData *app_data);
void showResults(AppData *app_data, int res);
void reopen_window(AppData *app_data, const char *str);
GtkWidget* load_image(const char *filename);
#endif