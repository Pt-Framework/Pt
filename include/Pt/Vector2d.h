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
template <typename T, typename Allocator = std::allocator<T> >
class Vector2d
{

public:

    /** @brief Default constructor. */
    Vector2d();

    /**
     * @brief Constructs a two dimensional array with given width and height.
     *
     * @param width The width of the Array2d object to be constructed.
     * @param height The height of the Array2d object to be constructed.
     */
    Vector2d(Pt::size_t width, Pt::size_t height);

    /**
     * @brief Copies an Array2d object.
     *
     * @param rhs The Array2d object to be copied.
     */
    Vector2d(const Vector2d<T, Allocator>& rhs);

    /** @brief Destructor. */
    ~Vector2d();

    /**
     * @brief Assignment operator.
     *
     * @param rhs The Array2d object to be copied.
     */
    Vector2d<T, Allocator>& operator=(const Vector2d<T, Allocator>& rhs);

    /**
     * @brief Returns a constant reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    const T& operator()(Pt::size_t x, Pt::size_t y) const;

    /**
     * @brief Returns a reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    T& operator()(Pt::size_t x, Pt::size_t y);

    /**
     * @brief Returns a constant reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    const T& at(Pt::size_t x, Pt::size_t y) const;

    /**
     * @brief Returns a reference to an element
     *        at a given position inside the array.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return Reference to the element at the given index.
     */
    T& at(Pt::size_t x, Pt::size_t y);

    /**
     * @brief Returns if the array is empty.
     *
     * @return \c true if the array is empty, \c false otherwise.
     */
    bool empty() const;

    /**
     * @brief Deletes all elements of the two dimensional array
     *        and sets width and height to zero.
     */
    void clear();

    /**
     * @brief Resizes the array to given sizes.
     *
     * @param width The horizontal size of the array.
     * @param height The vertical size of the array.
     */
    void resize(Pt::size_t width, Pt::size_t height);

    /**
     * @brief Exchanges contents of the current array
     *        with those of an other array.
     *
     * This function operates in constant time.
     *
     * @param other Reference to the Array2d object to exchange contents with.
     */
    void swap(Vector2d<T, Allocator>& other);

    /**
     * @brief Returns the height of this two dimensional array.
     *
     * @return The heigth of this two dimensional array.
     */
    inline Pt::size_t height() const;

    /**
     * @brief Returns the width of this two dimensional array.
     *
     * @return The width of this two dimensional array.
     */
    inline Pt::size_t width() const;


private:

    /**
     * @brief Computes the index in m_array of an two dimensional index.
     *
     * @param x The horizontal index inside the array.
     * @param y The vertical index inside the array.
     * @return The index inside m_array.
     */
    inline Pt::size_t index(Pt::size_t x, Pt::size_t y) const;


private:

    /** @brief The height. */
    Pt::size_t m_height;

    /** @brief The two dimensional array stored as vector. */
    std::vector<T, Allocator> m_array;


};

#include "Vector2d.tpp"

} // namespace Pt

#endif // PT_ARRAY2D_H
