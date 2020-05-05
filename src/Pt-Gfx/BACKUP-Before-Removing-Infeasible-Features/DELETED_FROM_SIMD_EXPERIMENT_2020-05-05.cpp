// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonXWAA(const PointF* points, std::size_t pointCount,
                                    const Color& color,
                                    Pt::int32_t minX, Pt::int32_t minY,
                                    Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<float>& nodeX = _polygonNodeX;
    nodeX.resize(pointCount * 2);

    // List of polygon scanlines
    PolygonScanlines& scanlines = _polygonScanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

#ifdef WITH_EXPERIMENTAL_GFX

    auto lambdaWorker = [&](Pt::int32_t y1, Pt::int32_t y2) {
        size_t tot = 0;

        // Loop through the rows of the image
        for(Pt::int32_t y = y1; y <= y2; ++y)
        {
            // Pixel-by-pixel clipping
            if(y < _currentClip.top   ()) continue;
            if(y > _currentClip.bottom()) continue;

            // Build a list of nodes using the coordinates from the polygon
            std::size_t nodes = 0;

#if 0

            // Loop through the points
            for(size_t j = 0; j < pointCount; ++j)
            {
                //++tot;
                // Calculate the i
                const size_t i = ( j >= (pointCount - 1) ) ? 0 : (j + 1);

                #if 0
                if(y == y2 - 10) {
                    printf("j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)\n",
                        j, i,
                        points[j].x(), points[j].y(),
                        points[i].x(), points[i].y()
                    );
                }
                #endif

                // Get the Y coordinates
                const float curYi = points[i].y();
                const float curYj = points[j].y();

                // Check againts the Y coordinates
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if( nodes >= nodeX.size() )
                        return;

                    #if 0
                    if(y == y2 - 10) {
                        printf("IN_Y   j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)\n",
                            j, i,
                            points[j].x(), points[j].y(),
                            points[i].x(), points[i].y()
                        );
                    }
                    #endif

                    // Get the X coordinates
                    const float curXi = points[i].x();
                    const float curXj = points[j].x();

                    // Calculate the node's coordinate
                    const float deltaYp = y     - curYi;
                    const float deltaYj = curYj - curYi;
                    const float deltaXj = curXj - curXi;
                    const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                    nodeX[nodes++] = interXf;

                    #if 0
                    if(y == y2 - 10) {
                        printf("IN_X   j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)   ->   %05.1f   [%03zd]\n",
                            j, i,
                            points[j].x(), points[j].y(),
                            points[i].x(), points[i].y(),
                            interXf, nodes
                        );
                    }
                    #endif
                }
            }

#else

            // Loop through the points
            /*
            // Scalar
            //    000000000011111111112222222222333 PointCount = 33
            //    012345678901234567890123456789012
            //  0 ji
            //  1  ji
            //  2   ji
            //  3    ji
            //  4     ji
            //  5      ji
            //  6       ji
            //  7        ji
            //  8         ji
            //  9          ji
            // 10           ji
            // 11            ji
            // 12             ji
            // 13              ji
            // 14               ji
            // 15                ji
            // 16                 ji
            // 17                  ji
            // 18                   ji
            // 19                    ji
            // 20                     ji
            // 21                      ji
            // 22                       ji
            // 23                        ji
            // 24                         ji
            // 25                          ji
            // 26                           ji
            // 27                            ji
            // 28                             ji
            // 29                              ji
            // 30                               ji
            // 31                                ji
            // 32 i                               j
            //    000000000011111111112222222222333
            //    012345678901234567890123456789012
            //
            //
            // VectorSize = 4
            //    000000000011111111112222222222333 PointCount = 33   Loop     = PointCount / VecSize / 2 = 4
            //    012345678901234567890123456789012                   LoopEnd  = Loop * VecSize * 2       = 32
            // 0a jijijiji                                            LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijiji                                           RemStart = Loop * VecSize * 2       = 32
            // 1a         jijijiji                                    RemEnd   = PointCount - 1           = 32
            // 1b          jijijiji
            // 2a                 jijijiji
            // 2b                  jijijiji
            // 3a                         jijijiji
            // 3b                          jijijiji
            // R0 i                               j
            //    000000000011111111112222222222333
            //    012345678901234567890123456789012
            //
            // VectorSize = 4
            //    00000000001111111111222222222233  PointCount = 32   Loop     = PointCount / VecSize / 2  = 4
            //    01234567890123456789012345678901                    LoopEnd  = Loop * VecSize * 2        = 32
            // 0a jijijiji                                            LastB    = (LoopEnd < PointCount)    = false
            // 0b  jijijiji                                           RemStart = LoopEnd - VecSize * 2 + 1 = 25
            // 1a         jijijiji                                    RemEnd   = PointCount - 1            = 31
            // 1b          jijijiji
            // 2a                 jijijiji
            // 2b                  jijijiji
            // 3a                         jijijiji
            // R0                          ji
            // R1                           ji
            // R2                            ji
            // R3                             ji
            // R4                              ji
            // R5                               ji
            // R6 i                              j
            //    00000000001111111111222222222233
            //    01234567890123456789012345678901
            //
            // VectorSize = 4
            //    000000000011111111112             PointCount = 21   Loop     = PointCount / VecSize / 2 = 2
            //    012345678901234567890                               LoopEnd  = Loop * VecSize * 2       = 16
            // 0a jijijiji                                            LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijiji                                           RemStart = Loop * VecSize * 2       = 16
            // 0a         jijijiji                                    RemEnd   = PointCount - 1           = 20
            // 0b          jijijiji
            // R0                 ji
            // R1                  ji
            // R2                   ji
            // R3                    ji
            // R4 i                   j
            //    000000000011111111112
            //    012345678901234567890
            //
            //
            // VectorSize = 8
            //    000000000011111111112222222222333 PointCount = 33   Loop     = PointCount / VecSize / 2 =  2
            //    012345678901234567890123456789012                   LoopEnd  = Loop * VecSize * 2       = 32
            // 0a jijijijijijijiji                                    LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijijijijijiji                                   RemStart = Loop * VecSize * 2       = 32
            // 1a                 jijijijijijijiji                    RemEnd   = PointCount - 1           = 32
            // 1b                  jijijijijijijiji
            // R0 i                               j
            //    000000000011111111112222222222333
            //    012345678901234567890123456789012
            //
            // VectorSize = 8
            //    00000000001111111111222222222233  PointCount = 32   Loop     = PointCount / VecSize / 2  =  2
            //    01234567890123456789012345678901                    LoopEnd  = Loop * VecSize * 2        = 32
            // 0a jijijijijijijiji                                    LastB    = (LoopEnd < PointCount)    = false
            // 0b  jijijijijijijiji                                   RemStart = LoopEnd - VecSize * 2 + 1 = 17
            // 1a                 jijijijijijijiji                    RemEnd   = PointCount - 1            = 31
            // R0                  ji
            // R1                   ji
            // R2                    ji
            // R3                     ji
            // R4                      ji
            // R5                       ji
            // R6                        ji
            // R7                         ji
            // R8                          ji
            // R9                           ji
            // RA                            ji
            // RB                             ji
            // RC                              ji
            // RD                               ji
            // RE i                              j
            //    00000000001111111111222222222233
            //    01234567890123456789012345678901
            //
            // VectorSize = 8
            //    000000000011111111112             PointCount = 21   Loop     = PointCount / VecSize / 2 =  1
            //    012345678901234567890                               LoopEnd  = Loop * VecSize * 2       = 16
            // 0a jijijijijijijiji                                    LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijijijijijiji                                   RemStart = Loop * VecSize * 2       = 16
            // R0                 ji                                  RemEnd   = PointCount - 1           = 20
            // R1                  ji
            // R2                   ji
            // R3                    ji
            // R4 i                   j
            //    000000000011111111112
            //    012345678901234567890
            */

            // Calculate the number of loop that can be calculated using SIMD and the number of remaining loop
            const size_t vecSize  = 4;

            const size_t loop     = pointCount / vecSize / 2;
            const size_t loopEnd  = loop * vecSize * 2;
            const bool   lastB    = (loopEnd < pointCount);

            const size_t remStart = lastB ? loopEnd : (loopEnd - vecSize * 2 + 1);
            const size_t remEnd   = pointCount - 1;

            // Make a vector Y for the current scanline
            //const xsimd::batch<float, 4> curY( y );

        #if 1
            // Loop as many as the number of loop that can be calculated using SIMD operations
            for(size_t j = 0; j < loop; ++j) {

                // Determine if the second SIMD part needs to be executed
                const size_t bOne = ( lastB || ( j < (loop - 1) ) ) ? 1 : 0;

                for(size_t b = 0; b <= bOne; ++b) {
                    tot += vecSize;

                    // Calculate the real position within the array of points
                    const size_t pj = j * vecSize * 2 + b;
                    const size_t pi = pj + 1;
                  //if(pi >= (pointCount - 1)) pi = 0;

                    #if 0
                    if(y == y2 - 10) {
                        printf("j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)\n",
                            pj + 0, pi + 0,
                            points[pj + 0].x(), points[pj + 0].y(),
                            points[pi + 0].x(), points[pi + 0].y()
                        );
                        printf("j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)\n",
                            pj + 2, pi + 2,
                            points[pj + 2].x(), points[pj + 2].y(),
                            points[pi + 2].x(), points[pi + 2].y()
                        );
                        printf("j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)\n",
                            pj + 4, pi + 4,
                            points[pj + 4].x(), points[pj + 4].y(),
                            points[pi + 4].x(), points[pi + 4].y()
                        );
                        printf("j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)\n",
                            pj + 6, pi + 6,
                            points[pj + 6].x(), points[pj + 6].y(),
                            points[pi + 6].x(), points[pi + 6].y()
                        );
                    }
                    #endif

                    // Get the Y coordinates
                    /*
                    const xsimd::batch<float, 4> curYi( points[pi + 0].y(),
                                                        points[pi + 2].y(),
                                                        points[pi + 4].y(),
                                                        points[pi + 6].y() );

                    const xsimd::batch<float, 4> curYj( points[pj + 0].y(),
                                                        points[pj + 2].y(),
                                                        points[pj + 4].y(),
                                                        points[pj + 6].y() );

                    // Get the X coordinates
                    const xsimd::batch<float, 4> curXi( points[pi + 0].x(),
                                                        points[pi + 2].x(),
                                                        points[pi + 4].x(),
                                                        points[pi + 6].x() );

                    const xsimd::batch<float, 4> curXj( points[pj + 0].x(),
                                                        points[pj + 2].x(),
                                                        points[pj + 4].x(),
                                                        points[pj + 6].x() );
                    */

                    // ### Check with scalar ###
                    const float scurYi[4] = { points[pi + 0].y(),
                                              points[pi + 2].y(),
                                              points[pi + 4].y(),
                                              points[pi + 6].y() };

                    const float scurYj[4] = { points[pj + 0].y(),
                                              points[pj + 2].y(),
                                              points[pj + 4].y(),
                                              points[pj + 6].y() };

                    const float scurXi[4] = { points[pi + 0].x(),
                                              points[pi + 2].x(),
                                              points[pi + 4].x(),
                                              points[pi + 6].x() };

                    const float scurXj[4] = { points[pj + 0].x(),
                                              points[pj + 2].x(),
                                              points[pj + 4].x(),
                                              points[pj + 6].x() };

                    // Compare againts the Y coordinates
                    //const xsimd::batch_bool<float, 4>& cmpYs = (
                    //    ( curY >= curYi && curY < curYj ) || ( curY >= curYj && curY < curYi )
                    //);

                    // Calculate the interpolated X coordinates
                    //const xsimd::batch<float, 4> deltaYp = curY  - curYi;
                    //const xsimd::batch<float, 4> deltaYj = curYj - curYi;
                    //const xsimd::batch<float, 4> deltaXj = curXj - curXi;
                    //const xsimd::batch<float, 4> interXf = curXi + deltaYp / deltaYj * deltaXj;

                    // ### Check with scalar ###
                    float sdeltaYp[4], sdeltaYj[4], sdeltaXj[4], sinterXf[4];

                    for(int k = 0; k < vecSize; ++k) {
                        sdeltaYp[k] = y         - scurYi[k];
                        sdeltaYj[k] = scurYj[k] - scurYi[k];
                        sdeltaXj[k] = scurXj[k] - scurXi[k];
                        sinterXf[k] = scurXi[k] + sdeltaYp[k] / sdeltaYj[k] * sdeltaXj[k];
                    }

                    // Bail out if we have produced too many nodes
                    //if( (nodes + vecSize) >= nodeX.size() )
                    //    return;

                    // Store the coordinates as needed
                    //if(cmpYs[0]) nodeX[nodes++] = interXf[0];
                    //if(cmpYs[1]) nodeX[nodes++] = interXf[1];
                    //if(cmpYs[2]) nodeX[nodes++] = interXf[2];
                    //if(cmpYs[3]) nodeX[nodes++] = interXf[3];

                    // ### Check with scalar ###
                    for(int k = 0; k < vecSize; ++k) {
                        if( ( y >= scurYi[k] && y < scurYj[k] ) || ( y >= scurYj[k] && y < scurYi[k] ) ) {
                            if( nodes >= nodeX.size() ) return;
                            nodeX[nodes++] = sinterXf[k];

                            #if 0
                            if(y == y2 - 10) {
                                printf("IN_Y   j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)\n",
                                    pj + k * 2, pi + k * 2,
                                    points[pj + k * 2].x(), points[pj + k * 2].y(),
                                    points[pi + k * 2].x(), points[pi + k * 2].y()
                                );
                            }
                            #endif

                            #if 0
                            if(y == y2 - 10) {
                                printf("IN_X   j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)   ->   %05.1f   [%03zd]\n",
                                    pj + k * 2, pi + k * 2,
                                    points[pj + k * 2].x(), points[pj + k * 2].y(),
                                    points[pi + k * 2].x(), points[pi + k * 2].y(),
                                    sinterXf[k], nodes
                                );
                            }
                            #endif
                        }
                    }
                }
            }
        #endif

        #if 1
            // Process the remaining loop using scalar operations
            for(size_t j = remStart; j <= remEnd; ++j)
            {
                ++tot;
                // Calculate the i
                const size_t i = ( j >= (pointCount - 1) ) ? 0 : (j + 1);

                #if 0
                if(y == y2 - 10) {
                    printf("j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)   ###\n",
                        j, i,
                        points[j].x(), points[j].y(),
                        points[i].x(), points[i].y()
                    );
                }
                #endif

                // Get the Y coordinates
                const float curYi = points[i].y();
                const float curYj = points[j].y();

                // Check againts the Y coordinates
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if( nodes >= nodeX.size() )
                        return;

                    #if 0
                    if(y == y2 - 10) {
                        printf("IN_Y   j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)   ###\n",
                            j, i,
                            points[j].x(), points[j].y(),
                            points[i].x(), points[i].y()
                        );
                    }
                    #endif

                    // Get the X coordinates
                    const float curXi = points[i].x();
                    const float curXj = points[j].x();

                    // Calculate the node's coordinate
                    const float deltaYp = y     - curYi;
                    const float deltaYj = curYj - curYi;
                    const float deltaXj = curXj - curXi;
                    const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                    nodeX[nodes++] = interXf;
                    #if 0
                    if(y == y2 - 10) {
                        printf("IN_X   j = %03zd   i = %03zd   ->   (%05.1f, %05.1f)   (%05.1f, %05.1f)   ->   %05.1f   [%03zd]   ###\n",
                            j, i,
                            points[j].x(), points[j].y(),
                            points[i].x(), points[i].y(),
                            interXf, nodes
                        );
                    }
                    #endif
                }
            }
        #endif

