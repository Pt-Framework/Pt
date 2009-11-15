#if !defined(PT_ProcessImpl_h)
#define PT_ProcessImpl_h

#include "Pt/System/Process.h"
#include "Pt/System/SystemError.h"
#include "Pt/NonCopyable.h"
#include "Pt/System/Pipe.h"
#include <cstdlib>
#include <sstream>
#include <unistd.h>

namespace Pt {

namespace System {

class ProcessImpl : private NonCopyable
{
    public:
        ProcessImpl(const ProcessInfo& procInfo);

        ~ProcessImpl();

        const ProcessInfo& procInfo() const
        { return _procInfo; }

        Process::State state() const
        { return _state; }

        void start();

        void kill();

        int wait();

        bool tryWait(int& status);

        IODevice* stdInput()
        { return _stdInput; }

        IODevice* stdOutput()
        { return _stdOutput; }

        IODevice* stdError()
        { return _stdError; }

        static void setEnvVar(const std::string& name, const std::string& value);

        static void unsetEnvVar(const std::string& name);

        static std::string getEnvVar(const std::string& name);

        static void sleep(size_t msecs);

        static unsigned long usedMemory();

    private:
        pid_t _pid;
        Process::State _state;
        ProcessInfo _procInfo;

        IODevice* _stdInput;
        IODevice* _stdOutput;
        IODevice* _stdError;

        Pipe* _stdinPipe;
        Pipe* _stdoutPipe;
        Pipe* _stderrPipe;
};

} // namespace System

} // namespace Pt

#endif // PT_ProcessImpl_h
