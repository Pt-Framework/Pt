template <typename PainterT>
static size_t benchDrawSolidThickLineSimple(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    Pen penBCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::ButtCap,   Pen::BevelJoin);
    Pen penSCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::SquareCap, Pen::BevelJoin);
    Pen penRCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::RoundCap,  Pen::BevelJoin);
    Pen penRCapMJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::RoundCap,  Pen::MiterJoin);
    Pen penRCapRJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::RoundCap,  Pen::RoundJoin);

    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);

        painter.setPen(penBCapBJoin);
        painter.drawLine( Point(100, 100), Point(300, 200) );

        painter.setPen(penSCapBJoin);
        painter.drawLine( Point(100, 100 + 200), Point(300, 200 + 200) );

        painter.setPen(penRCapBJoin);
        painter.drawLine( Point(100, 100 + 400), Point(300, 200 + 400) );

        painter.setPen(penRCapBJoin);
        const Point poly1a[] = { // CCW
            Point(100 + 400, 100),
            Point(300 + 400, 200),
            Point(200 + 400, 300)
        };
        if(ip2) ip2->drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]), false );
        else painter.drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );

        painter.setPen(penRCapMJoin);
        const Point poly1b[] = { // CCW
            Point(100 + 400, 100 + 200),
            Point(300 + 400, 200 + 200),
            Point(200 + 400, 300 + 200)
        };
        if(ip2) ip2->drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]), false );
        else painter.drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );

        painter.setPen(penRCapRJoin);
        const Point poly1c[] = { // CCW
            Point(100 + 400, 100 + 400),
            Point(300 + 400, 200 + 400),
            Point(200 + 400, 300 + 400)
        };
        if(ip2) ip2->drawPolyline( poly1c, sizeof(poly1c) / sizeof(poly1c[0]), false );
        else painter.drawPolyline( poly1c, sizeof(poly1c) / sizeof(poly1c[0]) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawThickLine_impl(
    int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode,
    const Pen& penRCapBJoin,
    const Pen& penSCapBJoin,
    const Pen& penBCapBJoin,
    const Pen& penBCapMJoin,
    const Pen& penBCapRJoin
)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);

        painter.setPen(penRCapBJoin);
        painter.drawLine( Point( 20, 300 - 30), Point(100, 350 - 30) );
        painter.drawLine( Point( 20, 430 - 30), Point(100, 380 - 30) );

        painter.setPen(penSCapBJoin);
        painter.drawLine( Point(120, 300 - 30), Point(200, 350 - 30) );
        painter.drawLine( Point(120, 430 - 30), Point(200, 380 - 30) );

        painter.setPen(penBCapBJoin);
        painter.drawLine( Point(220, 300 - 30), Point(300, 350 - 30) );
        painter.drawLine( Point(220, 430 - 30), Point(300, 380 - 30) );

        painter.setPen(penBCapBJoin);
        const Point poly1a[] = { // CCW
            Point(670 + 10, 120),
            Point(700 + 10, 180),
            Point(800 + 10, 130),
            Point(650 + 10,  20)
        };
        if(ip2) ip2->drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]), false );
        else painter.drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );

        const Point poly1b[] = { // CCW
            Point(670 + 170, 120),
            Point(700 + 170, 180),
            Point(800 + 170, 130),
            Point(650 + 170,  20)
        };
        if(ip2) ip2->drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]), true );
        else painter.drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );

        painter.setPen(penBCapMJoin);
        const Point poly2a[] = { // CCW
            Point(670 + 10, 120 + 200),
            Point(700 + 10, 180 + 200),
            Point(800 + 10, 130 + 200),
            Point(650 + 10,  20 + 200)
        };
        if(ip2) ip2->drawPolyline( poly2a, sizeof(poly2a) / sizeof(poly2a[0]), false );
        else painter.drawPolyline( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );

        const Point poly2b[] = { // CCW
            Point(670 + 170, 120 + 200),
            Point(700 + 170, 180 + 200),
            Point(800 + 170, 130 + 200),
            Point(650 + 170,  20 + 200)
        };
        if(ip2) ip2->drawPolyline( poly2b, sizeof(poly2b) / sizeof(poly2b[0]), true );
        else painter.drawPolyline( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );

        painter.setPen(penBCapRJoin);
        const Point poly3a[] = { // CCW
            Point(670 + 10, 120 + 400),
            Point(700 + 10, 180 + 400),
            Point(800 + 10, 130 + 400),
            Point(650 + 10,  20 + 400)
        };
        if(ip2) ip2->drawPolyline( poly3a, sizeof(poly3a) / sizeof(poly3a[0]), false );
        else painter.drawPolyline( poly3a, sizeof(poly3a) / sizeof(poly3a[0]) );

        const Point poly3b[] = { // CCW
            Point(670 + 170, 120 + 400),
            Point(700 + 170, 180 + 400),
            Point(800 + 170, 130 + 400),
            Point(650 + 170,  20 + 400)
        };
        if(ip2) ip2->drawPolyline( poly3b, sizeof(poly3b) / sizeof(poly3b[0]), true );
        else painter.drawPolyline( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawSolidThickLine(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    Pen penRCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::RoundCap,  Pen::BevelJoin);
    Pen penSCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::SquareCap, Pen::BevelJoin);
    Pen penBCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::ButtCap,   Pen::BevelJoin);
    Pen penBCapMJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::ButtCap,   Pen::MiterJoin);
    Pen penBCapRJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::ButtCap,   Pen::RoundJoin);

    return benchDrawThickLine_impl<PainterT>(
        loopCount, cm, antiAliasingMode,
        penRCapBJoin,
        penSCapBJoin,
        penBCapBJoin,
        penBCapMJoin,
        penBCapRJoin
    );
}

template <typename PainterT>
static size_t benchDrawPatternedThickLine(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    Pen penRCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Dash, Pen::RoundCap,  Pen::BevelJoin);
    Pen penSCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Dash, Pen::SquareCap, Pen::BevelJoin);
    Pen penBCapBJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Dash, Pen::ButtCap,   Pen::BevelJoin);
    Pen penBCapMJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Dash, Pen::ButtCap,   Pen::MiterJoin);
    Pen penBCapRJoin(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Dash, Pen::ButtCap,   Pen::RoundJoin);

    return benchDrawThickLine_impl<PainterT>(
        loopCount, cm, antiAliasingMode,
        penRCapBJoin,
        penSCapBJoin,
        penBCapBJoin,
        penBCapMJoin,
        penBCapRJoin
    );
}

template <typename PainterT>
static size_t benchDrawThickQuadBezier(int loopCount, Pen::Style style, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{ return benchDrawQuadBezier<PainterT>(loopCount, style, cm, antiAliasingMode, 12); }
