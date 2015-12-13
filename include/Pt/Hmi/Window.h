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
#include <Pt/Hmi/Key.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PointerEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Hmi {

class Widget;
class WindowImpl;
class PaintSurface;

class PT_HMI_API Window : public Pt::Connectable
{
   friend class Widget;
   friend class WindowManager;

  public:           
    Window(Window* parent = 0);     

    virtual ~Window();                                

    Window* parent();

    const Window* parent() const;
    
    void add(Window& w);

    void remove(Window& w);

    Window* findWindow(const std::string& name);

    const std::vector<Window*>& windows() const;

    Widget* findWidget(const std::string& name);

    Widget* mainWidget();

    const Widget* mainWidget()  const;

    void setMainWidget(Widget* widget);
  
    void close();    
    
    void activate();

    bool isClosed() const;

    bool isActive() const;

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

    bool isEnabled() const;

    void setEnabled( bool e );

    void setVisible( bool b );

    bool isVisible() const;

    const Gfx::SizeF& size() const;

    void setSize( const Gfx::SizeF& s );

    const Gfx::PointF& position() const;

    void setPosition( const Gfx::PointF&  p);

    void setFont(const Gfx::Font& ft);
    
    const Gfx::Font& font() const;

    void setDecoration( WindowDecoration::Flags d );

    WindowDecoration::Flags decoration() const;

    const std::string& name() const
    {
        return _name; 
    }

    void setName(const std::string&  n)
    {
        _name = n;
    }

    void processEvent(const Pt::Event& ev);

    WindowImpl* impl()
    {
        return _impl;   
    }


    PaintSurface&  surface()
    {
        return _surface;
    }


    void invalidate();

    void render();

    WindowManager& windowManager()
    {
        return _windowManager;
    }

    const WindowManager& windowManager() const 
    {
        return _windowManager;
    }

  protected:
    virtual void onEvent(const Pt::Event& ev);

    virtual void onKeyEvent( const KeyEvent& ev );

    virtual void onPointerEvent( const PointerEvent& ev );

    virtual void onResizeEvent(const ResizeEvent& ev);

    virtual void onMoveEvent( const MoveEvent& ev);

    virtual void onCloseEvent(const CloseEvent& ev);

    virtual void onActivateEvent(const ActivateEvent& ev);
    

    void removeWidget(Widget& w);

    void setPointedWidget( Widget* widget );

    Widget* focusedWidget() 
    {
        return _focusedWidget;
    }

    void setFocusedWidget( Widget* widget );
    

  private:
    WindowManager                        _windowManager; 
    bool                                 _isClosed;
    bool                                 _isActive;
    Gfx::SizeF                           _minimumSize;
    Gfx::SizeF                           _maximumSize;
    Hmi::WindowPosition::Type            _startPostion;
    Hmi::WindowState::Type               _state;    
    bool                                 _enabled;
    bool                                 _visible;
    Gfx::SizeF                           _size;
    Gfx::PointF                          _position; 
    Hmi::WindowBorder::Type              _border;
	std::string                          _title;
    Gfx::Image                           _icon;
    bool                                 _canClose;    
    WindowDecoration::Flags             _decoration;   
    std::string                         _name;    
    Gfx::Font                           _font;    
    
	Widget*                        _mainWidget;
    Window*                        _parent;
    Widget*                        _pointerWidget;
    Widget*                        _focusedWidget;
    PaintSurface                   _surface;
    WindowImpl*                    _impl;           
    Pt::Signal<const Pt::Event&>   _eventReady;
  
};

} // namespace

} // namespace

#endif // PT_HMI_WINDOW_H
