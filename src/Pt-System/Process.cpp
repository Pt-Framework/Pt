/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "Pt/System/Process.h"
#include "ProcessImpl.h"


namespace Pt {

namespace System {

Process::Process(const std::string& command,
                 bool suppStdIn,
                 bool suppStdOut,
                 bool suppStdErr)
{
    _impl = new ProcessImpl(command, suppStdIn, suppStdOut, suppStdErr);
}

Process::~Process()
{
    try {
        this->kill();
    } 
    catch(...) {}

    delete _impl;
}

const std::string& Process::command()
{
     return _impl->command();
}

const std::string& Process::args()
{
     return _impl->args();
}

void Process::setInput( IODevice& dev)
{
	_impl->setInput( dev);
}
		  
void Process::setOutput( IODevice& dev)
{
	_impl->setOutput( dev);
}

void Process::setErrput( IODevice& dev)
{
	_impl->setErrput( dev);
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

void Process::setArgs(const std::string& strArgs)
{
     _impl->setArgs(strArgs);
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

} // namespace System

} // namespace Pt
