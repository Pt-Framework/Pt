// Inspired by http://create.stephan-brumme.com/antialiased-circle
void ImagePainter2::fillEllipse( const PointF& topLeft, const SizeF& size )
{
    // Update the gradient as needed
    _rasterizer->updateGradientBrushAsNeeded(size.width(), size.height());

    // Call the fast non-AA rasterizer as needed
    if(_rasterizer->antiAliasingMode() == AntiAliasingMode::None) {
        fillEllipseImplNoAA(topLeft, size);
        return;
    }

    // Calculate the ellipse's parameters
    Pt::int32_t minX  = topLeft.x();
    Pt::int32_t minY  = topLeft.y();
    Pt::int32_t radX  = size.width () / 2;
    Pt::int32_t radY  = size.height() / 2;
    Pt::int32_t ctrX  = minX + radX;
    Pt::int32_t ctrY  = minY + radY;
    Pt::int32_t radX2 = radX * radX;
    Pt::int32_t radY2 = radY * radY;

    // === Process the scanlines ===

    // List of scanlines to be drawn later
    Scanlines scanlines;

    // Top and bottom halves
    const Pt::int32_t quartersX = round( radX2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the Y coordinate
        const float       y   = radY * fastSqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly = floor(y);
        // Store/update the scanline coordinates
        Scanlines::iterator it1 = scanlines.find(ctrY - fly);
        Scanlines::iterator it2 = scanlines.find(ctrY + fly);
        if(it1 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY - fly, ScanlineElement(ctrX - x, ctrX + x) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - x < it1->second.from ) it1->second.from = ctrX - x;
            if( ctrX + x > it1->second.to   ) it1->second.to   = ctrX + x;
        }
        if(it2 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY + fly, ScanlineElement(ctrX - x, ctrX + x) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - x < it2->second.from ) it2->second.from = ctrX - x;
            if( ctrX + x > it2->second.to   ) it2->second.to   = ctrX + x;
        }
    }

    // Left and right halves
    const Pt::int32_t quartersY = round( radY2 * fastInvSqrt(radX2 + radY2) );

    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the X coordinate
        const float       x   = radX * fastSqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx = floor(x);
        // Store/update the scanline coordinates
        Scanlines::iterator it1 = scanlines.find(ctrY - y);
        Scanlines::iterator it2 = scanlines.find(ctrY + y);
        if(it1 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY - y, ScanlineElement(ctrX - flx, ctrX + flx) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - flx < it1->second.from ) it1->second.from = ctrX - flx;
            if( ctrX + flx > it1->second.to   ) it1->second.to   = ctrX + flx;
        }
        if(it2 == scanlines.end()) { // Insert a new element
            scanlines.insert( std::make_pair( ctrY + y, ScanlineElement(ctrX - flx, ctrX + flx) ) );
        }
        else { // Update the scanline's "from" and "to" coordinates
            if( ctrX - flx < it2->second.from ) it2->second.from = ctrX - flx;
            if( ctrX + flx > it2->second.to   ) it2->second.to   = ctrX + flx;
        }
    }

    // Draw the scanlines
    for(Scanlines::const_iterator it = scanlines.begin(); it != scanlines.end(); ++it) {
        _rasterizer->fillOneScanlineNoAA(it->second.from, it->second.to, it->first, minX, minY);
    }

    scanlines.clear();

    // === Process the circumference's pixels ===

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= quartersX; ++x) {
        // Calculate the Y coordinate and alpha
        const float       y     = radY * fastSqrt(1 - (float) x * x / radX2);
        const Pt::int32_t fly   = floor(y);
        const float       error = y - fly;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = ctrX - x;
        const Pt::int32_t x2 = ctrX + x;
        const Pt::int32_t y1 = ctrY - fly - 1;
        const Pt::int32_t y2 = ctrY + fly + 1;
        _rasterizer->fill4Pixels(x1, y1, x2, y2, minX, minY, alpha);
        /*
        // Draw the first part of the pixels
        const Pt::int32_t x1  = ctrX - x;
        const Pt::int32_t x2  = ctrX + x;
        const Pt::int32_t y10 = ctrY - fly;
        const Pt::int32_t y20 = ctrY + fly;
        Scanlines::const_iterator it10 = scanlines.find(y10);
        Scanlines::const_iterator it20 = scanlines.find(y20);
        if( ( it10 == scanlines.end() || (it10->second.from > x1 && it10->second.to < x2) ) ||
            ( it20 == scanlines.end() || (it20->second.from > x1 && it20->second.to < x2) )
        ) _rasterizer->fill4Pixels(x1, y10, x2, y20, minX, minY, alpha);
        // Draw the second part of the pixels
        const Pt::int32_t y11 = ctrY - fly - 1;
        const Pt::int32_t y21 = ctrY + fly + 1;
        Scanlines::const_iterator it11 = scanlines.find(y11);
        Scanlines::const_iterator it21 = scanlines.find(y21);
        if( ( it11 == scanlines.end() || (it11->second.from > x1 && it11->second.to < x2) ) ||
            ( it21 == scanlines.end() || (it21->second.from > x1 && it21->second.to < x2) )
        ) _rasterizer->fill4Pixels(x1, y11, x2, y21, minX, minY, alpha);
        */
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= quartersY; ++y) {
        // Calculate the X coordinate and alpha
        const float       x     = radX * fastSqrt(1 - (float) y * y / radY2);
        const Pt::int32_t flx   = floor(x);
        const float       error = x - flx;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = ctrX - flx - 1;
        const Pt::int32_t x2 = ctrX + flx + 1;
        const Pt::int32_t y1 = ctrY - y;
        const Pt::int32_t y2 = ctrY + y;
        _rasterizer->fill4Pixels(x1, y1, x2, y2, minX, minY, alpha);
        /*
        // Draw the first part of the pixels
        const Pt::int32_t x10 = ctrX - flx;
        const Pt::int32_t x20 = ctrX + flx;
        const Pt::int32_t y1  = ctrY - y;
        const Pt::int32_t y2  = ctrY + y;
        Scanlines::const_iterator it1 = scanlines.find(y1);
        Scanlines::const_iterator it2 = scanlines.find(y2);
        if( ( it1 == scanlines.end() || (it1->second.from > x10 && it1->second.to < x20) ) ||
            ( it2 == scanlines.end() || (it2->second.from > x10 && it2->second.to < x20) )
        ) _rasterizer->fill4Pixels(x10, y1, x20, y2, minX, minY, alpha);
        // Draw the second part of the pixels
        const Pt::int32_t x11 = ctrX - flx - 1;
        const Pt::int32_t x21 = ctrX + flx + 1;
        if( ( it1 == scanlines.end() || (it1->second.from > x11 && it1->second.to < x21) ) ||
            ( it2 == scanlines.end() || (it2->second.from > x11 && it2->second.to < x21) )
        ) _rasterizer->fill4Pixels(x11, y1, x21, y2, minX, minY, alpha);
        */
    }
}




        static void arcUtil_findExactBegEndPointsCoordinate(FilledArcInfo& fai);
        static void arcUtil_runXWLineAlgorithm(XWLineData& xwLine, Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2);
        static void arcUtil_genScanlinesForChord(const FilledArcInfo& fai, XWLineData& xwLine, Scanlines& scanlines/*, Scanlines& scanlinesRef*/);
        static void arcUtil_cropAndStoreScanlineForChord(XWLineData& xwLine, Scanlines& scanlines, /*Scanlines& scanlinesRef,*/ Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t yt, Pt::int32_t yb);

        void arcUtil_drawCircumferencePixels(FilledArcInfo& fai/*, const Scanlines& scanlinesRef*/);
        void arcUtil_drawXWLine(const FilledArcInfo& fai, const XWLineData& xwLine);


