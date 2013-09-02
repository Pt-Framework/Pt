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

Panel::~Panel()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
 
}}}
