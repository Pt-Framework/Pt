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

#include "FreeType2.h"
#include "ArcMode.h"
#include "Rasterizer2.h"
#include "LineRenderer.h"
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/ImagePainter2.h>
#include "clipper_aj/clipper.hpp"

namespace Pt {
namespace Gfx {


void ImagePainter2::drawThickPolyline_impl(const PointF* ps, const size_t pointCount,
                                           bool autoClose, const int32_t* segmentIndexMarker)
{
    // Check if there is no actual point
    if( ! pointCount)
        return;

    // Prepare the buffer
    std::vector<PointF> pointsF, pointsT;
    pointsF.reserve( pointCount * _rasterizer->pen().size() );

    // Is the pen solid?
    const bool solidPen = (_rasterizer->pen().style() == Pen::Solid);

    // Separate the polygons convert them and recombine them
    size_t startIndex = 0;

    for(size_t i = 0; i <= pointCount; ++i) 
    {
        // Search for the end and/or separator points
        if( i == pointCount || (ps[i].x() > MAXIMUM_COORD && ps[i].y() > MAXIMUM_COORD) ) 
        {
            // Get the base pointer and the number of points for this polygon
            const PointF* basePtr = ps + startIndex;
                  size_t  curPCnt = i - startIndex;

            // Update the start index
            startIndex = i + 1;

            // Determine if this polygon is a closed polygon
            bool closedPolygon = autoClose;

            // Thicken polygon with solid line
            if(solidPen) 
            {
                if(basePtr[0] == basePtr[curPCnt - 1]) {
                    closedPolygon = true;
                    --curPCnt;
                }
                if(closedPolygon) {
                    if( ! thickenSolidClosedPolygon(pointsF, basePtr,
                                                    curPCnt, segmentIndexMarker) )
                        return;
                }
                else {
                    if( ! thickenSolidOpenPolygon(pointsF, basePtr,
                                                  curPCnt, segmentIndexMarker) )
                        return;
                }
            }
            // Thicken polygon with patterned line
            else {
                if(closedPolygon && basePtr[0] != basePtr[curPCnt - 1]) {
                    pointsT.clear();

                    for(size_t j = 0; j < curPCnt; ++j) pointsT.push_back(*(basePtr + j));
                        pointsT.push_back(*basePtr);

                    thickenPatternedPolygon(pointsF, pointsT.data(), pointsT.size());
                }
                else {
                    thickenPatternedPolygon(pointsF, basePtr, curPCnt);
                }
            }

            // Use anti-aliasing
            if( _rasterizer->isAntiAliasing() ) 
            {
                // Remove duplicates
                std::vector<PointF> points;
                deduplicatePointsF(points, pointsF.data(), pointsF.size());

                // Rasterize the polygon
                if(solidPen && closedPolygon) {
                    _rasterizer->penFillPolygon(points.data(), points.size());
                }
                else {
                    _rasterizer->penFillPolygonSeparate(points.data(), points.size());
                }
            }
            // Do not use use anti-aliasing
            else {
                // Round the points and remove duplicates
                std::vector<Point> points;
                cnvPointsFToPointsDeduplicate(points, pointsF.data(), pointsF.size());

                // Rasterize the polygon
                if(solidPen && closedPolygon) {
                    _rasterizer->penFillPolygon(points.data(), points.size());
                }
                else {
                    _rasterizer->penFillPolygonSeparate(points.data(), points.size());
                }
            }
        }
    }
}


void ImagePainter2::drawWidePolyline(const PointF* points, const size_t pointCount)
{
    //LineRenderer lr;

    // TODO: set pattern only once
    //if( _rasterizer->pen().style() != Pen::Solid )
    //    lr.setPattern( _rasterizer->pen().style() );
    
    std::vector<Polygon> polygons;
    _lr->renderWidePolyline( polygons, points, pointCount, _rasterizer->pen() );

    bool isSolid = _rasterizer->pen().style() == Pen::Solid;
    bool isClosed = points[0] == points[pointCount - 1];

    if( isSolid && isClosed )
    {
        _rasterizer->fillPolyline( polygons );
    }
    else
    {
        for(std::size_t n = 0; n < polygons.size(); ++n)
        {
            const std::vector<PointF>& pp = polygons[n].points();
            _rasterizer->fillLine( &pp[0], pp.size() );
        }
    }
}


void ImagePainter2::generateSolidLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap)
{
    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, _rasterizer->pen().size());

    // Generate points (CCW)
    // --- Begin point ---
    if(openingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundHoleCap     : generateLineRoundHoleCap    (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::Arrow1Cap        : generateLineArrow1Cap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::Arrow2Cap        : generateLineArrow2Cap       (dst, x1, y1,     dx, dy, nx, ny); break;
            default                    : openingCap = false;
        }
    }
    if(!openingCap) generateLineButtCap(dst, x1, y1, nx, ny);
    // --- End point ---
    if(closingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundHoleCap     : generateLineRoundHoleCap    (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::Arrow1Cap        : generateLineArrow1Cap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::Arrow2Cap        : generateLineArrow2Cap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            default                    : closingCap = false;
        }
    }
    if(!closingCap) generateLineButtCap(dst, x2, y2, -nx, -ny);
}


void ImagePainter2::deduplicatePointsF(std::vector<PointF>& dst, const PointF* src, const size_t pointCount)
{
    // Check if there is no actual point
    if(!pointCount) return;

    // Prepare the buffer
    const size_t ofs = dst.size();
    dst.resize(ofs + pointCount);

    // Process the coordinates
    size_t putCnt = 0;
    for(size_t i = 0; i < pointCount; ++i) {
        // Round the coordinates
        const double x = std::floor(src[i].x() * VecResScaleUp + 0.5);
        const double y = std::floor(src[i].y() * VecResScaleUp + 0.5);
        // Skip duplicated coordinates
        if( ofs + putCnt >= 1 && dst[ofs + putCnt - 1].x() == x && dst[ofs + putCnt - 1].y() == y ) continue;
        // Store the coordinate and increment the "put" counter
        dst[ofs + putCnt].set(x, y);
        ++putCnt;
    }

    // Discard the last point if it has the same coordinate with the first point
    if(dst[ofs] == dst[ofs + putCnt - 1]) --putCnt;

    // Resize the buffer to discard unused elements
    dst.resize(ofs + putCnt);

    // Scale back the coordinates
    for(size_t i = 0; i < dst.size(); ++i) {
        dst[i].set(
            dst[i].x() * VecResScaleDn,
            dst[i].y() * VecResScaleDn
        );
    }
}


void ImagePainter2::generatePatternedSingleLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, Pt::int32_t& piCtrInOut)
{
    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, _rasterizer->pen().size());

    // Get the pattern buffer and calculate the number of "pattern" segments
    const Pt::uint8_t* pBuff = _rasterizer->patternBufferMP64();
    const float        xLen  = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    const float        yLen  = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    const float        lLen  = sqrt(xLen * xLen + yLen * yLen);
    const size_t       nSegs = (size_t) lround(lLen / wh) * 2;
    const float        xInc  = (x2 - x1) / nSegs;
    const float        yInc  = (y2 - y1) / nSegs;

    // Generate the segments
    Pt::uint8_t prvPat = 0;
    float       xs     = x1;
    float       ys     = y1;
    for(size_t i = 0; i <= nSegs; ++i) {
        // Get the pattern
        const Pt::uint8_t curPat = pBuff[piCtrInOut++];
        if(piCtrInOut >= PATTERN_BUFFER_COUNTER_MAXMP) piCtrInOut -= PATTERN_BUFFER_COUNTER_MAXMP;
        // Determine whether we should draw this segment as well as its coordinate
        const bool draw = (!curPat && prvPat);
        if(curPat && !prvPat) {
            x1 = xs;
            y1 = ys;
        }
        else if(draw) {
            x2 = xs;
            y2 = ys;
            if(_rasterizer->pen().capStyle() == Pen::ButtCap) {
                x2 += xInc;
                y2 += yInc;
            }
        }
        prvPat = curPat;
        // Update the coordinates
        xs += xInc;
        ys += yInc;
        // Skip if we are not going to draw this segment
        if(!draw) continue;
        // Add polygon separator point as needed
        if(!dst.empty()) dst.push_back(Painter::PolygonSeparatorPointF);
        // Generate points (CCW)
        // --- Begin point ---
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundHoleCap     : generateLineRoundHoleCap    (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::Arrow1Cap        : generateLineArrow1Cap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::Arrow2Cap        : generateLineArrow2Cap       (dst, x1, y1,     dx, dy, nx, ny); break;
            default                    : generateLineButtCap         (dst, x1, y1,             nx, ny); break;
        }
        // --- End point ---
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundHoleCap     : generateLineRoundHoleCap    (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::Arrow1Cap        : generateLineArrow1Cap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::Arrow2Cap        : generateLineArrow2Cap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            default                    : generateLineButtCap         (dst, x2, y2,               -nx, -ny); break;
        }
    }
}


