#include "ApplicationImpl.h"
#include "Pt/System/SystemError.h"

namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{
}

ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::catchSystemSignal(int sig)
{

}


int ApplicationImpl::getSignalFd() const
{
    return 0;
}

} // namespace System

} // namespace Pt
