static void testDrawFillPolygon(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

#if 0
    painter.setBrush( Color::fromRgb8(255, 255, 255) );
    const Pt::int32_t sx = 5;
    const PointF polyx[] = { PointF(10 * sx, 10 * sx), PointF(40 * sx, 40 * sx), PointF(30 * sx, 60 * sx) };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( polyx, sizeof(polyx) / sizeof(polyx[0]) );
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), false);
    return;
#endif

    painter.setPen( Color::fromRgb8(255, 0, 0) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( RectF( PointF(800, 120), SizeF(170, 130) ) );
    painter.fillRect( RectF( PointF(170, 250), SizeF(800, 200) ) );
    painter.fillRect( RectF( PointF(170, 450), SizeF(100, 100) ) );

    // PointFy polygons
    painter.setBrush( brush1 );

    const PointF poly1a[] = { // CCW
        PointF(100 + 5, 100),
        PointF(300 + 5, 350),
        PointF(400 + 5, 250),
        PointF(200 + 5, 100),
        PointF(  0 + 5,  50)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillPolygon( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );
    painter.drawText( PointF(30, 50), "NOAA" );

    const PointF poly1b[] = { // CCW
        PointF(100 + 198, 100),
        PointF(300 + 198, 350),
        PointF(400 + 198, 250),
        PointF(200 + 198, 100),
        PointF(  0 + 198,  50)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );
    painter.drawText( PointF(30 + 198, 50), "XWAA" );

    const PointF poly1c[] = { // CCW
        PointF(100 + 391, 100),
        PointF(300 + 391, 350),
        PointF(400 + 391, 250),
        PointF(200 + 391, 100),
        PointF(  0 + 391,  50)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::LowMemory);
    painter.fillPolygon( poly1c, sizeof(poly1b) / sizeof(poly1b[0]) );
    painter.drawText( PointF(30 + 391, 50), "FSAA2x2" );

    // Diamond-like polygons
    painter.setBrush( brush2 );

    const PointF poly2a[] = { // CCW
        PointF(110, 310),
        PointF(160, 340),
        PointF(210, 310),
        PointF(140, 260)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillPolygon( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );
    painter.drawText( PointF(10, 250 + 100), "NOAA" );

    const PointF poly2b[] = { // CCW
        PointF(110, 310 + 100),
        PointF(160, 340 + 100),
        PointF(210, 310 + 100),
        PointF(140, 260 + 100)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );
    painter.drawText( PointF(10, 250 + 200), "XWAA" );

    const PointF poly2c[] = { // CCW
        PointF(110, 310 + 200),
        PointF(160, 340 + 200),
        PointF(210, 310 + 200),
        PointF(140, 260 + 200)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::LowMemory);
    painter.fillPolygon( poly2c, sizeof(poly2c) / sizeof(poly2c[0]) );
    painter.drawText( PointF(10, 250 + 300), "FSAA2x2" );

    // Complex U-like polygons
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
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
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
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );
    painter.drawText( PointF(330 + 220, 530), "XWAA" );

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
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::LowMemory);
    painter.fillPolygon( poly3c, sizeof(poly3c) / sizeof(poly3c[0]) );
    painter.drawText( PointF(330 + 460, 530), "FSAA 2x2" );

    // A polygon with holes
    painter.setBrush( brush2 );

    const PointF poly4[] = { // CCW
        // Outside
        PointF(840,  30),
        PointF(960, 110),
        PointF(880, 230),
        PointF(760, 150),
        Painter::PolygonSeparatorPointF,
        // Hole 1
        PointF(860 - 40 +  0, 130 - 20     ),
        PointF(860 - 40 + 30, 130 - 20 + 15),
        PointF(860 - 40 -  0, 130 - 20 + 40),
        PointF(860 - 40 - 20, 130 - 20 + 20),
        Painter::PolygonSeparatorPointF,
        // Hole 2
        PointF(860 + 40 +  0, 130 - 20     ),
        PointF(860 + 40 + 20, 130 - 20 + 20),
        PointF(860 + 40 -  0, 130 - 20 + 40),
        PointF(860 + 40 - 30, 130 - 20 + 15)
    };
    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillPolygon( poly4, sizeof(poly4) / sizeof(poly4[0]) );
    painter.drawText( PointF(680, 70), "XWAA" );


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
    ip2->fillArc( PointF ( 20,  20), SizeF(800, 800), -225 + 30, 45 + 30, ArcMode::Pie );
    ip2->fillArc( PointF (100, 100), SizeF(105, 105), -225 + 30, 45 + 30, ArcMode::Pie );
    ip2->fillArc( PointF (300, 100), SizeF(105, 105), 0 + 3, 135 + 3, ArcMode::Pie );
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), false);
    return;
