static void testDrawSolidLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

#if 1
    painter.setClip( Rect (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Color::fromRgb8(0, 255, 0, 175) );
    painter.drawLine( Point(  0,   0), Point(999, 599) );
    painter.setClip( Rect (0, image.width() - 1, 0, image.height() - 1) );
#endif

    painter.drawImage(Point(image.width() - textureWithWhiteBackground.width(),   0), textureWithWhiteBackground);
    painter.drawImage(Point(image.width() - textureWithWhiteBackground.width(), 300), textureWithWhiteBackground, Rect(20, 70, 20, 70));

    painter.setPen( Color::fromRgb8(255, 0, 0, 175) );

    painter.drawLine( Point(  0,   0), Point(999,   0) );
    painter.drawLine( Point(  0, 599), Point(999, 599) );
    painter.drawLine( Point(  0,   0), Point(  0, 599) );
    painter.drawLine( Point(999,   0), Point(999, 599) );

    painter.setPen( Color::fromRgb8(255, 255, 255, 175) );

    painter.drawLine( Point( 10, 110), Point(100, 150) );
    painter.drawLine( Point( 10, 250), Point(100, 210) );

    painter.drawLine( Point(100,  10), Point(110,  20) );

    painter.setPen( Color::fromRgb8( 63,  63,  63, 175) ); painter.drawLine( Point(500-50, 500), Point(200-50, 200) );
    painter.setPen( Color::fromRgb8(127, 127, 127, 175) ); painter.drawLine( Point(500   , 500), Point(200   , 200) );
    painter.setPen( Color::fromRgb8(255, 255, 255, 175) ); painter.drawLine( Point(500+50, 500), Point(200+50, 200) );
                                                           painter.drawLine( Point(300   , 200), Point(700   , 100) );

    painter.drawLine( Point(770,  11), Point(770, 500) );
    painter.drawLine( Point(780,  11), Point(782, 500) );

    painter.drawLine( Point( 10, 540), Point(781, 540) );
    painter.drawLine( Point(781, 552), Point( 10, 550) );

    painter.setFont( Pt::Gfx::Font(FONT_SPEC_R) );
    painter.setPen( Color::fromRgb8(0, 255, 255, 255) ); painter.drawText( Point(100 + 200, 175 + 200), "Hello world!" );
    painter.setPen( Color::fromRgb8(0, 255, 255, 175) ); painter.drawText( Point(100 + 200, 225 + 200), "Hello world!" );

    painter.setFont( Pt::Gfx::Font(FONT_SPEC_N) );
    painter.setPen( Color::fromRgb8(0, 255, 255, 255) ); painter.drawText( Point(100, 175), "Hello world!" );
    painter.setPen( Color::fromRgb8(0, 255, 255, 175) ); painter.drawText( Point(100, 225), "Hello world!" );

    if(ip2) {
        const Point bezier1a[] = { // CCW
            Point(300, 100),
            Point(265,  65),
            Point(200,  50)
        };
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Solid ) );
        ip2->drawQuadraticPolybezier( bezier1a, sizeof(bezier1a) / sizeof(bezier1a[0]), false );

        const Point bezier1b[] = { // CCW
            Point(300 + 100, 100),
            Point(265 + 100,  65),
            Point(200 + 100,  50)
        };
        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Solid ) );
        ip2->drawQuadraticPolybezier( bezier1b, sizeof(bezier1b) / sizeof(bezier1b[0]), false );
    }

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawPatternedLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    painter.setClip( Rect (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Pen( Color::fromRgb8(0, 255, 0, 175), 1, Pen::Dash ) );
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None    ); painter.drawLine( Point(  0, -30), Point(999, 599 - 30) );
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard); painter.drawLine( Point(  0,  30), Point(999, 599 + 30) );
    painter.setClip( Rect (0, image.width() - 1, 0, image.height() - 1) );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    painter.setPen( Pen( Color::fromRgb8(255, 0, 0, 175), 1, Pen::DotDash ) );
    painter.drawLine( Point(  5,   5), Point(994,   5) );
    painter.drawLine( Point(  5, 594), Point(994, 594) );
    painter.drawLine( Point(  5,   5), Point(  5, 594) );
    painter.drawLine( Point(994,   5), Point(994, 594) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dash ) );
    painter.drawLine( Point( 10, 110), Point(100, 150) );
    painter.drawLine( Point( 10, 250), Point(100, 210) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dot ) );
    painter.drawLine( Point(500, 500), Point(200, 200) );
    painter.drawLine( Point(300, 150), Point(700,  50) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDash ) );
    painter.drawLine( Point(550, 500), Point(250, 200) );
    painter.drawLine( Point(300, 200), Point(700, 100) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dot ) );
    //painter.drawLine( Point(870,  11), Point(870, 500) );
    painter.drawLine( Point(880,  11), Point(882, 500) );
    painter.drawLine( Point( 10, 540), Point(781, 540) );
    painter.drawLine( Point(781, 552), Point( 10, 550) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDot ) );
    painter.drawLine( Point(970,  11), Point(970, 500) );
    painter.drawLine( Point(980,  11), Point(982, 500) );
    painter.drawLine( Point( 10, 440), Point(781, 440) );
    painter.drawLine( Point(781, 452), Point( 10, 450) );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.setPen( Pen( Color::fromRgb8(0, 255, 255, 175), 1, Pen::Dash ) );

    const Point poly[] = { // CCW
        Point(110 - 0, 310 - 70),
        Point(160 - 0, 340 - 70),
        Point(210 - 0, 310 - 70),
        Point(140 - 0, 260 - 70)
    };
    if(ip2) ip2->drawPolyline( poly, sizeof(poly) / sizeof(poly[0]), true );
    else painter.drawPolyline( poly, sizeof(poly) / sizeof(poly[0]) );

    painter.drawRect( Rect(Point(550, 170), Size(100, 50)) );

    painter.drawEllipse( Point (730,  50), Size(95, 95) );
    if(ip2) ip2->drawArc( Point (730, 170), Size(95, 43), 30, 330, ArcMode::Chord );
    if(ip2) ip2->drawArc( Point (753, 243), Size(43, 95), 30, 330, ArcMode::Pie );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, 0xB38F0F83F03F8000 ) ); // 1011001110001111000011111000001111110000001111111000000000000000
    painter.drawRect( Rect(Point(260, 20), Size(180, 80)) );

    if(ip2) {
        const Point bezier1a[] = { // CCW
            // Bottom left
            Point(400 - 350,  90 + 300),
            Point(400 - 350, 110 + 300),
            Point(420 - 350, 110 + 300),
            // Bottom middle
            Point(450 - 350, 110 + 300),
            // Bottom right
            Point(480 - 350, 110 + 300),
            Point(500 - 350, 110 + 300),
            Point(500 - 350,  90 + 300),
            // Center right
            Point(500 - 350,  70 + 300),
            // Top right
            Point(500 - 350,  50 + 300),
            Point(500 - 350,  30 + 300),
            Point(480 - 350,  30 + 300),
            // Top middle
            Point(450 - 350,  30 + 300),
            // Top left
            Point(420 - 350,  30 + 300),
            Point(400 - 350,  30 + 300),
            Point(400 - 350,  50 + 300),
            // Center left
            Point(400 - 350,  70 + 300)
        };
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Dash ) );
        ip2->drawQuadraticPolybezier( bezier1a, sizeof(bezier1a) / sizeof(bezier1a[0]), true );

        const Point bezier1b[] = { // CCW
            // Bottom left
            Point(400 - 200,  90 + 300),
            Point(400 - 200, 110 + 300),
            Point(420 - 200, 110 + 300),
            // Bottom middle
            Point(450 - 200, 110 + 300),
            // Bottom right
            Point(480 - 200, 110 + 300),
            Point(500 - 200, 110 + 300),
            Point(500 - 200,  90 + 300),
            // Center right
            Point(500 - 200,  70 + 300),
            // Top right
            Point(500 - 200,  50 + 300),
            Point(500 - 200,  30 + 300),
            Point(480 - 200,  30 + 300),
            // Top middle
            Point(450 - 200,  30 + 300),
            // Top left
            Point(420 - 200,  30 + 300),
            Point(400 - 200,  30 + 300),
            Point(400 - 200,  50 + 300),
            // Center left
            Point(400 - 200,  70 + 300)
        };
        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->setPen( Pen( Color::fromRgb8(255, 127, 255, 175), 1, Pen::Dash ) );
        ip2->drawQuadraticPolybezier( bezier1b, sizeof(bezier1b) / sizeof(bezier1b[0]), true );
    }

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawRect(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    // Solid
    painter.setPen  ( Color::fromRgb8(255, 255,   0, 175) );
    painter.setBrush( Color::fromRgb8(255,   0, 255, 175) );
    painter.drawRect( Rect(Point(100,  50), Size(200, 100)) );
    painter.fillRect( Rect(Point(400,  50), Size(200, 100)) );

    painter.setPen  ( Color::fromRgb8(  0, 255, 255, 175) );
    painter.setBrush( Color::fromRgb8(255, 255,   0, 175) );
    painter.drawRect( Rect(Point(150, 100), Size(200, 100)) );
    painter.fillRect( Rect(Point(450, 100), Size(200, 100)) );

    // Gradient
    painter.setBrush( Color::fromRgb8(255, 255, 255, 175) );
    painter.fillRect( Rect(Point(100, 220), Size(200, 100)) );
    painter.fillRect( Rect(Point(400, 220), Size(200, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Horizontal) );
    painter.fillRect( Rect(Point(150, 270), Size(200, 100)) );

    painter.setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Vertical) );
    painter.fillRect( Rect(Point(450, 270), Size(200, 100)) );

    // Texture
    painter.setBrush( Color::fromRgb8(255, 255, 255, 175) );
    painter.fillRect( Rect(Point(100, 390), Size(200, 100)) );
    painter.fillRect( Rect(Point(400, 390), Size(200, 100)) );

    painter.setBrush( Brush(textureWithWhiteBackground) );
    painter.fillRect( Rect(Point(150, 440), Size(200, 100)) );

    painter.setBrush( Brush(textureWithTransBackground) );
    painter.fillRect( Rect(Point(450, 440), Size(200, 100)) );

    // Thick
    painter.setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid, Pen::ButtCap, Pen::BevelJoin) );
    painter.drawRect( Rect(Point(700,  50), Size(200, 100)) );

    painter.setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid, Pen::ButtCap, Pen::MiterJoin) );
    painter.drawRect( Rect(Point(700, 220), Size(200, 100)) );

    painter.setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid, Pen::ButtCap, Pen::RoundJoin) );
    painter.drawRect( Rect(Point(700, 390), Size(200, 100)) );

    painter.setPen  ( Pen(Color::fromRgb8(255, 0, 0, 175), 6, Pen::Solid, Pen::ButtCap, Pen::NoJoin) );
    painter.drawRect( Rect(Point(750, 100), Size(200, 100)) );
    painter.drawRect( Rect(Point(750, 270), Size(200, 100)) );
    painter.drawRect( Rect(Point(750, 440), Size(200, 100)) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawRoundRect(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    // Solid
    ip2->setBrush( Color::fromRgb8(255,   0, 255, 175) );
    ip2->fillRoundRect( Rect(Point( 20,  20), Size(200, 100)), 10 );

    ip2->setBrush( Color::fromRgb8(255, 255,   0, 175) );
    ip2->fillRoundRect( Rect(Point(240,  20), Size(200, 100)), 10 );

    // Gradient
    ip2->setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Horizontal) );
    ip2->fillRoundRect( Rect(Point( 20, 160), Size(200, 100)), 10 );

    ip2->setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Vertical) );
    ip2->fillRoundRect( Rect(Point(240, 160), Size(200, 100)), 10 );

    // Texture
    ip2->setBrush( Brush(textureWithWhiteBackground) );
    ip2->fillRoundRect( Rect(Point( 20, 290), Size(200, 100)), 10 );

    ip2->setBrush( Brush(textureWithTransBackground) );
    ip2->fillRoundRect( Rect(Point(240, 290), Size(200, 100)), 10 );

    // Outline - thin
    ip2->setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 1, Pen::Solid ) );
    ip2->drawRoundRect( Rect(Point(480,  20), Size(200, 100)), 10 );

    // Outline - thick
    ip2->setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid ));
    ip2->drawRoundRect( Rect(Point(480, 160), Size(200, 100)), 10 );

    ip2->setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::Solid ));
    ip2->drawRoundRect( Rect(Point(480, 290), Size(200, 100)), 25 );

    ip2->setPen  ( Color::fromRgb8(255, 127, 127, 127) );
    ip2->drawRoundRect( Rect(Point(480, 160), Size(200, 100)), 10 );
    ip2->drawRoundRect( Rect(Point(480, 290), Size(200, 100)), 25 );

    // Outline - thin - patterned
    ip2->setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 1, Pen::DotDash, Pen::RoundCap, Pen::MiterJoin ) );
    ip2->drawRoundRect( Rect(Point(712,  20), Size(200, 100)), 10 );

    // Outline - thick - patterned
    ip2->setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::DotDash, Pen::RoundCap, Pen::MiterJoin ));
    ip2->drawRoundRect( Rect(Point(712, 160), Size(200, 100)), 10 );

    ip2->setPen  ( Pen(Color::fromRgb8(0, 255, 0, 175), 12, Pen::DotDash, Pen::RoundCap, Pen::MiterJoin ));
    ip2->drawRoundRect( Rect(Point(712, 290), Size(200, 100)), 25 );

    ip2->setPen  ( Color::fromRgb8(255, 127, 127, 127) );
    ip2->drawRoundRect( Rect(Point(712, 160), Size(200, 100)), 10 );
    ip2->drawRoundRect( Rect(Point(712, 290), Size(200, 100)), 25 );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawEllipseArc(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

    Pen draw( Color::fromRgb8(127, 255, 191, 175) );
    Pen text( Color::fromRgb8(255,   0,   0, 255) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( Rect( Point(0, 80), Size(image.width(), 60) ) );
    painter.fillRect( Rect( Point(0, 80 + 185), Size(image.width(), 30) ) );
    painter.fillRect( Rect( Point(0, 80 + 400), Size(image.width(), 60) ) );

    // First row
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.setPen(draw); painter.drawEllipse( Point (30, 50), Size(135, 135) );
    painter.setPen(text); painter.drawText( Point(30, 30), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.setPen(draw); painter.drawEllipse( Point (30 + 200, 50), Size(135, 135) );
    painter.setPen(text); painter.drawText( Point(30 + 200, 30), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        painter.setPen(draw); ip2->drawArc( Point (30 + 400, 50), Size(135, 135), 0, 135, ArcMode::Open );
        painter.setPen(text); ip2->drawText( Point(30 + 400, 30), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        painter.setPen(draw); ip2->drawArc( Point (30 + 600, 50), Size(135, 135), 0, 135, ArcMode::Open );
        painter.setPen(text); ip2->drawText( Point(30 + 600, 30), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        painter.setPen(draw); ip2->drawArc( Point (30 + 800, 50), Size(135, 135), -135, 135, ArcMode::Open );
        painter.setPen(text); ip2->drawText( Point(30 + 800, 30), "XWAA" );
    }

    // Second row
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.setPen(draw); painter.drawEllipse( Point (30, 50 + 200), Size(135, 67) );
    painter.setPen(text); painter.drawText( Point(30, 30 + 200), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.setPen(draw); painter.drawEllipse( Point (30 + 200, 50 + 200), Size(135, 67) );
    painter.setPen(text); painter.drawText( Point(30 + 200, 30 + 200), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        painter.setPen(draw); ip2->drawArc( Point (30 + 400, 50 + 200), Size(135, 135), 120, 330, ArcMode::Open );
        painter.setPen(text); ip2->drawText( Point(30 + 400, 30 + 200), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        painter.setPen(draw); ip2->drawArc( Point (30 + 600, 50 + 200), Size(135, 135), 120, 330, ArcMode::Open );
        painter.setPen(text); ip2->drawText( Point(30 + 600, 30 + 200), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        painter.setPen(draw); ip2->drawArc( Point (30 + 800, 50 + 200), Size(135, 135), -135, 135, ArcMode::Chord );
        painter.setPen(text); ip2->drawText( Point(30 + 800, 30 + 200), "XWAA" );
    }

    // Third row
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.setPen(draw); painter.drawEllipse( Point (30, 50 + 400), Size(67, 135) );
    painter.setPen(text); painter.drawText( Point(30, 30 + 400), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.setPen(draw); painter.drawEllipse( Point (30 + 200, 50 + 400), Size(67, 135) );
    painter.setPen(text); painter.drawText( Point(30 + 200, 30 + 400), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        painter.setPen(draw); ip2->drawArc( Point (30 + 400, 50 + 400), Size(135, 135), -150, 30, ArcMode::Open );
        painter.setPen(text); ip2->drawText( Point(30 + 400, 30 + 400), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        painter.setPen(draw); ip2->drawArc( Point (30 + 600, 50 + 400), Size(135, 135), -150, 30, ArcMode::Open );
        painter.setPen(text); ip2->drawText( Point(30 + 600, 30 + 400), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        painter.setPen(draw); ip2->drawArc( Point (30 + 800, 50 + 400), Size(135, 135), -135, 135, ArcMode::Pie );
        painter.setPen(text); ip2->drawText( Point(30 + 800, 30 + 400), "XWAA" );
    }

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
