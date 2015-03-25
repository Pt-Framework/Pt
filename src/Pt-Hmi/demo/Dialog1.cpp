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
	Size  = Pt::Gfx::SizeF(700,500);
	Position = Pt::Gfx::PointF(400,400);
	Caption = std::string("This is a sample modal dialog 1");
	WindowStartPostion = Hmi::WindowStartPositionType::CenterParent;

	//New dialog button 
	_newDialog.Position.set(Pt::Gfx::PointF(400,300));
	_newDialog.Size.set(Pt::Gfx::SizeF(200,25));
	_newDialog.Caption.set("New Dialog [CTRL+F]");
	_newDialog.ActionKey.set("C//f");
	_newDialog.Clicked += Pt::slot(*this,&Dialog1::onShowNextDialog);
	addChild(&_newDialog);

	//Close Button
	_closeButton.Position.set(Pt::Gfx::PointF(400,360));
	_closeButton.Size.set(Pt::Gfx::SizeF(200,25));
	_closeButton.ActionKey.set("C//x");
	_closeButton.Caption.set("Close [CTRL+X]");
	_closeButton.Clicked +=  Pt::slot(*this,&Dialog1::onClosedByButton);
	addChild(&_closeButton);
}

void Dialog1::onClosedByButton()
{
	Closed = true;		
}

void Dialog1::onShowNextDialog()
{
	Dialog2 dialog;
	dialog.doModal(this);		
}
	
}}}

