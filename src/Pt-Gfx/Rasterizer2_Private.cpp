// ======================================================================================
// ===== Private Member Functions - Rasterisations ======================================
// ======================================================================================

void Rasterizer2::rasterOnePixelLine(const Point& a, const Point& b)
{
    // Clip the points
    Pt::int32_t x1 = a.x();
    Pt::int32_t y1 = a.y();
    Pt::int32_t x2 = b.x();
    Pt::int32_t y2 = b.y();

    if(!ClipShape::clipLine(x1, y1, x2, y2, _currentClip)) return;

    // Find the minimum and maximum coordinates
    Pt::int32_t minX, minY, maxX, maxY;

    if(x2 > x1) {
        minX = x1;
        maxX = x2;
    }
    else {
        minX = x2;
        maxX = x1;
    }

    if(y2 > y1) {
        minY = y1;
        maxY = y2;
    }
    else {
        minY = y2;
        maxY = y1;
    }

    // Calculate the size of the line
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;
    if(!sizeX && !sizeY) return;

    // Check for horizontal line
    if(minY == maxY) {
        rasterOnePixelHLineSegment(minX, maxX, minY, _pen.color());
        return;
    }

    // Check for vertical line
    if(minX == maxX) {
        rasterOnePixelVLineSegment(minX, minY, maxY, _pen.color());
        return;
    }

    // Convert the coordinates to fixed-points
    const Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    const Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    const Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    const Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Raster the line
    rasterOnePixelLineSegment(fx1, fy1, fx2, fy2, _pen.color(), false);
}

void Rasterizer2::rasterOnePixelHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color)
{
    // Calculate the length of the line
    const Pt::int32_t sizeL = x2 - x1 + 1;

    // Draw the line
    Pixel pixel(_image->view(), x1, y);

    for(Pt::int32_t i = 0; i < sizeL; ++i) {
        _image->format().setPixel(pixel, color, _compositionMode);
        pixel.advance();
    }
}

void Rasterizer2::rasterOnePixelVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color)
{
    // Calculate the length of the line
    const Pt::int32_t sizeL = y2 - y1 + 1;

    // Draw the line
    Pixel pixel(_image->view(), x, y1);

    for(Pt::int32_t i = 0; i < sizeL; ++i) {
        _image->format().setPixel(pixel, color, _compositionMode);
        pixel.advance(_image->width());
    }
}

