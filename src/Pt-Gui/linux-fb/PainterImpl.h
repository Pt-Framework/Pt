/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_linux_fb_PainterImpl_h
#define Pt_linux_fb_PainterImpl_h

#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <Pt/Gui/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gfx/Rgb555Image.h>
#include <Pt/Gui/Painter.h>
#include <Pt/String.h>


namespace Pt {

namespace Gui {

    class PainterImpl {
        public:
            PainterImpl();

            virtual ~PainterImpl();

            void begin();

            void end();

            Pt::ssize_t depth() const
            { return _screenInfo.bits_per_pixel;}

            Pt::ssize_t width() const
            { return _screenInfo.xres;}

            Pt::ssize_t height() const
            { return _screenInfo.yres;}

            void setPen(const Gfx::Pen& pen);

            const Gfx::Pen& pen() const;

            void setBrush(const Gfx::Brush& brush);

            const Gfx::Brush& brush() const;

            const Gfx::Font& font() const;

            void setFont(const Gfx::Font& font);

            Gfx::FontMetrics fontMetrics() const;

            Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

            const std::list<std::string>& fontFamilyNames();

            void drawPixel(const Math::Point& to);

            void drawLine(const Math::Point& from, const Math::Point& to);

            void drawText(const Math::Point& to, const Pt::String& text);

            void drawRect(const Math::Rect& rect);

            void drawEllipse(const Math::Point& topLeft, const Math::Size& size);

            void drawPolyline(const Math::Point* points, const size_t pointCount);

            void fillRect(const Math::Rect& rect);

            void fillEllipse(const Math::Point& topLeft, const Math::Size& size);

            void fillPolygon(const Math::Point* points, const size_t pointCount);

            void drawPixmap(const Math::Point& to, Pixmap& pm);

            void drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion);

            void drawImage(const Math::Point& to, const Gfx::ARgbImage& image);

            void drawImage(const Math::Point& to, const Gfx::ARgbImage& image,
                           const Gfx::Region& imageRegion);

        protected:
            template <typename Iterator>
            void drawImage(ssize_t toX, ssize_t toY, Iterator begin, Iterator end, size_t width, size_t height)
            {
                const char* imageData = 0;

                switch( this->depth() )
                {
                    case 32:
                    {
                        Gfx::Rgb888Image rgbImage( width, height );
                        assign( begin, end, rgbImage.begin() );
                        this->copyImageData( toX, toY, (char*)rgbImage.data(), rgbImage.width(), rgbImage.height() );
                        break;
                    }
                    case 16:
                    {
                        Gfx::Rgb565Image rgbImage( width, height );
                        assign( begin, end, rgbImage.begin() );
                        imageData = (char*)( rgbImage.data() );
                        this->copyImageData( toX, toY, (char*)rgbImage.data(), rgbImage.width(), rgbImage.height() );
                        break;
                    }
                    default:
                        imageData = 0;
                }
            }

            void copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight)
            {
                size_t pixelSize = this->depth() / 8;
                unsigned bufferOffset = toX + ( toY * this->width() );
                char* bufferData = (char*)( _buffer) + ( bufferOffset * pixelSize);

                for(size_t n = 0; n < fromHeight; ++n)
                {
                    memcpy(bufferData, data, fromWidth * pixelSize);
                    bufferData += this->width() * pixelSize;
                    data += fromWidth * pixelSize;
                }
            }

        private:
            int _fd;
            fb_var_screeninfo _screenInfo;
            fb_fix_screeninfo _fixedInfo;
            void* _buffer;
            Pt::size_t _bufferSize;
            Gfx::Pen _pen;
            Gfx::Brush _brush;
            Gfx::Font  _font;
    };

} // namespace Gui

} // namespace Pt

#endif
