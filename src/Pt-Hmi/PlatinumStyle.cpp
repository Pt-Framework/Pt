/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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

#include <Pt/Hmi/PlatinumStyle.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/LineEdit.h>
#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/CheckBox.h>
#include <Pt/Hmi/MenuBar.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/MenuItem.h>
#include <Pt/Hmi/ScrollBar.h>
#include <Pt/Hmi/ProgressBar.h>
#include <Pt/Hmi/Slider.h>
#include <Pt/Hmi/ListBox.h>
#include <Pt/Hmi/ComboBox.h>

namespace {

Pt::Gfx::Color brighten(const Pt::Gfx::Color& c, float factor)
{
    float r = c.red() * factor;
    float g = c.green() * factor;
    float b = c.blue() * factor;

    Pt::uint16_t r16 = r > 65535 ? 65535 : static_cast<Pt::uint16_t>(r);
    Pt::uint16_t g16 = g > 65535 ? 65535 : static_cast<Pt::uint16_t>(g);
    Pt::uint16_t b16 = b > 65535 ? 65535 : static_cast<Pt::uint16_t>(b);

    return Pt::Gfx::Color(c.alpha(), r16, g16, b16);
}

} // namespace

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// PlatinumRendererBase
///////////////////////////////////////////////////////////////////////////////

PlatinumRendererBase::PlatinumRendererBase()
{
}

    
PlatinumRendererBase::~PlatinumRendererBase()
{
}


void PlatinumRendererBase::renderFrame(Painter& painter,
                                       const Gfx::Rect& frameRect,
                                       const Gfx::Pen& pen,
                                       Pt::ssize_t corner) const
{
    Gfx::Rect borderRect = frameRect;

    borderRect.setOrigin( Gfx::Point(corner, corner) );
    borderRect.setSize( Gfx::Size(frameRect.size().width() - corner, 
                                   frameRect.size().height() - corner) );

    Gfx::Point outline[9] = {};

    // top left
    outline[0].setX(0);
    outline[0].setY(corner);

    outline[1].setX(corner);
    outline[1].setY(0);

    // top right
    outline[2].setX(borderRect.width() - corner);
    outline[2].setY(0);

    outline[3].setX(borderRect.width());
    outline[3].setY(corner);

    // bottom right
    outline[4].setX(borderRect.width());
    outline[4].setY(borderRect.height() - corner);

    outline[5].setX(borderRect.width() - corner);
    outline[5].setY(borderRect.height());

    // bottom left
    outline[6].setX(corner);
    outline[6].setY(borderRect.height());

    outline[7].setX(0);
    outline[7].setY(borderRect.height() - corner);
            
    outline[8] = outline[0];
    
    painter.setPen(pen);
    painter.drawPolyline(outline, 9);
}


void PlatinumRendererBase::renderPlane(Painter& painter,
                                       const Gfx::Rect& rect,
                                       const Gfx::Brush& brush,
                                       Pt::ssize_t corner) const
{
    Gfx::Rect borderRect( Gfx::Point(corner, corner),
                           Gfx::Size(rect.size().width() - corner, 
                                      rect.size().height() - corner) );

    Gfx::Point outline[9] = {};

    // top left
    outline[0].setX(0);
    outline[0].setY(corner);

    outline[1].setX(corner);
    outline[1].setY(0);

    // top right
    outline[2].setX(borderRect.width() - corner);
    outline[2].setY(0);

    outline[3].setX(borderRect.width());
    outline[3].setY(corner);

    // bottom right
    outline[4].setX(borderRect.width());
    outline[4].setY(borderRect.height() - corner);

    outline[5].setX(borderRect.width() - corner);
    outline[5].setY(borderRect.height());

    // bottom left
    outline[6].setX(corner);
    outline[6].setY(borderRect.height());

    outline[7].setX(0);
    outline[7].setY(borderRect.height() - corner);
            
    outline[8] = outline[0];

    painter.setBrush( brush );
    painter.fillPolygon(outline, 9);
}


///////////////////////////////////////////////////////////////////////////////
// PlatinumButtonRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumButtonRenderer::PlatinumButtonRenderer(std::size_t refs)
: ButtonRenderer(refs)
{
}

    
PlatinumButtonRenderer::~PlatinumButtonRenderer()
{
}


