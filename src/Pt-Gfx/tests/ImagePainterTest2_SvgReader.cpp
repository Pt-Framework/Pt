static void testSvgReader(const char* title, Image& image, Painter& painter)
{
    resetImage(image);

    ImagePainter2* ip2 = dynamic_cast<ImagePainter2*>(dynamic_cast<Painter*>(&painter));
    if(!ip2) ip2->setAntiAliasingMode(AntiAliasingMode::Standard);

    SvgReader     svgReader;
    std::string   fnmReader;
    std::ifstream ifsReader;
    Image         imgReader( ImageFormat::argb32(), image.size() );

    //
    fnmReader = std::string(sfileDirXPrefix) + "../etc/images/line1-dtd-direct.svg";
    ifsReader.open(fnmReader.c_str());
    svgReader.attach(ifsReader, imgReader);
    resetImage(imgReader);
    svgReader.get();

    sdlPreviewRGB888Buffer(title, image.data(), image.width(), image.height(), !!ip2);
}
