static void testDrawExtra(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    /*
    Pen penThinSolid ( Pen(Color::fromRgb8(255, 191, 127, 175), 1, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThinDot   ( Pen(Color::fromRgb8(255, 191, 127, 175), 1, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThickSolid( Pen(Color::fromRgb8(255, 191, 127, 175), 6, Pen::Solid, Pen::RoundCap, Pen::BevelJoin ) );
    Pen penThickDot  ( Pen(Color::fromRgb8(255, 191, 127, 175), 6, Pen::Dot,   Pen::RoundCap, Pen::BevelJoin ) );
    */

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    /*
    AffineMatrix2D matrix2d;
    Path2D         path2d;

    // Create a new path
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
    */

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
