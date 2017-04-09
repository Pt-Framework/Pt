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

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

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

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

// ======================================================================================

static void testDrawPathClipping_drawCars(
    ImagePainter2* ip2, Path& path, Transform& transform,
    Pt::int32_t& row, Pt::int32_t& col, const Brush& brush
)
{
    TransformStack      tstack;
    std::vector<PointF> pointsF;

#if 1
    path.clear    ();
    path.beginPath();
    path.moveTo   (0, 0);
    path.putText  ("Pt-Q");
    path.endPath  ();
#else
    Pt::int32_t dx, dy;
    path.clear         ();
    path.beginPath     ();
    path.moveTo        (0, 0);
    path.putChar       ('P');
    path.getCharSpacing(dx, dy, 'P', 't');
    path.relMoveTo     (dx, dy);
    path.putChar       ('t');
    path.getCharSpacing(dx, dy, 't', '-');
    path.relMoveTo     (dx, dy);
    path.putChar       ('-');
    path.getCharSpacing(dx, dy, '-', 'Q');
    path.relMoveTo     (dx, dy);
    path.putChar       ('Q');
    path.endPath       ();
#endif

    path.generatePoints(pointsF, 1);
    tstack.push(transform);
    transform.translate(50 + 50 * col, 50 + 50 * row);
    transform.transformPoints(pointsF.data(), pointsF.size());
    transform = tstack.pop();
    ip2->setBrush(brush);
    ip2->fillPolygon(pointsF.data(), pointsF.size());

    ip2->setFont( path.font() );
    ip2->setPen( Color::fromRgb8(255, 255, 255, 255) );
    ip2->drawText( PointF(50 + 50 * col, 50 + 50 * row + 120), "Pt-Q" );

    ip2->setPen( Color::fromRgb8(255, 0, 0, 255) );
    ip2->drawLine( PointF(50 + 50 * col, 50 + 50 * row - 120), PointF(50 + 50 * col,       50 + 50 * row + 120) );
    ip2->drawLine( PointF(50 + 50 * col, 50 + 50 * row      ), PointF(50 + 50 * col + 180, 50 + 50 * row      ) );
    ip2->drawLine( PointF(50 + 50 * col, 50 + 50 * row + 120), PointF(50 + 50 * col + 180, 50 + 50 * row + 120) );
}

static void testDrawPathClipping(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

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

    // Characters with path
    row = 7;
    col = 0;
    path.setFont( Pt::Gfx::Font(FONT_SPEC_H) );
    testDrawPathClipping_drawCars(ip2, path, transform, row, col, brush2);

    row = 1;
    col = 15;
    path.setFont( Pt::Gfx::Font(FONT_SPEC_Q) );
    testDrawPathClipping_drawCars(ip2, path, transform, row, col, brush2);

    row = 7;
    col = 15;
    path.setFont( Pt::Gfx::Font(FONT_SPEC_C) );
    testDrawPathClipping_drawCars(ip2, path, transform, row, col, brush2);


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
