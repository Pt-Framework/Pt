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
#ifndef Pt_Forms_ViewImpl_H
#define Pt_Forms_ViewImpl_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Forms/Model.h>
#include <Pt/Forms/Output.h>
#include <Pt/Forms/Api.h>
#include <Pt/Forms/WindowModel.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/PointingEvent.h>
#include <map>

namespace Pt{
namespace Forms{

class ViewImpl : public Pt::Connectable
{
public:
	ViewImpl();
	virtual ~ViewImpl();

	void output(Pt::Forms::Model* model);

	Pt::Gfx::Painter* nativePainter();

protected:
	virtual void onPaint();
	virtual void onSize();
	virtual void onMouse();
	virtual void onKey();
	virtual void onMove();
	virtual bool onClosing();
	virtual void onClosed();

protected:	
	void getWindowSize();
	void getWindowPos();
	void setWindowSizeAndPos(bool firstShow);
	void setWindowProperties();	
	void setWindowIcon();
	void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);
	void create();
	void destroy();
	void output();
	void centerWindowTo(void* parent);

private:
	Pt::Forms::WindowModel*	_model;
	Pt::Gfx::Painter*		_nativePainter;
	bool					_ignoreEvent;
	KeyEvent				_keyEvent;
	PointingEvent			_pointerEvent;
	Pt::Gfx::Rgb888Image	_rgb88Image;
};

}}
#endif