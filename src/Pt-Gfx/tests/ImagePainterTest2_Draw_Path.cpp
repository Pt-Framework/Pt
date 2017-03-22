static void testDrawPath(const char* title, Image& image, Painter& painter, const Brush& brush1, const Brush& brush2)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    // ### TODO ###

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
