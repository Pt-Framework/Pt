/* Copyright (C) 2016 Marc Boris Duerner 
  
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

#ifndef PT_FORMS_WIN32_PAINTCONTEXT_H
#define PT_FORMS_WIN32_PAINTCONTEXT_H

#include <Pt/Forms/Api.h>

#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/PaintContext.h>

#include <Windows.h>

namespace Pt {

namespace Forms {

#ifndef PT_FORMS_WIN32_RASTER

class PaintContext : public Gfx::PaintContext
{
    public:
        PaintContext();

        ~PaintContext();

        const Gfx::CompositionMode& compositionMode() const;

        HPEN pen() const;

        Gfx::Color penColor() const;

        HBRUSH brush() const;

        bool gradientBrush() const;

        const Gfx::Brush::GradientStyle& gradient() const;
        
        const Gfx::Color& gradientStart() const;
        
        const Gfx::Color& gradientStop() const;

        HFONT font() const;

        HRGN clipRect() const;

        const Gfx::Path& path() const;

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) override;

        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onSetFont(const Gfx::Font& font) override;

        virtual void onSetClip(const Gfx::RectF* clip) override;

        virtual void onSetPath(const Gfx::Path& path) override;

    private:
        Gfx::Scaling              _scaling;
        Gfx::CompositionMode      _compositionMode;
        HPEN                      _pen;
        Gfx::Color                _penColor;
        HBRUSH                    _brush;
        bool                      _gradientBrush;
        Gfx::Brush::GradientStyle _gradient;
        Gfx::Color                _gradientStart;
        Gfx::Color                _gradientStop;
        HRGN                      _clipRect;
        HFONT                     _font;
        Gfx::Path                 _path;
};

#endif

} // namespace

} // namespace

#endif
