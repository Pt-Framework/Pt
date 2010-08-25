/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "ApplicationImpl.h"
#include "IODeviceImpl.h"
#include "Pt/System/Pipe.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Application.h"
#include "Pt/System/SystemError.h"
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

#ifndef SA_RESTART
#define SA_RESTART 0
#endif

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
        pt_signal_pipe->in().ioimpl().sigwrite(sigNo);
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


void ApplicationImpl::init(EventLoop& loop)
{
    pt_signal_pipe->out().setParent(&loop);
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
            throw SystemError( PT_ERROR_MSG("sigaction failed") );
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
            throw SystemError( PT_ERROR_MSG("sigaction failed") );
        }

		return true;
	}
	
	return false;
}

} // namespace System

} // namespace Pt
