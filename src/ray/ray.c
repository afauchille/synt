#define _BSD_SOURCE

/******************************* INCLUDES ************************************/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <sys/syscall.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "ray/camera.h"
#include "tools/color.h"
#include "tools/image.h"
#include "tools/vectors.h"
#include "shapes/shapes.h"
#include "lights/lights.h"
#include "ray/ray.h"
#include "parser/parser.h"
#include "shapes/material.h"
#include "gui/gui.h"

unsigned coreNB;
int curr_line;
int curr_thread;
char path_changed;
/****************************** FUNCTIONS ************************************/

struct ray *new_ray(double x, double y, double z, double t, double xdir,
        double ydir, double zdir)
{
    struct ray *r = malloc(sizeof(struct ray));
    r->pos = new_vector(x, y, z);
    r->t = t;
    r->dir = new_vector(xdir, ydir, zdir);
    return r;
}

void free_ray(struct ray *r)
{
    free_vector(r->dir);
    free_vector(r->pos);
    free(r);
}

struct ray *inter_ray(struct camera *c, struct viewplane *v, double x,
        double y, struct image *img)
{
    struct vector *tmp0 =
        mult_scalar(c->right, x * v->width / (double)(img->w));
    struct vector *tmp1 = add_vector(tmp0, c->upleft);
    struct vector *tmp2 =
        mult_scalar(c->up, -y * v->height / (double)(img->h));
    struct vector *tmp3 = add_vector (tmp2, tmp1);
    struct vector *tmp4 = vector_from_points(c->pos, tmp3);
    struct vector *inter_pos = normal_vector(tmp4);

    double xi = inter_pos->x, yi = inter_pos->y, zi = inter_pos->z;

    free_vector(tmp0);
    free_vector(tmp1);
    free_vector(tmp2);
    free_vector(tmp3);
    free_vector(tmp4);
    free_vector(inter_pos);

    return new_ray(c->pos->x, c->pos->y, c->pos->z, 0, xi, yi, zi);
}

void raytracing(struct image *img, struct camera *c,
        struct shape_list *sl, struct light_list *ll, struct viewplane *v,
        double anti_cr, double y, int nt)
{
    struct ray *r;
    double min_dist, curr_dist;
    struct vector *inter_dir, *inter_pos, *tmp;
    struct shape_list *tmp_sl, *inter_sl = NULL;
    struct light_list *tmp_ll;
    struct color *curr_color, *final_color;

    double dist_tmp;
    struct vector *light_vector_tmp;
    struct vector *normal_tmp;
    struct ray *r_tmp;
    int inter_count;

    for (double x = 0; x < img->w; ++x)
    {
        final_color = new_color(0, 0, 0);

        for (double xc = 0; xc < anti_cr; ++xc)
        {
            for (double yc = 0; yc < anti_cr; ++yc)
            {
                min_dist = INT_MAX;
                r = inter_ray(c, v, x + xc / anti_cr , y + yc / anti_cr, img);
                curr_color = new_color(0, 0, 0);

                tmp_sl = sl;

                while (tmp_sl)
                {
                    curr_dist = tmp_sl->intersect_shape(tmp_sl->obj, r, nt);

                    if (curr_dist < min_dist)
                    {
                        inter_sl = tmp_sl;
                        min_dist = curr_dist;
                    }

                    tmp_sl = tmp_sl->next;
                }
                if (min_dist == INT_MAX || min_dist < 0)
                    set_pixel_img(img, 0, 0, 0, x, y);
                else
                {
                    if (xc == 0 && yc == 0)
                    {
                        final_color->r = inter_sl->m->diffuse->r;
                        final_color->g = inter_sl->m->diffuse->g;
                        final_color->b = inter_sl->m->diffuse->b;

                        reverse_normalize_color(final_color);
                    }

                    tmp = mult_scalar(r->dir, min_dist);
                    inter_pos = add_vector(tmp, c->pos);
                    inter_dir = inter_sl->get_normal_shape(inter_pos,
                                    inter_sl->obj, nt);

                    apply_texture(curr_color, inter_sl->m,
                                                    inter_dir, img, x, y);
                    inter_dir = apply_mapping(inter_dir, inter_sl->m);

                    tmp_ll = ll;
                    inter_count = 0;

                    while (tmp_ll)
                    {
                        light_vector_tmp = vector_from_points(inter_pos,
                                tmp_ll->pos);
                        normal_tmp = normal_vector(light_vector_tmp);
                        r_tmp = new_ray(inter_pos->x, inter_pos->y,
                            inter_pos->z, 0, normal_tmp->x, normal_tmp->y,
                            normal_tmp->z);

                        tmp_sl = sl;

                        get_light(inter_dir, inter_pos, inter_sl, tmp_ll,
                                    curr_color);

                        if (inter_sl->m->textured != 4)
                        {
                            while (tmp_sl)
                            {
                                dist_tmp = tmp_sl->intersect_shape(tmp_sl->obj,
                                    r_tmp, nt);

                                if (tmp_sl != inter_sl && dist_tmp > 0 &&
                                    dist_tmp != INT_MAX)
                                {
                                    inter_count += 1;
                                    break;
                                }

                                tmp_sl = tmp_sl->next;
                            }

                            for (int bis = 0; bis < inter_count; bis++)
                            {
                                curr_color->r *= 0.3;
                                curr_color->g *= 0.3;
                                curr_color->b *= 0.3;
                            }
                        }

                        free_vector(light_vector_tmp);
                        free_vector(normal_tmp);
                        free_ray(r_tmp);

                        tmp_ll = tmp_ll->next;
                    }

                    free_vector(tmp);
                    free_vector(inter_pos);
                    free_vector(inter_dir);
                }

                void_add_color(final_color, curr_color);

                free_ray(r);
                free_color(curr_color);
            }
        }

        void_div_scalar_color(final_color, anti_cr * anti_cr);
        normalize_color(final_color);
        set_pixel_img(img, final_color->r, final_color->g, final_color->b,
                                                                    x, y);

        free_color(final_color);
    }
}

