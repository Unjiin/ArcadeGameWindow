#include "../app.h"
#include <stdlib.h>
// Структура для данных клетки
typedef struct {
    AppData *data;
    int row;
    int col;
} OnClickData;
typedef struct {
    int score;
    int row;
    int col;
} MinimaxResult;

static void check_and_open_final_menu(CellTicTacToeStatus board[3][3], AppData *app_data);
static void copy_board(CellTicTacToeData **src, CellTicTacToeStatus dest[3][3]);
void set_tictactoe_cell(AppData *app_data, int row, int col);
static void get_empty_cells(CellTicTacToeStatus board[3][3], int out[9][2], int *count);
static gboolean check_win(CellTicTacToeStatus board[3][3], CellTicTacToeStatus player);
static int evaluate(CellTicTacToeStatus b[3][3], int depth, CellTicTacToeStatus ai, CellTicTacToeStatus hu);
static MinimaxResult minimax_basic(CellTicTacToeStatus board[3][3],
                                    int depth,
                                    int max_depth,
                                    CellTicTacToeStatus player,
                                    CellTicTacToeStatus aiPlayer,
                                    CellTicTacToeStatus huPlayer);
static gboolean ai_make_move(gpointer user_data);

void set_tictactoe_cell(AppData *app_data, int row, int col) {
    GtkWidget *cell_button = app_data->tic_tac_toe->cells[row][col].button;
    GtkWidget *current_child = gtk_button_get_child(GTK_BUTTON(cell_button));

    if (current_child) {
        gtk_widget_unparent(current_child);
    }
    char *image_filename;
    switch (app_data->tic_tac_toe->last_pick) {
        case Cross:
            image_filename = "circle.png";
            app_data->tic_tac_toe->last_pick = Circle;
            app_data->tic_tac_toe->cells[row][col].status = Cross;
            break;
        case Circle:
            image_filename = "cross.png";
            app_data->tic_tac_toe->last_pick = Cross;
            app_data->tic_tac_toe->cells[row][col].status = Circle;
            break;
        default:
            image_filename = "cross.png";
            break;
    }
    GtkWidget *symbol = load_image(image_filename);
    gtk_widget_set_halign(symbol, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(symbol, GTK_ALIGN_CENTER);
    gtk_button_set_child(GTK_BUTTON(cell_button), symbol);
    CellTicTacToeStatus board[3][3];
    copy_board(app_data->tic_tac_toe->cells, board);
    check_and_open_final_menu(board, app_data);
}

// Полная копия доски для minimax
static void copy_board(CellTicTacToeData **src, CellTicTacToeStatus dest[3][3]) {
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            dest[r][c] = src[r][c].status;
        }
    }
}

static void get_empty_cells(CellTicTacToeStatus board[3][3], int out[9][2], int *count) {
    *count = 0;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (board[r][c] == Empty) {
                out[*count][0] = r;
                out[*count][1] = c;
                (*count)++;
            }
        }
    }
}

