static void renderSVG(ImagePainter2& targetImagePainter, const PointF& targetImageXY, const Size& targetImageSize, const AffineTransform& atrans, const char* fileName)
{
    Image imgReader(ImageFormat::argb32(), targetImageSize);

    const std::string& fnm = std::string(sfileDirXPrefix) + fileName;
    std::ifstream      ifs(fnm.c_str());

    SvgReader svgr(ifs, imgReader, atrans);
    svgr.get();

    ifs.close();

    targetImagePainter.setAntiAliasingMode(AntiAliasingMode::Standard);
    targetImagePainter.setCompositionMode(CompositionMode::SourceOver);
    targetImagePainter.drawImage(targetImageXY, imgReader);

    lprintf("\n");
}

static void testSvgReader(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    AffineTransform atrans;

    atrans.translate(0, 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), atrans, "../etc/images/line1-dtd-direct.svg");

    atrans.translate(125, 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), atrans, "../etc/images/line1-plain-direct.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
