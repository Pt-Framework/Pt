#define CAIRO_BENCHMARK_DISPLAY_RESULTING_IMAGE \
    if(CAIRO_CHECK_RESULTING_IMAGE && !i) sdlPreviewRGB888Buffer(formatCaption("Cairo", cm, __FUNCTION__), buffer.data(), imgSize.width(), imgSize.height(), false)

static size_t cairoBenchFillPolygon(int loopCount, CompositionMode cm)
{
    size_t sum = 0;

    const Size& imgSize = BENCHMARK_IMAGE_SIZE;

    std::vector<Pt::uint8_t> buffer(imgSize.width() * imgSize.height() * 4, 0);

    cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data ( &buffer[0], CAIRO_FORMAT_ARGB32, imgSize.width(), imgSize.height(), imgSize.width() * 4);
    cairo_t*         cairo        = cairo_create(cairoSurface);

    for(int i = 0; i < loopCount ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(cm == CompositionMode::SourceOver)
            cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 175.0f / 255.0f);
        else
            cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 1.0f);

        cairo_move_to(cairo,  50,  50);
        cairo_line_to(cairo, 250, 100);
        cairo_line_to(cairo, 450, 250);
        cairo_line_to(cairo, 350, 350);
        cairo_line_to(cairo, 150, 100);
        cairo_fill (cairo);

        cairo_move_to(cairo, 250,  50);
        cairo_line_to(cairo, 450, 100);
        cairo_line_to(cairo, 650, 250);
        cairo_line_to(cairo, 550, 350);
        cairo_line_to(cairo, 350, 100);
        cairo_fill (cairo);

        sum += clock.stop().toUSecs();

        CAIRO_BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    cairo_destroy (cairo);
    cairo_surface_destroy (cairoSurface);

    sum /= loopCount;
    return sum;
}

static void cairoBenchmark(CompositionMode cm)
{
    double time1;

    std::clog << "                                                   (Time)" << std::endl;
    std::clog << "                                                   ------" << std::endl;

    // Filled polygons
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = cairoBenchFillPolygon(BENCHMARK_LOOP_COUNT_LONG, cm);
        std::clog << "    Solid-filled    polygon      @ Cairo         = " << std::setw(6) << time1 << std::endl;
        std::clog << std::endl;
    }
}

#undef CAIRO_BENCHMARK_DISPLAY_RESULTING_IMAGE
