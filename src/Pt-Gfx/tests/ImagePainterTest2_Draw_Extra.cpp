static void testDrawExtra(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    Pen penHCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 16, Pen::Solid, Pen::RoundHoleCap, Pen::BevelJoin);
    Pen penACapBJoin(Color::fromRgb8(  0, 255, 0, 175), 20, Pen::Solid, Pen::ArrowCap,     Pen::BevelJoin);

    Pen penText( Color::fromRgb8(255,   0,   0, 175) );
    Pen penRef ( Color::fromRgb8(255, 127, 127, 127) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    // Round-Hole caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    ip2->setPen(penHCapBJoin);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 170), PointF(100, 120) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 170), PointF(100, 120) );

    ip2->setPen(penHCapBJoin);
    ip2->drawArc( PointF (150, 20), SizeF(150, 150), 60, 300, ArcMode::Open );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150, 20), SizeF(150, 150), 60, 300, ArcMode::Open );

    ip2->setPen(penText);
    ip2->drawText( PointF(70, 180), "Round-Hole" );

    // Arrow caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    ip2->setPen(penACapBJoin);
    ip2->drawLine( PointF( 20,  20 + 250), PointF(100,  70 + 250) );
    ip2->drawLine( PointF( 20, 170 + 250), PointF(100, 120 + 250) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20,  20 + 250), PointF(100,  70 + 250) );
    ip2->drawLine( PointF( 20, 170 + 250), PointF(100, 120 + 250) );

    ip2->setPen(penACapBJoin);
    ip2->drawArc( PointF (150, 20 + 250), SizeF(150, 150), 60, 300, ArcMode::Open );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150, 20 + 250), SizeF(150, 150), 60, 300, ArcMode::Open );

    ip2->setPen(penText);
    ip2->drawText( PointF(70, 180 + 250), "Arrow" );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