void ImagePainter2::cnvPointsFToPointsDeduplicate(std::vector<Point>& dst, const PointF* src, const size_t pointCount)
{
    // Check if there is no actual point
    if(!pointCount) return;

    // Prepare the buffer
    const size_t ofs = dst.size();
    dst.resize(ofs + pointCount);

    // Process the coordinates
    size_t putCnt = 0;
    for(size_t i = 0; i < pointCount; ++i) {
        // Round the coordinates
        const Pt::int32_t x = lround(src[i].x());
        const Pt::int32_t y = lround(src[i].y());
        // Skip duplicated coordinates
        if( ofs + putCnt >= 1 && dst[ofs + putCnt - 1].x() == x && dst[ofs + putCnt - 1].y() == y ) 
        {
            continue;
        }
        // Store the coordinate and increment the "put" counter
        dst[ofs + putCnt].set(x, y);
        ++putCnt;
    }

    // Discard the last point if it has the same coordinate with the first point
    if(dst[ofs] == dst[ofs + putCnt - 1]) --putCnt;

    // Resize the buffer to discard unused elements
    dst.resize(ofs + putCnt);
}


void ImagePainter2::setFontDir(const Pt::System::Path& path)
{
  FreeType2::instance().setFontDir(path);
}


void ImagePainter2::setDefaultFont(const std::string& f)
{
  FreeType2::instance().setDefaultFont(f);
}


std::string ImagePainter2::defaultFont()
{
  return FreeType2::instance().defaultFont();
}


std::vector<std::string> ImagePainter2::fontNames()
{
  return FreeType2::instance().fontNames();
}


ImagePainter2::ImagePainter2(Image& image)
: _rasterizer( new Rasterizer2(image) )
, _lr( new LineRenderer )
{
    setAntiAliasing(true);
}


ImagePainter2::~ImagePainter2()
{
  delete _rasterizer;
  delete _lr;
}


void ImagePainter2::setAntiAliasing(bool on)
{
  _rasterizer->setAntiAliasing(on);
}


void ImagePainter2::setImage(Image& image)
{
  _rasterizer->setImage(image);
}


const ImageFormat& ImagePainter2::format() const
{
  return _rasterizer->format();
}


void ImagePainter2::setCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


const CompositionMode& ImagePainter2::compositionMode() const
{
  return _rasterizer->compositionMode();
}


void ImagePainter2::setClip(const RectF& clip)
{
    Rect roundedClip( Point( lround( clip.x() ),
                             lround( clip.y() ) ),
                      Size( lround( clip.width() ),
                             lround( clip.height() ) ) );

    _rasterizer->setClip(roundedClip);

    _clip = clip;
}


const Gfx::RectF& ImagePainter2::clip() const
{
    return _clip;
}


void ImagePainter2::setPen( const Pen& pen )
{
    _rasterizer->setPen(pen);

    if( pen.style() != Pen::Solid )
        _lr->setPattern( pen.style() );
}


const Pen& ImagePainter2::pen() const
{
  return _rasterizer->pen();
}


void ImagePainter2::setBrush(const Brush& brush)
{
    _rasterizer->setBrush(brush);
}


const Brush& ImagePainter2::brush() const
{
    return _rasterizer->brush();
}


void ImagePainter2::setFont(const Font& font)
{
    _rasterizer->setFont( font );
}


const Font& ImagePainter2::font() const
{
    return _rasterizer->font();
}


void ImagePainter2::drawImage( const PointF& to, const Image& image )
{
    const Point to_( lround(to.x()), lround(to.y()) );

    _rasterizer->drawImage(to_, image);

}


void ImagePainter2::drawImage( const PointF& to, const Image& image, const RectF& imageRect )
{
    const Point to_( lround(to.x()), lround(to.y()) );
    const Rect  ir_(
        Point( lround(imageRect.    x()), lround(imageRect.     y()) ),
        Size ( lround(imageRect.width()), lround(imageRect.height()) )
    );

    _rasterizer->drawImage(to_, image, ir_);

}

void ImagePainter2::drawText( const PointF& to, const String& text )
{

    Point to_( lround(to.x()), lround(to.y()) );
    Transform identity;
    _rasterizer->drawText(to_, text, identity);
}


FontMetrics ImagePainter2::fontMetrics(const String& text) const
{
    return _rasterizer->fontMetrics( text );
}


FontMetrics ImagePainter2::fontMetrics( const Font& font, const Pt::String& text )
{
    return Rasterizer2::fontMetrics(font, text);
}


void ImagePainter2::drawLine( const PointF& from, const PointF& to )
{
    // Rasterize one-pixel line
    if(_rasterizer->pen().size() == 1)
    {
        // Convert the points
        const Point a( lround(from.x()), lround(from.y()) );
        const Point b( lround(to  .x()), lround(to  .y()) );

        // Rasterize the line
        _rasterizer->strokeOnePixelLine(a, b, 0);
        return;
    }

#if 1
    std::vector<Polygon> polygons;

    //LineRenderer lr;

    // TODO: set pattern only once
    //if( _rasterizer->pen().style() != Pen::Solid )
    //    lr.setPattern( _rasterizer->pen().style() );
    
    PointF points[2] = { from, to };

    _lr->renderWidePolyline( polygons, points, 2, _rasterizer->pen() );

    // no performance benefit to use renderWideLine
    //lr.renderWideLine( polygons, from, to, _rasterizer->pen() );

    for(std::size_t n = 0; n < polygons.size(); ++n)
    {
        const std::vector<PointF>& pp = polygons[n].points();
        _rasterizer->fillLine( &pp[0], pp.size() );
    }
    
#else

    // Generate a polygon that represents the thick line
    std::vector<PointF> pointsF;

    if(_rasterizer->pen().style() == Pen::Solid)
    {
        generateSolidLineSegment(pointsF, from.x(), from.y(), to.x(), to.y(), true, true);
    }
    else
    {
        Pt::int32_t piCtrInOut = 0;
        generatePatternedSingleLineSegment(pointsF, from.x(), from.y(), to.x(), to.y(), piCtrInOut);
    }

    // Use anti-aliasing
    if( _rasterizer->isAntiAliasing() )
    {
        // Remove duplicates
        std::vector<PointF> points;
        deduplicatePointsF(points, pointsF.data(), pointsF.size());
        // Rasterize the polygon
        _rasterizer->penFillPolygonSeparate(points.data(), points.size());
    }
    // Do not use use anti-aliasing
    else
    {
        // Round the points and remove duplicates
        std::vector<Point> points;
        cnvPointsFToPointsDeduplicate(points, pointsF.data(), pointsF.size());
        // Rasterize the polygon
        _rasterizer->penFillPolygonSeparate(points.data(), points.size());
    }
#endif
}


void ImagePainter2::drawPolyline(const PointF* ps, const size_t pointCount)
{
#if 1
    if(_rasterizer->pen().size() == 1) 
    {          
        _rasterizer->drawNarrowPolyline2(ps, pointCount);
    }
    else
    {
        drawWidePolyline(ps, pointCount);
    }
#else
    bool autoClose = false;

    // Rasterize one-pixel polyline
    if(_rasterizer->pen().size() == 1) 
    {
        // Use anti-aliasing
        if( _rasterizer->isAntiAliasing() ) 
        {
            // Remove duplicates
            std::vector<PointF> pointsF;
            deduplicatePointsF(pointsF, ps, pointCount);
            
            // Rasterize the polygon
            if(ps[0] == ps[pointCount - 1]) autoClose = true;
            
            _rasterizer->drawNarrowPolyline(ps, pointCount, autoClose);
        }
        else // Do not use use anti-aliasing
        {
            // Round the points and remove duplicates
            std::vector<Point> points;
            cnvPointsFToPointsDeduplicate(points, ps, pointCount);
            
            // Rasterize the polygon
            if(ps[0] == ps[pointCount - 1]) 
                autoClose = true;
            
            _rasterizer->drawNarrowPolyline(points.data(), points.size(), autoClose);
        }
        
        return;
    }

    drawThickPolyline_impl(ps, pointCount, autoClose, 0);
#endif
}


