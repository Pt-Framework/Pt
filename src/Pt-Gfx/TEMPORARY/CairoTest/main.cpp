// https://www.lemoda.net/c/cairo-to-png
// https://www.cairographics.org/FAQ

#include <stdlib.h>
#include <cairo.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

int main ()
{
    int SIZEX = 80;
    int SIZEY = 80;

    cairo_surface_t* cs;
    cairo_t*         c;

    cs = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, SIZEX, SIZEY);
    c  = cairo_create (cs);

    cairo_set_operator  (c, CAIRO_OPERATOR_SOURCE);
    cairo_set_antialias (c, CAIRO_ANTIALIAS_DEFAULT);
    cairo_set_line_width(c, 1.0);

    /* Draw something and write to PNG */
    cairo_set_source_rgba(c, 0.0, 0.0, 0.0, 1.0);
    cairo_paint          (c);

    cairo_set_source_rgba(c, 1.0, 1.0, 1.0, 1.0);

    cairo_move_to(c, 1.5, 1.5);
    cairo_line_to(c, 3.5, 1.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 1.5);
    cairo_line_to(c, 11.5, 3.5);
    cairo_stroke (c);

    cairo_move_to(c, 1.5, 21.5);
    cairo_line_to(c, 3.5, 21.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 21.5);
    cairo_line_to(c, 11.5, 23.5);
    cairo_stroke (c);

    cairo_move_to(c, 1.5, 41.0);
    cairo_line_to(c, 3.5, 41.0);
    cairo_stroke (c);

    cairo_move_to(c, 11.0, 41.5);
    cairo_line_to(c, 11.0, 43.5);
    cairo_stroke (c);

    cairo_surface_write_to_png (cs, "CairoTest1.png");

    /* Draw something and write to PNG */
    cairo_set_source_rgba(c, 0.0, 0.0, 0.0, 1.0);
    cairo_paint          (c);

    cairo_set_source_rgba(c, 1.0, 1.0, 1.0, 1.0);

    cairo_move_to(c, 1.0, 1.5);
    cairo_line_to(c, 4.0, 1.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 1.0);
    cairo_line_to(c, 11.5, 4.0);
    cairo_stroke (c);

    cairo_move_to(c, 1.0, 21.5);
    cairo_line_to(c, 4.0, 21.5);
    cairo_stroke (c);

    cairo_move_to(c, 11.5, 21.0);
    cairo_line_to(c, 11.5, 24.0);
    cairo_stroke (c);

    cairo_move_to(c, 1.0, 41.0);
    cairo_line_to(c, 4.0, 41.0);
    cairo_stroke (c);

    cairo_move_to(c, 11.0, 41.0);
    cairo_line_to(c, 11.0, 44.0);
    cairo_stroke (c);

    cairo_surface_write_to_png (cs, "CairoTest2.png");

    /* Done */
    cairo_destroy        (c);
    cairo_surface_destroy(cs);
    return 0;
}
