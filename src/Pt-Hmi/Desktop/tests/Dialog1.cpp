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

#include "Dialog1.h"
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/DialogController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/ButtonController.h>
#include "Dialog2.h"

namespace Pt{
namespace Hmi{
namespace Demo{

Dialog1::Dialog1()
: _clicked(false)
{
	init();
}

Dialog1::~Dialog1()
{
}

void Dialog1::init()
{
	//Dialog
	DialogModel* dialogModel = (DialogModel*) controller().model();
	dialogModel->WinSize.set(Pt::Gfx::SizeF(800,600));
	dialogModel->WinPos.set(Pt::Gfx::PointF(400,400));
	dialogModel->Caption.set("This is a sample modal dialog 1");

	//New dialog button 
	Pt::Hmi::ButtonController* newDialogController = (Pt::Hmi::ButtonController*) &_newDialog.controller();
	Pt::Hmi::ButtonModel* newDialogButtonModel = (Pt::Hmi::ButtonModel*) newDialogController->model();
	newDialogButtonModel->Position.set(Pt::Gfx::PointF(400,300));
	newDialogButtonModel->Size.set(Pt::Gfx::SizeF(200,25));
	newDialogButtonModel->Caption.set("New Dialog [CTRL+F]");
	newDialogButtonModel->ActionKey.set("C//f");
	newDialogController->PressedAction += Pt::slot(*this,&Dialog1::onShowNextDialog);
	addChild(&_newDialog);

	//Close Button
	Pt::Hmi::ButtonController* closeButtonController = (Pt::Hmi::ButtonController*) &_closeButton.controller();
	Pt::Hmi::ButtonModel* closeButtonModel = (Pt::Hmi::ButtonModel*) closeButtonController->model();
	closeButtonModel->Position.set(Pt::Gfx::PointF(400,360));
	closeButtonModel->Size.set(Pt::Gfx::SizeF(200,25));
	closeButtonModel->Caption.set("Close [CTRL+X]");
	closeButtonModel->ActionKey.set("C//x");
	closeButtonController->PressedAction += Pt::slot(*this,&Dialog1::onClosedByButton);
	addChild(&_closeButton);
}

void Dialog1::onClosedByButton(Controller* ctrl)
{
	DialogModel* m = (DialogModel*) controller().model();
	m->Closed = true;
}

void Dialog1::onShowNextDialog(Controller* ctrl)
{
	Dialog2 dialog;
	dialog.show(this);		
}
	
}}}

