#ifndef PT_SYSTEM_APPLICATION_IMPL_H
#define PT_SYSTEM_APPLICATION_IMPL_H

#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include <Pt/System/Process.h>
#include <Pt/System/IOError.h>
#include <Pt/System/SystemError.h>
#include <string>
#include <windows.h>

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

        static void chdir(const std::string& path)
        {
        #ifdef _WIN32_WCE

            throw AccessFailed("chdir failed");

        #else

            if( FALSE == ::SetCurrentDirectory( path.c_str() ) )
                throw SystemError("SetCurrentDirectory");

        #endif
        }

        static std::string cwd()
        {
        #ifdef _WIN32_WCE

            throw AccessFailed("cwd failed");

        #else

            char path[MAX_PATH+2];
            DWORD len = ::GetCurrentDirectory(MAX_PATH+2, path);
            return std::string(path, len);

        #endif
        }

        static std::string tmpdir()
        {
            std::string tmpDir = Process::getEnvVar("TEMP");
            if(tmpDir.length() == 0)
            {
                tmpDir = Process::getEnvVar("TMP");
            }

            return tmpDir;
        }

        static std::string rootdir()
        {
            return "c:\\";
        }
};

} // namespace System

} // namespace Pt

#endif
