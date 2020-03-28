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

#include "Rasterizer2.h"
#include "FreeType.h"
#include "ArcMode.h"
#include <Pt/Gfx/ImagePainter2.h>
#include "clipper_aj/clipper.hpp"

namespace Pt {

namespace Gfx {


// ### TEMPORARY ###
bool IP2_DEBUG::DUMP_POLYGON_COORDINATES  = false;
bool IP2_DEBUG::DUMP_SCANLINE_COORDINATES = false;
// ### TEMPORARY ###


ImagePainter2::ImagePainter2(Image& image)
: _rasterizer( new Rasterizer2(image) )
{
    setAntiAliasing(true);
}


ImagePainter2::~ImagePainter2()
{
  delete _rasterizer;
}


bool ImagePainter2::isAntiAliasing() const
{
 return _rasterizer->isAntiAliasing();
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


const CompositionMode& ImagePainter2::compositionMode() const
{
  return _rasterizer->compositionMode();
}


void ImagePainter2::setCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


void ImagePainter2::setClip(const RectF& clip)
{
    Rect roundedClip(round(clip));
    _rasterizer->setClip(roundedClip);
}


void ImagePainter2::resetClip()
{
    _rasterizer->resetClip();
}


const Pen& ImagePainter2::pen() const
{
  return _rasterizer->pen();
}


void ImagePainter2::setPen(const Pen& pen)
{
    _rasterizer->setPen(pen);
}


const Brush& ImagePainter2::brush() const
{
    return _rasterizer->brush();
}


void ImagePainter2::setBrush(const Brush& brush)
{
    _rasterizer->setBrush(brush);
}


const Font& ImagePainter2::font() const
{
    return _rasterizer->font();
}


void ImagePainter2::setFont(const Font& font)
{
    _rasterizer->setFont( font );
}


void ImagePainter2::drawImage(const PointF& to, const Image& image)
{
    const Point to_( round(to) );

    _rasterizer->drawImage(to_, image);

}


void ImagePainter2::drawImage(const PointF& toF, const Image& image,
                              const RectF& rectF )
{
    Point to(round(toF));

    Rect rect(round(rectF));

    _rasterizer->drawImage(to, image, rect);

}

void ImagePainter2::drawText(const PointF& toF, const String& text)
{
    Point to(round(toF));

    Transform identity;
    _rasterizer->drawText(to, text, identity);
}


void ImagePainter2::drawText(const PointF& toF, const Pt::String& text,
                             const Transform& transform)
{
    Point to(round(toF));

    _rasterizer->drawText(to, text, transform);
}



FontMetrics ImagePainter2::fontMetrics(const String& text) const
{
    return _rasterizer->fontMetrics( text );
}


FontMetrics ImagePainter2::fontMetrics(const Font& font, const Pt::String& text)
{
    return Rasterizer2::fontMetrics(font, text);
}


void ImagePainter2::drawLine(const PointF& from, const PointF& to)
{
    _rasterizer->drawLine(from, to);
}


void ImagePainter2::drawPolyline(const PointF* points, const size_t pointCount)
{
    _rasterizer->drawPolyline(points, pointCount);
}


void ImagePainter2::drawRect(const RectF& rect)
{
    _rasterizer->drawRect(rect);
}


void ImagePainter2::drawRoundedRect(const RectF& rect, float radius)
{
    _rasterizer->drawRoundedRect(rect, radius);
}


void ImagePainter2::drawEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->drawEllipse(topLeft, size);
}


void ImagePainter2::drawArc(const PointF& topLeft, const SizeF& size,
                            float degBegin, float degEnd)
{
    _rasterizer->drawArc(topLeft, size, degBegin, degEnd, ArcMode::Open);
}


void ImagePainter2::drawChord(const PointF& topLeft, const SizeF& size,
                              float degBegin, float degEnd)
{
    _rasterizer->drawArc(topLeft, size, degBegin, degEnd, ArcMode::Chord);
}


void ImagePainter2::drawPie(const PointF& topLeft, const SizeF& size,
                            float degBegin, float degEnd)
{
    _rasterizer->drawArc(topLeft, size, degBegin, degEnd, ArcMode::Pie);
}


void ImagePainter2::drawPath(const Path& path, float smoothness)
{
    _rasterizer->drawPath(path, smoothness);
}


void ImagePainter2::fillPolygon(const PointF* points, const size_t pointCount)
{
    _rasterizer->fillPolygon(points, pointCount);
}


void ImagePainter2::fillPolygon_NR(const PointF* points, const size_t pointCount)
{
    _rasterizer->fillPolygon_NR(points, pointCount);
}


void ImagePainter2::fillRect( const RectF& rect )
{
    _rasterizer->fillRect(rect);
}


void ImagePainter2::fillRoundedRect(const RectF& rect, float radius)
{
    _rasterizer->fillRoundedRect(rect, radius);
}


void ImagePainter2::fillEllipse(const PointF& topLeft, const SizeF& size)
{
    _rasterizer->fillEllipse(topLeft, size);
}


void ImagePainter2::fillPie(const PointF& topLeft, const SizeF& size,
                            float degBegin, float degEnd)
{
     _rasterizer->fillPie(topLeft, size, degBegin, degEnd);
}


void ImagePainter2::fillChord(const PointF& topLeft, const SizeF& size,
                              float degBegin, float degEnd)
{
     _rasterizer->fillChord(topLeft, size, degBegin, degEnd);
}


void ImagePainter2::fillPath(const Path& path, float smoothness)
{
    _rasterizer->fillPath(path, smoothness);
}


void ImagePainter2::setFontDir(const Pt::System::Path& path)
{
  FreeType::instance().setFontDir(path);
}


void ImagePainter2::setDefaultFont(const std::string& f)
{
  FreeType::instance().setDefaultFont(f);
}


std::string ImagePainter2::defaultFont()
{
  return FreeType::instance().defaultFont();
}


std::vector<std::string> ImagePainter2::fontNames()
{
  return FreeType::instance().fontNames();
}


//enum ClipMode
//{
//  Intersection, Union, Difference, Xor
//};
//
//
//void ImagePainter2::clipPolygonXXX(std::vector<PointF>& result, const std::vector<PointF>& subject, const std::vector<PointF>& clipRegion)
//{
//    ClipMode cm = Intersection;
//
//    // Working variables
//    ClipperLib::Clipper clipper;
//    ClipperLib::Path    cpath;
//    ClipperLib::Paths   cpresult;
//    size_t              startIndex;
//
//    // Separate and append the clipper polygons
//    startIndex = 0;
//    for(size_t i = 0; i <= clipRegion.size(); ++i) {
//        // Search for the end and/or separator points
//        if( i == clipRegion.size() || (clipRegion[i].x() > Painter::MaximumCoordinateF && clipRegion[i].y() > Painter::MaximumCoordinateF) ) {
//            // Calculate the number of points for this polygon
//            const size_t curPC = i - startIndex;
//            // Append the polygon to the clipper
//            cpath.resize(curPC);
//            for(size_t j = 0; j < curPC; ++j) {
//                cpath[j].X = lround( clipRegion[startIndex + j].x() * VecResScaleUp );
//                cpath[j].Y = lround( clipRegion[startIndex + j].y() * VecResScaleUp );
//            }
//            clipper.AddPath(cpath, ClipperLib::ptClip, true);
//            // Increment the start index
//            startIndex += curPC + 1;
//        }
//    }
//
//    // Separate and append the subject polygons
//    startIndex = 0;
//    for(size_t i = 0; i <= subject.size(); ++i) {
//        // Search for the end and/or separator points
//        if( i == subject.size() || (subject[i].x() > Painter::MaximumCoordinateF && subject[i].y() > Painter::MaximumCoordinateF) ) {
//            // Calculate the number of points for this polygon
//            const size_t curPC = i - startIndex;
//            // Append the polygon to the clipper
//            cpath.resize(curPC);
//            for(size_t j = 0; j < curPC; ++j) {
//                cpath[j].X = lround( subject[startIndex + j].x() * VecResScaleUp );
//                cpath[j].Y = lround( subject[startIndex + j].y() * VecResScaleUp );
//            }
//            clipper.AddPath(cpath, ClipperLib::ptSubject, cpath[0] == cpath.back());
//            // Increment the start index
//            startIndex += curPC + 1;
//        }
//    }
//
//    // Perform clipping
//    result.clear();
//
//    switch(cm) {
//        case Intersection:
//            clipper.Execute(ClipperLib::ctIntersection, cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
//            break;
//
//        case Union:
//            clipper.Execute(ClipperLib::ctUnion,        cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
//            break;
//
//        case Difference:
//            clipper.Execute(ClipperLib::ctDifference,   cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
//            break;
//
//        case Xor:
//            clipper.Execute(ClipperLib::ctXor,          cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
//            break;
//
//        default:
//            return;
//    }
//
//    // Combine back the result polygons
//    for(size_t i = 0; i < cpresult.size(); ++i) {
//        const ClipperLib::Path& curPath = cpresult[i];
//        if(!result.empty()) result.push_back(Painter::PolygonSeparatorPointF);
//        for(size_t j = 0; j < curPath.size(); ++j) {
//            result.push_back( PointF(
//                curPath[j].X * VecResScaleDn,
//                curPath[j].Y * VecResScaleDn
//            ) );
//        }
//    }
//}

} // namespace

} // namespace
