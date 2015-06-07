#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/WindowProperties.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Ui/Brush.h>

namespace Pt{
namespace Hmi{

Widget::Widget()
: _mnemonicWidget(0)
, PT_HMI_INIT_PROPERTY_VALUE(Enabled, true)
, PT_HMI_INIT_PROPERTY_VALUE(Visible, true )
, PT_HMI_INIT_PROPERTY_VALUE(Font,Ui::Font("Sans Serif",12))
, Position("Position", *this, &Widget::position, &Widget::setPosition )
, Size("Size", *this, &Widget::size, &Widget::setSize)
, PT_HMI_INIT_PROPERTY_VALUE(BackColor,Ui::Color(237/255.0,237/255.0,237/255.0))
, PT_HMI_INIT_PROPERTY_VALUE(HighlightColor,Ui::Color(200/255.0,200/255.0,200/255.0))
, PT_HMI_INIT_PROPERTY_VALUE(ForeColor, Ui::Color(0,0,0))
, PT_HMI_INIT_PROPERTY_VALUE(DisabledColor, Ui::Color(178/255.0,178/255.0,178/255.0))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImage, Ui::Image(1,1))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImageLayout, ImageLayout::NoLayout)
, PT_HMI_INIT_PROPERTY_VALUE(Opacity,0)
, PT_HMI_INIT_PROPERTY_VALUE(Cursor, Hmi::Cursor::defaultCursor())
, PT_HMI_INIT_PROPERTY_VALUE(TextAlign, Align::MidleCenter)
, PT_HMI_INIT_PROPERTY_VALUE(AcceptFocus,true)
, PT_HMI_INIT_PROPERTY_VALUE(HighLight, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocusedActionKey," ")
, Caption("Caption", *this, &Widget::caption, &Widget::setCaption )
, PT_HMI_INIT_PROPERTY_VALUE(UseMnemonic,true)
, PT_HMI_INIT_PROPERTY_VALUE(Name,"Widget")
, PT_HMI_INIT_PROPERTY(Margin)
, PT_HMI_INIT_PROPERTY_VALUE(Dock, Docking::None)
, PT_HMI_INIT_PROPERTY_VALUE(FlowLayout, Hmi::FlowLayout::None)
, PT_HMI_INIT_PROPERTY_VALUE(FlowDirection, Hmi::FlowLayoutDirection::LeftToRightTopToBottom)
, PT_HMI_INIT_PROPERTY_VALUE(ShortcutKey, "")
, _parent(0)
, _isValid(false)
, _containPointer(false)
, _size(200,200)
, _position( 0, 0)
, _isFocused(false)
{	
	bindMnemonicToWidget( *this );	
	
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


Ui::PointF Widget::toClient( const Ui::PointF& globalPoint )
{
	if( _parent == 0 )
		return globalPoint;

	Ui::PointF parPoint = _parent->toClient( globalPoint );
	return Ui::PointF( parPoint.x() - Position.get().x(), parPoint.y() - Position.get().y() );
}


Ui::PointF Widget::fromClient( const Ui::PointF& localPoint, bool toRoot )
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
	return Ui::PointF(x,y);
}


