#ifndef Pt_Hmi_Demo_ChildW_h
#define Pt_Hmi_Demo_ChildW_h

#include <Pt/Hmi/MainWindow.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/Property.h>

namespace Pt{
namespace Hmi{
namespace Demo{


class ChildW : public Hmi::ChildWindow
{
	public:
		ChildW();

		~ChildW();

	private:
		void onShowDialog();
		void onCloseApp();		

	private:
		Hmi::ChildWindow     _childWindow2;
		Hmi::Panel  _mainPanel;	
		Hmi::Button _closeButton;
		Hmi::Button _toggleButton;
		Hmi::Button _dialogButton;
		Hmi::Label  _textLabel;			
};

}}}

#endif