/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Brush.h>

namespace Pt{
namespace Hmi{


Widget::Widget()
: _enabled( true)
, _visible( false)
, _highlight( false)
, _font("Sans Serif",12)
, _backgroundColor(Gfx::Color::fromRgb8(237,237,237))
, _highlightColor(  Gfx::Color::fromRgb8(200,200,200) )
, _foregroundColor( Gfx::Color::fromRgb8(0,0,0) )
, _disabledColor( Gfx::Color::fromRgb8(178,178,178))
, _backgroundImage()
, _backgroundImageLayout(  ImageLayout::NoLayout )
, _cursor( Hmi::Cursor::defaultCursor() )
, _textAlign( Align::MidleCenter )
, _acceptFocus( true) 
, _focusedActionKey( "")
, _caption("")
, _name("")
, _margin(0)
, _dock(Docking::None )
, _flowLayout( FlowLayout::None )
, _flowDirection( FlowLayoutDirection::LeftToRightTopToBottom)
, _shortcutKey("")
, _hasFocus( false)
, _useMnemonic( false)
, _mnemonicKey("")
, _size( 100, 100)
, _position( 10,10) 
, _mnemonicWidget(0)
, _parent(0)
, _isValid(false)
{	

  bindMnemonicToWidget( *this );

	_eventReceived += Pt::slot(*this, &Widget::onKeyEvent);
	_eventReceived += Pt::slot(*this, &Widget::onPointerEvent);			
}


Widget::~Widget()
{
}


void Widget::addChild(Widget* child)
{
	_children.push_back(child);
	child->setParent(this);
	child->_isValid = false;
	child->setVisible( true);  
	child->invalidate();  
	invalidate();
}


void Widget::removeChild(Widget* child)
{
	for(size_t i = 0; i < _children.size(); ++i)
	{
		if(_children[i] != child)
			continue;
		
		_children.erase(_children.begin() + i);
		child->setVisible( false);
		child->setParent(0);
		return;
	}			

  invalidate();
}	


Gfx::PointF Widget::toClient( const Gfx::PointF& globalPoint )
{
	if( _parent == 0 )
		return globalPoint;

	Gfx::PointF parPoint = _parent->toClient( globalPoint );
	return Gfx::PointF( parPoint.x() - position().x(), parPoint.y() - position().y() );
}

 
Gfx::PointF Widget::fromClient( const Gfx::PointF& localPoint )
{
	double x = localPoint.x();
	double y = localPoint.y();
	const Widget* widget = parent();
	

	while( widget != 0  &&  widget->parent() != 0 )
	{						
		x += widget->position().x();
		y += widget->position().y();	

    	widget = widget->parent();
	}	

	return Gfx::PointF(x,y);
}


void Widget::updatePosAndSize(Widget& w, const Gfx::SizeF& s, const Gfx::PointF& p)
{       
  w.setPosition(p);
  w.setSize(s);      	
  _isValid = false;
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


void Widget::onLayout( PaintSurface& surface )
{
 Gfx::SizeF clientSize = surface.size();
  double posLeft  = 0;
  double posTop   = 0;
  double posRight  = clientSize.width();
  double posBottom = clientSize.height();
  std::vector<Widget*> fillLayoutChildren;

	for( size_t i = 0; i < children().size(); ++i )
	{
		Widget*	child = _children[i];			
		    
   Gfx::PointF point = child->position();

    if( flowLayout() == Hmi::FlowLayout::None )
    {
      switch( child->dock() )
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
         Gfx::SizeF size( child->size().width(), (posBottom - posTop));

					posLeft += child->size().width();      
					        
          updatePosAndSize( *child, size, point );           
        }
        break;

        case Docking::Top:
        {
          point.setX( posLeft );
          point.setY( posTop  );		    
					Gfx::SizeF size( (posRight - posLeft), child->size().height());

				  posTop += child->size().height();      
        
          updatePosAndSize(*child, size, point );
        }
        break;

        case Docking::Right:
        {
          posRight -= child->size().width();     
          point.setX( posRight );
          point.setY( posTop );		                     
					
					Gfx::SizeF size( child->size().width(), (posBottom - posTop) );
        
          updatePosAndSize(*child, size, point );
        }
        break;

        case Docking::Bottom:
        {
          posBottom -= child->size().height();    
          point.setX( posLeft );
          point.setY( posBottom  );		                      
					Gfx::SizeF size( (posRight - posLeft), child->size().height() );
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
      switch( flowLayout() )
      {
        case Hmi::FlowLayout::Horizontal:
        {                        
          if( flowLayout() == FlowLayoutDirection::LeftToRightTopToBottom )
          {
            point.setX( posLeft );
            posLeft += child->size().width();
          }
          else
          {
            posRight -= child->size().width();
            point.setX( posRight );              
          }

          point.setY( 0 );	
          updatePosAndSize( *child,Gfx::SizeF( child->size().width(), clientSize.height() ), point );
        }
        break;

        case Hmi::FlowLayout::Vertical:
        {
          point.setX( 0 );

          if( flowDirection() == FlowLayoutDirection::LeftToRightTopToBottom )
          {
            point.setY( posTop );
            posTop += child->size().height();	
          }
          else
          {
            posBottom -= child->size().height(); 
            point.setY( posBottom  );	
          }

          updatePosAndSize( *child,Gfx::SizeF( clientSize.width(), child->size().height() ), point );
        }
        break;
      }
    }		        
	}	

  if( fillLayoutChildren.size() != 0 )
  {
    Widget* child = fillLayoutChildren[0]; 
   Gfx::PointF point(posLeft, posTop);

    const double width  = posRight - posLeft;
    const double height = posBottom - posTop;

    updatePosAndSize( *child,Gfx::SizeF( width, height ), point );
  }
}


void Widget::render()
{	
	if( !visible())
		return;

	//Layout children
	onLayout( surface() );	

	//Render       
	if( !_isValid )
	{
   	surface().clear();
	  onRender( surface() );		  
	}

	//Render children
	for( size_t i = 0; i < _children.size(); ++i )
	{
		Widget* child = _children[i];	
		
    	_children[i]->render();   

	   surface().painter().drawSurface( child->position(), _children[i]->surface() );
	}
		
	_isValid = true;
}


void Widget::onRender( PaintSurface& surface )
{		
  const Gfx::SizeF size = clientSize();

	if( size.width() < 0 || size.height() < 0)
		return; 

	const Gfx::Image&   backImage = backgroundImage();
	Pt::Hmi::Painter&	 painter = surface.painter();
 Gfx::PointF         pos = clientPos();
	Gfx::RectF		       rectClient( pos, size );
 Gfx::RectF		       rect(Gfx::PointF(0,0), surface.size() );
  
  if( _parent != 0 )
  {
   Gfx::Brush	backBrush(_parent->backgroundColor() );  		  
    painter.setBrush(backBrush);
    painter.fillRect(rect);
  }

  painter.setFont(font());

	if( highlight() )
	{       
		Gfx::Brush	brush( highlightColor() );        
		painter.setBrush(brush);
		painter.fillRect(rectClient);
	}
	else
	{
		Gfx::Brush	brush(backgroundColor());
	
		painter.setBrush(brush);    			
		painter.fillRect(rectClient);
	}

	if( !backImage.empty() )
	{
		switch( backgroundImageLayout())
		{				
			case ImageLayout::NoLayout:
			{
				painter.drawImage( Pt::Gfx::PointF(0,0), backImage );
			}
			break;
			
			case ImageLayout::Tile:
			{
				for( double x = pos.x(); x < size.width();  x += backImage.width() )
				{
					for( double y = pos.y(); y < size.height();  y += backImage.height() )
						painter.drawImage(Gfx::PointF(x,y), backImage);
				}
			}
			break;

			case ImageLayout::Center:
			{
				const double x = pos.x() + size.width()/2  - backImage.width()/2;
				const double y = pos.y() + size.height()/2  - backImage.height()/2;
				painter.drawImage(Gfx::PointF(x,y), backImage);
			}
			break;
			
			case ImageLayout::Strech:
			{
				Gfx::Image strech = backImage.blockScale(Gfx::Size((int) size.width(), (int)size.height() ) );
				painter.drawImage( pos, strech );
			}
			break;

			case ImageLayout::Zoom:
			{
        const double factor = size.width()/(double)backImage.width();
        Pt::Gfx::Size newSize( ( size_t)( backImage.width()*factor), (size_t)(backImage.height()*factor));

       Gfx::Image strech = backImage.blockScale(newSize);
				
        painter.drawImage(pos, strech);
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
	Application::instance().mainScreen().setCursor( &cursor() );	
}


void Widget::onPointerLeave()
{	
}


void Widget::processEvent(const Pt::Event& ev)
{
	onEvent(ev);
}


void Widget::onEvent(const Pt::Event& ev)
{
	_eventReceived.send(ev);
}

		
void Widget::onPointerEvent(const PointerEvent& ev)
{		
}


void Widget::onKeyEvent(const KeyEvent& ev)
{ 	
	if( ! isEnabled() )
			return;

	// mnemonic handling
	if( useMnemonic() && ev.state() == Pt::Hmi::KeyEvent::KeyUp )
	{		
		std::string mnKey;

		if( ev.alt())
			mnKey = "A//";
			
		mnKey += ev.toUTF8String();

		 if(_mnemonicKey == mnKey)
		 {
				_mnemonicWidget->mnemonic();						
		 }
	}

	// action key handling
	if( ev.toUTF8String() == focusedActionKey() && hasFocus() )	
	{
		onActionKey( ev.state() );		
	}
	
	// shortcurt Key
	if( ev.shortCutKey() == shortcutKey() )
	{
		onShortcutKey( ev.state() );		
	}

	// propagate to children
	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->onKeyEvent(ev);
}


void Widget::bindMnemonicToWidget(Widget& widget)
{
	_mnemonicWidget = &widget;
}


void Widget::onMnemonic()
{	
	setFocus();
}


bool Widget::contains(const Gfx::PointF& p)
{
	if( p.x()  < size().width() && p.x() >= 0 && p.y() < size().height() && p.y() >= 0)
			return true;
 
	return false;
}


bool Widget::focusPrevChild(int index)
{
	index--;
	
	for( ; index >= 0; --index)
	{
		Widget* child = children()[index];

		if(child->acceptFocus())
		{
			child->setFocus();
			return true;
		}

		if(child->focusPrev())
		{
			child->setFocus();
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

		if(child->acceptFocus())
		{
			child->setFocus();
			return true;
		}

		if(child->focusNext())
		{
			child->setFocus();
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
		if( child->hasFocus() )
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

	if(!child->acceptFocus())
	{
		if(child->focusPrev())
			return true;
	}

	child->onFocus( false);
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
		
	if(!child->acceptFocus())
	{
		if(child->focusNext())
			return true;
	}

  child->onFocus(false);
	return focusNextChild(index);
}


void Widget::onFocus( bool isFocused )
{	
	_hasFocus = isFocused;

  if( !_hasFocus)
	{//False => all childs set to false.
		for( size_t i = 0; i < children().size(); ++i)
		{
			Pt::Hmi::Widget* child = children()[i];			
			child->onFocus(false);									
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
	parent()->onFocus(true);

	//All sibling set to false. Only me let it true
	for( size_t i = 0; i < parent()->children().size(); i++ )
	{
		Widget* child = parent()->children()[i];
				
		if( child != this )
			child->onFocus( false );
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
  
  if( text.empty() )    
	  return std::string::npos;

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

	if( !useMnemonic() )
		return;  
	
	int index = getMnemonicIndex(_caption);

	if( index == std::string::npos )
		return;
					
	std::string unescaped = Widget::removeMnemonic(_caption);

	_mnemonicKey = "A//";	
	_mnemonicKey += std::tolower(unescaped[index]);
}


void Widget::onResize(const Gfx::SizeF& size)
{
	_size = size;			
  
  surface().resize( _size );
	_isValid = false;
}

	
void Widget::onMove(const Gfx::PointF& pos)
{
    _position = pos;
    _isValid = false;
}

Widget* Widget::findWidget( const Gfx::PointF& pos )
{
	std::vector<Widget*>::reverse_iterator it = _children.rbegin();

	if( !visible() )
			return 0;

	for( ; it != _children.rend(); ++it )
	{
		Widget* child = *it;

		Gfx::PointF localPos = child->toClient( pos );		
		
		if( child->contains( localPos ) )
		{
			Widget* found = child->findWidget( pos );

			if( found )
				return found;
		}
	}
		
	return contains( toClient( pos ) )  ? this : 0;
}
		


Widget* Widget::findWidget( const std::string& name )
{

  if( _name == name )
      return this;

  std::vector<Widget*>::iterator it = _children.begin();

	for( ; it != _children.end(); ++it )
	{
		Widget* child = *it;

		Widget* found = child->findWidget( name );

		if( found )
			return found;		
	}
		
	return  0;
}

}} //namespace
