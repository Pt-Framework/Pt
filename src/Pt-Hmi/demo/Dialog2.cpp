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
	Size               = Ui::SizeF(450,200);
	Position           = Ui::PointF(400,400);
	Caption            = std::string("Pt-Hmi-demo");
	StartPostion       = Hmi::WindowStartPosition::CenterParent;
	ShowMaximizeButton = false;
	ShowMinimizeButton = false;
	ShowSysMenu        = false;
	Border = WindowBorder::DialogSizeable;
  	//Label
	_label.Dock         = Docking::Fill;
	_label.AutoSize     = false;
  _label.UseMnemonic  = true;
  _label.TextAlign    = Hmi::Align::MidleCenter;
	_label.Caption      = std::string("&Do you want to close me?");
  _label.Margin       = Hmi::Margin(10);
  _label.bindMnemonicToWidget( _okButton );
	_mainPanel.addChild(&_label);

  //Main panel
  _mainPanel.Name      = std::string("Main panel");
	_mainPanel.Margin    = Hmi::Margin(4);
  _mainPanel.Size      = Ui::SizeF(385,40);
  _mainPanel.Dock      = Hmi::Docking::Fill;
  _mainPanel.PanelBorderStyle = Hmi::BorderStyle::Border3D;
  _mainPanel.PanelBorderWidth = 2;
  _mainPanel.PanelBorderRoundEdge = true;
	addChild(&_mainPanel);

  //Ok/Cancel Panel	
  _okCancelPanel.Margin = Hmi::Margin(6);
  _okCancelPanel.Size = Ui::SizeF(385,80);
  _okCancelPanel.Dock = Hmi::Docking::Bottom;
  _okCancelPanel.PanelBorderStyle = Hmi::BorderStyle::NoBorder;
  _okCancelPanel.FlowLayout = Hmi::FlowLayout::Horizontal;
  _okCancelPanel.FlowDirection = Hmi::FlowLayoutDirection::RightToLeftBottomToTop;	
	_okCancelPanel.BackColor = Ui::Color(1,1,0,0);
	addChild(&_okCancelPanel);

	//Cancel button
	std::string actionCancel;
	actionCancel += (char) 27;
	_cancelButton.ShortcutKey = actionCancel;
	
  _cancelButton.Margin = Hmi::Margin(2,4,2,4);
  _cancelButton.Dock = Docking::Right;
	_cancelButton.Size = Ui::SizeF(100,40);
	_cancelButton.Caption = std::string("Cancel");
	_cancelButton.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	_okCancelPanel.addChild(&_cancelButton);

	//OK Button
  _okButton.Margin = Hmi::Margin(2,4,2,4);
	_okButton.Dock = Docking::Right;
	_okButton.Size = Ui::SizeF(100,40);
	_okButton.Caption = std::string("OK");
	_okButton.ShortcutKey = std::string("C//x");
	_okButton.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	std::string ac;
	ac += (char) 13;
	_okButton.ShortcutKey = ac;
	_okCancelPanel.addChild(&_okButton);

}
    
void Dialog2::onClosedByButton()
{
	close();
}

}}}

