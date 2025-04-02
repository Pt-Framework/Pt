static void noclipDrawPath(ImagePainter2& ip2, const Path& path, const Pen* pen, const Brush* brush)
{
    if(pen) {
        ip2.setPen(*pen);
        ip2.drawPath(path);
    }

    if(brush) {
        ip2.setBrush(*brush);
        ip2.fillPath(path);
    }
}

static void setClipRegionAndDrawPath(const Image& image, ImagePainter2& ip2, const RectF& clipRect, const Path& path, const Pen* pen, const Brush* brush)
{
#define __USE_CLIP__

#ifdef __USE_CLIP__
    // Set clip region
    ip2.setClip(clipRect);

    // Draw the clip region
    static const Pen clipBorderA( Color::fromRgb8(255, 0, 0, 255) );
    ip2.setPen(clipBorderA);
    ip2.drawRect(clipRect);
#endif

    // Draw the path
    if(pen) {
        ip2.setPen(*pen);
        ip2.drawPath(path);
    }

    if(brush) {
        ip2.setBrush(*brush);
        ip2.fillPath(path);
    }

#ifdef __USE_CLIP__
    // Save and replace the composition mode
    const CompositionMode cm = ip2.compositionMode();
    ip2.setCompositionMode(CompositionMode::SourceOver);

    // Draw the clip region
    static const Pen clipBorderB( Color::fromRgb8(255, 255, 255, 63) );
    ip2.setPen(clipBorderB);
    ip2.drawRect(clipRect);

    // Restore the composition mode
    ip2.setCompositionMode(cm);

    // Reset the clip region
    ip2.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );
#undef __USE_CLIP__
#endif
}


