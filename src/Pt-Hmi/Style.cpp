/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

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
   MA  02110-1301  USA
*/

#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Label.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// Style
///////////////////////////////////////////////////////////////////////////////

Style::Style()
{
    set(new StyleOptions());
}


Style::Style(const Style& style)
{
    assign(style);
}


Style::~Style()
{
    FacetMap::iterator it;
    for(it = _facets.begin(); it != _facets.end(); ++it)
    {
        Facet* facet = it->second;
        if( 0 == facet->unref() )
            delete facet;
    }
}


Style& Style::operator=(const Style& style)
{
    assign(style);
    return *this;
}


void Style::assign(const Style& style)
{
    if(this == &style)
        return;

    for(FacetMap::iterator it = _facets.begin(); it != _facets.end(); ++it)
    {
        Facet* facet = it->second;
        
        if( 0 == facet->unref() )
            delete facet;
    }

    _facets.clear();

    FacetMap::const_iterator cit;
    for(cit = style._facets.begin(); cit != style._facets.end(); ++cit)
    {
        set(cit->second);
    }
}


void Style::combine(const Style& style)
{
    if(this == &style)
        return;

    _facets.clear();

    FacetMap::const_iterator it;
    for(it = style._facets.begin(); it != style._facets.end(); ++it)
    {
        set(it->second);
    }
}


void Style::set(Facet* facet)
{
    if( ! facet )
        return;

    FacetMap::value_type val(facet->typeId(), facet);
    
    std::pair<FacetMap::iterator, bool> r = _facets.insert(val);
    if( ! r.second )
    {
        Facet* prev = r.first->second;
        if( 0 == prev->unref() )
            delete prev;

        r.first->second = facet;
    }

    facet->ref();
}


const Style::Facet* Style::find(const std::type_info& ti) const
{
    FacetMap::const_iterator it = _facets.find(ti);
    if( it == _facets.end() )
        return 0;

    return it->second;
}

///////////////////////////////////////////////////////////////////////////////
// ButtonRenderer
///////////////////////////////////////////////////////////////////////////////

ButtonRenderer::ButtonRenderer(std::size_t refs)
: Style::Facet( typeid(ButtonRenderer), refs )
{
}

    
ButtonRenderer::~ButtonRenderer()
{
}


void ButtonRenderer::prepare(const PushButton& button,
                             const StyleOptions& options,
                             Gfx::Brush& brush,
                             Gfx::Pen& contour,
                             Gfx::Font& font,
                             Gfx::Pen& textPen) const
{
    onPrepare(button, options, brush, contour, font, textPen); 
}


void ButtonRenderer::prepareIcon(const PushButton& button,
                                 const StyleOptions& options,
                                 const Gfx::Image& icon,
                                 Picture& picture) const
{
    onPrepareIcon(button, options, icon, picture); 
}


void ButtonRenderer::renderBackground(const PushButton& button,
                                      const StyleOptions& options,
                                      Painter& painter, 
                                      const Gfx::RectF& rect,
                                      const Gfx::Brush& brush,
                                      const Gfx::Pen& pen) const
{ 
    onRenderBackground(button, options, painter, rect, brush, pen); 
}  


void ButtonRenderer::renderText(const PushButton& button,
                                const StyleOptions& options,
                                Painter& painter, 
                                const Gfx::RectF& rect,
                                const String& text,
                                const Gfx::PointF& textPos,
                                const Gfx::Font& font, 
                                const Gfx::Pen& textPen,
                                const Gfx::RectF& mnemonic) const
{ 
    onRenderText(button, options, painter, rect, 
                 text, textPos, font, textPen, mnemonic); 
}  

///////////////////////////////////////////////////////////////////////////////
// CheckBoxRenderer
///////////////////////////////////////////////////////////////////////////////

CheckBoxRenderer::CheckBoxRenderer(std::size_t refs)
: Style::Facet( typeid(CheckBoxRenderer), refs )
{
}

    
CheckBoxRenderer::~CheckBoxRenderer()
{
}


void CheckBoxRenderer::prepare(const CheckBox& cb,
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen,
                               Gfx::SizeF& boxSize) const
{
    onPrepare(cb, options, brush, contour, font, textPen, boxSize); 
}


void CheckBoxRenderer::renderBox(const CheckBox& cb,
                                 const StyleOptions& options,
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 const Gfx::RectF& boxRect,
                                 const Gfx::Brush& brush,
                                 const Gfx::Pen& pen) const
{ 
    onRenderBox(cb, options, painter, rect, 
                boxRect, brush, pen); 
}  


void CheckBoxRenderer::renderText(const CheckBox& cb,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::FontMetrics& textMetric,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& mnemonic) const
{ 
    onRenderText(cb, options, painter, rect, 
                 text, textPos, textMetric, font, textPen, mnemonic); 
}  


///////////////////////////////////////////////////////////////////////////////
// PanelRenderer
///////////////////////////////////////////////////////////////////////////////

PanelRenderer::PanelRenderer(std::size_t refs)
: Style::Facet( typeid(PanelRenderer), refs )
{
}

    
PanelRenderer::~PanelRenderer()
{
}


void PanelRenderer::renderBackground(const Panel& p,
                                     const StyleOptions& options,
                                     Painter& painter, 
                                     const Gfx::RectF& rect,
                                     const Gfx::Brush& brush) const
{
    onRenderBackground(p, options, painter, rect, brush);
}


void PanelRenderer::renderFrame(const Panel& p,
                                const StyleOptions& options,
                                Painter& painter, 
                                const Gfx::RectF& rect, 
                                const Gfx::Pen& pen) const
{
    onRenderFrame(p, options, painter, rect, pen);
}

