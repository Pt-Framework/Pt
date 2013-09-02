#include <Pt/Hmi/Desktop/Button.h>
#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/ButtonRenderer.h>
#include <Pt/Hmi/Controller.h>

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
	_defModel->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0));
	_defModel->BorderWidth.set(1);
	_defModel->BorderStyle.set(BorderStyleType::Widget);

    _defController->PressedAction += Pt::slot(*this, &Button::handleOnClicked);
    _defController->CheckedAction += Pt::slot(*this, &Button::handleOnChecked);
	setController(*_defController);
}
    
void Button::setToggleButton(bool toggle)
{
        
}
    
void Button::setCaption(const char* caption)
{
        
}

void Button::setSize(const Pt::Gfx::SizeF& size)
{
        
}

void Button::setPosition(const Pt::Gfx::PointF& position)
{
        
}

void Button::onClicked()
{
    ClickedAction.send();
}
    
void Button::onChecked(bool state)
{
    CheckedAction.send(state);
}
    

void Button::handleOnClicked(Controller* sender)
{
    onClicked();
}

void Button::handleOnChecked(Controller* sender, bool state)
{
    onChecked(state);
}
    
Button::~Button()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
 
}}}
