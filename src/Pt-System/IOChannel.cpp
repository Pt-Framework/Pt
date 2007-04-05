#include "Pt/System/IOChannel.h"

namespace Pt {

namespace System {


IOChannel::IOChannel()
: _device(0)
, _waitMode(WaitInput)
{
}


IOChannel::IOChannel(IODevice& device, WaitMode wm)
: _device(&device)
, _waitMode(wm)
{
}


IOChannel::~IOChannel()
{
    this->destroyed(*this);
}


void IOChannel::attach(IODevice& device, WaitMode wm)
{
    _device = &device;
    _waitMode = wm;
}


IODevice& IOChannel::device()
{
    if(_device == 0)
        throw std::logic_error("No IODevice in channel" + PT_SOURCEINFO);

    return *_device;
}


const IODevice& IOChannel::device() const
{
    if(_device == 0)
        throw std::logic_error("No IODevice in channel" + PT_SOURCEINFO);

    return *_device;
}


IOChannel::WaitMode IOChannel::waitMode() const
{
    return _waitMode;
}

}//namespace System

}//namespace Pt
