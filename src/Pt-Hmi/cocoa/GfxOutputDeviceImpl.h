/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris Dürner
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

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/System/Timer.h>
#include <map>

#ifdef __OBJC__
    #import <AppKit/NSWindow.h>
    #import <AppKit/NSGraphicsContext.h>
    #import <AppKit/NSView.h>
#else
struct NSRect;
	struct NSView;
    struct NSWindow;
    struct NSResponder;
    struct NSGraphicsContext;
#endif

namespace Pt{	
namespace Hmi{

class GfxOutputDeviceImpl : public Pt::Connectable
{
public:
	GfxOutputDeviceImpl();
	virtual ~GfxOutputDeviceImpl();

	void output(Pt::Hmi::Model* model);
	Pt::Gfx::Painter* nativePainter();
    
    inline WindowModel* model()
    {
        return _model;
    }
    
    NSView* view();
    
    NSWindow* window()
    {
        return _window;
    }
public:
    void onPositionAndSize();
    void onPosition();
    bool onCanClose();
    void onMouseMove(double x,double y);
    void onLMouseDown(double x, double y);
    void onLMouseUp(double x, double y);
    void onKeyDown(int key);
    void onKeyUp(int key);
    void onSpezialKeyEvent(unsigned int mask);
    void onLostFocus();
    
private:
    void writeWindowSizeAndPos(bool firstShow);
	void checkModal();
	void writeWindowProperties();    
	void create();
	void destroy();
    void centerWindowTo(NSRect* parentRect);
    Pt::Gfx::PointF convertMousePosition(double x, double y);

private:
	NSWindow*				_window;
	NSView*					_view;
	Pt::Hmi::WindowModel*	_model;
	Pt::Gfx::Painter*		_nativePainter;
	bool					_ignoreSizeEvent;
    Pt::Hmi::PointingEvent 	_mouseEvent;
	Pt::Hmi::KeyEvent      	_keyEvent;
    Pt::System::Timer       _timer;
    bool					_visible;
    int						_level;
};

}}
#endif