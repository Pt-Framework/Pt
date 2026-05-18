/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
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
   MA  02110-1301  USA
*/

#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Forms/TextBlock.h>

#include <algorithm>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////////////
// Style
///////////////////////////////////////////////////////////////////////////////

Style::Style()
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


Style::Facet* Style::find(const std::type_info& ti) const
{
    FacetMap::const_iterator it = _facets.find(ti);
    if( it == _facets.end() )
        return 0;

    return it->second;
}

///////////////////////////////////////////////////////////////////////////////
// PanelRenderer
///////////////////////////////////////////////////////////////////////////////

PanelRenderer::PanelRenderer(std::size_t refs)
: Style::Facet( typeid(PanelRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


PanelRenderer::~PanelRenderer()
{
}


PanelRenderer* PanelRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* PanelRenderer::background() const
{
    return _background.get();
}


void PanelRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* PanelRenderer::contour() const
{
    return _contour.get();
}


void PanelRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& PanelRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void PanelRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& PanelRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void PanelRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const StyleOptions& PanelRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}


void PanelRenderer::renderBackground(PaintContext& context,
                                     const Gfx::RectF& rect,
                                     StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderBackground(context, rect, opts, state);
}


Gfx::SizeF PanelRenderer::measureFrame(PaintSurface& surface,
                                        const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF PanelRenderer::layoutFrame(PaintSurface& surface,
                                       const Gfx::RectF& frameRect)
{
    return onLayoutFrame(surface, frameRect);
}


void PanelRenderer::renderFrame(PaintContext& context,
                                const Gfx::RectF& rect,
                                StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderFrame(context, rect, opts, state);
}


const Painter& PanelRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void PanelRenderer::renderText(PaintContext& context,
                               const Gfx::RectF& rect,
                               const String& text,
                               const Gfx::PointF& pos,
                               StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, rect, opts, text, pos, state);
}


void PanelRenderer::renderIcon(PaintContext& context,
                               const Gfx::RectF& rect,
                               const Pixmap& picture,
                               const Gfx::PointF& pos,
                               StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderIcon(context, rect, opts, picture, pos, state);
}

///////////////////////////////////////////////////////////////////////////////
// ButtonRenderer
///////////////////////////////////////////////////////////////////////////////

ButtonRenderer::ButtonRenderer(std::size_t refs)
: Style::Facet( typeid(ButtonRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


ButtonRenderer::~ButtonRenderer()
{
}


ButtonRenderer* ButtonRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& ButtonRenderer::foreground() const
{
    if( _foreground )
        return *_foreground;

    return Application::instance().styleOptions().foreground();
}


void ButtonRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& ButtonRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void ButtonRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& ButtonRenderer::accentColor() const
{
    if( _accentColor )
        return *_accentColor;

    return Application::instance().styleOptions().accentColor();
}


void ButtonRenderer::setAccentColor(const Gfx::Color& c)
{
    _accentColor.reset( new Gfx::Color(c) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& ButtonRenderer::highlightColor() const
{
    if( _highlightColor )
        return *_highlightColor;

    return Application::instance().styleOptions().highlightColor();
}


void ButtonRenderer::setHighlightColor(const Gfx::Color& c)
{
    _highlightColor.reset( new Gfx::Color(c) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& ButtonRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void ButtonRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& ButtonRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void ButtonRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const StyleOptions& ButtonRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}


Gfx::SizeF ButtonRenderer::measureSurface(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize)
{
    return onMeasureSurface(surface, contentSize);
}


Gfx::RectF ButtonRenderer::layoutSurface(PaintSurface& surface,
                                          const Gfx::RectF& surfaceRect)
{
    return onLayoutSurface(surface, surfaceRect);
}


void ButtonRenderer::renderSurface(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   ButtonStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderSurface(context, rect, opts, state);
}


const Painter& ButtonRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void ButtonRenderer::renderText(PaintContext& context,
                                const Gfx::RectF& rect,
                                const String& text,
                                const Gfx::PointF& pos,
                                ButtonStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, rect, opts, text, pos, state);
}


Gfx::RectF ButtonRenderer::layoutMnemonic(PaintSurface& surface,
                                          const String& text,
                                          const Gfx::PointF& textPos,
                                          const Gfx::FontMetrics& fontMetrics,
                                          String::size_type mnemonicIndex)
{
    prepare();
    return onLayoutMnemonic(surface, text, textPos, fontMetrics, mnemonicIndex);
}


void ButtonRenderer::renderMnemonic(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& mnemonic,
                                    ButtonStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderMnemonic(context, rect, opts, mnemonic, state);
}


void ButtonRenderer::prepareIcon(const Gfx::Image& icon,
                                 Pixmap& picture,
                                 ButtonStyleFlags state) const
{
    const StyleOptions& opts = Application::instance().styleOptions();
    onPrepareIcon(opts, icon, picture, state);
}


void ButtonRenderer::renderIcon(PaintContext& context,
                                const Gfx::RectF& rect,
                                const Pixmap& picture,
                                const Gfx::PointF& pos,
                                ButtonStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderIcon(context, rect, opts, picture, pos, state);
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
                                  const Gfx::TextMetrics& textMetric,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& mnemonic) const
{ 
    onRenderText(cb, options, painter, rect, 
                 text, textPos, textMetric, font, textPen, mnemonic); 
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
                               Gfx::Pen& textPen) const
{
    onPrepare(le, options, brush, contour, font, textPen);
}


void LineEditRenderer::renderBackground(const LineEdit& le, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const
{
    onRenderBackground(le, options, painter, rect, contour, brush);
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
                               Pixmap& picture,
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
                                   const StyleOptions& options,
                                   Painter& painter, 
                                   const Gfx::RectF& rect) const
{ 
    onRenderIndicator(m, options, painter, rect); 
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


void MenuBarRenderer::prepare(const MenuBar& m, 
                              const StyleOptions& options,
                              Gfx::Brush& brush,
                              Gfx::Pen& contour) const
{
    onPrepare(m, options, brush, contour);
}


void MenuBarRenderer::renderBackground(const MenuBar& m, 
                                       const StyleOptions& options,
                                       Painter& painter, 
                                       const Gfx::RectF& rect,
                                       const Gfx::Brush& brush,
                                       const Gfx::Pen& contour) const
{ 
    onRenderBackground(m, options, painter, rect, brush, contour); 
}


void MenuBarRenderer::prepareItem(const MenuBarItem& m, 
                                  const StyleOptions& options, 
                                  Gfx::Brush& brush,
                                  Gfx::Pen& contour,
                                  Gfx::Font& font,
                                  Gfx::Pen& textPen) const
{ 
    onPrepareItem(m, options, brush, contour, font, textPen); 
}


void MenuBarRenderer::renderItem(const MenuBarItem& m, 
                                 const StyleOptions& options,
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 const Gfx::Brush& brush,
                                 const Gfx::Pen& contour) const
{ 
    onRenderItem(m, options, painter, rect, brush, contour); 
}


void MenuBarRenderer::renderItemText(const MenuBarItem& m, 
                                 const StyleOptions& options,
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::Font& font, 
                                 const Gfx::Pen& textPen,
                                 const Gfx::RectF& mnemonic) const
{ 
    onRenderItemText(m, options, painter, rect, 
                     text, textPos, font, textPen, mnemonic); 
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


void ScrollBarRenderer::prepare(const ScrollBar& s,
                                const StyleOptions& options,
                                Gfx::Brush& background,
                                Gfx::Brush& foreground,
                                Gfx::Pen& contour) const
{ 
    onPrepare(s, options, background, foreground, contour); 
}


void ScrollBarRenderer::render(const ScrollBar& s,
                               const StyleOptions& options,
                               Painter& painter,
                               const Gfx::RectF& rect,
                               const Gfx::RectF& handleRect,
                               const Gfx::Brush& background,
                               const Gfx::Brush& foreground,
                               const Gfx::Pen& contour) const
{ 
    onRender(s, options, painter,  rect, handleRect,
             background, foreground, contour); 
}


///////////////////////////////////////////////////////////////////////////////
// ProgressBarRenderer
///////////////////////////////////////////////////////////////////////////////

ProgressBarRenderer::ProgressBarRenderer(std::size_t refs)
: Style::Facet( typeid(ProgressBarRenderer), refs )
{
}

    
ProgressBarRenderer::~ProgressBarRenderer()
{
}


void ProgressBarRenderer::prepare(const ProgressBar&  p,
                                  const StyleOptions&  options,
                                  Gfx::Brush&          background,
                                  Gfx::Brush&          foreground,
                                  Gfx::Pen&            contour,
                                  Gfx::Pen&            textPen,
                                  Gfx::Font&          font) const
{ 
    onPrepare(p, options, background, foreground, contour, textPen, font); 
}

void ProgressBarRenderer::render( const ProgressBar& p,
                                           const StyleOptions& options,
                                          Painter& painter,
                                          const Gfx::RectF& rect,
                                          const Gfx::Brush& background,
                                          const Gfx::Brush& foreground,
                                          const Gfx::Pen& contour,
                                          const Gfx::Pen&            textPen,
                                      const Gfx::Font&            font
                                        ) const
{
  onRender(p, options, painter, rect, background, foreground, contour, textPen, font); 
}


///////////////////////////////////////////////////////////////////////////////
// SliderRenderer
///////////////////////////////////////////////////////////////////////////////

SliderRenderer::SliderRenderer(std::size_t refs)
: Style::Facet( typeid(SliderRenderer), refs )
{
}

    
SliderRenderer::~SliderRenderer()
{
}


void SliderRenderer::prepare( const Slider&        s,
                              const StyleOptions&  options,
                              Gfx::Brush&          background,
                              Gfx::Brush&          foreground,
                              Gfx::Pen&            contour,
                              Gfx::Pen&            textPen,
                              Gfx::Font&          font) const
{ 
    onPrepare(s, options, background, foreground, contour, textPen, font); 
}

void SliderRenderer::render( const Slider&        s,
                             const StyleOptions&  options,
                             Painter&              painter,
                             const Gfx::RectF&    rect,
                             const Gfx::Brush&    background,
                             const Gfx::Brush&    foreground,
                             const Gfx::Pen&      contour,
                             const Gfx::Pen&      textPen,
                             const Gfx::Font&      font
                            ) const
{
  onRender(s, options, painter, rect, background, foreground, contour, textPen, font); 
}

///////////////////////////////////////////////////////////////////////////////
// ListBoxRenderer
///////////////////////////////////////////////////////////////////////////////

ListBoxRenderer::ListBoxRenderer(std::size_t refs)
: Style::Facet( typeid(ListBoxRenderer), refs )
{
}

    
ListBoxRenderer::~ListBoxRenderer()
{
}


void ListBoxRenderer::prepareLayout(Spacing& frameSize)
{
    onPrepareLayout(frameSize);
}


void ListBoxRenderer::renderBackground(const ListBox& lb,
                                       const StyleOptions& options,
                                       Painter& painter, 
                                       const Gfx::RectF& rect,
                                       const Gfx::Brush& brush) const
{
    onRenderBackground(lb, options, painter, rect, brush);
}


void ListBoxRenderer::renderFrame(const ListBox& lb,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect, 
                                  const Gfx::Pen& pen) const
{
    onRenderFrame(lb, options, painter, rect, pen);
}


void ListBoxRenderer::prepareItem(const ListBoxItem& item, 
                                  const StyleOptions& options, 
                                  Gfx::Brush& brush,
                                  Gfx::Pen& contour,
                                  Gfx::Font& font,
                                  Gfx::Pen& textPen) const
{ 
    onPrepareItem(item, options, brush, contour, font, textPen); 
}


void ListBoxRenderer::renderItem(const ListBoxItem& item, 
                                 const StyleOptions& options,
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 Gfx::Brush& brush,
                                 Gfx::Pen& contour) const
{ 
    onRenderItem(item, options, painter, rect, brush, contour); 
}

///////////////////////////////////////////////////////////////////////////////
// ComboBoxRenderer
///////////////////////////////////////////////////////////////////////////////

ComboBoxRenderer::ComboBoxRenderer(std::size_t refs)
: Style::Facet( typeid(ComboBoxRenderer), refs )
{
}


ComboBoxRenderer::~ComboBoxRenderer()
{
}


void ComboBoxRenderer::prepare(const ComboBox& cb, 
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Brush& foreground,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const
{
    onPrepare(cb, options, background, foreground, contour, font, textPen);
}


void ComboBoxRenderer::prepareLayout(const ComboBox& cb,
                                     Gfx::SizeF& buttonSize) const
{
    return onPrepareLayout(cb, buttonSize);
}


void ComboBoxRenderer::renderBackground(const ComboBox& cb, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const
{
    onRenderBackground(cb, options, painter, rect, contour, brush);
}


void ComboBoxRenderer::renderButton(const ComboBox& cb, 
                                    const StyleOptions& options,
                                    Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::Pen& contour,
                                    const Gfx::Brush& foreground) const
{
    onRenderButton(cb, options, painter, rect, contour, foreground);
}


void ComboBoxRenderer::renderText(const ComboBox& cb,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& cursor) const
{
    onRenderText(cb, options, painter, rect, text, textPos, font, textPen, cursor);
}

///////////////////////////////////////////////////////////////////////////////
// SpinBoxRenderer
///////////////////////////////////////////////////////////////////////////////

SpinBoxRenderer::SpinBoxRenderer(std::size_t refs)
: Style::Facet( typeid(SpinBoxRenderer), refs )
{
}


SpinBoxRenderer::~SpinBoxRenderer()
{
}


void SpinBoxRenderer::prepare(const SpinBox& sb, 
                              const StyleOptions& options,
                              Gfx::Brush& background,
                              Gfx::Pen& contour,
                              Gfx::Font& font,
                              Gfx::Pen& textPen) const
{
    onPrepare(sb, options, background, contour, font, textPen);
}


void SpinBoxRenderer::prepareButton(const SpinBoxButton& sb, 
                                    const StyleOptions& options,
                                    Gfx::Brush& foreground,
                                    Gfx::Pen& contour) const
{
    onPrepareButton(sb, options, foreground, contour);
}


void SpinBoxRenderer::layout(const SpinBox& sb,
                             Gfx::RectF& downButton,
                             Gfx::RectF& upButton,
                             Gfx::RectF& textBox) const
{
    return onLayout(sb, downButton, upButton, textBox);
}


void SpinBoxRenderer::renderBackground(const SpinBox& sb, 
                                       const StyleOptions& options,
                                       Painter& painter, 
                                       const Gfx::RectF& rect,
                                       const Gfx::Pen& contour,
                                       const Gfx::Brush& brush) const
{
    onRenderBackground(sb, options, painter, rect, contour, brush);
}


void SpinBoxRenderer::renderButton(const SpinBoxButton& sb, 
                                   const StyleOptions& options,
                                   Painter& painter, 
                                   const Gfx::RectF& rect,
                                   const Gfx::Brush& foreground,
                                   const Gfx::Pen& contour) const
{
    onRenderButton(sb, options, painter, rect, foreground, contour);
}


void SpinBoxRenderer::renderText(const SpinBox& sb,
                                 const StyleOptions& options,
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::Font& font, 
                                 const Gfx::Pen& textPen,
                                 const Gfx::RectF& cursor) const
{
    onRenderText(sb, options, painter, rect, text, textPos, font, textPen, cursor);
}

///////////////////////////////////////////////////////////////////////////////
// TabViewRenderer
///////////////////////////////////////////////////////////////////////////////

TabViewRenderer::TabViewRenderer(std::size_t refs)
: Style::Facet( typeid(TabViewRenderer), refs )
{
}

    
TabViewRenderer::~TabViewRenderer()
{
}


void TabViewRenderer::prepare(const TabView& tv,
                              const StyleOptions& options,
                              Gfx::Brush& background,
                              Gfx::Brush& foreground,
                              Gfx::Pen& contour) const
{
    onPrepare(tv, options, background, foreground, contour); 
}


void TabViewRenderer::render(const TabView& tv,
                             const StyleOptions& options,
                             Painter& painter,
                             const Gfx::RectF& rect,
                             const Gfx::Brush& background,
                             const Gfx::Brush& foreground,
                             const Gfx::Pen& contour) const
{ 
    onRender(tv, options, painter, rect, background, foreground, contour); 
}  


Gfx::SizeF TabViewRenderer::measureTabs(PaintSurface& surface,
                                        const std::vector<TabItem>& tabs,
                                        const Gfx::Font& font) const
{
    return onMeasureTabs(surface, tabs, font);
}


void TabViewRenderer::layoutTabs(PaintSurface& surface,
                                 std::vector<TabItem>& tabs,
                                 const Gfx::RectF& rect, 
                                 const Gfx::Font& font) const
{
    onLayoutTabs(surface, tabs, rect, font);
}


void TabViewRenderer::prepareTabs(const TabBar& tabs,
                                  const StyleOptions& options,
                                  const Gfx::Brush& background,
                                  const Gfx::Brush& foreground,
                                  const Gfx::Pen& contour,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen) const
{
    onPrepareTabs(tabs, options, 
                  background, foreground, contour, font, textPen);
}


void TabViewRenderer::renderTabs(const std::vector<TabItem>& tabs,
                                 const StyleOptions& options,
                                 Painter& painter,
                                 const Gfx::RectF& rect,
                                 const Gfx::Brush& background,
                                 const Gfx::Brush& foreground,
                                 const Gfx::Pen& contour,
                                 const Gfx::Font& font, 
                                 const Gfx::Pen& textPen) const
{
    onRenderTabs(tabs, options, painter, rect, 
                 background, foreground, contour,font, textPen);
}

} // namespace

} // namespace
