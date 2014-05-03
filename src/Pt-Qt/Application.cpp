#include "ApplicationImpl.h"
#include <Pt/Qt/Application.h>
#include <Pt/System/MainLoop.h>

namespace Pt {
namespace Qt {

Application::Application(int argc, char** argv)
: System::Application(0, argc, argv)
, QApplication(argc, argv)
, _impl(0) 
{ 	
    _impl =  new ApplicationImpl(this->loop().eventReceived());
	_impl->init(this);
	
	Pt::System::Application::init( *_impl );
}


Application::~Application()
{
	delete _impl;
}


Application& Application::instance()
{
    return static_cast<Application&>( System::Application::instance() );
}

ApplicationImpl* Application::impl()
{
	return _impl;
}

}}