void ImagePainter2::drawRect( const RectF& rect )
{
    // Rasterize one-pixel rectangle
    if(_rasterizer->pen().size() == 1) {
        // Convert the points
        const Point tl( lround(rect.topLeft    ().x()), lround(rect.topLeft    ().y()) );
        const Point br( lround(rect.bottomRight().x()), lround(rect.bottomRight().y()) );
        // Rasterize the rectangle
        _rasterizer->strokeOnePixelRect(tl, br);
        return;
    }

    // Generate and draw a polyline that represents the rectangle
    const PointF pointsF[5] = {
        rect.bottomLeft(), 
        rect.bottomRight(), 
        rect.topRight(), 
        rect.topLeft(), 
        rect.bottomLeft()
    };

    drawPolyline(pointsF, 5);
}


void ImagePainter2::drawRoundedRect( const RectF& rect, float radius )
{
    // Extract the coordinates
    const float x1 = rect.topLeft    ().x();
    const float y1 = rect.topLeft    ().y();
    const float x2 = rect.bottomRight().x();
    const float y2 = rect.bottomRight().y();

    // Rasterize one-pixel round rectangle
    if(_rasterizer->pen().size() == 1)
    {
        _rasterizer->strokeNarrowRoundedRect(rect, radius);
        return;
    }
    
#if 1
    // Save the original pen and create a new pen with bevel join
    const Pen orgPen = _rasterizer->pen();
    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::BevelJoin);

    std::vector<Polygon> polygons;
    _lr->renderRoundedRect(polygons, rect, radius, newPen);

    // Draw the polygon
    //_rasterizer->setPen(newPen);

    _rasterizer->fillPolyline( polygons );

    //_rasterizer->setPen(orgPen);

#else

    // Generate a polygon that represents the rounded-rectangle
    std::vector<PointF> pointsF;
    generateRoundRectPoints(pointsF, x1, y1, x2, y2, radius, Pt::lround(ceil(_rasterizer->pen().size() * 0.5f)));

    // Save the original pen and create a new pen with bevel join
    const Pen orgPen = _rasterizer->pen();

    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::BevelJoin);

    // Draw the polygon
    _rasterizer->setPen(newPen);

    drawThickPolyline_impl(pointsF.data(), pointsF.size(), true, 0);

    _rasterizer->setPen(orgPen);

#endif
}


#if 1

void ImagePainter2::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    // Rasterize one-pixel ellipse
    if(_rasterizer->pen().size() == 1) 
    {
        // Convert the points
        const Point tl( lround(topLeft.x    ()), lround(topLeft.y     ()) );
        const Size  sz( lround(size   .width()), lround(size   .height()) );
        
        // Rasterize the ellipse
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, 0, 0, ArcMode::Open);
        return;
    }

    // Save the original pen and create a new pen with bevel join
    Pen orgPen = _rasterizer->pen();
    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::BevelJoin);

    std::vector<Polygon> polygons;
    _lr->renderEllipse(polygons, topLeft, size, newPen);

    //_rasterizer->setPen(newPen);

    bool isSolid = _rasterizer->pen().style() == Pen::Solid;

    if( isSolid )
    {
        _rasterizer->fillPolyline( polygons );
    }
    else
    {
        for(std::size_t n = 0; n < polygons.size(); ++n)
        {
            const std::vector<PointF>& pp = polygons[n].points();
            _rasterizer->fillLine( &pp[0], pp.size() );
        }
    }

    //_rasterizer->setPen(orgPen);
}

#else // old code

void ImagePainter2::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    // Rasterize one-pixel ellipse
    if(_rasterizer->pen().size() == 1) 
    {
        // Convert the points
        const Point tl( lround(topLeft.x    ()), lround(topLeft.y     ()) );
        const Size  sz( lround(size   .width()), lround(size   .height()) );
        // Rasterize the ellipse
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, 0, 0, ArcMode::Open);
        return;
    }

    // Calculate the ellipse's parameters
    const size_t      penSize  = _rasterizer->pen().size();
    const Pt::int32_t radiusX  = size.width () / 2;
    const Pt::int32_t radiusY  = size.height() / 2;
    const Pt::int32_t centerX  = topLeft.x() + radiusX;
    const Pt::int32_t centerY  = topLeft.y() + radiusY;

    // Save the original pen and create a new pen with bevel join
    const Pen orgPen = _rasterizer->pen();

    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::BevelJoin);

    if(_rasterizer->pen().style() == Pen::Solid) 
    {
        // Calculate the additional ellipse's parameters
        const Pt::int32_t radiusXo = ( size.width () + penSize ) / 2;
        const Pt::int32_t radiusYo = ( size.height() + penSize ) / 2;
        const Pt::int32_t radiusXi = ( size.width () - penSize ) / 2;
        const Pt::int32_t radiusYi = ( size.height() - penSize ) / 2;
        
        // Generate a polygon that approximates the ellipse
        std::vector<PointF> pointsF;
        generateEllipsePoints(pointsF, radiusXo, radiusYo, centerX, centerY, 0);
        
        pointsF.push_back(Painter::PolygonSeparatorPointF);
        generateEllipsePoints(pointsF, radiusXi, radiusYi, centerX, centerY, 0);
        
        // Use anti-aliasing
        if( _rasterizer->isAntiAliasing() ) {
            // Remove duplicates
            std::vector<PointF> points;
            deduplicatePointsF(points, pointsF.data(), pointsF.size());
            // Rasterize the polygon
            _rasterizer->setPen(newPen);
            _rasterizer->penFillPolygon(points.data(), points.size());
            _rasterizer->setPen(orgPen);
        }
        // Do not use use anti-aliasing
        else 
        {
            // Round the points and remove duplicates
            std::vector<Point> points;
            cnvPointsFToPointsDeduplicate(points, pointsF.data(), pointsF.size());
            
            // Rasterize the polygon
            _rasterizer->setPen(newPen);
            _rasterizer->penFillPolygon(points.data(), points.size());
            _rasterizer->setPen(orgPen);
        }
    }
    else // Patterned
    {
        // Generate a polygon that approximates the ellipse
        std::vector<PointF> pointsF;
        generateEllipsePoints(pointsF, radiusX, radiusY, centerX, centerY, newPen.size());
        
        // Rasterize the polygon
        _rasterizer->setPen(newPen);
        drawThickPolyline_impl(pointsF.data(), pointsF.size(), false, 0);
        _rasterizer->setPen(orgPen);
    }
}

#endif


void ImagePainter2::drawArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    drawArc(topLeft, size, degBegin, degEnd, ArcMode::Open);
}


void ImagePainter2::drawChord(const PointF& topLeft, const SizeF& size,  float degBegin, float degEnd)
{
    drawArc(topLeft, size, degBegin, degEnd, ArcMode::Chord);
}


void ImagePainter2::drawPie(const PointF& topLeft, const SizeF& size,  float degBegin, float degEnd)
{
    drawArc(topLeft, size, degBegin, degEnd, ArcMode::Pie);
}

#if 1

void ImagePainter2::drawArc( const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd, const ArcMode& arcMode)
{
    // Rasterize one-pixel arc
    if(_rasterizer->pen().size() == 1) 
    {
        // Convert the points
        const Point tl( lround(topLeft.x    ()), lround(topLeft.y     ()) );
        const Size  sz( lround(size   .width()), lround(size   .height()) );
        // Rasterize the arc
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, degBegin, degEnd, arcMode);
        return;
    }

    // Save the original pen and create a new pen with bevel join
    const Pen orgPen = _rasterizer->pen();
    Pen newPen = orgPen;
    //newPen.setJoinStyle(Pen::BevelJoin);
    //_rasterizer->setPen(newPen);

    std::vector<Polygon> polygons;
    _lr->renderArc(polygons, arcMode, topLeft, size, degBegin, degEnd, newPen);

    bool isSolid = _rasterizer->pen().style() == Pen::Solid;
    bool isClosed = arcMode != ArcMode::Open;

    if( isSolid && isClosed )
    {
        _rasterizer->fillPolyline( polygons );
    }
    else
    {
        for(std::size_t n = 0; n < polygons.size(); ++n)
        {
            const std::vector<PointF>& pp = polygons[n].points();
            _rasterizer->fillLine( &pp[0], pp.size() );
        }
    }

    //_rasterizer->setPen(orgPen);
}

