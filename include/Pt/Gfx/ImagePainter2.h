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

#ifndef PT_GFX_IMAGEPAINTER_2_H
#define PT_GFX_IMAGEPAINTER_2_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Math.h>

#include <Pt/Gfx/AntiAliasingMode.h>
#include <Pt/Gfx/ArcMode.h>
#include <Pt/Gfx/Painter.h>

#include <Pt/System/Path.h>

namespace Pt {
namespace Gfx {


class AffineMatrix2D;
class Rasterizer2;

class PT_GFX_API ImagePainter2 : public Painter
{
    public:
        ImagePainter2( Image& image );

        virtual ~ImagePainter2();

        void setAntiAliasingMode(AntiAliasingMode mode = AntiAliasingMode::Standard);

        void setImage(Image& image);

        virtual const ImageFormat& format() const;

        virtual void setCompositionMode(const CompositionMode& mode);

        virtual const CompositionMode& compositionMode() const;

        virtual void setClip( const RectF& clip );

        virtual const Gfx::RectF& clip() const;

        virtual void setPen(const Pen& pen);

        virtual const Pen& pen() const;

        virtual void setBrush(const Brush& brush);

        virtual const Brush& brush() const;

        virtual void setFont(const Font& font);

        virtual const Font& font() const;

        virtual FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawImage(const PointF& to, const Image& image);

        virtual void drawImage(const PointF& to, const Image& image, const RectF& imageRect);

        virtual void drawText(const PointF& to, const Pt::String& text);

        virtual void drawLine(const PointF& from, const PointF& to);

        virtual void drawRect(const RectF& rect);

        // NOTE: The points must move in counter-clockwise (CCW) direction or something wrong will be drawn!
        virtual void drawPolyline(const PointF* points, const size_t pointCount, bool autoClose);

        // NOTE: The points must move in counter-clockwise (CCW) direction or something wrong will be drawn!
        //       When autoClose == false : the number of points must be >= 3 and odd
        //       When autoClose == true  : the number of points must be >= 4 and even
        virtual void drawQuadraticPolybezier(const PointF* points, const size_t pointCount, bool autoClose);

        virtual void drawEllipse(const PointF& topLeft, const SizeF& size);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong will be drawn!
        virtual void drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode);

        virtual void fillRect(const RectF& rect);

        // NOTE: The points must move in counter-clockwise (CCW) direction or something wrong will be drawn!
        virtual void fillPolygon(const PointF* points, const size_t pointCount);

        virtual void fillEllipse(const PointF& topLeft, const SizeF& size);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong will be drawn!
        virtual void fillArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode);

    public:
        // Just to make the API match the interface defined by the Pt::GfxPainter class
        virtual void drawPolyline(const PointF* points, const size_t pointCount)
        { drawPolyline(points, pointCount, false); }

    public:
        static void setFontDir(const System::Path& path);
        static void setDefaultFont(const std::string& name);
        static std::string defaultFont();
        static std::vector<std::string> fontNames();
        static FontMetrics fontMetrics(const Font& font, const Pt::String& text);

    private:
        inline void convertPointTrunc(std::vector<Point>& dst, const PointF* src, const size_t pointCount);
        inline void convertPointRound(std::vector<Point>& dst, const PointF* src, const size_t pointCount);

        void generateSolidLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap);

        bool thickenSolidOpenPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt);
        bool thickenSolidClosedPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt);

        bool combineLineSegmentForSolidOpenPolygon(std::vector<PointF>& polygon, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint);
        bool combineLineSegmentForSolidClosedPolygon(std::vector<PointF>& outer, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint);

        void generatePatternedLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap);

    private:
        RectF           _clip;
        AffineMatrix2D* _affineMatrix2D;
        Rasterizer2*    _rasterizer;
};


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

void ImagePainter2::convertPointTrunc(std::vector<Point>& dst, const PointF* src, const size_t pointCount)
{
    const size_t ofs = dst.size();
    dst.resize(ofs + pointCount);

    for(size_t i = 0; i < pointCount; ++i)
        dst[i + ofs].set( (Pt::int32_t) src[i].x(), (Pt::int32_t) src[i].y() );
}

void ImagePainter2::convertPointRound(std::vector<Point>& dst, const PointF* src, const size_t pointCount)
{
    const size_t ofs = dst.size();
    dst.resize(ofs + pointCount);

    for(size_t i = 0; i < pointCount; ++i)
        dst[i + ofs].set( (Pt::int32_t) round(src[i].x()), (Pt::int32_t) round(src[i].y()) );
}


} // namespace
} // namespace

#endif

