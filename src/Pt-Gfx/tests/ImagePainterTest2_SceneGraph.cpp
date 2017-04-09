static void testSceneGraph(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    SGNodePath sgn;

    // Generate a new path
    Path pathPoly4;

    pathPoly4.beginPath();
    pathPoly4.moveTo   (  0, 50); // CCW
    pathPoly4.lineTo   ( 50, 80);
    pathPoly4.lineTo   (100, 50);
    pathPoly4.lineTo   ( 30,  0);
    pathPoly4.endPath  ();

    // Parent's 1st child
    SGNodePath* csgn1 = &sgn.addChild(new SGNodePath(SGNode::RenderFill, pathPoly4));
                csgn1 = csgn1;

    // Parent's 2nd child
    SGNodePath* csgn2 = &sgn.addChild(new SGNodePath(SGNode::RenderFill, pathPoly4));
                csgn2->transform().translate(150, 0);
                csgn2->setBrush(Color::fromRgb8(255, 0, 0));

    // 2nd-child's 1st child
    SGNodePath* csgn2_1 = &csgn2->addChild(new SGNodePath(SGNode::RenderStrokeAutoClose, pathPoly4));
                csgn2_1->transform().translate(150, 0);
                csgn2_1->setPen(Color::fromRgb8(0, 255, 0));

    // 2nd-child's 2nd child
    SGNodePath* csgn2_2 = &csgn2->addChild(new SGNodePath(SGNode::RenderInherit, pathPoly4));
                csgn2_2->setBrush(Color::fromRgb8(0, 0, 255));
                csgn2_2->transform().translate(0, 150);

    // 2nd-child's 3rd child
    SGNodePath* csgn2_3 = &csgn2->addChild(new SGNodePath(SGNode::RenderStroke, pathPoly4));
                csgn2_3->setPen(Pen(Color::fromRgb8(255, 255, 127), 6, Pen::Solid, Pen::RoundCap, Pen::MiterJoin));
                csgn2_3->transform().translate(150, 150);

    // 1st child of the 2nd-child's 1st child
    SGNodeLine* csgn2_1_1 = &csgn2_1->addChild(new SGNodeLine(SGNode::RenderStroke, PointF(50, 50), PointF(200, 100)));
                csgn2_1_1->setPen(Pen(Color::fromRgb8(127, 255, 255, 127), 20, Pen::Solid, Pen::TriangularInCap));

    // Draw it
    sgn.setPen  (Color::fromRgb8(255, 255, 255));
    sgn.setBrush(Color::fromRgb8(255, 255, 255));

    sgn.transform().rotate(-15);
    sgn.transform().translate(50, 50);
    sgn.draw(*ip2);

    sgn.transform().identity();
    sgn.transform().scale(0.5f, 0.5f);
    sgn.transform().translate(500, 50);
    sgn.draw(*ip2);

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