void Rasterizer2::rasterOnePixelLineSegment(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, const Color& color, bool skipLastPoint)
{
    // Xiaolin Wu's Anti-Aliased Line Algorithm
    // https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm

    // A helper macro to set pixel
    #define XW_SET_PIXEL(IMG, COL, X, Y, A)                                        \
        do {                                                                       \
            if( X < 0 || X >= IMG->width() || Y < 0 || Y >= IMG->height() ) break; \
            Pixel PIX(IMG->view(), X, Y);                                          \
            IMG->format().setPixel(PIX, COL, _compositionMode, A);                 \
        } while(false)

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    bool              steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    if(fx1 > fx2) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Calculate the gradient
    Pt::int32_t gradient = (fy2 - fy1) / ((fx2 - fx1) >> FIXED_POINT_SHIFT_FACTOR);

    // Handle the first endpoint
    Pt::int32_t xend  = FIXED_POINT_ROUND(fx1);
    Pt::int32_t yend  = fy1 + gradient * FIXED_POINT_TO_INT(xend - fx1);
    Pt::int32_t xgap  = FIXED_POINT_RFPART(fx1 + FIXED_POINT_CONSTANT_HALF);
    Pt::int32_t xpxl1 = xend; // Will be used to raster the rest of the pixels
    Pt::int32_t ypxl1 = FIXED_POINT_IPART(yend);
    Pt::uint8_t a1    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_RFPART(yend), xgap);
    Pt::uint8_t a2    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_FPART (yend), xgap);
    if(steep) {
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl1                           ), FIXED_POINT_TO_INT(xpxl1), a1);
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl1 + FIXED_POINT_CONSTANT_ONE), FIXED_POINT_TO_INT(xpxl1), a2);
    }
    else {
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl1), FIXED_POINT_TO_INT(ypxl1                           ), a1);
        XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl1), FIXED_POINT_TO_INT(ypxl1 + FIXED_POINT_CONSTANT_ONE), a2);
    }

    // First y-intersection for the main loop
    Pt::int32_t intery = yend + gradient;

    // Handle the second endpoint
                xend  = FIXED_POINT_ROUND(fx2);
                yend  = fy2 + gradient * FIXED_POINT_TO_INT(xend - fx2);
                xgap  = FIXED_POINT_RFPART(fx2 + FIXED_POINT_CONSTANT_HALF);
    Pt::int32_t xpxl2 = xend; // Will be used to raster the rest of the pixels
    Pt::int32_t ypxl2 = FIXED_POINT_IPART(yend);
                a1    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_RFPART(yend), xgap);
                a2    = FIXED_POINT_MUL_TO_A8(FIXED_POINT_FPART (yend), xgap);
    if(!skipLastPoint) {
        if(steep) {
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl2                           ), FIXED_POINT_TO_INT(xpxl2), a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(ypxl2 + FIXED_POINT_CONSTANT_ONE), FIXED_POINT_TO_INT(xpxl2), a2);
        }
        else {
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl2), FIXED_POINT_TO_INT(ypxl2                           ), a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(xpxl2), FIXED_POINT_TO_INT(ypxl2 + FIXED_POINT_CONSTANT_ONE), a2);
        }
    }

    // Loop through the rest of the pixels
    Pt::int32_t from = FIXED_POINT_TO_INT(xpxl1 + FIXED_POINT_CONSTANT_ONE);
    Pt::int32_t to   = FIXED_POINT_TO_INT(xpxl2 - FIXED_POINT_CONSTANT_ONE);
    if(steep) {
        for(Pt::int32_t i = from; i <= to; ++i) {
            a1 = FIXED_POINT_RFPART_TO_A8(intery);
            a2 = FIXED_POINT_FPART_TO_A8 (intery);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery)                           ), i, a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery) + FIXED_POINT_CONSTANT_ONE), i, a2);
            intery += gradient;
        }
    }
    else {
        for(Pt::int32_t i = from; i <= to; ++i) {
            a1 = FIXED_POINT_RFPART_TO_A8(intery);
            a2 = FIXED_POINT_FPART_TO_A8 (intery);
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery)                           ), a1);
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery) + FIXED_POINT_CONSTANT_ONE), a2);
            intery += gradient;
        }
    }
}

void Rasterizer2::rasterRectArea(const Point& tl, const Point& br)
{
    // Get the minimum and maximum coordinates
    Pt::int32_t minX = tl.x();
    Pt::int32_t minY = tl.y();
    Pt::int32_t maxX = br.x();
    Pt::int32_t maxY = br.y();

    // Clip the coordinates
    if(minX < _currentClip.left  ()) minX = _currentClip.left  ();
    if(minY < _currentClip.top   ()) minY = _currentClip.top   ();
    if(maxX > _currentClip.right ()) maxX = _currentClip.right ();
    if(maxY > _currentClip.bottom()) maxY = _currentClip.bottom();

    // Calculate the width of the rectangle
    const Pt::int32_t sizeX = maxX - minX + 1;

    // Draw the rectangles
    for(Pt::int32_t y = minY; y <= maxY; ++y) {
        Pt::int32_t spanWidth = sizeX;
        while(spanWidth > 0) {
            const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
            Pixel             pixel(_image->view(), minX + sizeX - spanWidth, y);
            _image->format().copy(pixel, _brushPixel, n, _compositionMode);
            spanWidth -= n;
        }
    }
}

void Rasterizer2::rasterOnePixelRectOutline(const Point& tl, const Point& br)
{
    // Get the minimum and maximum coordinates
    Pt::int32_t minX = tl.x();
    Pt::int32_t minY = tl.y();
    Pt::int32_t maxX = br.x();
    Pt::int32_t maxY = br.y();

    // Clip the coordinates
    if(minX < _currentClip.left  ()) minX = _currentClip.left  ();
    if(minY < _currentClip.top   ()) minY = _currentClip.top   ();
    if(maxX > _currentClip.right ()) maxX = _currentClip.right ();
    if(maxY > _currentClip.bottom()) maxY = _currentClip.bottom();

    // Draw the rectangle's horizontal lines
    rasterOnePixelHLineSegment(minX, maxX, minY, _pen.color());
    rasterOnePixelHLineSegment(minX, maxX, maxY, _pen.color());

    // Draw the rectangle's vertical lines
    rasterOnePixelVLineSegment(minX, minY + 1, maxY - 1, _pen.color());
    rasterOnePixelVLineSegment(maxX, minY + 1, maxY - 1, _pen.color());
}

