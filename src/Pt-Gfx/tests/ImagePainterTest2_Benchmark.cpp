#define BENCHMARK_DISPLAY_RESULTING_IMAGE             \
    if(BENCHMARK_CHECK_RESULTING_IMAGE && !i)         \
        sdlPreviewRGB888Buffer(                       \
            formatCaption(painter, cm, __FUNCTION__), \
             image.data(),                            \
             image.width(), image.height(), false     \
        )

#include "ImagePainterTest2_Benchmark_Outline.cpp"
#include "ImagePainterTest2_Benchmark_Filled.cpp"

static void doBenchmark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                                       (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Texts
    if(BENCHMARK_TEXT) {
        time1 = benchDrawText<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Text                             @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Text NOAA                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Standard);
        std::clog << "    Text FTAA                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid lines
    if(BENCHMARK_SOLID_LINE) {
        time1 = benchDrawSolidLine<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid     line                   @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawSolidLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid     line NOAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawSolidLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid     line XWAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned lines
    if(BENCHMARK_PATTERNED_LINE) {
        time1 = benchDrawPatternedLine<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Patterned line                   @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPatternedLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Patterned line NOAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawPatternedLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Standard);
        std::clog << "    Patterned line XWAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }


    // Rectangles
    if(BENCHMARK_RECTANGLE) {
        time1 = benchDrawRect<ImagePainter >(BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Rectangle                        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Rectangle                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Ellipse
    if(BENCHMARK_ELLIPSE) {
        time1 = benchDrawEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Ellipse                          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Ellipse NOAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Standard);
        std::clog << "    Ellipse XWAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Arc
    if(BENCHMARK_ARC) {
        time1 = benchDrawArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Arc     NOAA                     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Standard);
        std::clog << "    Arc     XWAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled rectangles
    if(BENCHMARK_SOLID_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm);
        std::clog << "    Solid-filled    Rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm);
        std::clog << "    Solid-filled    Rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm);
        std::clog << "    Gradient-filled Rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm);
        std::clog << "    Gradient-filled Rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm);
        std::clog << "    Texture-filled  Rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm);
        std::clog << "    Texture-filled  Rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled polygons
    if(BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid-filled    polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::LowMemory);
        std::clog << "    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::Standard);
        std::clog << "    Gradient-filled polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::LowMemory);
        std::clog << "    Gradient-filled polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::Standard);
        std::clog << "    Texture-filled  polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::LowMemory);
        std::clog << "    Texture-filled  polygon FSAA 2x2 @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled ellipse
    if(BENCHMARK_SOLID_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid-filled    ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::Standard);
        std::clog << "    Gradient-filled ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::Standard);
        std::clog << "    Texture-filled  ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled arc
    if(BENCHMARK_SOLID_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Standard);
        std::clog << "    Solid-filled    arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_GRADIENT_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::Standard);
        std::clog << "    Gradient-filled arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_TEXTURE_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::Standard);
        std::clog << "    Texture-filled  arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
}

#undef BENCHMARK_DISPLAY_RESULTING_IMAGE
