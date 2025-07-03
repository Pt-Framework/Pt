//Copyright (C) 2010-2023 Atesion GmbH. All rights reserved.
#include <Pt/Forms/MenuSubItem.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/Application.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

MenuSubItem::MenuSubItem()
{
}

MenuSubItem::~MenuSubItem()
{
}


void MenuSubItem::onPaint(Pt::Gfx::PaintSurface& surface, const Pt::Gfx::RectF& rect)
{
    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();

    
    Pt::Gfx::Painter painter(surface);
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
    painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceOver);
    painter.drawLayer(iconPos, _picture);
    painter.setCompositionMode(Pt::Gfx::CompositionMode::SourceCopy);

    
    // item text    
    painter.setFont(_font);
    painter.setPen(_textPen);

    Pt::Gfx::TextMetrics fm = painter.textMetrics(_text);
    double textX = padding().left() + _iconWidth;
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Pt::Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);

    
    // shortcut text    
    const Pt::Forms::Key* sk = shortcut();
    if(sk)
    {
        Pt::String skText = shortcutText(*sk);
        Pt::Gfx::TextMetrics skm = painter.textMetrics(skText);

        double skX = size().width() - skm.width() - padding().right();
        double skY = (size().height() - skm.height()) / 2;
        skY += skm.ascent();
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
