/*

// REVIEW: Seems nothing actually uses these functions anymore?

// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaNoAA(const Point* points, const size_t* pointCount,
                                        size_t polyCount, size_t totalPointCount,
                                        const Color& color,
                                        Pt::int32_t minX, Pt::int32_t minY,
                                        Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    // Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY)
    {
        // Pixel-by-pixel clipping
        if(pixelY < _currentClip.top   ()) continue;
        if(pixelY > _currentClip.bottom()) continue;

        // Base pointer for the polygons
        const Point* curPointBase = points;

        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p)
        {
            // Get the current point count
            const size_t curPointCount = pointCount[p];

            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i)
            {
                // Get the coordinates
                const Pt::int32_t curXi = (curPointBase + i)->x();
                const Pt::int32_t curYi = (curPointBase + i)->y();
                const Pt::int32_t curXj = (curPointBase + j)->x();
                const Pt::int32_t curYj = (curPointBase + j)->y();

                // Calculate the node's coordinate
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size())
                        return;

                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF) /
                                                  deltaYj * deltaXj
                                                );
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
                }

                // Update the searching index
                j = i;
            }
            // Increment the base pointer
            curPointBase += curPointCount;
        }

        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2)
        {
            // Get the coordinate
            Pt::int32_t from = nodeX[i    ];
            Pt::int32_t to   = nodeX[i + 1];

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }
}


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaXWAA(const PointF* points, const size_t* pointCount,
                                        size_t polyCount, size_t totalPointCount,
                                        const Color& color,
                                        float minX_, float minY_,
                                        float maxX_, float maxY_)
{
    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // Calculate the scaled Y coordinates
    const Pt::int32_t minX = Pt::lround(floor(minX_));
  //const Pt::int32_t maxX = Pt::lround(floor(maxX_));
    const Pt::int32_t minY = Pt::lround(floor(minY_));
    const Pt::int32_t maxY = Pt::lround(floor(maxY_));

    // List of polygon scanlines
    PolygonScanlines scanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

    // Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY)
    {
        // Pixel-by-pixel clipping
        if(pixelY < _currentClip.top   ()) continue;
        if(pixelY > _currentClip.bottom()) continue;

        // Base pointer for the polygons
        const PointF* curPointBase = points;

        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p)
        {
            // Get the current point count
            const size_t curPointCount = pointCount[p];

            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i)
            {
                // Get the coordinates
                const float curXi = (curPointBase + i)->x();
                const float curYi = (curPointBase + i)->y();
                const float curXj = (curPointBase + j)->x();
                const float curYj = (curPointBase + j)->y();

                // Calculate the node's coordinate
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;

                    // Calculate the node's coordinate
                    const float deltaYp = pixelY - curYi;
                    const float deltaYj = curYj  - curYi;
                    const float deltaXj = curXj  - curXi;
                    const float interXf = curXi  + (deltaYp) / deltaYj * deltaXj;
                    nodeX[nodes++] = interXf;
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
            // Calculate the coordinate
#if 1
            Pt::int32_t from = Pt::lround( ceil (nodeX[i    ]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]) );
#else
            Pt::int32_t from = Pt::lround( nodeX[i    ] );
            Pt::int32_t to   = Pt::lround( nodeX[i + 1] );
#endif

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;
            //if( (to - from) < 1 ) continue;

            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy)
            {
                scanlines[pixelY - minY + 1].push_back(ScanlineElement16(from, to));
            }

            // Draw the scanline
            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }

    // Raster the anti-aliased outline
    const PointF* curPointBase = points;
    for(size_t p = 0; p < polyCount; ++p)
    {
        // Mask
        DrawLineMask mask_zero;
        DrawLineMask mask_nnp1;
        memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

        // From point N to point (N + 1), successively
        const size_t pc1 = pointCount[p] - 1;
        for(size_t i = 0; i < pc1; ++i) {
            rasterPolygonBorderXWAA_F(
                curPointBase[i    ].x(), curPointBase[i    ].y(),
                curPointBase[i + 1].x(), curPointBase[i + 1].y(),
                color, minX, minY_ - 1, scanlines, mask_nnp1
            );
            if(!i) memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
        }

        // From the last point to the first point
        mask_zero[2] = mask_zero[0];
        mask_zero[3] = mask_zero[1];
        mask_zero[0] = mask_nnp1[2];
        mask_zero[1] = mask_nnp1[3];
        rasterPolygonBorderXWAA_F(
             curPointBase[pc1].x(), curPointBase[pc1].y(),
             curPointBase[0  ].x(), curPointBase[0  ].y(),
             color, minX, minY_ - 1, scanlines, mask_zero
        );

        // Increment the base pointer
        curPointBase += pointCount[p];
    }
}
*/


