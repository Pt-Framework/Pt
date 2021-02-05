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

#ifndef Pt_Hmi_ScreenImpl_H
#define Pt_Hmi_ScreenImpl_H

#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl;
class Window;
class WindowBase;
class MouseEvent;
class TouchEvent;
class ScrollEvent;

class ScreenImpl
{
    public:
        ScreenImpl(ApplicationImpl& app);
        
        virtual ~ScreenImpl();

        void init(WindowBase& w);

        double scaleFactor(const Window& w) const;

        double scaleFactor() const;

        Gfx::SizeF size() const;

        void registerWindow(Window& w);

        void unregisterWindow(Window& w);

        void dispatchMouseEvent(const MouseEvent& ev);

        void dispatchTouchEvent(const TouchEvent& ev);

        void dispatchScrollEvent(const ScrollEvent& ev);

        Gfx::PointF toParent(const Window& w, const Gfx::PointF& pos) const;

        Gfx::PointF fromParent(const Window& w, const Gfx::PointF& pos) const;

        void paint(const Gfx::RectF& rect);

        void onResize(Window& w, const Gfx::SizeF& s);

        void onMove(Window& w, const Gfx::PointF& p);

        void onFrameChanged(Window& w);

        void onStateChanged(Window& w);

        void onClosing(Window& w);

        void onClose(Window& w);

        void onShow(Window& w, bool visible);

        void onActivate(Window& w, bool active);

        void onEnable(Window& w, bool enable);
};

} // namespace

} // namespace

#endif // include guard
