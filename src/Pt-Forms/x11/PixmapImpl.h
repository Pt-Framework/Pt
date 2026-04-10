 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#ifndef Pt_Forms_PixmapImpl_h
#define Pt_Forms_PixmapImpl_h

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/Bitmap.h>

namespace Pt {

namespace Forms {

class Pixmap;

class PixmapImpl
{
    public:
        PixmapImpl()
        { }

        void reset(const Gfx::Image& image)
        {
            _bitmap.reset(image);
        }

        void reset(const Gfx::SizeF& size)
        {
            _bitmap.reset(size);
        }

        const Gfx::Bitmap& bitmap() const 
        {
            return _bitmap;
        }

        void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
        {
            bitmap.reset( rect.size() );

            Gfx::Paint paint;
            bitmap.drawBitmap(Gfx::PointF(0, 0), _bitmap, paint, &rect);
        }

        const Gfx::SizeF& size() const
        {
            return _bitmap.size();
        }

        void setScaleFactor(double scaleFactor)
        {
            _bitmap.setScaleFactor(scaleFactor);
        }

        void drawPixmap(const Gfx::PointF& to,
                        const Pixmap& pixmap,
                        const Gfx::Paint& paint,
                        const Gfx::RectF* rect);
        
        const Gfx::ImageFormat& format() const
        {
            return _bitmap.format();
        }

        const Gfx::Scaling& scaling() const
        {
            return _bitmap.scaling();
        }

        Gfx::Canvas* getCanvas(Gfx::Canvas* reuse)
        {
            return _bitmap.getCanvas(reuse);
        }

        Gfx::Canvas* createCanvas(Gfx::Canvas* reuse)
        {
            return 0;
        }

        void releaseCanvas()
        {
        }

        void sync()
        {
            _bitmap.sync();
        }

        void finish()
        {
            _bitmap.finish();
        }

    public:
        static const std::string& defaultFont()
        {
            return Gfx::Bitmap::defaultFont();
        }

        static void setDefaultFont(const std::string& family)
        {
            Gfx::Bitmap::setDefaultFont(family);
        }

        static std::vector<std::string> fontFamilies()
        {
            return Gfx::Bitmap::fontFamilies();
        }

        static std::vector<Gfx::FontFace> fontFaces(const std::string& family)
        {
            return Gfx::Bitmap::fontFaces(family);
        }
    
    private:
        Gfx::Bitmap _bitmap;
};

} // namespace

} // namespace

#endif
