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
#include <Pt/Forms/PaintSurface.h>

#define PT_FORMS_WIN32_RASTER 1
//#define PT_FORMS_GDIPLUS 1

#ifdef PT_FORMS_WIN32_RASTER

#include <Pt/Gfx/ImageSurface.h>

#else

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Path.h>
#include <Pt/System/Path.h>

#include <vector>

#include <Windows.h>

#endif

namespace Pt {

namespace Forms {

class Pixmap;

#ifdef PT_FORMS_WIN32_RASTER

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
            return Gfx::ImageSurface::defaultFont();
        }

        static void setDefaultFont(const std::string& name)
        {
            Gfx::ImageSurface::setDefaultFont(name);
        }

        static std::vector<std::string> fontNames()
        {
            return Gfx::ImageSurface::fontNames();
        }
        
        static void setFontDir(const System::Path& path)
        {
            Gfx::ImageSurface::setFontDir(path);
        }
    
    private:
        Gfx::ImageSurface _image;
};

#else // PT_FORMS_WIN32_RASTER

class PaintContext;

class PixmapImpl
{
    public:
        PixmapImpl();

        virtual ~PixmapImpl();

        void set(const Gfx::Image& image);

        Gfx::Image toImage() const;
        
        void clear(const Gfx::Color& c);

        const Gfx::SizeF& physicalSize() const;

        const Gfx::SizeF& logicalSize() const;

        void resize(const Gfx::SizeF& size);
        
        void setScaleFactor(double scaleFactor);

        HDC deviceContext() const;

        void drawPixmap(const Gfx::PointF& to,
                                const Pixmap& pm,
                                const Gfx::Paint& paint,
                                const Gfx::RectF* rect);

        const Gfx::ImageFormat& format() const;

        const Gfx::SizeF& size() const;

        const Gfx::Scaling& scaling() const;

        Gfx::PaintContext* getContext(Gfx::PaintContext* reuse)
        {
            return 0;
        }

        Gfx::PaintContext* createContext(Gfx::PaintContext* context);

        void releaseContext();

        void sync();

    public:
        static const std::string& defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::vector<std::string> fontNames();

        static void setFontDir(const System::Path& path);

    private: 
        static std::string& getDefaultFont();

        static std::string getSystemFont();

    private:
        Gfx::SizeF     _physicalSize;
        Gfx::SizeF     _logicalSize;
        Gfx::Scaling   _scaling;

        LONG           _width;
        LONG           _height;
        HDC            _dc;
        HBITMAP        _bitmap;
        HPEN           _oldPen;
        HBRUSH         _oldBrush;
        HFONT          _oldFont;
        HBITMAP        _oldBitmap;

        PaintContext*  _paintContext;
};

#endif // PT_FORMS_WIN32_RASTER

} // namespace

} // namespace

#endif
