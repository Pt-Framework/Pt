/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/

#ifndef Pt_Hmi_WindowImpl_H
#define Pt_Hmi_WindowImpl_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowState.h>
#include <Pt/Hmi/WindowBorder.h>
#include <Pt/Hmi/WindowDecoration.h>
#include <Pt/Hmi/WindowStartPosition.h>
#include <Pt/Hmi/CloseEvent.h>
#include "WindowManager.h"
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Connectable.h>

namespace Pt{

namespace Hmi{

class Window;
class ChildWindowImpl;

class WindowImpl : public Pt::Connectable
{
    public:
        WindowImpl(Window* api);        
        
        WindowImpl(Window* api, const WindowImpl* impl);  

        virtual ~WindowImpl();
	
        virtual void show() = 0;
	
        virtual void hide() = 0;

        virtual void activate() = 0;        

        virtual void invalidate() = 0;  

        bool isActive() const
        {
            return _isActive;
        }

        bool isClosed() const
        {
            return _isClosed;
        }

        void setPosition(const Gfx::PointF& p)
        {
            onSetPosition(p);
            _position = p;
        }
    
        const Gfx::PointF& position() const
        {
            return _position;
        }


        void setSize(const Gfx::SizeF& size)
        {
            onSetSize(size );
            _size = size;
            _surface.resize(size);
        }


        const Gfx::SizeF& size() const
        {
            return _size;
        }	

        void setState(WindowState::Type s)
        {
            onSetState(s);
            _state = s; 
        }

        WindowState::Type state() const
        {
            return _state;
        }
    
        void setBorder(WindowBorder::Type b)
        {
            onSetBorder(b);
            _border = b; 
        }
       
        WindowBorder::Type border() const
        {
            return _border;
        }

        void setIcon(const Gfx::Image& i)
        {
            onSetIcon(i);
            _icon = i; 
        }
    
        const Gfx::Image& icon() const
        {
            return _icon;
        }

	    void setEnabled(bool e)
        {
            onSetEnabled(e);
            _enabled = e;
        }

        bool isEnabled() const
        {
            return _enabled;
        }
    
	    void setMinimumSize(const Gfx::SizeF& s)
        {
            onSetMinimumSize(s);
            _minimumSize = s;
        }

        const Gfx::SizeF& minimumSize() const
        {
            return _minimumSize;
        }
	
	    void setMaximumSize(const Gfx::SizeF& s)
        {
            onSetMaximumSize(s);
            _maximumSize = s;
        }

        const Gfx::SizeF& maximumSize() const
        {
            return _maximumSize;
        }

        void setDecoration( WindowDecoration::Flags d )
        {
            onSetDecoration(d);
            _decoration = d;
        }
    
        WindowDecoration::Flags decoration() const
        {
            return _decoration;
        }


        void setTitle( const std::string& t )
        {
            _title = t;
        }

        const std::string& title() const
        {
            return _title;
        }


        bool isClosable() const
        {
            return _canClose;
        }

        void setClosable(bool c)
        {
            _canClose = c;
        }

        virtual void close() = 0;
 
        void add(Window& ch );

        void remove(Window& ch );

        const std::vector<Window*>& windows() const 
        {
           return _windowManager.windows();

        }

        Window* parent()
        {
            return _parent;
        }


        const Window* parent() const
        {
            return _parent;
        }

        const Widget* mainWidget() const
        {
            return _mainWidget;
        }

        Widget* mainWidget()
        {
            return _mainWidget;
        }

        void setDefaultPosition( Hmi::WindowPosition::Type p )
        {
            _startPostion = p;
        }

        Hmi::WindowPosition::Type defaultPosition() const
        {
            return _startPostion;
        }

        bool isVisible() const
        {
            return _visible;
        }

        const std::string& name() const
        {
            return _name; 
        }

        void setName(const std::string&  n)
        {
            _name = n;
        }

        void setFont(const Gfx::Font& ft)
        {
            _font = ft;
        }

        const Gfx::Font& font() const
        {
            return _font; 
        }


        void setMainWidget( Widget* w);

        Window* findWindow(const std::string& n );

        Widget* findWidget(const std::string& n );

        void setMainWidget(Widget& w);

        void onPointerEvent(const PointerEvent& ev);

        void onKeyEvent(const KeyEvent& ev);

        void onResizeEvent(const ResizeEvent& ev);
        
        void onMoveEvent( const MoveEvent& ev);

        void onActivateEvent(const ActivateEvent& ev);
        
        void onCloseEvent(const CloseEvent& ev);

        void removeWidget(Widget& w);

        PaintSurface&  surface()
        {
            return _surface;
        }

        void render();

        WindowManager& windowManager()
        {
            return _windowManager;
        }

        void setPointedWidget( Widget* widget );
        
        void setFocusedWidget( Widget* w );
        
        Widget* focusedWidget() 
        {
            return _focusedWidget;
        }

    protected:
        virtual void onSetPosition(const Gfx::PointF& p)  = 0;

        virtual void onSetSize(const Gfx::SizeF& size) = 0;
	
        virtual void onSetState(WindowState::Type p) = 0;
    
        virtual void onSetBorder(WindowBorder::Type p) = 0;
       
        virtual void onSetIcon(const Gfx::Image& p) = 0;
    
	    virtual void onSetEnabled(bool e) = 0;	
    
	    virtual void onSetMinimumSize(const Gfx::SizeF& s) = 0;
	
	    virtual void onSetMaximumSize(const Gfx::SizeF& s) = 0;	

        virtual void onSetDecoration( WindowDecoration::Flags d ) = 0;
        
        virtual void onSetTitle( const std::string& t ) = 0;
        

   protected:
        Window*                              _apiWindow;
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

    private:
	    Widget*                        _mainWidget;
        Window*                        _parent;
        Widget*                        _pointerWidget;
        Widget*                        _focusedWidget;
        PaintSurface                   _surface;
};

}}

#endif
