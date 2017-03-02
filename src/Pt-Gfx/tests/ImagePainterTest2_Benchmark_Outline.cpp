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

        painter.drawText( PointF(100, 100), "Hello world!" );
        painter.drawText( PointF(100, 150), "Hello world!" );

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
        painter.drawLine( PointF( 10,  10), PointF(789, 110) );
        painter.drawLine( PointF(789, 489), PointF( 10, 589) );

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
        painter.drawLine( PointF( 10,  10), PointF(789, 110) );

        painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDash )  );
        painter.drawLine( PointF(789, 489), PointF( 10, 589) );

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

        painter.drawRect( RectF(PointF(100, 100), SizeF(200, 100)) );
        painter.drawRect( RectF(PointF(400, 100), SizeF(200, 100)) );

        painter.drawRect( RectF(PointF(150, 150), SizeF(200, 100)) );
        painter.drawRect( RectF(PointF(450, 150), SizeF(200, 100)) );

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
        painter.drawEllipse( PointF (30, 60), SizeF(120, 60) );

        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.drawEllipse( PointF (230, 60), SizeF(60, 120) );

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
        ip2->drawArc( PointF (30, 60), SizeF(120, 60), 30, 330, ArcMode::Open );

        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->drawArc( PointF (230, 60), SizeF(60, 120), -150, 150, ArcMode::Open );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawBezier(int loopCount, Pen::Style style, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, style ) );
    painter.setPen(pen);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return 0;

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        ip2->setAntiAliasingMode(antiAliasingMode);

        const PointF bezier1[] = {
            PointF(200 + 100,  50),
            PointF(265 + 100,  65),
            PointF(300 + 100, 100)
        };
        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->drawPolybezier( bezier1, sizeof(bezier1) / sizeof(bezier1[0]), false );

        const PointF bezier2[] = {
            // Top left
            PointF(400 - 350,  50 + 300),
            PointF(400 - 350,  30 + 300),
            PointF(420 - 350,  30 + 300),
            // Top middle
            PointF(450 - 350,  30 + 300),
            // Top right
            PointF(480 - 350,  30 + 300),
            PointF(500 - 350,  30 + 300),
            PointF(500 - 350,  50 + 300),
            // Center right
            PointF(500 - 350,  70 + 300),
            // Bottom right
            PointF(500 - 350,  90 + 300),
            PointF(500 - 350, 110 + 300),
            PointF(480 - 350, 110 + 300),
            // Bottom middle
            PointF(450 - 350, 110 + 300),
            // Bottom left
            PointF(420 - 350, 110 + 300),
            PointF(400 - 350, 110 + 300),
            PointF(400 - 350,  90 + 300),
            // Center left
            PointF(400 - 350,  70 + 300)
        };
        ip2->drawPolybezier( bezier2, sizeof(bezier2) / sizeof(bezier2[0]), true );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}