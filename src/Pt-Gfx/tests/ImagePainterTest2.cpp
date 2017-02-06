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

#define DO_TEST_DRAW    1
#define DO_BENCHMARKING 1

//

#define TEST_SOURCECOPY                        1
#define TEST_SOURCEOVER                        1

#define TEST_DRAW_LINE_AND_TEXT                0
#define TEST_DRAW_RECTANGLES_FILLED_RECTANGLES 1
#define TEST_DRAW_SOLID_FILLED_POLYGONS        0

//

#define BENCHMARK_CHECK_RESULTING_IMAGE  0
#define BENCHMARK_IMAGE_SIZE             Size(1280, 800)
#define BENCHMARK_LOOP_COUNT             250

#define BENCHMARK_TEXT                      0
#define BENCHMARK_LINE                      0

#define BENCHMARK_RECTANGLE                 1
#define BENCHMARK_SOLID_FILLED_RECTANGLE    1
#define BENCHMARK_GRADIENT_FILLED_RECTANGLE 1

#define BENCHMARK_SOLID_FILLED_POLYGON      0

//

#define FONT_DIR  "../src/Pt-Gfx/fonts"
#define FONT_SPEC "DejaVu Serif", 24, Pt::Gfx::Font::BoldItalic

#include "ImagePainterTest2_Util.cpp"
#include "ImagePainterTest2_Draw.cpp"
#include "ImagePainterTest2_Benchmark.cpp"

int main(int argc, char* args[])
{
    Image         image( ImageFormat::argb32(), Size(800, 600) );
    ImagePainter  painter1obj(image);
    ImagePainter2 painter2obj(image);

    painter1obj.setFontDir( Pt::System::Path(FONT_DIR) );
    painter1obj.setFont( Pt::Gfx::Font(FONT_SPEC) );

    painter2obj.setFontDir( Pt::System::Path(FONT_DIR) );
    painter2obj.setFont( Pt::Gfx::Font(FONT_SPEC) );

    Painter* painter1 = dynamic_cast<Painter*>(&painter1obj);
    Painter* painter2 = dynamic_cast<Painter*>(&painter2obj);

    // Lines
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_LINE_AND_TEXT) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawLine("Lines and Texts - ImagePainter2 [SourceCopy]", image, *painter2);
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_LINE_AND_TEXT) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawLine("Lines and Texts - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Rectangles and solid-filled rectangles
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_RECTANGLES_FILLED_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter2 [SourceCopy]", image, *painter2);
        testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter1 [SourceCopy]", image, *painter1);
    }

    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_RECTANGLES_FILLED_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter2 [SourceOver]", image, *painter2);
        testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter1 [SourceOver]", image, *painter1);
    }

    // Solid-filled polygons
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidFillPolygon("Solid-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2);
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidFillPolygon("Solid-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2);
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
