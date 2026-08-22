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

class PT_FORMS_API ProgressBarState
{
    public:
        ProgressBarState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};


class PT_FORMS_API ProgressBarRenderer : public Style::Facet
{
    public:
        explicit ProgressBarRenderer(std::size_t refs = 0);

        virtual ~ProgressBarRenderer();

        ProgressBarRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const StyleOptions& progressBarOptions);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::SizeF measureBar(PaintSurface& surface);

        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& barSize,
                          const Gfx::SizeF& textSize,
                          Gfx::RectF& barRect,
                          Gfx::RectF& textRect);

        void layoutBar(PaintSurface& surface,
                       const Gfx::RectF& barRect,
                       float progressRatio,
                       Gfx::RectF& trackRect,
                       Gfx::RectF& chunkRect);

        const Painter& textPainter(PaintSurface& surface);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& chunkRect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const ProgressBarState& state);

        void renderBar(PaintContext& context,
                       const Gfx::RectF& trackRect,
                       const Gfx::RectF& chunkRect,
                       const ProgressBarState& state);

        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         const ProgressBarState& state);

        void renderChunk(PaintContext& context,
                         const Gfx::RectF& chunkRect,
                         const ProgressBarState& state);

        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const Gfx::RectF& chunkRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const ProgressBarState& state);

    protected:
        virtual void onReset(const StyleOptions& options);

        virtual ProgressBarRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& progressBarOptions) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureBar(PaintSurface& surface) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    const Gfx::SizeF& barSize,
                                    const Gfx::SizeF& textSize,
                                    Gfx::RectF& barRect,
                                    Gfx::RectF& textRect) = 0;

        virtual void onLayoutBar(PaintSurface& surface,
                                 const Gfx::RectF& barRect,
                                 float progressRatio,
                                 Gfx::RectF& trackRect,
                                 Gfx::RectF& chunkRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& chunkRect,
                                    const Gfx::RectF& textRect,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    const ProgressBarState& state);

        virtual void onRenderBar(PaintContext& context,
                                 const Gfx::RectF& trackRect,
                                 const Gfx::RectF& chunkRect,
                                 const ProgressBarState& state);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const ProgressBarState& state) = 0;

        virtual void onRenderChunk(PaintContext& context,
                                   const Gfx::RectF& chunkRect,
                                   const ProgressBarState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const Gfx::RectF& chunkRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const ProgressBarState& state) = 0;
};


class PT_FORMS_API ProgressBarStyle : public Styler<ProgressBarRenderer,
                                                         StyleOptions>
{
    public:
        ProgressBarStyle();
};

} // namespace

} // namespace

#endif
