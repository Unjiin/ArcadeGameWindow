#include "../app.h"
#include <gst/gst.h>
typedef struct {
    GstElement *player;
    gboolean busy;
    gboolean loop;
    int sound_type; // 0 = effect, 1 = music
    char *loop_uri; // если loop == TRUE, храним путь
} SoundChannel;
typedef struct {
    char *path;
    int type;
    AppData *data;
    gboolean loop;
} HoverSoundArgs;
#define SOUND_POOL_SIZE 15

static SoundChannel sound_pool[SOUND_POOL_SIZE];
static gboolean sound_busy[SOUND_POOL_SIZE];

void set_music_volume(double vol, AppData *app_data) {
    app_data->music_volume = vol;

    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        if (sound_pool[i].busy && sound_pool[i].sound_type == 1) {
            g_object_set(sound_pool[i].player, "volume", vol, NULL);
        }
    }
}

void set_effects_volume(double vol, AppData *app_data) {
    app_data->effects_volume = vol;

    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        if (sound_pool[i].busy && sound_pool[i].sound_type == 0) {
            g_object_set(sound_pool[i].player, "volume", vol, NULL);
        }
    }
}

static gboolean sound_bus_call(GstBus *bus, GstMessage *msg, gpointer user_data) {
    int id = GPOINTER_TO_INT(user_data);
    SoundChannel *ch = &sound_pool[id];

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            if (ch->loop && ch->loop_uri) {
                // Перезапуск
                gst_element_set_state(ch->player, GST_STATE_NULL);
                g_object_set(ch->player, "uri", ch->loop_uri, NULL);
                gst_element_set_state(ch->player, GST_STATE_PLAYING);
            } else {
                // Освобождаем канал
                gst_element_set_state(ch->player, GST_STATE_READY);
                ch->busy = FALSE;
            }
            break;

        case GST_MESSAGE_ERROR:
            gst_element_set_state(ch->player, GST_STATE_READY);
            ch->busy = FALSE;
            break;
    }

    return TRUE;
}

void init_sound_pool() {
    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        sound_pool[i].player = gst_element_factory_make("playbin", NULL);
        sound_pool[i].busy = FALSE;
        sound_pool[i].loop = FALSE;
        sound_pool[i].loop_uri = NULL;

        gst_element_set_state(sound_pool[i].player, GST_STATE_READY);

        GstBus *bus = gst_element_get_bus(sound_pool[i].player);
        gst_bus_add_watch(bus, sound_bus_call, GINT_TO_POINTER(i));
        gst_object_unref(bus);
    }
}

// Вызывай эту функцию ВЕЗДЕ вместо старой play_sound
void play_sound(const char *relative_path, int type, gboolean loop, AppData *data) {
    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        SoundChannel *ch = &sound_pool[i];

        if (ch->loop && ch->loop_uri && g_str_has_suffix(ch->loop_uri, relative_path)) {
            return;
        }
    }
    gchar *full_path = g_build_filename(g_get_current_dir(), relative_path, NULL);
    gchar *uri = g_filename_to_uri(full_path, NULL, NULL);
    g_free(full_path);

    if (!uri) return;

    // Ищем свободный канал
    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        SoundChannel *ch = &sound_pool[i];

        if (!ch->busy) {
            ch->busy = TRUE;
            ch->loop = loop;
            ch->sound_type = type;

            // Если loop → сохраняем путь
            if (loop) {
                ch->loop_uri = g_strdup(uri);
            } else {
                g_free(ch->loop_uri);
                ch->loop_uri = NULL;
            }

            g_object_set(ch->player, "uri", uri, NULL);
            double val = type == 0 ? data->effects_volume : data->music_volume;
            g_object_set(ch->player, "volume", val, NULL);

            gst_element_set_state(ch->player, GST_STATE_PLAYING);

            g_free(uri);
            return;
        }
    }

    printf("⚠ Все звуковые каналы заняты (%d)\n", SOUND_POOL_SIZE);
    g_free(uri);
}
void stop_loop_sound(const char *relative_path) {
    for (int i = 0; i < SOUND_POOL_SIZE; i++) {
        SoundChannel *ch = &sound_pool[i];

        if (ch->loop && ch->loop_uri && g_str_has_suffix(ch->loop_uri, relative_path)) {
            // Останавливаем
            gst_element_set_state(ch->player, GST_STATE_READY);
            ch->busy = FALSE;
            ch->loop = FALSE;

            g_free(ch->loop_uri);
            ch->loop_uri = NULL;
        }
    }
}

static void hover_sound_callback(HoverSoundArgs *args) {
    play_sound(args->path, args->type, args->loop, args->data);
}
void add_hover_sound(GtkWidget *button, const char *sound_file, int type, gboolean loop, AppData *data) {
    GtkEventController *motion = gtk_event_controller_motion_new();

    HoverSoundArgs *args = g_malloc(sizeof(HoverSoundArgs));
    args->path = g_strdup(sound_file);
    args->type = type;
    args->loop = loop;
    args->data = data;

    g_signal_connect_swapped(
        motion,
        "enter",
        G_CALLBACK(hover_sound_callback),
        args
    );

    gtk_widget_add_controller(button, motion);
}