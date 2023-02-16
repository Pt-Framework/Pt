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
#include <Pt/Hmi/Sheet.h>
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

#include <vector>

namespace Pt {

namespace Hmi {

class WindowManager;
class MainWindowImpl;
class WindowStateEvent;
class PaintEvent;

/** @internal @brief Window implementation base class.
*/
class WindowImpl
{
    public:
        WindowImpl(WindowType type = WindowType::Default);

        virtual ~WindowImpl();

        virtual double scaleFactor() const = 0;

        WindowType type() const;

    private:
        //WindowParams*  _params;
        WindowType    _type;
};

/** @brief Window base class.
*/
class PT_HMI_API Window : public Sheet
{
    public:
        typedef Sheet Base;
        typedef WindowType Type;
        typedef WindowState State;

    public:
        explicit Window(WindowManager* parent = 0, WindowType type = WindowType::Default);

        virtual ~Window();

    
        void setParent(WindowManager& parent);

        void unparent();

        Gfx::Image getImage() const;

    public:
        Widget* content()
        { return _form.content(); }

        const Widget* content()  const
        { return _form.content(); }

        void setContent(Widget* widget)
        { _form.setContent(widget); }

        Widget* findWidget2(const Gfx::PointF& pos)
        { return _form.findWidget2(pos); }

        Widget* findWidget2(const std::string& name)
        { return _form.findWidget2(name); }

        Widget* findWidget2(Pt::uint64_t vid)
        { return _form.findWidget2(vid); }

    public:
        PixmapSurface& surface();

        const PixmapSurface& surface() const;

        void setNextResponder(Responder* r);


        bool acceptsInput() const;


        bool isActive() const;

        void activate(bool active = true);


        void show( bool b = true );

        void showModal();


        void enable(bool e = true);


        void move(const Gfx::PointF& p);

        void resize(const Gfx::SizeF& s);

        Gfx::SizeF resize(const SizePolicy& policy);


        bool isClosed() const;

        void tryClose();

        void close(bool force = false);


        Type type() const;


        const Gfx::Image& icon() const;

        void setIcon(const Gfx::Image& i);


        const std::string& title() const;

        void setTitle( const std::string& t );


        const Gfx::SizeF& minimumSize() const;

        void setMinimumSize(const Gfx::SizeF& s);

        void setMinimumWidth(double w);

        void setMinimumHeight(double h);

        
        const Gfx::SizeF& maximumSize() const;

        void setMaximumSize(const Gfx::SizeF& s);

        void setMaximumWidth(double w);

        void setMaximumHeight(double h);


        bool isAbove() const;

        void setAbove(bool top);


        // TODO: setFullScreen()

        WindowState state() const;

        void setState(const WindowState& s);


        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

    public:
        WindowImpl* impl();

        const WindowImpl* impl() const; 

    protected:
        virtual void onParentChanged(WindowManager*);

    //
    // Responder
    //
    protected:
        virtual Responder* onNextResponder();

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent( const TouchEvent& ev );
    
        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

    //
    // Visual
    //
    protected:
        virtual Visual* onGetParent() const;

        Visual* onHitTest(const Gfx::PointF& p);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onEvent(const Pt::Event& ev);

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

        virtual void onShowRequest(Form& form, bool isShow);

        virtual void onMove(Form& form, const Gfx::PointF& pos);

        virtual void onResize(Form& form, const Gfx::SizeF& size);

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

        virtual void onPaint(Gfx::PaintSurface& surface, 
                             const Gfx::RectF& updateRect);

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
    // closing
    //
    protected:
        virtual void onProcessCloseEvent(const CloseEvent& ev);

        virtual void onCloseEvent(const CloseEvent& ev);

    //
    // activation
    //
    protected:
        virtual void onProcessActivateEvent(const ActivateEvent& ev);

        virtual void onActivateEvent(const ActivateEvent& ev);

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
    // window state
    //
    protected:
        virtual void onProcessWindowStateEvent(const WindowStateEvent& ev);

        virtual void onWindowStateEvent(const WindowStateEvent& ev);

    private:
        WindowImpl*                  _impl;

        PixmapSurface                _surface;
        Responder*                   _nextResponder;
        Form                         _form;

        WindowManager*               _parent;

        bool                         _show; 
        bool                         _isActive;
        bool                         _enabled; 
        bool                         _isClosed; 

        Gfx::PointF                  _requestedPosition;
        Gfx::SizeF                   _requestedSize;

        Type                         _type;
        std::string                  _title;
        Gfx::Image                   _icon;
        Gfx::SizeF                   _minimumSize;
        Gfx::SizeF                   _maximumSize;
        State                        _state;
        bool                         _isAbove;
   
        AutoPtr<Gfx::Brush>          _background;
        Gfx::Brush                   _backgroundBrush;
};

} // namespace

} // namespace

#endif // include guard
