template <typename PainterT>
static size_t benchDrawText(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 175) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    painter.setFontDir( Pt::System::Path(FONT_DIR) );
    painter.setFont( Pt::Gfx::Font(FONT_SPEC) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);

    for(int i = 0; i < loopCount ; ++i) {
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
static size_t benchDrawLine(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 175) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    for(int i = 0; i < loopCount ; ++i) {
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
static size_t benchDrawRect(int loopCount, CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 175) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    for(int i = 0; i < loopCount ; ++i) {
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

    for(int i = 0; i < loopCount ; ++i) {
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

    for(int i = 0; i < loopCount ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->drawArc( PointF (30, 60), SizeF(120, 60), 30, 330, ArcMode::Open );

        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->drawArc( PointF (230, 60), SizeF(60, 120), 210, 150, ArcMode::Open );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}
