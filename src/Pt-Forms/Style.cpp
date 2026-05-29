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

#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Forms/TextBlock.h>

#include <algorithm>

namespace Pt {

namespace Forms {

namespace {

std::size_t nextStyleGeneration()
{
    static std::size_t generation = 0;
    return ++generation;
}

}

///////////////////////////////////////////////////////////////////////////////
// Style
///////////////////////////////////////////////////////////////////////////////

Style::Style()
: _generation( nextStyleGeneration() )
{
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
    _generation = nextStyleGeneration();

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
    _generation = nextStyleGeneration();

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
    _generation = nextStyleGeneration();
}


void Style::reset(const StyleOptions& options)
{
    FacetMap::iterator it;
    for(it = _facets.begin(); it != _facets.end(); ++it)
    {
        it->second->reset(options);
    }
}


Style::Facet* Style::find(const std::type_info& ti) const
{
    FacetMap::const_iterator it = _facets.find(ti);
    if( it == _facets.end() )
        return 0;

    return it->second;
}

///////////////////////////////////////////////////////////////////////////////
// LineEditRenderer
///////////////////////////////////////////////////////////////////////////////

LineEditRenderer::LineEditRenderer(std::size_t refs)
: Style::Facet( typeid(LineEditRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}

    
LineEditRenderer::~LineEditRenderer()
{
}


LineEditRenderer* LineEditRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& LineEditRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().textBackground();
}


void LineEditRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& LineEditRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void LineEditRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& LineEditRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void LineEditRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& LineEditRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void LineEditRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush* LineEditRenderer::selectionBackground() const
{
    return _selectionBackground.get();
}


void LineEditRenderer::setSelectionBackground(const Gfx::Brush& b)
{
    _selectionBackground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* LineEditRenderer::selectionTextColor() const
{
    return _selectionTextColor.get();
}


void LineEditRenderer::setSelectionTextColor(const Gfx::Pen& p)
{
    _selectionTextColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF LineEditRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF LineEditRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect)
{
    prepare();
    return onLayoutFrame(surface, rect);
}


const Painter& LineEditRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void LineEditRenderer::renderChrome(PaintContext& context,
                              const Gfx::RectF& rect,
                              const Gfx::RectF& textRect,
                              const String& text,
                              const Gfx::PointF& textPos,
                              const Gfx::RectF& cursor,
                              const Gfx::RectF& selection,
                              LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, textRect, opts, text, textPos,
             cursor, selection, state);
}


void LineEditRenderer::renderEntry(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderEntry(context, rect, opts, state);
}


void LineEditRenderer::renderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const Gfx::RectF& selection,
                                       LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderSelection(context, textRect, opts, selection, state);
}


void LineEditRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, textRect, opts, text, textPos, state);
}


void LineEditRenderer::renderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const Gfx::RectF& cursor,
                                    LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderCursor(context, textRect, opts, cursor, state);
}


void LineEditRenderer::onRenderChrome(PaintContext& context,
                                const Gfx::RectF& rect,
                                const Gfx::RectF& textRect,
                                const StyleOptions& options,
                                const String& text,
                                const Gfx::PointF& textPos,
                                const Gfx::RectF& cursor,
                                const Gfx::RectF& selection,
                                LineEditStyleFlags state)
{
    onRenderEntry(context, rect, options, state);
    onRenderSelection(context, textRect, options, selection, state);
    onRenderText(context, textRect, options, text, textPos, state);
    onRenderCursor(context, textRect, options, cursor, state);
}


const StyleOptions& LineEditRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// ListBoxRenderer
///////////////////////////////////////////////////////////////////////////////

ListBoxRenderer::ListBoxRenderer(std::size_t refs)
: Style::Facet( typeid(ListBoxRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}

    
ListBoxRenderer::~ListBoxRenderer()
{
}


ListBoxRenderer* ListBoxRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* ListBoxRenderer::background() const
{
    return _background.get();
}


void ListBoxRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* ListBoxRenderer::contour() const
{
    return _contour.get();
}


void ListBoxRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF ListBoxRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF ListBoxRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect)
{
    prepare();
    return onLayoutFrame(surface, rect);
}


void ListBoxRenderer::renderBackground(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       ListBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderBackground(context, rect, opts, state);
}


void ListBoxRenderer::renderChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   ListBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, opts, state);
}


const StyleOptions& ListBoxRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// ListItemRenderer
///////////////////////////////////////////////////////////////////////////////

ListItemRenderer::ListItemRenderer(std::size_t refs)
: Style::Facet( typeid(ListItemRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


ListItemRenderer::~ListItemRenderer()
{
}


ListItemRenderer* ListItemRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* ListItemRenderer::background() const
{
    return _background.get();
}


void ListItemRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush* ListItemRenderer::foreground() const
{
    return _foreground.get();
}


void ListItemRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& ListItemRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void ListItemRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& ListItemRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void ListItemRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF ListItemRenderer::measureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize)
{
    prepare();
    return onMeasureContent(surface, iconSize, textSize);
}


Gfx::SizeF ListItemRenderer::measureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


const Painter& ListItemRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


Gfx::RectF ListItemRenderer::layoutFrame(PaintSurface& surface,
                                          const Gfx::RectF& rect)
{
    prepare();
    return onLayoutFrame(surface, rect);
}


void ListItemRenderer::layoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect)
{
    prepare();
    onLayoutContent(surface, contentRect, iconSize, textSize, iconRect, textRect);
}


