#include <gtk/gtk.h>
#include "app.h"
GtkWidget* load_image(const char *filename) {
    gchar *path = g_build_filename("assets", filename, NULL);

    GtkWidget *widget = gtk_picture_new_for_filename(path);
    g_free(path);

    // Если файл не найден или виджет не создался — fallback
    if (widget == NULL || gtk_picture_get_file(GTK_PICTURE(widget)) == NULL) {
        g_warning("Не найден assets/%s — ставим заглушку", filename);
        GtkWidget *fallback = gtk_label_new("?");
        gtk_widget_add_css_class(fallback, "image-fallback");
        return fallback;
    }

    GtkPicture *pic = GTK_PICTURE(widget);

    // Идеальное масштабирование под любой размер (всегда работает)
    gtk_picture_set_content_fit(pic, GTK_CONTENT_FIT_SCALE_DOWN);  // вписывает с сохранением пропорций
    gtk_picture_set_can_shrink(pic, TRUE);   // разрешает уменьшать (важно под Windows)
    // gtk_picture_set_can_grow(pic, TRUE);     // разрешает увеличивать (на всякий случай)

    return widget;
}
void reopen_window(AppData *app_data, const char *str) {

    GtkWidget *old_screen = gtk_stack_get_child_by_name(GTK_STACK(app_data->stack), str);

    if (old_screen) {
        gtk_stack_remove(GTK_STACK(app_data->stack), old_screen);

        if (strcmp(str, "minesweeper") == 0) {
            cleanup_minesweeper_game(app_data);
        }
    }

    // Создаём свежий экран
    GtkWidget *new_screen = NULL;

    if (strcmp(str, "settings") == 0) {
        new_screen = createSettingsScreen(app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    } else if (strcmp(str, "minesweeper") == 0) {
        new_screen = createMinesweeperScreen(app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    } else if (strcmp(str, "menu") == 0) {
        new_screen = createMainMenu(app_data);
        gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_RIGHT);
    }

    if (new_screen) {
        gtk_stack_add_named(GTK_STACK(app_data->stack), new_screen, str);
        gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), str);
    }
}