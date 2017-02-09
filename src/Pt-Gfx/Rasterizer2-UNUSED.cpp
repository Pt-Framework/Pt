        /*
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            Pt::int32_t a1 = FIXED_POINT_TO_INT(nodeXf[i    ]);
            Pt::int32_t b1 = FIXED_POINT_TO_INT(nodeXf[i + 1]);
            if(!nodeFl[i]) {
                lprintf("%d\n", i);
                continue;
            }
            for(Pt::int32_t j = 2; j < nodes; j += 2) {
                Pt::int32_t a2 = FIXED_POINT_TO_INT(nodeXf[j    ]);
                Pt::int32_t b2 = FIXED_POINT_TO_INT(nodeXf[j + 1]);
                if(a1 == a2 && b1 == b2) nodeFl[i] = 0;
            }
        }
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            Pt::int32_t from = FIXED_POINT_TO_INT(nodeXf[i    ]);
            Pt::int32_t to   = FIXED_POINT_TO_INT(nodeXf[i + 1]);
            lprintf("%d to %d\n", from, to);
        }
        //*/


        //int m = 0;
        //for(int i = 0; i < sizeX; ++i) if(alphas[i] > m) m = alphas[i]; // 2 =>60 ; 4 => 240
        //lprintf("%d\n", m);



    /*   0 1 2 3 4 5 6 7       0 1 2 3 4 5 6 7     0 1 2 3 4 5 6 7
     * 0 * * * * * * * *     0 * * . . . . . .     0 * * . . . . . .
     * 0 * * * * * * * *     1 * * . . . . . .     1 * * . . . . . .
     * 0 * * * * * * * *     2 * * . . . . . .     2 . . . . . . . .
     * 0 * * * * * * * *     3 * * . . . . . .     3 . . . . . . . .
     * 0 * * * * * * * *     4 * * . . . . . .     4 . . . . . . . .
     * 0 * * * * * * * *     5 * * . . . . . .     5 . . . . . . . .
     * 0 * * * * * * * *     6 * * . . . . . .     6 . . . . . . . .
     * 0 * * * * * * * *     7 * * . . . . . .     7 . . . . . . . .
     */

for(Pt::int32_t iterX = 0; iterX < sizeX; ++iterX) printf("%d", alphas[row + iterX]); printf("\n");

/*
0000000000000000000000000000
0000000011000000000000000000
0000000111000000000000000000
0000000111100000000000000000
0000001111110000000000000000
0000001111110000000000000000
0000011111111000000000000000
0000011111111100000000000000
0000111111111100000000000000
0000111111111110000000000000
0001111111111111000000000000
0001111111111111000000000000
0011111111111111100000000000
0011111111111111110000000000
0111111111111111110000000000
0111111111111111111000000000
1111111111111111111100000000
0001111111111111111100000000
0000001111111111111110000000
0000000001111111111111000000
0000000000001111111111000000
0000000000000001111111100000
0000000000000000001111110000
0000000000000000000001110000
0000000000000000000000001000
0000000000000000000000000000
0000000000000000000000000000
0000000000000000000000000000
*/

/*
#000000000000000000000000000
0020000000000000000000000000
0120000000000000000000000000
0130000000000000000000000000
0240000000000000000000000000
0240000000000000000000000000
0341000000000000000000000000
0342000000000000000000000000
0442000000000000000000000000
0443000000000000000000000000
1444000000000000000000000000
1444000000000000000000000000
2444100000000000000000000000
2444200000000000000000000000
3444200000000000000000000000
3444300000000000000000000000
4444400000000000000000000000
1444400000000000000000000000
0244410000000000000000000000
0034420000000000000000000000
0004420000000000000000000000
0001430000000000000000000000
0000240000000000000000000000
0000030000000000000000000000
0000001000000000000000000000
0000000000000000000000000000
0000000000000000000000000000
*/


