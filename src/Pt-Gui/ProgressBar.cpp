/*
 * Copyright (C) 2006-2007 Tobias Mueller
 * Copyright (C) 2006-2007 PTV AG
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Pt/Gui/ProgressBar.h"

#include "Pt/StringStream.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Point.h"
#include "Pt/Gfx/Size.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/ResizeEvent.h"


using namespace Pt::Gfx;


namespace Pt {

namespace Gui {


ProgressBar::ProgressBar(Widget& parent, const Gfx::Point& at, const Gfx::Size& size)
: Widget(parent, at, size)
, _minimum(0)
, _maximum(100)
, _value(0)
, _blockWidth(10)
{
    setBackgroundColor(ARgbColor(65535, 65535, 65535));
    setForegroundColor(ARgbColor(0,     0,     0    ));
}


ProgressBar::~ProgressBar()
{
}


void ProgressBar::update()
{
    Painter painter = this->painter();

    // Calculate pixel-precise bar length and number of rectangles that fit into this bar.
    size_t barLength = (size_t)(percentage() * (size().width() - 5));
    size_t numberOfRects = barLength / _blockWidth;

    // Draw border of widget.
    size_t width  = size().width();
    size_t height = size().height();

    painter.setPen(Pen(1, ARgbColor(43908, 43908, 43908), Pen::SolidStyle, Pen::FlatCap));
    painter.drawLine(Gfx::Point(0, 0), Gfx::Point(width - 1, 0));
    painter.drawLine(Gfx::Point(0, 0), Gfx::Point(0, height - 1));

    painter.setPen(Pen(1, ARgbColor(25700, 25700, 25700), Pen::SolidStyle, Pen::FlatCap));
    painter.drawLine(Gfx::Point(1, 1), Gfx::Point(width - 2, 1));
    painter.drawLine(Gfx::Point(1, 1), Gfx::Point(1, height - 2));

    painter.setPen(Pen(1, ARgbColor(65535, 65535, 65535), Pen::SolidStyle, Pen::FlatCap));
    painter.drawLine(Gfx::Point(0, height - 1), Gfx::Point(width, height - 1));
    painter.drawLine(Gfx::Point(width - 1, 0),  Gfx::Point(width - 1, height - 1));

    painter.setPen(Pen(1, ARgbColor(53000, 53000, 53000), Pen::SolidStyle, Pen::FlatCap));
    painter.drawLine(Gfx::Point(1, height - 2), Gfx::Point(width - 1, height - 2));
    painter.drawLine(Gfx::Point(width - 2, 1),  Gfx::Point(width - 2, height - 2));

    // Clear the area behind the progress blocks with background color.
    painter.setBrush(backgroundColor());
    painter.fillRect(Gfx::Rect(Gfx::Point(numberOfRects * _blockWidth + 2, 2),
                                Gfx::Size(size().width() - numberOfRects * _blockWidth - 4, size().height() - 4)));

    // Set pen and brush for the progress blocks.
    // (White clearing rectangle outlines around blue filled rectangles).
    painter.setPen(Pen(1, backgroundColor()));
    painter.setBrush(ARgbColor(12593, 27242, 50629));

    // Draw progress blocks.
    for (size_t i = 0; i < numberOfRects; i++)
    {
        painter.drawRect(Gfx::Rect(Gfx::Point(2 + _blockWidth * i, 2), Gfx::Size(_blockWidth + 1, size().height() - 4)));
        painter.fillRect(Gfx::Rect(Gfx::Point(3 + _blockWidth * i, 3), Gfx::Size(_blockWidth - 1, size().height() - 6)));
    }

    // Convert percentage to String and draw it at center of widget.
    StringStream percentageStringStream;
    percentageStringStream << (size_t)(percentage() * 100) << Pt::String(L"%");

    String percentageString = percentageStringStream.str();
    FontMetrics metrics = painter.fontMetrics(percentageString);

    size_t textX = (size().width()  - metrics.width())  / 2;
    size_t textY = (size().height() - metrics.height()) / 2 + metrics.ascent();

    painter.setFont(Font("Tahoma", 11, Font::BoldStyle)); // TODO Font name
    painter.setPen(foregroundColor());

    painter.drawText(Gfx::Point(textX, textY), percentageString);
}


void ProgressBar::setMinimum(ssize_t minimum)
{
    _minimum = minimum;
}

ssize_t ProgressBar::minimum() const
{
    return _minimum;
}

void ProgressBar::setMaximum(ssize_t maximum)
{
    _maximum = maximum;
}

ssize_t ProgressBar::maximum() const
{
    return _maximum;
}

void ProgressBar::setValue(ssize_t value)
{
    _value = value;
}

ssize_t ProgressBar::value() const
{
    return _value;
}

float ProgressBar::percentage() const
{
    if (_maximum - _minimum == 0)
    {
        return 0;
    }

    if (_value > _maximum)
    {
        return 1;
    }

    if (_value < _minimum)
    {
        return 0;
    }

    return (_value - _minimum) / float(_maximum - _minimum);
}

void ProgressBar::setBlockWidth(size_t width)
{
    if (width > 0)
    {
        _blockWidth = width;
    }
}

size_t ProgressBar::blockWidth() const
{
    return _blockWidth;
}

void ProgressBar::_resizeEvent(const ResizeEvent& event)
{
    this->update();
}


void ProgressBar::_paintEvent(const PaintEvent& event)
{
    this->update();
}


} // namespace Gui

} // namespace Pt
