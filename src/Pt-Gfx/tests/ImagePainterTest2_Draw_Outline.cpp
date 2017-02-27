static void testDrawLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);

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

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawPatternedLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);

    painter.setClip( RectF (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Pen( Color::fromRgb8(0, 255, 0, 175), 1, Pen::Dash )  );
    painter.drawLine( PointF(  0,   0), PointF(999, 599) );
    painter.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );

    painter.setPen( Pen( Color::fromRgb8(255, 0, 0, 175), 1, Pen::Dash )  );
    painter.drawLine( PointF(  0,   0), PointF(999,   0) );
    painter.drawLine( PointF(  0, 599), PointF(999, 599) );
    painter.drawLine( PointF(  0,   0), PointF(  0, 599) );
    painter.drawLine( PointF(999,   0), PointF(999, 599) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dash )  );
    painter.drawLine( PointF( 10, 110), PointF(100, 150) );
    painter.drawLine( PointF( 10, 250), PointF(100, 210) );
    painter.drawLine( PointF(100,  10), PointF(110,  20) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dash )  );
    painter.drawLine( PointF(500, 500), PointF(200, 200) );
    painter.drawLine( PointF(300, 150), PointF(700,  50) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDash )  );
    painter.drawLine( PointF(550, 500), PointF(250, 200) );
    painter.drawLine( PointF(300, 200), PointF(700, 100) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::Dash )  );
    painter.drawLine( PointF(870,  11), PointF(870, 500) );
    painter.drawLine( PointF(880,  11), PointF(882, 500) );
    painter.drawLine( PointF( 10, 540), PointF(781, 540) );
    painter.drawLine( PointF(781, 552), PointF( 10, 550) );

    painter.setPen( Pen( Color::fromRgb8(255, 255, 255, 175), 1, Pen::DoubleDash )  );
    painter.drawLine( PointF(970,  11), PointF(970, 500) );
    painter.drawLine( PointF(980,  11), PointF(982, 500) );
    painter.drawLine( PointF( 10, 440), PointF(781, 440) );
    painter.drawLine( PointF(781, 452), PointF( 10, 450) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawRect(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);

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


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawEllipse(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

    Pen draw( Color::fromRgb8(127, 255, 191, 175) );
    Pen text( Color::fromRgb8(255,   0,   0, 255) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( RectF( PointF(0, 80), SizeF(image.width(), 60) ) );
    painter.fillRect( RectF( PointF(0, 80 + 185), SizeF(image.width(), 30) ) );
    painter.fillRect( RectF( PointF(0, 80 + 400), SizeF(image.width(), 60) ) );


    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.setPen(draw); painter.drawEllipse( PointF (30, 50), SizeF(135, 135) );
    painter.setPen(text); painter.drawText( PointF(30, 30), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
    painter.setPen(draw); painter.drawEllipse( PointF (30 + 200, 50), SizeF(135, 135) );
    painter.setPen(text); painter.drawText( PointF(30 + 200, 30), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 400, 50), SizeF(135, 135), 0, 135, ArcMode::Open );
        painter.setPen(text); ip2->drawText( PointF(30 + 400, 30), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 600, 50), SizeF(135, 135), 0, 135, ArcMode::Open );
        painter.setPen(text); ip2->drawText( PointF(30 + 600, 30), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 800, 50), SizeF(135, 135), -135, 135, ArcMode::Open );
        painter.setPen(text); ip2->drawText( PointF(30 + 800, 30), "XWAA" );
    }

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.setPen(draw); painter.drawEllipse( PointF (30, 50 + 200), SizeF(135, 67) );
    painter.setPen(text); painter.drawText( PointF(30, 30 + 200), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
    painter.setPen(draw); painter.drawEllipse( PointF (30 + 200, 50 + 200), SizeF(135, 67) );
    painter.setPen(text); painter.drawText( PointF(30 + 200, 30 + 200), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 400, 50 + 200), SizeF(135, 135), 120, 330, ArcMode::Open );
        painter.setPen(text); ip2->drawText( PointF(30 + 400, 30 + 200), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 600, 50 + 200), SizeF(135, 135), 120, 330, ArcMode::Open );
        painter.setPen(text); ip2->drawText( PointF(30 + 600, 30 + 200), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 800, 50 + 200), SizeF(135, 135), -135, 135, ArcMode::Chord );
        painter.setPen(text); ip2->drawText( PointF(30 + 800, 30 + 200), "XWAA" );
    }


    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.setPen(draw); painter.drawEllipse( PointF (30, 50 + 400), SizeF(67, 135) );
    painter.setPen(text); painter.drawText( PointF(30, 30 + 400), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
    painter.setPen(draw); painter.drawEllipse( PointF (30 + 200, 50 + 400), SizeF(67, 135) );
    painter.setPen(text); painter.drawText( PointF(30 + 200, 30 + 400), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 400, 50 + 400), SizeF(135, 135), -150, 30, ArcMode::Open );
        painter.setPen(text); ip2->drawText( PointF(30 + 400, 30 + 400), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 600, 50 + 400), SizeF(135, 135), -150, 30, ArcMode::Open );
        painter.setPen(text); ip2->drawText( PointF(30 + 600, 30 + 400), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Fastest);
        painter.setPen(draw); ip2->drawArc( PointF (30 + 800, 50 + 400), SizeF(135, 135), -135, 135, ArcMode::Pie );
        painter.setPen(text); ip2->drawText( PointF(30 + 800, 30 + 400), "XWAA" );
    }


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
