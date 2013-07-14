#include <Pt/System/MouseDevice.h>

#include "MouseDeviceImpl.h"

namespace Pt{
namespace System{

MouseDevice::MouseDevice()
: _impl(new MouseDeviceImpl(me()))
{
}

MouseDevice::~MouseDevice()
{
}

void MouseDevice::onClose()
{
}

void MouseDevice::onSetTimeout(size_t timeout)
{
}

size_t MouseDevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
	 return _impl->beginRead(*parent(), buffer, n, eof);
}

size_t MouseDevice::onEndRead(char* buffer, size_t n, bool& eof)
{
	 return _impl->endRead(*parent(), buffer, n, eof);
}

size_t MouseDevice::onBeginWrite(const char* buffer, size_t n)
{
	return 0;
}

size_t MouseDevice::onEndWrite(const char* buffer, size_t n)
{
	return 0;
}

size_t MouseDevice::onRead(char* buffer, size_t count, bool& eof)
{
	return 0;
}

size_t MouseDevice::onWrite(const char* buffer, size_t count)
{
	return 0;
}

bool MouseDevice::onRun()
{
   inputReady().send(*this);
   return true;
}

void MouseDevice::onCancel()
{
    IODevice::onCancel();
}

void MouseDevice::onSync() const
{

    IODevice::onSync();
}

}}
