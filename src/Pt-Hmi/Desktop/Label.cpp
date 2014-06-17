#include <Pt/Hmi/Desktop/Label.h>
#include <Pt/Hmi/LabelController.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/LabelRenderer.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

Label::Label()
: _defController( new LabelController())
, _defModel( new LabelModel())
, _defRenderer( new LabelRenderer())
{
	_defController->setModel(_defModel);
	_defController->setRenderer(_defRenderer);	
	_defModel->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0));
	_defModel->BorderWidth.set(1);
	_defModel->BorderStyle.set(BorderStyleType::NoBorder);
	setController(*_defController);
}

const Pt::Hmi::LabelController& Label::labelController() const
{
	return *((Pt::Hmi::LabelController*) &controller());
}

const Pt::Hmi::LabelModel& Label::labelModel() const 
{
	return *((Pt::Hmi::LabelModel*) labelController().model());
}

Pt::Hmi::LabelController& Label::labelController()
{
	return *((Pt::Hmi::LabelController*) &controller());
}

Pt::Hmi::LabelModel& Label::labelModel()
{
	return *((Pt::Hmi::LabelModel*) labelController().model());
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
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
 
}}}
