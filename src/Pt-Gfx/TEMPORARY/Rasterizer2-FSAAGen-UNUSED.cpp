// Fixed-Point 16.16 Settings
#define FIXED_POINT_SHIFT_FACTOR     16         // Shift factor
#define FIXED_POINT_FRACT_BITMASK    0x0000FFFF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
#define FIXED_POINT_CONSTANT_ONE     65536      // The value 1.0  in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR    )
#define FIXED_POINT_CONSTANT_HALF    32768      // The value 0.5  in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 2)
#define FIXED_POINT_CONSTANT_QUARTER 16384      // The value 0.25 in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 4)

// Fixed-Point 16.16 Helper Macros
#define FIXED_POINT_IPART(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_FPART(V)        ( (V) &  FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_RFPART(V)       ( FIXED_POINT_FRACT_BITMASK - FIXED_POINT_FPART(V) )
#define FIXED_POINT_ROUND(V)        ( FIXED_POINT_IPART( (V) + FIXED_POINT_CONSTANT_HALF ) )
#define FIXED_POINT_FPART_TO_A8(V)  ( FIXED_POINT_FPART (V) >> 8 )
#define FIXED_POINT_RFPART_TO_A8(V) ( FIXED_POINT_RFPART(V) >> 8 )
#define FIXED_POINT_MUL_TO_A8(A, B) ( ( ( (Pt::uint32_t)(A) * (Pt::uint32_t)(B) + FIXED_POINT_FRACT_BITMASK ) >> FIXED_POINT_SHIFT_FACTOR ) )
#define FIXED_POINT_FROM_INT(V)     ( (V) << FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_TO_INT(V)       ( (V) >> FIXED_POINT_SHIFT_FACTOR )


// Polygon scanline structure (used for drawing filled polygons with XWAA)
struct Rasterizer2::PolygonScanline16 {
    Pt::int16_t from, to;

    PolygonScanline16(Pt::int16_t from_, Pt::int16_t to_)
    : from(from_), to(to_)
    {}
};







        template<Pt::uint8_t SUPERSAMPLE_SIZE>
        void rasterPolygonAreaFSAAGen(
            const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount,
            const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY
        );


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
template<Pt::uint8_t SUPERSAMPLE_SIZE>
void Rasterizer2::rasterPolygonAreaFSAAGen(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Internal macros
    #define FSAA_MUL_ALPHA 255
    #define FSAA_MIN_ALPHA 1
    #define FSAA_MAX_ALPHA (FSAA_MIN_ALPHA * SUPERSAMPLE_SIZE * SUPERSAMPLE_SIZE)
    #define FSAA_MID_ALPHA (FSAA_MIN_ALPHA * SUPERSAMPLE_SIZE)

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1);

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to be twice as large and translate its origin to (0, 0)
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * SUPERSAMPLE_SIZE;
        pointY[i] = (points[i].y() - minY) * SUPERSAMPLE_SIZE;
    }

    // List of nodes that define the horizontal spans
    // Row (Y) ... Row (Y + SUPERSAMPLE_SIZE - 1)
    std::vector<Pt::int32_t> nodeX[SUPERSAMPLE_SIZE];
    for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
        nodeX[s].resize(totalPointCount * 2);
    }

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY <= sizeY; ++pixelY) {
        // We examine multiple rows at a time
        Pt::int32_t iterY[SUPERSAMPLE_SIZE];
        iterY[0] = pixelY * SUPERSAMPLE_SIZE;
        for(Pt::int32_t s = 1; s < SUPERSAMPLE_SIZE; ++s) {
            iterY[s] = iterY[0] + s;
        }
        // Base pointers for the polygons
        const Pt::int32_t* curPointBaseX = pointX.data();
        const Pt::int32_t* curPointBaseY = pointY.data();
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes[SUPERSAMPLE_SIZE] = { 0 };
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
                // Row (Y) ... Row (Y + SUPERSAMPLE_SIZE - 1)
                for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                    if( ( iterY[s] >= curYi && iterY[s] < curYj ) || ( iterY[s] >= curYj && iterY[s] < curYi ) ) {
                        // Bail out if we have produced too many nodes
                        if((size_t) nodes[s] >= nodeX[s].size()) return;
                        // Calculate the nodes' coordinates
                        const Pt::int32_t deltaYp = iterY[s] - curYi;
                        const Pt::int32_t deltaYj = curYj    - curYi;
                        const Pt::int32_t deltaXj = curXj    - curXi;
                        const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                                  + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_QUARTER) /
                                                      deltaYj * deltaXj
                                                    );
                        nodeX[s][nodes[s]++] = FIXED_POINT_TO_INT(interXf);
                    }
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointers
            curPointBaseX += curPointCount;
            curPointBaseY += curPointCount;
        }
        // Skip if there is no node
        bool gotNodes = false;
        for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
            if(nodes[s]) {
                gotNodes = true;
                break;
            }
        }
        if(!gotNodes) continue;
        // Sort the nodes using bubble sort
        for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
            bubbleSortAscending(nodeX[s], nodes[s]);
        }
        // Reset the alphas
        memset(&alphas[0], 0, alphas.size());
