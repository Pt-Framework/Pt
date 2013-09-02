#ifndef Pt_Hmi_Desktop_Button_H
#define Pt_Hmi_Desktop_Button_H

#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Signal.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>

namespace Pt{
namespace Hmi{

class ButtonController;
class ButtonModel;
class ButtonRenderer;
class Controller;
    
    
namespace Desktop{

class PT_HMI_DESKTOP_API Button : public Widget 
{
public:
	Button();
	virtual ~Button();
    
    void setToggleButton(bool toggle = true);
    void setCaption(const char* caption);
    void setSize(const Pt::Gfx::SizeF& size);
    void setPosition(const Pt::Gfx::PointF& position);
    
    Pt::Signal<> ClickedAction;
    Pt::Signal<bool> CheckedAction;

protected:
    virtual void onClicked();
    virtual void onChecked(bool state);
    
private:
    void handleOnClicked(Controller* sender);
    void handleOnChecked(Controller* sender, bool state);
    
private:
	Pt::Hmi::ButtonController*      _defController;
	Pt::Hmi::ButtonModel*           _defModel;
	Pt::Hmi::ButtonRenderer*		_defRenderer;
};
 
}}}
#endif