#else // old code

void ImagePainter2::drawArc( const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd, const ArcMode& arcMode)
{
    // Rasterize one-pixel arc
    if(_rasterizer->pen().size() == 1) 
    {
        // Convert the points
        const Point tl( lround(topLeft.x    ()), lround(topLeft.y     ()) );
        const Size  sz( lround(size   .width()), lround(size   .height()) );
        // Rasterize the arc
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, degBegin, degEnd, arcMode);
        return;
    }

    // Ensure that the begin angle is within the acceptable range
    while(degBegin < -360.0f) degBegin += 360.0f;
    while(degBegin >  360.0f) degBegin -= 360.0f;

    // Ensure that the end angle is within the acceptable range
    while(degEnd < -360.0f) degEnd += 360.0f;
    while(degEnd >  360.0f) degEnd -= 360.0f;

    // Calculate the coordinate shift
    const size_t penSize  = _rasterizer->pen().size();
    const size_t penSize2 = penSize / 2;
    const float  degMid   = (degBegin + degEnd) / 2.0f * DegToRadF;
    const float  shiftX   = std::cos(degMid);
    const float  shiftY   = std::sin(degMid);
    const float  shiftXps = shiftX * penSize2;
    const float  shiftYps = shiftY * penSize2;

    // Calculate the angle adjustment factor
    const float aafa = size.width () / 2.0f;
    const float aafb = size.height() / 2.0f;
    const float aafc = Pt::piDouble<float>() * sqrt( (aafa * aafa + aafb * aafb) / 2.0f );
    const float aafd = 360.0f * penSize2 / aafc;

    // Calculate the arc's parameters
    const Pt::int32_t radiusX = size.width () / 2;
    const Pt::int32_t radiusY = size.height() / 2;
    const Pt::int32_t centerX = topLeft.x() + radiusX;
    const Pt::int32_t centerY = topLeft.y() + radiusY;

    // Save the original pen and create a new pen with bevel join
    const Pen orgPen = _rasterizer->pen();

    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::BevelJoin);

    // Solid
    if(newPen.style() == Pen::Solid) 
    {
        // Calculate the additional arc's parameters
        const Pt::int32_t radiusXo   = ( size.width () + penSize ) / 2;
        const Pt::int32_t radiusYo   = ( size.height() + penSize ) / 2;
        const Pt::int32_t radiusXi   = ( size.width () - penSize ) / 2;
        const Pt::int32_t radiusYi   = ( size.height() - penSize ) / 2;
        const Pt::int32_t centerXsub = lround(centerX - shiftXps);
        const Pt::int32_t centerYsub = lround(centerY - shiftYps);
        const Pt::int32_t centerXadd = lround(centerX + shiftXps);
        const Pt::int32_t centerYadd = lround(centerY + shiftYps);
        
        // The arc's points
        std::vector<PointF> pointsF;
        
        // Generate a polygon that approximates the arc
        if(arcMode == ArcMode::Chord) 
        {
            // The arc's "outside" lines
            generateArcPoints(pointsF, radiusXo, radiusYo, centerX, centerY, degBegin, degEnd, 0);
            pointsF.push_back(pointsF[0]);
            
            // The arc's "inside" lines
            pointsF.push_back(Painter::PolygonSeparatorPointF);
            
            const size_t fp = pointsF.size();
            generateArcPoints(pointsF, radiusXi, radiusYi, centerX + shiftX, centerY + shiftY, degBegin, degEnd, 0);
            pointsF.push_back(pointsF[fp]);
        }
        else if(arcMode == ArcMode::Pie) 
        {
            // Calculate the adjusted angle
            const float odegBegin = (degBegin < 0) ? (degBegin - aafd) : (degBegin + aafd);
            const float odegEnd   = (degEnd   < 0) ? (degEnd   - aafd) : (degEnd   + aafd);
            const float idegBegin = (degBegin < 0) ? (degBegin + aafd) : (degBegin - aafd);
            const float idegEnd   = (degEnd   < 0) ? (degEnd   + aafd) : (degEnd   - aafd);
            
            // The arc's "outside" lines
            generateArcPoints(pointsF, radiusXo, radiusYo, centerX, centerY, odegBegin, odegEnd, 0);
            pointsF.push_back(PointF(centerXsub, centerYsub));
            pointsF.push_back(pointsF[0]);
            
            // The arc's "inside" lines
            pointsF.push_back(Painter::PolygonSeparatorPointF);
            
            const size_t fp = pointsF.size();
            generateArcPoints(pointsF, radiusXi, radiusYi, centerX, centerY, idegBegin, idegEnd, 0);
            pointsF.push_back(PointF(centerXadd, centerYadd));
            pointsF.push_back(pointsF[fp]);
        }
        else // ArcMode::Open
        { 
            // The arc's "inside" and "outside" lines
            std::vector<PointF> inner, outer;
            if(newPen.capStyle() == Pen::Arrow2Cap) {
                // Calculate the adjusted angle
                const float adegBegin = (degBegin < 0) ? (degBegin - aafd) : (degBegin + aafd);
                const float adegEnd   = (degEnd   < 0) ? (degEnd   + aafd) : (degEnd   - aafd);
                // Generate the points
                generateArcPoints(inner, radiusXi, radiusYi, centerX, centerY, adegBegin, adegEnd, 0);
                generateArcPoints(outer, radiusXo, radiusYo, centerX, centerY, adegBegin, adegEnd, 0);
            }
            else {
                generateArcPoints(inner, radiusXi, radiusYi, centerX, centerY, degBegin, degEnd, 0);
                generateArcPoints(outer, radiusXo, radiusYo, centerX, centerY, degBegin, degEnd, 0);
            }
            
            // Combine the arc's lines and add caps
            combineLinePointsAndAddCaps(pointsF, inner, outer, newPen.capStyle(), newPen.capStyle(), penSize);
        }
        
        // Use anti-aliasing
        if( _rasterizer->isAntiAliasing() )
        {
            // Remove duplicates
            std::vector<PointF> points;
            deduplicatePointsF(points, pointsF.data(), pointsF.size());
            
            // Rasterize the polygon
            _rasterizer->setPen(newPen);
            _rasterizer->penFillPolygon(points.data(), points.size());
            _rasterizer->setPen(orgPen);
        }
        else // Do not use use anti-aliasing
        {
            // Round the points and remove duplicates
            std::vector<Point> points;
            cnvPointsFToPointsDeduplicate(points, pointsF.data(), pointsF.size());
            
            // Rasterize the polygon
            _rasterizer->setPen(newPen);
            _rasterizer->penFillPolygon(points.data(), points.size());
            _rasterizer->setPen(orgPen);
        }
    }
    else // Patterned
    {
        // Generate a polygon that approximates the arc
        std::vector<PointF> pointsF;
        if(arcMode == ArcMode::Chord) 
        {
            generateArcPoints(pointsF, radiusX, radiusY, centerX, centerY, degBegin, degEnd, newPen.size());
            pointsF.push_back( pointsF[0] );
        }
        else if(arcMode == ArcMode::Pie) 
        {
            pointsF.push_back( PointF(centerX, centerY) );
            generateArcPoints(pointsF, radiusX, radiusY, centerX, centerY, degBegin, degEnd, newPen.size());
            pointsF.push_back( PointF(centerX, centerY) );
        }
        else // ArcMode::Open
        { 
            if(newPen.capStyle() == Pen::Arrow2Cap) {
                // Calculate the adjusted angle
                const float adegBegin = (degBegin < 0) ? (degBegin - aafd) : (degBegin + aafd);
                const float adegEnd   = (degEnd   < 0) ? (degEnd   + aafd) : (degEnd   - aafd);
                // Generate the points
                generateArcPoints(pointsF, radiusX, radiusY, centerX, centerY, adegBegin, adegEnd, newPen.size());
            }
            else {
                generateArcPoints(pointsF, radiusX, radiusY, centerX, centerY, degBegin, degEnd, newPen.size());
            }
        }
        
        // Rasterize the polygon
        _rasterizer->setPen(newPen);
        drawThickPolyline_impl(pointsF.data(), pointsF.size(), false, 0);
        _rasterizer->setPen(orgPen);
    }
}

