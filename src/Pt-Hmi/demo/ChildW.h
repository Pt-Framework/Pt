#ifndef Pt_Hmi_Demo_ChildW_h
#define Pt_Hmi_Demo_ChildW_h

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include "Dialog1.h"

namespace Pt{
namespace Hmi{
namespace Demo{


class ChildW : public Hmi::Window
{
	public:
		ChildW();
		~ChildW();

        void xxx()
        {
            remove(_childWindow2);
            add(_childWindow2);
            _childWindow2.setVisible(true);
        }
	private:
		void onShowDialog(Button& button);
		void onCloseApp();		

	private:
		Hmi::Window     _childWindow2;
		Hmi::Panel  _mainPanel;	
		Hmi::Button _closeButton;
		Hmi::Button _toggleButton;
		Hmi::Button _dialogButton;
		Hmi::Label  _textLabel;			
};

}}}

#endif