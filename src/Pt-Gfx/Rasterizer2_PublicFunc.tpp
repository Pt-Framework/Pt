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
// Access to the pattern buffer
//

inline const Pt::uint8_t* Rasterizer2::patternBufferMP64() const
{ return _patternBufferMP; }


inline void Rasterizer2::strokeNarrowRoundedRect(const RectF& rect, float radius)
{
    const float x1 = rect.topLeft().x();
    const float y1 = rect.topLeft().y();
    const float x2 = rect.bottomRight().x();
    const float y2 = rect.bottomRight().y();

    // line end masks
    DrawLineMask mask_zero;
    memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // pattern state
    Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_COUNTER_START;

    // bottom left corner
    rasterOnePixelQuadraticBezierCurve(
        x1 + radius, y2,
        x1         , y2,
        x1         , y2 - radius,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // left staight line
    rasterOnePixelQuadraticBezierCurve(
        x1, y2 - radius,
        x1, y1 + rect.height() / 2,
        x1, y1 + radius,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // top left corner
    rasterOnePixelQuadraticBezierCurve(
        x1        , y1 + radius,
        x1        , y1,
        x1 + radius, y1,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // top straight line
    rasterOnePixelQuadraticBezierCurve(
        x1 + radius          , y1,
        x1 + rect.width() / 2, y1,
        x2 - radius          , y1,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // top right corner
    rasterOnePixelQuadraticBezierCurve(
        x2 - radius, y1,
        x2         , y1,
        x2         , y1 + radius,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // right straight line
    rasterOnePixelQuadraticBezierCurve(
        x2, y1 + radius,
        x2, y1 + rect.height() / 2,
        x2, y2 - radius,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // bottom right corner
    rasterOnePixelQuadraticBezierCurve(
        x2         , y2 - radius,
        x2         , y2,
        x2 - radius, y2,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );

    // bottom straight line
    rasterOnePixelQuadraticBezierCurve(
        x2 - radius          , y2,
        x1 + rect.width() / 2, y2,
        x1 + radius          , y2,
        _pen.color(),
        _pen.style() == Pen::Solid ? 0 : &fpiCtrInOut,
        &mask_nnp1
    );
}

//
// Stroke polygon outline
//

template <typename PointT>
inline void Rasterizer2::drawNarrowPolyline(const BasicPoint<PointT>* points, size_t pointCount, bool autoClose)
{
    // Check if there are too few points
    if(pointCount < 2) return;

    // Separate the polygons, clip their coordinates, and raster them
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
            genClippedPolygonPoints(clipped, points + startIndex, curPC, true);
            if(autoClose && clipped.back() != clipped[0]) clipped.push_back(points[0]);
            // Increment the start index
            startIndex += curPC + 1;
            
            // Draw the polygon
            rasterOnePixelPolygonOutline(clipped.data(), clipped.size(), _pen.color());
        }
    }
}


inline void Rasterizer2::drawNarrowPolyline2(const PointF* points, size_t pointCount)
{
  // TODO
}

//
// Fill polygon
//

template <typename PointT>
inline void Rasterizer2::penFillPolygon(const BasicPoint<PointT>* points, size_t pointCount)
{
    // Check if there are too few points
    if(pointCount < 3) return;

    // Disable texture and gradient
    const bool isTexture  = _isTexture;
    const bool isGradient = _isGradient;

    _isTexture  = false;
    _isGradient = false;

    // Separate the polygons and clip their coordinates
    typename CnvValueT<PointT>::T minX, minY, maxX, maxY;

    std::vector< BasicPoint<PointT> > clippedPoints;
    std::vector< size_t             > clippedCounts;

    separateAndClipPolygons(minX, maxX, minY, maxY, clippedPoints, clippedCounts, points, pointCount);
    if(clippedPoints.empty()) return;

    // Draw the polygon
    rasterPolygonArea(
        clippedPoints.data(), clippedCounts.data(),
        clippedCounts.size(), clippedPoints.size(),
        _pen.color(), minX, minY, maxX, maxY
    );

    // Restore texture and gradient
    _isTexture  = isTexture;
    _isGradient = isGradient;
}

template <typename PointT>
inline void Rasterizer2::penFillPolygonSeparate(const BasicPoint<PointT>* points, size_t pointCount)
{
    // Check if there are too few points
    if(pointCount < 3) return;

    // Disable texture and gradient
    const bool isTexture  = _isTexture;
    const bool isGradient = _isGradient;

    _isTexture  = false;
    _isGradient = false;

    // Separate the polygons, clip their coordinates, and raster them
    size_t startIndex = 0;

    for(size_t i = 0; i <= pointCount; ++i) {
        // Search for the end and/or separator points
        if( i == pointCount || (points[i].x() > MAXIMUM_COORD && points[i].y() > MAXIMUM_COORD) ) {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            // Clip the coordinates
            std::vector< BasicPoint<PointT> > clipped;
            genClippedPolygonPoints(clipped, points + startIndex, curPC, false);
            // Increment the start index
            startIndex += curPC + 1;
            // Calculate the minimum and maximum coordinate values
            typename CnvValueT<PointT>::T minX, minY, maxX, maxY;
            getPolygonRectMinMax(clipped.data(), clipped.size(), minX, minY, maxX, maxY);
            // Get the number of points for drawing this polygon
            const size_t numPoint[1] = { clipped.size() };
            // Draw the polygon
            rasterPolygonArea(clipped.data(), numPoint, 1, clipped.size(), _pen.color(), minX, minY, maxX, maxY);
        }
    }

    // Restore texture and gradient
    _isTexture  = isTexture;
    _isGradient = isGradient;
}

// OBSOLETE
template <typename PointT>
inline void Rasterizer2::fillPolygon(const BasicPoint<PointT>* points, size_t pointCount)
{
    // Check if there are too few points
    if(pointCount < 3)
        return;

    // Separate the polygons and clip their coordinates
    typename CnvValueT<PointT>::T minX, minY, maxX, maxY;

    std::vector< BasicPoint<PointT> > clippedPoints;
    std::vector< size_t             > clippedCounts;

    separateAndClipPolygons(minX, maxX, minY, maxY,
                            clippedPoints, clippedCounts, points, pointCount);
    if( clippedPoints.empty() )
        return;

    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    // Draw the polygon
    rasterPolygonArea(
        clippedPoints.data(), clippedCounts.data(),
        clippedCounts.size(), clippedPoints.size(),
        _brush.color(), minX, minY, maxX, maxY
    );
}

//
// Same as fillPolygon2, just uses pen and temporarily turns off gradient
// and texture filling
//
inline void Rasterizer2::fillLine(const PointF* ps, std::size_t n)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<PointF> clippedPolygon(ps, ps + n);

    BasicClipShape<double>::clipPolygon(clippedPolygon, _currentClip);

    for(size_t j = 0; j < clippedPolygon.size(); ++j)
    {
        const double x = clippedPolygon[j].x();
        const double y = clippedPolygon[j].y();

        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    // Disable texture and gradient
    const bool isTexture  = _isTexture;
    const bool isGradient = _isGradient;

    _isTexture  = false;
    _isGradient = false;

    if( this->isAntiAliasing() )
    {
        rasterPolygonXWAA(&clippedPolygon[0], clippedPolygon.size(),
                           _pen.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonNoAA(&clippedPolygon[0], clippedPolygon.size(),
                           _pen.color(), minX, minY, maxX, maxY);
    }

    // Restore texture and gradient
    _isTexture  = isTexture;
    _isGradient = isGradient;
}

//
// Same as fillPolygons, just uses pen and temporarily turns off gradient
// and texture filling
//
inline void Rasterizer2::fillPolyline(const std::vector<Polygon>& polygons)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<Polygon> clippedPolygons = polygons;

    for(size_t i = 0; i < clippedPolygons.size(); ++i)
    {
        Polygon& polygon = clippedPolygons[i];

        BasicClipShape<double>::clipPolygon(polygon.points(), _currentClip);

        for(size_t j = 0; j < polygon.size(); ++j)
        {
            const double x = polygon.at(j).x();
            const double y = polygon.at(j).y();

            if(x < minX) minX = x;
            if(y < minY) minY = y;
            if(x > maxX) maxX = x;
            if(y > maxY) maxY = y;
        }
    }

    // Disable texture and gradient
    const bool isTexture  = _isTexture;
    const bool isGradient = _isGradient;

    _isTexture  = false;
    _isGradient = false;

    if( this->isAntiAliasing() )
    {
        rasterPolygonsXWAA(clippedPolygons, _pen.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonsNoAA(clippedPolygons, _pen.color(), minX, minY, maxX, maxY);
    }

    // Restore texture and gradient
    _isTexture  = isTexture;
    _isGradient = isGradient;
}


inline void Rasterizer2::fillPolygon2(const PointF* ps, std::size_t n)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<PointF> clippedPolygon(ps, ps + n);

    BasicClipShape<double>::clipPolygon(clippedPolygon, _currentClip);

    for(size_t j = 0; j < clippedPolygon.size(); ++j)
    {
        const double x = clippedPolygon[j].x();
        const double y = clippedPolygon[j].y();

        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    if( this->isAntiAliasing() )
    {
        rasterPolygonXWAA(&clippedPolygon[0], clippedPolygon.size(),
                           _brush.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonNoAA(&clippedPolygon[0], clippedPolygon.size(),
                           _brush.color(), minX, minY, maxX, maxY);
    }
}


inline void Rasterizer2::fillPolygons(const std::vector<Polygon>& polygons)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<Polygon> clippedPolygons = polygons;

    for(size_t i = 0; i < clippedPolygons.size(); ++i)
    {
        Polygon& polygon = clippedPolygons[i];

        BasicClipShape<double>::clipPolygon(polygon.points(), _currentClip);

        for(size_t j = 0; j < polygon.size(); ++j)
        {
            const double x = polygon.at(j).x();
            const double y = polygon.at(j).y();

            if(x < minX) minX = x;
            if(y < minY) minY = y;
            if(x > maxX) maxX = x;
            if(y > maxY) maxY = y;
        }
    }

    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    if( this->isAntiAliasing() )
    {
        rasterPolygonsXWAA(clippedPolygons, _brush.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonsNoAA(clippedPolygons, _brush.color(), minX, minY, maxX, maxY);
    }
}
