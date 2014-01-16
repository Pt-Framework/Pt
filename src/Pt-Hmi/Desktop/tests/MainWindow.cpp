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
#include "Dialog1.h"
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImageReader.h>
#include "DemoImage.h"
#include "AtesionIcon.h"
#include <sstream>

namespace Pt{
namespace Hmi{
namespace Demo{

MainWindow::MainWindow()
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
	windowModel().Caption.set("This is a Platinum C++ Human Mashine Interface demo  ");
    windowModel().WindowState.set(WindowStateType::Normal);
	windowModel().WindowStartPostion.set(WindowStartPositionType::CenterScreen);
	windowController().ClosedAction += Pt::slot(*this, &MainWindow::onClosedByWindow);

	
	//Panel
	_mainPanel.setSize(Pt::Gfx::SizeF(700,480));
	_mainPanel.setPosition(Pt::Gfx::PointF(40,40));
	_mainPanel.panelModel().BorderWidth.set(3);	
	_mainPanel.panelModel().BorderStyle.set(Pt::Hmi::BorderStyleType::Sizeable);
	{
		std::stringstream memoryStream;
		memoryStream.write((char*)Pt::Hmi::DemoImage::image, Pt::Hmi::DemoImage::imageSize);	
		Pt::Gfx::ARgbImage* im = Pt::Gfx::ImageReader::read(memoryStream);
		_mainPanel.panelModel().BackgroundImage = *im;
		delete im;
	}

	_mainPanel.panelModel().BackgroundImageLayout = Pt::Hmi::ImageLayoutType::Strech;
	addChild(&_mainPanel);

	//Text
	_textLabel.setAutoSize(true);
	_textLabel.setCaption("This is a Platinum C++ Human Mashine Interface demo");
	_textLabel.setPosition(Pt::Gfx::PointF(20,20));
	_textLabel.labelModel().ForeColor.set(Pt::Gfx::ARgbColor(255,0,0,0));
	_mainPanel.addChild(&_textLabel);

	//Toggle button
	_toggleButton.setToggleButton(true);
	_toggleButton.setCaption("Toggle Me [CTRL+I]");
	_toggleButton.setActionKey("C//i");
	_toggleButton.setPosition(Pt::Gfx::PointF(20,60));
	_toggleButton.setSize(Pt::Gfx::SizeF(150,25));
	_mainPanel.addChild(&_toggleButton);

	//Dialog button
	_dialogButton.setToggleButton(false);
	_dialogButton.setCaption("Dialog [CTRL+D]");
	_dialogButton.setActionKey("C//d");
	_dialogButton.setPosition(Pt::Gfx::PointF(20,100));
	_dialogButton.setSize(Pt::Gfx::SizeF(150,25));
	_dialogButton.ClickedAction  += Pt::slot(*this, &MainWindow::onShowDialog);
	_mainPanel.addChild(&_dialogButton);

	//Close button
	_closeButton.setToggleButton(false);
	_closeButton.setCaption("Close [CTRL+X]");
	_closeButton.setActionKey("C//x");
	_closeButton.setPosition(Pt::Gfx::PointF(590,525));
	_closeButton.setSize(Pt::Gfx::SizeF(150,25));
	_closeButton.ClickedAction += Pt::slot(*this, &MainWindow::onClosed);
	addChild(&_closeButton);
}

void MainWindow::onShowDialog()
{
	Dialog1 dialog;
	dialog.show(this);
}

void MainWindow::onClosedByWindow(Controller* ctrl)
{
	onClosed();
}

void MainWindow::onClosed()
{
	Pt::Hmi::Application::instance().exit();
}

}}}
