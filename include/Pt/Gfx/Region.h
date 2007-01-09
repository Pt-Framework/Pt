/***************************************************************************
 *   Copyright (C)                                                         *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

#ifndef PT_GFX_REGION_H
#define PT_GFX_REGION_H

#include <Pt/Exception.h>
#include <Pt/Math/math.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Size.h>
#include <Pt/Math/Rect.h>

namespace Pt {

    namespace Gfx {

        //! \brief 
        class PT_API Region
		{
            public:
				Region(const Pt::Math::Point& topLeft, const Pt::Math::Size& size)
                : _topLeft(topLeft)
                , _size(size)
                {
					/*if (size.width() <= 0 || size.height() <= 0) {
						throw LogicError("The size for a Region needs to be at least one pixel in each dimension!", PT_SOURCEINFO);
					}*/
				}

                void setSize(const Pt::Math::Size& size)
                {
					_size = size;
				}

                const Pt::Math::Size& size() const
                {
					return _size;
				}

                ssize_t left() const
                {
					return _topLeft.x();
				}

                ssize_t top() const
                {
					return _topLeft.y();
				}

                ssize_t x() const
                {
					return _topLeft.x();
				}

                ssize_t y() const
                {
					return _topLeft.y();
				}

                Region& setX(ssize_t x)
                {
                    _topLeft.setX( x );
                    return *this;
                }

                Region& setY(ssize_t y)
                {
                    _topLeft.setY( y );
                    return *this;
                }

                ssize_t right() const
                {
					return _topLeft.x() + _size.width() - 1;
				}

                ssize_t bottom() const
                {
                    return _topLeft.y() + _size.height() - 1;
                }

                void setLeft(ssize_t left)
                {
                    _topLeft.setX(left);
                }

                void setTop(ssize_t top)
                {
                    _topLeft.setY(top);
                }

                Region& addLeft(ssize_t delta)
                {
                  setLeft(left() + delta);
                  return *this;
                }

                Region& subLeft(ssize_t delta)
                {
                  setLeft(left() - delta);
                  return *this;
                }

                Region& addTop(ssize_t delta)
                {
                  setTop(top() +  delta);
                  return *this;
                }

                Region& subTop(ssize_t delta)
                {
                  setTop(top() -  delta);
                  return *this;
                }

                size_t width() const
                {
					return _size.width();
				}

                size_t height() const
                {
					return _size.height();
				}

                Region& setWidth(size_t width)
                {
					/*if (width <= 0) {
						throw LogicError("The width of a Region needs to be at least one pixel!", PT_SOURCEINFO);
					}*/

					_size.setWidth(width);
					return *this;
                }

                Region& setHeight(size_t height)
                {
					/*if (height <= 0) {
						throw LogicError("The height of a Region needs to be at least one pixel!", PT_SOURCEINFO);
					}*/

					_size.setHeight(height);
					return *this;
                }

                Region& setGeometry(const Pt::Math::Point& topLeft, const Pt::Math::Size& size)
                {
					/*if (size.width() <= 0 || size.height() <= 0) {
						throw new LogicError("The size for a Region needs to be at least one pixel in each dimension!", PT_SOURCEINFO);
					}*/

					_topLeft = topLeft;
					_size = size;
					return *this;
                }

                Region& setGeometry(const Pt::Math::Point& topLeft, const Pt::Math::Point& bottomRight)
                {
					if (topLeft.x() > bottomRight.x() || topLeft.y() > bottomRight.y()) {
						throw LogicError("The bottom right point needs to be bottom-right from the top-left point!", PT_SOURCEINFO);
					}

                    _topLeft = topLeft;

                    _size.setWidth(bottomRight.x() - topLeft.x() + 1);
                    _size.setHeight(bottomRight.y() - topLeft.y() + 1);
                    return *this;
                }

                Pt::Math::Point topLeft() const
                {
					return _topLeft;
				}

				Pt::Math::Rect toRect() const
				{
					return Pt::Math::Rect(_topLeft, _size);
				}

                bool operator==(const Region& other) const
                {
					return _topLeft == other._topLeft && _size == other._size;
				}

                bool operator!=(const Region& other) const
                {
					return _topLeft != other._topLeft || _size != other._size;
				}

            protected:
				Pt::Math::Point _topLeft;
                Pt::Math::Size  _size;
        };

    } // namespace Gfx

} // namespace Pt

#endif
