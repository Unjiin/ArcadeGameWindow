#include "../app.h"
#include <stdlib.h>

// Callback для input (enter или кнопка)
static void entry_callback(GtkEntry *entry, AppData *app_data) {
    const char *text;
    g_object_get(entry, "text", &text, NULL);
    g_free((gchar*)text);
    int guess = atoi(text);

    if (guess < 1 || guess > app_data->lucky_game->max_num) {
        gtk_editable_set_text(GTK_EDITABLE(entry), "Invalid!");  // фидбек
        return;
    }

    app_data->lucky_game->attempts_left--;

    if (guess == app_data->lucky_game->secret_number) {
        showResults(app_data, 1, app_data->lucky_game->grid, 5, 1, 2);  // win
        app_data->lucky_game->is_won = 1;
        gtk_widget_set_sensitive(GTK_WIDGET(entry), FALSE);
    } else if (app_data->lucky_game->attempts_left <= 0) {
        showResults(app_data, 0, app_data->lucky_game->grid, 5, 1, 2);  // lose
        gtk_widget_set_sensitive(GTK_WIDGET(entry), FALSE);
    } else {
        gtk_editable_set_text(GTK_EDITABLE(entry), g_strdup_printf("Wrong! %d left", app_data->lucky_game->attempts_left));
    }
}

GtkWidget* createLuckyGameScreen(AppData *app_data) {
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(main_box, 400, 400);

    // Заголовок
    GtkWidget *title = gtk_label_new("LUCKY GAME");
    gtk_widget_add_css_class(title, "lucky-title");
    gtk_box_append(GTK_BOX(main_box), title);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    // Текст с уровнем
    int max_num = (app_data->difficult == 0) ? 10 : (app_data->difficult == 1) ? 20 : 50;
    GtkWidget *prompt = gtk_label_new(g_strdup_printf("Choose number from 1 to %d", max_num));
    gtk_widget_add_css_class(prompt, "lucky-prompt");
    gtk_grid_attach(GTK_GRID(grid), prompt, 0, 0, 1, 1);

    // Input
    GtkWidget *entry = gtk_entry_new();
    gtk_widget_set_size_request(entry, 200, 50);
    gtk_widget_add_css_class(entry, "lucky-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter number...");
    g_signal_connect(entry, "activate", G_CALLBACK(entry_callback), app_data);  // enter
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 1, 1, 1);

    gtk_box_append(GTK_BOX(main_box), grid);

    // Инициализация данных
    app_data->lucky_game = g_new0(LuckyGameData, 1);
    app_data->lucky_game->grid = grid;
    app_data->lucky_game->entry = entry;
    app_data->lucky_game->max_num = max_num;
    app_data->lucky_game->secret_number = (rand() % max_num) + 1;
    app_data->lucky_game->attempts_left = 3;
    app_data->lucky_game->is_won = 0;

    printf("Number: %d\n", app_data->lucky_game->secret_number);fflush(stdout);

    // Кнопка назад
    GtkWidget *back_btn = gtk_button_new_with_label("← Назад");
    gtk_widget_add_css_class(back_btn, "lucky-back-btn");
    add_hover_sound(back_btn, "sounds/MenuSelection.wav", 0, 0, app_data);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(back_to_menu), app_data);
    gtk_box_append(GTK_BOX(main_box), back_btn);

    return main_box;
}