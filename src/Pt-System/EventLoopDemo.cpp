#include <Pt/Main.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Thread.h>
#include <iostream>
#include <sstream>

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
        
        ~ConsumerThread()
        {}

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
            printf(testEvent->text().c_str() );
            printf("\n");
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
        
        ~ProducerThread()
        {  
            eventDispatcher.shutDown();
        }
        
        void stop()
        {
            _stop = true;
        }

        EventDispatcher eventDispatcher;
        
    protected:
        virtual void run()
        {
            for( size_t i = 0; i < 1000 && !_stop; i++ )         
            {   
                std::stringstream ss;
                ss<<_text <<" :" <<i;
               
               _event.setText( ss.str().c_str() );
                eventDispatcher.send( _event );
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
        producer2.eventDispatcher.connect( consumer->eventLoop() );
        producer1->eventDispatcher.connect( consumer->eventLoop() );
        
        //Start working
        consumer->start();
        producer1->start();                
        producer2.start();

        //Changing the producer consumer relation
        for( int i = 0; i < 1000; i++)
        {            
            if( i % 10 == 0)
            { 
                //Remove the consumer
                consumer->stop();
                delete consumer;
                printf("B");
                
                //Add a new consumer 
                consumer = new ConsumerThread();                                
                producer2.eventDispatcher.connect( consumer->eventLoop() );
                producer1->eventDispatcher.connect( consumer->eventLoop() );
                
                consumer->start();   
            }
            
            if( i % 9 == 0)
            {
                //Remove a producer
                producer1->stop();
                producer1->wait();
                delete producer1;
                
                //Add a new producer
                printf("A");
                producer1 = new ProducerThread("p1");
                producer1->eventDispatcher.connect( consumer->eventLoop() );
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
