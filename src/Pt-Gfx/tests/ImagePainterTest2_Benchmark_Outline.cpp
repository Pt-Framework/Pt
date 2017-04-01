template <typename PainterT>
static size_t benchDrawText(int loopCount, bool rotated, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    painter.setFontDir( Pt::System::Path(FONT_DIR) );
    if(rotated) painter.setFont( Pt::Gfx::Font(FONT_SPEC_R) );
    else        painter.setFont( Pt::Gfx::Font(FONT_SPEC_N) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.drawText( Point(100, 100), "Hello world!" );
        painter.drawText( Point(100, 150), "Hello world!" );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawSolidLine(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.drawLine( Point( 10,  10), Point(789, 110) );
        painter.drawLine( Point(789, 489), Point( 10, 589) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawPatternedLine(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);

        painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dash )  );
        painter.drawLine( Point( 10,  10), Point(789, 110) );

        painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDash )  );
        painter.drawLine( Point(789, 489), Point( 10, 589) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawRect(int loopCount, CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.drawRect( Rect(Point(100, 100), Size(200, 100)) );
        painter.drawRect( Rect(Point(400, 100), Size(200, 100)) );

        painter.drawRect( Rect(Point(150, 150), Size(200, 100)) );
        painter.drawRect( Rect(Point(450, 150), Size(200, 100)) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawEllipse(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.drawEllipse( Point (30, 60), Size(120, 60) );

        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.drawEllipse( Point (230, 60), Size(60, 120) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawArc(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return 0;

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->drawArc( Point (30, 60), Size(120, 60), 30, 330, ArcMode::Open );

        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->drawArc( Point (230, 60), Size(60, 120), -150, 150, ArcMode::Open );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawQuadBezier(int loopCount, Pen::Style style, CompositionMode cm, AntiAliasingMode antiAliasingMode, size_t penSize = 1)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Pen( Color::fromRgb8(255, 255, 255, 175), penSize, style ) );
    painter.setPen(pen);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return 0;

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        ip2->setAntiAliasingMode(antiAliasingMode);

        const Point bezier1[] = { // CCW
            Point(300 + 100, 100),
            Point(265 + 100,  65),
            Point(200 + 100,  50)
        };
        ip2->drawQuadraticPolybezier( bezier1, sizeof(bezier1) / sizeof(bezier1[0]), false );

        const Point bezier2[] = { // CCW
            // Bottom left
            Point(400 - 350,  90 + 300),
            Point(400 - 350, 110 + 300),
            Point(420 - 350, 110 + 300),
            // Bottom middle
            Point(450 - 350, 110 + 300),
            // Bottom right
            Point(480 - 350, 110 + 300),
            Point(500 - 350, 110 + 300),
            Point(500 - 350,  90 + 300),
            // Center right
            Point(500 - 350,  70 + 300),
            // Top right
            Point(500 - 350,  50 + 300),
            Point(500 - 350,  30 + 300),
            Point(480 - 350,  30 + 300),
            // Top middle
            Point(450 - 350,  30 + 300),
            // Top left
            Point(420 - 350,  30 + 300),
            Point(400 - 350,  30 + 300),
            Point(400 - 350,  50 + 300),
            // Center left
            Point(400 - 350,  70 + 300)
        };
        ip2->drawQuadraticPolybezier( bezier2, sizeof(bezier2) / sizeof(bezier2[0]), true );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}