static gboolean check_win(CellTicTacToeStatus board[3][3], CellTicTacToeStatus player) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player &&
            board[i][1] == player &&
            board[i][2] == player) return TRUE;

        if (board[0][i] == player &&
            board[1][i] == player &&
            board[2][i] == player) return TRUE;
    }
    if (board[0][0] == player &&
        board[1][1] == player &&
        board[2][2] == player) return TRUE;

    if (board[0][2] == player &&
        board[1][1] == player &&
        board[2][0] == player) return TRUE;

    return FALSE;
}
static int evaluate(CellTicTacToeStatus b[3][3], int depth,
                    CellTicTacToeStatus ai, CellTicTacToeStatus hu) {

    // win AI
    if (check_win(b, ai))
        return 10 - depth;

    // win human
    if (check_win(b, hu))
        return depth - 10;

    return 0; // draw or not terminal
}
static MinimaxResult minimax_basic(CellTicTacToeStatus board[3][3],
                                    int depth,
                                    int max_depth,
                                    CellTicTacToeStatus player,
                                    CellTicTacToeStatus aiPlayer,
                                    CellTicTacToeStatus huPlayer)
{
    int empty[9][2];
    int emptyCount;
    get_empty_cells(board, empty, &emptyCount);

    // terminal states
    int score = evaluate(board, depth, aiPlayer, huPlayer);

    if (score != 0 || depth == max_depth) {
        // победа / поражение
        return (MinimaxResult){ score, -1, -1 };
    }
    if (emptyCount == 0) {
        return (MinimaxResult){ 0, -1, -1 };
    }

    // Simulate all moves
    MinimaxResult moves[9];
    for (int i = 0; i < emptyCount; i++) {
        int r = empty[i][0];
        int c = empty[i][1];

        board[r][c] = player; // place move

        MinimaxResult result;
        if (player == aiPlayer) {
            result = minimax_basic(board, depth + 1, max_depth, huPlayer, aiPlayer, huPlayer);
        } else {
            result = minimax_basic(board, depth + 1, max_depth, aiPlayer, aiPlayer, huPlayer);
        }

        moves[i].score = result.score;
        moves[i].row = r;
        moves[i].col = c;

    }

    // choose best move
    int bestIndex = 0;

    if (player == aiPlayer) {
        int bestScore = -10000;
        for (int i = 0; i < emptyCount; i++) {
            if (moves[i].score > bestScore) {
                bestScore = moves[i].score;
                bestIndex = i;
            }
        }
    } else {
        int bestScore = 10000;
        for (int i = 0; i < emptyCount; i++) {
            if (moves[i].score < bestScore) {
                bestScore = moves[i].score;
                bestIndex = i;
            }
        }
    }

    return moves[bestIndex];
}
static void check_and_open_final_menu(CellTicTacToeStatus board[3][3], AppData *app_data) {
    CellTicTacToeStatus ai = !app_data->tic_tac_toe->isHumanStart ? Circle : Cross;
    CellTicTacToeStatus human = !app_data->tic_tac_toe->isHumanStart ? Cross : Circle;
    if (check_win(board, ai)) {
        showResults(app_data, 0, app_data->tic_tac_toe->grid, 4, 3, 3);
    } else if (check_win(board, human)) {
        showResults(app_data, 1, app_data->tic_tac_toe->grid, 4, 3, 3);
    } else {
        int empty[9][2];
        int count = 0;
        get_empty_cells(board, empty, &count);
        if (count == 0) {
            showResults(app_data, 2, app_data->tic_tac_toe->grid, 4, 3, 3);
        }
    }
}
static gboolean ai_make_move(gpointer user_data) {
    AppData *app_data = user_data;

    CellTicTacToeStatus ai = app_data->tic_tac_toe->isHumanStart ? Circle : Cross;
    CellTicTacToeStatus human = app_data->tic_tac_toe->isHumanStart ? Cross : Circle;

    CellTicTacToeStatus board[3][3];
    copy_board(app_data->tic_tac_toe->cells, board);
    int max_depth = 0;
    switch (app_data->difficult) {
        case 0:
            max_depth = 2;
            break;
        case 1:
            max_depth = 5;
        default:
            max_depth = 100;
    }
    MinimaxResult best = minimax_basic(board, 0, max_depth, ai, ai, human);

    if (best.row != -1) {
        set_tictactoe_cell(app_data, best.row, best.col);
    }

    return G_SOURCE_REMOVE;
}
// Клик игрока
static void on_cell_clicked(GtkButton *button, OnClickData *data) {
    AppData *app_data = data->data;

    if (app_data->tic_tac_toe->isWin || app_data->tic_tac_toe->cells[data->row][data->col].status != Empty) return;

    set_tictactoe_cell(app_data, data->row, data->col);

    g_timeout_add(400, ai_make_move, app_data);
}
static GtkWidget* create_cell(int row, int col, AppData *app_data) {
    GtkWidget *button = gtk_button_new();
    gtk_widget_set_size_request(button, 180, 180);
    gtk_widget_add_css_class(button, "tictactoe-cell");
    gtk_widget_set_cursor_from_name(button, "pointer");

    OnClickData *data = g_new(OnClickData, 1);
    data->data = app_data;
    data->row = row;
    data->col = col;

    g_signal_connect(button, "clicked", G_CALLBACK(on_cell_clicked), data);

    app_data->tic_tac_toe->cells[row][col].button = button;
    app_data->tic_tac_toe->cells[row][col].status = Empty;

    return button;
}
GtkWidget* createTicTacToeScreen(AppData *app_data) {
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    app_data->tic_tac_toe->last_pick = Circle;
    if (rand() % 2) {
        app_data->tic_tac_toe->isHumanStart = 0;
    } else {
        app_data->tic_tac_toe->isHumanStart = 1;
    }
    app_data->tic_tac_toe->isWin = 0;
    if (!app_data->tic_tac_toe->isHumanStart) {
        g_timeout_add(200, ai_make_move, app_data);
    }
    // Заголовок — жирный неон
    GtkWidget *title = gtk_label_new("TIC-TAC-TOE");
    gtk_widget_add_css_class(title, "ttt-title");
    gtk_box_append(GTK_BOX(main_box), title);

    // Инфо-панель
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 60);
    gtk_widget_set_halign(info_box, GTK_ALIGN_CENTER);

    const char *mode_text = "Easy";
    if (app_data->difficult == 1) mode_text = "Medium";
    if (app_data->difficult == 2) mode_text = "Hard";

    GtkWidget *mode_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(mode_label), g_strdup_printf("<span size='20000' weight='bold'>%s</span>", mode_text));
    gtk_widget_add_css_class(mode_label, "ttt-mode");


    gtk_box_append(GTK_BOX(info_box), mode_label);

    gtk_box_append(GTK_BOX(main_box), info_box);

    // ===== ГЛАВНОЕ ПОЛЕ =====
    GtkWidget *field_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(field_container, 400, 400);
    gtk_widget_add_css_class(field_container, "ttt-field");

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    // Инициализация клеток
    app_data->tic_tac_toe->cells = g_new0(CellTicTacToeData*, 3);
    for (int r = 0; r < 3; r++) {
        app_data->tic_tac_toe->cells[r] = g_new0(CellTicTacToeData, 3);
        for (int c = 0; c < 3; c++) {
            GtkWidget *cell = create_cell(r, c, app_data);
            gtk_grid_attach(GTK_GRID(grid), cell, c, r, 1, 1);
        }
    }

    gtk_box_append(GTK_BOX(field_container), grid);
    gtk_box_append(GTK_BOX(main_box), field_container);

    // Кнопка назад
    GtkWidget *back_btn = gtk_button_new_with_label("← Назад");
    gtk_widget_add_css_class(back_btn, "ttt-back-btn");
    add_hover_sound(back_btn, "sounds/MenuSelection.wav", 0, 0, app_data);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(back_to_menu), app_data);
    gtk_box_append(GTK_BOX(main_box), back_btn);

    app_data->tic_tac_toe->grid = grid;

    return main_box;
}