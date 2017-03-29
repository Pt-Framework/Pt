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

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    // Round-Hole caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    ip2->setPen(penHRCapBJoin);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 170), PointF(100, 120) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20,  20), PointF(100,  70) );
    ip2->drawLine( PointF( 20, 170), PointF(100, 120) );

    ip2->setPen(penHRCapBJoin);
    ip2->drawArc( PointF (150, 20), SizeF(100, 100), 60, 300, ArcMode::Open );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150, 20), SizeF(100, 100), 60, 300, ArcMode::Open );

    ip2->setPen(penText);
    ip2->drawText( PointF(70, 180), "Round-Hole" );

    // Arrow-1 caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    ip2->setPen(penA1CapBJoin);
    ip2->drawLine( PointF( 20 + 350,  20), PointF(100 + 350,  70) );
    ip2->drawLine( PointF( 20 + 350, 170), PointF(100 + 350, 120) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20 + 350,  20), PointF(100 + 350,  70) );
    ip2->drawLine( PointF( 20 + 350, 170), PointF(100 + 350, 120) );

    ip2->setPen(penA1CapBJoin);
    ip2->drawArc( PointF (150 + 350, 20), SizeF(100, 100), 60, 300, ArcMode::Open );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150 + 350, 20), SizeF(100, 100), 60, 300, ArcMode::Open );

    ip2->setPen(penText);
    ip2->drawText( PointF(70 + 350, 180), "Arrow-1" );

    // Arrow-2 caps
    ip2->setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    ip2->setPen(penA2CapBJoin);
    ip2->drawLine( PointF( 20 + 700,  20), PointF(100 + 700,  70) );
    ip2->drawLine( PointF( 20 + 700, 170), PointF(100 + 700, 120) );
    ip2->setPen(penRef);
    ip2->drawLine( PointF( 20 + 700,  20), PointF(100 + 700,  70) );
    ip2->drawLine( PointF( 20 + 700, 170), PointF(100 + 700, 120) );

    ip2->setPen(penA2CapBJoin);
    ip2->drawArc( PointF (150 + 700, 20), SizeF(100, 100), 60, 300, ArcMode::Open );
    ip2->setPen(penRef);
    ip2->drawArc( PointF (150 + 700, 20), SizeF(100, 100), 60, 300, ArcMode::Open );

    ip2->setPen(penText);
    ip2->drawText( PointF(70 + 700, 180), "Arrow-2" );

    // --- Part 1 ---

    // Rectangular gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Rectangular) );
    painter.fillRect( RectF(PointF(20, 250), SizeF(100, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Rectangular, 30.0f) );
    painter.fillRect( RectF(PointF(20 + 180, 250), SizeF(100, 100)) );

    // Radial gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Radial) );
    painter.fillRect( RectF(PointF(20 + 360, 250), SizeF(100, 100)) );

    // Conical gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Conical) );
    painter.fillRect( RectF(PointF(20 + 540, 250), SizeF(100, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Conical, 30.0f) );
    painter.fillRect( RectF(PointF(20 + 720, 250), SizeF(100, 100)) );

    // --- Part 2 ---

    // Rectangular gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Rectangular ) );
    painter.fillRect( RectF(PointF(20, 250 + 135), SizeF(50, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Rectangular, 30.0f) );
    painter.fillRect( RectF(PointF(20 + 180, 250 + 135), SizeF(50, 100)) );

    // Radial gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Radial) );
    painter.fillRect( RectF(PointF(20 + 360, 250 + 135), SizeF(50, 100)) );

    // Conical gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Conical) );
    painter.fillRect( RectF(PointF(20 + 540, 250 + 135), SizeF(50, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Conical, 30.0f) );
    painter.fillRect( RectF(PointF(20 + 720, 250 + 135), SizeF(50, 100)) );

    // Linear gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Linear) );
    painter.fillRect( RectF(PointF(20 + 90, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Linear, 15.0f) );
    painter.fillRect( RectF(PointF(20 + 90 + 180, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Linear, -15.0f) );
    painter.fillRect( RectF(PointF(20 + 90 + 360, 250 + 135), SizeF(50, 50)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Linear) );
    painter.fillRect( RectF(PointF(20 + 90 + 540, 250 + 135), SizeF(50, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Linear) );
    painter.fillRect( RectF(PointF(20 + 90 + 720, 250 + 135), SizeF(100, 50)) );

    // --- Part 3 ---

    // Rectangular gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Rectangular) );
    painter.fillRect( RectF(PointF(20, 250 + 270), SizeF(100, 50)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Rectangular, 30.0f) );
    painter.fillRect( RectF(PointF(20 + 180, 250 + 270), SizeF(100, 50)) );

    // Radial gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Radial) );
    painter.fillRect( RectF(PointF(20 + 360, 250 + 270), SizeF(100, 50)) );

    // Conical gradient
    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Conical) );
    painter.fillRect( RectF(PointF(20 + 540, 250 + 270), SizeF(100, 50)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Conical, 30.0f) );
    painter.fillRect( RectF(PointF(20 + 720, 250 + 270), SizeF(100, 50)) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