void Widget::updatePosAndSize(Widget& w, const Ui::SizeF& s, const Ui::PointF& p)
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
  Ui::SizeF clientSize = paintSurface().size();
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
   
    Ui::PointF point = child->Position.get();

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
          Ui::SizeF size( child->Size.get().width(), (posBottom - posTop));

					posLeft += child->Size.get().width();      
					        
          updatePosAndSize( *child, size, point ); 
        }
        break;

        case Docking::Top:
        {
          point.setX( posLeft );
          point.setY( posTop  );		    
					Ui::SizeF size( (posRight - posLeft), child->Size.get().height());

				  posTop += child->Size.get().height();      
        
          updatePosAndSize(*child, size, point );
        }
        break;

        case Docking::Right:
        {
          posRight -= child->Size.get().width();     
          point.setX( posRight );
          point.setY( posTop );		                     
					
					Ui::SizeF size( child->Size.get().width(), (posBottom - posTop) );
        
          updatePosAndSize(*child, size, point );
        }
        break;

        case Docking::Bottom:
        {
          posBottom -= child->Size.get().height();    
          point.setX( posLeft );
          point.setY( posBottom  );		                      
					Ui::SizeF size( (posRight - posLeft), child->Size.get().height() );
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
          updatePosAndSize(*child, Ui::SizeF( child->Size.get().width(), clientSize.height() ), point);
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

          updatePosAndSize(*child, Ui::SizeF( clientSize.width(), child->Size.get().height() ), point);
        }
        break;
      }
    }
		
    child->render();
    
    const double x = point.x() + child->Margin.get().left();
    const double y = point.y() + child->Margin.get().top();

    localPainter.drawSurface(Ui::PointF(x, y) , child->paintSurface());
	}	

  if( fillLayoutChildren.size() != 0 )
  {
    Widget* child = fillLayoutChildren[0]; 
    Ui::PointF point(posLeft, posTop);

    const double& width = posRight - posLeft;
    const double& height = posBottom - posTop;

    updatePosAndSize( *child, Ui::SizeF( width, height ), point );
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

	  Ui::SizeF size = _paintSurface.size();

	if( size.width() < 0 ||  size.height() < 0)
		return; 

	Ui::Image& backImage    = BackgroundImage.get();
	Pt::Hmi::Painter&	  localPainter = _paintSurface.painter();
	Ui::RectF		  rect(Ui::PointF(0,0),size);
	
	localPainter.setFont(Font.get());

	if( HighLight.get() )
	{       
		Ui::Brush	brush(HighlightColor.get());
        
		localPainter.setBrush(brush);
		localPainter.fillRect(rect);
	}
	else
	{
		Ui::Brush	brush(BackColor.get());
	
		localPainter.setBrush(brush);    	
		localPainter.fillRect(rect);
	}

	if( backImage.width() != 0 && backImage.height() != 0)
	{
		switch( BackgroundImageLayout.get())
		{				
			case ImageLayout::NoLayout:
			{
				localPainter.drawImage(Ui::PointF(0,0), backImage);
			}
			break;
			
			case ImageLayout::Tile:
			{
				for( size_t x = 0; x < _paintSurface.size().width();  x += backImage.width())
				{
					for( size_t y = 0; y < _paintSurface.size().height();  y += backImage.height())
						localPainter.drawImage(Ui::PointF(x,y), backImage);
				}
			}
			break;

			case ImageLayout::Center:
			{
				double x = size.width()/2  - backImage.width()/2;
				double y = size.height()/2  - backImage.height()/2;
				localPainter.drawImage(Ui::PointF(x,y), backImage);
			}
			break;
			
			case ImageLayout::Strech:
			{
				Ui::Image strech((size_t) _paintSurface.size().width(), (size_t)_paintSurface.size().height() );

//				Ui::blockScale(backImage.begin(), backImage.width(), backImage.height(), strech.begin(), (size_t) _paintSurface.size().width(),  (size_t) _paintSurface.size().height());
				localPainter.drawImage(Ui::PointF(0,0), strech);
			}
			break;

			case ImageLayout::Zoom:
			{
				Ui::Image strech( (size_t) _paintSurface.size().width(), (size_t) _paintSurface.size().height() );
				double factor = _paintSurface.size().width()/(double)backImage.width();

//				Ui::blockScale(backImage.begin(), backImage.width(), backImage.height(),strech.begin(),  strech.width(), (Pt::size_t)(backImage.height()*factor));
				localPainter.drawImage(Ui::PointF(0,0), strech);
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
  if( _parent != 0 )
	  Application::instance().setCursor( &_parent->Cursor.get() ); 
}

		
void Widget::onPointerInput(const PointerEvent& ev)
{	
	Ui::PointF local = toClient( Ui::PointF( ev.x(), ev.y() ) );
	
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
	if( ev.toUTF8String() == FocusedActionKey.get() && isFocused() )	
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
	setFocus( true );
}


bool Widget::contains(const Ui::PointF& p)
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
			child->setFocus(true);
			return true;
		}

		if(child->focusPrev())
		{
			child->setFocus(true);
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
			child->setFocus(true);
			return true;
		}

		if(child->focusNext())
		{
			child->setFocus(true);
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
		if( child->isFocused() )
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

	child->setFocus(false);
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

	child->setFocus(false);
	return focusNextChild(index);
}


void Widget::setFocus( bool isFocused )
{	
	_isFocused = isFocused;

  if( !_isFocused )
	{//False => all childs set to false.
		for( size_t i = 0; i < children().size(); ++i)
		{
			Pt::Hmi::Widget* child = children()[i];			
			child->setFocus(false);									
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
	parent()->setFocus(true);

	//All sibling set to false. Only me let it true
	for( size_t i = 0; i < parent()->children().size(); i++ )
	{
		Widget* child = parent()->children()[i];
				
		if( child != this )
			child->setFocus( false );
	}

	invalidate();

	Focused.send(_isFocused);
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


void Widget::setCaption( const std::string& c )
{
	_caption = c;

	_mnemonicKey.clear();

	if( !UseMnemonic.get() )
		return;  
	
	int index = getMnemonicIndex(_caption);

	if( index == std::string::npos )
		return;
					
	std::string unescaped = Widget::removeMnemonic(_caption);

	_mnemonicKey = "A//";	
	_mnemonicKey += std::tolower(unescaped[index]);
}


void Widget::setSize(const Ui::SizeF& size)
{
	_size = size;			

	const double  width  = _size.width() -  ( Margin.get().left() + Margin.get().right() );
	const double  height = _size.height() - ( Margin.get().top() + Margin.get().bottom() );

	paintSurface().resize( Ui::SizeF(width,  height ) );	
	
	if( Visible.get() )
		invalidate();
			
}


}} //namespace
