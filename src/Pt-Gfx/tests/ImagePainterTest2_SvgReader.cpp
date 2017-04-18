static void renderSVG(ImagePainter2& targetImagePainter, const PointF& targetImageXY, const Size& targetImageSize, const Transform& transform, const char* fileName)
{
    Image imgReader(ImageFormat::argb32(), targetImageSize);

    const std::string& fnm = std::string(sfileDirXPrefix) + "../etc/images/" + fileName;
    std::ifstream      ifs(fnm.c_str());

    SvgReader svgr(ifs, imgReader, transform);
    svgr.get();

    ifs.close();

    targetImagePainter.setAntiAliasingMode(AntiAliasingMode::Default);
    targetImagePainter.setCompositionMode(CompositionMode::SourceOver);
    targetImagePainter.drawImage(targetImageXY, imgReader);

    lprintf("\n");
}

static void testSvgReader1(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    Transform transform;

    transform.translate(125 * 0, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-dtd-nznv-direct.svg");

    transform.translate(125 * 1, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-nznv-direct.svg");

    transform.translate(125 * 2, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-nznv-direct-with-style.svg");

    transform.translate(125 * 3, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-nznv-with-group.svg");

    transform.translate(125 * 4, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-nznv-with-group-style.svg");

    transform.translate(125 * 5, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-nzwv-direct.svg");

    transform.translate(125 * 6, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-nzwv-noar-direct.svg");

    transform.translate(125 * 7, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-wzanv-direct.svg");

    transform.translate(125 * 8, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-wzawv-direct.svg");

    transform.translate(125 * 9, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-wzrnv-direct.svg");

    transform.translate(125 * 10, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-wzrwv-direct.svg");

    transform.translate(125 * 11, 125 * 0, true);
    renderSVG(*ip2, PointF(0, 0), image.size(), transform, "line1-plain-wzrwv-noar-direct.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
