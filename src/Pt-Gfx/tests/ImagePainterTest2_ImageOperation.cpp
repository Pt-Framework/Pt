static void testImageOperation(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    painter.setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    painter.setPen( Color::fromRgb8(255, 255, 0, 255) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    // Reference image
    Pt::int32_t x = 20;
    Pt::int32_t y = 20;

    painter.drawImage(PointF(x, y), textureWithWhiteBackground);
    painter.drawText( PointF(x, y + textureWithWhiteBackground.height() + 20), "Original (Reference) Image" );
    x += textureWithWhiteBackground.width() + 50;

    // Scaled and rotated image
    Image srImage( textureWithWhiteBackground.format(), Size(120, 120) );

    // Block scale
    blockScaleImage(textureWithWhiteBackground, srImage);
    painter.drawImage(PointF(x, y), srImage);
    painter.drawText( PointF(x, y + srImage.height() + 20), "Block Scaling" );
    x += srImage.width() + 50;

    // Bilinear scale
    bilinearScaleImage(textureWithWhiteBackground, srImage);
    painter.drawImage(PointF(x, y), srImage);
    painter.drawText( PointF(x, y + srImage.height() + 20), "Bilinear Scaling" );

    x  = 20 + textureWithWhiteBackground.width() + 50;
    y += srImage.height() + 50;

    // Block rotate - normal
    blockRotateImage(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 127, 127, 255), false);
    painter.drawImage(PointF(x, y), srImage);
    painter.drawText( PointF(x, y + srImage.height() + 20), "Block Rotate" );
    x += srImage.width() + 50;

    // Block rotate - full
    blockRotateImage(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 127, 127, 255), true);
    painter.drawImage(PointF(x, y), srImage);
    painter.drawText( PointF(x, y + srImage.height() + 20), "Block Rotate (Full-fit)" );

    x  = 20 + textureWithWhiteBackground.width() + 50;
    y += srImage.height() + 50;

    // Bilinear rotate - normal
    bilinearRotateImage(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 127, 127, 255), false);
    painter.drawImage(PointF(x, y), srImage);
    painter.drawText( PointF(x, y + srImage.height() + 20), "Bilinear Rotate" );
    x += srImage.width() + 50;

    // Bilinear rotate - fullscale
    bilinearRotateImage(textureWithWhiteBackground, srImage, 30, Color::fromRgb8(0, 127, 127, 255), true);
    painter.drawImage(PointF(x, y), srImage);
    painter.drawText( PointF(x, y + srImage.height() + 20), "Bilinear Rotate (Full-fit)" );
    x += srImage.width() + 50;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
