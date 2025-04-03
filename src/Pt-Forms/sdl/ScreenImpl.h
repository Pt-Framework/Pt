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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef Pt_Forms_ScreenImpl_H
#define Pt_Forms_ScreenImpl_H

#include <Pt/Forms/Form.h>
#include <Pt/Forms/Shell.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>

#include <SDL.h>

namespace Pt {

namespace Forms {

class ApplicationImpl;
class Cursor;
class Screen;

class ScreenImpl : public Form
{
    typedef Form Base;

  public:
    // take typedefs from FramBuffer
    typedef Gfx::ImageView::Point Point;
    typedef Gfx::ImageView::Size Size;
    typedef Gfx::ImageView::Rect Rect;

    public:
        ScreenImpl(ApplicationImpl& app);

        virtual ~ScreenImpl();

        
        void setParent(Screen* screen);

        
        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;

        WindowManager& windowManager();

        
        void setCapture(Widget* capture);

    //
    // Responder
    //
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    //
    // Widget
    //
    protected:
        virtual Widget* onHitTest(const Gfx::PointF& pos);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onProcessEvent(const Event& ev);

        virtual void onRequestResize(const Gfx::SizeF& s);

        virtual void onRequestRepaint(const Gfx::RectF& rect);

    //
    // scaling
    //
    protected:
        void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);

        virtual void onResizeEvent(const ResizeEvent& ev);

    //
    // enable
    //
    protected:
        void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnable(bool e);

    //
    // painting
    //
    protected:
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

        virtual void onPaint(Gfx::PaintSurface& surface, 
                             const Gfx::RectF& updateRect);

    //
    // input
    //
    protected:
        void onProcessMouseEvent(const MouseEvent& ev);

        void onProcessTouchEvent(const TouchEvent& ev);

        void onProcessScrollEvent(const ScrollEvent& ev);

        void onProcessKeyEvent(const KeyEvent& ev);

    private:
        Screen*      _parent;
        Shell        _shell;

        Pixmap       _pixmap;

        SDL_Window*   _screen;
        SDL_Surface*  _imageSurface;
};

} // namespace

} // namespace

#endif
