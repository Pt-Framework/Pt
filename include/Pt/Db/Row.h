/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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

#ifndef PT_DB_ROW_H
#define PT_DB_ROW_H

#include <Pt/Db/Api.h>
#include <Pt/Db/IRow.h>
#include <Pt/Db/Value.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Db {

    /**  \brief Database Row class

        The class Row represents a row, which is fetched from the database.
    */
    class PT_DB_API Row
    {
        public:
            //! \brief Iterator for iterating through the values of this row.
            class ConstIterator;

            //! \brief The size-type used by Row.
            typedef std::size_t size_type;

            //! \brief The value-type contained in the row.
            typedef Value value_type;

        private:

            //! \brief Reference counted implementation.
            SmartPtr<IRow, InternalRefCounted<IRow> > row;

        public:

            //! \brief default constructor.
            Row()  { }

            /** \brief Construct a Row from an implemenataion.

                \param row_ The implementation of a specific row.
            */
            Row(IRow* row_)
            : row(row_)
            {
            }

            /** \brief Returns the number of columns of this row.

                Returns the number of columns of this row.

                \return Number of values in row.
            */
            size_type size() const   { return row->size(); }

            /** \brief Test if this row-object has no columns.

                Returns true if this row-object has no values.

                \return True if row is empty.
            */
            bool empty() const      { return !row || size() == 0; }

            /** \brief Returns a value at a given index.

                Returns a value at a given index, without range checking.

                \param field_num Specific row index.
                \return Value at index.
            */
            Value getValue(size_type field_num) const
            { return row->getValue(field_num); }

            /** \brief Returns a value at a given index.

                Returns a value at a given index, without range checking.

                \param field_num Specific value index.
                \return Value at index.
            */
            Value operator[] (size_type field_num) const
            { return row->getValue(field_num); }

            /** \brief Test if a value is null at a given index

                Return true if the specified value is null.

                \param field_num Specific row index.
                \return True if value is null.
            */
            bool isNull(size_type field_num) const
            { return getValue(field_num).isNull(); }

            /** \brief Returns the value at a given index as boolean.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as bool.

                \return Value as boolean
                \throw LogicError
            */
            bool getBool(size_type field_num) const
            { return getValue(field_num).getBool(); }

            /** \brief Returns the value at a given index as int.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as int.

                \return Value as int
                \throw LogicError
            */
            int getInt(size_type field_num) const
            { return getValue(field_num).getInt(); }

            /** \brief Returns the value at a given index as unsigned int.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as unsigned int.

                \return Value as unsigned int
                \throw LogicError
            */
            unsigned getUnsigned(size_type field_num) const
            { return getValue(field_num).getUnsigned(); }

            /** \brief Returns the value at a given index as float.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as float.

                \return Value as float
                \throw LogicError
            */
            float getFloat(size_type field_num) const
            { return getValue(field_num).getFloat(); }

            /** \brief Returns the value at a given index as double.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as double.

                \return Value as double
                \throw LogicError
            */
            double getDouble(size_type field_num) const
            { return getValue(field_num).getDouble(); }

            /** \brief Returns the value at a given index as char.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as char. If the Value is a string the first char is returned.

                \return Value as char
                \throw LogicError
            */
            char getChar(size_type field_num) const
            { return getValue(field_num).getChar(); }

            /** \brief Returns the value at a given index as string.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as string.

                \return Value as string
                \throw LogicError
            */
            void getString(size_type field_num, std::string& stringdata) const
            { return getValue(field_num).getString(stringdata); }

            /** \brief Returns an iterator to the first column.

                The returned iterator is a random access iterator, thus i can be used
                with all algorithms of the stl.

                \return Iterator to begin of row.
            */
            ConstIterator begin() const;

            /** \brief Returns an iterator past the last column.

                The returned iterator is a random access iterator, thus i can be used
                with all algorithms of the stl.

                \return Iterator to end of row.
            */
            ConstIterator end() const;

            /** \brief Test if bound to a database-row.

                Returns true if not bound to a database-row.

                \return True if unbound.
            */
            bool operator!() const { return !row; }

            //! \brief Returns the actual row implementation-class.
            const IRow* getImpl() const { return &*row; }
    };


    /** \brief Iterator to iterate over the values of a row.

        The ConstIterator can perform random access to the values of a row.
        Offsets can be added and substracted and iterators can be compared
        by using relational operators such as < or >.
    */
    class Row::ConstIterator
            : public std::iterator<std::random_access_iterator_tag, Value>
    {
        public:
            //! \brief Const reference to a value.
            typedef const value_type& const_reference;

            //! \brief Const pointer to a value.
            typedef const value_type* const_pointer;

        private:
            const Row& _row;
            size_type _offset;
            Value _current;

            /** \brief Moves this iterator to the value at a given offset.

                Set iterator to column at a given offset.

                \param off The new offset.
            */
            void setOffset(size_type off)
            {
                if (off != _offset)
                {
                    _offset = off;
                    // is range checking needed here ???
                    if (_offset < _row.size())
                        _current = _row.getValue(_offset);
                }
            }

        public:

            /**    \brief Construct a const iterator pointing to a value in a row.

                Construct a const iterator that points to a value at a given index in row.

                \param row Reference to a row.
                \param offset Offset of a value.
            */
            ConstIterator(const Row& row, size_type offset)
            : _row(row)
            , _offset(offset)
            {
                // is range checking needed here ???
                if (_offset < row.size())
                    _current = row.getValue(_offset);
            }

            /** \brief Test it two iterators are equal.

                Two iterators are equal if they occupy the same position.
                The iterators should point to the same row. This is
                not checked. Only the offsets are considered.

                \param it Other iterator.
                \return True if equal.
            */
            bool operator== (const ConstIterator& it) const
            { return _offset == it._offset; }

            /** \brief Test it two iterators are not equal.

                Two iterators are not equal if they occupy different positions.
                The iterators should point to the same row. This is
                not checked. Only the offsets are considered.

                \param it Other iterator.
                \return True if not equal.
            */
            bool operator!= (const ConstIterator& it) const
            { return !operator== (it); }

            /** \brief Steps forward.

                Steps forward and returns new position.

                \return Iterator to next element.
            */
            ConstIterator& operator++()
            { this->setOffset(_offset + 1); return *this; }

            /** \brief Steps forward.

                Steps forward and returns old position.

                \return Iterator to old element.
            */
            ConstIterator operator++(int)
            {
                ConstIterator ret = *this;
                this->setOffset(_offset + 1);
                return ret;
            }

            /** \brief Steps backward.

                Steps backward and returns new position.

                \return Iterator to previous element.
            */
            ConstIterator operator--()
            { setOffset(_offset - 1); return *this; }

            /** \brief Steps backward.

                Steps backward and returns old position.

                \return Iterator to old element.
            */
            ConstIterator operator--(int)
            {
                ConstIterator ret = *this;
                this->setOffset(_offset - 1);
                return ret;
            }

            /** \brief Get current element.

                Provides read access to the current value in the row.

                \return Const reference to a value.
            */
            const_reference operator*() const
            { return _current; }

            /** \brief Get pointer to current element.

                Provides read access to the current value in the row.

                \return Const pointer to a value.
            */
            const_pointer operator->() const
            { return &_current; }

            /** \brief Steps n elements forward.

                Moves this iterator n elements forward and returns an iterator to new position.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator& operator+= (difference_type n)
            {
                this->setOffset(_offset + n);
                return *this;
            }

            /** \brief Get an iterator to the nth next value.

                Returns an iterator pointing to the nth value after the value
                this iterator points to. Leaves this iterator unchanged.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator operator+ (difference_type n) const
            {
                ConstIterator it(*this);
                it += n;
                return it;
            }

            /** \brief Steps n elements backward.

                Moves this iterator n elements backward and returns an
                iterator to new position.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator& operator-= (difference_type n)
            {
                this->setOffset(_offset - n);
                return *this;
            }

            /** \brief Get an iterator to the nth previous value.

                Returns an iterator pointing to the nth value before the value
                this iterator points to. Leaves this iterator unchanged.

                \param n Positions to move by.
                \return Const iterator to new position.
            */
            ConstIterator operator- (difference_type n) const
            {
                ConstIterator it(*this);
                it -= n;
                return it;
            }

            /** Get the distance between two iterators

                Calculate the offset distance between this iterator and
                a given one. The iterators should point to the same row.
                This is    not checked. Only the offsets are used.

                \param it Other iterator.
                \return Distance.

            */
            difference_type operator- (const ConstIterator& it) const
            { return _offset - it._offset; }
  };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_ROW_H
