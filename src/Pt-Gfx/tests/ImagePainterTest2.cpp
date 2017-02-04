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

#define DO_BENCHMARKING                  0
#define BENCHMARK_CHECK_RESULTING_IMAGE  0
#define BENCHMARK_IMAGE_SIZE             Size(1280, 800)
#define BENCHMARK_LOOP_COUNT             250

#define BENCHMARK_TEXT                   0
#define BENCHMARK_LINE                   0
#define BENCHMARK_RECTANGLE              0
#define BENCHMARK_SOLID_FILLED_RECTANGLE 0
#define BENCHMARK_SOLID_FILLED_POLYGON   1

#define TEST_SOURCECOPY                        1
#define TEST_SOURCEOVER                        0

#define TEST_DRAW_LINE_AND_TEXT                0
#define TEST_DRAW_RECTANGLES_FILLED_RECTANGLES 0
#define TEST_DRAW_SOLID_FILLED_POLYGONS        1

#include "ImagePainterTest2_Util.cpp"
#include "ImagePainterTest2_Draw.cpp"
#include "ImagePainterTest2_Benchmark.cpp"

int main(int argc, char* args[])
{
    Image         image( ImageFormat::argb32(), Size(800, 600) );
    ImagePainter2 painter(image);

    painter.setFontDir( Pt::System::Path(FONT_DIR) );
    painter.setFont( Pt::Gfx::Font(FONT_SPEC) );

    // Lines
    if(TEST_SOURCECOPY && TEST_DRAW_LINE_AND_TEXT) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawLine("Lines and Texts - SourceCopy", image, painter);
    }
    if(TEST_SOURCEOVER && TEST_DRAW_LINE_AND_TEXT) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawLine("Lines and Texts - SourceOver", image, painter);
    }

    // Rectangles and solid-filled rectangles
    if(TEST_SOURCECOPY && TEST_DRAW_RECTANGLES_FILLED_RECTANGLES) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawRect("Rectangles & Solid-Filled Rectangles - SourceCopy", image, painter);
    }

    if(TEST_SOURCEOVER && TEST_DRAW_RECTANGLES_FILLED_RECTANGLES) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawRect("Rectangles & Solid-Filled Rectangles - SourceOver", image, painter);
    }

    // Solid-filled polygons
    if(TEST_SOURCECOPY && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceCopy", image, painter);
    }
    if(TEST_SOURCEOVER && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidFillPolygon("Solid-Filled Polygons - SourceOver", image, painter);
    }

    // Benchmark
    if(DO_BENCHMARKING) {
        std::clog << std::fixed << std::setprecision(0) << std::endl;

        std::clog << "CompositionMode::SourceCopy" << std::endl;
        doBenchMark(CompositionMode::SourceCopy);

        std::clog << "CompositionMode::SourceOver" << std::endl;
        doBenchMark(CompositionMode::SourceOver);
    }

    return 0;
}