/*
// THE ORIGINAL ONE USING FIXED POINTS
void Rasterizer2::rasterPolygonNoAA(const PointF* points, std::size_t pointCount,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    if( pointCount < 3 )
        return;

#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES && !this->isAntiAliasing()) {
        std::cerr << "### 2 ###\n";
        for (size_t i = 0; i < pointCount; ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << points[i].x() << ", " << points[i].y() << std::endl;
        }
    }
#endif

    std::vector<Point> polygon;
    polygon.reserve( pointCount );

    for(size_t i = 0; i < pointCount; ++i)
    {
        const PointF& pf = points[i];
        const Pt::int32_t x = Pt::lround( pf.x() );
        const Pt::int32_t y = Pt::lround( pf.y() );

        polygon.push_back( Point(x, y) );
        //polygon.push_back( Point(pf.x(), pf.y()) );
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(pointCount * 2, 0);

    // Loop through the rows of the image
    //for(Pt::int32_t y = minY; y <= maxY; ++y)
    for(Pt::int32_t y = minY; y < maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        // loop through the points
        Pt::int32_t j = pointCount - 1;

        for(size_t i = 0; i < pointCount; ++i)
        {
            // Get the coordinates
            const Pt::int32_t curXi = polygon[i].x();
            const Pt::int32_t curYi = polygon[i].y();
            const Pt::int32_t curXj = polygon[j].x();
            const Pt::int32_t curYj = polygon[j].y();

            // Calculate the node's coordinate
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if(nodes >= nodeX.size())
                    return;

                // Calculate the node's coordinate
                const Pt::int32_t deltaYp = y - curYi;
                const Pt::int32_t deltaYj = curYj  - curYi;
                const Pt::int32_t deltaXj = curXj  - curXi;
                //const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi) +
                //                            ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF ) /
                //                              deltaYj * deltaXj );
                const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi) +
                                            ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF ) *
                                              deltaXj / deltaYj );
                nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
            }


            j = i;
        }


        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Get the coordinate
            Pt::int32_t from = nodeX[i    ];
            Pt::int32_t to   = nodeX[i + 1];

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
#if 1
            if(IP2_DEBUG::DUMP_SCANLINE_COORDINATES) {
                std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << "RP NOAA " << (minX + (from - minX)) << ", " << (minY + y - minY) << " LEN " << ((to - minX) - (from - minX) + 1) << std::endl;
            }
#endif
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }
}
*/


