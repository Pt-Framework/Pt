static void testDrawExtra(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    Pen penHRCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 20, Pen::Solid, Pen::RoundHoleCap, Pen::BevelJoin);
    Pen penA1CapBJoin(Color::fromRgb8(  0, 255, 0, 175), 20, Pen::Solid, Pen::Arrow1Cap,    Pen::BevelJoin);
    Pen penA2CapBJoin(Color::fromRgb8(  0, 255, 0, 175), 20, Pen::Solid, Pen::Arrow2Cap,    Pen::BevelJoin);

    Pen penText( Color::fromRgb8(255,   0,   0, 175) );
    Pen penRef ( Color::fromRgb8(255, 127, 127, 127) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasing(true);

    // Round-Hole caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );

    ip2->setPen(penHRCapBJoin);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 170), PointF(100, 120) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 170), PointF(100, 120) );

    ip2->setPen(penHRCapBJoin);
    ip2->drawArc( PointF (150, 20), SizeF(100, 100), 60, 300 );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150, 20), SizeF(100, 100), 60, 300 );

    ip2->setPen(penText);
    ip2->drawText( PointF(70, 180), "Round-Hole" );

    // Arrow-1 caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );

    ip2->setPen(penA1CapBJoin);
    ip2->drawLine( PointF( 20 + 300,  20), PointF(100 + 300,  70) );
    ip2->drawLine( PointF( 20 + 300, 170), PointF(100 + 300, 120) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20 + 300,  20), PointF(100 + 300,  70) );
    ip2->drawLine( PointF( 20 + 300, 170), PointF(100 + 300, 120) );

    ip2->setPen(penA1CapBJoin);
    ip2->drawArc( PointF (150 + 300, 20), SizeF(100, 100), 60, 300 );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150 + 300, 20), SizeF(100, 100), 60, 300 );

    ip2->setPen(penText);
    ip2->drawText( PointF(70 + 300, 180), "Arrow-1" );

    // Arrow-2 caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );

    ip2->setPen(penA2CapBJoin);
    ip2->drawLine( PointF( 20 + 600,  20), PointF(100 + 600,  70) );
    ip2->drawLine( PointF( 20 + 600, 170), PointF(100 + 600, 120) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20 + 600,  20), PointF(100 + 600,  70) );
    ip2->drawLine( PointF( 20 + 600, 170), PointF(100 + 600, 120) );

    ip2->setPen(penA2CapBJoin);
    ip2->drawArc( PointF (150 + 600, 20), SizeF(100, 100), 60, 300 );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150 + 600, 20), SizeF(100, 100), 60, 300 );

    ip2->setPen(penText);
    ip2->drawText( PointF(70 + 600, 180), "Arrow-2" );

    // --- Part 1 ---

    // Rectangular gradient
    painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175) ) );
    painter.fillRect( RectF(PointF(20, 250), SizeF(100, 100)) );

    painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 30.0f) );
    painter.fillRect( RectF(PointF(20 + 180, 250), SizeF(100, 100)) );

    // Radial gradient
    painter.setBrush( Brush::radialGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 360, 250), SizeF(100, 100)) );

    // Conical gradient
    painter.setBrush( Brush::conicalGradient( Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175) ) );
    painter.fillRect( RectF(PointF(20 + 540, 250), SizeF(100, 100)) );

    painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 30.0f) );
    painter.fillRect( RectF(PointF(20 + 720, 250), SizeF(100, 100)) );

    // --- Part 2 ---

    // Rectangular gradient
    painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175) ) );
    painter.fillRect( RectF(PointF(20, 250 + 135), SizeF(50, 100)) );

    painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 30.0f) );
    painter.fillRect( RectF(PointF(20 + 180, 250 + 135), SizeF(50, 100)) );

    // Radial gradient
    painter.setBrush( Brush::radialGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 360, 250 + 135), SizeF(50, 100)) );

    // Conical gradient
    painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 540, 250 + 135), SizeF(50, 100)) );

    painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 30.0f) );
    painter.fillRect( RectF(PointF(20 + 720, 250 + 135), SizeF(50, 100)) );

    // Linear gradient
    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 90, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 15.0f) );
    painter.fillRect( RectF(PointF(20 + 90 + 180, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), -15.0f) );
    painter.fillRect( RectF(PointF(20 + 90 + 360, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 90 + 540, 250 + 135), SizeF(50, 100)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 90 + 720, 250 + 135), SizeF(100, 50)) );

    // --- Part 3 ---

    // Rectangular gradient
    painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20, 250 + 270), SizeF(100, 50)) );

    painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 30.0f) );
    painter.fillRect( RectF(PointF(20 + 180, 250 + 270), SizeF(100, 50)) );

    // Radial gradient
    painter.setBrush( Brush::radialGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 360, 250 + 270), SizeF(100, 50)) );

    // Conical gradient
    painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 540, 250 + 270), SizeF(100, 50)) );

    painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 30.0f) );
    painter.fillRect( RectF(PointF(20 + 720, 250 + 270), SizeF(100, 50)) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
