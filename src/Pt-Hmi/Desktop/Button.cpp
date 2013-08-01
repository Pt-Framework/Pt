#include <Pt/Hmi/Desktop/Button.h>
#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/ButtonRenderer.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

Button::Button()
: _defController( new ButtonController())
, _defModel( new ButtonModel())
, _defRenderer( new ButtonRenderer())
{
	_defController->setModel(_defModel);
	_defController->setRenderer(_defRenderer);	
	_defModel->BackColor.set(Pt::Gfx::ARgbColor(200,200,200));
	_defModel->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0));
	_defModel->BorderWidth.set(1);
	_defModel->BorderStyle.set(BorderStyle::Widget);

	setController(*_defController);
}

Button::~Button()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
 
}}}
