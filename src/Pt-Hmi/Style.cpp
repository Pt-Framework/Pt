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


void ButtonRenderer::render(const Button& button, 
                            PaintSurface& surface, 
                            const Gfx::RectF& rect) const
{ 
    onRender(button, surface, rect); 
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


void CheckBoxRenderer::render(const CheckBox& cb, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const
{ 
    onRender(cb, surface, rect); 
}  

///////////////////////////////////////////////////////////////////////////////
// FrameRenderer
///////////////////////////////////////////////////////////////////////////////

FrameRenderer::FrameRenderer(std::size_t refs)
: Style::Facet( typeid(FrameRenderer), refs )
{
}

    
FrameRenderer::~FrameRenderer()
{
}


void FrameRenderer::render(const Frame& f, 
                           PaintSurface& surface, 
                           const Gfx::RectF& rect) const
{ 
    onRender(f, surface, rect); 
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


void PanelRenderer::render(const Panel& p, 
                           PaintSurface& surface, 
                           const Gfx::RectF& rect) const
{ 
    onRender(p, surface, rect); 
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


void LabelRenderer::render(const Label& label, 
                           PaintSurface& surface, 
                           const Gfx::RectF& rect) const
{ 
    const StyleOptions* options = label.getFacet<StyleOptions>();
    if( ! options )
      return;

    Gfx::Brush brush = options->background();
    const Gfx::Color*color = label.planeColor();
    if(color)
        brush = *color;

    const Gfx::Color* borderColor = label.borderColor();
    if( ! borderColor )
        borderColor = &options->foreground();
    
    const Gfx::Font* font = label.font();
    if( ! font )
        font = &options->font();

    const Gfx::Color* textColor = label.textColor();
    if( ! textColor )
        textColor = &options->textColor(); 
    
    Gfx::PointF pos = label.textPosition(*font);

    //onRender(l, surface, rect);

    Painter painter(surface);
    painter.setClip(rect);

    onRenderBackground(painter, 
                       rect,
                       label,
                       *options, 
                       brush, 
                       *borderColor);

    onRenderText(painter, 
                 rect,
                 label,
                 *options,
                 pos,
                 *font, 
                 *textColor);
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


void MenuRenderer::render(const Menu& m, 
                           PaintSurface& surface, 
                           const Gfx::RectF& rect) const
{ 
    onRender(m, surface, rect); 
}


void MenuRenderer::renderItem(const MenuItem& m, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const
{ 
    onRenderItem(m, surface, rect); 
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
