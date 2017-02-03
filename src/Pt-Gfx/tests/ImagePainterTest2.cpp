// ./jam.sh configure --with-hmi -sGUI=linux-fb --with-rasterizer2

#include <iomanip>

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

    const int testDraw    = 1;
    const int doBenchmark = 1;

    // Lines
    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawLine("Lines and Texts - SourceCopy", image, painter);
    }
    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawLine("Lines and Texts - SourceOver", image, painter);
    }

    // Rectangles and solid-filled rectangles
    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawRect("Rectangles & Solid-Filled Rectangles - SourceCopy", image, painter);
    }

    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawRect("Rectangles & Solid-Filled Rectangles - SourceOver", image, painter);
    }

    // Solid-filled polygons
    if(1 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceCopy", image, painter);
    }
    if(1 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceOver", image, painter);
    }

    // Benchmark
    if(doBenchmark) {
        std::clog << std::fixed << std::setprecision(0) << std::endl;

        std::clog << "CompositionMode::SourceCopy" << std::endl;
        doBenchMark(CompositionMode::SourceCopy);

        std::clog << "CompositionMode::SourceOver" << std::endl;
        doBenchMark(CompositionMode::SourceOver);
    }

    return 0;
}
