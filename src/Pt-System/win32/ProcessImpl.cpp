#include "ProcessImpl.h"

// include base class code
#include "ProcessImplBase.cpp"

#include <string>

namespace Pt {

namespace System {

ProcessImpl::ProcessImpl(const std::string& command)
: ProcessImplBase(command)
{}

ProcessImpl::ProcessImpl(const ProcessInfo& procInfo)
: ProcessImplBase(procInfo)
{}


void ProcessImpl::setEnvVar(const std::string& name, const std::string& value)
{
    if( 0 == SetEnvironmentVariable(name.c_str(), value.c_str()) )
    {
        throw SystemError("Set Environment Variable Error!", PT_SOURCEINFO);
    }    
}


void ProcessImpl::unsetEnvVar(const std::string& name)
{
    if( 0 == SetEnvironmentVariable(name.c_str(), NULL) )
    {
        throw SystemError("UnSet Environment Variable Error!",PT_SOURCEINFO);
    }
}


std::string ProcessImpl::getEnvVar(const std::string& name)
{
    char cp[200];
    std::string ret;
    DWORD cnt;
    cnt = GetEnvironmentVariable(name.c_str(), cp, 200);
    if( 0 == cnt )
    {
        cnt = GetLastError();
        if( ERROR_ENVVAR_NOT_FOUND ==  cnt )
        {
            return ret;
        }
        throw SystemError("Get Environment Variable Error 1!",PT_SOURCEINFO);
    }
    if(cnt<200)
    {
        ret=cp;
    }
    else
    {
        char *cp2 = new char[cnt+1];
        cnt = GetEnvironmentVariable(name.c_str(), cp2, cnt);
        if( 0 == cnt )
        {
            delete[] cp2;
            throw SystemError("Get Environment Variable Error 2!",PT_SOURCEINFO);
        }

        ret = cp2;
        delete [] cp2;
    }

    return ret;
}

} // namespace Pt

} //namespace System
