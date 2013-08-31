#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/System/IOError.h>

namespace Pt {

namespace Hmi {

Selector::Selector()
{
}


Selector::~Selector()
{
}


DWORD Selector::waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout)
{	
    DWORD result = MsgWaitForMultipleObjects(numHandles, (HANDLE *)handles, false, msecs, QS_ALLEVENTS);
    if(result == WAIT_FAILED)
    {
        //DWORD err = GetLastError();
        throw Pt::System::IOError( PT_ERROR_MSG("WaitForMultipleObjects failed") );
    }

    if( result == WAIT_TIMEOUT)
    {
        isTimeout = true;
        return 0;
    }

    DWORD offset = result - WAIT_OBJECT_0;

    if(offset == numHandles)
    {
        processMessage();
    }

    return offset;
}


void Selector::processMessage()
{
    MSG msg;

    while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	
}


ApplicationImpl::ApplicationImpl()
: Pt::System::EventLoop()
, _dpi(72.0)
{
	
    _instanceHandle = (HINSTANCE)GetModuleHandle(NULL);

    registerWindowClasses();

	getScreeResolution(_screenWidth, _screenHeight);

	_width  = _screenWidth * unitSizeInch()*_dpi;
	_height = _screenHeight * unitSizeInch()*_dpi;
	
	_factorX = _width / _screenWidth;
	_factorY = _height / _screenHeight;
	_offsetX = 0;
	_offsetY = 0;

	//FreeConsole();
}

void ApplicationImpl::showConsole(bool show)
{
	if(show)
		AllocConsole();
	else
		FreeConsole();
}

ApplicationImpl::~ApplicationImpl()
{
}

void ApplicationImpl::setResolution(double dpi)
{
	_dpi = dpi;
}


double ApplicationImpl::resolutionDPI() const
{
	return _dpi;
}


int ApplicationImpl::fromUnit(double unit)
{
	return (int) (unit *unitSizeInch()* _dpi);
}


double ApplicationImpl::toUnit(int unit)
{
	return unitSizeInch()/_dpi * unit;
}


void ApplicationImpl::registerWindowClasses()
{
    std::string topLevelWindow = "Pt-Hmi";

    WNDCLASS topWindowClass;

    topWindowClass.style         = CS_HREDRAW | CS_VREDRAW;
    topWindowClass.lpfnWndProc   = (WNDPROC)ApplicationImpl::wndProc;
    topWindowClass.cbClsExtra    = 0;
    topWindowClass.cbWndExtra    = 0;
    topWindowClass.hInstance     = _instanceHandle;
    topWindowClass.hIcon         = NULL;
    topWindowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    topWindowClass.hbrBackground = NULL;
    topWindowClass.lpszMenuName  = NULL;
    topWindowClass.lpszClassName = topLevelWindow.c_str();

    RegisterClass(&topWindowClass);
}


void ApplicationImpl::unregisterWindowClasses()
{
    UnregisterClass("Pt-Hmi", _instanceHandle);
}


long CALLBACK ApplicationImpl::wndProc(HWND hwnd, unsigned int message, unsigned int wParam, long lParam)
{
	Pt::Hmi::Application& app =  *((Pt::Hmi::Application*)&Pt::Hmi::Application::instance());

    return app.impl()->dispatchGDIEvent(hwnd, message, wParam, lParam);
}


LRESULT ApplicationImpl::dispatchGDIEvent(HWND hwnd, unsigned int message, unsigned int wParam, long lParam)
{
	bool handled = false;
	WindowEvent.send(hwnd, message, wParam, lParam, handled);

	if(!handled)
		return DefWindowProc(hwnd, message, wParam, lParam);

	return 0;
}


void ApplicationImpl::onAttachSelectable(System::Selectable& s)
{ 
    _selector.attach(s); 
}


void ApplicationImpl::onDetachSelectable(System::Selectable& s)
{ 
    _selector.detach(s); 
}


void ApplicationImpl::onIdle(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);

    std::vector<System::Selectable*>::iterator it = _avail.begin();
    while(it != _avail.end())
    {
        if(*it == &s)
            it = _avail.erase(it);
        else
            ++it;
    }
}


void ApplicationImpl::onReady(System::Selectable& s)
{
    Pt::System::MutexLock lock(_mutex);
    _avail.push_back(&s);
}


void ApplicationImpl::onRun()
{
    while( this->waitNext() )
        ;
}


void ApplicationImpl::onExit()
{
    _eventQueue.exit();
    wake();
}


void ApplicationImpl::onCommitEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev); 
    wake();
}


void ApplicationImpl::onQueueEvent(const Pt::Event& ev)
{ 
    _eventQueue.pushEvent(ev); 
}


void ApplicationImpl::onProcessEvents()
{ 
    _eventQueue.processEvents( this->event() );
}


void ApplicationImpl::onWake()
{ 
    _selector.wake(); 
}


void ApplicationImpl::onAttachTimer(System::Timer& timer)
{ 
    _timerQueue.addTimer(timer); 
}


void ApplicationImpl::onDetachTimer(System::Timer& timer )
{ 
    _timerQueue.removeTimer(timer); 
}

bool ApplicationImpl::waitNext()
{
    size_t timeout = _timerQueue.processTimers();

    // check all selectables that did not require waiting
    while( true )
    {
        Pt::System::MutexLock lock(_mutex);

        if( _avail.empty() )
            break;

        timeout = 0;
        System::Selectable* s = _avail.back();
        _avail.pop_back();
        lock.unlock();

        s->run();
    }

    bool isActive = true;
    if( _selector.waitForWake(timeout) )
        isActive = _eventQueue.processEvents( this->event() );

    return isActive;
}

	
Pt::Gfx::PointF ApplicationImpl::toUnit(const Pt::Gfx::Point& value)
{
	const double x = value.x() * _factorX  + _offsetX;
	const double y = value.y() * _factorY  + _offsetY;

	return Pt::Gfx::PointF(std::ceil(x),std::ceil(y));
}

Pt::Gfx::SizeF ApplicationImpl::toUnit(const Pt::Gfx::Size& value)
{
	const double width = value.width() * _factorX  + _offsetX;
	const double height = value.height() * _factorY  + _offsetY;

	return Pt::Gfx::SizeF(std::ceil(width),std::ceil(height));
}

Pt::Gfx::Point ApplicationImpl::fromUnit(const Pt::Gfx::PointF& value)
{
	double factorX = _screenWidth / _width;
	double factorY = _screenHeight / _height;
	int x = (int) ( value.x() * factorX); 
	int y = (int) ( value.y() * factorY);
	
	return Pt::Gfx::Point(x,y);
}

Pt::Gfx::Size ApplicationImpl::fromUnit(const Pt::Gfx::SizeF& value)
{
	double factorX = _screenWidth / _width;
	double factorY = _screenHeight / _height;
	int width = (int) ( value.width() * factorX); 
	int height = (int) ( value.height() * factorY);
	
	return Pt::Gfx::Size(width,height);
}

double ApplicationImpl::unitSizeInch() const
{
	return 1.0/72.0;
}

double ApplicationImpl::unitSizeMm() const
{
	return 25.4 * unitSizeInch();
}


void ApplicationImpl::nextEvent()
{
	waitNext();
}

void ApplicationImpl::getScreeResolution(int& horizontal, int& vertical)
{
   const HWND hDesktop = GetDesktopWindow();
   RECT desktop;   
   GetWindowRect(hDesktop, &desktop);
   horizontal = desktop.right;
   vertical = desktop.bottom;
}

}}
