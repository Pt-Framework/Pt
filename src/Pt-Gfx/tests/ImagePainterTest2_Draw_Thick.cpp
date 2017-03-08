static void testDrawSolidThickLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    painter.setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 10, Pen::Solid, Pen::FlatCap, Pen::BevelJoin) );

    // FlatCap, ButtCap, SquareCap, RoundCap, TriangularOutCap, TriangularInCap
    // NoJoin, BevelJoin, MiterJoin, RoundJoin

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.drawLine( PointF( 20,  20), PointF(200, 120) );
    painter.drawLine( PointF( 20, 250), PointF(200, 150) );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.drawLine( PointF(220,  20), PointF(400, 120) );
    painter.drawLine( PointF(220, 250), PointF(400, 150) );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::LowMemory);
    painter.drawLine( PointF(420,  20), PointF(600, 120) );
    painter.drawLine( PointF(420, 250), PointF(600, 150) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawPatternedThickLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

