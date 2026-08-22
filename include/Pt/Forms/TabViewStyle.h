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

#ifndef PT_FORMS_TABVIEWSTYLE_H
#define PT_FORMS_TABVIEWSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API TabViewState
{
    public:
        TabViewState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};


class PT_FORMS_API TabViewItemState
{
    public:
        TabViewItemState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isActive() const;

        void setActive(bool value);

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isPressed() const;

        void setPressed(bool value);

    private:
        bool _enabled;
        bool _active;
        bool _highlighted;
        bool _pressed;
};


class PT_FORMS_API TabViewRenderer : public Style::Facet
{
    public:
        explicit TabViewRenderer(std::size_t refs = 0);

        virtual ~TabViewRenderer();

        TabViewRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const StyleOptions& tabViewOptions);

    public:
        Gfx::SizeF measureTab(PaintSurface& surface,
                              const Pt::String& text);

        Gfx::RectF layoutTab(PaintSurface& surface,
                             const Gfx::RectF& tabRect);

        const Painter& textPainter(PaintSurface& surface);

        void renderBackground(PaintContext& context,
                              const Gfx::RectF& contentRect,
                              const TabViewState& state);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& contentRect,
                          const Gfx::RectF& activeTabRect,
                          const TabViewState& state);

        void renderTab(PaintContext& context,
                       const Gfx::RectF& tabRect,
                       const Pt::String& text,
                       const Gfx::PointF& textPos,
                       const TabViewItemState& state);

    protected:
        virtual void onReset(const StyleOptions& options);

        virtual TabViewRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& tabViewOptions) = 0;

        virtual Gfx::SizeF onMeasureTab(PaintSurface& surface,
                                        const Pt::String& text) = 0;

        virtual Gfx::RectF onLayoutTab(PaintSurface& surface,
                                       const Gfx::RectF& tabRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& contentRect,
                                        const TabViewState& state) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& contentRect,
                                    const Gfx::RectF& activeTabRect,
                                    const TabViewState& state) = 0;

        virtual void onRenderTab(PaintContext& context,
                                 const Gfx::RectF& tabRect,
                                 const Pt::String& text,
                                 const Gfx::PointF& textPos,
                                 const TabViewItemState& state) = 0;
};


class PT_FORMS_API TabViewStyle : public Styler<TabViewRenderer,
                                                     StyleOptions>
{
    public:
        TabViewStyle();
};

} // namespace

} // namespace

#endif
