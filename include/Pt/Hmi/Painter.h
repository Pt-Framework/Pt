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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_HMI_PAINTER_H
#define PT_HMI_PAINTER_H

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Hmi {

class PainterImpl;
class PaintSurface;
class PixmapSurface;
class Picture;

class PT_HMI_API Painter : public Gfx::Painter
{
    friend class PaintSurface;

    public:
        Painter(PaintSurface& surface);

        virtual ~Painter();

        void begin(PaintSurface& surface);

        void finish();

        virtual const Gfx::ImageFormat& format() const;

        virtual void setCompositionMode(const Gfx::CompositionMode& mode);

        virtual const Gfx::CompositionMode& compositionMode() const; 

        virtual void setClip(const Gfx::RectF& clip);

        virtual void resetClip();

        virtual void setPen(const Gfx::Pen& pen);

        virtual const Gfx::Pen& pen() const;

        virtual void setBrush(const Gfx::Brush& brush);

        virtual const Gfx::Brush& brush() const;

        virtual void setFont(const Gfx::Font& font);

        virtual const Gfx::Font& font() const;  

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& Text) const; 
        
        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, const Pt::String& text);   

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text, const Gfx::Transform& trans);

        virtual void drawRect(const Gfx::RectF& rectangle);        

        virtual void fillRect(const Gfx::RectF& rectangle);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, const size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, const size_t pointCount);
        
        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        virtual void drawPicture(const Gfx::PointF& to, const Picture& pic);
    
        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imageRect);

        double toPhysical(double n) const;

        Gfx::PointF toPhysical(const Gfx::PointF& p) const;

        Gfx::SizeF toPhysical(const Gfx::SizeF& s) const;

        Gfx::RectF toPhysical(const Gfx::RectF& r) const;

        double toLogical(double n) const;

        Gfx::PointF toLogical(const Gfx::PointF& p) const;

        Gfx::SizeF toLogical(const Gfx::SizeF& s) const;

        Gfx::RectF toLogical(const Gfx::RectF& s) const;

        double alignContour(size_t n) const;

        double align(double n) const;

        Gfx::PointF align(const Gfx::PointF& rect) const;

        Gfx::RectF align(const Gfx::RectF& rect) const;

    public:
        void drawSurface(const Gfx::PointF& to, const PixmapSurface& pm);

        void drawSurface(const Gfx::PointF& to, const PixmapSurface& pm, const Gfx::RectF& pmRect);
    
        PainterImpl* impl()
        { return _impl;}

    public:
        static std::vector<std::string> fontNames();

    private:
        static void setDefaultFont(std::string f);

    private:
        void onDetach();

    private:
        PainterImpl*         _impl;
        PaintSurface*        _surface;
        Gfx::Pen             _pen;
        Gfx::Brush           _brush;
        Gfx::Font            _font;
        Gfx::CompositionMode _compositionMode;
        Gfx::RectF           _clip;
};

} // namespace

} // namespace

#endif
