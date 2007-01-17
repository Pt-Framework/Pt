#ifndef PTV_Xml_ParseError_h
#define PTV_Xml_ParseError_h

#include <Pt/Exception.h>

namespace Pt {

namespace Xml {

	//! Exception that is thrown when a parse error occured.
	class PT_API ParseError : public std::logic_error {
		public:
			/**
			 * @brief Creates a new ParseError object using the given reason and source info.
			 *
			 * @param what The reason of the parse error.
			 * @param info Source info containing information about where the exception occured.
			 */
			ParseError(const std::string& what, const Pt::SourceInfo& info);
	};

}

}

#endif
