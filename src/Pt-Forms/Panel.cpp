#include <Pt/Forms/Panel.h>

namespace Pt { 
namespace Forms {

Panel::Panel()
: _defController(_defModel,_defView )
, _currController(0)
{
	_defModel.ForeColor.set(Pt::Gfx::ARgbColor(160,160,160));
	_defModel.BorderWidth.set(1);
	_defModel.BorderStyle.set(Hmi::BorderStyleType::Widget);

	setPanel(_defController);
}

void Panel::setPanel(Pt::Hmi::Panel& ctrl)
{
	_currController = &ctrl;
}

Pt::Hmi::Panel& Panel::panel()
{
	return *_currController;
}

Pt::Hmi::PanelModel& Panel::panelModel()
{
	return _currController->panelModel();
}

const Pt::Hmi::Panel&	Panel::panel() const
{
	return *_currController;
}

const Pt::Hmi::PanelModel& Panel::panelModel() const
{
	return _currController->panelModel();
}

void Panel::setSize(const Pt::Gfx::SizeF& size)
{
	panelModel().Size = size;
}

const Pt::Gfx::SizeF& Panel::size() const
{
	return panelModel().Size.get(); 
}

void Panel::setPosition(const Pt::Gfx::PointF& position)
{
	panelModel().Position = position;
}

const Pt::Gfx::PointF& Panel::position() const
{
	return panelModel().Position.get();
}

Panel::~Panel()
{
}
 
}}
