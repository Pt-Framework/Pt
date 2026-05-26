/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner
   Copyright (C) 2017 Ilja Maier
 
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

#ifndef Pt_Forms_PlatinumStyle_h
#define Pt_Forms_PlatinumStyle_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Path.h>
#include <Pt/String.h>

#include <cstddef>

namespace Pt {

namespace Forms {

class PlatinumRendererBase : public Style::Facet
{
    public:
        PlatinumRendererBase(std::size_t refs = 0);

        virtual ~PlatinumRendererBase();

        void renderFrame(Painter& painter, 
                         const Gfx::RectF& rect,
                         const Gfx::Pen& pen,
                         double corner) const;

        void renderFrame(Painter& painter,
                         const Gfx::RectF& rect,
                         double penSize,
                         double corner) const;

        void renderPlane(Painter& painter,
                         const Gfx::RectF& rect,
                         const Gfx::Brush& brush,
                         double corner) const;

        void renderPlane(Painter& painter,
                         const Gfx::RectF& rect,
                         double corner) const;

    private:
        static Gfx::Polygon getPolygon(const Gfx::RectF& rect, double inset, double corner);
};


class PT_FORMS_API PlatinumPanelRenderer : public PanelRenderer
{
    public:
        PlatinumPanelRenderer(FacetPtr<PlatinumRendererBase> base,
                              std::size_t refs = 0);

        virtual ~PlatinumPanelRenderer();

    protected:
        virtual PanelRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        StyleFlags state);

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const StyleOptions& options,
                                   StyleFlags state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  StyleFlags state);

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  StyleFlags state);

    private:
        FacetPtr<PlatinumRendererBase> _base;
        Painter              _bgPainter;
        Painter              _framePainter;
        Painter              _textPainter;
        Painter              _iconPainter;
};


class PT_FORMS_API PlatinumButtonRenderer : public ButtonRenderer
{
    public:
        PlatinumButtonRenderer(FacetPtr<PlatinumRendererBase> base,
                               std::size_t refs = 0);

        virtual ~PlatinumButtonRenderer();

    protected:
        virtual ButtonRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            Direction direction,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize);

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect);

        virtual Gfx::RectF onLayoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     Direction direction,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        ButtonStyleFlags state);

        virtual void onPrepareIcon(const StyleOptions& options,
                                   const Gfx::Image& icon,
                                   Pixmap& picture,
                                   ButtonStyleFlags state) const;

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const StyleOptions& options,
                                   ButtonStyleFlags state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  ButtonStyleFlags state);

        virtual void onRenderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const StyleOptions& options,
                                      const Gfx::RectF& mnemonic,
                                      ButtonStyleFlags state);

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  ButtonStyleFlags state);

    private:
        FacetPtr<PlatinumRendererBase> _base;
        Painter _normalPainter;
        Painter _pressedPainter;
        Painter _highlightPainter;
        Painter _textPainter;
        Painter _iconPainter;
};


class PT_FORMS_API PlatinumCheckBoxRenderer : public CheckBoxRenderer
{
    public:
        PlatinumCheckBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumCheckBoxRenderer();

    protected:
        virtual CheckBoxRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureIndicator(PaintSurface& surface);

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& indicatorSize,
                                            const Gfx::SizeF& textSize);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect);

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& indicatorSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& indicatorRect,
                                     Gfx::RectF& textRect);

        virtual Gfx::RectF onLayoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderIndicator(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const StyleOptions& options,
                                       const Gfx::RectF& boxRect,
                                       CheckBoxStyleFlags state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  CheckBoxStyleFlags state);

        virtual void onRenderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const StyleOptions& options,
                                      const Gfx::RectF& mnemonic,
                                      CheckBoxStyleFlags state);

    private:
        Painter _boxPainter;
        Painter _textPainter;
};


class PT_FORMS_API PlatinumSpinBoxRenderer : public SpinBoxRenderer
{
    public:
        PlatinumSpinBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumSpinBoxRenderer();

