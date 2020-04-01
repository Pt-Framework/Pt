static void testDrawThickLine_impl(
    const char* title, Image& image, Painter& painter,
    const Pen& penBCapBJoin,
    const Pen& penSCapBJoin,
    const Pen& penRCapBJoin
    //const Pen& penICapBJoin,
    //const Pen& penOCapNJoin,
    //const Pen& penOCapBJoin,
    //const Pen& penOCapMJoin,
    //const Pen& penOCapRJoin
)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    Pen penText( Color::fromRgb8(255,   0,   0, 175) );
    Pen penRef ( Color::fromRgb8(255, 127, 127, 127) );

    ip2->setPen(penSCapBJoin);

    // Test corner
    Path      path;
    Transform transform;

    path.moveTo( PointF(  0,   0) );
    path.lineTo( PointF( 80,  90) );
    path.lineTo( PointF(-20, 120) );
    path.lineTo( PointF( 10,  40) );
    path.close ();

    transform.translate(530, 20);
    path.transform(transform);
    ip2->drawPath(path);

#if 1
    // Test intersection
    path.clear();
    transform.reset();

    path.moveTo( PointF(  0,   0) );
    path.lineTo( PointF( 80,  80) );
    path.lineTo( PointF(  0,  80) );
    path.lineTo( PointF( 80,   0) );
    path.close ();

    transform.translate(530, 200);
    path.transform(transform);
    ip2->drawPath(path, 1.0, true); // Set 'useNonZeroFillingRule' to 'true'

    transform.translate(-530, -50);
    path.transform(transform);
    ip2->setPen( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Solid, Pen::FlatCap) );
    ip2->drawPath(path, 1.0, true); // Set 'useNonZeroFillingRule' to 'true'
#endif