///////////////////////////////////////////////////////////////////////////////
// LabelRenderer
///////////////////////////////////////////////////////////////////////////////

LabelRenderer::LabelRenderer(std::size_t refs)
: Style::Facet( typeid(LabelRenderer), refs )
{
}

    
LabelRenderer::~LabelRenderer()
{
}


void LabelRenderer::prepare(const Label& l,
                            const StyleOptions& options,
                            Gfx::Font& font,
                            Gfx::Pen& textPen) const
{
    onPrepare(l, options, font, textPen);
}


void LabelRenderer::renderBackground(const Label& l,
                                     const StyleOptions& options,
                                     Painter& p, 
                                     const Gfx::RectF& rect,
                                     const Gfx::Brush& brush) const
{
    onRenderBackground(l, options, p, rect, brush);
}


void LabelRenderer::renderFrame(const Label& l,
                                const StyleOptions& options,
                                Painter& p, 
                                const Gfx::RectF& rect, 
                                const Gfx::Pen& contour) const
{
    onRenderFrame(l, options, p, rect, contour);
}


void LabelRenderer::renderText(const Label& l,
                               const StyleOptions& options,
                               Painter& p, 
                               const Gfx::RectF& rect,
                               const String& text,
                               const Gfx::PointF& textPos,
                               const Gfx::Font& font, 
                               const Gfx::Pen& textPen) const
{
    onRenderText(l, options, p, rect, text, textPos, font, textPen);
}

///////////////////////////////////////////////////////////////////////////////
// LineEditRenderer
///////////////////////////////////////////////////////////////////////////////

LineEditRenderer::LineEditRenderer(std::size_t refs)
: Style::Facet( typeid(LineEditRenderer), refs )
{
}

    
LineEditRenderer::~LineEditRenderer()
{
}


void LineEditRenderer::prepare(const LineEdit& le, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen,
                               Gfx::Pen& placeholderPen) const
{
    onPrepare(le, options, brush, contour, font, textPen, placeholderPen);
}


void LineEditRenderer::renderItem(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const Gfx::Pen& contour,
                                  const Gfx::Brush& brush) const
{
    onRenderItem(le, options, painter, rect, contour, brush);
}


void LineEditRenderer::renderText(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font,
                                  const Gfx::Pen& textPen) const
{
    onRenderText(le, options, painter,  rect, text, textPos, font, textPen);
}


void LineEditRenderer::renderCursor(const LineEdit& le, 
                                    const StyleOptions& options,
                                    Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& cursorRect) const
{
    onRenderCursor(le, options, painter, rect, cursorRect);
}

///////////////////////////////////////////////////////////////////////////////
// MenuRenderer
///////////////////////////////////////////////////////////////////////////////

MenuRenderer::MenuRenderer(std::size_t refs)
: Style::Facet( typeid(MenuRenderer), refs )
{
}

    
MenuRenderer::~MenuRenderer()
{
}


void MenuRenderer::prepare(const Menu& m, 
                           const StyleOptions& options,
                           Gfx::Brush& brush,
                           Gfx::Pen& contour) const
{ 
    onPrepare(m, options, brush, contour); 
}


void MenuRenderer::prepareItem(const MenuItem& m, 
                               const StyleOptions& options,
                               const Gfx::Image& icon,
                               Picture& picture,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const
{
    onPrepareItem(m, options, icon, picture, brush, contour, font, textPen);
}


void MenuRenderer::renderBackground(const Menu& m, 
                                    const StyleOptions& options,
                                    Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::Brush& brush,
                                    const Gfx::Pen& contour) const
{ 
    onRenderBackground(m, options, painter, rect, brush, contour); 
}


void MenuRenderer::renderItem(const MenuItem& m, 
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              Gfx::Brush& brush,
                              Gfx::Pen& contour) const
{ 
    onRenderItem(m, options, painter, rect, brush, contour); 
}


void MenuRenderer::renderIndicator(const MenuItem& m, 
                                   PaintSurface& surface, 
                                   const Gfx::RectF& rect) const
{ 
    onRenderIndicator(m, surface, rect); 
}

///////////////////////////////////////////////////////////////////////////////
// MenuBarRenderer
///////////////////////////////////////////////////////////////////////////////

MenuBarRenderer::MenuBarRenderer(std::size_t refs)
: Style::Facet( typeid(MenuBarRenderer), refs )
{
}

    
MenuBarRenderer::~MenuBarRenderer()
{
}


void MenuBarRenderer::render(const MenuBar& m, 
                             PaintSurface& surface, 
                             const Gfx::RectF& rect) const
{ 
    onRender(m, surface, rect); 
}


void MenuBarRenderer::renderItem(const MenuBarItem& m, 
                                 PaintSurface& surface, 
                                 const Gfx::RectF& rect) const
{ 
    onRenderItem(m, surface, rect); 
}

///////////////////////////////////////////////////////////////////////////////
// ScrollBarRenderer
///////////////////////////////////////////////////////////////////////////////

ScrollBarRenderer::ScrollBarRenderer(std::size_t refs)
: Style::Facet( typeid(ScrollBarRenderer), refs )
{
}

    
ScrollBarRenderer::~ScrollBarRenderer()
{
}


void ScrollBarRenderer::render(const ScrollBar& s,
                               const Gfx::RectF& handleRect,
                               PaintSurface& surface, 
                               const Gfx::RectF& rect) const
{ 
    onRender(s, handleRect, surface, rect); 
}

} // namespace

} // namespace