#endif

            // Skip if there is no node generated
            if( !nodes ) continue;

            // Sort the nodes
            bubbleSortAscending(nodeX, nodes);

            // Fill the pixels between the node pairs
            for(std::size_t i = 0; i < nodes; i += 2)
            {
                // Calculate the coordinate
                Pt::int32_t from = ceil ( nodeX[i] );
                Pt::int32_t to   = floor( nodeX[i + 1] );

                // Pixel-by-pixel clipping
                if(from < _currentClip.left ()) from = _currentClip.left ();
                if(to   > _currentClip.right()) to   = _currentClip.right();

                if(to < from) continue;

                // Store the scanline coordinate as needed
                if(_compositionMode != CompositionMode::SourceCopy)
                {
                    scanlines[y - minY + 1].push_back(ScanlineElement16(from, to));
                }

                // Draw the scanline
                rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
            }
        }

        printf("$$$$$ %zd\n", tot);
    };

    lambdaWorker(minY, maxY);

    //auto worker1 = std::async(lambdaWorker, minY            , maxY * 1 / 4);
    //auto worker2 = std::async(lambdaWorker, maxY * 1 / 4 + 1, maxY * 2 / 4);
    //auto worker3 = std::async(lambdaWorker, maxY * 2 / 4 + 1, maxY * 3 / 4);
    //auto worker4 = std::async(lambdaWorker, maxY * 3 / 4 + 1, maxY        );

    //worker1.wait();
    //worker2.wait();
    //worker3.wait();
    //worker4.wait();