#if 1

    /*
    painter.setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(penOCapBJoin);
    painter.drawLine( PointF( 100, 100), PointF(300, 100) );
    //painter.drawLine( PointF( 300, 100), PointF(100, 100) );
    //painter.drawLine( PointF( 100, 100), PointF(100, 300) );
    //painter.drawLine( PointF( 100, 300), PointF(100, 100) );
    painter.drawLine( PointF( 100, 100), PointF(300, 300) );
    //painter.drawLine( PointF( 100, 300), PointF(300, 100) );
    //painter.drawLine( PointF( 300, 300), PointF(100, 100) );
    //painter.drawLine( PointF( 300, 100), PointF(100, 300) );
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
    return;
    //*/

    /*
    painter.setPen(penOCapBJoin);
    const PointF polyX[] = { // CCW
        PointF(670 + 10, 120),
        PointF(700 + 10, 180),
        PointF(800 + 10, 130),
        PointF(650 + 10,  20)
    };
    if(ip2) ip2->drawPolyline( polyX, sizeof(polyX) / sizeof(polyX[0]), false );
    else painter.drawPolyline( polyX, sizeof(polyX) / sizeof(polyX[0]) );
    if(true) {
        painter.setPen(penRef);
        if(ip2) ip2->drawPolyline( polyX, sizeof(polyX) / sizeof(polyX[0]), false );
        else painter.drawPolyline( polyX, sizeof(polyX) / sizeof(polyX[0]) );
    }
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
    return;
    //*/

    // Test anti-aliasing
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(penBCapBJoin);
    painter.drawLine( PointF( 20,  20), PointF(200, 120) );
    painter.drawLine( PointF( 20, 240), PointF(200, 140) );
    if(true) {
        painter.setPen(penRef);
        painter.drawLine( PointF( 20,  20), PointF(200, 120) );
        painter.drawLine( PointF( 20, 240), PointF(200, 140) );
    }
    painter.setPen(penText); painter.drawText( PointF( 20, 135), "NOAA" );

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(penBCapBJoin);
    painter.drawLine( PointF(220,  20), PointF(400, 120) );
    painter.drawLine( PointF(220, 240), PointF(400, 140) );
    if(true) {
        painter.setPen(penRef);
        painter.drawLine( PointF(220,  20), PointF(400, 120) );
        painter.drawLine( PointF(220, 240), PointF(400, 140) );
    }
    painter.setPen(penText); painter.drawText( PointF(220, 135), "XWAA" );

    // Test caps
    painter.setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    if(ip2) ip2->setAntiAliasing(true);

    painter.setPen(penBCapBJoin);
    painter.drawLine( PointF( 20, 300 - 30), PointF(100, 350 - 30) );
    painter.drawLine( PointF( 20, 430 - 30), PointF(100, 380 - 30) );
    if(true) {
        painter.setPen(penRef);
        painter.drawLine( PointF( 20, 300 - 30), PointF(100, 350 - 30) );
        painter.drawLine( PointF( 20, 430 - 30), PointF(100, 380 - 30) );
    }
    painter.setPen(penText); painter.drawText( PointF(20, 370 - 30), "Butt" );

    painter.setPen(penSCapBJoin);
    painter.drawLine( PointF(120, 300 - 30), PointF(200, 350 - 30) );
    painter.drawLine( PointF(120, 430 - 30), PointF(200, 380 - 30) );
    if(true) {
        painter.setPen(penRef);
        painter.drawLine( PointF(120, 300 - 30), PointF(200, 350 - 30) );
        painter.drawLine( PointF(120, 430 - 30), PointF(200, 380 - 30) );
    }
    painter.setPen(penText); painter.drawText( PointF(120, 370 - 30), "Square" );

    painter.setPen(penRCapBJoin);
    painter.drawLine( PointF(220, 300 - 30), PointF(300, 350 - 30) );
    painter.drawLine( PointF(220, 430 - 30), PointF(300, 380 - 30) );
    if(true) {
        painter.setPen(penRef);
        painter.drawLine( PointF(220, 300 - 30), PointF(300, 350 - 30) );
        painter.drawLine( PointF(220, 430 - 30), PointF(300, 380 - 30) );
    }
    painter.setPen(penText); painter.drawText( PointF(220, 370 - 30), "Round" );

    /*
    painter.setPen(penOCapBJoin);
    painter.drawLine( PointF(320, 300 - 30), PointF(400, 350 - 30) );
    painter.drawLine( PointF(320, 430 - 30), PointF(400, 380 - 30) );
    if(true) {
        painter.setPen(penRef);
        painter.drawLine( PointF(320, 300 - 30), PointF(400, 350 - 30) );
        painter.drawLine( PointF(320, 430 - 30), PointF(400, 380 - 30) );
    }
    painter.setPen(penText); painter.drawText( PointF(320, 370 - 30), "Tri-Out" );

    // ### TODO: PRODUCE LESS NUMBER OF SEGMENTS THAN "TRI-OUT" !!! ###
    painter.setPen(penICapBJoin);
    painter.drawLine( PointF(420, 300 - 30), PointF(500, 350 - 30) );
    painter.drawLine( PointF(420, 430 - 30), PointF(500, 380 - 30) );
    if(true) {
        painter.setPen(penRef);
        painter.drawLine( PointF(420, 300 - 30), PointF(500, 350 - 30) );
        painter.drawLine( PointF(420, 430 - 30), PointF(500, 380 - 30) );
    }
    painter.setPen(penText); painter.drawText( PointF(420, 370 - 30), "Tri-In" );

    // Test joins
    painter.setPen(penOCapBJoin);
    const PointF poly1a[] = { // CCW
        PointF(670 + 10, 120),
        PointF(700 + 10, 180),
        PointF(800 + 10, 130),
        PointF(650 + 10,  20)
    };
    if(ip2) ip2->drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );
    else painter.drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );
    if(true) {
        //penRef.setStyle(painter.pen().style());
        painter.setPen(penRef);
        if(ip2) ip2->drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );
        else painter.drawPolyline( poly1a, sizeof(poly1a) / sizeof(poly1a[0]) );
    }

    painter.setPen(penOCapBJoin);
    const PointF poly1b[] = { // CCW
        PointF(670 + 170, 120),
        PointF(700 + 170, 180),
        PointF(800 + 170, 130),
        PointF(650 + 170,  20)
    };
    if(ip2) ip2->drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );
    else painter.drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );
    if(true) {
        painter.setPen(penRef);
        if(ip2) ip2->drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );
        else painter.drawPolyline( poly1b, sizeof(poly1b) / sizeof(poly1b[0]) );
    }

    painter.setPen(penText); painter.drawText( PointF(650 + 20, 100), "Bevel" );

    painter.setPen(penOCapMJoin);
    const PointF poly2a[] = { // CCW
        PointF(670 + 10, 120 + 200),
        PointF(700 + 10, 180 + 200),
        PointF(800 + 10, 130 + 200),
        PointF(650 + 10,  20 + 200)
    };
    if(ip2) ip2->drawPolyline( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );
    else painter.drawPolyline( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );
    if(true) {
        painter.setPen(penRef);
        if(ip2) ip2->drawPolyline( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );
        else painter.drawPolyline( poly2a, sizeof(poly2a) / sizeof(poly2a[0]) );
    }

    painter.setPen(penOCapMJoin);
    const PointF poly2b[] = { // CCW
        PointF(670 + 170, 120 + 200),
        PointF(700 + 170, 180 + 200),
        PointF(800 + 170, 130 + 200),
        PointF(650 + 170,  20 + 200)
    };
    if(ip2) ip2->drawPolyline( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );
    else painter.drawPolyline( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );
    if(true) {
        painter.setPen(penRef);
        if(ip2) ip2->drawPolyline( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );
        else painter.drawPolyline( poly2b, sizeof(poly2b) / sizeof(poly2b[0]) );
    }

    painter.setPen(penText); painter.drawText( PointF(650 + 20, 100 + 200), "Miter" );

    painter.setPen(penOCapRJoin);
    const PointF poly3a[] = { // CCW
        PointF(670 + 10, 120 + 400),
        PointF(700 + 10, 180 + 400),
        PointF(800 + 10, 130 + 400),
        PointF(650 + 10,  20 + 400)
    };
    if(ip2) ip2->drawPolyline( poly3a, sizeof(poly3a) / sizeof(poly3a[0]) );
    else painter.drawPolyline( poly3a, sizeof(poly3a) / sizeof(poly3a[0]) );
    if(true) {
        painter.setPen(penRef);
        if(ip2) ip2->drawPolyline( poly3a, sizeof(poly3a) / sizeof(poly3a[0]) );
        else painter.drawPolyline( poly3a, sizeof(poly3a) / sizeof(poly3a[0]) );
    }

    painter.setPen(penOCapRJoin);
    const PointF poly3b[] = { // CCW
        PointF(670 + 170, 120 + 400),
        PointF(700 + 170, 180 + 400),
        PointF(800 + 170, 130 + 400),
        PointF(650 + 170,  20 + 400)
    };
    if(ip2) ip2->drawPolyline( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );
    else painter.drawPolyline( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );
    if(true) {
        painter.setPen(penRef);
        if(ip2) ip2->drawPolyline( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );
        else painter.drawPolyline( poly3b, sizeof(poly3b) / sizeof(poly3b[0]) );
    }

    painter.setPen(penText); painter.drawText( PointF(650 + 20, 100 + 400), "Round" );

    if(ip2) { // This join type is not supported by the old painter
        ip2->setPen(penOCapNJoin);
        const PointF poly4a[] = { // CCW
            PointF(670 - 600 - 30, 120 + 400),
            PointF(700 - 600 - 30, 180 + 400),
            PointF(800 - 600 - 30, 130 + 400),
            PointF(650 - 600 - 30,  20 + 400)
        };
        ip2->drawPolyline( poly4a, sizeof(poly4a) / sizeof(poly4a[0]) );
        if(true) {
            painter.setPen(penRef);
            ip2->drawPolyline( poly4a, sizeof(poly4a) / sizeof(poly4a[0]) );
        }

        ip2->setPen(penOCapNJoin);
        const PointF poly4b[] = { // CCW
            PointF(670 - 430 - 40, 120 + 400),
            PointF(700 - 430 - 40, 180 + 400),
            PointF(800 - 430 - 40, 130 + 400),
            PointF(650 - 430 - 40,  20 + 400)
        };
        ip2->drawPolyline( poly4b, sizeof(poly4b) / sizeof(poly4b[0]) );
        if(true) {
            ip2->setPen(penRef);
            ip2->drawPolyline( poly4b, sizeof(poly4b) / sizeof(poly4b[0]) );
        }

        ip2->setPen(penText); painter.drawText( PointF(650 - 600 - 20, 100 + 400), "None" );
    }
    */

    // Test bezier
    if(ip2) {
        /*
        // Test caps
        const PointF bezier1a[] = { // CCW
            PointF(150 + 310, 150 + 330),
            PointF(150 + 310, 100 + 330),
            PointF(100 + 310, 100 + 330)
        };
        ip2->setPen(penSCapBJoin);
        ip2->drawQuadraticPolybezier( bezier1a, sizeof(bezier1a) / sizeof(bezier1a[0]), false );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier1a, sizeof(bezier1a) / sizeof(bezier1a[0]), false );
        }

        const PointF bezier1b[] = { // CCW
            PointF(100 + 280, 100 + 340),
            PointF(100 + 280, 150 + 340),
            PointF(150 + 280, 150 + 340)
        };
        ip2->setPen(penRCapBJoin);
        //ip2->drawQuadraticPolybezier( bezier1b, sizeof(bezier1b) / sizeof(bezier1b[0]), false );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier1b, sizeof(bezier1b) / sizeof(bezier1b[0]), false );
        }
        */

        /*
        const PointF bezier1c[] = { // CCW
            PointF(150 + 310, 150 + 420),
            PointF(150 + 310, 100 + 420),
            PointF(100 + 310, 100 + 420)
        };
        ip2->setPen(penICapBJoin);
        //ip2->drawQuadraticPolybezier( bezier1c, sizeof(bezier1c) / sizeof(bezier1c[0]), false );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier1c, sizeof(bezier1c) / sizeof(bezier1c[0]), false );
        }

        const PointF bezier1d[] = { // CCW
            PointF(100 + 280, 100 + 430),
            PointF(100 + 280, 150 + 430),
            PointF(150 + 280, 150 + 430)
        };
        ip2->setPen(penOCapBJoin);
        //ip2->drawQuadraticPolybezier( bezier1d, sizeof(bezier1d) / sizeof(bezier1d[0]), false );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier1d, sizeof(bezier1d) / sizeof(bezier1d[0]), false );
        }

        // Test joins
        const PointF bezier2a[] = { // CCW
            PointF(100 + 390, 100 + 330),
            PointF(100 + 390, 150 + 330),
            PointF(150 + 390, 150 + 330),
            PointF(150 + 390, 100 + 330)
        };
        ip2->setPen(penOCapBJoin);
        //ip2->drawQuadraticPolybezier( bezier2a, sizeof(bezier2a) / sizeof(bezier2a[0]), true );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier2a, sizeof(bezier2a) / sizeof(bezier2a[0]), true );
        }

        const PointF bezier2b[] = { // CCW
            PointF(100 + 390, 100 + 430),
            PointF(100 + 390, 150 + 430),
            PointF(150 + 390, 150 + 430),
            PointF(150 + 390, 100 + 430)
        };
        ip2->setPen(penOCapMJoin);
        //ip2->drawQuadraticPolybezier( bezier2b, sizeof(bezier2b) / sizeof(bezier2b[0]), true );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier2b, sizeof(bezier2b) / sizeof(bezier2b[0]), true );
        }

        const PointF bezier2c[] = { // CCW
            PointF(100 + 470, 100 + 330),
            PointF(100 + 470, 150 + 330),
            PointF(150 + 470, 150 + 330),
            PointF(150 + 470, 100 + 330)
        };
        ip2->setPen(penOCapRJoin);
        //ip2->drawQuadraticPolybezier( bezier2c, sizeof(bezier2c) / sizeof(bezier2c[0]), true );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier2c, sizeof(bezier2c) / sizeof(bezier2c[0]), true );
        }

        const PointF bezier2d[] = { // CCW
            PointF(100 + 470, 100 + 430),
            PointF(100 + 470, 150 + 430),
            PointF(150 + 470, 150 + 430),
            PointF(150 + 470, 100 + 430)
        };
        ip2->setPen(penOCapNJoin);
        //ip2->drawQuadraticPolybezier( bezier2d, sizeof(bezier2d) / sizeof(bezier2d[0]), true );
        if(true) {
            ip2->setPen(penRef);
            //ip2->drawQuadraticPolybezier( bezier2d, sizeof(bezier2d) / sizeof(bezier2d[0]), true );
        }
        */
    }

