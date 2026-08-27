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

#ifndef Pt_Forms_PlatinumStyle_h
#define Pt_Forms_PlatinumStyle_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/ButtonStyle.h>
#include <Pt/Forms/PanelStyle.h>
#include <Pt/Forms/CheckBoxStyle.h>
#include <Pt/Forms/SpinBoxStyle.h>
#include <Pt/Forms/SliderStyle.h>
#include <Pt/Forms/ScrollBarStyle.h>
#include <Pt/Forms/ProgressBarStyle.h>
#include <Pt/Forms/LineEditStyle.h>
#include <Pt/Forms/ComboBoxStyle.h>
#include <Pt/Forms/ListBoxStyle.h>
#include <Pt/Forms/TabViewStyle.h>
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

        void renderChrome(Painter& painter,
                          const Gfx::RectF& rect,
                          const Gfx::Pen& pen,
                          double corner) const;

        void renderChrome(Painter& painter,
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
                         const Gfx::RectF& rect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const PanelState& state);

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const PanelState& state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const PanelState& state);

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const PanelState& state);

    private:
        FacetPtr<PlatinumRendererBase> _base;
        Painter              _bgPainter;
        Painter              _framePainter;
        Painter              _textPainter;
        Painter              _iconPainter;
        double               _cornerRadius;
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
                                          const Gfx::RectF& rect);

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
                                        const ButtonState& state);

        virtual void onPrepareIcon(const Gfx::Image& icon,
                                   Pixmap& picture,
                                   const ButtonState& state) const;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const ButtonState& state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const ButtonState& state);

        virtual void onRenderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& mnemonic,
                                      const ButtonState& state);

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const ButtonState& state);

    private:
        FacetPtr<PlatinumRendererBase> _base;
        Gfx::Color _accentColor;
        Gfx::Color _textColor;
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
                                          const Gfx::RectF& rect);

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

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& boxRect,
                                    const CheckBoxState& state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const CheckBoxState& state);

        virtual void onRenderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& mnemonic,
                                      const CheckBoxState& state);

    private:
        Painter _boxPainter;
        Painter _textPainter;
        Gfx::Color _checkColor;
};


class PT_FORMS_API PlatinumSpinBoxRenderer : public SpinBoxRenderer
{
    public:
        PlatinumSpinBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumSpinBoxRenderer();

    protected:
        virtual SpinBoxRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& spinBoxOptions);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureEntry(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureIndicator(PaintSurface& surface);

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& upButtonRect,
                                    Gfx::RectF& downButtonRect,
                                    Gfx::RectF& textRect);

        virtual Gfx::RectF onLayoutEntry(PaintSurface& surface,
                                         const Gfx::RectF& entryRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& upButtonRect,
                                    const Gfx::RectF& downButtonRect,
                                    const SpinBoxState& state);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const SpinBoxState& state);

        virtual void onRenderUpButton(PaintContext& context,
                                      const Gfx::RectF& buttonRect,
                                      const SpinBoxState& state);

        virtual void onRenderDownButton(PaintContext& context,
                                        const Gfx::RectF& buttonRect,
                                        const SpinBoxState& state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  const SpinBoxState& state);

    private:
        void renderIndicator(Painter& painter,
                             const Gfx::RectF& rect,
                             bool up,
                             bool hovered);

        Gfx::Brush   _background;
        Gfx::Pen     _contour;
        Gfx::Brush   _foreground;
        Gfx::Font    _font;
        Gfx::Color   _textColor;
        Gfx::Color   _accentColor;
        Painter       _bgPainter;
        Painter       _buttonPainter;
        Painter       _textPainter;
        double        _inset;
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
                                   const LineEditState& state);

        virtual void onRenderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const Gfx::RectF& selection,
                                       const LineEditState& state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const LineEditState& state);

        virtual void onRenderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const Gfx::RectF& cursor,
                                    const LineEditState& state);

    private:
        Painter    _bgPainter;
        Painter    _selectionPainter;
        Painter    _textPainter;
        double     _inset;

        Gfx::Brush _background;
        Gfx::Pen   _contour;
        Gfx::Font  _font;
        Gfx::Color _textColor;
        Gfx::Color _accentColor;
        Gfx::Color _selectionBackground;
        Gfx::Color _selectionTextColor;
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

        virtual void onLayoutChrome(PaintSurface& surface,
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
                                   const ProgressBarState& state);

        virtual void onRenderChunk(PaintContext& context,
                                   const Gfx::RectF& chunkRect,
                                   const ProgressBarState& state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const Gfx::RectF& chunkRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const ProgressBarState& state);

    private:
        //Gfx::Brush  _foreground;
        Gfx::Color  _textBackground;
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

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& sliderOptions);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface);

        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface);

        virtual void onLayoutChrome(PaintSurface& surface,
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
                                   const SliderState& state);

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state);

    private:
        Gfx::Brush _hoverBrush;
        Gfx::Brush _contourBrush;
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

        /** @brief Prepares this renderer from effective scroll bar options.
        */
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

        virtual void onLayoutChrome(PaintSurface& surface,
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
                                   Direction direction,
                                   const ScrollBarState& state);

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    Direction direction,
                                    const ScrollBarState& state);

        virtual void onRenderDecreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            Direction direction,
                                            const ScrollBarState& state);

        virtual void onRenderIncreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            Direction direction,
                                            const ScrollBarState& state);

    private:
        Gfx::Brush _background;
        Gfx::Pen   _contour;
        Gfx::Color _accentColor;
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
                                         const Gfx::RectF& rect);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListBoxState& state);

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const ListBoxState& state);

    private:
        Gfx::Brush  _viewBackground;
        Gfx::Pen    _contour;
        Painter     _bgPainter;
        Painter     _framePainter;
};


