static const Size renderSVG(ImagePainter2& target, const PointF& svgImageTopLeft, const Size& svgImageSize, const char* fileName)
{
    Image imgReader(ImageFormat::argb32(), svgImageSize);

    const std::string& fnm = std::string(sfileDirXPrefix) + "../etc/images/" + fileName;
    std::ifstream      ifs(fnm.c_str());

    lprintf("%s\n", fnm.c_str());

    SvgReader svgr(ifs, imgReader, PointF(0, 0));
    svgr.get();

    ifs.close();

    target.setAntiAliasingMode(AntiAliasingMode::Default);
    target.drawImage(svgImageTopLeft, imgReader);

    target.setPen( Color::fromRgb8(255, 255, 255, 255) );
    target.drawRect( RectF(PointF(svgImageTopLeft.x() - 1, svgImageTopLeft.y() - 1), SizeF(imgReader.width() + 2, imgReader.height() + 2)) );

    lprintf("--- DONE ---\n\n");

    return Size(imgReader.width(), imgReader.height());
}

static void testSvgReader1(const char* title, Image& image, Painter& painter)
{
    #define SVG_SUB_DIR "svg_absolute_basic/"

    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    Size svgImgSize(180, 180);

    Pt::uint32_t r = 10, c = 10;

    // First row
    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "dtd-nznv-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-nznv-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-nznv-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-nznv-direct-with-style.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-nznv-with-group.svg");
    r += svgImgSize.height() + 10;
    c  = 10;

    // Second row
    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-nznv-with-group-style.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-nzwv-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-nzwv-noar-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-wzanv-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-wzawv-direct.svg");
    r += svgImgSize.height() + 10;
    c  = 10;

    // Third row
    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-wzrnv-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-wzrwv-direct.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "plain-wzrwv-noar-direct.svg");
    c += svgImgSize.width() + 10;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);

    #undef SVG_SUB_DIR
}

static void testSvgReader2(const char* title, Image& image, Painter& painter)
{
    #define SVG_SUB_DIR "svg_vpb_aratio_px/"

    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    Size svgImgSize(100, 80);

    Pt::uint32_t r = 10, c = 10;

    // First row
    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "none.svg");
    r += svgImgSize.height() + 10;
    r += svgImgSize.height() + 10;
    c  = 10;

    // Second row
    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xminymin-meet.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xminymid-meet.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xminymax-meet.svg");
    c += svgImgSize.width() + 10;
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xminymin-slice.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xminymid-slice.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xminymax-slice.svg");
    r += svgImgSize.height() + 10;
    c  = 10;

    // Third row
    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmidymin-meet.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmidymid-meet.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmidymax-meet.svg");
    c += svgImgSize.width() + 10;
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmidymin-slice.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmidymid-slice.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmidymax-slice.svg");
    r += svgImgSize.height() + 10;
    c  = 10;

    // Fourth row
    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmaxymin-meet.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmaxymid-meet.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmaxymax-meet.svg");
    c += svgImgSize.width() + 10;
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmaxymin-slice.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmaxymid-slice.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), svgImgSize, SVG_SUB_DIR "xmaxymax-slice.svg");
    r += svgImgSize.height() + 10;
    c  = 10;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);

    #undef SVG_SUB_DIR
}

static void testSvgReader3(const char* title, Image& image, Painter& painter)
{
    #define SVG_SUB_DIR "svg_basic_defs_use/"

    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    Size svgImgSize;

    Pt::uint32_t r = 10, c = 10;

    // First row
    svgImgSize = renderSVG(*ip2, PointF(c, r), Size(0, 0), SVG_SUB_DIR "use-line1.svg");
    c += svgImgSize.width() + 10;

    svgImgSize = renderSVG(*ip2, PointF(c, r), Size(0, 0), SVG_SUB_DIR "use-line1-with-attr.svg");
    c += svgImgSize.width() + 10;

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);

    #undef SVG_SUB_DIR
}