#endif

    // Done
    painter.setFont( Pt::Gfx::Font(FONT_SPEC_N) );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!dynamic_cast<ImagePainter2*>(&painter));
}

static void testDrawSolidThickLine(const char* title, Image& image, Painter& painter)
{
    Pen penBCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::FlatCap,          Pen::BevelJoin);
    Pen penSCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::SquareCap,        Pen::BevelJoin);
    Pen penRCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::RoundCap,         Pen::BevelJoin);

  //Pen penICapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::TriangularInCap,  Pen::BevelJoin);
  //Pen penOCapNJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::TriangularOutCap, Pen::NoJoin   );
  //Pen penOCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::TriangularOutCap, Pen::BevelJoin);
  //Pen penOCapMJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::TriangularOutCap, Pen::MiterJoin);
  //Pen penOCapRJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Solid, Pen::TriangularOutCap, Pen::RoundJoin);

    testDrawThickLine_impl(
        title, image, painter,
        penBCapBJoin,
        penSCapBJoin,
        penRCapBJoin
      //penICapBJoin,
      //penOCapNJoin,
      //penOCapBJoin,
      //penOCapMJoin,
      //penOCapRJoin
    );
}

static void testDrawPatternedThickLine(const char* title, Image& image, Painter& painter)
{
    Pen penBCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::FlatCap,          Pen::BevelJoin);
    Pen penSCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::SquareCap,        Pen::BevelJoin);
    Pen penRCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::RoundCap,         Pen::BevelJoin);

  //Pen penICapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::TriangularInCap,  Pen::BevelJoin);
  //Pen penOCapNJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::TriangularOutCap, Pen::NoJoin   );
  //Pen penOCapBJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::TriangularOutCap, Pen::BevelJoin);
  //Pen penOCapMJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::TriangularOutCap, Pen::MiterJoin);
  //Pen penOCapRJoin(Color::fromRgb8(  0, 255, 0, 175), 12, Pen::Dot, Pen::TriangularOutCap, Pen::RoundJoin);

    testDrawThickLine_impl(
        title, image, painter,
        penBCapBJoin,
        penSCapBJoin,
        penRCapBJoin
      //penICapBJoin,
      //penOCapNJoin,
      //penOCapBJoin,
      //penOCapMJoin,
      //penOCapRJoin
    );
}

