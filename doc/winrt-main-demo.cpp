//AUTHOR: Matt Guerrette
//DATE:	8/1/2013
//CREDIT: CREATED FOLLOWING THE DIRECTXTUTORIAL.COM WIN8 METRO SERIES
//DESCRIPTION: BASIC EXAMPLE OF A METRO APP MADE USING C++ AND WINRT

#include "pch.h"

//PCH.H INCLUDES
//#include <wrl/client.h>
//#include <d3d11_1.h>
//#include <DirectXMath.h>
//#include <memory>
//#include <agile.h>
//#include <string>


//NAMESPACE INCLUDE REQUIRED FOR WINDOWS STORE APP DEVELOPMENT
//USING VISUAL C++ AND WINRT
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

//Define class definition for core "Framework" of the app
ref class App sealed : public IFrameworkView
{
	//BOOLEAN FLAG FOR APPLICATION LOOP CONTAINED IN RUN()
	BOOL WindowClosed;
public:
	
	//Initialize override for setting up the basic application architecture
	virtual void Initialize( Windows::ApplicationModel::Core::CoreApplicationView^ applicationView ) 
	{
		//hook our activated event
		applicationView->Activated += 
			ref new TypedEventHandler<CoreApplicationView^,
			 IActivatedEventArgs^>(this, &App::OnActivated);

		//Hook our Suspending and Resuming events
		CoreApplication::Suspending += 
			ref new EventHandler<SuspendingEventArgs^>(this, &App::Suspending);
		CoreApplication::Resuming +=
			ref new EventHandler<Object^>(this, &App::Resuming);

		//Set window closed to initially be false, that
		//way our application loop remains intact.
		WindowClosed = FALSE;
	}

	//Uninitialize override where we handle all necessary code for
	//releasing objects from memory that were loaded in Load
	virtual void Uninitialize()
	{

	}

	//Set window function through which we hook window events
	virtual void SetWindow( Windows::UI::Core::CoreWindow^ window )
	{
		//Hook our PointerPressed event
		window->PointerPressed += ref new
			TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::PointerPressed);
	
		//Hook our Closed event
		window->Closed +=
			ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::Closed);
	}

	//Main application method
	//Here is where our application loop resides in which we
	//implement our application specific code
	virtual void Run() 
	{
		//Obtain hat pointer to the window
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();
	
		//Loop until application closes
		while(!WindowClosed)
		{
			//Run ProcessEvents() to dispatch events
			Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
		
			//Run Application/Game code below
			//.....
			//.....
			//.....
		}
	
	}
	
	//Here is where you could load content for the loading screen ONLY, you have about 2-5 seconds
	//etc....
	virtual void Load( Platform::String^ entryPoint ) {}

	//OnActivated event handler
	//Tells our window to activate itself
	void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args)
	{
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();
		Window->Activate();
	}

	//Suspending window event where we handle all necessary code
	//that should run just before user suspends the application.
	void Suspending(Object^ Sender, SuspendingEventArgs^ Args)
	{
	}

	//Resuming window event where we handle all necessary code
	//that should run just before user resumes the application.
	void Resuming(Object^ Sender, Object^ Args)
	{

	}

	//Closed event thrown when the window is closing, here we set
	//WindowClosed to true so that our application loop breaks.
	void Closed(CoreWindow^ Sender, CoreWindowEventArgs^ Args)
	{
		WindowClosed = TRUE;
	}

	//SAMPLE EVENT HANDLER FOR USER INPUT:
	//Pointer pressed event handler for when the user pressed a button on a mouse,
	//or touches a tablet/phone with a finger or pen
	void PointerPressed(CoreWindow^ Window, PointerEventArgs^ Args)
	{
		MessageDialog dialog("This is a simple Message Dialog notice.", "Notice!");
		dialog.ShowAsync();
	}	
};

//Create class definition that creates an instance of our core framework class
//Purpose of this class is to be used below in our application entry point.
//See: CoreApplication:Run(ref new AppSource());
ref class AppSource sealed : IFrameworkViewSource
{
public:
	
	//Basic method that returns to us an instance of our user defined App class.
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	{
		return ref new App();
	}

};

//STANDS FOR MULTI-THREADED APARTMENT THREAD
//USED FOR HANDLING THREADING WITH METRO APPS AND DIRECTX
[MTAThread]
//APPLICATION ENTRY POINT
//NOTICE THE ARRAY<STRING^> ARGS
//THIS IS SIMILAR TO JAVA/C# STRING[] ARGS
int main(Array<String^>^ args)
{
	//Run our application
	//Notice the similar syntax to Winforms
	//Application.Run(new Form())
	//WinRT and C++ has a similar approach
	CoreApplication::Run(ref new AppSource());
	return 0;
}