class PT_FORMS_API PlatinumListItemRenderer : public ListItemRenderer
{
    public:
        PlatinumListItemRenderer(std::size_t refs = 0);

        virtual ~PlatinumListItemRenderer();

    protected:
        virtual ListItemRenderer* onCreate() const;

        /** @brief Prepares the list item renderer from resolved style options.
        */
        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect);

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListItemState& state);

        virtual void onRenderHighlight(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const ListItemState& state);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state);

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state);

    private:
        Gfx::Brush  _background;
        Gfx::Font   _font;
        Gfx::Color  _textColor;
        Gfx::Brush  _highlightBrush;
        Gfx::Color  _highlightedTextColor;
        Painter     _bgPainter;
        Painter     _iconPainter;
        Painter     _textPainter;
};


class PT_FORMS_API PlatinumComboBoxRenderer : public ComboBoxRenderer
{
    public:
        PlatinumComboBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumComboBoxRenderer();

    protected:
        virtual ComboBoxRenderer* onCreate() const;

        /** @brief Prepares the combo box renderer from resolved style options.
        */
        virtual void onPrepare(const StyleOptions& options);

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize);

        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface);

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& buttonRect,
                                    Gfx::RectF& textRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const ComboBoxState& state);

        virtual void onRenderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState);

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  const ComboBoxState& state);

    private:
        Gfx::Brush  _background;
        Gfx::Pen    _contour;
        Gfx::Brush  _foreground;
        Gfx::Font   _font;
        Gfx::Color  _textColor;
        Gfx::Color  _accentColor;
        double      _inset;
        Painter     _bgPainter;
        Painter     _buttonPainter;
        Painter     _textPainter;
};


class PT_FORMS_API PlatinumTabViewRenderer : public TabViewRenderer
{
    public:
        PlatinumTabViewRenderer(std::size_t refs = 0);

        virtual ~PlatinumTabViewRenderer();

    protected:
        virtual TabViewRenderer* onCreate() const;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& tabViewOptions);

        virtual Gfx::SizeF onMeasureTab(PaintSurface& surface,
                                         const Pt::String& text);

        virtual Gfx::RectF onLayoutTab(PaintSurface& surface,
                                        const Gfx::RectF& tabRect);

        virtual const Painter& onGetTextPainter(PaintSurface& surface);

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& contentRect,
                                        const TabViewState& state);

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& contentRect,
                                    const Gfx::RectF& activeTabRect,
                                    const TabViewState& state);

        virtual void onRenderTab(PaintContext& context,
                                 const Gfx::RectF& tabRect,
                                 const Pt::String& text,
                                 const Gfx::PointF& textPos,
                                 const TabViewItemState& state);

    private:
        Gfx::Brush  _background;
        Gfx::Pen    _contour;
        Gfx::Font   _font;
        Gfx::Color  _textColor;
        Gfx::Color  _accentColor;
        double      _inset;
        Painter     _bgPainter;
        Painter     _framePainter;
        Painter     _textPainter;
        Painter     _activeTextPainter;
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
