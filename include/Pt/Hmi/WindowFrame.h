/* Copyright (C) 2015 Marc Boris Duerner 
  
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
  02110-1301  USA
*/
 
#ifndef PT_HMI_WINDOWFRAME_H
#define PT_HMI_WINDOWFRAME_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/WindowType.h>
#include <Pt/Hmi/PixmapSurface.h>

namespace Pt {

namespace Hmi {

class WindowManager;
class Window;

/** @internal @brief Window implementation base class.
*/
class WindowFrame : public Visual
{
    typedef Visual Base;

    friend class Window;

    public:
        WindowFrame(WindowManager& wm, Window& window);

        virtual ~WindowFrame();

        Window& window();

        const Window& window() const;

        PixmapSurface& surface();

        const PixmapSurface& surface() const;

    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

        
        virtual void onProcessActivateEvent(const ActivateEvent& ev);

        virtual void onActivateEvent(const ActivateEvent& ev);


        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);

        virtual void onWindowStateEvent(const WindowStateEvent& ev);

        
        virtual void onProcessCloseEvent(const CloseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    protected:
        virtual void onInit(Window& w) = 0;

        virtual void onRelease(Window& w) = 0;

        virtual Gfx::PointF onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const = 0;

        virtual void onSetTitle(Window& w, const std::string& text) = 0;

        virtual void onSetIcon(Window& w, const Gfx::Image& icon) = 0;

        virtual void onSetState(Window& w, const WindowState& state) = 0;

        virtual void onSetAbove(Window& w, bool above) = 0;

        virtual void onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
                                                const Gfx::SizeF& maxSize) = 0;

        virtual void onRepaint(Window& w, const Gfx::RectF& rect) = 0;

        virtual void onShow(Window& w, bool visible) = 0;

        virtual void onActivate(Window& w, bool active) = 0;

        virtual void onEnable(Window& w, bool enable) = 0;

        virtual void onMove(Window& w, const Gfx::PointF& to) = 0;

        virtual void onResize(Window& w, const Gfx::SizeF& s) = 0;

        virtual void onClose(Window& w) = 0;

    private:
        WindowManager& _wm;
        Window&        _window;
        PixmapSurface  _surface;
};

} // namespace

} // namespace

#endif // include guard
