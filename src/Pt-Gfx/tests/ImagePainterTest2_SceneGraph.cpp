static void testSceneGraph(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    TransformStack tstack;
    Transform      transform;
    SGNodePath     sgn;

    SGNodePath*    csgn;

    // Generate a new path
    Path pathPoly4;

    pathPoly4.beginPath();
    pathPoly4.moveTo   (  0, 50); // CCW
    pathPoly4.lineTo   ( 50, 80);
    pathPoly4.lineTo   (100, 50);
    pathPoly4.lineTo   ( 30,  0);
    pathPoly4.endPath  ();

    // Parent's 1st child
    csgn = &sgn.addChild(new SGNodePath(SGNode::RenderFill, pathPoly4));

    // Parent's 2nd child
    csgn = &sgn.addChild(new SGNodePath(SGNode::RenderFill, pathPoly4));
    csgn->transform().translate(150, 0);
    csgn->setBrush(Color::fromRgb8(255, 0, 0));

    // 2nd-child's 1st child
    csgn = &csgn->addChild(new SGNodePath(SGNode::RenderStrokeAutoClose, pathPoly4));
    csgn->transform().translate(150, 0);
    csgn->setPen(Color::fromRgb8(0, 255, 0));

    // Draw it
    sgn.transform().rotate(-15);
    sgn.transform().translate(100, 100);

    sgn.setPen  (Color::fromRgb8(255, 255, 255));
    sgn.setBrush(Color::fromRgb8(255, 255, 255));
    sgn.draw(*ip2, tstack, transform);

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
