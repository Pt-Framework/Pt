#define BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE \
    if(BENCHMARK_CAIRO_CHECK_RESULTING_IMAGE && !i) sdlPreviewRGB888Buffer(formatCaption("Cairo", cm, __FUNCTION__), buffer.data(), imgSize.width(), imgSize.height(), false)

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

        if(cm == CompositionMode::SourceOver) {
            cairo_set_operator(cairo, CAIRO_OPERATOR_OVER);
            cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 175.0f / 255.0f);
        }
        else {
            cairo_set_operator(cairo, CAIRO_OPERATOR_SOURCE);
            cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        // NOTE: * The calls to Cairo's clipping functions are not actually needed.
        //       * They are put here so that the benchmarking process will be a fair one,
        //         due to the fact that Pt-Gfx always perform clipping.

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

        cairo_reset_clip   (cairo);
        cairo_new_path     (cairo);
        cairo_move_to      (cairo, 110, 310); // CCW
        cairo_line_to      (cairo, 160, 340);
        cairo_line_to      (cairo, 210, 310);
        cairo_line_to      (cairo, 140, 260);
        cairo_close_path   (cairo);
        cairo_clip_preserve(cairo);
        cairo_fill         (cairo);

        cairo_reset_clip   (cairo);
        cairo_new_path     (cairo);
        cairo_move_to      (cairo, 110, 410); // CCW
        cairo_line_to      (cairo, 160, 440);
        cairo_line_to      (cairo, 210, 410);
        cairo_line_to      (cairo, 140, 360);
        cairo_close_path   (cairo);
        cairo_clip_preserve(cairo);
        cairo_fill         (cairo);

        sum += clock.stop().toUSecs();

        BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE;
    }

    cairo_destroy (cairo);
    cairo_surface_destroy (cairoSurface);

    sum /= loopCount;
    return sum;
}

static size_t cairoBenchFillEllipse(int loopCount, CompositionMode cm)
{
    size_t sum = 0;

    const Size& imgSize = BENCHMARK_IMAGE_SIZE;

    std::vector<Pt::uint8_t> buffer(imgSize.width() * imgSize.height() * 4, 0);

    cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data ( &buffer[0], CAIRO_FORMAT_ARGB32, imgSize.width(), imgSize.height(), imgSize.width() * 4);
    cairo_t*         cairo        = cairo_create(cairoSurface);

    for(int i = 0; i < loopCount ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(cm == CompositionMode::SourceOver) {
            cairo_set_operator(cairo, CAIRO_OPERATOR_OVER);
            cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 175.0f / 255.0f);
        }
        else {
            cairo_set_operator(cairo, CAIRO_OPERATOR_SOURCE);
            cairo_set_source_rgba(cairo, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        // NOTE: * The calls to Cairo's clipping functions are not actually needed.
        //       * They are put here so that the benchmarking process will be a fair one,
        //         due to the fact that Pt-Gfx always perform clipping.

        cairo_reset_clip     (cairo);
        cairo_new_path       (cairo);
        cairo_identity_matrix(cairo);
        cairo_scale          (cairo, 1, 0.5);
        cairo_arc            (cairo, (30 + 100 / 2), (50 + 100 / 2 / 2) * 2, 100 / 2, 0, 2 * Pt::Pi);
        cairo_clip_preserve  (cairo);
        cairo_fill           (cairo);

        cairo_reset_clip     (cairo);
        cairo_new_path       (cairo);
        cairo_identity_matrix(cairo);
        cairo_scale          (cairo, 0.5, 1);
        cairo_arc            (cairo, (230 + 100 / 2 / 2) * 2, (50 + 100 / 2), 100 / 2, 0, 2 * Pt::Pi);
        cairo_clip_preserve  (cairo);
        cairo_fill           (cairo);

        sum += clock.stop().toUSecs();

        BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE;
    }

    cairo_destroy (cairo);
    cairo_surface_destroy (cairoSurface);

    sum /= loopCount;
    return sum;
}

static void cairoBenchmark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                                       (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Filled polygons
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = cairoBenchFillPolygon              (BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Solid-filled    polygon          @ Cairo         = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        std::clog << "    Solid-filled    polygon          @ ImagePainter  = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        std::clog << "    Solid-filled    polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 1);
        std::clog << "    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 2);
        std::clog << "    Solid-filled    polygon SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled ellipses
    if(BENCHMARK_SOLID_FILLED_ELLIPSE) {
        time1 = cairoBenchFillEllipse              (BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Solid-filled    ellipse          @ Cairo         = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        std::clog << "    Solid-filled    ellipse          @ ImagePainter  = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        std::clog << "    Solid-filled    ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 1);
        std::clog << "    Solid-filled    ellipse FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 2);
        std::clog << "    Solid-filled    ellipse SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
}

#undef BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE
