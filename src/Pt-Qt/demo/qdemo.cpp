#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>

#include <Pt/Qt/Application.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/Timer.h>

#include <iostream>

void onConnect(Pt::Net::TcpSocket& sock)
{
	sock.endConnect();
	std::clog << "Connect !" << std::endl;
}

void onTimer()
{
	std::clog << "Timer !" << std::endl;
}

int main(int argc, char *argv[]) 
{
    Pt::Qt::Application app(argc, argv);
    
    QPushButton button("Hello world");
	QObject::connect(&button, SIGNAL(clicked()), &app, SLOT(quit()));
    button.show();

	Pt::Net::TcpSocket socket;
	socket.connected() += Pt::slot( &onConnect );
	socket.setActive( app.loop() );

	Pt::Net::Endpoint ep("www.google.de", 80);
	socket.beginConnect(ep);

	Pt::System::Timer timer;
	timer.setActive(app.loop());
	timer.start(1000);
	timer.timeout() += Pt::slot( &onTimer );
	timer.timeout() += Pt::slot( app, &Pt::System::Application::exit );

    app.run();
	return 0;
}