static void testDrawLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingLevel(1);

#if 1
    painter.setClip( RectF (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Color::fromRgb8(0, 255, 0, 175) );
    painter.drawLine( PointF(  0,   0), PointF(999, 599) );
    painter.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );
#endif

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

    painter.setPen( Color::fromRgb8(0, 255, 255, 255) ); painter.drawText( PointF(100, 175), "Hello world!" );
    painter.setPen( Color::fromRgb8(0, 255, 255, 175) ); painter.drawText( PointF(100, 225), "Hello world!" );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawRect(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(ip2) ip2->setAntiAliasingLevel(1);

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

static void testDrawFillPolygon(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

#if 0
    painter.setBrush( Color::fromRgb8(255, 255, 255) );
    const Pt::int32_t sx = 5;
    const PointF polyx[] = { PointF(10 * sx, 10 * sx), PointF(40 * sx, 40 * sx), PointF(30 * sx, 60 * sx) };
    if(ip2) ip2->setAntiAliasingLevel(2);
    painter.fillPolygon( polyx, sizeof(polyx) / sizeof(polyx[0]) );
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
    return;
#endif

    painter.setPen( Color::fromRgb8(255, 0, 0) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( RectF( PointF(800, 120), SizeF(170, 130) ) );
    painter.fillRect( RectF( PointF(170, 250), SizeF(800, 200) ) );
    painter.fillRect( RectF( PointF(170, 450), SizeF(100, 100) ) );


    painter.setBrush( brush1 );

    const PointF poly1a[] = { // CCW
        PointF(100 + 5, 100),
        PointF(300 + 5, 350),
        PointF(400 + 5, 250),
        PointF(200 + 5, 100),
        PointF(  0 + 5,  50)
    };
    if(ip2) ip2->setAntiAliasingLevel(0);
    painter.fillPolygon( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );
    painter.drawText( PointF(30, 50), "NOAA" );

    const PointF poly1b[] = { // CCW
        PointF(100 + 198, 100),
        PointF(300 + 198, 350),
        PointF(400 + 198, 250),
        PointF(200 + 198, 100),
        PointF(  0 + 198,  50)
    };
    if(ip2) ip2->setAntiAliasingLevel(1);
    painter.fillPolygon( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );
    painter.drawText( PointF(30 + 198, 50), "FSAA 2x2" );

    const PointF poly1c[] = { // CCW
        PointF(100 + 391, 100),
        PointF(300 + 391, 350),
        PointF(400 + 391, 250),
        PointF(200 + 391, 100),
        PointF(  0 + 391,  50)
    };
    if(ip2) ip2->setAntiAliasingLevel(2);
    painter.fillPolygon( poly1c, sizeof(poly1b) / sizeof(poly1b[0]) );
    painter.drawText( PointF(30 + 391, 50), "FSAA 4x4" );


    painter.setBrush( brush2 );

    const PointF poly2a[] = { // CCW
        PointF(110, 310),
        PointF(160, 340),
        PointF(210, 310),
        PointF(140, 260)
    };
    if(ip2) ip2->setAntiAliasingLevel(0);
    painter.fillPolygon( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );
    painter.drawText( PointF(10, 250 + 100), "NOAA" );

    const PointF poly2b[] = { // CCW
        PointF(110, 310 + 100),
        PointF(160, 340 + 100),
        PointF(210, 310 + 100),
        PointF(140, 260 + 100)
    };
    if(ip2) ip2->setAntiAliasingLevel(1);
    painter.fillPolygon( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );
    painter.drawText( PointF(10, 250 + 200), "FSAA 2x2" );

    const PointF poly2c[] = { // CCW
        PointF(110, 310 + 200),
        PointF(160, 340 + 200),
        PointF(210, 310 + 200),
        PointF(140, 260 + 200)
    };
    if(ip2) ip2->setAntiAliasingLevel(2);
    painter.fillPolygon( poly2c, sizeof(poly2b) / sizeof(poly2b[0]) );
    painter.drawText( PointF(10, 250 + 300), "FSAA 4x4" );


    painter.setBrush( brush1 );

    const PointF poly3a[] = { // CCW
        // Outside
        PointF(400 - 20, 300),
        PointF(500 - 20, 400),
        PointF(400 - 20, 500),
        PointF(300 - 20, 400),
        // Inside
        PointF(320 - 20, 400),
        PointF(400 - 20, 480),
        PointF(480 - 20, 400),
        PointF(400 - 20, 320)
    };
    if(ip2) ip2->setAntiAliasingLevel(0);
    painter.fillPolygon( poly3a, sizeof(poly3a) / sizeof(poly3a[0]) );
    painter.drawText( PointF(330, 530), "NOAA" );

    const PointF poly3b[] = { // CCW
        // Outside
        PointF(400 + 220, 300),
        PointF(500 + 220, 400),
        PointF(400 + 220, 500),
        PointF(300 + 220, 400),
        // Inside
        PointF(320 + 220, 400),
        PointF(400 + 220, 480),
        PointF(480 + 220, 400),
        PointF(400 + 220, 320)
    };
    if(ip2) ip2->setAntiAliasingLevel(1);
    painter.fillPolygon( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );
    painter.drawText( PointF(330 + 220, 530), "FSAA 2x2" );

    const PointF poly3c[] = { // CCW
        // Outside
        PointF(400 + 460, 300),
        PointF(500 + 460, 400),
        PointF(400 + 460, 500),
        PointF(300 + 460, 400),
        // Inside
        PointF(320 + 460, 400),
        PointF(400 + 460, 480),
        PointF(480 + 460, 400),
        PointF(400 + 460, 320)
    };
    if(ip2) ip2->setAntiAliasingLevel(2);
    painter.fillPolygon( poly3c, sizeof(poly3c) / sizeof(poly3c[0]) );
    painter.drawText( PointF(330 + 460, 530), "FSAA 4x4" );


    painter.setBrush( brush2 );

    const PointF poly4[] = { // CCW
        // Outside
        PointF(840,  30),
        PointF(960, 110),
        PointF(880, 230),
        PointF(760, 150),
        ImagePainter2::PolygonSeparatorPointF,
        // Hole 1
        PointF(860 - 40 +  0, 130 - 20     ),
        PointF(860 - 40 + 30, 130 - 20 + 15),
        PointF(860 - 40 -  0, 130 - 20 + 40),
        PointF(860 - 40 - 20, 130 - 20 + 20),
        ImagePainter2::PolygonSeparatorPointF,
        // Hole 2
        PointF(860 + 40 +  0, 130 - 20     ),
        PointF(860 + 40 + 20, 130 - 20 + 20),
        PointF(860 + 40 -  0, 130 - 20 + 40),
        PointF(860 + 40 - 30, 130 - 20 + 15)
    };
    if(ip2) ip2->setAntiAliasingLevel(3);
    painter.fillPolygon( poly4, sizeof(poly4) / sizeof(poly4[0]) );
    painter.drawText( PointF(680, 70), "FSAA 8x8" );


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

static void testDrawFillEllipse(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

    painter.setPen( Color::fromRgb8(255, 0, 0) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( RectF( PointF(0, 80), SizeF(image.width(), 60) ) );
    painter.fillRect( RectF( PointF(0, 80 + 185), SizeF(image.width(), 30) ) );
    painter.fillRect( RectF( PointF(0, 80 + 400), SizeF(image.width(), 60) ) );


    painter.setBrush( brush1 );

    if(ip2) ip2->setAntiAliasingLevel(0);
    painter.fillEllipse( PointF (30, 50), SizeF(120, 120) );
    painter.drawText( PointF(30, 30), "NOAA" );

    if(ip2) ip2->setAntiAliasingLevel(1);
    painter.fillEllipse( PointF (30 + 200, 50), SizeF(120, 120) );
    painter.drawText( PointF(30 + 200, 30), "FSAA 2x2" );

    if(ip2) ip2->setAntiAliasingLevel(2);
    painter.fillEllipse( PointF (30 + 400, 50), SizeF(120, 120) );
    painter.drawText( PointF(30 + 400, 30), "FSAA 4x4" );

    if(ip2) ip2->setAntiAliasingLevel(4);
    painter.fillEllipse( PointF (30 + 600, 50), SizeF(120, 120) );
    painter.drawText( PointF(30 + 600, 30), "FSAA 8x8" );

    if(ip2) {
        painter.setBrush( brush2 );
        ip2->setAntiAliasingLevel( 0 );
        ip2->fillArc( PointF (30 + 800, 50), SizeF(120, 120), 0, 180 - 45, true );
        ip2->drawText( PointF(30 + 800, 30), "NOAA" );
    }


    painter.setBrush( brush2 );

    if(ip2) ip2->setAntiAliasingLevel(0);
    painter.fillEllipse( PointF (30, 50 + 200), SizeF(120, 60) );
    painter.drawText( PointF(30, 30 + 200), "NOAA" );

    if(ip2) ip2->setAntiAliasingLevel(1);
    painter.fillEllipse( PointF (30 + 200, 50 + 200), SizeF(120, 60) );
    painter.drawText( PointF(30 + 200, 30 + 200), "FSAA 2x2" );

    if(ip2) ip2->setAntiAliasingLevel(2);
    painter.fillEllipse( PointF (30 + 400, 50 + 200), SizeF(120, 60) );
    painter.drawText( PointF(30 + 400, 30 + 200), "FSAA 4x4" );

    if(ip2) ip2->setAntiAliasingLevel(3);
    painter.fillEllipse( PointF (30 + 600, 50 + 200), SizeF(120, 60) );
    painter.drawText( PointF(30 + 600, 30 + 200), "FSAA 8x8" );

    if(ip2) {
        painter.setBrush( brush1 );
        ip2->setAntiAliasingLevel( 1 );
        ip2->fillArc( PointF (30 + 800, 50 + 200), SizeF(120, 120), 90 + 30, 360 - 30, true );
        ip2->drawText( PointF(30 + 800, 30 + 200), "FSAA 2x2" );
    }


    painter.setBrush( brush2 );

    if(ip2) ip2->setAntiAliasingLevel(0);
    painter.fillEllipse( PointF (30, 50 + 400), SizeF(60, 120) );
    painter.drawText( PointF(30, 30 + 400), "NOAA" );

    if(ip2) ip2->setAntiAliasingLevel(1);
    painter.fillEllipse( PointF (30 + 200, 50 + 400), SizeF(60, 120) );
    painter.drawText( PointF(30 + 200, 30 + 400), "FSAA 2x2" );

    if(ip2) ip2->setAntiAliasingLevel(2);
    painter.fillEllipse( PointF (30 + 400, 50 + 400), SizeF(60, 120) );
    painter.drawText( PointF(30 + 400, 30 + 400), "FSAA 4x4" );

    if(ip2) ip2->setAntiAliasingLevel(3);
    painter.fillEllipse( PointF (30 + 600, 50 + 400), SizeF(60, 120) );
    painter.drawText( PointF(30 + 600, 30 + 400), "FSAA 8x8" );

    if(ip2) {
        painter.setBrush( brush1 );
        ip2->setAntiAliasingLevel( 2 );
        ip2->fillArc( PointF (30 + 800, 50 + 400), SizeF(120, 120), 90 + 30, 360 - 30, false );
        ip2->drawText( PointF(30 + 800, 30 + 400), "FSAA 4x4" );
    }


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
