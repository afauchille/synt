// gui.c
// boutons open, run, save, exit
// afficher une image
// valgrind

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ray/ray.h"
#include "gui/gui.h"

char *filename = NULL;
GtkWidget *table = NULL;
GtkWidget *MainWindow = NULL;
GtkImage *displayed_image = NULL;
GdkPixbuf *pix = NULL;

extern char path_changed;

extern struct image* animation_frames;
extern unsigned nb_frames;

void display_image(struct image* img)
{
    if (img == NULL)
	printf("in display_image() => image is NULL\n");
    if (img->pixels == NULL)
	printf("in display_image() => image data is NULL\n");

    pix = image_to_GdkPixbuf(img);
    display_pixbuf(pix);
}

void display_pixbuf(GdkPixbuf* pixbuf)
{
    static char image_is_visible = 0;

    if (!image_is_visible)
    {
	displayed_image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf));
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(displayed_image),
                      0, 4, 0, 4,
                      GTK_EXPAND, GTK_EXPAND, // | GTK_FILL
                      0, 0);
	image_is_visible = 1;
    }
    else
	gtk_image_set_from_pixbuf(displayed_image, pixbuf);

    gtk_widget_show_all(MainWindow);
}

/*void show_img(char *filename)
{
    GtkWidget * pWindow = NULL;

    pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_default_size(GTK_WINDOW(pWindow), 620, 400);

    GtkWidget *img;
    img =  gtk_image_new_from_file  (filename);

    gtk_container_add(GTK_CONTAINER(pWindow), img);
    g_signal_connect(G_OBJECT(pWindow), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(pWindow);
    gtk_main();

}*/

extern unsigned fps;
extern unsigned seconds;

void get_name(GtkWidget *pEntry)
{
    GtkEntryBuffer *buffer;
    buffer = gtk_entry_get_buffer (GTK_ENTRY(pEntry));
    const gchar *buff = gtk_entry_get_text(GTK_ENTRY(pEntry));
    guint strln = gtk_entry_buffer_get_length (buffer);
    char dest[100];
    if(strln == 0)
        strcpy(dest, nb_frames ? "anim" : "output");
    else
        strcpy(dest, (char *) buff);
    if (!nb_frames)
    {
	strcat(dest, ".png");
	gdk_pixbuf_save(pix, dest, "png", NULL, NULL);
	printf("Image has been saved as \"%s\"\n", dest);
    }
    else
    {
	mkdir(dest, 0777);
	char path[150] = {0};
	strcpy(path, dest);
	strcat(path, "/animation.fm");
	FILE* f = fopen(path, "w");
	fprintf(f, "%d:%d", fps, seconds);
	fclose(f);
	printf("Animation has been saved as \"%s\"\n", path);
	memset(path, 0, 150);
	for (unsigned i = 0; i < nb_frames; i++)
	{
	    sprintf(path, "%s/%04d.png", dest, i);
	    gdk_pixbuf_save(image_to_GdkPixbuf(animation_frames + i), path, "png", NULL, NULL);
	}
    }
}

void save_callback()
{
    if (!pix)
	   printf("No image to save\n");
    else
    {
        //nouvelle fenetre
        GtkWidget *pWindow = NULL;
        pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_default_size(GTK_WINDOW(pWindow), 300, 100);
        gtk_window_set_title(GTK_WINDOW(pWindow), nb_frames ? "Save animation" : "Save file");

        table = gtk_table_new(2, 4, TRUE); //2 lignes 3 colonnes

        GtkWidget *nlabel = gtk_label_new(nb_frames ? "/animation.fm" : ".png");
        gtk_table_attach(GTK_TABLE(table), nlabel,
                        3, 4, 0, 1, //column 3, line 1
                        GTK_EXPAND, GTK_EXPAND, // | GTK_FILL
                        0, 0);

        GtkWidget *entry = gtk_entry_new();
        gtk_table_attach(GTK_TABLE(table), entry,
                        0, 3, 0, 1, //column 2, line 1
                        GTK_EXPAND | GTK_FILL, GTK_EXPAND, // | GTK_FILL
                        0, 0);

        GtkWidget *instr = gtk_label_new(nb_frames ? "Give a file name and press Enter.\nDefault: anim/animation.fm" : "Give a file name and press Enter.\nDefault: output.png" );
        gtk_table_attach(GTK_TABLE(table), instr,
                        0, 2, 1, 2, //column 2, line 2
                        GTK_EXPAND, GTK_EXPAND, // | GTK_FILL
                        0, 0);

        gtk_container_add(GTK_CONTAINER(pWindow), table);
        g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(get_name), NULL);
        g_signal_connect(G_OBJECT(pWindow), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
        gtk_widget_show_all(pWindow);
        gtk_main();
    }
}


