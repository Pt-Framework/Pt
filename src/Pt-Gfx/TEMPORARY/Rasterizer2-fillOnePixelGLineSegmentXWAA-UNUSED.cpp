void fillOnePixelGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const std::map<Pt::int32_t, Pt::int32_t>* exclusionZone, DrawLineMask* maskInOut);


// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::fillOnePixelGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const std::map<Pt::int32_t, Pt::int32_t>* exclusionZone, DrawLineMask* maskInOut)
{
    // Get the mask's coordinate as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // A helper macro to fill pixel
    #define XW_FILL_PIXEL(IMG, X, Y, A)                                                         \
        do {                                                                                    \
            /* Check the boundary limit, just in case */                                        \
            if( (X) < 0 || (X) >= IMG->width() || (Y) < 0 || (Y) >= IMG->height() ) break;      \
            /* Check if we should skip drawing the pixel */                                     \
            bool skipDrawing = false;                                                           \
            if(exclusionZone) {                                                                 \
                std::map<Pt::int32_t, Pt::int32_t>::const_iterator xt = exclusionZone->find(Y); \
                skipDrawing = ( xt != exclusionZone->end() ) && ( xt->second == X );            \
            }                                                                                   \
            for(Pt::int32_t j = 0; !skipDrawing && j < 4; ++j) {                                \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                                    \
                skipDrawing = true;                                                             \
                break;                                                                          \
            }                                                                                   \
            if(skipDrawing) break;                                                              \
            /* Fill the pixel */                                                                \
            if(_isTexture || _isGradient) {                                                     \
                const Pt::int32_t bw = _brushImage->width();                                    \
                const Pt::int32_t bh = _brushImage->height();                                   \
                ConstPixel srcPixel(_brushImage->view(), (X - minX) % bw, (Y - minY) % bh);     \
                Pixel      dstPixel(IMG->view(), X, Y);                                         \
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, A);             \
            }                                                                                   \
            else {                                                                              \
                Pixel pixel(IMG->view(), X, Y);                                                 \
                _image->format().setPixel(pixel, _brush.color(), _compositionMode, A);          \
            }                                                                                   \
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
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), i, a1);
            XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), i, a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ), from);
            (*maskInOut)[1].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE), from);
            (*maskInOut)[2].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ), to  );
            (*maskInOut)[3].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE), to  );
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a1);
            XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ));
            (*maskInOut)[1].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE));
            (*maskInOut)[2].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ));
            (*maskInOut)[3].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE));
        }
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}
#endif













//
// XWAA for polygon -> FAIL!
//





        void rasterPolygonAreaNoAAInsideOnly(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);
        void fillOnePixelSolidPolygonOutlineOutsideAAOnly(const Point* points, size_t pointCount, Pt::int32_t minX, Pt::int32_t minY);
        void fillOnePixelGLineSegmentXWAAOutsideOnly(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, DrawLineMask* maskInOut);

// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::fillOnePixelGLineSegmentXWAAOutsideOnly(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, DrawLineMask* maskInOut)
{
    // Get the mask's coordinate as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Calculate the direction vector
    const Pt::int32_t vx = x1 - x2;           // Vector from the end point to the begin point
    const Pt::int32_t vy = y1 - y2;           // ---
    const Pt::int32_t vz = 0;                 // ---
    const Pt::int32_t rx = 0;                 // Vector from the point of origin (0, 0, 0) that points out of the monitor
    const Pt::int32_t ry = 0;                 // ---
    const Pt::int32_t rz = 1;                 // ---
    const Pt::int32_t cx = vy * rz - vz * ry; // Cross product of the above vectors
    const Pt::int32_t cy = vz * rx - vx * rz; // ---
  //const Pt::int32_t cz = vx * ry - vy * rx; // ---

    // Determine where the direction that the line faces to
    const bool faceT = cy < 0;
    const bool faceB = cy > 0;
    const bool faceL = cx < 0;
    const bool faceR = cx > 0;

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
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];

            // TL
           // XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), i, a1);

            // BR
            //XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), i, a2);

            //lif(faceL) XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), i, a1);

            //if(faceR) XW_FILL_PIXEL(_image, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), i, a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ), from);
            (*maskInOut)[1].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE), from);
            (*maskInOut)[2].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ), to  );
            (*maskInOut)[3].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE), to  );
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            // TR
            //XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a2);
            XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a1);

            // Botttom
            //XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);

            //XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);

            //lif(faceR) XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);

//            XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a1);
            //if(faceT) XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a1);
            //if(faceB) XW_FILL_PIXEL(_image, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        )                           ));
            (*maskInOut)[1].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl        ) + FIXED_POINT_CONSTANT_ONE));
            (*maskInOut)[2].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad)                           ));
            (*maskInOut)[3].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli - grad) + FIXED_POINT_CONSTANT_ONE));
        }
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}



// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaNoAAInsideOnly(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

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
            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }
}

void Rasterizer2::fillOnePixelSolidPolygonOutlineOutsideAAOnly(const Point* points, size_t pointCount, Pt::int32_t minX, Pt::int32_t minY)
{
    // Mask
    Rasterizer2::DrawLineMask mask_zero = Rasterizer2::NullLineMask;
    Rasterizer2::DrawLineMask mask_nnp1 = Rasterizer2::NullLineMask;

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        fillOnePixelGLineSegmentXWAAOutsideOnly(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), minX, minY, &mask_nnp1);
        if(!i) memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
    }

    mask_zero[2] = mask_zero[0];
    mask_zero[3] = mask_zero[1];
    mask_zero[0] = mask_nnp1[2];
    mask_zero[1] = mask_nnp1[3];

    // From the last point to the first point
    fillOnePixelGLineSegmentXWAAOutsideOnly(points[pc1].x(), points[pc1].y(), points[0].x(), points[0].y(), minX, minY, &mask_zero);

    // From the first point to the last point
    //fillOnePixelGLineSegmentXWAAOutsideOnly(points[0].x(), points[0].y(), points[pc1].x(), points[pc1].y(), minX, minY, &mask_zero);
}


#if 1

    // Without anti-aliasing
    if(_aaMode == AntiAliasingMode::None) {
        rasterPolygonAreaNoAA(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
        return;
    }

    // With anti-aliasing
    rasterPolygonAreaNoAAInsideOnly(
        clippedPoints.data(), clippedCounts.data(),
        clippedCounts.size(), clippedPoints.size(),
        _brush.color(), minX, minY, maxX, maxY
    );

    // Raster the anti-aliased outline
    const Point* curPointBase = clippedPoints.data();
    for(size_t p = 0; p < clippedCounts.size(); ++p) {
        fillOnePixelSolidPolygonOutlineOutsideAAOnly(curPointBase, clippedCounts[p], minX, minY);
        curPointBase += clippedCounts[p];
    }

#else
#endif
