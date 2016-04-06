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
#include <Pt/Hmi/WindowBorder.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowDecoration.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/MouseEvent.h>
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
class WindowImpl;

class PT_HMI_API Window : public Visual
{
   friend class Widget; 
   friend class WindowImpl;
   friend class ChildWindowImpl;
   friend class MainWindowImpl;

  public:           
    explicit Window(Window* parent = 0);     

    virtual ~Window();                                

    Window* parent();

    const Window* parent() const;
    
    void add(Window& w);

    void remove(Window& w);

    const std::vector<Window*>& windows() const;

    std::vector<Window*>& windows();

    Widget* mainWidget();

    const Widget* mainWidget()  const;

    void setMainWidget(Widget* widget);

    Visual* findVisual( Pt::uint64_t id);
    
    Widget* pointerWidget();

    Widget* focusWidget();

    bool isClosed() const;

    void close();

    bool isActive() const;

    void activate();

    // TODO: return new focusWidget() and remove focusWidget()
    void focusNext();
    
    // TODO: return new focusWidget() and remove focusWidget()
    void focusPrev();

    PixmapSurface& surface();

    // TODO: remove this only needed by WindowManager::activeWindow
    WindowManager& windowManager();

    // TODO: remove this only needed by WindowManager::activeWindow
    const WindowManager& windowManager() const;

    WindowImpl* impl();

    void runModal();
    
    void paint(const Gfx::RectF& rect);

    virtual void resize( const Gfx::SizeF& s );

    virtual void move(const Gfx::PointF& p);

    virtual void show( bool b = true );
    
    virtual void enable( bool e = true );

    virtual void update();

    void update(const Gfx::RectF& rect);

    const Gfx::SizeF& size() const
    {
        return _size;
    }

    const Gfx::PointF& position() const
    {
        return _position;
    }

  protected:
    void onUpdate(Window& w, const Gfx::RectF& rect);

    void onShow(Window& w, bool visible);

    void onActivate(Window& w);

    void onEnable(Window& w, bool enable);

    void onMove(Window& w, const Gfx::PointF& to);

    void onResize(Window& w, const Gfx::SizeF& to);

  protected:
    virtual void onEvent(const Pt::Event& ev);

    virtual void onKeyEvent( const KeyEvent& ev );

    virtual void onPointerEvent( const MouseEvent& ev );

    virtual void onTouchEvent( const TouchEvent& ev );
    
    virtual void onScrollEvent( const ScrollEvent& ev );

    virtual void onEnterEvent( const EnterEvent& ev );    

    virtual void onLeaveEvent(const LeaveEvent& ev );
    
    virtual void onResizeEvent(const ResizeEvent& ev);
    
    virtual void onMoveEvent(const MoveEvent& ev);

    virtual void onPaintEvent(const PaintEvent& ev);

    virtual void onCloseEvent(const CloseEvent& ev);

    virtual void onActivateEvent(const ActivateEvent& ev);  

    virtual void onShowEvent( const ShowEvent& ev );

    virtual void onEnableEvent( const EnableEvent& ev );

    // TODO:
  public:
    bool isEnabled() const
    {
        return _enabled;
    }

    bool isVisible() const
    {
        return _visible;
    }

    const Gfx::SizeF& minimumSize() const;

    void setMinimumSize(const Gfx::SizeF& s);

    const Gfx::SizeF& maximumSize() const;

    void setMaximumSize(const Gfx::SizeF& s);

    Hmi::WindowPosition::Type defaultPosition() const;

    void setDefaultPosition(Hmi::WindowPosition::Type p);

    Hmi::WindowState::Type state() const;

    void setState(Hmi::WindowState::Type s);
    
    Hmi::WindowBorder::Type border() const;

    void setBorder( Hmi::WindowBorder::Type t);

    const Gfx::Image& icon() const;

    void setIcon(const Gfx::Image& i);

    bool isClosable() const;

    void setClosable(bool c);

    const std::string& title() const;

    void setTitle( const std::string& t );

    const Gfx::Font& font() const;

    void setFont(const Gfx::Font& ft);

    WindowDecoration::Flags decoration() const;

    void setDecoration( WindowDecoration::Flags d );

    const std::string& name() const;

    void setName(const std::string& n);

  private:
    void createImpl();

    void addWidget(Widget& w);

    void removeWidget(Widget& w);
    
    void setShortcut(Widget& w, const Key* key);

    void setMnemonic(Widget& w, const Char* ch);

    void setPointerWidget( Widget* widget );

    void setFocusWidget(Widget* widget);

    template <typename Iter>
    void moveFocus(Iter begin, Iter end);

    void addFocusWidget(Widget& w);

    void removeFocusWidget(Widget& w);

    void setFocusIndex(Widget& w, size_t index);  
    
  private:
    Pt::Signal<const Pt::Event&>   _eventReady;
    WindowManager                  _windowManager;
    bool                           _isClosed;
    bool                           _isActive;
    bool                           _enabled;
    bool                           _visible;
    Gfx::SizeF                     _minimumSize;
    Gfx::SizeF                     _maximumSize;
    Hmi::WindowPosition::Type      _startPostion;
    Hmi::WindowState::Type         _state;    
    Hmi::WindowBorder::Type        _border;
    std::string                    _title;
    Gfx::Image                     _icon;
    bool                           _canClose;    
    WindowDecoration::Flags        _decoration;   
    Gfx::Font                      _font;    
    std::map<Key, Widget*>         _shortcuts; 
    std::map<Pt::Char, Widget*>    _mnemonics;     
    Widget*                        _mainWidget;
    Window*                        _parent;
    Widget*                        _pointerWidget;
    Widget*                        _focusWidget;
    std::vector<Widget*>           _focusList;
    PixmapSurface                  _surface;
    WindowImpl*                    _impl;        
    Gfx::PointF                    _position;
    Gfx::SizeF                     _size;
    Gfx::SizeF                     _requestedSize;
};

} // namespace

} // namespace

#endif // PT_HMI_WINDOW_H
