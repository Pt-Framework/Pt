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
#include <iostream>
#include <sstream>
#include <stdio.h>
using namespace Pt;
using namespace Pt::System;



class TestEvent : public Pt::Event
{
    public:
        TestEvent( const char* text)
        : _text( text )
        { }

        ~TestEvent()
        { }

        Pt::Event* clone() const
        {
            return new TestEvent(*this);
        }

        const std::type_info& typeInfo() const
        {
            return typeid( *this );
        }


        std::string text() const
        { return _text; }

        void setText( const char* text )
        {
            _text = text;
        }


    private:
        std::string _text;
};


class ConsumerThread : public Thread, public Connectable
{
    public:
        ConsumerThread()
        {
            connect( _eventLoop.event , *this, &ConsumerThread::handleEvent );
        }

        EventLoop& eventLoop()
        { return _eventLoop; }

        void stop()
        {
            _eventLoop.exit();
            this->wait();
        }

        void handleEvent( const Pt::Event& event )
        {
            TestEvent* testEvent  = (TestEvent*) &event;
            printf( testEvent->text().c_str() );
            printf("-");
        }

    protected:

        virtual void run()
        {
            _eventLoop.run();
        }

    private:
        EventLoop _eventLoop;
};

class ProducerThread : public Thread
{
    public:
        ProducerThread(const char * text)
        : _event( text )
        , _text( text )
        , _stop( false)
        { }

        void stop()
        {
            _stop = true;
        }

        EventSource eventSource;

    protected:
        virtual void run()
        {
            for( size_t i = 0; i < 1000 && !_stop; i++ )
            {
                std::stringstream ss;
                ss<<_text <<" :" <<i;

               _event.setText( _text.c_str() );
                eventSource.send( _event );
            }
        }

    private:
        TestEvent   _event;
        std::string _text;
        bool        _stop;
};



int main( int argc, char* argv[] )
{
    try
    {
        //Create a consumer and two producer
        ConsumerThread* consumer = new ConsumerThread();
        ProducerThread  producer2("p2");
        ProducerThread* producer1 = new ProducerThread("p1");

        //Connect the two producer to the consumer
        producer2.eventSource.connect( consumer->eventLoop() );
        producer1->eventSource.connect( consumer->eventLoop() );

        //Start working
        consumer->start();
        producer1->start();
        producer2.start();

        //Changing the producer consumer relation
        for( int i = 0; i < 100; i++)
        {
            if( i % 2 == 0)
            {
                //Remove the consumer
                consumer->stop();
                delete consumer;

                //Add a new consumer
                printf("C-");
                consumer = new ConsumerThread();
                producer2.eventSource.connect( consumer->eventLoop() );
                producer1->eventSource.connect( consumer->eventLoop() );

                consumer->start();
            }
            else
            {
                //Remove a producer
                producer1->stop();
                producer1->wait();
                delete producer1;

                //Add a new producer
                printf("P-");
                producer1 = new ProducerThread("p1");
                producer1->eventSource.connect( consumer->eventLoop() );
                producer1->start();
            }
        }

        //Remove the producer 1
        producer1->wait();
        delete producer1;

        //Remove the producer 2
        producer2.wait();

        consumer->stop();
        delete consumer;
    }
    catch( const std::exception& e )
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
