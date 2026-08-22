/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_FORMS_LINEEDITSTYLE_H
#define PT_FORMS_LINEEDITSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API LineEditState
{
    public:
        LineEditState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isEditable() const;

        void setEditable(bool value);

        bool isPlaceholder() const;

        void setPlaceholder(bool value);

    private:
        bool _enabled;
        bool _focused;
        bool _highlighted;
        bool _editable;
        bool _placeholder;
};


class PT_FORMS_API LineEditRenderer : public Style::Facet
{
    public:
        explicit LineEditRenderer(std::size_t refs = 0);

        virtual ~LineEditRenderer();

        LineEditRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const StyleOptions& lineEditOptions);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        const Painter& textPainter(PaintSurface& surface);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const Gfx::RectF& cursor,
                          const Gfx::RectF& selection,
                          const LineEditState& state);

        void renderEntry(PaintContext& context,
                         const Gfx::RectF& rect,
                         const LineEditState& state);

        void renderSelection(PaintContext& context,
                             const Gfx::RectF& textRect,
                             const Gfx::RectF& selection,
                             const LineEditState& state);

        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const LineEditState& state);

        void renderCursor(PaintContext& context,
                          const Gfx::RectF& textRect,
                          const Gfx::RectF& cursor,
                          const LineEditState& state);

    protected:
        virtual void onReset(const StyleOptions& options);

        virtual LineEditRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& lineEditOptions) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& textRect,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    const Gfx::RectF& cursor,
                                    const Gfx::RectF& selection,
                                    const LineEditState& state);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const LineEditState& state) = 0;

        virtual void onRenderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const Gfx::RectF& selection,
                                       const LineEditState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const LineEditState& state) = 0;

        virtual void onRenderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const Gfx::RectF& cursor,
                                    const LineEditState& state) = 0;
};


class PT_FORMS_API LineEditStyle : public Styler<LineEditRenderer,
                                                      StyleOptions>
{
    public:
        LineEditStyle();
};

} // namespace

} // namespace

#endif