#if 1
        // Accumulate the alphas of the samples between the node pairs
        for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
            for(Pt::int32_t i = 0; i < nodes[0]; i += 2) {

                // Calculate the cells and coverage areas
                const Pt::int32_t from      = nodeX[s][i    ];
                const Pt::int32_t to        = nodeX[s][i + 1];
                const Pt::int32_t from_cell = from / SUPERSAMPLE_SIZE;
                const Pt::int32_t to_cell   = to   / SUPERSAMPLE_SIZE;
                const Pt::int32_t from_area = SUPERSAMPLE_SIZE - ( from - from_cell * SUPERSAMPLE_SIZE );
                const Pt::int32_t to_area   = ( to - to_cell * SUPERSAMPLE_SIZE ) + 1;
                // If the span is short, accumulate alphas for the whole span directly
                // in order to avoid some alpha-related artifacts
                if( to - from <= SUPERSAMPLE_SIZE ) {
                    for(Pt::int32_t k = from; k <= to; ++k) {
                        alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                    }
                    continue;
                }
                // Accumulate alphas for the left side and right side of the span
                alphas[from_cell] += from_area;
                alphas[to_cell  ] += to_area;
                // Assign alphas for the middle side of the span
                for(Pt::int32_t k = (from_cell + 1); k <= (to_cell - 1); ++k) {
                    alphas[k] += FSAA_MID_ALPHA;
                }
            }
        }
