/* 
 * Copyright (C) 2014 Marc Boris Dürner
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

#include <Pt/WxWidgets/MainLoop.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/Timer.h>
#include <wx/app.h>
#include <iostream>
#include <stdexcept>

const char request[] = "GET / HTTP/1.1\r\n"
                       "Host: www.google.de\r\n"
                       "Connection: close\r\n"
                       "\r\n";

char reply[1000];

void onConnect(Pt::Net::TcpSocket& sock)
{
    sock.endConnect();
    std::clog << "Connect !" << std::endl;
    
    sock.beginWrite(request, sizeof(request));
}

void onOutput(Pt::System::IODevice& dev)
{
    std::size_t n = dev.endWrite();
    std::clog << "Output: " << n << " bytes." << std::endl;

    dev.beginRead(reply, sizeof(reply));
}

void onInput(Pt::System::IODevice& dev)
{
    std::size_t n = dev.endRead();
    std::clog << "Input: \n\n";
    std::clog.write(reply, n) << std::endl;

    dev.close();
}

void onTimer()
{
    std::clog << "Timeout !" << std::endl;
}


class DemoApp : public wxApp
{
    public:
        DemoApp()
        : _loop(0) 
        { }
    
        virtual ~DemoApp() 
        { 
            delete _loop;
        }

        virtual void OnLaunched()
        {
            wxApp::OnLaunched();
    
            wxEventLoopBase* wxLoop = this->GetMainLoop();
            if( ! wxLoop)
                throw std::logic_error("GetMainLoop");

            _loop = new Pt::WxWidgets::MainLoop(*wxLoop);

            _timer.setActive(*_loop);
            _timer.start(5000);
            _timer.timeout() += Pt::slot( &onTimer );

            _socket.connected() += Pt::slot( &onConnect );
            _socket.inputReady() += Pt::slot( &onInput );
            _socket.outputReady() += Pt::slot( &onOutput );
            _socket.setActive(*_loop);

            Pt::Net::Endpoint ep("www.google.de", 80);
            _socket.beginConnect(ep);
        }

        virtual int OnExit()
        { return 0; }

    private:
        Pt::WxWidgets::MainLoop* _loop;
        Pt::Net::TcpSocket _socket;
        Pt::System::Timer _timer;

};

IMPLEMENT_APP(DemoApp)
