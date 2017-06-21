template <bool WITH_RASTERISATION>
static void benchDrawPath_drawRow(
    ImagePainter2* ip2, Transform& transform, Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThinDot, const Pen& penThickSolid, const Pen& penThickDot,
    const Brush& brush1, const Brush& brush2, bool antiAliasingMode
)
{
    std::vector<PointF> pointsF;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThinSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    ++col;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThinDot);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    ++col;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThickSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    ++col;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThickDot);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    ++col;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setBrush(brush1);
        ip2->fillPolygon(pointsF.data(), pointsF.size());
    }
    ++col;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setBrush(brush2);
        ip2->fillPolygon(pointsF.data(), pointsF.size());
    }
    ++row;
    col = 0;
}

template <bool WITH_RASTERISATION>
static void benchDrawPath_drawCol(
    ImagePainter2* ip2, Transform& transform, Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThickSolid, bool antiAliasingMode
)
{
    std::vector<PointF> pointsF;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(150 + 120 * col, 70 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThinSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    ++row;

    transform.rotate(15);
    path.setTransform(transform);
    path.transform().translate(150 + 120 * col, 70 + 120 * row);
    pointsF.clear();
    path.generatePoints(pointsF, 1);
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThickSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    ++row;
}

// WEIRD BUG: with "-O2" this function will cause "Floating point exception"
//            when compiled using GCC version 4.8.5
//#if defined(PT_GFX_USE_X86_CPU)
//#pragma GCC push_options
//#pragma GCC optimize ("-O3")
//#endif

template <typename PainterT, bool WITH_RASTERISATION>
static size_t benchDrawPath(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, bool antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    PainterT painter(image);
    painter.setCompositionMode(cm);

    Pen penThinSolid ( Pen(Color::fromRgb8(255, 255, 255, 175), 1, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThinDot   ( Pen(Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThickSolid( Pen(Color::fromRgb8(255, 255, 255, 175), 6, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThickDot  ( Pen(Color::fromRgb8(255, 255, 255, 175), 6, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return 0;

    for(int i = 0; i < loopCount; ++i) {
        Transform transform;
        Path      path;

        Pt::int32_t row = 0, col = 0;

        Pt::System::Clock clock;
        clock.start();

        // Create a new path
        path.clear    ();
        path.moveTo   (  0, 50); // CCW
        path.lineTo   ( 50, 80);
        path.lineTo   (100, 50);
        path.lineTo   ( 30,  0);
        path.close  ();

        transform.translate(-50, -40);

        // First row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Second row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Create a new path
        path.clear            ();
        path.moveTo           (       120, 70); // CCW
        path.lineTo           (       100, 50);
        path.lineTo           (        75, 50);
        path.quadraticBezierTo( PointF(50, 0),  PointF(25, 50));
        path.lineTo           (         0, 50);
        path.lineTo           (       -20, 70);
        path.close          ();

        transform.reset();
        transform.translate(-50, -35);
        transform.scale(0.75f, 1.0f);

        // Third row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Create a new path
        path.clear            ();
        path.moveTo           (        120,  0); // CCW
        path.lineTo           (        100, 20);
        path.lineTo           (         75, 20);
        path.quadraticBezierTo(PointF(50, 50),  PointF(25, 20));
        path.lineTo           (          0, 20);
        path.lineTo           (        -20,  0);
        path.close          ();

        // Fourth row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Rightmost part of the image
        row = 0;
        col = 6;

        // Create a new path
        path.clear    ();
        path.moveTo   (120, 70    ); // CCW
        path.lineTo   (100, 50    );
        path.lineTo   ( 75, 50    );
        path.arcTo    ( PointF(25, 50), 50);
        path.lineTo   ( 0,  50    );
        path.lineTo   (-20, 70    );
        path.close  ();

        transform.reset();
        transform.translate(-50, -25);
        transform.scale(0.75f, 1.0f);

        benchDrawPath_drawCol<WITH_RASTERISATION>(ip2, transform, path, row, col, penThinSolid, penThickSolid, antiAliasingMode);

        // Create a new path
        path.clear    ();
        path.moveTo   (120,  0     ); // CCW
        path.lineTo   (100, 20     );
        path.lineTo   ( 75, 20     );
        path.arcTo    ( PointF(25, 20), -50);
        path.lineTo   ( 0,  20     );
        path.lineTo   (-20,  0     );
        path.close  ();

        transform.reset();
        transform.translate(-50, -25);
        transform.scale(0.75f, 1.0f);

        benchDrawPath_drawCol<WITH_RASTERISATION>(ip2, transform, path, row, col, penThinSolid, penThickSolid, antiAliasingMode);

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

static size_t benchDrawPathSimple(int loopCount, CompositionMode cm, bool antiAliasingMode)
{
    size_t sum = 0;

    Image image( ImageFormat::argb32(), BENCHMARK_IMAGE_SIZE );

    ImagePainter2 painter(image);
    painter.setCompositionMode(cm);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return 0;

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();
        
        Path path;
        path.moveTo (400, 200); // CCW
        path.cubicBezierTo( PointF(300, 150), PointF(150, 350), PointF(100, 500));
        path.close();

        ip2->setAntiAliasing(antiAliasingMode);
        ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
        ip2->drawPath( path );

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}

//#if defined(PT_GFX_USE_X86_CPU)
//#pragma GCC pop_options
//#endif
