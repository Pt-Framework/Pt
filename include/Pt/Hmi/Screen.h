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
  MA  02110-1301  USA
*/

#ifndef Pt_Hmi_Screen_H
#define Pt_Hmi_Screen_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowBase.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/Spacing.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Clock.h>
#include <map>
#include <vector>

namespace Pt {

namespace Hmi {

class ScreenImpl;
class ApplicationImpl;
class Widget;

class PT_HMI_API Screen : public WindowBase
{
    friend class Window;

    public:
        Screen(ApplicationImpl& app);

        virtual ~Screen();

        Window* findWindow(const std::string& name);

        Widget* findWidget(const std::string& name);

        const std::vector<Window*>& windows() const;
  
        ScreenImpl* impl();

        void setScaleFactor(double scale);

        double scaleFactor() const
        {
            return _scaling;
        }

        Gfx::PointF toPhysical(const Gfx::PointF& p) const
        {
            Gfx::PointF point;
            point.set(p.x() * _scaling, p.y()  * _scaling);
            return point;
        }

        Gfx::SizeF toPhysical(const Gfx::SizeF& s) const
        {
            Gfx::SizeF size(s.width()* _scaling, s.height() * _scaling);
            return size;
        }

        Gfx::RectF toPhysical(const Gfx::RectF& r) const
        {
            Gfx::RectF rect(toPhysical(r.topLeft()), toPhysical(r.size()));
            return rect;
        }

        Gfx::PointF toLogical(const Gfx::PointF& p) const
        {
            Gfx::PointF point;
            point.set(p.x() / _scaling, p.y() / _scaling);
            return point;
        }

        Gfx::SizeF toLogical(const Gfx::SizeF& s) const
        {
            Gfx::SizeF size(s.width()/ _scaling, s.height() / _scaling);
            return size;
        }

        Gfx::RectF toLogical(const Gfx::RectF& s) const
        {
            Gfx::RectF rect(toLogical(s.topLeft()), toLogical(s.size()));
            return rect;
        }

        virtual double toLogical(double n) const
        {
            return n / _scaling;
        }

        virtual double toPhysical(double n) const
        {
            return n * _scaling;
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
          // keep contour size when downscaling
          if( _scaling < 1.0 )
              return toLogical( static_cast<double>(n) );

          double p = toPhysical( static_cast<double>(n) );
          size_t s = static_cast<size_t>(p);
          return toLogical(static_cast<double>(s) );
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

        Spacing align(const Spacing& spacing) const
        {
            Spacing alignedSpacing( align(spacing.left()),
                                    align(spacing.top()),
                                    align(spacing.right()),
                                    align(spacing.bottom()));
            return alignedSpacing;
        }
       

    public:
        virtual Pt::Gfx::PointF toScreen(const Pt::Gfx::PointF& p) const;

        virtual Pt::Gfx::PointF fromScreen(const Pt::Gfx::PointF& p) const;

    protected:
        virtual Gfx::SizeF onSize() const;
        
        virtual void onUpdate(const Gfx::RectF& updateRect);

    protected:
        virtual void onInit(Window& w);
    
        virtual void onDeinit(Window& w);

        virtual Gfx::PointF onToParent(const Window& w, const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Window& w, const Gfx::PointF& pos) const;

        virtual void onResize(Window& w, const Gfx::SizeF& s);

        virtual void onMove(Window& w, const Gfx::PointF& p);

        virtual void onFrameChanged(Window& w);

        virtual void onStateChanged(Window& w);

        virtual void onClosing(Window& w);

        virtual void onClose(Window& w);

        virtual void onShow(Window& w, bool visible);

        virtual void onActivate(Window& w);

        virtual void onEnable(Window& w, bool enable);

        virtual void onUpdate(Window& w, const Gfx::RectF& rect);

    protected:
        virtual void onEvent( const Event& ev );
        
        virtual void onUpdateEvent(const UpdateEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);
    
    private:
        ScreenImpl*          _impl;
        Gfx::RectF           _updateRect;
        int                  _updates;
        std::vector<Window*> _windows;
        Pt::System::Clock    _clock;
        double             _scaling;
};

} // namespace

} // namespace

#endif
