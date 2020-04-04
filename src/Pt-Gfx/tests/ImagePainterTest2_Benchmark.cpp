
#if defined(WITH_EXPERIMENTAL_GFX)
    #define BENCHMARK_DISPLAY_RESULTING_IMAGE             \
      if(BENCHMARK_CHECK_RESULTING_IMAGE && !i)         \
          sdlPreviewRGB888Buffer(                       \
              formatCaption(painter, cm, __FUNCTION__), \
               image.data(),                            \
               image.width(), image.height(), false     \
          )
#else
    #define BENCHMARK_DISPLAY_RESULTING_IMAGE
#endif

#include "ImagePainterTest2_Benchmark_Outline.cpp"
#include "ImagePainterTest2_Benchmark_Filled.cpp"
#include "ImagePainterTest2_Benchmark_Thick.cpp"
#include "ImagePainterTest2_Benchmark_Path.cpp"
//#include "ImagePainterTest2_Benchmark_ImageOperation.cpp"

static void doBenchmark(CompositionMode cm)
{
    double time1, time2;

    std::clog << "                                                       (Time) (Factor)" << std::endl;
    std::clog << "                                                       ------ --------" << std::endl;

    // Texts
    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXT) {
        time1 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, false, cm, false);
        std::clog << "    Text NOAA                        @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, false, cm, true);
        std::clog << "    Text FTAA                        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Rotated texts
    if(BENCHMARK_RESULT_HTML || BENCHMARK_ROTATED_TEXT) {
        time1 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, true, cm, false);
        std::clog << "    Rotated text NOAA                @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawText<ImagePainter2>(BENCHMARK_LOOP_COUNT, true, cm, true);
        std::clog << "    Rotated text FTAA                @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_LINE) {
        time1 = benchDrawSolidLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Solid     line NOAA              @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawSolidLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, true);
        std::clog << "    Solid     line XWAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_LINE) {
        time1 = benchDrawPatternedLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Patterned line NOAA              @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPatternedLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, true);
        std::clog << "    Patterned line XWAA              @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid thick lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_THICK_LINE) {
        time1 = benchDrawSolidThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Solid     thick line NOAA        @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawSolidThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, true);
        std::clog << "    Solid     thick line XWAA        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned thick lines
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_THICK_LINE) {
        time1 = benchDrawPatternedThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Patterned thick line NOAA        @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPatternedThickLine<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, true);
        std::clog << "    Patterned thick line XWAA        @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
    // Rectangles
    if(BENCHMARK_RESULT_HTML || BENCHMARK_RECTANGLE) {
        time1 = benchDrawRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm);
        std::clog << "    Rectangle                        @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        std::clog << std::endl;
    }

    // Ellipse
    if(BENCHMARK_RESULT_HTML || BENCHMARK_ELLIPSE) {
        time1 = benchDrawEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Ellipse NOAA                     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, true);
        std::clog << "    Ellipse XWAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Arc
    if(BENCHMARK_RESULT_HTML || BENCHMARK_ARC) {
        time1 = benchDrawArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, false);
        std::clog << "    Arc     NOAA                     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, cm, true);
        std::clog << "    Arc     XWAA                     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_BEZIER) {
        time1 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, false);
        std::clog << "    Solid     q-bezier NOAA          @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, true);
        std::clog << "    Solid     q-bezier XWAA          @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_BEZIER) {
        time1 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, false);
        std::clog << "    Patterned q-bezier NOAA          @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, true);
        std::clog << "    Patterned q-bezier XWAA          @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Solid thick quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_THICK_BEZIER) {
        time1 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, false);
        std::clog << "    Solid     thick q-bezier NOAA    @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Solid, cm, true);
        std::clog << "    Solid     thick q-bezier XWAA    @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Patterned thick quadratic bezier curve
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATTERNED_THICK_BEZIER) {
        time1 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, false);
        std::clog << "    Patterned thick q-bezier NOAA    @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawThickQuadBezier<ImagePainter2>(BENCHMARK_LOOP_COUNT, Pen::Dash, cm, true);
        std::clog << "    Patterned thick q-bezier XWAA    @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }
    
    // Filled rectangles
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm);
        std::clog << "    Solid-filled    rectangle        @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm);
        std::clog << "    Gradient-filled rectangle        @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_RECTANGLE) {
        time1 = benchDrawFillRect<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm);
        std::clog << "    Texture-filled  rectangle        @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        std::clog << std::endl;
    }

    // Filled polygons
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, false);
        std::clog << "    Solid-filled    polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, true);
        std::clog << "    Solid-filled    polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, false);
        std::clog << "    Gradient-filled polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, true);
        std::clog << "    Gradient-filled polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_POLYGON) {
        time1 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, false);
        std::clog << "    Texture-filled  polygon NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillPolygon<ImagePainter2, false>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, true);
        std::clog << "    Texture-filled  polygon XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled ellipse
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, false);
        std::clog << "    Solid-filled    ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, true);
        std::clog << "    Solid-filled    ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, false);
        std::clog << "    Gradient-filled ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, true);
        std::clog << "    Gradient-filled ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_ELLIPSE) {
        time1 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, false);
        std::clog << "    Texture-filled  ellipse NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillEllipse<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, true);
        std::clog << "    Texture-filled  ellipse XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Filled arc
    if(BENCHMARK_RESULT_HTML || BENCHMARK_SOLID_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, false);
        std::clog << "    Solid-filled    arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushSolid, bmBrushSolid, cm, true);
        std::clog << "    Solid-filled    arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_GRADIENT_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, false);
        std::clog << "    Gradient-filled arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushGradientV, cm, true);
        std::clog << "    Gradient-filled arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    if(BENCHMARK_RESULT_HTML || BENCHMARK_TEXTURE_FILLED_ARC) {
        time1 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, false);
        std::clog << "    Texture-filled  arc     NOAA     @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawFillArc<ImagePainter2>(BENCHMARK_LOOP_COUNT, bmBrushTextureT, bmBrushTextureW, cm, true);
        std::clog << "    Texture-filled  arc     XWAA     @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

    // Path
    if(BENCHMARK_RESULT_HTML || BENCHMARK_PATH) {
        time1 = benchDrawPath<ImagePainter2, false, false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, false);
        std::clog << "    Path NOAA (NO RASTER, NO CLIP)   @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPath<ImagePainter2, false, true >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, true);
        std::clog << "    Path XWAA (NO RASTER, /W CLIP)   @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        time1 = benchDrawPath<ImagePainter2, true , false>(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, false);
        std::clog << "    Path NOAA (/W RASTER, NO CLIP)   @ ImagePainter2 = " << std::setw(6) << time1 << std::endl;
        time2 = benchDrawPath<ImagePainter2, true , true >(BENCHMARK_LOOP_COUNT, bmBrushGradientH, bmBrushTextureT, cm, true);
        std::clog << "    Path XWAA (/W RASTER, /W CLIP)   @ ImagePainter2 = " << std::setw(6) << time2
                  << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
        std::clog << std::endl;
    }

}

