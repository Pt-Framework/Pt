 /* 
  Copyright (C) 2015 Marc Boris Duerner 
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

#include "FrameBuffer.h"

#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl;
class FrameBuffer;
class Cursor;

class ScreenImpl : public Pt::Connectable
{
    public:
        ScreenImpl(ApplicationImpl& app);

        virtual ~ScreenImpl();

        void init(WindowBase& w);

        void registerWindow(Window& w);

        void unregisterWindow(Window& w);

        const FrameBuffer& frameBuffer() const
        {
            return _frameBuffer;
        }

        void drawCursor(const Pt::Hmi::MouseEvent& mev);

        Gfx::SizeF size() const
        {
            Gfx::Size fs = _frameBuffer.size();
            return Gfx::SizeF(fs.width(), fs.height());
        }

        const Gfx::Image& image() const;

        Gfx::Image& image();

        void paint(const Gfx::RectF& updateRect);

        WindowManager& windowManager()
        {
            return _windowManager;
        }

        double scaleFactor(const Window&) const
        {
            return 1.0;
        }

        double scaleFactor() const
        {
            return 1.0;
        }

    public:
        void dispatchMouseEvent(const MouseEvent& ev);

        void dispatchTouchEvent(const TouchEvent& ev);

        void dispatchScrollEvent(const ScrollEvent& ev);

        void dispatchKeyEvent(const KeyEvent& ev);

        Gfx::PointF toParent(const Window& w, const Gfx::PointF& pos) const;

        Gfx::PointF fromParent(const Window& w, const Gfx::PointF& pos) const;

        void onResize(Window& w, const Gfx::SizeF& s);

        void onMove(Window& w, const Gfx::PointF& pos);

        void onFrameChanged(Window& w);

        void onStateChanged(Window& w);

        void onClosing(Window& w);

        void onClose(Window& w);

        void onShow(Window& w, bool visible);

        void onActivate(Window& w);

        void onEnable(Window& w, bool enable);

    private:
        enum BlitOp
        {
            CopyOp,
            AndOp,
            XorOp
        };
        
        void updateScreen(const Pt::Gfx::Rect& area);

        void drawCursor(Pt::uint8_t* buffer);
        
        void grabImage(const Pt::uint8_t* buffer, const Gfx::Point& pos,
                       Gfx::Image& image);
 
        void bitBlit(const Pt::uint8_t* from, size_t width, size_t height, 
                     const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op);

    private:
        FrameBuffer&  _frameBuffer;
        Gfx::Image    _cursorBackground;
        Gfx::Point    _cursorPos;
        double        _dpi;
        bool          _drawCursor;
        WindowManager _windowManager;
        PixmapSurface _surface;
};

} // namespace

} // namespace

#endif
