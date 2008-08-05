#ifndef PT_APPLICATION_H
#define PT_APPLICATION_H

#include <Pt/Api.h>
#include <Pt/Connectable.h>
#include <stdexcept>

namespace Pt {

class Application : public Pt::Connectable
{
    public:
        explicit Application(int argc = 0, char** argv = 0)
        : _argc(argc)
        , _argv(argv)
        {
            if( getAppPtr() )
                throw std::logic_error("application already initialized");

            getAppPtr() = this;
        }

        virtual ~Application()
        {    
            getAppPtr() = 0;
        }

        Application& instance()
        {
            Application* app = getAppPtr();
            if( ! app )
                throw std::logic_error("application not initialized");

            return *app;
        }

        int argc() const
        { return _argc; }

        char** argv() const
        { return _argv; }

    private:
        static Application*& getAppPtr()
        {
            static Application* _app = 0;
            return _app;
        }

    private:
        int     _argc;
        char**  _argv;
};

} // namespace Pt

#endif
