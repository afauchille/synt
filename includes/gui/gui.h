#ifndef _GUI_H_
#define _GUI_H_

/******************************* INCLUDES ************************************/
#include <gtk/gtk.h>
#include "tools/image.h"

/******************************* DEFINES *************************************/

extern char *filename;
extern GtkWidget *table;
extern GtkWidget *MainWindow;
extern GdkPixbuf *pix;
extern GtkImage *displayed_image;

/****************************** STRUCTURES ***********************************/



/****************************** PROTOTYPES ***********************************/

//Initialize the GUI and start the main loop.
void init_gui(int argc, char **argv);
void display_image(struct image* img);
void display_pixbuf(GdkPixbuf* pixbuf);
void run(GtkWidget *table);
void quit(GtkWidget *table);
void select_file(GtkWidget *table, GtkWidget *showpath);
void save_image(GtkWidget *table);

/******************************* ENDFILE *************************************/

#endif /* _GUI_H_ */
