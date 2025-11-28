#include "app.h"
#include <gtk/gtk.h>
#include <stdlib.h>

#define CELL_SIZE 40
// Структура для данных клетки
typedef struct {
    AppData *data;
    int row;
    int col;
} OnClickData;
static void update_cell_appearance(CellData *cell);
static GtkWidget* create_cell(int row, int col, AppData *app_data);
static void update_counts(AppData *app_data);
static void place_bombs(AppData *app_data, int exclude_row, int exclude_col) {
    int bombs_placed = 0;
    int total_cells = ROWS * COLS;
    int bomb_count = app_data->bombs_count;

    // Если бомб больше, чем клеток (минус исключенная), то скорректировать?
    if (bomb_count >= total_cells) {
        bomb_count = total_cells - 1;
    }

    while (bombs_placed < bomb_count) {
        int row = rand() % ROWS;
        int col = rand() % COLS;

        // Пропускаем исключенную клетку и уже занятые бомбами
        if ((row == exclude_row && col == exclude_col) || app_data->minesweeper_cells[row][col].is_bomb) {
            continue;
        }
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i == 0 && j == 0) continue;
                if (row + i >= 0 && row + i < ROWS && col + j >= 0 && col + j < COLS) app_data->minesweeper_cells[row + i][col + j].neighbor_bombs++;
            }
        }
        app_data->minesweeper_cells[row][col].is_bomb = 1;
        bombs_placed++;
    }
}
void cleanup_minesweeper_game(AppData *app_data) {
    if (app_data->timer_id != 0) {
        g_source_remove(app_data->timer_id);
        app_data->timer_id = 0;
    }

    /* Массив клеток — единственное, что мы аллоцировали сами */
    if (app_data->minesweeper_cells != NULL) {
        for (int row = 0; row < ROWS; row++) {
            if (app_data->minesweeper_cells[row] != NULL) {
                g_free(app_data->minesweeper_cells[row]);
            }
        }
        g_free(app_data->minesweeper_cells);
        app_data->minesweeper_cells = NULL;
    }

    /* Сброс состояния */
    app_data->minesweeper_grid = NULL;
    app_data->flags_label = NULL;
    app_data->time_label = NULL;
    app_data->game_over_label = NULL;

    // Сброс состояния
    app_data->is_bombs_set = 0;
    app_data->game_time = 0;
    app_data->bombs_count = 0;  // важно! будет переустановлено в createMinesweeperScreen
    app_data->label_flags_count = 0;
}
void showResults(AppData *app_data, int res) {
    // Останавливаем таймер
    if (app_data->timer_id != 0) {
        g_source_remove(app_data->timer_id);
        app_data->timer_id = 0;
    }

    // Главный оверлей — вертикальный бокс по центру
    GtkWidget *result_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
    gtk_widget_set_halign(result_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(result_box, GTK_ALIGN_CENTER);

    // Большая центральная картинка (твоя эмодзи-победа или взрыв)
    GtkWidget *big_emoji = load_image(res == 1 ? "win.svg" : "lose.svg");  // размер подстраивай под свои PNG
    gtk_box_append(GTK_BOX(result_box), big_emoji);

    // Текст под картинкой — жирный, огромный, как в старые добрые времена
    GtkWidget *text_label = gtk_label_new(res == 1 ? "You Win!!!" : "Game Over");
    gtk_label_set_markup(GTK_LABEL(text_label),
                        res == 1 ? "<span size='48000' weight='bold'>You Win!!!</span>"
                                 : "<span size='48000' weight='bold'>Game Over</span>");

    gtk_widget_add_css_class(text_label, "game-result-text");
    gtk_widget_add_css_class(text_label, res == 1 ? "win-message" : "lose-message");

    gtk_box_append(GTK_BOX(result_box), text_label);

    // Кладём весь бокс поверх поля
    gtk_grid_attach(GTK_GRID(app_data->minesweeper_grid), result_box, 0, 0, COLS, ROWS);
    gtk_widget_set_visible(result_box, TRUE);

    app_data->game_over_label = result_box;  // сохраняем, если потом понадобится

    // Блокируем клетки
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            gtk_widget_set_sensitive(app_data->minesweeper_cells[row][col].button, FALSE);
        }
    }
}
void checkWinOrLose(AppData *app_data) {
    int countClosed = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!app_data->minesweeper_cells[i][j].is_open) countClosed++;
        }
    }
    if (countClosed == app_data->bombs_count) showResults(app_data, 1);
}
gboolean update_timer_callback(gpointer user_data) {
    AppData *app_data = (AppData *)user_data;

    // Ваш код, который выполняется каждую секунду
    printf("Timer tick\n");

    // Обновление времени в сапере
    app_data->game_time++;
    g_autofree char *time_text = g_strdup_printf("%d", app_data->game_time);
    gtk_label_set_text(GTK_LABEL(app_data->time_label), time_text);

    // Возвращаем TRUE чтобы таймер продолжал работать
    return TRUE;
}
void on_cell_clicked_function(int row, int col, AppData *app_data) {

    if (!app_data->minesweeper_cells) {
        return;
    }

    CellData *cell = &app_data->minesweeper_cells[row][col];
    // Проверяем, можно ли открыть клетку
    if (cell->is_open || cell->is_flagged) {
        return;
    }

    // Размещаем бомбы при первом клике
    if (!app_data->is_bombs_set) {
        app_data->timer_id = g_timeout_add_seconds(1, update_timer_callback, app_data);
        place_bombs(app_data, cell->row, cell->col);
        app_data->is_bombs_set = 1;
    }

    // Открываем текущую клетку
    cell->is_open = 1;
    update_cell_appearance(cell);

    // ВАЖНО: Рекурсия только если у клетки 0 соседних бомб И она не бомба
    if (cell->neighbor_bombs == 0 && !cell->is_bomb) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;  // Пропускаем текущую клетку

                int ni = cell->row + i;
                int nj = cell->col + j;

                // Проверяем границы
                if (ni >= 0 && ni < ROWS && nj >= 0 && nj < COLS) {
                    CellData *neighbor = &app_data->minesweeper_cells[ni][nj];

                    // Рекурсия только для закрытых и нефлагированных клеток
                    if (!neighbor->is_open && !neighbor->is_flagged) {
                        on_cell_clicked_function(ni, nj, app_data);
                    }
                }
            }
        }
    }
    if (app_data->label_flags_count == 0) checkWinOrLose(app_data);
    if (cell->is_bomb) showResults(app_data, 0);
}
static void on_cell_clicked(GtkButton *button, gpointer user_data) {
    OnClickData *cell_from_signal = (OnClickData *)user_data;
    on_cell_clicked_function(cell_from_signal->row, cell_from_signal->col, cell_from_signal->data);
}

