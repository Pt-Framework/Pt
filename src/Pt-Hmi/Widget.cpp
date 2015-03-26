#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Brush.h>

namespace Pt{
namespace Hmi{

Widget::Widget()
: _mnemonicWidget(0)
, PT_HMI_INIT_PROPERTY_VALUE(Enabled,true)
, PT_HMI_INIT_PROPERTY_VALUE(Visible,true)
, PT_HMI_INIT_PROPERTY_VALUE(Font,Pt::Gfx::Font("Sans Serif",12))
, PT_HMI_INIT_PROPERTY_VALUE(Position, Pt::Gfx::PointF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(Size, Pt::Gfx::SizeF(100,100))
, PT_HMI_INIT_PROPERTY_VALUE(BackColor,Pt::Gfx::ARgbColor(237,237,237))
, PT_HMI_INIT_PROPERTY_VALUE(BackColorHightLight,Pt::Gfx::ARgbColor(200,200,200))
, PT_HMI_INIT_PROPERTY_VALUE(ForeColor, Pt::Gfx::ARgbColor(70,70,70))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImage, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImageLayout, ImageLayout::NoLayout)
, PT_HMI_INIT_PROPERTY_VALUE(Opacity,0)
, PT_HMI_INIT_PROPERTY(CursorT)
, PT_HMI_INIT_PROPERTY_VALUE(TextAlign, Align::MidleCenter)
, PT_HMI_INIT_PROPERTY_VALUE(Focused, false)
, PT_HMI_INIT_PROPERTY_VALUE(AcceptFocus,true)
, PT_HMI_INIT_PROPERTY_VALUE(HighLight, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocusedActionKey," ")
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(UseMnemonic,true)
, PT_HMI_INIT_PROPERTY_VALUE(Name,"Widget")
, PT_HMI_INIT_PROPERTY(Margin)
, PT_HMI_INIT_PROPERTY_VALUE(Dock, Docking::None)
, _parent(0)
{
	Focused.Changed += Pt::slot( *this, &Widget::onFocusChanged );
  Size.Changed += Pt::slot(*this, &Widget::onSizeChanged);				
  Caption.Changed += Pt::slot(*this, &Widget::onCaptionChanged);		
}


Widget::~Widget()
{
}


void Widget::addChild(Widget* child)
{
	_children.push_back(child);
	child->setParent(this);
}


void Widget::removeChild(Widget* child)
{
	for(size_t i = 0; i < _children.size(); ++i)
	{
		if(_children[i] != child)
			continue;
		
		_children.erase(_children.begin() + i);
		child->setParent(0);
		return;
	}			
}	


Pt::Gfx::PointF Widget::toClient( const Pt::Gfx::PointF& globalPoint )
{
	if( _parent == 0 )
		return Pt::Gfx::PointF( globalPoint.x(), globalPoint.y() );

	Pt::Gfx::PointF parPoint = _parent->toClient( globalPoint );
	return Pt::Gfx::PointF( parPoint.x() - Position.get().x(), parPoint.y() - Position.get().y() );
}


Pt::Gfx::PointF Widget::fromClient( const Pt::Gfx::PointF& localPoint, bool toRoot )
{
	double x = localPoint.x();
	double y = localPoint.y();
	const Widget* widget = _parent;

	while( widget != 0 )
	{		
		widget = widget->parent();

		if(!(toRoot && widget == 0))
		{
			x += widget->Position.get().x();
			y += widget->Position.get().y();
		}
	}	
	return Pt::Gfx::PointF(x,y);
}

void Widget::updatePosSizeNoInval(Widget& w, const Pt::Gfx::SizeF& s, const Pt::Gfx::PointF& p)
{
  bool visible  = w.Visible.get();
        
  w.Visible.set(false);                
  w.Position = p;
  w.Size = s;  
  w.Visible.set(visible);
}


void Widget::mnemonic()
{
	onMnemonic();
}


void Widget::invalidate()
{
	onInvalidate();
}


void Widget::render()
{
	if( ! Visible.get() )
		return;
	
	onRender();

	Pt::Hmi::Painter& localPainter = _paintSurface.painter();
  double posLeft  = 0;
  double posTop   = 0;
  double posRight  = Size.get().width();
  double posBottom = Size.get().height();
  std::vector<Widget*> fillChilds;

	for( size_t i = 0; i < children().size(); ++i )
	{
		Widget*	child = _children[i];			
		    
    Pt::Gfx::PointF point = child->Position.get();

    switch(child->Dock.get())
    {
      case Docking::None:        
      {
        point.setX( point.x() );
        point.setY( point.y() );		    
      }
      break;

      case Docking::Left:
      {
        point.setX( posLeft );
        point.setY( posTop );		    
        posLeft += child->Size.get().width();      
        
        updatePosSizeNoInval( *child, Pt::Gfx::SizeF( child->Size.get().width(), Size.get().height() ), point ); 
      }
      break;

      case Docking::Top:
      {
        point.setX( posLeft );
        point.setY( posTop  );		    
        posTop += child->Size.get().height();      
        
        updatePosSizeNoInval(*child, Pt::Gfx::SizeF(  Size.get().width(), child->Size.get().height() ), point );
      }
      break;

      case Docking::Right:
      {
        posRight -= child->Size.get().width();     
        point.setX( posRight );
        point.setY( posTop );		                     
        
        updatePosSizeNoInval(*child, Pt::Gfx::SizeF(  child->Size.get().width(), Size.get().height() - posTop ), point );
      }
      break;

      case Docking::Bottom:
      {
        posBottom -= child->Size.get().height();    
        point.setX( posLeft );
        point.setY( posBottom  );		              
        
        double width = posRight - posLeft;
        updatePosSizeNoInval(*child, Pt::Gfx::SizeF( width, child->Size.get().height() ), point);
      }
      break;

      case Docking::Fill:
      {
        fillChilds.push_back(child);
        continue;
      }      
    }

    child->render();
    localPainter.drawSurface(point, child->paintSurface());
	}	

  if( fillChilds.size() != 0 )
  {
    Widget* child = fillChilds[0]; 
    Pt::Gfx::PointF point(posLeft, posTop);

    const double& width = posRight - posTop;
    const double& height = posBottom - posTop;
    updatePosSizeNoInval( *child, Pt::Gfx::SizeF( width, height ), point );

    child->render();
    localPainter.drawSurface(point, child->paintSurface());
  }
}



void Widget::onRender()
{		
	if( ! Visible.get() )
		return;

  Pt::Gfx::SizeF size = Size.get();
  size.setWidth( size.width() );
	if( Size.get().width() < 0 ||  Size.get().height() < 0)
		return;

	Pt::Gfx::SizeF bufferSize = _paintSurface.size();

	//ToDo: move this check to surface resize.
	if(bufferSize.width() != size.width() ||bufferSize.height() != size.height())
		_paintSurface.resize(size);

	Pt::Gfx::ARgbImage& backImage = BackgroundImage.get();
	Pt::Hmi::Painter&	localPainter = _paintSurface.painter();
	Pt::Gfx::RectF		rect(Pt::Gfx::PointF(0,0),size);
	
	localPainter.setFont(Font.get());

	if(HighLight.get())
	{       
		Pt::Gfx::Brush	brush(BackColorHightLight.get());
        
		localPainter.setBrush(brush);
		localPainter.fillRect(rect);
	}
	else
	{
		Pt::Gfx::Brush	brush(BackColor.get());
	
		localPainter.setBrush(brush);
    
		localPainter.fillRect(rect);
	}

	if( backImage.width() != 0 && backImage.height() != 0)
	{
		switch( BackgroundImageLayout.get())
		{				
			case ImageLayout::NoLayout:
			{
				localPainter.drawImage(Pt::Gfx::PointF(0,0), backImage);
			}
			break;
			
			case ImageLayout::Tile:
			{
				for( size_t x = 0; x < _paintSurface.size().width();  x += backImage.width())
				{
					for( size_t y = 0; y < _paintSurface.size().height();  y += backImage.height())
						localPainter.drawImage(Pt::Gfx::PointF(x,y), backImage);
				}
			}
			break;

			case ImageLayout::Center:
			{
				double x = size.width()/2  - backImage.width()/2;
				double y = size.height()/2  - backImage.height()/2;
				localPainter.drawImage(Pt::Gfx::PointF(x,y), backImage);
			}
			break;
			
			case ImageLayout::Strech:
			{
				Pt::Gfx::ARgbImage strech((size_t) _paintSurface.size().width(), (size_t)_paintSurface.size().height() );

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(), strech.begin(), (size_t) _paintSurface.size().width(),  (size_t) _paintSurface.size().height());
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;

			case ImageLayout::Zoom:
			{
				Pt::Gfx::ARgbImage strech( (size_t) _paintSurface.size().width(), (size_t) _paintSurface.size().height() );
				double factor = _paintSurface.size().width()/(double)backImage.width();

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(),strech.begin(),  strech.width(), (Pt::size_t)(backImage.height()*factor));
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;
		}
	}	
}


void Widget::onInvalidate()
{
	if( parent() )
		parent()->onInvalidate();		
}




void Widget::onKeyInput(const KeyEvent& ev)
{ 	
	if(UseMnemonic.get() && _mnemonicWidget != 0 && Enabled.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp)
	{		
		std::string mnKey = "";

		if( ev.alt())
			mnKey = "A//";
			
		mnKey += ev.toUTF8String();

		 if(_mnemonicKey == mnKey)
			_mnemonicWidget->mnemonic();			
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->onKeyInput(ev);
}


void Widget::onPointerInput(const PointingEvent& ev)
{
	for( size_t i = 0; i < children().size(); ++i)
		_children[i]->onPointerInput(ev);
}


void Widget::bindMnemonicToWidget(Widget* widget)
{
	_mnemonicWidget = widget;
}


void Widget::onMnemonic()
{	
		Focused = true;
}


bool Widget::contains(const Pt::Gfx::PointF& p)
{
	double x1, x2, y1, y2;
	
	x1 = 0;
	x2 = x1  + Size.get().width();

	y1 = 0;
	y2 = y1  + Size.get().height();
	
	return ((p.x() >= x1) && (p.x() < x2) && (p.y() >= y1) && (p.y() < y2));
}


bool Widget::focusPrevChild(int index)
{
	index--;
	
	for( ; index >= 0; --index)
	{
		Widget* child = children()[index];

		if(child->AcceptFocus.get())
		{
			child->Focused = true;
			return true;
		}

		if(child->focusPrev())
		{
			child->Focused = true;
			return true;
		}
	}

	return false;
}



bool Widget::focusNextChild( int index )
{
	index++;
	
	for( ; index < (int)children().size(); ++index )
	{
		Widget* child = children()[index];

		if(child->AcceptFocus.get())
		{
			child->Focused = true;
			return true;
		}

		if(child->focusNext())
		{
			child->Focused = true;
			return true;
		}
	}

	return false;
}


int Widget::getFocusedChild() const
{
	int i = 0;
	
	for( ; i < (int)children().size(); ++i)
	{
		const Widget* child = children()[i];
		if(child->Focused.get())
			return i;		
	}		

	return -1;
}


bool Widget::focusPrev()
{
	if(children().size() == 0)
		return false;

	int index = getFocusedChild();

	if( index != -1)
	{
		Widget* child  = children()[index];

		if(!child->AcceptFocus.get())
		{
			if(child->focusPrev())
				return true;
		}

		child->Focused = false;
		return focusPrevChild(index);
	}
	
	return focusPrevChild(children().size());
}


bool Widget::focusNext()
{
	if( children().size() == 0 )
		return false;

	const int index = getFocusedChild();

	if( index == -1)
		return focusNextChild(index);
	
	Widget* child = children()[index];
		
	if(!child->AcceptFocus.get())
	{
		if(child->focusNext())
			return true;
	}

	child->Focused = false;

	return focusNextChild(index);
}


void Widget::onFocusChanged( const Property<bool>& prop )
{
  if( !Focused.get() )
	{//False => all childs set to false.
		for( size_t i = 0; i < children().size(); ++i)
		{
			Pt::Hmi::Widget* child = children()[i];
			child->Focused.set(false);						
			child->Focused.Changed.send(child->Focused);
		}
    return;
	}

	if( parent() == 0 )
    return;
		
	//Set parent focused.
	parent()->Focused.set(true);
	parent()->Focused.Changed.send(parent()->Focused);

	//All sibling set to false. Only me let it true
	for( size_t i = 0; i < parent()->children().size(); i++ )
	{
		Widget* child = parent()->children()[i];
				
		if( child != this )
			child->Focused = false;
	}
}


void Widget::onCaptionChanged(const Property<std::string>& prop)
{
  _mnemonicKey.clear();

	int index = prop.get().find('&');
	
	if( index < 0 || ((index + 1) > (int) prop.get().size()))
		return;

	_mnemonicKey = "A//";	
	_mnemonicKey+= std::tolower(prop.get()[index + 1]);
}


void Widget::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{
	paintSurface().resize( prop.get() );	
	
	if( Visible.get() )
		invalidate();
}


}} //namespace