#else
        // Accumulate the alphas of the samples between the node pairs
        // --- Check if all the rows have the same number of nodes ---
        const Pt::int32_t nodes0            = nodes[0];
              bool        hasSameNumOfNodes = true;
        for(Pt::int32_t s = 1; s < SUPERSAMPLE_SIZE; ++s) {
            if(nodes[s] == nodes0) continue;
            hasSameNumOfNodes = false;
            break;
        }
        // --- The number of nodes within all the rows are equal ---
        if(hasSameNumOfNodes) {
            for(Pt::int32_t i = 0; i < nodes0; i += 2) {
                // Get the coordinates
                Pt::int32_t from[SUPERSAMPLE_SIZE];
                Pt::int32_t to  [SUPERSAMPLE_SIZE];
                for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                    from[s] = nodeX[s][i    ];
                    to  [s] = nodeX[s][i + 1];
                }
                // Sort the coordinates
                bubbleSortAscending(from, SUPERSAMPLE_SIZE);
                bubbleSortAscending(to,   SUPERSAMPLE_SIZE);
                // Calculate the cells
                Pt::int32_t from_cell[SUPERSAMPLE_SIZE];
                Pt::int32_t to_cell  [SUPERSAMPLE_SIZE];
                bool        shortSpan = false;
                for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                    from_cell[s] = from[s] / SUPERSAMPLE_SIZE;
                    to_cell  [s] = to  [s] / SUPERSAMPLE_SIZE;
                    if(abs(to_cell[s] - from_cell[s]) <= SUPERSAMPLE_SIZE) shortSpan = true;
                }
                // If the span is short, accumulate alphas for the whole span directly
                // in order to avoid some alpha-related artifacts
                if(shortSpan) {
                    for(Pt::int32_t n = 0; n < SUPERSAMPLE_SIZE; ++n) {
                        for(Pt::int32_t k = from[n]; k <= to[n]; ++k) {
                            alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                        }
                    }
                    continue;
                }
                // Accumulate alphas for the left side of the span
                // --- Each distinct cells ---
                std::set<Pt::int32_t> proc_ds;
                for(Pt::int32_t n = 0; n < SUPERSAMPLE_SIZE; ++n) {
                    // Alpha contribution from this cell
                    alphas[ from_cell[n] ] += SUPERSAMPLE_SIZE - ( from[n] - from_cell[n] * SUPERSAMPLE_SIZE );
                    // Alpha Contribution from cells of the left side of this cell
                    for(Pt::int32_t b = n; b > 0; --b) {
                        // Ensure that each cell is not processed more than once
                        if(proc_ds.find(from_cell[b]) != proc_ds.end()) continue;
                        proc_ds.insert(from_cell[b]);
                        // Accumulate the alphas
                        for(Pt::int32_t k = 0; k < b; ++k) {
                            // Accumulate the alpha if the cell is on the left side of the reference cell
                            if(from_cell[k] < from_cell[b]) alphas[ from_cell[b] ] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // --- In-between cells ---
                std::set<Pt::int32_t> proc_is;
                for(Pt::int32_t n = (SUPERSAMPLE_SIZE - 1); n > 0; --n) {
                    // Ensure that each reference cell is not processed more than once
                    if(proc_is.find(from_cell[n]) != proc_is.end()) continue;
                    proc_is.insert(from_cell[n]);
                    // Walk through the cells on the left side of the span up until the reference cell
                    for(Pt::int32_t b = 0; b < n; ++b) {
                        // Skip if this cell is not really on the left side of the reference cell
                        if(from_cell[b] >= from_cell[n]) continue;
                        // Walk through the in-between cells
                        for(Pt::int32_t k = (from_cell[b] + 1); k < from_cell[n]; ++k) {
                            // Accumulate the alpha only if the cell is not one of the reference cell
                            if(proc_ds.find(k) == proc_ds.end()) alphas[k] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // Accumulate alphas for the right side of the span
                // --- Each distinct cells ---
                proc_ds.clear();
                for(Pt::int32_t n = 0; n < SUPERSAMPLE_SIZE; ++n) {
                    // Alpha contribution from this cell
                    alphas[ to_cell[n] ] += ( to[n] - to_cell[n] * SUPERSAMPLE_SIZE ) + 1;
                    // Alpha Contribution from cells of the right side of this cell
                    for(Pt::int32_t b = n; b < (SUPERSAMPLE_SIZE - 1); ++b) {
                        // Ensure that each cell is not processed more than once
                        if(proc_ds.find(to_cell[b]) != proc_ds.end()) continue;
                        proc_ds.insert(to_cell[b]);
                        // Accumulate the alphas
                        for(Pt::int32_t k = (b + 1); k < SUPERSAMPLE_SIZE; ++k) {
                            // Accumulate the alpha if the cell is on the right side of the reference cell
                            if(to_cell[k] > to_cell[b]) alphas[ to_cell[b] ] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // --- In-between cells ---
                proc_is.clear();
                for(Pt::int32_t n = 0; n < (SUPERSAMPLE_SIZE - 1); ++n) {
                    // Ensure that each reference cell is not processed more than once
                    if(proc_is.find(to_cell[n]) != proc_is.end()) continue;
                    proc_is.insert(to_cell[n]);
                    // Walk through the cells on the right side of the span down until the reference cell
                    for(Pt::int32_t b = (SUPERSAMPLE_SIZE - 1); b > n; --b) {
                        // Skip if this cell is not really on the right side of the reference cell
                        if(to_cell[b] <= to_cell[n]) continue;
                        // Walk through the in-between cells
                        for(Pt::int32_t k = (to_cell[b] - 1); k > to_cell[n]; --k) {
                            // Accumulate the alpha only if the cell is not one of the reference cell
                            if(proc_ds.find(k) == proc_ds.end()) alphas[k] += SUPERSAMPLE_SIZE;
                        }
                    }
                }
                // Assign alphas for the middle side of the span
                const Pt::int32_t msMin = (from_cell[SUPERSAMPLE_SIZE - 1] + 1);
                const Pt::int32_t msMax = (to_cell  [0                   ] - 1);
                const Pt::int32_t msLen = msMax - msMin + 1;
                if(msLen > 0) memset(&alphas[msMin], FSAA_MAX_ALPHA, msLen);
            }
        }
        // Accumulate the alphas of the samples between the node pairs
        // --- The number of nodes within all or some of the rows are not equal ---
        else {
            for(Pt::int32_t s = 0; s < SUPERSAMPLE_SIZE; ++s) {
                //for(Pt::int32_t i = 0; i < nodes[s]; i += 2) {
                //    const Pt::int32_t from = nodeX[s][i    ];
                //    const Pt::int32_t to   = nodeX[s][i + 1];
                //    for(Pt::int32_t k = from; k <= to; ++k) {
                //        alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                //    }
                //}
                for(Pt::int32_t i = 0; i < nodes0; i += 2) {

                    // Calculate the cells and coverage areas
                    const Pt::int32_t from      = nodeX[s][i    ];
                    const Pt::int32_t to        = nodeX[s][i + 1];
                    const Pt::int32_t from_cell = from / SUPERSAMPLE_SIZE;
                    const Pt::int32_t to_cell   = to   / SUPERSAMPLE_SIZE;
                    const Pt::int32_t from_area = SUPERSAMPLE_SIZE - ( from - from_cell * SUPERSAMPLE_SIZE );
                    const Pt::int32_t to_area   = ( to - to_cell * SUPERSAMPLE_SIZE ) + 1;
                    // If the span is short, accumulate alphas for the whole span directly
                    // in order to avoid some alpha-related artifacts
                    if( to - from <= SUPERSAMPLE_SIZE ) {
                        for(Pt::int32_t k = from; k <= to; ++k) {
                            alphas[k / SUPERSAMPLE_SIZE] += FSAA_MIN_ALPHA;
                        }
                        continue;
                    }
                    // Accumulate alphas for the left side and right side of the span
                    alphas[from_cell] += from_area;
                    alphas[to_cell  ] += to_area;
                    // Assign alphas for the middle side of the span
                    for(Pt::int32_t k = (from_cell + 1); k <= (to_cell - 1); ++k) {
                        alphas[k] += FSAA_MID_ALPHA;
                    }
                }
            }
        }
#endif
        //lprintf("%03d: ", pixelY); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%02d ", alphas[k] / FSAA_MIN_ALPHA); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes[0]; i += 2) {
            const Pt::int32_t iterL = nodeX[0][i    ] / SUPERSAMPLE_SIZE - 1;
            const Pt::int32_t iterR = nodeX[0][i + 1] / SUPERSAMPLE_SIZE + 1;
            rasterScanline<SUPERSAMPLE_SIZE, FSAA_MIN_ALPHA, FSAA_MUL_ALPHA>(
                iterL, iterR, pixelY, minX, minY, sizeX, color, alphas
            );
        }
    }

    // Undefine the macros
    #undef FSAA_MUL_ALPHA
    #undef FSAA_MIN_ALPHA
    #undef FSAA_MAX_ALPHA
    #undef FSAA_MID_ALPHA
}

