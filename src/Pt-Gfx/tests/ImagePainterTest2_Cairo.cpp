#define CAIRO_BENCHMARK_DISPLAY_RESULTING_IMAGE \
    if(BENCHMARK_CHECK_RESULTING_IMAGE && !i) sdlPreviewRGB888Buffer(formatCaption("Cairo", cm, __FUNCTION__), buffer.data(), imgSize.width(), imgSize.height(), false)

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

        /*
        painter.setBrush(brushH);
        const PointF poly1[] = { PointF(50, 50), PointF(250, 100), PointF(450, 250), PointF(350, 350), PointF(150, 100) };
        if(ip2) ip2->fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]), useAntiAliasing);
        else    painter.fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]));

        painter.setBrush(brushV);
        const PointF poly2[] = { PointF(250, 50), PointF(450, 100), PointF(650, 250), PointF(550, 350), PointF(350, 100) };
        if(ip2) ip2->fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]), useAntiAliasing);
        else    painter.fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]));
        */

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
