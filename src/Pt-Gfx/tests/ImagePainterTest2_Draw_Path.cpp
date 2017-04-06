static void testDrawPath_drawRow(
    ImagePainter2* ip2, AffineTransform& atrans, const Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThinDot, const Pen& penThickSolid, const Pen& penThickDot,
    const Brush& brush1, const Brush& brush2
)
{
    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path, atrans, false);
    atrans.pop();
    ++col;

    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path, atrans, false);
    atrans.pop();
    ++col;

    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path, atrans, false);
    atrans.pop();
    ++col;

    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path, atrans, false);
    atrans.pop();
    ++col;

    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path, atrans);
    atrans.pop();
    ++col;

    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path, atrans);
    atrans.pop();
    col = 0;
    ++row;
}

static void testDrawPath_drawCol(
    ImagePainter2* ip2, AffineTransform& atrans, const Path& path, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThickSolid
)
{
    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path, atrans, false);
    atrans.pop();
    ++col;

    atrans.rotate(15);
    atrans.push();
    atrans.translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path, atrans, false);
    atrans.pop();
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

    AffineTransform atrans;
    Path            path;

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

    atrans.translate(-50, -40);

    // First row
    testDrawPath_drawRow(ip2, atrans, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

    // Second row
    testDrawPath_drawRow(ip2, atrans, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

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

    atrans.identity();
    atrans.translate(-50, -35);
    atrans.scale(0.75f, 1.0f);

    // Third row
    testDrawPath_drawRow(ip2, atrans, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

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
    testDrawPath_drawRow(ip2, atrans, path, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

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

    atrans.identity();
    atrans.translate(-50, -25);
    atrans.scale(0.75f, 1.0f);

    row = 0;
    col = 6;

    testDrawPath_drawCol(ip2, atrans, path, row, col, penThinSolid, penThickSolid);

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

    atrans.identity();
    atrans.translate(-50, -25);
    atrans.scale(0.75f, 1.0f);

    row = 1;
    col = 6;

    testDrawPath_drawCol(ip2, atrans, path, row, col, penThinSolid, penThickSolid);

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

    atrans.identity();
    atrans.rotate(-25);
    atrans.scale(2.5, 2.5);

#define DRAW_CB(PEN)                                 \
    do {                                             \
        atrans.push();                               \
        atrans.translate(90 + 160 * col, 140 * row); \
        ip2->setPen(PEN);                            \
        ip2->drawPath(path, atrans, false, 2);       \
        atrans.pop();                                \
        ++col;                                       \
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

    atrans.identity();
    atrans.translate(-5.0, -2.5);
    atrans.scale(25, 25);

    atrans.push();
    atrans.translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    //ip2->fillPath(path, atrans, 1);
    ip2->drawPath(path, atrans, false, 1);
    atrans.pop();
    ++row;

    atrans.push();
    atrans.translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    //ip2->fillPath(path, atrans, 20);
    ip2->drawPath(path, atrans, false, 20);
    atrans.pop();

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

static void testDrawPathClipping(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    Pt::int32_t row = 0, col = 0;

    AffineTransform atrans;
    Path            path;

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
    atrans.push();
    atrans.scale(2, 2);
    atrans.translate(50, 70);
    atrans.transformPoints(cregPointsF.data(), cregPointsF.size());
    atrans.pop();

    // Create a subject path
    path.clear    ();
    path.beginPath();
    path.moveTo   (0,  50     ); // CCW
    path.arcTo    (100, 50, 50);
    path.arcTo    (  0, 50, 50);
    path.endPath  ();

    subjPointsF.clear();
    path.generatePoints(subjPointsF, 3);
    atrans.push();
    atrans.scale(2, 2);
    atrans.transformPoints(subjPointsF.data(), subjPointsF.size());
    atrans.pop();

    // Draw the clip-region and subject polygons
    atrans.push();
    atrans.translate(10 + 250 * col, 10 + 250 * row);
    atrans.transformPoints(cregPointsF.data(), cregPointsF.size());
    atrans.transformPoints(subjPointsF.data(), subjPointsF.size());
    atrans.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(subjPointsF.data(), subjPointsF.size());
    ip2->setBrush(brush2);
    ip2->fillPolygon(cregPointsF.data(), cregPointsF.size());
    ++col;

    // Perform clipping and draw the clipped polygon
    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Intersection);
    atrans.push();
    atrans.translate(10 + 250 * col, 10 + 250 * row);
    atrans.transformPoints(clipPointsF.data(), clipPointsF.size());
    atrans.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());
    ++col;

    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Union);
    atrans.push();
    atrans.translate(10 + 250 * col, 10 + 250 * row);
    atrans.transformPoints(clipPointsF.data(), clipPointsF.size());
    atrans.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());
    ++row;
    col = 1;

    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Difference);
    atrans.push();
    atrans.translate(10 + 250 * col, 10 + 250 * row);
    atrans.transformPoints(clipPointsF.data(), clipPointsF.size());
    atrans.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());
    ++col;

    Path::clipPolygon(clipPointsF, subjPointsF, cregPointsF, Path::Xor);
    atrans.push();
    atrans.translate(10 + 250 * col, 10 + 250 * row);
    atrans.transformPoints(clipPointsF.data(), clipPointsF.size());
    atrans.pop();
    ip2->setBrush(brush1);
    ip2->fillPolygon(clipPointsF.data(), clipPointsF.size());

    // Character outline
    row = 4;
    col = 0;

    path.clear    ();
    path.beginPath();
    path.moveTo   (100, 100);
    path.putChar  ('P');
    path.endPath  ();

    subjPointsF.clear();
    path.setFont( Pt::Gfx::Font(FONT_SPEC_H) );
    path.generatePoints(subjPointsF, 1);
    atrans.push();
    atrans.translate(50 + 100 * col, 50 + 100 * row);
    atrans.transformPoints(subjPointsF.data(), subjPointsF.size());
    atrans.pop();

    ip2->setPen(Color::fromRgb8(255,0,0));
    ip2->setBrush(brush2);
    ip2->drawPolyline(subjPointsF.data(), subjPointsF.size(), false);

    //ip2->setFont( Pt::Gfx::Font(FONT_SPEC_H) );
    //ip2->setPen( Color::fromRgb8(0, 255, 255, 255) );
    //ip2->drawText( PointF(10 + 250 * col, 10 + 250 * row), "P" );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
