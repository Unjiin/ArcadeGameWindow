#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *stack;


    int *difficult;
} AppData;
GtkWidget* createMainMenu(AppData *data);

#endif