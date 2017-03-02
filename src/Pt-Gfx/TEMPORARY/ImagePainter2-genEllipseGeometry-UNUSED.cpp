void ImagePainter2::genArcGeometryQSC( std::vector<Point>& points, const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, bool createPie )
{
    // Ensure that the begin and end degrees are within the acceptable range
    while(degBegin <   0) degBegin += 360;
    while(degBegin > 360) degBegin -= 360;

    while(degEnd <   0) degEnd += 360;
    while(degEnd > 360) degEnd -= 360;

    if(degEnd < degBegin) std::swap(degEnd, degBegin);

    // Calculate the arc's parameters
    const Pt::int32_t radiusX = size.width () / 2;
    const Pt::int32_t radiusY = size.height() / 2;
    const Pt::int32_t radiusM = std::max(radiusX, radiusY);
    const Pt::int32_t centerX = topLeft.x() + radiusX;
    const Pt::int32_t centerY = topLeft.y() + radiusY;
    const Pt::int32_t degFac  = (degEnd - degBegin) / 36;
    const Pt::int32_t numSegD = (radiusM * (degFac ? degFac : 1) / 10 / 20) * 20;
    const Pt::int32_t numSegs = (numSegD >= 16) ? numSegD : 16;
    const float       fdegInc = (Pt::Pi * (degEnd -  degBegin) / 180) / (numSegs - 1);

    // Generate a polygon that approximates the arc
    Pt::int32_t prevX = ImagePainter2::MaximumCoordinate;
    Pt::int32_t prevY = ImagePainter2::MaximumCoordinate;
    float       angle = Pt::Pi * degBegin / 180;
    Pt::int32_t p     = 0;

    points.resize(numSegs + (createPie ? 1 : 0));

    for(Pt::int32_t i = 0; i < numSegs; ++i) {
        // Calculate the coordinates
        const Pt::int32_t x = centerX + radiusX * fastCos(angle);
        const Pt::int32_t y = centerY - radiusY * fastSin(angle);
        // Update the angle
        angle += fdegInc;
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    if(createPie) // For drawing a pie, add one more point at the center of the arc
        points[p++].set(centerX, centerY);

    // Resize the vector to remove extra elements that may exist
    points.resize(p);
}

void ImagePainter2::genEllipseGeometryQSC( std::vector<Point>& points, const PointF& topLeft, const SizeF& size )
{
    // Calculate the ellipse's parameters
    const Pt::int32_t radiusX = size.width () / 2;
    const Pt::int32_t radiusY = size.height() / 2;
    const Pt::int32_t radiusM = std::max(radiusX, radiusY);
    const Pt::int32_t centerX = topLeft.x() + radiusX;
    const Pt::int32_t centerY = topLeft.y() + radiusY;
    const Pt::int32_t numSegD = (radiusM / 20) * 20;
    const Pt::int32_t numSegs = (numSegD >= 16) ? numSegD : 16;
    const Pt::int32_t qtrSegs = (numSegs / 4);
    const Pt::int32_t qtrSeg1 = qtrSegs - 1;

    // Calculate the coordinate displacements
    std::vector<float> disX(qtrSegs);
    std::vector<float> disY(qtrSegs);
    for(Pt::int32_t i = 0; i < qtrSegs; ++i) {
        // Calculate the angle
        const float angle = 0.5f * Pt::Pi * i / qtrSegs;
        // Calculate the displacements
        disX[i] =  radiusX * fastCos<float, true>(angle);
        disY[i] = -radiusY * fastSin<float, true>(angle);
    }

    // Generate a polygon that approximates the ellipse
    Pt::int32_t prevX = ImagePainter2::MaximumCoordinate;
    Pt::int32_t prevY = ImagePainter2::MaximumCoordinate;
    Pt::int32_t p     = 0;

    points.resize(numSegs);

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant I
        // Calculate the coordinates
        const Pt::int32_t x = centerX + disX[i];
        const Pt::int32_t y = centerY + disY[i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant II
        // Calculate the coordinates
        const Pt::int32_t x = centerX - disX[qtrSeg1 - i];
        const Pt::int32_t y = centerY + disY[qtrSeg1 - i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant III
        // Calculate the coordinates
        const Pt::int32_t x = centerX - disX[i];
        const Pt::int32_t y = centerY - disY[i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    for(Pt::int32_t i = 0; i < qtrSegs; ++i) { // Quadrant IV
        // Calculate the coordinates
        const Pt::int32_t x = centerX + disX[qtrSeg1 - i];
        const Pt::int32_t y = centerY - disY[qtrSeg1 - i];
        // Skip duplicated points
        if(prevX == x && prevY == y) continue;
        prevX = x;
        prevY = y;
        // Store the point and increment the index
        points[p++].set(x, y);
    }

    // Resize the vector to remove extra elements that may exist
    points.resize(p);
}

void ImagePainter2::genEllipseGeometryXMI( std::vector<Point>& points, const PointF& topLeft, const SizeF& size )
{
    // Calculate the coordinate displacements as per this equation:
    //     e(X, Y) = ( b^2 * X^2 ) + ( a^2 * Y^2 ) - ( a^2 * b^2 )

    std::vector<Pt::int32_t> disX;

    const Pt::int32_t minX   =  (Pt::int32_t) topLeft.x();
    const Pt::int32_t minY   =  (Pt::int32_t) topLeft.y();
    const Pt::int32_t a      =  size.width () / 2;
    const Pt::int32_t b      =  size.height() / 2;
    const Pt::int32_t a2     =  a * a;
    const Pt::int32_t b2     =  b * b;
    const Pt::int32_t xc     =  a;
    const Pt::int32_t crit1  = -(a2 / 4 + a % 2 + b2);
    const Pt::int32_t crit2  = -(b2 / 4 + b % 2 + a2);
    const Pt::int32_t crit3  = -(b2 / 4 + b % 2     );
    const Pt::int32_t d2xt   =  2 * b2;
    const Pt::int32_t d2yt   =  2 * a2;
          Pt::int32_t dxt    =  0;
          Pt::int32_t dyt    = -2 * a2 * b;
          Pt::int32_t x      =  0;
          Pt::int32_t y      =  b;
          Pt::int32_t width  =  1;
          Pt::int32_t t      = -a2 * b;

    while( y > 0 && x <= a ) {
        if( (t + b2 * x) <= crit1 || (t + a2 * y) <= crit3 ) {
            ++x;
            dxt   += d2xt;
            t     += dxt;
            width += 2;
        }
        else if( (t - a2 * y) > crit2 )  {
            disX.push_back(xc - x);
            --y;
            dyt += d2yt;
            t   += dyt;
        }
        else {
            disX.push_back(xc - x);
            ++x;
            dxt   += d2xt;
            t     += dxt;
            width += 2;
            --y;
            dyt   += d2yt;
            t     += dyt;
        }
    }

    if( !b ) disX.push_back(xc - a);

    // Sort the coordinates
    std::sort(disX.begin(), disX.end());

    // Generate a polygon that approximates the ellipse
    const Pt::int32_t addY  = ((Pt::int32_t) size.height() % 2) ? 1 : 0;
    const Pt::int32_t incY  = FIXED_POINT_FROM_INT((Pt::int32_t) (size.height() + addY)) / disX.size() / 2;
          Pt::int32_t iterY = FIXED_POINT_FROM_INT(minY);

    points.clear();

    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Top-left
        points.push_back( Point( minX + disX[disX.size() - 1 - iterX], FIXED_POINT_TO_INT(iterY) ) );
        iterY += incY;
    }
    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Bottom-left
        points.push_back( Point( minX + disX[iterX], FIXED_POINT_TO_INT(iterY) ) );
        iterY += incY;
    }
    iterY -= incY;
    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Bottom-right
        points.push_back( Point( minX + 2 * a - disX[disX.size() - 1 - iterX], FIXED_POINT_TO_INT(iterY) ) );
        iterY -= incY;
    }
    for(size_t iterX = 0; iterX < disX.size(); ++iterX) { // Top-right
        points.push_back( Point( minX + 2 * a - disX[iterX], FIXED_POINT_TO_INT(iterY) ) );
        iterY -= incY;
    }
}
