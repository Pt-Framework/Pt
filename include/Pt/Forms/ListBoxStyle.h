/* Copyright (C) 2017 Marc Boris Duerner

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

#ifndef PT_FORMS_LISTBOXSTYLE_H
#define PT_FORMS_LISTBOXSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

class Pixmap;


class PT_FORMS_API ListBoxState
{
    public:
        ListBoxState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};


class PT_FORMS_API ListBoxRenderer : public Style::Facet
{
    public:
        explicit ListBoxRenderer(std::size_t refs = 0);

        virtual ~ListBoxRenderer();

        ListBoxRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const StyleOptions& listBoxOptions);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListBoxState& state);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const ListBoxState& state);

    protected:
        virtual void onReset(const StyleOptions& options);

        virtual ListBoxRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& listBoxOptions) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListBoxState& state) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const ListBoxState& state) = 0;
};


class PT_FORMS_API ListBoxStyle : public Styler<ListBoxRenderer,
                                                     StyleOptions>
{
    public:
        ListBoxStyle();
};


class PT_FORMS_API ListItemState
{
    public:
        ListItemState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isFocused() const;

        void setFocused(bool value);

        bool isSelected() const;

        void setSelected(bool value);

    private:
        bool _enabled;
        bool _highlighted;
        bool _focused;
        bool _selected;
};


class PT_FORMS_API ListItemRenderer : public Style::Facet
{
    public:
        explicit ListItemRenderer(std::size_t refs = 0);

        virtual ~ListItemRenderer();

        ListItemRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const StyleOptions& listItemOptions);

    public:
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  const Gfx::SizeF& iconSize,
                                  const Gfx::SizeF& textSize);

        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        const Painter& textPainter(PaintSurface& surface);

        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           const Gfx::SizeF& iconSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& iconRect,
                           Gfx::RectF& textRect);

        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListItemState& state);

        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const ListItemState& state);

        void renderIcon(PaintContext& context,
                        const Gfx::RectF& iconRect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        const ListItemState& state);

    protected:
        virtual void onReset(const StyleOptions& options);

        virtual ListItemRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& listItemOptions) = 0;

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListItemState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state) = 0;

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state) = 0;
};


class PT_FORMS_API ListItemStyle : public Styler<ListItemRenderer,
                                                      StyleOptions>
{
    public:
        ListItemStyle();
};

} // namespace

} // namespace

#endif