#endif

    // Background filled rectangles
    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( RectF( PointF(0, 80), SizeF(image.width(), 60) ) );
    painter.fillRect( RectF( PointF(0, 80 + 135), SizeF(image.width(), 30) ) );
    painter.fillRect( RectF( PointF(0, 80 + 300), SizeF(image.width(), 60) ) );

    // First row
    painter.setBrush( brush1 );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillEllipse( PointF (30, 50), SizeF(105, 105) );
    painter.drawText( PointF(30, 40), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillEllipse( PointF (30 + 150, 50), SizeF(105, 105) );
    painter.drawText( PointF(30 + 150, 40), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 350, 50), SizeF(105, 105), 0 + 3, 135 + 3, ArcMode::Chord );
        ip2->fillArc( PointF (30 + 350, 50), SizeF(105, 105), -135 - 3, 0 - 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 350, 40), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 500, 50), SizeF(105, 105), 0 + 3, 135 + 3, ArcMode::Chord );
        ip2->fillArc( PointF (30 + 500, 50), SizeF(105, 105), -135 - 3, 0 - 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 500, 40), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 700, 50), SizeF(105, 105), 0 + 3, 135 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 700, 40), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 850, 50), SizeF(105, 105), 0 + 3, 135 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 850, 40), "XWAA" );
    }

    // Second row
    painter.setBrush( brush2 );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillEllipse( PointF (30, 50 + 150), SizeF(105, 53) );
    painter.drawText( PointF(30, 40 + 150), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillEllipse( PointF (30 + 150, 50 + 150), SizeF(105, 53) );
    painter.drawText( PointF(30 + 150, 40 + 150), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 350, 50 + 140), SizeF(105, 105), 120 + 3, 330 + 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 350, 40 + 150), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 500, 50 + 140), SizeF(105, 105), 120 + 3, 330 + 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 500, 40 + 150), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 700, 50 + 140), SizeF(105, 105), 120 + 3, 330 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 700, 40 + 150), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 850, 50 + 140), SizeF(105, 105), 120 + 3, 330 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 850, 40 + 150), "XWAA" );
    }

    // Third row
    painter.setBrush( brush2 );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::None);
    painter.fillEllipse( PointF (30, 50 + 300), SizeF(53, 105) );
    painter.drawText( PointF(30, 40 + 300), "NOAA" );

    if(ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
    painter.fillEllipse( PointF (30 + 150, 50 + 300), SizeF(53, 105) );
    painter.drawText( PointF(30 + 150, 40 + 300), "XWAA" );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 350, 50 + 270), SizeF(105, 105), -150 + 3, 30 + 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 350, 40 + 300), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 500, 50 + 270), SizeF(105, 105), -150 + 3, 30 + 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 500, 40 + 300), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 700, 50 + 270), SizeF(105, 105), -150 + 3, 30 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 700, 40 + 300), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 850, 50 + 270), SizeF(105, 105), -150 + 3, 30 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 850, 40 + 300), "XWAA" );
    }

    // Fourth row
    painter.setBrush( brush1 );

    if(ip2) {
        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 , 50 + 440), SizeF(105, 105), -225 + 30, 45 + 30, ArcMode::Pie );
        ip2->fillArc( PointF (30 + 90 , 50 + 370), SizeF(55, 55), 30 + 20, 330 + 20, ArcMode::Pie );
        ip2->fillArc( PointF (30 + 90 , 50 + 300), SizeF(55, 55), -130, 170, ArcMode::Chord );
        ip2->drawText( PointF(30 , 40 + 440), "XWAA" );
        /*
        const CompositionMode cm = ip2->compositionMode();
        ip2->setPen( Color::fromRgb8(0, 255, 255, 127) ); // Draw some arc outlines for comparison
        ip2->setCompositionMode(CompositionMode::SourceOver);
        ip2->drawArc( PointF (30 + 90 , 50 + 390), SizeF(55, 55), 30 + 30, 330 + 30, ArcMode::Pie );
        ip2->drawArc( PointF (30 , 50 + 440), SizeF(105, 105), -225 + 30, 45 + 30, ArcMode::Pie );
        ip2->setCompositionMode(cm);
        ip2->setPen( Color::fromRgb8(255, 0, 0) );
        //*/

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 150, 50 + 440), SizeF(105, 105), -45 + 30, 225 + 30, ArcMode::Pie );
        ip2->fillArc( PointF (30 + 150 + 90, 50 + 370), SizeF(55, 55), -150 - 20, 150 - 20, ArcMode::Pie );
        ip2->fillArc( PointF (30 + 150 + 90, 50 + 300), SizeF(55, 55), 10, 310, ArcMode::Chord );
        ip2->drawText( PointF(30 + 150, 40 + 440), "XWAA" );
        /*
        ip2->setPen( Color::fromRgb8(0, 255, 255, 127) ); // Draw some arc outlines for comparison
        ip2->setCompositionMode(CompositionMode::SourceOver);
        ip2->drawArc( PointF (30 + 150, 50 + 440), SizeF(105, 105), -45 + 30, 225 + 30, ArcMode::Pie );
        ip2->drawArc( PointF (30 + 150 + 90, 50 + 390), SizeF(55, 55), -150 - 30, 150 - 30, ArcMode::Pie );
        ip2->setCompositionMode(cm);
        ip2->setPen( Color::fromRgb8(255, 0, 0) );
        //*/

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 350, 50 + 440), SizeF(105, 105), 120 + 3, 240 + 3, ArcMode::Chord );
        ip2->fillArc( PointF (30 + 350, 50 + 440), SizeF(105, 105), -60 - 3, 60 - 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 350, 40 + 440), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 500, 50 + 440), SizeF(105, 105), 120 + 3, 240 + 3, ArcMode::Chord );
        ip2->fillArc( PointF (30 + 500, 50 + 440), SizeF(105, 105), -60 - 3, 60 - 3, ArcMode::Chord );
        ip2->drawText( PointF(30 + 500, 40 + 440), "XWAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::None);
        ip2->fillArc( PointF (30 + 700, 50 + 440), SizeF(105, 105), 120 + 3, 240 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 700, 40 + 440), "NOAA" );

        ip2->setAntiAliasingMode(AntiAliasingMode::Standard);
        ip2->fillArc( PointF (30 + 850, 50 + 440), SizeF(105, 105), 120 + 3, 240 + 3, ArcMode::Pie );
        ip2->drawText( PointF(30 + 850, 40 + 440), "XWAA" );
    }

    // Done
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
