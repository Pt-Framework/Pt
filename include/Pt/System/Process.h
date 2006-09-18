#ifndef Pt_Sys_Process_h
#define Pt_Sys_Process_h

#include <Pt/Api.h>
#include <Pt/Exception.h>


namespace Pt {

namespace System {

//! Process Environment
class PT_EXPORT Process {
	public:
		~Process()
		{}

		//! Set environment variable
		static void set(const char* name, const char* value);

		//! Unset environment variable
		static void unset(const char* name);

		//! Get environment variable
		static const char* get(const char* name);

	protected:
		Process()
		{}
};

}

}

#endif
