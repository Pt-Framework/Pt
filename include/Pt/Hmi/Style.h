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

#ifndef Pt_Hmi_Style_h
#define Pt_Hmi_Style_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/TypeInfo.h>
#include <Pt/NonCopyable.h>
#include <map>

namespace Pt {

namespace Hmi {

class PaintSurface;
class Painter;
class Picture;
class StyleOptions;
class Panel;
class Label;
class LineEdit;
class PushButton;
class CheckBox;
class Menu;
class MenuItem;
class MenuBar;
class MenuBarItem;
class ScrollBar;

class PT_HMI_API Style
{
    public:
        class Facet : private NonCopyable
        {
            public:
                explicit Facet(const std::type_info& ti, std::size_t refs = 0)
                : _typeId(&ti)
                , _refs(refs)
                {}

                virtual ~Facet()
                {}

                const std::type_info& typeId() const
                {
                    return *_typeId;
                }

                void ref()
                { 
                    ++_refs; 
                }

                std::size_t unref()
                { 
                    return --_refs; 
                }

            private:
                const std::type_info* _typeId;
                std::size_t _refs;
        };

    public:
        Style();

        Style(const Style& style);

        virtual ~Style();

        Style& operator=(const Style& style);

        void assign(const Style& style);

        void combine(const Style& style);

        void set(Facet* facet);

        template <typename FacetT> 
        const FacetT* get() const
        {
            const Facet* facet = find( typeid(FacetT) );
            return static_cast<const FacetT*>(facet);
        }

    private:
        const Facet* find(const std::type_info& ti) const;

    private:
        typedef std::map<TypeInfo, Facet*> FacetMap;
        FacetMap _facets;
};


class PT_HMI_API ButtonRenderer : public Style::Facet
{
    public:
        ButtonRenderer(std::size_t refs = 0);

        virtual ~ButtonRenderer();

        void prepare(const PushButton& button,
                     const StyleOptions& options,
                     Gfx::Brush& brush,
                     Gfx::Pen& contour,
                     Gfx::Font& font,
                     Gfx::Pen& textPen) const;

        void prepareIcon(const PushButton& button,
                         const StyleOptions& options,
                         const Gfx::Image& icon,
                         Picture& picture) const;

        void renderBackground(const PushButton& button,
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Brush& brush,
                              const Gfx::Pen& pen) const;
        
        void renderText(const PushButton& button,
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::Font& font, 
                        const Gfx::Pen& textPen,
                        const Gfx::RectF& mnemonic) const;

    protected:
        virtual void onPrepare(const PushButton& button,
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const = 0;

        virtual void onPrepareIcon(const PushButton& button,
                                   const StyleOptions& options,
                                   const Gfx::Image& icon,
                                   Picture& picture) const = 0;

        virtual void onRenderBackground(const PushButton& button,
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush,
                                        const Gfx::Pen& pen) const = 0;

        virtual void onRenderText(const PushButton& button,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& mnemonic) const = 0;
};


class PT_HMI_API CheckBoxRenderer : public Style::Facet
{
    public:
        CheckBoxRenderer(std::size_t refs = 0);

        virtual ~CheckBoxRenderer();

        void prepare(const CheckBox& cb,
                     const StyleOptions& options,
                     Gfx::Brush& brush,
                     Gfx::Pen& contour,
                     Gfx::Font& font,
                     Gfx::Pen& textPen,
                     Gfx::SizeF& boxSize) const;

        void renderBox(const CheckBox& cb,
                       const StyleOptions& options,
                       Painter& painter, 
                       const Gfx::RectF& rect,
                       const Gfx::RectF& boxRect,
                       const Gfx::Brush& brush,
                       const Gfx::Pen& pen) const;

        void renderText(const CheckBox& cb,
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::FontMetrics& textMetric,
                        const Gfx::Font& font, 
                        const Gfx::Pen& textPen,
                        const Gfx::RectF& mnemonic) const;

    protected:
        virtual void onPrepare(const CheckBox& cb,
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen,
                               Gfx::SizeF& boxSize) const = 0;

        virtual void onRenderBox(const CheckBox& cb,
                                 const StyleOptions& options,
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 const Gfx::RectF& boxRect,
                                 const Gfx::Brush& brush,
                                 const Gfx::Pen& pen) const = 0;

        virtual void onRenderText(const CheckBox& cb,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::FontMetrics& textMetric,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& mnemonic) const = 0;
};


class PT_HMI_API PanelRenderer : public Style::Facet
{
    public:
        PanelRenderer(std::size_t refs = 0);

        virtual ~PanelRenderer();

        void renderBackground(const Panel& p,
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Brush& brush) const;

        void renderFrame(const Panel& p,
                         const StyleOptions& options,
                         Painter& painter, 
                         const Gfx::RectF& rect, 
                         const Gfx::Pen& pen) const;

