/***************************************************************************
 *   Copyright (C) 2006-2008 by Marc Boris Duerner                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ProcessImpl.h"
#include "Pt/System/Process.h"

namespace Pt {

namespace System {

ProcessInfo::ProcessInfo(const std::string& command)
: _command(command)
, _stdin(0)
, _stdinClosed(false)
, _stdout(0)
, _stdoutClosed(false)
, _stderr(0)
, _stderrClosed(false)
{
}


const std::string& ProcessInfo::command() const
{
    return _command;
}


void ProcessInfo::addArg(const std::string& argument)
{
    _args.push_back(argument);
}


void ProcessInfo::setStdInput(IODevice* dev)
{
    if( dev )
        _stdinClosed = false;
    else
        _stdinClosed = true;

    _stdin = dev;
}


IODevice* ProcessInfo::stdInput() const
{
    return _stdin;
}


bool ProcessInfo::stdInputClosed() const
{
    return _stdinClosed;
}


void ProcessInfo::setStdOutput(IODevice* dev)
{
    if( dev )
        _stdoutClosed = false;
    else
        _stdoutClosed = true;

    _stdout = dev;
}


IODevice* ProcessInfo::stdOutput() const
{
    return _stdout;
}


bool ProcessInfo::stdOutputClosed() const
{
    return _stdoutClosed;
}


void ProcessInfo::setStdError(IODevice* dev)
{
    if( dev )
        _stderrClosed = false;
    else
        _stderrClosed = true;

    _stderr = dev;
}


IODevice* ProcessInfo::stdError() const
{
    return _stderr;
}


bool ProcessInfo::stdErrorClosed() const
{
    return _stderrClosed;
}


unsigned ProcessInfo::argCount() const
{
    return _args.size();
}


const std::string& ProcessInfo::arg(unsigned idx) const
{
    return _args.at(idx);
}


Process::Process(const std::string& command)
{
    _impl = new ProcessImpl( ProcessInfo(command) );
}


Process::Process(const ProcessInfo& procInfo)
{
    _impl = new ProcessImpl( procInfo);
}


Process::~Process()
{
    try {
        this->kill();
    } 
    catch(...) {}

    delete _impl;
}


const ProcessInfo& Process::procInfo() const
{
    return _impl->procInfo();
}


void Process::start()
{
     _impl->start();
}


void Process::kill()
{
     _impl->kill();
}


int Process::wait() 
{
    return _impl->wait();
}


void Process::setEnvVar(const std::string& name, const std::string& value)
{
    ProcessImpl::setEnvVar(name, value);
}


void Process::unsetEnvVar(const std::string& name)
{
     ProcessImpl::unsetEnvVar(name);
}


std::string Process::getEnvVar(const std::string& name)
{
     return ProcessImpl::getEnvVar(name);
}


void Process::sleep(size_t milliSec)
{
     ProcessImpl::sleep(milliSec);
}


unsigned long Process::usedMemory()
{
     return ProcessImpl::usedMemory();
}

} // namespace System

} // namespace Pt
