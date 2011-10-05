#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/IOError.h>
#include <Pt/Connectable.h>
#include <iostream>

class GoogleWeatherClient : public Pt::Connectable
{
    public:
        GoogleWeatherClient(Pt::System::MainLoop& loop)
        : _loop(loop)
        , _client(loop, "www.google.com", 80)
        {
            _client.headerReceived += Pt::slot(*this,  &GoogleWeatherClient::onHeaderReceived);
            _client.bodyAvailable += Pt::slot( *this, &GoogleWeatherClient::onBodyAvailable);
            _client.replyFinished += Pt::slot( *this, &GoogleWeatherClient::onReplyFinished);
        }

        void beginRequest(const std::string& city)
        {
            _req.url("/ig/api?weather=" + city);
            _req.method("GET");
            _req.setHeader("Host", "www.google.com");
            //_req.setHeader("Connection", "close");
            _client.beginExecute(_req);
        }

        void endRequest()
        {

        }

    private:  
        void onHeaderReceived(Pt::Http::Client& client)
        {
            std::cout << "header received: " << client.header().httpReturnCode() 
                      << " " << client.header().httpReturnText() << std::endl;

            if(client.header().httpReturnCode() != 200)
            {
                std::cerr << "http error"<< std::endl;
                return;
            }
        }
        
        size_t onBodyAvailable(Pt::Http::Client& client)
        {
            std::istream& is = client.in();
            std::cout << "body data available: " << is.rdbuf()->in_avail() << std::endl;

            size_t availBytes = is.rdbuf()->in_avail();
            size_t ret = 0;
            char buf[100];
            while(ret < availBytes)
            {
                ret += is.readsome( buf, sizeof(buf) );
            }

            std::cout << "\nread bytes: " << ret << std::endl;
            return ret;
        }
        
        void onReplyFinished(Pt::Http::Client& client)
        {
            try
            {
                std::cout << "reply finished" << std::endl;
                client.endExecute();
            }
            catch(Pt::System::IOError& e)
            {
                std::cerr << "i/o error: " << e.what() << std::endl;
                client.cancel();
            }

            std::cout << "exit loop" << std::endl;
            _loop.exit();
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Http::Client _client;
        Pt::Http::Request _req;
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

