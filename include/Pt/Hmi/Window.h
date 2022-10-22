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
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Form.h>
#include <Pt/Hmi/Sheet.h>
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
#include <Pt/Connectable.h>

#include <vector>

namespace Pt {

namespace Hmi {

class WindowManager;
class MainWindowImpl;
class WindowStateEvent;
class PaintEvent;

class WindowState
{
    public:
        enum State
        {
            Normal = 0,
            Minimized = 1,
            Maximized = 2
        };

        WindowState(State t = Normal)
        : _state(t)
        {}

        WindowState& operator=(State t)
        {
            _state = t;
            return *this;
        }

        operator Pt::uint32_t() const
        { 
            return _state; 
        }

    private:
        Pt::uint32_t _state;
};

/** @internal @brief Window implementation base class.
*/
class WindowImpl
{
    public:
        WindowImpl(WindowType type = WindowType::Default);

        virtual ~WindowImpl();

        virtual double scaleFactor() const = 0;

        WindowType type() const;

        void setType(WindowType type);

        const std::string& title() const;

        void setTitle(const std::string& text);

        const Gfx::Image& icon() const;
       
        void setIcon(const Gfx::Image& image);

        void setTopMost(bool isTop);

        bool isTopMost() const;

        WindowState state() const;

        void setState(WindowState s);

        const Gfx::SizeF& minimumSize() const;
        
        void setMinimumSize(const Gfx::SizeF& s);

        const Gfx::SizeF& maximumSize() const;
    
        void setMaximumSize(const Gfx::SizeF& s);

    protected:
        virtual void onSetType(WindowType type) = 0;

        virtual void onSetTitle(const std::string& text) = 0;

        virtual void onSetIcon(const Gfx::Image& p) = 0;

        virtual void onSetTopMost(bool top) = 0;

        virtual void onSetState(WindowState s) = 0;

        virtual void onSetMinimumSize(const Gfx::SizeF& s) = 0;

       virtual  void onSetMaximumSize(const Gfx::SizeF& s) = 0;

    private:
        //WindowParams*  _params;

        WindowType    _type;
        std::string   _title;
        Gfx::Image    _icon;
        bool          _isTopMost;
        WindowState   _state;
        Gfx::SizeF    _minimumSize;
        Gfx::SizeF    _maximumSize;
};

/** @brief Window base class.
*/
class PT_HMI_API Window : public Visual
                        , public Form
                        , public Pt::Connectable
{
    public:
        typedef WindowType Type;
        typedef WindowState State;

    public:
        explicit Window(WindowManager* parent = 0, WindowType type = WindowType::Default);

        virtual ~Window();

    
        void setParent(WindowManager& parent);

        void unparent();


        Visual* peer() const;

        void setTransient(Visual* owner);

    protected:
        void onTransientPeerClosed();

    public:
        PixmapSurface& surface();

        const PixmapSurface& surface() const;

        void setNextResponder(Responder* r);


        double scaleFactor() const;

        
        void invalidate();


        void repaint();

        void repaint(const Gfx::RectF& rect);

       // deprecated
        void update()
        { repaint(); }

        // deprecated
        void update(const Gfx::RectF& rect)
        { repaint(rect); }

        bool acceptsInput() const;


        bool isActive() const;

        void activate(bool active = true);


        bool isVisible() const;

        void show( bool b = true );

        void showModal();


        bool isEnabled() const;

        void enable(bool e = true);


        const Gfx::PointF& position() const;

        const Gfx::SizeF& size() const;

        const Gfx::RectF& geometry() const;

        void move(const Gfx::PointF& p);

        void resize(const Gfx::SizeF& s);

        void resize(const SizePolicy& policy);


        bool isClosed() const;

        void tryClose();

        void close(bool force = false);


        Type type() const;

        void setType(Type type);


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


        bool isTopMost() const;

        void setTopMost(bool top);


        State state() const;

        void setState(State s);


        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        Pt::Signal<const Pt::Event&>& eventReceived();

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

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        virtual void onEvent(const Pt::Event& ev);

        virtual void onRelease();

    //
    // Form
    //
    protected:
        virtual Visual& onGetVisual();

        virtual void onAttach(Sheet& view);
    
        virtual void onDetach(Sheet& view);

        virtual void onInit(Sheet& view);

        virtual void onRelease(Sheet& view);

        virtual Gfx::PointF onFromSheet(const Sheet& sheet, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onToSheet(const Sheet& sheet, 
                                     const Gfx::PointF& pos) const;

        virtual void onRepaint(Sheet& view, const Gfx::RectF& rect);

        virtual void onActivate(Sheet& w, bool active);

        virtual void onMove(Sheet& sheet, const Gfx::PointF& pos);

        virtual void onResize(Sheet& sheet, const Gfx::SizeF& size);

    //
    // invalidation
    //
    protected:
        void onProcessInvalidateEvent(const InvalidateEvent& ev);

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
        Pt::Signal<const Pt::Event&> _eventReceived;

        PixmapSurface                _surface;
        Gfx::RectF                   _damageRect;

        Sheet                        _sheet;

        WindowManager*               _parent;
        Responder*                   _nextResponder;
        Visual*                      _capture;
        Visual*                      _peer;
   
        int                          _invalidates;
        bool                         _visible; 
        bool                         _isActive;
        bool                         _enabled; 
        bool                         _enabledState;
        bool                         _isClosed; 

        Gfx::PointF                  _requestedPosition;
        Gfx::SizeF                   _requestedSize;
        Gfx::RectF                   _geometry;

        Type                         _type;
        std::string                  _title;
        Gfx::Image                   _icon;
        Gfx::SizeF                   _minimumSize;
        Gfx::SizeF                   _maximumSize;
        State                        _state;
        bool                         _topMost;
   
        AutoPtr<Gfx::Brush>          _background;
        Gfx::Brush                   _backgroundBrush;
};

} // namespace

} // namespace

#endif // include guard
