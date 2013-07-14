#ifndef Pt_System_MouseDevice_H
#define Pt_System_MouseDevice_H

#include <Pt/System/IODevice.h>
#include <Pt/System/Input2DEvent.h>
#include <Pt/Signal.h>

namespace Pt{
namespace System{

class MouseDeviceImpl;

class PT_SYSTEM_API MouseDevice : public Pt::System::IODevice
{
public:
	MouseDevice();
	virtual ~MouseDevice();

	Pt::Signal<const Input2DEvent&> InputEvent;

protected:
    void onClose();

    void onSetTimeout(size_t timeout);

    size_t onBeginRead(char* buffer, size_t n, bool& eof);

    size_t onEndRead(char* buffer, size_t n, bool& eof);

    size_t onBeginWrite(const char* buffer, size_t n);

    size_t onEndWrite(const char* buffer, size_t n);

    size_t onRead(char* buffer, size_t count, bool& eof);

    size_t onWrite(const char* buffer, size_t count);

    bool onRun();

    void onCancel();

    void onSync() const;

private:
	inline MouseDevice& me()
	{
		return *this;
	}

	MouseDeviceImpl* _impl;
	
};

}}
#endif
