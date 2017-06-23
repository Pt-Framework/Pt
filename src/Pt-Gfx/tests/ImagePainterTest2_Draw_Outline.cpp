static void testDrawSolidLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasing(true);

#if 1
    painter.setClip( RectF (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Color::fromRgb8(0, 255, 0, 175) );
    painter.drawLine( PointF(  0,   0), PointF(999, 599) );
    painter.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );
#endif

    painter.drawImage(PointF(image.width() - textureWithWhiteBackground.width(),   0), textureWithWhiteBackground);
    painter.drawImage(PointF(image.width() - textureWithWhiteBackground.width(), 300), textureWithWhiteBackground, RectF(20, 70, 20, 70));

    painter.setPen( Color::fromRgb8(255, 0, 0, 175) );

    painter.drawLine( PointF(  0,   0), PointF(999,   0) );
    painter.drawLine( PointF(  0, 599), PointF(999, 599) );
    painter.drawLine( PointF(  0,   0), PointF(  0, 599) );
    painter.drawLine( PointF(999,   0), PointF(999, 599) );

    painter.setPen( Color::fromRgb8(255, 255, 255, 175) );

    painter.drawLine( PointF( 10, 110), PointF(100, 150) );
    painter.drawLine( PointF( 10, 250), PointF(100, 210) );

    painter.drawLine( PointF(100,  10), PointF(110,  20) );

    painter.setPen( Color::fromRgb8( 63,  63,  63, 175) ); painter.drawLine( PointF(500-50, 500), PointF(200-50, 200) );
    painter.setPen( Color::fromRgb8(127, 127, 127, 175) ); painter.drawLine( PointF(500   , 500), PointF(200   , 200) );
    painter.setPen( Color::fromRgb8(255, 255, 255, 175) ); painter.drawLine( PointF(500+50, 500), PointF(200+50, 200) );
                                                           painter.drawLine( PointF(300   , 200), PointF(700   , 100) );

    painter.drawLine( PointF(770,  11), PointF(770, 500) );
    painter.drawLine( PointF(780,  11), PointF(782, 500) );

    painter.drawLine( PointF( 10, 540), PointF(781, 540) );
    painter.drawLine( PointF(781, 552), PointF( 10, 550) );

    painter.setFont( Pt::Gfx::Font(FONT_SPEC_R) );
    painter.setPen( Color::fromRgb8(0, 255, 255, 255) ); painter.drawText( PointF(100 + 200, 175 + 200), "Hello world!" );
    painter.setPen( Color::fromRgb8(0, 255, 255, 175) ); painter.drawText( PointF(100 + 200, 225 + 200), "Hello world!" );

    painter.setFont( Pt::Gfx::Font(FONT_SPEC_N) );
    painter.setPen( Color::fromRgb8(0, 255, 255, 255) ); painter.drawText( PointF(100, 175), "Hello world!" );
    painter.setPen( Color::fromRgb8(0, 255, 255, 175) ); painter.drawText( PointF(100, 225), "Hello world!" );

    //if(ip2) {
    //    const PointF bezier1a[] = { // CCW
    //        PointF(300, 100),
    //        PointF(265,  65),
    //        PointF(200,  50)
    //    };
    //    ip2->setAntiAliasing(false);
    //    ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Solid ) );
    //    ip2->drawQuadraticPolybezier( bezier1a, sizeof(bezier1a) / sizeof(bezier1a[0]), false );

    //    const PointF bezier1b[] = { // CCW
    //        PointF(300 + 100, 100),
    //        PointF(265 + 100,  65),
    //        PointF(200 + 100,  50)
    //    };
    //    ip2->setAntiAliasing(true);
    //    ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Solid ) );
    //    ip2->drawQuadraticPolybezier( bezier1b, sizeof(bezier1b) / sizeof(bezier1b[0]), false );
    //}

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawPatternedLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    painter.setClip( RectF (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Pen( Color::fromRgb8(0, 255, 0, 175), 1, Pen::Dash ) );
    if(ip2) ip2->setAntiAliasing(false); painter.drawLine( PointF(  0, -30), PointF(999, 599 - 30) );
    if(ip2) ip2->setAntiAliasing(true); painter.drawLine( PointF(  0,  30), PointF(999, 599 + 30) );
    painter.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );

    if(ip2) ip2->setAntiAliasing(true);

    painter.setPen( Pen( Color::fromRgb8(255, 0, 0, 175), 1, Pen::DotDash ) );
    painter.drawLine( PointF(  5,   5), PointF(994,   5) );
    painter.drawLine( PointF(  5, 594), PointF(994, 594) );
    painter.drawLine( PointF(  5,   5), PointF(  5, 594) );
    painter.drawLine( PointF(994,   5), PointF(994, 594) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dash ) );
    painter.drawLine( PointF( 10, 110), PointF(100, 150) );
    painter.drawLine( PointF( 10, 250), PointF(100, 210) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dot ) );
    painter.drawLine( PointF(500, 500), PointF(200, 200) );
    painter.drawLine( PointF(300, 150), PointF(700,  50) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDash ) );
    painter.drawLine( PointF(550, 500), PointF(250, 200) );
    painter.drawLine( PointF(300, 200), PointF(700, 100) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dot ) );
    //painter.drawLine( PointF(870,  11), PointF(870, 500) );
    painter.drawLine( PointF(880,  11), PointF(882, 500) );
    painter.drawLine( PointF( 10, 540), PointF(781, 540) );
    painter.drawLine( PointF(781, 552), PointF( 10, 550) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDot ) );
    painter.drawLine( PointF(970,  11), PointF(970, 500) );
    painter.drawLine( PointF(980,  11), PointF(982, 500) );
    painter.drawLine( PointF( 10, 440), PointF(781, 440) );
    painter.drawLine( PointF(781, 452), PointF( 10, 450) );

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen( Pen( Color::fromRgb8(0, 255, 255, 175), 1, Pen::Dash ) );

    const PointF poly[] = { // CCW
        PointF(110 - 0, 310 - 70),
        PointF(160 - 0, 340 - 70),
        PointF(210 - 0, 310 - 70),
        PointF(140 - 0, 260 - 70),
        PointF(110 - 0, 310 - 70)
    };

    if(ip2)
      ip2->drawPolyline( poly, sizeof(poly) / sizeof(poly[0]) );
    else
      painter.drawPolyline( poly, sizeof(poly) / sizeof(poly[0]) );

    painter.drawRect( RectF(PointF(550, 170), SizeF(100, 50)) );

    painter.drawEllipse( PointF (730,  50), SizeF(95, 95) );
    if(ip2)
      ip2->drawChord( PointF (730, 170), SizeF(95, 43), 30, 330);
    if(ip2)
      ip2->drawPie( PointF (753, 243), SizeF(43, 95), 30, 330 );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, 0xB38F0F83F03F8000 ) ); // 1011001110001111000011111000001111110000001111111000000000000000
    painter.drawRect( RectF(PointF(260, 20), SizeF(180, 80)) );

    //if(ip2) {
    //    const PointF bezier1a[] = { // CCW
    //        // Bottom left
    //        PointF(400 - 350,  90 + 300),
    //        PointF(400 - 350, 110 + 300),
    //        PointF(420 - 350, 110 + 300),
    //        // Bottom middle
    //        PointF(450 - 350, 110 + 300),
    //        // Bottom right
    //        PointF(480 - 350, 110 + 300),
    //        PointF(500 - 350, 110 + 300),
    //        PointF(500 - 350,  90 + 300),
    //        // Center right
    //        PointF(500 - 350,  70 + 300),
    //        // Top right
    //        PointF(500 - 350,  50 + 300),
    //        PointF(500 - 350,  30 + 300),
    //        PointF(480 - 350,  30 + 300),
    //        // Top middle
    //        PointF(450 - 350,  30 + 300),
    //        // Top left
    //        PointF(420 - 350,  30 + 300),
    //        PointF(400 - 350,  30 + 300),
    //        PointF(400 - 350,  50 + 300),
    //        // Center left
    //        PointF(400 - 350,  70 + 300)
    //    };
    //    ip2->setAntiAliasing(false);
    //    ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Dash ) );
    //    ip2->drawQuadraticPolybezier( bezier1a, sizeof(bezier1a) / sizeof(bezier1a[0]), true );

    //    const PointF bezier1b[] = { // CCW
    //        // Bottom left
    //        PointF(400 - 200,  90 + 300),
    //        PointF(400 - 200, 110 + 300),
    //        PointF(420 - 200, 110 + 300),
    //        // Bottom middle
    //        PointF(450 - 200, 110 + 300),
    //        // Bottom right
    //        PointF(480 - 200, 110 + 300),
    //        PointF(500 - 200, 110 + 300),
    //        PointF(500 - 200,  90 + 300),
    //        // Center right
    //        PointF(500 - 200,  70 + 300),
    //        // Top right
    //        PointF(500 - 200,  50 + 300),
    //        PointF(500 - 200,  30 + 300),
    //        PointF(480 - 200,  30 + 300),
    //        // Top middle
    //        PointF(450 - 200,  30 + 300),
    //        // Top left
    //        PointF(420 - 200,  30 + 300),
    //        PointF(400 - 200,  30 + 300),
    //        PointF(400 - 200,  50 + 300),
    //        // Center left
    //        PointF(400 - 200,  70 + 300)
    //    };
    //    ip2->setAntiAliasing(true);
    //    ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Dash ) );
    //    ip2->drawQuadraticPolybezier( bezier1b, sizeof(bezier1b) / sizeof(bezier1b[0]), true );
    //}

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawRect(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasing(true);

#if 1

    // Solid
    painter.setPen  ( Color::fromRgb8(255, 255,   0, 175) );
    painter.setBrush( Color::fromRgb8(255,   0, 255, 175) );
    painter.drawRect( RectF(PointF(100,  50), SizeF(200, 100)) );
    painter.fillRect( RectF(PointF(400,  50), SizeF(200, 100)) );

    painter.setPen  ( Color::fromRgb8(  0, 255, 255, 175) );
    painter.setBrush( Color::fromRgb8(255, 255,   0, 175) );
    painter.drawRect( RectF(PointF(150, 100), SizeF(200, 100)) );
    painter.fillRect( RectF(PointF(450, 100), SizeF(200, 100)) );

    // Gradient
    painter.setBrush( Color::fromRgb8(255, 255, 255, 175) );
    painter.fillRect( RectF(PointF(100, 220), SizeF(200, 100)) );
    painter.fillRect( RectF(PointF(400, 220), SizeF(200, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Horizontal) );
    painter.fillRect( RectF(PointF(150, 270), SizeF(200, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Vertical) );
    painter.fillRect( RectF(PointF(450, 270), SizeF(200, 100)) );

    // Texture
    painter.setBrush( Color::fromRgb8(255, 255, 255, 175) );
    painter.fillRect( RectF(PointF(100, 390), SizeF(200, 100)) );
    painter.fillRect( RectF(PointF(400, 390), SizeF(200, 100)) );

    painter.setBrush( Brush(textureWithWhiteBackground) );
    painter.fillRect( RectF(PointF(150, 440), SizeF(200, 100)) );

    painter.setBrush( Brush(textureWithTransBackground) );
    painter.fillRect( RectF(PointF(450, 440), SizeF(200, 100)) );

    // Thick
    painter.setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid, Pen::ButtCap, Pen::BevelJoin) );
    painter.drawRect( RectF(PointF(700,  50), SizeF(200, 100)) );

    painter.setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid, Pen::ButtCap, Pen::MiterJoin) );
    painter.drawRect( RectF(PointF(700, 220), SizeF(200, 100)) );

    painter.setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid, Pen::ButtCap, Pen::RoundJoin) );
    painter.drawRect( RectF(PointF(700, 390), SizeF(200, 100)) );

    painter.setPen  ( Pen(Color::fromRgb8(255, 0, 0, 175), 6, Pen::Solid, Pen::ButtCap, Pen::NoJoin) );
    painter.drawRect( RectF(PointF(750, 100), SizeF(200, 100)) );
    painter.drawRect( RectF(PointF(750, 270), SizeF(200, 100)) );
    painter.drawRect( RectF(PointF(750, 440), SizeF(200, 100)) );

