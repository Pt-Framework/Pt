class membuf : public std::streambuf {
    public:
        inline membuf(char* begin, char* end)
        { this->setg(begin, begin, end); }
};

#define BENCHMARK_GD_DISPLAY_RESULTING_IMAGE                                            \
    if(BENCHMARK_GD_CHECK_RESULTING_IMAGE && !i) {                                      \
        int          size;                                                              \
        char*        data = (char*) gdImagePngPtr(gdImg, &size);                        \
        membuf       sbuf(data, data + size);                                           \
        std::istream is(&sbuf);                                                         \
        Image        imgRes;                                                            \
        PngReader    tbgrPng(is, imgRes);                                               \
        tbgrPng.get();                                                                  \
        gdFree((void*) data);                                                           \
        sdlPreviewRGB888Buffer( formatCaption("GD", cm, __FUNCTION__),                  \
                                imgRes.data(), imgRes.width(), imgRes.height(), false); \
    }

static size_t gdBenchFillPolygon(int loopCount, CompositionMode cm)
{
    size_t sum = 0;

    const Size& imgSize = BENCHMARK_IMAGE_SIZE;

    const gdImagePtr  gdImg = gdImageCreateTrueColor(imgSize.width(), imgSize.height());
    const Pt::int32_t white = gdTrueColorAlpha(255, 255, 255, 127 * 175 / 255);

    for(int i = 0; i < loopCount ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        gdImageAlphaBlending(gdImg, (cm == CompositionMode::SourceOver) ? 1 : 0);
        gdImageSetAntiAliased(gdImg, white);

        gdPoint poly1a[5]; // CCW
        poly1a[0].x = 150; poly1a[0].y = 100;
        poly1a[1].x = 350, poly1a[1].y = 350;
        poly1a[2].x = 450, poly1a[2].y = 250;
        poly1a[3].x = 250, poly1a[3].y = 100;
        poly1a[4].x =  50, poly1a[4].y =  50;
        gdImageFilledPolygon(gdImg, poly1a, 5, gdAntiAliased);

        gdPoint poly1b[5]; // CCW
        poly1b[0].x = 350; poly1b[0].y = 100;
        poly1b[1].x = 550, poly1b[1].y = 350;
        poly1b[2].x = 650, poly1b[2].y = 250;
        poly1b[3].x = 450, poly1b[3].y = 100;
        poly1b[4].x = 250, poly1b[4].y =  50;
        gdImageFilledPolygon(gdImg, poly1b, 5, gdAntiAliased);

        gdPoint poly2a[5]; // CCW
        poly2a[0].x = 110; poly2a[0].y = 310;
        poly2a[1].x = 160, poly2a[1].y = 340;
        poly2a[2].x = 210, poly2a[2].y = 310;
        poly2a[3].x = 140, poly2a[3].y = 260;
        gdImageFilledPolygon(gdImg, poly2a, 4, gdAntiAliased);

        gdPoint poly2b[5]; // CCW
        poly2b[0].x = 110; poly2b[0].y = 410;
        poly2b[1].x = 160, poly2b[1].y = 440;
        poly2b[2].x = 210, poly2b[2].y = 410;
        poly2b[3].x = 140, poly2b[3].y = 360;
        gdImageFilledPolygon(gdImg, poly2b, 4, gdAntiAliased);

        sum += clock.stop().toUSecs();

        BENCHMARK_GD_DISPLAY_RESULTING_IMAGE;
    }

    gdImageDestroy(gdImg);

    sum /= loopCount;
    return sum;
}

static void gdBenchmark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                                       (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Filled polygons
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = gdBenchFillPolygon                 (BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Solid-filled    polygon          @ GD            = " << std::setw(6) << time1 << std::endl;
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
}

#undef BENCHMARK_GD_DISPLAY_RESULTING_IMAGE
