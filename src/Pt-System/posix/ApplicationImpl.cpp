#include "ApplicationImpl.h"
#include "Pt/System/Pipe.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Application.h"
#include "Pt/System/SystemError.h"
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

namespace {

    Pt::System::Pipe* pt_signal_pipe = 0;
    static char _signalBuffer[128];

    void initSignalPipe()
    {
        if( ! pt_signal_pipe )
        {
            pt_signal_pipe = new Pt::System::Pipe(Pt::System::Pipe::Async);
            pt_signal_pipe->out().beginRead( _signalBuffer, sizeof(_signalBuffer) );
        }
    }

    void processSignal(Pt::System::IODevice& device)
    {
        try
        {
            size_t n = device.endRead();

            int sigNo = 0;
            char* it = _signalBuffer;
            char* last = &_signalBuffer[ n- sizeof(sigNo) ];
            while(it <= last)
            {
                memcpy(&sigNo, it, sizeof(sigNo));
                Pt::System::Application::instance().systemSignal.send(sigNo);
                it += sizeof(sigNo);
            }

            device.beginRead( _signalBuffer, sizeof(_signalBuffer) );
        }
        catch(...)
        {
            device.beginRead( _signalBuffer, sizeof(_signalBuffer) );
            throw;
        }
    }

}


extern "C" void pt_system_application_sighandler(int sigNo)
{
    if(pt_signal_pipe)
    {
        pt_signal_pipe->in().write( (char*)&sigNo, sizeof(sigNo) );
    }
}

namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{
    ::initSignalPipe();
}


ApplicationImpl::~ApplicationImpl()
{
    disconnect(pt_signal_pipe->out().inputReady, processSignal);
}


void ApplicationImpl::init(SelectorBase& s)
{
    pt_signal_pipe->out().setSelector(&s);
    connect(pt_signal_pipe->out().inputReady, processSignal);
}



bool ApplicationImpl::catchSystemSignal(int sig)
{
    if (sig > 0 && sig < NSIG)
    {
        struct sigaction act;

        act.sa_handler = pt_system_application_sighandler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_RESTART;

        if (-1 == ::sigaction(sig, &act, NULL))
        {
            throw SystemError("sigaction failed", PT_SOURCEINFO);
        }
		
		return true;
    }

	return false;
}


bool ApplicationImpl::raiseSystemSignal(int sig)
{
    if (sig > 0 && sig < NSIG)
    {
		if( 0 != ::raise(sig) )
        {
            throw SystemError("sigaction failed", PT_SOURCEINFO);
        }

		return true;
	}
	
	return false;
}

} // namespace System

} // namespace Pt
