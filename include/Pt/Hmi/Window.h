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

#ifndef PT_HMI_WINDOW_H
#define PT_HMI_WINDOW_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Form.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Hmi/WindowType.h>
#include <Pt/Hmi/SizePolicy.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/EnterEvent.h>
#include <Pt/Hmi/LeaveEvent.h>
#include <Pt/Hmi/ShowEvent.h>
#include <Pt/Hmi/EnableEvent.h>
#include <Pt/Hmi/InvalidateEvent.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Hmi {

class WindowFrame;
class WindowManager;
class WindowStateEvent;
class PaintEvent;

/** @brief Window base class.
*/
class PT_HMI_API Window : public Form
{
    public:
        typedef Form Base;
        typedef WindowType Type;
        typedef WindowState State;

    public:
        explicit Window(WindowManager* parent = 0, 
                        WindowType type = WindowType::Default);

        virtual ~Window();

    public:
        void setParent(WindowManager& parent);

        void unparent();

        WindowManager* windowManager();

        const WindowManager* windowManager() const; 

        WindowFrame* frame();

        const WindowFrame* frame() const; 

    public:
        Gfx::Image getImage() const;

    public:
        bool isAutoSize() const;

        Gfx::SizeF setAutoSize(const SizePolicy& policy);


        Type type() const;

        
        const Gfx::Image& icon() const;

        void setIcon(const Gfx::Image& i);

        const std::string& title() const;

        void setTitle( const std::string& t );

        
        bool isAbove() const;

        void setAbove(bool top);

        
        bool isActive() const;


        // TODO: setFullScreen()

        WindowState state() const;

        void setState(const WindowState& s);


        void showModal();


        bool isClosed() const;

        void close();


        bool acceptsInput() const;

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

    //
    // Form
    //
    protected:
        virtual Gfx::SizeF onMeasure();

        virtual void onLayoutEvent(const LayoutEvent& ev);

    //
    // Visual
    //
    protected:
        Visual* onHitTest(const Gfx::PointF& p);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onProcessEvent(const Pt::Event& ev);

        virtual void onRequestRepaint(const Gfx::RectF& rect);

        virtual void onRequestEnable(bool isEnable);

        virtual void onRequestActivate(bool active);

        virtual void onRequestShow(bool isShown);

        virtual void onRequestMove(const Gfx::PointF& pos);

        virtual void onRequestResize(const Gfx::SizeF& s);

        virtual void onSetSizeLimits(const Gfx::SizeF& minSize,
                                     const Gfx::SizeF& maxSize);

    //
    // invalidation
    //
    protected:
        virtual void onInvalidateEvent(const InvalidateEvent& ev);

        virtual void onInvalidate();

    //
    // painting
    //
    protected:
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

    //
    // scaling
    //
    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);

    //
    // geometry
    //
    protected:
        virtual void onProcessMoveEvent(const MoveEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    //
    // visibility
    //
    protected:
        virtual void onProcessShowEvent(const ShowEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);

        virtual void onShow(bool visible);

    //
    // enabling
    //
    protected:
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onEnable(bool e);

    //
    // activation
    //
    protected:
        virtual void onProcessActivateEvent(const ActivateEvent& ev);

        virtual void onActivateEvent(const ActivateEvent& ev);

    //
    // window state
    //
    protected:
        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);

        virtual void onWindowStateEvent(const WindowStateEvent& ev);

    //
    // closing
    //
    protected:
        virtual void onProcessCloseEvent(const CloseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    //
    // input
    //
    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent( const TouchEvent& ev );
    
        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

    private:
        WindowFrame*                 _frame;
        WindowManager*               _wm;

        bool                         _show; 
        bool                         _isActive;
        bool                         _enabled; 
        bool                         _isClosed; 

        Gfx::PointF                  _requestedPosition;
        Gfx::SizeF                   _requestedSize;

        SizePolicy                   _sizePolicy;
        bool                         _autoSize;

        Type                         _type;
        std::string                  _title;
        Gfx::Image                   _icon;
        State                        _state;
        bool                         _isAbove;
   
        AutoPtr<Gfx::Brush>          _background;
        Gfx::Brush                   _backgroundBrush;
};

} // namespace

} // namespace

#endif // include guard
