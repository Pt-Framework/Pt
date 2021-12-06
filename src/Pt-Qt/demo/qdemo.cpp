/* 
 * Copyright (C) 2014 Marc Boris Duerner
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <Pt/Qt/Application.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/Timer.h>
#include <iostream>

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

int main(int argc, char *argv[]) 
{
    Pt::Qt::Application app(argc, argv);
    
    QPushButton button("QUIT");
    QObject::connect(&button, SIGNAL(clicked()), qApp, SLOT(quit()));
    button.show();

    Pt::Net::TcpSocket socket;
    socket.connected() += Pt::slot( &onConnect );
    socket.inputReady() += Pt::slot( &onInput );
    socket.outputReady() += Pt::slot( &onOutput );
    socket.setActive( app.loop() );

    Pt::Net::Endpoint ep("www.google.de", 80);
    socket.beginConnect(ep);

    Pt::System::Timer timer;
    timer.setActive( app.loop() );
    timer.start(5000);
    timer.timeout() += Pt::slot( &onTimer );
    timer.timeout() += Pt::slot( app, &Pt::System::Application::exit );

    app.run();
    return 0;
}
