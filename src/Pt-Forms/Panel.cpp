#include <Pt/Forms/Panel.h>
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/PanelRenderer.h>

namespace Pt { 
namespace Forms {

Panel::Panel()
: _defController(_defModel,_defRenderer )
, _currController(0)
{
	_defModel.ForeColor.set(Pt::Gfx::ARgbColor(160,160,160));
	_defModel.BorderWidth.set(1);
	_defModel.BorderStyle.set(Hmi::BorderStyleType::Widget);

	setPanelController(_defController);
}

void Panel::setPanelController(Pt::Hmi::PanelController& ctrl)
{
	_currController = &ctrl;
}

Pt::Hmi::PanelController& Panel::panelController()
{
	return *_currController;
}

Pt::Hmi::PanelModel& Panel::panelModel()
{
	return _currController->panelModel();
}

const Pt::Hmi::PanelController&	Panel::panelController() const
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
