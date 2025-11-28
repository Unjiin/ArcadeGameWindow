#include "app.h"
typedef enum {
    Easy,
    Medium,
    Hard,
    Back
} Buttons;
static const char* get_settings_text(int difficult) {
    switch (difficult) {
        case 0: return "Easy";
        case 1: return "Medium";
        case 2: return "Hard";
        default: return "Abracadabra";
    }
}

static void on_button_clicked(GtkButton *button, AppData *data) {
    int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "button-index"));

    if (index >= 0 && index <= 2) {
        data->difficult = index;

        g_autofree char *text = g_strdup_printf("Current difficulty: %s", get_settings_text(data->difficult));
        gtk_label_set_text(GTK_LABEL(data->difficulty_label), text);
        return;
    }

    if (index == 3) {
        reopen_window(data, "menu");
    }
}
GtkWidget* createSettingsScreen(AppData *data) {
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(main_box, 1000, 600);

    // Заголовок
    GtkWidget *titleLabel = gtk_label_new("Settings");
    gtk_widget_add_css_class(titleLabel, "titleMenu");
    gtk_box_append(GTK_BOX(main_box), titleLabel);

    // Описание
    g_autofree char *text = g_strdup_printf("Current difficulty: %s", get_settings_text(data->difficult));
    data->difficulty_label = gtk_label_new(text);
    gtk_widget_add_css_class(data->difficulty_label, "descMenu");
    gtk_box_append(GTK_BOX(main_box), data->difficulty_label);

    // Контейнер для кнопок
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER); // центрируем кнопки
    gtk_box_append(GTK_BOX(main_box), button_box);
    gtk_widget_add_css_class(button_box, "buttonBox");


    // Создаём кнопки
    const char *buttons[] = {"Easy", "Medium", "Hard", "Back"};
    for (int i = 0; i < 4; i++) {
        GtkWidget *btn = gtk_button_new_with_label(buttons[i]);
        gtk_widget_set_size_request(btn, 120, 50); // фиксированная ширина и высота
        gtk_widget_add_css_class(btn, "menuBtn");
        gtk_widget_set_cursor(btn, gdk_cursor_new_from_name("pointer", NULL));
        gtk_widget_set_size_request(btn, 200, 60);
        g_object_set_data(G_OBJECT(btn), "button-index", GINT_TO_POINTER(i));
        g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), data);
        gtk_box_append(GTK_BOX(button_box), btn);
    }

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), footer);
    gtk_widget_add_css_class(button_box, "footer");

    gtk_widget_add_css_class(main_box, "menu");
    return main_box;
}//
// Created by 1 on 28.11.2025.
//