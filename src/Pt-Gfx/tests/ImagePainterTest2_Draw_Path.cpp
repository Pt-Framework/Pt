static void testDrawPath_drawRow(
    ImagePainter2* ip2, AffineMatrix2D& matrix2d, const Path2D& path2d, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThinDot, const Pen& penThickSolid, const Pen& penThickDot,
    const Brush& brush1, const Brush& brush2
)
{
    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 110 * col, 60 + 110 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    col = 0;
    ++row;
}

static void testDrawPath_drawCol(
    ImagePainter2* ip2, AffineMatrix2D& matrix2d, const Path2D& path2d, Pt::int32_t& row, Pt::int32_t& col,
    const Pen& penThinSolid, const Pen& penThickSolid
)
{
    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(60 + 120 * col, 60 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
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

    AffineMatrix2D matrix2d;
    Path2D         path2d;

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
    testDrawPath_drawRow(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

    // Second row
    testDrawPath_drawRow(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

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
    testDrawPath_drawRow(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

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
    testDrawPath_drawRow(ip2, matrix2d, path2d, row, col, penThinSolid, penThinDot, penThickSolid, penThickDot, brush1, brush2);

    //
    // Rightmost part of the image
    //

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

    row = 0;
    col = 6;

    testDrawPath_drawCol(ip2, matrix2d, path2d, row, col, penThinSolid, penThickSolid);

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

    row = 1;
    col = 6;

    testDrawPath_drawCol(ip2, matrix2d, path2d, row, col, penThinSolid, penThickSolid);

    //
    // The effect of smoothness
    //
    row = 2;
    col = 6;

    path2d.clear    ();
    path2d.beginPath();
    path2d.moveTo   (10.0, 2.0      ); // CCW
    path2d.lineTo   ( 7.5, 2.0      );
    path2d.arcTo    ( 2.5, 2.0, -5.0);
    path2d.lineTo   ( 0.0, 2.0      );
    path2d.endPath  ();

    matrix2d.identity();
    matrix2d.translate(-5.0, -2.5);
    matrix2d.scaleAboutOrigin(25, 25);

    matrix2d.push();
    matrix2d.translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    ip2->drawPath(path2d, matrix2d, false, 0);
    matrix2d.pop();
    ++row;

    matrix2d.push();
    matrix2d.translate(60 + 130 * col, 150 * row);
    ip2->setPen( Color::fromRgb8(255, 255, 255, 175) );
    ip2->drawPath(path2d, matrix2d, false, 20);
    matrix2d.pop();

    //
    // Cubic bezier
    //
    row = 3;
    col = 0;

    path2d.clear        ();
    path2d.beginPath    ();
    path2d.moveTo       (60, 60                ); // CCW
    path2d.cubicBezierTo(40, 30, 10, 70, 0, 100);
    path2d.endPath      ();

    matrix2d.identity();
    matrix2d.translate(-30, -20);
    matrix2d.scaleAboutOrigin(1.5, 1.5);

#define DRAW_CB(PEN)                                   \
    do {                                               \
        matrix2d.push();                               \
        matrix2d.translate(60 + 130 * col, 150 * row); \
        ip2->setPen(PEN);                              \
        ip2->drawPath(path2d, matrix2d, false, 1);     \
        matrix2d.pop();                                \
        ++col;                                         \
    } while(false)

    DRAW_CB(penThinSolid );
    DRAW_CB(penThinDot   );
    DRAW_CB(penThickSolid);
    DRAW_CB(penThickDot  );

#undef DRAW_CB

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
