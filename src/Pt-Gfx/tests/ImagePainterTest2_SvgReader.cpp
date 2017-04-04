static void testSvgReader(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    painter.setCompositionMode(CompositionMode::SourceOver);

    AffineTransform atrans;

    SvgReader       svgReader;
    std::string     fnmReader;
    std::ifstream   ifsReader;
    Image           imgReader( ImageFormat::argb32(), image.size() );

    //
    //*
    atrans.translate(0, 0, true);
    resetImage(imgReader);
    fnmReader = std::string(sfileDirXPrefix) + "../etc/images/line1-dtd-direct.svg";
    ifsReader.open(fnmReader.c_str());
    svgReader.attach(ifsReader, imgReader, atrans);
    svgReader.get();
    ifsReader.close();
    painter.drawImage(PointF(0, 0), imgReader);
    lprintf("\n");
    //*/

    //
    //*
    atrans.translate(125, 0, true);
    resetImage(imgReader);
    fnmReader = std::string(sfileDirXPrefix) + "../etc/images/line1-plain-direct.svg";
    ifsReader.open(fnmReader.c_str());
    svgReader.attach(ifsReader, imgReader, atrans);
    svgReader.get();
    ifsReader.close();
    painter.drawImage(PointF(0, 0), imgReader);
    lprintf("\n");
    //*/

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
