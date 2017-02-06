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
static size_t benchDrawText(CompositionMode cm)
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

    for(int i = 0; i < BENCHMARK_LOOP_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.drawText( PointF(100, 100), "Hello world!" );
        painter.drawText( PointF(100, 150), "Hello world!" );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= BENCHMARK_LOOP_COUNT;
    return sum;
}

template <typename PainterT>
static size_t benchDrawLine(CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 175) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    for(int i = 0; i < BENCHMARK_LOOP_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.drawLine( PointF( 10,  10), PointF(789, 110) );
        painter.drawLine( PointF(789, 489), PointF( 10, 589) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= BENCHMARK_LOOP_COUNT;
    return sum;
}

template <typename PainterT>
static size_t benchDrawRect(CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 175) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    for(int i = 0; i < BENCHMARK_LOOP_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.drawRect( RectF(PointF(100, 100), SizeF(200, 100)) );
        painter.drawRect( RectF(PointF(400, 100), SizeF(200, 100)) );

        painter.drawRect( RectF(PointF(150, 150), SizeF(200, 100)) );
        painter.drawRect( RectF(PointF(450, 150), SizeF(200, 100)) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= BENCHMARK_LOOP_COUNT;
    return sum;
}

template <typename PainterT>
static size_t benchDrawSolidFillRect(CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 175) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    for(int i = 0; i < BENCHMARK_LOOP_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.fillRect( RectF(PointF(100, 100), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(400, 100), SizeF(200, 100)) );

        painter.fillRect( RectF(PointF(150, 150), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(450, 150), SizeF(200, 100)) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= BENCHMARK_LOOP_COUNT;
    return sum;
}

template <typename PainterT>
static size_t benchDrawGradFillRect(CompositionMode cm)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Brush brush( Color::fromRgb8(255, 255, 255, 175) );
    painter.setBrush(brush);

    Pen pen( Color::fromRgb8(255, 255, 255, 175) );
    painter.setPen(pen);

    for(int i = 0; i < BENCHMARK_LOOP_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        painter.fillRect( RectF(PointF(100, 100), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(400, 100), SizeF(200, 100)) );

        painter.fillRect( RectF(PointF(150, 150), SizeF(200, 100)) );
        painter.fillRect( RectF(PointF(450, 150), SizeF(200, 100)) );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= BENCHMARK_LOOP_COUNT;
    return sum;
}

template <typename PainterT>
static size_t benchDrawSolidFillPolygon(CompositionMode cm, bool ss)
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

    for(int i = 0; i < BENCHMARK_LOOP_COUNT ; ++i) {
        Pt::System::Clock clock;
        clock.start();

        const PointF poly1[] = { PointF(50, 50), PointF(250, 100), PointF(450, 250), PointF(350, 350), PointF(150, 100) };
        if(ip2) ip2->fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]), ss);
        else    painter.fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]));

        const PointF poly2[] = { PointF(140, 260), PointF(210, 310), PointF(160, 340), PointF(110, 310) };
        if(ip2) ip2->fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]), ss);
        else    painter.fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]));

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= BENCHMARK_LOOP_COUNT;
    return sum;
}

static void doBenchMark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                              (Time) (Factor)" << std::endl;
    std::clog << "                                              ------ --------" << std::endl;

    if(BENCHMARK_TEXT) {
        time1 = benchDrawText<ImagePainter >(cm);
        time2 = benchDrawText<ImagePainter2>(cm);
        std::clog << "    Text                    @ ImagePainter     = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Text                    @ ImagePainter2    = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_LINE) {
        time1 = benchDrawLine<ImagePainter >(cm);
        time2 = benchDrawLine<ImagePainter2>(cm);
        std::clog << "    Line                    @ ImagePainter     = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Line                    @ ImagePainter2    = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RECTANGLE) {
        time1 = benchDrawRect<ImagePainter >(cm);
        time2 = benchDrawRect<ImagePainter2>(cm);
        std::clog << "    Rectangle               @ ImagePainter     = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Rectangle               @ ImagePainter2    = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_SOLID_FILLED_RECTANGLE) {
        time1 = benchDrawSolidFillRect<ImagePainter >(cm);
        time2 = benchDrawSolidFillRect<ImagePainter2>(cm);
        std::clog << "    Solid-filled Rectangle  @ ImagePainter     = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled Rectangle  @ ImagePainter2    = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_RECTANGLE) {
        time1 = benchDrawGradFillRect<ImagePainter >(cm);
        time2 = benchDrawGradFillRect<ImagePainter2>(cm);
        std::clog << "    Gradient-filled Rectangle  @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Gradient-filled Rectangle  @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawSolidFillPolygon<ImagePainter >(cm, false);
        time2 = benchDrawSolidFillPolygon<ImagePainter2>(cm, false);
        std::clog << "    Solid-filled polygon    @ ImagePainter     = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled polygon    @ ImagePainter2    = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawSolidFillPolygon<ImagePainter >(cm, true);
        time2 = benchDrawSolidFillPolygon<ImagePainter2>(cm, true);
        std::clog << "    Solid-filled polygon SS @ ImagePainter     = " << std::setw(6) << time1 << std::endl;
        std::clog << "    Solid-filled polygon SS @ ImagePainter2    = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
}
