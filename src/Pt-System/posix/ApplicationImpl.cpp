#include "ApplicationImpl.h"
#include "Pt/System/SystemError.h"
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

namespace {

    int signalPipe[2] = {-1, -1};

    void initSignalPipe()
    {
        if (signalPipe[0] == -1)
        {
            if (pipe(signalPipe) == -1)
            {
                throw std::runtime_error("error creating signal pipe");
            }

            int flags = ::fcntl(signalPipe[0], F_GETFL);
            if(-1 == flags)
                throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

            int ret = ::fcntl(signalPipe[0], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);

            flags = ::fcntl(signalPipe[1], F_GETFL);
            if(-1 == flags)
                throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

            ret = ::fcntl(signalPipe[1], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);

        }
    }

}


extern "C" void pt_system_application_sighandler(int sigNo)
{
    if (signalPipe[1] != -1)
        write(signalPipe[1], &sigNo, sizeof(sigNo));
}


namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{
    ::initSignalPipe();
}


void ApplicationImpl::catchSystemSignal(int sig)
{
    if (sig > 0 && sig < NSIG)
    {
        struct sigaction act;

        act.sa_handler = pt_system_application_sighandler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_RESTART;

        if (-1 == sigaction(sig, &act, NULL))
        {
            throw SystemError("sigaction failed", PT_SOURCEINFO);
        }
    }
}


int ApplicationImpl::getSignalFd() const
{
    return signalPipe[0];
}

} // namespace System

} // namespace Pt
