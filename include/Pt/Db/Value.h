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

#ifndef PT_DB_VALUE_H
#define PT_DB_VALUE_H

#include <Pt/Db/Api.h>
#include <Pt/Db/IValue.h>
#include <Pt/Db/Blob.h>
#include <Pt/Date.h>
#include <Pt/Time.h>
#include <Pt/DateTime.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Db {

    /** \brief Database Value class

        The class Value represents a value, which is fetched from the database.
    */
    class PT_DB_API Value
    {
        public:
            //! \brief The size-type used for the Value class.
            typedef std::size_t size_type;

        private:
            //! \brief Reference counted implementation.
            SmartPtr<IValue, InternalRefCounted<IValue> > _value;

        public:
            /** \brief Construct a Value from an implemenataion.

                \param value The implementation of a specific value.
            */
            explicit Value(IValue* value = 0)
            : _value(value)
            { }

            /** \brief Returns true if Value is uninitialised.

                A Value is uninitialised until a value has been assigned.

                \return True if uninitialised.
            */
            bool isNull() const { return !_value || _value->isNull(); }

            /** \brief Returns the value as boolean.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as bool.

                \return Value as boolean.
                \throw LogicError
            */
            bool getBool() const { return _value->getBool(); }

            /** \brief Returns the value as int.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as int.

                \return Value as int
                \throw LogicError
            */
            int getInt() const { return _value->getInt(); }

            /** \brief Returns the value as unsigned int.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as unsigned int.

                \return Value as unsigned int.
                \throw LogicError
            */
            unsigned getUnsigned() const { return _value->getUnsigned(); }

            /** \brief Returns the value as float.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as float.

                \return Value as float.
                \throw LogicError
            */
            float getFloat() const { return _value->getFloat(); }

            /** \brief Returns the value as double.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as double.

                \return Value as double
                \throw LogicError
            */
            double getDouble() const { return _value->getDouble(); }

            /** \brief Returns the value as char.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as char. If the Value is a string the first char is returned.

                \return Value as char.
                \throw LogicError
            */
            char getChar() const { return _value->getChar(); }

            /** \brief Returns the value as string.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as string.

                \return Value as string.
                \throw LogicError
            */
            void getString(std::string& stringdata) const { return _value->getString(stringdata); }

            /** \brief Returns the value as date.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as date.

                \return Value as date.
                \throw LogicError
            */
            Date getDate() const { return _value->getDate(); }

            /** \brief Returns the value as time.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as time.

                \return Value as time.
                \throw LogicError
            */
            Time getTime() const { return _value->getTime(); }

            /** \brief Returns the value as date-time.

                If the value can't be converted to the requested type,
                an exception is thrown, otherwise the value is interpreted
                as date-time.

                \return Value as date-time.
                \throw LogicError
            */
            DateTime getDateTime() const { return _value->getDateTime(); }

            /** \brief Returns true if the Value is not bound to a statement.

                \return True if unbound.
            */
            bool operator!() const { return !_value; }

            //! \brief  Returns the actual implementation-class.
            const IValue* getImpl() const { return &*_value; }

            //DoTo: extra blob function needed??? ->    alternative: read all text values with blob sql function
            //void getBlob(std::string& blobdata) const { return _value->getBlob(blobdata); }

            void getBlob(Blob& blobdata) const
            { return _value->getBlob(blobdata); }
    };

} // namespace Db

} // namespace Pt

#endif // PT_DB_VALUE_H
