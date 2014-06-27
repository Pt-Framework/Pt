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
#include <Pt/Hmi/GfxController.h>
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Renderer.h>
#include <Pt/Hmi/PaintSurface.h>

namespace Pt{
namespace Hmi{

GfxController::GfxController(GfxModel& model, Renderer& renderer)
: Controller(model)
, _renderer(renderer)
{
}

GfxController::~GfxController()
{

}

Pt::Gfx::PointF GfxController::toClient(const Pt::Gfx::PointF& globalPoint)
{
	GfxModel& m = gfxModel();	

	if( parent() == 0)
		return Pt::Gfx::PointF(globalPoint.x(), globalPoint.y());

	Pt::Gfx::PointF parPoint = parent()->toClient(globalPoint);
	return Pt::Gfx::PointF(parPoint.x() - m.Position.get().x(), parPoint.y() - m.Position.get().y());
}

Pt::Gfx::PointF GfxController::fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot)
{
	const GfxController* par = parent();	

	double x = localPoint.x();
	double y = localPoint.y();

	while(par != 0)
	{
		const GfxModel& m = gfxModel();
		par = parent()->parent();
		
		if(!(toRoot && par == 0))
		{
			x += m.Position.get().x();
			y += m.Position.get().y();
		}
	}
	
	return Pt::Gfx::PointF(x,y);
}

const GfxModel& GfxController::gfxModel() const
{
	return static_cast<const GfxModel&>(model());
}

void GfxController::invalidate()
{
	GfxController* par = parent();
	
	if( par != 0)
		par->invalidate();
}


GfxModel& GfxController::gfxModel()
{
	return static_cast<GfxModel&>(model());
}

void GfxController::render()
{
	if(!gfxModel().Visible.get())
		return;	
		
	//Draw me
	_renderer.render(&gfxModel());

	//Let the user to render 
	Render.send(*this, *gfxModel().paintSurface());

	//Render my childs
	for( size_t i = 0; i < children().size(); ++i)
	{
		GfxController* child = childAt(i);
		child->render();
	}
}

void GfxController::output()
{
	if(!gfxModel().Visible.get())
		return;
	
	Pt::Hmi::Painter& localPainter = gfxModel().paintSurface()->painter();

	for( size_t i = 0; i < children().size(); ++i)
	{
		GfxController* child = childAt(i);			
		child->output();

		GfxModel& childModel = child->gfxModel();
		localPainter.drawSurface(childModel.Position.get(),*childModel.paintSurface());
	}

	Controller::output();
}

bool GfxController::onMoveFocusPrev()
{
	if(children().size() == 0)
		return false;

	int index = getFocusedChild();

	if( index != -1)
	{
		GfxController* child = childAt(index);
		GfxModel& model = child->gfxModel();	
		
		if(!model.AcceptFocus.get())
		{
			if(child->moveFocusPrev())
				return true;
		}

		model.Focused = false;
		return focusPrevChild(index);
	}
	
	return focusPrevChild(children().size());
}

bool GfxController::focusPrevChild(int index)
{
	index--;
	
	for( ; index >= 0; --index)
	{
		GfxController* child = childAt(index);
		GfxModel& model = child->gfxModel();		

		if(model.AcceptFocus.get())
		{
			model.Focused = true;
			return true;
		}

		if(child->moveFocusPrev())
		{
			model.Focused = true;
			return true;
		}
	}

	return false;
}



bool GfxController::focusNextChild(int index)
{
	index++;
	
	for( ; index < (int)children().size(); ++index)
	{
		GfxController* child = childAt(index);
		GfxModel& model = child->gfxModel();
		
		if(model.AcceptFocus.get())
		{
			model.Focused = true;
			return true;
		}

		if(child->moveFocusNext())
		{
			model.Focused = true;
			return true;
		}
	}

	return false;
}


int GfxController::getFocusedChild() const
{
	int i = 0;
	
	for( ; i < (int)children().size(); ++i)
	{
		const GfxController* child = childAt(i);
		const GfxModel& model = child->gfxModel();

		if(model.Focused.get())
			return i;		
	}		

	return -1;
}

bool GfxController::onMoveFocusNext()
{
	if(children().size() == 0)
		return false;

	const int index = getFocusedChild();

	if( index == -1)
		return focusNextChild(index);
	
	GfxController* child = childAt(index);
	GfxModel& model = child->gfxModel();	
		
	if(!model.AcceptFocus.get())
	{
		if(child->moveFocusNext())
			return true;
	}

	model.Focused = false;

	return focusNextChild(index);
}

}}