struct color *reflect_ray(struct vector *dir, struct vector *pos,
                            struct shape_list *inter_sl, struct shape_list *sl,
                            struct camera *c, struct image *img,
                            struct viewplane *v, struct light_list *ll,
                            int limit)
{
    struct shape_list *tmp_sl, *new_inter_sl = NULL;
    double curr_dist;
    double min_dist = INT_MAX;
    int nt = 1;
    struct vector *tmp, *inter_pos;

    struct vector *normal = inter_sl->get_normal_shape(pos, inter_sl->obj, nt);
    struct vector *old_dir = normal;
    double angle = dot_product(dir, normal);
    normal = mult_scalar(normal, -2 * angle);
    struct vector *new_dir = add_vector(dir, normal);

    struct ray *ray = new_ray(pos->x, pos->y, pos->z, 0, new_dir->x,
                                                    new_dir->y, new_dir->z);

    //size_t place;
    struct color *curr_color;
    struct color *reflect_color = new_color(0, 0, 0);

    struct light_list *tmp_ll;

    /*place = pos->y * img->w + pos->x;
    curr_color = new_color(img->pixels[place].r,
                        img->pixels[place].g, img->pixels[place].b);*/
    curr_color = new_color(inter_sl->m->diffuse->r,
                    inter_sl->m->diffuse->g, inter_sl->m->diffuse->b);

    reverse_normalize_color(curr_color);

    tmp_sl = sl;

    if (limit == 0)
        return new_color(0, 0, 0);

    while (tmp_sl)
    {
        if (tmp_sl != inter_sl)
        {
            curr_dist = tmp_sl->intersect_shape(tmp_sl->obj, ray, nt);

            if (curr_dist < min_dist)
            {
                new_inter_sl = tmp_sl;
                min_dist = curr_dist;
            }
        }

        tmp_sl = tmp_sl->next;
    }

    if (min_dist != INT_MAX && min_dist > 0)
    {
        tmp = mult_scalar(ray->dir, min_dist);
        inter_pos = add_vector(tmp, pos);

        tmp_ll = ll;

        while (tmp_ll)
        {
            get_light(old_dir, inter_pos, new_inter_sl, tmp_ll, reflect_color);

            tmp_ll = tmp_ll->next;
        }

        /*place = inter_pos->y * img->w + inter_pos->x;
        img->pixels[place].r = reflect_color->r;
        img->pixels[place].g = reflect_color->g;
        img->pixels[place].b = reflect_color->b;*/
        /*new_inter_sl->m->diffuse->r = reflect_color->r;
        new_inter_sl->m->diffuse->g = reflect_color->g;
        new_inter_sl->m->diffuse->b = reflect_color->b;*/

        if (new_inter_sl->m->textured == 4)
        {
            reflect_color = reflect_ray(ray->dir, inter_pos, new_inter_sl, sl,
                    c, img, v, ll, limit - 1);
        }
        reflect_color->r *= 0.5;
        reflect_color->g *= 0.5;
        reflect_color->b *= 0.5;

        void_add_color(curr_color, reflect_color);
    }

    return curr_color;
}

