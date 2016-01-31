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

#ifndef Pt_Hmi_MainWindowImpl_H
#define Pt_Hmi_MainWindowImpl_H

#include <Pt/Hmi/Api.h>
#include "../WindowImpl.h"
#include <Windows.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <Pt/Hmi/KeyEvent.h>

namespace Pt{
namespace Hmi{

class Application;
class Screen;

class MainWindowImpl : public WindowImpl
{
    public:
        MainWindowImpl(Window* window);

        virtual ~MainWindowImpl();
	
        virtual void setVisible( bool v);
	
        virtual void activate();
       
        virtual void update();

        virtual void setPosition(const Gfx::PointF& p);

        virtual void setSize(const Gfx::SizeF& size);
	
        virtual void setState(WindowState::Type p);
    
        virtual void setBorder(WindowBorder::Type p);
       
        virtual void setIcon(const Gfx::Image& p);
    
	    virtual void setEnabled(bool e);	
    
	    virtual void setMinimumSize(const Gfx::SizeF& s);
	
	    virtual void setMaximumSize(const Gfx::SizeF& s);	

        virtual void setDecoration( WindowDecoration::Flags d );	

        virtual void setTitle( const std::string& t );
        
        virtual void close();

         HWND hwnd()
         {
            return _hwnd;
         }

	    bool processEvent( unsigned int msg, WPARAM wparam, LPARAM lparam );

    protected:
	    void onPaint();
	    void onSize(WPARAM wparam, LPARAM lparam);
	    void onMouse(unsigned int msg,  WPARAM wparam, LPARAM lparam);
	    void onKey(UINT vkey, UINT scanCode, bool isPress);
	    void onMove(LPARAM lparam);	
	    void onClose();
	    void onActivate(bool f);

    private:
        void setShowTitle(bool p);
        void setShowMinimizeButton(bool p);
        void setShowMaximizeButton(bool p);
        void setShowSystemMenu(bool p);
        void create();	
        void destroy();

    private:	
        Pt::Hmi::Application& _app; 
        Screen&               _screen; 
        HWND				          _hwnd;
        MouseEvent            _mouseEvent;
        ScrollEvent           _scrollEvent;
        KeyEvent              _keyEvent;
        bool                  _hasPointer;     
};

}}

#endif
