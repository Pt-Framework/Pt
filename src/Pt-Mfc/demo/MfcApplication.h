#pragma once

#include <Pt/Mfc/Application.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/Timer.h>

class CMfcApplication : public Pt::Mfc::WinAppEx
{
public:
	CMfcApplication(void);
	virtual ~CMfcApplication(void);


	virtual BOOL InitInstance();

private:
	void TestPtLoop();
	void onConnect(Pt::Net::TcpSocket& sock);
	void onOutput(Pt::System::IODevice& dev);
	void onInput(Pt::System::IODevice& dev);
	void onTimer();

private:
	CFrameWnd*  _mainWindow;
	Pt::Net::TcpSocket _socket;
	Pt::System::Timer _timer;
	 static const char _request[];
	char _reply[1000];
};
