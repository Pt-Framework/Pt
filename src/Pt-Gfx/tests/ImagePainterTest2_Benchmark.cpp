//#define CHECK_RESULTING_IMAGE

#define BENCH_COUNT 250

template <typename PainterT>
size_t benchDrawText(const char* info)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), Size(800, 600) );

    PainterT painter(image);
    painter.setCompositionMode(CompositionMode::SourceOver);

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
    std::clog << info << sum << std::endl;
    return sum;
}

template <typename PainterT>
size_t benchDrawLine(const char* info)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), Size(800, 600) );

    PainterT painter(image);
    painter.setCompositionMode(CompositionMode::SourceOver);

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
    std::clog << info << sum << std::endl;
    return sum;
}

template <typename PainterT>
size_t benchDrawSolidFillPolygon(const char* info)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), Size(800, 600) );

    PainterT painter(image);
    painter.setCompositionMode(CompositionMode::SourceOver);

    Brush brush( Color::fromRgb8(255, 255, 255, 255) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 255) );
    painter.setPen(pen);

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
    std::clog << info << sum << std::endl;
    return sum;
}
