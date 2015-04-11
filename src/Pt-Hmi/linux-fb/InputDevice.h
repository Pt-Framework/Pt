/* Copyright (C) 2013 Marc Boris Duerner
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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

#ifndef Pt_Hmi_InputDevice_h
#define Pt_Hmi_InputDevice_h

#include "posix/Selector.h"

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PointerEvent.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/MainLoop.h>

#include <unistd.h>

namespace Pt {

namespace Hmi {

class InputDevice : public System::Selectable
{
	public:
		InputDevice(const char* deviceName);
		~InputDevice();

		void begin()
		{      
		  if( ! _loop )
			throw std::logic_error("input device not active");

			Pt::System::Selector& selector = _loop->selector();
			selector.beginRead(&_ioh);
		}

		void close()
		{ 
			if(_ioh.fd != -1)
			{
				::close(_ioh.fd); 
				_ioh.fd = -1;
			}
		}

		void flush()
		{ this->onRun(); }

	protected:
		virtual bool onRun();

		virtual void onCancel()
		{ throw std::logic_error("not implemented"); }
    
		void onAttach(System::EventLoop& loop);

		void onDetach(System::EventLoop& loop);    

	private:
		Pt::System::IOHandle _ioh;
		Pt::System::EventLoop* _loop;
		Pt::Hmi::KeyEvent _keyEvent;
		Pt::Hmi::PointerEvent _mouseEvent;
};

} // namespace

} // namespace

#endif
