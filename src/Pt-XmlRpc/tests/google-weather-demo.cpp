#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/IOError.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
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
                // cheesy parsing code. Need to replace this once I have the requirements
                while(true)
                {
                    std::streamsize m = _ts.buffer().import();
                    if( ! m )
                        break;
        
                    n += m;
        
                    while( _reader.advance() ) // Xml::ParseError
                    {
                        const Pt::Xml::Node& node = _reader.get();
                        if(node.type() == Pt::Xml::Node::StartElement)
                        {
                            const Pt::Xml::StartElement& se = static_cast<const Pt::Xml::StartElement&>(node);
                            if(se.name() == L"temp_c")
                            {
                                const Pt::String& temp = se.attribute(L"data");
                                std::cout << "The temperature is: " << temp.narrow() << std::endl;
                            }
                        }
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
        Pt::System::EventLoop& _loop;
        Pt::Http::Client _client;
        Pt::Http::Request _req;
        Pt::TextIStream _ts;
        Pt::Xml::XmlReader _reader;
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

