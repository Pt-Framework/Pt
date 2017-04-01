template <typename PainterT>
static size_t benchDrawFillRect(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.setBrush(brush1);
        painter.fillRect( Rect(Point(100, 100), Size(200, 100)) );
        painter.fillRect( Rect(Point(400, 100), Size(200, 100)) );

        painter.setBrush(brush2);
        painter.fillRect( Rect(Point(150, 150), Size(200, 100)) );
        painter.fillRect( Rect(Point(450, 150), Size(200, 100)) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT, bool USE_RANDOM>
static size_t benchDrawFillPolygon(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    // Reinitialize the random number generator here, so it will produce
    // the same sequence at the start of every benchmark
    if(USE_RANDOM) srand(13579);

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

#define RV (USE_RANDOM ? (rand() % 21 - 10) : 0)

        painter.setBrush(brush1);
        const Point poly1a[] = { // CCW
            Point(150 + RV, 100 + RV),
            Point(350 + RV, 350 + RV),
            Point(450 + RV, 250 + RV),
            Point(250 + RV, 100 + RV),
            Point( 50 + RV,  50 + RV)
        };
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillPolygon(poly1a, sizeof(poly1a) / sizeof(poly1a[0]));

        painter.setBrush(brush2);
        const Point poly1b[] = { // CCW
            Point(350 + RV, 100 + RV),
            Point(550 + RV, 350 + RV),
            Point(650 + RV, 250 + RV),
            Point(450 + RV, 100 + RV),
            Point(250 + RV,  50 + RV)
        };
        painter.fillPolygon(poly1b, sizeof(poly1b) / sizeof(poly1b[0]));

        const Point poly2a[] = { // CCW
            Point(110 + RV, 310 + RV),
            Point(160 + RV, 340 + RV),
            Point(210 + RV, 310 + RV),
            Point(140 + RV, 260 + RV)
        };
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillPolygon(poly2a, sizeof(poly2a) / sizeof(poly2a[0]));

        const Point poly2b[] = { // CCW
            Point(110 + RV, 410 + RV),
            Point(160 + RV, 440 + RV),
            Point(210 + RV, 410 + RV),
            Point(140 + RV, 360 + RV)
        };
        painter.fillPolygon(poly2b, sizeof(poly2b) / sizeof(poly2b[0]));

#undef RV

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawFillEllipse(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.setBrush(brush1);
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillEllipse( Point (30, 60), Size(120, 60) );

        painter.setBrush(brush2);
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillEllipse( Point (230, 60), Size(60, 120) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawFillArc(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return 0;

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.setBrush(brush1);
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->fillArc( Point (30, 60), Size(120, 120), 30, 330, ArcMode::Chord );
        ip2->fillArc( Point (30, 60 + 200), Size(120, 120), 30, 330, ArcMode::Pie );

        painter.setBrush(brush2);
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->fillArc( Point (30 + 200, 60), Size(120, 120), 30, 330, ArcMode::Chord );
        ip2->fillArc( Point (30 + 200, 60 + 200), Size(120, 120), 30, 330, ArcMode::Pie );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

