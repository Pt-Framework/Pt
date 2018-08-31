/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 As a special exception, you may use this file as part of a free
 software library without restriction. Specifically, if other files
 instantiate templates or use macros or inline functions from this
 file, or you compile this file and link it with other files to
 produce an executable, this file does not by itself cause the
 resulting executable to be covered by the GNU General Public
 License. This exception does not however invalidate any other
 reasons why the executable file might be covered by the GNU Library
 General Public License.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA 02110-1301 USA
*/

#ifndef Pt_Hmi_Cursor_h
#define Pt_Hmi_Cursor_h

#include <Pt/Hmi/Api.h>
#include <Pt/Types.h>
#include <Pt/Gfx/Image.h>
#include <vector>

namespace Pt {

namespace Hmi {

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
          return _andMask.empty() || _width == 0 || _height == 0;
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
        static void fromImage( const Gfx::Image& image, Cursor& cursor );    
        static void loadCursor( const char* pngFile, const Gfx::Color& alphaColor, Cursor& cursor );
        static void loadCursor( std::istream& pngStream, const Gfx::Color& alphaColor, Cursor& cursor );
        static void loadCursor( const Pt::uint8_t* pngBuffer, const size_t streamSize, const Gfx::Color& alphaColor, Cursor& cursor );

    private:
        std::vector<Pt::uint8_t> _andMask;
        std::vector<Pt::uint8_t> _xorMask;        
        size_t _width;
        size_t _height;
        size_t _xHotspot;
        size_t _yHotspot;
        std::string  _name;
};

} // namespace

} // namespace

#endif // include guard

