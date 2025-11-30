#include "../app.h"
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
    play_sound("sounds/click.wav", 0, 0, data);
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
static void on_music_change(GtkRange *range, AppData *data) {
    double val = gtk_range_get_value(range) / 100.0;
    set_music_volume(val, data);
}

static void on_effects_change(GtkRange *range, AppData *data) {
    double val = gtk_range_get_value(range) / 100.0;
    set_effects_volume(val, data);
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



    // Контейнер для кнопок
    // GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    // gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER); // центрируем кнопки
    // gtk_box_append(GTK_BOX(main_box), button_box);
    // gtk_widget_add_css_class(button_box, "buttonBox");
    GtkWidget *hbox = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(hbox), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(hbox), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(hbox), 40);
    gtk_grid_set_column_spacing(GTK_GRID(hbox), 40);
    gtk_box_append(GTK_BOX(main_box), hbox);

    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(left_box, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(hbox), left_box, 0, 0, 1, 1);

    g_autofree char *text = g_strdup_printf("Current difficulty: %s", get_settings_text(data->difficult));
    data->difficulty_label = gtk_label_new(text);
    gtk_widget_add_css_class(data->difficulty_label, "descMenu");
    gtk_box_append(GTK_BOX(left_box), data->difficulty_label);


    // Создаём кнопки
    const char *buttons[] = {"Easy", "Medium", "Hard"};
    for (int i = 0; i < 3; i++) {
        GtkWidget *btn = gtk_button_new_with_label(buttons[i]);
        add_hover_sound(btn, "sounds/MenuSelection.wav", 0, 0, data);
        gtk_widget_set_size_request(btn, 120, 50); // фиксированная ширина и высота
        gtk_widget_add_css_class(btn, "menuBtn");
        gtk_widget_set_cursor(btn, gdk_cursor_new_from_name("pointer", NULL));
        gtk_widget_set_size_request(btn, 200, 60);
        g_object_set_data(G_OBJECT(btn), "button-index", GINT_TO_POINTER(i));
        g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), data);
        gtk_box_append(GTK_BOX(left_box), btn);
    }

    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_grid_attach(GTK_GRID(hbox), right_box, 1, 0, 1, 1);

    GtkWidget *sound_label = gtk_label_new("Audio Settings");
    gtk_widget_add_css_class(sound_label, "descMenu");
    gtk_box_append(GTK_BOX(right_box), sound_label);

    // Ползунок музыки
    GtkWidget *music_label = gtk_label_new("Music Volume");
    gtk_widget_add_css_class(music_label, "scaleLabel");
    gtk_box_append(GTK_BOX(right_box), music_label);

    GtkWidget *music_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    gtk_range_set_value(GTK_RANGE(music_scale), data->music_volume * 100);
    g_signal_connect(music_scale, "value-changed", G_CALLBACK(on_music_change), data);
    gtk_box_append(GTK_BOX(right_box), music_scale);

    // Ползунок эффектов
    GtkWidget *effect_label = gtk_label_new("Effects Volume");
    gtk_widget_add_css_class(effect_label, "scaleLabel");
    gtk_box_append(GTK_BOX(right_box), effect_label);

    GtkWidget *effects_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    gtk_range_set_value(GTK_RANGE(effects_scale), data->effects_volume * 100);
    g_signal_connect(effects_scale, "value-changed", G_CALLBACK(on_effects_change), data);
    gtk_box_append(GTK_BOX(right_box), effects_scale);

    GtkWidget *btn = gtk_button_new_with_label("Back to menu");
    add_hover_sound(btn, "sounds/MenuSelection.wav", 0, 0, data);
    gtk_widget_set_size_request(btn, 120, 50); // фиксированная ширина и высота
    gtk_widget_add_css_class(btn, "menuBtn");
    gtk_widget_set_cursor(btn, gdk_cursor_new_from_name("pointer", NULL));
    gtk_widget_set_size_request(btn, 200, 60);
    g_object_set_data(G_OBJECT(btn), "button-index", GINT_TO_POINTER(3));
    g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), data);
    gtk_box_append(GTK_BOX(main_box), btn);

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), footer);
    gtk_widget_add_css_class(main_box, "footer");

    gtk_widget_add_css_class(main_box, "menu");
    return main_box;
}//
// Created by 1 on 28.11.2025.
//