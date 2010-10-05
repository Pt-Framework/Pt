/*
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * Copyright (C) 2010 Aloysius Indrayanto
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
#ifndef PT_GFX_REGION_H
#define PT_GFX_REGION_H

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>


namespace Pt {

    namespace Gfx {

        //! \brief
        class Region
        {
            public:
                Region(const Pt::Gfx::Point& topLeft, const Pt::Gfx::Size& size)
                : _topLeft(topLeft)
                , _size(size)
                {
                    /*if (size.width() <= 0 || size.height() <= 0) {
                        throw std::logic_error("The size for a Region needs to be at least one pixel in each dimension!", PT_SOURCEINFO);
                    }*/
                }

                void setSize(const Pt::Gfx::Size& size)
                {
                    _size = size;
                }

                const Pt::Gfx::Size& size() const
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
                        throw std::logic_error("The width of a Region needs to be at least one pixel!", PT_SOURCEINFO);
                    }*/

                    _size.setWidth(width);
                    return *this;
                }

                Region& setHeight(size_t height)
                {
                    /*if (height <= 0) {
                        throw std::logic_error("The height of a Region needs to be at least one pixel!", PT_SOURCEINFO);
                    }*/

                    _size.setHeight(height);
                    return *this;
                }

                Region& setGeometry(const Pt::Gfx::Point& topLeft, const Pt::Gfx::Size& size)
                {
                    /*if (size.width() <= 0 || size.height() <= 0) {
                        throw new std::logic_error("The size for a Region needs to be at least one pixel in each dimension!", PT_SOURCEINFO);
                    }*/

                    _topLeft = topLeft;
                    _size = size;
                    return *this;
                }

                Region& setGeometry(const Pt::Gfx::Point& topLeft, const Pt::Gfx::Point& bottomRight)
                {
                    if (topLeft.x() > bottomRight.x() || topLeft.y() > bottomRight.y()) {
                        throw std::logic_error("The bottom right point needs to be bottom-right from the top-left point!" + PT_SOURCEINFO);
                    }

                    _topLeft = topLeft;

                    _size.setWidth(bottomRight.x() - topLeft.x() + 1);
                    _size.setHeight(bottomRight.y() - topLeft.y() + 1);
                    return *this;
                }

                Pt::Gfx::Point topLeft() const
                {
                    return _topLeft;
                }

                Pt::Gfx::Rect toRect() const
                {
                    return Pt::Gfx::Rect(_topLeft, _size);
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
                Pt::Gfx::Point _topLeft;
                Pt::Gfx::Size  _size;
        };

    } // namespace Gfx

} // namespace Pt

#endif
