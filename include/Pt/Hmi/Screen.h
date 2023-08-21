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
#include <Pt/Hmi/Visual.h>
#include <Pt/Gfx/Size.h>
#include <Pt/System/Clock.h>
#include <Pt/Signal.h>

#include <vector>

namespace Pt {

namespace Hmi {

class ScreenImpl;
class ApplicationImpl;

class Window;
class WindowManager;
class RescaleEvent;

/** @brief Screen of a display.
*/
class PT_HMI_API Screen : public Visual
{
    friend class ScreenImpl;

    typedef Visual Base;

    public:
        Screen(ApplicationImpl& app);

        virtual ~Screen();

        
        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;

        WindowManager& windowManager();
       
        
        Visual* underPointer();

        void setPointer(Visual* visual);

        void setPointer(Visual& visual, bool isPointer);

    public:
        ScreenImpl* impl();
    
    protected:
        virtual void onInit(ScreenImpl& s);

        virtual void onRelease(ScreenImpl& s);

        virtual void onResize(ScreenImpl& s, const Gfx::SizeF& size);

        virtual void onShow(ScreenImpl& s, bool isShow);

    //
    // Visual
    //
    protected:
        virtual Visual* onHitTest(const Gfx::PointF& pos);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const;

        virtual void onProcessEvent( const Event& ev );

        virtual void onRequestRepaint(const Gfx::RectF& rect);

    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);

    protected:
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

        virtual void onPaint(const Gfx::RectF& rect);
        
    protected:
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    private:
        ScreenImpl*         _impl;
        Gfx::RectF          _updateRect;
        int                 _updates;
        Visual*             _pointer;
        Pt::System::Clock   _clock;
};

} // namespace

} // namespace

#endif
