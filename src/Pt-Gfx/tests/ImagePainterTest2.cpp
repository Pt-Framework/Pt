// Use the buillt-in FreeType engine and libpng included with Pt:
//     ./jam.sh configure --with-experimental-gfx --with-hmi -sGUI=linux-fb -sOPTIM=-O2 --with-freetype --with-libpng
//
//
// Enable debugging information for use with Valgrind:
//     ./jam.sh configure --with-experimental-gfx --with-hmi -sGUI=linux-fb -sOPTIM=-g --with-freetype --with-libpng
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


// ./jam.sh configure --optimize --with-experimental-gfx --with-hmi -sGUI=xorg
//#define WITH_EXPERIMENTAL_GFX

#include <Pt/Gfx/Argb32Image.h>
#include <Pt/Gfx/BlockScale.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/PngReader.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>
#include <Pt/Math.h>
#include <Pt/Types.h>

#include <ctime>
#include <fstream>
#include <iomanip>

#if defined(PT_GFX_USE_GNU_STYLE_COMPILER)
#include <cxxabi.h>
#endif

#include <png.h>

#if defined(WITH_EXPERIMENTAL_GFX)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_syswm.h>
    #include <unistd.h>

    // Grmph ..., macro from X11 is interfering with us ;)
    #ifdef None
    #undef None
    #endif
#endif

using namespace Pt::Gfx;

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
#define TEST_DRAW_ROUND_OMPF_RECTANGLES         1 // (including thick and filled)

#define TEST_DRAW_ELLIPSES_ARCS                 0
#define TEST_DRAW_SOLID_THICK_ELLIPSES_ARCS     0
#define TEST_DRAW_PATTERNED_THICK_ELLIPSES_ARCS 1

#define TEST_DRAW_SOLID_FILLED_POLYGONS         0
#define TEST_DRAW_GRADIENT_FILLED_POLYGONS      0
#define TEST_DRAW_TEXTURE_FILLED_POLYGONS       0

#define TEST_DRAW_SOLID_FILLED_ELLIPSES_ARCS    0
#define TEST_DRAW_GRADIENT_FILLED_ELLIPSES_ARCS 0
#define TEST_DRAW_TEXTURE_FILLED_ELLIPSES_ARCS  0

//#define TEST_DRAW_EXTRA                       0

#define TEST_DRAW_PATH                          0 // (including thick and filled and rectangle clipping area)
//#define TEST_DRAW_PATH_CLIPPING               0 // (including path-based text)

//#define TEST_IMAGE_OPERATION                  0

// Detailed-test benchmark settings for Pt-Gfx
#define BENCHMARK_RESULT_HTML               0 // (automatically disabling test drawing)
#define BENCHMARK_RESULT_HTML_SIDE_BY_SIDE  1

#define BENCHMARK_CHECK_RESULTING_IMAGE     0

#define BENCHMARK_IMAGE_SIZE                Size(1280, 800)
#define BENCHMARK_LOOP_COUNT                ( 500 * (BENCHMARK_RESULT_HTML ? 10 : 1) )

#define BENCHMARK_TEXT                      0
#define BENCHMARK_ROTATED_TEXT              0 // XXX

#define BENCHMARK_SOLID_LINE                0
#define BENCHMARK_PATTERNED_LINE            1
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

#define BENCHMARK_PATH                      0 // (including thick and filled and rectangle clipping area)
#define BENCHMARK_IMAGE_OPERATION           0

// Configurations and objects
Pt::System::Path FONT_DIR;
#define FONT_SPEC_S "DejaVu Sans"  ,  12, Pt::Gfx::Font::BoldItalic,    0
#define FONT_SPEC_N "DejaVu Sans"  ,  24, Pt::Gfx::Font::BoldItalic,    0

#define FONT_SPEC_H "DejaVu Serif" ,  92, Pt::Gfx::Font::Normal    ,    0
#define FONT_SPEC_Q "QumpellkaNo12",  64, Pt::Gfx::Font::Normal    ,    0 /* OTF */
#define FONT_SPEC_C "Charakterny"  , 116, Pt::Gfx::Font::Italic    ,    0 /* OTF */

//#define TEX_FILE_TRANS_BGR "../etc/images/bleech-200x200-tbgr.png"
//#define TEX_FILE_WHITE_BGR "../etc/images/bleech-200x200-wbgr.png"

static Image textureWithTransBackground;
static Image textureWithWhiteBackground;

