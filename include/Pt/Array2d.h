/***************************************************************************
 *   Copyright (C) 2007 Sebastian Knopp                                    *
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

#ifndef PT_ARRAY2D_H
#define PT_ARRAY2D_H


#include "Pt/Types.h"

#include <stdexcept>
#include <vector>


namespace Pt {


/**
 * @brief A dynamic two dimensional array.
 *
 */
template <typename T>
class Array2d
{

public:

    /** @brief Default constructor. */
    Array2d()
    : m_height(0)
    , m_array()
    {
    }

    /**
     * @brief Constructs a two dimensional array with given width and height.
     *
     * @param width The width of the Array2d object to be constructed.
     * @param height The height of the Array2d object to be constructed.
     */
    Array2d(const Pt::size_t width, const Pt::size_t height)
    : m_height(height)
    , m_array(width * height)
    {
    }

    /**
     * @brief Copies an Array2d object.
     *
     * @param rhs The Array2d object to be copied.
     */
    Array2d(const Array2d& rhs)
    : m_height(rhs.height)
    , m_array(rhs.m_array)
    {
    }

    /** @brief Destructor. */
    ~Array2d()
    {
    }

    /**
     * @brief Assignment operator.
     *
     * @param rhs The Array2d object to be copied.
     */
    Array2d& operator=(const Array2d& rhs)
    {
        m_array = rhs.m_array;
        m_height = rhs.m_height;
        return *this;
    }

    /**
     * @brief Returns a constant reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    const T& operator()(const Pt::size_t x, const Pt::size_t y) const
    {
        return m_array[index(x, y)];
    }

    /**
     * @brief Returns a reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    T& operator()(const Pt::size_t x, const Pt::size_t y)
    {
        return m_array[index(x, y)];
    }

    /**
     * @brief Returns a constant reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    const T& at(const Pt::size_t x, const Pt::size_t y) const
    {
        if (x >= width())
        {
            throw std::out_of_range();
        }
        if (y >= height())
        {
            throw std::out_of_range();
        }
        return m_array.at(index(x, y));
    }

    /**
     * @brief Returns a reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    T& at(const Pt::size_t x, const Pt::size_t y)
    {
        if (x >= width())
        {
            throw std::out_of_range();
        }
        if (y >= height())
        {
            throw std::out_of_range();
        }
        return m_array.at(index(x, y));
    }

    /**
     * @brief Returns if the array is empty.
     *
     * @return \c true if the array is empty, \c false otherwise.
     */
    bool empty() const
    {
        return m_array.empty();
    }

    /**
     * @brief Deletes all elements of the two dimensional array
     *        and sets width and height to zero.
     */
    void clear()
    {
        m_height = 0;
        m_array.clear();
    }

    /**
     * @brief Resizes the array to given sizes.
     *
     * @param width The horizontal size of the array.
     * @param height The vertical size of the array.
     */
    void resize(const Pt::size_t width, const Pt::size_t height)
    {
        m_array.resize(width * height);
        m_height = height;
    }

    /**
     * @brief Exchanges contents of the current array
     *        with those of an other array.
     *
     * This function operates in constant time.
     *
     * @param other Reference to the Array2d object to exchange contents with.
     */
    void swap(Array2d<T>& other)
    {
        m_array.swap(other.m_array);
        swap(m_height, other.m_height);
    }

    /**
     * @brief Returns the height of this two dimensional array.
     *
     * @return The heigth of this two dimensional array.
     */
    inline Pt::size_t height() const
    {
        return m_height;
    }

    /**
     * @brief Returns the width of this two dimensional array.
     *
     * @return The width of this two dimensional array.
     */
    inline Pt::size_t width() const
    {
        if (m_height == 0)
        {
            return 0;
        }
        return m_array.size() / m_height;
    }


private:

    /**
     * @brief Computes the index in m_array of an two dimensional index.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     */
    inline Pt::size_t index(const Pt::size_t x, const Pt::size_t y) const
    {
        return x * m_height + y;
    }


private:

    /** @brief The height. */
    Pt::size_t m_height;

    /** @brief The two dimensional array stored as vector. */
    std::vector<T> m_array;


};

} // namespace Pt

#endif // PT_ARRAY2D_H