    protected:
        virtual SpinBoxRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureEntry(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureIndicator(PaintSurface& surface);

        virtual void onLayoutFrame(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Gfx::RectF& entryRect,
                                   Gfx::RectF& upButtonRect,
                                   Gfx::RectF& downButtonRect,
                                   Gfx::RectF& textRect);

        virtual Gfx::RectF onLayoutEntry(PaintSurface& surface,
                                         const Gfx::RectF& entryRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const StyleOptions& options,
                                   const Gfx::RectF& entryRect,
                                   const Gfx::RectF& upButtonRect,
                                   const Gfx::RectF& downButtonRect,
                                   SpinBoxStyleFlags state,
                                   ButtonStyleFlags upButtonState,
                                   ButtonStyleFlags downButtonState);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const StyleOptions& options,
                                   SpinBoxStyleFlags state);

        virtual void onRenderUpButton(PaintContext& context,
                                      const Gfx::RectF& buttonRect,
                                      const StyleOptions& options,
                                      SpinBoxStyleFlags state,
                                      ButtonStyleFlags buttonState);

        virtual void onRenderDownButton(PaintContext& context,
                                        const Gfx::RectF& buttonRect,
                                        const StyleOptions& options,
                                        SpinBoxStyleFlags state,
                                        ButtonStyleFlags buttonState);

        virtual void onRenderText(PaintContext& context,
                                  const StyleOptions& options,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  SpinBoxStyleFlags state);

    private:
        void renderIndicator(Painter& painter,
                             const Gfx::RectF& rect,
                             bool up,
                             ButtonStyleFlags state,
                             const StyleOptions& options);

        Painter _bgPainter;
        Painter _buttonPainter;
        Painter _textPainter;
        double _inset;
};


class PT_FORMS_API PlatinumLineEditRenderer : public LineEditRenderer
{
    public:
        PlatinumLineEditRenderer(std::size_t refs = 0);

        virtual ~PlatinumLineEditRenderer();

    protected:
        virtual LineEditRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                     const Gfx::SizeF& contentSize);

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                    const Gfx::RectF& rect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderEntry(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        LineEditStyleFlags state);

        virtual void onRenderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const StyleOptions& options,
                                       const Gfx::RectF& selection,
                                       LineEditStyleFlags state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  LineEditStyleFlags state);

        virtual void onRenderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const StyleOptions& options,
                                    const Gfx::RectF& cursor,
                                    LineEditStyleFlags state);

    private:
        Painter _bgPainter;
        Painter _selectionPainter;
        Painter _textPainter;
        double  _inset;
};


class PT_FORMS_API PlatinumProgressBarRenderer : public ProgressBarRenderer
{
    public:
        PlatinumProgressBarRenderer(std::size_t refs = 0);

        virtual ~PlatinumProgressBarRenderer();

    protected:
        virtual ProgressBarRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureBar(PaintSurface& surface);

        virtual void onLayoutFrame(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   const Gfx::SizeF& barSize,
                                   const Gfx::SizeF& textSize,
                                   Gfx::RectF& barRect,
                                   Gfx::RectF& textRect);

        virtual void onLayoutBar(PaintSurface& surface,
                                 const Gfx::RectF& barRect,
                                 float progressRatio,
                                 Gfx::RectF& trackRect,
                                 Gfx::RectF& chunkRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const StyleOptions& options,
                                   ProgressBarStyleFlags state);

        virtual void onRenderChunk(PaintContext& context,
                                   const Gfx::RectF& chunkRect,
                                   const StyleOptions& options,
                                   ProgressBarStyleFlags state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const Gfx::RectF& chunkRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  ProgressBarStyleFlags state);

    private:
        Painter _trackPainter;
        Painter _chunkPainter;
        Painter _textPainter;
        Painter _invertTextPainter;
};

class PT_FORMS_API PlatinumSliderRenderer : public SliderRenderer
{
    public:
        PlatinumSliderRenderer(std::size_t refs = 0);

        virtual ~PlatinumSliderRenderer();

    protected:
        virtual SliderRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface);

        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface);

        virtual void onLayoutFrame(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   const Gfx::SizeF& trackSize,
                                   const Gfx::SizeF& handleSize,
                                   Gfx::RectF& trackRect,
                                   Gfx::RectF& handleRect);

        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    float fraction,
                                    Gfx::RectF& handleRect);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const StyleOptions& options,
                                   SliderStyleFlags state);

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    const StyleOptions& options,
                                    SliderStyleFlags state);

    private:
        Painter _trackPainter;
        Painter _handlePainter;
};


class PT_FORMS_API PlatinumScrollBarRenderer : public ScrollBarRenderer
{
    public:
        PlatinumScrollBarRenderer(std::size_t refs = 0);

