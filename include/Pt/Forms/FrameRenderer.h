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

#ifndef Pt_Forms_FrameRenderer_h
#define Pt_Forms_FrameRenderer_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleFlags.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Forms {

class PaintContext;
class PaintSurface;
class StyleOptions;

/** @brief Render primitive for widget frames.

    Draws a border around a rectangular area using a pen derived from
    the current widget state and style options.
*/
class PT_FORMS_API FrameRenderer : public Style::Facet
{
  public:
    explicit FrameRenderer(std::size_t refs = 0);

    virtual ~FrameRenderer();

    const Gfx::Pen& contour() const;

    void setContour(const Gfx::Pen& p);

    FrameRenderer* create() const;

    void render(PaintContext& context,
                const Gfx::RectF& rect,
                StyleFlags state);

  protected:
    virtual FrameRenderer* onCreate() const = 0;

    virtual void onPrepare(const StyleOptions& options);

    virtual void onRender(PaintContext& context,
                          const StyleOptions& options,
                          const Gfx::RectF& rect,
                          StyleFlags state) = 0;

  private:
    AutoPtr<Gfx::Pen> _contour;
    std::size_t       _styleGeneration;
};

} // namespace Forms

} // namespace Pt

#endif
