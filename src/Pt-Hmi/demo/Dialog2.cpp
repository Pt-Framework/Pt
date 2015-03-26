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
#include <Pt/Hmi/Application.h>

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
	Size = Pt::Gfx::SizeF(450,200);
	Position = Pt::Gfx::PointF(400,400);
	Caption = std::string("Pt-Hmi-demo");
	StartPostion = Hmi::WindowStartPosition::CenterParent;
	ShowMaximizeButton.set(false);
	ShowMinimizeButton.set(false);
	ShowSysMenu.set(false);

	_panel1.Dock = Docking::Bottom;
	_panel1.Size.set(Pt::Gfx::SizeF(385,100));
	_panel1.PanelBorderStyle.set(Hmi::BorderStyle::Single);
	_panel1.PanelBorderRoundEdge.set(true);
	addChild(&_panel1);

	//Cancel button
  _cancelButton.Dock = Docking::Right;
	_cancelButton.Size = Pt::Gfx::SizeF(100,23);
	_cancelButton.Caption.set("Cancel");
	_cancelButton.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	_panel1.addChild(&_cancelButton);

	//OK Button
	_okButton.Dock = Docking::Right;
	_okButton.Size = Pt::Gfx::SizeF(100,23);
	_okButton.Caption.set("OK");
	_okButton.ActionKey.set("C//x");
	_okButton.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	_panel1.addChild(&_okButton);

	//Label
	_label.Dock = Docking::Fill;
	_label.AutoSize.set(true);
	_label.Caption.set("Do you want to close me?");
	addChild(&_label);
}
    
void Dialog2::onClosedByButton()
{
	Closed = true;
}

}}}

