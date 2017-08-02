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

    // Color stops
    ColorStops stops2;
    stops2.add(0.0, Color::fromRgb8(255, 0, 0, 175));
    stops2.add(1.0, Color::fromRgb8(0, 255, 0, 175));

    ColorStops stops3;
    stops3.add(0.1, Color::fromRgb8(255, 0, 0, 175));
    stops3.add(0.5, Color::fromRgb8(0, 255, 0, 175));
    stops3.add(0.9, Color::fromRgb8(0, 0, 255, 175));

    // Radial gradients
    painter.setBrush( Brush::radialGradient(0.5, 0.5, 0.0, 0.5, 0.5, 0.5, stops2) );
    painter.fillRect( RectF(PointF(20 + 150 * 0, 250 + 150 * 0), SizeF(100, 100)) );

    painter.setBrush( Brush::radialGradient(0.75, 0.75, 0.0, 0.5, 0.25, 0.5, stops2) );
    painter.fillRect( RectF(PointF(20 + 150 * 1, 250 + 150 * 0), SizeF(100, 100)) );

    painter.setBrush( Brush::radialGradient(0.25, 0.75, 0.0, 0.5, 0.25, 0.5, stops3) );
    painter.fillRect( RectF(PointF(20 + 150 * 2, 250 + 150 * 0), SizeF(100, 100)) );

    // Linear gradients
    painter.setBrush( Brush::linearGradient(0.0, 0.5, 1.0, 0.5, stops2) );
    painter.fillRect( RectF(PointF(20 + 150 * 0, 250 + 150 * 1), SizeF(100, 100)) );

    painter.setBrush( Brush::linearGradient(0.5, 0.0, 0.5, 1.0, stops2) );
    painter.fillRect( RectF(PointF(20 + 150 * 1, 250 + 150 * 1), SizeF(100, 100)) );

    painter.setBrush( Brush::linearGradient(0.1, 0.2, 1.0, 0.9, stops3) );
    painter.fillRect( RectF(PointF(20 + 150 * 2, 250 + 150 * 1), SizeF(100, 100)) );

/*
    // --- Part 1 ---

    // Rectangular gradient
    //painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.3f, 0.7f ) );
    //painter.fillRect( RectF(PointF(20, 250), SizeF(100, 100)) );

    //painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.5f, 0.5f, 30.0f) );
    //painter.fillRect( RectF(PointF(20 + 150, 250), SizeF(100, 100)) );

    // Radial gradient
    ColorStops stops;
    stops.add(0, Color::fromRgb8(255, 0, 0, 175));
    stops.add(1.0, Color::fromRgb8(0, 255, 0, 175));

    painter.setBrush( Brush::radialGradient(0.5, 0.5, 0,
                                            0.5, 0.5, 0.5, stops) );
    painter.fillRect( RectF(PointF(20 + 300, 250), SizeF(100, 100)) );

    painter.setBrush( Brush::radialGradient(0.3f, 0.7f, 0,
                                            0.5, 0.5, 0.5,
                                            stops) );
    painter.fillRect( RectF(PointF(20 + 450, 250), SizeF(100, 100)) );

    // Conical gradient
    //painter.setBrush( Brush::conicalGradient( Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175) ) );
    //painter.fillRect( RectF(PointF(20 + 600, 250), SizeF(100, 100)) );

    //painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.7f, 0.3f, 30.0f) );
    //painter.fillRect( RectF(PointF(20 + 750, 250), SizeF(100, 100)) );

    // --- Part 2 ---

    // Rectangular gradient
    //painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175) ) );
    //painter.fillRect( RectF(PointF(20, 250 + 135), SizeF(50, 100)) );

    //painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.5f, 0.5f, 30.0f) );
    //painter.fillRect( RectF(PointF(20 + 180, 250 + 135), SizeF(50, 100)) );

    // Radial gradient
    painter.setBrush( Brush::radialGradient(0.5, 0.5, 0,
                                            0.5, 0.5, 0.5, stops) );
    painter.fillRect( RectF(PointF(20 + 360, 250 + 135), SizeF(50, 100)) );

    // Conical gradient
    //painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    //painter.fillRect( RectF(PointF(20 + 540, 250 + 135), SizeF(50, 100)) );

    //painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.5f, 0.5f, 30.0f) );
    //painter.fillRect( RectF(PointF(20 + 720, 250 + 135), SizeF(50, 100)) );

    // Linear gradient
    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 90, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.5f, 0.5f, 15.0f) );
    painter.fillRect( RectF(PointF(20 + 90 + 180, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.5f, 0.5f, -15.0f) );
    painter.fillRect( RectF(PointF(20 + 90 + 360, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 90 + 540, 250 + 135), SizeF(50, 100)) );

    painter.setBrush( Brush::linearGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 90 + 720, 250 + 135), SizeF(100, 50)) );

    // --- Part 3 ---

    // Rectangular gradient
    //painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    //painter.fillRect( RectF(PointF(20, 250 + 270), SizeF(100, 50)) );

    //painter.setBrush( Brush::rectangularGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.5f, 0.5f, 30.0f) );
    //painter.fillRect( RectF(PointF(20 + 180, 250 + 270), SizeF(100, 50)) );

    // Radial gradient
    painter.setBrush( Brush::radialGradient(0.5, 0.5, 0,
                                            0.5, 0.5, 0.5, stops) );
    painter.fillRect( RectF(PointF(20 + 360, 250 + 270), SizeF(100, 50)) );

    // Conical gradient
    //painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175)) );
    //painter.fillRect( RectF(PointF(20 + 540, 250 + 270), SizeF(100, 50)) );

    //painter.setBrush( Brush::conicalGradient(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), 0.5f, 0.5f, 30.0f) );
    //painter.fillRect( RectF(PointF(20 + 720, 250 + 270), SizeF(100, 50)) );

    */

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
