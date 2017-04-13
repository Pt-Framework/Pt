#define BENCHMARK_DISPLAY_RESULTING_IMAGE             \
    if(BENCHMARK_CHECK_RESULTING_IMAGE && !i)         \
        sdlPreviewRGB888Buffer(                       \
            formatCaption(painter, cm, __FUNCTION__), \
             image.data(),                            \
             image.width(), image.height(), false     \
        )

#include "ImagePainterTest2_Benchmark_Outline.cpp"
#include "ImagePainterTest2_Benchmark_Filled.cpp"
#include "ImagePainterTest2_Benchmark_Thick.cpp"
#include "ImagePainterTest2_Benchmark_Path.cpp"

#include "ImagePainterTest2_Benchmark_ImageScaling.cpp"

static void doBenchmark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                                       (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Texts
    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXT) {
        time1 = benchDrawText<ImagePainter >(BENCHMARK_LOOP_COUNT, false, cm, AntiAliasingMode::None);
        std::clog << "    Text                             @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, false, cm, AntiAliasingMode::None);
        std::clog << "    Text NOAA                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, false, cm, AntiAliasingMode::Default);
        std::clog << "    Text FTAA                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Rotated texts
    if(BENCHMARK_RESULT_HTML || BENCHMARK_ROTATED_TEXT) {
        time1 = benchDrawText<ImagePainter >(BENCHMARK_LOOP_COUNT, true, cm, AntiAliasingMode::None);
        std::clog << "    Rotated text (FAILED)            @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, true, cm, AntiAliasingMode::None);
        std::clog << "    Rotated text NOAA                @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, true, cm, AntiAliasingMode::Default);
        std::clog << "    Rotated text FTAA                @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_LINE) {
        time1 = benchDrawSolidLine<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid     line                   @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawSolidLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid     line NOAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawSolidLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Default);
        std::clog << "    Solid     line XWAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_LINE) {
        time1 = benchDrawPatternedLine<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Patterned line                   @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPatternedLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Patterned line NOAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawPatternedLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Default);
        std::clog << "    Patterned line XWAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid thick lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_THICK_LINE) {
        time1 = benchDrawSolidThickLine<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid     thick line             @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawSolidThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Solid     thick line NOAA        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawSolidThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Default);
        std::clog << "    Solid     thick line XWAA        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned thick lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_THICK_LINE) {
        time1 = benchDrawPatternedThickLine<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Patterned thick line             @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPatternedThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Patterned thick line NOAA        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawPatternedThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Default);
        std::clog << "    Patterned thick line XWAA        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
    // Rectangles
    if(BENCHMARK_RESULT_HTML || BENCHMARK_RECTANGLE) {
        time1 = benchDrawRect<ImagePainter >(BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Rectangle                        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Rectangle                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Ellipse
    if(BENCHMARK_RESULT_HTML || BENCHMARK_ELLIPSE) {
        time1 = benchDrawEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Ellipse                          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Ellipse NOAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Default);
        std::clog << "    Ellipse XWAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Arc
    if(BENCHMARK_RESULT_HTML || BENCHMARK_ARC) {
        time1 = benchDrawArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::None);
        std::clog << "    Arc     NOAA                     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, AntiAliasingMode::Default);
        std::clog << "    Arc     XWAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_BEZIER) {
        time1 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, AntiAliasingMode::None);
        std::clog << "    Solid     q-bezier NOAA          @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, AntiAliasingMode::Default);
        std::clog << "    Solid     q-bezier XWAA          @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_BEZIER) {
        time1 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, AntiAliasingMode::None);
        std::clog << "    Patterned q-bezier NOAA          @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, AntiAliasingMode::Default);
        std::clog << "    Patterned q-bezier XWAA          @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid thick quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_THICK_BEZIER) {
        time1 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, AntiAliasingMode::None);
        std::clog << "    Solid     thick q-bezier NOAA    @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, AntiAliasingMode::Default);
        std::clog << "    Solid     thick q-bezier XWAA    @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned thick quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_THICK_BEZIER) {
        time1 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, AntiAliasingMode::None);
        std::clog << "    Patterned thick q-bezier NOAA    @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, AntiAliasingMode::Default);
        std::clog << "    Patterned thick q-bezier XWAA    @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
    // Filled rectangles
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm);
        std::clog << "    Solid-filled    rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm);
        std::clog << "    Solid-filled    rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm);
        std::clog << "    Gradient-filled rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm);
        std::clog << "    Gradient-filled rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm);
        std::clog << "    Texture-filled  rectangle        @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm);
        std::clog << "    Texture-filled  rectangle        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled polygons
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Default);
        std::clog << "    Solid-filled    polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::Default);
        std::clog << "    Gradient-filled polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter , false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  polygon          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::Default);
        std::clog << "    Texture-filled  polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled ellipse
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Default);
        std::clog << "    Solid-filled    ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::Default);
        std::clog << "    Gradient-filled ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter >(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  ellipse          @ ImagePainter  = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::Default);
        std::clog << "    Texture-filled  ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled arc
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::None);
        std::clog << "    Solid-filled    arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, AntiAliasingMode::Default);
        std::clog << "    Solid-filled    arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::None);
        std::clog << "    Gradient-filled arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, AntiAliasingMode::Default);
        std::clog << "    Gradient-filled arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::None);
        std::clog << "    Texture-filled  arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, AntiAliasingMode::Default);
        std::clog << "    Texture-filled  arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Path
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATH) {
        time1 = benchDrawPath<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, AntiAliasingMode::None);
        std::clog << "    Path NOAA (28 SHAPES, NO RASTER) @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPath<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, AntiAliasingMode::Default);
        std::clog << "    Path XWAA (28 SHAPES, NO RASTER) @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time1 = benchDrawPath<ImagePainter2, true >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, AntiAliasingMode::None);
        std::clog << "    Path NOAA (28 SHAPES, /W RASTER) @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPath<ImagePainter2, true >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, AntiAliasingMode::Default);
        std::clog << "    Path XWAA (28 SHAPES, /W RASTER) @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

}

static void doBenchmarkImageScaling()
{
    double time1, time2;

    std::clog << "                                                       (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Image scaling
    if(BENCHMARK_RESULT_HTML || BENCHMARK_IMAGE_SCALING) {
        time1 = benchImageScalingBlock(BENCHMARK_LOOP_COUNT);
        std::clog << "    Image scaling (block    plain C)                 = " << std::setw(6) << time1 << std::endl;
        time1 = benchImageScalingBilinear<GetPixel_C     >(BENCHMARK_LOOP_COUNT);
        std::clog << "    Image scaling (bilinear plain C)                 = " << std::setw(6) << time1 << std::endl;
        time2 = benchImageScalingBilinear<GetPixel_SSE2  >(BENCHMARK_LOOP_COUNT);
        std::clog << "    Image scaling (bilinear SSE 2  )                 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time2 = benchImageScalingBilinear<GetPixel_SSE4P1>(BENCHMARK_LOOP_COUNT);
        std::clog << "    Image scaling (bilinear SSE 4.1)                 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        if(!BENCHMARK_RESULT_HTML) std::clog << std::endl;
    }
}

#undef BENCHMARK_DISPLAY_RESULTING_IMAGE
