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
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImageReader.h>
#include <Pt/Gfx/ARgbImage.h>
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
		
		memoryStream.write((char*)Pt::Forms::Atesion::icon, Pt::Forms::Atesion::iconSize);	
		
		Pt::Gfx::ARgbImage* im = Pt::Gfx::ImageReader::read(memoryStream);
		Icon = *im;
		delete im;
		//Generate Alpha channel
		for(size_t y = 0;  y < Icon.get().height(); ++y)
		{
			for(size_t x = 0;  x < Icon.get().width(); ++x)
			{
				Pt::Gfx::ARgbColor& pix =  Icon.get().pixel(x,y);
				
				if( pix.blue() == 255 && pix.red() == 255 && pix.green() == 255)
					pix.setAlpha(0);
			}
		}			
	}
	
	Position.set(Pt::Gfx::PointF(200,200));
	Size.set(Pt::Gfx::SizeF(800,615));
	Caption.set("This is a Platinum C++ Human Machine Interface demo  ");
  WindowState.set(Hmi::WindowStateType::Normal);
	WindowStartPostion.set(Hmi::WindowStartPositionType::CenterScreen);
	Closed.Changed += Pt::slot(*this, &MainWindow::onClosedByWindow);

	
	//Panel
	_mainPanel.Size.set(Pt::Gfx::SizeF(700,480));
	_mainPanel.Position.set(Pt::Gfx::PointF(40,40));
	_mainPanel.BorderWidth.set(3);	
	_mainPanel.BorderStyle.set(Pt::Hmi::BorderStyleType::Sizeable);
	{
		std::stringstream memoryStream;
		memoryStream.write((char*)Pt::Forms::DemoImage::image, Pt::Forms::DemoImage::imageSize);	
		Pt::Gfx::ARgbImage* im = Pt::Gfx::ImageReader::read(memoryStream);
		_mainPanel.BackgroundImage = *im;
		delete im;
	}

	_mainPanel.BackgroundImageLayout = Pt::Hmi::ImageLayoutType::Strech;

	//Text
	_textLabel.AutoSize.set(true);
	_textLabel.Size.set(Pt::Gfx::SizeF(500,30));
	_textLabel.Caption.set("&This is a Platinum C++");
	_textLabel.Position.set(Pt::Gfx::PointF(20,20));
	_textLabel.ForeColor.set(Pt::Gfx::ARgbColor(255,0,0,0));
	_textLabel.UseMnemonic.set(true);	
	//_textLabel.labelModel().PainterSurfaceType = Pt::Hmi::PainterType::Image;
	_mainPanel.addChild(&_textLabel);
	
	//Toggle button
	_toggleButton.ButtonType.set(Hmi::ButtonType::Toggle);
	_toggleButton.Caption.set("Toggle Me [CTRL+I]");
	_toggleButton.ActionKey.set("C//i");
	_toggleButton.Position.set(Pt::Gfx::PointF(20,60));
	_toggleButton.Size.set(Pt::Gfx::SizeF(150,25));
	_textLabel.bindMnemonicToWidget(&_toggleButton);
	_mainPanel.addChild(&_toggleButton);

	//Dialog button
	_dialogButton.ButtonType.set(Hmi::ButtonType::Press);
	_dialogButton.Caption.set("&Dialog [CTRL+D]");
	_dialogButton.ActionKey.set("C//d");
	_dialogButton.Position.set(Pt::Gfx::PointF(20,100));
	_dialogButton.Size.set(Pt::Gfx::SizeF(150,25));
	_dialogButton.UseMnemonic.set(false);
	_dialogButton.Clicked  += Pt::slot(*this, &MainWindow::onShowDialog);
	
	_mainPanel.addChild(&_dialogButton);
		
	//Close button
	_closeButton.ButtonType.set(Hmi::ButtonType::Press);
	_closeButton.Caption.set("Close [CTRL+X]");
	_closeButton.ActionKey.set("C//x");
	_closeButton.Position.set(Pt::Gfx::PointF(590,525));
	_closeButton.Size.set(Pt::Gfx::SizeF(150,25));
	_closeButton.Clicked += Pt::slot(*this, &MainWindow::onClosed);
	
	addChild(&_closeButton);
	addChild(&_mainPanel);
}

void MainWindow::onShowDialog()
{
	Dialog1 dialog;
	dialog.doModal(this);
}

void MainWindow::show()
{
	Visible = true;
	invalidate();
}

void MainWindow::onClosedByWindow(const Property<bool>& closed )
{
	close();
	Pt::Hmi::Application::instance().exit();
}

void MainWindow::onClosed()
{
	close();	
	Pt::Hmi::Application::instance().exit();
}

}}}