#endif


void ImagePainter2::drawPath(const Path& path, float smoothness)
{
    std::vector<Polygon> polygons;
    path.toPolygons(polygons, smoothness);

    for(std::size_t n = 0; n < polygons.size(); ++n)
    {
        const std::vector<PointF>& pointsF = polygons[n].points();

        if(_rasterizer->pen().size() == 1) 
        {          
            _rasterizer->drawNarrowPath( &pointsF[0], pointsF.size() );
        }
        else
        {
            drawWidePolyline( &pointsF[0], pointsF.size() );
        }
    }
}


void ImagePainter2::fillPolygon( const PointF* ps, const size_t pointCount)
{
    // NOTE: enable the next two lines to use another internal API of the
    //       polygon rasterizer

#if 1

    _rasterizer->fillPolygon2(ps, pointCount);

#else

    // Use anti-aliasing
    if( _rasterizer->isAntiAliasing() ) {
        // Remove duplicates
        std::vector<PointF> pointsF;
        deduplicatePointsF(pointsF, ps, pointCount);

        // Rasterize the polygon
        _rasterizer->fillPolygon(pointsF.data(), pointsF.size());
    }

    // Do not use use anti-aliasing
    else {
        // Round the points and remove duplicates
        std::vector<Point> points;
        cnvPointsFToPointsDeduplicate(points, ps, pointCount);
        // Rasterize the polygon
        _rasterizer->fillPolygon(points.data(), points.size());
    }
#endif
}


void ImagePainter2::fillRect( const RectF& rect )
{
    // Convert the points
    const Point tl( lround(rect.topLeft    ().x()), lround(rect.topLeft    ().y()) );
    const Point br( lround(rect.bottomRight().x()), lround(rect.bottomRight().y()) );

    // Rasterize the rectangle
    _rasterizer->fillRect(tl, br);
}


void ImagePainter2::fillRoundedRect( const RectF& rect, float radius )
{
    std::vector<PointF> pointsF;
    _lr->fillRoundedRect(pointsF, rect, radius);

    _rasterizer->fillPolygon2( &pointsF[0], pointsF.size() );
}


void ImagePainter2::fillEllipse( const PointF& topLeft, const SizeF& size )
{
    // Convert the points
    const Point tl( lround(topLeft.x    ()), lround(topLeft.y     ()) );
    const Size  sz( lround(size   .width()), lround(size   .height()) );

    // Rasterize the ellipse
    _rasterizer->fillEllipse(tl, sz);
}


void ImagePainter2::fillPie( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
     Point tl( lround(topLeft.x()), lround(topLeft.y()) );
     Size  sz( lround(size.width()), lround(size.height()) );

     _rasterizer->fillArc(tl, sz, degBegin, degEnd, ArcMode::Pie);
}


void ImagePainter2::fillChord( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    const Point tl( lround(topLeft.x    ()), lround(topLeft.y()) );
    const Size  sz( lround(size   .width()), lround(size.height()) );

     _rasterizer->fillArc(tl, sz, degBegin, degEnd, ArcMode::Chord);
}


void ImagePainter2::fillPath(const Path& path, float smoothness)
{
    // Convert the path to polygon points
    std::vector<Polygon> polygons;
    path.toPolygons(polygons, smoothness);

    _rasterizer->fillPolygons(polygons);
    return;


    //
    // OLD CODE FOR COMPARISON:
    //

    //std::vector<PointF> pointsF;
    //path.toPoints(pointsF);

    //// Use anti-aliasing
    //if( _rasterizer->isAntiAliasing() )
    //{
    //    // Remove duplicates
    //    std::vector<PointF> points;
    //    deduplicatePointsF(points, pointsF.data(), pointsF.size());

    //    // Draw the path as a filled polygon
    //    _rasterizer->fillPolygon( points.data(), points.size() );
    //}
    //else // Do not use use anti-aliasing
    //{
    //    // Round the points and remove duplicates
    //    std::vector<Point> points;
    //    cnvPointsFToPointsDeduplicate(points, pointsF.data(), pointsF.size());

    //    // Draw the path as a filled polygon
    //    _rasterizer->fillPolygon( points.data(), points.size() );
    //}
}


bool ImagePainter2::thickenSolidOpenPolygon(std::vector<PointF>& pointsF, const PointF* basePtr,
                                            size_t curPCnt, const int32_t* segmentIndexMarker)
{
    // Prepare the buffers
    std::vector<PointF> pointsFPolygon;
    std::vector<PointF> pointsFInner;
    std::vector<PointF> pointsFSegment;

    const size_t pz2 = (_rasterizer->pen().size() > 2) ? (_rasterizer->pen().size() / 2) : 1;
    const size_t pz4 = (_rasterizer->pen().size() > 4) ? (_rasterizer->pen().size() / 4) : 1;

    pointsFPolygon.reserve(curPCnt * pz2);
    pointsFInner  .reserve(curPCnt * pz4);
    pointsFSegment.reserve(          pz2);

    // Get the number of point
    const size_t curPC1 = curPCnt - 1;
    const size_t curPC2 = curPCnt - 2;

    // Walk through the polygon's lines
    for(size_t i = 0; i < curPC1; ++i) 
    {
        // Get the coordinates
        const PointF& from = *basePtr++;
        const PointF& to   = *basePtr;

        // Check if the "to" point belongs to the same segment
        bool inSameSegment = !!segmentIndexMarker;

        if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
            inSameSegment = false;
            ++segmentIndexMarker;
        }
        
        // Generate and combine line segments
        pointsFSegment.clear();
        generateSolidLineSegment(pointsFSegment, from.x(), from.y(), to.x(), to.y(), i == 0, i == curPC2);
        if(!combineLineSegmentForSolidOpenPolygon(
            pointsFPolygon, pointsFInner, pointsFSegment, from, inSameSegment
        )) return false;
    }

    // Process and store the "inside" lines' points to the main polygon buffer in reverse
    pointsFPolygon.insert(pointsFPolygon.end(), pointsFInner.rbegin(), pointsFInner.rend());

    // Combine the polygon data
    if(!pointsF.empty()) pointsF.push_back(Painter::PolygonSeparatorPointF);
    pointsF.insert(pointsF.end(), pointsFPolygon.begin(), pointsFPolygon.end());

    // Done
    return true;
}

bool ImagePainter2::thickenSolidClosedPolygon(std::vector<PointF>& pointsF, 
                                              const PointF* basePtr, size_t curPCnt, 
                                              const int32_t* segmentIndexMarker)
{
    // Prepare the buffers
    std::vector<PointF> pointsFOuter;
    std::vector<PointF> pointsFInner;
    std::vector<PointF> pointsFSegment;

    const size_t pz2 = (_rasterizer->pen().size() > 2) ? (_rasterizer->pen().size() / 2) : 1;
    const size_t pz4 = (_rasterizer->pen().size() > 4) ? (_rasterizer->pen().size() / 4) : 1;

    pointsFOuter  .reserve(curPCnt * pz2);
    pointsFInner  .reserve(curPCnt * pz4);
    pointsFSegment.reserve(          pz2);

    // Get the number of point
    const size_t curPC1 = curPCnt - 1;

    // Save the original base pointer
    const PointF* ptrZero = basePtr;

    // Walk through the polygon's lines
    for(size_t i = 0; i <= curPC1; ++i) 
    {
        // Get the coordinates
        const PointF& from = *basePtr++;
        const PointF& to   = (i == curPC1) ? *ptrZero : *basePtr;

        // Check if the "to" point belongs to the same segment
        bool inSameSegment = !!segmentIndexMarker && (i != curPC1);

        if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
            inSameSegment = false;
            ++segmentIndexMarker;
        }
        
        // Generate and combine line segments
        pointsFSegment.clear();
        generateSolidLineSegment(pointsFSegment, from.x(), from.y(), to.x(), to.y(), false, false);
        if(!combineLineSegmentForSolidClosedPolygon(
            pointsFOuter, pointsFInner, pointsFSegment, from, i == 1, false, inSameSegment
        )) return false;
    }

    // Reprocess the first and second segments to generate the last join
    if(true) {
        // Get the coordinates
        const PointF& from = *ptrZero++;
        const PointF& to   = *ptrZero;
        // Generate and combine line segments
        pointsFSegment.clear();
        generateSolidLineSegment(pointsFSegment, from.x(), from.y(), to.x(), to.y(), false, false);
        if(!combineLineSegmentForSolidClosedPolygon(
            pointsFOuter, pointsFInner, pointsFSegment, from, false, true, false
        )) return false;
    }

    // Combine the polygon data
    if(pointsFOuter.empty() || pointsFInner.empty()) return false;

    if( ! pointsF.empty() ) 
        pointsF.push_back(Painter::PolygonSeparatorPointF);

    pointsF.insert(pointsF.end(), pointsFOuter.begin(), pointsFOuter.end());

    if(!pointsF.empty()) pointsF.push_back(Painter::PolygonSeparatorPointF);
    pointsF.insert(pointsF.end(), pointsFInner.begin(), pointsFInner.end());

    // Done
    return true;
}

