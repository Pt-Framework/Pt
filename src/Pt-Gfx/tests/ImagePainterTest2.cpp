// Use the buillt-in FreeType engine and libpng included with Pt:
//     ./jam.sh configure --with-rasterizer2 --with-hmi -sGUI=linux-fb -sOPTIM=-O2 --with-freetype --with-libpng
//
//
// Enable debugging information for use with Valgrind:
//     ./jam.sh configure --with-rasterizer2 --with-hmi -sGUI=linux-fb -sOPTIM=-g --with-freetype --with-libpng
//
// Generate Valgrind suppression list:
//     valgrind --leak-check=full --show-leak-kinds=all --gen-suppressions=yes --demangle=no --suppressions=../src/Pt-Gfx/tests/ImagePainterTest2.supp ./ImagePainterTest2
//
// Use Valgrind to check memory leak:
//     valgrind --leak-check=full --show-leak-kinds=definite,possible --track-origins=yes --suppressions=../src/Pt-Gfx/tests/ImagePainterTest2.supp ./ImagePainterTest2
//
//     Note: * Optimization level other than -O0 can cause false positive detection!
//           * Some SIMD operations can also cause false positive detection!
//
//
// Use performance analysis tools for Linux:
//     perf record -d -g -T -e cycles,instructions,cache-references,cache-misses,bus-cycles ./ImagePainterTest2
//     perf report
//
//     perf archive
//         Now please run: 'tar xvf perf.data.tar.bz2 -C ~/.debug'
//         wherever you need to run 'perf report' on.
//
//     while true; do ps -aF | grep "[I]magePainterTest2"; done
//
//
// svn propset svn:mime-type text/plain etc/images/*.svg
//

#include <ctime>
#include <fstream>
#include <iomanip>

#include <unistd.h>

#include <Pt/Math.h>

#include <Pt/Gfx/SGNodeLine.h>
#include <Pt/Gfx/SGNodeRectangle.h>
#include <Pt/Gfx/SGNodeEllipse.h>
#include <Pt/Gfx/SGNodeArc.h>
#include <Pt/Gfx/SGNodeProxy.h>

#include <Pt/Gfx/TransformStack.h>

#include <Pt/Gfx/Argb32Image.h>
#include <Pt/Gfx/BlockScale.h>
#include <Pt/Gfx/ImageOperation2.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ImagePainter2.h>

#include <Pt/Gfx/PngReader.h>
#include <Pt/Gfx/SvgReader.h>

#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>

#if defined(PT_GFX_USE_GNU_STYLE_COMPILER)
#include <cxxabi.h>
#endif

#include <cairo.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#ifdef WITH_BUILTIN_LIBPNG
    #include "../../libpng/png.h"
#else
    #include <png.h>
#endif

// Grmph ..., macro from X11 is interfering with us ;)
#ifdef None
#undef None
#endif

using namespace Pt::Gfx;

// Benchmark mathematical functions only
#define DO_MATH_BENCHMARKING_ONLY 0

// Benchmark 2D transform operations only
#define DO_TRANSFORM_BENCHMARKING_ONLY 0

// Comparison with Cairo (solid-filled polygons and ellipses only)
#define DO_BENCHMARKING_CAIRO                 0
#define BENCHMARK_CAIRO_CHECK_RESULTING_IMAGE 0

// General settings for Pt-Gfx
#define DO_TEST_DRAW    1
#define DO_BENCHMARKING 1

// Detailed-test enable settings for Pt-Gfx
#define TEST_SOURCECOPY                         1
#define TEST_SOURCEOVER                         1

#define TEST_DRAW_SOLID_LINE_AND_TEXT           0 // (including bezier)
#define TEST_DRAW_PATTERNED_LINE                0 // (including bezier)
#define TEST_DRAW_SOLID_THICK_LINE              0 // (including bezier)
#define TEST_DRAW_PATTERNED_THICK_LINE          0 // (including bezier)

#define TEST_DRAW_OMPF_RECTANGLES               0 // (including thick and filled)
#define TEST_DRAW_ROUND_OMPF_RECTANGLES         0 // (including thick and filled)

#define TEST_DRAW_ELLIPSES_ARCS                 0
#define TEST_DRAW_SOLID_THICK_ELLIPSES_ARCS     0
#define TEST_DRAW_PATTERNED_THICK_ELLIPSES_ARCS 0

