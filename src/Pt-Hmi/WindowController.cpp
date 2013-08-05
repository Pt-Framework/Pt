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
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/PointingDevice.h>
#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/GfxOutput.h>

#include <iostream>

namespace Pt{
namespace Hmi{

WindowController::WindowController(GfxModel* m, Renderer* r,  GfxOutput* out, PointingDevice* in1, InputDevice* in2)
: _painter(0)
{	
	if( m != 0)
		Controller::setModel(m);

	if( r != 0)
		Controller::setRenderer(r);

	if( out != 0)
		Controller::addOutputDevice(out);

	if( in1 != 0)
		Controller::addInputDevice(in1);

	if( in2 != 0)
		Controller::addInputDevice(in2);
}

WindowController::~WindowController()
{
	delete _painter;
}

WidgetController* WindowController::mainWidget()
{
	if( Controller::children().size() != 0)
		return dynamic_cast<WidgetController*>( Controller::children()[0]);

	return 0;
}

const WidgetController* WindowController::mainWidget() const
{
	if( Controller::children().size() != 0)
		return dynamic_cast<WidgetController*>( Controller::children()[0]);

	return 0;
}

void WindowController::render()
{
	GfxController::render(_painter);
}

void WindowController::invalidate()
{
	render();
	output();
}

void WindowController::onInput2D(const PointingEvent& ev)
{
	GfxModel* m = gfxModel();
	
	m->Pointer2DStatus = ev;

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyInput2D(ev);
	
	invalidate();
}

void WindowController::onSizeChanged(Pt::Gfx::SizeF& sizeUnits)
{
	GfxModel* m = gfxModel();

	Pt::Gfx::Size size = m->fromUnit(sizeUnits);

	m->PaintBuffer.resize(size.width(), size.height());	
	render();	
}

GfxModel* WindowController::gfxModel()
{
	GfxModel* m = dynamic_cast<GfxModel*>(model());

	if( m == 0)
		throw std::logic_error("ERROR: WindowController expect a GFXModel!");

	return m;
}

void WindowController::onModelChanged(bool created)
{
	if( created)
	{
		GfxModel* m = gfxModel();

		if( _painter != 0)
			delete _painter;

		_painter = new Pt::Gfx::ImagePainter(m->PaintBuffer);
		_painter->setFont(m->Font.get());

		m->Size.PropertyChanged += Pt::slot(*this, &WindowController::onSizeChanged);					
		m->Size.PropertyChanged.send(m->Size.get());		
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyModelChanged(created);										
}

}}
