#include <Pt/Hmi/Cursor.h>
#include <fstream>

namespace Pt{
namespace Hmi{

Cursor::Cursor()
{
}


Cursor::~Cursor()
{

}


void Cursor::loadCur(const char* curFile, Cursor& cursor)
{
	std::ifstream stream(curFile, std::ios::binary);

	if(!stream)
		throw std::invalid_argument("file not found");

	loadCur(stream, cursor);
}


void Cursor::loadCur(std::istream& stream, Cursor& cursor)
{		
	Pt::uint16_t size16;
	Pt::uint32_t size32;
	Pt::uint32_t headerOffset;	
	
	stream.seekg(18, std::ios_base::beg);
	stream.read((char*)&headerOffset, 4);

	stream.seekg(headerOffset + 4, std::ios_base::beg);
	
	stream.read((char*)&size32, 4);
	cursor._width = size32;
	
	stream.read((char*)&size32, 4);
	cursor._height = size32;

	stream.read((char*)&size16, 2);
	size_t planes = size16;

	stream.read((char*)&size16, 2);
	cursor._bitsPerPixel = size16;

	stream.seekg(headerOffset + 4 + 4 + 4 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 4 + 8, std::ios_base::beg);

	cursor._andBitmap.resize(cursor._width * cursor._height * cursor._bitsPerPixel/8, 0);
	cursor._xorBitmap.resize(cursor._width * cursor._height * cursor._bitsPerPixel/8, 0);
	
	stream.read((char*)&cursor._xorBitmap[0], cursor._xorBitmap.size() );
	stream.read((char*)&cursor._andBitmap[0], cursor._andBitmap.size() );
}

}}
