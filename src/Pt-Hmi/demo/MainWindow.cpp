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
#include <Pt/Hmi/Cursor.h>
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
	
	{//Icon
		std::stringstream memoryStream;
		
		memoryStream.write((char*)Pt::Forms::Atesion::icon, Pt::Forms::Atesion::iconSize);	
		
		Pt::Gfx::ARgbImage* im = Pt::Gfx::ImageReader::read(memoryStream);

		//Generate Alpha channel
		for( size_t y = 0;  y < im->height(); ++y )
		{
			for( size_t x = 0;  x < im->width(); ++x )
			{
				Pt::Gfx::ARgbColor& pix =  im->pixel(x,y);
				
				if( pix.blue() == 255 && pix.red() == 255 && pix.green() == 255)
					pix.setAlpha(0);
			}
		}			

    Icon = *im;
	  delete im;
	}
  
	
	Position = Pt::Gfx::PointF(200,200);
	Size = Pt::Gfx::SizeF(800,600);
	ShowTitle = true;	
	ShowInTaskbar = true;
	ShowSysMenu = true;
	Border = WindowBorder::Sizeable;
	Caption = std::string("This is a Platinum C++ Human Machine Interface demo  ");
  State = Hmi::WindowState::Normal;
	StartPostion = Hmi::WindowStartPosition::CenterParent;
	Closed.Changed += Pt::slot(*this, &MainWindow::onClosedByWindow);
	BackColor = Gfx::ARgbColor(100,100,100);
	
  //Panel
	_mainPanel.Size = Pt::Gfx::SizeF(700,480);
	_mainPanel.Position = Pt::Gfx::PointF(40,40);
	_mainPanel.PanelBorderWidth = 3;
	_mainPanel.PanelBorderStyle = Pt::Hmi::BorderStyle::Single;

	{
		std::stringstream memoryStream;
		memoryStream.write((char*)Pt::Forms::DemoImage::image, Pt::Forms::DemoImage::imageSize);	
		std::auto_ptr<Pt::Gfx::ARgbImage> im(Pt::Gfx::ImageReader::read(memoryStream));
		_mainPanel.BackgroundImage = *im;		
	}

	_mainPanel.BackgroundImageLayout = Pt::Hmi::ImageLayout::Strech;

	//Text
	_textLabel.AutoSize = true;
  _textLabel.Margin = Hmi::Margin(10);
	_textLabel.Size = Pt::Gfx::SizeF(500,30);
	_textLabel.Caption = std::string("T&his is a Platinum C++");
	_textLabel.Position = Pt::Gfx::PointF(20,20);
	_textLabel.ForeColor = Pt::Gfx::ARgbColor(255,0,0,0);
	_textLabel.UseMnemonic = true;	
  _textLabel.bindMnemonicToWidget(_toggleButton);
  _textLabel.Cursor = Hmi::Cursor::waitCursor();
	_mainPanel.addChild(&_textLabel);
	
	//Toggle button
	_toggleButton.ButtonType = Hmi::ButtonType::Toggle;
	_toggleButton.Caption = std::string("Toggle Me [CTRL+I]");
	_toggleButton.ShortcutKey = std::string("C//i");
	_toggleButton.Position = Pt::Gfx::PointF(20,60);
	_toggleButton.Size = Pt::Gfx::SizeF(150,25);		
	_mainPanel.addChild(&_toggleButton);

	//Dialog button  
	_dialogButton.ButtonType = Hmi::ButtonType::Press;
	_dialogButton.Caption = std::string("&&Dia&log [CTRL+D]&");
	_dialogButton.ShortcutKey = std::string("C//d");
	_dialogButton.Position = Pt::Gfx::PointF(20,100);
	_dialogButton.Size = Pt::Gfx::SizeF(150,25);	
	_dialogButton.UseMnemonic = true;
	_dialogButton.Clicked  += Pt::slot(*this, &MainWindow::onShowDialog);
	
	_mainPanel.addChild(&_dialogButton);
	
	//Close button
	_closeButton.ButtonType = Hmi::ButtonType::Press;
	_closeButton.Caption = std::string("Close App [CTRL+X]");
	_closeButton.ShortcutKey = std::string("C//x");
	_closeButton.Position = Pt::Gfx::PointF(590,525);
	_closeButton.Size = Pt::Gfx::SizeF(150,25);
	_closeButton.Clicked += Pt::slot(*this, &MainWindow::onClosed);
	_closeButton.Size = Pt::Gfx::SizeF(20, 40);
	_closeButton.Dock = Docking::Bottom;
	_closeButton.Margin = Hmi::Margin(5);
	_childWindow2.addChild(&_closeButton);
	
	_childWindow2.addChild(&_mainPanel);


  _childWindow1.Cursor = Hmi::Cursor::waitCursor();
	_childWindow1.Position = Pt::Gfx::PointF(20,20);
	_childWindow1.Size = Pt::Gfx::SizeF(200,300);
	_childWindow2.Position = Pt::Gfx::PointF(80,80);
	_childWindow2.Size = Pt::Gfx::SizeF(800,600);

	addChildWindow(_childWindow1);
	//addChildWindow(_childWindow2);
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
	Pt::Hmi::Application::instance().exit();
}


void MainWindow::onClosed()
{
	Closed = true;	
	Pt::Hmi::Application::instance().exit();
}

}}}
