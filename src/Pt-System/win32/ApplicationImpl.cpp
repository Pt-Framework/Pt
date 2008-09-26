#include "ApplicationImpl.h"

namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::init(SelectorBase& s)
{
}


bool ApplicationImpl::catchSystemSignal(int sig)
{
    return false;
}


bool ApplicationImpl::raiseSystemSignal(int sig)
{
    return false;
}

} // namespace System

} // namespace Pt
