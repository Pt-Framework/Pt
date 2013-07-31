#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/WindowController.h>

namespace Pt{
namespace Hmi{

PanelController::PanelController()
{
	
}

PanelController::~PanelController()
{
}


void PanelController::recalcPosSize(const Pt::Gfx::PointF& p)
{

}

void PanelController::onInput2D(const Event2D& ev)
{
	Pt::Gfx::PointF p =  toClient(Pt::Gfx::PointF(ev.x(), ev.y()));

	PanelModel* m = (PanelModel*) gfxModel();
	Pt::Gfx::SizeF size = m->Size.get();
	
	double sizeR = size.width() -  m->BorderWidth.get();
	double sizeB = size.height() -  m->BorderWidth.get();

	m->CursorStatus.get().setCursor(Cursors::Default);

	switch(m->BorderStyle.get())
	{
		case BorderStyle::Sizebale:
		case BorderStyle::ToolSizeable:
		{
			if( p.x() < size.width() && p.y() < size.height())
			{
				if(p.x() < m->BorderWidth.get() && p.y() <  m->BorderWidth.get())
				{//Corner NW
					m->CursorStatus.get().setCursor(Cursors::SizeNWSE);
					recalcPosSize(p);
				}	
				else if(p.x() > sizeR && p.y() <  m->BorderWidth.get())
				{//corner NE
					m->CursorStatus.get().setCursor(Cursors::SizeNESW);
					recalcPosSize(p);
				}
				else if(p.x() < m->BorderWidth.get() &&  p.y() > sizeB )
				{//corner SW
					m->CursorStatus.get().setCursor(Cursors::SizeNESW);
					recalcPosSize(p);
				}
				else if(p.x() > sizeR &&  p.y() > sizeB )
				{//corner SE
					m->CursorStatus.get().setCursor(Cursors::SizeNWSE);
					recalcPosSize(p);
				}
				else
				{
					if( p.x() < m->BorderWidth.get()  || p.x() > sizeR)
					{//W<->E
						m->CursorStatus.get().setCursor(Cursors::SizeWE);
						recalcPosSize(p);
					}
					else if( p.y() < m->BorderWidth.get() || p.y() >sizeB)
					{//N<->S
						m->CursorStatus.get().setCursor(Cursors::SizeNS);
						recalcPosSize(p);
					}
				}
			}
		}
		break;
	}
	
	WidgetController::onInput2D(ev);			
}


}}
