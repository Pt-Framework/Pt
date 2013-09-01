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

#include <Pt/Hmi/DialogController.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/GfxOutput.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Hmi/DialogModel.h>

namespace Pt{
namespace Hmi{

DialogController::DialogController(GfxModel* m, Renderer* r, GfxOutput* out )
: WindowController(m, r, out)
{				
}

DialogController::~DialogController()
{
}

void DialogController::onClosed()
{
	_closed = true;
}

void DialogController::doModal(WindowController* parent)
{	
	bool		 parentTopMost = false;
	WindowModel* parentModel   = (WindowModel*)parent->model();
	WindowModel* myModel	   = (WindowModel*)model();

	_closed  = false;
	
	//Set my parent window.
	setWindowParent(parent);
	 
	//Setup the parent as disabled and TopMost = false.
	parentTopMost = parentModel->TopMost.get();
	parentModel->Enable = false;	
	parentModel->TopMost = false;

	//Setup the dialog as aenabled and top most.	
	myModel->Enable = true;
	myModel->TopMost = true;

	//Invalidate the dialog
	invalidate();
	
	//Wait of termination of the dialog.
	while(!_closed)
		Application::instance().nextEvent();

	//Restore the parent state.
	parentModel->Enable = true;
	parentModel->TopMost = parentTopMost;
}

}}
