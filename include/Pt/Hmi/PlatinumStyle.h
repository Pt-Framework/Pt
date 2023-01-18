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

#ifndef Pt_Hmi_PlatinumStyle_h
#define Pt_Hmi_PlatinumStyle_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/String.h>

#include <cstddef>

namespace Pt {

namespace Hmi {

class PlatinumRendererBase
{
    public:
        PlatinumRendererBase();

        virtual ~PlatinumRendererBase();

        void renderFrame(Gfx::Painter& painter, 
                         const Gfx::RectF& rect,
                         const Gfx::Pen& pen,
                         double corner) const;

        void renderPlane(Gfx::Painter& painter,
                         const Gfx::RectF& rect,
                         const Gfx::Brush& brush,
                         double corner) const;

    private:
        static Gfx::Polygon toPolygon(const Gfx::RectF& rect, double inset, double corner);
};


class PT_HMI_API PlatinumButtonRenderer : public ButtonRenderer
{
    public:
        PlatinumButtonRenderer(std::size_t refs = 0);

        virtual ~PlatinumButtonRenderer();

    protected:
        virtual void onPrepare(const PushButton& button,
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const;

        virtual void onPrepareIcon(const PushButton& button,
                                   const StyleOptions& options,
                                   const Gfx::Image& icon,
                                   PixmapSurface& picture) const;

        virtual void onRenderBackground(const PushButton& button,
                                        const StyleOptions& options,
                                        Gfx::Painter& painter,
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush,
                                        const Gfx::Pen& color) const;

        virtual void onRenderText(const PushButton& button,
                                  const StyleOptions& options,
                                  Gfx::Painter& painter,
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& mnemonic) const;

    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumCheckBoxRenderer : public CheckBoxRenderer
{
    public:
        PlatinumCheckBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumCheckBoxRenderer();

    protected:
        virtual void onPrepare(const CheckBox& cb,
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen,
                               Gfx::SizeF& boxRect) const;

        virtual void onRenderBox(const CheckBox& cb,
                                 const StyleOptions& options,
                                 Gfx::Painter& painter,
                                 const Gfx::RectF& rect,
                                 const Gfx::RectF& boxRect,
                                 const Gfx::Brush& brush,
                                 const Gfx::Pen& pen) const;

        virtual void onRenderText(const CheckBox& cb,
                                  const StyleOptions& options,
                                  Gfx::Painter& painter,
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::FontMetrics& textMetric,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& mnemonic) const;

    private:
        PlatinumRendererBase _baseRenderer;
};

class PT_HMI_API PlatinumPanelRenderer : public PanelRenderer
{
    public:
        PlatinumPanelRenderer(std::size_t refs = 0);

        virtual ~PlatinumPanelRenderer();

    protected:
        virtual void onRenderBackground(const Panel& p,
                                        const StyleOptions& options,
                                        Gfx::Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderFrame(const Panel& p,
                                   const StyleOptions& options,
                                   Gfx::Painter& painter, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Pen& pen) const;
    
    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumLabelRenderer : public LabelRenderer
{
    public:
        PlatinumLabelRenderer(std::size_t refs = 0);

        virtual ~PlatinumLabelRenderer();

    protected:
        virtual void onPrepare(const Label& l,
                               const StyleOptions& options,
                               Gfx::Font& font,
                               Gfx::Pen& contour,
                               Gfx::Pen& textPen) const;

        virtual void onRenderBackground(const Label& l,
                                        const StyleOptions& options,
                                        Gfx::Painter& p, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderFrame(const Label& l,
                                   const StyleOptions& options,
                                   Gfx::Painter& p, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Pen& contour) const;

        virtual void onRenderText(const Label& l,
                                  const StyleOptions& options,
                                  Gfx::Painter& p, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen) const;

    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumLineEditRenderer : public LineEditRenderer
{
    public:
        PlatinumLineEditRenderer(std::size_t refs = 0);

        virtual ~PlatinumLineEditRenderer();

    protected:
        virtual void onPrepare(const LineEdit& le, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const;

        virtual void onRenderBackground(const LineEdit& le, 
                                        const StyleOptions& options,
                                        Gfx::Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderText(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Gfx::Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font,
                                  const Gfx::Pen& textPen) const;

        virtual void onRenderCursor(const LineEdit& le, 
                                    const StyleOptions& options,
                                    Gfx::Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& cursorRect ) const;

    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumMenuRenderer : public MenuRenderer
{
    public:
        PlatinumMenuRenderer(std::size_t refs = 0);

        virtual ~PlatinumMenuRenderer();

    protected:
        virtual void onPrepare(const Menu& m, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour) const;
        
        virtual void onRenderBackground(const Menu& m, 
                                        const StyleOptions& options,
                                        Gfx::Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush,
                                        const Gfx::Pen& contour) const;
        
        virtual void onPrepareItem(const MenuItem& m, 
                                   const StyleOptions& options,
                                   const Gfx::Image& icon,
                                   PixmapSurface& picture,
                                   Gfx::Brush& brush,
                                   Gfx::Pen& contour,
                                   Gfx::Font& font,
                                   Gfx::Pen& textPen) const;



        virtual void onRenderItem(const MenuItem& m, 
                                  const StyleOptions& options,
                                  Gfx::Painter& painter, 
                                  const Gfx::RectF& rect,
                                  Gfx::Brush& brush,
                                  Gfx::Pen& contour) const;

        virtual void onRenderIndicator(const MenuItem& m, 
                                       const StyleOptions& options,
                                       Gfx::Painter& painter, 
                                       const Gfx::RectF& rect) const;

    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumMenuBarRenderer : public MenuBarRenderer
{
    public:
        PlatinumMenuBarRenderer(std::size_t refs = 0);

        virtual ~PlatinumMenuBarRenderer();

    protected:
        virtual void onPrepare(const MenuBar& m, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour) const;

        virtual void onRenderBackground(const MenuBar& m, 
                                        const StyleOptions& options,
                                        Gfx::Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush,
                                        const Gfx::Pen& contour) const;

        virtual void onPrepareItem(const MenuBarItem& m, 
                                   const StyleOptions& options, 
                                   Gfx::Brush& brush,
                                   Gfx::Pen& contour,
                                   Gfx::Font& font,
                                   Gfx::Pen& textPen) const;

        virtual void onRenderItem(const MenuBarItem& m, 
                                  const StyleOptions& options,
                                  Gfx::Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const Gfx::Brush& brush,
                                  const Gfx::Pen& contour) const;

        virtual void onRenderItemText(const MenuBarItem& m,
                                      const StyleOptions& options,
                                      Gfx::Painter& painter, 
                                      const Gfx::RectF& rect,
                                      const String& text,
                                      const Gfx::PointF& textPos,
                                      const Gfx::Font& font, 
                                      const Gfx::Pen& textPen,
                                      const Gfx::RectF& mnemonic) const;
    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumScrollBarRenderer : public ScrollBarRenderer
{
    public:
        PlatinumScrollBarRenderer(std::size_t refs = 0);

        virtual ~PlatinumScrollBarRenderer();

    protected:
        virtual void onPrepare(const ScrollBar& s,
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Brush& foreground,
                               Gfx::Pen& contour) const;
        
        virtual void onRender(const ScrollBar& s,
                              const StyleOptions& options,
                              Gfx::Painter& painter,
                              const Gfx::RectF& rect,
                              const Gfx::RectF& handleRect,
                              const Gfx::Brush& background,
                              const Gfx::Brush& foreground,
                              const Gfx::Pen& contour) const;
    
    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumProgressBarRenderer : public ProgressBarRenderer
{
    public:
        PlatinumProgressBarRenderer(std::size_t refs = 0);

        virtual ~PlatinumProgressBarRenderer();

    protected:
        virtual void onPrepare(const ProgressBar&    p,
                               const StyleOptions&  options,
                               Gfx::Brush&          background,
                               Gfx::Brush&          foreground,
                               Gfx::Pen&            contour,
                               Gfx::Pen&            textPen,
                               Gfx::Font&            font
                               ) const;

        virtual void onRender( const ProgressBar& p,
                               const StyleOptions& options,
                              Gfx::Painter& painter,
                              const Gfx::RectF& rect,
                              const Gfx::Brush& background,
                              const Gfx::Brush& foreground,
                              const Gfx::Pen& contour,
                              const Gfx::Pen&            textPen,
                              const Gfx::Font&            font
                                         ) const;
};


class PT_HMI_API PlatinumSliderRenderer : public SliderRenderer
{
    public:
        PlatinumSliderRenderer(std::size_t refs = 0);

        virtual ~PlatinumSliderRenderer();

    protected:
        virtual void onPrepare( const Slider&        s,
                                const StyleOptions&  options,
                                Gfx::Brush&          background,
                                Gfx::Brush&          foreground,
                                Gfx::Pen&            contour,
                                Gfx::Pen&            textPen,
                                Gfx::Font&          font
                               ) const;

        virtual void onRender( const Slider&        s,
                               const StyleOptions&  options,
                               Gfx::Painter&              painter,
                               const Gfx::RectF&    rect,
                               const Gfx::Brush&    background,
                               const Gfx::Brush&    foreground,
                               const Gfx::Pen&      contour,
                               const Gfx::Pen&      textPen,
                               const Gfx::Font&      font
                              ) const;
};


class PT_HMI_API PlatinumListBoxRenderer : public ListBoxRenderer
{
    public:
        PlatinumListBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumListBoxRenderer();

    protected:
        virtual void onPrepareLayout(Spacing& frameSize);

        virtual void onRenderBackground(const ListBox& p,
                                        const StyleOptions& options,
                                        Gfx::Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderFrame(const ListBox& p,
                                   const StyleOptions& options,
                                   Gfx::Painter& painter, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Pen& pen) const;
        
        virtual void onPrepareItem(const ListBoxItem& item, 
                                   const StyleOptions& options, 
                                   Gfx::Brush& brush,
                                   Gfx::Pen& contour,
                                   Gfx::Font& font,
                                   Gfx::Pen& textPen) const;

        virtual void onRenderItem(const ListBoxItem& item, 
                                  const StyleOptions& options,
                                  Gfx::Painter& painter, 
                                  const Gfx::RectF& rect,
                                  Gfx::Brush& brush,
                                  Gfx::Pen& contour) const;
    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumComboBoxRenderer : public ComboBoxRenderer
{
    public:
        PlatinumComboBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumComboBoxRenderer();

    protected:
        virtual void onPrepare(const ComboBox& cb, 
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Brush& foreground,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const;
        
        virtual void onPrepareLayout(const ComboBox& cb,
                                     Gfx::SizeF& buttonSize) const;
        
        virtual void onRenderBackground(const ComboBox& cb, 
                                        const StyleOptions& options,
                                        Gfx::Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderButton(const ComboBox& cb, 
                                    const StyleOptions& options,
                                    Gfx::Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::Pen& contour,
                                    const Gfx::Brush& foreground) const;

        virtual void onRenderText(const ComboBox& cb,
                                  const StyleOptions& options,
                                  Gfx::Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& cursor) const;
    
    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumSpinBoxRenderer : public SpinBoxRenderer
{
    public:
        PlatinumSpinBoxRenderer(std::size_t refs = 0);

        virtual ~PlatinumSpinBoxRenderer();

    protected:
        virtual void onPrepare(const SpinBox& cb, 
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const;
        
        virtual void onPrepareButton(const SpinBoxButton& sb, 
                                     const StyleOptions& options,
                                     Gfx::Brush& foreground,
                                     Gfx::Pen& contour) const;
        
        virtual void onLayout(const SpinBox& cb,
                              Gfx::RectF& downButton,
                              Gfx::RectF& upButton,
                              Gfx::RectF& textBox) const;
        
        virtual void onRenderBackground(const SpinBox& cb, 
                                        const StyleOptions& options,
                                        Gfx::Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderButton(const SpinBoxButton& sb, 
                                    const StyleOptions& options,
                                    Gfx::Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::Brush& foreground,
                                    const Gfx::Pen& contour) const;

        virtual void onRenderText(const SpinBox& cb,
                                  const StyleOptions& options,
                                  Gfx::Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& cursor) const;
};


class PT_HMI_API PlatinumTabViewRenderer : public TabViewRenderer
{
    public:
        PlatinumTabViewRenderer(std::size_t refs = 0);

        virtual ~PlatinumTabViewRenderer();
        
    protected:
        virtual void onPrepare(const TabView& tv,
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Brush& foreground,
                               Gfx::Pen& contour) const;

        virtual void onRender(const TabView& tv,
                              const StyleOptions& options,
                              Gfx::Painter& painter,
                              const Gfx::RectF& rect,
                              const Gfx::Brush& background,
                              const Gfx::Brush& foreground,
                              const Gfx::Pen& contour) const;

        virtual Gfx::SizeF onMeasureTabs(Gfx::PaintSurface& surface,
                                         const std::vector<TabItem>& tabs,
                                         const Gfx::Font& font) const;

        virtual void onLayoutTabs(Gfx::PaintSurface& surface,
                                  std::vector<TabItem>& tabs,
                                  const Gfx::RectF& rect, 
                                  const Gfx::Font& font) const;

        virtual void onPrepareTabs(const TabBar& tabs,
                                   const StyleOptions& options,
                                   const Gfx::Brush& background,
                                   const Gfx::Brush& foreground,
                                   const Gfx::Pen& contour,
                                   const Gfx::Font& font, 
                                   const Gfx::Pen& textPen) const;

        virtual void onRenderTabs(const std::vector<TabItem>& tabs,
                                  const StyleOptions& options,
                                  Gfx::Painter& painter,
                                  const Gfx::RectF& rect,
                                  const Gfx::Brush& background,
                                  const Gfx::Brush& foreground,
                                  const Gfx::Pen& contour,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen) const;
};


class PT_HMI_API PlatinumStyle : public Style
{
    public:
        PlatinumStyle();

        ~PlatinumStyle();
};

} // namespace

} // namespace

#endif
