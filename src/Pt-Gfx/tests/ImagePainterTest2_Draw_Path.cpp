static void testDrawPath(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    Pen penThinSolid ( Pen(Color::fromRgb8(127, 255, 191, 175), 1, Pen::Solid, Pen::RoundCap) );
    Pen penThinDot   ( Pen(Color::fromRgb8(127, 255, 191, 175), 1, Pen::Dot,   Pen::RoundCap) );
    Pen penThickSolid( Pen(Color::fromRgb8(127, 255, 191, 175), 6, Pen::Solid, Pen::RoundCap) );
    Pen penThickDot  ( Pen(Color::fromRgb8(127, 255, 191, 175), 6, Pen::Dot,   Pen::RoundCap) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    AffineMatrix2D matrix2d;
    Path2D         path2d;

    path2d.clear    ();
    path2d.beginPath();
    path2d.moveTo   (  0, 50);
    path2d.lineTo   ( 50, 80);
    path2d.lineTo   (100, 50);
    path2d.lineTo   ( 30,  0);
    path2d.endPath  ();

    matrix2d.translate(-50, -40);

    Pt::int32_t row = 0, col = 0;

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

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
