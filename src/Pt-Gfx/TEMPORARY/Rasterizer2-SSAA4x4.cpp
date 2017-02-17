void rasterPolygonAreaSSAA4x4(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY);


// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaSSAA4x4(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Internal macros
    #define SSAA4X4_SUPERSAMPLE_SIZE 4
    #define SSAA4X4_MUL_ALPHA        17
    #define SSAA4X4_MIN_ALPHA        15

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1) * SSAA4X4_SUPERSAMPLE_SIZE;

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to be four times as large and translate its origin to (0, 0)
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * SSAA4X4_SUPERSAMPLE_SIZE;
        pointY[i] = (points[i].y() - minY) * SSAA4X4_SUPERSAMPLE_SIZE;
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // Base pointers for the polygons
        const Pt::int32_t* curPointBaseX = pointX.data();
        const Pt::int32_t* curPointBaseY = pointY.data();
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const Pt::int32_t curXi = *(curPointBaseX + i);
                const Pt::int32_t curYi = *(curPointBaseY + i);
                const Pt::int32_t curXj = *(curPointBaseX + j);
                const Pt::int32_t curYj = *(curPointBaseY + j);
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    // Calculate the node's coordinate
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_QUARTER) /
                                                  deltaYj * deltaXj
                                                );
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointers
            curPointBaseX += curPointCount;
            curPointBaseY += curPointCount;
        }
        // Skip if there is no node
        if(!nodes) continue;
        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);
        // Accumulate the alphas of the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t from = nodeX[i    ];
            const Pt::int32_t to   = nodeX[i + 1];
            // Use Duff's device for:
            //     for(Pt::int32_t k = from; k <= to; ++k) {
            //         alphas[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA;
            //     }
            register Pt::uint8_t* dst = &alphas[0];
            register Pt::int32_t  cnt = to - from + 1;
            register Pt::int32_t  n   = (cnt + 7) / 8;
            register Pt::int32_t  k   = from;
            switch(cnt % 8) {
                    case 0 : do { dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 7 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 6 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 5 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 4 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 3 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 2 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 1 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                             } while (--n > 0);
            }
        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SSAA4X4_SUPERSAMPLE_SIZE) ) continue;
        //lprintf("%03d: ", pixelY / SSAA4X4_SUPERSAMPLE_SIZE); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%02d ", alphas[k] / SSAA4X4_MIN_ALPHA); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t iterL = nodeX[i    ] / SSAA4X4_SUPERSAMPLE_SIZE - 1;
            const Pt::int32_t iterR = nodeX[i + 1] / SSAA4X4_SUPERSAMPLE_SIZE + 1;
            rasterScanline<SSAA4X4_SUPERSAMPLE_SIZE, SSAA4X4_MIN_ALPHA, SSAA4X4_MUL_ALPHA>(
                iterL, iterR, pixelY / SSAA4X4_SUPERSAMPLE_SIZE, minX, minY, sizeX, color, alphas
            );
        }
        // Clear the work buffer
        memset(&alphas[0], 0, alphas.size());
    }

    // Undefine the macros
    #undef SSAA4X4_SUPERSAMPLE_SIZE
    #undef SSAA4X4_MUL_ALPHA
    #undef SSAA4X4_MIN_ALPHA
}
