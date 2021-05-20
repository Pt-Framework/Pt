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
    public:
        Screen(ApplicationImpl& app);

        virtual ~Screen();

        Window* findWindow(const std::string& name);

        Widget* findWidget(const std::string& name);

        const std::vector<Window*>& windows() const;

        void repaint()
        {
            Gfx::RectF rect( Gfx::PointF(0, 0), size() );
            onRepaint(rect);
        }

        void repaint(const Gfx::RectF& rect)
        {
            onRepaint(rect);
        }

        ScreenImpl* impl();

    protected:
        virtual Visual* onParent() const;

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual const Gfx::PointF& onPosition() const;

        virtual const Gfx::SizeF& onSize() const;

        virtual double onScaleFactor() const;

        virtual void onRelayout();

        virtual void onRepaint(const Gfx::RectF& r);

    protected:
        virtual Gfx::SizeF onMeasureContent(const SizePolicy& policy)
        { 
            return Gfx::SizeF(0, 0); 
        }

        virtual void onLayoutContent(const Gfx::RectF& rect)
        {}

    protected:
        // onPaint
        void onPaintContent(const Gfx::RectF& r);

        // onPaintContent (screen specific)
        void onPaintScreen(const Gfx::RectF& rect);

        Responder* onNextResponder()
        {
            // TODO: possibly pass on to application
            return 0;
        }

        Gfx::PointF onToNextResponder(const Gfx::PointF& pos)
        {
            // TODO: possibly pass on to application
            return pos;
        }

        virtual bool onMouseEvent(const MouseEvent& ev)
        { 
            // TODO: possibly pass on to application
            return false; 
        }

    protected:
        virtual void onInit(Window& w);
    
        virtual void onDeinit(Window& w);

        virtual Gfx::PointF onToHost(const Window& w, const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromHost(const Window& w, const Gfx::PointF& pos) const;

        virtual void onResize(Window& w, const Gfx::SizeF& s);

        virtual void onMove(Window& w, const Gfx::PointF& p);

        virtual void onFrameChanged(Window& w);

        virtual void onStateChanged(Window& w);

        virtual void onClosing(Window& w);

        virtual void onClose(Window& w);

        virtual void onShow(Window& w, bool visible);

        virtual void onActivate(Window& w, bool active);

        virtual void onEnable(Window& w, bool enable);

    protected:
        virtual void onEvent( const Event& ev );
        
        virtual void paintEvent(const PaintEvent& ev);
    
    private:
        ScreenImpl*          _impl;
        Gfx::PointF          _position;
        Gfx::SizeF           _size;
        Gfx::RectF           _updateRect;
        int                  _updates;
        std::vector<Window*> _windows;
        Pt::System::Clock    _clock;
};

} // namespace

} // namespace

#endif
