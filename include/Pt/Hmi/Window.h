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

#include <Pt/Hmi/WindowBase.h>
#include <Pt/Hmi/WindowManager.h>
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
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/ShowEvent.h>
#include <Pt/Hmi/EnableEvent.h>
#include <Pt/Hmi/InvalidateEvent.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/SizePolicy.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Signal.h>
#include <map>

namespace Pt {

namespace Hmi {

class Screen;
class Widget;
class MainWindowImpl;
class WindowStateEvent;

class PT_HMI_API Window : public WindowBase
                        , public View
{
  friend class Widget; 
 
  public:
    enum Type
    {
        Default = 0, // default window frame
        Popup = 1    // frameless window
    };
    
    enum State
    {
        Normal = 0,
        Minimized = 1,
        Maximized = 2
    };

  public:
    explicit Window(Window* parent = 0, Window::Type type = Default);

    virtual ~Window();

    PixmapSurface& surface();

    //WindowBase* parent();

    //const WindowBase* parent() const;

    Screen* screen();

    const Screen* screen() const;

    Window& mainWindow();

    const Window& mainWindow() const;

    Window* parentWindow();

    const Window* parentWindow() const;

    const std::vector<Window*>& windows() const;

    void add(Window& w);

    void remove(Window& w);

    Widget* mainWidget();

    const Widget* content()  const;

    void setContent(Widget* widget);

    Widget* findWidget(const Gfx::PointF& pos);

    Widget* findWidget(const std::string& name);

    Widget* findWidget(Pt::uint64_t vid);

    Widget* focusWidget();

    void focusNext();

    void focusPrev();

    // deprecated
    void update()
    { repaint(); }

    // deprecated
    void update(const Gfx::RectF& rect)
    { repaint(rect); }


    void invalidate();


    void repaint()
    {
        Gfx::RectF rect( Gfx::PointF(0, 0), size() );
        onRepaint(rect);
    }

    void repaint(const Gfx::RectF& rect)
    {
        onRepaint(rect);
    }


    bool isActive() const;

    void activate(bool active = true);

    bool isVisible() const;

    void show( bool b = true );

    void showModal();

    bool isEnabled() const;

    void enable( bool e = true );

    void grabPointer();

    void releasePointer();

    void move(const Gfx::PointF& p);

    void resize( const Gfx::SizeF& s );

    bool isClosed() const;

    void close();

    Type type() const;

    void setType(Type type);

    const Gfx::Image& icon() const;

    void setIcon(const Gfx::Image& i);

    const std::string& title() const;

    void setTitle( const std::string& t );

    const Gfx::SizeF& minimumSize() const;

    void setMinimumSize(const Gfx::SizeF& s);

    const Gfx::SizeF& maximumSize() const;

    void setMaximumSize(const Gfx::SizeF& s);

    bool isTopMost() const;

    void setTopMost(bool top);

    State state() const;

    void setState(State s);

    Pt::Signal<const Pt::Event&>& eventReady();

    MainWindowImpl* impl();

    const MainWindowImpl* impl() const; 

  public:
    const Gfx::Brush& background() const;

    void setBackground(const Gfx::Brush& b);

  protected:
    virtual void onSetScreen(Screen* screen);

    virtual void onAddWindow(Window& w);

    virtual void onRemoveWindow(Window& w);

    virtual void onParentChanged(Window* w);

    virtual void onAddWidget(Widget& w);

    virtual void onRemoveWidget(Widget& w);
  
  //
  // layouting
  //
  public:
    //Gfx::SizeF setSizePolicy(const SizePolicy& policy);

    // onMeasure
    virtual Gfx::SizeF measure(const SizePolicy& policy);

  protected:
    // onLayout
    virtual void layout(const Gfx::RectF& rect);

    void layoutEvent(const LayoutEvent& ev);

  //
  // painting
  //
  protected:
    // onPaint
    virtual void onPaintContent(const Gfx::RectF& r);

    // onPaintContent (window specific)
    virtual void onPaintContent(const PaintEvent& ev);

    void paintEvent(const PaintEvent& ev);

  protected:
    virtual Visual* onParent() const;

    Gfx::PointF onToParent(const Gfx::PointF& pos) const;

    Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

    virtual const Gfx::PointF& onPosition() const;

    virtual const Gfx::SizeF& onSize() const;

    virtual double onScaleFactor() const;

    virtual void onRelayout();

    virtual void onRepaint(const Gfx::RectF& rect);

    virtual void onRepaintView(View& view, const Gfx::RectF& rect);

  protected:
    virtual void onInvalidate();

  protected:
    virtual void onInit(Window& w);
    
    virtual void onDeinit(Window& w);

    virtual Gfx::PointF onToHost(const Window& w, const Gfx::PointF& pos) const;

    virtual Gfx::PointF onFromHost(const Window& w, const Gfx::PointF& pos) const;

    virtual void onShow(Window& w, bool visible);

    virtual void onActivate(Window& w, bool active);

    virtual void onEnable(Window& w, bool enable);

    virtual void onMove(Window& w, const Gfx::PointF& to);

    virtual void onResize(Window& w, const Gfx::SizeF& to);

    virtual void onFrameChanged(Window& w);

    virtual void onStateChanged(Window& w);

    virtual void onClose(Window& w);

    virtual void onClosing(Window& w);

  protected:
    virtual void onEvent(const Pt::Event& ev);

    void processMouseEvent(const MouseEvent& ev);

    virtual Responder* onNextResponder();

    virtual Gfx::PointF onToNextResponder(const Gfx::PointF& pos);

    virtual bool onMouseEvent(const MouseEvent& ev);

    void processTouchEvent( const TouchEvent& ev );

    virtual void onTouchEvent( const TouchEvent& ev );

    void processScrollEvent( const ScrollEvent& ev );

    virtual void onScrollEvent( const ScrollEvent& ev );

    virtual void onKeyEvent( const KeyEvent& ev );

    virtual void onEnterEvent( const EnterEvent& ev );

    virtual void onLeaveEvent(const LeaveEvent& ev );

    virtual void onMoveEvent(const MoveEvent& ev);

    virtual void onResizeEvent(const ResizeEvent& ev);

    virtual void onCloseEvent(const CloseEvent& ev);

    virtual void onActivateEvent(const ActivateEvent& ev);

    virtual void onShowEvent( const ShowEvent& ev);

    virtual void onEnableEvent(const EnableEvent& ev);

    virtual void onWindowStateEvent(const WindowStateEvent& ev);

    virtual void onInvalidateEvent(const InvalidateEvent& ev);

  private:
    void init(Window* parent);

    void deinit();

    void setParent(Window* parent);

  private:
    virtual Visual* onGetVisual();

    virtual Window* onGetWindow();

    virtual Screen* onGetScreen();

    virtual void onAttach(Widget& widget);

    virtual void onDetach(Widget& widget);

    virtual void onRaise(Widget& widget);

    virtual void onEnable(Widget& widget, bool isEnable);

    virtual void onShow(Widget& widget, bool isShown);

  protected:
    virtual void touchEvent(const TouchEvent& ev);

    virtual void scrollEvent(const ScrollEvent& ev);

  private:
    void addWidget(Widget& w);

    void removeWidget(Widget& w);

    void setFocusWidget(Widget* widget);

    template <typename Iter>
    void moveFocus(Iter begin, Iter end);

    void addFocusWidget(Widget& w);

    void removeFocusWidget(Widget& w);

    void setFocusIndex(Widget& w, size_t index);
    
    void setShortcut(Widget& w, const Key* key);

    void setMnemonic(Widget& w, const Char* ch);

    Widget* findWidget(const Gfx::PointF& pos, bool input);

    void setScreen(Screen* screen);

  private:
    MainWindowImpl*              _impl;
    WindowManager                _windowManager;
    PixmapSurface                _surface;
    Gfx::RectF                   _damageRect;
    Pt::Signal<const Pt::Event&> _eventReady;
    int                          _layouts;
    int                          _invalidates;

    std::vector<Window*>         _windows;
    WindowBase*                  _parent;
    Window*                      _parentWindow;
    Screen*                      _screen; 
    Widget*                      _mainWidget;
    Widget*                      _focusWidget;
    std::vector<Widget*>         _focusList;
    std::map<Key, Widget*>       _shortcuts; 
    std::map<Pt::Char, Widget*>  _mnemonics; 

    bool                         _init;
    bool                         _visible; 
    bool                         _isActive;
    bool                         _enabled; 
    bool                         _enabledState;
    bool                         _isClosed; 
    Gfx::PointF                  _position;
    Gfx::SizeF                   _size;
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

#endif // PT_HMI_WINDOW_H
