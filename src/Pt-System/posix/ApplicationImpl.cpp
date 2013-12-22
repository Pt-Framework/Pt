/*
 * Copyright (C) 2013 Marc Boris Duerner
 * 
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
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ApplicationImpl.h"
#include "PipeImpl.h"
#include "Pt/System/Pipe.h"
#include "Pt/System/Application.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/FileInfo.h"
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>


#ifndef SA_RESTART
#define SA_RESTART 0
#endif

namespace {

Pt::System::Pipe* pt_signal_pipe = 0;
static char _signalBuffer[128];

void initSignalPipe(Pt::System::EventLoop& loop)
{
    if( ! pt_signal_pipe )
    {
        pt_signal_pipe = new Pt::System::Pipe();
        pt_signal_pipe->out().setActive(loop);
        pt_signal_pipe->out().beginRead( _signalBuffer, sizeof(_signalBuffer) );
    }
}

void processSignal(Pt::System::IODevice& device)
{
    try
    {
        std::size_t n = device.endRead();

        int sigNo = 0;
        char* it = _signalBuffer;
        char* last = &_signalBuffer[ n- sizeof(sigNo) ];
        while(it <= last)
        {
            memcpy(&sigNo, it, sizeof(sigNo));
            Pt::System::Application::instance().systemSignal().send(sigNo);
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
        pt_signal_pipe->impl()->in().sigwrite(sigNo);
    }
}

namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{   
}


ApplicationImpl::~ApplicationImpl()
{
    pt_signal_pipe->out().inputReady() -= slot(processSignal);
}


void ApplicationImpl::init(EventLoop& loop)
{
    ::initSignalPipe(loop);
    pt_signal_pipe->out().inputReady() += slot(processSignal);
}


bool ApplicationImpl::ignoreSystemSignal(int sig)
{
    if (sig > 0 && sig < NSIG)
    {
        if( SIG_ERR == ::signal(sig, SIG_IGN) )
        {
            throw SystemError("signal() failed");
        }

        return true;
    }

    return false;
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


void ApplicationImpl::chdir(const std::string& path)
{
    if( -1 == ::chdir(path.c_str()) )
    {
        throw SystemError("chdir");
    }
}


std::string ApplicationImpl::cwd()
{
    const long size = pathconf(".", _PC_PATH_MAX);
    if(size == -1)
        throw SystemError( PT_ERROR_MSG("pathconf() failed for .") );

    std::vector<char> buffer(size);
    if( ! getcwd(&buffer[0], size) )
    {
        throw SystemError("getcwd");
    }

    return std::string( &buffer[0] );
}


std::string ApplicationImpl::tmpdir()
{
    const char* tmpdir = getenv("TEMP");

    if(tmpdir)
    {
        return tmpdir;
    }

    tmpdir = getenv("TMP");
    if(tmpdir)
    {
        return tmpdir;
    }

    return FileInfo::type("/tmp") == FileInfo::Directory ? "/tmp" 
                                                           : FileInfo::curdir();
}
        
std::string ApplicationImpl::rootdir()
{
    return "/";
}


void ApplicationImpl::setEnvVar(const std::string& name, const std::string& value)
{
    if( 0 > ::setenv(name.c_str(), value.c_str(), 1) )
    {
        throw SystemError( PT_ERROR_MSG("setenv failed") );
    }
}


void ApplicationImpl::unsetEnvVar(const std::string& name)
{
    ::unsetenv( name.c_str() );
}


std::string ApplicationImpl::getEnvVar(const std::string& name)
{
    std::string ret;
    const char* cp = ::getenv(name.c_str());
    if( NULL == cp )
    {
        return ret;
    }
    ret = cp;
    return ret;
}


unsigned long ApplicationImpl::usedMemory()
{
    struct rusage usage;
    int r =  getrusage(RUSAGE_SELF, &usage);
    if( r == -1)
        throw SystemError( PT_ERROR_MSG("getrusage failed") );

    return usage.ru_idrss;
}

} // namespace System

} // namespace Pt
