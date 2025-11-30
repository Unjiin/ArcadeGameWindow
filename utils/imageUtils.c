#include "../app.h"
GtkWidget* load_image(const char *filename) {
    gchar *exe_dir = get_exe_directory();
    if (!exe_dir) {
        g_warning("Не удалось получить путь к exe");
        return gtk_label_new("X");
    }

    gchar *full_path = g_build_filename(exe_dir, "assets", filename, NULL);
    g_free(exe_dir);


    GtkWidget *widget = gtk_picture_new_for_filename(full_path);

    if (widget == NULL || gtk_picture_get_file(GTK_PICTURE(widget)) == NULL) {
        g_warning("Не найден или не загружен %s", full_path);
        g_free(full_path);
        GtkWidget *fallback = gtk_label_new("?");
        gtk_widget_add_css_class(fallback, "image-fallback");
        return fallback;
    }

    g_free(full_path);

    GtkPicture *pic = GTK_PICTURE(widget);
    gtk_picture_set_content_fit(pic, GTK_CONTENT_FIT_SCALE_DOWN);
    gtk_picture_set_can_shrink(pic, TRUE);

    return widget;
}

/* Структура для хранения состояния анимации, привязанного к GtkImage */
typedef struct {
    GdkPixbufAnimation *anim;
    GdkPixbufAnimationIter *iter;
    GdkTexture *current_texture;
    guint timeout_id;

    gint64 end_time_us;   // точное время окончания в микросекундах
    gboolean finished;
} GifAnim;


static void gif_anim_free_data(gpointer data)
{
    GifAnim *ga = (GifAnim*)data;
    if (!ga) return;

    if (ga->timeout_id)
        g_source_remove(ga->timeout_id);

    g_clear_object(&ga->current_texture);
    g_clear_object(&ga->iter);
    g_clear_object(&ga->anim);

    g_free(ga);
}


static gboolean gif_frame_timeout(gpointer data)
{
    GtkPicture *pic = GTK_PICTURE(data);
    GifAnim *ga = g_object_get_data(G_OBJECT(pic), "gif-anim");
    if (!ga || ga->finished) {
        if (ga) {
            ga->timeout_id = 0; // Обнуляем ID при завершении
        }
        return G_SOURCE_REMOVE;
    }


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    GTimeVal tv;
    g_get_current_time(&tv);
#pragma GCC diagnostic pop

    /* переход к следующему кадру */
    gdk_pixbuf_animation_iter_advance(ga->iter, &tv);

    /* отображаем кадр */
    GdkPixbuf *pix = gdk_pixbuf_animation_iter_get_pixbuf(ga->iter);
    if (pix) {
        GdkTexture *tex = gdk_texture_new_for_pixbuf(pix);
        gtk_picture_set_paintable(pic, GDK_PAINTABLE(tex));
        g_clear_object(&ga->current_texture);
        ga->current_texture = tex;
        gtk_widget_queue_draw(GTK_WIDGET(pic));
    }

    /* останов по времени */
    gint64 now = g_get_real_time();
    if (now >= ga->end_time_us) {
        ga->finished = TRUE;
        ga->timeout_id = 0; // Обнуляем ID
        return G_SOURCE_REMOVE;
    }

    /* планируем следующий кадр */
    gint delay = gdk_pixbuf_animation_iter_get_delay_time(ga->iter);
    if (delay < 10) delay = 100; // минимальная задержка
    ga->timeout_id = 0;
    ga->timeout_id = g_timeout_add(delay, gif_frame_timeout, pic);
    return G_SOURCE_REMOVE;
}


/*
 * load_gif_animation("assets/explosion.gif", 1300)
 * → будет играть 1300 мс и автоматически остановится.
 */
GtkWidget *load_gif_animation_ms(const char *filename, int duration_ms)
{
    GError *error = NULL;
    GdkPixbufAnimation *anim = gdk_pixbuf_animation_new_from_file(filename, &error);
    if (!anim) {
        g_warning("GIF load failed: %s", error->message);
        g_clear_error(&error);
        return NULL;
    }

    GdkPixbufAnimationIter *iter = gdk_pixbuf_animation_get_iter(anim, NULL);

    GtkWidget *picture = gtk_picture_new();
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), FALSE);
    gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_COVER);

    GifAnim *ga = g_new0(GifAnim, 1);
    ga->anim = anim;
    ga->iter = iter;
    ga->finished = FALSE;

    gint64 now = g_get_real_time();
    ga->end_time_us = now + ((gint64)duration_ms * 1000); // перевод мс → мкс

    g_object_set_data_full(G_OBJECT(picture), "gif-anim", ga, gif_anim_free_data);

    /* показать первый кадр */
    GdkPixbuf *first = gdk_pixbuf_animation_iter_get_pixbuf(iter);
    if (first) {
        GdkTexture *tex = gdk_texture_new_for_pixbuf(first);
        gtk_picture_set_paintable(GTK_PICTURE(picture), GDK_PAINTABLE(tex));
        ga->current_texture = tex;
    }

    /* запустить таймер */
    gint delay = gdk_pixbuf_animation_iter_get_delay_time(iter);
    if (delay < 10) delay = 100;
    ga->timeout_id = g_timeout_add(delay, gif_frame_timeout, picture);

    return picture;
}