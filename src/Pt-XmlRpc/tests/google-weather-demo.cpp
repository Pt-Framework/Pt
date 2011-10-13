#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/Application.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Timer.h>
#include <Pt/System/EventSink.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Connectable.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Event.h>
#include <iostream>
#include <ctime>

// This event object can be passed accross thread borders, stored in
// event queues and dispatched by type within the reveiving thread
class WeatherEvent : public Pt::BasicEvent<WeatherEvent>
{
    public:
        WeatherEvent(const std::string& city)
        : _city(city)
        , _error(false)
        { }

        const std::string& city() const
        { return _city; }

        const Pt::String& temperature() const
        { return _temp; }

        void setTemperature(const Pt::String& temp)
        { _temp = temp; }

        const Pt::String& condition() const
        { return _condition; }

        void setCondition(const Pt::String& cond)
        { _condition = cond; }

        bool failed() const
        { return _error; }

        void setFailed(bool hasFailed)
        { _error = hasFailed; }

        void clear()
        { 
            _temp.clear(); 
            _condition.clear(); 
            _error = false;
        }

    private:
        std::string _city;
        Pt::String _temp;
        Pt::String _condition;
        bool _error;
};


class GoogleWeatherClient : public Pt::Connectable
{
    public:
        GoogleWeatherClient(const std::string& city)
        : _thread(0)
        , _client(_loop, "www.google.com", 80)
        , _sink(0)
        , _ts(new Pt::Utf8Codec)
        , _reader(_ts)
        , _parseFunc(&GoogleWeatherClient::onXmlBegin)
        , _weather(city)
        {
            // hook up to the events of the http client. We will receive headerReceived first
            // then bodyAvailable if the reply has a body, and finally replyFinished. All Errors
            // are deferred until replyFinished, so we can handle them in a callback.
            _client.headerReceived += Pt::slot(*this,  &GoogleWeatherClient::onHeaderReceived);
            _client.bodyAvailable += Pt::slot( *this, &GoogleWeatherClient::onBodyAvailable);
            _client.replyFinished += Pt::slot( *this, &GoogleWeatherClient::onReplyFinished);

            // the timeout signal is called when the interval has expired
            _timer.timeout += Pt::slot(*this, &GoogleWeatherClient::beginRequest);
            _loop.add(_timer); // timers are managed by a loop

            // run member function in a worker thread, this does not start the thread
            _thread = new Pt::System::AttachedThread( Pt::callable(*this, &GoogleWeatherClient::run) );
        }

        ~GoogleWeatherClient()
        {
            // exit the loop, so the thread can be joined
            _loop.exit();
            delete _thread;
        }

        void start(Pt::System::EventSink& sink)
        {
            // thread starts to run
            _thread->start();
            _sink = &sink;
        }

        void exit()
        {
            // it is thread safe to call exit on a loop used by another thread
            // this is the way to cooperatively end the thread...
            _loop.exit();

            // ...so we can join it without being blocked for a long time
            _thread->join();
        }

    private:
        void run()
        {
            try
            {
                // initial request to google weather
                this->beginRequest();
    
                // the rest of this thread happens in the slots
                _loop.run();
            }
            catch(const std::exception& e)
            {
                std::cerr << "unexpected error: " << e.what() << std::endl;
            }
        }

        void beginRequest()
        {
            _timer.stop();

            // cancel and reset previous request
            _client.cancel();
            _weather.clear();
            _parseFunc = &GoogleWeatherClient::onXmlBegin;

            _req.clear();
            _req.url("/ig/api?weather=" + _weather.city());
            _req.method("GET");
            _req.setHeader("Host", "www.google.com");
            _req.setHeader("Accept-Charset", "UTF-8");

            // begin a new request, non-blocking, incoming data is processed
            // via slots: onHeaderReceived, onBodyAvailable, onReplyFinished
            _client.beginExecute(_req);
        }

        void onHeaderReceived(Pt::Http::Client& client)
        {
            if(client.header().httpReturnCode() != 200)
            {
                std::cerr << "unexpected http return: " << client.header().httpReturnCode() 
                          << " - " << client.header().httpReturnText() << std::endl;

                // don't need to cancel here, because replyFinished will be sent
            }

            // we have the expected reply and prepare our text stream to process
            // the incomimg body
            _ts.attach( client.in() );
        }

