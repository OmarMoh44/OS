#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <sys/wait.h>
#include <cairo.h>

int pid;

void create_image_from_text(const char *filename1, const char *filename2)
{
    // Open the first text file
    FILE *file1 = fopen(filename1, "r");
    if (file1 == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Open the second text file
    FILE *file2 = fopen(filename2, "r");
    if (file2 == NULL)
    {
        perror("Error opening file");
        fclose(file1);
        return;
    }

    // Create a Cairo surface for the image
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 3000, 3000);
    cairo_t *cr = cairo_create(surface);

    // Check if surface and context creation were successful
    cairo_status_t status = cairo_surface_status(surface);
    if (status != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Error creating Cairo surface: %s\n", cairo_status_to_string(status));
        fclose(file1);
        fclose(file2);
        return;
    }

    status = cairo_status(cr);
    if (status != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "Error creating Cairo context: %s\n", cairo_status_to_string(status));
        cairo_surface_destroy(surface);
        fclose(file1);
        fclose(file2);
        return;
    }

    // Set font options
    cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    // Start drawing
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // White background
    cairo_paint(cr);

    // Set font color to black
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black font color

    // Variables for drawing text
    char character;
    double x = 10.0, y = 20.0;

    // Draw text from first file
    while ((character = fgetc(file1)) != EOF)
    {
        cairo_move_to(cr, x, y);

        if (character == '\n')
        {
            y += 20.0;
            x = 10.0;
        }
        else if (character == ' ')
        {
            x += 10.0;
        }
        else if (character == '\t')
        {
            x += 40.0;
        }
        else
        {
            char str[2] = {character, '\0'};
            cairo_show_text(cr, str);
            x += 8.0;
        }
    }

    // Draw text from second file
    while ((character = fgetc(file2)) != EOF)
    {
        cairo_move_to(cr, x, y);

        if (character == '\n')
        {
            y += 20.0;
            x = 10.0;
        }
        else if (character == ' ')
        {
            x += 10.0;
        }
        else if (character == '\t')
        {
            x += 40.0;
        }
        else
        {
            char str[2] = {character, '\0'};
            cairo_show_text(cr, str);
            x += 8.0;
        }
    }

    // Finish drawing
    cairo_surface_write_to_png(surface, "output_image.png");

    // Cleanup
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    fclose(file1);
    fclose(file2);
}


void button_clicked(GtkButton *button, gpointer data)
{
    GSList *group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(data));
    char *selected_option = NULL;
    while (group)
    {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(group->data)))
        {
            selected_option = (gchar *)gtk_button_get_label(GTK_BUTTON(group->data));
            break;
        }
        group = g_slist_next(group);
    }

    // Retrieve the entered text
    GtkWidget *path_entry = g_object_get_data(G_OBJECT(button), "path_entry");
    char *entered_path = (gchar *)gtk_entry_get_text(GTK_ENTRY(path_entry));

    GtkWidget *quantum_entry = g_object_get_data(G_OBJECT(button), "quantum_entry");
    char *entered_quantum = (gchar *)gtk_entry_get_text(GTK_ENTRY(quantum_entry));

    // Print the retrieved values
    g_print("Selected option: %s\n", selected_option);
    g_print("Entered path: %s\n", entered_path);

    // Fork to create a child process
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // Child process
    {
        // Detach the child process from the parent process
        if (setsid() == -1)
        {
            perror("setsid");
            exit(EXIT_FAILURE);
        }

        // Execute the process generator
        execl("./process_generator.out", "process_generator", selected_option, entered_path, entered_quantum, NULL);
        // If execl returns, there was an error
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else // Parent process
    {
        int status;
        // Wait for the child process to finish
        waitpid(pid, &status, 0);
        create_image_from_text("scheduler.pref", "scheduler.log");
        printf("\nHello from GUI\n");
    }
}

void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *vbox;
    GtkWidget *radio1, *radio2, *radio3;
    GtkWidget *path_entry;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *quantum_label; // New label for "Quantum"
    GtkWidget *quantum_entry; // New entry for quantum value

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);

    // Set the default size of the window
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600); // Width: 300, Height: 200

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Add radio buttons to vbox
    radio1 = gtk_radio_button_new_with_label(NULL, "HPF");
    gtk_box_pack_start(GTK_BOX(vbox), radio1, FALSE, FALSE, 0);

    radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "SRTN");
    gtk_box_pack_start(GTK_BOX(vbox), radio2, FALSE, FALSE, 0);

    radio3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "RR");
    gtk_box_pack_start(GTK_BOX(vbox), radio3, FALSE, FALSE, 0);

    // Add grid to vbox
    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 0);

    // Add label "Path" to grid
    label = gtk_label_new("Quantum:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    // Add text entry to grid
    path_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), path_entry, 1, 0, 1, 1);

    // Add label "Quantum" to grid
    quantum_label = gtk_label_new("Path:");
    gtk_grid_attach(GTK_GRID(grid), quantum_label, 0, 0, 1, 1);

    // Add text entry for quantum to grid
    quantum_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), quantum_entry, 1, 1, 1, 1);

    // Add button
    button = gtk_button_new_with_label("Run");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    // Set data to button
    g_object_set_data(G_OBJECT(button), "path_entry", path_entry);
    g_object_set_data(G_OBJECT(button), "quantum_entry", quantum_entry);

    // Connect button click event
    g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), radio1);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
