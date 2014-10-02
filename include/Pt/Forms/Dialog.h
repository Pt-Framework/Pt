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
#ifndef Pt_Forms_Dialog_H
#define Pt_Forms_Dialog_H

#include <Pt/Forms/Window.h>
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/View.h>
#include <Pt/Hmi/DialogController.h>
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/DialogRenderer.h>
#include <Pt/Hmi/Widget.h>

namespace Pt {
namespace Forms {

class PT_FORMS_API Dialog : public Window
{
public:
	Dialog();
	virtual ~Dialog();

	void show(Window* parent);
	void show(Dialog* parent);

	virtual Hmi::DialogResultType::Type result() const;
	virtual void setResult(Hmi::DialogResultType::Type r);

	Hmi::DialogController& dialogController();
	void setDialogController(Hmi::DialogController& controller);

	Hmi::DialogModel& dialogModel();

	const Hmi::DialogController& dialogController() const;
	const Hmi::DialogModel& dialogModel()const ;

	virtual void setSize(const Pt::Gfx::SizeF& size);
	virtual const Pt::Gfx::SizeF& size() const;

    virtual void setPosition(const Pt::Gfx::PointF& position);
	virtual const Pt::Gfx::PointF& position() const;

protected:
	virtual void show(Hmi::WindowController& parent); 

private:
	Pt::Hmi::DialogModel		_defModel;
	Pt::Hmi::DialogRenderer 	_defRenderer;

	Pt::Hmi::DialogController	_defController;
	Pt::Hmi::DialogController*	_currController;

	Pt::Hmi::View    _View;
};
 
}}
#endif
