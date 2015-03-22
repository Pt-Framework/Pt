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


Dialog2::Dialog2(DialogModel* model)
: Dialog(model)
, _okButton(&_okButtonModel)
, _cancelButton(&_cancelButtonModel)
, _label(&_labelModel)
, _panel(&_panelModel)
, _model(model)
{
	init();
}

Dialog2::~Dialog2()
{
}

void Dialog2::init()
{
	//Dialog
	_model->Size.set(Pt::Gfx::SizeF(450,200));
	_model->Position.set(Pt::Gfx::PointF(400,400));
	_model->Caption.set("Pt-Hmi-demo");
	_model->WindowStartPostion.set(Hmi::WindowStartPositionType::CenterParent);
	_model->ShowMaximizeButton.set(false);
	_model->ShowMinimizeButton.set(false);
	_model->ShowSysMenu.set(false);

	_panelModel.Position.set(Pt::Gfx::PointF(25,15)); 
	_panelModel.Size.set(Pt::Gfx::SizeF(385,100));
	_panelModel.BorderStyle.set(Hmi::BorderStyleType::Single);
	_panelModel.BorderRoundEdge.set(true);
	addChild(&_panel);

	//Label
		_labelModel.Position.set(Pt::Gfx::PointF(50,40));
	_labelModel.AutoSize.set(true);
	_labelModel.Caption.set("Do you want to close me?");
	_panel.addChild(&_label);

	//OK Button
	_okButtonModel.Position.set(Pt::Gfx::PointF(200,125));
	_okButtonModel.Size.set(Pt::Gfx::SizeF(100,23));
	_okButtonModel.Caption.set("OK");
	_okButtonModel.ActionKey.set("C//x");
	_okButtonModel.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	addChild(&_okButton);

	//Cancel button
	_cancelButtonModel.Position.set(Pt::Gfx::PointF(310,125));
	_cancelButtonModel.Size.set(Pt::Gfx::SizeF(100,23));
	_cancelButtonModel.Caption.set("Cancel");
	_cancelButtonModel.Clicked += Pt::slot(*this,&Dialog2::onClosedByButton);
	addChild(&_cancelButton);
}
    
void Dialog2::onClosedByButton()
{
	close();
}

}}}