void ImagePainter2::arcUtil_genScanlinesForChord(const FilledArcInfo& fai, XWLineData& xwLine, Scanlines& scanlines/*, Scanlines& scanlinesRef*/)
{
    // Find the line's minimum and maximum Y coordinates
    Pt::int32_t lineMinY, lineMaxY;

    lineMinY = xwLine.points.begin ()->first;
    lineMaxY = xwLine.points.rbegin()->first;

    //lprintf("%d %d\n", lineMinY, lineMaxY);

    // Minimum and maximum X coordinates of the shape
    const Pt::int32_t xlMin = std::min(fai.x1, fai.x2);
    const Pt::int32_t xlMax = std::max(fai.x1, fai.x2);

    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the coordinate
        const float       y  = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t yt = fai.ctrY - ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t yb = fai.ctrY + ( fai.antiAlias ? floor(y) : round(y) );
        const Pt::int32_t xl = fai.ctrX - x;
        const Pt::int32_t xr = fai.ctrX + x;
        // Skip if the scanline will be completely outside the shape
        if(xwLine.faceL && xr < xlMin) continue;
        if(xwLine.faceR && xl > xlMax) continue;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForChord(xwLine, scanlines, /*scanlinesRef,*/ lineMinY, lineMaxY, xl, xr, yt, yb);
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Calculate the coordinate
        const float       x  = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t yt = fai.ctrY - y;
        const Pt::int32_t yb = fai.ctrY + y;
        const Pt::int32_t xl = fai.ctrX - ( fai.antiAlias ? floor(x) : round(x) );
        const Pt::int32_t xr = fai.ctrX + ( fai.antiAlias ? floor(x) : round(x) );
        // Skip if the scanline will be completely outside the shape
        if(xwLine.faceL && xr < xlMin) continue;
        if(xwLine.faceR && xl > xlMax) continue;
        // Store/update the scanline coordinates
        arcUtil_cropAndStoreScanlineForChord(xwLine, scanlines, /*scanlinesRef,*/ lineMinY, lineMaxY, xl, xr, yt, yb);
    }
}