static void doBenchmarkImageOperation()
{
    //double time1, time2;

    //std::clog << "                                                       (Time) (Factor)" << std::endl;
    //std::clog << "                                                       ------ --------" << std::endl;

    //// Image scaling
    //time1 = benchImageScalingBlock<-1>(BENCHMARK_LOOP_COUNT);
    //std::clog << "    Image scaling    (block    - generic )           = " << std::setw(6) << time1 << std::endl;
    //time2 = benchImageScalingBlock<-2>(BENCHMARK_LOOP_COUNT);
    //std::clog << "    Image scaling    (block    - argb32  )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //time2 = benchImageScalingBlock<0>(BENCHMARK_LOOP_COUNT);
    //std::clog << "    Image scaling  2 (block              )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //time2 = benchImageScalingBilinear(BENCHMARK_LOOP_COUNT);
    //std::clog << "    Image scaling  2 (bilinear           )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //std::clog << std::endl;

    //// Image rotation
    //time1 = benchImageRotationBlock(BENCHMARK_LOOP_COUNT, ImageOperation2::RotateCrop);
    //std::clog << "    Image rotation 2 (block    - crop    )           = " << std::setw(6) << time1 << std::endl;
    //time2 = benchImageRotationBlock(BENCHMARK_LOOP_COUNT, ImageOperation2::RotateNoCrop);
    //std::clog << "    Image rotation 2 (block    - no-crop )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //time2 = benchImageRotationBlock(BENCHMARK_LOOP_COUNT, ImageOperation2::RotateFit);
    //std::clog << "    Image rotation 2 (block    - fit     )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //time2 = benchImageRotationBilinear(BENCHMARK_LOOP_COUNT, ImageOperation2::RotateCrop);
    //std::clog << "    Image rotation 2 (bilinear - crop    )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //time2 = benchImageRotationBilinear(BENCHMARK_LOOP_COUNT, ImageOperation2::RotateNoCrop);
    //std::clog << "    Image rotation 2 (bilinear - no-crop )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //time2 = benchImageRotationBilinear(BENCHMARK_LOOP_COUNT, ImageOperation2::RotateFit);
    //std::clog << "    Image rotation 2 (bilinear - fit     )           = " << std::setw(6) << time2
    //          << " (" << std::setw(6) << std::setprecision(3) << (time2 / time1) << ")" << std::setprecision(0) << std::endl;
    //if(!BENCHMARK_RESULT_HTML) std::clog << std::endl;
}

#undef BENCHMARK_DISPLAY_RESULTING_IMAGE