static void testDrawPath(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    static const Pen penThinSolid ( Pen(Color::fromRgb8(255, 191, 127, 175), 1, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    static const Pen penThinDot   ( Pen(Color::fromRgb8(255, 191, 127, 175), 1, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );
    static const Pen penThickSolid( Pen(Color::fromRgb8(255, 191, 127, 175), 6, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    static const Pen penThickDot  ( Pen(Color::fromRgb8(255, 191, 127, 175), 6, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasing(true);

    Path      path;
    Transform transform;

    path.moveTo( PointF( 0,  0) ); // Absolute size (100, 90)
    path.lineTo( PointF(40, 70) );
    path.lineTo( PointF(80, 89) );
    path.lineTo( PointF(99, 20) );
    path.lineTo( PointF( 0,  0) );
    path.close ();

    // Outline - not clipped
    transform.translate(100, 50);   // Absolute start position (100,  50)
    path.transform(transform);
    noclipDrawPath(*ip2, path, &penThinSolid, 0);

    transform.translate(100, -50);  // Absolute start position (300,  50)
    path.transform(transform);
    noclipDrawPath(*ip2, path, &penThinDot, 0);

    path.transform(transform);      // Absolute start position (500,  50)
    noclipDrawPath(*ip2, path, &penThickSolid, 0);

    path.transform(transform);      // Absolute start position (700,  50)
    noclipDrawPath(*ip2, path, &penThickDot, 0);

    // Outline - clipped
    transform.translate(-800, 200); // Absolute start position (100, 250)
    path.transform(transform);
    setClipRegionAndDrawPath(image, *ip2, RectF( PointF(110, 260), SizeF(80, 70) ), path, &penThinSolid, 0);

    transform.translate(800, -200); // Absolute start position (300, 250)
    path.transform(transform);
    setClipRegionAndDrawPath(image, *ip2, RectF( PointF(310, 260), SizeF(80, 70) ), path, &penThinDot, 0);

    path.transform(transform);      // Absolute start position (500, 250)
    setClipRegionAndDrawPath(image, *ip2, RectF( PointF(510, 260), SizeF(80, 70) ), path, &penThickSolid, 0);

    path.transform(transform);      // Absolute start position (700, 250)
    setClipRegionAndDrawPath(image, *ip2, RectF( PointF(710, 260), SizeF(80, 70) ), path, &penThickDot, 0);

    // Filled - not clipped
    transform.translate(-800, 200); // Absolute start position (100, 450)
    path.transform(transform);
    noclipDrawPath(*ip2, path, 0, &brush1);

    transform.translate(800, -200); // Absolute start position (300, 450)
    path.transform(transform);
    noclipDrawPath(*ip2, path, 0, &brush2);

    // Filled - clipped
    path.transform(transform);      // Absolute start position (500, 450)
    setClipRegionAndDrawPath(image, *ip2, RectF( PointF(510, 460), SizeF(80, 70) ), path, 0, &brush1);

    path.transform(transform);      // Absolute start position (700, 450)
    setClipRegionAndDrawPath(image, *ip2, RectF( PointF(710, 460), SizeF(80, 70) ), path, 0, &brush2);

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}


/*
static void testDrawPath_drawRow(
    ImagePainter2* ip2, Transform& transform, Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThinDot, const Pen& penThickSolid, const Pen& penThickDot,
    const Brush& brush1, const Brush& brush2
)
{
    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path, false);
    ++col;

    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path, false);
    ++col;

    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path, false);
    ++col;

    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path, false);
    ++col;

    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path);
    ++col;

    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path);
    col = 0;
    ++row;
}

static void testDrawPath_drawCol(
    ImagePainter2* ip2, Transform& transform, Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThickSolid
)
{
    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path, false);
    ++col;

    transform.rotateDeg(15);
    path.setTransform(transform);
    path.transform().translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path, false);
    ++col;
}

static void testDrawPath(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    Pen penThinSolid ( Pen(Color::fromRgb8(255, 191, 127, 175), 1, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThinDot   ( Pen(Color::fromRgb8(255, 191, 127, 175), 1, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThickSolid( Pen(Color::fromRgb8(255, 191, 127, 175), 6, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThickDot  ( Pen(Color::fromRgb8(255, 191, 127, 175), 6, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Default);

    Pt::int32_t row = 0, col = 0;

    Transform transform;
    Path      path;

    //
    // Various shapes (top left part of the image)
    //

    // Create a new path
    path.clear    ();
    path.beginPath();
    path.moveTo   (  0, 50); // CCW
    path.lineTo   ( 50, 80);
    path.lineTo   (100, 50);
    path.lineTo   ( 30,  0);
    path.endPath  ();

    transform.translate(-50, -40);

    // First row
    testDrawPath_drawRow(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

    // Second row
    testDrawPath_drawRow(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

    // Create a new path
    path.clear            ();
    path.beginPath        ();
    path.moveTo           (       120, 70); // CCW
    path.lineTo           (       100, 50);
    path.lineTo           (        75, 50);
    path.quadraticBezierTo(50, 0,  25, 50);
    path.lineTo           (         0, 50);
    path.lineTo           (       -20, 70);
    path.endPath          ();

    transform.identity();
    transform.translate(-50, -35);
    transform.scale(0.75f, 1.0f);

    // Third row
    testDrawPath_drawRow(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

    // Create a new path
    path.clear            ();
    path.beginPath        ();
    path.moveTo           (        120,  0); // CCW
    path.lineTo           (        100, 20);
    path.lineTo           (         75, 20);
    path.quadraticBezierTo(50, 50,  25, 20);
    path.lineTo           (          0, 20);
    path.lineTo           (        -20,  0);
    path.endPath          ();

    // Fourth row
    testDrawPath_drawRow(ip2, transform, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

    //
    // Arcs (top right part of the image)
    //

    // Create a new path
    path.clear    ();
    path.beginPath();
    path.moveTo   (120, 70    ); // CCW
    path.lineTo   (100, 50    );
    path.lineTo   ( 75, 50    );
    path.arcTo    ( 25, 50, 50);
    path.lineTo   ( 0,  50    );
    path.lineTo   (-20, 70    );
    path.endPath  ();

    transform.identity();
    transform.translate(-50, -25);
    transform.scale(0.75f, 1.0f);

    row = 0;
    col = 6;

    testDrawPath_drawCol(ip2, transform, path, row, col, penThinSolid, penThickSolid);

    // Create a new path
    path.clear    ();
    path.beginPath();
    path.moveTo   (120,  0     ); // CCW
    path.lineTo   (100, 20     );
    path.lineTo   ( 75, 20     );
    path.arcTo    ( 25, 20, -50);
    path.lineTo   ( 0,  20     );
    path.lineTo   (-20,  0     );
    path.endPath  ();

    transform.identity();
    transform.translate(-50, -25);
    transform.scale(0.75f, 1.0f);

    row = 1;
    col = 6;

    testDrawPath_drawCol(ip2, transform, path, row, col, penThinSolid, penThickSolid);

    //
    // Cubic bezier curves (bottom left part of the image)
    //
    row = 3;
    col = 0;

    // Create a new path
    path.clear        ();
    path.beginPath    ();
    path.moveTo       (30, 50                ); // CCW
    path.cubicBezierTo(20, 35,  5, 55,  0, 70);
    path.endPath      ();
    path.beginPath    ();
    path.moveTo       (28, 28                );
    path.cubicBezierTo( 7, 40, 44, 52, 44,  8);
    path.endPath      ();

    transform.identity();
    transform.rotate(-25);
    transform.scale(2.5, 2.5);

#define DRAW_CB(PEN)                                           \
    do {                                                       \
        path.setTransform(transform);                          \
        path.transform().translate(90 + 160 * col, 140 * row); \
        ip2->setPen(PEN);                                      \
        ip2->drawPath(path, false);                            \
        ++col;                                                 \
    } while(false)
    DRAW_CB(penThinSolid );
    DRAW_CB(penThinDot   );
    DRAW_CB(penThickSolid);
    DRAW_CB(penThickDot  );
#undef DRAW_CB

    //
    // Demonstrating the effect of smoothness (bottom right part of the image)
    //
    row = 2;
    col = 6;

    // Create a new path
    path.clear    ();
    path.beginPath();
    path.moveTo   ( 0.0, 2.0     ); // CCW
    path.lineTo   ( 2.5, 2.0     );
    path.arcTo    ( 7.5, 2.0, 5.0);
    path.lineTo   (10.0, 2.0     );
    path.endPath  ();

    transform.identity();
    transform.translate(-5.0, -2.5);
    transform.scale(25, 25);

    path.setTransform(transform);
    path.transform().translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    //ip2->fillPath(path, 0.05);
    ip2->drawPath(path, false, 0.05f);
    ++row;

    path.setTransform(transform);
    path.transform().translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    //ip2->fillPath(path, 1);
    ip2->drawPath(path, false, 1);

    //ip2->setPen(Color::fromRgb8(255, 0, 0, 255));
    //ip2->drawEllipse( PointF (60 + 130 * col - 2.5 * 25, 150 * row -0.5 * 25 - 125), SizeF(5 * 25, 2 * 5 * 25) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

// ======================================================================================

static void testDrawPathClipping(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Default);

    Pt::int32_t row = 0, col = 0;

    std::vector<PointF> points;
    Transform           transform;
    Path                clipPath;
    Path                drawPath;

    // Create a new clip-region path
    clipPath.clear    ();
    clipPath.beginPath();
    clipPath.moveTo   (  0, 50); // CCW
    clipPath.lineTo   ( 50, 80);
    clipPath.lineTo   (100, 50);
    clipPath.lineTo   ( 30,  0);
    clipPath.endPath  ();

    clipPath.setTransform(transform);
    clipPath.transform().scale(2, 2);
    clipPath.transform().translate(50, 70);

    // Create a subject path
    drawPath.clear    ();
    drawPath.beginPath();
    drawPath.moveTo   (  0,  50   ); // CCW
    drawPath.arcTo    (100, 50, 50);
    drawPath.arcTo    (  0, 50, 50);
    drawPath.endPath  ();

    drawPath.setTransform(transform);
    drawPath.transform().scale(2, 2);

    // Draw the clip-region and subject polygons
    transform.translate(10 + 300 * col, 10 + 300 * row, true);

    drawPath.generatePoints(points);
    transform.transformPoints(points.data(), points.size());
    ip2->setBrush(brush1);
    ip2->fillPolygon(points.data(), points.size());
    points.clear();

    clipPath.generatePoints(points);
    transform.transformPoints(points.data(), points.size());
    ip2->setBrush(brush2);
    ip2->fillPolygon(points.data(), points.size());
    points.clear();

    ++col;

    // Perform clipping and draw the resulting polygon
    drawPath.setClipPath(clipPath);
    drawPath.setClipMode(Path::Intersection);
    drawPath.generatePoints(points);
    transform.translate(10 + 250 * col, 10 + 250 * row, true);
    transform.transformPoints(points.data(), points.size());
    ip2->setBrush(brush1);
    ip2->fillPolygon(points.data(), points.size());
    points.clear();
    ++col;

    drawPath.setClipMode(Path::Union);
    drawPath.generatePoints(points);
    transform.translate(10 + 250 * col, 10 + 250 * row, true);
    transform.transformPoints(points.data(), points.size());
    ip2->setBrush(brush1);
    ip2->fillPolygon(points.data(), points.size());
    points.clear();
    ++row;
    col = 1;

    drawPath.setClipMode(Path::Difference);
    drawPath.generatePoints(points);
    transform.translate(10 + 250 * col, 10 + 250 * row, true);
    transform.transformPoints(points.data(), points.size());
    ip2->setBrush(brush1);
    ip2->fillPolygon(points.data(), points.size());
    points.clear();
    ++col;

    drawPath.setClipMode(Path::Xor);
    drawPath.generatePoints(points);
    transform.translate(10 + 250 * col, 10 + 250 * row, true);
    transform.transformPoints(points.data(), points.size());
    ip2->setBrush(brush1);
    ip2->fillPolygon(points.data(), points.size());
    points.clear();

    // Testing window buttons' icons
    col = 0;
    row = 2;

    //ip2->setAntiAliasingMode(AntiAliasingMode::None);

    if(true) { // Maximize
        RectF geometry( PointF(10 + 250 * col + 20 * 0, 10 + 250 * row), SizeF(16, 16) );
        PointF tl = geometry.topLeft    () + PointF( 4, 4);
        PointF tr = geometry.topRight   () + PointF(-4, 4);
        PointF bl = geometry.bottomLeft () - PointF(-4, 4);
        PointF br = geometry.bottomRight() - PointF( 4, 4);
        ip2->setPen( Pen(Color(62258, 45874, 3276), 2, Pen::Solid, Pen::FlatCap) );
        ip2->drawLine(tl, tr);
        ip2->drawLine(bl, tr);
        ip2->drawLine(br, tr);
    }

    if(true) { // Minimize
        RectF geometry( PointF(10 + 250 * col + 20 * 1, 10 + 250 * row), SizeF(16, 16) );
        PointF tl = geometry.topLeft    () + PointF( 4, 4);
        PointF tr = geometry.topRight   () + PointF(-4, 4);
        PointF bl = geometry.bottomLeft () - PointF(-4, 4);
        PointF br = geometry.bottomRight() - PointF( 4, 4);
        ip2->setPen( Pen(Color(22937, 42597, 16383), 2, Pen::Solid, Pen::FlatCap) );
        ip2->drawLine(bl, br);
        ip2->drawLine(tl, bl);
        ip2->drawLine(tr, bl);
    }

    if(true) { // Close
        RectF geometry( PointF(10 + 250 * col + 20 * 2, 10 + 250 * row), SizeF(16, 16) );
        PointF tl = geometry.topLeft    () + PointF( 4, 4);
        PointF tr = geometry.topRight   () + PointF(-4, 4);
        PointF bl = geometry.bottomLeft () - PointF(-4, 4);
        PointF br = geometry.bottomRight() - PointF( 4, 4);
        ip2->setPen( Pen(Color(53738, 16383, 14417), 2, Pen::Solid, Pen::FlatCap) );
        ip2->drawLine(tl, br);
        ip2->drawLine(tr, bl);
    }

    col = 1;
    row = 2;

    if(true) { // Maximize
        RectF geometry( PointF(10 + 250 * col + 20 * 0, 10 + 250 * row), SizeF(10, 10) );
        std::vector<PointF> points(9);
        points[0] = geometry.topRight() + PointF( 0, 0);
        points[1] = geometry.topRight() + PointF(-9, 0);
        points[2] = geometry.topRight() + PointF(-9, 1);
        points[3] = geometry.topRight() + PointF(-3, 2);
        points[4] = geometry.topRight() + PointF(-9, 7);
        points[5] = geometry.topRight() + PointF(-7, 9);
        points[6] = geometry.topRight() + PointF(-2, 3);
        points[7] = geometry.topRight() + PointF(-1, 9);
        points[8] = geometry.topRight() + PointF( 0, 9);
        ip2->setBrush( Color(62258, 45874, 3276) );
        ip2->fillPolygon( points.data(), points.size() );
    }

    if(true) { // Minimize
        RectF geometry( PointF(10 + 250 * col + 20 * 1, 10 + 250 * row), SizeF(10, 10) );
        std::vector<PointF> points(9);
        points[0] = geometry.bottomLeft() + PointF(0,  0);
        points[1] = geometry.bottomLeft() + PointF(9,  0);
        points[2] = geometry.bottomLeft() + PointF(9, -1);
        points[3] = geometry.bottomLeft() + PointF(3, -2);
        points[4] = geometry.bottomLeft() + PointF(9, -7);
        points[5] = geometry.bottomLeft() + PointF(7, -9);
        points[6] = geometry.bottomLeft() + PointF(2, -3);
        points[7] = geometry.bottomLeft() + PointF(1, -9);
        points[8] = geometry.bottomLeft() + PointF(0, -9);
        ip2->setBrush( Color(22937, 42597, 16383) );
        ip2->fillPolygon( points.data(), points.size() );
    }

    if(true) { // Close - 1
        RectF geometry( PointF(10 + 250 * col + 20 * 2, 10 + 250 * row), SizeF(10, 10) );
        std::vector<PointF> points(16);
        points[ 0] = geometry.topRight() + PointF( -2,  0);
        points[ 1] = geometry.topRight() + PointF( -4,  2);
        points[ 2] = geometry.topRight() + PointF( -5,  2);
        points[ 3] = geometry.topRight() + PointF( -7,  0);
        points[ 4] = geometry.topRight() + PointF( -9,  2);
        points[ 5] = geometry.topRight() + PointF( -7,  4);
        points[ 6] = geometry.topRight() + PointF( -7,  5);
        points[ 7] = geometry.topRight() + PointF( -9,  7);
        points[ 8] = geometry.topRight() + PointF( -7,  9);
        points[ 9] = geometry.topRight() + PointF( -5,  7);
        points[10] = geometry.topRight() + PointF( -4,  7);
        points[11] = geometry.topRight() + PointF( -2,  9);
        points[12] = geometry.topRight() + PointF(  0,  7);
        points[13] = geometry.topRight() + PointF( -2,  5);
        points[14] = geometry.topRight() + PointF( -2,  4);
        points[15] = geometry.topRight() + PointF(  0,  2);
        ip2->setBrush( Color(53738, 16383, 14417) );
        ip2->fillPolygon( points.data(), points.size() );
    }

    if(true) { // Close - 2
        RectF geometry( PointF(10 + 250 * col + 20 * 3, 10 + 250 * row), SizeF(10, 10) );
        std::vector<PointF> points(12);
        points[ 0] = geometry.topRight() + PointF( -2  ,  0  );
        points[ 1] = geometry.topRight() + PointF( -4.5,  3.5);
        points[ 2] = geometry.topRight() + PointF( -7  ,  0  );
        points[ 3] = geometry.topRight() + PointF( -9  ,  2  );
        points[ 4] = geometry.topRight() + PointF( -5.5,  4.5);
        points[ 5] = geometry.topRight() + PointF( -9  ,  7  );
        points[ 6] = geometry.topRight() + PointF( -7  ,  9  );
        points[ 7] = geometry.topRight() + PointF( -4.5,  5.5);
        points[ 8] = geometry.topRight() + PointF( -2  ,  9  );
        points[ 9] = geometry.topRight() + PointF(  0  ,  7  );
        points[10] = geometry.topRight() + PointF( -3.5,  4.5);
        points[11] = geometry.topRight() + PointF(  0  ,  2  );
        ip2->setBrush( Color(53738, 16383, 14417) );
        ip2->fillPolygon( points.data(), points.size() );
    }

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
*/
