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

#include <Pt/Hmi/WindowStartPosition.h>
#include <Pt/Hmi/WindowState.h>
#include <Pt/Hmi/WindowDecoration.h>
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
#include <Pt/Hmi/Visual.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <map>

namespace Pt {

namespace Hmi {

class Widget;
class MainWindowImpl;

class PT_HMI_API Window : public Visual
{
   friend class Widget; 
   friend class Screen;

  public:
    explicit Window(Window* parent = 0);

    virtual ~Window();

    Window* parent();

    const Window* parent() const;

    const std::vector<Window*>& windows() const;

    void add(Window& w);

    void remove(Window& w);

    Window* activeWindow();

    Widget* mainWidget();

    const Widget* mainWidget()  const;

    void setMainWidget(Widget* widget);

    Widget* findWidget(const Gfx::PointF& pos);

    Widget* pointerWidget();

    const Widget* pointerWidget() const;

    Widget* focusWidget();

    void focusNext();

    void focusPrev();

    Gfx::PointF toParent(const Gfx::PointF& pos) const;

    Gfx::PointF toScreen(const Gfx::PointF& pos) const;

    void update();

    void update(const Gfx::RectF& rect);

    bool isActive() const;

    void activate();

    bool isVisible() const;

    void show( bool b = true );

    void showModal();

    bool isEnabled() const;

    void enable( bool e = true );

    const Gfx::PointF& position() const;

    void move(const Gfx::PointF& p);

    const Gfx::SizeF& size() const;

    void resize( const Gfx::SizeF& s );

    bool isClosed() const;

    void close();    

    PixmapSurface& surface();

    MainWindowImpl* impl();

  protected:
    void onUpdate(Window& w, const Gfx::RectF& rect);

    void onPaint(const Gfx::RectF& rect);

    void onShow(Window& w, bool visible);

    void onActivate(Window& w);

    void onEnable(Window& w, bool enable);

    void onMove(Window& w, const Gfx::PointF& to);

    void onResize(Window& w, const Gfx::SizeF& to);

    void onClose(Window& w);

    void onClosing(Window& w);

    Gfx::PointF onToParent(const Window& w, const Gfx::PointF& pos) const;

  protected:
    virtual void onEvent(const Pt::Event& ev);

    virtual void onPaintEvent(const PaintEvent& ev);

    virtual void onMouseEvent( const MouseEvent& ev );

    virtual void onTouchEvent( const TouchEvent& ev );

    virtual void onScrollEvent( const ScrollEvent& ev );

    virtual void onKeyEvent( const KeyEvent& ev );

    virtual void onEnterEvent( const EnterEvent& ev );

    virtual void onLeaveEvent(const LeaveEvent& ev );

    virtual void onMoveEvent(const MoveEvent& ev);

    virtual void onResizeEvent(const ResizeEvent& ev);

    virtual void onCloseEvent(const CloseEvent& ev);

    virtual void onActivateEvent(const ActivateEvent& ev);

    virtual void onShowEvent( const ShowEvent& ev );

    virtual void onEnableEvent( const EnableEvent& ev );

  public:
    // TODO:
    const Gfx::SizeF& minimumSize() const;

    // TODO:
    void setMinimumSize(const Gfx::SizeF& s);

    // TODO:
    const Gfx::SizeF& maximumSize() const;

    // TODO:
    void setMaximumSize(const Gfx::SizeF& s);

    // TODO:
    Hmi::WindowPosition::Type defaultPosition() const;

    // TODO:
    void setDefaultPosition(Hmi::WindowPosition::Type p);

    // TODO:
    Hmi::WindowState::Type state() const;

    // TODO:
    void setState(Hmi::WindowState::Type s);
    
    // TODO:
    bool hasBorder() const;

    // TODO:
    void setBorder(bool s);

    // TODO:
    const Gfx::Image& icon() const;

    // TODO:
    void setIcon(const Gfx::Image& i);

    // TODO:
    bool isClosable() const;

    // TODO:
    void setClosable(bool c);

    // TODO:
    const std::string& title() const;

    // TODO:
    void setTitle( const std::string& t );

    // TODO:
    const Gfx::Font& font() const;

    // TODO:
    void setFont(const Gfx::Font& ft);

    // TODO:
    WindowDecoration::Flags decoration() const;

    // TODO:
    void setDecoration( WindowDecoration::Flags d );

  private:
    void init(Window* parent);

    void deinit();

    void addWidget(Widget& w);

    void removeWidget(Widget& w);
    
    void setPointerWidget( Widget* widget );

    void setFocusWidget(Widget* widget);

    template <typename Iter>
    void moveFocus(Iter begin, Iter end);

    void addFocusWidget(Widget& w);

    void removeFocusWidget(Widget& w);

    void setFocusIndex(Widget& w, size_t index);
    
    void setShortcut(Widget& w, const Key* key);

    void setMnemonic(Widget& w, const Char* ch);

    void onEnable( bool b );

  private:      
    MainWindowImpl*                _impl;
    WindowManager                  _windowManager;
    PixmapSurface                  _surface;
    Pt::Signal<const Pt::Event&>   _eventReady;

    std::vector<Window*>           _windows;
    Window*                        _parent;
    Widget*                        _mainWidget;
    Widget*                        _pointerWidget;
    Widget*                        _focusWidget;
    std::vector<Widget*>           _focusList;
    std::map<Key, Widget*>         _shortcuts; 
    std::map<Pt::Char, Widget*>    _mnemonics; 

    bool                           _init;
    bool                           _visible; 
    bool                           _isActive;
    bool                           _enabled; 
    bool                           _enabledState;
    bool                           _isClosed; 
    Gfx::PointF                    _position;
    Gfx::SizeF                     _size;

    Gfx::SizeF                     _minimumSize;
    Gfx::SizeF                     _maximumSize;
    Hmi::WindowPosition::Type      _startPostion;
    Hmi::WindowState::Type         _state;    
    bool                           _border;
    std::string                    _title;
    Gfx::Image                     _icon;
    bool                           _canClose;    
    WindowDecoration::Flags        _decoration;   
    Gfx::Font                      _font;   
};

} // namespace

} // namespace

#endif // PT_HMI_WINDOW_H
