template<typename T>
static inline void insertionsortAscending(T& basket, Pt::int32_t size)
{
    if(size <= 1) return;

    for(Pt::int32_t i = 1; i < size; ++i)
    {
        const typename T::value_type key = basket[i];
        Pt::int32_t                  j   = i - 1;

        while(j >= 0 && basket[j] > key)
        {
            basket[j + 1] = basket[j];
            --j;
        }
        basket[j + 1] = key;

    }
}


template<typename T>
static inline void selectionsortAscending(T& basket, Pt::int32_t size)
{
    for(Pt::int32_t i = 0; i < size - 1; ++i)
    {
        Pt::int32_t min = i;
        for(Pt::int32_t j = i+1; j < size; ++j)
        {
            if(basket[j] < basket[min]) min = j;
        }
        std::swap(basket[min], basket[i]);
    }
}


template<typename T>
static inline Pt::int32_t quicksortAscending_partition(T& basket, Pt::int32_t start, Pt::int32_t end)
{
    const typename T::value_type pivot = basket[end];
    Pt::int32_t                  pidx  = start;

    for(Pt::int32_t i = start; i < end; ++i)
    {
        if(basket[i] <= pivot)
        {
            std::swap(basket[i], basket[pidx]);
            ++pidx;
        }
    }

    std::swap(basket[pidx], basket[end]);

    return pidx;
}

template<typename T>
static inline void quicksortAscending(T& basket, Pt::int32_t start, Pt::int32_t end)
{
    if(start >= end) return;

    const Pt::int32_t pidx = quicksortAscending_partition(basket, start, end);

    quicksortAscending(basket, start,    pidx - 1);
    quicksortAscending(basket, pidx + 1, end     );
}

template<typename T>
static inline void quicksortAscending(T& basket, Pt::int32_t size)
{
    quicksortAscending(basket, 0, size - 1);
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
    std::vector<float> nodeX(pointCount * 2, 0);

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

#if 1
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
#else
        // Loop through the points to build a list of nodes using the coordinates from the polygon
        const PointF* pIterI    = &points[0];
        const PointF* pIterIEnd = pIterI + pointCount;
        const PointF* pIterJ    = pIterIEnd - 1;

        float* nodeXIterBeg = &nodeX[0];
        float* nodeXIterEnd = nodeXIterBeg + nodeX.size();
        float* nodeXIter    = nodeXIterBeg;

        while(pIterI < pIterIEnd)
        {
            // Get the Y coordinates
            const float curYi = pIterI->y();
            const float curYj = pIterJ->y();

            // Check againts the Y coordinates
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodeXIter >= nodeXIterEnd )
                    return;

                // Get the X coordinates
                const float curXi = pIterI->x();
                const float curXj = pIterJ->x();

                // Calculate the node's coordinate
                const float deltaYp = y     - curYi;
                const float deltaYj = curYj - curYi;
                const float deltaXj = curXj - curXi;
                const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                *nodeXIter++ = interXf;
            }

            // Update the searching index
            pIterJ = pIterI++;
        }

        // Claculate the number of generated nodes
        const std::size_t nodes = (size_t) (nodeXIter - nodeXIterBeg);
#endif

        // Skip if there is no node generated
        if( !nodes ) continue;

        // Sort the nodes
        //selectionsortAscending(nodeX, nodes);
        //insertionsortAscending(nodeX, nodes);
        bubbleSortAscending(nodeX, nodes);
        //quicksortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            //Pt::int32_t from = Pt::lround( ceil ( nodeX[i] ) );
            //Pt::int32_t to   = Pt::lround( floor( nodeX[i + 1]/* - 0.5f*/ ) );
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

