/*
TODO: Write class description
*/

#include "game_window.h"
#include "model.h"

// ------------------------------------------------------------
// Button callback functions
// ------------------------------------------------------------
static void d_button_callback(GtkWidget* widget, gpointer data) {
    char DIRECTION_CHARS[] = {'N', 'W', 'E', 'S'}; // remove this if it can be defined elsewhere
    boolean swapResult = SwapCandy(gameboard, DIRECTION_CHARS[(int)data]);
    refresh_window(app);
}

static void c_button_callback(GtkWidget* widget, gpointer data) {
    SetSelectedCandy(gameboard, (int)data);
}

// ------------------------------------------------------------
// Assemble the window
// ------------------------------------------------------------
void make_window(GtkApplication* app) {
    GtkWidget* window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "Candy-Clone");
    gtk_window_set_resizable((GtkWindow*)window, FALSE);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
}

void fill_window(GtkApplication *app) {
    char remaining_moves[256];
    int *candy, row, col;
    GtkWidget* imageArray[]; // candy images
    GtkWidget* buttonArray[GetBoardSize(gameboard)]; // candy buttons

    sprintf(remaining_moves, "Moves left: %d", GetMoves(gameboard));

    // declare & initialize window components
    GtkWidget* window = gtk_application_get_active_window(app);
    GtkWidget* top_lvl_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget* candy_grid = gtk_grid_new();
    GtkWidget* interface_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* move_counter = gtk_label_new(remaining_moves);
    GtkWidget* dpad_grid = gtk_grid_new();

    // assemble window components
    gtk_container_add(GTK_CONTAINER(window), top_lvl_container);
    gtk_box_pack_start(GTK_BOX(top_lvl_container), candy_grid, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(top_lvl_container), interface_container, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(interface_container), move_counter, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(interface_container), dpad_grid, FALSE, FALSE, 0);

    candy = (int *)malloc(sizeof(int *));

    for(int i = 0; i < GetSize(gameboard); i++) {
        row = GetRow(gameboard, i);
        col = GetCol(gameboard, i);

        GetCandy(gameboard, i, candy);

        // create button, image and place in grid
        GtkWidget* currButton = buttonArray[i];
        currButton = gtk_toggle_button_new();
        GtkWidget* currImage = imageArray[i];
        currImage = gtk_image_new_from_file(candy_image_filenames[*candy]);
        gtk_button_set_image((GtkButton*)currButton, currImage);
        gtk_grid_attach(GTK_GRID(candy_grid), currButton, col, row, 1, 1);

        // add signal handler to button
        g_signal_connect(currButton, "toggled", G_CALLBACK(c_button_callback), GINT_TO_POINTER(i));
    }

    free(candy);

    // create directional buttons
    for (int i = 1; i < 9; i += 2) { // places buttons in shape of cross
        GtkWidget* currButton = gtk_button_new();
        GtkWidget* currImage = gtk_image_new_from_file(dbutton_image_filenames[i / 2]);
        gtk_button_set_image((GtkButton*)currButton, currImage);
        g_signal_connect(currButton, "clicked", G_CALLBACK(d_button_callback), GINT_TO_POINTER(i / 2));
        gtk_grid_attach(GTK_GRID(dpad_grid), currButton, (i % 3), (i / 3), 1, 1);
    }

    gtk_widget_show_all(window);

}

void refresh_window(GtkApplication* app) {
    GtkWidget* window = gtk_application_get_active_window(app);
    gtk_widget_destroy(gtk_bin_get_child(window));
    fill_window(app);
}

static void open(GApplication *app, GFile **files, gint n_files, const gchar *hint) {
    // initialize game board
    gameboard = CreateBoard(g_file_get_path(files[0]));

    make_window(app);
    fill_window(app);
}

int main(int argc, char **argv) {
    int status;

    app = gtk_application_new("edu.washington.cs", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(app, "open", G_CALLBACK(open), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    DestroyBoard(gameboard);

    return status;
}