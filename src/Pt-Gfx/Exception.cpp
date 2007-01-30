#include "Pt/Gfx/Exception.h"

namespace Pt{
namespace Gfx{

InvalidFont::InvalidFont(const std::string& what)
: invalid_argument( what )
{ }

InvalidFont::~InvalidFont()
{ }

}
}