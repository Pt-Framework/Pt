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

#ifndef Pt_Forms_ScreenImpl_H
#define Pt_Forms_ScreenImpl_H

#include "FrameBuffer.h"

#include <Pt/Forms/Form.h>
#include <Pt/Forms/Workspace.h>
#include <Pt/Forms/Window.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Forms {

class ApplicationImpl;
class FrameBuffer;
class Cursor;
class Screen;

class ScreenImpl : public Form
{
    typedef Form Base;

    friend class Screen;

  public:
    // take typedefs from FramBuffer
    typedef Gfx::Image::pos_t      pos_t;
    typedef Gfx::BasicPoint<pos_t> PointI;
    typedef Gfx::BasicSize<pos_t>  SizeI;
    typedef Gfx::BasicRect<pos_t>  RectI;

    public:
        ScreenImpl(ApplicationImpl& app);

        virtual ~ScreenImpl();

        
        void setParent(Screen* screen);

        
        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;

        WindowManager& windowManager();


        void setCapture(Widget* capture);

        
        //double scaleFactor() const;


        //bool isEnabled() const;

    public:
        void drawCursor(const Pt::Gfx::PointF& pos);

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

        virtual void onRequestRepaint(const Gfx::RectF& rect);

    //
    // scaling
    //
    protected:
        void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);

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

        virtual void onPaint(PaintSurface& surface, 
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
        enum BlitOp
        {
            CopyOp,
            AndOp,
            XorOp
        };

        const Gfx::Image& image() const;
        
        void updateScreen(const RectI& area);

        void drawCursor(Pt::uint8_t* buffer);
        
        void grabImage(const Pt::uint8_t* buffer, const PointI& pos,
                       Gfx::Image& image);
 
        void bitBlit(const Pt::uint8_t* from, size_t width, size_t height, 
                     const PointI& pos, Pt::uint8_t* buffer, BlitOp op);

    private:
        FrameBuffer&                 _frameBuffer;
        Pixmap                       _pixmap;

        Screen*                      _parent;
        Workspace                    _workspace;
        
        double                       _dpi;       
        Gfx::Image                   _cursorBackground;
        PointI                       _cursorPos;
        bool                         _drawCursor;
};

} // namespace

} // namespace

#endif
