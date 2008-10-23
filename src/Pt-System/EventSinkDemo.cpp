#include <Pt/Main.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/EventSource.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Application.h>
#include <iostream>


class TestEvent : public Pt::Event
{
    public:
        TestEvent(const std::string& text = "")
        : _text( text )
        { }

        Pt::Event& clone(Pt::Allocator& allocator) const
        {
            void* ev = allocator.allocate( sizeof(TestEvent)) ;
            return *(new (ev)TestEvent(*this));
        }

        void destroy(Pt::Allocator& allocator)
        {
            allocator.deallocate(this, sizeof(TestEvent));
        }

        const std::type_info& typeInfo() const
        {
            return typeid(TestEvent);
        }

        const std::string& text() const
        { 
            return _text; 
        }

        void setText(const std::string& text)
        {
            _text = text;
        }

    private:
        std::string _text;
};


class TestThread : public Pt::System::Thread
{
    public:
        TestThread()
        {
            init(_loop);
        }

        ~TestThread()
        {
            // NOTE: must end thread before loop is destructed
            this->wait();
        }

    private:
        Pt::System::EventLoop _loop;
};


int main( int argc, char* argv[] )
{
    try
    {
        Pt::System::Application app;

        TestThread thread;
        thread.start();

        app.run();
    }
    catch( const std::exception& e )
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
