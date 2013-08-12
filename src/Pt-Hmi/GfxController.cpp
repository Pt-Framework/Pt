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
#include <Pt/Gfx/ImagePainter.h>

namespace Pt{
namespace Hmi{

GfxController::GfxController()
{
}

GfxController::~GfxController()
{

}

Pt::Gfx::PointF GfxController::toClient(const Pt::Gfx::PointF& globalPoint)
{
	GfxController* par = dynamic_cast<GfxController*>(Controller::parent());	
	GfxModel* m = gfxModel();	

	if( par == 0)
		return Pt::Gfx::PointF(globalPoint.x(), globalPoint.y());	

	Pt::Gfx::PointF parPoint = par->toClient(globalPoint);
	return Pt::Gfx::PointF(parPoint.x() - m->Position.get().x(), parPoint.y() - m->Position.get().y());
}

Pt::Gfx::PointF GfxController::fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot)
{
	const GfxController* par = dynamic_cast<const GfxController*>(Controller::parent());
	const GfxModel* m = gfxModel();

	double x = localPoint.x();
	double y = localPoint.y();

	while(par != 0)
	{
		m = par->gfxModel();
		par = dynamic_cast<const GfxController*>(par->parent());
		
		if(!(toRoot && par == 0))
		{
			x += m->Position.get().x();
			y += m->Position.get().y();
		}
	}
	
	return Pt::Gfx::PointF(x,y);
}

const GfxModel* GfxController::gfxModel() const
{
	const GfxModel* m = dynamic_cast<const GfxModel*>(model());

	if( m == 0)
		throw std::logic_error("GfXmOdel"); 

	return m;
}

void GfxController::invalidate()
{
	GfxController* par = dynamic_cast<GfxController*>(parent());
	
	if( par != 0)
		par->invalidate();
}


GfxModel* GfxController::gfxModel()
{
	GfxModel* m = dynamic_cast<GfxModel*>(model());

	if( m == 0)
		throw std::logic_error("ERROR: GfxModel expected."); 

	return m;
}


void GfxController::render()
{
	GfxModel* m = (GfxModel*) model();

	if(!m->Visible.get())
		return;

	//Draw me
	Renderer* re = renderer();	
	re->render(model());

	//Draw my childs
	for( size_t i = 0; i < children().size(); ++i)
	{
		GfxController* child = dynamic_cast<GfxController*> (children()[i]);
		child->render();
	}

	//Bit-Blit my childs	
	Pt::Gfx::ImagePainter localPainter(m->PaintBuffer);

	for( size_t i = 0; i < children().size(); ++i)
	{
		GfxController* child = dynamic_cast<GfxController*> (children()[i]);				

		GfxModel* childModel = (GfxModel*) child->model();
		Pt::Gfx::Point pos  = childModel->fromUnit(childModel->Position.get());
		localPainter.drawImage(pos,childModel->PaintBuffer);
	}
}


bool GfxController::onMoveFocusPrev()
{
	if(children().size() == 0)
		return false;

	int index = getFocusedChild();

	if( index != -1)
	{
		Controller* child = children()[index];
		GfxModel* model = (GfxModel*)child->model();	
		
		if(!model->AcceptFocus.get())
		{
			if(child->moveFocusPrev())
				return true;
		}

		model->Focused = false;
		return focusPrevChild(index);
	}
	
	return focusPrevChild(children().size());
}

bool GfxController::focusPrevChild(int index)
{
	index--;
	
	for( ; index >= 0; --index)
	{
		Controller* child = children()[index];
		GfxModel* model = (GfxModel*)child->model();		

		if(model->AcceptFocus.get())
		{
			model->Focused = true;
			return true;
		}

		if(child->moveFocusPrev())
		{
			model->Focused = true;
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
		Controller* child = children()[index];
		GfxModel* model = (GfxModel*)child->model();
		
		if(model->AcceptFocus.get())
		{
			model->Focused = true;
			return true;
		}

		if(child->moveFocusNext())
		{
			model->Focused = true;
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
		const GfxController* child = (GfxController*)children()[i];
		const GfxModel* model = (const GfxModel*)child->model();

		if(model->Focused.get())
			return i;		
	}		

	return -1;
}

bool GfxController::onMoveFocusNext()
{
	if(children().size() == 0)
		return false;

	const int index = getFocusedChild();

	if( index != -1)
	{
		Controller* child = children()[index];
		GfxModel* model = (GfxModel*)child->model();	
		
		if(!model->AcceptFocus.get())
		{
			if(child->moveFocusNext())
				return true;
		}

		model->Focused = false;
	}

	return focusNextChild(index);
}

}}