void PlatinumButtonRenderer::onPrepare(const PushButton& button,
                                       const StyleOptions& options,
                                       Gfx::Brush& foreground,
                                       Gfx::Pen& contour,
                                       Gfx::Font& font,
                                       Gfx::Pen& textPen) const 
{
    if( button.isEnabled() )
    {
        Gfx::Color buttonColor = foreground.color();

        if( button.isHighlighted() )
        {
            buttonColor = brighten(buttonColor, 0.9f);
        }

        if( button.isPressed() )
        {
            buttonColor = brighten(buttonColor, 0.9f);
        }

        foreground = buttonColor;
    }
}


void PlatinumButtonRenderer::onPrepareIcon(const PushButton& button,
                                           const StyleOptions& options,
                                           const Gfx::Image& icon,
                                           Picture& picture) const
{
    picture.set(icon);
}


void PlatinumButtonRenderer::onRenderBackground(const PushButton& button,
                                                const StyleOptions& options,
                                                Painter& painter, 
                                                const Gfx::Rect& rect,
                                                const Gfx::Brush& brush,
                                                const Gfx::Pen& pen) const 
{
    Gfx::Rect borderRect( button.size() );
    _baseRenderer.renderPlane(painter, borderRect, brush);

    if( button.hasFocus() )
    {
        Gfx::Size focusSize = button.size();
        focusSize.addHeight(-4);
        focusSize.addWidth(-4);

        Gfx::Pen focusPen(pen.color(), 1, Gfx::Pen::Dash);
        painter.setPen(focusPen);
        
        Gfx::Rect rect(Gfx::Point(2,2), focusSize);
        painter.drawRect(rect);
    }

    _baseRenderer.renderFrame(painter, borderRect, pen);
}


