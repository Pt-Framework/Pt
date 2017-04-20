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

    Pt::uint8_t r = 0, c = 0;

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-dtd-nznv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-direct-with-style.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-with-group.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++r; c = 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nznv-with-group-style.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nzwv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-nzwv-noar-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzanv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzawv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++r; c = 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrnv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrwv-direct.svg");

    svgImgTopLeft.set(200 * c + 10, 200 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "line1-plain-wzrwv-noar-direct.svg");

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}

static void testSvgReader2(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) return;

    PointF svgImgTopLeft;
    Size   svgImgSize(100, 80);

    Pt::uint8_t r = 0, c = 0;

    // First row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/none.svg");

    ++r;

    // Second row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xminymin-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xminymid-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xminymax-meet.svg");

    ++c;

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xminymin-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xminymid-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;  c= 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xminymax-slice.svg");

    // Third row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmidymin-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmidymid-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmidymax-meet.svg");

    ++c;

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmidymin-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmidymid-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;  c= 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmidymax-slice.svg");

    // Fourth row
    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmaxymin-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmaxymid-meet.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmaxymax-meet.svg");

    ++c;

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmaxymin-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++c;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmaxymid-slice.svg");

    svgImgTopLeft.set(120 * c + 10, 100 * r + 10); ++r;  c= 0;
    renderSVG(*ip2, svgImgTopLeft, svgImgSize, "svg_vbtest_px/xmaxymax-slice.svg");


    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
