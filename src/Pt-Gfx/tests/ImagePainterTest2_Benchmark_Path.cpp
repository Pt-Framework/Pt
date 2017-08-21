template <bool WITH_RASTERISATION, bool WITH_CLIPPING>
static void bmSetClipRegionAndDrawPath(const Image& image, ImagePainter2& ip2, const RectF& clipRect, const Path& path, const Pen* pen, const Brush* brush)
{
    // Set clip region as needed
    if(WITH_CLIPPING) ip2.setClip(clipRect);

    // Draw the path as needed
    if(WITH_RASTERISATION) {
        if(pen) {
            ip2.setPen(*pen);
            ip2.drawPath(path);
        }
        if(brush) {
            ip2.setBrush(*brush);
            ip2.fillPath(path);
        }
    }

    // Reset the clip region as needed
    if(WITH_CLIPPING) ip2.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );
}


template <typename PainterT, bool WITH_RASTERISATION, bool WITH_CLIPPING>
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

    ip2->setAntiAliasing(antiAliasingMode);

    for(int i = 0; i < loopCount; ++i) {
        Pt::System::Clock clock;
        clock.start();

        Path      path;
        Transform transform;

        path.moveTo( PointF( 0,  0) ); // Absolute size (100, 90)
        path.lineTo( PointF(40, 70) );
        path.lineTo( PointF(80, 89) );
        path.lineTo( PointF(99, 20) );
        path.lineTo( PointF( 0,  0) );
        path.close ();

        // Outline
        transform.translate(100,   50); // Absolute start position (100,  50)
        path.transform(transform);
        bmSetClipRegionAndDrawPath<WITH_RASTERISATION, WITH_CLIPPING>(image, *ip2, RectF( PointF(110, 60), SizeF(80, 70) ), path, &penThinSolid, 0);

        transform.translate(100,  -50); // Absolute start position (300,  50)
        path.transform(transform);
        bmSetClipRegionAndDrawPath<WITH_RASTERISATION, WITH_CLIPPING>(image, *ip2, RectF( PointF(310, 60), SizeF(80, 70) ), path, &penThinDot, 0);

        path.transform(transform);      // Absolute start position (500,  50)
        bmSetClipRegionAndDrawPath<WITH_RASTERISATION, WITH_CLIPPING>(image, *ip2, RectF( PointF(510, 60), SizeF(80, 70) ), path, &penThickSolid, 0);

        path.transform(transform);      // Absolute start position (700,  50)
        bmSetClipRegionAndDrawPath<WITH_RASTERISATION, WITH_CLIPPING>(image, *ip2, RectF( PointF(710, 60), SizeF(80, 70) ), path, &penThickDot, 0);

        // Filled
        transform.translate(-800, 200); // Absolute start position (100, 250)
        path.transform(transform);
        bmSetClipRegionAndDrawPath<WITH_RASTERISATION, WITH_CLIPPING>(image, *ip2, RectF( PointF(110, 260), SizeF(80, 70) ), path, 0, &brush1);

        transform.translate(800, -200); // Absolute start position (300, 250)
        path.transform(transform);
        bmSetClipRegionAndDrawPath<WITH_RASTERISATION, WITH_CLIPPING>(image, *ip2, RectF( PointF(310, 260), SizeF(80, 70) ), path, 0, &brush2);

        sum += clock.stop().toUSecs();

        BENCHMARK_DISPLAY_RESULTING_IMAGE;
    }

    sum /= loopCount;
    return sum ? sum : 1;
}
