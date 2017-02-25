#include <math.h>
// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaNoAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<NodeData> nodeX(totalPointCount * 2, 0);

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY) {
        // Base pointer for the polygons
        const Point* curPointBase = points;
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const Pt::int32_t curXi = (curPointBase + i)->x();
                const Pt::int32_t curYi = (curPointBase + i)->y();
                const Pt::int32_t curXj = (curPointBase + j)->x();
                const Pt::int32_t curYj = (curPointBase + j)->y();
                // Calculate the node's coordinate
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF) /
                                                  deltaYj * deltaXj
                                                );
                    const Pt::int32_t iterX   = FIXED_POINT_TO_INT(interXf);

                    // Convert the coordinates to fixed-points
                    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(curXi);
                    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(curYi);
                    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(curXj);
                    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(curYj);

                    // Swap the values as needed
                    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
                    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
                    const bool        steep  = deltaY > deltaX;
                    if(steep) {
                        std::swap(fx1, fy1);
                        std::swap(fx2, fy2);
                    }

                    if(fx1 > fx2) {
                        std::swap(fx1, fx2);
                        std::swap(fy1, fy2);
                    }
                    // Handle the gradient, starting point, and ending point
                    const Pt::int32_t grad = (fy2 - fy1) / FIXED_POINT_TO_INT(fx2 - fx1);
                    const Pt::int32_t xpxl1 = FIXED_POINT_ROUND(fx1);
                    const Pt::int32_t xpxl2 = FIXED_POINT_ROUND(fx2);
                    const Pt::int32_t ypxl  = fy1 + grad * FIXED_POINT_TO_INT(xpxl1 - fx1);
                    // Iterate the pixels
                    Pt::int32_t from  = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
                    Pt::int32_t to    = FIXED_POINT_TO_INT(xpxl2);
                    Pt::int32_t ypxli = ypxl;

                    Pt::int32_t gx1 = iterX, gx2 = iterX;
                    Pt::uint8_t ga1 = 255, ga2 = 255;

                    if(steep) {
                        for(Pt::int32_t i = from; i <= to; ++i) {
                            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
                            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
                            const Pt::int32_t x1 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           );
                            const Pt::int32_t x2 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE);
                            const Pt::int32_t y  = i;
                            if(y == pixelY) {
                                if(x1 == iterX && x2 == iterX + 1) {
                                    gx1 = x1;
                                    gx2 = x2;
                                    ga1 = a1;
                                    ga2 = a2;
                                }
                                else if(x1 == iterX) {
                                    gx1 = x1;
                                    gx2 = x1 + 1;
                                    ga1 = a1;
                                    ga2 = a2;
                                }
                                else if(x2 == iterX) {
                                    gx1 = x2;
                                    gx2 = x2 + 1;
                                    ga1 = a1;
                                    ga2 = a2;
                                }
                            }
                            ypxli += grad;
                        }
                    }
                    else {
                        for(Pt::int32_t i = from; i <= to; ++i) {
                            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
                            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
                            const Pt::int32_t x  = i;
                            const Pt::int32_t y1 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           );
                            const Pt::int32_t y2 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE);
                            if(x == iterX) {
                                if(y1 == pixelY) {
                                    gx1 = x;
                                    gx2 = x;
                                    ga1 = a1;
                                    ga2 = a1;
                                }
                                else if(y2 == pixelY) {
                                    gx1 = x;
                                    gx2 = x;
                                    ga1 = a2;
                                    ga2 = a2;
                                }
                            }
                            ypxli += grad;
                        }
                    }
                    nodeX[nodes++] = NodeData(gx1, gx2, ga1, ga2);
                    //
                    /*
                    const float       interXf  = curXi + (float) deltaYp / deltaYj * deltaXj;
                    const Pt::int32_t interXf1 = floor(interXf);
                    const Pt::int32_t interXf2 = ceil (interXf);
                    const Pt::uint8_t alpha1   = (interXf  - interXf1) * 255;
                    const Pt::uint8_t alpha2   = (interXf2 - interXf ) * 255;
                    nodeX[nodes++] = NodeData(interXf1, interXf2, alpha1, alpha2);
                    */
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointer
            curPointBase += curPointCount;
        }
        // Skip if there is no node
        if(!nodes) continue;
        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t from1  = nodeX[i    ].x1;
            const Pt::int32_t from2  = nodeX[i    ].x2;
            const Pt::int32_t from1a = nodeX[i    ].a1;
            const Pt::int32_t from2a = nodeX[i    ].a2;
            const Pt::int32_t to1    = nodeX[i + 1].x1;
            const Pt::int32_t to2    = nodeX[i + 1].x2;
            const Pt::int32_t to1a   = nodeX[i + 1].a1;
            const Pt::int32_t to2a   = nodeX[i + 1].a2;

            if(from1 == from2) {
               // fillPixel(from1, pixelY, minX, minY, from1a);
            }
            else {
               // fillPixel(from1, pixelY, minX, minY, from1a);
                fillPixel(from2, pixelY, minX, minY, from2a);
            }

            fillPixel(to1,   pixelY, minX, minY, to1a  );
            //fillPixel(to2,   pixelY, minX, minY, to2a  );

            const Pt::int32_t from   = from2 + 1 - minX;
            const Pt::int32_t to     = to1   - 1 - minX;
            if(to < from) continue;

            rasterScanline(from, to, pixelY - minY, minX, minY, color);
        }
    }
}



















        typedef std::set<Pt::int32_t> ScanlineEdges;
        typedef std::map<Pt::int32_t, ScanlineEdges> PolygonEdges;

        void rasterPolygonAreaNoAAExt(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void fillOnePixelGLineSegmentXWAAExt(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const PolygonEdges& polyEdges, DrawLineMask& maskInOut);

// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaXWAANaive(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    // List of polygon edges
    PolygonEdges polyEdges;

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY) {
        // Base pointer for the polygons
        const Point* curPointBase = points;
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const Pt::int32_t curXi = (curPointBase + i)->x();
                const Pt::int32_t curYi = (curPointBase + i)->y();
                const Pt::int32_t curXj = (curPointBase + j)->x();
                const Pt::int32_t curYj = (curPointBase + j)->y();
                // Calculate the node's coordinate
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + ( FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj );
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointer
            curPointBase += curPointCount;
        }
        // Skip if there is no node
        if(!nodes) continue;
        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t from = nodeX[i    ] + 1;
            const Pt::int32_t to   = nodeX[i + 1];
            if(to < from) continue;

            PolygonEdges::iterator it = polyEdges.find(pixelY);
            if(it != polyEdges.end()) {
                it->second.insert(from);
                it->second.insert(to);
            }
            else {
                ScanlineEdges sle;
                sle.insert(from);
                sle.insert(to);
                polyEdges.insert(make_pair(pixelY, sle));
            }

            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }

    // Raster the anti-aliased outline
    const Point* curPointBase = points;

    for(size_t p = 0; p < polyCount; ++p) {
        // Mask
        Rasterizer2::DrawLineMask mask_zero = Rasterizer2::NullLineMask;
        Rasterizer2::DrawLineMask mask_nnp1 = Rasterizer2::NullLineMask;
        // From point N to point (N + 1), successively
        const size_t pc1 = pointCount[p] - 1;
        for(size_t i = 0; i < pc1; ++i) {
            fillOnePixelGLineSegmentXWAAExt(curPointBase[i].x(), curPointBase[i].y(), curPointBase[i + 1].x(), curPointBase[i + 1].y(), minX, minY, polyEdges, mask_nnp1);
            if(!i) memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
        }
        mask_zero[2] = mask_zero[0];
        mask_zero[3] = mask_zero[1];
        mask_zero[0] = mask_nnp1[2];
        mask_zero[1] = mask_nnp1[3];
        // From the last point to the first point
        fillOnePixelGLineSegmentXWAAExt(curPointBase[pc1].x(), curPointBase[pc1].y(), curPointBase[0].x(), curPointBase[0].y(), minX, minY, polyEdges, mask_zero);
        // Increment the base pointer
        curPointBase += pointCount[p];
    }
}

// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::fillOnePixelGLineSegmentXWAAExt(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const PolygonEdges& polyEdges, DrawLineMask& maskInOut)
{
    // Get the mask's coordinate
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    for(Pt::int32_t i = 0; i < 4; ++i) {
        mx[i] = maskInOut[i].x();
        my[i] = maskInOut[i].y();
    }

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // A helper macro to fill pixel
    #define XW_FILL_PIXEL(IMG, X, Y, A)                                                     \
        do {                                                                                \
            /* Check the boundary limit, just in case */                                    \
            if( (X) < 0 || (X) >= IMG->width() || (Y) < 0 || (Y) >= IMG->height() ) break;  \
            /* Check if we should skip drawing the pixel */                                 \
            bool skipDrawing = false;                                                       \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                            \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                                \
                skipDrawing = true;                                                         \
                break;                                                                      \
            }                                                                               \
            if(skipDrawing) break;                                                          \
            /* Fill the pixel */                                                            \
            if(_isTexture || _isGradient) {                                                 \
                const Pt::int32_t bw = _brushImage->width();                                \
                const Pt::int32_t bh = _brushImage->height();                               \
                ConstPixel srcPixel(_brushImage->view(), (X - minX) % bw, (Y - minY) % bh); \
                Pixel      dstPixel(IMG->view(), X, Y);                                     \
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, A);         \
            }                                                                               \
            else {                                                                          \
                Pixel pixel(IMG->view(), X, Y);                                             \
                _image->format().setPixel(pixel, _brush.color(), _compositionMode, A);      \
            }                                                                               \
        } while(false)

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool        steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    if(fx1 > fx2) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const Pt::int32_t grad = (fy2 - fy1) / FIXED_POINT_TO_INT(fx2 - fx1);
    const Pt::int32_t xpxl1 = FIXED_POINT_ROUND(fx1);
    const Pt::int32_t xpxl2 = FIXED_POINT_ROUND(fx2);
    const Pt::int32_t ypxl  = fy1 + grad * FIXED_POINT_TO_INT(xpxl1 - fx1);

    // Draw the pixels
    Pt::int32_t from  = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
    Pt::int32_t to    = FIXED_POINT_TO_INT(xpxl2);
    Pt::int32_t ypxli = ypxl;

    if(steep) {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the alphas and coordinates
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            const Pt::int32_t x1 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           );
            const Pt::int32_t x2 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE);
            const Pt::int32_t y  = i;
            ypxli += grad;
            // Draw the pixels as needed
            PolygonEdges::const_iterator it = polyEdges.find(y);
            if(it != polyEdges.end()) {
                if(it->second.find(x1) == it->second.end()) XW_FILL_PIXEL(_image, x1, y, a1);
                if(it->second.find(x2) == it->second.end()) XW_FILL_PIXEL(_image, x2, y, a2);
            }
            else {
                XW_FILL_PIXEL(_image, x1, y, a1);
                XW_FILL_PIXEL(_image, x2, y, a2);
            }
        }
        // Store back the start and end coordinates to the mask as needed
        maskInOut[0].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ), from);
        maskInOut[1].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE), from);
        maskInOut[2].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ), to  );
        maskInOut[3].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE), to  );
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the alphas and coordinates
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            const Pt::int32_t x  = i;
            const Pt::int32_t y1 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           );
            const Pt::int32_t y2 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE);
            ypxli += grad;
            // Draw the pixels as needed
            PolygonEdges::const_iterator it1 = polyEdges.find(y1);
            PolygonEdges::const_iterator it2 = polyEdges.find(y2);
            if(it1 != polyEdges.end() && it1->second.find(x) == it1->second.end()) XW_FILL_PIXEL(_image, x, y1, a1);
            if(it2 != polyEdges.end() && it2->second.find(x) == it2->second.end()) XW_FILL_PIXEL(_image, x, y2, a2);
        }
        // Store back the start and end coordinates to the mask as needed
        maskInOut[0].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ));
        maskInOut[1].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE));
        maskInOut[2].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ));
        maskInOut[3].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE));
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}
