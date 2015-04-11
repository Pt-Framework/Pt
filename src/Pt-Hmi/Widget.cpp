#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/WindowProperties.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Brush.h>

namespace Pt{
namespace Hmi{

Widget::Widget()
: _mnemonicWidget(0)
, PT_HMI_INIT_PROPERTY_VALUE(Enabled, true)
, PT_HMI_INIT_PROPERTY_VALUE(Visible, true )
, PT_HMI_INIT_PROPERTY_VALUE(Font,Pt::Gfx::Font("Sans Serif",12))
, PT_HMI_INIT_PROPERTY_VALUE(Position, Pt::Gfx::PointF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(Size, Pt::Gfx::SizeF(100,100))
, PT_HMI_INIT_PROPERTY_VALUE(BackColor,Pt::Gfx::ARgbColor(237,237,237))
, PT_HMI_INIT_PROPERTY_VALUE(HighlightColor,Pt::Gfx::ARgbColor(200,200,200))
, PT_HMI_INIT_PROPERTY_VALUE(ForeColor, Pt::Gfx::ARgbColor(0,0,0))
, PT_HMI_INIT_PROPERTY_VALUE(DisabledColor, Pt::Gfx::ARgbColor(178,178,178))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImage, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImageLayout, ImageLayout::NoLayout)
, PT_HMI_INIT_PROPERTY_VALUE(Opacity,0)
, PT_HMI_INIT_PROPERTY_VALUE(Cursor, Hmi::Cursor::defaultCursor())
, PT_HMI_INIT_PROPERTY_VALUE(TextAlign, Align::MidleCenter)
, PT_HMI_INIT_PROPERTY_VALUE(AcceptFocus,true)
, PT_HMI_INIT_PROPERTY_VALUE(HighLight, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocusedActionKey," ")
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(UseMnemonic,true)
, PT_HMI_INIT_PROPERTY_VALUE(Name,"Widget")
, PT_HMI_INIT_PROPERTY(Margin)
, PT_HMI_INIT_PROPERTY_VALUE(Dock, Docking::None)
, PT_HMI_INIT_PROPERTY_VALUE(FlowLayout, Hmi::FlowLayout::None)
, PT_HMI_INIT_PROPERTY_VALUE(FlowDirection, Hmi::FlowLayoutDirection::LeftToRightTopToBottom)
, PT_HMI_INIT_PROPERTY_VALUE(ShortcutKey, "")
, PT_HMI_INIT_PROPERTY_VALUE(Focused,true)
, _parent(0)
, _isValid(false)
, _containPointer(false)
{	
	bindMnemonicToWidget( *this );	
	
 	 Size.Changed += Pt::slot(*this, &Widget::onSizeChanged);				
 	 Caption.Changed += Pt::slot(*this, &Widget::onCaptionChanged);			
	Focused.Changed += Pt::slot( *this, &Widget::onFocusChanged );
	Cursor.Changed +=  Pt::slot( *this, &Widget::onCursorChanged );

	eventReceived() += Pt::slot(*this, &Widget::onKeyInput);
	eventReceived() += Pt::slot(*this, &Widget::onPointerInput);
}


Widget::~Widget()
{

}


void Widget::addChild(Widget* child)
{
	_children.push_back(child);
	child->setParent(this);
	invalidate();
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
		return globalPoint;

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


void Widget::updatePosAndSize(Widget& w, const Pt::Gfx::SizeF& s, const Pt::Gfx::PointF& p)
{
  bool visible  = w.Visible.get();
        
  w.Visible.set(false);  //Avoid invalidate.              
  w.Position = p;
  w.Size = s;  
	w._isValid = false;
  w.Visible.set(visible);
}


void Widget::mnemonic()
{
	onMnemonic();
}


void Widget::invalidate()
{  	
	_isValid = false;
	onInvalidate();
}


void Widget::onLayout()
{
  Pt::Hmi::Painter& localPainter = paintSurface().painter();
  Gfx::SizeF clientSize = paintSurface().size();
  double posLeft  = 0;
  double posTop   = 0;
  double posRight  = clientSize.width();
  double posBottom = clientSize.height();
  std::vector<Widget*> fillLayoutChildren;

	for( size_t i = 0; i < children().size(); ++i )
	{
		Widget*	child = _children[i];			
		 
		if( !child->Visible.get() )
				continue;
   
    Pt::Gfx::PointF point = child->Position.get();

    if( FlowLayout.get() == Hmi::FlowLayout::None )
    {
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
          Pt::Gfx::SizeF size( child->Size.get().width(), (posBottom - posTop));

					posLeft += child->Size.get().width();      
					        
          updatePosAndSize( *child, size, point ); 
        }
        break;

        case Docking::Top:
        {
          point.setX( posLeft );
          point.setY( posTop  );		    
					Pt::Gfx::SizeF size( (posRight - posLeft), child->Size.get().height());

				  posTop += child->Size.get().height();      
        
          updatePosAndSize(*child, size, point );
        }
        break;

        case Docking::Right:
        {
          posRight -= child->Size.get().width();     
          point.setX( posRight );
          point.setY( posTop );		                     
					
					Pt::Gfx::SizeF size( child->Size.get().width(), (posBottom - posTop) );
        
          updatePosAndSize(*child, size, point );
        }
        break;

        case Docking::Bottom:
        {
          posBottom -= child->Size.get().height();    
          point.setX( posLeft );
          point.setY( posBottom  );		                      
					Pt::Gfx::SizeF size( (posRight - posLeft), child->Size.get().height() );
          updatePosAndSize(*child, size, point);
        }
        break;

        case Docking::Fill:
        {
          fillLayoutChildren.push_back( child );
          continue;
        }      
      }
    }
    else
    {
      switch( FlowLayout.get() )
      {
        case Hmi::FlowLayout::Horizontal:
        {
                        
          if( FlowDirection.get() == FlowLayoutDirection::LeftToRightTopToBottom )
          {
            point.setX( posLeft );
            posLeft += child->Size.get().width() + child->Margin.get().left() + child->Margin.get().right();
          }
          else
          {
            posRight -= (child->Size.get().width() + child->Margin.get().left() + child->Margin.get().right() );
            point.setX( posRight );              
          }

          point.setY( 0 );	
          updatePosAndSize(*child, Pt::Gfx::SizeF( child->Size.get().width(), clientSize.height() ), point);
        }
        break;

        case Hmi::FlowLayout::Vertical:
        {
          point.setX( 0 );

          if( FlowDirection.get() == FlowLayoutDirection::LeftToRightTopToBottom )
          {
            point.setY( posTop );
            posTop += child->Size.get().height() + child->Margin.get().top() + child->Margin.get().bottom();	
          }
          else
          {
            posBottom -= child->Size.get().height();              
            point.setY( posBottom  );	
          }

          updatePosAndSize(*child, Pt::Gfx::SizeF( clientSize.width(), child->Size.get().height() ), point);
        }
        break;
      }
    }
		
    child->render();
    
    const double x = point.x() + child->Margin.get().left();
    const double y = point.y() + child->Margin.get().top();

    localPainter.drawSurface(Gfx::PointF(x, y) , child->paintSurface());
	}	