static void on_cell_right_click(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data) {
    OnClickData *cell_from_signal = (OnClickData *)user_data;
    AppData *app_data = cell_from_signal->data;

    // Получаем правильную клетку из массива
    CellData *cell = &app_data->minesweeper_cells[cell_from_signal->row][cell_from_signal->col];

    if (cell->is_open) return;

    if (app_data->label_flags_count > 0 || (app_data->label_flags_count == 0 && cell->is_flagged)) {
        if (cell->is_flagged) app_data->label_flags_count++;
        else app_data->label_flags_count--;

        update_counts(app_data);
        cell->is_flagged = !cell->is_flagged;
        update_cell_appearance(cell);
    }
    if (app_data->label_flags_count == 0) checkWinOrLose(app_data);
}
// Функция возврата в меню
void back_to_menu(GtkWidget *widget, AppData *app_data) {
    (void)widget;  // чтобы компилятор не срал варнингами
    reopen_window(app_data, "menu");
}

GtkWidget* createMinesweeperScreen(AppData *app_data) {
    // Главный контейнер
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    // Заголовок
    GtkWidget *title = gtk_label_new("Minesweeper");
    gtk_widget_add_css_class(title, "minesweeper-title");

    // Контейнер для игрового поля
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 2);
    gtk_widget_add_css_class(grid, "minesweeper-grid");

    switch (app_data->difficult) {
        case 0:
            app_data->bombs_count = 10;
            break;
        case 1:
            app_data->bombs_count = 30;
            break;
        case 2:
            app_data->bombs_count = 40;
            break;
        default:
            app_data->bombs_count = 20;
    }
    app_data->label_flags_count = app_data->bombs_count;
    // Создаём все клетки поля 9x9
    CellData **cells = g_new0(CellData*, ROWS);
    for (int row = 0; row < ROWS; row++) {
        cells[row] = g_new0(CellData, COLS);
        for (int col = 0; col < COLS; col++) {
            cells[row][col].row = row;
            cells[row][col].col = col;
            cells[row][col].is_open = FALSE;
            cells[row][col].is_flagged = FALSE;
            cells[row][col].is_bomb = FALSE;
            cells[row][col].neighbor_bombs = 0;
            cells[row][col].button = create_cell(row, col, app_data);

            update_cell_appearance(&cells[row][col]);
            gtk_grid_attach(GTK_GRID(grid), cells[row][col].button, col, row, 1, 1);
        }
    }
    // Панель информации (счётчик флагов, время и т.д.)
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
    gtk_widget_set_halign(info_box, GTK_ALIGN_CENTER);

    // Флаги
    GtkWidget *flag_pic = load_image("flag.svg");
    g_autofree char *flag_text = g_strdup_printf("%d", app_data->label_flags_count);
    GtkWidget *flags_label = gtk_label_new(flag_text);
    gtk_widget_add_css_class(flags_label, "minesweeper-info");
    gtk_widget_set_size_request(flag_pic, 20, 20);

    GtkWidget *flags_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(flags_box), flag_pic);
    gtk_box_append(GTK_BOX(flags_box), flags_label);

    // Время
    GtkWidget *clock_pic = load_image("clock.svg");
    GtkWidget *time_label = gtk_label_new("0");
    gtk_widget_add_css_class(time_label, "minesweeper-info");

    GtkWidget *time_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(time_box), clock_pic);
    gtk_box_append(GTK_BOX(time_box), time_label);

    gtk_box_append(GTK_BOX(info_box), flags_box);
    gtk_box_append(GTK_BOX(info_box), time_box);
    // Кнопка возврата
    GtkWidget *back_btn = gtk_button_new_with_label("← Назад в меню");
    gtk_widget_add_css_class(back_btn, "minesweeper-back-btn");
    gtk_widget_set_cursor(back_btn, gdk_cursor_new_from_name("pointer", NULL));
    g_signal_connect(back_btn, "clicked", G_CALLBACK(back_to_menu), app_data);

    // Собираем всё вместе
    gtk_box_append(GTK_BOX(main_box), title);
    gtk_box_append(GTK_BOX(main_box), info_box);
    gtk_box_append(GTK_BOX(main_box), grid);
    gtk_box_append(GTK_BOX(main_box), back_btn);

    gtk_widget_add_css_class(main_box, "minesweeper-screen");

    // Сохраняем указатель для будущего обновления
    app_data->minesweeper_grid = grid;
    app_data->minesweeper_cells = cells;
    app_data->flags_label = flags_label;
    app_data->time_label = time_label;
    return main_box;
}

