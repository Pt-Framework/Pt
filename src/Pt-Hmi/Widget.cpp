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
, PT_HMI_INIT_PROPERTY_VALUE(Size, Pt::Gfx::SizeF(10,10))
, PT_HMI_INIT_PROPERTY_VALUE(BackColor,Pt::Gfx::ARgbColor(237,237,237))
, PT_HMI_INIT_PROPERTY_VALUE(BackColorHightLight,Pt::Gfx::ARgbColor(200,200,200))
, PT_HMI_INIT_PROPERTY_VALUE(ForeColor, Pt::Gfx::ARgbColor(70,70,70))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImage, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImageLayout, ImageLayoutType::NoLayout)
, PT_HMI_INIT_PROPERTY_VALUE(Opacity,0)
, PT_HMI_INIT_PROPERTY(Pointer2DStatus)
, PT_HMI_INIT_PROPERTY(KeyStatus)
, PT_HMI_INIT_PROPERTY(CursorT)
, PT_HMI_INIT_PROPERTY_VALUE(TextAlign,TextAlignType::MidleCenter)
, PT_HMI_INIT_PROPERTY_VALUE(Focused, false)
, PT_HMI_INIT_PROPERTY_VALUE(AcceptFocus,true)
, PT_HMI_INIT_PROPERTY_VALUE(HighLight, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocusedActionKey," ")
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(UseMnemonic,true)
, _parent(0)
{
	Focused.Changed += Pt::slot( *this, &Widget::onFocusChanged );
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


void Widget::onFocusChanged( const Property<bool>& prop )
{
	if(Focused.get())
	{//True
		if( _parent != 0)
		{
			//All parents set to true.
			_parent->Focused.set(true);
			_parent->Focused.Changed.send(_parent->Focused);

			//All sibling set to false. Only me let it true
			for( size_t i = 0; i < _parent->children().size(); i++)
			{
				Widget* child = _parent->children()[i];
				child->Focused = false;
			}
		}
	}
	else
	{//False  
		Pt::Hmi::Widget& widget = *this;

		for( size_t i = 0; i < widget.children().size(); ++i)
		{//All childs set to false

			Widget* child = widget.children()[i];
			child->Focused.set(false);						
			child->Focused.Changed.send(child->Focused);
		}
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


void Widget::render()
{
	if( !Visible.get() )
		return;
	
	onRender();

	Pt::Hmi::Painter& localPainter = _paintSurface.painter();

	for( size_t i = 0; i < children().size(); ++i)
	{
		Widget*				child = _children[i];			
		child->render();

		localPainter.drawSurface(child->Position.get(), child->paintSurface());
	}	
}


void Widget::invalidate()
{
	onInvalidate();
}


void Widget::onInvalidate()
{
	if( parent() )
		parent()->onInvalidate();		
}



std::string Widget::getMnemonicKey() const
{
	std::string mnemonic = "";

	int index = Caption.get().find('&');
	
	if( index < 0 || ((index + 1)> Caption.get().size()))
		return mnemonic;


	mnemonic = "A//";	
	mnemonic+= std::tolower(Caption.get()[index + 1]);
	return mnemonic;	
}


void Widget::onKeyInput(const KeyEvent& ev)
{ 
	KeyStatus = ev;
	
	if(UseMnemonic.get() && _mnemonicWidget != 0 && Enabled.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp)
	{		
		std::string mnKey = "";

		if( KeyStatus.get().alt())
			mnKey = "A//";
			
		mnKey += KeyStatus.get().toUTF8String();

		 if(getMnemonicKey() == mnKey)
			_mnemonicWidget->onMnemonic();			
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->keyInput(ev);
}


void Widget::onPointerInput(const PointingEvent& ev)
{
	Pointer2DStatus = ev;

	for( size_t i = 0; i < children().size(); ++i)
		_children[i]->pointerInput(ev);
}

void Widget::bindMnemonicToWidget(Widget* widget)
{
	_mnemonicWidget = widget;
}

void Widget::onMnemonic()
{
	if(Focused.get() != true)
		Focused = true;
}

void Widget::onRender()
{		
	if(!Visible.get())
		return;

	if( Size.get().width() < 0 ||  Size.get().height() < 0)
		return;

	Pt::Gfx::SizeF size = Size.get();
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
			case ImageLayoutType::NoLayout:
			{
				localPainter.drawImage(Pt::Gfx::PointF(0,0), backImage);
			}
			break;
			
			case ImageLayoutType::Tile:
			{
				for( size_t x = 0; x < _paintSurface.size().width();  x += backImage.width())
				{
					for( size_t y = 0; y < _paintSurface.size().height();  y += backImage.height())
						localPainter.drawImage(Pt::Gfx::PointF(x,y), backImage);
				}
			}
			break;

			case ImageLayoutType::Center:
			{
				double x = size.width()/2  - backImage.width()/2;
				double y = size.height()/2  - backImage.height()/2;
				localPainter.drawImage(Pt::Gfx::PointF(x,y), backImage);
			}
			break;
			
			case ImageLayoutType::Strech:
			{
				Pt::Gfx::ARgbImage strech(_paintSurface.size().width(), _paintSurface.size().height() );

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(), strech.begin(), _paintSurface.size().width(),  _paintSurface.size().height());
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;

			case ImageLayoutType::Zoom:
			{
				Pt::Gfx::ARgbImage strech(_paintSurface.size().width(), _paintSurface.size().height() );
				double factor = _paintSurface.size().width()/(double)backImage.width();

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(),strech.begin(),  strech.width(), (Pt::size_t)(backImage.height()*factor));
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;
		}
	}	
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


bool Widget::contains(const Pt::Gfx::PointF& p)
{
	double x1, x2, y1, y2;
	
	x1 = 0;
	x2 = x1  + Size.get().width();

	y1 = 0;
	y2 = y1  + Size.get().height();
	
	return ((p.x() >= x1) && (p.x() < x2) && (p.y() >= y1) && (p.y() < y2));
}


}} //namespace