void ImagePainter2::arcUtil_cropAndStoreScanlineForChord(XWLineData& xwLine, Scanlines& scanlines, /*Scanlines& scanlinesRef,*/ Pt::int32_t lineMinY, Pt::int32_t lineMaxY, Pt::int32_t xl, Pt::int32_t xr, Pt::int32_t yt, Pt::int32_t yb)
{
    /*
    // Store/update the reference scanline coordinates
    Scanlines::iterator rit1 = scanlinesRef.find(yt);
    if(rit1 == scanlinesRef.end()) { // Insert a new element
        scanlinesRef.insert( std::make_pair( yt, ScanlineElement(xl, xr) ) );
    }
    else { // Update the reference scanline's "from" and "to" coordinates
        if( xl < rit1->second.from ) rit1->second.from = xl;
        if( xr > rit1->second.to   ) rit1->second.to   = xr;
    }

    Scanlines::iterator rit2 = scanlinesRef.find(yb);
    if(rit2 == scanlinesRef.end()) { // Insert a new element
        scanlinesRef.insert( std::make_pair( yb, ScanlineElement(xl, xr) ) );
    }
    else { // Update the reference scanline's "from" and "to" coordinates
        if( xl < rit2->second.from ) rit2->second.from = xl;
        if( xr > rit2->second.to   ) rit2->second.to   = xr;
    }
    */

    // For convenience
    typedef XWLineData::XWPoints::iterator XWPointsIterator;

    // Store/update the scanline coordinates
    if( (!xwLine.faceT && !xwLine.faceB) || (xwLine.faceT && yt >= lineMinY) || (xwLine.faceB && yt <= lineMaxY) ) {
        // Get the element with the wanted coordinate
        XWPointsIterator lwb = xwLine.points.lower_bound(yt);
        XWPointsIterator upb = xwLine.points.upper_bound(yt);
        XWPointsIterator lit = lwb;
        for(XWPointsIterator cit = lwb; cit != upb; ++cit) {
            if(xwLine.faceL && cit->second.x > lit->second.x) lit = cit;
            if(xwLine.faceR && cit->second.x < lit->second.x) lit = cit;
        }
        // Crop the coordinates
        Pt::int32_t xlc = xl;
        Pt::int32_t xrc = xr;
        if(lit != xwLine.points.end()) {
            if(xwLine.faceL) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xlc < lit->second.x + 1) xlc = lit->second.x + 1;
                }
                else { // (X)
                    if(xlc <= lit->second.x) xlc = lit->second.x + 1;
                }
            }
            if(xwLine.faceR) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xrc > lit->second.x) xrc = lit->second.x;
                }
                else { // (X)
                    if(xrc >= lit->second.x) xrc = lit->second.x - 1;
                }
            }
        }
        // Store/update the scanline coordinates as needed
        if(xrc >= xlc) {
            Scanlines::iterator sit = scanlines.find(yt);
            if(sit == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yt, ScanlineElement(xlc, xrc) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xlc < sit->second.from ) sit->second.from = xlc;
                if( xrc > sit->second.to   ) sit->second.to   = xrc;
            }
        }
    }

    if( (!xwLine.faceT && !xwLine.faceB) || (xwLine.faceT && yb >= lineMinY) || (xwLine.faceB && yb <= lineMaxY) ) {
        // Get the element with the wanted coordinate
        XWPointsIterator lwb = xwLine.points.lower_bound(yb);
        XWPointsIterator upb = xwLine.points.upper_bound(yb);
        XWPointsIterator lit = lwb;
        for(XWPointsIterator cit = lwb; cit != upb; ++cit) {
            if(xwLine.faceL && cit->second.x > lit->second.x) lit = cit;
            if(xwLine.faceR && cit->second.x < lit->second.x) lit = cit;
        }
        // Crop the coordinates
        Pt::int32_t xlc = xl;
        Pt::int32_t xrc = xr;
        if(lit != xwLine.points.end()) {
            if(xwLine.faceL) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xlc < lit->second.x + 1) xlc = lit->second.x + 1;
                }
                else { // (X)
                    if(xlc <= lit->second.x) xlc = lit->second.x + 1;
                }
            }
            if(xwLine.faceR) {
                if(xwLine.steep) { // (X), (X + 1)
                    if(xrc > lit->second.x) xrc = lit->second.x;
                }
                else { // (X)
                    if(xrc >= lit->second.x) xrc = lit->second.x - 1;
                }
            }
        }
        // Store/update the scanline coordinates as needed
        if(xrc >= xlc) {
            Scanlines::iterator sit = scanlines.find(yb);
            if(sit == scanlines.end()) { // Insert a new element
                scanlines.insert( std::make_pair( yb, ScanlineElement(xlc, xrc) ) );
            }
            else { // Update the scanline's "from" and "to" coordinates
                if( xlc < sit->second.from ) sit->second.from = xlc;
                if( xrc > sit->second.to   ) sit->second.to   = xrc;
            }
        }
    }
}

