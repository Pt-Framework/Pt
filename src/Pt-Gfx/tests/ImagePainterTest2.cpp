// ./jam.sh configure --with-hmi -sGUI=linux-fb --with-rasterizer2

#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Clock.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <unistd.h>

using namespace Pt::Gfx;

#include "ImagePainterTest2_Util.cpp"
#include "ImagePainterTest2_Draw.cpp"
#include "ImagePainterTest2_Benchmark.cpp"

int main(int argc, char* args[])
{
    Image         image( ImageFormat::argb32(), Size(800, 600) );
    ImagePainter2 painter(image);

    painter.setFontDir( Pt::System::Path("../src/Pt-Gfx/fonts") );
    painter.setFont( Pt::Gfx::Font("DejaVu Serif", 24, Pt::Gfx::Font::BoldItalic) );

    if(1) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testLines("Test Lines - CompositionMode::SourceCopy", image, painter);
    }

    if(0) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testLines("Test Lines - CompositionMode::SourceOver", image, painter);
    }

    if(1) {
        painter.setCompositionMode(CompositionMode::SourceCopy);
        testFillPolygon("Test Fill Polygons - CompositionMode::SourceCopy", image, painter);
    }

    if(0) {
        painter.setCompositionMode(CompositionMode::SourceOver);
        testFillPolygon("Test Fill Polygons - CompositionMode::SourceOver", image, painter);
    }

    if(1) {
        benchDrawLine            <ImagePainter >("Drawing line using ImagePainter                  = ");
        benchDrawLine            <ImagePainter2>("Drawing line using ImagePainter2                 = ");
        benchDrawSolidFillPolygon<ImagePainter >("Drawing solid-filled polygon using ImagePainter  = ");
        benchDrawSolidFillPolygon<ImagePainter2>("Drawing solid-filled polygon using ImagePainter2 = ");
    }

/*
 * Drawing line using ImagePainter                  = 189
 * Drawing line using ImagePainter2                 = 2420
 * Drawing solid-filled polygon using ImagePainter  = 558
 * Drawing solid-filled polygon using ImagePainter2 = 2327
 */
    return 0;
}

