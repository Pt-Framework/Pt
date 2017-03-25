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
    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    col = 0;
    ++row;

    // Second row
    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path2d, matrix2d, true);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path2d, matrix2d, true);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path2d, matrix2d, true);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path2d, matrix2d, true);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    col = 0;
    ++row;

    // Create a new path
    path2d.clear            ();
    path2d.beginPath        ();
    path2d.moveTo           (          120, 70); // CCW
    path2d.lineTo           (          100, 50);
    path2d.lineTo           (           75, 50);
    path2d.quadraticBezierTo( 50,   0,  25, 50);
    path2d.lineTo           (            0, 50);
    path2d.lineTo           (          -20, 70);
    path2d.endPath          ();

    matrix2d.identity();
    matrix2d.translate(-50, -35);
    matrix2d.scaleAboutOrigin(0.75f, 1.0f);

    // Third row
    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    col = 0;
    ++row;

    // Create a new path
    path2d.clear            ();
    path2d.beginPath        ();
    path2d.moveTo           (          120,  0); // CCW
    path2d.lineTo           (          100, 20);
    path2d.lineTo           (           75, 20);
    path2d.quadraticBezierTo( 50,  50,  25, 20);
    path2d.lineTo           (            0, 20);
    path2d.lineTo           (          -20,  0);
    path2d.endPath          ();

    // Fourth row
    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThinDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickSolid);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setPen(penThickDot);
    ip2->drawPath(path2d, matrix2d, false);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush1);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    ++col;

    matrix2d.rotateAboutOrigin(15);
    matrix2d.push();
    matrix2d.translate(80 + 120 * col, 80 + 120 * row);
    ip2->setBrush(brush2);
    ip2->fillPath(path2d, matrix2d);
    matrix2d.pop();
    col = 0;
    ++row;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
