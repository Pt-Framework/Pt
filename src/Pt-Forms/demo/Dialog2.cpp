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
#include <Pt/Forms/Application.h>

namespace Pt{
namespace Forms{
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
/*
	//Dialog
	Size               =Gfx::SizeF(450,200);
	Position           =Gfx::PointF(400,400);
	Caption            = std::string("Pt-Forms-demo");
	StartPostion       = Forms::WindowStartPosition::CenterParent;
	ShowMaximizeButton = false;
	ShowMinimizeButton = false;
	ShowSysMenu        = false;
	Border = WindowBorder::DialogSizeable;
  	//Label
	_label.Dock         = Docking::Fill;
	_label.AutoSize     = false;
  _label.UseMnemonic  = true;
  _label.TextAlign    = Forms::Align::MidleCenter;
	_label.Caption      = std::string("&Do you want to close me?");
  _label.Margin       = Forms::Margin(10);
  _label.bindMnemonicToWidget( _okButton );
	_mainPanel.addChild(&_label);

  //Main panel
  _mainPanel.Name      = std::string("Main panel");
	_mainPanel.Margin    = Forms::Margin(4);
  _mainPanel.Size      =Gfx::SizeF(385,40);
  _mainPanel.Dock      = Forms::Docking::Fill;
  _mainPanel.PanelBorderStyle = Forms::BorderStyle::Border3D;
  _mainPanel.PanelBorderWidth = 2;
  _mainPanel.PanelBorderRoundEdge = true;
	addChild(&_mainPanel);

  //Ok/Cancel Panel	
  _okCancelPanel.Margin = Forms::Margin(5);
  _okCancelPanel.Size =Gfx::SizeF(385,80);
  _okCancelPanel.Dock = Forms::Docking::Bottom;
  _okCancelPanel.PanelBorderStyle = Forms::BorderStyle::NoBorder;
  _okCancelPanel.FlowLayout = Forms::FlowLayout::Horizontal;
  _okCancelPanel.FlowDirection = Forms::FlowLayoutDirection::RightToLeftBottomToTop;
	_okCancelPanel.BackColor =Gfx::ColorF(1,1,0,0);
	addChild(&_okCancelPanel);

	//Cancel button
	std::string actionCancel;
	actionCancel += (char) 27;
	_cancelButton.ShortcutKey = actionCancel;
	
  _cancelButton.Margin = Forms::Margin(5);
//  _cancelButton.Dock = Docking::Right;
	_cancelButton.Size =Gfx::SizeF(100,40);
	_cancelButton.Caption = std::string("Cancel");
	_cancelButton.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	_okCancelPanel.addChild(&_cancelButton);

	//OK Button
  _okButton.Margin = Forms::Margin(5);
//	_okButton.Dock = Docking::Right;
	_okButton.Size =Gfx::SizeF(100,40);
	_okButton.Caption = std::string("OK");
	_okButton.ShortcutKey = std::string("C//x");
	_okButton.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	std::string ac;
	ac += (char) 13;
	_okButton.ShortcutKey = ac;
	_okCancelPanel.addChild(&_okButton);*/

}
    
void Dialog2::onClosedByButton()
{
	close();
}

}}}

