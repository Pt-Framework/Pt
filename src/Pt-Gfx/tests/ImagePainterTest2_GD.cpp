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

        gdPoint poly1[5]; // CCW
        poly1[0].x = 150; poly1[0].y = 100;
        poly1[1].x = 350, poly1[1].y = 350;
        poly1[2].x = 450, poly1[2].y = 250;
        poly1[3].x = 250, poly1[3].y = 100;
        poly1[4].x =  50, poly1[4].y =  50;
        gdImageFilledPolygon(gdImg, poly1, 5, gdAntiAliased);

        gdPoint poly2[5]; // CCW
        poly2[0].x = 350; poly2[0].y = 100;
        poly2[1].x = 550, poly2[1].y = 350;
        poly2[2].x = 650, poly2[2].y = 250;
        poly2[3].x = 450, poly2[3].y = 100;
        poly2[4].x = 250, poly2[4].y =  50;
        gdImageFilledPolygon(gdImg, poly2, 5, gdAntiAliased);

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