// Based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonArea(const Point* points, size_t pointCount, const Color& color)
{
    // Find the minimum and maximum coordinates
    Pt::int32_t minX =  65535;
    Pt::int32_t minY =  65535;
    Pt::int32_t maxX = -65535;
    Pt::int32_t maxY = -65535;

    for(size_t i = 0; i < pointCount; ++i) {
        const Pt::int32_t x = points[i].x();
        const Pt::int32_t y = points[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

#if defined(SUPERSAMPLING_SIZE) && ( SUPERSAMPLING_SIZE == 2 ||  SUPERSAMPLING_SIZE == 4 )

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1) * SUPERSAMPLING_SIZE;

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to match the super sampling size and translate it to (0, 0)
    std::vector<Pt::int32_t> pointX(pointCount, 0);
    std::vector<Pt::int32_t> pointY(pointCount, 0);

    for(size_t i = 0; i < pointCount; ++i) {
        pointX[i] = points[i].x() * SUPERSAMPLING_SIZE - minX * SUPERSAMPLING_SIZE;
        pointY[i] = points[i].y() * SUPERSAMPLING_SIZE - minY * SUPERSAMPLING_SIZE;
    }

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeXf(pointCount * 2, 0);

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( pointY[i] < pixelY && pointY[j] >= pixelY ) ||
                ( pointY[j] < pixelY && pointY[i] >= pixelY )
            ) {
                Pt::int32_t deltaYp = pixelY    - pointY[i];
                Pt::int32_t deltaYj = pointY[j] - pointY[i];
                Pt::int32_t deltaXj = pointX[j] - pointX[i];
                Pt::int32_t interXf = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeXf[nodes++] = interXf + FIXED_POINT_CONSTANT_HALF;
                // Bail out if we have produced too many nodes
                if((size_t)nodes >= nodeXf.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeXf[i] > nodeXf[i + 1]) {
                std::swap(nodeXf[i], nodeXf[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes/2; i += 2) {
            Pt::int32_t from = FIXED_POINT_TO_INT(nodeXf[i    ]);
            Pt::int32_t to   = FIXED_POINT_TO_INT(nodeXf[i + 1]);
            for(Pt::int32_t k = from; k <= to; ++k) {
                alphas[k / SUPERSAMPLING_SIZE] += 15;
            }
        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SUPERSAMPLING_SIZE) ) continue;

        #define SCALE_ALPHA(A) ( (A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

#if 0

        // Draw pixels that belongs to the left-part of the shape to the image
        const Pt::uint8_t* iterB = alphas.data();
        const Pt::uint8_t* iterE = iterB + sizeX;
        const Pt::uint8_t* iterL = iterB;
        while(iterL < iterE) {
            // Get and check the alpha
            Pt::uint8_t a = *iterL++;
            if(!a) continue;
            // Draw the 1st pixel from the left
            Pixel pixel(_image->view(), (iterL - iterB) + minX, pixelY / SUPERSAMPLING_SIZE + minY);
            _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(a));
            if(iterL >= iterE) break;
            // Draw the 2nd pixel from the left
            pixel.advance();
            _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(*iterL++));
            if(iterL >= iterE) break;
            // Draw the 3rd pixel from the left
            pixel.advance();
            _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(*iterL  ));
            break;
        }
        // Draw pixels that belongs to the right-part of the shape to the image
        const Pt::uint8_t* iterR = alphas.data() + sizeX - 1 - 3;
        const Pt::uint8_t* iterF = iterR;
        while(iterR > iterL) {
            // Get and check the alpha
            Pt::uint8_t a = *iterR--;
            if(!a) continue;
            //
            iterR -= 3;
            iterF = iterR;
            if(iterR <= iterL) break;
            // Draw the 3rd pixel from the right
            Pixel pixel(_image->view(), (iterR - iterB) + minX, pixelY / SUPERSAMPLING_SIZE + minY);
            _image->format().setPixel(pixel, color, _compositionMode,  SCALE_ALPHA(*iterR++));
            if(iterR >= iterE) break;
            // Draw the 2nd pixel from the right
            pixel.advance();
            _image->format().setPixel(pixel, color, _compositionMode,  SCALE_ALPHA(*iterR++));
            if(iterR >= iterE) break;
            // Draw the 1st pixel from the right
            pixel.advance();
            _image->format().setPixel(pixel, color, _compositionMode,  SCALE_ALPHA(*iterR  ));
            break;
        }
        // Draw pixels that belongs to the middle-part of the shape to the image
        Pt::int32_t from      = (iterL + 1 - iterB);
        Pt::int32_t to        = (iterF - 1 - iterB);
        if(to >= from) {
            Pt::int32_t spanWidth = to - from + 1;
            Pixel       pixel(_image->view(), minX + from, minY + pixelY / SUPERSAMPLING_SIZE);
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                pixel.advance(n);
                spanWidth -= n;
            }
        }

