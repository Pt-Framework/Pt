#ifndef Pt_Hmi_Cursor_h
#define Pt_Hmi_Cursor_h

#include <Pt/Hmi/Api.h>
#include <Pt/Types.h>
#include <vector>

namespace Pt{
namespace Hmi{

class PT_HMI_API Cursor
{
	public:
		static void loadCur(const char* curFile, Cursor& cursor);
		static void loadCur(std::istream& stream, Cursor& cursor);

	public:
		Cursor();			
		
		virtual ~Cursor();

		int width() const
		{
			return _width;
		}
		
		int height() const
		{
			return _height;
		}

		int xHotSpot() const
		{
			return _xHotSpot;
		}

		int yHotSpot() const
		{
			return _yHotSpot;
		}
	
		int bitsPerPixel() const
		{
			return _bitsPerPixel;
		}

		const Pt::uint8_t* xorBitmap() const
		{
			return &_xorBitmap[0];
		}

		const Pt::uint8_t* andBitmap() const
		{
			return &_andBitmap[0];
		}

	private:
		int _width;
		int _height;
		int _xHotSpot;
		int _yHotSpot;
		int _bitsPerPixel;
		std::vector<Pt::uint8_t> _xorBitmap;
		std::vector<Pt::uint8_t> _andBitmap;
};

}}

#endif

