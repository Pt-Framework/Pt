#ifndef Pt_TestPlugin_h
#define Pt_TestPlugin_h

#include <Pt/Api.h>

class PT_EXPORT TestPlugin
{
	public:
		virtual ~TestPlugin()
		{}

		virtual void test() = 0;
};

#endif