void ask_file_dialog(GtkWidget *btn, GtkWidget *path)
{
    GtkWidget *dialog = btn;
    dialog = gtk_file_chooser_dialog_new ("Open File", NULL,
                          GTK_FILE_CHOOSER_ACTION_OPEN,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                          NULL);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        printf("Path %s at Adress %p\n", filename, (void*)filename);
    }
    gtk_widget_destroy (dialog);
    gtk_label_set_text(GTK_LABEL(path), filename);
    //show_img(filename);
}


void select_file(GtkWidget *table, GtkWidget *showpath)
{
    GtkWidget *OpenBtn;

    /* Bouton avec un label */
    OpenBtn = gtk_button_new_with_label("Open");

    /* Connexion du signal "clicked" du bouton */
    g_signal_connect(G_OBJECT(OpenBtn), "clicked", G_CALLBACK(ask_file_dialog), (GtkWidget*) showpath);

    gtk_table_attach(GTK_TABLE(table), OpenBtn,
                      0, 1, 4, 5,
                      GTK_EXPAND, GTK_EXPAND,
                      0, 0);
    path_changed = 1;
}


void quit(GtkWidget *table)
{
    GtkWidget *QuitBtn;
    /* Bouton avec un label */
    QuitBtn = gtk_button_new_with_label("Quit");

    /* Connexion du signal "clicked" du bouton */
    g_signal_connect(G_OBJECT(QuitBtn), "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_table_attach(GTK_TABLE(table), QuitBtn,
                      3, 4, 4, 5, //column 2, line 3
                      GTK_EXPAND, GTK_EXPAND, // | GTK_FILL
                      0, 0);
}


void run(GtkWidget *table)
{
    GtkWidget *RunBtn;
    /* Bouton avec un label */
    RunBtn = gtk_button_new_with_label("Start");

    /* Connexion du signal "clicked" du bouton */
    g_signal_connect(G_OBJECT(RunBtn), "clicked", G_CALLBACK(raytrace_callback), NULL);

    gtk_table_attach(GTK_TABLE(table), RunBtn,
                      1, 2, 4, 5, //column 2, line 3
                      GTK_EXPAND, GTK_EXPAND, // | GTK_FILL
                      0, 0);
}

void save_image(GtkWidget *table)
{
    GtkWidget *SaveBtn;
    /* Bouton avec un label */
    SaveBtn = gtk_button_new_with_label("Save");

    /* Connexion du signal "clicked" du bouton */
    g_signal_connect(G_OBJECT(SaveBtn), "clicked", G_CALLBACK(save_callback), NULL);

    gtk_table_attach(GTK_TABLE(table), SaveBtn,
                      2, 3, 4, 5,
                      GTK_EXPAND, GTK_EXPAND, // | GTK_FILL
                      0, 0);
}

void init_gui(int argc, char **argv)
{
    /* Variables */
    //GtkWidget * MainWindow = NULL;

    /* Initialisation de GTK+ */
    gtk_init(&argc, &argv);

    /* Création de la fenêtre */
    MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /* Définition de la position */
    gtk_window_set_position(GTK_WINDOW(MainWindow), GTK_WIN_POS_CENTER);

    /* Définition de la taille de la fenêtre */
    gtk_window_set_default_size(GTK_WINDOW(MainWindow), 620, 400);

    /* Titre de la fenêtre */
    gtk_window_set_title(GTK_WINDOW(MainWindow), "FastManta GUI");

    //gtk_window_set_icon_from_file(GTK_WINDOW(MainWindow), "manta_ray.png", NULL);


    // ------- CREATION DE LA TABLE -------
    table = gtk_table_new(6, 4, TRUE); // 4 lignes, 4 colonnes

    gtk_container_add(GTK_CONTAINER(MainWindow), GTK_WIDGET(table));
    
    GtkWidget *path = gtk_label_new(NULL);
    gtk_table_attach(GTK_TABLE(table), path,
                      0, 4, 5, 6, //column 4, line 4
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND, // | GTK_FILL
                      0, 0);

    // ------- AJOUT BOUTONS -------
    select_file(table, path);
    quit(table);
    save_image(table);
    run(table);


    //display_image(table, NULL);

    /* Connexion du signal "delete-event": ferme la fenetre */
    g_signal_connect(G_OBJECT(MainWindow), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    /* Affichage et boucle évènementielle */
    gtk_widget_show_all(MainWindow);
    gtk_main();
}
