// g++ -O2 -I/usr/include/cairo -lcairo ../src/Pt-Gfx/tests/CairoTest.cpp -o CairoTest

#include <cairo.h>

int main()
{
    const int imgWidth  = 1280;
    const int imgHeight = 800;

    unsigned char buffer[imgWidth * imgHeight * 4];

    cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data ( buffer, CAIRO_FORMAT_ARGB32, imgWidth, imgHeight, imgWidth * 4);
    cairo_t*         cairo        = cairo_create(cairoSurface);


    //if(cm == CompositionMode::SourceOver) {
    //    cairo_set_operator(cairo, CAIRO_OPERATOR_OVER);
    //    cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 175.0f / 255.0f);
    //}
    //else {
        cairo_set_operator(cairo, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 1.0f);
    //}

    cairo_reset_clip   (cairo);
    cairo_new_path     (cairo);
    cairo_move_to      (cairo, 150, 100); // CCW
    cairo_line_to      (cairo, 350, 350);
    cairo_line_to      (cairo, 450, 250);
    cairo_line_to      (cairo, 250, 100);
    cairo_line_to      (cairo,  50,  50);
    cairo_close_path   (cairo);
    cairo_clip_preserve(cairo);
    cairo_fill         (cairo);

    cairo_reset_clip   (cairo);
    cairo_new_path     (cairo);
    cairo_move_to      (cairo, 350, 100); // CCW
    cairo_line_to      (cairo, 550, 350);
    cairo_line_to      (cairo, 650, 250);
    cairo_line_to      (cairo, 450, 100);
    cairo_line_to      (cairo, 250,  50);
    cairo_close_path   (cairo);
    cairo_clip_preserve(cairo);
    cairo_fill         (cairo);

    cairo_destroy (cairo);
    cairo_surface_destroy (cairoSurface);

    return 0;
}
