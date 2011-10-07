#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/IOError.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Connectable.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <iostream>

class GoogleWeatherClient : public Pt::Connectable
{
    public:
        GoogleWeatherClient(Pt::System::MainLoop& loop)
        : _loop(loop)
        , _client(loop, "www.google.com", 80)
        , _ts(new Pt::Utf8Codec)
        , _reader(_ts)
        , _parseFunc(&GoogleWeatherClient::onXmlBegin)
        {
            // hook up to the events of the http client. We will receive headerReceived first
            // then bodyAvailable if the reply has a body, and finally replyFinished. All Errors
            // are deferred until replyFinished, so we can handle them in a callback.
            _client.headerReceived += Pt::slot(*this,  &GoogleWeatherClient::onHeaderReceived);
            _client.bodyAvailable += Pt::slot( *this, &GoogleWeatherClient::onBodyAvailable);
            _client.replyFinished += Pt::slot( *this, &GoogleWeatherClient::onReplyFinished);
        }

        void beginRequest(const std::string& city)
        {
            _parseFunc = &GoogleWeatherClient::onXmlBegin;

            _req.clear();
            _req.url("/ig/api?weather=" + city);
            _req.method("GET");
            _req.setHeader("Host", "www.google.com");
            _req.setHeader("Accept-Charset", "UTF-8");

            _client.beginExecute(_req);
        }

        // TODO: next step is to implement a way to notify the caller that
        //       we have received a reply from the service.
        void endRequest()
        { }

    private:  
        void onHeaderReceived(Pt::Http::Client& client)
        {
            if(client.header().httpReturnCode() != 200)
            {
                std::cerr << "http error: " << client.header().httpReturnCode() 
                          << " - " << client.header().httpReturnText() << std::endl;

                // don't need to cancel here, because replyFinished will be sent
                return;
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
                        (this->*_parseFunc)(node);
                    }
                }
            }
            catch(const Pt::Xml::XmlError& error)
            {
                std::cerr << error.what() << std::endl;
                _client.cancel();
            }

            return n;
        }
        
        void onReplyFinished(Pt::Http::Client& client)
        {
            //if(_parseFunc == &GoogleWeatherClient::onXmlBegin)
            //{  
            //    // incomplete XML document
            //}

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
            }

            // of course we would't stop here in a real program. This is
            // just to demonstrate how to quit an event loop.
            std::cout << "good bye!" << std::endl;
            _loop.exit();
        }

    private:
        void onXmlBegin(const Pt::Xml::Node& node)
        {
            switch( node.type() )
            {
                case Pt::Xml::Node::StartElement:
                {
                    const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                    if(se.name() == L"xml_api_reply")
                    {
                        _parseFunc = &GoogleWeatherClient::onXmlApiReply;
                    }
                    break;
                }

                default:
                    break;
            }
        }

        void onXmlApiReply(const Pt::Xml::Node& node)
        {
            switch( node.type() )
            {
                case Pt::Xml::Node::StartElement:
                {
                    const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                    if(se.name() == L"weather")
                    {
                        _parseFunc = &GoogleWeatherClient::onXmlWeather;
                    }

                    break;
                }

                case Pt::Xml::Node::EndElement:
                {
                    const Pt::Xml::EndElement& ee = static_cast<const Pt::Xml::EndElement&>(node);
                    if(ee.name() == L"xml_api_reply")
                    {
                        _parseFunc = &GoogleWeatherClient::onXmlBegin;
                    }
                }

                default:
                    break;
            }
        }

        void onXmlWeather(const Pt::Xml::Node& node)
        {
            switch( node.type() )
            {
                case Pt::Xml::Node::StartElement:
                {
                    const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                    if(se.name() == L"current_conditions")
                    {
                        _parseFunc = &GoogleWeatherClient::onXmlCurrentConditions;
                    }

                    break;
                }

                case Pt::Xml::Node::EndElement:
                {
                    const Pt::Xml::EndElement& ee = static_cast<const Pt::Xml::EndElement&>(node);
                    if(ee.name() == L"weather")
                    {
                        _parseFunc = &GoogleWeatherClient::onXmlApiReply;
                    }
                }

                default:
                    break;
            }
        }

        void onXmlCurrentConditions(const Pt::Xml::Node& node)
        {
            switch( node.type() )
            {
                case Pt::Xml::Node::StartElement:
                {
                    const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                    if(se.name() == L"temp_c")
                    {
                        std::cout << "TEMP: " << se.attribute(L"data").narrow() << std::endl;
                        _temp = se.attribute(L"data");
                    }
                    else if(se.name() == L"condition")
                    {
                        std::cout << "CONDITION: " << se.attribute(L"data").narrow() << std::endl;
                        _condition = se.attribute(L"data");
                    }

                    break;
                }

                case Pt::Xml::Node::EndElement:
                {
                    const Pt::Xml::EndElement& ee = static_cast<const Pt::Xml::EndElement&>(node);
                    if(ee.name() == L"current_conditions")
                    {
                        _parseFunc = &GoogleWeatherClient::onXmlWeather;
                    }
                }

                default:
                    break;
            }
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Http::Client _client;
        Pt::Http::Request _req;
        Pt::TextIStream _ts;
        Pt::Xml::XmlReader _reader;
        typedef void (GoogleWeatherClient::*ParseFunc)(const Pt::Xml::Node&);
        ParseFunc _parseFunc;
        Pt::String _condition;
        Pt::String _temp;
};


int main(int argc, char** argv)
{
    try 
    {
        Pt::System::MainLoop loop;
        GoogleWeatherClient client(loop);
        client.beginRequest("Berlin");
        loop.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return 1;
}

