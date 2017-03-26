static void testDrawExtra(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    Pen penACapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::ArrowCap, Pen::BevelJoin);

    Pen penText( Color::fromRgb8(255,   0,   0, 175) );
    Pen penRef ( Color::fromRgb8(255, 127, 127, 127) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    // Arrow caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    ip2->setPen(penACapBJoin);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 150), PointF(100, 100) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 150), PointF(100, 100) );

    ip2->setPen(penACapBJoin);
    ip2->drawArc( PointF (130, 20), SizeF(150, 150), 0, 135, ArcMode::Open );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (130, 20), SizeF(150, 150), 0, 135, ArcMode::Open );

    ip2->setPen(penText);
    ip2->drawText( PointF(150, 100), "Arrow" );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
