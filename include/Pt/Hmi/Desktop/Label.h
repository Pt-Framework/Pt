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
#ifndef Pt_Hmi_Desktop_Label_H
#define Pt_Hmi_Desktop_Label_H

#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Hmi/LabelController.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/LabelRenderer.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

class PT_HMI_DESKTOP_API Label : public Widget 
{
public:
	Label();
	virtual ~Label();

	void setCaption(const std::string& caption);
	const std::string& caption() const;

	void setPosition(const Pt::Gfx::PointF& position);
	const Pt::Gfx::PointF& position() const;

	void setSize(const Pt::Gfx::SizeF& size);
	const Pt::Gfx::SizeF& size() const;

	void setAutoSize(bool autoSize=true);
	bool isAutoSize() const;

	Pt::Hmi::LabelController& labelController();
	void setLabelController(Pt::Hmi::LabelController& controller);
	Pt::Hmi::LabelModel& labelModel();


	const Pt::Hmi::LabelController& labelController() const;
	const Pt::Hmi::LabelModel& labelModel() const;

protected:
	virtual WidgetController& widgetController()
	{
		return *_currController;
	}

private:
	Pt::Hmi::LabelModel		  _defModel;
	Pt::Hmi::LabelRenderer	  _defRenderer;
	Pt::Hmi::LabelController  _defController;
	Pt::Hmi::LabelController* _currController;
};
 
}}}
#endif