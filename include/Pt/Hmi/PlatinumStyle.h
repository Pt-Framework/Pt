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

#ifndef Pt_Hmi_PlatinumStyle_h
#define Pt_Hmi_PlatinumStyle_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

class Painter;

class PlatinumRendererBase
{
    public:
        PlatinumRendererBase();

        virtual ~PlatinumRendererBase();

        void renderFrame(Painter& painter, 
                         const Gfx::RectF& rect,
                         const Gfx::Pen& pen,
                         double corner = 1.0) const;

        void renderPlane(Painter& painter,
                         const Gfx::RectF& rect,
                         const Gfx::Brush& brush,
                         double corner = 1.0) const;


        void renderItemText(Painter& painter, 
                            const Gfx::PointF& textPos,
                            const String& text, 
                            const Char* mnemonic,
                            const Gfx::Font& font,
                            const Gfx::Color& color) const;
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
                                   Picture& picture) const;

        virtual void onRenderBackground(const PushButton& button,
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush,
                                        const Gfx::Pen& color) const;

        virtual void onRenderText(const PushButton& button,
                                  const StyleOptions& options,
                                  Painter& painter, 
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
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 const Gfx::RectF& boxRect,
                                 const Gfx::Brush& brush,
                                 const Gfx::Pen& pen) const;

        virtual void onRenderText(const CheckBox& cb,
                                  const StyleOptions& options,
                                  Painter& painter, 
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
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderFrame(const Panel& p,
                                   const StyleOptions& options,
                                   Painter& painter, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Color& borderColor) const;
    
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
                               Gfx::Pen& textPen) const;

        virtual void onRenderBackground(const Label& l,
                                        const StyleOptions& options,
                                        Painter& p, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const;

        virtual void onRenderFrame(const Label& l,
                                   const StyleOptions& options,
                                   Painter& p, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Pen& contour) const;

        virtual void onRenderText(const Label& l,
                                  const StyleOptions& options,
                                  Painter& p, 
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
        virtual void onRenderItem(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const Gfx::Color& foreground,
                                  const Gfx::Brush& background) const;

        virtual void onRenderText(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font,
                                  const Gfx::Color& textColor) const;

        virtual void onRenderCursor(const LineEdit& le, 
                                    const StyleOptions& options,
                                    Painter& painter, 
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
        virtual void onRender(const Menu& m, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const;

        virtual void onRenderItem(const MenuItem& m, 
                                  PaintSurface& surface, 
                                  const Gfx::RectF& rect) const;

        virtual void onRenderIndicator(const MenuItem& m, 
                                       PaintSurface& surface, 
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
        virtual void onRender(const MenuBar& m, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const;

        virtual void onRenderItem(const MenuBarItem& m, 
                                  PaintSurface& surface, 
                                  const Gfx::RectF& rect) const;

    private:
        PlatinumRendererBase _baseRenderer;
};


class PT_HMI_API PlatinumScrollBarRenderer : public ScrollBarRenderer
{
    public:
        PlatinumScrollBarRenderer(std::size_t refs = 0);

        virtual ~PlatinumScrollBarRenderer();

    protected:
        virtual void onRender(const ScrollBar& s,
                              const Gfx::RectF& handleRect,
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const;

    private:
        PlatinumRendererBase _baseRenderer;
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
