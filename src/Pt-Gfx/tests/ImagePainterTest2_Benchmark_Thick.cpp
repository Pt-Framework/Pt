template <typename PainterT>
static size_t benchDrawSolidThickLine(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
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
        painter.setPen( Pen(Color::fromRgb8(255, 255, 255, 175), 12, Pen::Solid, Pen::FlatCap, Pen::BevelJoin) );
        painter.drawLine( PointF(50, 50), PointF(400, 150) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawPatternedThickLine(int loopCount, CompositionMode cm, AntiAliasingMode antiAliasingMode)
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

        painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDash )  );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}
