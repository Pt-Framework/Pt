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


