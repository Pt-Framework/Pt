#include <cstdlib>
#include <sstream>
using namespace std;

#include <windows.h>

namespace Pt {

namespace System {

	class ProcessImpl {
		public:
			static void set(const char* name, const char* value)
			{
				ostringstream os;
				os << name << "=" << value;
				::_putenv( os.str().c_str() );
			}
		
			static void unset(const char* name)
			{
				ProcessImpl::set(name,"");
			}
		
			static const char* get(const char* name)
			{
				//return ::getenv(name);
				return "";
			}
	};

}

}



