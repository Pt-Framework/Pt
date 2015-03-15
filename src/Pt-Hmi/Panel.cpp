#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/PanelView.h>

namespace Pt{
namespace Hmi{

Panel::Panel(PanelModel& model, PanelView& view)
: Widget(model, view)
, _resizeDir(No)
{
	
}

Panel::~Panel()
{
}


void Panel::handleResize(const PointingEvent& ev)
{
	const std::vector<DeviceButton>& but = ev.buttons();

	if( but.size() == 0)
		return;
	
	Pt::Gfx::PointF pos(ev.x(), ev.y());

	if(but[0].state() != DeviceButton::Pressed)
	{
		_lastSizePoint = pos;
		_resizeDir = No;
		return;
	}	
	
	recalcPosAndSize(pos, _resizeDir);
}

void Panel::recalcPosAndSize(const Pt::Gfx::PointF& p, ResizeDirection dir)
{
	WidgetModel& model = widgetModel();

	double width  = model.Size.get().width();
	double height = model.Size.get().height();
	double posX   =  model.Position.get().x();
	double posY   =  model.Position.get().y();
	double deltaX =  (p.x() - _lastSizePoint.x());
	double deltaY =  (p.y() - _lastSizePoint.y());

	switch(dir)
	{
		case  North:
		{			
			posY +=  deltaY;
			height -= deltaY;
		}
		break;
		
		case NorthEast:
		{
			posY +=  deltaY;
			height -= deltaY;
			width += deltaX;
		}
		break;

		case East:
		{
			width += deltaX;
		}
		break;

		case SouthEast:
		{
			height += deltaY;
			width += deltaX;
		}
		break;

		case South:
		{
			height += deltaY;
		}
		break;

		case SouthWest:
		{
			height += deltaY;
			posX +=  deltaX;
			width -= deltaX;
		}
		break;

		case West:
		{
			posX +=  deltaX;
			width -= deltaX;
		}		
		break;

		case NorthWest:
		{
			posX +=  deltaX;
			width -= deltaX;
			posY +=  deltaY;
			height -= deltaY;
		}
		break;
		default:
        {
    
		}
        break;
	}

	model.Size = Pt::Gfx::SizeF(width,height);
	model.Position = Pt::Gfx::PointF(posX, posY);
	_lastSizePoint = p;
	output();	
}


void Panel::onPointerInput(const PointingEvent& ev)
{
	Pt::Gfx::PointF p =  toClient(Pt::Gfx::PointF(ev.x(), ev.y()));

	PanelModel& m = panelModel();	
	Pt::Gfx::SizeF size = m.Size.get();
	
	double sizeR = size.width() -  m.BorderWidth.get();
	double sizeB = size.height() -  m.BorderWidth.get();

			
	switch(m.BorderStyle.get())
	{
		case BorderStyleType::Sizeable:
		{
			if( m.contains(p))
			{
				if(p.x() < m.BorderWidth.get() && p.y() <  m.BorderWidth.get())
				{//Corner NW
					m.CursorT.get().setCursor(Cursors::SizeNS);
					_resizeDir = NorthWest;
				}	
				else if(p.x() > sizeR && p.y() <  m.BorderWidth.get())
				{//corner NE
					m.CursorT.get().setCursor(Cursors::SizeNS);
					_resizeDir= NorthEast;
				}
				else if(p.x() < m.BorderWidth.get() &&  p.y() > sizeB )
				{//corner SW
					m.CursorT.get().setCursor(Cursors::SizeWE);
					_resizeDir = SouthWest;
				}
				else if(p.x() > sizeR &&  p.y() > sizeB )
				{//corner SE
					m.CursorT.get().setCursor(Cursors::SizeWE);
					_resizeDir = SouthEast;
				}
				else
				{
					if( p.x() < m.BorderWidth.get())				
					{//West
						m.CursorT.get().setCursor(Cursors::SizeWE);
						_resizeDir = West;
					}
					else if(p.x() >= sizeR)
					{//East
						m.CursorT.get().setCursor(Cursors::SizeWE);
						_resizeDir = East;
					}
					else if( p.y() < m.BorderWidth.get())
					{//North
						m.CursorT.get().setCursor(Cursors::SizeNS);
						_resizeDir = North;
					}
					else if(p.y() >sizeB)
					{//South
						m.CursorT.get().setCursor(Cursors::SizeNS);
						_resizeDir = South;
					}
					else
					{
						m.CursorT.get().setCursor(Cursors::Default);
					}
				}
			}
            else
            {
                m.CursorT.get().setCursor(Cursors::Default);
            }
			handleResize(ev);
		}
		break;
            
		default:
          
		break;
	}
	
	
	Widget::onPointerInput(ev);			
}


}}