// Функция создания клетки
static GtkWidget* create_cell(int row, int col, AppData *app_data) {
    GtkWidget *button = gtk_button_new();
    gtk_widget_set_size_request(button, CELL_SIZE, CELL_SIZE);
    gtk_widget_add_css_class(button, "minesweeper-cell");
    gtk_widget_add_css_class(button, "cell-closed");
    gtk_widget_set_cursor(button, gdk_cursor_new_from_name("pointer", NULL));
    OnClickData *data = g_malloc0(sizeof(OnClickData));
    data->data = app_data;
    data->row = row;
    data->col = col;
    // Левый клик
    g_signal_connect(button, "clicked", G_CALLBACK(on_cell_clicked), data);

    // Правый клик (для флажка)
    GtkGesture *gesture = gtk_gesture_click_new();
    GtkGestureClick *right_click = GTK_GESTURE_CLICK(gesture);
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click), GDK_BUTTON_SECONDARY);
    g_signal_connect(right_click, "pressed", G_CALLBACK(on_cell_right_click), data);
    gtk_widget_add_controller(button, GTK_EVENT_CONTROLLER(right_click));
    return button;
}
// Обновление счетчиков
static void update_counts(AppData *app_data) {
    g_autofree char *flag_text = g_strdup_printf("%d", app_data->label_flags_count);
    gtk_label_set_text(GTK_LABEL(app_data->flags_label), flag_text);
}


// Обновление внешнего вида клетки
static void update_cell_appearance(CellData *cell) {
    // Сбрасываем все классы
    gtk_widget_remove_css_class(cell->button, "cell-closed");
    gtk_widget_remove_css_class(cell->button, "cell-open");
    gtk_widget_remove_css_class(cell->button, "cell-flagged");
    gtk_widget_remove_css_class(cell->button, "cell-bomb");
    for (int i = 1; i <= 8; i++) {
        char cls[20];
        snprintf(cls, sizeof(cls), "cell-number-%d", i);
        gtk_widget_remove_css_class(cell->button, cls);
    }

    // ОЧИЩАЕМ СОДЕРЖИМОЕ КНОПКИ
    gtk_button_set_child(GTK_BUTTON(cell->button), NULL);
    gtk_button_set_label(GTK_BUTTON(cell->button), "");

    if (cell->is_open) {
        gtk_widget_add_css_class(cell->button, "cell-open");
        gtk_widget_set_cursor(cell->button, gdk_cursor_new_from_name("default", NULL));

        if (cell->is_bomb) {
            GtkWidget *bomb_pic = load_image("bomb.svg"); // чуть меньше, чтоб красиво
            gtk_button_set_child(GTK_BUTTON(cell->button), bomb_pic);
        } else if (cell->neighbor_bombs > 0) {
            char num_text[4];
            snprintf(num_text, sizeof(num_text), "%d", cell->neighbor_bombs);
            GtkWidget *num_label = gtk_label_new(num_text);
            char css_class[20];
            snprintf(css_class, sizeof(css_class), "cell-number-%d", cell->neighbor_bombs);
            gtk_widget_add_css_class(num_label, css_class);
            gtk_widget_add_css_class(num_label, "number-label"); // можешь добавить в CSS жирность
            gtk_button_set_child(GTK_BUTTON(cell->button), num_label);
        }
        // если 0 и не бомба — просто пусто, открытая клетка
    } else if (cell->is_flagged) {
        GtkWidget *flag_pic = load_image("flag.svg");
        gtk_button_set_child(GTK_BUTTON(cell->button), flag_pic);
        gtk_widget_add_css_class(cell->button, "cell-flagged");
    } else {
        gtk_widget_add_css_class(cell->button, "cell-closed");
    }
}

