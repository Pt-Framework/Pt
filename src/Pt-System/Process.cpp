/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "Pt/System/Process.h"
#include "ProcessImpl.h"


namespace Pt {

namespace System {

Process::Process(const std::string& command)
{
    _impl = new ProcessImpl(command);
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


void Process::start()
{
     _impl->start();
}

void Process::kill()
{
     _impl->kill();
}

void Process::wait() 
{
     _impl->wait();
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
