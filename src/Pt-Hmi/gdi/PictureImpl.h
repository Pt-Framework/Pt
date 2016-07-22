/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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
  MA  02110-1301  USA
*/

#ifndef Pt_Hmi_PictureImpl_h
#define Pt_Hmi_PictureImpl_h

#include <Pt/Gfx/Image.h>
#include <vector>
#include <Windows.h>

namespace Pt {

namespace Hmi {

class PictureImpl
{
    public:
        PictureImpl();

        virtual ~PictureImpl();

        void set(const Gfx::Image& image);

        const HBITMAP andMask() const
        {
          return _hAndMask;
        }

        HBITMAP andMask()
        {
          return _hAndMask;
        }

        const HBITMAP xorMask() const
        {
            return _hXorMask;
        }

        HBITMAP xorMask()
        {
            return _hXorMask;
        }

        size_t width() const
        {
            return _width;
        }
        
        size_t height() const
        {
            return _height;
        }

        bool empty() const
        {
          return _andMask.empty();
        }
          
    private:
      void clear();

    private:
        std::vector<Pt::uint8_t> _andMask;
        std::vector<Pt::uint8_t> _xorMask;

        HBITMAP _hAndMask;
        HBITMAP _hXorMask;
        size_t _width;
        size_t _height;
};

}  // namespace

} // namespace

#endif
