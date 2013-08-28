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
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/Application.h>
#include "Dialog1.h"

namespace Pt{
namespace Hmi{
namespace Demo{

MainWindow::MainWindow()
: _clicked(false)
{
	init();
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
	//Window
	Pt::Hmi::WindowController* windowController = (Pt::Hmi::WindowController*) &this->controller();
	Pt::Hmi::WindowModel* windowModel = (Pt::Hmi::WindowModel*) windowController->model();	
	windowModel->WinPos.set(Pt::Gfx::PointF(200,200));
	windowModel->WinSize.set(Pt::Gfx::SizeF(800,615));
	windowModel->Caption.set("Pt-Hmi demo");
	windowModel->Closed.PropertyChanged += Pt::slot(*this, &MainWindow::onClosedByWindow);

	//Panel
	Pt::Hmi::PanelModel* panelModel = (Pt::Hmi::PanelModel*)_mainPanel.controller().model();
	panelModel->Position.set(Pt::Gfx::PointF(40,40));
	panelModel->Size.set(Pt::Gfx::SizeF(700,480));
	panelModel->BorderStyle.set(Pt::Hmi::BorderStyle::Single);
	addChild(&_mainPanel);

	//Text
	Pt::Hmi::LabelModel* labelModel = (Pt::Hmi::LabelModel*) _textLabel.controller().model();
	labelModel->AutoSize.set(true);
	labelModel->Caption.set("This is a Platinum C++ Human Mashine Interface demo");
	labelModel->Position.set(Pt::Gfx::PointF(20,20));
	labelModel->ForeColor.set(Pt::Gfx::ARgbColor(255,0,0,0));
	_mainPanel.addChild(&_textLabel);

	//Toggle button
	Pt::Hmi::ButtonModel* toggleButtonModel = (Pt::Hmi::ButtonModel*) _toggleButton.controller().model();
	toggleButtonModel->ButtonType.set(Pt::Hmi::ButtonType::Toggle);
	toggleButtonModel->Caption.set("Toggle Me");
	toggleButtonModel->Position.set(Pt::Gfx::PointF(20,60));
	toggleButtonModel->Size.set(Pt::Gfx::SizeF(150,25));
	_mainPanel.addChild(&_toggleButton);

	//Dialog button
	Pt::Hmi::ButtonModel* dialogButtonModel = (Pt::Hmi::ButtonModel*) _dialogButton.controller().model();
	dialogButtonModel->ButtonType.set(Pt::Hmi::ButtonType::Press);
	dialogButtonModel->Caption.set("Dialog [CTRL+D]");
	dialogButtonModel->ActionKey.set("CTRL//D");
	dialogButtonModel->Position.set(Pt::Gfx::PointF(20,100));
	dialogButtonModel->Size.set(Pt::Gfx::SizeF(150,25));
	dialogButtonModel->ButtonState.PropertyChanged += Pt::slot(*this, &MainWindow::onShowDialog);
	_mainPanel.addChild(&_dialogButton);

	//Close button
	Pt::Hmi::ButtonModel* closeButtonModel = (Pt::Hmi::ButtonModel*) _closeButton.controller().model();
	closeButtonModel->ButtonType.set(Pt::Hmi::ButtonType::Press);
	closeButtonModel->Caption.set("Close [CTRL+X]");
	closeButtonModel->ActionKey.set("CTRL//X");
	closeButtonModel->Position.set(Pt::Gfx::PointF(590,525));
	closeButtonModel->Size.set(Pt::Gfx::SizeF(150,25));
	closeButtonModel->ButtonState.PropertyChanged += Pt::slot(*this, &MainWindow::onClosedByButton);
	addChild(&_closeButton);
}


void MainWindow::onShowDialog(Pt::Hmi::DeviceButton::State& state)
{
	if( state == Pt::Hmi::DeviceButton::Pressed)
	{
		_clicked = true;
		return;
	}

	if( state == Pt::Hmi::DeviceButton::Released && _clicked)
	{
		_clicked = false;
		Dialog1 dialog;
		dialog.show(this);
	}
}

void MainWindow::onClosedByButton(Pt::Hmi::DeviceButton::State& state)
{
	if( state == Pt::Hmi::DeviceButton::Pressed)
		Pt::Hmi::Application::instance().exit();
}

void MainWindow::onClosedByWindow(bool& state)
{
	if( state)
		Pt::Hmi::Application::instance().exit();
}

}}}
