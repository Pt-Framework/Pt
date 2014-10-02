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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA */
#include "MainWindow.h"
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/ImageReader.h>
#include <Pt/Gfx/ARgbColor.h>
#include "AtesionIcon.h"
#include <sstream>

namespace Pt{
namespace Hmi{
namespace Demo{

MainWindow::MainWindow()
:_color(255,255,0,0)
,_painting(false)
{
	init();
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{

	
	//Window
	{
		std::stringstream memoryStream;
		
		memoryStream.write((char*)Pt::Hmi::Desktop::Atesion::icon, Pt::Hmi::Desktop::Atesion::iconSize);	
		
		Pt::Gfx::ARgbImage* im = Pt::Gfx::ImageReader::read(memoryStream);
		windowModel().Icon = *im;
		delete im;
		//Generate Alpha channel
		for(size_t y = 0;  y < windowModel().Icon.get().height(); ++y)
		{
			for(size_t x = 0;  x <  windowModel().Icon.get().width(); ++x)
			{
				Pt::Gfx::ARgbColor& pix =  windowModel().Icon.get().pixel(x,y);
				
				if( pix.blue() == 255 && pix.red() == 255 && pix.green() == 255)
					pix.setAlpha(0);
			}
		}			
	}
	
	setPosition(Pt::Gfx::PointF(200,200));
	setSize(Pt::Gfx::SizeF(800,615));
	windowModel().Caption.set("This is a Platinum C++ Human Machine Interface Scribble  ");
	windowModel().BackColor.set(Pt::Gfx::ARgbColor(0,255,255,255));
    windowModel().WindowState.set(WindowStateType::Normal);
	windowModel().WindowStartPostion.set(WindowStartPositionType::CenterScreen);
	windowController().ClosedAction += Pt::slot(*this, &MainWindow::onClosedByWindow);
	windowModel().Pointer2DStatus.Changed += Pt::slot(*this, &MainWindow::onPointerChanged);

	windowController().Render += Pt::slot(*this, &MainWindow::onRender);

	//Close button
	_closeButton.setToggleButton(false);
	_closeButton.setCaption("Close [CTRL+X]");
	_closeButton.setActionKey("C//x");
	_closeButton.setPosition(Pt::Gfx::PointF(590,525));
	_closeButton.setSize(Pt::Gfx::SizeF(150,25));
	_closeButton.ClickedAction += Pt::slot(*this, &MainWindow::onClosed);

	//Red button
	_redButton.setToggleButton(false);
	_redButton.buttonModel().BackColor = Pt::Gfx::ARgbColor(255,255,0,0);
	_redButton.setCaption("RED [CTRL+R]");
	_redButton.setActionKey("C//R");
	_redButton.setPosition(Pt::Gfx::PointF(10,50));
	_redButton.setSize(Pt::Gfx::SizeF(150,25));
	_redButton.ClickedAction += Pt::slot(*this, &MainWindow::onRed);
	
	
	//Green button
	_greenButton.setToggleButton(false);
	_greenButton.buttonModel().BackColor = Pt::Gfx::ARgbColor(255,0,255,0);
	_greenButton.setCaption("GREEN [CTRL+G]");
	_greenButton.setActionKey("C//G");
	_greenButton.setPosition(Pt::Gfx::PointF(10,100));
	_greenButton.setSize(Pt::Gfx::SizeF(150,25));
	_greenButton.ClickedAction += Pt::slot(*this, &MainWindow::onGreen);


	//Blue button
	_blueButton.setToggleButton(false);
	_blueButton.buttonModel().BackColor = Pt::Gfx::ARgbColor(255,0,0,255);
	_blueButton.setCaption("BLUE [CTRL+B]");
	_blueButton.setActionKey("C//G");
	_blueButton.setPosition(Pt::Gfx::PointF(10,150));
	_blueButton.setSize(Pt::Gfx::SizeF(150,25));
	_blueButton.ClickedAction += Pt::slot(*this, &MainWindow::onBlue);

	addChild(&_closeButton);
	addChild(&_redButton);
	addChild(&_greenButton);
	addChild(&_blueButton);
}

void MainWindow::onClosedByWindow(Controller* ctrl)
{
	onClosed();
}

void MainWindow::onClosed()
{
	Pt::Hmi::Application::instance().exit();
}

void MainWindow::onRed()
{
	_color = Pt::Gfx::ARgbColor(255,255,0,0);
}

void MainWindow::onGreen()
{
	_color = Pt::Gfx::ARgbColor(255,0,255,0);
}

void MainWindow::onBlue()
{
	_color = Pt::Gfx::ARgbColor(255,0,0,255);
}

void MainWindow::onPointerChanged(const Property<PointingEvent>& prop)
{
	const PointingEvent& pointerEvent = prop.get();

	if(pointerEvent.buttons()[0].state() == DeviceButton::Pressed && !_painting)
	{
		_painting = true;
		_points.clear();
		_points.push_back(Pt::Gfx::PointF(pointerEvent.x(), pointerEvent.y()));
		return;
	}

	if(_painting)
	{
		_points.push_back(Pt::Gfx::PointF(pointerEvent.x(), pointerEvent.y())); 
		 windowController().invalidate();
	}

	if(pointerEvent.buttons()[0].state() == DeviceButton::Released && _painting)
	{
		_painting = false;
		return;
	}
}


void MainWindow::onRender(GfxController& controller, PaintSurface& surface)
{
	if(_points.size() == 0)
		return;

	Pt::Hmi::Painter& painter = surface.painter();

	painter.setPen(Pt::Gfx::Pen(5, _color));
	painter.drawPolyline(&_points[0], _points.size());
}
}}}
