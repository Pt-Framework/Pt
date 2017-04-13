static void testImageScaling(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));

    // Reference image
    Pt::int32_t x = 0;
    Pt::int32_t y = 0;

    painter.drawImage(PointF(x, y), textureWithWhiteBackground);
    x += textureWithWhiteBackground.width() + 20;

    // Scaled image (block scale)
    Image scaledImage( image.format(), Size(120, 120) );

    blockScale(
        textureWithWhiteBackground.begin(), textureWithWhiteBackground.width(), textureWithWhiteBackground.height(),
        scaledImage               .begin(), scaledImage               .width(), scaledImage               .height()
    );
    painter.drawImage(PointF(x, y), scaledImage);
    x += scaledImage.width() + 20;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
