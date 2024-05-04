#include <stdio.h>
#include <cairo.h>

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
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1920, 1080);
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
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    // Start drawing
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // White background
    cairo_paint(cr);

    // Set font color to black
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black font color

    // Draw text from first file
    char buffer[256];
    double x = 10, y = 20;
    while (fgets(buffer, sizeof(buffer), file1))
    {
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, buffer);
        y += 20; // Move to next line
    }

    // Draw text from second file
    while (fgets(buffer, sizeof(buffer), file2))
    {
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, buffer);
        y += 20; // Move to next line
    }

    // Finish drawing
    cairo_surface_write_to_png(surface, "output_image.png");

    // Cleanup
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    fclose(file1);
    fclose(file2);
}

int main()
{
    const char *filename1 = "scheduler.pref";
    const char *filename2 = "scheduler.log";
    create_image_from_text("scheduler.pref", "scheduler.log");
    printf("Image created from text in %s and %s\n", filename1, filename2);
    return 0;
}
