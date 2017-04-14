static void testImageOperation(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    painter.setFont( Pt::Gfx::Font(FONT_SPEC_S) );
    painter.setPen( Color::fromRgb8(255, 255, 0, 255) );

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    // Reference image
    Pt::int32_t x = 0;
    Pt::int32_t y = 0;

    painter.drawImage(PointF(x, y), textureWithWhiteBackground);
    x += textureWithWhiteBackground.width() + 20;

    // Scaled image (block scale)
    Image scaledImage( textureWithWhiteBackground.format(), Size(120, 120) );

    blockScale4(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Block Scaling" );
    x += scaledImage.width() + 20;

    // Scaled image (bilinear scale)
    bilinearScale4(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    painter.drawText( PointF(x, y + scaledImage.height() + 20), "Bilinear Scaling" );
    x += scaledImage.width() + 20;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
