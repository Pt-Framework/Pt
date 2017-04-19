static void renderSVG(ImagePainter2& targetImagePainter, const PointF& svgImageTopLeft, const Size& svgImageSize, const char* fileName)
{
    Image imgReader(ImageFormat::argb32(), svgImageSize);

    const std::string& fnm = std::string(sfileDirXPrefix) + "../etc/images/" + fileName;
    std::ifstream      ifs(fnm.c_str());

    lprintf("%s\n", fnm.c_str());

    SvgReader svgr(ifs, imgReader, PointF(0, 0));
    svgr.get();

    ifs.close();

    targetImagePainter.setAntiAliasingMode(AntiAliasingMode::Default);
    targetImagePainter.setCompositionMode(CompositionMode::SourceOver);
    targetImagePainter.drawImage(svgImageTopLeft, imgReader);

    lprintf("--- DONE ---\n\n");
}

static void testSvgReader1(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    PointF svgImgTopLeft;
    Size   svgImgSize(200, 200);

    svgImgTopLeft.set(200 * 0, 200 * 0);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-dtd-nznv-direct.svg");

    svgImgTopLeft.set(200 * 1, 200 * 0);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-direct.svg");

    svgImgTopLeft.set(200 * 2, 200 * 0);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-direct-with-style.svg");

    svgImgTopLeft.set(200 * 3, 200 * 0);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-with-group.svg");

    svgImgTopLeft.set(200 * 4, 200 * 0);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-with-group-style.svg");

    svgImgTopLeft.set(200 * 0, 200 * 1);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nzwv-direct.svg");

    svgImgTopLeft.set(200 * 1, 200 * 1);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nzwv-noar-direct.svg");

    svgImgTopLeft.set(200 * 2, 200 * 1);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzanv-direct.svg");

    svgImgTopLeft.set(200 * 3, 200 * 1);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzawv-direct.svg");

    svgImgTopLeft.set(200 * 4, 200 * 1);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrnv-direct.svg");

    svgImgTopLeft.set(200 * 0, 200 * 2);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrwv-direct.svg");

    svgImgTopLeft.set(200 * 1, 200 * 2);
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrwv-noar-direct.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
