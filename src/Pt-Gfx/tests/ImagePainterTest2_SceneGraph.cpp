static void testSceneGraph(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    TransformStack tstack;
    Transform      transform;
    SGNodePath     sgn;
    Path*          path;

    path = &sgn.addChild(new SGNodePath).path();

    path->beginPath();
    path->moveTo   (  0, 50); // CCW
    path->lineTo   ( 50, 80);
    path->lineTo   (100, 50);
    path->lineTo   ( 30,  0);
    path->endPath  ();

    sgn.draw(*ip2, tstack, transform);

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
