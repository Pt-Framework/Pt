#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>

namespace Pt{
namespace Hmi{

Label::Label(LabelModel* model)
: Panel(model)
, _labelModel(model)
{
	_labelModel->BorderStyle.set(Hmi::BorderStyleType::NoBorder);
}

Label::~Label()
{
}


void Label::onRender()
{
	if(!_labelModel->Visible.get())
		return;		

	Pt::Hmi::Painter& localPainter = paintSurface().painter();	
			
	if(_labelModel->AutoSize.get())
	{		
		//Calculate the current and adjust the size
		localPainter.setFont(_labelModel->Font.get());
		Pt::Gfx::FontMetrics metric = localPainter.fontMetrics(Pt::String(_labelModel->Caption.get().c_str()));
		Pt::Gfx::SizeF currentSize = Pt::Gfx::SizeF(_labelModel->toUnit(Pt::Gfx::Size(metric.width(), metric.height())));
		_labelModel->Size.set(currentSize);

		//Render the panel	
		Panel::onRender();

		//Render the label
		Pt::Gfx::PointF	pos(0, metric.ascent());
		Pt::Gfx::Pen	pen(1, _labelModel->ForeColor.get());

		localPainter.setFont(_labelModel->Font.get());

		localPainter.setPen(pen);
		localPainter.drawText(pos,Pt::String(_labelModel->Caption.get().c_str()));
	}
	else
	{		
		Panel::onRender();

		switch(_labelModel->TextAlign.get())
		{
			case Pt::Hmi::TextAlignType::MidleCenter:
			{
				Pt::Gfx::Pen	pen(1,_labelModel->ForeColor.get());
				Pt::Gfx::SizeF	widgetSize =  _labelModel->Size.get();
				
				localPainter.setFont(_labelModel->Font.get());
				Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(Pt::String(_labelModel->Caption.get().c_str()));
				
				const double widthHalf		= _labelModel->Size.get().width()/2;				
				const double heightHalf		= _labelModel->Size.get().height()/2;				
				const double textWidthHalf	= metric.width()/2;	
				const double textHeightHalf = metric.height()/2;	
								
				Pt::Gfx::PointF pos(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) + metric.ascent());							
				localPainter.setFont(_labelModel->Font.get());
				localPainter.setPen(pen);

				localPainter.drawText(pos,Pt::String(_labelModel->Caption.get().c_str()));		
			}
			break;
		}
	}
}

}}