#ifdef __GNUC__
#include <cxxabi.h>
#endif

static const std::string formatCaption(const Painter& painter, CompositionMode cm, const char* funcName)
{
    // Get the class name string
    std::string className = typeid(painter).name();
#ifdef __GNUC__
    int s;
    char* demangled = abi::__cxa_demangle(className.c_str(), 0, 0, &s);
    className = demangled;
    free(demangled);
#endif

    // Get the composition mode string
    std::string cmStr;
    switch(cm) {
        case CompositionMode::SourceCopy : cmStr = "SourceCopy"; break;
        case CompositionMode::SourceOver : cmStr = "SourceOver"; break;
        default                          : cmStr = "<unknown>";  break;
    }

    // Generate the information text
    std::stringstream ss;
    ss << funcName << "() - " << className << " [" << cmStr << "]";

    // Return the text
    return ss.str();
}

#define BENCHMARK_DISPLAY_RESULTING_IMAGE if(BENCHMARK_CHECK_RESULTING_IMAGE && !i) sdlPreviewRGB888Buffer(formatCaption(painter, cm, __FUNCTION__), image.data(), image.width(), image.height())

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
static size_t benchDrawLine(int loopCount, CompositionMode cm)
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
static size_t benchDrawFillPolygon(int loopCount, const Brush& brushH, const Brush& brushV, CompositionMode cm, bool ss)
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
        const PointF poly1[] = { PointF(50, 50), PointF(250, 100), PointF(450, 250), PointF(350, 350), PointF(150, 100) };
        if(ip2) ip2->fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]), ss);
        else    painter.fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]));

        painter.setBrush(brushV);
        const PointF poly2[] = { PointF(250, 50), PointF(450, 100), PointF(650, 250), PointF(550, 350), PointF(350, 100) };
        if(ip2) ip2->fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]), ss);
        else    painter.fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]));

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

static void doBenchMark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                                 (Time) (Factor)" << std::endl;
    std::clog << "                                                 ------ --------" << std::endl;

    // Create the brushes
    const Brush brushSolid    (Color::fromRgb8(255, 255, 255, 175));
    const Brush brushGradientH(Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Horizontal);
    const Brush brushGradientV(Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Vertical  );
    const Brush brushTextureT (textureWithTransBackground);
    const Brush brushTextureW (textureWithWhiteBackground);

    // Texts
    if(BENCHMARK_TEXT) {
        time1 = benchDrawText<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, cm);
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, cm);
        std::clog << "    Text                       @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Text                       @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Lines
    if(BENCHMARK_LINE) {
        time1 = benchDrawLine<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, cm);
        time2 = benchDrawLine<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, cm);
        std::clog << "    Line                       @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Line                       @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Rectangles
    if(BENCHMARK_RECTANGLE) {
        time1 = benchDrawRect<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, cm);
        time2 = benchDrawRect<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, cm);
        std::clog << "    Rectangle                  @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Rectangle                  @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled rectangles
    if(BENCHMARK_SOLID_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, brushSolid, brushSolid, cm);
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, brushSolid, brushSolid, cm);
        std::clog << "    Solid-filled    Rectangle  @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled    Rectangle  @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT_SHORT, brushGradientH, brushGradientV, cm);
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT_SHORT, brushGradientH, brushGradientV, cm);
        std::clog << "    Gradient-filled Rectangle  @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Gradient-filled Rectangle  @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, brushTextureT, brushTextureW, cm);
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, brushTextureT, brushTextureW, cm);
        std::clog << "    Texture-filled  Rectangle  @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Texture-filled  Rectangle  @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled polygons
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, brushSolid, brushSolid, cm, false);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, brushSolid, brushSolid, cm, false);
        std::clog << "    Solid-filled    polygon    @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled    polygon    @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT_SHORT, brushGradientH, brushGradientV, cm, false);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_SHORT, brushGradientH, brushGradientV, cm, false);
        std::clog << "    Gradient-filled polygon    @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Gradient-filled polygon    @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, brushTextureT, brushTextureW, cm, false);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, brushTextureT, brushTextureW, cm, false);
        std::clog << "    Texture-filled  polygon    @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Texture-filled  polygon    @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled polygons (with supersampling)
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, brushSolid, brushSolid, cm, true);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, brushSolid, brushSolid, cm, true);
        std::clog << "    Solid-filled    polygon SS @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled    polygon SS @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT_SHORT, brushGradientH, brushGradientV, cm, true);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_SHORT, brushGradientH, brushGradientV, cm, true);
        std::clog << "    Gradient-filled polygon SS @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Gradient-filled polygon SS @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter >(BENCHMARK_LOOP_COUNT_LONG, brushTextureT, brushTextureW, cm, true);
        time2 = benchDrawFillPolygon<ImagePainter2>(BENCHMARK_LOOP_COUNT_LONG, brushTextureT, brushTextureW, cm, true);
        std::clog << "    Texture-filled  polygon SS @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Texture-filled  polygon SS @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
}
