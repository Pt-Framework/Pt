#define BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE       \
    if(BENCHMARK_CAIRO_CHECK_RESULTING_IMAGE && !i)   \
        sdlPreviewRGB888Buffer(                       \
            formatCaption("Cairo", cm, __FUNCTION__), \
            buffer.data(),                            \
            imgSize.width(), imgSize.height(), false  \
        )

static volatile Pt::int32_t dummyRandomValue;

static size_t cairoBenchRandCallOverheadTimes1000(int loopCount)
{
    size_t sum = 0;

    // Reinitialize the random number generator here, so it will produce
    // the same sequence at the start of every benchmark
    srand(13579);

    dummyRandomValue = 0;

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        for(int j = 0; j < 1000; ++j) dummyRandomValue += (rand() % 21 - 10);

        sum += clock.stop().toUSecs();
    }

    sum /= loopCount;
    return sum;
}

static size_t cairoBenchThickLine(int loopCount, CompositionMode cm, bool useAntiAliasing)
{
    size_t sum = 0;

    const Size& imgSize = BENCHMARK_IMAGE_SIZE;

    std::vector<Pt::uint8_t> buffer(imgSize.width() * imgSize.height() * 4, 0);

    cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data ( &buffer[0], CAIRO_FORMAT_ARGB32, imgSize.width(), imgSize.height(), imgSize.width() * 4);
    cairo_t*         cairo        = cairo_create(cairoSurface);

    cairo_set_antialias(cairo, useAntiAliasing ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
    cairo_set_line_width(cairo, 12);

    for(int i = 0; i < loopCount; ++i) {
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

        cairo_set_line_cap(cairo, CAIRO_LINE_CAP_BUTT);
        cairo_move_to     (cairo, 100, 100);
        cairo_line_to     (cairo, 300, 200);
        cairo_stroke      (cairo);

        cairo_set_line_cap(cairo, CAIRO_LINE_CAP_SQUARE);
        cairo_move_to     (cairo, 100, 100 + 200);
        cairo_line_to     (cairo, 300, 200 + 200);
        cairo_stroke      (cairo);

        cairo_set_line_cap(cairo, CAIRO_LINE_CAP_ROUND);
        cairo_move_to     (cairo, 100, 100 + 400);
        cairo_line_to     (cairo, 300, 200 + 400);
        cairo_stroke      (cairo);

        cairo_set_line_join(cairo, CAIRO_LINE_JOIN_BEVEL);
        cairo_move_to      (cairo, 100 + 400, 100);
        cairo_line_to      (cairo, 300 + 400, 200);
        cairo_line_to      (cairo, 200 + 400, 300);
        cairo_stroke       (cairo);

        cairo_set_line_join(cairo, CAIRO_LINE_JOIN_MITER);
        cairo_move_to      (cairo, 100 + 400, 100 + 200);
        cairo_line_to      (cairo, 300 + 400, 200 + 200);
        cairo_line_to      (cairo, 200 + 400, 300 + 200);
        cairo_stroke       (cairo);

        cairo_set_line_join(cairo, CAIRO_LINE_JOIN_ROUND);
        cairo_move_to      (cairo, 100 + 400, 100 + 400);
        cairo_line_to      (cairo, 300 + 400, 200 + 400);
        cairo_line_to      (cairo, 200 + 400, 300 + 400);
        cairo_stroke       (cairo);

        sum += clock.stop().toUSecs();

        BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE;
    }

    cairo_destroy (cairo);
    cairo_surface_destroy (cairoSurface);

    sum /= loopCount;
    return sum;
}

template <bool USE_RANDOM>
static size_t cairoBenchFillPolygon(int loopCount, CompositionMode cm, bool useAntiAliasing)
{
    size_t sum = 0;

    const Size& imgSize = BENCHMARK_IMAGE_SIZE;

    std::vector<Pt::uint8_t> buffer(imgSize.width() * imgSize.height() * 4, 0);

    cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data ( &buffer[0], CAIRO_FORMAT_ARGB32, imgSize.width(), imgSize.height(), imgSize.width() * 4);
    cairo_t*         cairo        = cairo_create(cairoSurface);

    cairo_set_antialias(cairo, useAntiAliasing ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);

    // Reinitialize the random number generator here, so it will produce
    // the same sequence at the start of every benchmark
    if(USE_RANDOM) srand(13579);

    for(int i = 0; i < loopCount; ++i) {
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

#define RV (USE_RANDOM ? (rand() % 21 - 10) : 0)

        cairo_reset_clip   (cairo);
        cairo_new_path     (cairo);
        cairo_move_to      (cairo, 150 + RV, 100 + RV); // CCW
        cairo_line_to      (cairo, 350 + RV, 350 + RV);
        cairo_line_to      (cairo, 450 + RV, 250 + RV);
        cairo_line_to      (cairo, 250 + RV, 100 + RV);
        cairo_line_to      (cairo,  50 + RV,  50 + RV);
        cairo_close_path   (cairo);
        cairo_clip_preserve(cairo);
        cairo_fill         (cairo);

        cairo_reset_clip   (cairo);
        cairo_new_path     (cairo);
        cairo_move_to      (cairo, 350 + RV, 100 + RV); // CCW
        cairo_line_to      (cairo, 550 + RV, 350 + RV);
        cairo_line_to      (cairo, 650 + RV, 250 + RV);
        cairo_line_to      (cairo, 450 + RV, 100 + RV);
        cairo_line_to      (cairo, 250 + RV,  50 + RV);
        cairo_close_path   (cairo);
        cairo_clip_preserve(cairo);
        cairo_fill         (cairo);

        cairo_reset_clip   (cairo);
        cairo_new_path     (cairo);
        cairo_move_to      (cairo, 110 + RV, 310 + RV); // CCW
        cairo_line_to      (cairo, 160 + RV, 340 + RV);
        cairo_line_to      (cairo, 210 + RV, 310 + RV);
        cairo_line_to      (cairo, 140 + RV, 260 + RV);
        cairo_close_path   (cairo);
        cairo_clip_preserve(cairo);
        cairo_fill         (cairo);

        cairo_reset_clip   (cairo);
        cairo_new_path     (cairo);
        cairo_move_to      (cairo, 110 + RV, 410 + RV); // CCW
        cairo_line_to      (cairo, 160 + RV, 440 + RV);
        cairo_line_to      (cairo, 210 + RV, 410 + RV);
        cairo_line_to      (cairo, 140 + RV, 360 + RV);
        cairo_close_path   (cairo);
        cairo_clip_preserve(cairo);
        cairo_fill         (cairo);

#undef RV

        sum += clock.stop().toUSecs();

        BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE;
    }

    cairo_destroy (cairo);
    cairo_surface_destroy (cairoSurface);

    sum /= loopCount;
    return sum;
}

static size_t cairoBenchFillEllipse(int loopCount, CompositionMode cm, bool useAntiAliasing)
{
    size_t sum = 0;

    const Size& imgSize = BENCHMARK_IMAGE_SIZE;

    std::vector<Pt::uint8_t> buffer(imgSize.width() * imgSize.height() * 4, 0);

    cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data ( &buffer[0], CAIRO_FORMAT_ARGB32, imgSize.width(), imgSize.height(), imgSize.width() * 4);
    cairo_t*         cairo        = cairo_create(cairoSurface);

    cairo_set_antialias(cairo, useAntiAliasing ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);

    for(int i = 0; i < loopCount; ++i) {
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
        cairo_arc            (cairo, (30 + 120 / 2), (60 + 120 / 2 / 2) * 2, 120 / 2, 0, 2 * Pt::Pi);
        cairo_clip_preserve  (cairo);
        cairo_fill           (cairo);

        cairo_reset_clip     (cairo);
        cairo_new_path       (cairo);
        cairo_identity_matrix(cairo);
        cairo_scale          (cairo, 0.5, 1);
        cairo_arc            (cairo, (230 + 120 / 2 / 2) * 2, (60 + 120 / 2), 120 / 2, 0, 2 * Pt::Pi);
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

static size_t cairoBenchPath(int loopCount, CompositionMode cm, bool useAntiAliasing)
{
    size_t sum = 0;

    const Size& imgSize = BENCHMARK_IMAGE_SIZE;

    std::vector<Pt::uint8_t> buffer(imgSize.width() * imgSize.height() * 4, 0);

    cairo_surface_t* cairoSurface = cairo_image_surface_create_for_data ( &buffer[0], CAIRO_FORMAT_ARGB32, imgSize.width(), imgSize.height(), imgSize.width() * 4);
    cairo_t*         cairo        = cairo_create(cairoSurface);

    cairo_set_antialias(cairo, useAntiAliasing ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);

    for(int i = 0; i < loopCount; ++i) {
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

        cairo_new_path(cairo);
        cairo_move_to (cairo, 400, 200); // CCW
        cairo_curve_to(cairo, 300, 150, 150, 350, 100, 500);
        cairo_stroke  (cairo);

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

    // Thick lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_THICK_LINE) {
        time1 = cairoBenchThickLine(BENCHMARK_LOOP_COUNT, cm, true );
        std::clog << "    Solid thick line                 @ Cairo         = " << std::setw(6) << time1 << std::endl;
        time2 = cairoBenchThickLine(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Solid thick line                 @ Cairo - No AA = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawSolidThickLineSimple<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid thick line                 @ ImagePainter  = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawSolidThickLineSimple<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid thick line NOAA            @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawSolidThickLineSimple<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid thick line XWAA            @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawSolidThickLineSimple<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::LowMemory);
        std::clog << "    Solid thick line FSAA2x2         @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled polygons
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = cairoBenchFillPolygon<false>(BENCHMARK_LOOP_COUNT, cm, true );
        std::clog << "    Solid-filled    polygon          @ Cairo         = " << std::setw(6) << time1 << std::endl;
        time2 = cairoBenchFillPolygon<false>(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Solid-filled    polygon          @ Cairo - No AA = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    polygon          @ ImagePainter  = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid-filled    polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::LowMemory);
        std::clog << "    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled ellipses
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_ELLIPSE) {
        time1 = cairoBenchFillEllipse              (BENCHMARK_LOOP_COUNT, cm, true);
        std::clog << "    Solid-filled    ellipse          @ Cairo         = " << std::setw(6) << time1 << std::endl;
        time2 = cairoBenchFillEllipse              (BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Solid-filled    ellipse          @ Cairo - No AA = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    ellipse          @ ImagePainter  = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid-filled    ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Path
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATH) {
        time1 = cairoBenchPath(BENCHMARK_LOOP_COUNT, cm, true );
        std::clog << "    Path (cubic bezier)              @ Cairo         = " << std::setw(6) << time1 << std::endl;
        time2 = cairoBenchPath(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Path (cubic bezier)              @ Cairo - No AA = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawPathSimple(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Path (cubic bezier) NOAA         @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawPathSimple(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Standard);
        std::clog << "    Path (cubic bezier) XWAA         @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // --- With random factor ---

    std::clog << "    >>> +Random: Factor => Relative to Self Non-Random (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Filled polygons
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = cairoBenchRandCallOverheadTimes1000(BENCHMARK_LOOP_COUNT);
        std::clog << "    Overhead for calling the rand() function in libc = " << std::setw(6) << std::setprecision(3) << (time1 / 1000.0f) <<  std::setprecision(0) << std::endl;
        time1 = cairoBenchFillPolygon<false>(BENCHMARK_LOOP_COUNT, cm, true );
        time2 = cairoBenchFillPolygon<true >(BENCHMARK_LOOP_COUNT, cm, true );
        std::clog << "    Solid-filled  R-polygon          @ Cairo         = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time1 = cairoBenchFillPolygon<false>(BENCHMARK_LOOP_COUNT, cm, false);
        time2 = cairoBenchFillPolygon<true >(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Solid-filled  R-polygon          @ Cairo - No AA = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time1 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        time2 = benchDrawFillPolygon<ImagePainter , true >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled  R-polygon          @ ImagePainter  = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time1 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        time2 = benchDrawFillPolygon<ImagePainter2, true >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled  R-polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time1 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Standard);
        time2 = benchDrawFillPolygon<ImagePainter2, true >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid-filled  R-polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time1 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::LowMemory);
        time2 = benchDrawFillPolygon<ImagePainter2, true >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::LowMemory);
        std::clog << "    Solid-filled  R-polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    }
}

#undef BENCHMARK_CAIRO_DISPLAY_RESULTING_IMAGE
