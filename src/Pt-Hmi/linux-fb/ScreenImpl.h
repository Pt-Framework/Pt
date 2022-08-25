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

#include <Pt/Hmi/Form.h>
#include <Pt/Hmi/Shell.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl;
class FrameBuffer;
class Cursor;
class Screen;

class ScreenImpl : public Visual
                 , public Responder
                 , public Form
                 , public Connectable
{
    public:
        ScreenImpl(ApplicationImpl& app);

        virtual ~ScreenImpl();

        
        void setParent(Screen* screen);

        void setNextResponder(Responder* r);


        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;

        const Gfx::SizeF& size() const;

        double scaleFactor() const;

        bool isEnabled() const;

        void repaint(const Gfx::RectF& rect);

    public:
        void drawCursor(const Pt::Gfx::PointF& pos);

    //
    // Responder
    //
    protected:
        virtual Responder* onNextResponder();

        virtual Gfx::PointF onToScreen(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromScreen(const Gfx::PointF& pos) const;

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    //
    // Visual
    //
    protected:
        virtual void onEvent(const Event& ev);

        virtual void onSetCapture(bool capture);

    //
    // Form
    //
    protected:
        virtual void onAttach(Sheet& view);
    
        virtual void onDetach(Sheet& view);

        virtual void onInit(Sheet& view);

        virtual void onRelease(Sheet& view);

        virtual Gfx::PointF onFromSheet(const Sheet& sheet, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onToSheet(const Sheet& sheet, 
                                     const Gfx::PointF& pos) const;

        virtual Gfx::PointF onToScreen(const Sheet& sheet, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromScreen(const Sheet& sheet, 
                                         const Gfx::PointF& pos) const;

        virtual void onRepaint(Sheet& view, const Gfx::RectF& rect);

        virtual void onActivate(Sheet& w, bool active);

        virtual void onMove(Sheet& sheet, const Gfx::PointF& pos);

        virtual void onResize(Sheet& sheet, const Gfx::SizeF& size);

        virtual void onEnter(Sheet& sheet, Visual& v);

        virtual void onSetCapture(Sheet& sheet, bool capture);

    //
    // scaling
    //
    protected:
        void onProcessRescaleEvent(const RescaleEvent& ev);

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
        enum BlitOp
        {
            CopyOp,
            AndOp,
            XorOp
        };

        const Gfx::Image& image() const;

        Gfx::Image& image();
        
        void updateScreen(const Pt::Gfx::Rect& area);

        void drawCursor(Pt::uint8_t* buffer);
        
        void grabImage(const Pt::uint8_t* buffer, const Gfx::Point& pos,
                       Gfx::Image& image);
 
        void bitBlit(const Pt::uint8_t* from, size_t width, size_t height, 
                     const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op);

    private:
        Pt::Signal<const Pt::Event&> _eventReceived;

        FrameBuffer&                 _frameBuffer;
        PixmapSurface                _surface;

        Screen*                      _parent;
        Sheet                        _sheet;
        Shell                        _shell;
                                     
        Responder*                   _nextResponder;
        Visual*                      _capture;

        Gfx::SizeF                   _size;

        bool                         _enabled;
        bool                         _enabledState;
        
        double                       _dpi;       
        Gfx::Image                   _cursorBackground;
        Gfx::Point                   _cursorPos;
        bool                         _drawCursor;

        // TODO: return windows from _shell
        std::vector<Window*> _windows;
};

} // namespace

} // namespace

#endif
