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
    x += textureWithWhiteBackground.width() + 50;

    // Scaled and rotated image (block scale)
    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    // Block scale
    blockScale4(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Block Scaling" );
    x += scaledImage.width() + 50;

    // Bilinear scale
    bilinearScale4(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Bilinear Scaling" );

    x  = 20 + textureWithWhiteBackground.width() + 50;
    y += scaledImage.height() + 50;

    // Block rotate - normal
    blockRotate4<false>(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height(),
        30
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Block Rotate" );
    x += scaledImage.width() + 50;

    // Block rotate - full
    blockRotate4<true>(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height(),
        30
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Block Rotate (Fullscale)" );

    x  = 20 + textureWithWhiteBackground.width() + 50;
    y += scaledImage.height() + 50;

    // Bilinear rotate - normal
    bilinearRotate4<false>(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height(),
        30
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Bilinear Rotate" );
    x += scaledImage.width() + 50;

    // Bilinear rotate - fullscale
    bilinearRotate4<true>(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height(),
        30
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Bilinear Rotate (Fullscale)" );
    x += scaledImage.width() + 50;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
