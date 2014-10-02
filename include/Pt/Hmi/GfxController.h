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
#ifndef Pt_Hmi_GfxController_H
#define Pt_Hmi_GfxController_H

#include <Pt/Hmi/Controller.h>
#include <Pt/Gfx/Painter.h>

namespace Pt{
namespace Hmi{

class GfxModel;
class Renderer;
class PaintSurface;

class PT_HMI_API GfxController : public Controller
{
public:
	GfxController(GfxModel& model, Renderer& renderer);
	virtual ~GfxController();

	Pt::Gfx::PointF toClient(const Pt::Gfx::PointF& globalPoint);
	Pt::Gfx::PointF fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot);

	virtual void render();
	virtual void output();
	virtual void invalidate();
			
	GfxModel& gfxModel();
	const GfxModel& gfxModel() const;
	
	Pt::Signal<GfxController&, PaintSurface&> Render;
	Pt::Signal<GfxController&> Output;

	inline const GfxController* GfxController::parent() const
	{
		return dynamic_cast<const GfxController*>(Controller::widgetParent());
	}

	inline GfxController* GfxController::parent()
	{
		return dynamic_cast<GfxController*>(Controller::widgetParent());
	}

protected:
	bool onMoveFocusNext();
	bool onMoveFocusPrev();	
	void onFocusChanged(const Property<bool>& prop);

private:
	bool focusNextChild(int index);
	bool focusPrevChild(int index);
	int getFocusedChild() const;

	inline GfxController* childAt(size_t index)
	{
		return dynamic_cast<GfxController*> (children()[index]);
	}

	inline const GfxController* childAt(size_t index) const
	{
		return dynamic_cast<const GfxController*> (children()[index]);
	}

private:
	Renderer& _renderer;
};

}}
#endif