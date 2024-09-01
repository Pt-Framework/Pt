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

#ifndef Pt_Hmi_PaintData_h
#define Pt_Hmi_PaintData_h

#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>
#include <algorithm>
#include <cctype>

using std::max;
using std::min;
#include <Windows.h>
#include <Gdiplus.h>

//#define PT_HMI_GDIPLUS 1

namespace Pt {

namespace Hmi {

class PixmapSurfaceImpl;

#ifndef PT_HMI_WIN32_RASTER

class PaintData : public Gfx::PaintContext
{
    public:
        PaintData(PixmapSurfaceImpl& canvas);

        ~PaintData();

        HPEN pen() const;

        Gfx::Color penColor() const;

        HBRUSH brush() const;

        bool gradientBrush() const;

        const Gfx::Brush::GradientStyle& gradient() const;
        
        const Gfx::Color& gradientStart() const;
        
        const Gfx::Color& gradientStop() const;

        HFONT font() const;

        HRGN clipRect() const;

        const Gfx::CompositionMode& compositionMode() const;

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) override;

        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onSetFont(const Gfx::Font& font) override;

        virtual void onSetClip(const Gfx::RectF& rectF) override;

        virtual void onResetClip() override;

    public:
        void updateMode(const Gfx::CompositionMode& mode);

        void updatePen(const Gfx::Pen& pen);

        void updateBrush(const Gfx::Brush& brush);

        void updateFont(const Gfx::Font& font);

        void updateClip(const Gfx::RectF* clip);

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
};

#endif

} // namespace

} // namespace

#endif
