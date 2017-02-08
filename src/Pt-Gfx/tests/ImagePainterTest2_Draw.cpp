static void testDrawLine(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

#if 1
    painter.setClip( RectF (20, image.width() - 20, 20, image.height() - 20) );
    painter.setPen( Color::fromRgb8(0, 255, 0, 175) );
    painter.drawLine( PointF(  0,   0), PointF(799, 599) );
    painter.setClip( RectF (0, image.width() - 1, 0, image.height() - 1) );
#endif

    painter.setPen( Color::fromRgb8(255, 0, 0, 175) );

    painter.drawLine( PointF(  0,   0), PointF(799,   0) );
    painter.drawLine( PointF(  0, 599), PointF(799, 599) );
    painter.drawLine( PointF(  0,   0), PointF(  0, 599) );
    painter.drawLine( PointF(799,   0), PointF(799, 599) );

    painter.setPen( Color::fromRgb8(255, 255, 255, 175) );

    painter.drawLine( PointF( 10,  10), PointF(100,  50) );
    painter.drawLine( PointF( 10, 150), PointF(100, 110) );

    painter.drawLine( PointF(100,  10), PointF(110,  20) );

    painter.setPen( Pen(Color::fromRgb8( 63,  63,  63, 175)) ); painter.drawLine( PointF(500-50, 500), PointF(200-50, 200) );
    painter.setPen( Pen(Color::fromRgb8(127, 127, 127, 175)) ); painter.drawLine( PointF(500   , 500), PointF(200   , 200) );
    painter.setPen( Pen(Color::fromRgb8(255, 255, 255, 175)) ); painter.drawLine( PointF(500+50, 500), PointF(200+50, 200) );
                                                                painter.drawLine( PointF(300   , 200), PointF(700   , 100) );
    painter.drawLine( PointF(770,  11), PointF(770, 500) );
    painter.drawLine( PointF(780,  11), PointF(782, 500) );

    painter.drawLine( PointF( 10, 540), PointF(781, 540) );
    painter.drawLine( PointF(781, 552), PointF( 10, 550) );

    painter.setPen( Pen(Color::fromRgb8(0, 255, 255, 175)) ); painter.drawText( PointF(100, 100), "Hello world!" );
    painter.setPen( Pen(Color::fromRgb8(0, 255, 255, 255)) ); painter.drawText( PointF(100, 150), "Hello world!" );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height());
}

static void testDrawRect(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

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


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height());
}

static void testDrawFillPolygon(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(&painter);


    painter.setBrush( Color::fromRgb8(255, 255, 255, 175) );
    painter.fillRect( RectF( PointF(170, 250), SizeF(image.width() - 340, image.height() - 400) ) );


    painter.setBrush( brush1 );

    const PointF poly1[] = { PointF(50, 50), PointF(250, 100), PointF(450, 250), PointF(350, 350), PointF(150, 100) };
    if(ip2) ip2   ->fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]), false);
    else    painter.fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]));

    const int    o1       = 250;
    const PointF poly1b[] = { PointF(50 + o1, 50), PointF(250 + o1, 100), PointF(450 + o1, 250), PointF(350 + o1, 350), PointF(150 + o1, 100) };
    if(ip2) ip2   ->fillPolygon(poly1b, sizeof(poly1b) / sizeof(poly1b[0]), true);
    else    painter.fillPolygon(poly1b, sizeof(poly1b) / sizeof(poly1b[0]));


    painter.setBrush( brush2 );

    const PointF poly2[] = { PointF(140, 260), PointF(210, 310), PointF(160, 340), PointF(110, 310) };
    if(ip2) ip2   ->fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]), false);
    else    painter.fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]));

    const int    o2       = 100;
    const PointF poly2b[] = { PointF(140, 260 + o2), PointF(210, 310 + o2), PointF(160, 340 + o2), PointF(110, 310 + o2) };
    if(ip2) ip2   ->fillPolygon(poly2b, sizeof(poly2b) / sizeof(poly2b[0]), true);
    else    painter.fillPolygon(poly2b, sizeof(poly2b) / sizeof(poly2b[0]));


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height());
}

