#include <Pt/Main.h>
#include <Pt/System/IOError.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/IOMonitor.h>
#include <iostream>
#include <memory>
#include <cerrno>
#include <unistd.h>


namespace Pt {

namespace System {

class IODeviceImpl
{
    public:
        IODeviceImpl();

        virtual ~IODeviceImpl();

        virtual int fd() const = 0;
};


class Pipe
{
    public:
        class PipeIODevice : public Pt::System::IODevice, public IODeviceImpl
        {
            public:
                PipeIODevice(int fd)
                : _fd(fd)
                { this->setValid(true); }

                ~PipeIODevice()
                {
                    try
                    {
                        this->close();
                    }
                    catch(...)
                    {}

                }

                virtual IODeviceImpl* impl()
                { return this; }

                int fd() const
                { return _fd;}

            protected:
                void _close()
                { ::close(_fd); }

                virtual size_t _read(char* buffer, size_t count, bool& eof)
                {
                    eof = false;
                    ssize_t ret = 0;

                    while(true)
                    {
                        ret = ::read(_fd, (void*)buffer, count);
                        eof = (ret == 0) ;

                        if(ret >= 0)
                            break;

                        if(errno == EINTR) // signal interrupt
                            continue;

                        if(errno == EAGAIN) // non-blocking and no data yet
                            return 0;

                        throw IOError("Could not read from file handle", PT_SOURCEINFO);
                    }

                    return ret;
                }

                virtual size_t _write(const char* buffer, size_t count)
                {
                    ssize_t ret = 0;

                    while(true)
                    {
                        ret = ::write(_fd, (const void*)buffer, count);

                        if(ret >= 0)
                            break;

                        if(errno == EINTR) // signal interrupt
                            continue;

                        if(errno == EAGAIN) // non-blocking and no data yet
                            return 0;

                        throw IOError("Could not read from file handle", PT_SOURCEINFO);
                    }

                    return ret;
                }


            private:
                int _fd;
        };

    public:
        Pipe()
        : _input(0)
        , _output(0)
        {
            int fds[2];
            if(-1 == ::pipe(fds) )
                throw OpenFailed("pipe", PT_SOURCEINFO);

            std::auto_ptr<PipeIODevice> in( new PipeIODevice( fds[0] ) );
            std::auto_ptr<PipeIODevice> out( new PipeIODevice( fds[1] ) );

            _input = in.release();
            _output = out.release();
        }

        ~Pipe()
        {
            delete _input;
            delete _output;
        }

        IODevice& input()
        { return *_input; }

        IODevice& output()
        { return *_output; }

    private:
        PipeIODevice* _input;
        PipeIODevice* _output;
};

}

}


int main( int argc, char* argv[] )
{
    Pt::System::Pipe pipe;

    const char* out = "Hello World!";
    pipe.output().write(out, 12);

    Pt::System::IOMonitor monitor;
    monitor.addDevice( pipe.input(), Pt::System::IODevice::WaitInput );
    bool ret = monitor.wait();

    std::cerr << "Data: " << std::boolalpha << ret << std::endl;
    char buffer[20];
    size_t sz = pipe.input().read(buffer, 20);
    std::cerr.write(buffer, sz);

    monitor.removeDevice( pipe.input() );
    std::cerr << std::endl;
    return 0;
}
