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
  02110-1301 USA
*/

#ifndef Pt_Hmi_WindowManagerBase_h
#define Pt_Hmi_WindowManagerBase_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowType.h>
#include <Pt/Gfx/Point.h>

namespace Pt {

namespace Hmi {

class Window;
class WindowImpl;

class WindowManager
{
    friend class Window;

    public:
        WindowManager()
        {}

        virtual ~WindowManager()
        {}

        Visual& visual()
        {
            return onGetVisual();
        }

        Gfx::PointF toWindow(const Window& w, 
                             const Gfx::PointF& pos) const
        { 
            return onToWindow(w, pos); 
        }

        Gfx::PointF fromWindow(const Window& w,
                               const Gfx::PointF& pos) const
        { 
            return onFromWindow(w, pos); 
        }

    protected:
        virtual Visual& onGetVisual() = 0;

        virtual WindowImpl* onCreateWindow(const WindowType& type) = 0;

        virtual void onAttach(Window& w) = 0;

        virtual void onDetach(Window& w) = 0;

        virtual void onInit(Window& w) = 0;

        virtual void onRelease(Window& w) = 0;

        virtual Gfx::PointF onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const = 0;

        virtual void onRepaint(Window& w, const Gfx::RectF& rect) = 0;

        virtual void onShow(Window& w, bool visible) = 0; 

        virtual void onActivate(Window& w, bool active) = 0; 

        virtual void onEnable(Window& w, bool enable) = 0;

        virtual void onMove(Window& w, const Gfx::PointF& to) = 0;

        virtual void onResize(Window& w, const Gfx::SizeF& to) = 0;

        virtual void onSetAbove(Window& w, bool above) = 0;

        virtual void onSetTitle(Window& w, const std::string& text) = 0;

        virtual void onSetIcon(Window& w, const Gfx::Image& icon) = 0;

        virtual void onSetState(Window& w, const WindowState& state) = 0;

        virtual void onClosing(Window& w) = 0;
};

} // namespace

} // namespace

#endif