static Brush bmBrushSolid;
static Brush bmBrushGradientH;
static Brush bmBrushGradientV;
static Brush bmBrushTextureT;
static Brush bmBrushTextureW;

Pt::System::Path buildDir;

// Include the other source files
#include "ImagePainterTest2_Util.cpp"
#include "ImagePainterTest2_Draw_Outline.cpp"
#include "ImagePainterTest2_Draw_Filled.cpp"
#include "ImagePainterTest2_Draw_Thick.cpp"
#include "ImagePainterTest2_Draw_Path.cpp"
#include "ImagePainterTest2_Draw_Extra.cpp"
//#include "ImagePainterTest2_ImageOperation.cpp"

#include "ImagePainterTest2_Benchmark.cpp"


void benchmarkWideLines()
{
    Pt::Gfx::Image image( Pt::Gfx::ImageFormat::argb32(), Pt::Gfx::Size(400, 400) );

    Pt::Gfx::ImagePainter2 imagePainter(image);
    imagePainter.setAntiAliasing(true);

    //imagePainter.setPen( Pen( Pt::Gfx::Color::fromRgb8(255, 0, 0), 1 )  );
    imagePainter.setPen( Pen( Pt::Gfx::Color::fromRgb8(255, 0, 0), 8 )  );
    //imagePainter.setPen( Pen( Pt::Gfx::Color::fromRgb8(255, 0, 0), 8, Pen::Dash )  );

    std::vector<PointF> polyline;
    polyline.push_back( Pt::Gfx::PointF(50, 50) );
    polyline.push_back( Pt::Gfx::PointF(320, 30) );
    polyline.push_back( Pt::Gfx::PointF(300, 290) );
    polyline.push_back( Pt::Gfx::PointF(60, 70) );

    Pt::System::Clock clock;
    clock.start();
    for(int n = 0; n < 2000; ++n)
    {
        imagePainter.drawPolyline( &polyline[0], polyline.size() );

        //imagePainter.drawLine( Pt::Gfx::PointF(50, 50), Pt::Gfx::PointF(300, 290) );
    }

    long long time = clock.stop().toUSecs();
    std::clog << "AA WIDE LINES: " << time << std::endl;
}


void benchmarkNarrowRoundedRect()
{
    Pt::Gfx::Image image( Pt::Gfx::ImageFormat::argb32(), Pt::Gfx::Size(400, 400) );

    Pt::Gfx::ImagePainter2 imagePainter(image);
    imagePainter.setAntiAliasing(true);
    imagePainter.setPen( Pt::Gfx::Pen( Pt::Gfx::Color::fromRgb8(255, 0, 0) ) );
    imagePainter.setBrush( Pt::Gfx::Color::fromRgb8(0, 0, 255)  );

    Pt::System::Clock clock;
    clock.start();
    for(int n = 0; n < 30000; ++n)
    {
        imagePainter.drawRoundedRect( Pt::Gfx::RectF(Pt::Gfx::PointF(150, 150),
                                                     Pt::Gfx::SizeF(50, 50)), 10);

    }

    long long time = clock.stop().toUSecs();
    std::clog << "ROUNDED RECT TIME: " << time << std::endl;
}

//
// Main program
//

/*
#include <x86intrin.h>

inline Pt::int32_t test_lround_libc_lround(double v)
{
    return ::lround(v);
}

inline Pt::int32_t test_lround_libc_lrint(double v)
{
    return ::lrint(v);
}

inline Pt::int32_t test_lround_sse(double v)
{
    return _mm_cvtsd_si32(_mm_load_sd(&v));
}

inline Pt::int32_t test_lround_asm(double v)
{
    Pt::int32_t tmp;
    __asm__ __volatile__ (
        "fldl   %1\n\t"
        "fistpl %0    "
        : "=m"(tmp)
        :  "m"(v)
        : "memory"
    );
    return tmp;
}

inline Pt::int32_t test_lround_fallback(double v)
{
    Pt::int32_t tmp = static_cast<Pt::int32_t>(v);
    tmp += (v - tmp >= 0.5) - (v - tmp <= -0.5);
    return tmp;
}
*/

