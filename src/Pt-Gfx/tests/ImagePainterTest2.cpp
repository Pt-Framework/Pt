// ./jam.sh configure --with-hmi -sGUI=linux-fb --with-rasterizer2

#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <unistd.h>

using namespace Pt::Gfx;

#define FONT_DIR  "../src/Pt-Gfx/fonts"
#define FONT_SPEC "DejaVu Serif", 24, Pt::Gfx::Font::BoldItalic

#include "ImagePainterTest2_Util.cpp"
#include "ImagePainterTest2_Draw.cpp"
#include "ImagePainterTest2_Benchmark.cpp"

int main(int argc, char* args[])
{
    Image         image( ImageFormat::argb32(), Size(800, 600) );
    ImagePainter2 painter(image);

    painter.setFontDir( Pt::System::Path(FONT_DIR) );
    painter.setFont( Pt::Gfx::Font(FONT_SPEC) );

    if(1) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawLine("Lines and Texts - SourceCopy", image, painter);
    }

    if(1) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawLine("Lines and Texts - SourceOver", image, painter);
    }

    if(1) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceCopy", image, painter);
    }

    if(1) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceOver", image, painter);
    }

    if(1) {
        double time1, time2;

        std::clog << std::endl;

        time1 = benchDrawText            <ImagePainter >("Drawing text using ImagePainter                  = ");
        time2 = benchDrawText            <ImagePainter2>("Drawing text using ImagePainter2                 = ");
        std::clog <<  "Slow-down factor                                 = " << (time2 / time1) << "" << std::endl << std::endl;

        time1 = benchDrawLine            <ImagePainter >("Drawing line using ImagePainter                  = ");
        time2 = benchDrawLine            <ImagePainter2>("Drawing line using ImagePainter2                 = ");
        std::clog <<  "Slow-down factor                                 = " << (time2 / time1) << "" << std::endl << std::endl;

        time1 = benchDrawSolidFillPolygon<ImagePainter >("Drawing solid-filled polygon using ImagePainter  = ");
        time2 = benchDrawSolidFillPolygon<ImagePainter2>("Drawing solid-filled polygon using ImagePainter2 = ");
        std::clog <<  "Slow-down factor                                 = " << (time2 / time1) << "" << std::endl << std::endl;
    }

    return 0;
}

/*

*/
