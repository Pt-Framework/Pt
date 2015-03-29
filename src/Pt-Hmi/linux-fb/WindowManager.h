/*
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_Hmi_WindowManager_h
#define Pt_Hmi_WindowManager_h

#include "WindowImpl.h"
#include <Pt/System/MainLoop.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/PositionEvent.h>
#include <Pt/Hmi/KeyEvent.h>

namespace Pt {
namespace Hmi {

class WindowManager : public Pt::Connectable
{
 public:
    WindowManager();

    virtual ~WindowManager();

		void add( WindowImpl* w );

		void remove( WindowImpl* window );

		void activate(WindowImpl* w);

		void invalidate();		

		Pt::Signal<const Pt::Event&>&  systemEvent()
		{
			return _systemEvent;
		}

	protected:		
		void onInputEvent(const struct input_event& ev);
		 
	private:
		void copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight);
		WindowImpl* active();
		void updateActive(size_t x, size_t y);
		bool contains(WindowImpl* w, const Pt::Gfx::Point& p);

	private:
		std::vector<WindowImpl*> _windows;
		Pt::Hmi::PointingEvent 	_mouseEvent;
		Pt::Hmi::KeyEvent      	_keyEvent;
		Pt::Hmi::KeyEvent      	_closeEvent;
		Pt::Hmi::KeyEvent      	_sizeEvent;

		int											_fd;
		fb_var_screeninfo				_screenInfo;
		fb_fix_screeninfo				_fixedInfo;
		void*										_buffer;
		Pt::size_t							_bufferSize;
		Pt::Signal<const Pt::Event&> _systemEvent;
};

}} // namespace

#endif

