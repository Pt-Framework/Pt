static void testDrawFillPolygon(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

#if 0
    painter.setBrush( Color::fromRgb8(255, 255, 255) );
    const Pt::int32_t sx = 5;
    const Point polyx[] = { Point(10 * sx, 10 * sx), Point(40 * sx, 40 * sx), Point(30 * sx, 60 * sx) };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( polyx, sizeof(polyx) / sizeof(polyx[0]) );
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), false);
    return;
#endif

    painter.setPen( Color::fromRgb8(255, 0, 0) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( Rect( Point(800, 120), Size(170, 130) ) );
    painter.fillRect( Rect( Point(170, 250), Size(800, 200) ) );
    painter.fillRect( Rect( Point(170, 450), Size(100, 100) ) );

    // Pointy polygons
    painter.setBrush( brush1 );

    const Point poly1a[] = { // CCW
        Point(100 + 5, 100),
        Point(300 + 5, 350),
        Point(400 + 5, 250),
        Point(200 + 5, 100),
        Point(  0 + 5,  50)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillPolygon( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );
    painter.drawText( Point(30, 50), "NOAA" );

    const Point poly1b[] = { // CCW
        Point(100 + 198, 100),
        Point(300 + 198, 350),
        Point(400 + 198, 250),
        Point(200 + 198, 100),
        Point(  0 + 198,  50)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );
    painter.drawText( Point(30 + 198, 50), "XWAA" );

    const Point poly1c[] = { // CCW
        Point(100 + 391, 100),
        Point(300 + 391, 350),
        Point(400 + 391, 250),
        Point(200 + 391, 100),
        Point(  0 + 391,  50)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::LowMemory);
    painter.fillPolygon( poly1c, sizeof(poly1b) / sizeof(poly1b[0]) );
    painter.drawText( Point(30 + 391, 50), "FSAA2x2" );

    // Diamond-like polygons
    painter.setBrush( brush2 );

    const Point poly2a[] = { // CCW
        Point(110, 310),
        Point(160, 340),
        Point(210, 310),
        Point(140, 260)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillPolygon( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );
    painter.drawText( Point(10, 250 + 100), "NOAA" );

    const Point poly2b[] = { // CCW
        Point(110, 310 + 100),
        Point(160, 340 + 100),
        Point(210, 310 + 100),
        Point(140, 260 + 100)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );
    painter.drawText( Point(10, 250 + 200), "XWAA" );

    const Point poly2c[] = { // CCW
        Point(110, 310 + 200),
        Point(160, 340 + 200),
        Point(210, 310 + 200),
        Point(140, 260 + 200)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::LowMemory);
    painter.fillPolygon( poly2c, sizeof(poly2c) / sizeof(poly2c[0]) );
    painter.drawText( Point(10, 250 + 300), "FSAA2x2" );

    // Complex U-like polygons
    painter.setBrush( brush1 );

    const Point poly3a[] = { // CCW
        // Outside
        Point(400 - 20, 300),
        Point(500 - 20, 400),
        Point(400 - 20, 500),
        Point(300 - 20, 400),
        // Inside
        Point(320 - 20, 400),
        Point(400 - 20, 480),
        Point(480 - 20, 400),
        Point(400 - 20, 320)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillPolygon( poly3a, sizeof(poly3a) / sizeof(poly3a[0]) );
    painter.drawText( Point(330, 530), "NOAA" );

    const Point poly3b[] = { // CCW
        // Outside
        Point(400 + 220, 300),
        Point(500 + 220, 400),
        Point(400 + 220, 500),
        Point(300 + 220, 400),
        // Inside
        Point(320 + 220, 400),
        Point(400 + 220, 480),
        Point(480 + 220, 400),
        Point(400 + 220, 320)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );
    painter.drawText( Point(330 + 220, 530), "XWAA" );

    const Point poly3c[] = { // CCW
        // Outside
        Point(400 + 460, 300),
        Point(500 + 460, 400),
        Point(400 + 460, 500),
        Point(300 + 460, 400),
        // Inside
        Point(320 + 460, 400),
        Point(400 + 460, 480),
        Point(480 + 460, 400),
        Point(400 + 460, 320)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::LowMemory);
    painter.fillPolygon( poly3c, sizeof(poly3c) / sizeof(poly3c[0]) );
    painter.drawText( Point(330 + 460, 530), "FSAA 2x2" );

    // A polygon with holes
    painter.setBrush( brush2 );

    const Point poly4[] = { // CCW
        // Outside
        Point(840,  30),
        Point(960, 110),
        Point(880, 230),
        Point(760, 150),
        Painter::PolygonSeparatorPoint,
        // Hole 1
        Point(860 - 40 +  0, 130 - 20     ),
        Point(860 - 40 + 30, 130 - 20 + 15),
        Point(860 - 40 -  0, 130 - 20 + 40),
        Point(860 - 40 - 20, 130 - 20 + 20),
        Painter::PolygonSeparatorPoint,
        // Hole 2
        Point(860 + 40 +  0, 130 - 20     ),
        Point(860 + 40 + 20, 130 - 20 + 20),
        Point(860 + 40 -  0, 130 - 20 + 40),
        Point(860 + 40 - 30, 130 - 20 + 15)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly4, sizeof(poly4) / sizeof(poly4[0]) );
    painter.drawText( Point(680, 70), "XWAA" );


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

static void testDrawFillEllipse(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

    painter.setPen( Color::fromRgb8(255, 0, 0) );

#if 0
    // Test big shape
    painter.setBrush( brush1 );
    ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    ip2->fillArc( Point ( 20,  20), Size(800, 800), -225 + 30, 45 + 30, ArcMode::Pie );
    ip2->fillArc( Point (100, 100), Size(105, 105), -225 + 30, 45 + 30, ArcMode::Pie );
    ip2->fillArc( Point (300, 100), Size(105, 105), 0 + 3, 135 + 3, ArcMode::Pie );
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), false);
    return;
#endif

    // Background filled rectangles
    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( Rect( Point(0, 80), Size(image.width(), 60) ) );
    painter.fillRect( Rect( Point(0, 80 + 135), Size(image.width(), 30) ) );
    painter.fillRect( Rect( Point(0, 80 + 300), Size(image.width(), 60) ) );

    // First row
    painter.setBrush( brush1 );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillEllipse( Point (30, 50), Size(105, 105) );
    painter.drawText( Point(30, 40), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillEllipse( Point (30 + 150, 50), Size(105, 105) );
    painter.drawText( Point(30 + 150, 40), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 350, 50), Size(105, 105), 0 + 3, 135 + 3, ArcMode::Chord );
        ip2->fillArc( Point (30 + 350, 50), Size(105, 105), -135 - 3, 0 - 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 350, 40), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 500, 50), Size(105, 105), 0 + 3, 135 + 3, ArcMode::Chord );
        ip2->fillArc( Point (30 + 500, 50), Size(105, 105), -135 - 3, 0 - 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 500, 40), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 700, 50), Size(105, 105), 0 + 3, 135 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 700, 40), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 850, 50), Size(105, 105), 0 + 3, 135 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 850, 40), "XWAA" );
    }

    // Second row
    painter.setBrush( brush2 );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillEllipse( Point (30, 50 + 150), Size(105, 53) );
    painter.drawText( Point(30, 40 + 150), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillEllipse( Point (30 + 150, 50 + 150), Size(105, 53) );
    painter.drawText( Point(30 + 150, 40 + 150), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 350, 50 + 140), Size(105, 105), 120 + 3, 330 + 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 350, 40 + 150), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 500, 50 + 140), Size(105, 105), 120 + 3, 330 + 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 500, 40 + 150), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 700, 50 + 140), Size(105, 105), 120 + 3, 330 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 700, 40 + 150), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 850, 50 + 140), Size(105, 105), 120 + 3, 330 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 850, 40 + 150), "XWAA" );
    }

    // Third row
    painter.setBrush( brush2 );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillEllipse( Point (30, 50 + 300), Size(53, 105) );
    painter.drawText( Point(30, 40 + 300), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillEllipse( Point (30 + 150, 50 + 300), Size(53, 105) );
    painter.drawText( Point(30 + 150, 40 + 300), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 350, 50 + 270), Size(105, 105), -150 + 3, 30 + 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 350, 40 + 300), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 500, 50 + 270), Size(105, 105), -150 + 3, 30 + 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 500, 40 + 300), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 700, 50 + 270), Size(105, 105), -150 + 3, 30 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 700, 40 + 300), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 850, 50 + 270), Size(105, 105), -150 + 3, 30 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 850, 40 + 300), "XWAA" );
    }

    // Fourth row
    painter.setBrush( brush1 );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 , 50 + 440), Size(105, 105), -225 + 30, 45 + 30, ArcMode::Pie );
        ip2->fillArc( Point (30 + 90 , 50 + 370), Size(55, 55), 30 + 20, 330 + 20, ArcMode::Pie );
        ip2->fillArc( Point (30 + 90 , 50 + 300), Size(55, 55), -130, 170, ArcMode::Chord );
        ip2->drawText( Point(30 , 40 + 440), "XWAA" );
        /*
        const CompositionMode cm = ip2->compositionMode();
        ip2->setPen( Color::fromRgb8(0, 255, 255, 127) ); // Draw some arc outlines for comparison
        ip2->setCompositionMode(CompositionMode::SourceOver);
        ip2->drawArc( Point (30 + 90 , 50 + 390), Size(55, 55), 30 + 30, 330 + 30, ArcMode::Pie );
        ip2->drawArc( Point (30 , 50 + 440), Size(105, 105), -225 + 30, 45 + 30, ArcMode::Pie );
        ip2->setCompositionMode(cm);
        ip2->setPen( Color::fromRgb8(255, 0, 0) );
        //*/

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 150, 50 + 440), Size(105, 105), -45 + 30, 225 + 30, ArcMode::Pie );
        ip2->fillArc( Point (30 + 150 + 90, 50 + 370), Size(55, 55), -150 - 20, 150 - 20, ArcMode::Pie );
        ip2->fillArc( Point (30 + 150 + 90, 50 + 300), Size(55, 55), 10, 310, ArcMode::Chord );
        ip2->drawText( Point(30 + 150, 40 + 440), "XWAA" );
        /*
        ip2->setPen( Color::fromRgb8(0, 255, 255, 127) ); // Draw some arc outlines for comparison
        ip2->setCompositionMode(CompositionMode::SourceOver);
        ip2->drawArc( Point (30 + 150, 50 + 440), Size(105, 105), -45 + 30, 225 + 30, ArcMode::Pie );
        ip2->drawArc( Point (30 + 150 + 90, 50 + 390), Size(55, 55), -150 - 30, 150 - 30, ArcMode::Pie );
        ip2->setCompositionMode(cm);
        ip2->setPen( Color::fromRgb8(255, 0, 0) );
        //*/

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 350, 50 + 440), Size(105, 105), 120 + 3, 240 + 3, ArcMode::Chord );
        ip2->fillArc( Point (30 + 350, 50 + 440), Size(105, 105), -60 - 3, 60 - 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 350, 40 + 440), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 500, 50 + 440), Size(105, 105), 120 + 3, 240 + 3, ArcMode::Chord );
        ip2->fillArc( Point (30 + 500, 50 + 440), Size(105, 105), -60 - 3, 60 - 3, ArcMode::Chord );
        ip2->drawText( Point(30 + 500, 40 + 440), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( Point (30 + 700, 50 + 440), Size(105, 105), 120 + 3, 240 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 700, 40 + 440), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( Point (30 + 850, 50 + 440), Size(105, 105), 120 + 3, 240 + 3, ArcMode::Pie );
        ip2->drawText( Point(30 + 850, 40 + 440), "XWAA" );
    }

    // Done
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
