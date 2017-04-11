//
// Just for testing ;)
//

// Enabling this feature will improve anti-aliased polyline and polygon rendering quality
// but reduce XWAA performance by ~10% to ~15%
#define USE_HIRES_WITH_STANDARD_AA


/*
Try to use fractional-pixel XWAA for these:
    _rasterizer->strokeOnePixelQuadraticPolybezierOutline ### NOT BENEFICIAL    ###
    _rasterizer->strokeOnePixelEllipseArc                 ### CANNOT BE DONE    ###
    _rasterizer->fillArc                                  ### SEEMS UNNECESSARY ###
    _rasterizer->fillEllipse                              ### SEEMS UNNECESSARY ###



#if defined(USE_HIRES_WITH_STANDARD_AA)
        const bool useFloatFuncs = _rasterizer->antiAliasingMode() == AntiAliasingMode::Standard;
#else
        const bool useFloatFuncs = false;
#endif
        // Use higher precision rasterization when using AntiAliasingMode::Standard
        if(useFloatFuncs) {
        }
        // Use lower precision rasterization when using other modes
        else {
        }



    // Determine whether to use the higher resolution function
#if defined(USE_HIRES_WITH_STANDARD_AA)
    const bool useFloatFuncs = _rasterizer->antiAliasingMode() == AntiAliasingMode::Standard;
#else
    const bool useFloatFuncs = false;
#endif

    // Use higher precision rasterization when using AntiAliasingMode::Standard
    if(useFloatFuncs) {
        // Remove duplicates
        std::vector<PointF> points;
        deduplicatePointsF(points, pointsF.data(), pointsF.size());
    }

    // Use lower precision rasterization when using other modes
    else {
    }

*/