#else

    painter.setPen( Color::fromRgb8(255, 0, 0) );
    painter.setFont( Pt::Gfx::Font(FONT_SPEC_N) );

    painter.setBrush( Brush(textureWithWhiteBackground, 100, 0) );
    painter.fillRect( RectF(PointF(0+1, 0+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(0+0, 0+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(0, 0+30), "100, 0" );

    painter.setBrush( Brush(textureWithWhiteBackground, 0, 100) );
    painter.fillRect( RectF(PointF(250+1, 0+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(250+0, 0+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(250, 0+30), "0, 100" );

    painter.setBrush( Brush(textureWithWhiteBackground, 100, 100) );
    painter.fillRect( RectF(PointF(500+1, 0+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(500+0, 0+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(500, 0+30), "100, 100" );

    painter.setBrush( Brush(textureWithWhiteBackground, -100, 0) );
    painter.fillRect( RectF(PointF(0+1, 250+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(0+0, 250+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(0, 250+30), "-100, 0" );

    painter.setBrush( Brush(textureWithWhiteBackground, 0, -100) );
    painter.fillRect( RectF(PointF(250+1, 250+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(250+0, 250+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(250, 250+30), "0, -100" );

    painter.setBrush( Brush(textureWithWhiteBackground, -100, -100) );
    painter.fillRect( RectF(PointF(500+1, 250+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(500+0, 250+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(500, 250+30), "-100, -100" );

    painter.setBrush( Brush(textureWithWhiteBackground, 100, -100) );
    painter.fillRect( RectF(PointF(750+1, 0+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(750+0, 0+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(750, 30), "100, -100" );

    painter.setBrush( Brush(textureWithWhiteBackground, -100, 100) );
    painter.fillRect( RectF(PointF(750+1, 250+1), SizeF(200, 200)) );
    painter.drawRect( RectF(PointF(750+0, 250+0), SizeF(200+2, 200+2)) );
    painter.drawText( PointF(750, 250+30), "-100, 100" );

#endif

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawRoundRect(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasing(true);

    // Solid
    ip2->setBrush( Color::fromRgb8(255,   0, 255, 175) );
    ip2->fillRoundedRect( RectF(PointF( 20,  20), SizeF(200, 100)), 10 );

    ip2->setBrush( Color::fromRgb8(255, 255,   0, 175) );
    ip2->fillRoundedRect( RectF(PointF(240,  20), SizeF(200, 100)), 10 );

    // Gradient
    ip2->setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Horizontal) );
    ip2->fillRoundedRect( RectF(PointF( 20, 160), SizeF(200, 100)), 10 );

    ip2->setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Vertical) );
    ip2->fillRoundedRect( RectF(PointF(240, 160), SizeF(200, 100)), 10 );

    // Texture
    ip2->setBrush( Brush(textureWithWhiteBackground) );
    ip2->fillRoundedRect( RectF(PointF( 20, 290), SizeF(200, 100)), 10 );

    ip2->setBrush( Brush(textureWithTransBackground) );
    ip2->fillRoundedRect( RectF(PointF(240, 290), SizeF(200, 100)), 10 );

    // Outline - thin
    ip2->setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 1, Pen::Solid ) );
    ip2->drawRoundedRect( RectF(PointF(480,  20), SizeF(200, 100)), 10 );

    // Outline - thick
    ip2->setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid ));
    ip2->drawRoundedRect( RectF(PointF(480, 160), SizeF(200, 100)), 10 );

    ip2->setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid ));
    ip2->drawRoundedRect( RectF(PointF(480, 290), SizeF(200, 100)), 25 );

    ip2->setPen( Color::fromRgb8(255, 127, 127, 127) );
    ip2->drawRoundedRect( RectF(PointF(480, 160), SizeF(200, 100)), 10 );
    ip2->drawRoundedRect( RectF(PointF(480, 290), SizeF(200, 100)), 25 );

    // Outline - thin - patterned
    ip2->setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 1, Pen::DotDash, Pen::RoundCap, Pen::MiterJoin ) );
    ip2->drawRoundedRect( RectF(PointF(712,  20), SizeF(200, 100)), 10 );

    // Outline - thick - patterned
    ip2->setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::DotDash, Pen::RoundCap, Pen::MiterJoin ));
    ip2->drawRoundedRect( RectF(PointF(712, 160), SizeF(200, 100)), 10 );

    ip2->setPen( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::DotDash, Pen::RoundCap, Pen::MiterJoin ));
    ip2->drawRoundedRect( RectF(PointF(712, 290), SizeF(200, 100)), 25 );

    ip2->setPen( Color::fromRgb8(255, 127, 127, 127) );
    ip2->drawRoundedRect( RectF(PointF(712, 160), SizeF(200, 100)), 10 );
    ip2->drawRoundedRect( RectF(PointF(712, 290), SizeF(200, 100)), 25 );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawEllipseArc(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

    Pen draw( Color::fromRgb8(127, 255, 191, 175) );
    Pen text( Color::fromRgb8(255,   0,   0, 255) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( RectF( PointF(0, 80      ), SizeF(image.width(), 60) ) );
    painter.fillRect( RectF( PointF(0, 80 + 185), SizeF(image.width(), 30) ) );
    painter.fillRect( RectF( PointF(0, 80 + 400), SizeF(image.width(), 60) ) );

    // First row
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(draw); painter.drawEllipse( PointF (30, 50), SizeF(135, 135) );
    painter.setPen(text); painter.drawText( PointF(30, 30), "NOAA" );

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(draw); painter.drawEllipse( PointF (30 + 200, 50), SizeF(135, 135) );
    painter.setPen(text); painter.drawText( PointF(30 + 200, 30), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasing(false);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 400, 50), SizeF(135, 135), 0, 135);
        painter.setPen(text); ip2->drawText( PointF(30 + 400, 30), "NOAA" );

        ip2->setAntiAliasing(true);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 600, 50), SizeF(135, 135), 0, 135);
        painter.setPen(text); ip2->drawText( PointF(30 + 600, 30), "XWAA" );

        ip2->setAntiAliasing(true);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 800, 50), SizeF(135, 135), -135, 135);
        painter.setPen(text); ip2->drawText( PointF(30 + 800, 30), "XWAA" );
    }

    // Second row
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(draw); painter.drawEllipse( PointF (30, 50 + 200), SizeF(135, 67) );
    painter.setPen(text); painter.drawText( PointF(30, 30 + 200), "NOAA" );

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(draw); painter.drawEllipse( PointF (30 + 200, 50 + 200), SizeF(135, 67) );
    painter.setPen(text); painter.drawText( PointF(30 + 200, 30 + 200), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasing(false);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 400, 50 + 200), SizeF(135, 135), 120, 330);
        painter.setPen(text); ip2->drawText( PointF(30 + 400, 30 + 200), "NOAA" );

        ip2->setAntiAliasing(true);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 600, 50 + 200), SizeF(135, 135), 120, 330);
        painter.setPen(text); ip2->drawText( PointF(30 + 600, 30 + 200), "XWAA" );

        ip2->setAntiAliasing(true);
        painter.setPen(draw); ip2->drawChord( PointF (30 + 800, 50 + 200), SizeF(135, 135), -135, 135);
        painter.setPen(text); ip2->drawText( PointF(30 + 800, 30 + 200), "XWAA" );
    }

    // Third row
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(draw); painter.drawEllipse( PointF (30, 50 + 400), SizeF(67, 135) );
    painter.setPen(text); painter.drawText( PointF(30, 30 + 400), "NOAA" );

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(draw); painter.drawEllipse( PointF (30 + 200, 50 + 400), SizeF(67, 135) );
    painter.setPen(text); painter.drawText( PointF(30 + 200, 30 + 400), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasing(true);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 400, 50 + 400), SizeF(135, 135), -150, 30);
        painter.setPen(text); ip2->drawText( PointF(30 + 400, 30 + 400), "NOAA" );

        ip2->setAntiAliasing(true);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 600, 50 + 400), SizeF(135, 135), -150, 30);
        painter.setPen(text); ip2->drawText( PointF(30 + 600, 30 + 400), "XWAA" );

        ip2->setAntiAliasing(true);
        painter.setPen(draw); ip2->drawPie( PointF (30 + 800, 50 + 400), SizeF(135, 135), -135, 135);
        painter.setPen(text); ip2->drawText( PointF(30 + 800, 30 + 400), "XWAA" );
    }

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
