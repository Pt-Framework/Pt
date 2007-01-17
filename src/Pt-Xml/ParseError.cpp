#include "Pt/Xml/ParseError.h"


namespace Pt {

namespace Xml {

ParseError::ParseError(const std::string& what, const Pt::SourceInfo& info)
: std::runtime_error(what + info)
{
}


} // namespace Xml

} // namespace Pt
