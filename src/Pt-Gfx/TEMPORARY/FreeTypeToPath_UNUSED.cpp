    // Walk through the contours
    Pt::int32_t cSIdx = 0;
    Pt::int32_t cEIdx = 0;
    for(size_t i = 0; i < contours.size(); ++i) {
        // Get the end index of the contour
        cEIdx = contours[i];
        // Calculate the number of points within the contour
        const size_t numPoints = cEIdx - cSIdx + 1;
        // Generate a new path that starts from the first point of the contour
        if(!dst.empty()) dst.push_back(Painter::PolygonSeparatorPointF);
        dst.push_back(pointsF[cSIdx]);
        // Walk through the points in this contour
        for(size_t j = 0; j < numPoints; ++j) {
            // Get the indexes and coordinates
            const Pt::int32_t idx0 = (j + 0) % numPoints + cSIdx;
            const Pt::int32_t idx1 = (j + 1) % numPoints + cSIdx;
            const Pt::int32_t idx2 = (j + 2) % numPoints + cSIdx;
            const double      x0   = pointsF[idx0].x();
            const double      y0   = pointsF[idx0].y();
            const double      x1   = pointsF[idx1].x();
            const double      y1   = pointsF[idx1].y();
            const double      x2   = pointsF[idx2].x();
            const double      y2   = pointsF[idx2].y();
            // Get the tags
            // Bit #0 -> 0 = control point          ; 1 = non-control point
            // Bit #1 -> 0 = quadratic bezier (TTF) ; 1 = cubic bezier (OTF)
            // Bit #2 -> 0 = bit #5-#7 is unused    ; 1 = bit #5-#7 contain the OTF drop-out mode (we ignored this)
                  bool isCtl0 = !(tags[idx0] & 0x01);
            const bool isCtl1 = !(tags[idx1] & 0x01);
            const bool isCtl2 = !(tags[idx2] & 0x01);
            const bool isCub0 =  (tags[idx0] & 0x02);
            const bool isCub1 =  (tags[idx1] & 0x02);
            // Does point #0 and point #1 are both control points?
            if(isCtl0 && isCtl1) {
                // If this is the first iteration, generate a new path that starts from
                // the halfway point between the two control points
                if(!j) {
                    // A contour cannot start with a cubic bezier control point
                    if(isCub0) {
                        j = numPoints;
                        continue;
                    }
                    // Generate a new path
                    const double xm = (x0 + x1) * 0.5;
                    const double ym = (y0 + y1) * 0.5;
                    if(!dst.empty()) dst.push_back(Painter::PolygonSeparatorPointF);
                    dst.push_back(PointF(xm, ym));
                }
                // Mark point #0 as a non-control point
                isCtl0 = false;
            }
            // If both point #0 and point #1 are not control points, generate a line
            if(!isCtl0 && !isCtl1) {
                dst.push_back(PointF(x1, y1));
            }
            // Is it a cubic bezier?
            // ### TODO: Something is not fully correct here! ###
            else if(isCub0 && isCub1) {
                // If Point #2 is not a control point, directly generate a cubic bezier curve
                if(!isCtl2) {
                    generateCubicBezierPoints(dst, dst.back().x(), dst.back().y(), x0, y0, x1, y1, x2, y2, smoothness);
                }
                // If Point #2 is also a control point, generate a cubic bezier curve
                // using the halfway point between the two control points
                else {
                    const double xm = (x1 + x2) * 0.5;
                    const double ym = (y1 + y2) * 0.5;
                    generateCubicBezierPoints(dst, dst.back().x(), dst.back().y(), x0, y0, x1, y1, xm, ym, smoothness);
                }
            }
            // It is a quadratic bezier
            else {
                // If point #1 is the only control point, directly generate a quadratic bezier curve
                if(!isCtl0 && isCtl1 && !isCtl2) {
                    generateQuadraticBezierPoints(dst, dst.back().x(), dst.back().y(), x1, y1, x2, y2, smoothness);
                }
                // If point #1 and point #2 are both control points, generate a quadratic bezier curve
                // using the halfway point between the two control points
                else if(!isCtl0 && isCtl1 && isCtl2) {
                    const double xm = (x1 + x2) * 0.5;
                    const double ym = (y1 + y2) * 0.5;
                    generateQuadraticBezierPoints(dst, dst.back().x(), dst.back().y(), x1, y1, xm, ym, smoothness);
                }
            }
        }
        // Update the start index of the contour
        cSIdx = cEIdx + 1;
    }