// ###
static void testDrawThickEllipseArc_impl(
    const char* title, Image& image, Painter& painter,
    const Pen& drawB,
    const Pen& drawS,
    const Pen& drawR
  //const Pen& drawO,
  //const Pen& drawI
)
{
    resetImage(image);

    const CompositionMode cm = painter.compositionMode();

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);

    Pen text ( Color::fromRgb8(255,   0,   0, 255) );
    Pen vref ( Color::fromRgb8(255, 127, 127, 127) );

    painter.setBrush( Color::fromRgb8(63, 63, 255) );
    painter.fillRect( RectF( PointF(0, 80), SizeF(image.width(), 60) ) );
    painter.fillRect( RectF( PointF(0, 80 + 185), SizeF(image.width(), 30) ) );
    painter.fillRect( RectF( PointF(0, 80 + 400), SizeF(image.width(), 60) ) );

    /*
    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(drawI); painter.drawEllipse( PointF (30, 50 + 400), SizeF(67, 135) );
    painter.setCompositionMode(CompositionMode::SourceOver);
    painter.setPen(vref ); painter.drawEllipse( PointF (30, 50 + 400), SizeF(67, 135) );
    painter.setCompositionMode(cm);
    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), false);
    return;
    //*/

    // First row
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(drawB); painter.drawEllipse( PointF (30, 50), SizeF(135, 135) );
    painter.setPen(text ); painter.drawText( PointF(30, 30), "NOAA" );
    painter.setCompositionMode(CompositionMode::SourceOver);
    painter.setPen(vref ); painter.drawEllipse( PointF (30, 50), SizeF(135, 135) );
    painter.setCompositionMode(cm);

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(drawB); painter.drawEllipse( PointF (30 + 200, 50), SizeF(135, 135) );
    painter.setPen(text ); painter.drawText( PointF(30 + 200, 30), "XWAA" );
    painter.setCompositionMode(CompositionMode::SourceOver);
    painter.setPen(vref ); painter.drawEllipse( PointF (30 + 200, 50), SizeF(135, 135) );
    painter.setCompositionMode(cm);

    if(ip2) {
        ip2->setAntiAliasing(false);
        painter.setPen(drawB); ip2->drawArc( PointF (30 + 400,  50), SizeF(135, 135), 0, 135 );
        painter.setPen(drawS); ip2->drawArc( PointF (30 + 400, 120), SizeF(135, 135), 0, 135 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 400,  30), "NOAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawArc( PointF (30 + 400,  50), SizeF(135, 135), 0, 135 );
                               ip2->drawArc( PointF (30 + 400, 120), SizeF(135, 135), 0, 135 );
        painter.setCompositionMode(cm);

        ip2->setAntiAliasing(true);
        painter.setPen(drawB); ip2->drawArc( PointF (30 + 600,  50), SizeF(135, 135), 0, 135 );
        painter.setPen(drawS); ip2->drawArc( PointF (30 + 600, 120), SizeF(135, 135), 0, 135 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 600,  30), "XWAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawArc( PointF (30 + 600,  50), SizeF(135, 135), 0, 135 );
                               ip2->drawArc( PointF (30 + 600, 120), SizeF(135, 135), 0, 135 );
        painter.setCompositionMode(cm);

        ip2->setAntiAliasing(true);
        painter.setPen(drawR); ip2->drawArc( PointF (30 + 800, 50), SizeF(135, 135), -135, 135 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 800, 30), "XWAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawArc( PointF (30 + 800, 50), SizeF(135, 135), -135, 135 );
        painter.setCompositionMode(cm);
    }

    /*
    // Second row
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(drawO); painter.drawEllipse( PointF (30, 50 + 200), SizeF(135, 67) );
    painter.setPen(text ); painter.drawText( PointF(30, 30 + 200), "NOAA" );
    painter.setCompositionMode(CompositionMode::SourceOver);
    painter.setPen(vref ); painter.drawEllipse( PointF (30, 50 + 200), SizeF(135, 67) );
    painter.setCompositionMode(cm);

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(drawO); painter.drawEllipse( PointF (30 + 200, 50 + 200), SizeF(135, 67) );
    painter.setPen(text ); painter.drawText( PointF(30 + 200, 30 + 200), "XWAA" );
    painter.setCompositionMode(CompositionMode::SourceOver);
    painter.setPen(vref ); painter.drawEllipse( PointF (30 + 200, 50 + 200), SizeF(135, 67) );
    painter.setCompositionMode(cm);

    if(ip2) {
        ip2->setAntiAliasing(false);
        painter.setPen(drawO); ip2->drawArc( PointF (30 + 400, 50 + 200), SizeF(135, 135), 120, 330 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 400, 30 + 200), "NOAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawArc( PointF (30 + 400, 50 + 200), SizeF(135, 135), 120, 330 );
        painter.setCompositionMode(cm);

        ip2->setAntiAliasing(true);
        painter.setPen(drawO); ip2->drawArc( PointF (30 + 600, 50 + 200), SizeF(135, 135), 120, 330 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 600, 30 + 200), "XWAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawArc( PointF (30 + 600, 50 + 200), SizeF(135, 135), 120, 330 );
        painter.setCompositionMode(cm);

        ip2->setAntiAliasing(true);
        painter.setPen(drawO); ip2->drawChord( PointF (30 + 800, 50 + 200), SizeF(135, 135), -135, 135 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 800, 30 + 200), "XWAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawChord( PointF (30 + 800, 50 + 200), SizeF(135, 135), -135, 135 );
        painter.setCompositionMode(cm);
    }

    // Third row
    if(ip2) ip2->setAntiAliasing(false);
    painter.setPen(drawI); painter.drawEllipse( PointF (30, 50 + 400), SizeF(67, 135) );
    painter.setPen(text ); painter.drawText( PointF(30, 30 + 400), "NOAA" );
    painter.setCompositionMode(CompositionMode::SourceOver);
    painter.setPen(vref ); painter.drawEllipse( PointF (30, 50 + 400), SizeF(67, 135) );
    painter.setCompositionMode(cm);

    if(ip2) ip2->setAntiAliasing(true);
    painter.setPen(drawI); painter.drawEllipse( PointF (30 + 200, 50 + 400), SizeF(67, 135) );
    painter.setPen(text ); painter.drawText( PointF(30 + 200, 30 + 400), "XWAA" );
    painter.setCompositionMode(CompositionMode::SourceOver);
    painter.setPen(vref ); painter.drawEllipse( PointF (30 + 200, 50 + 400), SizeF(67, 135) );
    painter.setCompositionMode(cm);

    if(ip2) {
        ip2->setAntiAliasing(false);
        painter.setPen(drawI); ip2->drawArc( PointF (30 + 400, 50 + 400), SizeF(135, 135), -150, 30 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 400, 30 + 400), "NOAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawArc( PointF (30 + 400, 50 + 400), SizeF(135, 135), -150, 30 );
        painter.setCompositionMode(cm);

        ip2->setAntiAliasing(true);
        painter.setPen(drawI); ip2->drawArc( PointF (30 + 600, 50 + 400), SizeF(135, 135), -150, 30 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 600, 30 + 400), "XWAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawArc( PointF (30 + 600, 50 + 400), SizeF(135, 135), -150, 30 );
        painter.setCompositionMode(cm);

        ip2->setAntiAliasing(true);
        painter.setPen(drawI); ip2->drawPie( PointF (30 + 800, 50 + 400), SizeF(135, 135), -135, 135 );
        painter.setPen(text ); ip2->drawText( PointF(30 + 800, 30 + 400), "XWAA" );
        painter.setCompositionMode(CompositionMode::SourceOver);
        painter.setPen(vref ); ip2->drawPie( PointF (30 + 800, 50 + 400), SizeF(135, 135), -135, 135 );
        painter.setCompositionMode(cm);
    }
    */

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

static void testDrawSolidThickEllipseArc(const char* title, Image& image, Painter& painter)
{
    Pen drawB( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Solid, Pen::FlatCap         ) );
    Pen drawS( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Solid, Pen::SquareCap       ) );
    Pen drawR( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Solid, Pen::RoundCap        ) );
  //Pen drawO( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Solid, Pen::TriangularOutCap) );
  //Pen drawI( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Solid, Pen::TriangularInCap ) );

    testDrawThickEllipseArc_impl(
        title, image, painter,
        drawB,
        drawS,
        drawR
      //drawO,
      //drawI
    );
}

static void testDrawPatternedThickEllipseArc(const char* title, Image& image, Painter& painter)
{
    Pen drawB( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Dot, Pen::FlatCap         ) );
    Pen drawS( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Dot, Pen::SquareCap       ) );
    Pen drawR( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Dot, Pen::RoundCap        ) );
  //Pen drawO( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Dot, Pen::TriangularOutCap) );
  //Pen drawI( Pen(Color::fromRgb8(127, 255, 191, 175), 12, Pen::Dot, Pen::TriangularInCap ) );

    testDrawThickEllipseArc_impl(
        title, image, painter,
        drawB,
        drawS,
        drawR
      //drawO,
      //drawI
    );
}


