static void testSceneGraph(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    SGNodePath sgn;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
