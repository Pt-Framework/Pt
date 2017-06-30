/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

//
// Line rasterization functions
//

inline void Rasterizer2::rasterOnePixelSolidLine(Pt::int32_t x1, Pt::int32_t y1, 
                                                 Pt::int32_t x2, Pt::int32_t y2, 
                                                 const Color& color, DrawLineMask* maskInOut)
{
    // Check for horizontal line
    if(y1 == y2) {
        rasterOnePixelSolidHLineSegment(x1, x2, y1, color, maskInOut);
        return;
    }

    // Check for vertical line
    if(x1 == x2) {
        rasterOnePixelSolidVLineSegment(x1, y1, y2, color, maskInOut);
        return;
    }

    // Check for 45-degree line
    if(abs(x2 - x1) == abs(y2 - y1)) {
        rasterOnePixelSolidXLineSegment(x1, y1, x2, y2, color, maskInOut);
        return;
    }

    // Generic line
    if( ! _aaMode) {
        // Raster the line without using anti-aliasing
        rasterOnePixelSolidGLineSegmentNoAA(x1, y1, x2, y2, color, maskInOut);
    }
    else {
        // Raster the line using anti-aliasing
        rasterOnePixelSolidGLineSegmentXWAA(x1, y1, x2, y2, color, maskInOut);
    }
}


inline void Rasterizer2::rasterOnePixelSolidLine_F(float x1, float y1, 
                                                   float x2, float y2, 
                                                   const Color& color, DrawLineMask* maskInOut)
{ 
    rasterOnePixelSolidGLineSegmentXWAA_F(x1, y1, x2, y2, color, maskInOut); 
}


