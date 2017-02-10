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

    gdImagePtr  gdImg = gdImageCreateTrueColor(imgSize.width(), imgSize.height());
    Pt::int32_t white = gdTrueColorAlpha(255, 255, 255, 127 * 175 / 255);

    for(int i = 0; i < loopCount ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        gdImageAlphaBlending(gdImg, (cm == CompositionMode::SourceOver) ? 1 : 0);
        gdImageFilledRectangle(gdImg, 0, 0, 25, 25, white);

        /*
        vectors[0].code = ART_MOVETO; vectors[0].x = 150; vectors[0].y = 100; // CCW
        vectors[1].code = ART_LINETO; vectors[1].x = 350; vectors[1].y = 350;
        vectors[2].code = ART_LINETO; vectors[2].x = 450; vectors[2].y = 250;
        vectors[3].code = ART_LINETO; vectors[3].x = 250; vectors[3].y = 100;
        vectors[4].code = ART_LINETO; vectors[4].x =  50; vectors[4].y =  50;
        vectors[5].code = ART_LINETO; vectors[5].x = 150; vectors[5].y = 100;
        vectors[6].code = ART_END;
        svp = art_svp_from_vpath(vectors);
        if(aa) art_rgb_svp_aa(svp, 0, 0, imgSize.width(), imgSize.height(), (cm == CompositionMode::SourceOver) ? colorT : colorS, 0, buffer, imgSize.width() * 4, 0);
        else   art_rgb_svp_alpha(svp, 0, 0, imgSize.width(), imgSize.height(), (cm == CompositionMode::SourceOver) ? colorT : colorS, buffer, imgSize.width() * 4, 0);
        art_free(svp);

        vectors[0].code = ART_MOVETO; vectors[0].x = 350; vectors[0].y = 100; // CCW
        vectors[1].code = ART_LINETO; vectors[1].x = 550; vectors[1].y = 350;
        vectors[2].code = ART_LINETO; vectors[2].x = 650; vectors[2].y = 250;
        vectors[3].code = ART_LINETO; vectors[3].x = 450; vectors[3].y = 100;
        vectors[4].code = ART_LINETO; vectors[4].x = 250; vectors[4].y =  50;
        vectors[5].code = ART_LINETO; vectors[5].x = 350; vectors[5].y = 100;
        vectors[6].code = ART_END;
        svp = art_svp_from_vpath(vectors);
        if(aa) art_rgb_svp_aa(svp, 0, 0, imgSize.width(), imgSize.height(), (cm == CompositionMode::SourceOver) ? colorT : colorS, 0, buffer, imgSize.width() * 4, 0);
        else   art_rgb_svp_alpha(svp, 0, 0, imgSize.width(), imgSize.height(), (cm == CompositionMode::SourceOver) ? colorT : colorS, buffer, imgSize.width() * 4, 0);
        art_free(svp);
        */

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

    std::clog << "                                                   (Time) (Factor)" << std::endl;
    std::clog << "                                                   ------ --------" << std::endl;

    // Filled polygons
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = gdBenchFillPolygon                 (BENCHMARK_LOOP_COUNT_LONG, cm);
        std::clog << "    Solid-filled    polygon      @ GD            = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, bmBrushSolid, bmBrushSolid, cm, false);
        std::clog << "    Solid-filled    polygon      @ ImagePainter  = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, bmBrushSolid, bmBrushSolid, cm, false);
        std::clog << "    Solid-filled    polygon NOAA @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, bmBrushSolid, bmBrushSolid, cm, true);
        std::clog << "    Solid-filled    polygon SSAA @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;

    }
}

#undef BENCHMARK_GD_DISPLAY_RESULTING_IMAGE
