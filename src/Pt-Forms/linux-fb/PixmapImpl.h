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
#include <Pt/Gfx/Bitmap.h>

namespace Pt {

namespace Forms {

class Pixmap;

class PixmapImpl
{
    public:
        PixmapImpl()
        { }

        void set(const Gfx::Image& image)
        {
            _image.reset(image);
        }

        const Gfx::Image& toImage() const
        {
            return _image.image();
        }

        void clear(const Gfx::Color& c)
        { }

        const Gfx::SizeF& size() const
        {
            return _image.size();
        }

        void resize(const Gfx::SizeF& size)
        {
            _image.reset(size);
        }

        void setScaleFactor(double scaleFactor)
        {
            _image.setScaleFactor(scaleFactor);
        }

        void drawPixmap(const Gfx::PointF& to,
                        const Pixmap& pixmap,
                        const Gfx::Paint& paint,
                        const Gfx::RectF* rect);
        
        const Gfx::ImageFormat& format() const
        {
            return _image.format();
        }

        const Gfx::Scaling& scaling() const
        {
            return _image.scaling();
        }

        Gfx::PaintContext* getContext(Gfx::PaintContext* reuse)
        {
            return _image.getContext(reuse);
        }

        Gfx::PaintContext* createContext(Gfx::PaintContext* reuse)
        {
            return 0;
        }

        void releaseContext()
        {
        }

        void sync()
        {
            _image.sync();
        }

    public:
        static const std::string& defaultFont()
        {
            return Gfx::Bitmap::defaultFont();
        }

        static void setDefaultFont(const std::string& name)
        {
            Gfx::Bitmap::setDefaultFont(name);
        }

        static std::vector<std::string> fontNames()
        {
            return Gfx::Bitmap::fontNames();
        }
        
        static void setFontDir(const System::Path& path)
        {
            Gfx::Bitmap::setFontDir(path);
        }
    
    private:
        Gfx::Bitmap _image;
};

} // namespace

} // namespace

#endif // include guard
