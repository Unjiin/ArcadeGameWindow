#include "app.h"
static void on_button_clicked(GtkButton *button, AppData *data) {
    play_sound("sounds/click.wav", 0, 0, data);
    int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "game-index"));
    switch (index)
        {
        case 0:
            reopen_window(data, "settings");
            break;
        case 1:
            gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "luckyGame");
            break;
        case 2:
            gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "cards");
            break;
        case 3:
            reopen_window(data, "minesweeper");
            break;
        default:
            break;
        }
}
// typedef struct {
//     GtkWidget *front;
//     GtkWidget *back;
//     gboolean is_flipped;
// } CardData;
// void flip_card_callback(GtkButton *btn, CardData *d) {
//     if (d->is_flipped) {
//         // Возврат к фронту
//         gtk_widget_set_transform(d->front, gtk_transform_scale(gtk_transform_new(), 0.0, 1.0));
//         gtk_widget_set_opacity(d->front, 1.0);
//         g_timeout_add(50, (GSourceFunc)(void(*)(CardData*))[](CardData *d)->gboolean {
//             static float step = 0.0;
//             step += 0.15;
//             if (step >= 1.0) {
//                 step = 0.0;
//                 gtk_widget_set_transform(d->front, NULL);
//                 gtk_widget_set_opacity(d->back, 0.0);
//                 gtk_widget_set_transform(d->back, gtk_transform_scale(gtk_transform_new(), 0.0, 1.0));
//                 d->is_flipped = FALSE;
//                 return FALSE;
//             }
//             gtk_widget_set_transform(d->front, gtk_transform_scale(gtk_transform_new(), 1.0 - step, 1.0));
//             return TRUE;
//         }, d);
//     } else {
//         // Переворот к бэку
//         g_timeout_add(50, (GSourceFunc)(void(*)(CardData*))[](CardData *d)->gboolean {
//             static float step = 0.0;
//             step += 0.15;
//             if (step >= 1.0) {
//                 step = 0.0;
//                 gtk_widget_set_transform(d->back, NULL);
//                 gtk_widget_set_opacity(d->back, 1.0);
//                 gtk_widget_set_transform(d->front, gtk_transform_scale(gtk_transform_new(), 0.0, 1.0));
//                 d->is_flipped = TRUE;
//                 return FALSE;
//             }
//             gtk_widget_set_transform(d->front, gtk_transform_scale(gtk_transform_new(), 1.0 - step, 1.0));
//             gtk_widget_set_transform(d->back, gtk_transform_scale(gtk_transform_new(), step, 1.0));
//             gtk_widget_set_opacity(d->back, step);
//             return TRUE;
//         }, d);
//     }
// }
// GtkWidget* create_flip_card(const char *front_text, const char *back_color) {
//     // Главный контейнер
//     GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     gtk_widget_set_size_request(container, 200, 300);
//
//     // Кнопка, по которой кликаем
//     GtkWidget *button = gtk_button_new();
//     gtk_button_set_has_frame(GTK_BUTTON(button), FALSE);
//     gtk_widget_set_cursor_from_name(button, "pointer");
//
//     // Overlay — чтобы front и back лежали друг на друге
//     GtkWidget *overlay = gtk_overlay_new();
//     gtk_button_set_child(GTK_BUTTON(button), overlay);
//
//     // FRONT
//     GtkWidget *front = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     gtk_widget_add_css_class(front, "card-front");
//     gtk_widget_set_size_request(front, 200, 300);
//     GtkWidget *front_label = gtk_label_new(front_text);
//     gtk_widget_add_css_class(front_label, "card-label");
//     gtk_box_append(GTK_BOX(front), front_label);
//
//     // BACK
//     GtkWidget *back = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     gtk_widget_add_css_class(back, "card-back");
//     gtk_widget_set_size_request(back, 200, 300);
//     GtkWidget *back_label = gtk_label_new("❤️");
//     gtk_widget_add_css_class(back_label, "card-label");
//     gtk_box_append(GTK_BOX(back), back_label);
//
//     // Изначально back скрыт и сжат
//     gtk_widget_set_opacity(back, 0.0);
//     gtk_widget_set_transform(back, gtk_transform_scale(gtk_transform_new(), 0.0, 1.0));
//
//     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), front);
//     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), back);
//
//     // Данные для анимации
//
//     CardData *data = g_new(CardData, 1);
//     data->front = front;
//     data->back = back;
//     data->is_flipped = FALSE;
//
//     // Анимация переворота
//     g_signal_connect(button, "clicked", G_CALLBACK(flip_card_callback), data);
//
//     gtk_box_append(GTK_BOX(container), button);
//     return container;
// }
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
        g_object_set_data(G_OBJECT(btn), "game-index", GINT_TO_POINTER(i+1));
        g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), data);
        gtk_box_append(GTK_BOX(right_box), btn);
    }
    GtkWidget *card = create_flip_card("♠️", "A");  // или картинка, или цвет
    gtk_box_append(GTK_BOX(right_box), card);

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), footer);
    gtk_widget_add_css_class(main_box, "footer");

    gtk_widget_add_css_class(main_box, "menu");
    return main_box;
}