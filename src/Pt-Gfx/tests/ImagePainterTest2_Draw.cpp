static void testLines(const char* title, Image& image, ImagePainter2& painter)
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
    painter.drawLine( PointF(781, 551), PointF( 10, 550) );

    painter.setPen( Pen(Color::fromRgb8(0, 255, 255, 175)) ); painter.drawText( PointF(100, 100), "Hello world!" );
    painter.setPen( Pen(Color::fromRgb8(0, 255, 255, 255)) ); painter.drawText( PointF(100, 150), "Hello world!" );

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height());
}

static void testFillPolygon(const char* title, Image& image, ImagePainter2& painter)
{
    resetImage(image);

    painter.setBrush( Color::fromRgb8(0, 255, 0, 175) );

    const PointF poly1[] = { PointF(50, 50), PointF(250, 100), PointF(450, 250), PointF(350, 350), PointF(150, 100) };
    painter.fillPolygon(poly1, sizeof(poly1) / sizeof(poly1[0]));

    painter.setBrush( Color::fromRgb8(0, 255, 255, 175) );

    const PointF poly2[] = { PointF(140, 260), PointF(210, 310), PointF(160, 340), PointF(110, 310) };
    painter.fillPolygon(poly2, sizeof(poly2) / sizeof(poly2[0]));

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height());
}
