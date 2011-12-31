/*
 * Copyright (C) 2007 by Laurentiu-Gheorghe Crisan
 * Copyright (C) 2007 by Marc Boris D�rner
 * Copyright (C) 2007 by Bjoern Oliver Streule
 * 
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
#include "Pt/System/Pipe.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/StreamBuffer.h"
#include "Pt/System/IOStream.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <iostream>


class PipeTest : public Pt::Unit::TestSuite
{
    public:
        PipeTest()
        : Pt::Unit::TestSuite("PipeTest")
        , _loop(0)
        , _pos(0)
        {
            Pt::Unit::TestSuite::registerMethod( "AsyncRead", *this, &PipeTest::AsyncRead );
            Pt::Unit::TestSuite::registerMethod( "AsyncWrite", *this, &PipeTest::AsyncWrite );
            Pt::Unit::TestSuite::registerMethod( "RemoveFromLoop", *this, &PipeTest::RemoveFromLoop );
            Pt::Unit::TestSuite::registerMethod( "PipeIOStream", *this, &PipeTest::PipeIOStream );
        }

        void setUp()
        {
            _data.clear();
            _result.clear();

            _loop = new Pt::System::MainLoop();
            _loop->timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);
            _loop->setIdleTimeout(2000);
        }

        void tearDown()
        {
            delete _loop;
            _loop = 0;
        }

    protected:
        void AsyncRead()
        {
            _data = "Hello World, where do you want to GOTO day!";

            Pt::System::Pipe pipe;
            pipe.in().write( _data.c_str(), _data.size() );

            pipe.out().setActive(*_loop);
            pipe.out().beginRead( _buffer, sizeof(_buffer) );
            pipe.out().inputReady() += Pt::slot(*this, &PipeTest::onRead);

            _loop->run();

            PT_UNIT_ASSERT(_result == _data);
        }

        void onRead(Pt::System::IODevice& dev)
        {
            size_t sz = dev.endRead();
            _result.append(_buffer, sz);

            if( _result.size() < _data.size() )
                dev.beginRead(_buffer, sizeof(_buffer));
            else
            {
                _loop->exit();
            }
        }

        void AsyncWrite()
        {
            _data = "Hello World, where do you want to GOTO day!";
            _pos = 0;

            Pt::System::Pipe pipe;
            
            std::memcpy(_buffer, _data.c_str(),  sizeof(_buffer));
            pipe.in().setActive(*_loop);
            pipe.in().beginWrite(_buffer, sizeof(_buffer));
            pipe.in().outputReady() += Pt::slot( *this, &PipeTest::onWrite);

            _loop->run();
            std::cerr << "run ended" << std::endl;

            char buffer[1024];
            size_t n = pipe.out().read(buffer, sizeof(buffer));
            std::string result(buffer, n);
            //this->reportMessage(result);
            PT_UNIT_ASSERT(result == _data);
        }

        void onWrite(Pt::System::IODevice& dev)
        {
            std::cerr << "ending write" << std::endl;
            _pos += dev.endWrite();

            std::cerr << "wrote " << _pos << std::endl;
            size_t len = std::min(sizeof(_buffer), _data.size() - _pos);
            if(0 == len)
            {
                std::cerr << "exiting loop" << std::endl;
                _loop->exit();
                return;
            }

            std::cerr << "next write" << std::endl;
            std::memcpy(_buffer, _data.c_str()+_pos, len);
            dev.beginWrite(_buffer, len);
        }

        void RemoveFromLoop()
        {
            std::string out("Hello World, where do you want to GOTO day!");

            Pt::System::Pipe pipe;
            pipe.in().write( out.c_str(), out.size() );

            pipe.out().setActive(*_loop);
            pipe.out().beginRead(_buffer, sizeof(_buffer));
            pipe.out().inputReady() += Pt::slot(*this, &PipeTest::onReadRemove);

            _loop->run();
        }

        void onReadRemove(Pt::System::IODevice& dev)
        {
            size_t sz = dev.endRead();
            _result.append(_buffer, sz);
            dev.close();
            _loop->setIdleTimeout(200);
        }

        void PipeIOStream()
        {
            _data = "Hello world!";
            Pt::System::Pipe pipe;
            pipe.in().setActive(*_loop);
            pipe.out().setActive(*_loop);

            outbuf.attach( pipe.in() );
            outbuf.outputReady() += Pt::slot(*this, &PipeTest::onStreamOutput);

            inbuf.attach( pipe.out() );
            inbuf.inputReady() += Pt::slot(*this, &PipeTest::onStreamInput);

            PT_UNIT_ASSERT( 0 == outbuf.out_avail() );
            outbuf.sputn(_data.c_str(), 12);
            PT_UNIT_ASSERT( 12 == outbuf.out_avail() );
            outbuf.beginWrite();

            _loop->run();

            PT_UNIT_ASSERT( 0 == inbuf.out_avail() );
        }

        void onStreamInput(Pt::System::StreamBuffer& buffer)
        {
            buffer.endRead();
            PT_UNIT_ASSERT( 0 < buffer.in_avail() );

            char in[20];
            size_t n = buffer.sgetn( in, buffer.in_avail() );
            PT_UNIT_ASSERT( 0 < n );

            std::string data(in, n);
            PT_UNIT_ASSERT( _data.find(data) == 0 );

            _loop->exit();
        }

        void onStreamOutput(Pt::System::StreamBuffer& buffer)
        {
            buffer.device()->close();
            inbuf.beginRead();
        }

    private:
        Pt::System::MainLoop* _loop;
        std::string _data;
        size_t _pos;
        char _buffer[10];
        std::string _result;
        Pt::System::StreamBuffer inbuf;
        Pt::System::StreamBuffer outbuf;
};


Pt::Unit::RegisterTest<PipeTest> register_PipeTest;