#else // WITH_EXPERIMENTAL_GFX

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using the coordinates from the polygon
        std::size_t nodes = 0;

        // Loop through the points
        Pt::int32_t j = pointCount - 1;

        for(size_t i = 0; i < pointCount; ++i)
        {
            // Get the Y coordinates
            const float curYi = points[i].y();
            const float curYj = points[j].y();

            // Check againts the Y coordinates
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodes >= nodeX.size() )
                    return;

                // Get the X coordinates
                const float curXi = points[i].x();
                const float curXj = points[j].x();

                // Calculate the node's coordinate
                const float deltaYp = y     - curYi;
                const float deltaYj = curYj - curYi;
                const float deltaXj = curXj - curXi;
                const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                nodeX[nodes++] = interXf;
            }

            // Update the searching index
            j = i;
        }

        // Skip if there is no node generated
        if( !nodes ) continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            Pt::int32_t from = ceil ( nodeX[i] );
            Pt::int32_t to   = floor( nodeX[i + 1] );

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy)
            {
                scanlines[y - minY + 1].push_back(ScanlineElement16(from, to));
            }

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }

#endif // WITH_EXPERIMENTAL_GFX

    // Raster the anti-aliased outline
#ifdef WITH_EXPERIMENTAL_GFX
    return;
