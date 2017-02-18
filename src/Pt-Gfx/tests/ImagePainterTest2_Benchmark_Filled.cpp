template <typename PainterT>
static size_t benchDrawFillRect(int loopCount, const Brush& brushH, const Brush& brushV, CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    for(int i = 0; i < loopCount ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.setBrush(brushH);
        painter.fillRect( RectF(PointF(100, 100), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(400, 100), SizeF(200, 100)) );

        painter.setBrush(brushV);
        painter.fillRect( RectF(PointF(150, 150), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(450, 150), SizeF(200, 100)) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawFillPolygon(int loopCount, const Brush& brushH, const Brush& brushV, CompositionMode cm, AntiAliasingMode antiAliasingMode)
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

        painter.setBrush(brushH);
        const PointF poly1a[] = { // CCW
            PointF(150, 100),
            PointF(350, 350),
            PointF(450, 250),
            PointF(250, 100),
            PointF( 50,  50)
        };
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillPolygon(poly1a, sizeof(poly1a) / sizeof(poly1a[0]));

        painter.setBrush(brushV);
        const PointF poly1b[] = { // CCW
            PointF(350, 100),
            PointF(550, 350),
            PointF(650, 250),
            PointF(450, 100),
            PointF(250,  50)
        };
        painter.fillPolygon(poly1b, sizeof(poly1b) / sizeof(poly1b[0]));

        const PointF poly2a[] = { // CCW
            PointF(110, 310),
            PointF(160, 340),
            PointF(210, 310),
            PointF(140, 260)
        };
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillPolygon(poly2a, sizeof(poly2a) / sizeof(poly2a[0]));

        const PointF poly2b[] = { // CCW
            PointF(110, 410),
            PointF(160, 440),
            PointF(210, 410),
            PointF(140, 360)
        };
        painter.fillPolygon(poly2b, sizeof(poly2b) / sizeof(poly2b[0]));

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

template <typename PainterT>
static size_t benchDrawFillEllipse(int loopCount, const Brush& brushH, const Brush& brushV, CompositionMode cm, AntiAliasingMode antiAliasingMode)
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

        painter.setBrush(brushH);
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillEllipse( PointF (30, 60), SizeF(120, 60) );

        painter.setBrush(brushV);
        if(ip2) ip2->setAntiAliasingMode(antiAliasingMode);
        painter.fillEllipse( PointF (230, 60), SizeF(60, 120) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

