/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
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


class TestThread
{
    public:
        TestThread()
        : _loop()
        , _thread( callable(_loop, &Pt::System::MainLoop::run) )
        {
            _thread.start();
        }

        ~TestThread()
        {
            // NOTE: must end thread before loop is destructed
            _thread.join();
        }

    private:
        Pt::System::MainLoop _loop;
        Pt::System::AttachedThread _thread;
};


int main( int argc, char* argv[] )
{
    try
    {
        Pt::System::Application app;

        TestThread thread;

        app.run();
    }
    catch( const std::exception& e )
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
