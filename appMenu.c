#include "app.h"
typedef struct {
    AppData *data;
    int index;
} MenuBtns;
typedef enum {
    Settings,
    LuckyGame,
    Cards,
    MineSweeper
} Buttons;
static void on_button_clicked(GtkButton *button, MenuBtns *data) {
    switch (data->index)
        {
        case 0:
            gtk_stack_set_visible_child_name(GTK_STACK(data->data->stack), "settings");
            break;
        case 1:
            gtk_stack_set_visible_child_name(GTK_STACK(data->data->stack), "luckyGame");
            break;
        case 2:
            gtk_stack_set_visible_child_name(GTK_STACK(data->data->stack), "cards");
            break;
        case 3:
            gtk_stack_set_visible_child_name(GTK_STACK(data->data->stack), "minesweeper");
            break;
        default:
            break;
        }
    g_free(data);
}
GtkWidget* createMainMenu(AppData *data) {
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    MenuBtns *btn = g_malloc0(sizeof(MenuBtns));
    btn->data = data;
    btn->index = Settings;
    GtkWidget *settingsButton = gtk_button_new_with_label("⚙️");
    g_signal_connect(settingsButton, "clicked", G_CALLBACK(on_button_clicked), btn);
    gtk_style_context_add_class(settingsButton, "settingsBtn");
    gtk_container_add(GTK_BOX(main_box), settingsButton);
    

    // Заголовок
    GtkWidget *titleLabel = gtk_label_new("My games");
    gtk_widget_add_css_class(titleLabel, "title");
    gtk_box_append(GTK_BOX(main_box), titleLabel);

    // Контейнер для кнопок
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER); // центрируем кнопки
    gtk_box_append(GTK_BOX(main_box), button_box);
    gtk_widget_add_css_class(button_box, "buttonBox");


    // Создаём кнопки
    const char *buttons[] = {"Lucky Game", "Cards", "Minesweeper"};
    for (int i = 0; i < 3; i++) {
        GtkWidget *btn = gtk_button_new_with_label(buttons[i]);
        gtk_widget_set_size_request(btn, 120, 50); // фиксированная ширина и высота
        gtk_widget_add_css_class(button_box, "menuBtn");
        MenuBtns *btnData = g_new0(MenuBtns, 1);
        btnData->data = data;
        switch (i)
        {
        case 0:
            btnData->index = LuckyGame;
            break;
        case 1:
            btnData->index = Cards;
            break;
        case 2:
            btnData->index = MineSweeper;
            break;
        
        default:
            break;
        }
        g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), btnData);
        gtk_box_pack_start(GTK_BOX(button_box), btn, FALSE, FALSE, 0);
    }

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), footer);
    gtk_widget_add_css_class(button_box, "footer");

    gtk_widget_add_css_class(main_box, "menu");
    return main_box;
}