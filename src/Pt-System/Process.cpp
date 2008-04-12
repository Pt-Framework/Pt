/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "Pt/System/Process.h"
#include "ProcessImpl.h"


namespace Pt {

namespace System {


ProcessInfo::ProcessInfo( const std::string& command)
    : m_command( command)
    , m_devInput(0)
    , m_devOutput(0)
    , m_devError(0)
{ }

const std::string& ProcessInfo::command() const
{
	return m_command;
}

void ProcessInfo::addArgument( const std::string& argument)
{
    m_argList.push_back( argument);
}

void ProcessInfo::setStdInput( IODevice* dev)
{
    m_mask.set(0);
    m_devInput = dev;
}

IODevice* ProcessInfo:: getStdInput() const
{
    return m_devInput;
}

 
void ProcessInfo::setStdOutput( IODevice* dev)
{
    m_mask.set(1);
    m_devOutput = dev;
}

IODevice* ProcessInfo::getStdOutput() const
{
    return m_devOutput;
}

 
void ProcessInfo::setStdError( IODevice* dev)
{
    m_mask.set(2);
    m_devError = dev;
}

IODevice* ProcessInfo::getStdError() const
{
    return m_devError;
}

std::bitset<3> ProcessInfo::mask() const
{
    return m_mask;
}

unsigned ProcessInfo::argCount() const
{
    return m_argList.size();
}

std::string ProcessInfo::getArgument( unsigned idx) const
{
    return (idx < m_argList.size()) ? m_argList[idx] : std::string();
}

// -------------------

Process::Process(const std::string& command)
{
    _impl = new ProcessImpl(command);
}

Process::Process( const ProcessInfo& procInfo)
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

const std::string& Process::command()
{
     return _impl->command();
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

} // namespace System

} // namespace Pt
