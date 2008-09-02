/***************************************************************************
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
 *   Copyright (C) 2007 by Marc Boris Duerner                              *
 *   Copyright (C) 2007 by Bjoern Oliver Streule                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/Selector.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/IOBuffer.h"
#include "Pt/System/IOStream.h"
#include "Pt/System/Pipe.h"
#include "Pt/System/Clock.h"
#include "Pt/System/Timer.h"
#include <string>
#include <cstring>

class IOStreamTest : public Pt::Unit::TestSuite
{
    public:
        IOStreamTest()
        : Pt::Unit::TestSuite("IOStreamTest")
        , _exit(false)
        {
            Pt::Unit::TestSuite::registerMethod( "AsyncIO", *this, &IOStreamTest::AsyncIO );
        }

        ~IOStreamTest()
        { }

        void AsyncIO()
        {
            Pt::System::Selector selector;
            Pt::System::Pipe pipe(Pt::System::IODevice::Async);

            Pt::System::IOBuffer outbuf( pipe.output() );
            outbuf.setSelector(&selector);
            connect(outbuf.outputReady, *this, &IOStreamTest::onOutput);

            inbuf.setDevice( pipe.input() );
            inbuf.setSelector(&selector);
            connect(inbuf.inputReady, *this, &IOStreamTest::onInput);

            std::cerr << "\nWriting: " << "Hello world!" << std::endl;
            outbuf.sputn("Hello world!", 12);
            std::cerr << "OUT_AVAIL: " << outbuf.out_avail() << std::endl;
            outbuf.beginFlush();

            while( ! _exit )
                selector.wait();

            std::cerr << "IN_AVAIL: " << inbuf.in_avail() << std::endl;
        }

        void onInput(Pt::System::IOBuffer& buffer)
        {
            std::cerr << "IN_AVAIL: " << buffer.in_avail() << std::endl;

            char in[20];
            size_t n = buffer.sgetn(in, 20);
            std::cerr << "Read: "; std::cerr.write(in, n ) << std::endl;
            _exit = true;
        }

        void onOutput(Pt::System::IOBuffer& buffer)
        {
            std::cerr << "Closing pipe" << std::endl;
            buffer.device()->close();
            inbuf.beginSync();
        }
    private:
        bool _exit;
        Pt::System::IOBuffer inbuf;
};

Pt::Unit::RegisterTest<IOStreamTest> register_IOStreamTest;


 class SelectorTest : public Pt::Unit::TestSuite
{
    public:
        SelectorTest()
        : Pt::Unit::TestSuite("SelectorTest")
        {
            Pt::Unit::TestSuite::registerMethod( "WaitTimer", *this, &SelectorTest::WaitTimer );
            Pt::Unit::TestSuite::registerMethod( "ReadTest", *this, &SelectorTest::ReadTest );
            Pt::Unit::TestSuite::registerMethod( "WriteTest", *this, &SelectorTest::WriteTest );
            Pt::Unit::TestSuite::registerMethod( "RemoveTest", *this, &SelectorTest::RemoveTest );
        }

        void setUp()
        {
            _result.clear();
            _counter  = 0;
        }

    private:
        void onTimeout1()
        {
            _timeval = _clock.stop();
            _counter++;
        }

        void onStreamInput(Pt::System::IOBuffer& buffer)
        {
            std::cerr << "IN_AVAIL: " << buffer.in_avail() << std::endl;

            char in[20];
            size_t n = buffer.sgetn(in, 20);
            std::cerr << "Read: "; std::cerr.write(in, n ) << std::endl;
        }

        void onStreamOutput(Pt::System::IOBuffer& buffer)
        {
            std::cerr << "Closing pipe" << std::endl;
            buffer.device()->close();
        }

        void WaitTimer()
        {
            Pt::System::Timer timer;
            connect(timer.timeout, *this, &SelectorTest::onTimeout1);
            timer.start(100);

            Pt::System::Selector selector;
            selector.add(timer);

            Pt::Timespan elapsed = 0;
            for (size_t i = 0; i < 5; i++)
            {
                _timeval  = 0;
                _clock.start();

                selector.wait();
                elapsed += _timeval;
            }

            PT_UNIT_ASSERT(5 == _counter);
            PT_UNIT_ASSERT(elapsed > 90000 * 5);
            PT_UNIT_ASSERT(elapsed < 110000 * 5);
        }

        void ReadTest()
        {
            std::string out("Hello World, where do you want to GOTO day!");

            Pt::System::Pipe pipe(Pt::System::IODevice::Async);
            pipe.output().write( out.c_str(), out.size() );

            pipe.input().beginRead(_buffer, sizeof(_buffer));
            connect(pipe.input().inputReady, *this, &SelectorTest::onRead);

            Pt::System::Selector selector;
            selector.add( pipe.input() );
            while(_result.size() < out.size())
            {
                bool avail = selector.wait(5000);
                PT_UNIT_ASSERT(avail);
            }

            this->reportMessage(_result);
            PT_UNIT_ASSERT(_result == out);
        }

        void onRead(Pt::System::IODevice& dev)
        {
            size_t sz = dev.endRead();
            _result.append(_buffer, sz);
            dev.beginRead(_buffer, sizeof(_buffer));
        }

        void WriteTest()
        {
            _out = "Hello World, where do you want to GOTO day!";

            Pt::System::Pipe pipe(Pt::System::IODevice::Async);

            _pos = 0;
            std::memcpy(_buffer, _out.c_str(),  sizeof(_buffer));
            pipe.output().beginWrite(_buffer, sizeof(_buffer));
            connect(pipe.output().outputReady, *this, &SelectorTest::onWrite);

            Pt::System::Selector selector;
            selector.add( pipe.output() );
            while(_pos < _out.size())
            {
                bool avail = selector.wait(5000);
                PT_UNIT_ASSERT(avail);
            }

            char buffer[1024];
            size_t n = pipe.input().read(buffer, sizeof(buffer));
            std::string inp(buffer, n);
            this->reportMessage(inp);

            PT_UNIT_ASSERT(_out.find(inp) != std::string::npos);
        }

        void onWrite(Pt::System::IODevice& dev)
        {
            _pos += dev.endWrite();
            size_t len= std::min(sizeof(_buffer), _out.size()-_pos);
            std::memcpy(_buffer, &(_out.c_str()[_pos]), len);
            dev.beginWrite(_buffer, len);
        }

        void RemoveTest()
        {
            std::string out("Hello World, where do you want to GOTO day!");

            Pt::System::Pipe pipe(Pt::System::IODevice::Async);
            pipe.output().write( out.c_str(), out.size() );

            pipe.input().beginRead(_buffer, sizeof(_buffer));
            connect(pipe.input().inputReady, *this, &SelectorTest::onReadRemove);

            Pt::System::Selector selector;
            selector.add( pipe.input() );

            bool avail = selector.wait(5000);
            PT_UNIT_ASSERT(avail);
            avail = selector.wait(1000);
            PT_UNIT_ASSERT(!avail);
        }

        void onReadRemove(Pt::System::IODevice& dev)
        {
            size_t sz = dev.endRead();
            _result.append(_buffer, sz);
            dev.close();
        }

    private:
        std::string _out;
        size_t _pos;
        char _buffer[10];
        std::string _result;
        Pt::Timespan _timeval;
        Pt::System::Clock _clock;
        size_t _counter;
};

Pt::Unit::RegisterTest<SelectorTest> register_SelectorTest;
