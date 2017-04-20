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
    #define SVG_SUB_DIR "svg_absolute_basic/"

    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    PointF svgImgTopLeft;
    Size   svgImgSize(180, 180);

    Pt::uint8_t r = 0, c = 0;

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "dtd-nznv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-nznv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-nznv-direct-with-style.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-nznv-with-group.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++r; c = 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-nznv-with-group-style.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-nzwv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-nzwv-noar-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-wzanv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-wzawv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++r; c = 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-wzrnv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-wzrwv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "plain-wzrwv-noar-direct.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);

    #undef SVG_SUB_DIR
}

static void testSvgReader2(const char* title, Image& image, Painter& painter)
{
    #define SVG_SUB_DIR "svg_vpb_aratio_px/"

    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    PointF svgImgTopLeft;
    Size   svgImgSize(100, 80);

    Pt::uint8_t r = 0, c = 0;

    // First row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "none.svg");

    ++r;

    // Second row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xminymin-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xminymid-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xminymax-meet.svg");

    ++c;

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xminymin-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xminymid-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;  c= 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xminymax-slice.svg");

    // Third row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmidymin-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmidymid-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmidymax-meet.svg");

    ++c;

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmidymin-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmidymid-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;  c= 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmidymax-slice.svg");

    // Fourth row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmaxymin-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmaxymid-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmaxymax-meet.svg");

    ++c;

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmaxymin-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmaxymid-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;  c= 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, SVG_SUB_DIR "xmaxymax-slice.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);

    #undef SVG_SUB_DIR
}
