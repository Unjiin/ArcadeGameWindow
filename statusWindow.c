#include <gtk/gtk.h>
#include "utils.h"
#include "menu.h"
typedef struct {
    GtkWidget* window;
    GtkWidget* container;
    int* difficult; // добавим для доступа к сложности
} CustomData;
static void on_submit_clicked(GtkWidget *widget, gpointer user_data) {
    CustomData *data = (CustomData *)user_data;
    
    openMenu(data->window, data->container, data->difficult);
    // Освобождаем память
    g_free(data);
}
void openStatusWindow(GtkWidget *window, GtkWidget *container, int* difficult, int status) {
    GdkDisplay *display = gdk_display_get_default();
    GdkCursor *cursor = gdk_cursor_new_from_name(display, "pointer");
    
    // Очищаем контейнер
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    // Контейнер для кнопок
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), button_box, FALSE, FALSE, 0);
    // win text
    gchar *label_text = g_strdup_printf(
        "<span size='xx-large' weight='bold'>%s</span>", 
        status ? "Congradulations!!!\n     You Win!\n       WWW" : "You lose"
    );
    GtkWidget *label_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_title), label_text);
    g_free(label_text);
    gtk_label_set_use_markup(GTK_LABEL(label_title), TRUE);
    gtk_widget_set_halign(label_title, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(button_box), label_title, FALSE, FALSE, 0);

    // Кнопка Submit
    GtkWidget *btn = gtk_button_new_with_label("Back to menu");
    gtk_widget_set_size_request(btn, 120, 50);
    
    // Данные для кнопки
    CustomData *data = g_new0(CustomData, 1);
    data->window = window;
    data->container = container;
    data->difficult = difficult; // сохраняем указатель на сложность

    // Сигналы
    g_signal_connect(btn, "enter-notify-event", G_CALLBACK(on_button_enter), cursor);
    g_signal_connect(btn, "leave-notify-event", G_CALLBACK(on_button_leave), NULL);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_submit_clicked), data);
    
    gtk_box_pack_start(GTK_BOX(button_box), btn, FALSE, FALSE, 0);

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_pack_end(GTK_BOX(container), footer, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}