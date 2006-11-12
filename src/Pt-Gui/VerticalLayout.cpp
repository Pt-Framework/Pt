/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Pt/Gui/VerticalLayout.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Gfx/Point.h"
#include "Pt/Gfx/Size.h"

#include <list>

using namespace std;
using namespace Pt::Gfx;


namespace Pt {

namespace Gui {


VerticalLayout& VerticalLayout::create(Widget& widget, Mode mode, size_t gap)
{
	// The layout will be deleted by the owning widget
	VerticalLayout* layout = new VerticalLayout(widget, mode, gap);
	return *layout;
}


VerticalLayout::VerticalLayout(Widget& widget, Mode mode, size_t gap)
: Layout(widget)
, _mode(mode)
, _gap(gap)
{
}


void VerticalLayout::set(Widget& widget, Orientation orient, const Margin& margin)
{
	WidgetMap::iterator it = _widgets.find(&widget);

	if (it == _widgets.end()) {
		connect(widget.destroyed, *this, &VerticalLayout::remove);
	}

	VerticalLayout::LayoutData data(orient, margin);
	_widgets[&widget] = data;
}


void VerticalLayout::remove(Widget& widget)
{
	_widgets.erase(&widget);

	// TODO disconnect ???
}


ssize_t VerticalLayout::maximumHeight() const
{
	const std::list<Widget*>& children = this->widget().childWidgets();
	list<Widget*>::const_iterator childrenIter;
	ssize_t maxHeight = 0;

	// If the mode is "uniform height" then the heighest widget's height is used.
	if (_mode == UniformHeight)
	{
		for (childrenIter = children.begin(); childrenIter != children.end(); childrenIter++) 
		{
			Widget* w = *childrenIter;
			maxHeight = max(maxHeight, w->preferredSize().height());
		}
	}

	return maxHeight;
}


void VerticalLayout::update()
{
	ssize_t maxHeight = this->maximumHeight();
	size_t  widgetWidth  = this->widget().size().width();
	ssize_t y = 0;

	// Calculate the position for each widget according to the order 
	// they were added to the layout.
	for(WidgetMap::const_iterator it = _widgets.begin(); it != _widgets.end(); it++) 
	{
		Widget* w = it->first;
		const LayoutData& layoutData = it->second;

		// Determine x-position and width according to the orientation 
		// (left, right, center, grab).
		size_t x     = 0;
		size_t width = 0;
		switch( layoutData.orientation() )
		{
			case VerticalLayout::Grab:
				x     = 0;
				width = widgetWidth;
				break;

			case VerticalLayout::Left:
				x     = 0;
				width = w->preferredSize().width();
				break;

			case VerticalLayout::Right:
				x     = widgetWidth - w->preferredSize().width();
				width = w->preferredSize().width();
				break;

			case VerticalLayout::Center:
				x     = ((ssize_t)widgetWidth - w->preferredSize().width()) / 2;
				width = w->preferredSize().width();
				break;
		};

		// The height of the specific widget is either all the same (uniform) or specific (varying).
		size_t height = (_mode == UniformHeight) ? maxHeight : w->preferredSize().height();

		// Add margins to x-position and width.
		x      += layoutData.margin().left();
		width  -= (layoutData.margin().left() + layoutData.margin().right());

		// Set the widget's calculated position and size.
		w->move  (x,     y + layoutData.margin().top());
		w->resize(width, height - layoutData.margin().top() - layoutData.margin().bottom());

		// Go to next y-position.
		y += height + _gap;
	}
}

Size VerticalLayout::minimumSize()
{
	return calculateSize(this->widget(), false);
}


Size VerticalLayout::preferredSize()
{
	return calculateSize(this->widget(), true);
}


Size VerticalLayout::calculateSize(Widget& parent, bool forPreferredSize)
{
	const std::list<Widget*>& children = parent.childWidgets();
	ssize_t maxWidth  = 0;
	ssize_t maxHeight = 0;
	ssize_t allHeight = 0;

	if (_mode == VaryingHeight)
	{
		// Sum all heights and find widest width.
		list<Widget*>::const_iterator child;

		for(child = children.begin(); child != children.end(); child++) {
			Widget* w = *child;

			if (forPreferredSize)
			{
				maxWidth  =  max(maxWidth,  w->preferredSize().width());
				allHeight += w->preferredSize().height();
			}
			else {
				maxWidth  =  max(maxWidth,  w->minimumSize().width());
				allHeight += w->minimumSize().height();
			}
		}

		allHeight += (children.size() - 1) * _gap;
		return Size(maxWidth, allHeight);

	}
	else
	{
		// Findet heighest height and widest width.
		list<Widget*>::const_iterator child = children.begin();

		for(child = children.begin(); child != children.end(); child++) {
			Widget* w = *child;

			if (forPreferredSize)
			{
				maxWidth  = max(maxWidth,  w->preferredSize().width());
				maxHeight = max(maxHeight, w->preferredSize().height());
			}
			else {
				maxWidth  = max(maxWidth,  w->minimumSize().width());
				maxHeight = max(maxHeight, w->minimumSize().height());
			}
		}

		return Size(maxWidth, maxHeight * children.size() + _gap * (children.size() - 1));
	}
}

} // namespace Gui

} // namespace Pt
