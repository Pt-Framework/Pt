
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/Timespan.h"
#include "Pt/Allocator.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/KeyboardDevice.h>

//Desktop
#include <Pt/Hmi/Desktop/Window.h>

#include <Pt/Hmi/Desktop/Panel.h>
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PanelModel.h>

#include <Pt/Hmi/Desktop/Label.h>
#include <Pt/Hmi/LabelModel.h>


#include <Pt/Hmi/Desktop/Button.h>
#include <Pt/Hmi/ButtonModel.h>


class Test : public Pt::Connectable
{
public:
	void print(Pt::Hmi::DeviceButton::State& s)
	{
		std::cout<<"State: "<<s<<std::endl;
		Pt::Hmi::ButtonModel* buttonModel = (Pt::Hmi::ButtonModel*)(button.controller().model());
	}


	void printKey(const Pt::Hmi::KeyEvent& ev)
	{
		if(ev.state() == Pt::Hmi::KeyEvent::KeyDown)
			std::cout<<ev.virtualCode()<<" "<< ev.alt() << " "<< ev.extCode()<<std::endl;
	}
	Pt::Hmi::Desktop::Button     button;
};


int main(int argc, char* args[])
{
	
	Pt::Hmi::Application		app;
	Test test;
	Pt::Hmi::Desktop::Window	window;	
	Pt::Hmi::Desktop::Panel     panel;
	Pt::Hmi::Desktop::Label     label;
	Pt::Hmi::KeyboardDevice     keyboardDevice;
	

	keyboardDevice.Event += Pt::slot(test, &Test::printKey);
	 keyboardDevice.start(app.loop());
	 

	Pt::Hmi::PanelModel* panelModel = (Pt::Hmi::PanelModel*) panel.controller().model();
	
	panelModel->Position.set(Pt::Gfx::PointF(30,120));
	panelModel->Size.set(Pt::Gfx::SizeF(400,500));	
	panelModel->BorderStyle.set(Pt::Hmi::BorderStyle::Sizebale);
	panelModel->BorderWidth.set(6);

	window.addChild(&panel);	

	Pt::Hmi::LabelModel* labelModel = (Pt::Hmi::LabelModel*)(label.controller().model());
	labelModel->Position.set(Pt::Gfx::PointF(100,100));
	labelModel->ForeColor.set(Pt::Gfx::ARgbColor(0,255,0,0));
	labelModel->Caption.set("This is a line label into a sizeable panel:");
	
	panel.addChild(&label);
	
	Pt::Hmi::ButtonModel* buttonModel = (Pt::Hmi::ButtonModel*)(test.button.controller().model());
	buttonModel->Caption.set("OK");
	buttonModel->Size.set(Pt::Gfx::SizeF(60,20));
	buttonModel->Position.set(Pt::Gfx::PointF(300,400));
	buttonModel->ButtonState.PropertyChanged += Pt::slot(test, &Test::print);


	panel.addChild(&test.button);

	window.show();
	

	app.run();		
}


