#include <Pt/Forms/Label.h>
#include <Pt/Hmi/LabelController.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/LabelRenderer.h>

namespace Pt {
namespace Forms {

Label::Label()
: _defController(_defModel, _defRenderer)
, _currController(0)
{
	_defModel.ForeColor.set(Pt::Gfx::ARgbColor(0,0,0));
	_defModel.BorderWidth.set(1);
	_defModel.BorderStyle.set(Hmi::BorderStyleType::NoBorder);
	setLabelController(_defController);	
}

void Label::setLabelController(Pt::Hmi::LabelController& controller)
{
	_currController = &controller;
}

const Pt::Hmi::LabelController& Label::labelController() const
{
	return *_currController;
}

const Pt::Hmi::LabelModel& Label::labelModel() const 
{
	return _currController->labelModel();
}

Pt::Hmi::LabelController& Label::labelController()
{
	return *_currController;
}

Pt::Hmi::LabelModel& Label::labelModel()
{
	return _currController->labelModel();
}

void Label::setCaption(const std::string& caption)
{
	labelModel().Caption = caption;
}

const std::string& Label::caption() const
{
	return labelModel().Caption.get();
}

void Label::setPosition(const Pt::Gfx::PointF& position)
{
	labelModel().Position = position;
}

const Pt::Gfx::PointF& Label::position() const
{
	return labelModel().Position.get();
}

void Label::setSize(const Pt::Gfx::SizeF& size)
{
	labelModel().Size = size;
}

const Pt::Gfx::SizeF& Label::size() const
{
	return labelModel().Size.get();
}

void Label::setAutoSize(bool autoSize)
{
	labelModel().AutoSize = autoSize;
}

bool Label::isAutoSize() const
{
	return labelModel().AutoSize.get();
}

Label::~Label()
{
}
 
}}
