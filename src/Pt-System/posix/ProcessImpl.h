#if !defined(PT_ProcessImpl_h)
#define PT_ProcessImpl_h

#include <cstdlib>
#include <sstream>
#include <unistd.h>

#include "Pt/System/Process.h"


namespace Pt {

namespace System {

class PT_API ProcessImpl
{
    public:
        ProcessImpl(const std::string& command);
        ProcessImpl(const ProcessInfo& procInfo);


        ~ProcessImpl();

        static void setEnvVar(const std::string& name, const std::string& value)
        {
            if( 0 > setenv(name.c_str(),value.c_str(),1) )
            {
                throw SystemError("not Enough Memory in Environment!",PT_SOURCEINFO);
            }
        }

        static void unsetEnvVar(const std::string& name)
        {
            unsetenv(name.c_str());
        }

        static std::string getEnvVar(const std::string& name)
        {
            std::string ret;
			const char* cp = std::getenv(name.c_str());
            if( NULL == cp )
            {
                return ret;
            }
            ret = cp;
            return ret;
        }

        static void sleep(size_t milliSec){
            usleep(milliSec*1000);
        }
    
        const std::string& command();
    
        void start();
    
        void kill();
    
        int wait();
    
    private:
        pid_t m_pid;
        std::string m_command;
        std::string m_args;

        std::bitset<3> m_mask;
        
		IODevice* m_devIn;
		IODevice* m_devOut;
		IODevice* m_devErr;
 
};

} // namespace System

} // namespace Pt

#endif // PT_ProcessImpl_h
