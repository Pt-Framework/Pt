// CentOS 7
//     ./jam.sh configure --with-rasterizer2 --with-hmi -sGUI=linux-fb -sOPTIM=-O2
//
// Raspbian Jessie 2017/01/11
//     ./jam.sh configure --with-rasterizer2 --with-hmi -sGUI=linux-fb -sOPTIM=-O2 -with-libpng
//
// while true; do ps -aF | grep "[I]magePainterTest2"; done
//
// perf record -d -g -T -e cycles,instructions,cache-references,cache-misses,bus-cycles ./ImagePainterTest2
// perf report
//
// perf archive
//     Now please run: 'tar xvf perf.data.tar.bz2 -C ~/.debug'
//     wherever you need to run 'perf report' on.
//

#include <fstream>
#include <iomanip>

#include <unistd.h>

#include <Pt/Math.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/PngReader.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>

#include <cairo.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

using namespace Pt::Gfx;

// Comparison with Cairo (solid-filled polygons and ellipses only)
#define DO_BENCHMARKING_CAIRO                 1
#define BENCHMARK_CAIRO_CHECK_RESULTING_IMAGE 0

// General settings for Pt-Gfx
#define DO_TEST_DRAW    1
#define DO_BENCHMARKING 0

// Detailed-test enable settings for Pt-Gfx
#define TEST_SOURCECOPY                        1
#define TEST_SOURCEOVER                        0

#define TEST_DRAW_LINE_AND_TEXT                0
#define TEST_DRAW_RECTANGLES_FILLED_RECTANGLES 0

#define TEST_DRAW_SOLID_FILLED_POLYGONS        1
#define TEST_DRAW_GRADIENT_FILLED_POLYGONS     0
#define TEST_DRAW_TEXTURE_FILLED_POLYGONS      0

#define TEST_DRAW_SOLID_FILLED_ELLIPSE         0
#define TEST_DRAW_GRADIENT_FILLED_ELLIPSE      0
#define TEST_DRAW_TEXTURE_FILLED_ELLIPSE       0

#define TEST_COMPARE_WITH_OLD_PAINTER          0 // (for some shapes only)

// Detailed-test benchmark settings for Pt-Gfx and some for Cairo/GD
#define BENCHMARK_CHECK_RESULTING_IMAGE     0

#define BENCHMARK_IMAGE_SIZE                Size(1280, 800)
#define BENCHMARK_LOOP_COUNT                500

#define BENCHMARK_TEXT                      1
#define BENCHMARK_LINE                      1

#define BENCHMARK_RECTANGLE                 1
#define BENCHMARK_SOLID_FILLED_RECTANGLE    1
#define BENCHMARK_GRADIENT_FILLED_RECTANGLE 1
#define BENCHMARK_TEXTURE_FILLED_RECTANGLE  1

#define BENCHMARK_SOLID_FILLED_POLYGON      1
#define BENCHMARK_GRADIENT_FILLED_POLYGON   1
#define BENCHMARK_TEXTURE_FILLED_POLYGON    1

#define BENCHMARK_SOLID_FILLED_ELLIPSE      1
#define BENCHMARK_GRADIENT_FILLED_ELLIPSE   1
#define BENCHMARK_TEXTURE_FILLED_ELLIPSE    1

// Configurations and objects
#define FONT_DIR  "../src/Pt-Gfx/fonts"
#define FONT_SPEC "DejaVu Serif", 24, Pt::Gfx::Font::BoldItalic

static Image textureWithTransBackground;
static Image textureWithWhiteBackground;

static Brush bmBrushSolid;
static Brush bmBrushGradientH;
static Brush bmBrushGradientV;
static Brush bmBrushTextureT;
static Brush bmBrushTextureW;

// Include the other source files
#include "ImagePainterTest2_Util.cpp"
#include "ImagePainterTest2_Draw.cpp"
#include "ImagePainterTest2_Benchmark.cpp"
#include "ImagePainterTest2_Cairo.cpp"

