/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_ROW_H
#define PT_DB_ROW_H

#include <Pt/Db/Api.h>
#include <Pt/Db/IRow.h>
#include <Pt/Db/Value.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Db {

    /** @brief One row of a buffered query result.

        %Row is one row of a %Result. Columns are addressed by index.
        It is a shared value. An unbound row is empty; %operator!() is
        true then.

        %size() is the column count. %empty() is true when there are
        no columns. %getValue() and %operator[] return a %Value
        without range checking. Typed getters convert that column.
        %isNull() reports SQL NULL at an index. A null or a conversion
        that cannot be performed throws %TypeMismatch. %begin() and
        %end() are random-access iterators over the values.

        @ingroup Pt-Db-Results
    */
    class PT_DB_API Row
    {
        public:
            class ConstIterator;

            typedef std::size_t size_type;

            typedef Value value_type;

        private:

            SmartPtr<IRow, InternalRefCounted<IRow> > row;

        public:

            /** @brief Creates an unbound row.
            */
            Row()  { }

            /** @brief Takes ownership of the backend @a row_.
            */
            Row(IRow* row_)
            : row(row_)
            {
            }

            /** @brief Returns the number of columns.
            */
            size_type size() const   { return row->size(); }

            /** @brief Returns true if this row has no columns.
            */
            bool empty() const      { return !row || size() == 0; }

            /** @brief Returns the value at @a field_num without range checking.
            */
            Value getValue(size_type field_num) const
            { return row->getValue(field_num); }

            /** @brief Returns the value at @a field_num without range checking.
            */
            Value operator[] (size_type field_num) const
            { return row->getValue(field_num); }

            /** @brief Returns true if the value at @a field_num is SQL NULL.
            */
            bool isNull(size_type field_num) const
            { return getValue(field_num).isNull(); }

            /** @brief Returns the value at @a field_num as bool.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            bool getBool(size_type field_num) const
            { return getValue(field_num).getBool(); }

            /** @brief Returns the value at @a field_num as int.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            int getInt(size_type field_num) const
            { return getValue(field_num).getInt(); }

            /** @brief Returns the value at @a field_num as unsigned.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            unsigned getUnsigned(size_type field_num) const
            { return getValue(field_num).getUnsigned(); }

            /** @brief Returns the value at @a field_num as float.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            float getFloat(size_type field_num) const
            { return getValue(field_num).getFloat(); }

            /** @brief Returns the value at @a field_num as double.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            double getDouble(size_type field_num) const
            { return getValue(field_num).getDouble(); }

            /** @brief Returns the value at @a field_num as char.

                If the value is a string, the first character is returned.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            char getChar(size_type field_num) const
            { return getValue(field_num).getChar(); }

            /** @brief Returns the value at @a field_num as string.

                @throw %TypeMismatch if the value is null or cannot be converted.
            */
            std::string getString(size_type field_num) const
            { return getValue(field_num).getString(); }

            /** @brief Returns a random-access iterator to the first column.
            */
            ConstIterator begin() const;

            /** @brief Returns a random-access iterator past the last column.
            */
            ConstIterator end() const;

            /** @brief Returns true if this object is not bound to a row.
            */
            bool operator!() const { return !row; }

            /** @brief Returns the backend implementation.
            */
            const IRow* getImpl() const { return &*row; }
    };


    /** @brief Random-access iterator over the values of a %Row.
    */
    class Row::ConstIterator
    {
        public:
            typedef std::random_access_iterator_tag iterator_category;
            typedef Value                           value_type;
            typedef std::ptrdiff_t                  difference_type;
            typedef const Value*                    pointer;
            typedef const Value&                    reference;

            typedef const value_type& const_reference;

            typedef const value_type* const_pointer;

        private:
            const Row& _row;
            size_type _offset;
            Value _current;

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

            ConstIterator(const Row& row, size_type offset)
            : _row(row)
            , _offset(offset)
            {
                // is range checking needed here ???
                if (_offset < row.size())
                    _current = row.getValue(_offset);
            }

            bool operator== (const ConstIterator& it) const
            { return _offset == it._offset; }

            bool operator!= (const ConstIterator& it) const
            { return !operator== (it); }

            ConstIterator& operator++()
            { this->setOffset(_offset + 1); return *this; }

            ConstIterator operator++(int)
            {
                ConstIterator ret = *this;
                this->setOffset(_offset + 1);
                return ret;
            }

            ConstIterator operator--()
            { setOffset(_offset - 1); return *this; }

            ConstIterator operator--(int)
            {
                ConstIterator ret = *this;
                this->setOffset(_offset - 1);
                return ret;
            }

            const_reference operator*() const
            { return _current; }

            const_pointer operator->() const
            { return &_current; }

            ConstIterator& operator+= (difference_type n)
            {
                this->setOffset(_offset + n);
                return *this;
            }

            ConstIterator operator+ (difference_type n) const
            {
                ConstIterator it(*this);
                it += n;
                return it;
            }

            ConstIterator& operator-= (difference_type n)
            {
                this->setOffset(_offset - n);
                return *this;
            }

            ConstIterator operator- (difference_type n) const
            {
                ConstIterator it(*this);
                it -= n;
                return it;
            }

            difference_type operator- (const ConstIterator& it) const
            { return _offset - it._offset; }
  };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_ROW_H
