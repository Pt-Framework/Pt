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
: _menu(0)
, _isOpen(false)
, _parentMenu(0)
{
}

MenuSubItem::~MenuSubItem()
{
}

void MenuSubItem::closeMenu()
{
    _isOpen = false;
    if (_parentMenu)
        _parentMenu->closeMenu(*this);
}

void MenuSubItem::openMenu()
{
    _isOpen = true;

    if (_parentMenu)
        _parentMenu->openMenu(*this);
}


void MenuSubItem::setMenu(Menu* menu)
{
    if (_menu)
    {
        _menu->setParentItem(0);

        if(_parentMenu)
            _parentMenu->addMenu(*this);
    }

    _menu = menu;

    if( _menu)
    {
        _menu->setParentItem(this);   
        
        if (_parentMenu)
            _parentMenu->removeMenu(*this);
    }    
}

const std::vector<Key> MenuSubItem::onGetShortcuts()
{
    std::vector<Key> sck = MenuItemBase::onGetShortcuts();
    
    if(_menu == 0)
        return sck;

    std::map<Key, Control*>::const_iterator  it = _menu->shortcuts().begin();

    for( ;it != _menu->shortcuts().end(); ++it)
        sck.push_back(it->first);

    return sck;
}

const std::vector<Pt::Char> MenuSubItem::onGetMnemonics()
{
    std::vector<Pt::Char> mns = MenuItemBase::onGetMnemonics();

    if (_menu == 0)
        return mns;

    std::map<Pt::Char, Control*>::const_iterator  it = _menu->mnemonics().begin();

    for (; it != _menu->mnemonics().end(); ++it)
        mns.push_back(it->first);

    return mns;
}

void MenuSubItem::onMnemonic(Pt::Char m)
{
    const Char* myMn = mnemonic();

    if (myMn)
    {
        if (m == *myMn)
        {
            MenuItemBase::onMnemonic(m);
            return;
        }
    }

    std::map<Pt::Char, Control*>::const_iterator  it = _menu->mnemonics().begin();

    for (; it != _menu->mnemonics().end(); ++it)
    {
        if (it->first == m)
        {
            it->second->processMnemonic(m);
            break;
        }
    }
}

void MenuSubItem::onShortcut(const Key& key)
{
    const Key* myKey = shortcut();

    if(myKey)
    {
        if( key == *myKey)
        {
            MenuItemBase::onShortcut(key);
            return;
        }
    }

    std::map<Key, Control*>::const_iterator  it = _menu->shortcuts().begin();

    for (; it != _menu->shortcuts().end(); ++it)
    {
        if(it->first == key)
        {
            it->second->processShortcut(key);
            break;
        }
    }
}


void MenuSubItem::cancel()
{
    if(_menu)
        _menu->cancel();
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
