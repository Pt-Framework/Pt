static void testDrawSolidThickLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    painter.setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 5, Pen::Solid, Pen::FlatCap, Pen::BevelJoin) );

    // FlatCap, ButtCap, SquareCap, RoundCap, TriangularOutCap, TriangularInCap
    // NoJoin, BevelJoin, MiterJoin, RoundJoin

    painter.drawLine( PointF(50, 50), PointF(400, 150) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawPatternedThickLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