void ListItemRenderer::renderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        ListItemStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderBackground(context, rect, opts, state);
}


void ListItemRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, textRect, opts, text, pos, state);
}


void ListItemRenderer::renderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderIcon(context, iconRect, opts, picture, pos, state);
}


const StyleOptions& ListItemRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// ComboBoxRenderer
///////////////////////////////////////////////////////////////////////////////

ComboBoxRenderer::ComboBoxRenderer(std::size_t refs)
: Style::Facet( typeid(ComboBoxRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


ComboBoxRenderer::~ComboBoxRenderer()
{
}


ComboBoxRenderer* ComboBoxRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& ComboBoxRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().textBackground();
}


void ComboBoxRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& ComboBoxRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void ComboBoxRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush& ComboBoxRenderer::foreground() const
{
    if( _foreground )
        return *_foreground;

    return Application::instance().styleOptions().foreground();
}


void ComboBoxRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& ComboBoxRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void ComboBoxRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& ComboBoxRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void ComboBoxRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const StyleOptions& ComboBoxRenderer::prepare()
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( _styleGeneration != options.generation() )
    {
        _styleGeneration = options.generation();
        onPrepare(options);
    }

    return options;
}


Gfx::SizeF ComboBoxRenderer::measureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF ComboBoxRenderer::measureButton(PaintSurface& surface)
{
    prepare();
    return onMeasureButton(surface);
}


void ComboBoxRenderer::layoutChrome(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Gfx::RectF& entryRect,
                                   Gfx::RectF& buttonRect,
                                   Gfx::RectF& textRect)
{
    prepare();
    onLayoutChrome(surface, rect, entryRect, buttonRect, textRect);
}


const Painter& ComboBoxRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void ComboBoxRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& buttonRect,
                                    ComboBoxStyleFlags state,
                                    ButtonStyleFlags buttonState)
{
    const StyleOptions& options = prepare();
    onRenderChrome(context, rect, entryRect, buttonRect, options, state, buttonState);
}


void ComboBoxRenderer::renderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    ComboBoxStyleFlags state,
                                    ButtonStyleFlags buttonState)
{
    const StyleOptions& options = prepare();
    onRenderButton(context, buttonRect, options, state, buttonState);
}


void ComboBoxRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  ComboBoxStyleFlags state)
{
    const StyleOptions& options = prepare();
    onRenderText(context, textRect, options, text, textPos, cursor, state);
}


void ComboBoxRenderer::onRenderChrome(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& entryRect,
                                      const Gfx::RectF& buttonRect,
                                      const StyleOptions& options,
                                      ComboBoxStyleFlags state,
                                      ButtonStyleFlags buttonState)
{
    onRenderEntry(context, entryRect, options, state);
    onRenderButton(context, buttonRect, options, state, buttonState);
}

///////////////////////////////////////////////////////////////////////////////
// TabViewRenderer
///////////////////////////////////////////////////////////////////////////////

TabViewRenderer::TabViewRenderer(std::size_t refs)
: Style::Facet( typeid(TabViewRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}

    
TabViewRenderer::~TabViewRenderer()
{
}


TabViewRenderer* TabViewRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* TabViewRenderer::background() const
{
    return _background.get();
}


void TabViewRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* TabViewRenderer::contour() const
{
    return _contour.get();
}


void TabViewRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& TabViewRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void TabViewRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& TabViewRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void TabViewRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


void TabViewRenderer::prepare()
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( _styleGeneration == options.generation() )
        return;

    _styleGeneration = options.generation();
    onPrepare(options);
}


Gfx::SizeF TabViewRenderer::measureTab(PaintSurface& surface,
                                        const Pt::String& text)
{
    prepare();
    return onMeasureTab(surface, text);
}


Gfx::RectF TabViewRenderer::layoutTab(PaintSurface& surface,
                                       const Gfx::RectF& tabRect)
{
    prepare();
    return onLayoutTab(surface, tabRect);
}


const Painter& TabViewRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void TabViewRenderer::renderBackground(PaintContext& context,
                                       const Gfx::RectF& contentRect,
                                       TabViewStyleFlags state)
{
    prepare();
    const StyleOptions& options = Application::instance().styleOptions();
    onRenderBackground(context, contentRect, options, state);
}


void TabViewRenderer::renderChrome(PaintContext& context,
                                   const Gfx::RectF& contentRect,
                                   const Gfx::RectF& activeTabRect,
                                   TabViewStyleFlags state)
{
    prepare();
    const StyleOptions& options = Application::instance().styleOptions();
    onRenderChrome(context, contentRect, activeTabRect, options, state);
}


void TabViewRenderer::renderTab(PaintContext& context,
                                const Gfx::RectF& tabRect,
                                const Pt::String& text,
                                const Gfx::PointF& textPos,
                                TabItemStyleFlags state)
{
    prepare();
    const StyleOptions& options = Application::instance().styleOptions();
    onRenderTab(context, tabRect, text, textPos, options, state);
}

} // namespace

} // namespace