        size_t onBodyAvailable(Pt::Http::Client& client)
        {
            // this callback can be called multiple times. Each time we
            // advance the XML parser.

            // need to return the number of bytes consumed.
            std::size_t n = 0;

            try
            { 
                while(true)
                {
                    std::streamsize m = _ts.buffer().import();
                    if( ! m )
                        break;
        
                    n += m;
        
                    while( _reader.advance() ) // Xml::ParseError
                    {
                        const Pt::Xml::Node& node = _reader.get();

                        // _parseFunc == 0 means end of document, we ignore the rest...
                        if(_parseFunc)
                            (this->*_parseFunc)(node);
                    }
                }
            }
            catch(const Pt::Xml::XmlError& error)
            {
                std::cerr << "XML error: " << error.what() << std::endl;
                _client.cancel();
                _weather.setFailed(true);

                // notify parent thread
                if(_sink)
                    _sink->commitEvent(_weather);
            }

            return n;
        }
        
        void onReplyFinished(Pt::Http::Client& client)
        {
            if(_parseFunc != 0)
            {  
                std::cerr << "inclomplete XML data" << std::endl;
                _weather.setFailed(true);
            }

            // we are being called when we have finished reading the body or
            // if an error occured.
            try
            {
                client.endExecute();
            }
            catch(Pt::System::IOError& e)
            {
                std::cerr << "i/o error: " << e.what() << std::endl;
                client.cancel();
                _weather.setFailed(true);
            }

            // notify parent thread
            if(_sink)
                _sink->commitEvent(_weather);


            // next request in 2000 ms
            _timer.start(2000);
        }

    private:
        void onXmlBegin(const Pt::Xml::Node& node)
        {
            if( node.type() == Pt::Xml::Node::StartElement)
            {
                const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                if(se.name() == L"xml_api_reply")
                    _parseFunc = &GoogleWeatherClient::onXmlApiReply;
            }
        }

        void onXmlApiReply(const Pt::Xml::Node& node)
        {
            if( node.type() == Pt::Xml::Node::StartElement)
            {
                const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                if(se.name() == L"weather")
                    _parseFunc = &GoogleWeatherClient::onXmlWeather;
            }

            if( node.type() == Pt::Xml::Node::EndElement)
            {
                const Pt::Xml::EndElement& ee = static_cast<const Pt::Xml::EndElement&>(node);
                if(ee.name() == L"xml_api_reply")
                    _parseFunc = 0;
            }
        }

        void onXmlWeather(const Pt::Xml::Node& node)
        {
            if( node.type() == Pt::Xml::Node::StartElement)
            {
                const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                if(static_cast<const Pt::Xml::StartElement&>(node).name() == L"current_conditions")
                    _parseFunc = &GoogleWeatherClient::onXmlCurrentConditions;
            }

            if( node.type() == Pt::Xml::Node::EndElement)
            {
                const Pt::Xml::EndElement& ee = static_cast<const Pt::Xml::EndElement&>(node);
                if(ee.name() == L"weather")
                    _parseFunc = &GoogleWeatherClient::onXmlApiReply;
            }
        }

        void onXmlCurrentConditions(const Pt::Xml::Node& node)
        {
            if( node.type() == Pt::Xml::Node::StartElement)
            {
                const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                if(se.name() == L"temp_c")
                    _weather.setTemperature( se.attribute(L"data") );
                else if(se.name() == L"condition")
                    _weather.setCondition( se.attribute(L"data") );
            }

            if( node.type() == Pt::Xml::Node::EndElement)
            {
                const Pt::Xml::EndElement& ee = static_cast<const Pt::Xml::EndElement&>(node);
                if(ee.name() == L"current_conditions")
                    _parseFunc = &GoogleWeatherClient::onXmlWeather;
            }
        }

    private:
        Pt::System::MainLoop _loop;
        Pt::System::AttachedThread* _thread;
        Pt::System::Timer _timer;
        Pt::Http::Client _client;
        Pt::System::EventSink* _sink;
        Pt::Http::Request _req;
        Pt::TextIStream _ts;
        Pt::Xml::XmlReader _reader;
        typedef void (GoogleWeatherClient::*ParseFunc)(const Pt::Xml::Node&);
        ParseFunc _parseFunc;
        WeatherEvent _weather;
};


class WeatherApplet : public Pt::System::Application
{
    public:
        WeatherApplet()
        : _client("Berlin")
        {
            // client will report WeatherEvents to this loop
            _client.start( this->loop() );

            // event loop dispatches WeatherEvents to a slot
            this->loop().event += Pt::slot(*this, &WeatherApplet::printWeatherInfo);
        }

    private:
        void printWeatherInfo(const WeatherEvent& wev)
        {
            std::cerr << "Temperture: " << wev.temperature().narrow() << std::endl;
            std::cerr << "Condition: " << wev.condition().narrow() << std::endl;
            std::cerr << std::endl;
        }

    private:
        GoogleWeatherClient _client;
};


int main(int argc, char** argv)
{
    try 
    {
        WeatherApplet app;
        app.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return 1;
}

