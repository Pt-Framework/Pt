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

    // 1D gradients
    painter.setBrush( Brush::horizontalGradient(Color::fromRgb8(0, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 130 * 0, 250 + 150 * 0), SizeF(100, 100)) );

    painter.setBrush( Brush::verticalGradient(Color::fromRgb8(0, 255, 0, 175), Color::fromRgb8(0, 0, 0, 175)) );
    painter.fillRect( RectF(PointF(20 + 130 * 1, 250 + 150 * 0), SizeF(100, 100)) );

    // 2D gradients (linear)
    painter.setBrush( Brush::linearGradient(0.0, 0.5, 1.0, 0.5, stops2) );
    painter.fillRect( RectF(PointF(20 + 130 * 0, 250 + 150 * 1), SizeF(100, 100)) );

    painter.setBrush( Brush::linearGradient(0.5, 0.0, 0.5, 1.0, stops2) );
    painter.fillRect( RectF(PointF(20 + 130 * 1, 250 + 150 * 1), SizeF(100, 100)) );

    painter.setBrush( Brush::linearGradient(0.1, 0.2, 1.0, 0.9, stops3) );
    painter.fillRect( RectF(PointF(20 + 130 * 2, 250 + 150 * 1), SizeF(100, 100)) );

    painter.setBrush( Brush::linearGradient(0.7, 0.2, 0.5, 0.8, stops3) );
    painter.fillRect( RectF(PointF(20 + 130 * 3, 250 + 150 * 1), SizeF(100, 100)) );

    // 2D gradients (radial)
    painter.setBrush( Brush::radialGradient(0.5, 0.5, 0.0, 0.5, 0.5, 0.5, stops2) );
    painter.fillRect( RectF(PointF(20 + 130 * 4, 250 + 150 * 1), SizeF(100, 100)) );

    painter.setBrush( Brush::radialGradient(0.75, 0.75, 0.0, 0.5, 0.25, 0.5, stops2) );
    painter.fillRect( RectF(PointF(20 + 130 * 5, 250 + 150 * 1), SizeF(100, 100)) );

    painter.setBrush( Brush::radialGradient(0.25, 0.75, 0.0, 0.5, 0.25, 0.5, stops3) );
    painter.fillRect( RectF(PointF(20 + 130 * 6, 250 + 150 * 1), SizeF(100, 100)) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