/*****************************************************************************/
void reflect_ray_call(struct raytracing_param rp)
{
    struct ray *r;
    double min_dist, curr_dist;
    struct vector *inter_pos, *tmp;
    struct shape_list *tmp_sl, *inter_sl = NULL;
    struct color *curr_color, *final_color, *reflect_color;
    int nt = 1;
    size_t place;
    int limit = 5;

    for (double x = 0; x < rp.img->w; ++x)
    {
        for (double y = 0; y < rp.img->h; ++y)
        {
            place = y * rp.img->w + x;
            final_color = new_color(rp.img->pixels[place].r,
                        rp.img->pixels[place].g,
                        rp.img->pixels[place].b);
            reverse_normalize_color(final_color);
            min_dist = INT_MAX;
            r = inter_ray(rp.c, rp.v, x, y, rp.img);
            curr_color = new_color(rp.img->pixels[place].r,
                        rp.img->pixels[place].g, rp.img->pixels[place].b);

            reverse_normalize_color(curr_color);

            tmp_sl = rp.sl;

            while (tmp_sl)
            {
                curr_dist = tmp_sl->intersect_shape(tmp_sl->obj, r, nt);

                if (curr_dist < min_dist)
                {
                    inter_sl = tmp_sl;
                    min_dist = curr_dist;
                }

               tmp_sl = tmp_sl->next;
            }

            if (min_dist != INT_MAX && min_dist > 0 &&
                    inter_sl->m->textured == 4)
            {
                tmp = mult_scalar(r->dir, min_dist);
                inter_pos = add_vector(tmp, rp.c->pos);

                reflect_color = reflect_ray(r->dir, inter_pos,
                                inter_sl, rp.sl, rp.c,
                                rp.img, rp.v, rp.ll, limit - 1);
                free_vector(tmp);
                free_vector(inter_pos);

                void_add_color(curr_color, reflect_color);
                void_add_color(final_color, curr_color);

                normalize_color(final_color);
            }
            else
            {
                set_pixel_img(rp.img, curr_color->r,
                    curr_color->g, curr_color->b, x, y);
            }

            free_ray(r);
            free_color(curr_color);

            set_pixel_img(rp.img, final_color->r, final_color->g,
                    final_color->b, x, y);
            free_color(final_color);
        }
    }
}

/*****************************************************************************/

void *calculator(void* data, int i)
{
    //static int curr_line = 0;
    static pthread_mutex_t curr_line_lock = PTHREAD_MUTEX_INITIALIZER;
    unsigned thread_safe_curr_line;
    struct raytracing_param* p = data;

    while (1)
    {
        pthread_mutex_lock(&curr_line_lock);
        thread_safe_curr_line = curr_line++;
        pthread_mutex_unlock(&curr_line_lock);

        if (thread_safe_curr_line >= p->img->h)
            return NULL;

        raytracing(p->img, p->c, p->sl, p->ll, p->v, p->anti_cr,
                thread_safe_curr_line, i);
    }
}

void *thread_number_calculator(void *data)
{
    //static int curr_thread = 0;
    curr_thread++;
    return calculator(data, curr_thread);
}

struct image* raytrace(struct raytracing_param rp)
{
    // Camera settings
    rp.v = new_viewplane(1, 0.5, 0.5);
    set_upleft(rp.c, rp.v);

    // Time initializer
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Calculus part
    curr_line = 0;
    curr_thread = 0;
    pthread_t *threads = malloc(sizeof(pthread_t) * coreNB);
    for (unsigned i = 0; i < coreNB; i++)
        pthread_create(threads + i, NULL, thread_number_calculator, &rp);

    for (unsigned i = 0; i < coreNB; i++)
        pthread_join(threads[i], NULL);

    reflect_ray_call(rp);

    // Display the calculation time
    gettimeofday(&end, NULL);
    long long delta_sec = end.tv_sec - start.tv_sec;
    long long delta_msec = (end.tv_usec - start.tv_usec) / 1000;
    if (delta_msec < 0)
    {
	delta_sec--;
	delta_msec += 1000;
    }
    printf("Time: %lld.%03lld s\n", delta_sec, delta_msec);

    // Free and return
    //free_shape_list(rp.sl);
    //free_camera(rp.c);
    //free_viewplane(rp.v);
    //free_light_list(rp.ll);
    return rp.img;
}

struct image* animation_frames;
unsigned curr_frame_id;
unsigned nb_frames;

GdkPixbuf** animation_frames_pixbuf;
unsigned fps;
unsigned seconds;

