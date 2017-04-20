static void renderSVG(ImagePainter2& target, const PointF& svgImageTopLeft, const Size& svgImageSize, const char* fileName)
{
    Image imgReader(ImageFormat::argb32(), svgImageSize);

    const std::string& fnm = std::string(sfileDirXPrefix) + "../etc/images/" + fileName;
    std::ifstream      ifs(fnm.c_str());

    lprintf("%s\n", fnm.c_str());

    SvgReader svgr(ifs, imgReader, PointF(0, 0));
    svgr.get();

    ifs.close();

    target.setAntiAliasingMode(AntiAliasingMode::Default);
    target.setCompositionMode(CompositionMode::SourceOver);
    target.drawImage(svgImageTopLeft, imgReader);

    target.setPen( Color::fromRgb8(255, 255, 255, 255) );
    target.drawRect( RectF(PointF(svgImageTopLeft.x() - 1, svgImageTopLeft.y() - 1), SizeF(svgImageSize.width() + 2, svgImageSize.height() + 2)) );

    lprintf("--- DONE ---\n\n");
}

static void testSvgReader1(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    PointF svgImgTopLeft;
    Size   svgImgSize(180, 180);

    svgImgTopLeft.set(200 * 0 + 10, 200 * 0 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-dtd-nznv-direct.svg");

    svgImgTopLeft.set(200 * 1 + 10, 200 * 0 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-direct.svg");

    svgImgTopLeft.set(200 * 2 + 10, 200 * 0 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-direct-with-style.svg");

    svgImgTopLeft.set(200 * 3 + 10, 200 * 0 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-with-group.svg");

    svgImgTopLeft.set(200 * 4 + 10, 200 * 0 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-with-group-style.svg");

    svgImgTopLeft.set(200 * 0 + 10, 200 * 1 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nzwv-direct.svg");

    svgImgTopLeft.set(200 * 1 + 10, 200 * 1 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nzwv-noar-direct.svg");

    svgImgTopLeft.set(200 * 2 + 10, 200 * 1 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzanv-direct.svg");

    svgImgTopLeft.set(200 * 3 + 10, 200 * 1 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzawv-direct.svg");

    svgImgTopLeft.set(200 * 4 + 10, 200 * 1 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrnv-direct.svg");

    svgImgTopLeft.set(200 * 0 + 10, 200 * 2 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrwv-direct.svg");

    svgImgTopLeft.set(200 * 1 + 10, 200 * 2 + 10);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrwv-noar-direct.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

static void testSvgReader2(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    //PointF svgImgTopLeft;
    //Size   svgImgSize(180, 180);

    //svgImgTopLeft.set(200 * 0 + 10, 200 * 0 + 10);
    //renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-dtd-nznv-direct.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