void rasterSolidTriangles(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY, Pt::int32_t& sizeX, Pt::int32_t& sizeY);
void rasterOneSolidTriangle(const Point& v1, const Point& v2, const Point& v3);
void rasterOneSolidTriangleBottomFlat(const Point& v1, const Point& v2, const Point& v3);
void rasterOneSolidTriangleTopFlat(const Point& v1, const Point& v2, const Point& v3);
void rasterFillTriangles(Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY, Pt::int32_t& sizeX, Pt::int32_t& sizeY);


    std::vector<Point> tris;

    // Bottom-flat
    tris.push_back(Point(200, 100));
    tris.push_back(Point(100, 200));
    tris.push_back(Point(300, 200));
    // Top-flat
    tris.push_back(Point(100, 300));
    tris.push_back(Point(300, 300));
    tris.push_back(Point(200, 400));
    // Non-flat
    tris.push_back(Point(450, 100));
    tris.push_back(Point(350, 300));
    tris.push_back(Point(650, 400));

    rasterFillTriangles(tris.data(), tris.size(), minX, minY, maxX, maxY, sizeX, sizeY);


void Rasterizer2::rasterSolidTriangles(const Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY, Pt::int32_t& sizeX, Pt::int32_t& sizeY)
{
    // Find the minimum and maximum coordinates
    minX =  65535;
    minY =  65535;
    maxX = -65535;
    maxY = -65535;

    for(size_t i = 0; i < pointCount; ++i) {
        const Pt::int32_t x = points[i].x();
        const Pt::int32_t y = points[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    // Calculate the size of the rectangle
    sizeX = maxX - minX + 1;
    sizeY = maxY - minY + 1;

    // Raster the triangles
    for(size_t i = 0; i < pointCount; i += 3) {
        rasterOneSolidTriangle(points[i], points[i + 1], points[i + 2]);
    }
}

// Based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Rasterizer2::rasterOneSolidTriangle(const Point& v1, const Point& v2, const Point& v3)
{
    // Sort the vertices by its Y coordinates
    const Point* vs[3] = { &v1, &v2, &v3 };

    if( vs[1]->y() < vs[0]->y() ) std::swap( vs[1], vs[0] );
    if( vs[2]->y() < vs[0]->y() ) std::swap( vs[2], vs[0] );
    if( vs[2]->y() < vs[1]->y() ) std::swap( vs[2], vs[1] );

    // Check for bottom-flat triangle
    if(vs[1]->y() == vs[2]->y()) {
        rasterOneSolidTriangleBottomFlat(*vs[0], *vs[1], *vs[2]);
    }
    // Check for top-flat triangle
    else if(vs[0]->y() == vs[1]->y()) {
        rasterOneSolidTriangleTopFlat(*vs[0], *vs[1], *vs[2]);
    }
    // Split the triangle to a bottom-flat and top-flat
    else {
        /*           v1
         *           *
         *          + +
         *         +   +
         *     v2 *     * v4     x4 = x1 + ( (y2 - y1) / (y3 - y1) ) * (x3 - x1)
         *         ++    +       y4 = y2
         *            ++  +
         *              ++ *
         *                  v3
         */
        const Pt::int32_t deltaY10   = vs[1]->y() - vs[0]->y();
        const Pt::int32_t deltaY20   = vs[2]->y() - vs[0]->y();
        const Pt::int32_t deltaX20   = vs[2]->x() - vs[0]->x();
        const Pt::int32_t ratioY1020 = ( (deltaY10 << FIXED_POINT_SHIFT_FACTOR) / deltaY20 );
        const Point       vm( vs[0]->x() + ( (ratioY1020 * deltaX20) >> FIXED_POINT_SHIFT_FACTOR ),
                              vs[1]->y()
                            );
        rasterOneSolidTriangleBottomFlat(*vs[0], *vs[1], vm);
        rasterOneSolidTriangleTopFlat(*vs[1], vm, *vs[2]);
    }
}

// Based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Rasterizer2::rasterOneSolidTriangleBottomFlat(const Point& v1, const Point& v2, const Point& v3)
{
    /*        v1
     *
     *
     *     v2    v3
     */

    const Pt::int32_t chgX1 = ( ( ((Pt::int32_t)v2.x() - (Pt::int32_t)v1.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v2.y() - (Pt::int32_t)v1.y())
                              );
    const Pt::int32_t chgX2 = ( ( ((Pt::int32_t)v3.x() - (Pt::int32_t)v1.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v3.y() - (Pt::int32_t)v1.y())
                              );

    Pt::int32_t curX1 = (v1.x() << FIXED_POINT_SHIFT_FACTOR);
    Pt::int32_t curX2 = (v1.x() << FIXED_POINT_SHIFT_FACTOR);

    for(Pt::int32_t y = v1.y(); y <= v2.y(); ++y) {
        // Calculate the span's position and size
              Pt::int32_t from      = std::min(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
        const Pt::int32_t to        = std::max(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
              Pt::int32_t spanWidth = to - from + 1;
        // Draw the span
        while(spanWidth > 0) {
            const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
            Pixel             dstPixel(_image->view(), from, y);
            ConstPixel        srcPixel(_brushBuffer.view(), 0, 0);
            _image->format().copy(dstPixel, srcPixel, n, _compositionMode);
            from      += n;
            spanWidth -= n;
        }
        // Update the span's coordinates
        /*
        if(y == v1.y()) {
            if(chgX1 <= 0) curX1 -= FIXED_POINT_CONSTANT_ONE;
            else           curX1 += FIXED_POINT_CONSTANT_ONE;
            if(chgX2 >= 0) curX2 += FIXED_POINT_CONSTANT_ONE;
            else           curX2 -= FIXED_POINT_CONSTANT_ONE;
        }
        /*/
        curX1 += chgX1;
        curX2 += chgX2;
    }
}

// Based on http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Rasterizer2::rasterOneSolidTriangleTopFlat(const Point& v1, const Point& v2, const Point& v3)
{
    /*     v1    v2
     *
     *
     *        v3
     */

    const Pt::int32_t chgX1 = ( ( ((Pt::int32_t)v1.x() - (Pt::int32_t)v3.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v3.y() - (Pt::int32_t)v1.y())
                              );
    const Pt::int32_t chgX2 = ( ( ((Pt::int32_t)v2.x() - (Pt::int32_t)v3.x()) << FIXED_POINT_SHIFT_FACTOR ) /
                                  ((Pt::int32_t)v3.y() - (Pt::int32_t)v1.y())
                              );

    Pt::int32_t curX1 = (v3.x() << FIXED_POINT_SHIFT_FACTOR);
    Pt::int32_t curX2 = (v3.x() << FIXED_POINT_SHIFT_FACTOR);

    for(Pt::int32_t y = v3.y(); y > v1.y(); --y) {
        // Calculate the span's position and size
              Pt::int32_t from      = std::min(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
        const Pt::int32_t to        = std::max(curX1, curX2) >> FIXED_POINT_SHIFT_FACTOR;
              Pt::int32_t spanWidth = to - from + 1;
        // Draw the span
        while(spanWidth > 0) {
            const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
            Pixel             dstPixel(_image->view(), from, y);
            ConstPixel        srcPixel(_brushBuffer.view(), 0, 0);
            _image->format().copy(dstPixel, srcPixel, n, _compositionMode);
            from      += n;
            spanWidth -= n;
        }
        // Update the span's coordinates
        /*
        if(y == v3.y()) {
            if(chgX1 <= 0) curX1 -= FIXED_POINT_CONSTANT_ONE;
            else           curX1 += FIXED_POINT_CONSTANT_ONE;
            if(chgX2 >= 0) curX2 += FIXED_POINT_CONSTANT_ONE;
            else           curX2 -= FIXED_POINT_CONSTANT_ONE;
        }
        /*/
        curX1 += chgX1;
        curX2 += chgX2;
    }
}

void Rasterizer2::rasterFillTriangles(Point* points, size_t pointCount, Pt::int32_t& minX, Pt::int32_t& minY, Pt::int32_t& maxX, Pt::int32_t& maxY, Pt::int32_t& sizeX, Pt::int32_t& sizeY)
{
    if(pointCount % 3) return;

    switch( _brush.fillStyle() ) {
        case Brush::Texture:
            break;

        case Brush::VerticalGradient:
            break;

        case Brush::HorizontalGradient:
            break;

        case Brush::Solid:
            rasterSolidTriangles(points, pointCount, minX, minY, maxX, maxY, sizeX, sizeY);
            break;
    }
}






// Based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaNOAA_ORIGINAL(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
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
                if((size_t) nodes >= nodeXf.size()) return;
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
            // Draw the span using texture
            if(_isTexture) {
                Pt::int32_t iterX     = from;
                Pt::int32_t spanWidth = to - from + 1;
                while(spanWidth > 0) {
                    const Pt::int32_t tX = (iterX  - minX) % _brushImage->width ();
                    const Pt::int32_t tY = (pixelY - minY) % _brushImage->height();
                    const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                    if(n) {
                        ConstPixel srcPixel(_brushImage->view(), tX, tY);
                        Pixel      dstPixel(_image->view(), iterX, pixelY);
                        _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                    }
                    spanWidth -= n;
                    iterX     += n;
                }
                continue;
            }
            // Draw the span using gradient
            if(_isGradient) {
                Pt::int32_t iterX     = from;
                Pt::int32_t spanWidth = to - from + 1;
                // Fill the span - vertical gradient
                if(_brush.fillStyle() == Pt::Gfx::Brush::VerticalGradient) {
                    const Pt::int32_t textureY = (pixelY - minY) % _brushImage->height();
                    ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                    Pixel             dstPixel(_image->view(), iterX, pixelY);
                    _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
                }
                // Fill the span - horizontal gradient
                else {
                    while(spanWidth > 0) {
                        const Pt::int32_t tX = (iterX  - minX) % _brushImage->width ();
                        const Pt::int32_t tY = (pixelY - minY) % _brushImage->height();
                        const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                        if(n) {
                            ConstPixel srcPixel(_brushImage->view(), tX, tY);
                            Pixel      dstPixel(_image->view(), iterX, pixelY);
                            _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                        }
                        spanWidth -= n;
                        iterX     += n;
                    }
                }
                continue;
            }
            // Draw the span using solid color
            Pt::int32_t iterX     = from;
            Pt::int32_t spanWidth = to - from + 1;
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                if(n) {
                    Pixel pixel(_image->view(), iterX, pixelY);
                    _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                }
                spanWidth -= n;
                iterX     += n;
            }
        }
    }
}







// Line
#ifdef R2_USE_PIXEL_ITERATOR
    if(steep) {
        ImageView::PixelIterator pixel = _image->view().pixel(FIXED_POINT_TO_INT(ypxl1), FIXED_POINT_TO_INT(xpxl1));
        _image->format().setPixel(*pixel, color, _compositionMode, a1);
        ++pixel;
        _image->format().setPixel(*pixel, color, _compositionMode, a2);
    }
    else {
        ImageView::PixelIterator pixel = _image->view().pixel( FIXED_POINT_TO_INT(xpxl1), FIXED_POINT_TO_INT(ypxl1));
        _image->format().setPixel(*pixel, color, _compositionMode, a1);
        pixel += _image->width();
        _image->format().setPixel(*pixel, color, _compositionMode, a2);
    }
#else

#ifdef R2_USE_PIXEL_ITERATOR
        if(steep) {
            ImageView::PixelIterator pixel = _image->view().pixel(FIXED_POINT_TO_INT(ypxl2), FIXED_POINT_TO_INT(xpxl2));
            _image->format().setPixel(*pixel, color, _compositionMode, a1);
            ++pixel;
            _image->format().setPixel(*pixel, color, _compositionMode, a2);
        }
        else {
            ImageView::PixelIterator pixel = _image->view().pixel(FIXED_POINT_TO_INT(xpxl2), FIXED_POINT_TO_INT(ypxl2));
            _image->format().setPixel(*pixel, color, _compositionMode, a1);
            pixel += _image->width();
            _image->format().setPixel(*pixel, color, _compositionMode, a2);
        }
#else

#ifdef R2_USE_PIXEL_ITERATOR
            ImageView::PixelIterator pixel = _image->view().pixel(FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery)), i);
            _image->format().setPixel(*pixel, color, _compositionMode, a1);
            ++pixel;
            _image->format().setPixel(*pixel, color, _compositionMode, a2);
#else

#ifdef R2_USE_PIXEL_ITERATOR
            ImageView::PixelIterator pixel = _image->view().pixel(i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(intery)));
            _image->format().setPixel(*pixel, color, _compositionMode, a1);
            pixel += _image->width();
            _image->format().setPixel(*pixel, color, _compositionMode, a2);
#else



// Rectagle
#ifdef R2_USE_PIXEL_ITERATOR
    ImageView::PixelIterator pixel = _image->view().pixel(minX, minY);
    for(Pt::int32_t y = minY; y <= maxY; ++y) {
        Pt::int32_t spanWidth = sizeX;
        while(spanWidth > 0) {
            const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
            _image->format().copy(*pixel, _brushPixel, n, _compositionMode);
            pixel     += n;
            spanWidth -= n;
        }
        pixel += (_image->width() - sizeX);
    }
#else






// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::rasterOnePixelGLineSegment_ORIGINAL(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, const Color& color, bool skipLastPoint)
{
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