#else
        // Draw pixels to the image
        Pixel pixel(_image->view(), minX, pixelY / SUPERSAMPLING_SIZE + minY);
        for(Pt::int32_t iterX = 0; iterX < sizeX; ++iterX) {
            Pt::uint8_t acc = SCALE_ALPHA(alphas[ iterX ]);
            _image->format().setPixel(pixel, color, _compositionMode, acc);
            pixel.advance();
        }
#endif
        // Clear the work buffer
        memset(&alphas[0], 0, alphas.size());
    }

#else

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeXf(pointCount * 2, 0);

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( points[i].y() < pixelY && points[j].y() >= pixelY ) ||
                ( points[j].y() < pixelY && points[i].y() >= pixelY )
            ) {
                Pt::int32_t deltaYp = pixelY        - points[i].y();
                Pt::int32_t deltaYj = points[j].y() - points[i].y();
                Pt::int32_t deltaXj = points[j].x() - points[i].x();
                Pt::int32_t interXf = FIXED_POINT_FROM_INT(points[i].x()) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeXf[nodes++] = interXf;
                // Bail out if we have produced too many nodes
                if((size_t)nodes >= nodeXf.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeXf[i] > nodeXf[i + 1]) {
                std::swap(nodeXf[i], nodeXf[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Determine the X coordinates
            Pt::int32_t from;
            Pt::int32_t to;
            if(true) {
                Pt::int32_t x1 = FIXED_POINT_TO_INT(nodeXf[i]                           );
                Pt::int32_t x2 = FIXED_POINT_TO_INT(nodeXf[i] + FIXED_POINT_CONSTANT_ONE);
                from = (x1 == x2) ? x1 : x2;
            }
            if(true) {
                Pt::int32_t x1 = FIXED_POINT_TO_INT(nodeXf[i + 1] - FIXED_POINT_CONSTANT_ONE);
                Pt::int32_t x2 = FIXED_POINT_TO_INT(nodeXf[i + 1]                           );
                to = (x1 == x2) ? x1 : x2;
            }
            if(to < from) continue;
            // Draw the spans
            Pt::int32_t spanWidth = to - from + 1;
            Pixel       pixel(_image->view(), from, pixelY);
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                pixel.advance(n);
                spanWidth -= n;
            }
        }
    }

#endif
}

void Rasterizer2::rasterPolygonOutline(const Point* points, size_t pointCount, const Color& color)
{
    // Convert the coordinates to fixed-points
    std::vector<Pt::int32_t> lineX(pointCount);
    std::vector<Pt::int32_t> lineY(pointCount);

    for(size_t i = 0; i < pointCount; ++i) {
        lineX[i] = FIXED_POINT_FROM_INT(points[i].x());
        lineY[i] = FIXED_POINT_FROM_INT(points[i].y());
    }

    // Raster the outlines as multiple one-pixel lines
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        rasterOnePixelLineSegment(lineX[i], lineY[i], lineX[i + 1], lineY[i + 1], color, true);
    }
    rasterOnePixelLineSegment(lineX[0], lineY[0], lineX[pc1], lineY[pc1], color, false);
}


// ======================================================================================
// ===== Private Member Functions - Utilities ===========================================
// ======================================================================================

void Rasterizer2::updateClip()
{
    const Rect imageRect( Point(0,0) , _image->size() );
    _currentClip = _clip.isNull() ? imageRect : _clip.intersect( imageRect );
}

void Rasterizer2::genClippedPolygonPoints(std::vector<Point>& dst, const Point* src, const size_t pointCount) const
{
    dst.clear();

    for(size_t i = 0; i < pointCount; ++i)
        dst.push_back( Point( src[i].x(), src[i].y() ) );

    ClipShape::clipPolygon(dst, _currentClip);
}
