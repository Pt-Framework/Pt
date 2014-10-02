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
#ifndef Pt_Hmi_GfxOutputDeviceImpl_H
#define Pt_Hmi_GfxOutputDeviceImpl_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Windows.h>
#include <map>

namespace Pt{
namespace Hmi{

class GfxOutputDeviceImpl : public Pt::Connectable
{
public:
	GfxOutputDeviceImpl();
	virtual ~GfxOutputDeviceImpl();
	void output(Pt::Hmi::Controller* controller, Pt::Hmi::Model* model);

	inline HWND hwnd()
	{
		return _hwnd;
	}

protected:
	void onWindowEvent(HWND wnd, unsigned int msg, WPARAM wparam, LPARAM lparam, bool& handled);
	virtual void onPaint();
	virtual void onSize(WPARAM wparam, LPARAM lparam);
	virtual void onMouse(unsigned int msg,  WPARAM wparam, LPARAM lparam);
	virtual void onKey(unsigned int ms, WPARAM wparam, LPARAM lparam);
	virtual void onMove();
	virtual bool onClosing();
	virtual void onClosed();

protected:	
	void updateModelSizeAndPos();
	void setWindowSizeAndPos(bool firstShow);
	void setWindowProperties();	
	void setWindowIcon();
	void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);
	void create();
	void destroy();
	void centerWindowTo(HWND parent);

private:
	HWND					_hwnd;
	Pt::Hmi::WindowModel*	_model;
	Pt::Hmi::WindowController* _controller;
	Pt::Gfx::Painter*		_nativePainter;
	KeyEvent				_keyEvent;
	PointingEvent			_pointerEvent;
	bool					_ignoreSizePositionEvent;
};

}}
#endif