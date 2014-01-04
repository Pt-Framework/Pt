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
#ifndef PT_DB_ISTATEMENT_H
#define PT_DB_ISTATEMENT_H


#include <Pt/RefCounted.h>
#include <Pt/Db/Api.h>
#include <Pt/Db/Blob.h>
#include <string>


namespace Pt {

    class Date;
    class Time;
    class DateTime;

namespace Db {

    class Result;
    class Row;
    class Value;
    class ICursor;

    class PT_DB_API IStatement : public RefCounted
    {
    public:
        typedef std::size_t size_type;

        virtual void clear() = 0;


        virtual void setNull(const std::string& col) = 0;
        virtual void setBool(const std::string& col, bool data) = 0;
        virtual void setInt(const std::string& col, int data) = 0;
        virtual void setUnsigned(const std::string& col, unsigned data) = 0;
        virtual void setFloat(const std::string& col, float data) = 0;
        virtual void setDouble(const std::string& col, double data) = 0;
        virtual void setChar(const std::string& col, char data) = 0;
        virtual void setString(const std::string& col, const std::string& data) = 0;
        virtual void setBlob(const std::string& col, const Blob& data) = 0;
        virtual void setDate(const std::string& col, const Date& data) = 0;
        virtual void setTime(const std::string& col, const Time& data) = 0;
        virtual void setDatetime(const std::string& col, const DateTime& data) = 0;

        virtual size_type execute() = 0;
        virtual Result select() = 0;
        virtual Row selectRow() = 0;
        virtual Value selectValue() = 0;
        virtual ICursor* createCursor() = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_ISTATEMENT_H