  if( fillLayoutChildren.size() != 0 )
  {
    Widget* child = fillLayoutChildren[0]; 
    Pt::Gfx::PointF point(posLeft, posTop);

    const double& width = posRight - posLeft;
    const double& height = posBottom - posTop;

    updatePosAndSize( *child, Pt::Gfx::SizeF( width, height ), point );
    child->render();

    const double& x = point.x() + child->Margin.get().left();
    const double& y = point.y() + child->Margin.get().top();

    point.setX(x);
    point.setY(y);
    
    localPainter.drawSurface(point, child->paintSurface());
  }
}


void Widget::render()
{
	if( ! Visible.get() )
		return;
	
	if( !_isValid )
		onRender();	

	onLayout();	

	_isValid = true;
}


void Widget::onRender()
{		
	if( ! Visible.get() )
		return;

	  Pt::Gfx::SizeF size = _paintSurface.size();

	if( size.width() < 0 ||  size.height() < 0)
		return; 

	Pt::Gfx::ARgbImage& backImage    = BackgroundImage.get();
	Pt::Hmi::Painter&	  localPainter = _paintSurface.painter();
	Pt::Gfx::RectF		  rect(Pt::Gfx::PointF(0,0),size);
	
	localPainter.setFont(Font.get());

	if( HighLight.get() )
	{       
		Pt::Gfx::Brush	brush(HighlightColor.get());
        
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
		parent()->invalidate();		
}


void Widget::onActionKey(KeyEvent::KeyState state)
{
}


void Widget::onShortcutKey(KeyEvent::KeyState state)
{
}


void Widget::onPointerEnter()
{
	_containPointer = true;
	Application::instance().setCursor( &Cursor.get() );
}


void Widget::onPointerLeaved()
{	
	_containPointer = false;
	Application::instance().setCursor( );
}

		
void Widget::onPointerInput(const PointerEvent& ev)
{	
	Pt::Gfx::PointF local = toClient( Pt::Gfx::PointF( ev.x(), ev.y() ) );
	
	if( ! Enabled.get() )
			return;
	
	if( !_containPointer )
	{
		if( contains( local ) )
			onPointerEnter();
	}
	else
	{
		if( !contains( local ) )
			onPointerLeaved();
	}

	for( size_t i = 0; i < children().size(); ++i)
		_children[i]->eventReceived().send(ev);
}


void Widget::onKeyInput(const KeyEvent& ev)
{ 	
	if( !Enabled.get() )
			return;

	//Mnemonic handling
	if( UseMnemonic.get() && Enabled.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp )
	{		
		std::string mnKey = "";

		if( ev.alt())
			mnKey = "A//";
			
		mnKey += ev.toUTF8String();

		 if(_mnemonicKey == mnKey)
		 {
				_mnemonicWidget->mnemonic();						
		 }
	}

	//Action key handling
	if( ev.toUTF8String() == FocusedActionKey.get() && Focused.get() )	
	{
		onActionKey( ev.state() );		
	}
	
	//Shortcurt Key
	if( ev.shortCutKey() == ShortcutKey.get() )
	{
		onShortcutKey( ev.state() );		
	}

	//Propagate to children.
	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->onKeyInput(ev);
}


void Widget::bindMnemonicToWidget(Widget& widget)
{
	_mnemonicWidget = &widget;
}


void Widget::onMnemonic()
{	
	Focused = true;
}


bool Widget::contains(const Pt::Gfx::PointF& p)
{
	if( p.x()  < Size.get().width() && p.x() >= 0 && p.y() < Size.get().height() && p.y() >= 0)
			return true;
 
	return false;
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
		if( child->Focused.get() )
			return i;		
	}		

	return -1;
}


bool Widget::focusPrev()
{
	if( children().size() == 0 )
		return false;

	int index = getFocusedChild();

	if( index == -1)
		return focusPrevChild( children().size() );
	
	Widget* child  = children()[index];

	if(!child->AcceptFocus.get())
	{
		if(child->focusPrev())
			return true;
	}

	child->Focused = false;
	return focusPrevChild(index);
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

		invalidate();
    return;
	}