int main(int argc, char* args[])
{
    // Load the textures
    std::ifstream tbgrIfs("../etc/images/bleech-200x200-tbgr.png");
    PngReader     tbgrPng(tbgrIfs, textureWithTransBackground);
    tbgrPng.get();

    std::ifstream wbgrIfs("../etc/images/bleech-200x200-wbgr.png");
    PngReader     wbgrPng(wbgrIfs, textureWithWhiteBackground);
    wbgrPng.get();

    // Prepare the images and painters
    Image         image( ImageFormat::argb32(), Size(1000, 600) );
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

    // Rectangles and filled rectangles
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_RECTANGLES_FILLED_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_RECTANGLES_FILLED_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawRect("Rectangles & Solid-Filled Rectangles - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Create the brushes used for drawing
    const Brush brushSolid1   (Color::fromRgb8(  0, 255, 0, 175));
    const Brush brushGradient1(Color::fromRgb8(  0, 255, 0, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Vertical);
    const Brush brushTexture1 (textureWithTransBackground);

    const Brush brushSolid2   (Color::fromRgb8(  0, 255, 255, 175));
    const Brush brushGradient2(Color::fromRgb8(  0, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Horizontal);
    const Brush brushTexture2 (textureWithWhiteBackground);

    // Solid-filled polygons
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillPolygon("Solid-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2, brushSolid1, brushSolid2);
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Solid-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushSolid1, brushSolid2);
    }

    // Gradient-filled polygons
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_GRADIENT_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2, brushGradient1, brushGradient2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter [SourceCopy]", image, *painter1, brushGradient1, brushGradient2);
        }
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_GRADIENT_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushGradient1, brushGradient2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter [SourceOver]", image, *painter1, brushGradient1, brushGradient2);
        }
    }

    // Texture-filled polygons
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_TEXTURE_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillPolygon("Texture-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2, brushTexture1, brushTexture2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillPolygon("Texture-Filled Polygons - ImagePainter [SourceCopy]", image, *painter1, brushTexture1, brushTexture2);
        }
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_TEXTURE_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Texture-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushTexture1, brushTexture2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawFillPolygon("Texture-Filled Polygons - ImagePainter [SourceOver]", image, *painter1, brushTexture1, brushTexture2);
        }
    }

    // Solid-filled ellipses
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_FILLED_ELLIPSE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillEllipse("Solid-Filled Ellipse - ImagePainter2 [SourceCopy]", image, *painter2, brushSolid1, brushSolid2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillEllipse("Solid-Filled Ellipse - ImagePainter [SourceCopy]", image, *painter1, brushSolid1, brushSolid2);
        }
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_FILLED_ELLIPSE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillEllipse("Solid-Filled Ellipse - ImagePainter2 [SourceOver]", image, *painter2, brushSolid1,  brushSolid2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillEllipse("Solid-Filled Ellipse - ImagePainter [SourceOver]", image, *painter1, brushSolid1, brushSolid2);
        }
    }

    // Gradient-filled ellipses
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_GRADIENT_FILLED_ELLIPSE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillEllipse("Gradient-Filled Ellipse - ImagePainter2 [SourceCopy]", image, *painter2, brushGradient1, brushGradient2);
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_GRADIENT_FILLED_ELLIPSE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillEllipse("Gradient-Filled Ellipse - ImagePainter2 [SourceOver]", image, *painter2, brushGradient1, brushGradient2);
    }

    // Texture-filled ellipses
    if(DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_TEXTURE_FILLED_ELLIPSE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillEllipse("Texture-Filled Ellipse - ImagePainter2 [SourceCopy]", image, *painter2, brushTexture1, brushTexture2);
    }
    if(DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_TEXTURE_FILLED_ELLIPSE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillEllipse("Texture-Filled Ellipse - ImagePainter2 [SourceOver]", image, *painter2, brushTexture1, brushTexture2);
    }

    // Create the brushes used for benchmarking
    bmBrushSolid     = Brush(Color::fromRgb8(255, 255, 255, 175));
    bmBrushGradientH = Brush(Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Horizontal);
    bmBrushGradientV = Brush(Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Vertical  );
    bmBrushTextureT  = Brush(textureWithTransBackground);
    bmBrushTextureW  = Brush(textureWithWhiteBackground);

    // Benchmark
    if(DO_BENCHMARKING) {
        std::clog << std::fixed << std::setprecision(0) << std::endl;

        std::clog << "Pt::Gfx - CompositionMode::SourceCopy" << std::endl;
        doBenchmark(CompositionMode::SourceCopy);

        std::clog << "Pt::Gfx - CompositionMode::SourceOver" << std::endl;
        doBenchmark(CompositionMode::SourceOver);
    }

    if(DO_BENCHMARKING_CAIRO) {
        std::clog << std::fixed << std::setprecision(0) << std::endl;

        std::clog << "Cairo - CompositionMode::SourceCopy" << std::endl;
        cairoBenchmark(CompositionMode::SourceCopy);

        std::clog << "Cairo - CompositionMode::SourceOver" << std::endl;
        cairoBenchmark(CompositionMode::SourceOver);
    }

    // All done
    return 0;
}
