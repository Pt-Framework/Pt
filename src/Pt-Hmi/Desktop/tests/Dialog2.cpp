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

#include "Dialog2.h"
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/DialogController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/ButtonController.h>

namespace Pt{
namespace Hmi{
namespace Demo{

Dialog2::Dialog2()
{
	init();
}

Dialog2::~Dialog2()
{
}

void Dialog2::init()
{
	//Dialog
	setSize(Pt::Gfx::SizeF(800,600));
	setPosition(Pt::Gfx::PointF(400,400));
	dialogModel().Caption.set("This is a sample modal dialog 2");

	//Close Button
	_closeButton.setPosition(Pt::Gfx::PointF(400,360));
	_closeButton.setSize(Pt::Gfx::SizeF(200,25));
	_closeButton.setCaption("Close [CTRL+X]");
	_closeButton.setActionKey("C//x");
	_closeButton.ClickedAction += Pt::slot(*this,&Dialog2::onClosedByButton);
	addChild(&_closeButton);
}

void Dialog2::onClosedByButton()
{
	((DialogController*)&controller())->close();
}

}}}

