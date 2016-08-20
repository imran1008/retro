/* Retro - Retro style programming
 * Copyright (C) 2016 Imran Haider
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Retro is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Retro.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

static pthread_t g_retro_thread;
static pthread_mutex_t g_framebuffer_lock;
static int g_argc;
static char **g_argv;
static void *g_framebuffer;
static volatile int g_framebuffer_width;
static volatile int g_framebuffer_size;
static volatile int g_viewport_x;
static volatile int g_viewport_y;
static volatile int g_viewport_width;
static volatile int g_viewport_height;
static volatile int g_viewport_updated;
static volatile int g_vsync_signal;
static volatile int g_fps;

const int g_bytes_per_pixel = 3;

static int do_draw(GtkWidget *widget, cairo_t *cr, void *data)
{
    int width, height;
    int cell_width, cell_height, cell_size;
    int x, y;
    int offset;
    GdkRGBA color;

    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);

    /* Compute the scale factor from a virtual pixel to physical pixel */
    cell_width = width / g_viewport_width;
    cell_height = height / g_viewport_height;
    cell_size = MIN(cell_width, cell_height);

    /* If the scale factor is less than 1, we will assume a scale factor of
     * 1 and clip out the obscured area */
    if (cell_size == 0) {
        cell_size = 1;
    }

    /* Clearig the vsync bit, which indicates that the display is in a state
     * of painting.
     */
    g_vsync_signal = 0;

    /* Lock the framebuffer. This ensures the memory buffer is not relocated
     * while it is being used.
     */
    pthread_mutex_lock(&g_framebuffer_lock);

    color.alpha = 1.0;

    /* Compute the frame buffer size, in bytes.
     */
    int framebuffer_byte_size = g_framebuffer_size * g_bytes_per_pixel;

    for (y=0; y < g_viewport_height; ++y) {
        for (x=0; x < g_viewport_width; ++x) {
            cairo_rectangle(cr, x * cell_size, y * cell_size, cell_size, cell_size);
            offset = ((y + g_viewport_y) * g_framebuffer_width + (x + g_viewport_x)) * g_bytes_per_pixel;

            /* If the offset is within the framebuffer, we will use the data in it;
             * otherwise, we will assume a black pixel
             */
            if (offset < framebuffer_byte_size) {
                color.red   = (double) ((unsigned char *) g_framebuffer)[offset+0] / 255;
                color.green = (double) ((unsigned char *) g_framebuffer)[offset+1] / 255;
                color.blue  = (double) ((unsigned char *) g_framebuffer)[offset+2] / 255;
            }
            else {
                color.red   = 0;
                color.green = 0;
                color.blue  = 0;
            }

            /* Draw the pixel.
             */
            gdk_cairo_set_source_rgba(cr, &color);
            cairo_fill (cr);
        }
    }

    /* Release the framebuffer lock and set the vsync bit. The vsync bit == 1
     * indicates that we are done drawing the current frame and we are waiting
     * for the next frame to be drawn
     */
    pthread_mutex_unlock(&g_framebuffer_lock);
    g_vsync_signal = 1;

    return FALSE;
}

static int timer_callback(GtkWidget *widget)
{
    int width, height;

    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);

    /* Schedule a redraw.
     */
    gtk_widget_queue_draw_area(widget, 0, 0, width, height);

    /* Update the window based on the currently set viewport size. The user
     * can then resize the window to any size.
     */
    if (g_viewport_updated) {
        g_viewport_updated = 0;
        GtkWidget *toplevel = gtk_widget_get_toplevel(widget);
        gtk_window_resize(GTK_WINDOW(toplevel),
                          g_viewport_width * 2,
                          g_viewport_height * 2);
    }

    return 1;
}

static void activate(GtkApplication *app, void *userdata)
{
    GtkWidget *window;

    /* Create the Retro application window.
     */
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Retro");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    /* Create a drawing area.
     */
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(do_draw), NULL);
    g_timeout_add(1000 / g_fps, (GSourceFunc) timer_callback, drawing_area);

    /* Show all windows.
     */
    gtk_widget_show_all(window);
}

static void *retro_main(void *userdata)
{
    GtkApplication *app;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_application_run(G_APPLICATION(app), g_argc, g_argv);
    g_object_unref(app);
}

int retro_init(int argc, char **argv)
{
    memset(&g_retro_thread, 0, sizeof(pthread_t));
    memset(&g_framebuffer_lock, 0, sizeof(pthread_mutex_t));

    if (pthread_mutex_init(&g_framebuffer_lock, NULL)) {
        fprintf(stderr, "Failed to create framebuffer lock\n");
        return 1;
    }

    g_argc = argc;
    g_argv = argv;
    g_fps = 25;

    pthread_create(&g_retro_thread, NULL, retro_main, NULL);

    return 0;
}

void retro_destroy()
{
    pthread_exit(NULL);
}

void *retro_video_get_framebuffer()
{
    return g_framebuffer;
}

void retro_video_set_framebuffer_size(int npixels)
{
    pthread_mutex_lock(&g_framebuffer_lock);
    g_framebuffer_size = npixels;
    g_framebuffer = realloc(g_framebuffer, npixels * g_bytes_per_pixel);
    pthread_mutex_unlock(&g_framebuffer_lock);
}

void retro_video_set_framebuffer_width(int npixels)
{
    g_framebuffer_width = npixels;
}

void retro_video_set_viewport_position(int x, int y)
{
    g_viewport_x = x;
    g_viewport_y = y;
}

void retro_video_set_viewport_size(int width, int height)
{
    g_viewport_width = width;
    g_viewport_height = height;
    g_viewport_updated = 1;
}

int retro_video_get_vsync_signal()
{
    return g_vsync_signal;
}

void retro_video_set_fps(int fps)
{
    g_fps = fps;
}

