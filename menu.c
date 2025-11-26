#include <gtk/gtk.h>
#include "utils.h"
#include "menu.h"
#include "settings.h"
#include "games.h"
typedef enum {
    Settings,
    LuckyGame,
    Cards,
    MineSweeper
} Buttons;
typedef struct {
    Buttons index;
    GtkWidget* window;
    GtkWidget* container;
    int* difficult;
} CustomData;
static void on_button_clicked(GtkWidget *widget, gpointer user_data) {
    CustomData *data = (CustomData *)user_data;
    switch (data->index)
    {
    case Settings:
        openSettings(data->window, data->container, data->difficult);
        break;
    case LuckyGame:
        openLuckyGame(data->window, data->container, data->difficult);
        break;
    case Cards:
        openCardsGame(data->window, data->container, data->difficult);
        break;
    default:
        break;
    }
    g_free(data);
    // if (data == "Settings") openSettings()
    // g_print("%s clicked!\n", (char*)data);
}
void openMenu(GtkWidget *window, GtkWidget *container, int* difficult) {
    GdkDisplay *display = gdk_display_get_default();
    GdkCursor *cursor = gdk_cursor_new_from_name(display, "pointer");
    // Основной вертикальный контейнер
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Настройки
    GtkWidget *settingsBtn = gtk_button_new_with_label("⚙️");
    gtk_widget_set_size_request(settingsBtn, 20, 20); // фиксированная ширина и высота
    g_signal_connect(settingsBtn, "enter-notify-event", G_CALLBACK(on_button_enter), cursor);
    g_signal_connect(settingsBtn, "leave-notify-event", G_CALLBACK(on_button_leave), window);
    CustomData *settingsData = g_new0(CustomData, 1);
    settingsData->window = window;
    settingsData->difficult = difficult;
    settingsData->index = Settings;
    settingsData->container = container;
    g_signal_connect(settingsBtn, "clicked", G_CALLBACK(on_button_clicked), settingsData);
    
    // Контейнер для настроек
    GtkWidget *settings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_size_request(settings_box, 20, 20);
    gtk_widget_set_halign(settings_box, GTK_ALIGN_END); // центрируем кнопки
    gtk_box_pack_start(GTK_BOX(settings_box), settingsBtn, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(container), settings_box, FALSE, TRUE, 0);

    // Заголовок
    GtkWidget *label_title = gtk_label_new("<span size='xx-large' weight='bold'>My Games</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_title), TRUE);
    gtk_widget_set_halign(label_title, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), label_title, FALSE, FALSE, 0);

    // Контейнер для кнопок
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER); // центрируем кнопки
    gtk_box_pack_start(GTK_BOX(container), button_box, TRUE, TRUE, 0);

    // Создаём кнопки
    const char *buttons[] = {"Lucky Game", "Cards", "Minesweeper"};
    for (int i = 0; i < 3; i++) {
        GtkWidget *btn = gtk_button_new_with_label(buttons[i]);
        gtk_widget_set_size_request(btn, 120, 50); // фиксированная ширина и высота

        // hover - меняем курсор
        g_signal_connect(btn, "enter-notify-event", G_CALLBACK(on_button_enter), cursor);
        g_signal_connect(btn, "leave-notify-event", G_CALLBACK(on_button_leave), NULL);
        CustomData *data = g_new0(CustomData, 1);
        data->window = window;
        data->difficult = difficult;
        data->container = container;
        switch (i)
        {
        case 0:
            data->index = LuckyGame;
            break;
        case 1:
            data->index = Cards;
            break;
        case 2:
            data->index = MineSweeper;
            break;
        
        default:
            break;
        }
        g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), data);
        gtk_box_pack_start(GTK_BOX(button_box), btn, FALSE, FALSE, 0);
    }

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_pack_end(GTK_BOX(container), footer, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}