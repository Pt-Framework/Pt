/***************************************************************************
 *   Copyright (C) 2006 Marc Boris D�rner                                  *
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

#include "Pt/Gui/HorizontalLayout.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Math/Point.h"
#include "Pt/Math/Size.h"
#include <algorithm>
#include <list>

using namespace Pt::Gfx;


namespace Pt {

namespace Gui {


HorizontalLayoutData::HorizontalLayoutData(Orientation orientation, const Margin& margin)
: LayoutData(margin)
, _orientation(orientation)
{
}


//HorizontalLayoutData* HorizontalLayoutData::clone() const
//{
//	return new HorizontalLayoutData(*this);
//}


void HorizontalLayoutData::setOrientation(HorizontalLayoutData::Orientation orienation)
{
	_orientation = orienation;
}


HorizontalLayoutData::Orientation HorizontalLayoutData::orientation() const
{
	return _orientation;
}



HorizontalLayout::HorizontalLayout(Widget& widget, WidthBehaviour widthBehaviour, ssize_t gap)
: Layout(widget)
, _widthBehaviour(widthBehaviour)
, _gap(gap)
{
}


void HorizontalLayout::setLayoutData(Widget& widget, const HorizontalLayoutData& layoutData)
{
	_widget2LayoutData.insert(std::make_pair(&widget, layoutData));
	connect(widget.destroyed, *this, &HorizontalLayout::remove);
}


void HorizontalLayout::remove(Widget& widget)
{
	_widget2LayoutData.erase(const_cast<Widget*>(&widget));
}


void HorizontalLayout::update()
{
	const std::list<Widget*>& children = this->widget().childWidgets();
	list<Widget*>::const_iterator childrenIter;

	// If the width behaviour is "uniform width" then the widest widget's width is
	// used. So find it here first.
	ssize_t maxWidth = 0;
	if (_widthBehaviour == UniformWidth) {
		for (childrenIter = children.begin(); childrenIter != children.end(); childrenIter++) {
			Widget* w = *childrenIter;
			maxWidth = max<size_t>(maxWidth, w->preferredSize().width());
		}
	}

	size_t parentHeight = this->widget().size().height();
	size_t x = 0;

	// Calculate the position for each widget according to the order they were added to the parent.
	for (childrenIter = children.begin(); childrenIter != children.end(); childrenIter++) {
		Widget* w = *childrenIter;

		// Initialize with default layout data. If the widget has a specific layout data
		// it will be used instead, though.
		HorizontalLayoutData layoutData;

		map<Widget*, HorizontalLayoutData>::const_iterator findIter = _widget2LayoutData.find(w);
		if (findIter != _widget2LayoutData.end()) {
			layoutData = findIter->second;
		}

		// Determine y-position and height according to the orientation (top, bottom, center, grab).
		ssize_t y      = 0;
		size_t  height = 0;
		switch (layoutData.orientation()) {
			case HorizontalLayoutData::Grab:
				y      = 0;
				height = parentHeight;
				break;

			case HorizontalLayoutData::Top:
				y      = 0;
				height = w->preferredSize().height();
				break;

			case HorizontalLayoutData::Bottom:
				y      = parentHeight - w->preferredSize().height();
				height = w->preferredSize().height();
				break;

			case HorizontalLayoutData::Center:
				y      = ((ssize_t)parentHeight - w->preferredSize().height()) / 2;
				height = w->preferredSize().height();
				break;
		};
		
		// The width of the specific widget is either all the same (uniform) or specific (varying).
		size_t width = (_widthBehaviour == UniformWidth ? maxWidth : w->preferredSize().width());

		// Add margins to y-position and height.
		y      += layoutData.margin().top();
		height -= (layoutData.margin().top() + layoutData.margin().bottom());

		// Set the widget's calculated position and size.
		w->move  (x + layoutData.margin().left(), y);
		w->resize(width - layoutData.margin().left() - layoutData.margin().right(), height);

		// Go to next y-position.
		x += width + _gap;
	}
}

Math::Size HorizontalLayout::minimumSize()
{
	return calculateSize(this->widget(), false);
}


Math::Size HorizontalLayout::preferredSize()
{
	return calculateSize(this->widget(), true);
}


Math::Size HorizontalLayout::calculateSize(Widget& parent, bool forPreferredSize)
{
	if (_widthBehaviour == VaryingWidth) {
		const std::list<Widget*>& children = parent.childWidgets();

		ssize_t allWidth  = 0;
		ssize_t maxHeight = 0;

		// Sum all width and find heighest height.
		list<Widget*>::const_iterator childrenIter = children.begin();

		while (childrenIter != children.end()) {
			Widget* w = *childrenIter;

			if (forPreferredSize) {
				allWidth  += w->preferredSize().width();
				maxHeight =  max<size_t>(maxHeight,  w->preferredSize().height());
			} else {
				allWidth  += w->minimumSize().width();
				maxHeight =  max<size_t>(maxHeight,  w->minimumSize().height());
			}

			childrenIter++;
		}

		allWidth += (children.size() - 1) * _gap;

		return Math::Size(allWidth, maxHeight);

	} else {  // _widthBehaviour == UniformWidth

		const std::list<Widget*>& children = parent.childWidgets();

		ssize_t maxWidth  = 0;
		ssize_t maxHeight = 0;

		// Findet heighest height and widest width.
		list<Widget*>::const_iterator childrenIter = children.begin();

		while (childrenIter != children.end()) {
			Widget* w = *childrenIter;

			if (forPreferredSize) {
				maxWidth  = max<size_t>(maxWidth,  w->preferredSize().width());
				maxHeight = max<size_t>(maxHeight, w->preferredSize().height());
			} else {
				maxWidth  = max<size_t>(maxWidth,  w->minimumSize().width());
				maxHeight = max<size_t>(maxHeight, w->minimumSize().height());
			}

			childrenIter++;
		}

		return Math::Size(maxWidth * children.size() + _gap * (children.size() - 1), maxHeight);
	}
}


HorizontalLayout& HorizontalLayout::create(Widget& widget, WidthBehaviour widthBehaviour, ssize_t gap)
{
	HorizontalLayout* layout = new HorizontalLayout(widget, widthBehaviour, gap);
	return *layout;
}


} // namespace Gui

} // namespace Pt