        virtual ~PlatinumScrollBarRenderer();

    protected:
        virtual ScrollBarRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize,
                                           Direction direction);

        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface,
                                           Direction direction);

        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface,
                                            Direction direction);

        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface,
                                            Direction direction);

        virtual void onLayoutFrame(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Direction direction,
                                   const Gfx::SizeF& buttonSize,
                                   Gfx::RectF& trackRect,
                                   Gfx::RectF& decreaseRect,
                                   Gfx::RectF& increaseRect);

        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    Direction direction,
                                    float fraction,
                                    float viewProportion,
                                    Gfx::RectF& handleRect);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const StyleOptions& options,
                                   Direction direction,
                                   ScrollBarStyleFlags state);

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    const StyleOptions& options,
                                    Direction direction,
                                    ScrollBarStyleFlags state);

        virtual void onRenderDecreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            const StyleOptions& options,
                                            Direction direction,
                                            ScrollBarStyleFlags state,
                                            ButtonStyleFlags buttonState);

        virtual void onRenderIncreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            const StyleOptions& options,
                                            Direction direction,
                                            ScrollBarStyleFlags state,
                                            ButtonStyleFlags buttonState);

    private:
        Painter _trackPainter;
        Painter _handlePainter;
        Painter _buttonPainter;
};


class PT_FORMS_API PlatinumListBoxRenderer : public ListBoxRenderer
{
    public:
        PlatinumListBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumListBoxRenderer();

    protected:
        virtual ListBoxRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        ListBoxStyleFlags state);

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const StyleOptions& options,
                                   ListBoxStyleFlags state);

    private:
        Painter              _bgPainter;
        Painter              _framePainter;
};


class PT_FORMS_API PlatinumListItemRenderer : public ListItemRenderer
{
    public:
        PlatinumListItemRenderer(std::size_t refs = 0);

        virtual ~PlatinumListItemRenderer();

    protected:
        virtual ListItemRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect);

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        ListItemStyleFlags state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state);

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const StyleOptions& options,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state);

    private:
        Painter _bgPainter;
        Painter _iconPainter;
        Painter _textPainter;
};


class PT_FORMS_API PlatinumComboBoxRenderer : public ComboBoxRenderer
{
    public:
        PlatinumComboBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumComboBoxRenderer();

    protected:
        virtual ComboBoxRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface);

        virtual void onLayoutFrame(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Gfx::RectF& entryRect,
                                   Gfx::RectF& buttonRect,
                                   Gfx::RectF& textRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& entryRect,
                                   const Gfx::RectF& buttonRect,
                                   const StyleOptions& options,
                                   ComboBoxStyleFlags state,
                                   ButtonStyleFlags buttonState);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const StyleOptions& options,
                                   ComboBoxStyleFlags state);

        virtual void onRenderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    const StyleOptions& options,
                                    ComboBoxStyleFlags state,
                                    ButtonStyleFlags buttonState);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  ComboBoxStyleFlags state);

    private:
        double _inset;
        Painter _bgPainter;
        Painter _buttonPainter;
        Painter _textPainter;
};


class PT_FORMS_API PlatinumTabViewRenderer : public TabViewRenderer
{
    public:
        PlatinumTabViewRenderer(std::size_t refs = 0);

        virtual ~PlatinumTabViewRenderer();
        
    protected:
        virtual TabViewRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureTab(PaintSurface& surface,
                                         const Pt::String& text);

        virtual Gfx::RectF onLayoutTab(PaintSurface& surface,
                                        const Gfx::RectF& tabRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& contentRect,
                                        const StyleOptions& options,
                                        TabViewStyleFlags state);

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& contentRect,
                                   const Gfx::RectF& activeTabRect,
                                   const StyleOptions& options,
                                   TabViewStyleFlags state);

        virtual void onRenderTab(PaintContext& context,
                                 const Gfx::RectF& tabRect,
                                 const Pt::String& text,
                                 const Gfx::PointF& textPos,
                                 const StyleOptions& options,
                                 TabItemStyleFlags state);

    private:
        double  _inset;
        Painter _bgPainter;
        Painter _framePainter;
        Painter _textPainter;
        Painter _activeTextPainter;
};


class PT_FORMS_API PlatinumStyle : public Style
{
    public:
        PlatinumStyle();

        ~PlatinumStyle();
};

} // namespace

} // namespace

#endif
