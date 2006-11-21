#ifndef Pt_System_Timer_h
#define Pt_System_Timer_h

#include <Pt/System/Runnable.h>
#include <Pt/System/Thread.h>
#include <Pt/Signal.h>

#include <vector>

namespace Pt
{
namespace System
{

class PT_EXPORT Timer : public Runnable
{
public:
	Timer(void);
	virtual ~Timer(void);

	static size_t resolution();

	void start();
	void stop();

	void run();

	Signal<size_t> onTime;

private:
	Thread			_timerThread;
	static size_t	_resolution;
	size_t			_eventCounter;
	bool			_run;

};

}

}
#endif
