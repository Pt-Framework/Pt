/*
 * Copyright (C) 2007 Sebastian Knopp
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

#ifndef PT_VECTOR2D_H
#define PT_VECTOR2D_H


#include "Pt/Types.h"

#include <stdexcept>
#include <vector>


namespace Pt {


/**
 * @brief A dynamic two dimensional array.
 *
 * @tparam T The type of the elements.
 * @tparam Container The type of the underlying (random access) container.
 *                   E.g., this can be a vector or a deque.
 */
template <typename T, typename Container = std::vector<T> >
class Vector2d
{

public:

    /** @brief Default constructor. */
    Vector2d();

    /**
     * @brief Constructs a two dimensional vector with given width and height.
     *
     * @param width The width of the Vector2d object to be constructed.
     * @param height The height of the Vector2d object to be constructed.
     * @param default The default value the elements are initialized with.
     */
    Vector2d(Pt::size_t width, Pt::size_t height, const T& default = T());

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
     * @param default The default value the elements are initialized with.
     */
    void resize(Pt::size_t width, Pt::size_t height, const T& default = T());

    /**
     * @brief Exchanges contents of the current array
     *        with those of an other array.
     *
     * This function operates in constant time.
     *
     * @param other Reference to the Array2d object to exchange contents with.
     */
    void swap(Vector2d<T, Container>& other);

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
    Container m_array;


};

#include "Vector2d.tpp"

} // namespace Pt

#endif // PT_VECTOR2D_H