#endif

    // Mask
    DrawLineMask xwaaMask;
    memcpy(xwaaMask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i)
    {
        rasterPolygonBorderXWAA_F2(
            points[i].x(),     points[i].y(),
            points[i + 1].x(), points[i + 1].y(),
            color, minX, minY - 1, scanlines, xwaaMask );
    }

    rasterPolygonBorderXWAA_F2(
          points[pc1].x(), points[pc1].y(),
          points[0].x(),   points[0].y(),
          color, minX, minY - 1, scanlines, xwaaMask );
}








// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonXWAA(const PointF* points, std::size_t pointCount,
                                    const Color& color,
                                    Pt::int32_t minX, Pt::int32_t minY,
                                    Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<float>& nodeX = _polygonNodeX;
    nodeX.resize(pointCount * 2);

    // List of polygon scanlines
    PolygonScanlines& scanlines = _polygonScanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

#ifdef WITH_EXPERIMENTAL_GFX

    auto lambdaWorker = [&](Pt::int32_t y1, Pt::int32_t y2) {
        //size_t __TOT__ = 0;

        // Loop through the rows of the image
        for(Pt::int32_t y = y1; y <= y2; ++y)
        {
            // Pixel-by-pixel clipping
            if(y < _currentClip.top   ()) continue;
            if(y > _currentClip.bottom()) continue;

            // Build a list of nodes using the coordinates from the polygon
            std::size_t nodes = 0;

#if 0

            // Loop through the points
            for(size_t j = 0; j < pointCount; ++j)
            {
                //++__TOT__;

                // Calculate the i
                const size_t i = ( j >= (pointCount - 1) ) ? 0 : (j + 1);

                // Get the Y coordinates
                const float curYi = points[i].y();
                const float curYj = points[j].y();

                // Check againts the Y coordinates
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if( nodes >= nodeX.size() )
                        return;

                    // Get the X coordinates
                    const float curXi = points[i].x();
                    const float curXj = points[j].x();

                    // Calculate the node's coordinate
                    const float deltaYp = y     - curYi;
                    const float deltaYj = curYj - curYi;
                    const float deltaXj = curXj - curXi;
                    const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                    nodeX[nodes++] = interXf;
                }
            }

#else

            /*
            // Scalar
            //    000000000011111111112222222222333 PointCount = 33
            //    012345678901234567890123456789012
            //  0 ji
            //  1  ji
            //  2   ji
            //  3    ji
            //  4     ji
            //  5      ji
            //  6       ji
            //  7        ji
            //  8         ji
            //  9          ji
            // 10           ji
            // 11            ji
            // 12             ji
            // 13              ji
            // 14               ji
            // 15                ji
            // 16                 ji
            // 17                  ji
            // 18                   ji
            // 19                    ji
            // 20                     ji
            // 21                      ji
            // 22                       ji
            // 23                        ji
            // 24                         ji
            // 25                          ji
            // 26                           ji
            // 27                            ji
            // 28                             ji
            // 29                              ji
            // 30                               ji
            // 31                                ji
            // 32 i                               j
            //    000000000011111111112222222222333
            //    012345678901234567890123456789012
            //
            //
            // VectorSize = 4
            //    000000000011111111112222222222333 PointCount = 33   Loop     = PointCount / VecSize / 2 = 4
            //    012345678901234567890123456789012                   LoopEnd  = Loop * VecSize * 2       = 32
            // 0a jijijiji                                            LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijiji                                           RemStart = Loop * VecSize * 2       = 32
            // 1a         jijijiji                                    RemEnd   = PointCount - 1           = 32
            // 1b          jijijiji                                   RemInc                              = 1
            // 2a                 jijijiji
            // 2b                  jijijiji
            // 3a                         jijijiji
            // 3b                          jijijiji
            // R0 i                               j
            //    000000000011111111112222222222333
            //    012345678901234567890123456789012
            //
            // VectorSize = 4
            //    00000000001111111111222222222233  PointCount = 32   Loop     = PointCount / VecSize / 2  = 4
            //    01234567890123456789012345678901                    LoopEnd  = Loop * VecSize * 2        = 32
            // 0a jijijiji                                            LastB    = (LoopEnd < PointCount)    = false
            // 0b  jijijiji                                           RemStart = LoopEnd - VecSize * 2 + 1 = 25
            // 1a         jijijiji                                    RemEnd   = PointCount - 1            = 31
            // 1b          jijijiji                                   RemInc                               = 2
            // 2a                 jijijiji
            // 2b                  jijijiji
            // 3a                         jijijiji
            // R0                          ji
            // R1                        *  ji
            // R2                            ji
            // R3                        *    ji
            // R4                              ji
            // R5                        *      ji
            // R6 i                              j
            //    00000000001111111111222222222233
            //    01234567890123456789012345678901
            //
            // VectorSize = 4
            //    000000000011111111112             PointCount = 21   Loop     = PointCount / VecSize / 2 = 2
            //    012345678901234567890                               LoopEnd  = Loop * VecSize * 2       = 16
            // 0a jijijiji                                            LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijiji                                           RemStart = Loop * VecSize * 2       = 16
            // 0a         jijijiji                                    RemEnd   = PointCount - 1           = 20
            // 0b          jijijiji                                   RemInc                              = 1
            // R0                 ji
            // R1                  ji
            // R2                   ji
            // R3                    ji
            // R4 i                   j
            //    000000000011111111112
            //    012345678901234567890
            //
            //
            // VectorSize = 8
            //    000000000011111111112222222222333 PointCount = 33   Loop     = PointCount / VecSize / 2 =  2
            //    012345678901234567890123456789012                   LoopEnd  = Loop * VecSize * 2       = 32
            // 0a jijijijijijijiji                                    LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijijijijijiji                                   RemStart = Loop * VecSize * 2       = 32
            // 1a                 jijijijijijijiji                    RemEnd   = PointCount - 1           = 32
            // 1b                  jijijijijijijiji                   RemInc                              = 1
            // R0 i                               j
            //    000000000011111111112222222222333
            //    012345678901234567890123456789012
            //
            // VectorSize = 8
            //    00000000001111111111222222222233  PointCount = 32   Loop     = PointCount / VecSize / 2  =  2
            //    01234567890123456789012345678901                    LoopEnd  = Loop * VecSize * 2        = 32
            // 0a jijijijijijijiji                                    LastB    = (LoopEnd < PointCount)    = false
            // 0b  jijijijijijijiji                                   RemStart = LoopEnd - VecSize * 2 + 1 = 17
            // 1a                 jijijijijijijiji                    RemEnd   = PointCount - 1            = 31
            // R0                  ji                                 RemInc                               = 2
            // R1                *  ji
            // R2                    ji
            // R3                *    ji
            // R4                      ji
            // R5                *      ji
            // R6                        ji
            // R7                *        ji
            // R8                          ji
            // R9                *          ji
            // RA                            ji
            // RB                *            ji
            // RC                              ji
            // RD                *              ji
            // RE i                              j
            //    00000000001111111111222222222233
            //    01234567890123456789012345678901
            //
            // VectorSize = 8
            //    000000000011111111112             PointCount = 21   Loop     = PointCount / VecSize / 2 =  1
            //    012345678901234567890                               LoopEnd  = Loop * VecSize * 2       = 16
            // 0a jijijijijijijiji                                    LastB    = (LoopEnd < PointCount)   = true
            // 0b  jijijijijijijiji                                   RemStart = Loop * VecSize * 2       = 16
            // R0                 ji                                  RemEnd   = PointCount - 1           = 20
            // R1                  ji                                 RemInc                              = 1
            // R2                   ji
            // R3                    ji
            // R4 i                   j
            //    000000000011111111112
            //    012345678901234567890
            */

            // Calculate the number of loop that can be calculated using SIMD and the number of remaining loop
            const size_t vecSize  = 4;

            const size_t loop     = pointCount / vecSize / 2;
            const size_t loopEnd  = loop * vecSize * 2;
            const bool   lastB    = (loopEnd < pointCount);

            const size_t remStart = lastB ? loopEnd : (loopEnd - vecSize * 2 + 1);
            const size_t remEnd   = pointCount - 1;
            const size_t remInc   = lastB ? 1 : 2;

            // Make a vector Y for the current scanline
            const xsimd::batch<float, 4> curY( y );

            #if 1
            // Loop as many as the number of loop that can be calculated using SIMD operations
            for(size_t j = 0; j < loop; ++j) {

                // Determine if the second SIMD part needs to be executed
                const size_t bOne = ( lastB || ( j < (loop - 1) ) ) ? 1 : 0;

                for(size_t b = 0; b <= bOne; ++b) {
                    //__TOT__ += vecSize;

                    // Calculate the real position within the array of points
                    const size_t pj = j * vecSize * 2 + b;
                    const size_t pi = pj + 1;

                    // Get the Y coordinates
                    const xsimd::batch<float, 4> curYi( points[pi + 0].y(),
                                                        points[pi + 2].y(),
                                                        points[pi + 4].y(),
                                                        points[pi + 6].y() );

                    const xsimd::batch<float, 4> curYj( points[pj + 0].y(),
                                                        points[pj + 2].y(),
                                                        points[pj + 4].y(),
                                                        points[pj + 6].y() );

                    // Get the X coordinates
                    const xsimd::batch<float, 4> curXi( points[pi + 0].x(),
                                                        points[pi + 2].x(),
                                                        points[pi + 4].x(),
                                                        points[pi + 6].x() );

                    const xsimd::batch<float, 4> curXj( points[pj + 0].x(),
                                                        points[pj + 2].x(),
                                                        points[pj + 4].x(),
                                                        points[pj + 6].x() );

                    // Compare againts the Y coordinates
                    const xsimd::batch_bool<float, 4>& cmpYs = (
                        ( curY >= curYi && curY < curYj ) || ( curY >= curYj && curY < curYi )
                    );

                    // Calculate the interpolated X coordinates
                    const xsimd::batch<float, 4> deltaYp = curY  - curYi;
                    const xsimd::batch<float, 4> deltaYj = curYj - curYi;
                    const xsimd::batch<float, 4> deltaXj = curXj - curXi;
                    const xsimd::batch<float, 4> interXf = curXi + deltaYp / deltaYj * deltaXj;

                    // Bail out if we have produced too many nodes
                    if( (nodes + vecSize) >= nodeX.size() )
                        return;

                    // Store the coordinates as needed
                    if(cmpYs[0]) nodeX[nodes++] = interXf[0];
                    if(cmpYs[1]) nodeX[nodes++] = interXf[1];
                    if(cmpYs[2]) nodeX[nodes++] = interXf[2];
                    if(cmpYs[3]) nodeX[nodes++] = interXf[3];
                }
            }
            #endif

            #if 1
            // Process the remaining loop using scalar operations
            for(size_t j = remStart; j <= remEnd; j += remInc)
            {
                //++__TOT__;

                // Calculate the i
                const size_t i = ( j >= (pointCount - 1) ) ? 0 : (j + 1);

                // Get the Y coordinates
                const float curYi = points[i].y();
                const float curYj = points[j].y();

                // Check againts the Y coordinates
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if( nodes >= nodeX.size() )
                        return;

                    // Get the X coordinates
                    const float curXi = points[i].x();
                    const float curXj = points[j].x();

                    // Calculate the node's coordinate
                    const float deltaYp = y     - curYi;
                    const float deltaYj = curYj - curYi;
                    const float deltaXj = curXj - curXi;
                    const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                    nodeX[nodes++] = interXf;
                }
            }
            #endif