gboolean show_animation(gpointer data)
{
    if (data) {}
    if (curr_frame_id == nb_frames)
	return FALSE;
    display_image(animation_frames + curr_frame_id);
    curr_frame_id++;
    return TRUE;
}

gboolean show_animation_pixbuf(gpointer data)
{
    if (data) {}
    if (curr_frame_id == nb_frames)
	return FALSE;
    display_pixbuf(animation_frames_pixbuf[curr_frame_id]);
    curr_frame_id++;
    return TRUE;
}

/*struct net {
    int socket, new_socket, go;
    nfds_t nb_socket;
    struct pollfd ufds[1000];
};

int curr_line;
int curr_thread_server;

void *thread_number_calculator_server(void *data)
{
    //static int curr_thread = 0;
    curr_thread_server++;
    return calculator(data, curr_thread);
}

void *receive_connection_from_client(void *data)
{
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;
    struct net *network = (struct net *)data;
    struct pollfd *fds = network->ufds;
    
    int sock = network->socket;
    nfds_t nb_sock = network->nb_socket;
    int new_sock;

    while (1)
    {
        new_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_size);

        if (new_sock == -1)
        {
            perror("accept");
            close(new_sock);
            exit(EXIT_FAILURE);
        }
        else
        {
            nb_sock++;
            fds[nb_sock - 1].fd = new_sock;
            fds[nb_sock - 1].events = POLLIN;
            fds[nb_sock - 1].revents = 0;
            network->nb_socket++;
            network->new_socket = new_sock;
            network->go = 1;
        }
    }

    pthread_exit(NULL);
}

void *receive_from_client(void *data)
{
    struct net *network = (struct net *)data;
    struct pollfd *fds = network->ufds;
    nfds_t nb_sock = 0;
    int curr_sock;
    char *lol[1000];
    char buf[2048];
    char *login = malloc(sizeof(char));
    char *logins = malloc(sizeof(char));
    FILE *f;
    int received;
    int polling;
    char *welcome =
        "\nWelcome. Please identify by typing \"IDENTIFY <login_x>\"\n\n";
    char *connected =
        "\nConnected. Type DISCONNECT to close the connection.\n\n";
    char *denied = "\nACCESS DENIED\n";
    char *message = malloc(sizeof(char) * 2048);
    nfds_t pos;

    while (1)
    {
        nb_sock = network->nb_socket;

        if (network->go)
        {
            send(network->new_socket, welcome, strlen(welcome), 0);
            network->go = 0;
	    }

        polling = poll(fds, nb_sock, 500);

        if (polling == -1)
            perror("poll");
        else if (polling != 0)
        {
            for (nfds_t j = 0; j < nb_sock; j++)
            {
                if (fds[j].revents & POLLIN)
                {
                    received = recv(fds[j].fd, buf, 2048, 0);
                    curr_sock = fds[j].fd;
                    fds[j].revents = 0;
                    break;
                }
            }

            if (received != -1)
            {
                if (received == 0)
                    fprintf(stderr, "socket %d hung up\n", curr_sock);
                else
                {
                    if (strncmp("IDENTIFY", buf, 8) == 0)
                    {
                        memset(login, 0, 8);

                        login = malloc(sizeof(char) * (strlen(buf) - 9));

                        for (size_t i = 9; i < strlen(buf) - 1; i++)
                            login[i - 9] = buf[i];
                    }
                    else
                    {
                        if (strlen(buf) != 0 && strlen(buf) <= 2048)
                        {
                            message = strcat(message, "<");
                            message = strcat(message, lol[curr_sock - 4]);
                            message = strcat(message, "> ");
                            message = strcat(message, buf);
                            message = strcat(message, "\n");

                            for (nfds_t j = 0; j < nb_sock; j++)
                            {
                                if (fds[j].fd != curr_sock)
                                    send(fds[j].fd, message, strlen(message),
                                                                            0);
                            }

                            fprintf(stdout, "<%s> %s\n", lol[curr_sock - 4],
                                                                        buf);
                        }
                        else if (strlen(buf) > 2048)
                            send(curr_sock, "Your message is too long\n", 25,
                                                                            0);
                    }
                    memset(message, 0, strlen(message));
                    memset(buf, 0, strlen(buf));
                }
            }
        }
    }

    fclose(f);
    free(login);
    free(logins);
    free(message);
    pthread_exit(NULL);
}*/

const char* get_filename_ext(const char *file)
{
    const char *dot = strrchr(file, '.');
    if(!dot || dot == file) return "";
    return dot + 1;
}

