#ifndef Pt_Xml_ParseError_h
#define Pt_Xml_ParseError_h

#include <Pt/Exception.h>

namespace Pt {

namespace Xml {

	class PT_EXPORT ParseError : public Pt::Exception {
		public:
			ParseError(const std::string& what, const Pt::SourceInfo& info);
	};

}

}

#endif
