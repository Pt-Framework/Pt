template <bool WITH_RASTERISATION>
static void benchDrawPath_drawRow(
    ImagePainter2* ip2, AffineMatrix2D& matrix2d, const Path2D& path2d, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThinDot, const Pen& penThickSolid, const Pen& penThickDot,
    const Brush& brush1, const Brush& brush2, AntiAliasingMode antiAliasingMode
)
{
    std::vector<PointF> pointsF;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThinSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThinDot);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThickSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThickDot);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setBrush(brush1);
        ip2->fillPolygon(pointsF.data(), pointsF.size());
    }
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setBrush(brush2);
        ip2->fillPolygon(pointsF.data(), pointsF.size());
    }
    matrix2d.pop();
    col = 0;
    ++row;
}

template <bool WITH_RASTERISATION>
static void benchDrawPath_drawCol(
    ImagePainter2* ip2, AffineMatrix2D& matrix2d, const Path2D& path2d, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThickSolid, AntiAliasingMode antiAliasingMode
)
{
    std::vector<PointF> pointsF;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(150 + 120 * col, 70 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThinSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    matrix2d.pop();
    ++row;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(150 + 120 * col, 70 + 120 * row);
    pointsF.clear();
    path2d.generatePoints(pointsF, 0);
    matrix2d.transformPoints(pointsF.data(), pointsF.size());
    if(WITH_RASTERISATION) {
        ip2->setAntiAliasingMode(antiAliasingMode);
        ip2->setPen(penThickSolid);
        ip2->drawPolyline(pointsF.data(), pointsF.size(), false);
    }
    matrix2d.pop();
    ++row;
}

// WEIRD BUG: with "-O2" this function will cause "Floating point exception"
#pragma GCC push_options
#pragma GCC optimize ("-O3")
template <typename PainterT, bool WITH_RASTERISATION>
static size_t benchDrawPath(int loopCount, const Brush& brush1, const Brush& brush2, CompositionMode cm, AntiAliasingMode antiAliasingMode)
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
        AffineMatrix2D matrix2d;
        Path2D         path2d;

        Pt::int32_t row = 0, col = 0;

        Pt::System::Clock clock;
        clock.start();

        // Create a new path
        path2d.clear    ();
        path2d.beginPath();
        path2d.moveTo   (  0, 50); // CCW
        path2d.lineTo   ( 50, 80);
        path2d.lineTo   (100, 50);
        path2d.lineTo   ( 30,  0);
        path2d.endPath  ();

        matrix2d.translate(-50, -40);

        // First row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Second row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Create a new path
        path2d.clear            ();
        path2d.beginPath        ();
        path2d.moveTo           (       120, 70); // CCW
        path2d.lineTo           (       100, 50);
        path2d.lineTo           (        75, 50);
        path2d.quadraticBezierTo(50, 0,  25, 50);
        path2d.lineTo           (         0, 50);
        path2d.lineTo           (       -20, 70);
        path2d.endPath          ();

        matrix2d.identity();
        matrix2d.translate(-50, -35);
        matrix2d.scaleAboutOrigin(0.75f, 1.0f);

        // Third row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Create a new path
        path2d.clear            ();
        path2d.beginPath        ();
        path2d.moveTo           (        120,  0); // CCW
        path2d.lineTo           (        100, 20);
        path2d.lineTo           (         75, 20);
        path2d.quadraticBezierTo(50, 50,  25, 20);
        path2d.lineTo           (          0, 20);
        path2d.lineTo           (        -20,  0);
        path2d.endPath          ();

        // Fourth row
        benchDrawPath_drawRow<WITH_RASTERISATION>(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2, antiAliasingMode);

        // Rightmost part of the image
        row = 0;
        col = 6;

        // Create a new path
        path2d.clear    ();
        path2d.beginPath();
        path2d.moveTo   (120, 70    ); // CCW
        path2d.lineTo   (100, 50    );
        path2d.lineTo   ( 75, 50    );
        path2d.arcTo    ( 25, 50, 50);
        path2d.lineTo   ( 0,  50    );
        path2d.lineTo   (-20, 70    );
        path2d.endPath  ();

        matrix2d.identity();
        matrix2d.translate(-50, -25);
        matrix2d.scaleAboutOrigin(0.75f, 1.0f);

        benchDrawPath_drawCol<WITH_RASTERISATION>(ip2, matrix2d, path2d, row, col, penThinSolid, penThickSolid, antiAliasingMode);

        // Create a new path
        path2d.clear    ();
        path2d.beginPath();
        path2d.moveTo   (120,  0     ); // CCW
        path2d.lineTo   (100, 20     );
        path2d.lineTo   ( 75, 20     );
        path2d.arcTo    ( 25, 20, -50);
        path2d.lineTo   ( 0,  20     );
        path2d.lineTo   (-20,  0     );
        path2d.endPath  ();

        matrix2d.identity();
        matrix2d.translate(-50, -25);
        matrix2d.scaleAboutOrigin(0.75f, 1.0f);

        benchDrawPath_drawCol<WITH_RASTERISATION>(ip2, matrix2d, path2d, row, col, penThinSolid, penThickSolid, antiAliasingMode);

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum;
}
#pragma GCC pop_options