/*
// THE ORIGINAL ONE USING FIXED POINTS
void Rasterizer2::rasterPolygonsNoAA(const std::vector<Polygon>& polygons,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    std::size_t totalPointCount = 0;
    std::vector< std::vector<Point> > polygonPoints;
    polygonPoints.reserve( polygons.size() );

    std::vector<Polygon>::const_iterator it;
    for(it = polygons.begin(); it != polygons.end(); ++it)
    {
        if( it->size() < 3 )
            continue;

        polygonPoints.push_back( std::vector<Point>() );
        polygonPoints.back().reserve( it->size() );

        for(size_t i = 0; i < it->size(); ++i)
        {
            const PointF& pf = it->at(i);
            const Pt::int32_t x = lround( pf.x() );
            const Pt::int32_t y = lround( pf.y() );

            polygonPoints.back().push_back( Point(x, y) );
        }

        totalPointCount += polygonPoints.back().size();
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;
        for(size_t p = 0; p < polygonPoints.size(); ++p)
        {
            const std::vector<Point>& polygon = polygonPoints[p];

            // loop through the points
            Pt::int32_t j = polygon.size() - 1;

            for(size_t i = 0; i < polygon.size(); ++i)
            {
                // Get the coordinates
                const Pt::int32_t curXi = polygon[i].x();
                const Pt::int32_t curYi = polygon[i].y();
                const Pt::int32_t curXj = polygon[j].x();
                const Pt::int32_t curYj = polygon[j].y();

                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if(nodes >= nodeX.size())
                        return;

                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = y - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi) +
                                                ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF ) /
                                                  deltaYj * deltaXj );
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
                }

                j = i;
            }
        }

        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Get the coordinate
            Pt::int32_t from = nodeX[i    ];
            Pt::int32_t to   = nodeX[i + 1];

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }
}
*/


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonXWAA(const PointF* points, std::size_t pointCount,
                                    const Color& color,
                                    Pt::int32_t minX, Pt::int32_t minY,
                                    Pt::int32_t maxX, Pt::int32_t maxY)
{
    std::size_t totalPointCount = pointCount;

    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // List of polygon scanlines
    PolygonScanlines scanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        // Loop through the points
        Pt::int32_t j = pointCount - 1;

        for(size_t i = 0; i < pointCount; ++i)
        {
            // Get the coordinates
            const float curXi = points[i].x();
            const float curYi = points[i].y();
            const float curXj = points[j].x();
            const float curYj = points[j].y();

            // Calculate the node's coordinate
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodes >= nodeX.size() )
                    return;

                // Calculate the node's coordinate
                const float deltaYp = y - curYi;
                const float deltaYj = curYj - curYi;
                const float deltaXj = curXj - curXi;
                const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                nodeX[nodes++] = interXf;
            }

            // Update the searching index
            j = i;
        }

        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]/* - 0.5f*/) );

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy)
            {
                scanlines[y - minY + 1].push_back(ScanlineElement16(from, to));
                //fprintf(stderr, "SL [%3d] = %3d - %3d\n", y, from, to);
                //if(y >= 32 && y <= 35) fprintf(stderr, "SL [%3d:%3d] = %3d - %3d\n", y - minY + 1, y, from, to);
            }

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
#if 0
            if(IP2_DEBUG::DUMP_SCANLINE_COORDINATES) {
                fprintf(stderr, "SL [%3d] = %3d - %3d\n", (minY + y - minY), (minX + (from - minX)), (minX + (from - minX)) + ((to - minX) - (from - minX) + 1) - 1 );
            }
#endif
        }
    }

    // Raster the anti-aliased outline

#if 0
    // OLD POLYGON XWAA
    // Mask
    DrawLineMask mask_zero;
    DrawLineMask mask_nnp1;
    memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i)
    {
        rasterPolygonBorderXWAA_F(
            points[i].x(),     points[i].y(),
            points[i + 1].x(), points[i + 1].y(),
            color, minX, minY_ - 1, scanlines, mask_nnp1 );

        if( ! i )
            memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
    }

    // From the last point to the first point
    mask_zero[2] = mask_zero[0];
    mask_zero[3] = mask_zero[1];
    mask_zero[0] = mask_nnp1[2];
    mask_zero[1] = mask_nnp1[3];

    rasterPolygonBorderXWAA_F(
          points[pc1].x(), points[pc1].y(),
          points[0].x(),   points[0].y(),
          color, minX, minY_ - 1, scanlines, mask_zero );
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
void Rasterizer2::rasterPolygonsXWAA(const std::vector<Polygon>& polygons,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    std::size_t totalPointCount = 0;

    for(std::vector<Polygon>::const_iterator it = polygons.begin();
        it != polygons.end(); ++it)
    {
        totalPointCount += it->size();
    }

    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // Calculate the scaled Y coordinates
    //const Pt::int32_t minX = Pt::lround(floor(minX_));
    // // unused: const Pt::int32_t maxX = Pt::lround(floor(maxX_));
    //const Pt::int32_t minY = Pt::lround(floor(minY_));
    //const Pt::int32_t maxY = Pt::lround(floor(maxY_));

    // List of polygon scanlines
    PolygonScanlines scanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        for(size_t p = 0; p < polygons.size(); ++p)
        {
            const Polygon* polygon = &polygons[p];

            if( polygon->size() < 2 )
                continue;

            // Loop through the points
            Pt::int32_t j = polygon->size() - 1;

            for(size_t i = 0; i < polygon->size(); ++i)
            {
                // Get the coordinates
                const float curXi = polygon->at(i).x();
                const float curYi = polygon->at(i).y();
                const float curXj = polygon->at(j).x();
                const float curYj = polygon->at(j).y();

                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size())
                        return;

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
        }

        // Skip if there is no node
        if( ! nodes)
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]/* - 0.5f*/) );

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

    // Raster the anti-aliased outline
    for(size_t p = 0; p < polygons.size(); ++p)
    {
        const Polygon* polygon = &polygons[p];

        if( polygon->size() < 2 )
            continue;

        /*
        // OLD POLYGON XWAA
        // Mask
        DrawLineMask mask_zero;
        DrawLineMask mask_nnp1;
        memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

        // From point N to point (N + 1), successively
        const size_t pc1 = polygon->size() - 1;

        for(size_t i = 0; i < pc1; ++i)
        {
            rasterPolygonBorderXWAA_F(
                polygon->at(i).x(), polygon->at(i).y(),
                polygon->at(i + 1).x(), polygon->at(i + 1).y(),
                color, minX, minY_ - 1, scanlines, mask_nnp1 );

            if( ! i )
                memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
        }

        // From the last point to the first point
        mask_zero[2] = mask_zero[0];
        mask_zero[3] = mask_zero[1];
        mask_zero[0] = mask_nnp1[2];
        mask_zero[1] = mask_nnp1[3];

        rasterPolygonBorderXWAA_F(
             polygon->at(pc1).x(), polygon->at(pc1).y(),
             polygon->at(0).x(), polygon->at(0).y(),
             color, minX, minY_ - 1, scanlines, mask_zero );
        */

        // Mask
        DrawLineMask xwaaMask;
        memcpy(xwaaMask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

        // From point N to point (N + 1), successively
        const size_t pc1 = polygon->size() - 1;

        for(size_t i = 0; i < pc1; ++i)
        {
            rasterPolygonBorderXWAA_F2(
                polygon->at(i).x(), polygon->at(i).y(),
                polygon->at(i + 1).x(), polygon->at(i + 1).y(),
                color, minX, minY - 1, scanlines, xwaaMask );
        }

        rasterPolygonBorderXWAA_F2(
             polygon->at(pc1).x(), polygon->at(pc1).y(),
             polygon->at(0).x(), polygon->at(0).y(),
             color, minX, minY - 1, scanlines, xwaaMask );
    }
}


