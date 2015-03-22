#include <Pt/Hmi/Panel.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>

namespace Pt{
namespace Hmi{

Panel::Panel()
: _resizeDir(No)
, PT_HMI_INIT_PROPERTY_VALUE(BorderStyle,BorderStyleType::Single)
, PT_HMI_INIT_PROPERTY_VALUE(BorderWidth,3)
, PT_HMI_INIT_PROPERTY_VALUE(BorderRoundEdge,false)
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
	double width  = Size.get().width();
	double height = Size.get().height();
	double posX   = Position.get().x();
	double posY   = Position.get().y();
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

	Size = Pt::Gfx::SizeF(width,height);
	Position = Pt::Gfx::PointF(posX, posY);
	_lastSizePoint = p;
	render();	
}

void Panel::onPointerInput(const PointingEvent& ev)
{
	Pt::Gfx::PointF p =  toClient(Pt::Gfx::PointF(ev.x(), ev.y()));
	Pt::Gfx::SizeF size = Size.get();
	
	double sizeR = size.width() -  BorderWidth.get();
	double sizeB = size.height() - BorderWidth.get();
			
	switch(BorderStyle.get())
	{
		case BorderStyleType::Sizeable:
		{
			if( contains(p) )
			{
				if(p.x() < BorderWidth.get() && p.y() <  BorderWidth.get())
				{//Corner NW
					CursorT.get().setCursor(Cursors::SizeNS);
					_resizeDir = NorthWest;
				}	
				else if(p.x() > sizeR && p.y() <  BorderWidth.get())
				{//corner NE
					CursorT.get().setCursor(Cursors::SizeNS);
					_resizeDir= NorthEast;
				}
				else if(p.x() < BorderWidth.get() &&  p.y() > sizeB )
				{//corner SW
					CursorT.get().setCursor(Cursors::SizeWE);
					_resizeDir = SouthWest;
				}
				else if(p.x() > sizeR &&  p.y() > sizeB )
				{//corner SE
					CursorT.get().setCursor(Cursors::SizeWE);
					_resizeDir = SouthEast;
				}
				else
				{
					if( p.x() < BorderWidth.get())				
					{//West
						CursorT.get().setCursor(Cursors::SizeWE);
						_resizeDir = West;
					}
					else if(p.x() >= sizeR)
					{//East
						CursorT.get().setCursor(Cursors::SizeWE);
						_resizeDir = East;
					}
					else if( p.y() < BorderWidth.get())
					{//North
						CursorT.get().setCursor(Cursors::SizeNS);
						_resizeDir = North;
					}
					else if(p.y() >sizeB)
					{//South
						CursorT.get().setCursor(Cursors::SizeNS);
						_resizeDir = South;
					}
					else
					{
						CursorT.get().setCursor(Cursors::Default);
					}
				}
			}
      else
      {
          CursorT.get().setCursor(Cursors::Default);
      }
			handleResize(ev);
		}
		break;
            
		default:
          
		break;
	}
		
	Widget::onPointerInput(ev);			
}



void Panel::onRender()
{	
	Widget::onRender();

	if(!Visible.get())
		return;

	int corner = 0;

	if(BorderRoundEdge.get())
		corner = 2;
    
	size_t border =  (size_t) BorderWidth.get();	
	Pt::Gfx::SizeF  clientSize(Size.get().width() - BorderWidth.get()/2, Size.get().height() - BorderWidth.get()/2);	
	Pt::Gfx::RectF  clientRect(Pt::Gfx::PointF( BorderWidth.get()/2, BorderWidth.get()/2), clientSize);
	
	Pt::Hmi::Painter& localPainter = paintSurface().painter();
						
	switch(BorderStyle.get())
	{
		case BorderStyleType::Single:
		{			
			std::vector<Pt::Gfx::PointF> points1(5);
			std::vector<Pt::Gfx::PointF> points2(5);

			//P0
			points1[0].setX(corner);
			points1[0].setY(clientRect.height());

			//P1
			points1[1].setX(0);
			points1[1].setY(clientRect.height() - corner);

			//P2
			points1[2].setX(0);
			points1[2].setY(corner);

			//P3
			points1[3].setX(corner);
			points1[3].setY(0);

			//P4
			points1[4].setX(clientRect.width() - corner);
			points1[4].setY(0);
			
			//---
			//P0
			points2[0].setX(clientRect.width() - corner);
			points2[0].setY(0);

			//P1
			points2[1].setX(clientRect.width());
			points2[1].setY(corner);

			//P2
			points2[2].setX(clientRect.width());
			points2[2].setY(clientRect.height() - corner);

			//P3
			points2[3].setX(clientRect.width() - corner);
			points2[3].setY(clientRect.height());

			//P4
			points2[4].setX(corner);
			points2[4].setY(clientRect.height());
		
			Pt::Gfx::Pen pen(1, Pt::Gfx::ARgbColor(0,178,178,178));
			localPainter.setPen(pen);
				
			localPainter.drawPolyline(&points1[0], points1.size());								
			localPainter.drawPolyline(&points2[0], points2.size());
		}

		break;
			
		case BorderStyleType::Widget:
		{			
			std::vector<Pt::Gfx::PointF> points1(5);
			std::vector<Pt::Gfx::PointF> points2(5);

			//P0
			points1[0].setX(corner);
			points1[0].setY(clientRect.height());

			//P1
			points1[1].setX(0);
			points1[1].setY(clientRect.height() - corner);

			//P2
			points1[2].setX(0);
			points1[2].setY(corner);

			//P3
			points1[3].setX(corner);
			points1[3].setY(0);

			//P4
			points1[4].setX(clientRect.width() - corner);
			points1[4].setY(0);
			
			//---
			//P0
			points2[0].setX(clientRect.width() - corner);
			points2[0].setY(0);

			//P1
			points2[1].setX(clientRect.width());
			points2[1].setY(corner);

			//P2
			points2[2].setX(clientRect.width());
			points2[2].setY(clientRect.height() - corner);

			//P3
			points2[3].setX(clientRect.width() - corner);
			points2[3].setY(clientRect.height());

			//P4
			points2[4].setX(corner);
			points2[4].setY(clientRect.height());


			Pt::Gfx::Pen pen(border, Pt::Gfx::ARgbColor(0,178,178,178));
			localPainter.setPen(pen);
				
			localPainter.drawPolyline(&points2[0], points2.size());
                
			Pt::Gfx::Pen pen2(border, Pt::Gfx::ARgbColor(0,178,178,178));
			localPainter.setPen(pen2);
                
			localPainter.drawPolyline(&points1[0], points1.size());
            
		}
		break;

		case BorderStyleType::Border3D:
		{
			std::vector<Pt::Gfx::PointF> points1(3);
			std::vector<Pt::Gfx::PointF> points2(3);

			points1[0].setX(0);
			points1[0].setY(clientRect.height());

			points1[1].setX(0);
			points1[1].setY(0);
				
			points1[2].setX(clientRect.width());
			points1[2].setY(0);


			points2[0].setX(clientRect.width());
			points2[0].setY(0);

			points2[1].setX(clientRect.width());
			points2[1].setY(clientRect.height());

			points2[2].setX(0);
			points2[2].setY(clientRect.height());

		
			Pt::Gfx::Pen pen(border, Pt::Gfx::ARgbColor(0,255,255,255));
			localPainter.setPen(pen);
				
			localPainter.drawPolyline(&points1[0], points1.size());
								
			Pt::Gfx::Pen pen2(border, Pt::Gfx::ARgbColor(0,0,0,0));
			localPainter.setPen(pen2);

			localPainter.drawPolyline(&points2[0], points2.size());			
		}
		break;

		case BorderStyleType::Sizeable:
		{
			Pt::Gfx::Pen pen1(border, ForeColor.get());
			localPainter.setPen(pen1);				
			localPainter.drawRect(clientRect);
			
			Pt::Gfx::Pen pen2(1, Pt::Gfx::ARgbColor(0,255,255,255));
			localPainter.setPen(pen2);				
			localPainter.drawRect(clientRect);
		}
		break;

		default:
		break;			
	}	
}

}}
