//
// Just for testing ;)
//

// Enabling this feature will improve anti-aliased polyline and polygon rendering quality
// but reduce XWAA performance by ~10% to ~15%
#define USE_HIRES_WITH_STANDARD_AA

/*
### TODO: Use fractional-pixel XWAA??? ###
_rasterizer->strokeOnePixelQuadraticPolybezierOutline
_rasterizer->fillEllipse
_rasterizer->fillArc

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

