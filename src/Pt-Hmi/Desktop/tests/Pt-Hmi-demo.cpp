
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/Timespan.h"
#include "Pt/Allocator.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/KeyboardDevice.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>

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
		for( int i = 0; i < 3; ++i)
		{
			Pt::Hmi::ButtonModel* buttonModel = (Pt::Hmi::ButtonModel*)(buttons[i].controller().model());
			std::cout<<"Button "<<i<<" : "<<buttonModel->ButtonState.get()<<std::endl;
		}
	}

	void printKey(Pt::Hmi::Controller* source, const Pt::Hmi::KeyEvent& ev)
	{
		std::cout<<"VC:"<<ev.virtualCode()<<" Alt:"<< ev.alt() << " Ext:"<< ev.extCode()<<" St:"<< ev.state()<<" Shift:"<<ev.shift()<<" Ctrl:" <<ev.ctrl()<<std::endl;
	}
	Pt::Hmi::Desktop::Button     buttons[3];

	void onClosed(Pt::Hmi::DeviceButton::State& s)
	{
		if(s == Pt::Hmi::DeviceButton::Pressed)
		{
			Pt::Hmi::Application::instance().exit();
		}
	}
};


int main(int argc, char* args[])
{
	

	Pt::Hmi::Application		app;
	Test test;
	Pt::Hmi::Desktop::Window	window;	
	Pt::Hmi::Desktop::Panel     panel;
	Pt::Hmi::Desktop::Panel     panel2;
	Pt::Hmi::Desktop::Label     label;
	Pt::Hmi::Desktop::Button    closeBt;
 
	Pt::Hmi::PanelModel* panelModel = (Pt::Hmi::PanelModel*) panel.controller().model();
	
	panelModel->Position.set(Pt::Gfx::PointF(40,120));
	panelModel->Size.set(Pt::Gfx::SizeF(600,500));	
	panelModel->BorderStyle.set(Pt::Hmi::BorderStyle::Sizebale);
	panelModel->BorderWidth.set(3);

	window.addChild(&panel);	

	Pt::Hmi::LabelModel* labelModel = (Pt::Hmi::LabelModel*)(label.controller().model());
	labelModel->Position.set(Pt::Gfx::PointF(100,100));
	labelModel->ForeColor.set(Pt::Gfx::ARgbColor(0,255,0,0));
	labelModel->Caption.set("Press TAB or TAB+SHIFT for focus handling. Space for action");
	
	panel.addChild(&label);
	
	for(size_t i = 0; i < 2; ++i)
	{
		Pt::Hmi::ButtonModel* buttonModel = (Pt::Hmi::ButtonModel*)(test.buttons[i].controller().model());
		std::stringstream ss;

		ss<<"Button "<<i;
		buttonModel->Caption.set(ss.str());
		buttonModel->Size.set(Pt::Gfx::SizeF(80,50));
		buttonModel->Position.set(Pt::Gfx::PointF(30 +(i*90),20));
		buttonModel->ButtonState.PropertyChanged += Pt::slot(test, &Test::print);
		//buttonModel->ButtonType.set(Pt::Hmi::ButtonType::Press);
		buttonModel->ButtonType.set(Pt::Hmi::ButtonType::Toggle);
		panel.addChild(&test.buttons[i]);
	}

	Pt::Hmi::ButtonModel* buttModel = (Pt::Hmi::ButtonModel*)(test.buttons[2].controller().model());

	buttModel->ButtonType.set(Pt::Hmi::ButtonType::Press);
	buttModel->Caption.set("Test shortcut(CTRL+A)");
	buttModel->Size.set(Pt::Gfx::SizeF(160,20));
	buttModel->Position.set(Pt::Gfx::PointF(15,15));
	buttModel->ActionKey.set("CTRL//A");
	


	Pt::Hmi::PanelModel* m = (Pt::Hmi::PanelModel*)panel2.controller().model();
	m->Size.set(Pt::Gfx::SizeF(260,150));
	m->Position.set(Pt::Gfx::PointF(20,120));
	m->BorderStyle.set(Pt::Hmi::BorderStyle::Single);
	panel2.addChild(&test.buttons[2]);

	panel.addChild(&panel2);
	
	Pt::Hmi::WindowModel* wm = (Pt::Hmi::WindowModel*) window.controller().model();
	wm->Border = Pt::Hmi::BorderStyle::Sizebale;
	
	Pt::Hmi::WindowController* winCtrl = (Pt::Hmi::WindowController*) &window.controller();

	Pt::Hmi::WindowModel* model =  (Pt::Hmi::WindowModel*) window.controller().model();
	model->Border = Pt::Hmi::BorderStyle::Sizebale;
	model->ShowInTaskbar.set(true);
	

	Pt::Hmi::ButtonModel* btm =  (Pt::Hmi::ButtonModel*) closeBt.controller().model();

	btm->Position.set(Pt::Gfx::PointF(600,650));
	btm->Size.set(Pt::Gfx::SizeF(130,40));
	btm->Caption.set("Close (CTRL+X)");
	btm->ActionKey.set("CTRL//X");
	btm->ButtonState.PropertyChanged += Pt::slot(test,  &Test::onClosed);


	window.addChild(&closeBt);	
	window.show();
	
	app.showConsole();
	app.run();		
}

