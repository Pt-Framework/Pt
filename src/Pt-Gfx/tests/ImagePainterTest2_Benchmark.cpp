#define BENCHMARK_DISPLAY_RESULTING_IMAGE \
    if(BENCHMARK_CHECK_RESULTING_IMAGE && !i) sdlPreviewRGB888Buffer(formatCaption(painter, cm, __FUNCTION__), image.data(), image.width(), image.height(), false)

template <typename PainterT>
static size_t benchDrawText(int loopCount, CompositionMode cm)
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
static size_t benchDrawLine(int loopCount, CompositionMode cm, Pt::uint8_t antiAliasingLevel)
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

        if(ip2) ip2->setAntiAliasingLevel(antiAliasingLevel);
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
static size_t benchDrawFillPolygon(int loopCount, const Brush& brushH, const Brush& brushV, CompositionMode cm, Pt::uint8_t antiAliasingLevel)
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
        if(ip2) ip2->setAntiAliasingLevel(antiAliasingLevel);
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
        if(ip2) ip2->setAntiAliasingLevel(antiAliasingLevel);
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
static size_t benchDrawFillEllipse(int loopCount, const Brush& brushH, const Brush& brushV, CompositionMode cm, Pt::uint8_t antiAliasingLevel)
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
        if(ip2) ip2->setAntiAliasingLevel(antiAliasingLevel);
        painter.fillEllipse( PointF (30, 60), SizeF(120, 60) );

        painter.setBrush(brushV);
        if(ip2) ip2->setAntiAliasingLevel(antiAliasingLevel);
        painter.fillEllipse( PointF (230, 60), SizeF(60, 120) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

static void doBenchmark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                                       (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Texts
    if(BENCHMARK_TEXT) {
        time1 = benchDrawText<ImagePainter >(BENCHMARK_LOOP_COUNT, cm);
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Text                             @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Text                             @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Lines
    if(BENCHMARK_LINE) {
        time1 = benchDrawLine<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, 0);
        time2 = benchDrawLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, 0);
        std::clog << "    Line                             @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Line NOAA                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, 1);
        std::clog << "    Line XWAA                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Rectangles
    if(BENCHMARK_RECTANGLE) {
        time1 = benchDrawRect<ImagePainter >(BENCHMARK_LOOP_COUNT, cm);
        time2 = benchDrawRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Rectangle                        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Rectangle                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled rectangles
    if(BENCHMARK_SOLID_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm);
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm);
        std::clog << "    Solid-filled    Rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled    Rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm);
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm);
        std::clog << "    Gradient-filled Rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Gradient-filled Rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm);
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm);
        std::clog << "    Texture-filled  Rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Texture-filled  Rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled polygons
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        std::clog << "    Solid-filled    polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled    polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 1);
        std::clog << "    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 2);
        std::clog << "    Solid-filled    polygon SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 0);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 0);
        std::clog << "    Gradient-filled polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Gradient-filled polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 1);
        std::clog << "    Gradient-filled polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 2);
        std::clog << "    Gradient-filled polygon SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 0);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 0);
        std::clog << "    Texture-filled  polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Texture-filled  polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 1);
        std::clog << "    Texture-filled  polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 2);
        std::clog << "    Texture-filled  polygon SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled ellpise
    if(BENCHMARK_SOLID_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 0);
        std::clog << "    Solid-filled    ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled    ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 1);
        std::clog << "    Solid-filled    ellipse FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, 2);
        std::clog << "    Solid-filled    ellipse SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 0);
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 0);
        std::clog << "    Gradient-filled ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Gradient-filled ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 1);
        std::clog << "    Gradient-filled ellipse FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, 2);
        std::clog << "    Gradient-filled ellipse SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 0);
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 0);
        std::clog << "    Texture-filled  ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Texture-filled  ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 1);
        std::clog << "    Texture-filled  ellipse FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, 2);
        std::clog << "    Texture-filled  ellipse SSAA 4x4 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
}

#undef BENCHMARK_DISPLAY_RESULTING_IMAGE
