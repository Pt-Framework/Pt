#ifndef PT_SYSTEM_APPLICATION_IMPL_H
#define PT_SYSTEM_APPLICATION_IMPL_H

#include <Pt/System/Api.h>

namespace Pt {

namespace System  {

class SelectorBase;

class ApplicationImpl
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        void init(SelectorBase& s);

        bool catchSystemSignal(int sig);

        bool raiseSystemSignal(int sig);

        //int signalFd() const;

};

} // namespace System

} // namespace Pt

#endif
