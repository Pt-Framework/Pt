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
#include <Pt/Forms/Spacing.h>

namespace Pt{
namespace Forms{
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
	resize( Gfx::SizeF(700,500) );
	move( Gfx::PointF(400,400) );
	setTitle( std::string("This is a sample modal dialog 1") );

  /*
	//New dialog button 
	_newDialogButton.setMargin( Forms::Margin(5) );
	_newDialogButton.Dock = Docking::Right;
	_newDialogButton.Position =Gfx::PointF(10,10);
	_newDialogButton.Size =Gfx::SizeF(200,30);
	_newDialogButton.Caption = std::string("New Dialog [CTRL+F]");
	_newDialogButton.ShortcutKey = std::string("C//f");
	_newDialogButton.Clicked += Pt::slot(*this,&Dialog1::onShowNextDialog);
	_panel1.addChild(&_newDialogButton);

	//Close Button
	_closeButton.Margin = Forms::Margin(5);
	_closeButton.Dock = Docking::Right;
	_closeButton.Position =Gfx::PointF(10,10);
	_closeButton.Size =Gfx::SizeF(200,30);
	_closeButton.ShortcutKey = std::string("C//x");
	_closeButton.Caption = std::string("Close [CTRL+X]");
	_closeButton.Clicked +=  Pt::slot(*this,&Dialog1::onClosedByButton);
	_panel1.addChild(&_closeButton);
  
	_panel1.Size =Gfx::SizeF(200,50);
  _panel1.Dock = Docking::Bottom;
  addChild(&_panel1);

  _panel2.BackColor =Gfx::Color(0, 1, 1,0);
  _panel2.Dock = Docking::Left;
  addChild(&_panel2);
  
  
  _panel3.BackColor =Gfx::Color(0, 0, 1,0);
  _panel3.Dock = Docking::Right;
  addChild(&_panel3);

  _panel4.BackColor =Gfx::Color(0, 1, 1,0);
  _panel4.Dock = Docking::Top;
  addChild(&_panel4);
  
  _panel5.BackColor =Gfx::Color(0, 1, 0,1);
  _panel5.Dock = Docking::Fill;
  addChild(&_panel5);    */
}

void Dialog1::onClosedByButton()
{
	close();
}

void Dialog1::onShowNextDialog()
{
	Dialog2 dialog;
	dialog.showModal();		
}
	
}}}

