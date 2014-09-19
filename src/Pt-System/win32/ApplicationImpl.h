#ifndef PT_SYSTEM_APPLICATION_IMPL_H
#define PT_SYSTEM_APPLICATION_IMPL_H

#include "PathImpl.h"
#include <Pt/WinVer.h>
#include <Pt/System/Api.h>
#include <Pt/System/Path.h>
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

        static void chdir(const Path& path)
        {
        #ifdef _WIN32_WCE

            throw AccessFailed("chdir failed");

        #else

            if( FALSE == ::SetCurrentDirectoryW( path.impl()->c_str() ) )
                throw SystemError("SetCurrentDirectory");

        #endif
        }

        static Path cwd()
        {
            
        #ifdef _WIN32_WCE

            throw AccessFailed("cwd failed");

        #else

            Path path;

            const std::size_t wbuflen = MAX_PATH + 2;
            wchar_t wpath[wbuflen];
            DWORD len = ::GetCurrentDirectoryW(wbuflen, wpath);

            if(len > 0 && len < wbuflen)
                path.impl()->assign(wpath);
            
            return path;

        #endif
        }

        static Path tmpdir()
        {
            Path path;

            const std::size_t wbuflen = MAX_PATH + 1;
            wchar_t wbuf[wbuflen];
            
            DWORD len = ::GetEnvironmentVariableW(L"TEMP", wbuf, wbuflen);
            if( len != 0 && len < wbuflen )
            {
                path.impl()->assign(wbuf);
            }
            else
            {
                len = ::GetEnvironmentVariableW(L"TMP", wbuf, wbuflen);
                if( len != 0 && len < wbuflen )
                {
                    path.impl()->assign(wbuf);
                }
                else
                {
                    path.assign( path.curdir() );
                }
            }

            return path;
        }

        static Path rootdir()
        {
            Path path;
            path.impl()->assign(L"c:\\");
            return path;
        }

        static unsigned long usedMemory();

        static void setEnvVar(const std::string& name, const std::string& value);

        static void unsetEnvVar(const std::string& name);

        static std::string getEnvVar(const std::string& name);
};

} // namespace System

} // namespace Pt

#endif
