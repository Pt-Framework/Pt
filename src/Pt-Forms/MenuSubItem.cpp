//Copyright (C) 2010-2023 Atesion GmbH. All rights reserved.
#include <Pt/Forms/MenuSubItem.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

MenuSubItem::MenuSubItem()
{
}

MenuSubItem::~MenuSubItem()
{
}


void MenuSubItem::onPaint(PaintContext& context, const Pt::Gfx::RectF& rect)
{
    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();

    
    Forms::Painter painter(context);
    painter.setClip(rect);
    
    
    // background
    bool highlight = this->isHighlighted();
    if (highlight)
    {
        painter.setBrush(_brush);
        painter.fillRect(rect);
    }
    
    
    // icon    
    double iconX = (iconPadding() - icon().width()) / 2;
    double iconY = (size().height() - icon().height()) / 2;

    Pt::Gfx::PointF iconPos(iconX, iconY);

    const Gfx::CompositionMode prevMode = painter.compositionMode();
    painter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    painter.drawPixmap(iconPos, _picture);
    painter.setCompositionMode(prevMode);


    // item text    
    painter.setFont(_font);
    painter.setPen(_textPen);

    Pt::Gfx::TextMetrics fm = painter.textMetrics(_text);
    Pt::Gfx::FontMetrics fontMet = painter.fontMetrics();
    double textX = padding().left() + _iconWidth;
    double textY = (size().height() - fontMet.height()) / 2;
    textY += fontMet.ascent();
    Pt::Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);

    
    // shortcut text    
    const Pt::Forms::Key* sk = shortcut();
    if(sk)
    {
        Pt::String skText = shortcutText(*sk);
        Pt::Gfx::TextMetrics skm = painter.textMetrics(skText);

        double skX = size().width() - skm.advance() - padding().right();
        double skY = (size().height() - fontMet.height()) / 2;
        skY += fontMet.ascent();
        Pt::Gfx::PointF skPos(skX, skY);

        painter.drawText(skPos, skText);
    }
    
    
    // menu indicator    
    static const double indicatorWidth = 5.0;

    double x = this->size().width() - indicatorWidth - this->padding().right();
    double y = this->size().height() / 2;

    Pt::Gfx::PointF indicator[3] = { Pt::Gfx::PointF(x - 3, y - 4),
                                    Pt::Gfx::PointF(x + 1, y),
                                    Pt::Gfx::PointF(x - 3, y + 4) };

    Pt::Gfx::Brush brush(this->textColor());
    painter.setBrush(brush);
    painter.fillPolygon(indicator, 3);


    // separator
    if (_hasSeparator)
    {       
        Pt::Gfx::PointF from(textX, size().height());
        Pt::Gfx::PointF to(size().width(), size().height());

        painter.setPen(this->contour());
        painter.drawLine(from, to);
    }
}

}}