#endif

            // Skip if there is no node generated
            if( !nodes ) continue;

            // Sort the nodes
            bubbleSortAscending(nodeX, nodes);

            // Fill the pixels between the node pairs
            for(std::size_t i = 0; i < nodes; i += 2)
            {
                // Calculate the coordinate
                Pt::int32_t from = ceil ( nodeX[i] );
                Pt::int32_t to   = floor( nodeX[i + 1] );

                // Pixel-by-pixel clipping
                if(from < _currentClip.left ()) from = _currentClip.left ();
                if(to   > _currentClip.right()) to   = _currentClip.right();

                if(to < from) continue;

                // Store the scanline coordinate as needed
                if(_compositionMode != CompositionMode::SourceCopy)
                {
                    scanlines[y - minY + 1].push_back(ScanlineElement16(from, to));
                }

                // Draw the scanline
                rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
            }
        }

        //printf("$$$$$ %zd\n", __TOT__);
    };

    lambdaWorker(minY, maxY);

    //auto worker1 = std::async(lambdaWorker, minY            , maxY * 1 / 4);
    //auto worker2 = std::async(lambdaWorker, maxY * 1 / 4 + 1, maxY * 2 / 4);
    //auto worker3 = std::async(lambdaWorker, maxY * 2 / 4 + 1, maxY * 3 / 4);
    //auto worker4 = std::async(lambdaWorker, maxY * 3 / 4 + 1, maxY        );

    //worker1.wait();
    //worker2.wait();
    //worker3.wait();
    //worker4.wait();