inline void Rasterizer2::rasterOnePixelPatternedLine(Pt::int32_t x1, Pt::int32_t y1, 
                                                     Pt::int32_t x2, Pt::int32_t y2, 
                                                     const Color& color, Pt::int32_t& fpiCtrInOut, 
                                                     DrawLineMask* maskInOut)
{
    // Check the size of the line
    const Pt::int32_t sizeX = abs(x2 - x1);
    const Pt::int32_t sizeY = abs(y2 - y1);
    const Pt::int32_t sizeS = sizeX + sizeY;
    const Pt::int32_t sizeL = sqrtf(sizeX * sizeX + sizeY * sizeY);

    // Calculate the incremental factor of the pattern indexing counter
    const Pt::int32_t fpiCtrInc = FIXED_POINT_CONSTANT_ISQRT2 * PATTERN_BUFFER_SCALE_FACTOR * sizeS / sizeL;

    // Check for 45-degree line
    if(abs(x2 - x1) == abs(y2 - y1)) {
        rasterOnePixelPatternedXLineSegment(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
    }

    // Generic line
    else {
        // Without anti-aliasing
        if( ! _aaMode)
            rasterOnePixelPatternedGLineSegmentNoAA(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
        // With anti-aliasing
        else
            rasterOnePixelPatternedGLineSegmentXWAA(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
    }
}


inline void Rasterizer2::rasterOnePixelPatternedLine_F(float x1, float y1, 
                                                       float x2, float y2, 
                                                       const Color& color, 
                                                       Pt::int32_t& fpiCtrInOut, 
                                                       DrawLineMask* maskInOut)
{
    // Check the size of the line
    const float sizeX = ::fabs(x2 - x1);
    const float sizeY = ::fabs(y2 - y1);
    const float sizeS = sizeX + sizeY;
    const float sizeL = sqrt(sizeX * sizeX + sizeY * sizeY);

    // Calculate the incremental factor of the pattern indexing counter
    const Pt::int32_t fpiCtrInc = lround(FIXED_POINT_CONSTANT_ISQRT2 * PATTERN_BUFFER_SCALE_FACTOR * sizeS / sizeL);

    // Rasterize line
    rasterOnePixelPatternedGLineSegmentXWAA_F(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
}


//
// Polygon rasterization functions
//

inline void Rasterizer2::rasterPolygonArea(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{ rasterPolygonAreaNoAA(points, pointCount, polyCount, totalPointCount, color, minX, minY, maxX, maxY); }

inline void Rasterizer2::rasterPolygonArea(const PointF* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, float minX, float minY, float maxX, float maxY)
{ rasterPolygonAreaXWAA(points, pointCount, polyCount, totalPointCount, color, minX, minY, maxX, maxY); }


//
// Generic helper functions
//

inline void Rasterizer2::updateGradientBrush_getStartEndColors(Pt::uint8_t rgbaStart[4], Pt::uint8_t rgbaEnd[4])
{
    rgbaStart[0] = _brush.color        ().red  () / 257;
    rgbaStart[1] = _brush.color        ().green() / 257;
    rgbaStart[2] = _brush.color        ().blue () / 257;
    rgbaStart[3] = _brush.color        ().alpha() / 257;

    rgbaEnd  [0] = _brush.gradientColor().red  () / 257;
    rgbaEnd  [1] = _brush.gradientColor().green() / 257;
    rgbaEnd  [2] = _brush.gradientColor().blue () / 257;
    rgbaEnd  [3] = _brush.gradientColor().alpha() / 257;
}

inline void Rasterizer2::updateGradientBrush_getCtrRatXY(float& ctrX, float& ctrY, float &xyRat, float& yxRat, Pt::int32_t width, Pt::int32_t height)
{
    ctrX  = width  * 0.5f + _brush.offsetX();
    ctrY  = height * 0.5f + _brush.offsetY();

    xyRat = (ctrX > ctrY) ? (ctrX / ctrY) : 1.0f;
    yxRat = (ctrY > ctrX) ? (ctrY / ctrX) : 1.0f;
}

inline Pt::uint8_t Rasterizer2::patternBuffer1PAlpha(Pt::int32_t idx) const
{ return _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ]; }

inline Pt::uint8_t Rasterizer2::patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale) const
{ return patternBuffer1PAlpha( toPolar(x, y) * scale); }

inline Pt::uint8_t Rasterizer2::patternBuffer1PAlphaPolar(Pt::int32_t x, Pt::int32_t y, float scale, float xyRat) const
{
    const float angle = toPolar(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    return patternBuffer1PAlpha(angle * scale);
}

inline void Rasterizer2::patternBuffer1PAlpha(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t idx, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{
    a0 = (Pt::uint32_t) _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ] * alpha0 / 255;
    a1 = (Pt::uint32_t) _patternBuffer1P[ idx % FIXED_POINT_TO_INT(PATTERN_BUFFER_COUNTER_MAX1P) ] * alpha1 / 255;
}

inline void Rasterizer2::patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{ patternBuffer1PAlpha(a0, a1, toPolar(x, y) * scale, alpha0, alpha1); }

inline void Rasterizer2::patternBuffer1PAlphaPolar(Pt::uint8_t& a0, Pt::uint8_t& a1, Pt::int32_t x, Pt::int32_t y, float scale, float xyRat, Pt::uint8_t alpha0, Pt::uint8_t alpha1) const
{
    const float angle = toPolar(x, y);

    if(xyRat >= 1.0 && angle >= 45) scale /= xyRat;
    if(xyRat <  1.0 && angle <  45) scale *= xyRat;

    patternBuffer1PAlpha(a0, a1, angle * scale, alpha0, alpha1);
}

template<typename T>
inline void Rasterizer2::bubbleSortAscending(T& basket, Pt::int32_t size)
{
    for(Pt::int32_t i = 0; i < size - 1;) {
        if(basket[i] > basket[i + 1]) {
            std::swap(basket[i], basket[i + 1]);
            if(i) --i;
        }
        else {
            ++i;
        }
    }
}


//
// Rasterization helper functions
//

inline void Rasterizer2::fillPixel(Pt::int32_t x, Pt::int32_t y, Pt::int32_t minX, Pt::int32_t minY, Pt::uint8_t alpha)
{
    // Check the clipping
    if(!ClipShapeI::insideXRange(x, _currentClip)) return;
    if(!ClipShapeI::insideYRange(y, _currentClip)) return;

    // Draw the pixels using texture or gradient
    if(_isTexture || _isGradient) {
        const Pt::int32_t bw = _brushImage->width();
        const Pt::int32_t bh = _brushImage->height();
        const Pt::int32_t dx = x - minX;
        const Pt::int32_t dy = y - minY;
        const Pt::int32_t tx = _isGradient ? std::min(bw - 1, dx) : (dx % bw);
        const Pt::int32_t ty = _isGradient ? std::min(bh - 1, dy) : (dy % bh);
        ConstPixel srcPixel(_brushImage->view(), tx, ty);
        Pixel      dstPixel(_image->view(), x, y);
        _image->format().setPixel(dstPixel, srcPixel, _compositionMode, alpha);
    }

    // Draw the pixels using solid color
    else {
        Pixel pixel(_image->view(), x, y);
        _image->format().setPixel(pixel, _brush.color(), _compositionMode, alpha);
    }
}


//
// Polygon-related helper functions
//

template <typename PointT, typename ValueT>
inline void Rasterizer2::getPolygonRectMinMax(const BasicPoint<PointT>* points, 
                                              size_t pointCount, 
                                              ValueT& minX, ValueT& minY, 
                                              ValueT& maxX, ValueT& maxY) const
{
    minX =  MAXIMUM_COORD;
    minY =  MAXIMUM_COORD;
    maxX = -MAXIMUM_COORD;
    maxY = -MAXIMUM_COORD;

    for(size_t i = 0; i < pointCount; ++i) 
    {
        const ValueT x = points[i].x();
        const ValueT y = points[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }
}

template <typename PointT>
inline void Rasterizer2::genClippedPolygonPoints(std::vector< BasicPoint<PointT> >& dst, 
                                                 const BasicPoint<PointT>* src, 
                                                 const size_t pointCount, 
                                                 bool forPolygonOutline) const
{
    for(size_t i = 0; i < pointCount; ++i)
        dst.push_back( BasicPoint<PointT>( src[i].x(), src[i].y() ) );

    if(forPolygonOutline) BasicClipShape<PointT>::clipPolyline(dst, _currentClip);
    else                  BasicClipShape<PointT>::clipPolygon (dst, _currentClip);
}

template <typename PointT, typename ValueT>
inline void Rasterizer2::separateAndClipPolygons(ValueT& minX, ValueT& maxX, 
                                                 ValueT& minY, ValueT& maxY, 
                                                 std::vector< BasicPoint<PointT> >& clippedPoints, 
                                                 std::vector<size_t>& clippedCounts, 
                                                 const BasicPoint<PointT>* points, size_t pointCount) const
{
    // Minimum and maximum coordinate values for all the polygons
    minX =  MAXIMUM_COORD;
    minY =  MAXIMUM_COORD;
    maxX = -MAXIMUM_COORD;
    maxY = -MAXIMUM_COORD;

    // Separate the polygons and clip their coordinates
    size_t startIndex = 0;
    for(size_t i = 0; i <= pointCount; ++i) 
    {
        // Search for the end and/or separator points
        if( i == pointCount || (points[i].x() > MAXIMUM_COORD && points[i].y() > MAXIMUM_COORD) ) 
        {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            
            // Clip the coordinates
            std::vector< BasicPoint<PointT> > clipped;
            genClippedPolygonPoints(clipped, points + startIndex, curPC, false);
            
            if( clipped.empty() ) 
              continue;
            
            // Increment the start index
            startIndex += curPC + 1;
            
            // Calculate the minimum and maximum coordinate values
            ValueT curMinX, curMinY, curMaxX, curMaxY;
            getPolygonRectMinMax(clipped.data(), clipped.size(), curMinX, curMinY, curMaxX, curMaxY);
            if(curMinX < minX) minX = curMinX;
            if(curMinY < minY) minY = curMinY;
            if(curMaxX > maxX) maxX = curMaxX;
            if(curMaxY > maxY) maxY = curMaxY;
            
            // Store the clipped points
            clippedPoints.insert(clippedPoints.end(), clipped.begin(), clipped.end());
            
            // Store the number of points
            clippedCounts.push_back(clipped.size());
        }
    }
}


//
// Arc-related helper functions
//

inline void Rasterizer2::arcUtil_detXWLineDirection(ArcXWLineData& xwLineData)
{
    // Calculate the direction vector
    const Pt::int32_t vx = xwLineData.x2 - xwLineData.x1; // Vector from the begin point to the end point
    const Pt::int32_t vy = xwLineData.y2 - xwLineData.y1; //
    const Pt::int32_t vz = 0;                             //
    const Pt::int32_t rx = 0;                             // Vector from the point of origin (0, 0, 0) that points out of the monitor
    const Pt::int32_t ry = 0;                             //
    const Pt::int32_t rz = 1;                             //
    const Pt::int32_t cx = vy * rz - vz * ry;             // Cross product of the above two vectors
    const Pt::int32_t cy = vz * rx - vx * rz;             //
  //const Pt::int32_t cz = vx * ry - vy * rx;             //

    // Determine the direction that the line is facing to
    xwLineData.faceT = cy < 0;
    xwLineData.faceB = cy > 0;
    xwLineData.faceL = cx < 0;
    xwLineData.faceR = cx > 0;
}

inline bool Rasterizer2::arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBegin, float degEnd, float xyRatio)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    const float angle = toPolar( (x - ctrX), -(y - ctrY) * xyRatio );

    // Both begin and end angle are negative
    if(degBegin < 0 && degEnd < 0) {
        return angle >= (degBegin + 360) && angle <= (degEnd + 360);
    }

    // Begin angle is negative but end angle is positive
    if(degBegin < 0 && degEnd >= 0) {
        if(angle >= (degBegin + 360) && angle <= 360   ) return true;
        if(angle >= 0                && angle <= degEnd) return true;
        return false;
    }

    // Both begin and end angle are positive
    return angle >= degBegin && angle <= degEnd;
}

inline Pt::uint8_t Rasterizer2::arcUtil_pointIsInsideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, Pt::uint8_t alpha, float degBegin, float degEnd, float xyRatio)
{
    // IMPORTANT NOTES:
    //     * The Y coordinate goes from low to high according to the coordinate system being used:
    //           - cartesian coordinate system: from the horizontal axis (the X axis) to the top;
    //           - computer  coordinate system: from the top of the screen to the bottom of the screen;
    //       This will cause sign inversion for trigonometry-based calculations in the Y coordinate.
    //     * The movement from begin angle to end angle must be in counter-clockwise (CCW), otherwise
    //       something wrong will be drawn.

    const float relX  = x - ctrX;
    const float relY  = y - ctrY;
    const float relM  = std::max( ::fabs(relX), ::fabs(relY) );
    const float angle = toPolar(relX, -relY * xyRatio);
    const float limit = 100.0f / relM;

    // Both begin and end angle are negative
    if(degBegin < 0 && degEnd < 0) {
        degBegin += 360;
        degEnd   += 360;
        if(angle >= degBegin && angle <= degEnd) {
            const float db = angle  - degBegin;
            const float de = degEnd - angle;
            const float dm = std::min(db, de);
            if(dm > limit) return alpha;
            return (dm > limit) ? alpha : (alpha * dm / limit);
        }
        return 0;
    }

    // Begin angle is negative but end angle is positive
    if(degBegin < 0 && degEnd >= 0) {
        degBegin += 360;
        if(angle >= degBegin && angle <= 360) {
            const float dm = angle - degBegin;
            if(dm > limit) return alpha;
            return (dm > limit) ? alpha : (alpha * dm / limit);
        }
        if(angle >= 0  && angle <= degEnd) {
            const float dm = degEnd - angle;
            if(dm > limit) return alpha;
            return (dm > limit) ? alpha : (alpha * dm / limit);
        }
        return 0;
    }

    // Both begin and end angle are positive
    if(angle >= degBegin && angle <= degEnd) {
        const float db = angle  - degBegin;
        const float de = degEnd - angle;
        const float dm = std::min(db, de);
        return (dm > limit) ? alpha : (alpha * dm / limit);
    }
    return 0;
}

