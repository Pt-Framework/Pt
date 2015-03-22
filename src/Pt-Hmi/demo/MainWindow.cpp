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

MainWindow::MainWindow(WindowModel* model)
: Pt::Hmi::Window(model)
, _model(model)
, _mainPanel(&_mainPanelModel)
, _closeButton(&_closeButtonModel)
, _toggleButton(&_toggleButtonModel)
, _dialogButton(&_dialogButtonModel)
, _textLabel(&_textLabelModel)
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
		_model->Icon = *im;
		delete im;
		//Generate Alpha channel
		for(size_t y = 0;  y < _model->Icon.get().height(); ++y)
		{
			for(size_t x = 0;  x < _model->Icon.get().width(); ++x)
			{
				Pt::Gfx::ARgbColor& pix =  _model->Icon.get().pixel(x,y);
				
				if( pix.blue() == 255 && pix.red() == 255 && pix.green() == 255)
					pix.setAlpha(0);
			}
		}			
	}
	
	_model->Position.set(Pt::Gfx::PointF(200,200));
	_model->Size.set(Pt::Gfx::SizeF(800,615));
	_model->Caption.set("This is a Platinum C++ Human Machine Interface demo  ");
  _model->WindowState.set(Hmi::WindowStateType::Normal);
	_model->WindowStartPostion.set(Hmi::WindowStartPositionType::CenterScreen);
	_model->Closed.Changed += Pt::slot(*this, &MainWindow::onClosedByWindow);

	
	//Panel
	_mainPanelModel.Size.set(Pt::Gfx::SizeF(700,480));
	_mainPanelModel.Position.set(Pt::Gfx::PointF(40,40));
	_mainPanelModel.BorderWidth.set(3);	
	_mainPanelModel.BorderStyle.set(Pt::Hmi::BorderStyleType::Sizeable);
	{
		std::stringstream memoryStream;
		memoryStream.write((char*)Pt::Forms::DemoImage::image, Pt::Forms::DemoImage::imageSize);	
		Pt::Gfx::ARgbImage* im = Pt::Gfx::ImageReader::read(memoryStream);
		_mainPanelModel.BackgroundImage = *im;
		delete im;
	}

	_mainPanelModel.BackgroundImageLayout = Pt::Hmi::ImageLayoutType::Strech;

	//Text
	_textLabelModel.AutoSize.set(true);
	_textLabelModel.Size.set(Pt::Gfx::SizeF(500,30));
	_textLabelModel.Caption.set("&This is a Platinum C++ Human Machine Interface demo");
	_textLabelModel.Position.set(Pt::Gfx::PointF(20,20));
	_textLabelModel.ForeColor.set(Pt::Gfx::ARgbColor(255,0,0,0));
	_textLabelModel.UseMnemonic.set(true);	
	//_textLabel.labelModel().PainterSurfaceType = Pt::Hmi::PainterType::Image;
	_mainPanel.addChild(&_textLabel);
	
	//Toggle button
	_toggleButtonModel.ButtonType.set(Hmi::ButtonType::Toggle);
	_toggleButtonModel.Caption.set("Toggle Me [CTRL+I]");
	_toggleButtonModel.ActionKey.set("C//i");
	_toggleButtonModel.Position.set(Pt::Gfx::PointF(20,60));
	_toggleButtonModel.Size.set(Pt::Gfx::SizeF(150,25));
	_textLabel.bindMnemonicToWidget(&_toggleButton);
	_mainPanel.addChild(&_toggleButton);

	//Dialog button
	_dialogButtonModel.ButtonType.set(Hmi::ButtonType::Press);
	_dialogButtonModel.Caption.set("&Dialog [CTRL+D]");
	_dialogButtonModel.ActionKey.set("C//d");
	_dialogButtonModel.Position.set(Pt::Gfx::PointF(20,100));
	_dialogButtonModel.Size.set(Pt::Gfx::SizeF(150,25));
	_dialogButtonModel.UseMnemonic.set(false);
	_dialogButton.PressedAction  += Pt::slot(*this, &MainWindow::onShowDialog);
	
	_mainPanel.addChild(&_dialogButton);
		
	//Close button
	_closeButtonModel.ButtonType.set(Hmi::ButtonType::Press);
	_closeButtonModel.Caption.set("Close [CTRL+X]");
	_closeButtonModel.ActionKey.set("C//x");
	_closeButtonModel.Position.set(Pt::Gfx::PointF(590,525));
	_closeButtonModel.Size.set(Pt::Gfx::SizeF(150,25));
	_closeButton.PressedAction += Pt::slot(*this, &MainWindow::onClosed);
	
	addChild(&_closeButton);
	addChild(&_mainPanel);
}

void MainWindow::onShowDialog(Button* button)
{
	Dialog1 dialog(&_dialog1model);
	dialog.doModal(this);
}

void MainWindow::show()
{
	_model->Visible = true;
	invalidate();
}

void MainWindow::onClosedByWindow(const Property<bool>& closed )
{
	close();
	Pt::Hmi::Application::instance().exit();
}

void MainWindow::onClosed(Button* button)
{
	close();	
	Pt::Hmi::Application::instance().exit();
}

}}}
