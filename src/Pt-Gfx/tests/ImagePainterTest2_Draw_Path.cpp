static void testDrawPath(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

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
    ip2->setBrush(brush1);
    ip2->fillPath(path2d, matrix2d);

    matrix2d.translate(100, 0);
    ip2->setBrush(brush2);
    ip2->fillPath(path2d, matrix2d);

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
