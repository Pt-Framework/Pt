/***************************************************************************
 *   Copyright (C) 2008 by PTV AG                                          *
 *   Copyright (C) 2008 by Peter Barth                                     *
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
#ifndef PT_SYMBIANTOOLS_H_
#define PT_SYMBIANTOOLS_H_

#include <Pt/Math/Point.h>
#include <Pt/Math/Rect.h>
#include "Pt/Gfx/Rect.h"

#include <e32std.h>

namespace Pt {

namespace Gui {

/**
 * @brief Various helper methods when dealing with Symbian.
 * 
 * This mainly helps in converting basic structures from Symbian to Pt 
 * and vice versa.
 */
namespace SymbianTools {

/**
 * @brief Make a symbian TPoint from Pt::Math::Point
 */
static TPoint makeTPoint(const Math::Point& point)
{
    return TPoint(point.x(), point.y());
}

/**
 * @brief Make a symbian TRect from Pt::Math::Rect
 */
static TRect makeTRect(const Gfx::Rect& rect)
{
    return TRect(rect.x(), rect.y(), 
            rect.x() + rect.width(), rect.y() + rect.height());
}

/**
 * @brief Make a symbian TRect from Pt::Math::Point and Pt::Math::Size
 */
static TRect makeTRect(const Math::Point& point, const Math::Size& size)
{
    return TRect(point.x(), point.y(), 
            point.x() + size.width(), 
            point.y() + size.height());
}

/**
 * @brief Make a symbian TRect from Pt::Gfx::Region
 */
static TRect makeTRect(const Pt::Gfx::Region& rgn)
{
    return TRect(rgn.x(), rgn.y(), 
            rgn.x() + rgn.width(), rgn.y() + rgn.height());
}

/**
 * @brief Make Pt::Gfx::Region from symbian TRect.
 */
static Pt::Gfx::Region makeRegion(const TRect& rect)
{
    return Pt::Gfx::Region(
            Pt::Math::Point(rect.iTl.iX, rect.iTl.iY),
            Pt::Math::Size(rect.iBr.iX - rect.iTl.iX, 
                    rect.iBr.iY - rect.iTl.iY)
    );    
}

}

}

}

#endif /*PT_SYMBIANTOOLS_H_*/
