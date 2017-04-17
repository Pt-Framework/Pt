static void testSceneGraph(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    ip2->setAntiAliasingMode(AntiAliasingMode::Default);

    SGNodePath psgn;

    // Generate scaled textures
    Image scaledTexH( textureWithWhiteBackground.format(), Size(textureWithWhiteBackground.width() / 2, textureWithWhiteBackground.height() / 2) );
    Image scaledTexQ( textureWithWhiteBackground.format(), Size(textureWithWhiteBackground.width() / 4, textureWithWhiteBackground.height() / 4) );

    bilinearScale(scaledTexH, textureWithWhiteBackground);
    bilinearScale(scaledTexQ, scaledTexH                );

    //Image scaledTexO( textureWithWhiteBackground.format(), Size(textureWithWhiteBackground.width() / 8, textureWithWhiteBackground.height() / 8) );
    //bilinearScale(scaledTexO, scaledTexQ                );
    //bilinearScale(scaledTexQ, scaledTexO                );

    // Generate a new path
    Path pathPoly4;

    pathPoly4.beginPath();
    pathPoly4.moveTo   (  0, 50); // CCW
    pathPoly4.lineTo   ( 50, 80);
    pathPoly4.lineTo   (100, 50);
    pathPoly4.lineTo   ( 30,  0);
    pathPoly4.endPath  ();

    // Parent's 1st child
    SGNodePath* csgn1 = &psgn.addChild( new SGNodePath( SGNode::RenderFill, pathPoly4 ) );
                csgn1 = csgn1;

    // Parent's 2nd child
    SGNodePath* csgn2 = &psgn.addChild( new SGNodePath( SGNode::RenderFill, pathPoly4 ) );
                csgn2->transform().translate(150, 0);
                csgn2->setBrush( Color::fromRgb8(255, 0, 0) );

        // 2nd-child's 1st child
        SGNodePath* csgn2_1 = &csgn2->addChild( new SGNodePath( SGNode::RenderStrokeAutoClose, pathPoly4 ) );
                    csgn2_1->transform().translate(150, 0);
                    csgn2_1->setPen( Color::fromRgb8(0, 255, 0) );

            // 1st child of the 2nd-child's 1st child
            SGNodeLine* csgn2_1_1 = &csgn2_1->addChild( new SGNodeLine( SGNode::RenderStroke, PointF(50, 50), PointF(200, 100) ) );
                        csgn2_1_1->setPen( Pen(Color::fromRgb8(127, 255, 255, 127), 20, Pen::Solid, Pen::TriangularInCap) );

        // 2nd-child's 2nd child
        SGNodePath* csgn2_2 = &csgn2->addChild( new SGNodePath( SGNode::RenderInherit, pathPoly4 ) );
                    csgn2_2->transform().translate(0, 150);
                    csgn2_2->setBrush( Color::fromRgb8(0, 0, 255) );

        // 2nd-child's 3rd child
        SGNodePath* csgn2_3 = &csgn2->addChild( new SGNodePath( SGNode::RenderStroke, pathPoly4 ) );
                    csgn2_3->transform().translate(150, 150);
                    csgn2_3->setPen( Pen(Color::fromRgb8(255, 255, 127), 6, Pen::Solid, Pen::RoundCap, Pen::MiterJoin) );

    // Parent's 3rd child
    SGNodeRectangle* csgn3 = &psgn.addChild( new SGNodeRectangle( SGNode::RenderFill, RectF(PointF(0, 0), SizeF(80, 80)) ) );
                     csgn3->transform().translate(0, 150);
                     csgn3->setBrush( scaledTexQ );
                     csgn3->setTextureRotationParameters( Color::fromRgb8(0, 255, 255, 255), Brush::Bilinear );

        // 3rd-child's 1st child
        SGNodeRectangle* csgn3_1 = &csgn3->addChild( new SGNodeRectangle( SGNode::RenderFill, RectF(PointF(0, 0), SizeF(80, 80)), 10 ) );
                         csgn3_1->transform().translate(120 * 0, 120 * 1);
                         csgn3_1->setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Rectangular) );

        // 3rd-child's 2nd child
        SGNodeRectangle* csgn3_2 = &csgn3->addChild( new SGNodeRectangle( SGNode::RenderStroke, RectF(PointF(0, 0), SizeF(80, 80)), 10 ) );
                         csgn3_2->transform().translate(120 * 1, 120 * 1);
                         csgn3_2->setPen( Color::fromRgb8(255, 0, 0) );

        // 3rd-child's 3rd child
        SGNodeEllipse* csgn3_3 = &csgn3->addChild( new SGNodeEllipse( SGNode::RenderStroke, PointF(0, 0), SizeF(100, 75) ) );
                       csgn3_3->transform().translate(120 * 2, 120 * 1);
                       csgn3_3->setPen( Color::fromRgb8(0, 255, 0) );

        // 3rd-child's 4th child
        SGNodeEllipse* csgn3_4 = &csgn3->addChild( new SGNodeEllipse( SGNode::RenderFill, PointF(0, 0), SizeF(75, 100) ) );
                       csgn3_4->transform().translate(120 * 3, 120 * 1);
                       csgn3_4->setBrush( Brush(Color::fromRgb8(255, 0, 0, 175), Color::fromRgb8(0, 255, 0, 175), Brush::Conical) );

        // 3rd-child's 5th child
        SGNodeArc* csgn3_5 = &csgn3->addChild( new SGNodeArc( SGNode::RenderStroke, PointF(30, 30), SizeF(30, 30), 30, 240, ArcMode::Open ) );
                   csgn3_5->transform().translate(120 * 4 + 75 * 0, 120 * 1 + 75 * 0);
                   csgn3_5->setPen( Pen(Color::fromRgb8(127, 255, 255, 255), 6, Pen::Dash, Pen::RoundCap) );

        // 3rd-child's 6th child
        SGNodeArc* csgn3_6 = &csgn3->addChild( new SGNodeArc( SGNode::RenderFill, PointF(30, 30), SizeF(30, 30), 30, 240, ArcMode::Chord ) );
                   csgn3_6->transform().translate(120 * 4 + 75 * 0, 120 * 1 + 75 * 1);

        // 3rd-child's 7th child
        SGNodeArc* csgn3_7 = &csgn3->addChild( new SGNodeArc( SGNode::RenderFill, PointF(30, 30), SizeF(30, 30), 30, 240, ArcMode::Pie) );
                   csgn3_7->transform().translate(120 * 4 + 75 * 1, 120 * 1 + 75 * 1);

    // Draw it
    psgn.setPen  (Color::fromRgb8(255, 255, 255));
    psgn.setBrush(Color::fromRgb8(255, 255, 255));

    psgn.transform().rotate(-15);
    psgn.transform().translate(120, 30);
    psgn.draw(*ip2);

    psgn.transform().identity();
    psgn.transform().scale(0.5f, 0.5f); // Note: the pen width is not scaled!
    psgn.transform().translate(650, 30);
    psgn.draw(*ip2);

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
