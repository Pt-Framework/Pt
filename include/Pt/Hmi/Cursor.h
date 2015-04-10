#ifndef Pt_Hmi_Cursor_h
#define Pt_Hmi_Cursor_h

#include <Pt/Hmi/Api.h>
#include <Pt/Types.h>
#include <Pt/Gfx/ARgbImage.h>
#include <vector>

namespace Pt{
namespace Hmi{

class PT_HMI_API Cursor
{
	public:
		Cursor();				

		virtual ~Cursor();

		void load( const Pt::Gfx::ARgbImage& image, size_t xHotspot, size_t yHotspot );	
			
		const std::vector<Pt::uint8_t>& andRgb888() const 
		{
			return _andMask;
		}
		
		const std::vector<Pt::uint8_t>& xorRgb888() const 
		{
			return _xorMask;
		}

		size_t width() const
		{
			return _width;
		}

		size_t height() const
		{
			return _height;
		}

		size_t xHotspot() const
		{
			return _xHotspot;
		}

		size_t yHotspot() const
		{
			return _yHotspot;
		}

		static const Cursor& defaultCursor();
		static const Cursor& arrowCursor();
		static const Cursor& waitCursor();
		static const Cursor& sizeNWSECursor();
		static const Cursor& sizeNESWCursor();
		static const Cursor& sizeWECursor();
		static const Cursor& sizeNSCursor();

	private:
		std::vector<Pt::uint8_t> _andMask;
		std::vector<Pt::uint8_t> _xorMask;		
		size_t _width;
		size_t _height;
		size_t _xHotspot;
		size_t _yHotspot;
};

}}

#endif

