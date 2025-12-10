#include "../app.h"
static void on_button_clicked(GtkButton *button, AppData *data) {
    play_sound("sounds/click.wav", 0, 0, data);
    int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "game-index"));
    switch (index)
        {
        case 0:
            reopen_window(data, "settings");
            break;
        case 1:
            reopen_window(data, "luckyGame");
            break;
        case 2:
            gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "cards");
            break;
        case 3:
            reopen_window(data, "minesweeper");
            break;
        case 4:
            reopen_window(data, "tictactoe");
            break;
        default:
            break;
        }
}
GtkWidget* createMainMenu(AppData *data) {
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(main_box, 1000, 600);
    // Верхняя панель для кнопки настроек (выравнивание по правому краю)
    GtkWidget *top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(top_box, TRUE);

    // Пустое пространство слева
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);

    // Кнопка настроек
    GtkWidget *settings_pic = load_image("settings.svg");
    GtkWidget *settingsButton = gtk_button_new();
    add_hover_sound(settingsButton, "sounds/MenuSelection.wav", 0, 0, data);
    gtk_widget_add_css_class(settings_pic, "settingsPic");
    gtk_button_set_child(GTK_BUTTON(settingsButton), settings_pic);
    g_object_set_data(G_OBJECT(settingsButton), "game-index", GINT_TO_POINTER(0));
    g_signal_connect(settingsButton, "clicked", G_CALLBACK(on_button_clicked), data);
    gtk_widget_set_cursor(settingsButton, gdk_cursor_new_from_name("pointer", NULL));
    gtk_widget_set_size_request(settingsButton, 50, 50);
    gtk_widget_add_css_class(settingsButton, "settingsBtn");

    // Собираем верхнюю панель
    gtk_box_append(GTK_BOX(top_box), spacer);
    gtk_box_append(GTK_BOX(top_box), settingsButton);
    gtk_box_append(GTK_BOX(main_box), top_box);
    

    // Заголовок
    GtkWidget *titleLabel = gtk_label_new("My games");
    gtk_widget_add_css_class(titleLabel, "titleMenu");
    gtk_box_append(GTK_BOX(main_box), titleLabel);

    // Контейнер для кнопок
    GtkWidget *hbox = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(hbox), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(hbox), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(hbox), 40);
    gtk_grid_set_column_spacing(GTK_GRID(hbox), 0);
    gtk_widget_set_halign(hbox, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(hbox, "buttonBox");
    gtk_widget_set_size_request(hbox, 200, 300);
    gtk_box_append(GTK_BOX(main_box), hbox);

    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(left_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(left_box, 100, 300);
    gtk_grid_attach(GTK_GRID(hbox), left_box, 0, 0, 1, 1);


    // Создаём кнопки
    const char *buttons1[] = {"Lucky Game", "Cards", "Minesweeper"};
    for (int i = 0; i < 3; i++) {
        GtkWidget *btn = gtk_button_new_with_label(buttons1[i]);
        add_hover_sound(btn, "sounds/MenuSelection.wav", 0, 0, data);
        gtk_widget_set_size_request(btn, 120, 50); // фиксированная ширина и высота
        gtk_widget_add_css_class(btn, "menuBtn");
        gtk_widget_set_cursor(btn, gdk_cursor_new_from_name("pointer", NULL));
        gtk_widget_set_size_request(btn, 200, 60);
        g_object_set_data(G_OBJECT(btn), "game-index", GINT_TO_POINTER(i+1));
        g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), data);
        gtk_box_append(GTK_BOX(left_box), btn);
    }

    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_grid_attach(GTK_GRID(hbox), right_box, 1, 0, 1, 1);
    gtk_widget_set_halign(right_box, GTK_ALIGN_CENTER);

    // Создаём кнопки
    const char *buttons2[] = {"Tic-tac-toe", "Tetris"};
    for (int i = 0; i < 2; i++) {
        GtkWidget *btn = gtk_button_new_with_label(buttons2[i]);
        add_hover_sound(btn, "sounds/MenuSelection.wav", 0, 0, data);
        gtk_widget_set_size_request(btn, 120, 50); // фиксированная ширина и высота
        gtk_widget_add_css_class(btn, "menuBtn");
        gtk_widget_set_cursor(btn, gdk_cursor_new_from_name("pointer", NULL));
        gtk_widget_set_size_request(btn, 200, 60);
        g_object_set_data(G_OBJECT(btn), "game-index", GINT_TO_POINTER(i+1+3));
        g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), data);
        gtk_box_append(GTK_BOX(right_box), btn);
    }
    // GtkWidget *card = create_flip_card("♠️", "A");  // или картинка, или цвет
    // gtk_box_append(GTK_BOX(right_box), card);

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), footer);
    gtk_widget_add_css_class(main_box, "footer");

    gtk_widget_add_css_class(main_box, "menu");
    return main_box;
}