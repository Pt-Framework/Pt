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

#ifndef Pt_Hmi_Window_H
#define Pt_Hmi_Window_H

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowStartPosition.h>
#include <Pt/Hmi/WindowState.h>
#include <Pt/Hmi/WindowBorder.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Gfx/Image.h>

namespace Pt {
namespace Hmi {

class ChildWindow;

class PT_HMI_API Window : public Widget
{
	 friend class Widget;

  public:    		
    virtual ~Window();        						

    Window* windowParent() const;
    
    void addWindow(ChildWindow& w);

    void removeWindow(ChildWindow& w);

    const std::vector<ChildWindow*> windows() const;

    WindowManager& windowManager()
    {
      return _windowManager;
    }

    const WindowManager& windowManager() const
    {
      return _windowManager;
    }

    bool isClosed() const
    {
      return _isClosed;
    }

    void close();    

    void activate();

    bool isActive() const
    {
      return _isActive;
    }

    void setPointedWidget( Widget* widget );

    const Gfx::SizeF&	minimumSize() const
    {
      return _minimumSize;
    }

    void setMinimumSize(const Gfx::SizeF& s)
    {
      onSetMinimumSize( s );
    }

    const Gfx::SizeF&	maximumSize() const
    {
      return _maximumSize;
    }

    void setMaximumSize(const Gfx::SizeF& s)
    {
      onSetMaximumSize(s);
    }

    const Hmi::WindowStartPosition::Type& startPostion() const
    {
      return _startPostion;
    }

    void setStartPostion(const Hmi::WindowStartPosition::Type& p)
    {
      _startPostion = p;
    }

    const Hmi::WindowState::Type& state() const
    {
      return _state;
    }

    void setState(const Hmi::WindowState::Type& s)
    {
      onState(s);
    }

    bool showInTaskbar() const
    {
      return _showInTaskbar;
    }

    void setShowInTaskbar(bool s)
    {
      onShowInTaskbar(s);
    }

    bool isShowTitle() const
    {
      return _showTitle;
    }

    void setShowTitle(bool s)
    {
      _showTitle = s;
      onShowTitle( s );
    }

    bool showMinimizeButton() const
    {
      return _showMinimizeButton;
    }

    void setShowMinimizeButton(bool s)
    {
      onShowMinimizeButton( s );
    }

    bool showMaximizeButton() const
    {
      return _showMaximizeButton;
    }

    void setShowMaximizeButton(bool s)
    {
      onShowMaximizeButton(s);
    }

    bool showSysMenu() const
    {
      return _showSysMenu;
    }

    void setShowSysMenu(bool s)
    {
        onShowSystemMenu(s);
    }

    const Hmi::WindowBorder::Type& border() const
    {
      return _border;
    }

    void setBorder(const Hmi::WindowBorder::Type& t)
    {
      onBorder(t);
    }

    const Gfx::Image&	icon() const
    {
      return _icon;
    }

    void setIcon(const Gfx::Image& i)
    {
      onIcon( i );
    }

    bool canClose() const
    {
      return _canClose;
    }

    void setCanClose(bool c)
    {
      _canClose = c;
    }

    bool firstShow() const
    {
      return _firstShow;
    }

    void setFirstShow(bool b)
    {
      _firstShow = b;
    }

    const std::string& focuseMoveKey()const
    {
      return _focuseMoveKey;
    }

    void setFocuseMoveKey(const std::string& s)
    {
      _focuseMoveKey = s;
    }

    const std::string& caption() const
    {
        return _caption;
    }

    void setCaption( const std::string& c )
    {
      onSetCaption( c );
    }


  protected:
    Window(Window* parent = 0);	

    virtual void onKeyEvent( const KeyEvent& ev );

    virtual void onPointerEvent( const PointerEvent& ev );

    virtual void onResizeEvent(const ResizeEvent& ev);

    virtual void onMoveEvent( const MoveEvent& ev);

    virtual void onCloseEvent(const CloseEvent& ev);

    virtual void onActivateEvent(const ActivateEvent& ev);

    virtual void onRender( PaintSurface& surface );
          
    virtual void onActivate() = 0;
    
    virtual void onShowTitle( bool s );

    virtual void onShowMinimizeButton( bool s );

    virtual void onShowMaximizeButton( bool s );
    
    virtual void onShowSystemMenu( bool  s );

    virtual void onState(const Hmi::WindowState::Type& s);

    virtual void onBorder(const Hmi::WindowBorder::Type& t);
    
    virtual void onShowInTaskbar(bool s);

    virtual void onIcon(const Gfx::Image& i);

    virtual void onSetMinimumSize( const Gfx::SizeF& s );

    virtual void onSetMaximumSize(const Gfx::SizeF& s);

    virtual void onClose();

		virtual void onWidgetRemoved( Widget& w  );

		void setFocusedWidget(Widget* w );

		virtual Window* getWindow()
		{
			return this;
		}

		Widget* focusedWidget() 
		{
			return _focusedWidget;
		}

		

  private:
    Window*													 _winParent;
    Widget*													 _apointedWidget;
		Widget*													 _focusedWidget;
    WindowManager										 _windowManager;
    bool														 _isClosed;
    bool														 _isActive;
    Gfx::SizeF											 _minimumSize;
    Gfx::SizeF											 _maximumSize;
    Hmi::WindowStartPosition::Type   _startPostion;
    Hmi::WindowState::Type           _state;    
    bool                             _showInTaskbar;
    bool                             _showTitle;
    bool                             _showMinimizeButton;
    bool                             _showMaximizeButton;
    bool                             _showSysMenu;
    Hmi::WindowBorder::Type          _border;
    Gfx::Image						           _icon;
    bool                             _canClose;
    bool                             _firstShow;
    std::string                      _focuseMoveKey;    
};

}}

#endif
