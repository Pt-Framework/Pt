#include "MfcApplication.h"
#include <iostream>

const char CMfcApplication::_request[] = "GET / HTTP/1.1\r\n"
										"Host: www.google.de\r\n"
										"Connection: close\r\n"             
										"\r\n";

CMfcApplication::CMfcApplication(void)
: _mainWindow(new CFrameWnd())
{

}


CMfcApplication::~CMfcApplication(void)
{
}


BOOL CMfcApplication::InitInstance()
{		
	CWinApp::InitInstance();

	TestPtLoop();

	CString wcn = ::AfxRegisterWndClass(NULL);
	
	_mainWindow->Create(wcn,"test");

	m_pMainWnd = _mainWindow;	
	_mainWindow->ShowWindow(SW_SHOW);
	return TRUE;
}



void CMfcApplication::onConnect(Pt::Net::TcpSocket& sock)
{
    sock.endConnect();
    std::clog << "Connect !" << std::endl;
    
    sock.beginWrite(_request, sizeof(_request));
}

void CMfcApplication::onOutput(Pt::System::IODevice& dev)
{
    std::size_t n = dev.endWrite();
    std::clog << "Output: " << n << " bytes." << std::endl;

    dev.beginRead(_reply, sizeof(_reply));
}

void CMfcApplication::onInput(Pt::System::IODevice& dev)
{
    std::size_t n = dev.endRead();
    std::clog << "Input: \n\n";
    std::clog.write(_reply, n) << std::endl;

    dev.close();
}

void CMfcApplication::onTimer()
{
    std::clog << "Timeout !" << std::endl;
}

void CMfcApplication::TestPtLoop()
{

    _socket.connected() += Pt::slot( *this, &CMfcApplication::onConnect );
    _socket.inputReady() += Pt::slot( *this, &CMfcApplication::onInput );
    _socket.outputReady() += Pt::slot( *this, &CMfcApplication::onOutput );
    _socket.setActive( this->loop() );

    Pt::Net::Endpoint ep("www.google.de", 80);
    _socket.beginConnect(ep);


    _timer.setActive( this->loop() );
    _timer.start(5000);
    _timer.timeout() += Pt::slot( *this, &CMfcApplication::onTimer );
    _timer.timeout() += Pt::slot( *this, &Pt::System::Application::exit );
}