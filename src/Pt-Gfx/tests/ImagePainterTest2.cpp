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
    if(1 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawLine("Lines and Texts - SourceCopy", image, painter);
    }
    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawLine("Lines and Texts - SourceOver", image, painter);
    }

    // Solid-filled polygons
    painter.setAntiAliasingQuality(0);
    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceCopy", image, painter);
    }
    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceOver", image, painter);
    }

    painter.setAntiAliasingQuality(1);
    if(1 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceCopy", image, painter);
    }
    if(0 && testDraw) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceOver", image, painter);
    }

    // Benchmark
    if(1 && doBenchmark) {
        double time1, time2, time3;
        std::clog << std::fixed << std::setprecision(0) << std::endl;

        time1 = benchDrawText<ImagePainter >();
        time2 = benchDrawText<ImagePainter2>();
        std::clog << "Text                 @ ImagePainter      = " << std::setw(4) << time1 << std::endl;
        std::clog << "Text                 @ ImagePainter2     = " << std::setw(4) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;

        time1 = benchDrawLine<ImagePainter >();
        time2 = benchDrawLine<ImagePainter2>();
        std::clog << "Line                 @ ImagePainter      = " << std::setw(4) << time1 << std::endl;
        std::clog << "Line                 @ ImagePainter2     = " << std::setw(4) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;

        time1 = benchDrawSolidFillPolygon<ImagePainter >( );
        time2 = benchDrawSolidFillPolygon<ImagePainter2>(0);
        time3 = benchDrawSolidFillPolygon<ImagePainter2>(1);
        std::clog << "Solid-filled polygon @ ImagePainter      = " << std::setw(4) << time1 << std::endl;
        std::clog << "Solid-filled polygon @ ImagePainter2 (N) = " << std::setw(4) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << "Solid-filled polygon @ ImagePainter2 (P) = " << std::setw(4) << time3
                  << " (" << std::setw(6) << std::setprecision(3) << (time3 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    return 0;
}