#else // WITH_EXPERIMENTAL_GFX

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using the coordinates from the polygon
        std::size_t nodes = 0;

        // Loop through the points
        Pt::int32_t j = pointCount - 1;

        for(size_t i = 0; i < pointCount; ++i)
        {
            // Get the Y coordinates
            const float curYi = points[i].y();
            const float curYj = points[j].y();

            // Check againts the Y coordinates
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodes >= nodeX.size() )
                    return;

                // Get the X coordinates
                const float curXi = points[i].x();
                const float curXj = points[j].x();

                // Calculate the node's coordinate
                const float deltaYp = y     - curYi;
                const float deltaYj = curYj - curYi;
                const float deltaXj = curXj - curXi;
                const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                nodeX[nodes++] = interXf;
            }

            // Update the searching index
            j = i;
        }

        // Skip if there is no node generated
        if( !nodes ) continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            Pt::int32_t from = ceil ( nodeX[i] );
            Pt::int32_t to   = floor( nodeX[i + 1] );

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy)
            {
                scanlines[y - minY + 1].push_back(ScanlineElement16(from, to));
            }

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }

#endif // WITH_EXPERIMENTAL_GFX

    // Raster the anti-aliased outline

    // Mask
    DrawLineMask xwaaMask;
    memcpy(xwaaMask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i)
    {
        rasterPolygonBorderXWAA_F2(
            points[i].x(),     points[i].y(),
            points[i + 1].x(), points[i + 1].y(),
            color, minX, minY - 1, scanlines, xwaaMask );
    }

    rasterPolygonBorderXWAA_F2(
          points[pc1].x(), points[pc1].y(),
          points[0].x(),   points[0].y(),
          color, minX, minY - 1, scanlines, xwaaMask );
}
