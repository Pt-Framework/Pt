template <typename PainterT, bool WITH_RASTERISATION>
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

    AffineMatrix2D      matrix2d;
    Path2D              path2d;
    std::vector<PointF> pointsF;

    asm volatile("nop\n\tnop\n\tnop");
    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        path2d.clear    ();
        path2d.beginPath();
        path2d.moveTo   (  0, 50);
        path2d.lineTo   ( 50, 80);
        path2d.lineTo   (100, 50);
        path2d.lineTo   ( 30,  0);
        path2d.endPath  ();

        path2d.generatePoints(pointsF, 0);
        matrix2d.transformPoints(pointsF.data(), pointsF.size());
        if(WITH_RASTERISATION) {
            ip2->setAntiAliasingMode(antiAliasingMode);
            ip2->setBrush(brush1);
            ip2->fillPolygon(pointsF.data(), pointsF.size());
        }
        pointsF.clear();

        path2d.generatePoints(pointsF, 0);
        matrix2d.push();
        matrix2d.translate(100, 100);
        matrix2d.transformPoints(pointsF.data(), pointsF.size());
        matrix2d.pop();
        if(WITH_RASTERISATION) {
            ip2->setAntiAliasingMode(antiAliasingMode);
            ip2->setBrush(brush2);
            ip2->fillPolygon(pointsF.data(), pointsF.size());
        }
        pointsF.clear();

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }
    asm volatile("nop\n\tnop\n\tnop");

    sum /= loopCount;
    return sum;
}