void raytrace_callback()
{
    struct raytracing_param rp;
    rp.img = malloc(sizeof(struct image));
    curr_frame_id = 0;

    if (!strcmp("fm", get_filename_ext(filename)))
    {
	FILE* f = fopen(filename, "r");
	fscanf(f, "%d:%d", &fps, &seconds);
	fclose(f);
	nb_frames = fps * seconds;
	animation_frames_pixbuf = calloc(sizeof(GdkPixbuf*), nb_frames);
	size_t i;
	for (i = 0; filename[i]; i++) ;
	for (; filename[i] != '/'; i--) ;
	filename[i] = 0;
	for (; filename[i] != '/'; i--) ;
	i++;
	char path[150] = {0};
	for (unsigned j = 0; j < nb_frames; j++)
	{
	    sprintf(path, "%s/%d.png", filename + i, j);
	    animation_frames_pixbuf[j] = gdk_pixbuf_new_from_file(path, NULL);
	    memset(path, 0, 150);
	}
	for (; filename[i]; i++) ;
	filename[i] = '/';
	g_timeout_add(1000 / fps, show_animation_pixbuf, NULL);
	return;
    }

    if (path_changed)
	parse_file(filename, &rp.c, &rp.sl, &rp.ll, rp.img, &coreNB,
		   &(rp.anti_cr), &fps, &seconds);
    if (!fps)
    {
	nb_frames = 0;
	rp.img->pixels = calloc(rp.img->w * rp.img->h, sizeof(struct pixel));
        raytrace(rp);
	display_image(rp.img);
	free(rp.img);
    }
    /*else
    {
	nb_frames = fps * seconds;
	animation_frames = malloc(sizeof(struct image) * nb_frames);
	char* port = "4242";
	struct addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	int addr = getaddrinfo(NULL, port, &hints, &res);
	
	if (addr != 0)
	{
	    perror("getaddrinfo");
	    exit(EXIT_FAILURE);
	}
	
	int sock = -1;
	
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	
	if (sock == -1)
	{
	    perror("socket");
	    exit(EXIT_FAILURE);
	}
	
	int yes = 1;
	
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
	int binding = bind(sock, res->ai_addr, res->ai_addrlen);
	
	if (binding == -1)
	{
	    perror("bind");
	    close(sock);
	    exit(EXIT_FAILURE);
	}
	
	int listening = listen(sock, 5);

	if (listening == -1)
	{
	    perror("listen");
	    exit(EXIT_FAILURE);
	}
	
	struct net *data = malloc(sizeof(struct net));
	
	data->socket = sock;
	data->go = 0;
	data->nb_socket = 0;
	
	pthread_t receive_from_client_thread;
	pthread_t receive_connection_from_client_thread;
	
	if (pthread_create(&receive_connection_from_client_thread, NULL,
			   receive_connection_from_client, (void *)data))
	{
        fprintf(stderr,
		"Error while creating thread receive_connection_from_client\n");
        return 1;
	}
	
	
	if (pthread_create(&receive_from_client_thread, NULL, receive_from_client,
			   (void *)data))
	{
	    fprintf(stderr, "Error while creating thread receive_from_client\n");
	    return 1;
	}
	
	if (pthread_join(receive_connection_from_client_thread, NULL))
	{
	    fprintf(stderr,
		    "Error joining thread receive_connection_from_client\n");
	    return 2;
	}
	
	if (pthread_join(receive_from_client_thread, NULL))
	{
	    fprintf(stderr, "Error joining thread receive_from_client\n");
	    return 2;
	}
	
	close (sock);
	}*/
    else
    {
	nb_frames = fps * seconds;
	if (path_changed)
	{
	    animation_frames = malloc(sizeof(struct image) * nb_frames);
	    struct image* dim = rp.img;
	    double l_posz = 0;
	    struct light_list *ll = rp.ll;
	    if (ll)
	      l_posz = ll->pos->z;
	    for (unsigned i = 0; i < nb_frames; i++)
	    {
		animation_frames[i].pixels = calloc(dim->w * dim->h, sizeof(struct pixel));
		animation_frames[i].w = dim->w;
		animation_frames[i].h = dim->h;
		rp.img = animation_frames + i;
		raytrace(rp);
		rp.c->pos = add_vector(rp.c->pos, mult_scalar(rp.c->dir, 0.4 / fps));
		if (ll == NULL)
		  return;
		ll->pos->z = l_posz + 36 * sin(2 * pi * (double)i / (double)(fps * 51.) );
	    }
	}
	path_changed = 0;
	//free_image(dim);
	g_timeout_add(1000 / fps, show_animation, NULL);
    }
}
/*****************************************************************************/