#if 0
// OLD POLYGON XWAA
// REVIEW: Seems nothing actually uses this function anymore?

// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::rasterPolygonBorderXWAA_F(float x1, float y1,
                                            float x2, float y2,
                                            const Color& color,
                                            Pt::int32_t minX, Pt::int32_t minY,
                                            const PolygonScanlines& exclusionZone,
                                            DrawLineMask& maskInOut)
{
    //fprintf(stderr, "XWAA: (%6.3f, %6.3f) - (%6.3f, %6.3f)\n", x1, y1, x2, y2);

    // Get the mask's coordinate
    float mx[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };
    float my[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };

    for(Pt::int32_t i = 0; i < 4; ++i) {
        mx[i] = maskInOut[i].x();
        my[i] = maskInOut[i].y();
    }

    // Used for storing back the mask's coordinates
    Pt::int32_t pCnt  = 0;
    Pt::int32_t lx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t ly[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    // A helper macro to fill pixel
    #define XW_FILL_PIXEL(X, Y, A)                                                     \
        do {                                                                           \
            /* Clip the point */                                                       \
            if( !ClipShapeI::insideXYRange(X, Y, _currentClip) ) break;                \
            /* Check if we should skip drawing the pixel */                            \
            bool skipDrawing = false;                                                  \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                       \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                           \
                skipDrawing = true;                                                    \
                break;                                                                 \
            }                                                                          \
            if(skipDrawing || !(A)) break;                                             \
            /* Store back the mask's coordinates */                                    \
            lx[2] = lx[3]; lx[3] = X;                                                  \
            ly[2] = ly[3]; ly[3] = Y;                                                  \
            if(pCnt < 2) {                                                             \
                lx[pCnt] = X;                                                          \
                ly[pCnt] = Y;                                                          \
                ++pCnt;                                                                \
            }                                                                          \
            /* Fill the pixel */                                                       \
            if(_isTexture || _isGradient) {                                            \
                const Pt::int32_t bw = _brushImage->width();                           \
                const Pt::int32_t bh = _brushImage->height();                          \
                const Pt::int32_t dx = std::max<Pt::int32_t>(X - minX, 0);             \
                const Pt::int32_t dy = std::max<Pt::int32_t>(Y - minY, 0);             \
                const Pt::int32_t tx = _isGradient ? std::min(bw - 1, dx) : (dx % bw); \
                const Pt::int32_t ty = _isGradient ? std::min(bh - 1, dy) : (dy % bh); \
                ConstPixel srcPixel(_brushImage->view(), tx, ty);                      \
                Pixel      dstPixel(_image->view(), X, Y);                             \
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, A);    \
            }                                                                          \
            else { /* Solid */                                                         \
                Pixel pixel(_image->view(), X, Y);                                     \
                _image->format().setPixel(pixel, color, _compositionMode, A);          \
            }                                                                          \
        } while(false)

    // Check if the start and end coordinates are the same
    if(x1 == x2 && y1 == y2) {
        // Draw the pixel
        XW_FILL_PIXEL( lround(x1), lround(y1), 255);
        // Store back the start and end coordinates to the mask
        maskInOut[0].set(lx[0], ly[0]);
        maskInOut[1].set(lx[0], ly[0]);
        maskInOut[2].set(lx[0], ly[0]);
        maskInOut[3].set(lx[0], ly[0]);
        // Exit here
        return;
    }

    // Copy the coordinates
    float fx1 = x1;
    float fy1 = y1;
    float fx2 = x2;
    float fy2 = y2;

    // Swap the values as needed
    const float deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const float deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool  steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    const bool swapDir = (fx1 > fx2);

    if(swapDir) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const float       grad  = (fy2 - fy1) / (fx2 - fx1);
    const Pt::int32_t xpxl1 = lround(fx1);
    const Pt::int32_t xpxl2 = lround(fx2);
    const float       ypxl  = fy1 + grad * (xpxl1 - fx1);

    // Draw the pixels
    Pt::int32_t from  = lround(fx1);
    Pt::int32_t to    = xpxl2;
    float       ypxli = ypxl;

    if(steep) {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the alphas and coordinates
            const Pt::int32_t fypxli = Pt::lround( floor(ypxli) );
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            const Pt::int32_t x1 = fypxli;
            const Pt::int32_t x2 = fypxli + 1;
            const Pt::int32_t y  = i;
            ypxli += grad;
            // Draw the pixels as needed
            bool skipPixel1 = false;
            bool skipPixel2 = false;
            if(!exclusionZone.empty()) {
                for(S16V::const_iterator it = exclusionZone[y - minY].begin(); it != exclusionZone[y - minY].end(); ++it) {
                    if(x1 >= it->from && x1 <= it->to) skipPixel1 = true;
                    if(x2 >= it->from && x2 <= it->to) skipPixel2 = true;
                    if(skipPixel1 && skipPixel2) break;
                }
            }
            if(!skipPixel1) XW_FILL_PIXEL(x1, y, a1);
            if(!skipPixel2) XW_FILL_PIXEL(x2, y, a2);
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the alphas and coordinates
            const Pt::int32_t fypxli = Pt::lround( floor(ypxli) );
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            const Pt::int32_t x  = i;
            const Pt::int32_t y1 = ypxli;
            const Pt::int32_t y2 = ypxli + 1;
            ypxli += grad;
            // Draw the pixels as needed
            bool skipPixel = false;
            if(!exclusionZone.empty()) {
                for(S16V::const_iterator it = exclusionZone[y1 - minY].begin(); it != exclusionZone[y1 - minY].end(); ++it) {
                    if (x <= it->from || x >= it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(x, y1, a1);
            skipPixel = false;
            if(!exclusionZone.empty()) {
                for(S16V::const_iterator it = exclusionZone[y2 - minY].begin(); it != exclusionZone[y2 - minY].end(); ++it) {
                    if (x <= it->from || x >= it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(x, y2, a2);
        }
    }

    // Store back the start and end coordinates to the mask
    if(swapDir) {
        maskInOut[2].set(lx[0], ly[0]);
        maskInOut[3].set(lx[1], ly[1]);
        maskInOut[0].set(lx[2], ly[2]);
        maskInOut[1].set(lx[3], ly[3]);
    }
    else {
        maskInOut[0].set(lx[0], ly[0]);
        maskInOut[1].set(lx[1], ly[1]);
        maskInOut[2].set(lx[2], ly[2]);
        maskInOut[3].set(lx[3], ly[3]);
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}
#endif





void Rasterizer2::fillPolygon(const PointF* ps, std::size_t n)
{
#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES) {
        const std::ios_base::fmtflags f(std::cerr.flags());
        std::cerr << (this->isAntiAliasing() ? "WAA: " : "NAA: ") << "Rasterizer2::fillPolygon ### AT ENTRY POINT ###" << std::endl;
        for (size_t i = 0; i < n; ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << ps[i].x() << ", " << ps[i].y() << std::endl;
        }
        std::cerr.flags(f);
    }
#endif

    // Perform coordinate adjustments
    std::vector<PointF> polygon(n);
    size_t              pointCount = 0;

#define FIXED_ADJUST

#ifdef FIXED_ADJUST
    for (size_t i = 0; i < n; ++i)
    {
        // Foor the coordinates while avoiding rounding errors
        const double x = Pt::lround(ps[i].x() - 0.4999);
        const double y = Pt::lround(ps[i].y() - 0.4999);
        //const double x = ps[i].x();
        //const double y = ps[i].y();
        if(pointCount && polygon[pointCount - 1].x() == x && polygon[pointCount - 1].y() == y) continue;
        polygon[pointCount++].set(x, y);
    }
    polygon.resize(pointCount);
#else
    double xc = 0.0f;
    double yc = 0.0f;
    size_t cn = n;
    if(ps[n - 1] == ps[0]) --cn;
    for (size_t i = 0; i < cn; ++i) {
        xc += ps[i].x();
        yc += ps[i].y();
    }
    xc = xc / (double) cn;
    yc = yc / (double) cn;

    for (size_t i = 0; i < n; ++i) {
        double xi = ps[i].x();
             if(xi < xc) xi += 0.5f;
        else if(xi > xc) xi -= 0.5f;

        double yi = ps[i].y();
             if(yi < yc) yi += 0.5f;
        else if(yi > yc) yi -= 0.5f;

        // Foor the coordinates while avoiding rounding errors
        const double x = Pt::lround(xi - 0.4999);
        const double y = Pt::lround(yi - 0.4999);
        //const double x = ps[i].x();
        //const double y = ps[i].y();
        if(pointCount && polygon[pointCount - 1].x() == x && polygon[pointCount - 1].y() == y) continue;
        polygon[pointCount++].set(x, y);
    }
    polygon.resize(pointCount);
#endif

#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES) {
        const std::ios_base::fmtflags f(std::cerr.flags());
#ifdef FIXED_ADJUST
        std::cerr << (this->isAntiAliasing() ? "WAA: " : "NAA: ") << "Rasterizer2::fillPolygon ### AFTER FIXED ADJUST ###" << std::endl;
#else
        std::cerr << (this->isAntiAliasing() ? "WAA: " : "NAA: ") << "Rasterizer2::fillPolygon ### AFTER DYNAMIC ADJUST ### CENTER = " ;
        std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                  << xc << ", " << yc << std::endl;
#endif
        for (size_t i = 0; i < polygon.size(); ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << polygon[i].x() << ", " << polygon[i].y() << std::endl;
        }
        std::cerr << std::endl;
        std::cerr.flags(f);
    }
#endif

    // Clip the polygon
    BasicClipShape<PointF::ValueT>::clipPolygon(polygon, _currentClip);

#if 0
    // #@#
    // Perform coordinate adjustments
    /*
     *
     * 0   1x
     *
     * 3y  2xy
     *
     */
    const size_t sz = polygon.size();

    std::vector<bool> adjX(sz, false);
    std::vector<bool> adjY(sz, false);

    for(size_t i = 0; i < sz; ++i)
    {
        size_t j = i + 1;
        if(j >= sz) j = 0;

        const double x0 = polygon[i].x();
        const double y0 = polygon[i].y();
        const double x1 = polygon[j].x();
        const double y1 = polygon[j].y();

        if(y0 == y1) {
            if(x1 > x0) adjX[j] = true;
            if(i > 0) {
                if(adjY[i]) adjY[j] = true;
            }
        }
        if(x0 == x1) {
            if(y1 > y0) adjY[j] = true;
            if(i > 0) {
                if(adjX[i]) adjX[j] = true;
            }
        }
    }

    for(size_t i = 0; i < sz; ++i) {
        if(adjX[i]) polygon[i].setX(polygon[i].x() - 1.0f);
        if(adjY[i]) polygon[i].setY(polygon[i].y() - 1.0f);
    }

    adjX.clear();
    adjY.clear();
#endif

    // Find the minimum and maximum coordinates
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;
    for(size_t j = 0; j < polygon.size(); ++j)
    {
#if 1
        const PointF::ValueT x = polygon[j].x();
        const PointF::ValueT y = polygon[j].y();

        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
#else
        const PointF::ValueT x1 = floor( polygon[j].x() );
        const PointF::ValueT x2 = ceil ( polygon[j].x() );
        if(x1 < minX) minX = x1;
        if(x2 < minX) minX = x2;
        if(x1 > maxX) maxX = x1;
        if(x2 > maxX) maxX = x2;

        const PointF::ValueT y1 = floor( polygon[j].y() );
        const PointF::ValueT y2 = ceil ( polygon[j].y() );
        if(y1 < minY) minY = y1;
        if(y2 < minY) minY = y2;
        if(y1 > maxY) maxY = y1;
        if(y2 > maxY) maxY = y2;
#endif
    }

    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    // #@#
    if( this->isAntiAliasing() )
    {
        rasterPolygonXWAA(&polygon[0], polygon.size(),
                          _brush.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonNoAA(&polygon[0], polygon.size(),
                          _brush.color(), minX, minY, maxX, maxY);
    }
}




#if 0
        int drawSPECIALTEST(int y, Pt::Gfx::Painter& painter)
        {
            using namespace Pt::Gfx;

            Color lightPurple = Color::fromRgb8(164, 100, 255);
            Color lightBlue = Color::fromRgb8(100, 100, 255);

            painter.setPen(lightBlue);
            painter.setBrush(lightPurple);

            double x      = 5;
            double width  = 5;
            double height = 5;
            std::vector<Pt::Gfx::PointF> polygon;

            auto makeSimpleRectangle = [](double x, double y, double width, double height) {
                std::vector<Pt::Gfx::PointF> p(5);
                p[0].set(x,         y         );
                p[1].set(x + width, y         );
                p[2].set(x + width, y + height);
                p[3].set(x,         y + height);
                p[4] = p[0];
                return p;
            };
            polygon = makeSimpleRectangle(x, y, width, height); x += width + 2;
            painter.drawPolyline(&polygon[0], polygon.size());
            polygon = makeSimpleRectangle(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
            polygon = makeSimpleRectangle(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            painter.drawPolyline(&polygon[0], polygon.size());
            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;
            y += 20;

            x      = 5;
            width  = 12;
            height = 12;
            auto makeSimpleDiamond = [](double x, double y, double width, double height) {
                std::vector<Pt::Gfx::PointF> p(5);
                p[0].set(x + width/2.0, y             );
                p[1].set(x + width,     y + height/2.0);
                p[2].set(x + width/2.0, y + height    );
                p[3].set(x,             y + height/2.0);
                p[4] = p[0];
                return p;
            };
            polygon = makeSimpleDiamond(x, y, width, height); x += width + 2;
            painter.drawPolyline(&polygon[0], polygon.size());
            polygon = makeSimpleDiamond(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
            polygon = makeSimpleDiamond(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            painter.drawPolyline(&polygon[0], polygon.size());
            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;
            y += 20;

            x      = 5;
            width  = 12;
            height = 12;
            auto makeSimpleFlag = [](double x, double y, double width, double height) {
                std::vector<Pt::Gfx::PointF> p(6);
                p[0].set(x,             y             );
                p[1].set(x + width,     y             );
                p[2].set(x + width/2.0, y + height/2.0);
                p[3].set(x + width,     y + height    );
                p[4].set(x,             y + height    );
                p[5] = p[0];
                return p;
            };
            polygon = makeSimpleFlag(x, y, width, height); x += width + 2;
            painter.drawPolyline(&polygon[0], polygon.size());
            polygon = makeSimpleFlag(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            //IP2_DEBUG::DUMP_POLYGON_COORDINATES = true;
            polygon = makeSimpleFlag(x, y, width, height); x += width + 2;
            painter.fillPolygon (&polygon[0], polygon.size());
            painter.drawPolyline(&polygon[0], polygon.size());
            IP2_DEBUG::DUMP_POLYGON_COORDINATES = false;
            y += 20;

            return y;
        }
#endif
