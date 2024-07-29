/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef Pt_Gfx_PaintSurface_h
#define Pt_Gfx_PaintSurface_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Path.h>

#include <vector>

namespace Pt {

namespace Gfx {

class Painter;
class PaintData;
class Line;
class Polyline;

class PaintSurface;

/** @brief Paint canvas.
*/
class PT_GFX_API Canvas
{
    friend class PaintData;

    public:
        Canvas();

        ~Canvas();

        const Gfx::ImageFormat& format() const;

        const Gfx::SizeF& size() const;

        const Scaling& scaling() const;

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const = 0;

        virtual const Gfx::SizeF& onSize() const = 0;

        virtual const Scaling& onGetScaling() const = 0;

        virtual void onFinish() = 0;

    protected:
        virtual void setCompositionMode(const Gfx::CompositionMode& mode) = 0;

        virtual void setPen(const Gfx::Pen& pen) = 0;

        virtual void setBrush(const Gfx::Brush& brush) = 0;

        virtual void setFont(const Gfx::Font& font) = 0;

        virtual void setClip(const Gfx::RectF& clip) = 0;

        virtual void resetClip() = 0;

    protected:
        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const = 0;

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text) = 0;

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans) = 0;

    protected:
        virtual void drawLine(const Gfx::Line& line) = 0;
    
        virtual void drawPolyline(const Polyline& line) = 0;

        virtual void fillPolygon(const Polyline& line) = 0;

        virtual void drawRect(const Gfx::RectF& rectangle) = 0;

        virtual void fillRect(const Gfx::RectF& rectangle) = 0;

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

        virtual void drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd) = 0;

        virtual void fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd) = 0;

        virtual void drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd) = 0;

        virtual void fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd) = 0;

        virtual void drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd) = 0;

        virtual void drawPath(const Gfx::Path& path, float smoothness) = 0;

        virtual void fillPath(const Path& path, float smoothness) = 0;

    protected:
        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image) = 0;

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image, 
                               const Gfx::RectF& imgRect) = 0;

        virtual void drawSurface(const Gfx::PointF& to, 
                                 const PaintSurface& surface) = 0;

        virtual void drawSurface(const Gfx::PointF& to, 
                                 const PaintSurface& surface, 
                                 const Gfx::RectF& surfaceRect) = 0;
    
    private:
        void attachPaint(PaintData& paint);

        void detachPaint(PaintData& paint);

    private:
        PaintData* _paint;
};

/** @brief Paint target for painters.
*/
class PT_GFX_API PaintSurface
{
    friend class Painter;
    friend class PaintRegion;

    protected:
        PaintSurface();

    public:
        virtual ~PaintSurface();
        
        //
        // TODO: size/format/scaling same as Canvas, unify?
        //
        // TODO: protected setSize instead of virtual getter?
        //
        const Gfx::SizeF& size() const;

        const Gfx::ImageFormat& format() const;

        const Scaling& scaling() const;

    public:
        virtual Image toImage() const = 0;

        PaintData* getPaint(PaintData* paint);

        Canvas* beginPaint(PaintData& paint);

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const = 0;

        virtual const Gfx::SizeF& onSize() const = 0;

        virtual const Scaling& onGetScaling() const = 0;

        virtual PaintData* onGetPaint(PaintData* paint) = 0;

        virtual Canvas* onBeginPaint(PaintData& paint) = 0;

        virtual void onReset();

    private:
        void attachRegion(PaintRegion& region);
        
        void detachRegion(PaintRegion& region);

    private:
        void attachPainter(Painter& painter);

        void detachPainter(Painter& painter);

    private:
        Painter*                  _painter;
        std::vector<PaintRegion*> _regions;
};

} // namespace

} // namespace

#endif
