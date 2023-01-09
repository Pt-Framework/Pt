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
#include <Pt/Hmi/Sheet.h>
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

class ScreenImpl : public Sheet
                 , public Connectable
{
    typedef Sheet Base;

    public:
        ScreenImpl(ApplicationImpl& app);

        virtual ~ScreenImpl();

        
        void setParent(Screen* screen);

        void setNextResponder(Responder* r);


        void addWindow(Window& w);

        void removeWindow(Window& w);

        const std::vector<Window*>& windows() const;


        void setCapture(Visual* capture);

        
        //double scaleFactor() const;


        //bool isEnabled() const;

    public:
        void drawCursor(const Pt::Gfx::PointF& pos);

    //
    // Responder
    //
    protected:
        virtual Responder* onNextResponder();

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    //
    // Visual
    //
    protected:
        virtual Visual* onGetParent() const;

        virtual Visual* onHitTest(const Gfx::PointF& pos);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onEvent(const Event& ev);

        virtual void onRepaintRequest(const Gfx::RectF& rect);

    //
    // Sheet
    //
    protected:
        virtual void onAttach(Form& form);
    
        virtual void onDetach(Form& form);

        virtual void onInit(Form& form);

        virtual void onRelease(Form& form);

        virtual Gfx::PointF onFromForm(const Form& form, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onToForm(const Form& form, 
                                     const Gfx::PointF& pos) const;

        virtual void onRepaint(Form& form, const Gfx::RectF& rect);

        virtual void onActivate(Form& form, bool active);

        virtual void onMove(Form& form, const Gfx::PointF& pos);

        virtual void onResize(Form& form, const Gfx::SizeF& size);

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
        Form                         _form;
        Shell                        _shell;
                                     
        Responder*                   _nextResponder;

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