bool ImagePainter2::combineLineSegmentForSolidOpenPolygon(std::vector<PointF>& polygon, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint, bool inSameSegment)
{
    // If the main polygon buffer is still empty, simply copy the points
    if(polygon.empty()) {
        polygon.insert(polygon.end(), segment.begin(), segment.end());
        return true;
    }

    // Get the width of the pen
    const size_t penSize = _rasterizer->pen().size();

    // Copy point #(N-1) and #(N-2) from the main polygon buffer
    const size_t N1           = polygon.size() - 1;
    const PointF polyPointN   = polygon[N1    ];
    const PointF polyPointNm1 = polygon[N1 - 1];
    const PointF polyPointNm2 = polygon[N1 - 2];

    // Remove point #(N-1) and #N from the main polygon buffer
    polygon.pop_back();
    polygon.pop_back();

    // Get the "outside" lines
    const PointF& oline1a = polyPointNm2;
    const PointF& oline1b = polyPointNm1;
    const PointF& oline2a = segment[1];
    const PointF& oline2b = segment[2];

    // Intersect the "outside" lines
    bool   inLine;
    PointF intersect;
    if(!intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b, penSize)) return false;

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    */

    // Store the "outside" line's points to the main polygon buffer
    const Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    switch(js1) {
        // No join
        case Pen::NoJoin:
            polygon.push_back(oline1b);
            polygon.push_back(origMeetingPoint);
            polygon.push_back(oline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            polygon.push_back(oline1b);
            polygon.push_back(oline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            polygon.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                polygon,
                lround(oline1b  .x()), lround(oline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(oline2a  .x()), lround(oline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }

    // Store points #2 to #N from the segment to the main polygon buffer
    const size_t N2 = segment.size() - 1;
    for(size_t i = 2; i <= N2; ++i) polygon.push_back(segment[i]);

    // Get the "inside" lines
    const PointF& iline1a = inner.empty() ? polygon[0 ] : inner.back();
    const PointF& iline1b =                 polyPointN;
    const PointF& iline2a =                 segment[0 ];
    const PointF& iline2b =                 segment[N2];

    // Intersect the "inside" lines
    if(!intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b, penSize)) return false;

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    */

    // Store the "inside" line's points to the auxiliary polygon buffer
    const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    switch(js2) {
        // No join
        case Pen::NoJoin:
            inner.push_back(iline1b);
            inner.push_back(origMeetingPoint);
            inner.push_back(iline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            inner.push_back(iline1b);
            inner.push_back(iline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            inner.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                inner,
                lround(iline1b  .x()), lround(iline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(iline2a  .x()), lround(iline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }

    // Done
    return true;
}

bool ImagePainter2::combineLineSegmentForSolidClosedPolygon(std::vector<PointF>& outer, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint, bool isFirst, bool isLast, bool inSameSegment)
{
    // If the main polygon buffer is still empty, simply copy the points
    if(outer.empty()) {
        outer.push_back(segment[1]);
        outer.push_back(segment[2]);
        inner.push_back(segment[0]);
        inner.push_back(segment[3]);
        return true;
    }

    // Get the width of the pen
    const size_t penSize = _rasterizer->pen().size();

    // Get the "outside" lines
    const PointF& oline1a = outer[outer.size() - 2];
    const PointF& oline1b = outer[outer.size() - 1];
    const PointF& oline2a = segment[1];
    const PointF& oline2b = segment[2];

    // Intersect the "outside" lines
    bool   inLine;
    PointF intersect;
    if(!intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b, penSize)) return false;

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    */

    // Store the "outside" line's points
    const Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    outer.pop_back();
    if(isFirst) outer.pop_back();
    switch(js1) {
        // No join
        case Pen::NoJoin:
            outer.push_back(oline1b);
            outer.push_back(origMeetingPoint);
            outer.push_back(oline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            outer.push_back(oline1b);
            outer.push_back(oline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            outer.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                outer,
                lround(oline1b  .x()), lround(oline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(oline2a  .x()), lround(oline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }
    if(!isLast) outer.push_back(oline2b);

    // Get the "inside" lines
    const PointF& iline1a = inner[inner.size() - 2];
    const PointF& iline1b = inner[inner.size() - 1];
    const PointF& iline2a = segment[0];
    const PointF& iline2b = segment[3];

    // Intersect the "inside" lines
    if(!intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b, penSize)) return false;

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    */

    // Store the "inside" line's points
    const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    inner.pop_back();
    if(isFirst) inner.pop_back();
    switch(js2) {
        // No join
        case Pen::NoJoin:
            inner.push_back(iline1b);
            inner.push_back(origMeetingPoint);
            inner.push_back(iline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            inner.push_back(iline1b);
            inner.push_back(iline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            inner.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                inner,
                lround(iline1b  .x()), lround(iline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(iline2a  .x()), lround(iline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }
    if(!isLast) inner.push_back(iline2b);

    // Done
    return true;
}

// --- Patterned Line Thickener ---

struct SAGOpState {
    std::vector<PointF>& dstPoints;  // Destination vector
    size_t               dstPStart;  // Start index of the previous polygon in the above vector
    size_t               dstPCount;  // The number of points of the previous polygon in the above vector
    size_t               dstPCount0; // The number of points of the first polygon in the above vector

    const PointF*        srcPoints;  // Source points
    size_t               srcCount;   // The number of source points

    float                cellSize;   // Cell size
    float                patSegLen;  // Length of the currently processed "pattern" segment

    size_t               idx1;       // Index to the first point which is currently being processed;
                                     // the index to the second point is always (idx1 + 1)

    float                px, py;     // Current interpolation coordinate (in-between the two points)
    float                ex, ey;     // Current end coordinate (coordinate of the the second point)
    float                uvx, uvy;   // Unit vector from the first point to the second point
    float                cvx, cvy;   // Cell vector from the first point to the second point
    float                remLen;     // Remaining length between the two points that has not been "consumed" by the "pattern" segment(s)

    std::vector<PointF>  gather;     // Gathered polygon points
    float                gatherLen;  // Length of the gathered points

    inline SAGOpState(std::vector<PointF>& pointsF, const PointF* src, size_t pointCount, size_t penSize)
    : dstPoints(pointsF), dstPStart(0), dstPCount(0), dstPCount0(0),
      srcPoints(src), srcCount(pointCount), cellSize(penSize * 0.25f),
      idx1(0), remLen(-1.0f), gatherLen(0.0f)
    {}
};



void ImagePainter2::thickenPatternedPolygon(std::vector<PointF>& pointsF, const PointF* src, size_t pointCount)
{
    // Initialize the operational state
    SAGOpState state(pointsF, src, pointCount, _rasterizer->pen().size());

    // The pattern buffer and its counter
    const Pt::uint8_t* pBuff      = _rasterizer->patternBufferMP64();
          Pt::int32_t  piCtrInOut = 0;

    // Loop until all the polygon's points are processed
    bool done = false;
    while(!done) {
        // Calculate the "pattern" segment length
        const Pt::uint8_t refPat = pBuff[piCtrInOut];
        state.patSegLen = 0.0f;
        for(;;) {
            // Get and compare the pattern bit
            const Pt::uint8_t curPat = pBuff[piCtrInOut++];
            if(piCtrInOut >= PATTERN_BUFFER_COUNTER_MAXMP) piCtrInOut -= PATTERN_BUFFER_COUNTER_MAXMP;
            if(curPat == refPat) {
                state.patSegLen += state.cellSize;
                continue;
            }
            // We have got a different pattern bit, exit to process the "pattern" segment
            --piCtrInOut;
            if(piCtrInOut < 0) piCtrInOut += PATTERN_BUFFER_COUNTER_MAXMP;
            break;
        }
        // Bail out if the "pattern" segment is shorter than the cell size
        if(state.patSegLen < state.cellSize) return;
        // Process the "pattern" segment
        done = sagPolygonPoints(state, !!refPat);
    }
}

bool ImagePainter2::sagPolygonPoints(SAGOpState& state, bool draw)
{
    // Temporary buffer for the generated points
    std::vector<PointF> pointsF;

    // Loop until the current "pattern" segment is completely processed
    while(state.patSegLen > 0.0f) {

        // (Re-)initialize some part of the operational state as needed
        if(state.remLen <= 0.0f) {
            // Check if all polygon's points have been processed
            if(state.idx1 + 1 >= state.srcCount) {
                state.gather.clear();
                state.gatherLen = 0.0f;
                return true;
            }
            // Calculate the vector, size, and coordinates
            const float x1 = state.srcPoints[state.idx1    ].x();
            const float y1 = state.srcPoints[state.idx1    ].y();
            const float x2 = state.srcPoints[state.idx1 + 1].x();
            const float y2 = state.srcPoints[state.idx1 + 1].y();
            const float vx = x2 - x1;
            const float vy = y2 - y1;
            const float vz = sqrt(vx * vx + vy * vy);
            // Initialize some part of the operational state
            state.px     = x1;
            state.py     = y1;
            state.ex     = x2;
            state.ey     = y2;
            state.uvx    = vx / vz;
            state.uvy    = vy / vz;
            state.cvx    = state.uvx * state.cellSize;
            state.cvy    = state.uvy * state.cellSize;
            state.remLen = vz;
        }

        // If we have the complete length from the gathered points, process them into a thick polygon
        if(state.gatherLen >= state.patSegLen) {
            // Generate one solid polygon segment as needed
            if(draw) {
                if(_rasterizer->pen().capStyle() == Pen::ButtCap) {
                    state.gather.back().set(
                        state.gather.back().x() + state.cvx,
                        state.gather.back().y() + state.cvy
                    );
                }
                sagGeneratePolyLineSegment(state);
            }
            // Reset the "pattern" segment length
            state.patSegLen = 0.0f;
            // Reset the gather buffer
            state.gather.clear();
            state.gatherLen = 0.0f;
            continue;
        }

        // If we have enough remainder length, process the polygon's edge as a simple line segment
        if(state.gather.empty() && state.remLen >= state.patSegLen) {
            // Generate a simple line segment as needed
            if(draw) {
                if(_rasterizer->pen().capStyle() == Pen::ButtCap) {
                    sagGenerateSimpleLineSegment(
                        state,
                        state.px,
                        state.py,
                        state.px + state.cvx + state.uvx * state.patSegLen,
                        state.py + state.cvy + state.uvy * state.patSegLen
                    );

                }
                else {
                    sagGenerateSimpleLineSegment(
                        state,
                        state.px,
                        state.py,
                        state.px + state.uvx * state.patSegLen,
                        state.py + state.uvy * state.patSegLen
                    );
                }
            }
            // Substract the remainder length
            state.remLen -= state.patSegLen;
            // Reset the "pattern" segment length
            state.patSegLen = 0.0f;
            // Process excess length (if any)
            if(state.remLen > 0.0f) {
                // Update the interpolation coordinate
                state.px = state.ex - state.uvx * state.remLen;
                state.py = state.ey - state.uvy * state.remLen;
            }
            else {
                // Reset the remainder length and increment the point index so that the next time
                // this loop is running, the next point within the polygon will be processed
                state.remLen = -1.0f;
                ++state.idx1;
            }
            continue;
        }

        // ------------------------------------------
        // If we got herem it means:
        //     1. The remainder  length is not enough
        //     2. The "gathered" length is not enough
        // ------------------------------------------

        // Store the current interpolation coordinate to the "gather" buffer as needed
        if(state.gather.empty() || state.gather.back().x() != state.px || state.gather.back().y() != state.py) {
            state.gather.push_back(PointF(state.px, state.py));
        }
        // If the combined length is less than or equal to the "pattern" segment length, simply store the end coordinate
        if(state.gatherLen + state.remLen <= state.patSegLen) {
            // Store the end coordinate
            state.gather.push_back(PointF(state.ex, state.ey));
            state.gatherLen += state.remLen;
            // Reset the remainder length and increment the point index so that the next time
            // this loop is running, the next point within the polygon will be processed
            state.remLen = -1.0f;
            ++state.idx1;
        }
        // Otherwise, store the in-between coordinate
        else {
            // Calculate the needed length
            const float nl = state.patSegLen - state.gatherLen;
            // Update the interpolation coordinate
            state.px += state.uvx * nl;
            state.py += state.uvy * nl;
            // Store the in-between coordinate (the updated interpolation coordinate)
            state.gather.push_back(PointF(state.px, state.py));
            state.gatherLen += nl;
            // Substract and check the remainder length
            state.remLen -= nl;
            if(state.remLen <= 0.0f) {
                // Reset the remainder length and increment the point index so that the next time
                // this loop is running, the next point within the polygon will be processed
                state.remLen = -1.0f;
                ++state.idx1;
            }
        }

    } // while()

    // Indicate that the current "pattern" segment are completely processed,
    // but there are still unprocessed polygon's points
    return false;
}

void ImagePainter2::sagGenerateSimpleLineSegment(SAGOpState& state, float x1, float y1, float x2, float y2)
{
    // Temporary buffer for the generated points
    std::vector<PointF> pointsF;

    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, _rasterizer->pen().size());

    // Generate points (CCW)
    // --- Begin point ---
    switch(_rasterizer->pen().capStyle()) {
        case Pen::SquareCap        : generateLineSquareCap       (pointsF, x1, y1,     dx, dy, nx, ny); break;
        case Pen::RoundCap         : generateLineRoundCap        (pointsF, x1, y1, wh, dx, dy, nx, ny); break;
        case Pen::TriangularOutCap : generateLineTriangularOutCap(pointsF, x1, y1,     dx, dy, nx, ny); break;
        case Pen::TriangularInCap  : generateLineTriangularInCap (pointsF, x1, y1,     dx, dy, nx, ny); break;
        case Pen::RoundHoleCap     : generateLineRoundHoleCap    (pointsF, x1, y1, wh, dx, dy, nx, ny); break;
        case Pen::Arrow1Cap        : generateLineArrow1Cap       (pointsF, x1, y1,     dx, dy, nx, ny); break;
        case Pen::Arrow2Cap        : generateLineArrow2Cap       (pointsF, x1, y1,     dx, dy, nx, ny); break;
        default                    : generateLineButtCap         (pointsF, x1, y1,             nx, ny); break;
    }
    // --- End point ---
    switch(_rasterizer->pen().capStyle()) {
        case Pen::SquareCap        : generateLineSquareCap       (pointsF, x2, y2,     -dx, -dy, -nx, -ny); break;
        case Pen::RoundCap         : generateLineRoundCap        (pointsF, x2, y2, wh, -dx, -dy, -nx, -ny); break;
        case Pen::TriangularOutCap : generateLineTriangularOutCap(pointsF, x2, y2,     -dx, -dy, -nx, -ny); break;
        case Pen::TriangularInCap  : generateLineTriangularInCap (pointsF, x2, y2,     -dx, -dy, -nx, -ny); break;
        case Pen::RoundHoleCap     : generateLineRoundHoleCap    (pointsF, x2, y2, wh, -dx, -dy, -nx, -ny); break;
        case Pen::Arrow1Cap        : generateLineArrow1Cap       (pointsF, x2, y2,     -dx, -dy, -nx, -ny); break;
        case Pen::Arrow2Cap        : generateLineArrow2Cap       (pointsF, x2, y2,     -dx, -dy, -nx, -ny); break;
        default                    : generateLineButtCap         (pointsF, x2, y2,               -nx, -ny); break;
    }

    // Check for intersection with the first polygons in the final destination buffer
    if(state.dstPCount0) {
        const bool r = satDetectPolygonCollision(
            &state.dstPoints[0], state.dstPCount0, pointsF.data(), pointsF.size()
        );
        if(r) return;
    }
    else {
        state.dstPCount0 = pointsF.size();
    }

    // Check for intersection with the previous polygons in the final destination buffer
    if(state.dstPCount && state.dstPStart) {
        const bool r = satDetectPolygonCollision(
            &state.dstPoints[state.dstPStart], state.dstPCount, pointsF.data(), pointsF.size()
        );
        if(r) return;
    }
    state.dstPStart = state.dstPoints.size();
    state.dstPCount = pointsF.size();

    // Add polygon separator point as needed
    if(!state.dstPoints.empty()) {
        state.dstPoints.push_back(Painter::PolygonSeparatorPointF);
        ++state.dstPStart;
    }

    // Copy the points
    state.dstPoints.insert(state.dstPoints.end(), pointsF.begin(), pointsF.end());
}

void ImagePainter2::sagGeneratePolyLineSegment(SAGOpState& state)
{
    // Temporary buffer for the generated points
    std::vector<PointF> pointsF;

    // Generate a new thick polygon
    thickenSolidOpenPolygon(pointsF, state.gather.data(), state.gather.size(), 0);

    // Exit here if the generated polygon does not actually have a meaningful number of points
    if(pointsF.size() < 2) return;

    // Check for intersection with the first polygons in the final destination buffer
    if(state.dstPCount0) {
        const bool r = satDetectPolygonCollision(
            &state.dstPoints[0], state.dstPCount0, pointsF.data(), pointsF.size()
        );
        if(r) return;
    }
    else {
        state.dstPCount0 = pointsF.size();
    }

    // Check for intersection with the previous polygons in the final destination buffer
    if(state.dstPCount && state.dstPStart) {
        const bool r = satDetectPolygonCollision(
            &state.dstPoints[state.dstPStart], state.dstPCount, pointsF.data(), pointsF.size()
        );
        if(r) return;
    }
    state.dstPStart = state.dstPoints.size();
    state.dstPCount = pointsF.size();

    // Add polygon separator point as needed
    if(!state.dstPoints.empty()) {
        state.dstPoints.push_back(Painter::PolygonSeparatorPointF);
        ++state.dstPStart;
    }

    // Copy the points
    state.dstPoints.insert(state.dstPoints.end(), pointsF.begin(), pointsF.end());
}


bool ImagePainter2::isAntiAliasing() const
{
 return _rasterizer->isAntiAliasing();
}


bool ImagePainter2::intersectLine(bool& inLine, PointF& intersect, const PointF& line1a, const PointF& line1b, const PointF& line2a, const PointF& line2b, size_t penSize)
{
    // The first line
    const float x11   = line1a.x();
    const float y11   = line1a.y();
    const float x12   = line1b.x();
    const float y12   = line1b.y();
    const float minX1 = std::min(x11, x12);
    const float minY1 = std::min(y11, y12);
    const float maxX1 = std::max(x11, x12);
    const float maxY1 = std::max(y11, y12);
    const float a1    = y12 - y11;
    const float b1    = x11 - x12;
    const float c1    = -(x11 * y12 - x12 * y11);

    // The second line
    const float x21   = line2a.x();
    const float y21   = line2a.y();
    const float x22   = line2b.x();
    const float y22   = line2b.y();
    const float minX2 = std::min(x21, x22);
    const float minY2 = std::min(y21, y22);
    const float maxX2 = std::max(x21, x22);
    const float maxY2 = std::max(y21, y22);
    const float a2    = y22 - y21;
    const float b2    = x21 - x22;
    const float c2    = -(x21 * y22 - x22 * y21);

    // Check if the line is parallel
    const float denom = a1 * b2 - a2 * b1;
    if(denom == 0.0f) {
        // Check for special cases
        if(y11 == y12 && y11 == y21 && y11 == y22 && x12 == x21) {
            intersect.set(x12, y11);
            inLine = true;
            return true;
        }
        if(x11 == x12 && x11 == x21 && x11 == x22 && y12 == y21) {
            intersect.set(x11, y12);
            inLine = true;
            return true;
        }
        // No intersection
        return false;
    }

    // Calculate the intersection point
    const float idenom = 1.0f / denom;
          float ipX    = (b1 * c2 - b2 * c1) * idenom;
          float ipY    = (a2 * c1 - a1 * c2) * idenom;

    // Check and fix the coordinate of the intersection point
    // (for very steep lines, the coordinate of the intersection point can be incorrectly calculated)
    const size_t pzf = FIXED_POINT_TO_INT(penSize * FIXED_POINT_CONSTANT_SQRT2);
          if(ipX < minX1 - pzf && ipX < minX2 - pzf) ipX = (minX1 + minX2) * 0.5f;
    else if(ipX > maxX1 + pzf && ipX > maxX2 + pzf) ipX = (maxX1 + maxX2) * 0.5f;
          if(ipY < minY1 - pzf && ipY < minY2 - pzf) ipY = (minY1 + minY2) * 0.5f;
    else if(ipY > maxY1 + pzf && ipY > maxY2 + pzf) ipY = (maxY1 + maxY2) * 0.5f;

    // Store the intersection point
    intersect.set(ipX, ipY);

    // Determine if the intersection point is inside the line
    inLine = (ipX >= minX1 && ipX <= maxX1 && ipY >= minY1 && ipY <= maxY1)
            | (ipX >= minX2 && ipX <= maxX2 && ipY >= minY2 && ipY <= maxY2);

    // Done
    //lprintf("Line 1       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x11, y11, x12, y12);
    //lprintf("Line 2       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x21, y21, x22, y22);
    //lprintf("Intersection : (%7.3f, %7.3f) - %s \n", ipX, ipY, inLine ? "inline" : "outline");
    //lprintf("\n");
    return true;
}


enum ClipMode
{
  Intersection, Union, Difference, Xor
};


void ImagePainter2::clipPolygonXXX(std::vector<PointF>& result, const std::vector<PointF>& subject, const std::vector<PointF>& clipRegion)
{
    ClipMode cm = Intersection;

    // Working variables
    ClipperLib::Clipper clipper;
    ClipperLib::Path    cpath;
    ClipperLib::Paths   cpresult;
    size_t              startIndex;

    // Separate and append the clipper polygons
    startIndex = 0;
    for(size_t i = 0; i <= clipRegion.size(); ++i) {
        // Search for the end and/or separator points
        if( i == clipRegion.size() || (clipRegion[i].x() > Painter::MaximumCoordinateF && clipRegion[i].y() > Painter::MaximumCoordinateF) ) {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            // Append the polygon to the clipper
            cpath.resize(curPC);
            for(size_t j = 0; j < curPC; ++j) {
                cpath[j].X = lround( clipRegion[startIndex + j].x() * VecResScaleUp );
                cpath[j].Y = lround( clipRegion[startIndex + j].y() * VecResScaleUp );
            }
            clipper.AddPath(cpath, ClipperLib::ptClip, true);
            // Increment the start index
            startIndex += curPC + 1;
        }
    }

    // Separate and append the subject polygons
    startIndex = 0;
    for(size_t i = 0; i <= subject.size(); ++i) {
        // Search for the end and/or separator points
        if( i == subject.size() || (subject[i].x() > Painter::MaximumCoordinateF && subject[i].y() > Painter::MaximumCoordinateF) ) {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            // Append the polygon to the clipper
            cpath.resize(curPC);
            for(size_t j = 0; j < curPC; ++j) {
                cpath[j].X = lround( subject[startIndex + j].x() * VecResScaleUp );
                cpath[j].Y = lround( subject[startIndex + j].y() * VecResScaleUp );
            }
            clipper.AddPath(cpath, ClipperLib::ptSubject, cpath[0] == cpath.back());
            // Increment the start index
            startIndex += curPC + 1;
        }
    }

    // Perform clipping
    result.clear();

    switch(cm) {
        case Intersection:
            clipper.Execute(ClipperLib::ctIntersection, cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        case Union:
            clipper.Execute(ClipperLib::ctUnion,        cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        case Difference:
            clipper.Execute(ClipperLib::ctDifference,   cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        case Xor:
            clipper.Execute(ClipperLib::ctXor,          cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
            break;

        default:
            return;
    }

    // Combine back the result polygons
    for(size_t i = 0; i < cpresult.size(); ++i) {
        const ClipperLib::Path& curPath = cpresult[i];
        if(!result.empty()) result.push_back(Painter::PolygonSeparatorPointF);
        for(size_t j = 0; j < curPath.size(); ++j) {
            result.push_back( PointF(
                curPath[j].X * VecResScaleDn,
                curPath[j].Y * VecResScaleDn
            ) );
        }
    }
}

} // namespace

} // namespace
