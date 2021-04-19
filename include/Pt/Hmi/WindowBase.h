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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301  USA
*/

#ifndef PT_HMI_WINDOWBASE_H
#define PT_HMI_WINDOWBASE_H

#include <Pt/Hmi/Visual.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>

#include <vector>

namespace Pt {

namespace Hmi {

class Window;

// WindowHost
class PT_HMI_API WindowBase : public virtual Visual
{
    friend class Window;

    public:
        WindowBase();

        virtual ~WindowBase();

        void moveWindow(Window& w, const Gfx::PointF& to)
        {
            return onMove(w, to);
        }

        void resizeWindow(Window& w, const Gfx::SizeF& to)
        {
            return onResize(w, to);
        }

        Gfx::PointF toHost(const Window& w, const Gfx::PointF& pos)
        {
            return onToHost(w, pos);
        }

        Gfx::PointF fromHost(const Window& w, const Gfx::PointF& pos) const
        {
            return onFromHost(w, pos);
        }

    protected:
        virtual void onInit(Window& w) = 0;

        virtual void onDeinit(Window& w) = 0;

        virtual Gfx::PointF onToHost(const Window& w, const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromHost(const Window& w, const Gfx::PointF& pos) const = 0;

        virtual void onShow(Window& w, bool visible) = 0; 

        virtual void onActivate(Window& w, bool active) = 0; 

        virtual void onEnable(Window& w, bool enable) = 0;

        virtual void onMove(Window& w, const Gfx::PointF& to) = 0;

        virtual void onResize(Window& w, const Gfx::SizeF& to) = 0;

        virtual void onFrameChanged(Window& w) = 0;

        virtual void onStateChanged(Window& w) = 0; 

        virtual void onClosing(Window& w) = 0;

        virtual void onClose(Window& w) = 0;
};

} // namespace

} // namespace

#endif // include guard
