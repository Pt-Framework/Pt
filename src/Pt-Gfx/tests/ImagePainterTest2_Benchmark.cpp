//#define CHECK_RESULTING_IMAGE

#define BENCH_COUNT 250

template <typename PainterT>
static size_t benchDrawText(CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), Size(800, 600) );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 255) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 255) );
    painter.setPen(pen);

    painter.setFontDir( Pt::System::Path(FONT_DIR) );
    painter.setFont( Pt::Gfx::Font(FONT_SPEC) );

    for(int i = 0; i < BENCH_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.drawText( PointF(100, 100), "Hello world!" );
        painter.drawText( PointF(100, 150), "Hello world!" );

        sum += clock.stop().toUSecs();
#ifdef CHECK_RESULTING_IMAGE
        if(!i) sdlPreviewRGB888Buffer("benchDrawText", image.data(), image.width(), image.height());
#endif
    }

    sum /= BENCH_COUNT;
    return sum;
}

template <typename PainterT>
static size_t benchDrawLine(CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), Size(800, 600) );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 255) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 255) );
    painter.setPen(pen);

    for(int i = 0; i < BENCH_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.drawLine( PointF( 10,  10), PointF(789, 110) );
        painter.drawLine( PointF(789, 489), PointF( 10, 589) );

        sum += clock.stop().toUSecs();
#ifdef CHECK_RESULTING_IMAGE
        if(!i) sdlPreviewRGB888Buffer("benchDrawLine", image.data(), image.width(), image.height());
#endif
    }

    sum /= BENCH_COUNT;
    return sum;
}

template <typename PainterT>
static size_t benchDrawSolidFillPolygon(CompositionMode cm, int aaQuality = 0)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), Size(800, 600) );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 255) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 255) );
    painter.setPen(pen);

    painter.setAntiAliasingQuality(aaQuality);

    for(int i = 0; i < BENCH_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        const PointF poly1[] = { PointF(50, 50), PointF(250, 100), PointF(450, 250), PointF(350, 350), PointF(150, 100) };
        painter.fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]));

        const PointF poly2[] = { PointF(140, 260), PointF(210, 310), PointF(160, 340), PointF(110, 310) };
        painter.fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]));

        sum += clock.stop().toUSecs();
#ifdef CHECK_RESULTING_IMAGE
        if(!i) sdlPreviewRGB888Buffer("benchDrawSolidFillPolygon", image.data(), image.width(), image.height());
#endif
    }

    sum /= BENCH_COUNT;
    return sum;
}

static void doBenchMark(CompositionMode cm)
{
    double time1, time2, time3;

    std::clog << "                                               Time (Factor)" << std::endl;
    std::clog << "                                               ---- --------" << std::endl;

    time1 = benchDrawText<ImagePainter >(cm);
    time2 = benchDrawText<ImagePainter2>(cm);
    std::clog << "    Text                 @ ImagePainter      = " << std::setw(4) << time1 << std::endl;
    std::clog << "    Text                 @ ImagePainter2     = " << std::setw(4) << time2
              << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    std::clog << std::endl;

    time1 = benchDrawLine<ImagePainter >(cm);
    time2 = benchDrawLine<ImagePainter2>(cm);
    std::clog << "    Line                 @ ImagePainter      = " << std::setw(4) << time1 << std::endl;
    std::clog << "    Line                 @ ImagePainter2     = " << std::setw(4) << time2
              << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    std::clog << std::endl;

    time1 = benchDrawSolidFillPolygon<ImagePainter >(cm   );
    time2 = benchDrawSolidFillPolygon<ImagePainter2>(cm, 0);
    time3 = benchDrawSolidFillPolygon<ImagePainter2>(cm, 1);
    std::clog << "    Solid-filled polygon @ ImagePainter      = " << std::setw(4) << time1 << std::endl;
    std::clog << "    Solid-filled polygon @ ImagePainter2 (N) = " << std::setw(4) << time2
              << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    std::clog << "    Solid-filled polygon @ ImagePainter2 (P) = " << std::setw(4) << time3
              << " (" << std::setw(6) << std::setprecision(3) << (time3 / time1) << ")" << std::setprecision(0) << std::endl;
    std::clog << std::endl;
}
