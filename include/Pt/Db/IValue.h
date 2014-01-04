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

#ifndef PT_DB_IVALUE_H
#define PT_DB_IVALUE_H

#include <Pt/RefCounted.h>
#include <Pt/Db/Blob.h>
#include <Pt/Db/Api.h>
#include <string>


namespace Pt {

    class Date;
    class Time;
    class DateTime;
    class Variant;

namespace Db {

    /** \brief Interface for DB Values
        \see Db::Value
    */
    class PT_DB_API IValue : public RefCounted
    {
        public:
            virtual bool isNull() const = 0;
            virtual bool getBool() const = 0;
            virtual int getInt() const = 0;
            virtual unsigned getUnsigned() const = 0;
            virtual float getFloat() const = 0;
            virtual double getDouble() const = 0;
            virtual char getChar() const = 0;
            virtual void getString(std::string& stringdata) const = 0;
            virtual Date getDate() const = 0;
            virtual Time getTime() const = 0;
            virtual DateTime getDateTime() const = 0;

            //TODO: extra blob function needed??? ->    alternative: read all text values with blob sql function
            //virtual void getBlob(std::string& blobdata) const = 0;
            virtual void getBlob(Blob& blobdata) const = 0;
            //virtual void getData(Pt::Variant& data) const = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_IVALUE_H

