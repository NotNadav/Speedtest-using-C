#include <gtk/gtk.h>
#include <pthread.h>
#include <stdio.h>
#include <curl/curl.h>
#include "speedtest.h"

GtkWidget *status_label;
GtkWidget *ping_value;
GtkWidget *download_value;
GtkWidget *upload_value;
GtkWidget *start_button;

void update_label(GtkWidget *label, const char *text) {
    gdk_threads_enter();
    gtk_label_set_text(GTK_LABEL(label), text);
    gdk_threads_leave();
}

void *speed_test_thread(void *data) {
    gtk_widget_set_sensitive(start_button, FALSE);
    
    update_label(status_label, "Testing ping...");
    double ping_ms;
    if (test_ping(&ping_ms) == 0) {
        char buffer[64];
        sprintf(buffer, "%.0f ms", ping_ms);
        update_label(ping_value, buffer);
    } else {
        update_label(ping_value, "Error");
    }
    
    update_label(status_label, "Testing download speed...");
    double download_mbps;
    if (test_download(&download_mbps) == 0) {
        char buffer[64];
        sprintf(buffer, "%.2f Mbps", download_mbps);
        update_label(download_value, buffer);
    } else {
        update_label(download_value, "Error");
    }
    
    update_label(status_label, "Testing upload speed...");
    double upload_mbps;
    if (test_upload(&upload_mbps) == 0) {
        char buffer[64];
        sprintf(buffer, "%.2f Mbps", upload_mbps);
        update_label(upload_value, buffer);
    } else {
        update_label(upload_value, "Error");
    }
    
    update_label(status_label, "Test complete!");
    
    gdk_threads_enter();
    gtk_widget_set_sensitive(start_button, TRUE);
    gdk_threads_leave();
    
    return NULL;
}

void on_start_clicked(GtkWidget *widget, gpointer data) {
    pthread_t thread;
    pthread_create(&thread, NULL, speed_test_thread, NULL);
    pthread_detach(thread);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gdk_threads_init();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Speedtest");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 280);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    start_button = gtk_button_new_with_label("Start Test");
    gtk_widget_set_size_request(start_button, 120, 40);
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), start_button, FALSE, FALSE, 0);
    
    status_label = gtk_label_new("Ready to test");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 10);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 10);
    
    GtkWidget *ping_label = gtk_label_new("Ping:");
    gtk_widget_set_halign(ping_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), ping_label, 0, 0, 1, 1);
    
    ping_value = gtk_label_new("-");
    gtk_widget_set_halign(ping_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), ping_value, 1, 0, 1, 1);
    
    GtkWidget *download_label = gtk_label_new("Download:");
    gtk_widget_set_halign(download_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), download_label, 0, 1, 1, 1);
    
    download_value = gtk_label_new("-");
    gtk_widget_set_halign(download_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), download_value, 1, 1, 1, 1);
    
    GtkWidget *upload_label = gtk_label_new("Upload:");
    gtk_widget_set_halign(upload_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), upload_label, 0, 2, 1, 1);
    
    upload_value = gtk_label_new("-");
    gtk_widget_set_halign(upload_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), upload_value, 1, 2, 1, 1);
    
    gtk_widget_show_all(window);
    
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
    
    curl_global_cleanup();
    
    return 0;
}
