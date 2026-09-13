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

#ifndef PT_FORMS_PROGRESSBARSTYLE_H
#define PT_FORMS_PROGRESSBARSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

/** @brief Transient visual state of a progress bar.

    %ProgressBarState is the snapshot a %ProgressBar passes to measure,
    layout, and paint. It is not the application model. Enabled and
    focused describe the look of one paint pass.

    @ingroup Pt-Forms-Displays
*/
class PT_FORMS_API ProgressBarState
{
    public:
        /** @brief Constructs an empty progress bar state.
        */
        ProgressBarState();

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

/** @brief Renders the look of a progress bar.

    A %ProgressBarRenderer is a %Style::Facet for the progress-bar
    family. Named measure methods run inside-out. Named layout methods
    run outside-in. Named render methods paint prepared rectangles.
    The widget owns geometry and orchestrates those passes. The
    renderer does not mutate widget geometry.

    Derive a renderer to change the look of progress bars. Register it
    on a %Style, or assign it with %ProgressBar::setRenderer().

    @ingroup Pt-Forms-Displays
*/
class PT_FORMS_API ProgressBarRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit ProgressBarRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~ProgressBarRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ProgressBarRenderer* create() const;

    public:
        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the natural size of the track.
        */
        Gfx::SizeF measureBar(PaintSurface& surface);

        /** @brief Places the track and text rectangles in @a rect.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& barSize,
                          const Gfx::SizeF& textSize,
                          Gfx::RectF& barRect,
                          Gfx::RectF& textRect);

        /** @brief Places the track and filled portion for @a progressRatio.
        */
        void layoutBar(PaintSurface& surface,
                       const Gfx::RectF& barRect,
                       float progressRatio,
                       Gfx::RectF& trackRect,
                       Gfx::RectF& chunkRect);

        /** @brief Returns a painter with the current font and text color.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints track, filled portion, and text for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& chunkRect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const ProgressBarState& state);

        /** @brief Paints the track and filled portion for @a state.
        */
        void renderBar(PaintContext& context,
                       const Gfx::RectF& trackRect,
                       const Gfx::RectF& chunkRect,
                       const ProgressBarState& state);

        /** @brief Paints the track for @a state.
        */
        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         const ProgressBarState& state);

        /** @brief Paints the filled portion for @a state.
        */
        void renderChunk(PaintContext& context,
                         const Gfx::RectF& chunkRect,
                         const ProgressBarState& state);

        /** @brief Paints @a text at @a textPos for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const Gfx::RectF& chunkRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const ProgressBarState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual ProgressBarRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        /** @brief Returns the natural size of the track.
        */
        virtual Gfx::SizeF onMeasureBar(PaintSurface& surface) = 0;

        /** @brief Places the track and text rectangles in @a rect.
        */
        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    const Gfx::SizeF& barSize,
                                    const Gfx::SizeF& textSize,
                                    Gfx::RectF& barRect,
                                    Gfx::RectF& textRect) = 0;

        /** @brief Places the track and filled portion for @a progressRatio.
        */
        virtual void onLayoutBar(PaintSurface& surface,
                                 const Gfx::RectF& barRect,
                                 float progressRatio,
                                 Gfx::RectF& trackRect,
                                 Gfx::RectF& chunkRect) = 0;

        /** @brief Returns a painter with the current font and text color.
        */
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Paints track, filled portion, and text for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& chunkRect,
                                    const Gfx::RectF& textRect,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    const ProgressBarState& state);

        /** @brief Paints the track and filled portion for @a state.
        */
        virtual void onRenderBar(PaintContext& context,
                                 const Gfx::RectF& trackRect,
                                 const Gfx::RectF& chunkRect,
                                 const ProgressBarState& state);

        /** @brief Paints the track for @a state.
        */
        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const ProgressBarState& state) = 0;

        /** @brief Paints the filled portion for @a state.
        */
        virtual void onRenderChunk(PaintContext& context,
                                   const Gfx::RectF& chunkRect,
                                   const ProgressBarState& state) = 0;

        /** @brief Paints @a text at @a textPos for @a state.
        */
        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const Gfx::RectF& chunkRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const ProgressBarState& state) = 0;
};

/** @brief Binds a progress bar to the current style renderer.

    A %ProgressBar owns a %ProgressBarStyler. Applications do not
    construct one. Call %Styler::bind() from %onInvalidate(). When
    the overlay has no local options, bind uses the shared renderer
    from the style. Local options use a private clone. %setRenderer()
    keeps an assigned renderer until it is cleared. A null renderer
    falls back on the next bind.

    Appearance getters return effective tokens after bind. Setters
    write widget-local options. Measure, layout, and paint call the
    typed methods on this styler, not a public renderer accessor.

    @ingroup Pt-Forms-Displays
*/
class PT_FORMS_API ProgressBarStyler : public Styler
{
    public:
        /** @brief Constructs an unbound progress bar styler.
        */
        ProgressBarStyler();

        /** @brief Returns the effective background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local background brush to @a brush.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush to @a brush.
        */
        void setForeground(const Gfx::Brush& brush);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen to @a pen.
        */
        void setContour(const Gfx::Pen& pen);

        /** @brief Returns the effective text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the widget-local text color to @a color.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Returns the effective font.
        */
        Gfx::Font font() const;

        /** @brief Sets the widget-local font to @a font.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the widget-local font size to @a size.
        */
        void setFontSize(std::size_t size);

        /** @brief Sets the widget-local font weight to @a weight.
        */
        void setFontWeight(Gfx::Font::Weight weight);

        /** @brief Sets the widget-local font slant to @a slant.
        */
        void setFontSlant(Gfx::Font::Slant slant);

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Measures the bar's natural size.
        */
        Gfx::SizeF measureBar(PaintSurface& surface) const;

        /** @brief Lays out the bar and text rectangles.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& barSize,
                          const Gfx::SizeF& textSize,
                          Gfx::RectF& barRect,
                          Gfx::RectF& textRect) const;

        /** @brief Lays out the track and chunk rectangles.
        */
        void layoutBar(PaintSurface& surface,
                       const Gfx::RectF& barRect,
                       float progressRatio,
                       Gfx::RectF& trackRect,
                       Gfx::RectF& chunkRect) const;

        /** @brief Returns the text painter for @a surface, or 0 when unavailable.
        */
        const Painter* textPainter(PaintSurface& surface) const;

        /** @brief Renders the chrome within @a rect.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& chunkRect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const ProgressBarState& state) const;

        /** @brief Renders the bar.
        */
        void renderBar(PaintContext& context,
                       const Gfx::RectF& trackRect,
                       const Gfx::RectF& chunkRect,
                       const ProgressBarState& state) const;

        /** @brief Renders the track.
        */
        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         const ProgressBarState& state) const;

        /** @brief Renders the chunk.
        */
        void renderChunk(PaintContext& context,
                         const Gfx::RectF& chunkRect,
                         const ProgressBarState& state) const;

        /** @brief Renders the text.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const Gfx::RectF& chunkRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const ProgressBarState& state) const;

        /** @brief Assigns a specific progress bar renderer.
        */
        void setRenderer(ProgressBarRenderer* renderer = 0);

        /** @brief Returns the bound effective progress bar options.

            Use %StyleOptions::findLocal() to query an explicit widget-local token.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective progress bar options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds the overlay to @a styleOptions and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& styleOptions);

        /** @brief Returns the shared progress-bar renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ProgressBarRenderer> _renderer;
        StyleOptions                  _options;
};

} // namespace

} // namespace

#endif
