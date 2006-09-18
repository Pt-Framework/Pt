#include "ProcessImpl.h"

#include "Pt/System/Process.h"


namespace Pt {

namespace System {


void Process::set(const char* name, const char* value)
{
	ProcessImpl::set(name, value);
}


void Process::unset(const char* name)
{
 	ProcessImpl::unset(name);
}


const char* Process::get(const char* name)
{
 	return ProcessImpl::get(name);
}


}

}