    protected:
        virtual void onRenderBackground(const Panel& p,
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const = 0;

        virtual void onRenderFrame(const Panel& p,
                                   const StyleOptions& options,
                                   Painter& painter, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Pen& pen) const = 0;
};


class PT_HMI_API LabelRenderer : public Style::Facet
{
    public:
        LabelRenderer(std::size_t refs = 0);

        virtual ~LabelRenderer();

        void prepare(const Label& l,
                     const StyleOptions& options,
                     Gfx::Font& font,
                     Gfx::Pen& textPen) const;
        
        void renderBackground(const Label& l,
                              const StyleOptions& options,
                              Painter& p, 
                              const Gfx::RectF& rect,
                              const Gfx::Brush& brush) const;

        void renderFrame(const Label& l,
                         const StyleOptions& options,
                         Painter& p, 
                         const Gfx::RectF& rect, 
                         const Gfx::Pen& contour) const;

        void renderText(const Label& l,
                        const StyleOptions& options,
                        Painter& p, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::Font& font, 
                        const Gfx::Pen& textPen) const;

    protected:
        virtual void onPrepare(const Label& l,
                               const StyleOptions& options,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const = 0;

        virtual void onRenderBackground(const Label& l,
                                        const StyleOptions& options,
                                        Painter& p, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const = 0;

        virtual void onRenderFrame(const Label& l,
                                   const StyleOptions& options,
                                   Painter& p, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Pen& contour) const = 0;

        virtual void onRenderText(const Label& l,
                                  const StyleOptions& options,
                                  Painter& p, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen) const = 0;
};

class PT_HMI_API LineEditRenderer : public Style::Facet
{
    public:
        LineEditRenderer(std::size_t refs = 0);

        virtual ~LineEditRenderer();

        void prepare(const LineEdit& le, 
                     const StyleOptions& options,
                     Gfx::Brush& brush,
                     Gfx::Pen& contour,
                     Gfx::Font& font,
                     Gfx::Pen& textPen,
                     Gfx::Pen& placeholderPen) const;
        
        void renderItem(const LineEdit& le, 
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const Gfx::Pen& contour,
                        const Gfx::Brush& brush) const;

        void renderText(const LineEdit& le, 
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::Font& font,
                        const Gfx::Pen& textPen) const;

        void renderCursor(const LineEdit& le, 
                          const StyleOptions& options,
                          Painter& painter, 
                          const Gfx::RectF& rect,
                          const Gfx::RectF& cursorRect ) const;
    
    protected:
        virtual void onPrepare(const LineEdit& le, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen,
                               Gfx::Pen& placeholderPen) const = 0;

        virtual void onRenderItem(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const Gfx::Pen& contour,
                                  const Gfx::Brush& brush) const = 0;

        virtual void onRenderText(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font,
                                  const Gfx::Pen& textPen) const = 0;

        virtual void onRenderCursor(const LineEdit& le, 
                                    const StyleOptions& options,
                                    Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& cursorRect ) const = 0;
};

class PT_HMI_API MenuRenderer : public Style::Facet
{
    public:
        MenuRenderer(std::size_t refs = 0);

        virtual ~MenuRenderer();

        void render(const Menu& m,
                    PaintSurface& surface,
                    const Gfx::RectF& rect) const;

        void renderItem(const MenuItem& m, 
                         PaintSurface& surface, 
                         const Gfx::RectF& rect) const;
        
        void renderIndicator(const MenuItem& m, 
                             PaintSurface& surface, 
                             const Gfx::RectF& rect) const;
    
    protected:
        virtual void onRender(const Menu& m, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;

        virtual void onRenderItem(const MenuItem& m, 
                                  PaintSurface& surface, 
                                  const Gfx::RectF& rect) const = 0;
        
        virtual void onRenderIndicator(const MenuItem& m, 
                                       PaintSurface& surface, 
                                       const Gfx::RectF& rect) const = 0;
};

class PT_HMI_API MenuBarRenderer : public Style::Facet
{
    public:
        MenuBarRenderer(std::size_t refs = 0);

        virtual ~MenuBarRenderer();

        void render(const MenuBar& m,
                    PaintSurface& surface,
                    const Gfx::RectF& rect) const;

        void renderItem(const MenuBarItem& m, 
                         PaintSurface& surface, 
                         const Gfx::RectF& rect) const;
    
    protected:
        virtual void onRender(const MenuBar& m, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;

        virtual void onRenderItem(const MenuBarItem& m, 
                                  PaintSurface& surface, 
                                  const Gfx::RectF& rect) const = 0;
};


class PT_HMI_API ScrollBarRenderer : public Style::Facet
{
    public:
        ScrollBarRenderer(std::size_t refs = 0);

        virtual ~ScrollBarRenderer();

        void render(const ScrollBar& s,
                    const Gfx::RectF& handleRect,
                    PaintSurface& surface, 
                    const Gfx::RectF& rect) const;
    
    protected:
        virtual void onRender(const ScrollBar& s,
                              const Gfx::RectF& handleRect,
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;
};

} // namespace

} // namespace

#endif
