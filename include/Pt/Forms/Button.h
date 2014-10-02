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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Hmi_Desktop_Button_H
#define Pt_Hmi_Desktop_Button_H

#include <Pt/Forms/Widget.h>
#include <Pt/Signal.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/ButtonRenderer.h>

namespace Pt{
namespace Forms{  

class PT_FORMS_API Button : public Widget 
{
public:
	Button();
	virtual ~Button();
    
    void setToggleButton(bool toggle = true);    
	bool isToggleButton() const;

	void setCaption(const std::string& caption);
    const std::string& caption() const;

	void setSize(const Pt::Gfx::SizeF& size);
	const Pt::Gfx::SizeF& size() const;

    void setPosition(const Pt::Gfx::PointF& position);
	const Pt::Gfx::PointF& position() const;

	void setActionKey(const std::string& keyString);
	const std::string& actionKey() const;

	const Hmi::ButtonModel& buttonModel() const;
	const Hmi::ButtonController& buttonController() const;

	Hmi::ButtonModel& buttonModel();
	Hmi::ButtonController& buttonController();
	void setButtonController(Pt::Hmi::ButtonController& ctrl);

public:    
    Pt::Signal<>	 ClickedAction;
    Pt::Signal<bool> CheckedAction;

protected:
    virtual void onClicked();
    virtual void onChecked(bool state);

protected:
	virtual Hmi::WidgetController& widgetController()
	{
		return *_currentController;
	}
    
private:
    void handleOnClicked(Hmi::Controller* sender);
    void handleOnChecked(Hmi::Controller* sender, bool state);
    
private:
	Pt::Hmi::ButtonModel      _defModel;
	Pt::Hmi::ButtonRenderer   _defRenderer;
	Pt::Hmi::ButtonController _defController;
	Pt::Hmi::ButtonController* _currentController;
};
 
}}
#endif