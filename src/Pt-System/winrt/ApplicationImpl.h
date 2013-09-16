#ifndef PT_SYSTEM_APPLICATION_IMPL_H
#define PT_SYSTEM_APPLICATION_IMPL_H

#include "Pt/WinVer.h"
#include "Pt/System/Api.h"

namespace Pt {

namespace System  {

class EventLoop;

class ApplicationImpl
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        void init(EventLoop& s);

        bool ignoreSystemSignal(int sig);

        bool catchSystemSignal(int sig);

        bool raiseSystemSignal(int sig);
};

} // namespace System

} // namespace Pt

#endif