#define TEST_DRAW_SOLID_FILLED_POLYGONS         0
#define TEST_DRAW_GRADIENT_FILLED_POLYGONS      0
#define TEST_DRAW_TEXTURE_FILLED_POLYGONS       0

#define TEST_DRAW_SOLID_FILLED_ELLIPSES_ARCS    0
#define TEST_DRAW_GRADIENT_FILLED_ELLIPSES_ARCS 0
#define TEST_DRAW_TEXTURE_FILLED_ELLIPSES_ARCS  0

#define TEST_DRAW_PATH                          0 // (including thick and filled)
#define TEST_DRAW_PATH_CLIPPING                 0 // (including path-based text)
#define TEST_DRAW_EXTRA                         0 // (including path-based n-bezier)

#define TEST_IMAGE_OPERATION                    1
#define TEST_SCENE_GRAPH                        0
#define TEST_SVG_READER                         DEFINE_CONFIG_BITS(0, 1, 2) // (multi-test)

#define TEST_COMPARE_WITH_OLD_PAINTER           0 // (for some shapes only)

// Detailed-test benchmark settings for Pt-Gfx and some for Cairo
#define BENCHMARK_RESULT_HTML               0 // (automatically disabling test drawing and enabling Cairo comparison)
#define BENCHMARK_RESULT_HTML_SIDE_BY_SIDE  1

#define BENCHMARK_CHECK_RESULTING_IMAGE     0

#define BENCHMARK_IMAGE_SIZE                Size(1280, 800)
#define BENCHMARK_LOOP_COUNT                ( 500 * (BENCHMARK_RESULT_HTML ? 10 : 1) )

#define BENCHMARK_TEXT                      0
#define BENCHMARK_ROTATED_TEXT              0

#define BENCHMARK_SOLID_LINE                0
#define BENCHMARK_PATTERNED_LINE            0
#define BENCHMARK_SOLID_THICK_LINE          0
#define BENCHMARK_PATTERNED_THICK_LINE      0

#define BENCHMARK_RECTANGLE                 0
#define BENCHMARK_ELLIPSE                   0
#define BENCHMARK_ARC                       0

#define BENCHMARK_SOLID_BEZIER              0
#define BENCHMARK_PATTERNED_BEZIER          0
#define BENCHMARK_SOLID_THICK_BEZIER        0
#define BENCHMARK_PATTERNED_THICK_BEZIER    0

#define BENCHMARK_SOLID_FILLED_RECTANGLE    0
#define BENCHMARK_GRADIENT_FILLED_RECTANGLE 0
#define BENCHMARK_TEXTURE_FILLED_RECTANGLE  0

#define BENCHMARK_SOLID_FILLED_POLYGON      0
#define BENCHMARK_GRADIENT_FILLED_POLYGON   0
#define BENCHMARK_TEXTURE_FILLED_POLYGON    0

#define BENCHMARK_SOLID_FILLED_ELLIPSE      0
#define BENCHMARK_GRADIENT_FILLED_ELLIPSE   0
#define BENCHMARK_TEXTURE_FILLED_ELLIPSE    0

#define BENCHMARK_SOLID_FILLED_ARC          0
#define BENCHMARK_GRADIENT_FILLED_ARC       0
#define BENCHMARK_TEXTURE_FILLED_ARC        0

#define BENCHMARK_PATH                      0
#define BENCHMARK_IMAGE_OPERATION           1

// Configurations and objects
#define FONT_DIR    "../src/Pt-Gfx/fonts"
#define FONT_SPEC_S "DejaVu Sans"  ,  12, Pt::Gfx::Font::BoldItalic,    0
#define FONT_SPEC_N "DejaVu Sans"  ,  24, Pt::Gfx::Font::BoldItalic,    0
#define FONT_SPEC_R "DejaVu Sans"  ,  24, Pt::Gfx::Font::BoldItalic, -150

#define FONT_SPEC_H "DejaVu Serif" ,  92, Pt::Gfx::Font::Normal    ,    0
#define FONT_SPEC_Q "QumpellkaNo12",  64, Pt::Gfx::Font::Normal    ,    0 /* OTF */
#define FONT_SPEC_C "Charakterny"  , 116, Pt::Gfx::Font::Italic    ,    0 /* OTF */

