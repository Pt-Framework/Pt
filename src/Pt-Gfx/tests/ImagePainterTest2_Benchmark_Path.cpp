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

    asm volatile("nop\n\tnop\n\tnop");
    for(int i = 0; i < loopCount; ++i) {
        AffineMatrix2D      matrix2d;
        Path2D              path2d;
        std::vector<PointF> pointsF;

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

        // Second row
        matrix2d.rotateAboutOrigin(15);
        matrix2d.push();
        matrix2d.translate(80 + 120 * col, 80 + 120 * row);
        pointsF.clear();
        path2d.generatePoints(pointsF, 0);
        matrix2d.transformPoints(pointsF.data(), pointsF.size());
        if(WITH_RASTERISATION) {
            ip2->setAntiAliasingMode(antiAliasingMode);
            ip2->setPen(penThinSolid);
            ip2->drawPolyline(pointsF.data(), pointsF.size(), true);
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
            ip2->drawPolyline(pointsF.data(), pointsF.size(), true);
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
            ip2->drawPolyline(pointsF.data(), pointsF.size(), true);
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
            ip2->drawPolyline(pointsF.data(), pointsF.size(), true);
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

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }
    asm volatile("nop\n\tnop\n\tnop");

    sum /= loopCount;
    return sum;
}
