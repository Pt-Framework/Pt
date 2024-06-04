/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Boris Duerner 
 
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

/** @brief Surface scaling.
*/
class Scaling
{
    public:
        Scaling(double scaleFactor = 1.0)
        : _scaleFactor(scaleFactor)
        { }

        double scaleFactor() const
        {
            return _scaleFactor;
        }
        
        void setScaleFactor(double scaleFactor)
        {
            _scaleFactor = scaleFactor;
        }

    public:
        double toPhysical(double n) const
        {
            return n * scaleFactor();
        }

        Gfx::PointF toPhysical(const Gfx::PointF& p) const
        {
            return p * scaleFactor();
        }

        Gfx::SizeF toPhysical(const Gfx::SizeF& s) const
        {
            return s * scaleFactor();
        }

        Gfx::RectF toPhysical(const Gfx::RectF& r) const
        {
            Gfx::PointF p = toPhysical( r.topLeft() );
            Gfx::SizeF s = toPhysical( r.size() );
            return Gfx::RectF(p, s);
        }

        double toLogical(double n) const
        {
            return n / scaleFactor();
        }

        Gfx::PointF toLogical(const Gfx::PointF& p) const
        {
            return p / scaleFactor();
        }

        Gfx::SizeF toLogical(const Gfx::SizeF& s) const
        {
            return s / scaleFactor();
        }

        Gfx::RectF toLogical(const Gfx::RectF& r) const
        {
            Gfx::PointF p = toLogical( r.topLeft() );
            Gfx::SizeF s = toLogical( r.size() );
            return Gfx::RectF(p, s);
        }

    private:
        double _scaleFactor;
};

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

    private:
        void attachPaint(PaintData& paint);

        void detachPaint();

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
        PaintData* _paint;
};

/** @brief Paint target for painters.
*/
class PT_GFX_API PaintSurface
{
    friend class PaintData;
    friend class Painter;
    friend class PaintRegion;

    protected:
        PaintSurface();

    public:
        virtual ~PaintSurface();
        
        const Gfx::SizeF& size() const;

        const Gfx::ImageFormat& format() const;

        const Scaling& scaling() const
        {
            return onGetScaling();
        }

        double scaleFactor() const
        {
            return onScaleFactor();
        }

        virtual Image toImage() const = 0;

    protected:
        Painter* painter();

        PaintData* getPaint(PaintData* paint);

        Canvas* canvas();

        RectF region() const;

    private:
        void attachPainter(Painter& painter);

        void detachPainter();

    private:
        void attachRegion(PaintSurface& region);
        
        void detachRegion(PaintSurface& region);

        virtual void onDestroy(PaintSurface* region);

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const = 0;

        virtual const Gfx::SizeF& onSize() const = 0;

        virtual double onScaleFactor() const = 0;

        virtual const Scaling& onGetScaling() const = 0;

        virtual PaintData* onGetPaint(PaintData*) = 0;

        virtual Canvas* onGetCanvas() = 0;

        virtual RectF onGetRegion() const;

        virtual void onReset();

    public:
        Gfx::PointF toPhysical(const Gfx::PointF& p) const
        {
            return p * scaleFactor();
        }

        Gfx::SizeF toPhysical(const Gfx::SizeF& s) const
        {
            return s * scaleFactor();
        }

        Gfx::RectF toPhysical(const Gfx::RectF& r) const
        {
            return Gfx::RectF(toPhysical(r.topLeft()), toPhysical(r.size()));
        }

        Gfx::PointF toLogical(const Gfx::PointF& p) const
        {
            return p / scaleFactor();
        }

        Gfx::SizeF toLogical(const Gfx::SizeF& s) const
        {
            return s / scaleFactor();
        }

        Gfx::RectF toLogical(const Gfx::RectF& r) const
        {
            return Gfx::RectF(toLogical(r.topLeft()), toLogical(r.size()));
        }

        double toLogical(double n) const
        {
            return n / scaleFactor();
        }

        double toPhysical(double n) const
        {
            return n * scaleFactor();
        }

        double align(double n) const
        {
            // better name: alignGrid()

            double p = toPhysical(n);
            p = lround(p);
            return toLogical(p);
        }

        double alignPixel(double n) const
        {
            double p = toPhysical(n);
            p = lround(p + 0.5) - 0.5;
            return toLogical(p);
        }

        double alignContour(size_t n) const
        {
            const double scaling = scaleFactor();
            // keep contour size when downscaling
            if (scaling < 1.0)
                return toLogical( static_cast<double>(n) );

            double p = toPhysical( static_cast<double>(n) );
            size_t s = static_cast<size_t>(p);
            return toLogical( static_cast<double>(s) );
        }

        Gfx::PointF align(const Gfx::PointF& p) const
        {
            Gfx::PointF pos = toPhysical(p);
            pos.setX(lround(pos.x()));
            pos.setY(lround(pos.y()));
            return toLogical(pos);
        }

        Gfx::SizeF align(const Gfx::SizeF& s) const
        {
            Gfx::SizeF size = toPhysical(s);
            size.setWidth(lround(size.width()));
            size.setHeight(lround(size.height()));
            return toLogical(size);
        }

        Gfx::RectF align(const Gfx::RectF& rect) const
        {
            Gfx::PointF pos = toPhysical(rect.topLeft());
            pos.setX(lround(pos.x()));
            pos.setY(lround(pos.y()));

            Gfx::SizeF size = toPhysical(rect.size());
            size.setWidth(lround(size.width()));
            size.setHeight(lround(size.height()));

            return toLogical(Gfx::RectF(pos, size));
        }

    private:
        Painter*                   _painter;
        std::vector<PaintSurface*> _regions;
};

} // namespace

} // namespace

#endif
