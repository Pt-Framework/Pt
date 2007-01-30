#include "FreeType.h"
#include <stdexcept>
#include "Pt/SourceInfo.h"

namespace Pt{
namespace Gfx{

FreeType::FreeType()
{
    if( FT_Init_FreeType( &_ft ) ) 
		throw std::runtime_error( "FT_Init_FreeType failed" + PT_SOURCEINFO );
}

FreeType::~FreeType()
{
    FT_Done_FreeType( _ft );
}

}
}