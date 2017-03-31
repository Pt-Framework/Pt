/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Boris Duerner 
 
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

#ifndef Pt_Hmi_PaintSurface_h
#define Pt_Hmi_PaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Hmi {

class Painter;
class PixmapSurface;
class Picture;

/** @brief Paint target for painters.
*/
class PT_HMI_API PaintSurface
{
    friend class Painter;
    friend class PaintRegion;

    public:
        virtual ~PaintSurface();
        
        const Gfx::Size& size() const;

        static void setDefaultFont(const std::string& f);
    
    protected:
        PaintSurface();

        void begin(Painter& painter);

        void finish(Painter& painter);

    protected:
        virtual const Gfx::Size& onSize() const = 0;

        virtual void onBegin(Painter& painter) = 0;

        virtual void onFinish() = 0;
    
    protected:
        virtual const Gfx::ImageFormat& format() const = 0;

        virtual void setClip(const Gfx::Rect& clip) = 0; 

        virtual void setCompositionMode(const Gfx::CompositionMode& mode) = 0;

        virtual void setPen(const Gfx::Pen& pen) = 0;

        virtual void setBrush(const Gfx::Brush& brush) = 0;

        virtual void setFont(const Gfx::Font& font) = 0;

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const = 0;
    
        virtual void drawLine(const Gfx::Point& from, const Gfx::Point& to) = 0;

        virtual void drawText(const Gfx::Point& to, const Pt::String& Text) = 0;

        virtual void drawRect(const Gfx::Rect& rectangle) = 0;

        virtual void fillRect(const Gfx::Rect& rectangle) = 0;

        virtual void drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size) = 0;

        virtual void fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size) = 0;

        virtual void drawPolyline(const Gfx::Point* points, size_t pointCount) = 0;

        virtual void fillPolygon(const Gfx::Point* points, size_t pointCount) = 0;

        virtual void drawSurface(const Gfx::Point& to, const PixmapSurface& surface) = 0;

        virtual void drawSurface(const Gfx::Point& to, const PixmapSurface& pm, const Gfx::Rect& pmRect) = 0;

        virtual void drawImage(const Gfx::Point& to, const Gfx::Image& image) = 0;

        virtual void drawImage(const Gfx::Point& to, const Gfx::Image& image, const Gfx::Rect& imgRect) = 0;

        virtual void drawPicture(const Gfx::Point& to, const Picture& pic) = 0;

    private:
        Painter* _painter;
};

} // namespace

} // namespace

#endif