void PlatinumButtonRenderer::onRenderText(const PushButton& button,
                                          const StyleOptions& options,
                                          Painter& painter, 
                                          const Gfx::Rect& rect,
                                          const String& text,
                                          const Gfx::Point& textPos,
                                          const Gfx::Font& font, 
                                          const Gfx::Pen& textPen,
                                          const Gfx::Rect& mnemonic) const 
{
    painter.setFont(font);
    painter.setPen(textPen);
    painter.drawText(textPos, text);

    if( ! mnemonic.isNull() )
    {
        Pt::ssize_t menmonicY = textPos.y() + 1;
        painter.drawLine( Gfx::Point(mnemonic.left(), menmonicY), 
                          Gfx::Point(mnemonic.right(), menmonicY) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumButtonRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumCheckBoxRenderer::PlatinumCheckBoxRenderer(std::size_t refs)
: CheckBoxRenderer(refs)
{
}

    
PlatinumCheckBoxRenderer::~PlatinumCheckBoxRenderer()
{
}


void PlatinumCheckBoxRenderer::onPrepare(const CheckBox& cb,
                                         const StyleOptions& options,
                                         Gfx::Brush& brush,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen,
                                         Gfx::Size& boxSize) const 
{
    boxSize.set( font.size(), font.size() );
}


void PlatinumCheckBoxRenderer::onRenderBox(const CheckBox& cb,
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::Rect& rect,
                                           const Gfx::Rect& boxRect,
                                           const Gfx::Brush& brush,
                                           const Gfx::Pen& pen) const
{
    Gfx::Color checkColor = options.textColor();

    painter.setBrush(brush);
    painter.fillRect(boxRect);

    painter.setPen(pen);
    painter.drawRect(boxRect);

    if( cb.isChecked() )
    {
        Gfx::Point tl = boxRect.topLeft() + Gfx::Point(2, 2);
        Gfx::Point br = boxRect.bottomRight() - Gfx::Point(2, 2);
        Gfx::Point tr = boxRect.topRight() + Gfx::Point(-2, 2);
        Gfx::Point bl = boxRect.bottomLeft() - Gfx::Point(-2, 2);

        Pt::Gfx::Pen pen(checkColor, 2, Gfx::Pen::Solid, Gfx::Pen::RoundCap);
        painter.setPen(pen);
        painter.drawLine(tl, br);
        painter.drawLine(tr, bl);
    }
}


void PlatinumCheckBoxRenderer::onRenderText(const CheckBox& cb,
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::Rect& rect,
                                            const String& text,
                                            const Gfx::Point& textPos,
                                            const Gfx::FontMetrics& textMetric,
                                            const Gfx::Font& font, 
                                            const Gfx::Pen& textPen,
                                            const Gfx::Rect& mnemonic) const 
{
    painter.setFont(font);
    painter.setPen(textPen);
    painter.drawText(textPos, text);

    if( ! mnemonic.isNull() )
    {
        Pt::ssize_t menmonicY = textPos.y() + 1;
        painter.drawLine( Gfx::Point(mnemonic.left(), menmonicY), 
                          Gfx::Point(mnemonic.right(), menmonicY) );
    }

    if( cb.hasFocus() )
    {       
        Gfx::Rect focusRect( Gfx::Point(textPos.x() - 2, 
                                          textPos.y() - textMetric.ascent()), 
                              Gfx::Size(textMetric.width() + 4, 
                                         textMetric.height() ) );
        
        Gfx::Pen pen(textPen.color(), 1, Gfx::Pen::Dash);
        painter.setPen(pen);
        painter.drawRect(focusRect);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumPanelRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumPanelRenderer::PlatinumPanelRenderer(std::size_t refs)
: PanelRenderer(refs)
{
}

    
PlatinumPanelRenderer::~PlatinumPanelRenderer()
{
}


void PlatinumPanelRenderer::onRenderBackground(const Panel& p,
                                               const StyleOptions& options,
                                               Painter& painter, 
                                               const Gfx::Rect& rect,
                                               const Gfx::Brush& brush) const 
{
    Gfx::Rect borderRect( Gfx::Point(0,0), p.size() );
    
    _baseRenderer.renderPlane(painter, borderRect, brush);
}


void PlatinumPanelRenderer::onRenderFrame(const Panel& p,
                                          const StyleOptions& options,
                                          Painter& painter, 
                                          const Gfx::Rect& rect,
                                          const Gfx::Pen& pen) const 
{
    Gfx::Rect borderRect( Gfx::Point(0,0), p.size() );
    
    _baseRenderer.renderFrame(painter, borderRect, pen);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumLabelRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumLabelRenderer::PlatinumLabelRenderer(std::size_t refs)
: LabelRenderer(refs)
{
}


PlatinumLabelRenderer::~PlatinumLabelRenderer()
{
}


void PlatinumLabelRenderer::onPrepare(const Label& l,
                                      const StyleOptions& options,
                                      Gfx::Font& font,
                                      Gfx::Pen& textPen) const 
{
 
}


void PlatinumLabelRenderer::onRenderBackground(const Label& l,
                                               const StyleOptions& options,
                                               Painter& painter, 
                                               const Gfx::Rect& rect,
                                               const Gfx::Brush& brush) const 
{
    Gfx::Rect borderRect( Gfx::Point(0,0), l.size() );
    
    _baseRenderer.renderPlane(painter, borderRect, brush);
}


void PlatinumLabelRenderer::onRenderFrame(const Label& l,
                                          const StyleOptions& options,
                                          Painter& painter, 
                                          const Gfx::Rect& rect,
                                          const Gfx::Pen& contour) const 
{
    Gfx::Rect borderRect( Gfx::Point(0,0), l.size() );
    
    _baseRenderer.renderFrame(painter, borderRect, contour);
}


void PlatinumLabelRenderer::onRenderText(const Label& l,
                                         const StyleOptions& options,
                                         Painter& painter, 
                                         const Gfx::Rect& rect,
                                         const String& text,
                                         const Gfx::Point& textPos,
                                         const Gfx::Font& font, 
                                         const Gfx::Pen& textPen) const 
{
    painter.setFont(font);
    painter.setPen(textPen);
    painter.drawText( textPos, text);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumLineEditRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumLineEditRenderer::PlatinumLineEditRenderer(std::size_t refs)
: LineEditRenderer(refs)
{
}

    
PlatinumLineEditRenderer::~PlatinumLineEditRenderer()
{
}


void PlatinumLineEditRenderer::onPrepare(const LineEdit& le, 
                                         const StyleOptions& options,
                                         Gfx::Brush& brush,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen) const
{
    if( ! le.hasFocus() && le.isEmpty() )
    {
        textPen = contour;
    }

    if( le.isEnabled() )
    {
        Gfx::Color color = contour.color();

        if( le.isHighlighted() || le.hasFocus() )
        {
            color = options.accentColor();
        }

        contour = Gfx::Pen( color, contour.size(), contour.style(), 
                            contour.capStyle(), contour.joinStyle() );
    }
}


void PlatinumLineEditRenderer::onRenderBackground(const LineEdit& le, 
                                                  const StyleOptions& options,
                                                  Painter& painter, 
                                                  const Gfx::Rect& rect,
                                                  const Gfx::Pen& contour,
                                                  const Gfx::Brush& brush) const
{
    Gfx::Rect borderRect( le.size() );

    painter.setBrush(brush);
    painter.fillRect(borderRect);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumLineEditRenderer::onRenderText(const LineEdit& le, 
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::Rect& rect,
                                            const String& text,
                                            const Gfx::Point& textPos,
                                            const Gfx::Font& font,
                                            const Gfx::Pen& textPen) const
{
    painter.setPen(textPen);
    painter.setFont(font);
    painter.drawText(textPos, text);
}


void PlatinumLineEditRenderer::onRenderCursor(const LineEdit& le, 
                                              const StyleOptions& options,
                                              Painter& painter, 
                                              const Gfx::Rect& rect,
                                              const Gfx::Rect& cursorRect ) const
{
    painter.setPen( options.textColor() );
    
    painter.drawLine( cursorRect.topLeft(),
                      cursorRect.bottomLeft() );
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumMenuRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumMenuRenderer::PlatinumMenuRenderer(std::size_t refs)
: MenuRenderer(refs)
{
}

    
PlatinumMenuRenderer::~PlatinumMenuRenderer()
{
}


void PlatinumMenuRenderer::onPrepare(const Menu& m, 
                                     const StyleOptions& options,
                                     Gfx::Brush& brush,
                                     Gfx::Pen& contour) const
{
}


void PlatinumMenuRenderer::onRenderBackground(const Menu& m, 
                                              const StyleOptions& options,
                                              Painter& painter, 
                                              const Gfx::Rect& rect,
                                              const Gfx::Brush& brush,
                                              const Gfx::Pen& contour) const
{
    const Gfx::Size& size = m.size();

    //
    // icon strip on the left side
    //
    
    // TODO: use separate render funtion for iconstrip

    Pt::ssize_t iconWidth = m.iconWidth();

    if(iconWidth > 0)
    {
        Gfx::Rect iconStrip( Gfx::Point(0, 0),
                              Gfx::Size(iconWidth, size.height()) );
                
         Gfx::Brush brush(brush.color(),
                          Gfx::Color(65000, 65000, 65000), 
                          Gfx::Brush::Vertical);

        painter.setBrush(brush);
        painter.fillRect(iconStrip);
    }

    //
    // menu border
    //
    Gfx::Rect borderRect(size);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumMenuRenderer::onPrepareItem(const MenuItem& m, 
                                         const StyleOptions& options,
                                         const Gfx::Image& icon,
                                         Picture& picture,
                                         Gfx::Brush& brush,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen) const
{
    picture.set(icon);

    if( m.isHighlighted() )
        brush =  options.highlightColor();
}


void PlatinumMenuRenderer::onRenderItem(const MenuItem& m, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::Rect& rect,
                                        Gfx::Brush& brush,
                                        Gfx::Pen& contour) const
{
    bool highlight = m.isHighlighted();
    if(highlight)
    {
        painter.setBrush(brush);
        painter.fillRect(rect);
    }
}


void PlatinumMenuRenderer::onRenderIndicator(const MenuItem& m, 
                                             const StyleOptions& options,
                                             Painter& painter, 
                                             const Gfx::Rect& rect) const
{
    static const Pt::ssize_t indicatorWidth = 5;

    Pt::ssize_t x = m.size().width() - indicatorWidth - m.padding().right();
    Pt::ssize_t y = m.size().height() / 2;

    Gfx::Point indicator[3] = { Gfx::Point(x - 3, y - 4),
                                 Gfx::Point(x + 1, y),
                                 Gfx::Point(x - 3, y + 4) };
  
    Gfx::Brush brush( m.textColor() );
    painter.setBrush(brush);
    painter.fillPolygon(indicator, 3);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumMenuBarRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumMenuBarRenderer::PlatinumMenuBarRenderer(std::size_t refs)
: MenuBarRenderer(refs)
{
}

    
PlatinumMenuBarRenderer::~PlatinumMenuBarRenderer()
{
}


void PlatinumMenuBarRenderer::onPrepare(const MenuBar& m, 
                                        const StyleOptions& options,
                                        Gfx::Brush& brush,
                                        Gfx::Pen& contour) const
{
}


void PlatinumMenuBarRenderer::onRenderBackground(const MenuBar& m, 
                                                 const StyleOptions& options,
                                                 Painter& painter, 
                                                 const Gfx::Rect& rect,
                                                 const Gfx::Brush& brush,
                                                 const Gfx::Pen& contour) const
{
    painter.setBrush(brush);
    painter.fillRect(rect);
}


void PlatinumMenuBarRenderer::onPrepareItem(const MenuBarItem& m, 
                                            const StyleOptions& options, 
                                            Gfx::Brush& brush,
                                            Gfx::Pen& contour,
                                            Gfx::Font& font,
                                            Gfx::Pen& textPen) const
{
    if( m.isHighlighted() )
        brush = options.highlightColor();
}


void PlatinumMenuBarRenderer::onRenderItem(const MenuBarItem& m, 
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::Rect& rect,
                                           const Gfx::Brush& brush,
                                           const Gfx::Pen& contour) const
{
    if( m.isHighlighted() )
    {
        painter.setBrush(brush);
        painter.fillRect( Gfx::Rect(Gfx::Point(0,0), m.size()) );
    }
}


void PlatinumMenuBarRenderer::onRenderItemText(const MenuBarItem& m,
                                               const StyleOptions& options,
                                               Painter& painter, 
                                               const Gfx::Rect& rect,
                                               const String& text,
                                               const Gfx::Point& textPos,
                                               const Gfx::Font& font, 
                                               const Gfx::Pen& textPen,
                                               const Gfx::Rect& mnemonic) const 
{
    painter.setFont(font);
    painter.setPen(textPen);
    painter.drawText(textPos, text);

    if( ! mnemonic.isNull() )
    {
        Pt::ssize_t menmonicY = textPos.y() + 1;
        painter.drawLine( Gfx::Point(mnemonic.left(), menmonicY), 
                          Gfx::Point(mnemonic.right(), menmonicY) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumScrollBarRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumScrollBarRenderer::PlatinumScrollBarRenderer(std::size_t refs)
: ScrollBarRenderer(refs)
{
}

    
PlatinumScrollBarRenderer::~PlatinumScrollBarRenderer()
{
}


void PlatinumScrollBarRenderer::onPrepare(const ScrollBar& s,
                                          const StyleOptions& options,
                                          Gfx::Brush& background,
                                          Gfx::Brush& foreground,
                                          Gfx::Pen& contour) const
{
}


void PlatinumScrollBarRenderer::onRender(const ScrollBar& s,
                                         const StyleOptions& options,
                                         Painter& painter,
                                         const Gfx::Rect& rect,
                                         const Gfx::Rect& handleRect,
                                         const Gfx::Brush& background,
                                         const Gfx::Brush& foreground,
                                         const Gfx::Pen& contour) const
{
    painter.setBrush(background);
    painter.fillRect(rect);

    painter.setPen(contour);
    painter.drawRect( Gfx::Rect( s.size() ) );

    painter.setBrush(foreground);
    painter.fillRect(handleRect);

    painter.setPen(contour);
    painter.drawRect(handleRect);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumProgressBarRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumProgressBarRenderer::PlatinumProgressBarRenderer(std::size_t refs)
: ProgressBarRenderer(refs)
{
}

    
PlatinumProgressBarRenderer::~PlatinumProgressBarRenderer()
{
}


void PlatinumProgressBarRenderer::onPrepare(const ProgressBar&    p,
                                            const StyleOptions&  options,
                                            Gfx::Brush&          background,
                                            Gfx::Brush&          foreground,
                                            Gfx::Pen&            contour,
                                            Gfx::Pen&            textPen,
                                            Gfx::Font&            font
                                            ) const
{
}

void PlatinumProgressBarRenderer::onRender( const ProgressBar& p,
                                            const StyleOptions& options,
                                            Painter& painter,
                                            const Gfx::Rect& rect,
                                            const Gfx::Brush& background,
                                            const Gfx::Brush& foreground,
                                            const Gfx::Pen& contour,
                                            const Gfx::Pen& textPen,
                                            const Gfx::Font& font
                                         ) const
{
    
    const Pt::ssize_t barHeight = 3;
    const Pt::ssize_t progressWidth = p.size().width() * p.progress();
    const Pt::ssize_t boxY = p.size().height()/2 - barHeight/2;

    Gfx::Rect boxRect( Gfx::Point(0.0, boxY),
                        Gfx::Size(p.size().width(), barHeight) );

    Gfx::Rect progressRect( Gfx::Point(0.0, boxY),
                             Gfx::Size(progressWidth,barHeight) );

    painter.setBrush(background);
    painter.fillRect(boxRect);

    painter.setBrush(foreground);
    painter.fillRect(progressRect);

    painter.setBrush(foreground);
    painter.fillCircle(Gfx::Point(progressWidth - barHeight/2, 
                                   boxY), barHeight);
}


///////////////////////////////////////////////////////////////////////////////
// PlatinumSliderRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumSliderRenderer::PlatinumSliderRenderer(std::size_t refs)
: SliderRenderer(refs)
{
}

    
PlatinumSliderRenderer::~PlatinumSliderRenderer()
{
}


void PlatinumSliderRenderer::onPrepare( const Slider& s,
                                        const StyleOptions& options,
                                        Gfx::Brush& background,
                                        Gfx::Brush& foreground,
                                        Gfx::Pen& contour,
                                        Gfx::Pen& textPen,
                                        Gfx::Font& font) const
{
    bool highlight = s.isHighlighted() && s.isEnabled();

    foreground = highlight ? options.accentColor() 
                           : options.contour().color();
}


void PlatinumSliderRenderer::onRender( const Slider& s,
                                       const StyleOptions& options,
                                       Painter& painter,
                                       const Gfx::Rect& rect,
                                       const Gfx::Brush& background,
                                       const Gfx::Brush& foreground,
                                       const Gfx::Pen& contour,
                                       const Gfx::Pen& textPen,
                                       const Gfx::Font& font) const
{
    Pt::ssize_t handleWidth = 7;
    Pt::ssize_t handleHeight = 17;
    Pt::ssize_t sliderHeight = 5;

    Pt::ssize_t sliderX = handleWidth / 2;
    Pt::ssize_t sliderY = s.size().height() / 2 - sliderHeight / 2;
    Pt::ssize_t sliderWidth = s.size().width() - handleWidth;

    Gfx::Rect boxRect( Gfx::Point(sliderX, sliderY),
                        Gfx::Size(sliderWidth, sliderHeight) );

    painter.setBrush(background);
    painter.fillRect(boxRect);

    int range = s.maximum() - s.minimum();
    int offset = s.position() - s.minimum();
    Pt::ssize_t handleOffset = sliderWidth * offset / range;

    Pt::ssize_t handleX = handleOffset;
    Pt::ssize_t handleY = s.size().height() / 2 - handleHeight / 2;

    Gfx::Rect handleRect( Gfx::Point(handleX, handleY),
                           Gfx::Size(handleWidth, handleHeight) );

    painter.setBrush(foreground);
    painter.fillRect(handleRect);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumListBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumListBoxRenderer::PlatinumListBoxRenderer(std::size_t refs)
: ListBoxRenderer(refs)
{
}

    
PlatinumListBoxRenderer::~PlatinumListBoxRenderer()
{
}


void PlatinumListBoxRenderer::onPrepareLayout(Spacing& frameSize)
{
    frameSize = Spacing(1);
}


void PlatinumListBoxRenderer::onRenderBackground(const ListBox& lb,
                                                 const StyleOptions& options,
                                                 Painter& painter, 
                                                 const Gfx::Rect& rect,
                                                 const Gfx::Brush& brush) const 
{   
    painter.setBrush(brush);
    painter.fillRect(rect);
}


void PlatinumListBoxRenderer::onRenderFrame(const ListBox& lb,
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::Rect& rect,
                                            const Gfx::Pen& pen) const 
{
    Gfx::Rect borderRect( lb.size() );
    
    painter.setPen(pen);
    painter.drawRect(borderRect);
}


void PlatinumListBoxRenderer::onPrepareItem(const ListBoxItem& item, 
                                            const StyleOptions& options,
                                            Gfx::Brush& brush,
                                            Gfx::Pen& contour,
                                            Gfx::Font& font,
                                            Gfx::Pen& textPen) const
{
    if( item.isHighlighted() )
    {
        brush =  options.highlightColor();
        textPen = options.highlightedTextColor();
    }
}


void PlatinumListBoxRenderer::onRenderItem(const ListBoxItem& item, 
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::Rect& rect,
                                           Gfx::Brush& brush,
                                           Gfx::Pen& contour) const
{
    if( item.isHighlighted() )
    {
        painter.setBrush(brush);
        painter.fillRect(rect);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumComboBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumComboBoxRenderer::PlatinumComboBoxRenderer(std::size_t refs)
: ComboBoxRenderer(refs)
{
}

    
PlatinumComboBoxRenderer::~PlatinumComboBoxRenderer()
{
}


void PlatinumComboBoxRenderer::onPrepare(const ComboBox& cb, 
                                         const StyleOptions& options,
                                         Gfx::Brush& background,
                                         Gfx::Brush& foreground,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen) const
{
    if( cb.isEnabled() )
    {
        Gfx::Color color = contour.color();

        if( cb.isHighlighted() || cb.hasFocus() )
        {
            color = options.accentColor();
        }

        contour = Gfx::Pen( color, contour.size(), contour.style(), 
                            contour.capStyle(), contour.joinStyle() );
    }

    foreground = contour.color();
}


void PlatinumComboBoxRenderer::onRenderBackground(const ComboBox& cb, 
                                                  const StyleOptions& options,
                                                  Painter& painter, 
                                                  const Gfx::Rect& rect,
                                                  const Gfx::Pen& contour,
                                                  const Gfx::Brush& background) const
{
    Gfx::Rect borderRect( cb.size() );

    painter.setBrush(background);
    painter.fillRect(borderRect);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumComboBoxRenderer::onPrepareLayout(const ComboBox& cb,
                                               Gfx::Size& buttonSize) const
{
    int indicatorWidth = static_cast<int>(cb.size().height()) / 3;
    if(indicatorWidth % 2 == 0)
        ++indicatorWidth;
    
    Pt::ssize_t width = (indicatorWidth * 2.5);
    Pt::ssize_t height = cb.size().height();
    buttonSize = Gfx::Size(width, height);
}


void PlatinumComboBoxRenderer::onRenderButton(const ComboBox& cb, 
                                              const StyleOptions& options,
                                              Painter& painter, 
                                              const Gfx::Rect& rect,
                                              const Gfx::Pen& contour,
                                              const Gfx::Brush& foreground) const
{
    int indicatorWidth = static_cast<int>(cb.size().height()) / 3;
    if(indicatorWidth % 2 == 0)
        ++indicatorWidth;
    
    int indicatorHeight = indicatorWidth / 2 + 1;

    Pt::ssize_t lineX = cb.size().width() - indicatorWidth * 2.5;

    painter.setPen(contour);
    painter.drawLine( Gfx::Point(lineX, 
                                  indicatorHeight - 1),
                      Gfx::Point(lineX, 
                                  cb.size().height() - indicatorHeight) );


    Pt::ssize_t x = cb.size().width() - indicatorWidth * 1.75;
    Pt::ssize_t y = (cb.size().height() - indicatorHeight) / 2 + 1;

    Gfx::Point indicator[3] = { Gfx::Point(x, y),
                                 Gfx::Point(x + indicatorWidth, y),
                                 Gfx::Point(x + indicatorHeight - 1, y + indicatorHeight) };

    painter.setBrush(foreground);
    painter.fillPolygon(indicator, 3);
}


void PlatinumComboBoxRenderer::onRenderText(const ComboBox& cb,
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::Rect& rect,
                                            const String& text,
                                            const Gfx::Point& textPos,
                                            const Gfx::Font& font, 
                                            const Gfx::Pen& textPen,
                                            const Gfx::Rect& cursor) const
{
    //
    // text
    //
    painter.setPen(textPen);
    painter.setFont(font);
    painter.drawText(textPos, text);

    //
    // cursor
    //
    if( cb.isEditable() && cb.hasFocus() )
    {
        painter.drawLine( cursor.topLeft(),
                          cursor.bottomLeft() );
    }
}


///////////////////////////////////////////////////////////////////////////////
// PlatinumStyle
///////////////////////////////////////////////////////////////////////////////

PlatinumStyle::PlatinumStyle()
{
    set(new PlatinumButtonRenderer);
    set(new PlatinumCheckBoxRenderer);
    set(new PlatinumPanelRenderer);
    set(new PlatinumLabelRenderer);
    set(new PlatinumLineEditRenderer);
    set(new PlatinumMenuRenderer);
    set(new PlatinumMenuBarRenderer);
    set(new PlatinumScrollBarRenderer);
    set(new PlatinumProgressBarRenderer);
    set(new PlatinumSliderRenderer);
    set(new PlatinumListBoxRenderer);
    set(new PlatinumComboBoxRenderer);
}


PlatinumStyle::~PlatinumStyle()
{
}

} // namespace

} // namespace
