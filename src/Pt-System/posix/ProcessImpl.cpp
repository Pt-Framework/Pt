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

#include "ProcessImpl.h"
#include "PipeImpl.h"
#include <vector>
#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <cstring> // strerror()
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace Pt {

namespace System {

ProcessImpl::ProcessImpl(const ProcessInfo& procInfo)
: _state(Process::Ready)
, _procInfo(procInfo)
, _stdInput(0)
, _stdOutput(0)
, _stdError(0)
, _stdinPipe(0)
, _stdoutPipe(0)
, _stderrPipe(0)
{
}


ProcessImpl::~ProcessImpl()
{
    delete _stdinPipe;
    delete _stdoutPipe;
    delete _stderrPipe;
}


void ProcessImpl::start()
{
    if (_state == Process::Running)
        throw std::runtime_error("invalid state in process start");

    delete _stdinPipe;
    _stdinPipe = 0;
    _stdInput = 0;

    delete _stdoutPipe;
    _stdoutPipe = 0;
    _stdOutput = 0;

    delete _stderrPipe;
    _stderrPipe = 0;
    _stdError = 0;

    if (_procInfo.isStdInputRedirected())
    {
        _stdinPipe = new Pipe();
        _stdInput = &_stdinPipe->in();
    }

    if (_procInfo.isStdOutputRedirected())
    {
        _stdoutPipe = new Pipe();
        _stdOutput = &_stdoutPipe->out();
    }

    if (_procInfo.isStdErrorRedirected() )
    {
        _stderrPipe = new Pipe();
        _stdError = &_stderrPipe->out();
    }
    else if (_procInfo.isStdErrorAsOutput())
    {
        _stdError = &_stdinPipe->out();
    }

    _state = Process::Running;
    _pid = fork();

    if( _pid < 0 )
    {
        _pid = -1;
        _state = Process::Failed;
        throw SystemError( PT_ERROR_MSG("fork failed") );
    }

    if( _pid == 0) // child Process
    {
        // check detach state

        if (_procInfo.isDetached())
        {
            _pid = fork();
            if( _pid < 0 )
            {
                fprintf(stderr, "%s\n", std::strerror(errno));
                std::exit(-1);
            }
            else if (_pid > 0)
            {
                // child
                std::exit(0);
            }

            // child child
        }

        // redirect stdin

        if (_procInfo.isStdInputClosed())
        {
            std::fclose(stdin);
        }
        else if (_procInfo.isStdInputRedirected())
        {
            _stdinPipe->in().close();
            _stdinPipe->impl()->redirectStdin();
        }
        //else if (_procInfo.stdInput())
        //{
            //dup2(_procInfo.stdInput()->ioimpl().fd(), STDIN_FILENO);
        //}

        // redirect stdout

        if (_procInfo.isStdOutputClosed())
        {
            std::fclose(stdout);
        }
        else if (_procInfo.isStdOutputRedirected())
        {
            _stdoutPipe->out().close();
            _stdoutPipe->impl()->redirectStdout();
        }
        //else if (_procInfo.stdOutput())
        //{
            //dup2(_procInfo.stdOutput()->ioimpl().fd(), STDOUT_FILENO);
        //}

        // redirect stderr

        if (_procInfo.isStdErrorClosed())
        {
            std::fclose(stderr);
        }
        else if (_procInfo.isStdErrorRedirected())
        {
            _stderrPipe->impl()->redirectStderr();
        }
        else if (_procInfo.isStdErrorAsOutput())
        {
            _stdoutPipe->impl()->redirectStderr(false);
        }
        //else if (_procInfo.stdError())
        //{
            //dup2(_procInfo.stdError()->ioimpl().fd(), STDERR_FILENO);
        //}

        // exec

        std::vector< std::vector<char> > args;

        const std::string& c = _procInfo.command();
        std::vector<char> cmd( c.begin(), c.end() );
        cmd.push_back('\0');
        args.push_back(cmd);

        for( unsigned i = 0; i < _procInfo.argCount(); i++)
        {
            const std::string& a = _procInfo.arg(i);
            std::vector<char> arg(a.begin(), a.end());
            arg.push_back('\0');
            args.push_back(arg);
        }

        std::vector<char*> argptrs;
        for( unsigned n = 0; n < args.size(); n++)
        {
            std::vector<char>& a = args[n];
            argptrs.push_back( &a[0] );
        }
        argptrs.push_back( 0 );

        if( 0 > execvp(argptrs[0], &argptrs[0]))
        {
            std::exit(-1);
        }
    }
    else if (_procInfo.isDetached())
    {
        // wait for 1st child to exit
        wait();
        _pid = 0;
    }
    else
    {
        // parent

        // check for open pipes

        if (_procInfo.isStdInputRedirected())
            _stdinPipe->out().close();

        if (_procInfo.isStdOutputRedirected())
            _stdoutPipe->in().close();

        if (_procInfo.isStdErrorRedirected())
            _stderrPipe->in().close();
    }
}


void ProcessImpl::kill()
{
    int iStatus;
    if( 0 > ::kill(_pid, SIGINT)
        || 0 > ::waitpid(_pid, &iStatus, WNOHANG|WUNTRACED) )
    {
        throw SystemError(std::strerror(errno));
    }

    _state = Process::Finished;
    _pid = 0;
}


int ProcessImpl::wait()
{
    int iStatus;
    if( 0 > waitpid(_pid, &iStatus, WUNTRACED) )
    {
        _state = Process::Failed;
        throw SystemError( PT_ERROR_MSG("waitpid failed") );
    }

    _state = Process::Finished;
    _pid = 0;

    if (!WIFEXITED(iStatus))
        throw ProcessFailed();

    return WEXITSTATUS(iStatus);
}


bool ProcessImpl::tryWait(int& status)
{
    int iStatus;
    pid_t ret = waitpid(_pid, &iStatus, WUNTRACED|WNOHANG);
    if (0 > ret)
    {
        _state = Process::Failed;
        throw SystemError(std::strerror(errno));
    }

    if (ret == 0)
        return false;

    _state = Process::Finished;
    _pid = 0;

    if (!WIFEXITED(status))
        throw ProcessFailed();

    status = WEXITSTATUS(iStatus);
    return true;
}

} // namespace Pt

} //namespace System
