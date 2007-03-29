#include "Pt/System/IOMonitor.h"
#include "IOMonitorImpl.h"


namespace Pt{
namespace System{

IOMonitor::IOMonitor()
: _impl( 0 )
{
    _impl = new IOMonitorImpl();
}

IOMonitor::~IOMonitor()
{ 
    delete _impl;
}

Signal<const IOEvent&>&  IOMonitor::addDevice( IODevice& device )
{
    return _impl->addDevice( device );
}

void IOMonitor::removeDevice( IODevice& device )
{
    _impl->removeDevice( device );
}

bool IOMonitor::wait(unsigned int msecs)
{
    return _impl->wait(msecs);
}

void IOMonitor::wake()
{
    _impl->wake();
}

}//namespace System
}//namespace Pt