#define TEX_FILE_TRANS_BGR "../etc/images/bleech-200x200-tbgr.png"
#define TEX_FILE_WHITE_BGR "../etc/images/bleech-200x200-wbgr.png"

static Image textureWithTransBackground;
static Image textureWithWhiteBackground;

static Brush bmBrushSolid;
static Brush bmBrushGradientH;
static Brush bmBrushGradientV;
static Brush bmBrushTextureT;
static Brush bmBrushTextureW;

static const char* sfileDirXPrefix = "";

// Include the other source files
#include "ImagePainterTest2_Util.cpp"
#include "ImagePainterTest2_Draw_Outline.cpp"
#include "ImagePainterTest2_Draw_Filled.cpp"
#include "ImagePainterTest2_Draw_Thick.cpp"
#include "ImagePainterTest2_Draw_Path.cpp"
#include "ImagePainterTest2_Draw_Extra.cpp"
#include "ImagePainterTest2_SceneGraph.cpp"
#include "ImagePainterTest2_ImageOperation.cpp"
#include "ImagePainterTest2_SvgReader.cpp"

#include "ImagePainterTest2_Benchmark.cpp"
#include "ImagePainterTest2_Cairo.cpp"


//
// Helper function to select which multi-test should be run
//

static inline Pt::uint32_t DEFINE_CONFIG_BITS(Pt::uint8_t numIdx, Pt::uint8_t idx,...)
{
    if(!numIdx) return 0;
    --numIdx;

    Pt::uint32_t result = 0;

    if(idx) result |= ( (Pt::uint32_t) 1 << (idx - 1) );

    va_list valist;
    va_start(valist, idx);

    for(Pt::uint8_t i = 0; i < numIdx; ++i) {
        idx = va_arg(valist, Pt::uint32_t);
        if(idx < 1 || idx > 32) continue;
        result |= ( (Pt::uint32_t) 1 << (idx - 1) );
    }

    va_end(valist);

    return result;
}

static inline bool CONFIG_BIT_ENABLED(Pt::uint32_t configBits, Pt::uint32_t idx)
{
    if(idx < 1 || idx > 32) return false;

    return !!( configBits & ( (Pt::uint32_t) 1 << (idx - 1) ) );
}


//
// Main program
//

