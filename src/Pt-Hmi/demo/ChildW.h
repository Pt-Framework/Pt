#ifndef Pt_Hmi_Demo_ChildW_h
#define Pt_Hmi_Demo_ChildW_h

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/DockingLayout.h>
#include <Pt/Hmi/FlowLayout.h>
#include "Dialog1.h"

namespace Pt {

namespace Hmi {

namespace Demo {

class ChildW : public Hmi::Window
{
    public:
        ChildW(const std::string& title);
        
        ~ChildW();

    private:
        void onShowDialog(Button& button);
        
        void onCloseApp(Button& button);

    private:
        Hmi::Window _childWindow2;
        Hmi::DockingLayout  _mainWidget;
        Hmi::Button _closeButton;
        Hmi::Button _toggleButton;
        Hmi::Button _dialogButton;
        Hmi::Label  _textLabel;
        Hmi::FlowLayout _buttonBar;
};

} // namespace

} // namespace

} // namespace

#endif