void ImagePainter2::arcUtil_drawCircumferencePixels(FilledArcInfo& fai/*, const Scanlines& scanlinesRef*/)
{
    // Top and bottom halves
    for(Pt::int32_t x = 0; x <= fai.quartersX; ++x) {
        // Calculate the Y coordinate and alpha
        const float       y     = fai.radY * fastSqrt(1 - (float) x * x / fai.radX2);
        const Pt::int32_t fly   = floor(y);
        const float       error = y - fly;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = fai.ctrX - x;
        const Pt::int32_t x2 = fai.ctrX + x;
        const Pt::int32_t y1 = fai.ctrY - fly - 1;
        const Pt::int32_t y2 = fai.ctrY + fly + 1;
        const bool mask[4] = {
            pointIsInsideArcDegRange(x1, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
            pointIsInsideArcDegRange(x1, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
            pointIsInsideArcDegRange(x2, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
            pointIsInsideArcDegRange(x2, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
        };
        _rasterizer->fill4Pixels(x1, y1, x2, y2, fai.minX, fai.minY, alpha, mask);
        /*
        // Draw the first part of the pixels
        const Pt::int32_t x1  = fai.ctrX - x;
        const Pt::int32_t x2  = fai.ctrX + x;
        const Pt::int32_t y10 = fai.ctrY - fly;
        const Pt::int32_t y20 = fai.ctrY + fly;
        Scanlines::const_iterator it10 = scanlinesRef.find(y10);
        Scanlines::const_iterator it20 = scanlinesRef.find(y20);
        if( ( it10 == scanlinesRef.end() || (it10->second.from > x1+1 && it10->second.to < x2-1) ) ||
            ( it20 == scanlinesRef.end() || (it20->second.from > x1+1 && it20->second.to < x2-1) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x1, y10, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x1, y20, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y10, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y20, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x1, y10, x2, y20, fai.minX, fai.minY, 255 - alpha, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t y11 = fai.ctrY - fly - 1;
        const Pt::int32_t y21 = fai.ctrY + fly + 1;
        Scanlines::const_iterator it11 = scanlinesRef.find(y11);
        Scanlines::const_iterator it21 = scanlinesRef.find(y21);
        if( ( it11 == scanlinesRef.end() || (it11->second.from > x1 && it11->second.to < x2) ) ||
            ( it21 == scanlinesRef.end() || (it21->second.from > x1 && it21->second.to < x2) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x1, y11, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x1, y21, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y11, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x2, y21, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x1, y11, x2, y21, fai.minX, fai.minY, alpha, mask);
        }
        */
    }

    // Left and right halves
    for(Pt::int32_t y = 0; y <= fai.quartersY; ++y) {
        // Calculate the X coordinate and alpha
        const float       x     = fai.radX * fastSqrt(1 - (float) y * y / fai.radY2);
        const Pt::int32_t flx   = floor(x);
        const float       error = x - flx;
        const Pt::uint8_t alpha = round(error * 255);
        // Draw the pixels
        const Pt::int32_t x1 = fai.ctrX - flx - 1;
        const Pt::int32_t x2 = fai.ctrX + flx + 1;
        const Pt::int32_t y1 = fai.ctrY - y;
        const Pt::int32_t y2 = fai.ctrY + y;
        const bool mask[4] = {
            pointIsInsideArcDegRange(x1, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
            pointIsInsideArcDegRange(x1, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
            pointIsInsideArcDegRange(x2, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
            pointIsInsideArcDegRange(x2, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
        };
        _rasterizer->fill4Pixels(x1, y1, x2, y2, fai.minX, fai.minY, alpha, mask);
        /*
        // Draw the first part of the pixels
        const Pt::int32_t x10 = fai.ctrX - flx;
        const Pt::int32_t x20 = fai.ctrX + flx;
        const Pt::int32_t y1  = fai.ctrY - y;
        const Pt::int32_t y2  = fai.ctrY + y;
        Scanlines::const_iterator it1 = scanlinesRef.find(y1);
        Scanlines::const_iterator it2 = scanlinesRef.find(y2);
        if( ( it1 == scanlinesRef.end() || (it1->second.from > x10 && it1->second.to < x20) ) ||
            ( it2 == scanlinesRef.end() || (it2->second.from > x10 && it2->second.to < x20) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x10, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x10, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x20, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x20, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x10, y1, x20, y2, fai.minX, fai.minY, 255 - alpha, mask);
        }
        // Draw the second part of the pixels
        const Pt::int32_t x11 = fai.ctrX - flx - 1;
        const Pt::int32_t x21 = fai.ctrX + flx + 1;
        if( ( it1 == scanlinesRef.end() || (it1->second.from > x11 && it1->second.to < x21) ) ||
            ( it2 == scanlinesRef.end() || (it2->second.from > x11 && it2->second.to < x21) )
        ) {
            const bool mask[4] = {
                pointIsInsideArcDegRange(x11, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x11, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x21, y1, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd),
                pointIsInsideArcDegRange(x21, y2, fai.ctrX, fai.ctrY, fai.degBegin, fai.degEnd)
            };
            _rasterizer->fill4Pixels(x11, y1, x21, y2, fai.minX, fai.minY, alpha, mask);
        }
        */
    }
}

void ImagePainter2::arcUtil_drawXWLine(const FilledArcInfo& fai, const XWLineData& xwLine)
{
    for(XWLineData::XWPoints::const_iterator it = xwLine.points.begin(); it != xwLine.points.end(); ++it) {
        const Pt::int32_t y  = it->first;
        const Pt::int32_t x  = it->second.x;
        const Pt::int32_t a1 = it->second.a1;
        const Pt::int32_t a2 = it->second.a2;
        if(xwLine.steep) {
            if( xwLine.faceL && (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) )
                _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a1);
            if( xwLine.faceR && (x + 1 != fai.x1 || y != fai.y1) && (x + 1 != fai.x2 || y != fai.y2) )
                _rasterizer->fillPixel(x + 1, y, fai.minX, fai.minY, a2);
            //_rasterizer->fillPixel(x,     y, fai.minX, fai.minY, a1);
            //_rasterizer->fillPixel(x + 1, y, fai.minX, fai.minY, a2);
        }
        else {
            if( (x != fai.x1 || y != fai.y1) && (x != fai.x2 || y != fai.y2) ) {
                if(xwLine.faceT) _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a1);
                if(xwLine.faceB) _rasterizer->fillPixel(x, y, fai.minX, fai.minY, a2);
            }
            //_rasterizer->fillPixel(x, y,     fai.minX, fai.minY, a1);
            //_rasterizer->fillPixel(x, y + 1, fai.minX, fai.minY, a2);
        }
    }
}

