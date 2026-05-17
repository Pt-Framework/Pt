/* Copyright (C) 2026 Marc Boris Duerner

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

#ifndef Pt_Forms_BackgroundRenderer_h
#define Pt_Forms_BackgroundRenderer_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleFlags.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Forms {

class PaintContext;
class PaintSurface;
class StyleOptions;

/** @brief Render primitive for widget backgrounds.

    Fills a rectangular area using a brush derived from the current
    widget state and style options. The brush can be overridden for
    individual widget instances via clone-on-customize.
*/
class PT_FORMS_API BackgroundRenderer : public Style::Facet
{
  public:
    explicit BackgroundRenderer(std::size_t refs = 0);

    virtual ~BackgroundRenderer();

    const Gfx::Brush& background() const;

    void setBackground(const Gfx::Brush& b);

    BackgroundRenderer* create() const;

    void render(PaintContext& context,
                const Gfx::RectF& rect,
                StyleFlags state);

  protected:
    virtual BackgroundRenderer* onCreate() const = 0;

    virtual void onPrepare(const StyleOptions& options);

    virtual void onRender(PaintContext& context,
                          const StyleOptions& options,
                          const Gfx::RectF& rect,
                          StyleFlags state) = 0;

  private:
    AutoPtr<Gfx::Brush> _background;
    std::size_t         _styleGeneration;
};

} // namespace Forms

} // namespace Pt

#endif
