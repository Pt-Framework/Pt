template <typename PainterT>
static Pt::int64_t benchDrawFillRect(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm)
{
    Pt::int64_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.setBrush(brush1);
        painter.fillRect( RectF(PointF(100, 100), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(400, 100), SizeF(200, 100)) );

        painter.setBrush(brush2);
        painter.fillRect( RectF(PointF(150, 150), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(450, 150), SizeF(200, 100)) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT, bool USE_RANDOM>
static Pt::int64_t benchDrawFillPolygon(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, bool antiAliasingMode)
{
    Pt::int64_t sum = 0;

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
        const PointF poly1a[] = { // CCW
            PointF(150 + RV, 100 + RV),
            PointF(350 + RV, 350 + RV),
            PointF(450 + RV, 250 + RV),
            PointF(250 + RV, 100 + RV),
            PointF( 50 + RV,  50 + RV)
        };
        if(ip2) ip2->setAntiAliasing(antiAliasingMode);
        painter.fillPolygon(poly1a, sizeof(poly1a) / sizeof(poly1a[0]));

        painter.setBrush(brush2);
        const PointF poly1b[] = { // CCW
            PointF(350 + RV, 100 + RV),
            PointF(550 + RV, 350 + RV),
            PointF(650 + RV, 250 + RV),
            PointF(450 + RV, 100 + RV),
            PointF(250 + RV,  50 + RV)
        };
        painter.fillPolygon(poly1b, sizeof(poly1b) / sizeof(poly1b[0]));

        const PointF poly2a[] = { // CCW
            PointF(110 + RV, 310 + RV),
            PointF(160 + RV, 340 + RV),
            PointF(210 + RV, 310 + RV),
            PointF(140 + RV, 260 + RV)
        };
        if(ip2) ip2->setAntiAliasing(antiAliasingMode);
        painter.fillPolygon(poly2a, sizeof(poly2a) / sizeof(poly2a[0]));

        const PointF poly2b[] = { // CCW
            PointF(110 + RV, 410 + RV),
            PointF(160 + RV, 440 + RV),
            PointF(210 + RV, 410 + RV),
            PointF(140 + RV, 360 + RV)
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
static Pt::int64_t benchDrawFillEllipse(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, bool antiAliasingMode)
{
    Pt::int64_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.setBrush(brush1);
        if(ip2) ip2->setAntiAliasing(antiAliasingMode);
        painter.fillEllipse( PointF (30, 60), SizeF(120, 60) );

        painter.setBrush(brush2);
        if(ip2) ip2->setAntiAliasing(antiAliasingMode);
        painter.fillEllipse( PointF (230, 60), SizeF(60, 120) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static Pt::int64_t benchDrawFillArc(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, bool antiAliasingMode)
{
    Pt::int64_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return 0;

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.setBrush(brush1);
        ip2->setAntiAliasing(antiAliasingMode);
        ip2->fillChord( PointF (30, 60), SizeF(120, 120), 30, 330 );
        ip2->fillPie( PointF (30, 60 + 200), SizeF(120, 120), 30, 330 );

        painter.setBrush(brush2);
        ip2->setAntiAliasing(antiAliasingMode);
        ip2->fillChord( PointF (30 + 200, 60), SizeF(120, 120), 30, 330 );
        ip2->fillPie( PointF (30 + 200, 60 + 200), SizeF(120, 120), 30, 330 );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

