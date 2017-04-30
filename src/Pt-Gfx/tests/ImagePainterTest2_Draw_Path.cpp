static void testDrawPath_drawRow(
    ImagePainter2* ip2, Transform& transform, const Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThinDot, const Pen& penThickSolid, const Pen& penThickDot,
    const Brush& brush1, const Brush& brush2
)
{
    TransformStack tstack;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path, transform, false);
    transform = tstack.pop();
    ++col;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path, transform, false);
    transform = tstack.pop();
    ++col;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path, transform, false);
    transform = tstack.pop();
    ++col;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path, transform, false);
    transform = tstack.pop();
    ++col;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path, transform);
    transform = tstack.pop();
    ++col;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path, transform);
    transform = tstack.pop();
    col = 0;
    ++row;
}

static void testDrawPath_drawCol(
    ImagePainter2* ip2, Transform& transform, const Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThickSolid
)
{
    TransformStack tstack;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path, transform, false);
    transform = tstack.pop();
    ++col;

    transform.rotate(15);
    tstack.push(transform);
    transform.translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path, transform, false);
    transform = tstack.pop();
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

    TransformStack tstack;
    Transform      transform;
    Path           path;

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

#define DRAW_CB(PEN)                                    \
    do {                                                \
        tstack.push(transform);                          \
        transform.translate(90 + 160 * col, 140 * row); \
        ip2->setPen(PEN);                               \
        ip2->drawPath(path, transform, false, 2);       \
        transform = tstack.pop();                                \
        ++col;                                          \
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

    tstack.push(transform);
    transform.translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    //ip2->fillPath(path, transform, 1);
    ip2->drawPath(path, transform, false, 1);
    transform = tstack.pop();
    ++row;

    tstack.push(transform);
    transform.translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    //ip2->fillPath(path, transform, 20);
    ip2->drawPath(path, transform, false, 20);
    transform = tstack.pop();

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

    TransformStack tstack;
    Transform      transform;
    Path           path;

    std::vector<PointF> cregPointsF;
    std::vector<PointF> subjPointsF;
    std::vector<PointF> clipPointsF;

    // Create a new clip-region path
    path.clear    ();
    path.beginPath();
    path.moveTo   (  0, 50); // CCW
    path.lineTo   ( 50, 80);
    path.lineTo   (100, 50);
    path.lineTo   ( 30,  0);
    path.endPath  ();

    cregPointsF.clear();
    path.generatePoints(cregPointsF, 1);
    tstack.push(transform);
    transform.scale(2, 2);
    transform.translate(50, 70);
    transform.transformPoints(cregPointsF.data(), cregPointsF.size());
    transform = tstack.pop();

    // Create a subject path
    path.clear    ();
    path.beginPath();
    path.moveTo   (  0,  50   ); // CCW
    path.arcTo    (100, 50, 50);
    path.arcTo    (  0, 50, 50);
    path.endPath  ();

    subjPointsF.clear();
    path.generatePoints(subjPointsF, 3);
    tstack.push(transform);
    transform.scale(2, 2);
    transform.transformPoints(subjPointsF.data(), subjPointsF.size());
    transform = tstack.pop();

    // Draw the clip-region and subject polygons
    tstack.push(transform);
    transform.translate(10 + 250 * col, 10 + 250 * row);
    transform.transformPoints(cregPointsF.data(), cregPointsF.size());
    transform.transformPoints(subjPointsF.data(), subjPointsF.size());
    transform = tstack.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(subjPointsF.data(), subjPointsF.size());
    ip2->setBrush(brush2);
    ip2->fillPolygon(cregPointsF.data(), cregPointsF.size());
    ++col;

    // Perform clipping and draw the resulting polygon
    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Intersection);
    tstack.push(transform);
    transform.translate(10 + 250 * col, 10 + 250 * row);
    transform.transformPoints(clipPointsF.data(), clipPointsF.size());
    transform = tstack.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());
    ++col;

    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Union);
    tstack.push(transform);
    transform.translate(10 + 250 * col, 10 + 250 * row);
    transform.transformPoints(clipPointsF.data(), clipPointsF.size());
    transform = tstack.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());
    ++row;
    col = 1;

    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Difference);
    tstack.push(transform);
    transform.translate(10 + 250 * col, 10 + 250 * row);
    transform.transformPoints(clipPointsF.data(), clipPointsF.size());
    transform = tstack.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());
    ++col;

    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Xor);
    tstack.push(transform);
    transform.translate(10 + 250 * col, 10 + 250 * row);
    transform.transformPoints(clipPointsF.data(), clipPointsF.size());
    transform = tstack.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());

    // Testing Hmi window buttons' icons
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
        points[ 1] = geometry.topRight() + PointF( -4.5,  3  );
        points[ 2] = geometry.topRight() + PointF( -7  ,  0  );
        points[ 3] = geometry.topRight() + PointF( -9  ,  2  );
        points[ 4] = geometry.topRight() + PointF( -6  ,  4.5);
        points[ 5] = geometry.topRight() + PointF( -9  ,  7  );
        points[ 6] = geometry.topRight() + PointF( -7  ,  9  );
        points[ 7] = geometry.topRight() + PointF( -4.5,  6  );
        points[ 8] = geometry.topRight() + PointF( -2  ,  9  );
        points[ 9] = geometry.topRight() + PointF(  0  ,  7  );
        points[10] = geometry.topRight() + PointF( -3  ,  4.5);
        points[11] = geometry.topRight() + PointF(  0  ,  2  );
        ip2->setBrush( Color(53738, 16383, 14417) );
        ip2->fillPolygon( points.data(), points.size() );
    }

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
