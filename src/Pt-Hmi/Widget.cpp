#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Brush.h>

namespace Pt{
namespace Hmi{

Widget::Widget(WidgetModel* model)
: _mnemonicWidget(0)
, _widgetModel(0)
{
	if( _widgetModel != 0 )
		_widgetModel->Focused.Changed -= Pt::slot( *this, &Widget::onFocusChanged );	

	_widgetModel = model; 
	_widgetModel->Focused.Changed += Pt::slot( *this, &Widget::onFocusChanged );
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
	if(_widgetModel->Focused.get())
	{//True
		if( _parent != 0)
		{
			WidgetModel* parentModel = _parent->widgetModel();
			
			//All parents set to true.
			parentModel->Focused.set(true);
			parentModel->Focused.Changed.send(parentModel->Focused);

			//All sibling set to false. Only me let it true
			for( size_t i = 0; i < _parent->children().size(); i++)
			{
				Widget* child = _parent->children()[i];
				WidgetModel* childModel = child->widgetModel();
				
				if(childModel != _widgetModel)
					childModel->Focused = false;
			}
		}
	}
	else
	{//False  
		Pt::Hmi::Widget& widget = *this;

		for( size_t i = 0; i < widget.children().size(); ++i)
		{//All childs set to false

			Widget* child = widget.children()[i];
			WidgetModel* childModed = child->widgetModel();

			childModed->Focused.set(false);						
			childModed->Focused.Changed.send(childModed->Focused);
		}
	}
}


Pt::Gfx::PointF Widget::toClient( const Pt::Gfx::PointF& globalPoint )
{
	if( _parent == 0 )
		return Pt::Gfx::PointF( globalPoint.x(), globalPoint.y() );

	Pt::Gfx::PointF parPoint = _parent->toClient( globalPoint );
	return Pt::Gfx::PointF( parPoint.x() - _widgetModel->Position.get().x(), parPoint.y() - _widgetModel->Position.get().y() );
}


Pt::Gfx::PointF Widget::fromClient( const Pt::Gfx::PointF& localPoint, bool toRoot )
{
	double x = localPoint.x();
	double y = localPoint.y();
	const Widget* parent = _parent;

	while( parent != 0 )
	{
		parent = parent->parent();
		const WidgetModel* model = parent->widgetModel();		
		
		if(!(toRoot && parent == 0))
		{
			x += model->Position.get().x();
			y += model->Position.get().y();
		}
	}
	
	return Pt::Gfx::PointF(x,y);
}


void Widget::render()
{
	if( !_widgetModel->Visible.get() )
		return;
	
	onRender();

	Pt::Hmi::Painter& localPainter = _paintSurface.painter();

	for( size_t i = 0; i < children().size(); ++i)
	{
		Widget*				child = _children[i];			
		WidgetModel*	childModel = child->widgetModel();

		child->render();

		localPainter.drawSurface(childModel->Position.get(), child->paintSurface());
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


void Widget::onKeyInput(const KeyEvent& ev)
{ 
	_widgetModel->KeyStatus = ev;
	
	if(_widgetModel->UseMnemonic.get() && _mnemonicWidget != 0 && _widgetModel->Enabled.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp)
	{		
		std::string mnKey = "";

		if(_widgetModel->KeyStatus.get().alt())
			mnKey = "A//";
			
		mnKey += _widgetModel->KeyStatus.get().toUTF8String();

		 if(_widgetModel->getMnemonicKey() == mnKey)
			_mnemonicWidget->onMnemonic();			
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->keyInput(ev);
}


void Widget::onPointerInput(const PointingEvent& ev)
{
	_widgetModel->Pointer2DStatus = ev;

	for( size_t i = 0; i < children().size(); ++i)
		_children[i]->pointerInput(ev);
}

void Widget::bindMnemonicToWidget(Widget* widget)
{
	_mnemonicWidget = widget;
}

void Widget::onMnemonic()
{
	if(_widgetModel->Focused.get() != true)
		_widgetModel->Focused = true;
}

void Widget::onRender()
{		
	if(!_widgetModel->Visible.get())
		return;

	if( _widgetModel->Size.get().width() < 0 ||  _widgetModel->Size.get().height() < 0)
		return;

	Pt::Gfx::SizeF size = _widgetModel->Size.get();
	Pt::Gfx::SizeF bufferSize = _paintSurface.size();

	//ToDo: move this check to surface resize.
	if(bufferSize.width() != size.width() ||bufferSize.height() != size.height())
		_paintSurface.resize(size);

	Pt::Gfx::ARgbImage& backImage = _widgetModel->BackgroundImage.get();
	Pt::Hmi::Painter&	localPainter = _paintSurface.painter();
	Pt::Gfx::RectF		rect(Pt::Gfx::PointF(0,0),size);
	
	localPainter.setFont(_widgetModel->Font.get());

	if(_widgetModel->HighLight.get())
	{       
		Pt::Gfx::Brush	brush(_widgetModel->BackColorHightLight.get());
        
		localPainter.setBrush(brush);
		localPainter.fillRect(rect);
	}
	else
	{
		Pt::Gfx::Brush	brush(_widgetModel->BackColor.get());
	
		localPainter.setBrush(brush);
    
		localPainter.fillRect(rect);
	}

	if( backImage.width() != 0 && backImage.height() != 0)
	{
		switch( _widgetModel->BackgroundImageLayout.get())
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
		const WidgetModel* model = child->widgetModel();

		if(model->Focused.get())
			return i;		
	}		

	return -1;
}

}} //namespace
