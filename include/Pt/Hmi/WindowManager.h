/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Hmi_WindowManager_h
#define Pt_Hmi_WindowManager_h

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Connectable.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/ResizeDirection.h>
#include <Pt/Hmi/DeviceButton.h>

namespace Pt {
namespace Hmi {

class ChildWindow;
class Window;
class PointerEvent;
class KeyEvent;
class Application;
class Screen;

class WindowManager : public Pt::Connectable
{
  public:
    WindowManager(Window& parent);

    virtual ~WindowManager();

		void add( ChildWindow* w );

		void remove( ChildWindow* window );
		
		void clear();	
		
		void activate( ChildWindow* w );
		
		void deactivate();
		
		void render();
		
		bool pointerInput( const Pt::Hmi::PointerEvent& pointerEvent );

		bool keyInput( const Pt::Hmi::KeyEvent& keyEvent );		 			

		const std::vector<ChildWindow*>&	windows() const
		{
			return _windows;
		}		

    size_t actionButton() const 
    {
        return _actionButton;
    }

    void setActionButton( size_t index )
    {
        _actionButton = index;
    }

		void invalidate();	

  protected:
    virtual Gfx::PointF renderFrame( ChildWindow* w);				     

  private:		
    bool contains( const ChildWindow* w, const Gfx::PointF& p );

		void setSizingCursor( ResizeDirection::Type type );
			
		ResizeDirection::Type getSizingDirection( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev );

		ResizeDirection::Type isSizing( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev );	
    bool isMoving ( const ChildWindow* w, const Pt::Hmi::PointerEvent& ev );

    void doSizing( ChildWindow* w, const PointerEvent& ev );
		void doMoving( ChildWindow* w, const PointerEvent& ev );
		
		void updateActive( const Pt::Hmi::PointerEvent& mouseEvent );
	
    Window* getFosusedWindow(WindowManager* manager);

		Gfx::PointF toClient(const ChildWindow* w, const Gfx::PointF& p);  

		ChildWindow* active();		
		

		ChildWindow* findWindow( const Gfx::PointF& pos );

		void setPointedWindow( ChildWindow* window );

    
	private:
		Application&              _app;  
		Window&										_parent;
		std::vector<ChildWindow*>	_windows;		
		ResizeDirection::Type			_sizingDirection;
		Gfx::PointF								_lastSizePoint;    
		double										_borderWidth;
   Gfx::Color						      _inactiveColor; 
   Gfx::Color						      _activeColor;
	 Gfx::Color                 _textColor;
		bool											_moving;	
		Gfx::PointF								_movingOffset;
    DeviceButton::State       _pointerLastState;  
    bool                      _focusOnPointerOver;    
    size_t                    _actionButton;  
		double                    _titleBarHeight;
		ChildWindow*							_pointedWindow;
};

}} // namespace

#endif

