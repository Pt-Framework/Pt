#ifndef Pt_Hmi_Cursor_h
#define Pt_Hmi_Cursor_h

#include <Pt/Hmi/Api.h>
#include <Pt/Types.h>
#include <Pt/Ui/Image.h>
#include <vector>

namespace Pt{
namespace Hmi{

class PT_HMI_API Cursor
{
	public:
		Cursor();				

		virtual ~Cursor();
			
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
    
		void setXHotspot(size_t v) 
		{
			_xHotspot = v;
		}

		void setYHotspot(size_t v) 
		{
			_yHotspot = v;
		}

    void setName( const std::string& n)
    {
      _name = n;
    }

    const std::string& name() const
    {
      return _name;
    }

    bool empty() const
    {
      return _andMask.empty();
    }

    void clear()
    {
    	_andMask.clear();
		   _xorMask.clear();
		  _width = 0;
		  _height = 0;
    }

  public:
		static const Cursor& defaultCursor();
		static const Cursor& arrowCursor();
		static const Cursor& waitCursor();    
		static const Cursor& sizeNWSECursor();
		static const Cursor& sizeNESWCursor();
		static const Cursor& sizeWECursor();
		static const Cursor& sizeNSCursor();
    static const Cursor& moveCursor();
  
  public:
    static void fromImage( const Ui::Image& image, Cursor& cursor );	
    static void loadCursor( const char* pngFile, const Ui::Color& alphaColor, Cursor& cursor );
    static void loadCursor( std::istream& pngStream, const Ui::Color& alphaColor, Cursor& cursor );
    static void loadCursor( const Pt::uint8_t* pngBuffer, const size_t streamSize, const Ui::Color& alphaColor, Cursor& cursor );

	private:
		std::vector<Pt::uint8_t> _andMask;
		std::vector<Pt::uint8_t> _xorMask;		
		size_t _width;
		size_t _height;
		size_t _xHotspot;
		size_t _yHotspot;
    std::string  _name;
};

}}

#endif