int main(int argc, char* args[])
{
    // Benchmark some mathematical functions only
    if(DO_MATH_BENCHMARKING_ONLY) {
        benchMathFunctions();
        return 0;
    }

    // Benchmark 2D transform operations only
    if(DO_TRANSFORM_BENCHMARKING_ONLY) {
        printf("<float>\n" ); bench2DTransOps<float >();
        printf("<double>\n"); bench2DTransOps<double>();
        return 0;
    }

    // Determine the exact locations of the support files and directories
    const char* texFileTransBgr;
    const char* texFileWhiteBgr;
    const char* ffilesDirectory;

    std::ifstream checkIfs(TEX_FILE_TRANS_BGR);

    if(checkIfs.is_open()) {
        checkIfs.close();
        texFileTransBgr = TEX_FILE_TRANS_BGR;
        texFileWhiteBgr = TEX_FILE_WHITE_BGR;
        ffilesDirectory = FONT_DIR;
    }
    else {
        checkIfs.open("../" TEX_FILE_TRANS_BGR);
        if(!checkIfs.is_open()) {
            std::clog << std::endl << "Cannot determine the exact locations of the support files and directories!" << std::endl << std::endl;
            exit(-1);
        }
        checkIfs.close();
        texFileTransBgr = "../" TEX_FILE_TRANS_BGR;
        texFileWhiteBgr = "../" TEX_FILE_WHITE_BGR;
        ffilesDirectory = "../" FONT_DIR;
        sfileDirXPrefix = "../";
    }

    // Load the textures
    std::ifstream tbgrIfs(texFileTransBgr);
    PngReader     tbgrPng(tbgrIfs, textureWithTransBackground);
    tbgrPng.get();
    tbgrIfs.close();

    std::ifstream wbgrIfs(texFileWhiteBgr);
    PngReader     wbgrPng(wbgrIfs, textureWithWhiteBackground);
    wbgrPng.get();
    wbgrIfs.close();

    // Prepare the images and painters
    Image         image( ImageFormat::argb32(), Size(1000, 600) );
    ImagePainter  painter1obj(image);
    ImagePainter2 painter2obj(image);

    painter1obj.setFontDir( Pt::System::Path(ffilesDirectory) );
    painter1obj.setFont( Pt::Gfx::Font(FONT_SPEC_N) );

    painter2obj.setFontDir( Pt::System::Path(ffilesDirectory) );
    painter2obj.setFont( Pt::Gfx::Font(FONT_SPEC_N) );

    Painter* painter1 = dynamic_cast<Painter*>(&painter1obj);
    Painter* painter2 = dynamic_cast<Painter*>(&painter2obj);

    // Create the brushes used for drawing
    const Brush brushSolid1   (Color::fromRgb8(0, 255, 0, 175));
    const Brush brushGradient1(Color::fromRgb8(0, 255, 0, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Vertical);
    const Brush brushTexture1 (textureWithTransBackground);

    const Brush brushSolid2   (Color::fromRgb8(0, 255, 255, 175));
    const Brush brushGradient2(Color::fromRgb8(0, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Horizontal);
    const Brush brushTexture2 (textureWithWhiteBackground);

    // Solid lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_LINE_AND_TEXT) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidLine("Solid Lines and Texts - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawSolidLine("Solid Lines and Texts - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_LINE_AND_TEXT) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidLine("Solid Lines and Texts - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawSolidLine("Solid Lines and Texts - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Patterned lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATTERNED_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPatternedLine("Patterned Lines - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawPatternedLine("Patterned Lines - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATTERNED_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPatternedLine("Patterned Lines - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawPatternedLine("Patterned Lines - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Solid thick lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidThickLine("Solid Thick Lines - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawSolidThickLine("Patterned Lines - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidThickLine("Solid Thick Lines - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawSolidThickLine("Patterned Lines - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Patterned thick lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATTERNED_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPatternedThickLine("Patterned Thick Lines - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawPatternedThickLine("Patterned Lines - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATTERNED_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPatternedThickLine("Patterned Thick Lines - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawPatternedThickLine("Patterned Lines - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Rectangles, thick rectangles and filled rectangles
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_OMPF_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawRect("Rectangles & Filled Rectangles - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawRect("Rectangles & -illed Rectangles - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_OMPF_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawRect("Rectangles & Filled Rectangles - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawRect("Rectangles & Filled Rectangles - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Round rectangles, thick round rectangles and filled round rectangles
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_ROUND_OMPF_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawRoundRect("Round Rectangles & Filled Round Rectangles - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_ROUND_OMPF_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawRoundRect("Round Rectangles & Filled Round Rectangles - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Ellipses and arcs
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawEllipseArc("Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawEllipseArc("Ellipse & Arcs - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawEllipseArc("Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawEllipseArc("Ellipse & Arcs - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Solid thick ellipses and arcs
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidThickEllipseArc("Solid Thick Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawSolidThickEllipseArc("Solid Thick Ellipse & Arcs - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidThickEllipseArc("Solid Thick Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawSolidThickEllipseArc("Solid Thick Ellipse & Arcs - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Patterned thick ellipses and arcs
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATTERNED_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPatternedThickEllipseArc("Patterned Thick Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawPatternedThickEllipseArc("Patterned Thick Ellipse & Arcs - ImagePainter [SourceCopy]", image, *painter1);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATTERNED_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPatternedThickEllipseArc("Patterned Thick Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawPatternedThickEllipseArc("Patterned Thick Ellipse & Arcs - ImagePainter [SourceOver]", image, *painter1);
        }
    }

    // Solid-filled polygons
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillPolygon("Solid-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2, brushSolid1, brushSolid2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Solid-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushSolid1, brushSolid2);
    }

    // Gradient-filled polygons
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_GRADIENT_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2, brushGradient1, brushGradient2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter [SourceCopy]", image, *painter1, brushGradient1, brushGradient2);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_GRADIENT_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushGradient1, brushGradient2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter [SourceOver]", image, *painter1, brushGradient1, brushGradient2);
        }
    }

    // Texture-filled polygons
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_TEXTURE_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillPolygon("Texture-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2, brushTexture1, brushTexture2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillPolygon("Texture-Filled Polygons - ImagePainter [SourceCopy]", image, *painter1, brushTexture1, brushTexture2);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_TEXTURE_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Texture-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushTexture1, brushTexture2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceOver);
            testDrawFillPolygon("Texture-Filled Polygons - ImagePainter [SourceOver]", image, *painter1, brushTexture1, brushTexture2);
        }
    }

    // Solid-filled ellipses
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillEllipse("Solid-Filled Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2, brushSolid1, brushSolid2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillEllipse("Solid-Filled Ellipse & Arcs - ImagePainter [SourceCopy]", image, *painter1, brushSolid1, brushSolid2);
        }
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillEllipse("Solid-Filled Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2, brushSolid1,  brushSolid2);
        if(TEST_COMPARE_WITH_OLD_PAINTER) {
            painter1->setCompositionMode(CompositionMode::SourceCopy);
            testDrawFillEllipse("Solid-Filled Ellipse & Arcs - ImagePainter [SourceOver]", image, *painter1, brushSolid1, brushSolid2);
        }
    }

    // Gradient-filled ellipses
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_GRADIENT_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillEllipse("Gradient-Filled Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2, brushGradient1, brushGradient2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_GRADIENT_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillEllipse("Gradient-Filled Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2, brushGradient1, brushGradient2);
    }

    // Texture-filled ellipses
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_TEXTURE_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillEllipse("Texture-Filled Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2, brushTexture1, brushTexture2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_TEXTURE_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillEllipse("Texture-Filled Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2, brushTexture1, brushTexture2);
    }

    // Path (including thick and filled)
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATH) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPath("Path - ImagePainter2 [SourceCopy]", image, *painter2, brushGradient2, brushTexture1);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATH) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPath("Path - ImagePainter2 [SourceOver]", image, *painter2, brushGradient2, brushTexture1);
    }

    // Path clipping
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATH_CLIPPING) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPathClipping("Path Clipping - ImagePainter2 [SourceCopy]", image, *painter2, brushGradient1, brushGradient2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATH_CLIPPING) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPathClipping("Path Clipping - ImagePainter2 [SourceOver]", image, *painter2, brushGradient1, brushGradient2);
    }

    // Extra features
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_EXTRA) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawExtra("Extra Features - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_EXTRA) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawExtra("Extra Features - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Image operations
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && (TEST_SOURCECOPY || TEST_SOURCEOVER) && TEST_IMAGE_OPERATION) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testImageOperation("Image Operation - ImagePainter2", image, *painter2);
    }

    // Scene-graph
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && (TEST_SOURCECOPY || TEST_SOURCEOVER) && TEST_SCENE_GRAPH) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testSceneGraph("Scene Graph - ImagePainter2", image, *painter2);
    }

    // Svg reader
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && (TEST_SOURCECOPY || TEST_SOURCEOVER) && TEST_SVG_READER) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        if(CONFIG_BIT_ENABLED(TEST_SVG_READER, 1)) testSvgReader1("SVG Reader - ImagePainter2 - Test #1", image, *painter2);
        if(CONFIG_BIT_ENABLED(TEST_SVG_READER, 2)) testSvgReader2("SVG Reader - ImagePainter2 - Test #2", image, *painter2);
    }

    // Create the brushes used for benchmarking
    bmBrushSolid     = Brush(Color::fromRgb8(255, 255, 255, 175));
    bmBrushGradientH = Brush(Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Horizontal);
    bmBrushGradientV = Brush(Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175), Brush::Vertical  );
    bmBrushTextureT  = Brush(textureWithTransBackground);
    bmBrushTextureW  = Brush(textureWithWhiteBackground);

    // Benchmark
    char hexStr[33];
    srand(time(NULL));
    sprintf(hexStr, "%08x", rand());

    char   dateStr[19];
    time_t rawtime;
    time(&rawtime);
    strftime(dateStr, sizeof(dateStr), "%Y/%m/%d - %H:%M", localtime(&rawtime));

    if(DO_BENCHMARKING && BENCHMARK_RESULT_HTML) {
        std::clog << std::endl;
        if(BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) {
            std::clog << "<pre>" << std::endl;
            std::clog << "<b>" << dateStr << "</b><br/>" << std::endl;
            std::clog << "<div id='wrapperTop" << hexStr << "'>" << std::endl;
            std::clog << "    <table id='contentTop" << hexStr << "'></table>" << std::endl;
            std::clog << "</div>" << std::endl;
            std::clog << "<div id='wrapperBot" << hexStr << "'>" << std::endl;
            std::clog << "<table id='contentBot" << hexStr << "'>" << std::endl;
            std::clog << "<tr>" << std::endl;
            std::clog << "    <td><i><b>Pt::Gfx - CompositionMode::SourceCopy</b></i></td><td>&nbsp;&nbsp;&nbsp;</td>" << std::endl;
            std::clog << "    <td><i><b>Pt::Gfx - CompositionMode::SourceOver</b></i></td><td>&nbsp;&nbsp;&nbsp;</td>" << std::endl;
            std::clog << "    <td><i><b>Comparison with Cairo</b></i></td>" << std::endl;
            std::clog << "</tr>" << std::endl;
            std::clog << "<tr>" << std::endl;
            std::clog << "    <!-- Pt::Gfx - CompositionMode::SourceCopy -->" << std::endl;
            std::clog << "    <td>" << std::endl;
        }
        else {
            std::clog << "<pre>" << std::endl;
            std::clog << "<b>" << dateStr << "</b><br/>" << std::endl;
        }
    }

    if(DO_BENCHMARKING) {
        std::clog << std::fixed << std::setprecision(0);
        if(!BENCHMARK_RESULT_HTML) {
            std::clog << std::endl;
            std::clog << "Time   : average time needed to run one pass of the benchmark unit (in microseconds)" << std::endl;
            std::clog << "Factor : slowdown factor compared to the reference benchmark unit" << std::endl << std::endl;
        }

        if(!BENCHMARK_RESULT_HTML || !BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) std::clog << "Pt::Gfx - CompositionMode::SourceCopy" << std::endl;
        doBenchmark(CompositionMode::SourceCopy);

        if(!BENCHMARK_RESULT_HTML || !BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) std::clog << "Pt::Gfx - Image Operation" << std::endl;
        doBenchmarkImageOperation();

        if(BENCHMARK_RESULT_HTML && BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) {
            std::clog << "    </td><td>&nbsp;&nbsp;&nbsp;</td>" << std::endl;
            std::clog << "    <!-- Pt::Gfx - CompositionMode::SourceOver -->" << std::endl;
            std::clog << "    <td>" << std::endl;
        }

        if(!BENCHMARK_RESULT_HTML || !BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) std::clog << "Pt::Gfx - CompositionMode::SourceOver" << std::endl;
        doBenchmark(CompositionMode::SourceOver);

        if(!BENCHMARK_RESULT_HTML || !BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) std::clog << "Pt::Gfx - Image Operation" << std::endl;
        doBenchmarkImageOperation();

        if(BENCHMARK_RESULT_HTML && BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) {
            std::clog << "    </td><td>&nbsp;&nbsp;&nbsp;</td>" << std::endl;
            std::clog << "    <!-- Comparison with Cairo -->" << std::endl;
            std::clog << "    <td>" << std::endl;
        }
    }

    if(DO_BENCHMARKING_CAIRO || (DO_BENCHMARKING && BENCHMARK_RESULT_HTML)) {
        std::clog << std::fixed << std::setprecision(0);
        if(!BENCHMARK_RESULT_HTML) std::clog << std::endl;

        std::clog << "Cairo - CompositionMode::SourceCopy" << std::endl;
        cairoBenchmark(CompositionMode::SourceCopy);
        std::clog << std::endl;

        std::clog << "Cairo - CompositionMode::SourceOver" << std::endl;
        cairoBenchmark(CompositionMode::SourceOver);
        if(!BENCHMARK_RESULT_HTML) std::clog << std::endl;
    }

    if(DO_BENCHMARKING && BENCHMARK_RESULT_HTML) {
        if(BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) {
            std::clog << "    </td>" << std::endl;
            std::clog << "</tr>" << std::endl;
            std::clog << "</table>" << std::endl;
            std::clog << "</div></pre><br/>" << std::endl;
            std::clog << "<script>$(document).ready(function() { pairHScroll('" << hexStr << "', 'wrapperTop', 'wrapperBot', 'contentTop', 'contentBot'); });</script>" << std::endl;
        }
        else {
            std::clog << "</pre><br/>" << std::endl;
        }
        std::clog << std::endl;
    }

    // All done
    return 0;
}
