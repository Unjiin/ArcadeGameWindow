#include <gtk/gtk.h>
#include "utils.h"
#include "menu.h"
#include "games.h"
#include "statusWindow.h"
#include <stdlib.h>

typedef struct {
    GtkWidget* window;
    GtkWidget* container;
    GtkWidget* entry;
    int* difficult; // добавим для доступа к сложности
    int computerNumber;
} CustomData;

typedef struct {
    GtkWidget* window;
    GtkWidget* container;
    GtkWidget* entry;
    int* difficult; // добавим для доступа к сложности
    int cardNumber;
} CardsData;

static void onLuckyGameClicked(GtkWidget *widget, gpointer user_data) {
    CustomData *data = (CustomData *)user_data;
    
    // Получаем текст из поля ввода
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(data->entry));
    
    if (text[0] != '\0') {
        int number = atoi(text);
        int max_number = difficultyValues[*(data->difficult)];
        
        g_print("Введено число: %d (максимум: %d)\n", number, max_number);
        
        // Проверяем валидность
        if (number >= 0 && number <= max_number) {
            g_print("Число валидно! Запускаем игру...\n");
            if (number == data->computerNumber) openStatusWindow(data->window, data->container, data->difficult, 1);
            else openStatusWindow(data->window, data->container, data->difficult, 0);
        } else {
            g_print("Ошибка: число должно быть от 0 до %d!\n", max_number);
        }
    } else {
        g_print("Поле ввода пустое!\n");
    }
    
    // Освобождаем память
    g_free(data);
}

void openLuckyGame(GtkWidget *window, GtkWidget *container, int* difficult) {
    GdkDisplay *display = gdk_display_get_default();
    GdkCursor *cursor = gdk_cursor_new_from_name(display, "pointer");
    
    // Очищаем контейнер
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Первый заголовок
    GtkWidget *label_title1 = gtk_label_new("<span size='xx-large' weight='bold'>Lucky Game</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_title1), TRUE);
    gtk_widget_set_halign(label_title1, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), label_title1, FALSE, FALSE, 0);

    // Второй заголовок (с числом)
    gchar *label_text2 = g_strdup_printf(
        "<span size='large' weight='bold'>Write number from 0 to %d</span>",
        difficultyValues[*difficult]
    );
    GtkWidget *label_title2 = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_title2), label_text2);
    g_free(label_text2);
    gtk_widget_set_halign(label_title2, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), label_title2, FALSE, FALSE, 0);

    // Поле ввода
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_size_request(input_box, 50, 50);
    gtk_widget_set_halign(input_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), input_box, FALSE, FALSE, 0);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Введите число...");
    gtk_widget_set_size_request(entry, 120, 50);
    gtk_box_pack_start(GTK_BOX(input_box), entry, FALSE, FALSE, 0);

    // Контейнер для кнопок
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), button_box, TRUE, TRUE, 0);

    // Кнопка Submit
    GtkWidget *btn = gtk_button_new_with_label("Submit");
    gtk_widget_set_size_request(btn, 120, 50);
    
    // Данные для кнопки
    CustomData *data = g_new0(CustomData, 1);
    data->window = window;
    data->entry = entry;
    data->container = container;
    data->difficult = difficult; // сохраняем указатель на сложность
    data->computerNumber = 1 + rand() % difficultyValues[*difficult];

    g_print("%d", data->computerNumber);
    // Сигналы
    g_signal_connect(btn, "enter-notify-event", G_CALLBACK(on_button_enter), cursor);
    g_signal_connect(btn, "leave-notify-event", G_CALLBACK(on_button_leave), NULL);
    g_signal_connect(btn, "clicked", G_CALLBACK(onLuckyGameClicked), data);
    
    gtk_box_pack_start(GTK_BOX(button_box), btn, FALSE, FALSE, 0);

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_pack_end(GTK_BOX(container), footer, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}
static void onCardClicked(GtkWidget *widget, gpointer user_data) {
    CardsData *data = (CustomData *)user_data;

}
void graphicsCardsGame(GtkWidget *window, GtkWidget *container, int* difficult) {
GdkDisplay *display = gdk_display_get_default();
    GdkCursor *cursor = gdk_cursor_new_from_name(display, "pointer");
    
    // Очищаем контейнер
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Первый заголовок
    GtkWidget *label_title1 = gtk_label_new("<span size='xx-large' weight='bold'>Find Pairs</span>");
    gtk_label_set_use_markup(GTK_LABEL(label_title1), TRUE);
    gtk_widget_set_halign(label_title1, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), label_title1, FALSE, FALSE, 0);

    // Второй заголовок (с числом)
    gchar *label_text2 = g_strdup_printf(
        "<span size='large' weight='bold'>Health %d</span>",
        5
    );
    GtkWidget *label_title2 = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_title2), label_text2);
    g_free(label_text2);
    gtk_widget_set_halign(label_title2, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(container), label_title2, FALSE, FALSE, 0);

    // Контейнеры для кнопок
    for (int i = 0; i < 20; i += 5) {
        GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
        gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(container), button_box, FALSE, TRUE, 0);
        for (int j = i; j < i + 5; j++) {
            GtkWidget *btn = gtk_button_new();
            gtk_widget_set_size_request(btn, 80, 120);

            // Данные для кнопки
            CustomData *data = g_new0(CustomData, 1);
            data->window = window;
            data->container = container;
            // Сигналы
            g_signal_connect(btn, "enter-notify-event", G_CALLBACK(on_button_enter), cursor);
            g_signal_connect(btn, "leave-notify-event", G_CALLBACK(on_button_leave), NULL);
            g_signal_connect(btn, "clicked", G_CALLBACK(onLuckyGameClicked), data);

            gtk_box_pack_start(GTK_BOX(button_box), btn, FALSE, FALSE, 0);
        }
    }

    // Футер
    GtkWidget *footer = gtk_label_new("Created by Unjin");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_box_pack_end(GTK_BOX(container), footer, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}
void openCardsGame(GtkWidget *window, GtkWidget *container, int* difficult) {
    int health = 5;
    int length = 5*(*difficult + 1);
    int countCards = 0;
    int* cards = (int*)malloc(sizeof(int) * (length*2));
    int* cardsStatus = (int*)calloc((length*2), sizeof(int));
    for (int i = 0; i < length; i++) {
        cards[i] = i + 1;
        cards[i + length] = i + 1;
    }
    for (int i = length * 2 - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }

    

    free(cards);
    free(cardsStatus);
}