	if( parent() == 0 )
	{
		invalidate();
    return;
	}
		
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

	invalidate();
}


std::string Widget::removeMnemonic(const std::string& text)
{
	std::string removed;

	for( size_t i = 0; i < text.size(); ++i )
	{				
		if( text[i] != '&')
		{		
			removed += text[i];	
			continue;
		}
		
		if( (i + 1) == text.size() )
			continue;

		if(text[i+1] != '&')
			continue;

		removed += text[i];
		++i;
	}

	return removed;		
}


size_t Widget::getMnemonicIndex(const std::string& text)
{	 
	size_t pos = 0;

	for( size_t i = 0; i < text.size() - 1; ++i)
	{				
		if( text[i] == '&'  && text[i +1] =='&' )
			++i;
		else if (text[i] == '&'  && 	text[i +1] !='&')
			return pos;

		pos++;
	}

	return std::string::npos;
}


void Widget::onCaptionChanged(const Property<std::string>& prop)
{
	_mnemonicKey.clear();

	if( !UseMnemonic.get() )
		return;  
	
	int index = getMnemonicIndex(prop.get());

	if( index == std::string::npos )
		return;
					
	std::string unescaped = Widget::removeMnemonic(prop.get());

	_mnemonicKey = "A//";	
	_mnemonicKey += std::tolower(unescaped[index]);
}


void Widget::onSizeChanged(const Property<Pt::Gfx::SizeF>& prop)
{
  const double  width  = prop.get().width() - ( Margin.get().left() + Margin.get().right() );
  const double  height = prop.get().height() - ( Margin.get().top() + Margin.get().bottom() );

	paintSurface().resize( Gfx::SizeF(width,  height ) );	
	
	if( Visible.get() )
		invalidate();
}

void Widget::onCursorChanged(const Property<Hmi::Cursor>& prop)
{
	if( _containPointer )
		Application::instance().setCursor( &Cursor.get() );	
}

}} //namespace
