/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner

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

#ifndef Pt_Forms_PanelStyle_h
#define Pt_Forms_PanelStyle_h

#include <Pt/Forms/StylerBase.h>

namespace Pt {

namespace Forms {

/** @brief Stores the transient render state for panel-like widgets.

    Carries only widget state that render hooks may observe directly.
*/
class PT_FORMS_API PanelState
{
    public:
        /** @brief Constructs an empty panel state.
        */
        PanelState();

        /** @brief Returns true if the widget is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the widget is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the widget currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the widget has focus.
        */
        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};

/** @brief Renders the visual appearance of a panel-like widget.

    Provides rendering primitives for panel backgrounds, frames, text,
    and icons. Subclasses override the protected virtuals.
*/
class PT_FORMS_API PanelRenderer : public Renderer
{
    public:
        explicit PanelRenderer(std::size_t refs = 0);

        virtual ~PanelRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        PanelRenderer* create() const;

        /** @brief Applies the panel style options to this renderer.
        */
        void prepare(const StyleOptions& options);

    public:
        /** @brief Returns the outer size including the frame for the given content size.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the inner content rectangle within the outer frame rect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& frameRect);

        /** @brief Returns a painter with the current font and text color applied.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints the panel background within the widget rect.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const PanelState& state);

        /** @brief Paints the frame border within the outer frame rect.
        */
        void renderFrame(PaintContext& context,
                         const Gfx::RectF& rect,
                         const PanelState& state);

        /** @brief Draws text at pos, clipped to the widget rect.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const PanelState& state);

        /** @brief Draws picture at pos, clipped to the widget rect.
        */
        void renderIcon(PaintContext& context,
                        const Gfx::RectF& rect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        const PanelState& state);

    protected:
        /** @brief Resets the shared panel renderer to global defaults.
        */
        virtual void onReset(const StyleOptions& options);

        virtual PanelRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const PanelState& state) = 0;

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const PanelState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const PanelState& state) = 0;

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const PanelState& state) = 0;
};

/** @brief Panel styler.
*/
class PT_FORMS_API PanelStyler : public StylerBase
{
    public:
        /** @brief Constructs an unbound panel styler.
        */
        PanelStyler();

        /** @brief Assigns a specific panel renderer.
        */
        void setRenderer(PanelRenderer* renderer = 0);

        /** @brief Returns the bound panel renderer or 0.
        */
        PanelRenderer* renderer();

        /** @brief Returns the panel style options.
        */
        StyleOptions& options();

        /** @brief Returns the panel style options.
        */
        const StyleOptions& options() const;

    protected:
        virtual StyleOptions& onBindOptions(const StyleOptions& styleOptions);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<PanelRenderer> _renderer;
        StyleOptions            _options;
};

} // namespace

} // namespace

#endif
