/* Copyright (C) 2020 Marc Boris Duerner
Copyright (C) 2020 Laurentiu-Gheorghe Crisan

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA
*/

#ifndef PT_FORMS_SKIAPAINTER_H
#define PT_FORMS_SKIAPAINTER_H

#include <SkSurface.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkPath.h>

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Forms {

    class SkiaPainter : public Gfx::Painter
    {
    public:
        SkiaPainter(Gfx::Image& image);

        virtual ~SkiaPainter();

        void setImage(Gfx::Image& image);

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

        virtual Gfx::TextMetrics textMetrics(const Pt::String& text) const;

        virtual void drawLine(const Gfx::PointF& from, const  Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans);

        virtual void drawRect(const Gfx::RectF& rect);

        virtual void fillRect(const Gfx::RectF& rect);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, const size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, const size_t pointCount);

        virtual void drawPath(const Gfx::Path& path, float smoothness);

        virtual void fillPath(const Gfx::Path& path, float smoothness);

        virtual void drawImage(const  Gfx::PointF& to, const Gfx::Image& image);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imageRect);

    public:
        static std::string defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::vector<std::string> fontFamilies();

        static std::vector<Gfx::FontFace> fontFaces(const std::string& family);

        static Gfx::TextMetrics textMetrics(const Gfx::Font& font, const Pt::String& text);

        static SkColor toSkia(const Gfx::Color& c)
        {
            return SkColorSetARGB((U8CPU)(c.alpha() / 257),
                                    (U8CPU)(c.red() / 257),
                                    (U8CPU)(c.green() / 257),
                                    (U8CPU)c.blue() / 257);
        }


        static SkPoint toSkia(const Gfx::PointF& p)
        {
            SkPoint sp;

            sp.fX = p.x();
            sp.fY = p.y();
            return sp;
        }

        static SkRect toSkia(const Gfx::RectF& r)
        {
            SkRect sr;

            sr.fLeft = r.left();
            sr.fRight = r.right();
            sr.fTop = r.top();
            sr.fBottom = r.bottom();

            return sr;
        }

        static SkPaint::Join toSkia(Gfx::Pen::JoinStyle s)
        {
            switch (s)
            {
                case Gfx::Pen::MiterJoin:
                    return SkPaint::kMiter_Join;

                case Gfx::Pen::RoundJoin:
                    return SkPaint::kRound_Join;

                case Gfx::Pen::BevelJoin:
                    return SkPaint::kBevel_Join;
            }

            return SkPaint::kMiter_Join;
        }


        static SkPaint::Cap toSkia(Gfx::Pen::CapStyle s)
        {
            switch (s)
            {
                case Gfx::Pen::RoundCap:
                    return SkPaint::kRound_Cap;

                case Gfx::Pen::FlatCap:
                    return SkPaint::kButt_Cap;

                case Gfx::Pen::SquareCap:
                    return SkPaint::kSquare_Cap;
            }

            return SkPaint::kSquare_Cap;
        }

        static SkRect toRect(const Gfx::PointF& p, const Gfx::SizeF& s)
        {
            SkRect sr;

            sr.fLeft = p.x();
            sr.fRight = p.x() + s.width();
            sr.fTop = p.y();
            sr.fBottom = p.y()+ s.height();

            return sr;
        }

        static SkPath toSkia(const Gfx::Path& p);

        static bool equals(const Gfx::PointF& p1, const Gfx::PointF& p2)
        {

            double dt = std::abs(p1.x() - p2.x());

            if (dt > 0.1)
                return false;


            dt = std::abs(p1.y() - p2.y());

            if (dt > 0.1)
                return false;

            return true;
        }

    private:
        Gfx::RectF _clip;
        Gfx::Pen _pen;
        Gfx::Brush _brush;
        Gfx::Font _font;
        Gfx::Image* _image;

    private:

        SkPaint _skiaPen;
        SkPaint _skiaBrush;
        sk_sp<SkSurface> _surface;
        SkCanvas* _canvas;
        Gfx::ImagePainter _imgPainter;

    };

}
}

#endif

