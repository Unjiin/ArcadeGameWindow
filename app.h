#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>

#define ROWS 15
#define COLS 15

// Объявление структуры CellData
typedef struct CellData CellData;

typedef struct {
    GtkWidget *window;
    GtkWidget *stack;
    int *difficult;

    // Для сапера
    GtkWidget *minesweeper_grid;
    GtkWidget *minesweeper_box;
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
    AppData* data;
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
void back_to_menu(AppData *app_data);
void cleanup_minesweeper_game(AppData *app_data);
void showResults(AppData *app_data, int res);

#endif