int main(int argc, char* args[])
{
    /*
    volatile double x = 578.5;
    std::cout << "libc lround = " << test_lround_libc_lround(x) << std::endl;
    std::cout << "libc lrint  = " << test_lround_libc_lrint (x) << std::endl;
    std::cout << "sse         = " << test_lround_sse        (x) << std::endl;
    std::cout << "asm         = " << test_lround_asm        (x) << std::endl;
    std::cout << "fallback    = " << test_lround_fallback   (x) << std::endl;

    // libc lround = 579
    // libc lrint  = 578
    // sse         = 578
    // asm         = 578
    // fallback    = 579

    return 0;
    */

    buildDir = args[0];
    buildDir = buildDir.dirName();
    std::cerr << buildDir.toLocal() << std::endl;
    if(buildDir.baseName() != "build")
      buildDir = buildDir.dirName();

    buildDir = "../";

    // Determine fonts dir
    FONT_DIR = buildDir;
    FONT_DIR /= Pt::System::Path::updir();
    FONT_DIR /= "src";
    FONT_DIR /= "Pt-Gfx";
    FONT_DIR /= "fonts";

    // Load the textures
    Pt::System::Path etcPath = buildDir;
    etcPath /= Pt::System::Path::updir();
    etcPath /= "etc";

    Pt::System::Path TEX_FILE_TRANS_BGR = etcPath;
    TEX_FILE_TRANS_BGR /= "images";
    TEX_FILE_TRANS_BGR /= "bleech-200x200-tbgr.png";

    Pt::System::Path TEX_FILE_WHITE_BGR = etcPath;
    TEX_FILE_WHITE_BGR /= "images";
    TEX_FILE_WHITE_BGR /= "bleech-200x200-wbgr.png";

    std::string localPath = TEX_FILE_TRANS_BGR.toLocal();
    std::ifstream tbgrIfs( localPath.c_str(), std::ios::in|std::ios::binary );
    PngReader     tbgrPng(tbgrIfs, textureWithTransBackground);
    tbgrPng.get();
    tbgrIfs.close();

    localPath = TEX_FILE_WHITE_BGR.toLocal();
    std::ifstream wbgrIfs( localPath.c_str(), std::ios::in|std::ios::binary );
    PngReader     wbgrPng(wbgrIfs, textureWithWhiteBackground);
    wbgrPng.get();
    wbgrIfs.close();

    // Prepare the images and painters

    Image         image( ImageFormat::argb32(), Size(1000, 600) );
    ImagePainter2 painter2obj(image);

    painter2obj.setFontDir( FONT_DIR );
    painter2obj.setFont( Pt::Gfx::Font(FONT_SPEC_N) );

    Painter* painter2 = dynamic_cast<Painter*>(&painter2obj);

    // Create the brushes used for drawing
    const Brush brushSolid1   (Color::fromRgb8(0, 255, 0, 175));
    const Brush brushGradient1 = Brush::verticalGradient(Color::fromRgb8(0, 255, 0, 175),
                                                         Color::fromRgb8(0, 0, 0, 175));
    const Brush brushTexture1 (textureWithTransBackground);

    const Brush brushSolid2   (Color::fromRgb8(0, 255, 255, 175));
    const Brush brushGradient2 = Brush::horizontalGradient(Color::fromRgb8(0, 255, 255, 175),
                                                           Color::fromRgb8(0, 0, 0, 175));
    const Brush brushTexture2 (textureWithWhiteBackground);

    // benchmarkNarrowRoundedRect();
    // benchmarkWideLines();

    // Solid lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_LINE_AND_TEXT) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidLine("Solid Lines and Texts - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_LINE_AND_TEXT) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidLine("Solid Lines and Texts - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Patterned lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATTERNED_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPatternedLine("Patterned Lines - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATTERNED_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPatternedLine("Patterned Lines - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Solid thick lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidThickLine("Solid Thick Lines - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidThickLine("Solid Thick Lines - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Patterned thick lines
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATTERNED_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPatternedThickLine("Patterned Thick Lines - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATTERNED_THICK_LINE) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPatternedThickLine("Patterned Thick Lines - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Rectangles, thick rectangles and filled rectangles
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_OMPF_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawRect("Rectangles & Filled Rectangles - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_OMPF_RECTANGLES) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawRect("Rectangles & Filled Rectangles - ImagePainter2 [SourceOver]", image, *painter2);
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
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawEllipseArc("Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Solid thick ellipses and arcs
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawSolidThickEllipseArc("Solid Thick Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawSolidThickEllipseArc("Solid Thick Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2);
    }

    // Patterned thick ellipses and arcs
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATTERNED_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawPatternedThickEllipseArc("Patterned Thick Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATTERNED_THICK_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawPatternedThickEllipseArc("Patterned Thick Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2);
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
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_GRADIENT_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Gradient-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushGradient1, brushGradient2);
    }

    // Texture-filled polygons
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_TEXTURE_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillPolygon("Texture-Filled Polygons - ImagePainter2 [SourceCopy]", image, *painter2, brushTexture1, brushTexture2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_TEXTURE_FILLED_POLYGONS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillPolygon("Texture-Filled Polygons - ImagePainter2 [SourceOver]", image, *painter2, brushTexture1, brushTexture2);
    }

    // Solid-filled ellipses
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_SOLID_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawFillEllipse("Solid-Filled Ellipse & Arcs - ImagePainter2 [SourceCopy]", image, *painter2, brushSolid1, brushSolid2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_SOLID_FILLED_ELLIPSES_ARCS) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawFillEllipse("Solid-Filled Ellipse & Arcs - ImagePainter2 [SourceOver]", image, *painter2, brushSolid1,  brushSolid2);
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
    //if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_PATH_CLIPPING) {
    //    painter2->setCompositionMode(CompositionMode::SourceCopy);
    //    testDrawPathClipping("Path Clipping - ImagePainter2 [SourceCopy]", image, *painter2, brushGradient1, brushGradient2);
    //}

    //if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_PATH_CLIPPING) {
    //    painter2->setCompositionMode(CompositionMode::SourceOver);
    //    testDrawPathClipping("Path Clipping - ImagePainter2 [SourceOver]", image, *painter2, brushGradient1, brushGradient2);
    //}

    /*
    // Extra features
    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCECOPY && TEST_DRAW_EXTRA) {
        painter2->setCompositionMode(CompositionMode::SourceCopy);
        testDrawExtra("Extra Features - ImagePainter2 [SourceCopy]", image, *painter2);
    }

    if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && TEST_SOURCEOVER && TEST_DRAW_EXTRA) {
        painter2->setCompositionMode(CompositionMode::SourceOver);
        testDrawExtra("Extra Features - ImagePainter2 [SourceOver]", image, *painter2);
    }
    */

    // Image operations
    //if((!DO_BENCHMARKING || !BENCHMARK_RESULT_HTML) && DO_TEST_DRAW && (TEST_SOURCECOPY || TEST_SOURCEOVER) && TEST_IMAGE_OPERATION) {
    //    painter2->setCompositionMode(CompositionMode::SourceCopy);
    //    testImageOperation("Image Operation - ImagePainter2", image, *painter2);
    //}

    // Create the brushes used for benchmarking
    bmBrushSolid     = Brush(Color::fromRgb8(255, 255, 255, 175));
    bmBrushGradientH = Brush::horizontalGradient( Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175) );
    bmBrushGradientV = Brush::verticalGradient( Color::fromRgb8(255, 255, 255, 175), Color::fromRgb8(0, 0, 0, 175) );
    bmBrushTextureT  = Brush(textureWithTransBackground);
    bmBrushTextureW  = Brush(textureWithWhiteBackground);

    // Benchmark
    char hexStr[33];
    srand( unsigned(time(NULL)) );
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

        if(BENCHMARK_RESULT_HTML || BENCHMARK_IMAGE_OPERATION) {
            if(!BENCHMARK_RESULT_HTML || !BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) std::clog << "Pt::Gfx - Image Operation" << std::endl;
            doBenchmarkImageOperation();
        }

        if(BENCHMARK_RESULT_HTML && BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) {
            std::clog << "    </td><td>&nbsp;&nbsp;&nbsp;</td>" << std::endl;
            std::clog << "    <!-- Pt::Gfx - CompositionMode::SourceOver -->" << std::endl;
            std::clog << "    <td>" << std::endl;
        }

        if(!BENCHMARK_RESULT_HTML || !BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) std::clog << "Pt::Gfx - CompositionMode::SourceOver" << std::endl;
        doBenchmark(CompositionMode::SourceOver);

        if(BENCHMARK_RESULT_HTML || BENCHMARK_IMAGE_OPERATION) {
            if(!BENCHMARK_RESULT_HTML || !BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) std::clog << "Pt::Gfx - Image Operation" << std::endl;
            doBenchmarkImageOperation();
        }

        if(BENCHMARK_RESULT_HTML && BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) {
            std::clog << "    </td><td>&nbsp;&nbsp;&nbsp;</td>" << std::endl;
        }
    }

    if(DO_BENCHMARKING && BENCHMARK_RESULT_HTML) {
        if(BENCHMARK_RESULT_HTML_SIDE_BY_SIDE) {
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
