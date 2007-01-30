#ifndef PT_GFX_FREETYPE_H
#define PT_GFX_FREETYPE_H

#include "freetype/include/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include "Pt/Singleton.h"

namespace Pt{
namespace Gfx{

class FreeType : public Pt::Singleton<FreeType>
{
	friend class Pt::Singleton<FreeType>;
	
	public:
		struct Init
		{
			Init()
			{ FreeType::instance(); }
		};

		FT_Library library() const
		{ return _ft; }

		~FreeType();
		
	protected:
		FreeType();
	
	private:
		FT_Library _ft;
};
 
 static FreeType::Init initFreeType;
 
}
}

#endif