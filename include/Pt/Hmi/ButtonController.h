/* Copyright (C) 2013 Marc Boris Duerner 
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Hmi_ButtonController_H
#define Pt_Hmi_ButtonController_H

#include <Pt/Hmi/LabelController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Timer.h>

namespace Pt{
namespace Hmi{

class PointingEvent;

class PT_HMI_API ButtonController  : public LabelController
{
public:
	ButtonController();
	virtual ~ButtonController();

	Signal<Controller*> PressedAction;
	Signal<Controller*> DoublePressedAction;

	Signal<Controller*, bool> CheckedAction;

protected:
	virtual void onPressedAction();
	virtual void onDoublePressedAction();
	virtual void onCheckedAction(bool checked);

	virtual void onModelChanged(bool created = false);

private:
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	void onButtonStateChanged(const void* sender, const PropertyBase& prop);
	void onDoublePressedTimeout();

private:	
	bool _pressed;
	bool _timeout;
	int _pressCounter;
	Pt::System::Timer _doublePressTimer;
	ButtonModel* _myModel;
};

}}
#endif