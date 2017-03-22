template <typename PainterT>
static size_t benchDrawPath(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, AntiAliasingMode antiAliasingMode)
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
        ip2->setBrush(brush1);

        // ### TODO ###

        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setBrush(brush2);

        // ### TODO ###

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}
