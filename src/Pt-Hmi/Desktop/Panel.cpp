#include <Pt/Hmi/Desktop/Panel.h>
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/PanelRenderer.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

Panel::Panel()
: _defController( new PanelController())
, _defModel( new PanelModel())
, _defRenderer( new PanelRenderer())
{
	_defController->setModel(_defModel);
	_defController->setRenderer(_defRenderer);	
	_defModel->ForeColor.set(Pt::Gfx::ARgbColor(160,160,160));
	_defModel->BorderWidth.set(1);
	_defModel->BorderStyle.set(BorderStyleType::Widget);

	setController(*_defController);
}

Pt::Hmi::PanelController& Panel::panelController()
{
	return *((Pt::Hmi::PanelController*) &controller());
}

Pt::Hmi::PanelModel& Panel::panelModel()
{
	return *((Pt::Hmi::PanelModel*)panelController().model());
}

const Pt::Hmi::PanelController&	Panel::panelController() const
{
	return *((Pt::Hmi::PanelController*) &controller());
}

const Pt::Hmi::PanelModel& Panel::panelModel() const
{
	return *((Pt::Hmi::PanelModel*)panelController().model());
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
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
 
}}}
