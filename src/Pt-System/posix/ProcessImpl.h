
#include <sstream>
using namespace std;

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>


namespace Pt {

namespace System {

	class ProcessImpl {
		public:
			static void set(const char* name, const char* value)
			{
				ostringstream os;
				os << name << "=" << value;
				::putenv( const_cast<char*>( os.str().c_str() ) );
			}
		
			static void unset(const char* name)
			{
				ProcessImpl::set(name,"");
			}
		
			static const char* get(const char* name)
			{
				return ::getenv(name);
			}
	};

}

}
