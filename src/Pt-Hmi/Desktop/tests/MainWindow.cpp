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
#include "Dialog1.h"

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
	Pt::Hmi::WindowController* windowController = (Pt::Hmi::WindowController*) &this->controller();
	Pt::Hmi::WindowModel* windowModel = (Pt::Hmi::WindowModel*) windowController->model();	
	windowModel->WinPos.set(Pt::Gfx::PointF(200,200));
	windowModel->WinSize.set(Pt::Gfx::SizeF(800,615));
	windowModel->Caption.set("Pt-Hmi demo");
	windowController->ClosedAction += Pt::slot(*this, &MainWindow::onClosed);

	//Panel
	Pt::Hmi::PanelModel* panelModel = (Pt::Hmi::PanelModel*)_mainPanel.controller().model();
	panelModel->Position.set(Pt::Gfx::PointF(40,40));
	panelModel->BorderWidth.set(3);
	panelModel->Size.set(Pt::Gfx::SizeF(700,480));
	panelModel->BorderStyle.set(Pt::Hmi::BorderStyleType::Sizeable);
	addChild(&_mainPanel);

	//Text
	Pt::Hmi::LabelModel* labelModel = (Pt::Hmi::LabelModel*) _textLabel.controller().model();
	labelModel->AutoSize.set(true);
	labelModel->Caption.set("This is a Platinum C++ Human Mashine Interface demo");
	labelModel->Position.set(Pt::Gfx::PointF(20,20));
	labelModel->ForeColor.set(Pt::Gfx::ARgbColor(255,0,0,0));
	_mainPanel.addChild(&_textLabel);

	//Toggle button
	_toggleButton.setToggleButton(true);
	_toggleButton.setCaption("Toggle Me");
	_toggleButton.setPosition(Pt::Gfx::PointF(20,60));
	_toggleButton.setSize(Pt::Gfx::SizeF(150,25));
	_mainPanel.addChild(&_toggleButton);

	//Dialog button
	Pt::Hmi::ButtonController* dialogButtonCotroller = (Pt::Hmi::ButtonController*) &_dialogButton.controller();
	Pt::Hmi::ButtonModel* dialogButtonModel = (Pt::Hmi::ButtonModel*) dialogButtonCotroller->model();
	_dialogButton.setToggleButton(false);
	dialogButtonModel->Caption.set("Dialog [CTRL+D]");
	dialogButtonModel->ActionKey.set("C//d");
	dialogButtonModel->Position.set(Pt::Gfx::PointF(20,100));
	dialogButtonModel->Size.set(Pt::Gfx::SizeF(150,25));
	dialogButtonCotroller->PressedAction  += Pt::slot(*this, &MainWindow::onShowDialog);
	_mainPanel.addChild(&_dialogButton);

	//Close button
	Pt::Hmi::ButtonController* closeButtonCtrl = (Pt::Hmi::ButtonController*) &_closeButton.controller();
	Pt::Hmi::ButtonModel* closeButtonModel = (Pt::Hmi::ButtonModel*) closeButtonCtrl->model();
	closeButtonModel->ButtonType.set(Pt::Hmi::ButtonType::Press);
	closeButtonModel->Caption.set("Close [CTRL+X]");
	closeButtonModel->ActionKey.set("C//x");
	closeButtonModel->Position.set(Pt::Gfx::PointF(590,525));
	closeButtonModel->Size.set(Pt::Gfx::SizeF(150,25));
	closeButtonCtrl->PressedAction += Pt::slot(*this, &MainWindow::onClosed);
	addChild(&_closeButton);
}

void MainWindow::onShowDialog(Pt::Hmi::Controller* ctrl)
{
	Dialog1 dialog;
	dialog.show(this);
}

void MainWindow::onClosed(Pt::Hmi::Controller* ctrl)
{
	Pt::Hmi::Application::instance().exit();
}

}}}
